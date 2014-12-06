CXXFLAGS= -I. -ggdb3  -Wno-write-strings  -O3  -DNDEBUG
CFLAGS=$(CXXFLAGS)
ASFLAGS=$(CFLAGS)
TFMDIR=../tomsfastmath-0.12/src
BINARIES= functionality_test mult_driver sqr_driver mont_driver div_driver exp_driver  rsa_driver mult_bench
ACCESSORY= $(TFMDIR)/../libtfm.a #$(TFMDIR)/bin/*.c $(TFMDIR)/bit/*.c $(TFMDIR)/addsub/fp_*.c $(TFMDIR)/mul/fp_mul*.c $(TFMDIR)/misc/fp_set.c $(TFMDIR)/divide/fp_div_d.c $(TFMDIR)/mont/fp_montgomery_calc_normalization.c
all: $(BINARIES)

mulmont.o: mulmont.S
functionality_test: $(ACCESSORY) functionality_test.cc mulmont.o -lcrypto
mult_driver: mult_driver.cc
mont_driver: mont_driver.cc 
sqr_driver: sqr_driver.cc   
div_driver: div_driver.cc 
div_test: div_test.cc   
exp_driver: exp_driver.cc 
rsa_driver: rsa_driver.cc  
sub_test: sub_test.c  
rsa.o: rsa.cc
mult_bench: mult_bench.o
mult_bench.o: mul.S
	nasm -f elf64 -o $@ $<
clean: 
	rm $(BINARIES) *.o
