#if 0
#define dbg_num(a,q) printf(#a " ="); fp_print(q)
#else
#define dbg_num(a,q) {}
#endif

template <int used>
void fp_rsamod_fixed(fp_int *a,fp_int *b, fp_int *r){
  //Thanks to Jelle van den Hooff for suggesting repeated subtraction. Because MSB[p] and MSB[q]= 1,
  //we always divide with a normalized number
  int i;
  fp_int tmp;
  fp_copy(a,r);
  for(i=0;i<4;i++){
    s_fp_sub_fixed<used>(r,b,&tmp);
    fp_notless_move<used>(r,b,r,&tmp);
  }
  fp_mod(a,b,&tmp);
  assert(fp_cmp_mag(r,&tmp) == FP_EQ);
}


template <int used>
void fp_modimpl_fixed(fp_int *a,fp_int *orig_b, fp_int *rout){
  fp_int tmp, b,r;
  fp_zero(&tmp);
  fp_copy(orig_b,&b);
  fp_copy(a,&r);
  fp_lshd(&b,used);
  unsigned i;
  for(i=0; i< used* DIGIT_BIT;i++){
    //fp_add(&r,&r,&r);
    //fp_add(q,q,q);
    s_fp_add_fixed<2*used>(&r,&r,&r);  
    s_fp_sub_fixed<2*used>(&r,&b,&tmp);
    fp_notless_move<2*used>(&r,&b,&r,&tmp);
  }
  fp_rshd(&r,used);
  r.used = used;
  if(a->sign == 1){
    s_fp_sub(orig_b,&r,&r);
    r.sign = 0;
  }
  if(rout)
    fp_copy(&r,rout);
}

template <int used> 
void fp_clean (fp_int *fp){
  for(int i =used;i<FP_SIZE;i++)
    fp->dp[i] = 0;
}
template <int used> 
void fp_mod_fixed(fp_int *a, fp_int  *b, fp_int *c){
  fp_int tmp;
    fp_zero(&tmp);
  fp_modimpl_fixed<used>(a,b,c);
  fp_mod(a,b,&tmp);
  tmp.used = c->used;
  assert(FP_EQ == fp_cmp_mag(&tmp, c));
}
