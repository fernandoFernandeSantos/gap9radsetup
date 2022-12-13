#!/usr/bin/python3
import argparse
import json
import os
import subprocess
import time
from typing import List

import configs
import console_logger
import dnn_log_helper


class Timer:
    time_measure = 0

    def tic(self):
        self.time_measure = time.time()

    def toc(self):
        self.time_measure = time.time() - self.time_measure

    @property
    def diff_time(self):
        return self.time_measure

    def __str__(self):
        return f"{self.time_measure:.4f}s"

    def __repr__(self):
        return str(self)


VFS_PERFORMANCE = "performance"
VFS_ENERGY = "energy"
VFS_MIDDLE = "middle"

# TODO: check if this make sense
VFS_CONFIGURATIONS = {
    VFS_PERFORMANCE: dict(RAD_FREQ_SET_FC=175 * 1000000, RAD_FREQ_SET_CL=250 * 1000000, RAD_VOLT_SET=1200),
    VFS_MIDDLE: dict(RAD_FREQ_SET_FC=120 * 1000000, RAD_FREQ_SET_CL=150 * 1000000, RAD_VOLT_SET=1000),
    VFS_ENERGY: dict(RAD_FREQ_SET_FC=80 * 1000000, RAD_FREQ_SET_CL=100 * 1000000, RAD_VOLT_SET=800),
}

KILL_PROGRAM_LIST = ["gapy", "gap8-openocd"]

DATA_DIR = "data"
LOG_PATH = f"{DATA_DIR}/logs"


def kill_after_error():
    for cmd in KILL_PROGRAM_LIST:
        os.system(f"pkill -9 -f {cmd} && killall -9 {cmd}")


def exec_cmd(cmd: list, path_to_execute: str, app_timeout: float, logger: console_logger.ColoredLogger) -> dict:
    if 'NNTOOL_DIR' not in os.environ:
        raise EnvironmentError("First set GAP9 ENV vars to execute this!!!")
    env = {}
    env.update(os.environ)
    cwd = os.getcwd()
    os.chdir(path_to_execute)
    addition_info = list()
    cycle_line, error_cycle_line = None, None
    cmd_stdout, cmd_stderr = "", ""
    if logger:
        logger.debug(" ".join(cmd))
    try:
        # p = subprocess.Popen(cmd, env=env, cwd=path_to_execute, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        #                      universal_newlines=True)

        # cmd_stdout, cmd_stderr = p.communicate(timeout=app_timeout)
        # remove time from result
        cmd_stdout, cmd_stderr = "", ""
        split_stderr = cmd_stderr.split("\n")
        cmd_stderr = cmd_stderr.replace(split_stderr[0], "")
        if logger:
            logger.debug(f"---------------STDOUT-----------------\n{cmd_stdout}")
            logger.debug(f"---------------STDERR-----------------\n{cmd_stderr}")
        # split_stdout = cmd_stdout.split('\n')
        # addition_info_list = ["cycle", "diff"]
        # for i in range(len(split_stdout)):
        #     target = split_stdout[i]
        #     if any([add_info in target.lower() for add_info in addition_info_list]):
        #         addition_info.append(target)
        #         cmd_stdout = cmd_stdout.replace(target, "")
        #         # print(cmd_stdout)
        #     if re.match(r"RATIT:.* CORE:.* CYCLES_IN:.* CYCLES_OUT:.* INST_IN:.* INST_OUT:.*", target):
        #         cycle_line = target
        #     if re.match(r"RATIT:.* ITS:.* TIME_IT:.* CYCLE_IT:.* ACCTIME:.* ACCCYCLES:.*", target):
        #         cycle_line = target
        #
        #     if re.match(r"ITS:.* CORE:.* CYCLES_T1:.* CYCLES_T2:.* INST_T1:.* INST_T2:.*", target):
        #         error_cycle_line = target
        #
        #     if re.match(r"ITS:.* TIME_IT:.* CYCLE_IT:.* CYCLES:.* TIME:.*", target):
        #         error_cycle_line = target

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


def program_and_generate_golds(reprogram: bool,
                               make_flags: List[str],
                               benchmark: str,
                               gold_path: str,
                               logger: console_logger.ColoredLogger) -> None:
    logger.debug("Programing the board and generating the gold files")
    bench_path = configs.CODES_CONFIG[benchmark]["path"]
    bench_exec = configs.CODES_CONFIG[benchmark]["exec"]
    bench_timeout = configs.CODES_CONFIG[benchmark]["timeout"]
    if reprogram is True:
        logger.debug(f"Programing {benchmark}")
        make_cmd = ["make", "clean", "all"] + make_flags
        exec_cmd(cmd=make_cmd, path_to_execute=bench_path, app_timeout=bench_timeout, logger=logger)
    logger.debug(f"Gen gold for {benchmark}")
    expected_output = exec_cmd(cmd=bench_exec, path_to_execute=bench_path, app_timeout=bench_timeout,
                               logger=logger)

    with open(gold_path, "w") as fp:
        json.dump(expected_output, fp, indent=4)


