#!/usr/bin/python3
import argparse
import getpass
import json
import logging
import os
import re
import subprocess
import time
from datetime import datetime
from typing import List

import reboot_machine
from logger_formatter import logging_setup

# Set timezone for the script
os.environ['TZ'] = 'Europe/London'

MXM = "MatMult"
MMAD = "MatrixAdd"
MNIST = "Mnist"
FIR = "Fir"
BILINEAR_RESIZE = "BilinearResize"
# MEM tests
MEM_TEST = "memradtest"
CNN_OP = "cnn"
MOBILENET_V1 = "mobilenetv1"
MOBILENET_V2 = "mobilenetv2"

MEMORIES_TO_TEST = {
    "L1": "-DMEM_LEVEL=1",
    "L2": "-DMEM_LEVEL=2"
}

CNN_OPS_TO_TEST = dict(
    RAD_SEQUENTIAL_MAX_POOL=0,
    RAD_SEQUENTIAL_AVG_MAX_POOL=1,
    RAD_SEQUENTIAL_CONV=2,
    RAD_SEQUENTIAL_LINEAR=3,
    RAD_PARALLEL_VECT_MAX_POOL=14,
    RAD_PARALLEL_VECT_AVG_MAX_POOL=15,
    RAD_PARALLEL_VECT_CONV=16,
    RAD_PARALLEL_VECT_LINEAR=17,
)

CNN_OPS_TO_TEST = {k: f"RAD_CNN_OP={v}" for k, v in CNN_OPS_TO_TEST.items()}

AFTER_REBOOT_SLEEPING_TIME = 60
GENERAL_TIMEOUT = 50
MAX_SEQUENTIALLY_ERRORS = 2
SLEEP_AFTER_MULTIPLE_ERRORS = 120

GIT_RESEARCH = f"/home/{getpass.getuser()}/git_research/gap9riscvsetup"
GAP_SDK_DIR = f"{GIT_RESEARCH}/gap_sdk_private"
# EXAMPLES_DIR = f"{GAP_SDK_DIR}/examples"
# EXAMPLES_AUTOTILER_DIR = f"{EXAMPLES_DIR}/autotiler"
# EXAMPLES_PMSIS_DIR = f"{EXAMPLES_DIR}/pmsis"
# EXAMPLES_BENCHMARKS_DIR = f"{GAP_SDK_DIR}/benchmarks/gap8"

HOST_IP = "130.246.247.140"
SWITCH_IP = "130.246.247.141"

CODES_CONFIG = {
    MXM: {
        "path": f"{GIT_RESEARCH}/dnnradtestgap9/{MXM}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={GIT_RESEARCH}/dnnradtestgap9/{MXM}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={GIT_RESEARCH}/dnnradtestgap9/{MXM}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{MXM} '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg'
                f' --openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": GENERAL_TIMEOUT,
    },
    # MMAD: {
    #     "path": f" ",
    #     "exec": [],
    #     "timeout": GENERAL_TIMEOUT,
    # },
    # MNIST: {
    #     "path": f" ",
    #     "exec": [f" "],
    #     "timeout": GENERAL_TIMEOUT,
    # },
    #
    # BILINEAR_RESIZE: {
    #     "path": f"{EXAMPLES_AUTOTILER_DIR}/{BILINEAR_RESIZE}",
    #     "exec": [],
    #     "timeout": GENERAL_TIMEOUT,
    # },
    #
    # MEM_TEST: {
    #     "path": f"{EXAMPLES_PMSIS_DIR}/{MEM_TEST}",
    #     "exec": [
    #         f"gapy", "--target=gapuino_v2", "--platform=board",
    #         f"--work-dir={EXAMPLES_PMSIS_DIR}/{MEM_TEST}/BUILD/GAP8_V2/GCC_RISCV",
    #         "run", "--exec-prepare", "--exec",
    #         f"--binary={EXAMPLES_PMSIS_DIR}/{MEM_TEST}/BUILD/GAP8_V2/GCC_RISCV/{MEM_TEST}"
    #     ],
    #     "timeout": 100,
    # },
    #
    CNN_OP: {
        "path": f"{GIT_RESEARCH}/dnnradtestgap9/{CNN_OP}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={GIT_RESEARCH}/dnnradtestgap9/{CNN_OP}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={GIT_RESEARCH}/dnnradtestgap9/{CNN_OP}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/cnnOps '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg'
                f' --openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": 500,
        "make_parameters": []
    },

    MOBILENET_V1: {
        "path": f"{GIT_RESEARCH}/dnnradtestgap9/mobilenet",
        "exec": 'openocd -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -f '
                f'"{GAP_SDK_DIR}/utils/openocd_tools/tcl/'
                f'gapuino_ftdi.cfg" -f "{GAP_SDK_DIR}/utils/'
                'openocd_tools/tcl/gap9revb.tcl" -c "load_and_start_binary {GIT_RESEARCH}/'
                'gap9riscvsetup/dnnradtestgap9/mobilenet/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/imagenet 0x1c010100"',
        "timeout": 300,
        "make_parameters": ["MOBNET_VERSION=1"]
    },
    MOBILENET_V2: {
        "path": f"{GIT_RESEARCH}/dnnradtestgap9/mobilenet",
        "exec": [],
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["MOBNET_VERSION=2"]
    }
}

