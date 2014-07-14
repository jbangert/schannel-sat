CXXFLAGS= -I. -ggdb  -Wno-write-strings -DNDEBUG -O3
TFMDIR=../tomsfastmath-0.12/src
BINARIES= functionality_test mult_driver sqr_driver mont_driver
ACCESSORY= $(TFMDIR)/bin/*.c $(TFMDIR)/bit/*.c $(TFMDIR)/addsub/fp_cmp*.c $(TFMDIR)/mul/fp_mul_d.c $(TFMDIR)/misc/fp_set.c
all: $(BINARIES)

functionality_test: functionality_test.cc $(ACCESSORY) -lcrypto
mult_driver: mult_driver.cc $(ACCESSORY)
mont_driver: mont_driver.cc $(ACCESSORY)
sqr_driver: sqr_driver.cc   $(ACCESSORY)
clean: 
	rm $(BINARIES) *.o