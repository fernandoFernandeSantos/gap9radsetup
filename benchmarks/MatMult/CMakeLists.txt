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

cmake_minimum_required(VERSION 3.16)

###############################################################################
# CMake pre initialization
###############################################################################

set(CONFIG_GAP_SDK_HOME $ENV{GAP_SDK_HOME})
include($ENV{GAP_SDK_HOME}/utils/cmake/setup.cmake)
setup_cmake()

###############################################################################
# Panel Control
###############################################################################
set(TARGET_NAME "MatMult")

# Paths
set(MATMULT_GEN_PATH $ENV{TILER_GENERATOR_PATH}/MatMult)
set(MATMULT_KER_PATH $ENV{TILER_GENERATOR_PATH}/MatMult)

set(MODEL_SRC ${CMAKE_SOURCE_DIR}/MatMultModel.c)
set(MODEL_GEN MatMultKernels)
set(MODEL_GEN_C ${MODEL_GEN}.c)
set(MODEL_GEN_H ${MODEL_GEN}.h)



list(APPEND TARGET_SRCS ${CMAKE_SOURCE_DIR}/MatMult.c
                        ${CMAKE_BINARY_DIR}/MatMultKernels.c
                        ${MATMULT_KER_PATH}/MatMultBasicKernels.c
)

list(APPEND TARGET_INCLUDES -I$ENV{TILER_INC}
                            -I$ENV{TILER_EMU_INC}
                            -I.
                            -I${MATMULT_KER_PATH}
                            -I${CMAKE_BINARY_DIR}
)

list(APPEND TARGET_CFLAGS   -O3
                            -mno-memcpy
                            -fno-tree-loop-distribute-patterns
                            -Wno-maybe-uninitialized
                            -Wno-unused-but-set-variable
                            -Wno-unused-variable
)

add_custom_target(
    gentile
    COMMAND     gcc -o ${CMAKE_BINARY_DIR}/GenTile -I$ENV{TILER_INC} -I$ENV{TILER_EMU_INC} -I${MATMULT_GEN_PATH} ${MODEL_SRC} ${MATMULT_GEN_PATH}/MatMultGenerator.c $ENV{TILER_LIB}
    BYPRODUCTS   ${CMAKE_BINARY_DIR}/GenTile
)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/${MODEL_GEN_C}
    DEPENDS ${CMAKE_BINARY_DIR}/GenTile
    COMMAND ${CMAKE_BINARY_DIR}/GenTile
)

add_custom_target(
    model
    DEPENDS ${CMAKE_BINARY_DIR}/${MODEL_GEN_C}
)

project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} ${TARGET_SRCS})

target_compile_options(${TARGET_NAME} PUBLIC ${TARGET_INCLUDES} ${TARGET_CFLAGS})

###############################################################################
# CMake post initialization
###############################################################################
setupos(${TARGET_NAME})

