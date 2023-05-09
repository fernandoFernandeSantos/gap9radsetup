// This testbench checks the basic functionality of:
//
// lf.add.s
// lf.sub.s
// lf.mul.s
// lf.div.s
// lf.itof.s
// lf.ftoi.s
// lf.sfeq.s
// lf.sfne.s
// lf.sfgt.s
// lf.sfge.s
// lf.sflt.s
// lf.sfle.s
// lf.sin.s
// lf.cos.s
// lf.atan.s
// etc.

/////////////////////////////////////////////////////////
// includes
/////////////////////////////////////////////////////////
#include "testLNU.h"
#include "math_fns.h"
#include "pmsis.h"

/////////////////////////////////////////////////////////
// Function prototypes
/////////////////////////////////////////////////////////
void check_basic(testresult_t *result, void (*start)(), void (*stop)());

void check_trig(testresult_t *result, void (*start)(), void (*stop)());

void check_fma(testresult_t *result, void (*start)(), void (*stop)());

void check_explog(testresult_t *result, void (*start)(), void (*stop)());

void check_sqrtpow(testresult_t *result, void (*start)(), void (*stop)());

void check_mexdex(testresult_t *result, void (*start)(), void (*stop)());

void check_vect_basic(testresult_t *result, void (*start)(), void (*stop)());

void check_vect_sqrtpow(testresult_t *result, void (*start)(), void (*stop)());

void check_vect_cmp(testresult_t *result, void (*start)(), void (*stop)());


/////////////////////////////////////////////////////////
// Test case selection
/////////////////////////////////////////////////////////
testcase_t testcases[] = {
        {.name = "basic", .test = check_basic},
        {.name = "trig", .test = check_trig},
        {.name = "fma", .test = check_fma},
        {.name = "explog", .test = check_explog},
        {.name = "sqrtpow", .test = check_sqrtpow},
        {.name = "mexdex", .test = check_mexdex},
        {.name = "vect_basic", .test = check_vect_basic},
        {.name = "vect_sqrtpow", .test = check_vect_sqrtpow},
        {.name = "vect_cmp", .test = check_vect_cmp},
        {0, 0}
};

/////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////
int main() {
    int errors = 1;
    int pass = 0;

    /* float fll; */

    /* __fp16 test = lala[0] + lala[1]; */

    /* asm volatile ("lf.htof.lo %[c], %[a]\n" */
    /*                 : [c] "=r" (fll) */
    /*                 : [a] "r"  (lala[0])); */

    /* fll = fll + g_in_b[2]; */
    /* printFloat(0,fll); */

    /* asm volatile ("lf.htof.lo %[c], %[a]\n" */
    /*                 : [c] "=r" (fll) */
    /*                 : [a] "r"  (lala2[0])); */

    /* fll = fll + g_in_b[2]; */
    /* printFloat(0,fll); */

    int coreid = pi_core_id();

    if (coreid == 0) {
        testresult_t result;

        run_suite(testcases, &result);


        if (result.errors == 0)
            pass = 1;

        team_barrier();
        flagPassFail(pass, coreid);
        printErrors(result.errors);
        cycleCount(result.time);
        eoc(0);
    } else {
        team_barrier();
        flagPassFail(1, coreid);
    }
}

/////////////////////////////////////////////////////////
// Function definitions
/////////////////////////////////////////////////////////
void check_basic(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    //-----------------------------------------------------------------
    // Check lf.add.s (floating point addition)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        asm volatile ("lf.add.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.add.s", g_add_act[i], g_add_min[i], g_add_max[i]);
    }

    //-----------------------------------------------------------------
    // Check lf.sub.s (floating point subtraction)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_sub_act) / 4); i++) {
        asm volatile ("lf.sub.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_sub_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.add.s", g_sub_act[i], g_sub_min[i], g_sub_max[i]);
    }
    //-----------------------------------------------------------------
    // Check lf.mul.s (floating point multiplication)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_mul_act) / 4); i++) {
        asm volatile ("lf.mul.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_mul_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.mul.s", g_mul_act[i], g_mul_min[i], g_mul_max[i]);
    }
    //-----------------------------------------------------------------
    // Check lf.div.s (floating point division)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_div_act) / 4); i++) {
        asm volatile ("lf.div.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_div_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.div.s", g_div_act[i], g_div_min[i], g_div_max[i]);
    }
    stop();
}

