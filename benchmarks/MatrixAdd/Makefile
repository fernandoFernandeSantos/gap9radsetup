# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = MatrixAdd

MATADD_GEN_PATH = $(TILER_GENERATOR_PATH)/MatAdd
MATADD_KER_PATH = $(TILER_GENERATOR_PATH)/MatAdd

APP_SRCS += MatAdd.c
APP_SRCS += MatAddKernels.c $(MATADD_KER_PATH)/MatAddBasicKernels.c

APP_INC += . $(TILER_EMU_INC) $(TILER_INC) $(MATADD_KER_PATH)

APP_CFLAGS += -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-unused-variable

APP_CFLAGS      += -DRAD_CNN_OP=$(RAD_CNN_OP) -DRAD_FREQ_SET_FC=$(RAD_FREQ_SET_FC)
APP_CFLAGS      += -DRAD_FREQ_SET_CL=$(RAD_FREQ_SET_CL) -DRAD_VOLT_SET=$(RAD_VOLT_SET)
APP_CFLAGS      += -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL) -DFREQ_PE=$(FREQ_PE)

MODEL_SRC = MatAddModel.c
MODEL_GEN = MatAddKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))

RM=rm -f

build:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(MATADD_GEN_PATH) $(MODEL_SRC) $(MATADD_GEN_PATH)/MatAddGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
