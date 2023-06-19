#!/usr/bin/python3
import logging
import os
import common
import paramiko
from run_experiment_rad import load_the_golds, gen_log_file_name, kill_after_error
from run_experiment_rad import program_and_generate_golds, logging_setup, exec_cmd
from common import Timer

# EM related imports
# from benches.code.generators import AVRK4
import fake_avrk4 as AVRK4
# from benches.code.helpers import ip_connection
import pint

PYNQ_REBOOT_COMMAND = "/home/pynq/reboot.py"
PYNQ_SSH_USERNAME = "pynq"
PYNQ_SSH_PASSWORD = "qwerty0"  # FIXME: change it
PYNQ_SSH_SERVER = "192.168.1.1"
assert PYNQ_SSH_PASSWORD != "qwerty0", "Dummy password not allowed, change for experiment"


def reboot_usb_device(logger: logging.Logger, reboot: bool):
    if reboot is False:
        return
    logger.info(f"Rebooting USB device")
    client_ssh = paramiko.SSHClient()
    client_ssh.connect(PYNQ_SSH_SERVER, username=PYNQ_SSH_USERNAME, password=PYNQ_SSH_PASSWORD)
    ssh_stdin, ssh_stdout, ssh_stderr = client_ssh.exec_command(PYNQ_REBOOT_COMMAND)
    # TODO: Implement a function that can turn off and on the BOARD
    raise NotImplementedError


