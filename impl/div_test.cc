#include <assert.h>
#include <stdint.h>
#include "rsa.cc"
#include <stdlib.h>
void fp_rand(fp_int *out){
  int i;
  fp_zero(out);
  for(i=0;i<16;i++){
    out->dp[i] =  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
  }
  out->used = 16;
}
void fp_rand32(fp_int *out){
  int i;
  fp_zero(out);
  for(i=0;i<32;i++){
    out->dp[i] =  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
  }
  out->used = 32;
}
int main(){
  int i = 0;
  fp_int m,c,a1,a2,a3;
  srand(1234);
  for(;i<1000;i++){
    fp_rand32(&m);
    fp_rand(&c);
    fp_zero(&a1);
    fp_zero(&a2);
    fp_div(&m,&c,&a2,NULL);
    fp_div_fixed<32>(&m,&c,&a1,&a3);
    a2.used = 32;
    if(fp_cmp(&a1,&a2) != FP_EQ){
     printf("Different results for division\n");
     fprintf(stderr,"div("); fp_print_stderr(&m);
     fprintf(stderr,","); fp_print_stderr(&c);
     fprintf(stderr,")\n");
         printf("a1="); fp_print(&a1);
            printf("a2="); fp_print(&a2);
            printf("\n");
     exit(-1);
   }
   // fp_mod_fixed<16>(&m,&c,&a1);
   fp_mod(&m,&c,&a2);
   a2.used = 32;
   if(fp_cmp(&a2,&a3) != FP_EQ){
     printf("Different results\n");

     printf("a2="); fp_print(&a2);
     printf("a3="); fp_print(&a3);
     printf("\n");
     exit(-1);
   }
  }
}
