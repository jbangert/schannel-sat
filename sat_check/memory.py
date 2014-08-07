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
    def read(self,addr,length):
        try:
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
        except ForkException:
            print "ForkException caught" 
            assert(length == 64) # We can only do 8 bit gather for now, to make the array logic smaller
            cacheline = addr.extract(63,linebits).as_long()
            slot = addr.extract(linebits,4)
            align = addr.extract(3,0).as_long()
            if(align!= NValue(0,3)):
                raise ForkException
            raise UnimplementedException
            # Build a huge if on slot 
            
    def make_free(self):
        for i in self.mem:
            self.mem[i] = NFree("Init_"+hex(i),8)
    
