#include <assert.h>
#include <stdint.h>
#include "rsa.cc"
#include <stdlib.h>
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
void fp_rand(fp_int *out){
  int i;
  fp_zero(out);
  for(i=0;i<17;i++){
    out->dp[i] =  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
    out->dp[i] +=  (uint16_t)rand();
    out->dp[i] <<= 16;
  }
  out->used = 17;
}
int main(){
  int i = 0;
  fp_int m,c,a1,a2,a3;
  srand(1234);
  
  for(;i<1000;i++){
    fp_rand(&m);
    fp_rand(&c);
    fp_zero(&a1);
    fp_zero(&a2);
    s_fp_sub_fixed<17>(&m,&c,&a1);
    s_fp_sub_fixed2<17>(&m,&c,&a2);
    if(fp_cmp_mag(&a1,&a2) != FP_EQ){
     printf("Different results for division\n");
     fprintf(stderr,"div("); fp_print_stderr(&m);
     fprintf(stderr,","); fp_print_stderr(&c);
     fprintf(stderr,")\n");
         printf("a1="); fp_print(&a1);
            printf("a2="); fp_print(&a2);
            printf("\n");
     exit(-1);
   }
  }
}
