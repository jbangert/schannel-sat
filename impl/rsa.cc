#include "tfm.h"
#include <time.h>
#include <openssl/bn.h>
#include <cassert>
#define TABLE 4
#include "tfm.cc"
template<int width>
static void scatter(fp_digit table[], int choice, fp_int *foo){
        int i = 0;
        for(i=0;i<width;i++)
                table[(1<<TABLE) * i + choice] = foo->dp[i];
}
template <int width> 
static void gather(fp_digit table[], int choice, fp_int *foo){
        int i = 0;
        for(i=0;i<width;i++)
                foo->dp[i]= table[(1<<TABLE) * i + choice];
        foo->used = width;
        foo->sign = 0;
}
int fixed_mont_setup(fp_int *a, fp_digit *rho)
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
  /*
  if ((b & 1) == 0) {
    return FP_VAL;
  }
  */
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
template <int used>
void fixed_montgomery_calc_normalization(fp_int *a, fp_int *b)
{
  int     x, bits;

  /* how many bits of last digit does b use */
  bits = fp_count_bits (b) % DIGIT_BIT;
  if (!bits) bits = DIGIT_BIT;

  /* compute A = B^(n-1) * 2^(bits-1) */

  fp_2expt (a, (b->used - 1) * DIGIT_BIT + bits - 1);


  /* now compute C = A * B mod b */
  for (x = bits - 1; x < (int)DIGIT_BIT; x++) {
    fp_mul_2 (a, a);
    if (fp_cmp_mag (a, b) != FP_LT) {
      s_fp_sub (a, b, a);
    }
  }
}
template <int used>
void fp_copy_fixed(fp_int *a, fp_int *b)
{
  int i;
  for(i=0;i<used;i++){
    a->dp[i] = b->dp[i];
  }
  a->used = used;
  a->sign = b->sign;
}
template <int used>
int table_sc_exp(fp_int *a, fp_int *b,fp_int *m, fp_int *m_mont, fp_int *res){
        fp_digit table[FP_SIZE * (1<<TABLE)] __attribute__ ((aligned (128)));   
        fp_int temp, temp1;

        fp_int foo_testing;
        fp_digit buf,mp,y;
        int i,digidx,err, bitcnt;
        int montgomery = 0;
        assert(used == m->used);
        /* now we need R mod m */
        fp_copy_fixed<used>(&temp,m_mont);
        //        fp_copy_fixed<used>(&temp1,m_mont);
        fixed_mont_setup(m, &mp);

        // fp_montgomery_calc_normalization (&temp, m); //Set used to 16 everywhere!
        /* now set R[0][1] to G * R mod m */
        //        fp_mod(a, m, &temp1);
        fp_mod_fixed<used+1>(a,m,&temp1);
        temp1.used = used;        
        temp.used = used;        
        fp_mul_comba_16(&temp1,&temp,&temp1);
        //fp_montgomery_reduce(&temp,m,mp);
        fp_mod_fixed<used+1>(&temp1,m,&temp1);

        temp1.used = used;
        //Table[0] = Mont(1) = temp
        //Table[1] = Mont(a) = temp1
        scatter<used>(table,0, &temp);
        scatter<used>(table,1, &temp1);
        fp_copy_fixed<used>(&temp, &temp1);

        for(i=2;i<(1<<TABLE); i++){
                fp_mul_comba_16(&temp,&temp1,&temp);
                fp_montgomery_reduce(&temp, m, mp);
                scatter<used>(table,i,&temp);
        }
        digidx= used;
        buf=b->dp[digidx--];
        bitcnt = (int)DIGIT_BIT;

        y = (buf >> (DIGIT_BIT-TABLE)) & ((1<<TABLE) -1);
        buf<<=(fp_digit)TABLE;
        gather<used>(table,y,res);
        for(;;){
          for(bitcnt=0;bitcnt < DIGIT_BIT;bitcnt += TABLE){
            y = (fp_digit) (buf >> (DIGIT_BIT - TABLE)) & ((1<<TABLE) -  1);
            buf <<= (fp_digit)TABLE;
            for(i=0;i<TABLE;i++){
              fp_sqr_comba_small16(res,res);    
              fp_montgomery_reduce(res,m, mp);
              assert(res->used == 16);
            }
            gather<used>(table,y,&temp);
            fp_mul_comba_16(res, &temp, res );
            fp_montgomery_reduce(res,m, mp); 
          }
          
          if(digidx == -1){
            break;
          }
          buf = b->dp[digidx--];
        }
        fp_montgomery_reduce(res,m,mp);
        return 0;
}


int rsa_crt(fp_int *c, fp_int *p, fp_int *p_mont, fp_int *q, fp_int *q_mont, fp_int *d_p, fp_int *d_q, fp_int *q_inv, fp_int *m){
  fp_int m1,m2,tmp;
  table_sc_exp<16>(c,d_p,p,p_mont,&m1);
  table_sc_exp<16>(c,d_q,q,q_mont,&m2);
  /* //M1 and M2 are unsigned. calc m1-m2 and m2-m1 unsigned and pick the right one.
  fp_notless_u(&m2,&m1,16);
  s_fp_sub_fixed<17>(&m2,&m1,&tmp);
  s_fp_sub_fixed<17>(&m1,&m2,&m1);
  fp_notless_move(&m2,&m1,&m1,&tmp);
  */
  
  fp_sub(&m1,&m2,&m1);
  fp_mul_comba_16(q_inv,&m1,&m1);
  //fp_mod(&m1,p,&m1);
  fp_clean<32>(&m1);
  fp_mod_fixed<17>(&m1,p,&m1);
  m1.used = 16;
  fp_mul_comba_16(q,&m1,&m1);
  /*if(gt>lt){
  }
  else{
  }*/
    
        fp_add(&m1,&m2,m);
        
}
/* $Source: /cvs/libtom/tomsfastmath/demo/rsa.c,v $ */
/* $Revision: 1.2 $ */
/* $Date: 2005/05/05 14:39:33 $ */
