/* TomsFastMath, a fast ISO C bignum library.
 * 
 * This project is meant to fill in where LibTomMath
 * falls short.  That is speed ;-)
 *
 * This project is public domain and free for all purposes.
 * 
 * Tom St Denis, tomstdenis@gmail.com
 */
/* TomsFastMath, a fast ISO C bignum library.
 * 
 * This project is meant to fill in where LibTomMath
 * falls short.  That is speed ;-)
 *
 * This project is public domain and free for all purposes.
 * 
 * Tom St Denis, tomstdenis@gmail.com
 */
#include "tfm.h"
template <int used>
void s_fp_add_fixed(fp_int *a, fp_int *b, fp_int *c){
  int      x;
  register fp_word  t;
  c->used = used;
 
  t = 0;
  for (x = 0; x < used; x++) {
      t         += ((fp_word)a->dp[x]) + ((fp_word)b->dp[x]);
      c->dp[x]   = (fp_digit)t;
      t        >>= DIGIT_BIT;
  }
}
template<int used>
void s_fp_sub_fixed2(fp_int *a, fp_int *b, fp_int *c){
  int      x, oldbused, oldused;
  fp_word  t;

  c->used  = used;
  t       = 0;
  for (x = 0; x < used; x++) {
     t         = ((fp_word)a->dp[x]) - (((fp_word)b->dp[x]) + t);
     c->dp[x]  = (fp_digit)t;
     t         = (t >> DIGIT_BIT)&1;
  }
}
asm (".macro m_subfixedloop a,b,c,tmp,iter,to ;\n"
     "movq 8*\\iter ( \\a ), \\tmp \n"
     "sbb 8*\\iter(\\b), \\tmp  \n"
     "movq \\tmp, 8*\\iter(\\c) \n"
     ".if \\to-\\iter \n"
     "m_subfixedloop \\a,\\b,\\c,\\tmp, \"(\\iter + 1)\", \\to \n"
     ".endif;\n .endm\n");
template <int used>
void s_fp_sub_fixed(fp_int *a, fp_int *b, fp_int *c);
template <>
void s_fp_sub_fixed<16> (fp_int *a, fp_int *b, fp_int *c){
  c->used  = 16;
  register fp_digit tmp;
  asm volatile("clc; \n"
      "m_subfixedloop %1,%2,%3,%0,0,16; \n"
               : "=&r"(tmp)
               : "r"(a->dp), "r"(b->dp), "r"(c->dp)
               : "memory");
}
template <>
void s_fp_sub_fixed<17> (fp_int *a, fp_int *b, fp_int *c){
  c->used  = 17;
  register fp_digit tmp;
  asm volatile("clc; \n"
      "m_subfixedloop %1,%2,%3,%0,0,17; \n"
               : "=&r"(tmp)
               : "r"(a->dp), "r"(b->dp), "r"(c->dp)
               : "memory");
}
template <>
void s_fp_sub_fixed<34>(fp_int *a, fp_int *b, fp_int *c) {
  c->used  = 34;
  register fp_digit tmp;
  asm volatile("clc; \n"
      "m_subfixedloop %1,%2,%3,%0,0,34; \n"
               : "=&r"(tmp)
               : "r"(a->dp), "r"(b->dp), "r"(c->dp)
               : "memory");
}

/* $Source: /cvs/libtom/tomsfastmath/src/mont/fp_montgomery_calc_normalization.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2006/12/31 21:25:53 $ */

/* setups the montgomery reduction */
int fp_montgomery_setup(fp_int *a, fp_digit *rho)
{
  fp_digit x, b;

/* fast inversion mod 2**k
 *
 * Based on the fact that
 *
 * XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
 *                    =>  2*X*A - X*X*A*A = 1
 *                    =>  2*(1) - (1)     = 1
 */
  b = a->dp[0];

  if ((b & 1) == 0) {
    return FP_VAL;
  }

  x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**32 */
#ifdef FP_64BIT
  x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
#endif

  /* rho = -1/m mod b */
  *rho = (((fp_word) 1 << ((fp_word) DIGIT_BIT)) - ((fp_word)x));

  return FP_OKAY;
}
#include "tfm_mul.cc"
#include "tfm_sqr.cc"
#include "tfm_mont.cc"
#include "mydiv.cc"
/* $Source: /cvs/libtom/tomsfastmath/src/mont/fp_montgomery_setup.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2006/12/31 21:25:53 $ */
