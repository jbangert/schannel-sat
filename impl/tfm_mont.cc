/* TomsFastMath, a fast ISO C bignum library.
 * 
 * This project is meant to fill in where LibTomMath
 * falls short.  That is speed ;-)
 *
 * This project is public domain and free for all purposes.
 * 
 * Tom St Denis, tomstdenis@gmail.com
 */


#include <tfm.h>
#define LO 0
/******************************************************************/
#if defined(TFM_X86) && !defined(TFM_SSE2) 
/* x86-32 code */

#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                          \
asm(                                                      \
   "movl %5,%%eax \n\t"                                   \
   "mull %4       \n\t"                                   \
   "addl %1,%%eax \n\t"                                   \
   "adcl $0,%%edx \n\t"                                   \
   "addl %%eax,%0 \n\t"                                   \
   "adcl $0,%%edx \n\t"                                   \
   "movl %%edx,%1 \n\t"                                   \
:"=g"(_c[LO]), "=r"(cy)                                   \
:"0"(_c[LO]), "1"(cy), "g"(mu), "g"(*tmpm++)              \
: "%eax", "%edx", "%cc")

#define PROPCARRY                           \
asm(                                        \
   "addl   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbl %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%eax", "%cc")

/******************************************************************/
#elif defined(TFM_X86_64)
/* x86-64 code */

#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                          \
asm(                                                      \
   "movq %5,%%rax \n\t"                                   \
   "mulq %4       \n\t"                                   \
   "addq %1,%%rax \n\t"                                   \
   "adcq $0,%%rdx \n\t"                                   \
   "addq %%rax,%0 \n\t"                                   \
   "adcq $0,%%rdx \n\t"                                   \
   "movq %%rdx,%1 \n\t"                                   \
:"=g"(_c[LO]), "=r"(cy)                                   \
:"0"(_c[LO]), "1"(cy), "r"(mu), "r"(*tmpm++)              \
: "%rax", "%rdx", "%cc")

#define INNERMUL8 \
 asm(                  \
 "movq 0(%5),%%rax    \n\t"  \
 "movq 0(%2),%%r10    \n\t"  \
 "movq 0x8(%5),%%r11  \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x8(%2),%%r10  \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0(%0)    \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x10(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x10(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x8(%0)  \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x18(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x18(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x10(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x20(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x20(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x18(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x28(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x28(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x20(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x30(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x30(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x28(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x38(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x38(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x30(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x38(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
:"=r"(_c), "=r"(cy)                    \
: "0"(_c),  "1"(cy), "g"(mu), "r"(tmpm)\
: "%rax", "%rdx", "%r10", "%r11", "%cc")


#define PROPCARRY                           \
asm(                                        \
   "addq   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbq %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%rax", "%cc")

/******************************************************************/
#elif defined(TFM_SSE2)  
/* SSE2 code (assumes 32-bit fp_digits) */
/* XMM register assignments:
 * xmm0  *tmpm++, then Mu * (*tmpm++)
 * xmm1  c[x], then Mu
 * xmm2  mp
 * xmm3  cy
 * xmm4  _c[LO]
 */

#define MONT_START \
   asm("movd %0,%%mm2"::"g"(mp))

#define MONT_FINI \
   asm("emms")

#define LOOP_START          \
asm(                        \
"movd %0,%%mm1        \n\t" \
"pxor %%mm3,%%mm3     \n\t" \
"pmuludq %%mm2,%%mm1  \n\t" \
:: "g"(c[x]))

/* pmuludq on mmx registers does a 32x32->64 multiply. */
#define INNERMUL               \
asm(                           \
   "movd %1,%%mm4        \n\t" \
   "movd %2,%%mm0        \n\t" \
   "paddq %%mm4,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm0  \n\t" \
   "paddq %%mm0,%%mm3    \n\t" \
   "movd %%mm3,%0        \n\t" \
   "psrlq $32, %%mm3     \n\t" \
:"=g"(_c[LO]) : "0"(_c[LO]), "g"(*tmpm++) );

