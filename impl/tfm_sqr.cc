/*
 * 
 * This project is meant to fill in where LibTomMath
 * falls short.  That is speed ;-)
 *
 * This project is public domain and free for all purposes.
 * 
 * Tom St Denis, tomstdenis@gmail.com
 */
#include <tfm.h>

#if defined(TFM_PRESCOTT) && defined(TFM_SSE2)
   #undef TFM_SSE2
   #define TFM_X86
#endif

#if defined(TFM_X86)

/* x86-32 optimized */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

#define SQRADD(i, j)                                      \
asm(                                            \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %%eax        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i) :"%eax","%edx","%cc");

#define SQRADD2(i, j)                                     \
asm(                                            \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %7           \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)  :"%eax","%edx","%cc");

#define SQRADDSC(i, j)                                    \
asm(                                                     \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %7           \n\t"                            \
     "movl  %%eax,%0     \n\t"                            \
     "movl  %%edx,%1     \n\t"                            \
     "xorl  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) :"%eax","%edx","%cc");

#define SQRADDAC(i, j)                                    \
asm(                                                     \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %7           \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) :"%eax","%edx","%cc");

#define SQRADDDB                                          \
asm(                                                     \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) : "%cc");

#elif defined(TFM_X86_64)
/* x86-64 optimized */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

#define SQRADD(i, j)                                      \
asm(                                                     \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %%rax        \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "g"(i) :"%rax","%rdx","%cc");

#define SQRADD2(i, j)                                     \
asm(                                                     \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)  :"%rax","%rdx","%cc");

#define SQRADDSC(i, j)                                    \
asm(                                                     \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "movq  %%rax,%0     \n\t"                            \
     "movq  %%rdx,%1     \n\t"                            \
     "xorq  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) :"%rax","%rdx","%cc");

#define SQRADDAC(i, j)                                                         \
asm(                                                     \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) :"%rax","%rdx","%cc");

#define SQRADDDB                                          \
asm(                                                     \
     "addq %6,%0         \n\t"                            \
     "adcq %7,%1         \n\t"                            \
     "adcq %8,%2         \n\t"                            \
     "addq %6,%0         \n\t"                            \
     "adcq %7,%1         \n\t"                            \
     "adcq %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) : "%cc");

#elif defined(TFM_SSE2)

/* SSE2 Optimized */
#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI \
   asm("emms");

#define SQRADD(i, j)                                      \
asm(                                            \
     "movd  %6,%%mm0     \n\t"                            \
     "pmuludq %%mm0,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "adcl  %%eax,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i) :"%eax","%cc");

