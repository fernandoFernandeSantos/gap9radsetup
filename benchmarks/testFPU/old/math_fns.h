/*----------------------------------------------------------------------------
| special FP math functions for ETHZ openrisc cores with FPU or LNU.
| Michael Gautschi, Michael Schaffner 2015
*----------------------------------------------------------------------------*/
#ifndef __MATH_FNS_H__
#define __MATH_FNS_H__

/*----------------------------------------------------------------------------
| some constants
*----------------------------------------------------------------------------*/
#define MATH_PI          3.141592653589793F
#define MATH_2PI         6.283185307179586F
#define MATH_0_5PI       1.570796326794897F
#define MATH_1_5PI       4.712388980384690F

#define MATH_LOG2_10     3.321928094887362F
#define MATH_LOG2_E      1.442695040888963F

#define MATH_INV_LOG2_10 0.301029995663981F
#define MATH_INV_LOG2_E  0.693147180559945F

// define masks for s8.23 FP/LNS format
#define MATH_ABS_MASK 0x7FFFFFFF
#define MATH_SGN_MASK 0x80000000
#define MATH_EXP_MASK 0x3F800000
#define MATH_MNT_MASK 0x007FFFFF

#define MATH_ABS_MASK_HI 0x7FFF
#define MATH_ABS_MASK_LO 0xFFFF

#define MATH_SGN_MASK_HI 0x8000

#define CLASS_INF_MASK     0x0081
#define CLASS_NAN_MASK     0x0300
#define CLASS_ZERO_MASK    0x0018
#define CLASS_SUBNORM_MASK 0x0024
#define CLASS_NORMAL_MASK  0x0042

/*----------------------------------------------------------------------------
| FP function headers. note that some functions are so small that they can
| be inlined! note that the inlined functions are implemented in this header
| (below)
*----------------------------------------------------------------------------*/

// note1: these are integers and cannot directly be assigned to a float using =

#define MATH_INF      0x7F800000
#define MATH_NAN      0x7FC00000
#define MATH_INF_HI   0x7F80
#define MATH_NAN_HI   0x7FC0
#define MATH_ZERO_HI  0x0000

#ifndef HWDIV
#define HWDIV 1
#endif
#ifndef HWSQRT
#define HWSQRT 1
#endif

// not used right now:
// in this case include the softfloat lib for div and sqrt emulation
//#include "softfloat.h"
// #include "math.h"

// uses 3 iterations newton raphson
// initial estimate is generated using
// a fast linear approximation
// exact to around 6 digits
// TODO: special case handling!
#if (HWDIV == 1)

static inline float fDiv(float a, float b);

#else
float fDiv(float a, float b);
#endif

// squareroot based on inverse newton raphson squareroot
#ifdef LINUX
float fSqrt(float x);
#else
#if (HWSQRT == 1)

static inline float fSqrt(float x);

#else
float fSqrt(float x);
#endif
#endif
// // uses 3 iterations newton raphson
// // initial estimate is generated using
// // a fast inverse square root
// // TODO: special case handling!
// float fInvSqrt(float x);

// 2^x
// precise to about 6 digits
float fExp2(float x);

// reuses 2^x
// precise to about 5 digits
static inline float fExp(float x);

// reuses 2^x
// precise to about 5 digits
static inline float fExp10(float x);

// log2(x)
// precise to about 6 digits
float fLog2(float x);

// reuses log2(x)
// precise to about 5 digits
static inline float fLog(float x);

// reuses log2(x)
// precise to about 5 digits
static inline float fLog10(float x);


/*----------------------------------------------------------------------------
| these functions are the same for LNS and regular FP.
*----------------------------------------------------------------------------*/

// enable this for trigonometric intrinsics
#ifdef TRIG
static inline float fCos(float x);
static inline float fAtan2(float y, float x);
static inline float fSin(float x);
static inline float fTan(float x);
static inline float fCot(float x);
static inline float fAtan(float x);
static inline float fAsin(float x);
static inline float fAcos(float x);
#else

// wrapper functions which determine
// the quadrant and call the actual implementation
// precise to about 6.7 digits
float fCos(float x);

// precise to around 6.4 digits
float fAtan2(float y, float x);

// these functions are derived from the cos and atan2 subroutines
// (they just perform a different range reduction)
static inline float fSin(float x);

static inline float fTan(float x);

static inline float fCot(float x);

static inline float fAtan(float x);

static inline float fAsin(float x);

static inline float fAcos(float x);

#endif

// mask away the sign bit
static inline float fAbs(float x);

static inline char fIsNan(float x);

static inline char fIsInf(float x);


/*----------------------------------------------------------------------------
| FP inline function implementations. 
*----------------------------------------------------------------------------*/

static inline float fExp(float x) {
    // correction factor for e
    return fExp2(x * MATH_LOG2_E);
}