void check_explog(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;

    //-----------------------------------------------------------------
    // Check lf.exp.s (floating point exponentiation ^2)
    //-----------------------------------------------------------------
    start();
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        float g_exp_in = g_in_a[i] / 100.0F;
        asm volatile ("lf.exp.s %[c], %[a]\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_exp_in));

        check_float(result, "lf.exp.s", g_add_act[i], g_exp_min[i], g_exp_max[i]);
    }

    //-----------------------------------------------------------------
    // Check lf.log.s (floating point logarithm (base 2))
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        asm volatile ("lf.log.s %[c], %[a]\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_in_a[i]));

        check_float(result, "lf.log.s", g_add_act[i], g_log_min[i], g_log_max[i]);
    }
    stop();
}

void check_sqrtpow(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    //-----------------------------------------------------------------
    // Check lf.sqrt.s (floating point square root)
    //-----------------------------------------------------------------

    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        asm volatile ("lf.sqrt.s %[c], %[a], 0x1\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_in_a[i]));

        check_float(result, "lf.sqrt.s", g_add_act[i], g_sqrt_min[i], g_sqrt_max[i]);
    }

    //-----------------------------------------------------------------
    // Check lf.pow.s (floating point power function)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        asm volatile ("lf.pow.s %[c], %[a], 0x1\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_in_a[i]));

        check_float(result, "lf.pow.s", g_add_act[i], g_pow_min[i], g_pow_max[i]);
    }
    stop();

}


#define TRIG
#ifdef TRIG

void check_trig(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    float pi2 = MATH_0_5PI;
    //-----------------------------------------------------------------
    // Check lf.sin.s (floating point sin)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        float tmp;
        asm volatile ("lf.sca.s %[c], %[a], %[b]\n"
                : [c] "=r"(tmp)
        : [a] "r"(g_trig_in[i]), [b] "r"(pi2));
        asm volatile ("lf.sin.s %[c], %[a]\n"
                : [c] "=r"(g_trig_act[i])
        : [a] "r"(tmp));

        check_float(result, "lf.sin.s", g_trig_act[i], g_sin_min[i], g_sin_max[i]);
    }
    //-----------------------------------------------------------------
    // Check lf.cos.s (floating point cos)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        float tmp;
        asm volatile ("lf.sca.s %[c], %[a], %[b]\n"
                : [c] "=r"(tmp)
        : [a] "r"(g_trig_in[i]), [b] "r"(pi2));
        asm volatile ("lf.cos.s %[c], %[a]\n"
                : [c] "=r"(g_trig_act[i])
        : [a] "r"(tmp));

        check_float(result, "lf.cos.s", g_trig_act[i], g_cos_min[i], g_cos_max[i]);
    }
    //-----------------------------------------------------------------
    // Check lf.atan.s (floating point atan) // atan2(y,x); y=a, x=b
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_trig_act) / 4); i++) {
        float x, y;
        x = g_in_b[i];
        y = g_in_a[i];
        asm volatile ("lf.ata.s %[c], %[a], %[b]\n"      : [c] "=r"(x): [a] "r"(y), [b] "r"(x));
        asm volatile ("lf.atan.s %[c], %[a], %[b]\n"     : [c] "=r"(x): [a] "r"(x), [b] "r"(y));
        asm volatile ("lf.atl.s %[c], %[a]\n"            : [c] "=r"(x): [a] "r"(x));
        float pi2 = MATH_0_5PI;
        g_trig_act[i] = x * pi2;
        check_float(result, "lf.atan.s", g_trig_act[i], g_atan_min[i], g_atan_max[i]);
    }
    stop();
}

#else
void check_trig(testresult_t *result, void (*start)(), void (*stop)()) {}
#endif

#define MEXDEX
#ifdef MEXDEX

