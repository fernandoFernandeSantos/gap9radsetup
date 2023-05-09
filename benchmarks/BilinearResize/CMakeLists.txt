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
# Panel Control
###############################################################################
set(TARGET_NAME "Bilinear_Resize")

set(RESIZE_GEN_PATH $ENV{TILER_BILINEAR_RESIZE_GENERATOR_PATH})
set(RESIZE_KER_PATH $ENV{TILER_BILINEAR_RESIZE_KERNEL_PATH})
set(MODEL_SRC ResizeModel.c)
set(MODEL_GEN ResizeKernels)
set(MODEL_GEN_C ${MODEL_GEN}.c)
set(MODEL_GEN_H ${MODEL_GEN}.h)
set(MODEL_GEN_CLEAN ${MODEL_GEN_C} ${MODEL_GEN_H})

list(APPEND TARGET_SRCS Bilinear_Resize.c
                        ImgIO.c
                        #  ResizeKernels.c
                        ${RESIZE_KER_PATH}/ResizeBasicKernels.c
                        ${MODEL_GEN_C}
                        $ENV{TILER_EMU_INC}
                        $ENV{TILER_INC}
                        ${RESIZE_KER_PATH}
                        $ENV{TILER_DSP_KERNEL_PATH}
                        $ENV{TILER_CNN_KERNEL_PATH_FP16}

)

list(APPEND TARGET_COMPILE_OPTIONS  -O3
                                    -mno-memcpy
                                    -fno-tree-loop-distribute-patterns
                                    -Wno-maybe-uninitialized
                                    -Wno-unused-but-set-variable
                                    -Wno-unused-variable
                                    -DOUT_FILE_PATH=\"${CMAKE_SOURCE_DIR}/resized_out.ppm\"
                                    -DIN_FILE_PATH=\"${CMAKE_SOURCE_DIR}/Pedestrian.ppm\"
                                    -I$ENV{GAP_SDK_HOME}/tools/autotiler_v3/Emulation
                                    -I$ENV{GAP_SDK_HOME}/tools/autotiler_v3/Autotiler
                                    -I$ENV{GAP_SDK_HOME}/tools/autotiler_v3/Generators/BilinearResizes
                                    -I$ENV{GAP_SDK_HOME}/tools/autotiler_v3/CNN_Libraries_fp16/
                                    -I${CMAKE_BINARY_DIR}
)


add_custom_target(
    gentile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND           gcc -o ${CMAKE_BINARY_DIR}/GenTile -I$ENV{TILER_INC} -I${RESIZE_GEN_PATH} ${CMAKE_SOURCE_DIR}/${MODEL_SRC} ${RESIZE_GEN_PATH}/ResizeGenerator.c $ENV{TILER_LIB}
    SOURCES           ${CMAKE_SOURCE_DIR}/${MODEL_SRC} ${RESIZE_GEN_PATH}/ResizeGenerator.c
    BYPRODUCTS        ${CMAKE_BINARY_DIR}/GenTile
)

add_custom_target(
    model DEPENDS GenTile
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
    COMMAND             ${CMAKE_BINARY_DIR}/GenTile
    SOURCES             ${CMAKE_BINARY_DIR}/GenTile
    BYPRODUCTS          ${CMAKE_BINARY_DIR}/ResizeKernels.c ${CMAKE_BINARY_DIR}/ResizeKernels.h ${CMAKE_BINARY_DIR}/resized_out.ppm
    )

###############################################################################
# CMake pre initialization
###############################################################################

set(CONFIG_GAP_SDK_HOME $ENV{GAP_SDK_HOME})
include($ENV{GAP_SDK_HOME}/utils/cmake/setup.cmake)

project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} ${TARGET_SRCS})

if(TARGET_COMPILE_OPTIONS)
    target_compile_options(${TARGET_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
endif()

###############################################################################
# CMake post initialization
###############################################################################
setupos(${TARGET_NAME})

