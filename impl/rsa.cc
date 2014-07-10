#include "tfm.h"
#include <time.h>
#include <openssl/bn.h>
#include <cassert>
#define TABLE 4
#include "tfm.cc"
static void scatter(fp_digit table[], int choice, fp_int *foo,int width){
        int i = 0;
        for(i=0;i<width;i++)
                table[(1<<TABLE) * i + choice] = foo->dp[i];
}
static void gather(fp_digit table[], int choice, fp_int *foo, int width){
        int i = 0;
        for(i=0;i<width;i++)
                foo->dp[i]= table[(1<<TABLE) * i + choice];
        foo->used = width;
        foo->sign = 0;
}
//TODO:  template for width
template <int used>
int table_sc_exp(fp_int *a, fp_int *b,fp_int *m, fp_int *res){
        fp_digit table[FP_SIZE * (1<<TABLE)];   
        fp_int temp, temp1;
        fp_digit buf,mp,y;
        int i,digidx,err, bitcnt;
        int width = m->used;
        int montgomery = 0;
        assert(used == m->used);
        fp_montgomery_setup (m, &mp);
        /* now we need R mod m */
        fp_montgomery_calc_normalization (&temp, m); //Set used to 16 everywhere!
        /* now set R[0][1] to G * R mod m */
        fp_mod(a, m, &temp1);
        fp_mul_comba_16(&temp1,&temp,&temp1);
        fp_mod(&temp1,m,&temp1);

        //Table[0] = Mont(1) = temp
        //Table[1] = Mont(a) = temp1
        scatter(table,0, &temp, width);
        fp_init_copy(&temp, &temp1);
        scatter(table,1, &temp1,width);

        for(i=2;i<(1<<TABLE); i++){
                fp_mul_comba_16(&temp,&temp1,&temp);
                fp_montgomery_reduce(&temp, m, mp);
                scatter(table,i,&temp,width);
        }
        digidx= b->used-1;
        buf=b->dp[digidx--];
        bitcnt = (int)DIGIT_BIT;

        y = (buf >> (DIGIT_BIT-TABLE)) & ((1<<TABLE) -1);
        buf<<=(fp_digit)TABLE;
        gather(table,y,res,width);
        for(;;){
          bitcnt -= TABLE;
          if(bitcnt==0 ){
            if(digidx == -1){
              break;
            }
            buf = b->dp[digidx--];
            bitcnt = (int) DIGIT_BIT;
          }  
          y = (fp_digit) (buf >> (DIGIT_BIT - TABLE)) & ((1<<TABLE) -  1);
          buf <<= (fp_digit)TABLE;
          for(i=0;i<TABLE;i++){
            fp_sqr_comba_small16(res,res);            
            montgomery++;
            fp_montgomery_reduce(res,m, mp);
            assert(res->used == 16);
          }
          gather(table,y,&temp,width);
          fp_mul_comba_16(res, &temp, res );
          montgomery++;
          fp_montgomery_reduce(res,m, mp); 
        }
        fp_montgomery_reduce(res,m,mp);
        return 0;
}
int rsa_crt(fp_int *c, fp_int *p, fp_int *q, fp_int *d_p, fp_int *d_q, fp_int *q_inv, fp_int *m){
        fp_int m1,m2;
        table_sc_exp<16>(c,d_p,p,&m1);
        table_sc_exp<16>(c,d_q,q,&m2);
        fp_sub(&m1,&m2,&m1);
        fp_mul_comba_16(q_inv,&m1,&m1);
        fp_mod(&m1,p,&m1);
        fp_mul_comba_16(q,&m1,&m1);
        fp_add(&m1,&m2,m);
        
}
/* $Source: /cvs/libtom/tomsfastmath/demo/rsa.c,v $ */
/* $Revision: 1.2 $ */
/* $Date: 2005/05/05 14:39:33 $ */
