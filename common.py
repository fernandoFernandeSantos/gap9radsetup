import os

import argparse
import time

MXM = "MatMult"
FMXM = "MatMulNew"
MMAD = "MatrixAdd"
MNIST = "Mnist"
FIR = "Fir"
CRC = "crc"
BILINEAR_RESIZE = "BilinearResize"
# MEM tests
MEM_TEST = "memradtest"
CNN_OP = "cnn"
MOBILENET_V1 = "mobilenetv1"
MOBILENET_V2 = "mobilenetv2"

MEMORIES_TO_TEST = {
    "L1": "MEM_LEVEL=1",
    "L2": "MEM_LEVEL=2"
}

# FPU microbenchmarks
FPU_MICROBENCHMARKS = "testFPU"

CNN_OPS_TO_TEST = dict(
    SEQUENTIAL_MAX_POOL=0,
    SEQUENTIAL_AVG_MAX_POOL=1,
    SEQUENTIAL_CONV=2,
    SEQUENTIAL_LINEAR=3,
    PARALLEL_VECT_MAX_POOL=14,
    PARALLEL_VECT_AVG_MAX_POOL=15,
    PARALLEL_VECT_CONV=16,
    PARALLEL_VECT_LINEAR=17,
)

CNN_OPS_TO_TEST = {k: f"CNN_OP={v}" for k, v in CNN_OPS_TO_TEST.items()}

FPU_MICRO_TO_TEST = dict(
    FADDS=0,
    FSUBS=1,
    FMULS=2,
    FDIVS=3,
    FSQRTS=4,
    FMADDS=5,
    FNMADDS=6,
    FMSUBS=7,
    FNMSUBS=8,
)

FPU_MICRO_TO_TEST = {k: f"TEST_MICRO_ID={v}" for k, v in FPU_MICRO_TO_TEST.items()}

AFTER_REBOOT_SLEEPING_TIME = 20
GENERAL_TIMEOUT = 50
MAX_SEQUENTIALLY_ERRORS = 10
SLEEP_AFTER_MULTIPLE_ERRORS = 20

try:
    GAP_SDK_DIR = os.environ["GAP_SDK_HOME"]
except EnvironmentError:
    raise EnvironmentError("Set the GAP_SDK_DIR_RAD env var first")

SETUP_PATH = os.path.abspath(os.getcwd())
BENCHMARKS_DIR = f"{SETUP_PATH}/benchmarks"

HOST_IP = "192.168.1.200"
SWITCH_IP = "192.168.1.102"
SWITCH_PORT = 8

