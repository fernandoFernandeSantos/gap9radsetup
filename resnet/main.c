
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


/* Autotiler includes. */
#include "ResNet.h"
#include "ResNetKernels.h"
#include "gaplib/fs_switch.h"
#include "gaplib/ImgIO.h"
#include "measurments_utils.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s 
#ifdef __EMUL__
#define pmsis_exit(n) exit(n)
#endif

#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif
#define NUM_CLASSES     1000

AT_HYPERFLASH_EXT_ADDR_TYPE ResNet_L3_Flash = 0;
AT_HYPERFLASH_EXT_ADDR_TYPE ResNet_L3_PrivilegedFlash = 0;

/* Outputs */
L2_MEM unsigned char Output_1[NUM_CLASSES];

/* Copy inputs function */
void copy_inputs() {
    char *ImageName = __XSTR(AT_IMAGE);
    printf("Reading image from %s\n",ImageName);

    // //Reading Image from Bridge
    img_io_out_t type = IMGIO_OUTPUT_CHAR;
    if (ReadImageFromFile(ImageName, 224, 224, 3, Input_1, 224*224*3*sizeof(char), type, 0)) {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-1);
    }
    for (int i=0; i<224*224*3; i++) Input_1[i] -= 128;
    printf("Finished reading image %s\n", ImageName);
}


static void cluster()
{
    #ifdef PERF
    printf("Start timer\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

  GPIO_HIGH();
    ResNetCNN(Output_1);
  GPIO_LOW();
    printf("Runner completed\n");

}

int test_ResNet(void)
{
    printf("Entering main controller\n");
    /* ----------------> 
     * Put here Your input settings
     * <---------------
     */

#ifndef __EMUL__
    OPEN_GPIO_MEAS();
    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;

    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;
    cl_conf.cc_stack_size = STACK_SIZE;

    cl_conf.id = 0; /* Set cluster ID. */
                    // Enable the special icache for the master core
    cl_conf.icache_conf = PI_CLUSTER_MASTER_CORE_ICACHE_ENABLE |
                    // Enable the prefetch for all the cores, it's a 9bits mask (from bit 2 to bit 10), each bit correspond to 1 core
                    PI_CLUSTER_ICACHE_PREFETCH_ENABLE |
                    // Enable the icache for all the cores
                    PI_CLUSTER_ICACHE_ENABLE;

    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }

    /* Frequency Settings: defined in the Makefile */
    int cur_fc_freq = pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
    int cur_cl_freq = pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
    int cur_pe_freq = pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE*1000*1000);
    if (cur_fc_freq == -1 || cur_cl_freq == -1 || cur_pe_freq == -1)
    {
        printf("Error changing frequency !\nTest failed...\n");
        pmsis_exit(-4);
    }
	printf("FC Frequency as %d Hz, CL Frequency = %d Hz, PERIIPH Frequency = %d Hz\n", 
            pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));

#endif
    

    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    printf("Constructor\n");
    int ConstructorErr = ResNetCNN_Construct();
    if (ConstructorErr)
    {
        printf("Graph constructor exited with error: %d\n(check the generated file ResNetKernels.c to see which memory have failed to be allocated)\n", ConstructorErr);
        pmsis_exit(-6);
    }
    

    copy_inputs();

    printf("Call cluster\n");
#ifndef __EMUL__
    struct pi_cluster_task task;
    pi_cluster_task(&task, (void (*)(void *))cluster, NULL);
    pi_cluster_task_stacks(&task, NULL, SLAVE_STACK_SIZE);

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    cluster();
#endif

    ResNetCNN_Destruct();

    //Check Results
    int outclass = 0, MaxPrediction = 0;
    for(int i=0; i<NUM_CLASSES; i++){
        if (Output_1[i] > MaxPrediction){
            outclass = i;
            MaxPrediction = Output_1[i];
        }
    }
    printf("Predicted class:\t%d\n", outclass);
    printf("With confidence:\t%d\n", MaxPrediction);

#ifdef PERF
    {
      unsigned int TotalCycles = 0, TotalOper = 0;
      printf("\n");
      for (unsigned int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
      }
      for (unsigned int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        printf("%45s: Cycles: %12u, Cyc%%: %5.1f%%, Operations: %12u, Op%%: %5.1f%%, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], 100*((float) (AT_GraphPerf[i]) / TotalCycles), AT_GraphOperInfosNames[i], 100*((float) (AT_GraphOperInfosNames[i]) / TotalOper), ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
      }
      printf("\n");
      printf("%45s: Cycles: %12u, Cyc%%: 100.0%%, Operations: %12u, Op%%: 100.0%%, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
      printf("\n");
    }
#endif

    #ifdef CI
    if(outclass != 1){
        printf("Results Error...\n");
        pmsis_exit(1);
    }
    else
        printf("Correct Results!\n");
    #endif

    printf("Ended\n");
    pmsis_exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("\n\n\t *** NNTOOL ResNet Example ***\n\n");
    #ifdef __EMUL__
    test_ResNet();
    #else
    return pmsis_kickoff((void *) test_ResNet);
    #endif
    return 0;
}
