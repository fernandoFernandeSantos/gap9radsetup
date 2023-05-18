# User Test
#------------------------------------
SIZE = 128
W_M1 = 128
H_M1 = 128
W_M2 = 128
include MatMulModel.mk
include $(RULES_DIR)/at_common_decl.mk

io?=host

APP              = test
APP_SRCS        += MatMulRunTest.c $(MATMUL_SRC_CODE) $(MODEL_LIB_DSP)
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I$(TILER_DSP_KERNEL_PATH) -I$(MATMULBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC)
APP_CFLAGS 		+= -DPERF $(MATMUL_OPT_FLAG) #-DPRINTDEB #-DPRINT_INOUT

#GENERATE_GOLDEN=1
ifeq ($(GENERATE_GOLDEN), 1)
APP_CFLAGS += -DGENERATE_GOLDEN
endif

#FREQ_FC =370000000
#FREQ_PE =370000000
#FREQ_CL =370000000
#VOLT_SET =800

APP_CFLAGS += -DVOLT_SET=$(VOLT_SET)
APP_CFLAGS += -DFREQ_FC=$(FREQ_FC)
APP_CFLAGS += -DFREQ_CL=$(FREQ_CL)
APP_CFLAGS += -DFREQ_PE=$(FREQ_PE)

build:: gen_matmul_code

include $(RULES_DIR)/pmsis_rules.mk
