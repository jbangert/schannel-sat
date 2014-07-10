#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z;
  int i =0;
  x.used = y.used = 16;
  x.sign = y.sign = 1;
  for(i=0;i<16;i++){
    x.dp[i]=0;
    y.dp[i]=0;
  }
  fp_mul_comba_16(&x,&y,&z);
}
