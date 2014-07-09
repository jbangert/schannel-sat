CXXFLAGS=-I ../src/headers -ggdb  -Wno-write-strings  -O3 -DNDEBUG

ACCESSORY=../src/bin/*.c ../src/bit/*.c ../src/addsub/fp_cmp*.c ../src/mul/fp_mul_d.c ../src/misc/fp_set.c
rsa: rsa.cc $(ACCESSORY) -lcrypto
clean: 
	rm rsa
