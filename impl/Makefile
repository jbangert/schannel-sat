CXXFLAGS= -I. -ggdb  -Wno-write-strings -O3 -DNDEBUG  -fno-omit-frame-pointer 
TFMDIR=../tomsfastmath-0.12/src
BINARIES= functionality_test mult_driver sqr_driver mont_driver div_test
ACCESSORY= $(TFMDIR)/bin/*.c $(TFMDIR)/bit/*.c $(TFMDIR)/addsub/fp_cmp*.c $(TFMDIR)/mul/fp_mul_d.c $(TFMDIR)/misc/fp_set.c
all: $(BINARIES)

functionality_test: functionality_test.cc $(ACCESSORY) -lcrypto
mult_driver: mult_driver.cc $(ACCESSORY)
mont_driver: mont_driver.cc $(ACCESSORY)
sqr_driver: sqr_driver.cc   $(ACCESSORY)
div_test: div_test.cc   $(ACCESSORY)
clean: 
	rm $(BINARIES) *.o
