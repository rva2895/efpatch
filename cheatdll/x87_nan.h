#pragma once

// X87 UNORDERED-INCLUSIVE COMPARE MACROS (precision sub-rule (i),
// DECOMP_TEMPLATE.md). The original binary's fnstsw/test-idiom
// branches evaluate NaN as TAKEN (unordered sets C0=C2=C3=1, so the
// C0|C3 and C0 masks fire); MSVC's compiled literal operators
// (<=, <, >, >=, ==) are unordered-FALSE under /arch:SSE2 /fp:fast -
// probe-verified under /O2 and /Od (nanprobe.cpp, 2026-11-18). These
// macros reproduce the x87 taken-sets exactly. The expansions compile
// to comiss/comisd + jna/jbe/jb/ja (unordered-taken) - verify with
// /Fa at the gate. Arguments are evaluated more than once: pass only
// side-effect-free expressions (field/local reads - every decode site
// qualifies; a call argument would be a review item).
//
// taken-set notation: the operand pairs for which the macro is TRUE.

#define X87_LE_NAN(x, c)  (!((x) > (c)))                  // {<=, NaN}   (x87 test ah,41h; jnz)
#define X87_LT_NAN(x, c)  (!((x) >= (c)))                 // {<,  NaN}   (x87 test ah,1;   jnz)
#define X87_GE_NAN(x, c)  (!((x) < (c)))                  // {>=, NaN}   (x87 test ah,1;   jz)
#define X87_GT_NAN(x, c)  (!((x) <= (c)))                 // {>,  NaN}   (x87 test ah,41h; jz)
#define X87_EQ_NAN(x, c)  (!((x) < (c)) && !((x) > (c)))  // {==, NaN}   (x87 test ah,40h; jnz)
#define X87_NE_ORD(x, c)  (((x) < (c)) || ((x) > (c)))    // ordered !=  (x87 test ah,41h; jz)
