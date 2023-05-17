cluster = 1
APP = testFPU
APP_SRCS = main.c # math_fns.c # testFPU.c
APP_CFLAGS += -O2 -fno-unroll-loops

ifeq '$(cluster)' '1'
APP_CFLAGS += -DUSE_CLUSTER
endif

APP_CFLAGS += -DTEST_MICRO_ID=$(TEST_MICRO_ID)
APP_CFLAGS += -DVOLT_SET=$(VOLT_SET)
APP_CFLAGS += -DFREQ_FC=$(FREQ_FC)
APP_CFLAGS += -DFREQ_CL=$(FREQ_CL)
APP_CFLAGS += -DFREQ_PE=$(FREQ_PE)

CONFIG_FREQUENCY_FC          = $(FREQ_FC)
CONFIG_FREQUENCY_CLUSTER     = $(FREQ_CL)
CONFIG_FREQUENCY_SFU         = $(FREQ_FC)
CONFIG_FREQUENCY_FPGA        = $(FREQ_FC)

include $(RULES_DIR)/pmsis_rules.mk
