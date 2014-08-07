#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z,mont,out;
  int i =0;
  x.used = y.used = z.used = mont.used= 16;
  x.sign = y.sign = z.sign = mont.sign = 1;
  for(i=0;i<FP_SIZE;i++){
    x.dp[i]=0;
    y.dp[i]=0;
    z.dp[i]=0;
    mont.dp[i]=0;
  }
  table_sc_exp<16>(&x,&y,&z, &mont,&out);
}
