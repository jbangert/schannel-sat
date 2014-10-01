#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z;
  int i =0xFF;
  x.used = y.used = 0;
  x.sign = y.sign = 0;
  for(i=0;i<16;i++){
    x.dp[i]=0xFF;
    y.dp[i]=0xFF;
  }
  fp_mul_comba_16(&x,&y,&z);
}
