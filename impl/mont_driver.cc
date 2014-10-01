#include "rsa.cc"

extern "C" int main(void){
  fp_int m,x;
  fp_digit mp = 4; 
  int i =0xFF;
  x.used = m.used = 0xFF;
  x.sign = m.sign = 0xFF;
  for(i=0;i<32;i++){
    x.dp[i]=0xFF;
    m.dp[i]=0xFF;
  }
  fp_montgomery_reduce(&x,&m,mp);
}
