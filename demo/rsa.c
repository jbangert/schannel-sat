#include "tfm.h"
#include <time.h>
#define TABLE 4

int fp_print(fp_int *test){
        char buf[128];
        fp_toradix(test,buf,10);
        printf("%s\n",buf);
        return 0;
}
static void scatter(fp_digit table[], int choice, fp_int *foo,int width){
        int i = 0;
        for(i=0;i<width;i++)
                table[(1<<TABLE) * i + choice] = foo->dp[i];
}
static void gather(fp_digit table[], int choice, fp_int *foo, int width){
        int i = 0;
        for(i=0;i<width;i++)
                foo->dp[i]= table[(1<<TABLE) * i + choice];
        foo->used = width;
        foo->sign = 0;
}
//WE need a compile time assert for byte-width
int table_sc_exp(fp_int *a, fp_int *b,fp_int *m, fp_int *res){
        fp_digit table[FP_SIZE * (1<<TABLE)];   
        fp_int temp, temp1;
        fp_digit buf,mp,y;
        int i,digidx,err, bitcnt;
        int width = a->used;
        if ((err = fp_montgomery_setup (m, &mp)) != FP_OKAY) {
                return err;
        }
        /* now we need R mod m */
        fp_montgomery_calc_normalization (&temp, m); //Set used to 16 everywhere!
        /* now set R[0][1] to G * R mod m */
        fp_mod(a, m, &temp1);
        fp_mulmod (&temp1, &temp, m, &temp1); 
        //Table[0] = Mont(1) = temp
        //Table[1] = Mont(a) = temp1
        scatter(table,0, &temp, width);
        fp_init_copy(&temp, &temp1);
        scatter(table,1, &temp1,width);

        for(i=2;i<(1<<TABLE); i++){
                fp_mul(&temp,&temp1,&temp);
                fp_montgomery_reduce(&temp, m, mp);
                scatter(table,i,&temp,width);
        }
        digidx= b->used-1;
        buf=b->dp[digidx--];
        bitcnt = (int)DIGIT_BIT;

        y = (buf >> (DIGIT_BIT-TABLE)) & ((1<<TABLE) -1);
        buf<<=(fp_digit)TABLE;
        gather(table,y,res,width);
        for(;;){
                bitcnt -= TABLE;
                if(bitcnt==0 ){
                        if(digidx == -1){
                                break;
                        }
                        buf = b->dp[digidx--];
                        bitcnt = (int) DIGIT_BIT;
                }  
                y = (fp_digit) (buf >> (DIGIT_BIT - TABLE)) & ((1<<TABLE) -  1);
                buf <<= (fp_digit)TABLE;
                for(i=0;i<TABLE;i++){
                        fp_sqr(res,res);                  
                        fp_montgomery_reduce(res,m, mp);
                }
                gather(table,y,&temp,width);
                fp_mul(res, &temp, res );
                fp_montgomery_reduce(res,m, mp);
        }
        fp_montgomery_reduce(res,m,mp);
        return 0;
}
int table_exp(fp_int *a, fp_int *b,fp_int *m, fp_int *res){
        fp_int table[1<<TABLE];   
        fp_digit buf,mp,y;
        int i,digidx,err, bitcnt;
        if ((err = fp_montgomery_setup (m, &mp)) != FP_OKAY) {
                return err;
        }
        /* now we need R mod m */
        fp_montgomery_calc_normalization (&table[0], m); //Set used to 16 everywhere!
        /* now set R[0][1] to G * R mod m */
        fp_mod(a, m, &table[1]);
        fp_mulmod (&table[1], &table[0], m, &table[1]); 
        //Table[0] = Mont(1)
        //Table[1] = Mont(a)
        for(i=2;i<(1<<TABLE); i++){
                fp_mul(&table[i-1],&table[1],&table[i]);
                fp_montgomery_reduce(&table[i], m, mp);
        }
        digidx= b->used-1;
        buf=b->dp[digidx--];
        bitcnt = (int)DIGIT_BIT;

        y = (buf >> (DIGIT_BIT-TABLE)) & ((1<<TABLE) -1);
        buf<<=(fp_digit)TABLE;
        fp_init_copy(res,&table[y]);
        for(;;){
                bitcnt -= TABLE;
                if(bitcnt==0 ){
                        if(digidx == -1){
                                break;
                        }
                        buf = b->dp[digidx--];
                        bitcnt = (int) DIGIT_BIT;
                }  
                y = (fp_digit) (buf >> (DIGIT_BIT - TABLE)) & ((1<<TABLE) -  1);
                buf <<= (fp_digit)TABLE;
                for(i=0;i<TABLE;i++){
                        fp_sqr(res,res);                  
                        fp_montgomery_reduce(res,m, mp);
                }
                fp_mul(res, &table[(int)y], res );
                fp_montgomery_reduce(res,m, mp);
        }
        fp_montgomery_reduce(res,m,mp);
        return 0;
}
int fasttest(void){

   fp_int d, e, n, c, m, e_m;
   /* read in the parameters */
   fp_read_radix(&n, "a7f30e2e04d31acc6936916af1e404a4007adfb9e97864de28d1c7ba3034633bee2cd9d5da3ea3cdcdc9a6f3daf5702ef750f4c3aadb0e27410ac04532176795995148cdb4691bd09a8a846e3e24e073ce2f89b34dfeb2ee89b646923ca60ee3f73c4d5397478380425e7260f75dfdc54826e160395b0889b1162cf115a9773f", 16);
   fp_read_radix(&d, "16d166f3c9a404d810d3611e6e8ed43293fe1db75c8906eb4810785a4b82529929dade1db7f11ac0335d5a59773e3167b022479eedefa514a0399db5c900750a56323cf9f5b0f21e7d60a46d75f3fcaabf30a63cbe34048b741a57ac36a13914afda798709dea5771f8d456cf72ec5f3afc1d88d023de40311143a36e7028739", 16);
   fp_read_radix(&c, "7d216641c32543f5b8428bdd0b11d819cfbdb16f1df285247f677aa4d44de62ab064f4a0d060ec99cb94aa398113a4317f2c550d0371140b0fd2c88886cac771812e72faad4b7adf495b9b850b142ccd7f45c0a27f164c8c7731731c0015f69d0241812e769d961054618aeb9e8e8989dba95714a2cf56c9e525c5e34b5812dd", 16);
   fp_read_radix(&m, "5f323bf0b394b98ffd78727dc9883bb4f42287def6b60fa2a964b2510bc55d61357bf5a6883d2982b268810f8fef116d3ae68ebb41fd10d65a0af4bec0530eb369f37c14b55c3be60223b582372fb6589b648d5a0c7252d1ae2dae5809785d993e9e5d0c4d9b0bcba0cde0d6671734747fba5483c735e1dab7df7b10ec6f62d8", 16);

   clock_t t1;
   int x;
   t1 = clock();
   for (x = 0; x < 1000; x++) {
      table_sc_exp(&c, &d, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("1000 sctable operations took      %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA (sctable) decrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 1000.0) );
}
int test(void)
{
   fp_int d, e, n, c, m, e_m;
   clock_t t1;
   int x;
   puts(fp_ident());
#if 0
   /* read in the parameters */
   fp_read_radix(&n, "ce032e860a9809a5ec31e4b0fd4b546f8c40043e3d2ec3d8f49d8f2f3dd19e887094ee1af75caa1c2e6cd9ec78bf1dfd6280002ac8c30ecd72da2e4c59a28a9248048aaae2a8fa627f71bece979cebf9f8eee2bd594d4a4f2e791647573c7ec1fcbd320d3825be3fa8a17c97086fdae56f7086ce512b81cc2fe44161270ec5e9", 16);
   fp_read_radix(&e, "10001", 16);
   fp_read_radix(&m, "39f5a911250f45b99390e2df322b33c729099ab52b5879d06b00818cce57c649a66ed7eb6d8ae214d11caf9c81e83a7368cf0edb2b71dad791f13fecf546123b40377851e67835ade1d6be57f4de18a62db4cdb1880f4ab2e6a29acfd85ca22a13dc1f6fee2621ef0fc8689cd738e6f065c033ec7c148d8d348688af83d6f6bd", 16);
   fp_read_radix(&c, "9ff70ea6968a04530e6b06bf01aa937209cc8450e76ac19477743de996ba3fb445923c947f8d0add8c57efa51d15485309918459da6c1e5a97f215193b797dce98db51bdb4639c2ecfa90ebb051e3a2daeffd27a7d6e62043703a7b15e0ada5170427b63099cd01ef52cd92d8723e5774bea32716aaa7f5adbae817fb12a5b50", 16);


   /* test it */
   fp_exptmod(&m, &e, &n, &e_m);
   if (fp_cmp(&e_m, &c)) {
      char buf[1024];
      printf("Encrypted text not equal\n");
      fp_toradix(&e_m, buf, 16);
      printf("e_m == %s\n", buf);
      return 0;
   
   }
#endif 
   fp_read_radix(&m,"3",10);
   fp_read_radix(&e,"2",10);
   fp_read_radix(&c,"2",10);
   fp_read_radix(&n,"7",10);

   //fp_exptmod(&m, &e, &n, &e_m);
   table_exp(&m, &e, &n, &e_m);
   if (fp_cmp(&e_m, &c)) {
      char buf[1024];
      printf("Table version not equal\n");
      fp_toradix(&e_m, buf, 16);
      printf("e_m == %s\n", buf);
      return 0;
   }

   printf("CLOCKS_PER_SEC = %llu\n", (unsigned long long)CLOCKS_PER_SEC);
   t1 = clock();
   for (x = 0; x < 1000; x++) {
      fp_exptmod(&m, &e, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("1000 RSA operations took     %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA encrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 1000.0) );

   /* read in the parameters */
   fp_read_radix(&n, "a7f30e2e04d31acc6936916af1e404a4007adfb9e97864de28d1c7ba3034633bee2cd9d5da3ea3cdcdc9a6f3daf5702ef750f4c3aadb0e27410ac04532176795995148cdb4691bd09a8a846e3e24e073ce2f89b34dfeb2ee89b646923ca60ee3f73c4d5397478380425e7260f75dfdc54826e160395b0889b1162cf115a9773f", 16);
   fp_read_radix(&d, "16d166f3c9a404d810d3611e6e8ed43293fe1db75c8906eb4810785a4b82529929dade1db7f11ac0335d5a59773e3167b022479eedefa514a0399db5c900750a56323cf9f5b0f21e7d60a46d75f3fcaabf30a63cbe34048b741a57ac36a13914afda798709dea5771f8d456cf72ec5f3afc1d88d023de40311143a36e7028739", 16);
   fp_read_radix(&c, "7d216641c32543f5b8428bdd0b11d819cfbdb16f1df285247f677aa4d44de62ab064f4a0d060ec99cb94aa398113a4317f2c550d0371140b0fd2c88886cac771812e72faad4b7adf495b9b850b142ccd7f45c0a27f164c8c7731731c0015f69d0241812e769d961054618aeb9e8e8989dba95714a2cf56c9e525c5e34b5812dd", 16);
   fp_read_radix(&m, "5f323bf0b394b98ffd78727dc9883bb4f42287def6b60fa2a964b2510bc55d61357bf5a6883d2982b268810f8fef116d3ae68ebb41fd10d65a0af4bec0530eb369f37c14b55c3be60223b582372fb6589b648d5a0c7252d1ae2dae5809785d993e9e5d0c4d9b0bcba0cde0d6671734747fba5483c735e1dab7df7b10ec6f62d8", 16);

   /* test it */
   fp_exptmod(&c, &d, &n, &e_m);
   if (fp_cmp(&e_m, &m)) {
      char buf[1024];
      printf("Decrypted text not equal\n");
      fp_toradix(&e_m, buf, 16);
      printf("e_m == %s\n", buf);
      return 0;
   }

   t1 = clock();
   for (x = 0; x < 100; x++) {
      fp_exptmod(&c, &d, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("100 RSA operations took      %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA decrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 100.0) );

   t1 = clock();
   for (x = 0; x < 100; x++) {
      table_exp(&c, &d, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("100 table operations took      %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA (table) decrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 100.0) );

   t1 = clock();
   for (x = 0; x < 100; x++) {
      table_sc_exp(&c, &d, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("100 sctable operations took      %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA (sctable) decrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 100.0) );

   /* test half size */
   fp_rshd(&n, n.used >> 1);
   fp_rshd(&d, d.used >> 1);
   fp_rshd(&c, c.used >> 1);
   printf("n.used == %4d bits\n", n.used * DIGIT_BIT);

   /* ensure n is odd */
   n.dp[0] |= 1;
   t1 = clock();
   for (x = 0; x < 100; x++) {
      fp_exptmod(&c, &d, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("100 RSA-half operations took %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA decrypt/sec              %10.5g (estimate of RSA-1024-CRT) \n", (double)CLOCKS_PER_SEC / ((double)t1 / 50.0) );



   return 0;
}

int main(void){
        fasttest();
}
/* $Source: /cvs/libtom/tomsfastmath/demo/rsa.c,v $ */
/* $Revision: 1.2 $ */
/* $Date: 2005/05/05 14:39:33 $ */
