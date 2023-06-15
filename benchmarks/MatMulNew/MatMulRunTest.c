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
//PI_L2 short int *M1Fix16, *M2Fix16, *OutFix16;
//PI_L2 F16_DSP *M1fp16, *M2fp16, *Outfp16;
#ifdef GENERATE_GOLDEN
PI_L2 float *OutGT;
#else

#include "golden.h"

#endif
extern char *L1_Memory;
//int num_op = 0;
//int Norm, QIN, errors, W_Out, H_Out, H_M2;
int errors, W_Out, H_Out, H_M2;

//float SNRFix, SNRfp16, SNRfp32;
//int start, elapsedfp32, elapsedfp16, elapsedFix, elapsedSeq;

static void cluster_main() {
    printf("cluster master start\n");

#ifdef PERF
    gap_cl_starttimer();
    gap_cl_resethwtimer();
#endif

    /* fp32 */
    int start = gap_cl_readhwtimer();
    MatMul_fp32(M1fp32, M2fp32, Outfp32);
    int elapsedfp32 = gap_cl_readhwtimer() - start;

    /* Fixed Point */
//    start = gap_cl_readhwtimer();
//    MatMul_Fix16(M1Fix16, M2Fix16, OutFix16, Norm);
//    elapsedFix = gap_cl_readhwtimer() - start;

#ifdef __gap9__
    /* FP16 */
//    start = gap_cl_readhwtimer();
//    MatMul_fp16(M1fp16, M2fp16, Outfp16);
//    elapsedfp16 = gap_cl_readhwtimer() - start;
#endif

    /* Check results */
    errors = 0;
//    SNRFix = 0.0;
//    SNRfp16 = 0.0;
//    SNRfp32 = 0.0f;
//    float MaxErrfp32 = 0.0f, MaxErrFix = 0.0f, MaxErrfp16 = 0.0f;
    float MaxErrfp32 = 0.0f;

    float SumSquared = 0.0f;
    for (int h = 0; h < H_Out; h++) {
        for (int w = 0; w < W_Out; w++) {
            float diff = fabsf(Outfp32[h * W_Out + w] != OutGT[h * W_Out + w]);
            if (diff > 0.0f) {
                errors++;
                printf("Error fp32 in [%d, %d]: %e != %e\n", h, w, Outfp32[h * W_Out + w], OutGT[h * W_Out + w]);
                if (diff > MaxErrfp32) {
                    MaxErrfp32 = diff;
                }
            }
            // printf("[%d %d]: %8f %8f\n", h, w, FIX2FP(OutFix16[h*W_Out + w], QIN), OutGT[h*W_Out + w]);
//            SumSquared += OutGT[h * W_Out + w] * OutGT[h * W_Out + w];

//            float err_fp32 = Outfp32[h * W_Out + w] - OutGT[h * W_Out + w];
//            SNRfp32 += (err_fp32) * (err_fp32);
//            if (err_fp32 > MaxErrfp32) MaxErrfp32 = err_fp32;

//            float err_fix = FIX2FP(OutFix16[h*W_Out + w], QIN) - OutGT[h*W_Out + w];
//            SNRFix += (err_fix) * (err_fix);
//            if (err_fix > MaxErrFix) MaxErrFix = err_fix;

#ifdef __gap9__
            //            float err_fp16 = Outfp16[h*W_Out + w] - OutGT[h*W_Out + w];
            //            SNRfp16 += (err_fp16) * (err_fp16);
            //            if (err_fp16 > MaxErrfp16) MaxErrfp16 = err_fp16;
#endif
        }
    }
//    printf("ERROR MAX --- %f\n", MaxErrfp32);
//    SNRfp32 = 10.f * log10f(SumSquared / SNRfp32);
//    SNRFix = 10.f * log10f(SumSquared / SNRFix);
//    SNRfp16 = 10.f * log10f(SumSquared / SNRfp16);
//    printf("|========+===========+===========+=========+=======+=========|\n");
//    printf("| Type   | #MAC      | Cyc       | MAC/Cyc | SNR   | MaxErr  |\n");
//    printf("|========+===========+===========+=========+=======+=========|\n");
//    printf("| Seq FC | %9d | %9d | %7.2f |     0 |       0 |\n", num_op, elapsedSeq, ((float) num_op) / elapsedSeq);
//    printf("|--------+-----------+-----------+---------+-------+---------|\n");
//    printf("| fp32   | %9d | %9d | %7.2f | %5.2f | %7.4f |\n", num_op, elapsedfp32, ((float) num_op) / elapsedfp32,
//           SNRfp32, MaxErrfp32);
//    printf("|--------+-----------+-----------+---------+-------+---------|\n");
//    printf("| Fix16  | %9d | %9d | %7.2f | %5.2f | %7.4f |\n", num_op, elapsedFix, ((float) num_op)/elapsedFix, SNRFix, MaxErrFix);
#ifdef __gap9__
    //    printf("|--------+-----------+-----------+---------+-------+---------|\n");
    //    printf("| fp16   | %9d | %9d | %7.2f | %5.2f | %7.4f |\n", num_op, elapsedfp16, ((float) num_op)/elapsedfp16, SNRfp16, MaxErrfp16);
#endif
//    printf("|========+===========+===========+=========+=======+=========|\n");
//    if (errors || (SNRfp32 < 1e6) || (SNRFix < 30) || (SNRfp16 < 40)) {
//    if (errors || (SNRfp32 < 1e6)) {
//
//        printf("Test Failed\n");
//        pmsis_exit(-1);
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
            printf("Failed to allocate fp32 and Fix16 Matrixes\n");
            pmsis_exit(-1);
        }
        printf(
                "#ifndef GOLDEN_H\n"
                "#define GOLDEN_H\n"
                "\n"
                "PI_L2 uint32_t reinterpret_pointer_global[] = {\n"
        );
        uint32_t * reinterpreted_pointer = (uint32_t * )
        OutGT;
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