VFS_PERFORMANCE = "performance"
VFS_ENERGY = "energy"
VFS_MIDDLE = "middle"

# TODO: check if this make sense
VFS_CONFIGURATIONS = {
    VFS_PERFORMANCE: dict(RAD_FREQ_SET_FC=370, RAD_FREQ_SET_CL=370, RAD_VOLT_SET=800),
    # VFS_MIDDLE: dict(RAD_FREQ_SET_FC=120 * 1000000, RAD_FREQ_SET_CL=150 * 1000000, RAD_VOLT_SET=1000),
    VFS_ENERGY: dict(RAD_FREQ_SET_FC=240, RAD_FREQ_SET_CL=240, RAD_VOLT_SET=800),
}

KILL_PROGRAM_LIST = ["gapy", "openocd"]

DATA_DIR = "data"
LOG_PATH = f"{DATA_DIR}/logs"


def kill_after_error():
    for cmd in KILL_PROGRAM_LIST:
        os.system(f"pkill -9 -f {cmd} && killall -9 {cmd}")


def load_the_golds(golds_path: str, benchmark: str = None) -> dict:
    golds_dict = dict()
    load_codes = CODES_CONFIG
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
            if re.match(r"RATIT:.* CORE:.* CYCLES_IN:.* CYCLES_OUT:.* INST_IN:.* INST_OUT:.*", target):
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
    gen_bench = CODES_CONFIG
    if benchmark:
        gen_bench = [benchmark]
    for benchmark in gen_bench:
        bench_path = CODES_CONFIG[benchmark]["path"]
        bench_exec = CODES_CONFIG[benchmark]["exec"]
        bench_timeout = CODES_CONFIG[benchmark]["timeout"]
        if reprogram is True:
            print(f"Programing {benchmark} path {bench_path}")
            make_cmd = " ".join(["make", "clean", "all"] + make_flags)
            exec_cmd(cmd=make_cmd, path_to_execute=bench_path, app_timeout=bench_timeout, verbose_level=2)
        print(f"Gen gold for {benchmark}")
        golds_dict[benchmark] = exec_cmd(cmd=bench_exec, path_to_execute=bench_path, app_timeout=bench_timeout,
                                         verbose_level=2)
    print(f"Saving golds to {DATA_DIR}")
    for benchmark in gen_bench:
        app_gold_path = f"{DATA_DIR}/{benchmark}.json"
        with open(app_gold_path, "w") as fp:
            json.dump(golds_dict[benchmark], fp, indent=4)


def gen_log_file_name(test_name, log_dir):
    # log example: 2021_11_15_22_08_25_cuda_trip_half_lava_ECC_OFF_fernando.log
    date = datetime.today()
    date_fmt = date.strftime('%Y_%m_%d_%H_%M_%S')
    log_filename = f"{log_dir}/{date_fmt}_{test_name}_gap8.log"
    return log_filename


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


def reboot_usb_device(script_name: str, logger: logging.Logger, reboot: bool):
    if reboot is False:
        return
    logger.info(f"Rebooting USB device")
    reboot_machine.reboot_machine(
        address=HOST_IP,
        switch_model="lindy", switch_port=8, switch_ip=SWITCH_IP, rebooting_sleep=AFTER_REBOOT_SLEEPING_TIME / 2,
        logger_name=script_name
    )
    logger.info(f"AFTER_REBOOT_SLEEPING_TIME: {AFTER_REBOOT_SLEEPING_TIME}s")
    time.sleep(AFTER_REBOOT_SLEEPING_TIME)
    logger.info("FINISHED REBOOT")


