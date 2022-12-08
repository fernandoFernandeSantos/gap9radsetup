# Device set
DEVICE = "gap9evk"

# Error threshold for the test
CLASSIFICATION_ABS_THRESHOLD = 0
SEGMENTATION_ABS_THRESHOLD = 0
DETECTION_BOXES_ABS_THRESHOLD = 1e-5
DETECTION_SCORES_ABS_THRESHOLD = 1e-5

MAXIMUM_ERRORS_PER_ITERATION = 4096
MAXIMUM_INFOS_PER_ITERATION = 256
ITERATION_INTERVAL_LOG_HELPER_PRINT = 30

CIFAR10 = "cifar10"
CIFAR100 = "cifar100"
IMAGENET = "imagenet"
COCO = "coco"

CLASSES = {
    CIFAR10: 10,
    CIFAR100: 100,
    IMAGENET: 1000
}

# File to save last status of the benchmark when log helper not active
TMP_CRASH_FILE = "/tmp/diehardnet_crash_file.txt"

VFS_PERFORMANCE = "PERFORMANCE"
VFS_ENERGY_EFFICIENT = "ENERGY_EFFICIENT"

# Gap9 related configurations
VFS_CONFIGS = {
    # High Performance
    VFS_PERFORMANCE: dict(F=370, V=800),
    # Energy Efficient
    VFS_ENERGY_EFFICIENT: dict(F=240, V=650)
}

# Set the supported goals
CLASSIFICATION = "classify"
SEGMENTATION = "segmentation"

AFTER_REBOOT_SLEEPING_TIME = 60
MAX_SEQUENTIALLY_ERRORS = 2
SLEEP_AFTER_MULTIPLE_ERRORS = 120
GAP_SDK_DIR = f"/home/fernando/git_research/gap9riscvsetup/gap_sdk_private"
CODES_CONFIG = {
    "mobilenet_v1": {
        "path": f"",
        "exec": [f"gapy", ],
        "timeout": 10,
        "make_parameters": "",
        "dnn_goal": CLASSIFICATION
    },
    "mobilenet_v2": {
        "path": f"",
        "exec": [f"gapy", ],
        "timeout": 10,
        "make_parameters": "",
        "dnn_goal": CLASSIFICATION
    },
}

DEVICE_NAME = "gap9"