def main():
    """ Main function """
    args = common.parse_arguments()
    make_parameters = list()
    disable_sequential_errors_check = args.disablecheckseqerr
    if args.benchmark == common.MEM_TEST:
        make_parameters = [common.MEMORIES_TO_TEST[args.memtotest]]

    if args.benchmark == common.CNN_OP:
        make_parameters = [common.CNN_OPS_TO_TEST[args.cnnop]]
    if args.benchmark == common.FPU_MICROBENCHMARKS:
        make_parameters = [common.FPU_MICRO_TO_TEST[args.fpuop]]

    make_parameters += [f"{k}={v}" for k, v in common.VFS_CONFIGURATIONS[args.vfsprof].items()]
    # FREQ_SET_FC=175 * 1000000, FREQ_SET_CL=250 * 1000000, VOLT_SET=1200
    make_parameters += [f"FREQ_CL={common.VFS_CONFIGURATIONS[args.vfsprof]['FREQ_CL']}",
                        f"FREQ_FC={common.VFS_CONFIGURATIONS[args.vfsprof]['FREQ_FC']}",
                        f"FREQ_PE={common.VFS_CONFIGURATIONS[args.vfsprof]['FREQ_FC']}",
                        f"VOLTAGE={common.VFS_CONFIGURATIONS[args.vfsprof]['VOLT_SET']}"]
    make_parameters += common.CODES_CONFIG[args.benchmark]["make_parameters"]

    reboot_disable = args.noreboot is False
    if os.path.isdir(common.DATA_DIR) is False:
        os.mkdir(common.DATA_DIR)
    if os.path.isdir(common.LOG_PATH) is False:
        os.mkdir(common.LOG_PATH)

    timer = Timer()
    acc_time = 0
    acc_errors = 0
    # IF not generate load the golds
    if args.generate:
        program_and_generate_golds(reprogram=args.reprogram, make_flags=make_parameters, benchmark=args.benchmark)
    else:
        script_name = os.path.basename(__file__)
        log_file = gen_log_file_name(f"GAP9-{args.benchmark}", log_dir=common.LOG_PATH)
        experiment_logger = logging_setup(logger_name=script_name, log_file=log_file, logging_level=logging.DEBUG)
        # reboot_usb_device(logger=experiment_logger, reboot=reboot_disable)
        args_info = " ".join([f"{k}:{v}" for k, v in vars(args).items()])
        experiment_logger.info(f"HEADER: {args_info}")
        benchmark = args.benchmark
        experiment_logger.info("Setting the EM parameters")
        # ==================================== EM Parameters
        # FIXME: To be debugged
        # TODO: Check the parameters of the EM FI to try it automatically
        #  -- The amplitude and the slope and duration
        # Setting things for EM
        ureg = pint.UnitRegistry()
        # avrk4_ip = ip_connection.IpConnection("127.0.0.1", port=8)
        # configuration = None
        # avrk4 = AVRK4.AVRK4(connection=avrk4_ip, configuration=configuration)
        avrk4 = AVRK4.AVRK4()
        # Set amplitude
        avrk4.set_ext_trig()
        amplitude = pint.Quantity(700, ureg.volt)
        delays = range(2700, 3500, 1)
        avrk4.set_amplitude(amplitude)
        # Activate
        avrk4.activate()
        tries_per_delay = 8
        # ==================================================

        golds_dict = load_the_golds(golds_path=common.DATA_DIR, benchmark=args.benchmark)
        benchmark_gold_output = golds_dict[args.benchmark]
        bench_exec = common.CODES_CONFIG[benchmark]["exec"]
        bench_path = common.CODES_CONFIG[benchmark]["path"]
        bench_timeout = common.CODES_CONFIG[benchmark]["timeout"]

        iteration_errors = 0
        sequential_errors = 0
        iteration = 0
        for delay in delays:
            # Set trigger delay
            delay_ns = pint.Quantity(delay, ureg.ns)
            avrk4.set_trigger_delay(delay_ns)

            for try_delay in range(tries_per_delay):
                # Activate before executing
                avrk4.activate()
                # Set up the test in the device
                # perform the data processing
                timer.tic()
                current_iteration_data = exec_cmd(cmd=bench_exec, app_timeout=bench_timeout, path_to_execute=bench_path,
                                                  verbose_level=1)
                timer.toc()
                acc_time += timer.diff_time

                # Compare with the output
                stdout_error = benchmark_gold_output["stdout"] != current_iteration_data["stdout"]
                stderr_error = benchmark_gold_output["stderr"] != current_iteration_data["stderr"]
                stdout_count, stderr_count = 0, 0
                if stdout_error is True:
                    experiment_logger.info(f"Iteration:{iteration} DIFF-STDOUT-IDENTIFIED")
                    experiment_logger.error("\n" + current_iteration_data["stdout"])
                    if current_iteration_data["additional_info"]:
                        experiment_logger.error(current_iteration_data["additional_info"])

                    stdout_count = 1
                if stderr_error is True:
                    experiment_logger.info(f"Iteration:{iteration} DIFF-STDERR-IDENTIFIED")
                    experiment_logger.error("\n" + current_iteration_data["stderr"])
                    stderr_count = 1
                past_errors_count = iteration_errors
                iteration_errors = (stdout_count + stderr_count)
                acc_errors += iteration_errors
                if iteration_errors != 0:
                    kill_after_error()
                    if iteration_errors == past_errors_count:
                        sequential_errors += 1
                        if sequential_errors > common.MAX_SEQUENTIALLY_ERRORS:
                            experiment_logger.error(
                                f"MAXIMUM NUMBER OF SEQUENTIALLY ERRORS REACHED, SLEEPING FOR "
                                f"{common.AFTER_REBOOT_SLEEPING_TIME} seconds"
                            )
                            reboot_usb_device(logger=experiment_logger, reboot=reboot_disable)

                            sequential_errors = 0
                    else:
                        sequential_errors = 0

                cycle_str = current_iteration_data["cycle_line"]
                error_cycle_line = current_iteration_data["error_cycle_line"]

                experiment_logger.info(f"Iteration:{iteration} Delay:{delay} delay_try:{try_delay}  time:{timer} "
                                       f"it_errors:{iteration_errors} acc_time:{acc_time} acc_errors:{acc_errors} "
                                       f"seq_errors:{sequential_errors} cycle_str:{cycle_str} "
                                       f"err_cycle:{error_cycle_line}")

                iteration += 1

        avrk4.shutdown()
        experiment_logger.debug("#END Experiment finished")


if __name__ == '__main__':
    main()
