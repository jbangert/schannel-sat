#include "rsa.cc"

extern "C" int main(void){
  fp_int x,y,z;
  int i =0;
  x.used = y.used = 16;
  x.sign = y.sign = 1;
  for(i=0;i<FP_SIZE;i++){
    x.dp[i]=0;
    y.dp[i]=0;
  }
  fp_mod_fixed<17>(&x,&y,&z);
}