void check_mexdex(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    //-----------------------------------------------------------------
    // Check lf.mex.s (floating point multiply-exponentiate)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        float g_exp_a_in = g_in_a[i] / 256.0F;
        float g_exp_b_in = g_in_b[i] / 256.0F;
        asm volatile ("lf.mex.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_exp_a_in), [b] "r"(g_exp_b_in));

        check_float(result, "lf.mex.s", g_add_act[i], g_mex_min[i], g_mex_max[i]);
    }

    //-----------------------------------------------------------------
    //Check lf.dex.s (floating point divide-exponentiate)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_add_act) / 4); i++) {
        asm volatile ("lf.dex.s %[c], %[a], %[b]\n"
                : [c] "=r"(g_add_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.dex.s", g_add_act[i], g_dex_min[i], g_dex_max[i]);
    }
    stop();
}

#else
void check_mexdex(testresult_t *result, void (*start)(), void (*stop)()) {}
#endif

#define FMA
#ifdef FMA

void check_fma(testresult_t *result, void (*start)(), void (*stop)()) {
    unsigned int i;
    start();
    //-----------------------------------------------------------------
    // Check lf.fma.s (floating point multiply-add)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("lf.fma.s %[c], %[a], %[b]\n"
                : [c] "+r"(g_fma_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.fma.s", g_fma_act[i], g_fma_min[i], g_fma_max[i]);
    }

    //-----------------------------------------------------------------
    //Check lf.fda.s (floating point divide-add)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("lf.fda.s %[c], %[a], %[b]\n"
                : [c] "+r"(g_fma_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.fda.s", g_fma_act[i], g_fda_min[i], g_fda_max[i]);
    }

    //-----------------------------------------------------------------
    // Check lf.fms.s (floating point multiply subtract)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("lf.fms.s %[c], %[a], %[b]\n"
                : [c] "+r"(g_fma_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.fms.s", g_fma_act[i], g_fms_min[i], g_fms_max[i]);
    }

    //-----------------------------------------------------------------
    // Check lf.fds.s (floating point divide subtract)
    //-----------------------------------------------------------------
    for (i = 0; i < (sizeof(g_fma_init) / 4); i++) {
        g_fma_act[i] = g_fma_init[i];
        asm volatile ("lf.fds.s %[c], %[a], %[b]\n"
                : [c] "+r"(g_fma_act[i])
        : [a] "r"(g_in_a[i]), [b] "r"(g_in_b[i]));

        check_float(result, "lf.fds.s", g_fma_act[i], g_fds_min[i], g_fds_max[i]);
    }
    stop();
}

#else
void check_fma(testresult_t *result, void (*start)(), void (*stop)()) {}
#endif



//#define VECT
#ifdef VECT
void check_vect_basic(testresult_t *result, void (*start)(), void (*stop)()) {
  unsigned int i;
  start();
  //-----------------------------------------------------------------
  // Check lf.add.vh (half precision floating point vectorial addition)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_add_act)/4); i++) {
    asm volatile ("lf.add.vh %[c], %[a], %[b]\n"
                  : [c] "=r" (g_add_act[i])
                : [a] "r"  (g_vect_a[i]), [b] "r" (g_vect_b[i]));

    check_vect_float(result, "lf.add.vh", g_add_act[i],  g_add_min[i], g_add_max[i]);
  }

  //-----------------------------------------------------------------
  // Check lf.sub.vh (half precision floating point vectorial subtraction)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_sub_act)/4); i++) {
    asm volatile ("lf.sub.vh %[c], %[a], %[b]\n"
                  : [c] "=r" (g_sub_act[i])
                : [a] "r"  (g_vect_a[i]), [b] "r" (g_vect_b[i]));

    check_vect_float(result, "lf.sub.vh", g_sub_act[i],  g_sub_min[i], g_sub_max[i]);
  }

  //-----------------------------------------------------------------
  // Check lf.mul.vh (half precision floating point vectorial multiplication)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_mul_act)/4); i++) {
    asm volatile ("lf.mul.vh %[c], %[a], %[b]\n"
                  : [c] "=r" (g_mul_act[i])
                : [a] "r"  (g_vect_small_a[i]), [b] "r" (g_vect_small_b[i]));

    check_vect_float(result, "lf.mul.vh", g_mul_act[i],  g_mul_small_min[i], g_mul_small_max[i]);
  }

  //-----------------------------------------------------------------
  // Check lf.div.vh (half precision floating point vectorial division)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_div_act)/4); i++) {
    asm volatile ("lf.div.vh %[c], %[a], %[b]\n"
                  : [c] "=r" (g_div_act[i])
                : [a] "r"  (g_vect_a[i]), [b] "r" (g_vect_b[i]));

    check_vect_float(result, "lf.div.vh", g_div_act[i],  g_div_min[i], g_div_max[i]);
  }



  stop();
}

