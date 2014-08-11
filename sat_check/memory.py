from threeval_expr import NFree, NValue, ForkException

import ipdb
class UndefinedMemoryError(Exception):
    pass
DEBUG=0
class Memory:
    def __init__(self,linebits):
        self.mem = {}
        self.cachebits = linebits
    def write(self,addr,value,length):
        global DEBUG
        a = addr.as_long()
        for i in range(length/8):
            self.mem[a+i] = value.extract(8*i+7,8*i)
    def _memif(self,slot,baseaddr,i,step,length):
        if i == 0:
            # slot == 0
            return self._read_defined(baseaddr,length)
        else: 
            return (slot == NValue(i,slot.size)).If(self._read_defined(baseaddr + i*step,length),self._memif(slot,baseaddr, i - 1,step,length))
    def _read_defined(self,addr,length):
            a = addr.as_long()
            #        print "r ", a
            if a not in self.mem:
                print "Undefined memory", hex(a), "reading ", length, "bytes"
                raise UndefinedMemoryError
            v = self.mem[a]
            for i in range(1,length/8):
                    #            print "r ", a+i
                if a+i not in self.mem:
                    raise UndefinedMemoryError
                v = self.mem[a+i].concat(v)
            return v
    def read(self,addr,length):
        try:
            return self._read_defined(addr,length)
        except ForkException:
#            print "ForkException caught" 
            assert(length == 64) # We can only do 8 bit gather for now, to make the array logic smaller
            cacheline = addr.extract(63,self.cachebits).as_long()
            slot = addr.extract(self.cachebits,3).simplify()
            align = addr.extract(2,0).as_long()
            if(align!=0):
                raise ForkException
            return self._memif(slot,NValue(cacheline<<self.cachebits,64),(2**self.cachebits)/8 - 1, 8,64)

            # Build a huge if on slot 
            
    def make_free(self):
        for i in self.mem:
            self.mem[i] = NFree("Init_"+hex(i),8)
    