#define INNERMUL8 \
asm(                           \
   "movd 0(%1),%%mm4     \n\t" \
   "movd 0(%2),%%mm0     \n\t" \
   "paddq %%mm4,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm0  \n\t" \
   "movd 4(%2),%%mm5     \n\t" \
   "paddq %%mm0,%%mm3    \n\t" \
   "movd 4(%1),%%mm6     \n\t" \
   "movd %%mm3,0(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "movd 8(%2),%%mm6     \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd 8(%1),%%mm7     \n\t" \
   "movd %%mm3,4(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm7,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm6  \n\t" \
   "movd 12(%2),%%mm7    \n\t" \
   "paddq %%mm6,%%mm3    \n\t" \
   "movd 12(%1),%%mm5     \n\t" \
   "movd %%mm3,8(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm5,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm7  \n\t" \
   "movd 16(%2),%%mm5    \n\t" \
   "paddq %%mm7,%%mm3    \n\t" \
   "movd 16(%1),%%mm6    \n\t" \
   "movd %%mm3,12(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "movd 20(%2),%%mm6    \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd 20(%1),%%mm7    \n\t" \
   "movd %%mm3,16(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm7,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm6  \n\t" \
   "movd 24(%2),%%mm7    \n\t" \
   "paddq %%mm6,%%mm3    \n\t" \
   "movd 24(%1),%%mm5     \n\t" \
   "movd %%mm3,20(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm5,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm7  \n\t" \
   "movd 28(%2),%%mm5    \n\t" \
   "paddq %%mm7,%%mm3    \n\t" \
   "movd 28(%1),%%mm6    \n\t" \
   "movd %%mm3,24(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd %%mm3,28(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
:"=r"(_c) : "0"(_c), "g"(tmpm) );

#define LOOP_END \
asm( "movd %%mm3,%0  \n" :"=r"(cy))

#define PROPCARRY                           \
asm(                                        \
   "addl   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbl %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%eax", "%cc")

/******************************************************************/
#elif defined(TFM_ARM)
   /* ARMv4 code */

#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                    \
asm(                                \
    " LDR    r0,%1            \n\t" \
    " ADDS   r0,r0,%0         \n\t" \
    " MOVCS  %0,#1            \n\t" \
    " MOVCC  %0,#0            \n\t" \
    " UMLAL  r0,%0,%3,%4      \n\t" \
    " STR    r0,%1            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(*tmpm++),"1"(_c[0]):"r0","%cc");

#define PROPCARRY                  \
asm(                               \
    " LDR   r0,%1            \n\t" \
    " ADDS  r0,r0,%0         \n\t" \
    " STR   r0,%1            \n\t" \
    " MOVCS %0,#1            \n\t" \
    " MOVCC %0,#0            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"r0","%cc");

/******************************************************************/
#elif defined(TFM_PPC32)

/* PPC32 */
#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                     \
asm(                                 \
   " mullw    16,%3,%4       \n\t"   \
   " mulhwu   17,%3,%4       \n\t"   \
   " addc     16,16,%0       \n\t"   \
   " addze    17,17          \n\t"   \
   " lwz      18,%1          \n\t"   \
   " addc     16,16,18       \n\t"   \
   " addze    %0,17          \n\t"   \
   " stw      16,%1          \n\t"   \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(tmpm[0]),"1"(_c[0]):"16", "17", "18","%cc"); ++tmpm;

#define PROPCARRY                    \
asm(                                 \
   " lwz      16,%1         \n\t"    \
   " addc     16,16,%0      \n\t"    \
   " stw      16,%1         \n\t"    \
   " xor      %0,%0,%0      \n\t"    \
   " addze    %0,%0         \n\t"    \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"16","%cc");

/******************************************************************/
#elif defined(TFM_PPC64)

/* PPC64 */
#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                     \
asm(                                 \
   " mulld    r16,%3,%4       \n\t"   \
   " mulhdu   r17,%3,%4       \n\t"   \
   " addc     r16,16,%0       \n\t"   \
   " addze    r17,r17          \n\t"   \
   " ldx      r18,0,%1        \n\t"   \
   " addc     r16,r16,r18       \n\t"   \
   " addze    %0,r17          \n\t"   \
   " sdx      r16,0,%1        \n\t"   \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(tmpm[0]),"1"(_c[0]):"r16", "r17", "r18","%cc"); ++tmpm;

#define PROPCARRY                    \
asm(                                 \
   " ldx      r16,0,%1       \n\t"    \
   " addc     r16,r16,%0      \n\t"    \
   " sdx      r16,0,%1       \n\t"    \
   " xor      %0,%0,%0      \n\t"    \
   " addze    %0,%0         \n\t"    \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"r16","%cc");

/******************************************************************/
#elif defined(TFM_AVR32)

/* AVR32 */
#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                    \
asm(                                \
    " ld.w   r2,%1            \n\t" \
    " add    r2,%0            \n\t" \
    " eor    r3,r3            \n\t" \
    " acr    r3               \n\t" \
    " macu.d r2,%3,%4         \n\t" \
    " st.w   %1,r2            \n\t" \
    " mov    %0,r3            \n\t" \
:"=r"(cy),"=r"(_c):"0"(cy),"r"(mu),"r"(*tmpm++),"1"(_c):"r2","r3");

#define PROPCARRY                    \
asm(                                 \
   " ld.w     r2,%1         \n\t"    \
   " add      r2,%0         \n\t"    \
   " st.w     %1,r2         \n\t"    \
   " eor      %0,%0         \n\t"    \
   " acr      %0            \n\t"    \
:"=r"(cy),"=r"(&_c[0]):"0"(cy),"1"(&_c[0]):"r2","%cc");

/******************************************************************/
#elif defined(TFM_MIPS)

/* MIPS */
#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                     \
asm(                                 \
   " multu    %3,%4          \n\t"   \
   " mflo     $12            \n\t"   \
   " mfhi     $13            \n\t"   \
   " addu     $12,$12,%0     \n\t"   \
   " sltu     $10,$12,%0     \n\t"   \
   " addu     $13,$13,$10    \n\t"   \
   " lw       $10,%1         \n\t"   \
   " addu     $12,$12,$10    \n\t"   \
   " sltu     $10,$12,$10    \n\t"   \
   " addu     %0,$13,$10     \n\t"   \
   " sw       $12,%1         \n\t"   \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(tmpm[0]),"1"(_c[0]):"$10","$12","$13"); ++tmpm;

#define PROPCARRY                    \
asm(                                 \
   " lw       $10,%1        \n\t"    \
   " addu     $10,$10,%0    \n\t"    \
   " sw       $10,%1        \n\t"    \
   " sltu     %0,$10,%0     \n\t"    \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"$10");

/******************************************************************/
#else

/* ISO C code */
#define MONT_START 
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                      \
   do { fp_word t;                                    \
   _c[0] = t  = ((fp_word)_c[0] + (fp_word)cy) +      \
                (((fp_word)mu) * ((fp_word)*tmpm++)); \
   cy = (t >> DIGIT_BIT);                             \
   } while (0)

#define PROPCARRY \
   do { fp_digit t = _c[0] += cy; cy = (t < cy); } while (0)

#endif
/******************************************************************/
template <int used>
static int fp_less_fixed(fp_int *a, fp_int *b){
  int x,lt,gt;
  lt = 0;
  gt = 0;
  for(x=0;x<used;x++){
    asm("cmp  %3, %2;"
        "cmovb %4, %0;"
        "cmova %4, %1;"
        : "=r"(lt), "=r"(gt)
        : "r"(a->dp[x]), "r"(b->dp[x]), "r"(x+1), "0"(lt), "1"(gt)
        :
        );
  }
  asm("cmp %1,%0;"
      "cmovna %2, %0;"
      : "=r"(lt)
      : "r"(gt) , "r"(0), "0"(lt)
      :);
  return lt;
}
#define fp_notless_u(a,b,used)                                    \
  int i=0,lt=0,gt=0;                                                \
  for(i=0;i<used;i++){                                            \
  asm("cmp  %3, %2;"                                              \
  "cmovb %4, %0;"                                                 \
  "cmova %4, %1;"                                                 \
  : "=r"(lt), "=r"(gt)                                            \
      : "r"((a)->dp[i]), "r"((b)->dp[i]), "r"(i+1), "0"(lt), "1"(gt)  \
      :                                                           \
      );                                                          \
  }

#define fp_notless(a,b) fp_notless_u(a,b,used)
asm (".macro m_notlessmove a,b, tmp, tmptwo, iter,to ;\n"
     "movq 8*\\iter(\\a ), \\tmp;\n"
     "movq 8*\\iter(\\b ), \\tmptwo;\n"
     "cmovna \\tmp, \\tmptwo;\n"
     "movq \\tmptwo, 8*\\iter(\\b);\n"
     ".if \\to-\\iter \n"
     "m_notlessmove \\a,\\b, \\tmp,\\tmptwo, \"(\\iter + 1)\", \\to \n"
     ".endif;\n .endm\n");
template <int used> 
static int fp_notless_move(fp_int *a, fp_int *b, fp_int *x, fp_int *y);

#define fp_notlessmove_impl(x,y,size)    {                              \
  register fp_digit tmp,tmp2;                                           \
  asm(" cmp %3,%2; \n"                                                  \
  "m_notlessmove %1,%0,%4,%5,0," #size    ";"                           \
      :                                                                 \
      : "r"((x)->dp), "r"((y)->dp), "r"((unsigned long)lt),             \
        "r"((unsigned long)gt) ,"r"(tmp),"r"(tmp2)                      \
      :  "memory");                                                     \
  }
  //if(fp_cmp_mag(a,b) != FP_LT){
  //    assert(fp_cmp_mag(x,y) == FP_EQ);
  //  }
template <> 
int fp_notless_move<17>(fp_int *a, fp_int *b, fp_int *x, fp_int *y)
{
  fp_notless_u(a,b,17);
  fp_notlessmove_impl(x,y,17);
}
template <> 
int fp_notless_move<34>(fp_int *a, fp_int *b, fp_int *x, fp_int *y)
{
  fp_notless_u(a,b,34);
  fp_notlessmove_impl(x,y,34);
}
template <int used> 
static int fp_notless_setlsb(fp_int *a, fp_int *b, fp_int *x){ 
  fp_notless(a,b);
  fp_digit dummy;
  asm(" mov %0,%1;"  
      " inc %1;"
      " cmp %3,%2;" // lt <= gt 
      "cmovna %1,%0       ;"
      : "+r"(x->dp[0]), "=&r"(dummy)
      : "r"((unsigned long)lt),"r"((unsigned long)gt) 
      : );
}

inline int fp_print(fp_int *test){
        char buf[2048];
        #ifndef NDEBUG
        //test->used = 32;
        fp_toradix(test,buf,16);
        printf("%s\n",buf);
        #else
        printf("fp_print disabled in debug\n");
        #endif
        return 0;
}
#ifndef NDEBUG
#define DBG(x) x;
#define DBG(X) ;
#else

#define DBG(x) ;
#endif
extern "C" void c_debug_print(char label,fp_int *a,int isbig){
  fp_int temp;
  if(isbig){
    memcpy(temp.dp,a,16*sizeof(fp_digit));
    temp.used = 17;
    printf("%c =",label);
    fp_print(&temp);
  } else {
    printf("%c = %lX\n",label,(fp_digit) a);
  }
}

void fp_mulmont2(fp_int *a, fp_int *b, fp_int *m,fp_int *c, fp_digit mp){
  fp_int real;
  fp_int temp,d,e;
  fp_digit q;
  int i,j;

  memset(&d,0,sizeof d);
  memset(&e,0,sizeof e);
  //  memset(&temp,0,sizeof temp);
  mp = -mp;
  //  fp_mul_d(m,mp,&temp);
  
  DBG(printf("a=0x"); fp_print(a));
  DBG( printf("b=0x"); fp_print(b));
  // printf("m=0x"); fp_print(m);
  //printf("multiply_out=0x"); fp_print(&temp);
  //   printf("mp=0x%lX\n",mp);
  
  const int pa = 16;
  d.used = e.used = pa;
  for (j=0;j<pa;j++){
    fp_word t0,t1;
    q = mp * b->dp[0] * a->dp[j] + mp*(d.dp[0] - e.dp[0]);
    DBG(printf("q = %lu\n", q));
    DBG(printf("aj = %lu\n", a->dp[j]));
    DBG(printf("b0 = %lu\n", b->dp[0]));
    DBG(printf("d0 = %lu\n", d.dp[0]));
    t0 = (fp_word)a->dp[j]*(fp_word) b->dp[0] + (fp_word)d.dp[0];
    //    printf("t0= %lX_%lX\n", t0>>64, t0);
    t0>>=64;
    t1 = (fp_word)q * m->dp[0] + e.dp[0];
    t1>>=64;
    for(i=1;i<pa;i++){
      fp_word p0,p1;
      p0 = ((fp_word)a->dp[j]) * b->dp[i] + t0 + d.dp[i];
      p1 = (fp_word)q * m->dp[i] + t1 + e.dp[i];
      t0 = p0 >> 64;
      t1 = p1 >> 64;
      d.dp[i-1] = p0;
      e.dp[i-1] = p1 ;
    }
    d.dp[pa-1] = t0;
    e.dp[pa-1] = t1;
    
    //printf("d ="); fp_print(&d);
    //printf("e ="); fp_print(&e);
  }
  {
    fp_notless_u(&e,&d,16);
    s_fp_sub_fixed2<16>(&d,&e,c);
    s_fp_sub_fixed2<16>(m, &e,&temp);
     s_fp_add_fixed<16>(&temp, &d,&real);
    fp_notlessmove_impl(c,&real,16);
  }
   
  //  printf("real=");fp_print(&real);
  //  printf("got =");fp_print(c);
  
}
extern "C" void mulmont_asm(fp_digit *a, fp_digit *b, fp_digit *m, fp_digit mp, fp_digit *d, fp_digit *e);
void fp_mulmont(fp_int *a, fp_int *b, fp_int *m,fp_int *c, fp_digit mp){
  fp_int real;
  fp_int temp,d,e;
  fp_digit q;
  int i,j;

  memset(&d,0,sizeof d);
  memset(&e,0,sizeof e);
  mp = -mp;
  
  DBG(printf("a=0x"); fp_print(a));
  DBG( printf("b=0x"); fp_print(b));
  // printf("m=0x"); fp_print(m);
  //printf("multiply_out=0x"); fp_print(&temp);
  //   printf("mp=0x%lX\n",mp);
  mulmont_asm(a->dp,b->dp,m->dp,mp, d.dp,e.dp);
  {
    fp_notless_u(&e,&d,16);
    s_fp_sub_fixed2<16>(&d,&e,c);
    s_fp_sub_fixed2<16>(m, &e,&temp);
     s_fp_add_fixed<16>(&temp, &d,&real);
    fp_notlessmove_impl(c,&real,16);
  }
   
  //  printf("real=");fp_print(&real);
  //  printf("got =");fp_print(c);
  
}
void fp_montreduce_fixed(fp_int *a, fp_int *m, fp_digit mp){
  fp_int temp;
  memset(&temp,0,sizeof temp);
  temp.used = 16;
  temp.dp[0]= 1;
  fp_mulmont(a,&temp,m,a,mp);
}
/* computes x/R == x (mod N) via Montgomery Reduction */
void fp_montgomery_reduce(fp_int *a, fp_int *m, fp_digit mp)
{
  fp_int test;
   fp_digit c[FP_SIZE], *_c, *tmpm, mu;
   int       x, y;
   
   assert(m->used == 16);
   assert(a->used <= 32);
   const int aused = 32;
   const int pa = 16;
   /* bail if too large */
   if (pa > (FP_SIZE/2)) {
      return;
   }

#if defined(USE_MEMSET)
   /* now zero the buff */
   memset(c, 0, sizeof c);
#endif

   /* copy the input */

   for (x = 0; x < aused; x++) {
       c[x] = a->dp[x];
   }
#if !defined(USE_MEMSET)
   for (; x < 2*pa+1; x++) {
       c[x] = 0;
   }
#endif
   MONT_START;

   for (x = 0; x < pa; x++) {
       fp_digit cy = 0;
       /* get Mu for this round */
       LOOP_START;
       _c   = c + x;
       tmpm = m->dp;
       y = 0;
       #if (defined(TFM_SSE2) || defined(TFM_X86_64))
        for (; y < (pa & ~7); y += 8) {
              INNERMUL8;
              _c   += 8;
              tmpm += 8;
           }
       #endif

       for (; y < pa; y++) {
          INNERMUL;
          ++_c;
       }
       LOOP_END;
       while (cy) {
           PROPCARRY;
           ++_c;
       }
  }         

  /* now copy out */
  _c   = c + pa;
  tmpm = a->dp;
  for (x = 0; x < pa+1; x++) {
     *tmpm++ = *_c++;
  }

  for (; x < aused; x++)   {
     *tmpm++ = 0;
  }

  MONT_FINI;

  //  aused = pa+1;
  /* if A >= m then A = A - m */
  //  fp_clamp(a);
  //  if((fp_less_fixed<17>(a,m) != 0) != (fp_cmp_mag(a,m) == FP_LT)){
  //     fp_print(a);
  //     fp_print(m);
  //   }
  s_fp_sub_fixed<17> (a,m, &test);
  fp_notless_move<17>(a,m,a,&test);
  // if (!fp_less_fixed<17> (a, m)) {
  //     s_fp_sub_fixed<17> (a, m, a);
  //  }
  a->used = pa;
  //  aused = pa;
}
