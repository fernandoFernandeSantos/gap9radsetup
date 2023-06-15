# Copyright (c) 2022 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.19)

###############################################################################
# CMake pre initialization
###############################################################################

set(CONFIG_GAP_SDK_HOME $ENV{GAP_SDK_HOME})
include($ENV{GAP_SDK_HOME}/utils/cmake/setup.cmake)

###############################################################################
# Panel Control
###############################################################################
set(TARGET_NAME "MatMulNew")

set(MATMUL_MODEL_GEN   GenMatMul)

# File definition
set(MATMUL_SRCG            $ENV{TILER_DSP_GENERATOR_PATH}/DSP_Generators.c)
set(MATMUL_KERNEL_NAME     MatMulKernels)
set(MATMUL_SRC_CODE        ${CMAKE_BINARY_DIR}/${MATMUL_KERNEL_NAME}.c)
set(MATMUL_HEADER          ${CMAKE_BINARY_DIR}/${MATMUL_KERNEL_NAME}.h)

# Memory
set(CLUSTER_STACK_SIZE 2048)
set(CLUSTER_SLAVE_STACK_SIZE 1024)
math(EXPR TOTAL_STACK_SIZE "${CLUSTER_STACK_SIZE} + ${CLUSTER_SLAVE_STACK_SIZE} * 8")
math(EXPR MODEL_L1_MEMORY "125000 - ${TOTAL_STACK_SIZE}")
list(APPEND MODEL_GEN_EXTRA_FLAGS --L1 ${MODEL_L1_MEMORY})

# Flags
list(APPEND TABLE_CFLAGS -lm)

list(APPEND MATMUL_OPT_FLAG -DW_M1=64
                            -DH_M1=64
                            -DW_M2=64)

list(APPEND SDL_FLAGS   -lSDL2
                        -lSDL2_ttf
                        -DAT_DISPLAY)

list(APPEND TARGET_SRCS ${CMAKE_SOURCE_DIR}/MatMulRunTest.c
                        ${MATMUL_SRC_CODE}
                        $ENV{MODEL_LIB_DSP}
                        $ENV{TILER_DSP_KERNEL_PATH}/MatMulDSP.c
)

list(APPEND TARGET_COMPILE_OPTIONS  -O3
                                    -I.
                                    -I$ENV{TILER_DSP_KERNEL_PATH}
                                    -I$ENV{TILER_DSP_GENERATOR_PATH}
                                    -I${CMAKE_BINARY_DIR}
                                    -I$ENV{TILER_INC}
                                    -I$ENV{TILER_EMU_INC}
                                    -DPERF
                                    ${MATMUL_OPT_FLAG}
                                    )

add_custom_command(
    OUTPUT              ${CMAKE_BINARY_DIR}/${MATMUL_MODEL_GEN}
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
    COMMAND             gcc -g -o ${CMAKE_BINARY_DIR}/${MATMUL_MODEL_GEN} -I${CMAKE_BINARY_DIR} -I$ENV{TILER_DSP_GENERATOR_PATH} -I$ENV{TILER_INC} -I$ENV{TILER_EMU_INC} ${CMAKE_SOURCE_DIR}/MatMulModel.c ${MATMUL_SRCG} $ENV{TILER_LIB} ${TABLE_CFLAGS} ${MATMUL_OPT_FLAG} ${SDL_FLAGS}
)

add_custom_command(
    OUTPUT              ${MATMUL_SRC_CODE} ${MATMUL_HEADER}
    WORKING_DIRECTORY   ${CMAKE_SOURCE_DIR}
    DEPENDS             ${CMAKE_BINARY_DIR}/${MATMUL_MODEL_GEN}
	COMMAND             ${CMAKE_BINARY_DIR}/${MATMUL_MODEL_GEN} -o ${CMAKE_BINARY_DIR} -c ${CMAKE_BINARY_DIR} ${MODEL_GEN_EXTRA_FLAGS}
)

add_custom_target(
    gen_matmul_code
    DEPENDS         ${MATMUL_SRC_CODE}
)

project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} ${TARGET_SRCS})

if(TARGET_COMPILE_OPTIONS)
    target_compile_options(${TARGET_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
endif()

target_link_libraries(${TARGET_NAME} PRIVATE m)

###############################################################################
# CMake post initialization
###############################################################################
setupos(${TARGET_NAME})

