
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


/* Autotiler includes. */
#include "efficientnet_lite.h"
#include "efficientnet_liteKernels.h"
#include "gaplib/ImgIO.h"

#ifdef __EMUL__
#define pmsis_exit(n) exit(n)
#endif

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s
#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif

AT_HYPERFLASH_FS_EXT_ADDR_TYPE efficientnet_lite_L3_Flash = 0;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE efficientnet_lite_L3_PrivilegedFlash = 0;

char *ImageName;
/* Inputs */
// L2_MEM signed char Input_1[150528];
/* Outputs */
L2_MEM short int Output_1[1000];
int outclass = 0, MaxPrediction = 0;

static void cluster()
{
    #ifdef PERF
    printf("Start timer\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

    GPIO_HIGH();
    efficientnet_liteCNN(Output_1);
    GPIO_LOW();
    printf("Runner completed\n");

    //Check Results
    outclass = 0, MaxPrediction = 0;
    for(int i=0; i<1000; i++){
        if (Output_1[i] > MaxPrediction){
            outclass = i;
            MaxPrediction = Output_1[i];
        }
    }
    printf("Predicted class:\t%d\n", outclass);
    printf("With confidence:\t%d\n", MaxPrediction);
}

int test_efficientnet_lite(void)
{
#ifndef __EMUL__
    OPEN_GPIO_MEAS();
    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;
    cl_conf.cc_stack_size = STACK_SIZE;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    pi_cluster_open(&cluster_dev);
    pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE*1000*1000);
    printf("Set FC Frequency = %d MHz, CL Frequency = %d MHz, PERIIPH Frequency = %d MHz\n",
            pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));
    #ifdef VOLTAGE
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLTAGE);
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLTAGE);
    #endif
    printf("Voltage: %dmV\n", pi_pmu_voltage_get(PI_PMU_VOLTAGE_DOMAIN_CHIP));
#endif
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    printf("Constructor\n");
    int ConstructorErr = efficientnet_liteCNN_Construct();
    if (ConstructorErr)
    {
        printf("Graph constructor exited with error: %d\n(check the generated file efficientnet_liteKernels.c to see which memory have failed to be allocated)\n", ConstructorErr);
        pmsis_exit(-6);
    }

    // Reading Image from Bridge
    img_io_out_t type = IMGIO_OUTPUT_CHAR;
    if (ReadImageFromFile(ImageName, IMAGE_SIZE, IMAGE_SIZE, 3, Input_1, IMAGE_SIZE*IMAGE_SIZE*3*sizeof(char), type, TRANSPOSE_2CHW)) {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-1);
    }
    #ifndef MODEL_NE16
    printf("Subracting -128 to the input image to make it signed...\n");
    for (int i=0; i<IMAGE_SIZE*IMAGE_SIZE*3; i++) Input_1[i] -= 128;
    #endif

    printf("Model:\t%s\n\n", __XSTR(MODEL_NAME));
#ifndef __EMUL__
    struct pi_cluster_task task;
    pi_cluster_task(&task, cluster, NULL);
    pi_cluster_task_stacks(&task, NULL, SLAVE_STACK_SIZE);
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    cluster();
#endif

    efficientnet_liteCNN_Destruct();

#ifdef PERF
    {
      unsigned int TotalCycles = 0, TotalOper = 0;
      printf("\n");
      for (unsigned int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
      }
      for (unsigned int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        printf("%45s: Cycles: %10u, Cyc%%: %5.1f%%, Operations: %10u, Op%%: %5.1f%%, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], 100*((float) (AT_GraphPerf[i]) / TotalCycles), AT_GraphOperInfosNames[i], 100*((float) (AT_GraphOperInfosNames[i]) / TotalOper), ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
      }
      printf("\n");
      printf("%45s: Cycles: %10u, Cyc%%: 100.0%%, Operations: %10u, Op%%: 100.0%%, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
      printf("\n");
    }
#endif

    if(outclass==42 && MaxPrediction>15000) printf("Test successful!\n");
    else {
        printf("Wrong results!\n");
        pmsis_exit(-1);
    }

    printf("Ended\n");
    pmsis_exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("\n\n\t *** NNTOOL efficientnet_lite Example ***\n\n");
    #ifdef __EMUL__
    if (argc < 2)
    {
      PRINTF("Usage: efficientnet_lite [image_file]\n");
      exit(-1);
    }
    ImageName = argv[1];
    test_efficientnet_lite();
    #else
    ImageName = __XSTR(AT_IMAGE);
    return pmsis_kickoff((void *) test_efficientnet_lite);
    #endif
    return 0;
}
