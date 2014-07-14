

#include "rsa.cc"
int exp_test(void) {
  fp_int n,e,m,c,d,e_m;
  int x;
  clock_t t1;
   fp_read_radix(&n,"ce032e860a9809a5ec31e4b0fd4b546f8c40043e3d2ec3d8f49d8f2f3dd19e887094ee1af75caa1c2e6cd9ec78bf1dfd6280002ac8c30ecd72da2e4c59a28a9248048aaae2a8fa627f71bece979cebf9f8eee2bd594d4a4f2e791647573c7ec1fcbd320d3825be3fa8a17c97086fdae56f7086ce512b81cc2fe44161270ec5e9" , 16);
   fp_read_radix(&e, "10001", 16);
   fp_read_radix(&m, "39f5a911250f45b99390e2df322b33c729099ab52b5879d06b00818cce57c649a66ed7eb6d8ae214d11caf9c81e83a7368cf0edb2b71dad791f13fecf546123b40377851e67835ade1d6be57f4de18a62db4cdb1880f4ab2e6a29acfd85ca22a13dc1f6fee2621ef0fc8689cd738e6f065c033ec7c148d8d348688af83d6f6bd", 16);
   fp_read_radix(&c, "9ff70ea6968a04530e6b06bf01aa937209cc8450e76ac19477743de996ba3fb445923c947f8d0add8c57efa51d15485309918459da6c1e5a97f215193b797dce98db51bdb4639c2ecfa90ebb051e3a2daeffd27a7d6e62043703a7b15e0ada5170427b63099cd01ef52cd92d8723e5774bea32716aaa7f5adbae817fb12a5b50", 16);


   /* test it */
   table_sc_exp<16>(&m, &e, &n, &e_m);
   if (fp_cmp(&e_m, &c)) {
      char buf[1024];
      printf("Encrypted text not equal\n");
      fp_toradix(&e_m, buf, 16);
      printf("e_m == %s\n", buf);
      return 0;
   
   }
   printf("CLOCKS_PER_SEC = %llu\n", (unsigned long long)CLOCKS_PER_SEC);
   t1 = clock();
   for (x = 0; x < 1000; x++) {
     //fp_exptmod(&m, &e, &n, &e_m);
     table_sc_exp<16>(&m, &e, &n, &e_m);
   }
   t1 = clock() - t1;
   printf("RSA-1024\n");
   printf("1000 RSA operations took     %10.5g seconds\n", (double)t1 / (double)CLOCKS_PER_SEC);
   printf("RSA encrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 / 1000.0) );

   /* read in the parameters */
   fp_read_radix(&n, "a7f30e2e04d31acc6936916af1e404a4007adfb9e97864de28d1c7ba3034633bee2cd9d5da3ea3cdcdc9a6f3daf5702ef750f4c3aadb0e27410ac04532176795995148cdb4691bd09a8a846e3e24e073ce2f89b34dfeb2ee89b646923ca60ee3f73c4d5397478380425e7260f75dfdc54826e160395b0889b1162cf115a9773f", 16);
   fp_read_radix(&d, "16d166f3c9a404d810d3611e6e8ed43293fe1db75c8906eb4810785a4b82529929dade1db7f11ac0335d5a59773e3167b022479eedefa514a0399db5c900750a56323cf9f5b0f21e7d60a46d75f3fcaabf30a63cbe34048b741a57ac36a13914afda798709dea5771f8d456cf72ec5f3afc1d88d023de40311143a36e7028739", 16);
   fp_read_radix(&c, "7d216641c32543f5b8428bdd0b11d819cfbdb16f1df285247f677aa4d44de62ab064f4a0d060ec99cb94aa398113a4317f2c550d0371140b0fd2c88886cac771812e72faad4b7adf495b9b850b142ccd7f45c0a27f164c8c7731731c0015f69d0241812e769d961054618aeb9e8e8989dba95714a2cf56c9e525c5e34b5812dd", 16);
   fp_read_radix(&m, "5f323bf0b394b98ffd78727dc9883bb4f42287def6b60fa2a964b2510bc55d61357bf5a6883d2982b268810f8fef116d3ae68ebb41fd10d65a0af4bec0530eb369f37c14b55c3be60223b582372fb6589b648d5a0c7252d1ae2dae5809785d993e9e5d0c4d9b0bcba0cde0d6671734747fba5483c735e1dab7df7b10ec6f62d8", 16);

   /* test it */
   table_sc_exp<16>(&c, &d, &n, &e_m);
   if (fp_cmp(&e_m, &m)) {
      char buf[1024];
      printf("Decrypted text not equal\n");
      fp_toradix(&e_m, buf, 16);
      printf("e_m == %s\n", buf);
      return 0;
   }

}
int crt_test(void ){
        fp_int p,q,d_p,d_q,q_inv, d,n;
        fp_int c,m,e_m;
        int x;
        fp_read_radix(&p,"00f97fd26a3b16684214427718403bef31f16778e63b0f60af7bdd12237675e4d157c05afcffa4fa8b96c13e854b3bfd2e5c5063a48e3105692513a034c8db1331453a605b8430aed329887cf5e272eceb8aca607f86b32200d4ff8bea65081a009a545491deb67389d281824b56a771f7a523793133ad5d751f9ccf198240601f",16);
    
        fp_read_radix(&q,"00f6dcb8a197dd9e96880f824a9a0839f8ac628460d9cdc2f49600a67d97784a76e3636af460b4fa4491aaf737edd88c1c811d09b93106b23f82215a1368d142f00b9bb20db46848902e3fd64f5c0bc0b052d3ff8c3cf18b66e7b4036b0d94b167a72ebdc50248e8f43b768cd50be7b88f449b3ddf6513235cc6d8244dfe788215",16);
        fp_read_radix(&d_p,"58c1a206ec7232309fa86d599130410d51456ffae733b73f761ed4df0cbb2fe551043ada19cf84d9b78d934233db695bdeb7325b4dc21e9ca2f5f25e14bdea7bf296abc4a2a7f6e963b30c512477a1d4c110f1772c61f94cb6791a3b0e29eda61d40b6bd15af6e36af4e271dc9fbd34d37d000c302114057f42102839e946c1d",16);
        fp_read_radix(&d_q,"1a7ddb4e7416b363221ccff5a38c087498434643827c2354ce1829c5c69081253b07bb3d98773378524d8f942ff1123028e6edd932daae1c44a1c1a1dd98bf5f354fb30ee7725d7f2387743f35f888e8232b5efc34ba1324aaeed58312850778c12731f447677da060153bcdcd2643d7141cb8ca1575b550ca81fd7b65389775",16);
        fp_read_radix(&q_inv, "0094edac81cfae95909eaed98f78b4dfaf470f6f4fefacc108ec7e1aca9ddca7180db24ccc61c685078ce6fc45d6da98486589c0efef221f5ecd3cf7e6ad65d79d86b3ec5b06612400c6f437ff18dbb672d0ec9d2e7b555179974e0b4f6d818544376677df16cfe7eaa610e335dfcc9a7f77607c25add853604ae12db600b11d39",16);
        fp_read_radix(&c,"4a905d306e848b4244ca846b159c8c28f811ccda7a3ddb00cb78948662ebfbaa8f33aeb26f3d3cad917ffecbc2a7bc594904aea3fd6a0059d65b378bc1cd0d7539478b060a8651c0cbbcc032965126f1fce2e907a4d379ac266a7223c542bb7ee68687c01eeb7d3ae2857f249446dcdae1340a13dfd2904838e0c4a771642a3d4bbfdc45d29995b72771adce3cffa326f7bd08c33628ceead6a168aa406564a61e7964270bfb2fce868e1d967b542b5bed3c357586924f9f4c9a330a0dc5d4c560560404bc61ff1fed957bf3d55bec566647c211abf381d93eb207e739dc4a9b184c8684f124ceb2c2c502789341cf457b5bde02557c7b8381c753b90fb06439",16);
        fp_read_radix(&m,"a56c485822ca2705d9e01d54e153d155cbdb2ee32b38ad247e12d82321abda6ce9454101a1306c709a04ef6aeb5c2ba7d9cca9d44b5a71358578ea2d6cce1848c7a4287cfd1322d56561be5d02f47f3d2a868e02455b0a2bc141681be50fa482c1469d8bfc73080b7f0f8841e82668500c249a61327931c16ce87daa768bc30fefddae70665203d1d3ee8e5f3613ec35988365f1fc19711675f06c43065965bf7436d9279bc1c036da96cb33c077958d0c2c9d0d2ece5b6009de6a7389aca0f93b9f0b7690eaa27b797865a0249cf64c7b2d5efc2a90bdbb2f4a2b7bf21b2e7410e6a589f931ed28e1b3647c2dd5a5fdf456a8a667bc3bc56603d4c3fa5590d2",16);
        fp_read_radix(&d, "16ce103ddd354bde68ccb8092498931c047599e17e26c7832530eecf0ddebbe315ed4ec02d8a0aecd29afc52f36f9824c561277c5d1cf1e6f8bde403ad70e3bc2cb2d9ad7405c268c01fe4afc853fc00eeca585cf936945f1f65e8826ab4b1e7585b8cd19213a34de74e57babbecd9d937e98a663f57ef9ca94ef4f0a88e12c33afef03fff8dc98016beaddb4f5dde468c6ffae25e4137896ff9e81f4c783f8691dad676bd1786db4a911674a3e4b77950264aa69182cb20383f9fd1bd91c2f4a50a7b48c42b3c0a9c3d9e267e5c28e8752546ebb08837828d220eb8d2433f0732f28e6b0fe68de916a8a43eede93b5356773c272fc443d1469b3c768bc1cf61",16);
        fp_read_radix(&n, "00f097f1fc44b461bd4435717cf9f8bc1c0162ac0ed64e5e5e7bccbbeb6c35cf9c82923e34ed22789e25937d5c230d2b888d596151c95388d0cc9ad16a0db48cfbe595c65156584a7281434a0034e79df4e7e73d20de5a904e9ec91a8d91307d1dedfc9637abd0bf47d05ee8cd36e017fcc695713ce60b7ef401235e7051b5a0c8510c6613e4aa388a7c4a4059abfe800e35e0b5ab05d898f3ff8aa510048406aea7a9f7ca4a5b1baacc206cb3ec7c07f9950ba0568023a20617c9ab0c941040a28d4c1cc3dc9d97323b4632cd185c5c5df83ad02db2fd8770d476ba2584e9aa0b87f8d345804d3dd128947300d2fd0084050ccb1c4c13e56feb42241b309fa08b",16 );

        // //   table_sc_exp(&c, &d, &n, &e_m);
        // if (fp_cmp(&e_m, &m)) {
        //         char buf[1024];
        //         printf("Encrypted text not equal\n");
        //         fp_toradix(&e_m, buf, 16);
        //         printf("e_m == %s\n", buf);
        //         exit(-1);
                
        // }
        rsa_crt(&c,&p,&q,&d_p,&d_q,&q_inv,&e_m);
        if (fp_cmp(&e_m, &m)) {
                char buf[1024];
                printf("Encrypted text not equal\n");
                fp_toradix(&e_m, buf, 16);
                printf("e_m == %s\n", buf);
                exit(-1);
        }

        clock_t t1;
        const int count = 1000;
        t1 = clock();
        for (x = 0; x < count; x++) {
                rsa_crt(&c,&p,&q,&d_p,&d_q,&q_inv,&e_m);
        }
        t1 = clock() - t1;
        printf("%d CRT RSA took     %10.5g seconds\n",count, (double)t1 / (double)CLOCKS_PER_SEC);
        
        printf("CRT decrypt/sec              %10.5g\n", (double)CLOCKS_PER_SEC / ((double)t1 /(double)count) );
}

int main(void){
  exp_test();
  crt_test();
}