void check_vect_sqrtpow(testresult_t *result, void (*start)(), void (*stop)()) {
  unsigned int i;
  start();
  //-----------------------------------------------------------------
  // Check lf.sqrt.vh (half precision floating point vectorial square root)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_add_act)/4); i++) {
    asm volatile ("lf.sqrt.vh %[c], %[a], 0x1\n"
                  : [c] "=r" (g_add_act[i])
              : [a] "r"  (g_vect_a[i]));

    check_vect_float(result, "lf.sqrt.vh", g_add_act[i],  g_sqrt_min[i], g_sqrt_max[i]);
  }

  //-----------------------------------------------------------------
  // Check lf.pow.vh (half precision floating point vectorial square operation)
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_add_act)/4); i++) {
    asm volatile ("lf.pow.vh %[c], %[a], 0x1\n"
                  : [c] "=r" (g_add_act[i])
                : [a] "r"  (g_vect_small_a[i]));
    
    check_vect_float(result, "lf.pow.vh", g_add_act[i],  g_pow_small_min[i], g_pow_small_max[i]);
  }
   
  stop();
}

float res_ok = 4.433F;
float res_bad = 1.1F;

void check_vect_cmp(testresult_t *result, void (*start)(), void (*stop)()) {
  unsigned int i;
  float act;
  start();
  //-----------------------------------------------------------------
  // Check lf.sfeq.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfeq.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfeq.vh.all", act, exp_sfeq_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfne.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfne.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfne.vh.all", act, exp_sfne_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfge.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfge.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfge.vh.all", act, exp_sfge_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfgt.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfgt.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfgt.vh.all", act, exp_sfgt_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfle.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfle.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfle.vh.all", act, exp_sfle_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sflt.vh.all
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sflt.vh.all %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sflt.vh.all", act, exp_sflt_vh_all[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfeq.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfeq.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfeq.vh.any", act, exp_sfeq_vh_any[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfne.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfne.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfne.vh.any", act, exp_sfne_vh_any[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfge.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfge.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfge.vh.any", act, exp_sfge_vh_any[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfgt.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfgt.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfgt.vh.any", act, exp_sfgt_vh_any[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sfle.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sfle.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sfle.vh.any", act, exp_sfle_vh_any[i],i);
  }
  //-----------------------------------------------------------------
  // Check lf.sflt.vh.any
  //-----------------------------------------------------------------
  for(i = 0; i < (sizeof(g_vect_sf_a)/4); i++) {
    asm volatile ("lf.sflt.vh.any %[a], %[b]\n"
                  "l.cmov %[c],  %[d0], %[d1]\n"
                  : [c]  "+r" (act)
                  : [d0] "r"  (res_ok),
                    [d1] "r"  (res_bad),
                    [a]  "r"  (g_vect_sf_a[i]),
                    [b]  "r"  (g_vect_sf_b[i]));


    check_float_value(result, "lf.sflt.vh.any", act, exp_sflt_vh_any[i],i);
  }   
  stop();
}
#else

void check_vect_basic(testresult_t *result, void (*start)(), void (*stop)()) {}

void check_vect_sqrtpow(testresult_t *result, void (*start)(), void (*stop)()) {}

void check_vect_cmp(testresult_t *result, void (*start)(), void (*stop)()) {}

#endif
