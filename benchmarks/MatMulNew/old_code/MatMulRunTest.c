/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "pmsis.h"
#include "../perf/performance_counter.h"

//#ifndef __EMUL__
//
//#include "pmsis.h"
//
//#else
//#include <stdlib.h>
//#define pmsis_exit(a)   exit(a)
//#endif

#include "Gap.h"
#include "MatMulKernels.h"
#include "math.h"

#define STACK_SIZE ( 1024 * 2 )
#define CID        ( 0 )

PI_L2 float *M1fp32, *M2fp32, *Outfp32;
//PI_L2 short int *M1Fix16, *M2Fix16, *OutFix16;
//PI_L2 F16_DSP *M1fp16, *M2fp16, *Outfp16;
//PI_L2 float *OutGT;
#include "golden.h"

extern char *L1_Memory;
//int num_op = 0, Norm, QIN;
int W_Out, H_Out, H_M2;
//float SNRFix, SNRfp16, SNRfp32;
//int start, elapsedfp32, elapsedfp16, elapsedFix, elapsedSeq;

static void cluster_main() {
//    printf("cluster master start\n");

//#ifdef PERF
//    gap_cl_starttimer();
//    gap_cl_resethwtimer();
//#endif

    /* fp32 */
//    start = gap_cl_readhwtimer();
    MatMul_fp32(M1fp32, M2fp32, Outfp32);
//    elapsedfp32 = gap_cl_readhwtimer() - start;

    /* Fixed Point */
//    start = gap_cl_readhwtimer();
//    MatMul_Fix16(M1Fix16, M2Fix16, OutFix16, Norm);
//    elapsedFix = gap_cl_readhwtimer() - start;

//#ifdef __gap9__
//    /* FP16 */
//    start = gap_cl_readhwtimer();
//    MatMul_fp16(M1fp16, M2fp16, Outfp16);
//    elapsedfp16 = gap_cl_readhwtimer() - start;
//#endif

    /* Check results */
//    errors = 0;
//    SNRFix = 0.0f;
//    SNRfp16 = 0.0f;
//    SNRfp32 = 0.0f;
//    float MaxErrfp32 = 0.0f, MaxErrFix = 0.0f, MaxErrfp16 = 0.0f;
//    float SumSquared = 0.0f;
//    for (int h = 0; h < H_Out; h++) {
//        for (int w = 0; w < W_Out; w++) {
//            if (Outfp32[h * W_Out + w] != OutGT[h * W_Out + w]) {
//                errors++;
//                printf("Error fp32 in [%d, %d]: %e != %e\n", h, w, Outfp32[h * W_Out + w], OutGT[h * W_Out + w]);
//            }
    // printf("[%d %d]: %8f %8f\n", h, w, FIX2FP(OutFix16[h*W_Out + w], QIN), OutGT[h*W_Out + w]);
//            SumSquared += OutGT[h * W_Out + w] * OutGT[h * W_Out + w];
//
//            float err_fp32 = Outfp32[h * W_Out + w] - OutGT[h * W_Out + w];
//            SNRfp32 += (err_fp32) * (err_fp32);
//            if (err_fp32 > MaxErrfp32) MaxErrfp32 = err_fp32;

//            float err_fix = FIX2FP(OutFix16[h * W_Out + w], QIN) - OutGT[h * W_Out + w];
//            SNRFix += (err_fix) * (err_fix);
//            if (err_fix > MaxErrFix) MaxErrFix = err_fix;
//
//#ifdef __gap9__
//            float err_fp16 = Outfp16[h*W_Out + w] - OutGT[h*W_Out + w];
//            SNRfp16 += (err_fp16) * (err_fp16);
//            if (err_fp16 > MaxErrfp16) MaxErrfp16 = err_fp16;
//#endif
//        }
//    }
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
//    printf("| Fix16  | %9d | %9d | %7.2f | %5.2f | %7.4f |\n", num_op, elapsedFix, ((float) num_op) / elapsedFix,
//           SNRFix, MaxErrFix);
//#ifdef __gap9__
//    printf("|--------+-----------+-----------+---------+-------+---------|\n");
//    printf("| fp16   | %9d | %9d | %7.2f | %5.2f | %7.4f |\n", num_op, elapsedfp16, ((float) num_op)/elapsedfp16, SNRfp16, MaxErrfp16);
//#endif
//    printf("|========+===========+===========+=========+=======+=========|\n");
//    if (errors) { //} || (SNRfp32 < 1e6) || (SNRFix < 30) || (SNRfp16 < 40)) {
//        printf("Test Failed\n");
//        pmsis_exit(-1);
//    }
}