CODES_CONFIG = {
    CRC: {
        "path": f"{BENCHMARKS_DIR}/{CRC}",
        "exec": 'openocd -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" '
                f'-f "{GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg" '
                f'-f "{GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl" '
                f'-c "load_and_start_binary {BENCHMARKS_DIR}/{CRC}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{CRC} 0x1c010100"',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },
    MXM: {
        "path": f"{BENCHMARKS_DIR}/{MXM}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={BENCHMARKS_DIR}/{MXM}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={BENCHMARKS_DIR}/{MXM}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{MXM} '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg '
                f'--openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },
    FMXM: {
        "path": f"{BENCHMARKS_DIR}/{FMXM}",
        "exec":
            'openocd -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" '
            f'-f "{GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg" '
            f'-f "{GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl" '
            f'-c "load_and_start_binary {BENCHMARKS_DIR}/{FMXM}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/test 0x1c010100"',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },
    MMAD: {
        "path": f"{BENCHMARKS_DIR}/{MMAD}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={BENCHMARKS_DIR}/{MMAD}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={BENCHMARKS_DIR}/{MMAD}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{MMAD} '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg '
                f'--openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },
    MNIST: {
        "path": f"{BENCHMARKS_DIR}/{MNIST}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={BENCHMARKS_DIR}/{MNIST}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={BENCHMARKS_DIR}/{MNIST}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{MNIST} '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg '
                f'--openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },

    BILINEAR_RESIZE: {
        "path": f"{BENCHMARKS_DIR}/{BILINEAR_RESIZE}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={BENCHMARKS_DIR}/{BILINEAR_RESIZE}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={BENCHMARKS_DIR}/{BILINEAR_RESIZE}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/Bilinear_Resize '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg '
                f'--openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": GENERAL_TIMEOUT,
        "make_parameters": ["run"]
    },
    CNN_OP: {},
    MEM_TEST: {
        "path": f"{BENCHMARKS_DIR}/{MEM_TEST}",
        "exec": f'{GAP_SDK_DIR}/utils/gapy_v2/bin/gapy '
                f'--target=gap9.evk '
                f'--target-dir={GAP_SDK_DIR}/utils/gapy_v2/targets '
                f'--platform=board '
                f'--work-dir={BENCHMARKS_DIR}/{MEM_TEST}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS '
                f'--binary={BENCHMARKS_DIR}/{MEM_TEST}/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/{MEM_TEST} '
                f'--config-opt=**/runner/boot/mode=jtag    '
                f'--flash-property=0@flash:lfs:size '
                f'--openocd-cable={GAP_SDK_DIR}/utils/openocd_tools/tcl/gapuino_ftdi.cfg '
                f'--openocd-script={GAP_SDK_DIR}/utils/openocd_tools/tcl/gap9revb.tcl '
                f'--openocd-tools={GAP_SDK_DIR}/utils/openocd_tools '
                f'--flash-property={GAP_SDK_DIR}/utils/ssbl/bin/ssbl-gap9_evk@mram:rom:binary run',
        "timeout": 100,
        "make_parameters": ["run"]
    },
    FPU_MICROBENCHMARKS: {
        "path": f"{BENCHMARKS_DIR}/{FPU_MICROBENCHMARKS}",
        "exec":
            'openocd -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" '
            '-f "/home/carol/git_research/gap9radsetup/gap_sdk_private/utils/openocd_tools/tcl/gapuino_ftdi.cfg" '
            '-f "/home/carol/git_research/gap9radsetup/gap_sdk_private/utils/openocd_tools/tcl/gap9revb.tcl" '
            '-c "load_and_start_binary /home/carol/git_research/gap9radsetup/gap9radsetup/benchmarks/'
            'testFPU/BUILD/GAP9_V2/GCC_RISCV_FREERTOS/testFPU 0x1c010100"',
        "timeout": 60,
        "make_parameters": ["run"]
    }
}

VFS_PERFORMANCE = "performance"
VFS_ENERGY = "energy"
VFS_EXTREME = "extreme"
VFS_MIDDLE = "middle"

# TODO: check if this make sense
VFS_CONFIGURATIONS = {
    VFS_PERFORMANCE: dict(FREQ_FC=370 * 1000 * 1000, FREQ_CL=370 * 1000 * 1000, VOLT_SET=800),
    VFS_EXTREME: dict(FREQ_FC=370 * 1000 * 1000, FREQ_CL=370 * 1000 * 1000, VOLT_SET=650),
    VFS_MIDDLE: dict(FREQ_FC=300 * 1000 * 1000, FREQ_CL=300 * 1000 * 1000, VOLT_SET=650),
    VFS_ENERGY: dict(FREQ_FC=240 * 1000 * 1000, FREQ_CL=240 * 1000 * 1000, VOLT_SET=650),
}

KILL_PROGRAM_LIST = ["gapy", "openocd"]

DATA_DIR = "data"
LOG_PATH = f"{DATA_DIR}/logs"


def parse_arguments():
    parser = argparse.ArgumentParser(description='GAP8 Setup')
    parser.add_argument('--iterations', type=int, default=int(1e6), help='Number of iterations for the experiment')
    parser.add_argument('--generate', default=False, action="store_true",
                        help="Set this flag to generate the golds and reprogram the board")
    parser.add_argument('--benchmark', default=None,
                        help='Benchmarks, can be ' + ", ".join(list(CODES_CONFIG.keys())))
    parser.add_argument('--reprogram', default=False, action="store_true",
                        help="This will in addition to golden generation reprogram the board")
    parser.add_argument('--noreboot', default=False, action="store_true", help="Enable or disable reboot")
    parser.add_argument('--memtotest', default=None,
                        help=f"If the benchmark is memradtest,  inform which memory "
                             f"to test:{MEMORIES_TO_TEST.keys()}")
    parser.add_argument('--disablecheckseqerr', default=False, action="store_true",
                        help="Enable or disable sequential errors check")
    parser.add_argument('--cnnop', default=None,
                        help=f"If the benchmark is cnn,  inform which CNNop to "
                             f"test:{CNN_OPS_TO_TEST.keys()}")
    parser.add_argument('--fpuop', default=None,
                        help=f"If the benchmark is cnn,  inform which FPU to "
                             f"test:{FPU_MICRO_TO_TEST.keys()}")
    parser.add_argument('--vfsprof', default=VFS_PERFORMANCE,
                        help=f"Voltage Frequenccy scaling, it can be {VFS_PERFORMANCE}, "
                             f"{VFS_ENERGY}, {VFS_MIDDLE}")
    args = parser.parse_args()
    return args


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