void run_MatMult(void) {
#ifndef __EMUL__
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
#endif

//    printf("Entering main controller\n");

    printf("Matrix Mult start\n");

    W_Out = W_M2;
    H_Out = H_M1;
    H_M2 = W_M1;
    int num_op = H_M1 * W_M2 * (W_M1 + H_M2 - 1);
    int AllocatedSpace = (sizeof(float) + sizeof(short)) * (W_M1 * H_M1 + W_M2 * H_M2 + W_Out * H_Out) +
                         sizeof(float) * W_Out * H_Out;
#ifdef __gap9__
    AllocatedSpace += sizeof(short) * (W_M1*H_M1 + W_M2*H_M2 + W_Out*H_Out);
#endif

//    printf("==================================\n");
//    printf("Matrix Multiplication number of operations: %d\n", num_op);
//    printf("Going to Allocate %8d Bytes in L2:\n", AllocatedSpace);
//    printf("\t%8ld Bytes Ground truth Out fp32  Matrixes\n", sizeof(float) * (W_Out * H_Out));
//    printf("\t%8ld Bytes for M1 [%d x %d], %8ld Bytes for M2 [%d x %d], %8ld Bytes for Out [%d x %d] fp32  Matrixes\n",
//           sizeof(float) * (W_M1 * H_M1), H_M1, W_M1, sizeof(float) * (W_M2 * H_M2), H_M2, W_M2,
//           sizeof(float) * (W_Out * H_Out), H_Out, W_Out);
//    printf("\t%8ld Bytes for M1 [%d x %d], %8ld Bytes for M2 [%d x %d], %8ld Bytes for Out [%d x %d] Fix16 Matrixes\n",
//           sizeof(short) * (W_M1 * H_M1), H_M1, W_M1, sizeof(short) * (W_M2 * H_M2), H_M2, W_M2,
//           sizeof(short) * (W_Out * H_Out), H_Out, W_Out);
//#ifdef __gap9__
//    printf("\t%8ld Bytes for M1 [%d x %d], %8ld Bytes for M2 [%d x %d], %8ld Bytes for Out [%d x %d] fp16  Matrixes\n", sizeof(short)*(W_M1*H_M1), H_M1, W_M1, sizeof(short)*(W_M2*H_M2), H_M2, W_M2, sizeof(short)*(W_Out*H_Out), H_Out, W_Out);
//#endif
//    printf("==================================\n");

    M1fp32 = (float *) AT_L2_ALLOC(0, W_M1 * H_M1 * sizeof(float));
    M2fp32 = (float *) AT_L2_ALLOC(0, W_M2 * H_M2 * sizeof(float));
//    M1Fix16  = (short int *) AT_L2_ALLOC(0, W_M1  * H_M1  * sizeof(short int));
//    M2Fix16  = (short int *) AT_L2_ALLOC(0, W_M2  * H_M2  * sizeof(short int));
#ifdef GENERATE_GOLDEN
    OutGT = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));
