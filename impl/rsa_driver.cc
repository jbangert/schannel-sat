#include "rsa.cc"

struct args {
  fp_int c; fp_int p; fp_int p_mont; fp_int q; fp_int q_mont; fp_int d_p; fp_int d_q; fp_int q_inv; fp_int m;
};
extern "C" int proxy(struct args *a){
  rsa_crt(&a->c, &a->p, &a->p_mont, &a->q, &a->q_mont, &a->d_p, &a->d_q, &a->q_inv, &a->m);
  
} 
extern "C" int main(void){
  struct args a;
  int i;

   a.c.used=0;  a.p.used=0;  a.p_mont.used=0;  a.q.used=0; a.q_mont.used=0; a.d_p.used=0; a.d_q.used=0; a.q_inv.used=0; a.m.used=0;
   a.c.sign=0; a.p.sign=0; a.p_mont.sign=0; a.q.sign=0; a.q_mont.sign=0; a.d_p.sign=0; a.d_q.sign=0; a.q_inv.sign=0; a.m.sign=0;

  
  for(i=0;i<FP_SIZE;i++){
   a.c.dp[i]=0; a.p.dp[i]=0; a.p_mont.dp[i]=0; a.q.dp[i]=0; a.q_mont.dp[i]=0; a.d_p.dp[i]=0; a.d_q.dp[i]=0; a.q_inv.dp[i]=0; a.m.dp[i]=0;
  }
  proxy(&a);
}
