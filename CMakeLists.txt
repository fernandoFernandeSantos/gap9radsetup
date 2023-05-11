cmake_minimum_required(VERSION 3.23)
project(dnnradtestgap9 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(benchmarks/efficient_net)
include_directories(benchmarks/mobilenet)
include_directories(benchmarks/mobilenet/accuracy_on_validation_data)
include_directories(benchmarks/mobilenet/common)
include_directories(benchmarks/mobilenet/common/headers)
include_directories(benchmarks/resnet)

add_executable(
        dnnradtestgap9
        benchmarks/efficient_net/efficientnet_lite.c
        benchmarks/efficient_net/efficientnet_lite.h
        benchmarks/mobilenet/accuracy_on_validation_data/main_emul.c
        benchmarks/mobilenet/accuracy_on_validation_data/ordered_synset.h
        benchmarks/mobilenet/common/headers/mcunet_imagenet_best.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_25_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_25_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_25_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_25_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_5_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_5_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_5_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_5_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_75_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_75_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_75_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_0_75_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_1_0_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_1_0_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_1_0_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v1_1_0_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_5_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_5_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_5_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_5_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_5_96_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_75_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_75_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_75_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_75_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_0_75_96_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_0_128_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_0_160_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_0_192_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_0_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_0_96_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v2_1_4_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v3_large_minimalistic_1_0_224_quant.h
        benchmarks/mobilenet/common/headers/mobilenet_v3_small_1_0_224_quant.h
        benchmarks/mobilenet/common/headers/squeezenet.h
        benchmarks/mobilenet/main.c
        benchmarks/mobilenet/main.h
        benchmarks/mobilenet/main_fp16.c
        benchmarks/resnet/main.c
        benchmarks/resnet/ResNet.h
        benchmarks/perf/performance_counter.h
        benchmarks/cnn/AllTest.c
        benchmarks/MatMult/MatMult.c
        benchmarks/BilinearResize/Bilinear_Resize.c
        benchmarks/Mnist/Mnist.c
        benchmarks/MatrixAdd/MatAdd.c

        benchmarks/testFPU/main.c
        benchmarks/memradtest/memradtest.c
)
