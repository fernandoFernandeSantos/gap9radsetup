#!/usr/bin/python3
import json
import logging
import os
import re
import subprocess
import time
from datetime import datetime
from typing import List

import reboot_machine
import common
from common import Timer
from logger_formatter import logging_setup

# Set timezone for the script
os.environ['TZ'] = 'Europe/London'


def kill_after_error():
    for cmd in common.KILL_PROGRAM_LIST:
        os.system(f"pkill -9 -f {cmd} && killall -9 {cmd}")


def load_the_golds(golds_path: str, benchmark: str = None) -> dict:
    golds_dict = dict()
    load_codes = common.CODES_CONFIG
    if benchmark:
        load_codes = [benchmark]
    for benchmark in load_codes:
        app_gold_path = f"{golds_path}/{benchmark}.json"
        with open(app_gold_path, "r") as fp:
            golds_dict[benchmark] = json.load(fp)
    return golds_dict


def exec_cmd(cmd: str, path_to_execute: str, app_timeout: float, verbose_level: int):
    if 'NNTOOL_DIR' not in os.environ:
        raise EnvironmentError("First set GAP9 ENV vars to execute this!!!")
    # print(f"EXECUTING " + " ".join(cmd))
    env = {}
    env.update(os.environ)
    cwd = os.getcwd()
    os.chdir(path_to_execute)
    addition_info = list()
    cycle_line, error_cycle_line = None, None
    cmd_stdout, cmd_stderr = "", ""
    if verbose_level == 2:
        print(cmd)
    try:
        p = subprocess.Popen(cmd, env=env, cwd=path_to_execute, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True, shell=True)
        cmd_stdout, cmd_stderr = p.communicate(timeout=app_timeout)
        # remove time from result
        split_stderr = cmd_stderr.split("\n")
        cmd_stderr = cmd_stderr.replace(split_stderr[0], "")
        if verbose_level == 2:
            print("---------------STDOUT-----------------")
            print(cmd_stdout)
            print("---------------STDERR-----------------")
            print(cmd_stderr)
            print("--------------------------------------")
        split_stdout = cmd_stdout.split('\n')
        addition_info_list = ["cycle", "diff"]
        for i in range(len(split_stdout)):
            target = split_stdout[i]
            if any([add_info in target.lower() for add_info in addition_info_list]):
                addition_info.append(target)
                cmd_stdout = cmd_stdout.replace(target, "")
                # print(cmd_stdout)
            if re.match(r"RATIT:.* CORE:.* CYCLES_OUT:.* INST_OUT:.*", target):
                cycle_line = target
            if re.match(r"RATIT:.* ITS:.* TIME_IT:.* CYCLE_IT:.* ACCTIME:.* ACCCYCLES:.*", target):
                cycle_line = target

            if re.match(r"ITS:.* CORE:.* CYCLES_T1:.* CYCLES_T2:.* INST_T1:.* INST_T2:.*", target):
                error_cycle_line = target

            if re.match(r"ITS:.* TIME_IT:.* CYCLE_IT:.* CYCLES:.* TIME:.*", target):
                error_cycle_line = target

    except subprocess.TimeoutExpired:
        cmd_stderr = "TIMEOUT_ERROR"
    except RuntimeError:
        cmd_stderr = "RUNTIME_ERROR"
    except UnicodeError:
        cmd_stderr = "UNICODE_ERROR"
    os.chdir(cwd)

    # print("================================")
    # print(cmd_stdout)
    # print("================================")

    return dict(stdout=cmd_stdout, stderr=cmd_stderr, additional_info=addition_info, cycle_line=cycle_line,
                error_cycle_line=error_cycle_line)


def program_and_generate_golds(reprogram: bool, make_flags: List[str], benchmark: str = None):
    print("Programing the board and generating the gold files")
    golds_dict = dict()
    gen_bench = common.CODES_CONFIG
    if benchmark:
        gen_bench = [benchmark]
    for benchmark in gen_bench:
        bench_path = common.CODES_CONFIG[benchmark]["path"]
        bench_exec = common.CODES_CONFIG[benchmark]["exec"]
        bench_timeout = common.CODES_CONFIG[benchmark]["timeout"] * 20
        if reprogram is True:
            print(f"Programing {benchmark} path {bench_path}")
            make_cmd = " ".join(["make", "clean", "all"] + make_flags)
            exec_cmd(cmd=make_cmd, path_to_execute=bench_path, app_timeout=bench_timeout, verbose_level=2)
        print(f"Gen gold for {benchmark}")
        golds_dict[benchmark] = exec_cmd(cmd=bench_exec, path_to_execute=bench_path, app_timeout=bench_timeout,
                                         verbose_level=2)
    print(f"Saving golds to {common.DATA_DIR}")
    for benchmark in gen_bench:
        app_gold_path = f"{common.DATA_DIR}/{benchmark}.json"
        with open(app_gold_path, "w") as fp:
            json.dump(golds_dict[benchmark], fp, indent=4)


def gen_log_file_name(test_name, log_dir):
    # log example: 2021_11_15_22_08_25_cuda_trip_half_lava_ECC_OFF_fernando.log
    date = datetime.today()
    date_fmt = date.strftime('%Y_%m_%d_%H_%M_%S')
    log_filename = f"{log_dir}/{date_fmt}_{test_name}_gap8.log"
    return log_filename


def reboot_usb_device(script_name: str, logger: logging.Logger, reboot: bool):
    if reboot is False:
        return
    logger.info(f"Rebooting USB device")
    reboot_machine.reboot_machine(
        address=common.HOST_IP,
        switch_model="lindy", switch_port=common.SWITCH_PORT, switch_ip=common.SWITCH_IP,
        rebooting_sleep=common.AFTER_REBOOT_SLEEPING_TIME,
        logger_name=script_name
    )
    logger.info(f"AFTER_REBOOT_SLEEPING_TIME: {common.AFTER_REBOOT_SLEEPING_TIME}s")
    time.sleep(common.AFTER_REBOOT_SLEEPING_TIME)
    logger.info("FINISHED REBOOT")


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
