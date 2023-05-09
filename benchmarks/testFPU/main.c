//
// Created by fernando on 09/05/23.
//
//#include "testFPU.h"
//#include "math_fns.h"

#include "bsp/pulp.h"

/* PMSIS includes */
#include "pmsis.h"

// This testbench checks the basic functionality of:
//
// fadd.s
// fsub.s
// fmul.s
// fdiv.s
// fsqrt.s
// fmadd.s
// fnmadd.s
// fmsub.s
// fnmsub.s
// fLog2
// fExp2
// fSin
// fCos
// fAtan2
typedef enum {
    FADDS = 0,
    FSUBS,
    FMULS,
    FDIVS,
    FSQRTS,
    FMADDS,
    FMSUBS
} MicroTypes;

float micro_golds[] = {
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

typedef struct {
    MicroTypes micro_t;

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


void execute_micro(MicroTypes micro_t, uint32_t core_id) {
    const int micro_iterations = 1 << 22;
    float acc = micro_golds[micro_t];

    switch (micro_t) {
        case FADDS: {
            //-----------------------------------------------------------------
            // Check fadd.s (floating point addition)
            //-----------------------------------------------------------------
            float add_a = 655.4779F;
            float add_inv = -add_a;
            for (int i = 0; i < micro_iterations; i++) {
                acc = fadd(acc, add_inv);
                acc = fadd(acc, add_a);
            }
        }
            break;
        case FSUBS: {
            //-----------------------------------------------------------------
            // Check fsub.s (floating point subtraction)
            //-----------------------------------------------------------------
            float sub_a = 97.1318F;
            float sub_inv = -sub_a;
            for (int i = 0; i < micro_iterations; i++) {
                acc = fsub(acc, sub_a);
                acc = fsub(acc, sub_inv);
            }
        }
            break;
        case FMULS: {
            //-----------------------------------------------------------------
            // Check fmul.s (floating point multiplication)
            //-----------------------------------------------------------------
            float mul_a = 46.1714F;
            float mul_inv = 1 / mul_a;
            for (int i = 0; i < micro_iterations; i++) {
                acc = fmul(acc, mul_a);
                acc = fmul(acc, mul_inv);
            }
        }
            break;
        case FDIVS: {
            float div_a = 694.8286F;
            float div_a_inv = 1 / div_a;
            //-----------------------------------------------------------------
            // Check fdiv.s (floating point division)
            //-----------------------------------------------------------------
            for (int i = 0; i < micro_iterations; i++) {
                acc = fdiv(acc, div_a_inv);
                acc = fdiv(acc, div_a);
            }
        }
            break;
        case FSQRTS: {
            //-----------------------------------------------------------------
            // Check fsqrt.s (floating point square root)
            //-----------------------------------------------------------------
            for (int i = 0; i < micro_iterations; i++) {
                acc = fsqrt(acc);
                acc *= acc;
            }
        }
            break;
        case FMADDS:
        case FMSUBS:
            break;
    }
//    printf("PASSOU AQUI %d core id %d\n", micro_t, core_id);
    printf("%d - Result fadd: %f core: %d\n", micro_iterations, acc, core_id);

}

/**
 *
void check_explog(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    float g_add_act[sizeof(g_in_a) / 4];
    //-----------------------------------------------------------------
    // Check fExp2 (floating point exponentiation 2^x)
    //-----------------------------------------------------------------
    start();
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        float g_exp_in = g_in_a[i] / 100.0F;
        g_add_act[i] = fExp2(g_exp_in);

        check_float(result, "fExp2", g_add_act[i], g_exp_min[i], g_exp_max[i]);
    }

    //-----------------------------------------------------------------
    // Check fExp2 (floating point exponentiation 2^(-x))
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        float g_exp_in = g_in_a[i] / 1000.0F;
        g_add_act[i] = fExp2(-g_exp_in);

        check_float(result, "fExp2", g_add_act[i], g_mexp_min[i], g_mexp_max[i]);
    }

    //-----------------------------------------------------------------
    // Check fLog2 (floating point logarithm (base 2))
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        g_add_act[i] = fLog2(g_in_a[i]);
        check_float(result, "gLog2", g_add_act[i], g_log_min[i], g_log_max[i]);
    }
    stop();
}

void check_trig(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    float g_trig_act[sizeof(g_trig_in) / 4];
    start();
    float pi2 = MATH_0_5PI;
    //-----------------------------------------------------------------
    // Check fSin (floating point sin)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        g_trig_act[i] = fSin(g_trig_in[i]);
        check_float(result, "fSin", g_trig_act[i], g_sin_min[i], g_sin_max[i]);
    }
    //-----------------------------------------------------------------
    // Check fCos (floating point cos)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        g_trig_act[i] = fCos(g_trig_in[i]);
        check_float(result, "fCos", g_trig_act[i], g_cos_min[i], g_cos_max[i]);
    }
    //-----------------------------------------------------------------
    // Check fAtan2 (floating point atan) // atan2(y,x); y=a, x=b
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        float x, y;
        x = g_in_b[i];
        y = g_in_a[i];
        g_trig_act[i] = fAtan2(y, x);
        check_float(result, "fAtan2", g_trig_act[i], g_atan_min[i], g_atan_max[i]);
    }
    stop();
}

void check_fma(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    float g_fma_act[(sizeof(g_fma_init) / 4)];

    //-----------------------------------------------------------------
    // Check fmadd.s (floating point multiply-add)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("fmadd.s %[c], %[a], %[b], %[d]\n"
                : [c] "=f"(g_fma_act[i])
        : [a] "f"(g_in_a[i]), [b] "f"(g_in_b[i]), [d] "f"(g_fma_act[i]));

        check_float(result, "fmadd.s", g_fma_act[i], g_fma_min[i], g_fma_max[i]);
    }
    //-----------------------------------------------------------------
    // Check fnmadd.s (floating point negative multiply-add)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("fnmadd.s %[c], %[a], %[b], %[d]\n"
                : [c] "=f"(g_fma_act[i])
        : [a] "f"(g_in_a[i]), [b] "f"(g_in_b[i]), [d] "f"(g_fma_act[i]));

        check_float(result, "fnmadd.s", g_fma_act[i], -g_fma_max[i], -g_fma_min[i]);
    }

    //-----------------------------------------------------------------
    // Check fmsub.s (floating point multiply-subtract)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("fmsub.s %[c], %[a], %[b], %[d]\n"
                : [c] "=f"(g_fma_act[i])
        : [a] "f"(g_in_a[i]), [b] "f"(-g_in_b[i]), [d] "f"(-g_fma_act[i]));

        check_float(result, "fmsub.s", g_fma_act[i], g_fms_min[i], g_fms_max[i]);
    }
    //-----------------------------------------------------------------
    // Check fnmsub.s (floating point negative multiply-subtract)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile("fnmsub.s %[c], %[a], %[b], %[d]\n"
                : [c] "=f"(g_fma_act[i])
        : [a] "f"(g_in_a[i]), [b] "f"(-g_in_b[i]), [d] "f"(-g_fma_act[i]));

        check_float(result, "fnmsub.s", g_fma_act[i], -g_fms_max[i], -g_fms_min[i]);
    }


    stop();
}
 */


/* Task executed by cluster cores. */
void cluster_call_micro(void *arg) {
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
//    printf("[%d %d] Hellosfsdf World!\n", cluster_id, core_id);
    MicroBenchDescriptor *micro_details = (MicroBenchDescriptor *) arg;
    execute_micro(micro_details->micro_t, core_id);
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
//    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
//    printf("Entering main controller\n");
    MicroBenchDescriptor test_definitions = {FSQRTS};

    int32_t errors = 0;
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

    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &test_definitions));

    pi_cluster_close(&cluster_dev);

    printf("Bye !\n");

    return errors;
}