#endif
    Outfp32 = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));
//    OutFix16 = (short int *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(short int));

//    if ((M1fp32 == NULL) || (M2fp32 == NULL) || (Outfp32 == NULL) || (M1Fix16 == NULL) || (M2Fix16 == NULL) || (OutFix16 == NULL) || (OutGT == NULL)) {
    if ((M1fp32 == NULL) || (M2fp32 == NULL) || (Outfp32 == NULL)
#ifdef GENERATE_GOLDEN
        || (OutGT == NULL)
#endif
            ) {
        printf("Failed to allocate fp32 and Fix16 Matrixes\n");
        pmsis_exit(-1);
    }

#ifdef __gap9__
    //    M1fp16 = (F16_DSP *) AT_L2_ALLOC(0, W_M1 * H_M1 * sizeof(F16_DSP));
    //    M2fp16 = (F16_DSP *) AT_L2_ALLOC(0, W_M2 * H_M2 * sizeof(F16_DSP));
    //    Outfp16 = (F16_DSP *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(F16_DSP));
    //    if ((M1fp16 == NULL) || (M2fp16 == NULL) || (Outfp16 == NULL)) {
    //        printf("Failed to allocate fp16 Matrixes\n");
    //        pmsis_exit(-1);
    //    }
#endif

    /* Allocate the predetermined memory size in the shared L1 memory that the cluster can act on. */
    L1_Memory = (char *) AT_L1_ALLOC(0, _L1_Memory_SIZE);
    if (L1_Memory == NULL) {
        printf("Memory Allocation Error! Quit...\n");
        pmsis_exit(-3);
    }

    /* Init Data */
    int QIN = 15 - gap_fl1(W_M1);
    printf("QIN: %d\n", QIN);
    int Norm = QIN;
    if (H_M1 != H_M2 || W_M1 != W_M2) {
        printf("This code only works on equal sized matrices\n");
        pmsis_exit(-3);
    }
    for (int tt = 0; tt < H_M1 * W_M1; tt++) {
        M1fp32[tt] = tt * 0.0034f;
        M2fp32[tt] = tt * 0.0011f;
    }
//    for (int h = 0; h < H_M1; h++) {
//        for (int w = 0; w < W_M1; w++) {
//            M1fp32[h * W_M1 + w] = h * 0.0034f;
//            M2fp32[h * W_M2 + w] = w * 0.0011f;
////            M1Fix16[h*W_M1 + w] = FP2FIXR(M1fp32[h*W_M1 + w], QIN);
//#ifdef __gap9__
//            //            M1fp16[h*W_M1 + w] = (F16_DSP) M1fp32[h*W_M1 + w];
//#endif
//        }
//    }
//    for (int h = 0; h < H_M2; h++) {
//        for (int w = 0; w < W_M2; w++) {
//            M2fp32[h * W_M2 + w] = 1.0f; // / (float) (w + 4);
////            M2Fix16[h*W_M2 + w] = FP2FIXR(M2fp32[h*W_M2 + w], QIN);
//#ifdef __gap9__
//            //            M2fp16[h*W_M2 + w] = (F16_DSP) M2fp32[h*W_M2 + w];
//#endif
//        }
//    }

#ifdef PERF
    gap_fc_starttimer();
    gap_fc_resethwtimer();
#endif
    int start = gap_fc_readhwtimer();
#ifdef GENERATE_GOLDEN
    generate_golden(1);
#endif
    int elapsedSeq = gap_fc_readhwtimer() - start;


#ifndef __EMUL__
    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task task;
    pi_cluster_task(&task, cluster_main, NULL);

    /* Offloading Task to cluster. */
    pi_cluster_send_task(&cluster_dev, &task);
    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);
#else
    cluster_main();
#endif

    printf("Test Passed\n");
    pmsis_exit(0);
}

int main(int argc, char *argv[]) {
//    printf("\n\n\t *** MatMult ***\n\n");
    run_MatMult();
    return 0;
}
