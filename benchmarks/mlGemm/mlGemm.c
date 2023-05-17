/////////////////////////////////////////////////////////
// includes
/////////////////////////////////////////////////////////

#include "mlGemm.h"
#include "pmsis.h"
#include "shared_globals.h"

#ifndef USE_CLUSTER
#define USE_CLUSTER
#endif

/////////////////////////////////////////////////////////
// subfunctions
/////////////////////////////////////////////////////////


float sum(const float x[100]) {
    float y;
    int k;
    y = x[0];
    for (k = 0; k < 99; k++) {
        y += x[k + 1];
    }

    return y;
}

float var(const float x[100]) {
    float y;
    int ix;
    float xbar;
    int k;
    float r;
    ix = 0;
    xbar = x[0];
    for (k = 0; k < 99; k++) {
        ix++;
        xbar += x[ix];
    }

    xbar *= 1.0F / 100.0F;
    ix = 0;
    r = x[0] - xbar;
    y = r * r;
    for (k = 0; k < 99; k++) {
        ix++;
        r = x[ix] - xbar;
        y += r * r;
    }

    y *= 1.0F / 99.0F;
    return y;
}

boolean_T checkRes(const float check[2], const float golden[4]) {
    int k;
    boolean_T y = true;
    for (k = 0; k < 2; k++) {
        y = y && (check[k] <= golden[k << 1]);
        y = y && (check[k] >= golden[1 + (k << 1)]);
        printErrors(!y, k, check[k], golden[k << 1], golden[1 + (k << 1)]);
    }

    return y;
}

void mlGemm(const float A[100], const float B[100], float C[100], float a, float b) {
    int i2;
    int i3;
    float f0;
    int i4;

    /*  mlGemm */
    for (i2 = 0; i2 < 10; i2++) {
        for (i3 = 0; i3 < 10; i3++) {
            f0 = 0.0F;
            for (i4 = 0; i4 < 10; i4++) {
                f0 += a * A[i2 + 10 * i4] * B[i4 + 10 * i3];
            }
            C[i2 + 10 * i3] = f0 + b * C[i2 + 10 * i3];
        }
    }
}

/////////////////////////////////////////////////////////
// main testing function
/////////////////////////////////////////////////////////
int main(int argc, const char *const argv[]) {
    (void) argc;
    (void) argv;

    int coreid;
    int it;

    boolean_T pass;
    float C[100];
    int i0;
    int i1;
    float f0;
    int i2;
    float tmp[2];


    /////////////////////////////////////////////////////////
    // main test loop
    // each core loops over a kernel instance
    /////////////////////////////////////////////////////////


    coreid = pi_core_id();

#ifdef USE_CLUSTER
    if (pi_is_fc())
      return exec_cluster(main);
#endif

    printf("starting %d kernel iterations... (coreid = %d)\n", KERNEL_ITS, coreid);

    if (pi_is_fc()) {
        coreid = 0;
    }

    if (coreid > 3)
        coreid = coreid - 4;

    team_barrier();

    perf_begin();

    for (it = 0; it < getKernelIts(); it++) {
        // matlab kernel
        for (i0 = 0; i0 < 10; i0++) {
            for (i1 = 0; i1 < 10; i1++) {
                C[i1 + 10 * i0] = fv0[(i1 + 10 * i0) + 100 * coreid];
            }
        }

        mlGemm(*(float (*)[100]) &fv4[100 * coreid], *(float (*)[100]) &fv3[100 * coreid], C, fv2[coreid], fv1[coreid]);
    }

    team_barrier();

    perf_end();

    team_barrier();

    /////////////////////////////////////////////////////////
    // check results
    /////////////////////////////////////////////////////////

    tmp[0] = sum(C);
    tmp[1] = var(C);
    pass = checkRes(tmp, *(float (*)[4]) &fv5[coreid << 2]);
    flagPassFail(pass, pi_core_id());


/////////////////////////////////////////////////////////
// synchronize and exit
/////////////////////////////////////////////////////////

    if (pass) {
        printf("==== %s: SUMMARY: SUCCESS\n", pi_is_fc() ? "FC" : "CLUSTER");
    }

    return !pass;
}
