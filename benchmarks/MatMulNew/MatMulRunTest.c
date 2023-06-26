/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#ifndef __EMUL__

#include "pmsis.h"

#else
#include <stdlib.h>
#define pmsis_exit(a)   exit(a)
#endif

#include "Gap.h"
#include "MatMulKernels.h"
#include "math.h"

#define STACK_SIZE ( 1024 * 2 )
#define CID        ( 0 )

PI_L2 float *M1fp32, *M2fp32, *Outfp32;

#define FLOAT_DIFF_THRESHOLD 0.0f
#define SETUP_ITERATIONS 32

#define MEASURE_TIME 0

#define GPIO_PAD1 (PI_PAD_068)
#define GPIO_PIN1 (PI_GPIO_A68)

#ifdef GENERATE_GOLDEN
PI_L2 float *OutGT;
#else

#include "golden.h"

#endif
extern char *L1_Memory;

int W_Out, H_Out, H_M2;


static void cluster_main() {
    //printf("cluster master start\n");

//#ifdef PERF
//    gap_cl_starttimer();
//    gap_cl_resethwtimer();
//#endif

    /* fp32 */
//    int start = gap_cl_readhwtimer();
    MatMul_fp32(M1fp32, M2fp32, Outfp32);
//    int elapsedfp32 = gap_cl_readhwtimer() - start;
//    errors = 0;
////    float MaxErrfp32 = 0.0f;
//    float SumSquared = 0.0f;
//    for (int h = 0; h < H_Out; h++) {
//        for (int w = 0; w < W_Out; w++) {
//            float diff = fabsf(Outfp32[h * W_Out + w] != OutGT[h * W_Out + w]);
//            if (diff > 0.0f) {
//                errors++;
//                printf("Error[%d, %d]: %f != %f\n", h, w, Outfp32[h * W_Out + w], OutGT[h * W_Out + w]);
////                if (diff > MaxErrfp32) {
////                    MaxErrfp32 = diff;
////                }
//            }
//        }
//    }
}

void generate_golden(int print_to_file) {
    /* Simple sequential in L2 */
    for (int h = 0; h < H_M1; h++) {
        for (int w2 = 0; w2 < W_M2; w2++) {
            float Acc = 0.0f;
            for (int w = 0; w < W_M1; w++) {
                Acc += M1fp32[h * W_M1 + w] * M2fp32[w * W_M2 + w2];
            }
            OutGT[h * W_M2 + w2] = Acc;
        }
    }
    if (print_to_file == 1) {
        // Just checking if the types are the same size
        if (sizeof(uint32_t) != sizeof(float)) {
            printf("Float and uint32_t don't have the same size\n");
            pmsis_exit(-1);
        }
        printf(
                "#ifndef GOLDEN_H\n"
                "#define GOLDEN_H\n"
                "\n"
                "PI_L2 uint32_t reinterpret_pointer_global[] = {\n"
        );
        uint32_t * reinterpreted_pointer = (uint32_t*) OutGT;
        for (int h = 0; h < H_M1; h++) {
            for (int w2 = 0; w2 < W_M2; w2++) {
                printf("0x%X, ", reinterpreted_pointer[h * W_M2 + w2]);
            }
        }
        printf(
                "};\n"
                "\nPI_L2 float *OutGT =  (float*) reinterpret_pointer_global;\n\n"
                "#endif\n"
        );
    }
}


int compare_output() {
    int errors = 0;
    for (int h = 0; h < H_Out; h++) {
        for (int w = 0; w < W_Out; w++) {
            if (Outfp32[h * W_Out + w] != OutGT[h * W_Out + w]) {
                errors++;
                printf("Error[%d, %d]: %f != %f\n", h, w, Outfp32[h * W_Out + w], OutGT[h * W_Out + w]);
            }
            // Set to zero for the next iteration
            Outfp32[h * W_Out + w] = 0.0f;
        }
    }
    return errors;
}

void run_MatMult(void) {
//#ifndef __EMUL__
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = (int) CID;   /* Cluster ID. */
    conf.cc_stack_size = STACK_SIZE;
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, (void *) &conf);
    if (pi_cluster_open(&cluster_dev)) {
        printf("Cluster open failed !\n");
        pmsis_exit(-2);
    }
