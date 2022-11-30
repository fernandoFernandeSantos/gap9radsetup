NNTOOL=nntool
MODEL_SQ8=1
# MODEL_POW2=1
# MODEL_FP16=1
MODEL_NE16=1


MODEL_SUFFIX?=
MODEL_PREFIX?=ResNet
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)

NNTOOL_SCRIPT = model/nntool_script

MODEL_TYPE ?= "fp32"
MODEL_SIZE ?= 50
ifeq ($(MODEL_SIZE), 50)
    TRAINED_MODEL = model/resnet50-v1-12
    STATS_DICT = model/resnet50_astats.pickle
else
    TRAINED_MODEL = model/resnet18-v1-7
    STATS_DICT = model/resnet18_astats.pickle
endif
ifeq '$(MODEL_TYPE)' 'int8'
    TRAINED_MODEL := $(TRAINED_MODEL)-int8
    NNTOOL_SCRIPT := $(NNTOOL_SCRIPT)_quant
endif
TRAINED_MODEL := $(TRAINED_MODEL).onnx


MODEL_EXPRESSIONS = $(MODEL_BUILD)/Expression_Kernels.c

NNTOOL_EXTRA_FLAGS += 

# Options for the memory settings: will require
# set l3_flash_device $(MODEL_L3_FLASH)
# set l3_ram_device $(MODEL_L3_RAM)
# in the nntool_script
# FLASH and RAM type
FLASH_TYPE = DEFAULT
RAM_TYPE   = DEFAULT

ifeq '$(FLASH_TYPE)' 'HYPER'
    MODEL_L3_FLASH=AT_MEM_L3_HFLASH
else ifeq '$(FLASH_TYPE)' 'MRAM'
    MODEL_L3_FLASH=AT_MEM_L3_MRAMFLASH
    READFS_FLASH = target/chip/soc/mram
else ifeq '$(FLASH_TYPE)' 'QSPI'
    MODEL_L3_FLASH=AT_MEM_L3_QSPIFLASH
    READFS_FLASH = target/board/devices/spiflash
else ifeq '$(FLASH_TYPE)' 'OSPI'
    MODEL_L3_FLASH=AT_MEM_L3_OSPIFLASH
else ifeq '$(FLASH_TYPE)' 'DEFAULT'
    MODEL_L3_FLASH=AT_MEM_L3_DEFAULTFLASH
endif

ifeq '$(RAM_TYPE)' 'HYPER'
    MODEL_L3_RAM=AT_MEM_L3_HRAM
else ifeq '$(RAM_TYPE)' 'QSPI'
    MODEL_L3_RAM=AT_MEM_L3_QSPIRAM
else ifeq '$(RAM_TYPE)' 'OSPI'
    MODEL_L3_RAM=AT_MEM_L3_OSPIRAM
else ifeq '$(RAM_TYPE)' 'DEFAULT'
    MODEL_L3_RAM=AT_MEM_L3_DEFAULTRAM
endif

USE_PRIVILEGED_FLASH?=0
ifeq ($(USE_PRIVILEGED_FLASH), 1)
MODEL_SEC_L3_FLASH=AT_MEM_L3_MRAMFLASH
else
MODEL_SEC_L3_FLASH=
endif

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
    FREQ_CL?=370
    FREQ_FC?=370
    FREQ_PE?=370
else
    ifeq '$(TARGET_CHIP)' 'GAP8_V3'
    FREQ_CL?=175
    else
    FREQ_CL?=50
    endif
    FREQ_FC?=250
    FREQ_PE?=250
endif

# Memory sizes for cluster L1, SoC L2 and Flash
TARGET_L1_SIZE = 128000
TARGET_L2_SIZE = 1350000
TARGET_L3_SIZE = 8000000

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=2048
CLUSTER_SLAVE_STACK_SIZE=512

QUANT_DATASET = $(CURDIR)/quant_data_ppm
$(QUANT_DATASET):
	./download_quant_data.sh

$(STATS_DICT): | $(QUANT_DATASET)
	python model/collect_statistics.py $(TRAINED_MODEL) --stats_path $(STATS_DICT) --n_images 5

stats: $(STATS_DICT)

clean_stats:
	rm -rf $(STATS_DICT)

$(info GEN ... $(CNN_GEN))
