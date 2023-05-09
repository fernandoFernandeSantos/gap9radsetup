cluster = 1
APP = testFPU
APP_SRCS = main.c # math_fns.c # testFPU.c
APP_CFLAGS += -O2 -fno-unroll-loops

ifeq '$(cluster)' '1'
APP_CFLAGS += -DUSE_CLUSTER
endif

include $(RULES_DIR)/pmsis_rules.mk