def load_gold(golds_path: str) -> dict:
    with open(golds_path, "r") as fp:
        return json.load(fp)


def parse_arguments() -> [argparse.Namespace, List[str]]:
    parser = argparse.ArgumentParser(description='GAP8 Setup')
    parser.add_argument('--iterations', type=int, default=int(1e6), help='Number of iterations for the experiment')
    parser.add_argument('--generate', default=False, action="store_true",
                        help="Set this flag to generate the golds and reprogram the board")
    parser.add_argument('--goldpath', help="Path to the gold file", required=True)
    parser.add_argument('--benchmark', default=None, help='Benchmarks, can be ' + ", ".join(configs.CODES_CONFIG),
                        required=True)

    args = parser.parse_args()
    args_text_list = [f"{k}={v}" for k, v in vars(args).items()]
    return args, args_text_list


def compare_output(output: dict, golden: dict) -> int:
    return 0


def main():
    """ Main function """
    args, args_text_list = parse_arguments()

    # Starting the setup
    generate = args.generate
    gold_path = args.goldpath
    iterations = args.iterations
    benchmark = args.benchmark
    dnn_goal = configs.CODES_CONFIG[benchmark]["dnn_goal"]
    # Define DNN goal
    dnn_log_helper.start_setup_log_file(device=configs.DEVICE_NAME, args_conf=args_text_list, dnn_name=args.name,
                                        activate_logging=not generate, dnn_goal=dnn_goal)

    # Defining a timer
    timer = Timer()

    if benchmark not in configs.CODES_CONFIG:
        dnn_log_helper.log_and_crash(fatal_string=f"{benchmark} not in the list of acceptable apps")

    # Terminal console
    main_logger_name = str(os.path.basename(__file__)).replace(".py", "")
    terminal_logger = console_logger.ColoredLogger(main_logger_name) if args.disableconsolelog is False else None

    if terminal_logger:
        terminal_logger.debug("\n".join(args_text_list))

    # Main setup loop
    make_parameters = configs.CODES_CONFIG[benchmark]

    if generate is True:
        program_and_generate_golds(reprogram=args.reprogram, make_flags=make_parameters, benchmark=args.benchmark,
                                   gold_path=gold_path, logger=terminal_logger)
    else:
        # Load if it is not a gold generating op
        timer.tic()
        golden = load_gold(golds_path=gold_path)
        timer.toc()
        golden_load_diff_time = timer.diff_time
        if terminal_logger:
            terminal_logger.debug(f"Time necessary to load the golden outputs: {golden_load_diff_time}")

        bench_exec = configs.CODES_CONFIG[benchmark]["exec"]
        bench_path = configs.CODES_CONFIG[benchmark]["path"]
        bench_timeout = configs.CODES_CONFIG[benchmark]["timeout"]
        for setup_iteration in range(iterations):
            # Loop over the input list
            timer.tic()
            dnn_log_helper.start_iteration()
            current_iteration_data = exec_cmd(cmd=bench_exec, app_timeout=bench_timeout, path_to_execute=bench_path,
                                              logger=terminal_logger)
            dnn_log_helper.end_iteration()
            timer.toc()
            kernel_time = timer.diff_time
            # Then compare the golden with the output
            timer.tic()
            errors = 0
            if generate is False:
                errors = compare_output(output=current_iteration_data, golden=golden)
            timer.toc()
            comparison_time = timer.diff_time

            # Printing timing information
            if terminal_logger:
                wasted_time = comparison_time
                time_pct = (wasted_time / (wasted_time + kernel_time)) * 100.0
                iteration_out = f"It:{setup_iteration:<3} inference time:{kernel_time:.5f}, "
                iteration_out += f"compare time:{comparison_time:.5f} "
                iteration_out += f"(wasted:{time_pct:.1f}%) errors:{errors}"
                terminal_logger.debug(iteration_out)

    if terminal_logger:
        terminal_logger.debug("Finish computation.")

    dnn_log_helper.end_log_file()


if __name__ == '__main__':
    try:
        main()
    except Exception as main_function_exception:
        dnn_log_helper.log_and_crash(fatal_string=f"EXCEPTION:{main_function_exception}")
