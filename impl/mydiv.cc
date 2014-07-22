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
void fp_div_fixed(fp_int *a,fp_int *orig_b, fp_int *q, fp_int *rout){
  fp_int tmp, b,r;
  fp_zero(&tmp);
  fp_copy(orig_b,&b);
  dbg_num(b,&b);
  fp_copy(a,&r);
  dbg_num(r,&r);
  fp_zero(q);
  fp_lshd(&b,used);
  dbg_num(b,&b);
  unsigned i;
  for(i=0; i< used* DIGIT_BIT;i++){
    s_fp_add_fixed<2*used>(&r,&r,&r);    
    s_fp_add_fixed<2*used>(q,q,q);
    dbg_num(r2,&r);
    dbg_num(B,&b);
    fp_sub(&r,&b,&tmp);
    //TODO: reuse the same comparison here
    fp_notless_setlsb<2*used>(&r,&b,q);
    /*if(!fp_less_fixed<2*used>(&r,&b))
      printf("LT\n"); */
    fp_notless_move<2*used>(&r,&b,&r,&tmp);
    dbg_num(q,q);
    dbg_num(r,&r);
  }
  fp_rshd(&r,used);
  r.used = used;
  q->used = used;
  if(rout)
    fp_copy(&r,rout);
}

template <int used> 
void fp_mod_fixed(fp_int *a, fp_int  *b, fp_int *c){
  fp_int  t;

  fp_div_fixed<used>(a,b,&t,c);
}
