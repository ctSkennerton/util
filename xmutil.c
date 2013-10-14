#include "msutil.h" // ffs,fls
#include "xmutil.h"

#undef  DO
#define DO_8x8(op,a) DO_8(op,a,0) DO_8(op,a,1) DO_8(op,a,2) DO_8(op,a,3) DO_8(op,a,4) DO_8(op,a,5) DO_8(op,a,6) DO_8(op,a,7)
#define DO_8(op,a,b) DO(op,a,b,0) DO(op,a,b,1) DO(op,a,b,2) DO(op,a,b,3) DO(op,a,b,4) DO(op,a,b,5) DO(op,a,b,6) DO(op,a,b,7)
#define DO(op,a,b,c) case 0##a##b##c: x = xm_##op##_##a##b##c(x); break;

#define xm_shl_000(x) (x)
XMM xm_shl(XMM x, unsigned nbits)
{
    switch (nbits) { DO_8x8(shl,0) DO_8x8(shl,1) default: x = xm_zero; }
    return x;
}

#define xm_shr_000(x) (x)
XMM
xm_shr(XMM x, unsigned nbits)
{
    switch (nbits) { DO_8x8(shr,0) DO_8x8(shr,1) default: x = xm_zero; }
    return x;
}

int
xm_ffs(XMM x)
{
    int pos = ffs(xm_diff(x, xm_zero)) - 1;
    return  pos < 0 ? pos : (pos << 3) + ffs(((unsigned char const*)&x)[pos]) - 1;
}

int
xm_fls(XMM x)
{
    int pos = fls(xm_diff(x, xm_zero)) - 1;
    return  pos < 0 ? pos : (pos << 3) + fls(((unsigned char const*)&x)[pos]) - 1;
}

char*
xm_dbl(__m128d x, char buf[48])
{
    union { __m128d xm; double b[2]; } a = { x };
    sprintf(buf, "%g,%g", a.b[0], a.b[1]);
    return buf;
}

char*
xm_llx(XMM x, char buf[48])
{
    union { XMM xm; unsigned long long b[2]; } a = { x };
    sprintf(buf, "0x%016llX,0x%016llX", a.b[0], a.b[1]);
    return buf;
}

char*
xm_str(XMM x, char buf[48])
{
    int i;
    union { XMM xm; unsigned char b[16]; } a = { x };
    char *cp = buf + sprintf(buf, "%02X", a.b[0]);
    for (i = 1; i < 16; ++i) cp += sprintf(cp, "%c%02X", i == 8 ? '-' : ',', a.b[i]);
    return buf;
}
