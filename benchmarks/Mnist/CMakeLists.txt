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

set(TARGET_NAME         "Mnist")
project(${TARGET_NAME}  C ASM)

###############################################################################
# Autotiler settings
###############################################################################

set(AT_MODEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/MnistModel.c)
autotiler_define_gen_names(${TARGET_NAME})
autotiler_setup(${TARGET_NAME})

###############################################################################
# Application dependencies
###############################################################################

list(APPEND TARGET_SRCS ${TARGET_NAME}.c
			${CNN_LIB}
			${CONFIG_MODEL_BUILDDIR}/${KERNEL_C}
)

list(APPEND TARGET_INCS -I.
			-I${TILER_INC}
                        -I${TILER_EMU_INC}
                        -I${GAP_SDK_HOME}/libs/gap_lib/include
                        -I${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_MODEL_BUILDDIR}
			-I${CMAKE_CURRENT_SOURCE_DIR}
			${CNN_LIB_INCLUDE}
)   

list(APPEND TARGET_CFLAGS   -w
                            -O3
                            -mno-memcpy
                            -fno-tree-loop-distribute-patterns
                            -fdata-sections
                            -ffunction-sections
			    -fcommon
                            -Wno-maybe-uninitialized
                            -Wno-unused-but-set-variable
                            -Wno-maybe-uninitialized 
                            -Wno-unused-but-set-variable
)

if(DEFINED USE_BRIDGE)
    list(APPEND TARGET_CFLAGS   -DENABLE_BRIDGE
                                -fileIO)
endif()
                                
list(APPEND TARGET_LDFLAGS  -Wl,--gc-sections
                            -flto)

add_executable(${TARGET_NAME} ${TARGET_SRCS})

target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_CFLAGS} ${TARGET_INCS})
target_link_options(${TARGET_NAME} PRIVATE -flto -Wl,--gc-section)

###############################################################################
# CMake post initialization
###############################################################################
setupos(${TARGET_NAME})
