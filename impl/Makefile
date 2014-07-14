CXXFLAGS= -I. -ggdb  -Wno-write-strings # -DNDEBUG -O3
TFMDIR=../tomsfastmath-0.12/src
ACCESSORY= $(TFMDIR)/bin/*.c $(TFMDIR)/bit/*.c $(TFMDIR)/addsub/fp_cmp*.c $(TFMDIR)/mul/fp_mul_d.c $(TFMDIR)/misc/fp_set.c
all: functionality_test mult_driver

functionality_test: functionality_test.cc $(ACCESSORY) -lcrypto
mult_driver: mult_driver.cc $(ACCESSORY)
clean: 
	rm mult_driver functionality_test *.o