#define SQRADD2(i, j)                                     \
asm(                                            \
     "movd  %6,%%mm0     \n\t"                            \
     "movd  %7,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%%edx  \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)  :"%eax","%edx","%cc");

#define SQRADDSC(i, j)                                                         \
asm(                                            \
     "movd  %6,%%mm0     \n\t"                            \
     "movd  %7,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%0     \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%1     \n\t"                            \
     "xorl  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "m"(i), "m"(j));

#define SQRADDAC(i, j)                                                         \
asm(                                            \
     "movd  %6,%%mm0     \n\t"                            \
     "movd  %7,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%%edx  \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "m"(i), "m"(j)  :"%eax","%edx","%cc");

#define SQRADDDB                                          \
asm(                                                     \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) : "%cc");

#elif defined(TFM_ARM)

/* ARM code */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)                                             \
asm(                                                             \
"  UMULL  r0,r1,%6,%6              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(i) : "r0", "r1", "%cc");
	
/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)                                            \
asm(                                                             \
"  UMULL  r0,r1,%6,%7              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j) : "r0", "r1", "%cc");

#define SQRADDSC(i, j)                                           \
asm(                                                             \
"  UMULL  %0,%1,%6,%7              \n\t"                         \
"  SUB    %2,%2,%2                 \n\t"                         \
:"=r"(sc0), "=r"(sc1), "=r"(sc2) : "0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j) : "%cc");

#define SQRADDAC(i, j)                                           \
asm(                                                             \
"  UMULL  r0,r1,%6,%7              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(sc0), "=r"(sc1), "=r"(sc2) : "0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j) : "r0", "r1", "%cc");

#define SQRADDDB                                                 \
asm(                                                             \
"  ADDS  %0,%0,%3                     \n\t"                      \
"  ADCS  %1,%1,%4                     \n\t"                      \
"  ADC   %2,%2,%5                     \n\t"                      \
"  ADDS  %0,%0,%3                     \n\t"                      \
"  ADCS  %1,%1,%4                     \n\t"                      \
"  ADC   %2,%2,%5                     \n\t"                      \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "%cc");

#elif defined(TFM_PPC32)

/* PPC32 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)             \
asm(                             \
   " mullw  16,%6,%6       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " mulhwu 16,%6,%6       \n\t" \
   " adde   %1,%1,16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"16","%cc");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
asm(                             \
   " mullw  16,%6,%7       \n\t" \
   " mulhwu 17,%6,%7       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " adde   %1,%1,17       \n\t" \
   " addze  %2,%2          \n\t" \
   " addc   %0,%0,16       \n\t" \
   " adde   %1,%1,17       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"16", "17","%cc");

#define SQRADDSC(i, j)            \
asm(                              \
   " mullw  %0,%6,%7        \n\t" \
   " mulhwu %1,%6,%7        \n\t" \
   " xor    %2,%2,%2        \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "%cc");

#define SQRADDAC(i, j)           \
asm(                             \
   " mullw  16,%6,%7       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " mulhwu 16,%6,%7       \n\t" \
   " adde   %1,%1,16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"16", "%cc");

#define SQRADDDB                  \
asm(                              \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "%cc");

#elif defined(TFM_PPC64)
/* PPC64 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)             \
asm(                             \
   " mulld  r16,%6,%6       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " mulhdu r16,%6,%6       \n\t" \
   " adde   %1,%1,r16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"r16","%cc");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
asm(                             \
   " mulld  r16,%6,%7       \n\t" \
   " mulhdu r17,%6,%7       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " adde   %1,%1,r17       \n\t" \
   " addze  %2,%2          \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " adde   %1,%1,r17       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r16", "r17","%cc");

#define SQRADDSC(i, j)            \
asm(                              \
   " mulld  %0,%6,%7        \n\t" \
   " mulhdu %1,%6,%7        \n\t" \
   " xor    %2,%2,%2        \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "%cc");

#define SQRADDAC(i, j)           \
asm(                             \
   " mulld  r16,%6,%7       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " mulhdu r16,%6,%7       \n\t" \
   " adde   %1,%1,r16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"r16", "%cc");

#define SQRADDDB                  \
asm(                              \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "%cc");


#elif defined(TFM_AVR32)

/* AVR32 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)             \
asm(                             \
   " mulu.d r2,%6,%6       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2             \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"r2","r3");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
asm(                             \
   " mulu.d r2,%6,%7       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2,            \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2,            \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r2", "r3");

#define SQRADDSC(i, j)            \
asm(                              \
   " mulu.d r2,%6,%7        \n\t" \
   " mov    %0,r2           \n\t" \
   " mov    %1,r3           \n\t" \
   " eor    %2,%2           \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "r2", "r3");

#define SQRADDAC(i, j)           \
asm(                             \
   " mulu.d r2,%6,%7       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2             \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"r2", "r3");

#define SQRADDDB                  \
asm(                              \
   " add    %0,%0,%3        \n\t" \
   " adc    %1,%1,%4        \n\t" \
   " adc    %2,%2,%5        \n\t" \
   " add    %0,%0,%3        \n\t" \
   " adc    %1,%1,%4        \n\t" \
   " adc    %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "%cc");

#elif defined(TFM_MIPS)

/* MIPS */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)              \
asm(                              \
   " multu  %6,%6          \n\t"  \
   " mflo   $12            \n\t"  \
   " mfhi   $13            \n\t"  \
   " addu    %0,%0,$12     \n\t"  \
   " sltu   $12,%0,$12     \n\t"  \
   " addu    %1,%1,$13     \n\t"  \
   " sltu   $13,%1,$13     \n\t"  \
   " addu    %1,%1,$12     \n\t"  \
   " sltu   $12,%1,$12     \n\t"  \
   " addu    %2,%2,$13     \n\t"  \
   " addu    %2,%2,$12     \n\t"  \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"$12","$13");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
asm(                             \
   " multu  %6,%7          \n\t" \
   " mflo   $12            \n\t" \
   " mfhi   $13            \n\t" \
                                 \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $14,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $15,%1,$13     \n\t" \
   " addu    %1,%1,$14     \n\t" \
   " sltu   $14,%1,$14     \n\t" \
   " addu    %2,%2,$15     \n\t" \
   " addu    %2,%2,$14     \n\t" \
                                 \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $14,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $15,%1,$13     \n\t" \
   " addu    %1,%1,$14     \n\t" \
   " sltu   $14,%1,$14     \n\t" \
   " addu    %2,%2,$15     \n\t" \
   " addu    %2,%2,$14     \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"$12", "$13", "$14", "$15");

#define SQRADDSC(i, j)            \
asm(                              \
   " multu  %6,%7          \n\t"  \
   " mflo   %0             \n\t"  \
   " mfhi   %1             \n\t"  \
   " xor    %2,%2,%2       \n\t"  \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "%cc");

#define SQRADDAC(i, j)           \
asm(                             \
   " multu  %6,%7          \n\t" \
   " mflo   $12            \n\t" \
   " mfhi   $13            \n\t" \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $12,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $13,%1,$13     \n\t" \
   " addu    %1,%1,$12     \n\t" \
   " sltu   $12,%1,$12     \n\t" \
   " addu    %2,%2,$13     \n\t" \
   " addu    %2,%2,$12     \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"$12", "$13", "$14");

#define SQRADDDB                  \
asm(                              \
   " addu    %0,%0,%3       \n\t" \
   " sltu   $10,%0,%3       \n\t" \
   " addu    %1,%1,$10      \n\t" \
   " sltu   $10,%1,$10      \n\t" \
   " addu    %1,%1,%4       \n\t" \
   " sltu   $11,%1,%4       \n\t" \
   " addu    %2,%2,$10      \n\t" \
   " addu    %2,%2,$11      \n\t" \
   " addu    %2,%2,%5       \n\t" \
                                  \
   " addu    %0,%0,%3       \n\t" \
   " sltu   $10,%0,%3       \n\t" \
   " addu    %1,%1,$10      \n\t" \
   " sltu   $10,%1,$10      \n\t" \
   " addu    %1,%1,%4       \n\t" \
   " sltu   $11,%1,%4       \n\t" \
   " addu    %2,%2,$10      \n\t" \
   " addu    %2,%2,$11      \n\t" \
   " addu    %2,%2,%5       \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "$10", "$11");

#else

#define TFM_ISO

/* ISO C portable code */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)                                 \
   do { fp_word t;                                   \
   t = c0 + ((fp_word)i) * ((fp_word)j);  c0 = t;    \
   t = c1 + (t >> DIGIT_BIT);             c1 = t; c2 += t >> DIGIT_BIT; \
   } while (0);
  

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)                                                 \
   do { fp_word t;                                                    \
   t  = ((fp_word)i) * ((fp_word)j);                                  \
   tt = (fp_word)c0 + t;                 c0 = tt;                              \
   tt = (fp_word)c1 + (tt >> DIGIT_BIT); c1 = tt; c2 += tt >> DIGIT_BIT;       \
   tt = (fp_word)c0 + t;                 c0 = tt;                              \
   tt = (fp_word)c1 + (tt >> DIGIT_BIT); c1 = tt; c2 += tt >> DIGIT_BIT;       \
   } while (0);

#define SQRADDSC(i, j)                                                         \
   do { fp_word t;                                                             \
      t =  ((fp_word)i) * ((fp_word)j);                                        \
      sc0 = (fp_digit)t; sc1 = (t >> DIGIT_BIT); sc2 = 0;                      \
   } while (0);

#define SQRADDAC(i, j)                                                         \
   do { fp_word t;                                                             \
   t = sc0 + ((fp_word)i) * ((fp_word)j);  sc0 = t;                            \
   t = sc1 + (t >> DIGIT_BIT);             sc1 = t; sc2 += t >> DIGIT_BIT;     \
   } while (0);

#define SQRADDDB                                                               \
   do { fp_word t;                                                             \
   t = ((fp_word)sc0) + ((fp_word)sc0) + c0; c0 = t;                                                 \
   t = ((fp_word)sc1) + ((fp_word)sc1) + c1 + (t >> DIGIT_BIT); c1 = t;                              \
   c2 = c2 + ((fp_word)sc2) + ((fp_word)sc2) + (t >> DIGIT_BIT);                                     \
   } while (0);

#endif


void fp_sqr_comba_small16(fp_int *A, fp_int *B)
{
   fp_digit *a, b[32], c0, c1, c2, sc0, sc1, sc2;
#ifdef TFM_ISO
   fp_word   tt;   
#endif   
   assert(A->used == 16);
      a = A->dp;
      COMBA_START; 

      /* clear carries */
      CLEAR_CARRY;

      /* output 0 */
      SQRADD(a[0],a[0]);
      COMBA_STORE(b[0]);

      /* output 1 */
      CARRY_FORWARD;
      SQRADD2(a[0], a[1]); 
      COMBA_STORE(b[1]);

      /* output 2 */
      CARRY_FORWARD;
      SQRADD2(a[0], a[2]);    SQRADD(a[1], a[1]); 
      COMBA_STORE(b[2]);

      /* output 3 */
      CARRY_FORWARD;
      SQRADD2(a[0], a[3]);    SQRADD2(a[1], a[2]); 
      COMBA_STORE(b[3]);

      /* output 4 */
      CARRY_FORWARD;
      SQRADD2(a[0], a[4]);    SQRADD2(a[1], a[3]);    SQRADD(a[2], a[2]); 
      COMBA_STORE(b[4]);

      /* output 5 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[5]); SQRADDAC(a[1], a[4]); SQRADDAC(a[2], a[3]); SQRADDDB; 
      COMBA_STORE(b[5]);

      /* output 6 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[6]); SQRADDAC(a[1], a[5]); SQRADDAC(a[2], a[4]); SQRADDDB; SQRADD(a[3], a[3]); 
      COMBA_STORE(b[6]);

      /* output 7 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[7]); SQRADDAC(a[1], a[6]); SQRADDAC(a[2], a[5]); SQRADDAC(a[3], a[4]); SQRADDDB; 
      COMBA_STORE(b[7]);

      /* output 8 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[8]); SQRADDAC(a[1], a[7]); SQRADDAC(a[2], a[6]); SQRADDAC(a[3], a[5]); SQRADDDB; SQRADD(a[4], a[4]); 
      COMBA_STORE(b[8]);

      /* output 9 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[9]); SQRADDAC(a[1], a[8]); SQRADDAC(a[2], a[7]); SQRADDAC(a[3], a[6]); SQRADDAC(a[4], a[5]); SQRADDDB; 
      COMBA_STORE(b[9]);

      /* output 10 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[10]); SQRADDAC(a[1], a[9]); SQRADDAC(a[2], a[8]); SQRADDAC(a[3], a[7]); SQRADDAC(a[4], a[6]); SQRADDDB; SQRADD(a[5], a[5]); 
      COMBA_STORE(b[10]);

      /* output 11 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[11]); SQRADDAC(a[1], a[10]); SQRADDAC(a[2], a[9]); SQRADDAC(a[3], a[8]); SQRADDAC(a[4], a[7]); SQRADDAC(a[5], a[6]); SQRADDDB; 
      COMBA_STORE(b[11]);

      /* output 12 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[12]); SQRADDAC(a[1], a[11]); SQRADDAC(a[2], a[10]); SQRADDAC(a[3], a[9]); SQRADDAC(a[4], a[8]); SQRADDAC(a[5], a[7]); SQRADDDB; SQRADD(a[6], a[6]); 
      COMBA_STORE(b[12]);

      /* output 13 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[13]); SQRADDAC(a[1], a[12]); SQRADDAC(a[2], a[11]); SQRADDAC(a[3], a[10]); SQRADDAC(a[4], a[9]); SQRADDAC(a[5], a[8]); SQRADDAC(a[6], a[7]); SQRADDDB; 
      COMBA_STORE(b[13]);

      /* output 14 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[14]); SQRADDAC(a[1], a[13]); SQRADDAC(a[2], a[12]); SQRADDAC(a[3], a[11]); SQRADDAC(a[4], a[10]); SQRADDAC(a[5], a[9]); SQRADDAC(a[6], a[8]); SQRADDDB; SQRADD(a[7], a[7]); 
      COMBA_STORE(b[14]);

      /* output 15 */
      CARRY_FORWARD;
   SQRADDSC(a[0], a[15]); SQRADDAC(a[1], a[14]); SQRADDAC(a[2], a[13]); SQRADDAC(a[3], a[12]); SQRADDAC(a[4], a[11]); SQRADDAC(a[5], a[10]); SQRADDAC(a[6], a[9]); SQRADDAC(a[7], a[8]); SQRADDDB; 
      COMBA_STORE(b[15]);

      /* output 16 */
      CARRY_FORWARD;
   SQRADDSC(a[1], a[15]); SQRADDAC(a[2], a[14]); SQRADDAC(a[3], a[13]); SQRADDAC(a[4], a[12]); SQRADDAC(a[5], a[11]); SQRADDAC(a[6], a[10]); SQRADDAC(a[7], a[9]); SQRADDDB; SQRADD(a[8], a[8]); 
      COMBA_STORE(b[16]);

      /* output 17 */
      CARRY_FORWARD;
   SQRADDSC(a[2], a[15]); SQRADDAC(a[3], a[14]); SQRADDAC(a[4], a[13]); SQRADDAC(a[5], a[12]); SQRADDAC(a[6], a[11]); SQRADDAC(a[7], a[10]); SQRADDAC(a[8], a[9]); SQRADDDB; 
      COMBA_STORE(b[17]);

      /* output 18 */
      CARRY_FORWARD;
   SQRADDSC(a[3], a[15]); SQRADDAC(a[4], a[14]); SQRADDAC(a[5], a[13]); SQRADDAC(a[6], a[12]); SQRADDAC(a[7], a[11]); SQRADDAC(a[8], a[10]); SQRADDDB; SQRADD(a[9], a[9]); 
      COMBA_STORE(b[18]);

      /* output 19 */
      CARRY_FORWARD;
   SQRADDSC(a[4], a[15]); SQRADDAC(a[5], a[14]); SQRADDAC(a[6], a[13]); SQRADDAC(a[7], a[12]); SQRADDAC(a[8], a[11]); SQRADDAC(a[9], a[10]); SQRADDDB; 
      COMBA_STORE(b[19]);

      /* output 20 */
      CARRY_FORWARD;
   SQRADDSC(a[5], a[15]); SQRADDAC(a[6], a[14]); SQRADDAC(a[7], a[13]); SQRADDAC(a[8], a[12]); SQRADDAC(a[9], a[11]); SQRADDDB; SQRADD(a[10], a[10]); 
      COMBA_STORE(b[20]);

      /* output 21 */
      CARRY_FORWARD;
   SQRADDSC(a[6], a[15]); SQRADDAC(a[7], a[14]); SQRADDAC(a[8], a[13]); SQRADDAC(a[9], a[12]); SQRADDAC(a[10], a[11]); SQRADDDB; 
      COMBA_STORE(b[21]);

      /* output 22 */
      CARRY_FORWARD;
   SQRADDSC(a[7], a[15]); SQRADDAC(a[8], a[14]); SQRADDAC(a[9], a[13]); SQRADDAC(a[10], a[12]); SQRADDDB; SQRADD(a[11], a[11]); 
      COMBA_STORE(b[22]);

      /* output 23 */
      CARRY_FORWARD;
   SQRADDSC(a[8], a[15]); SQRADDAC(a[9], a[14]); SQRADDAC(a[10], a[13]); SQRADDAC(a[11], a[12]); SQRADDDB; 
      COMBA_STORE(b[23]);

      /* output 24 */
      CARRY_FORWARD;
   SQRADDSC(a[9], a[15]); SQRADDAC(a[10], a[14]); SQRADDAC(a[11], a[13]); SQRADDDB; SQRADD(a[12], a[12]); 
      COMBA_STORE(b[24]);

      /* output 25 */
      CARRY_FORWARD;
   SQRADDSC(a[10], a[15]); SQRADDAC(a[11], a[14]); SQRADDAC(a[12], a[13]); SQRADDDB; 
      COMBA_STORE(b[25]);

      /* output 26 */
      CARRY_FORWARD;
      SQRADD2(a[11], a[15]);    SQRADD2(a[12], a[14]);    SQRADD(a[13], a[13]); 
      COMBA_STORE(b[26]);

      /* output 27 */
      CARRY_FORWARD;
      SQRADD2(a[12], a[15]);    SQRADD2(a[13], a[14]); 
      COMBA_STORE(b[27]);

      /* output 28 */
      CARRY_FORWARD;
      SQRADD2(a[13], a[15]);    SQRADD(a[14], a[14]); 
      COMBA_STORE(b[28]);

      /* output 29 */
      CARRY_FORWARD;
      SQRADD2(a[14], a[15]); 
      COMBA_STORE(b[29]);

      /* output 30 */
      CARRY_FORWARD;
      SQRADD(a[15], a[15]); 
      COMBA_STORE(b[30]);
      COMBA_STORE2(b[31]);
      COMBA_FINI;

      B->used = 32;
      B->sign = FP_ZPOS;
      memcpy(B->dp, b, 32 * sizeof(fp_digit));
      // fp_clamp(B);

}