def main():
    """ Main function """
    parser = argparse.ArgumentParser(description='GAP8 Setup')
    parser.add_argument('--iterations', type=int, default=int(1e6), help='Number of iterations for the experiment')
    parser.add_argument('--generate', default=False, action="store_true",
                        help="Set this flag to generate the golds and reprogram the board")
    parser.add_argument('--benchmark', default=None, help='Benchmarks, can be ' + ", ".join(list(CODES_CONFIG.keys())))
    parser.add_argument('--reprogram', default=False, action="store_true",
                        help="This will in addition to golden generation reprogram the board")
    parser.add_argument('--noreboot', default=False, action="store_true", help="Enable or disable reboot")
    parser.add_argument('--memtotest', default=None,
                        help=f"If the benchmark is memradtest,  inform which memory to test:{MEMORIES_TO_TEST.keys()}")
    parser.add_argument('--disablecheckseqerr', default=False, action="store_true",
                        help="Enable or disable sequential errors check")

    parser.add_argument('--cnnop', default=None,
                        help=f"If the benchmark is cnn,  inform which memory to test:{CNN_OPS_TO_TEST.keys()}")
    parser.add_argument('--vfsprof', default=VFS_PERFORMANCE,
                        help=f"Voltage Frequenccy scaling, it can be {VFS_PERFORMANCE}, {VFS_ENERGY}, {VFS_MIDDLE}")

    args = parser.parse_args()
    make_parameters = list()
    disable_sequential_errors_check = args.disablecheckseqerr
    if args.benchmark == MEM_TEST:
        make_parameters = [MEMORIES_TO_TEST[args.memtotest]]

    if args.benchmark == CNN_OP:
        make_parameters = [CNN_OPS_TO_TEST[args.cnnop]]

    make_parameters += [f"{k}={v}" for k, v in VFS_CONFIGURATIONS[args.vfsprof].items()]
    # RAD_FREQ_SET_FC=175 * 1000000, RAD_FREQ_SET_CL=250 * 1000000, RAD_VOLT_SET=1200
    make_parameters += [f"FREQ_CL={VFS_CONFIGURATIONS[args.vfsprof]['RAD_FREQ_SET_CL']}",
                        f"FREQ_FC={VFS_CONFIGURATIONS[args.vfsprof]['RAD_FREQ_SET_FC']}",
                        f"FREQ_PE={VFS_CONFIGURATIONS[args.vfsprof]['RAD_FREQ_SET_FC']}",
                        f"VOLTAGE={VFS_CONFIGURATIONS[args.vfsprof]['RAD_VOLT_SET']}"]
    make_parameters += CODES_CONFIG[args.benchmark]["make_parameters"]

    reboot_disable = args.noreboot is False
    if os.path.isdir(DATA_DIR) is False:
        os.mkdir(DATA_DIR)
    if os.path.isdir(LOG_PATH) is False:
        os.mkdir(LOG_PATH)

    timer = Timer()
    acc_time = 0
    acc_errors = 0
    # IF not generate load the golds
    if args.generate:
        program_and_generate_golds(reprogram=args.reprogram, make_flags=make_parameters, benchmark=args.benchmark)
    else:
        script_name = os.path.basename(__file__)
        log_file = gen_log_file_name(f"GAP9-{args.benchmark}", log_dir=LOG_PATH)
        experiment_logger = logging_setup(logger_name=script_name, log_file=log_file, logging_level=logging.DEBUG)
        reboot_usb_device(script_name=script_name, logger=experiment_logger, reboot=reboot_disable)
        args_info = " ".join([f"{k}:{v}" for k, v in vars(args).items()])
        experiment_logger.info(f"HEADER: {args_info}")
        benchmark = args.benchmark

        golds_dict = load_the_golds(golds_path=DATA_DIR, benchmark=args.benchmark)
        benchmark_gold_output = golds_dict[args.benchmark]
        bench_exec = CODES_CONFIG[benchmark]["exec"]
        bench_path = CODES_CONFIG[benchmark]["path"]
        bench_timeout = CODES_CONFIG[benchmark]["timeout"]

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
                    if sequential_errors > MAX_SEQUENTIALLY_ERRORS and disable_sequential_errors_check is False:
                        experiment_logger.error(f"MAXIMUM NUMBER OF SEQUENTIALLY ERRORS REACHED SLEEPING FOR "
                                                f"{SLEEP_AFTER_MULTIPLE_ERRORS + AFTER_REBOOT_SLEEPING_TIME} seconds")
                        reboot_usb_device(script_name=script_name, logger=experiment_logger, reboot=reboot_disable)
                        time.sleep(SLEEP_AFTER_MULTIPLE_ERRORS)

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
