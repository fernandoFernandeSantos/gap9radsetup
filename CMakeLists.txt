cmake_minimum_required(VERSION 3.23)
project(dnnradtestgap9 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(efficient_net)
include_directories(mobilenet)
include_directories(mobilenet/accuracy_on_validation_data)
include_directories(mobilenet/common)
include_directories(mobilenet/common/headers)
include_directories(resnet)

add_executable(
        dnnradtestgap9
        efficient_net/efficientnet_lite.c
        efficient_net/efficientnet_lite.h
        mobilenet/accuracy_on_validation_data/main_emul.c
        mobilenet/accuracy_on_validation_data/ordered_synset.h
        mobilenet/common/headers/mcunet_imagenet_best.h
        mobilenet/common/headers/mobilenet_v1_0_25_128_quant.h
        mobilenet/common/headers/mobilenet_v1_0_25_160_quant.h
        mobilenet/common/headers/mobilenet_v1_0_25_192_quant.h
        mobilenet/common/headers/mobilenet_v1_0_25_224_quant.h
        mobilenet/common/headers/mobilenet_v1_0_5_128_quant.h
        mobilenet/common/headers/mobilenet_v1_0_5_160_quant.h
        mobilenet/common/headers/mobilenet_v1_0_5_192_quant.h
        mobilenet/common/headers/mobilenet_v1_0_5_224_quant.h
        mobilenet/common/headers/mobilenet_v1_0_75_128_quant.h
        mobilenet/common/headers/mobilenet_v1_0_75_160_quant.h
        mobilenet/common/headers/mobilenet_v1_0_75_192_quant.h
        mobilenet/common/headers/mobilenet_v1_0_75_224_quant.h
        mobilenet/common/headers/mobilenet_v1_1_0_128_quant.h
        mobilenet/common/headers/mobilenet_v1_1_0_160_quant.h
        mobilenet/common/headers/mobilenet_v1_1_0_192_quant.h
        mobilenet/common/headers/mobilenet_v1_1_0_224_quant.h
        mobilenet/common/headers/mobilenet_v2_0_5_128_quant.h
        mobilenet/common/headers/mobilenet_v2_0_5_160_quant.h
        mobilenet/common/headers/mobilenet_v2_0_5_192_quant.h
        mobilenet/common/headers/mobilenet_v2_0_5_224_quant.h
        mobilenet/common/headers/mobilenet_v2_0_5_96_quant.h
        mobilenet/common/headers/mobilenet_v2_0_75_128_quant.h
        mobilenet/common/headers/mobilenet_v2_0_75_160_quant.h
        mobilenet/common/headers/mobilenet_v2_0_75_192_quant.h
        mobilenet/common/headers/mobilenet_v2_0_75_224_quant.h
        mobilenet/common/headers/mobilenet_v2_0_75_96_quant.h
        mobilenet/common/headers/mobilenet_v2_1_0_128_quant.h
        mobilenet/common/headers/mobilenet_v2_1_0_160_quant.h
        mobilenet/common/headers/mobilenet_v2_1_0_192_quant.h
        mobilenet/common/headers/mobilenet_v2_1_0_224_quant.h
        mobilenet/common/headers/mobilenet_v2_1_0_96_quant.h
        mobilenet/common/headers/mobilenet_v2_1_4_224_quant.h
        mobilenet/common/headers/mobilenet_v3_large_minimalistic_1_0_224_quant.h
        mobilenet/common/headers/mobilenet_v3_small_1_0_224_quant.h
        mobilenet/common/headers/squeezenet.h
        mobilenet/main.c
        mobilenet/main.h
        mobilenet/main_fp16.c
        resnet/main.c
        resnet/ResNet.h
        perf/performance_counter.h cnn/AllTest.c MatMult/MatMult.c BilinearResize/Bilinear_Resize.c Mnist/Mnist.c MatrixAdd/MatAdd.c)