//#endif
    printf("Matrix Mult start\n");
    W_Out = W_M2;
    H_Out = H_M1;
    H_M2 = W_M1;
    int num_op = H_M1 * W_M2 * (W_M1 + H_M2 - 1);
    int AllocatedSpace = (sizeof(float) + sizeof(short)) * (W_M1 * H_M1 + W_M2 * H_M2 + W_Out * H_Out) +
                         sizeof(float) * W_Out * H_Out;
//#ifdef __gap9__
//    AllocatedSpace += sizeof(short) * (W_M1*H_M1 + W_M2*H_M2 + W_Out*H_Out);
//#endif

    //GPIO A1
    pi_pad_function_set(GPIO_PAD1, PI_PAD_FUNC1);
    pi_gpio_e gpio_out_trigger_pin = GPIO_PIN1;
    /* Configure gpio output. */
    pi_gpio_flags_e cfg_flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(gpio_out_trigger_pin, cfg_flags);
    // Start with the pin on 0
    pi_gpio_pin_write(gpio_out_trigger_pin, 0);

    M1fp32 = (float *) AT_L2_ALLOC(0, W_M1 * H_M1 * sizeof(float));
    M2fp32 = (float *) AT_L2_ALLOC(0, W_M2 * H_M2 * sizeof(float));

#ifdef GENERATE_GOLDEN
    OutGT = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));
#endif
    Outfp32 = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));

    if ((M1fp32 == NULL) || (M2fp32 == NULL) || (Outfp32 == NULL)
#ifdef GENERATE_GOLDEN
        || (OutGT == NULL)
#endif
            ) {
        printf("Failed to allocate fp32 and Fix16 Matrixes\n");
        pmsis_exit(-1);
    }
    /* Allocate the predetermined memory size in the shared L1 memory that the cluster can act on. */
    L1_Memory = (char *) AT_L1_ALLOC(0, _L1_Memory_SIZE);
    if (L1_Memory == NULL) {
        printf("Memory Allocation Error! Quit...\n");
        pmsis_exit(-3);
    }

    /* Init Data */
//    int QIN = 15 - gap_fl1(W_M1);
//    printf("QIN: %d\n", QIN);
//    int Norm = QIN;
    if (H_M1 != H_M2 || W_M1 != W_M2) {
        printf("This code only works on equal sized matrices\n");
        pmsis_exit(-3);
    }
    for (int tt = 0; tt < H_M1 * W_M1; tt++) {
        M1fp32[tt] = tt * 0.0034f;
        M2fp32[tt] = tt * 0.0011f;
    }

//#ifdef PERF
//    gap_fc_starttimer();
//    gap_fc_resethwtimer();
//#endif
    int start = gap_fc_readhwtimer();
#ifdef GENERATE_GOLDEN
    generate_golden(1);
#endif
    int elapsedSeq = gap_fc_readhwtimer() - start;


//#ifndef __EMUL__
    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task task;
    pi_cluster_task(&task, cluster_main, NULL);

#ifndef GENERATE_GOLDEN
    const int setup_it = SETUP_ITERATIONS;
#else
    const int setup_it = 1;
#endif

    /* Offloading Task to cluster. */
    int errors = 0;
    for (int its = 0; (its < setup_it) & (errors == 0); its++) {
        pi_gpio_pin_write(gpio_out_trigger_pin, 1);
#if MEASURE_TIME == 1
        uint32_t start = pi_time_get_us();
#endif
        pi_cluster_send_task(&cluster_dev, &task);
#if MEASURE_TIME == 1
        uint32_t end = pi_time_get_us();
#endif
        pi_gpio_pin_write(gpio_out_trigger_pin, 0);

//        if (its == 5) Outfp32[34] = 333333;
        errors = compare_output();
#if MEASURE_TIME == 1
        uint32_t comp = pi_time_get_us();
        printf("Kernel Time:%d  Compare Time:%d Iteration:%d\n", end - start, comp - end, its);
#endif
    }

    // Free the memory
    AT_L2_FREE(0, M1fp32, W_M1 * H_M1 * sizeof(float));
    AT_L2_FREE(0, M2fp32, W_M2 * H_M2 * sizeof(float));
    AT_L2_FREE(0, Outfp32, W_Out * H_Out * sizeof(float));
    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);
//#else
//    cluster_main();
//#endif

    printf("Test Passed\n");
    pmsis_exit(0);
}

int main(int argc, char *argv[]) {
//    printf("\n\n\t *** MatMult ***\n\n");
    run_MatMult();
    return 0;
}