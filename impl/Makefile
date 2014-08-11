CXXFLAGS= -I. -ggdb  -Wno-write-strings -O3 -DNDEBUG  
TFMDIR=../tomsfastmath-0.12/src
BINARIES= functionality_test mult_driver sqr_driver mont_driver div_driver exp_driver  rsa_driver
ACCESSORY= $(TFMDIR)/bin/*.c $(TFMDIR)/bit/*.c $(TFMDIR)/addsub/fp_cmp*.c $(TFMDIR)/mul/fp_mul_d.c $(TFMDIR)/misc/fp_set.c
all: $(BINARIES)

functionality_test: functionality_test.cc $(ACCESSORY) -lcrypto
mult_driver: mult_driver.cc $(ACCESSORY)
mont_driver: mont_driver.cc $(ACCESSORY)
sqr_driver: sqr_driver.cc   $(ACCESSORY)
div_driver: div_driver.cc $(ACCESSORY)
div_test: div_test.cc   $(ACCESSORY)
exp_driver: exp_driver.cc   $(ACCESSORY)
rsa_driver: rsa_driver.cc   $(ACCESSORY)
sub_test: sub_test.c   $(ACCESSORY)
clean: 
	rm $(BINARIES) *.o