ALWAYS_INLINE__
int compare_output() {
    int errors = 0;
#ifndef GENERATE_GOLDEN
//    float max = -99999.f;
    for (int h = 0; h < H_Out; h++) {
        for (int w = 0; w < W_Out; w++) {
            if (fabs(Outfp32[h * W_Out + w] - OutGT[h * W_Out + w]) > 5.859375e-03) {
                errors++;
                printf("Error[%d, %d]: %e != %e\n", h, w, Outfp32[h * W_Out + w], OutGT[h * W_Out + w]);
            }
//            max = fmax(fabs(Outfp32[h * W_Out + w] - OutGT[h * W_Out + w]), max);

            // Set to zero for the next iteration
            Outfp32[h * W_Out + w] = 0.0f;
        }
    }
//    printf("MAX %e\n", max);
#else
    printf("\n{");
    for (int i = 0; i < H_Out * W_Out; i++) {
        printf("%e, ", Outfp32[i]);
    }
    printf("};\n");

#endif
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

//    printf("Entering main controller\n");
//
//    printf("Matrix Mult start\n");
    start_counters();
    printf("MXM Float VOLTAGE:%d FREQ_FC:%d FREQ_CL:%d -- %d %d %d\n",
           pi_pmu_voltage_get(PI_PMU_VOLTAGE_DOMAIN_CHIP),
           pi_freq_get(PI_FREQ_DOMAIN_FC),
           pi_freq_get(PI_FREQ_DOMAIN_CL),
           W_M1, H_M1, W_M2);
    // -------------------
    W_Out = W_M2;
    H_Out = H_M1;
    H_M2 = W_M1;
//    num_op = H_M1 * W_M2 * (W_M1 + H_M2 - 1);
    int AllocatedSpace = (sizeof(float) + sizeof(short)) * (W_M1 * H_M1 + W_M2 * H_M2 + W_Out * H_Out) +
                         sizeof(float) * W_Out * H_Out;
//#ifdef __gap9__
//    AllocatedSpace += sizeof(short) * (W_M1*H_M1 + W_M2*H_M2 + W_Out*H_Out);
//#endif

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
//    M1Fix16 = (short int *) AT_L2_ALLOC(0, W_M1 * H_M1 * sizeof(short int));
//    M2Fix16 = (short int *) AT_L2_ALLOC(0, W_M2 * H_M2 * sizeof(short int));
//    OutGT = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));
    Outfp32 = (float *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(float));
//    OutFix16 = (short int *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(short int));

    if ((M1fp32 == NULL) || (M2fp32 == NULL) || (Outfp32 == NULL)) {
//    || (M1Fix16 == NULL) || (M2Fix16 == NULL) ||  (OutFix16 == NULL) || (OutGT == NULL)) {
        printf("Failed to allocate fp32 and Fix16 Matrixes\n");
        pmsis_exit(-1);
    }

//#ifdef __gap9__
//    M1fp16 = (F16_DSP *) AT_L2_ALLOC(0, W_M1 * H_M1 * sizeof(F16_DSP));
//    M2fp16 = (F16_DSP *) AT_L2_ALLOC(0, W_M2 * H_M2 * sizeof(F16_DSP));
//    Outfp16 = (F16_DSP *) AT_L2_ALLOC(0, W_Out * H_Out * sizeof(F16_DSP));
//    if ((M1fp16 == NULL) || (M2fp16 == NULL) || (Outfp16 == NULL)) {
//        printf("Failed to allocate fp16 Matrixes\n");
//        pmsis_exit(-1);
//    }
//#endif

    /* Allocate the predetermined memory size in the shared L1 memory that the cluster can act on. */
    L1_Memory = (char *) AT_L1_ALLOC(0, _L1_Memory_SIZE);
    if (L1_Memory == NULL) {
        printf("Memory Allocation Error! Quit...\n");
        pmsis_exit(-3);
    }

    /* Init Data */
//    QIN = 15 - gap_fl1(W_M1);
//    printf("QIN: %d\n", QIN);
//    Norm = QIN;
    float div = 1.0f / (float) (H_M2 * W_M1);
    if (W_M2 != W_M1 || H_M2 != H_M1) {
        printf("Only for square matrices\n");
        pmsis_exit(-1);
    }
    for (int h = 0; h < H_M1 * W_M1; h++) {
//        for (int w = 0; w < W_M1; w++) {
        M1fp32[h] = (float) h;
        M2fp32[h] = (float) h * div;
        Outfp32[h] = 0.0f;
//            M1Fix16[h * W_M1 + w] = FP2FIXR(M1fp32[h * W_M1 + w], QIN);
//#ifdef __gap9__
//            M1fp16[h*W_M1 + w] = (F16_DSP) M1fp32[h*W_M1 + w];
//#endif
//        }
    }
//    for (int h = 0; h < H_M2; h++) {
//        for (int w = 0; w < W_M2; w++) {
//            M2fp32[h * W_M2 + w] = 1.0 / (float) (w + 4);
//            M2Fix16[h * W_M2 + w] = FP2FIXR(M2fp32[h * W_M2 + w], QIN);
//#ifdef __gap9__
//            M2fp16[h*W_M2 + w] = (F16_DSP) M2fp32[h*W_M2 + w];
//#endif
//        }
//    }

//#ifdef PERF
//    gap_fc_starttimer();
//    gap_fc_resethwtimer();
//#endif
//    start = gap_fc_readhwtimer();
//    /* Simple sequential in L2 */
//    for (int h = 0; h < H_M1; h++) {
//        for (int w2 = 0; w2 < W_M2; w2++) {
//            float Acc = 0.0f;
//            for (int w = 0; w < W_M1; w++) {
//                Acc += M1fp32[h * W_M1 + w] * M2fp32[w * W_M2 + w2];
//            }
//            OutGT[h * W_M2 + w2] = Acc;
//        }
//    }
//    elapsedSeq = gap_fc_readhwtimer() - start;


//#ifndef __EMUL__
    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task task;
    pi_cluster_task(&task, cluster_main, NULL);

#ifndef GENERATE_GOLDEN
    const int setup_it = SETUP_RADIATION_ITERATIONS;
#else
    const int setup_it = 1;
#endif

    /* Offloading Task to cluster. */
    int errors = 0, its;
    for (its = 0; (its < setup_it) & (errors == 0); its++) {
        begin_perf_iteration_i();
        pi_cluster_send_task(&cluster_dev, &task);
        end_perf_iteration_i();
//        if (its == 34) Outfp32[34] = 333333;
        errors = compare_output();
    }
    if (errors) {
        print_iteration_perf(its);
    }
//    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);
//#else
//    cluster_main();
//#endif
    end_counters();
//    printf("Test Passed\n");
    pmsis_exit(0);
}

int main(int argc, char *argv[]) {
//    printf("\n\n\t *** MatMult ***\n\n");
    run_MatMult();
    return 0;
}
