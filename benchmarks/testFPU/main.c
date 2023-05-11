//
// Created by fernando on 09/05/23.
//
//#include "testFPU.h"
//#include "math_fns.h"

#include "bsp/pulp.h"

/* PMSIS includes */
#include "pmsis.h"
#include "../perf/performance_counter.h"

// This testbench checks the basic functionality of:
//fadd.s, fsub.s, fmul.s, fdiv.s, fsqrt.s, fmadd.s, fnmadd.s, fmsub.s, fnmsub.s, fLog2, fAtan2
typedef enum {
    FADDS = 0,
    FSUBS,
    FMULS,
    FDIVS,
    FSQRTS,
    FMADDS,
    FNMADDS,
    FMSUBS,
    FNMSUBS,
} MicroTypes;

#ifndef TEST_MICRO_ID
#define TEST_MICRO_ID FADDS
#warning "Micro not defined, using FADDS as default"
#endif
#define MICRO_ITERATIONS 1 << 16

const char *micro_names[] = {
        "FADDS",
        "FSUBS",
        "FMULS",
        "FDIVS",
        "FSQRTS",
        "FMADDS",
        "FNMADDS",
        "FMSUBS",
        "FNMSUBS",
};

const float micro_golds[] = {
        97.1318F,   // FADDS
        171.1867F,  // FSUBS
        31.8328F,   // FMULS
        276.9230F,  // FDIVS
        46.1714F,   // FSQRTS
        97.1318F,   // FMADDS
        823.4578F,  // FMSUBS
        694.8286F,
        317.0995F
};

const float micro_ins[] = {
        655.4779F,   // FADDS
        97.1318F,  // FSUBS
        2232.8328F,   // FMULS
        787.9230F,  // FDIVS
        345.1714F,   // FSQRTS
        98.1318F,   // FMADDS
        454.2092F, //  FNMADDS
        88.4578F,  // FMSUBS
        555.8286F, //FNMSUBS
        317.0995F
};

#define MAX_NUMBER_CORES 16
typedef struct {
    MicroTypes micro_t;
    float micro_output[MAX_NUMBER_CORES]; // not more than 16 cores
} MicroBenchDescriptor;

inline float fadd(float a, float b) {
    float c = 0;
    asm volatile ("fadd.s %[c], %[a], %[b]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b));
    return c;
}

inline float fsub(float a, float b) {
    float c = 0;
    asm volatile ("fsub.s %[c], %[a], %[b]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b));
    return c;
}

inline float fmul(float a, float b) {
    float c = 0;
    asm volatile ("fmul.s %[c], %[a], %[b]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b));
    return c;
}

inline float fdiv(float a, float b) {
    float c = 0;
    asm volatile ("fdiv.s %[c], %[a], %[b]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b));
    return c;
}

inline float fsqrt(float a) {
    float c = 0;
    asm volatile ("fsqrt.s %[c], %[a]\n" : [c] "=f"(c) : [a] "f"(a));
    return c;
}

inline float fmadds(float a, float b, float d) {
    float c = 0;
    asm volatile ("fmadd.s %[c], %[a], %[b], %[d]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b), [d] "f"(d));
    return c;
}

inline float fnmadds(float a, float b, float d) {
    float c = 0;
    asm volatile ("fnmadd.s %[c], %[a], %[b], %[d]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b), [d] "f"(d));
    return c;
}

inline float fmsubs(float a, float b, float d) {
    float c = 0;
    asm volatile ("fmsub.s %[c], %[a], %[b], %[d]\n"  : [c] "=f"(c) : [a] "f"(a), [b] "f"(b), [d] "f"(d));
    return c;
}

inline float fnmsubs(float a, float b, float d) {
    float c = 0;
    asm volatile("fnmsub.s %[c], %[a], %[b], %[d]\n" : [c] "=f"(c) : [a] "f"(a), [b] "f"(b), [d] "f"(d));
    return c;
}


void execute_micro(MicroTypes micro_t, uint32_t core_id, float *micro_out) {
    float acc = micro_golds[micro_t];
    float a = micro_ins[micro_t];
    float b = -a;

    switch (micro_t) {
        case FADDS: {
            //-----------------------------------------------------------------
            // Check fadd.s (floating point addition)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fadd(acc, b);
                acc = fadd(acc, a);
            }
        }
            break;
        case FSUBS: {
            //-----------------------------------------------------------------
            // Check fsub.s (floating point subtraction)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fsub(acc, a);
                acc = fsub(acc, b);
            }
        }
            break;
        case FMULS: {
            //-----------------------------------------------------------------
            // Check fmul.s (floating point multiplication)
            //-----------------------------------------------------------------
            float mul_inv = 1 / a;
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fmul(acc, a);
                acc = fmul(acc, mul_inv);
            }
        }
            break;
        case FDIVS: {
            float div_a_inv = 1 / a;
            //-----------------------------------------------------------------
            // Check fdiv.s (floating point division)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fdiv(acc, div_a_inv);
                acc = fdiv(acc, a);
            }
        }
            break;
        case FSQRTS: {
            //-----------------------------------------------------------------
            // Check fsqrt.s (floating point square root)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fsqrt(acc);
                acc *= acc;
            }
        }
            break;
        case FMADDS: {
            //-----------------------------------------------------------------
            // Check fmadd.s (floating point multiply-add)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fmadds(a, b, acc);
                acc = fmadds(b, a, acc);
            }
        }
            break;
        case FNMADDS: {
            //-----------------------------------------------------------------
            // Check fnmadd.s (floating point negative multiply-add)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fnmadds(a, a, acc);
                acc = fnmadds(b, a, acc);
            }
        }
        case FMSUBS: {
            //-----------------------------------------------------------------
            // Check fmsub.s (floating point multiply-subtract)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fmsubs(a, a, acc);
                acc = fmsubs(b, a, acc);
            }
        }
            break;
        case FNMSUBS: {
            //-----------------------------------------------------------------
            // Check fnmsub.s (floating point negative multiply-subtract)
            //-----------------------------------------------------------------
            for (int i = 0; i < MICRO_ITERATIONS; i++) {
                acc = fnmsubs(a, a, acc);
                acc = fnmsubs(b, a, acc);
            }
        }
            break;
            /**
             * Why not other functions
             * fExp2, fLog2,  fsin, is approximated
             */
    }
