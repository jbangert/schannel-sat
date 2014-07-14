#include "rsa.cc"

extern "C" int main(void){
  fp_int m,x;
  fp_digit mp = 4; 
  int i =0;
  x.used = m.used = 16;
  x.sign = m.sign = 1;
  for(i=0;i<32;i++){
    x.dp[i]=0;
    m.dp[i]=0;
  }
  fp_montgomery_reduce(&x,&m,mp);
}
