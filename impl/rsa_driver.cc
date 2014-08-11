#include "rsa.cc"

extern "C" int main(void){
  fp_int c; fp_int p; fp_int p_mont; fp_int q; fp_int q_mont; fp_int d_p; fp_int d_q; fp_int q_inv; fp_int m;
  int i;

   c.used=0;  p.used=0;  p_mont.used=0;  q.used=0;  q_mont.used=0;  d_p.used=0;  d_q.used=0;  q_inv.used=0;  m.used=0;
   c.sign=0;  p.sign=0;  p_mont.sign=0;  q.sign=0;  q_mont.sign=0;  d_p.sign=0;  d_q.sign=0;  q_inv.sign=0;  m.sign=0;

  
  for(i=0;i<FP_SIZE;i++){
   c.dp[i]=0;  p.dp[i]=0;  p_mont.dp[i]=0;  q.dp[i]=0;  q_mont.dp[i]=0;  d_p.dp[i]=0;  d_q.dp[i]=0;  q_inv.dp[i]=0;  m.dp[i]=0;
  }
  rsa_crt(&c, &p, &p_mont, &q, &q_mont, &d_p, &d_q, &q_inv, &m);
}