//    printf("PASSOU AQUI %d core id %d\n", micro_t, core_id);
//    printf("%d - Result fadd: %f core: %d\n", MICRO_ITERATIONS, acc, core_id);
    *micro_out = acc;
}


/* Task executed by cluster cores. */
void cluster_call_micro(void *arg) {
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
//    printf("[%d %d] Hellosfsdf World!\n", cluster_id, core_id);
    MicroBenchDescriptor *micro_details = (MicroBenchDescriptor *) arg;
    float *micro_output = &micro_details->micro_output[core_id];
    execute_micro(micro_details->micro_t, core_id, micro_output);
}

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg) {
//    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_call_micro, arg);
//    printf("Cluster master core exit\n");
}

/* Program Entry. */
int main(void) {
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLT_SET);
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLT_SET);
    pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC * 1000 * 1000);
    pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL * 1000 * 1000);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE * 1000 * 1000);

    printf("micro VOLTAGE:%d FREQ_FC:%d FREQ_CL:%d\n",
           pi_pmu_voltage_get(PI_PMU_VOLTAGE_DOMAIN_CHIP),
           pi_freq_get(PI_FREQ_DOMAIN_FC),
           pi_freq_get(PI_FREQ_DOMAIN_CL));
//    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
//    printf("Entering main controller\n");
    MicroBenchDescriptor test_definitions = {TEST_MICRO_ID, {0}};
    float micro_gold = micro_golds[TEST_MICRO_ID];

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
//    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;                /* Set cluster ID. */
    // Enable the special icache for the master core
    cl_conf.icache_conf = PI_CLUSTER_MASTER_CORE_ICACHE_ENABLE |
                          // Enable the prefetch for all the cores, it's a 9bits mask (from bit 2 to bit 10), each bit correspond to 1 core
                          PI_CLUSTER_ICACHE_PREFETCH_ENABLE |
                          // Enable the icache for all the cores
                          PI_CLUSTER_ICACHE_ENABLE;

    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev)) {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task;
    int errors = 0, its;
    for (its = 0; its < SETUP_RADIATION_ITERATIONS && errors == 0; its++) {
        begin_perf_iteration_i();
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &test_definitions));
        end_perf_iteration_i();
//        if (its == 2) test_definitions.micro_output[6] = 44;
        // Verify the output
        for (int i = 0; i < gap_ncore(); i++) {
            float output = test_definitions.micro_output[i];
            if (output != micro_gold) {
                printf("Error:[%d]=%f != %f\n", i, output, micro_gold);
                errors++;
            }
        }

    }
    if (errors){
        print_iteration_perf(its);
    }

    end_counters();
    printf("Test finished\n");
    pi_cluster_close(&cluster_dev);
    return errors;
}