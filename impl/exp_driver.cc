#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z,mont,out;
  int i =0;
  x.used = y.used = z.used = mont.used= 0xFF;
  x.sign = y.sign = z.sign = mont.sign = 0xFF;
  for(i=0;i<FP_SIZE;i++){
    x.dp[i]=0xFF;
    y.dp[i]=0xFF;
    z.dp[i]=0xFF;
    mont.dp[i]=0xFF;
  }
  table_sc_exp<16>(&x,&y,&z, &mont,&out);
}
