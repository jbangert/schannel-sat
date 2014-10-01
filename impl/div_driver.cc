#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z;
  int i =0;
  x.used = y.used = 0xFF;
  x.sign = y.sign = 0xFF;
  for(i=0;i<FP_SIZE;i++){
    x.dp[i]=0xFF;
    y.dp[i]=0xFF;
  }
  fp_mod_fixed<17>(&x,&y,&z);
}