static inline float fExp10(float x) {
    // correction factor for 10
    return fExp2(x * MATH_LOG2_10);
}

static inline float fLog(float x) {
    // correction factor for e
    return fLog2(x) * MATH_INV_LOG2_E;
}

static inline float fLog10(float x) {
    // correction factor for 10
    return fLog2(x) * MATH_INV_LOG2_10;
}

/*----------------------------------------------------------------------------
| implementation of shared inline functions
*----------------------------------------------------------------------------*/
#if (HWSQRT == 1)
#ifndef LINUX

static inline float fSqrt(float a) {
    float c;
    asm ("fsqrt.s %[c], %[a]\n"
            : [c] "=f"(c)
    : [a] "f"(a));
    return c;
}

#endif
#endif

#if (HWDIV == 1)

static inline float fDiv(float a, float b) {
#ifndef LINUX
#ifndef FP_SW_EMUL
    float c;
    asm ("fdiv.s %[c], %[a], %[b]\n"
            : [c] "=f"(c)
    : [a] "f"(a), [b] "f"(b));
    return c;
#else
    return a / b;
#endif
#else
    return a / b;
#endif
}

#endif

static inline float fSin(float x) {
#ifdef TRIG
    float pi2 = MATH_0_5PI;
    asm ("lf.sca.s %[c], %[a], %[b]": [c] "=r" (x): [a] "r"  (x), [b] "r" (pi2));
    asm ("lf.sin.s %[c], %[a]"      : [c] "=r" (x): [a] "r"  (x));
    return x;
#else
    return fCos(MATH_0_5PI - x);
#endif
}

#ifdef TRIG
static inline float fCos(float x)
{
  float pi2 = MATH_0_5PI;
  asm ("lf.sca.s %[c], %[a], %[b]": [c] "=r" (x): [a] "r"  (x), [b] "r" (pi2));
  asm ("lf.cos.s %[c], %[a]"      : [c] "=r" (x): [a] "r"  (x));
  return x;
}
#endif

#ifdef TRIG
static inline float fAtan2(float y, float x)
{
  float pi2 = MATH_0_5PI;
  asm ("lf.ata.s  %[c], %[a], %[b]"     : [c] "=r" (x): [a] "r"  (y), [b] "r"(x));
  asm ("lf.atan.s %[c], %[a], %[b]"     : [c] "=r" (x): [a] "r"  (x), [b] "r"(y));
  asm ("lf.atl.s  %[c], %[a]"           : [c] "=r" (x): [a] "r"  (x));
  return (x*pi2);
}
#endif

// derived trig functions
static inline float fTan(float x) {
#ifdef TRIG
    float pi2 = MATH_0_5PI;
    float y;
    asm ("lf.sca.s %[c], %[a], %[b]": [c] "=r" (x): [a] "r"  (x), [b] "r" (pi2));
    asm ("lf.sin.s %[c], %[a]"      : [c] "=r" (y): [a] "r"  (x));
    asm ("lf.cos.s %[c], %[a]"      : [c] "=r" (x): [a] "r"  (x));
    return y/x;
#else
    return fDiv(fSin(x), fCos(x));
#endif
}

static inline float fCot(float x) {
#ifdef TRIG
    float pi2 = MATH_0_5PI;
    float y;
    asm ("lf.sca.s %[c], %[a], %[b]": [c] "=r" (x): [a] "r"  (x), [b] "r" (pi2));
    asm ("lf.sin.s %[c], %[a]"      : [c] "=r" (y): [a] "r"  (x));
    asm ("lf.cos.s %[c], %[a]"      : [c] "=r" (x): [a] "r"  (x));
    return x/y;
#else
    return fDiv(fCos(x), fSin(x));
#endif
}

static inline float fAtan(float x) {
    return fAtan2(x, 1.0F);
}

static inline float fAsin(float x) {
    return fAtan2(x, fSqrt(1.0F - x * x));
}

static inline float fAcos(float x) {
    return fAtan2(fSqrt(1.0F - x * x), x);
}


static inline float fAbs(float x) {

    return ((x < 0.0F) ? (-x) : (x));

}

static inline char fIsNan(float x) {
    return (x != x);
}

static inline char fIsInf(float x) {

#ifndef LINUX
#ifndef FP_SW_EMUL
    int class;
    asm ("fclass.s %[c], %[a]\n"
            : [c] "=r"(class)
    : [a] "f"(x));
    return ((class & CLASS_INF_MASK) != 0);
#else
    x = fAbs(x);
    return (*(unsigned int *)&x == (MATH_INF_HI<<16));
#endif
#else
    x = fAbs(x);
    return (*(unsigned int *)&x == (MATH_INF_HI<<16));
#endif
}

#endif
