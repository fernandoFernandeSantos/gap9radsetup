#!/usr/bin/python3
import argparse
import logging
import os
import time
import common
from run_experiment_rad import reboot_usb_device, load_the_golds, gen_log_file_name, kill_after_error
from run_experiment_rad import program_and_generate_golds, logging_setup, exec_cmd
from common import Timer


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
        reboot_usb_device(script_name=script_name, logger=experiment_logger, reboot=reboot_disable)
        args_info = " ".join([f"{k}:{v}" for k, v in vars(args).items()])
        experiment_logger.info(f"HEADER: {args_info}")
        benchmark = args.benchmark

        golds_dict = load_the_golds(golds_path=common.DATA_DIR, benchmark=args.benchmark)
        benchmark_gold_output = golds_dict[args.benchmark]
        bench_exec = common.CODES_CONFIG[benchmark]["exec"]
        bench_path = common.CODES_CONFIG[benchmark]["path"]
        bench_timeout = common.CODES_CONFIG[benchmark]["timeout"]

        iteration_errors = 0
        sequential_errors = 0
        for iteration in range(args.iterations):
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
                    if sequential_errors > common.MAX_SEQUENTIALLY_ERRORS and disable_sequential_errors_check is False:
                        experiment_logger.error(
                            f"MAXIMUM NUMBER OF SEQUENTIALLY ERRORS REACHED SLEEPING FOR "
                            f"{common.SLEEP_AFTER_MULTIPLE_ERRORS + common.AFTER_REBOOT_SLEEPING_TIME} seconds")
                        reboot_usb_device(script_name=script_name, logger=experiment_logger, reboot=reboot_disable)
                        time.sleep(common.SLEEP_AFTER_MULTIPLE_ERRORS)

                        sequential_errors = 0
                else:
                    sequential_errors = 0

            cycle_str = current_iteration_data["cycle_line"]
            error_cycle_line = current_iteration_data["error_cycle_line"]

            experiment_logger.info(f"Iteration:{iteration} time:{timer} it_errors:{iteration_errors} "
                                   f"acc_time:{acc_time} acc_errors:{acc_errors} seq_errors:{sequential_errors} "
                                   f"cycle_str:{cycle_str} err_cycle:{error_cycle_line}")

        experiment_logger.debug("#END Experiment finished")


if __name__ == '__main__':
    main()