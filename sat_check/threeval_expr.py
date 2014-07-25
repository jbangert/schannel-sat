from z3 import *
set_option(max_args=10000000, max_lines=1000000, max_depth=10000000, max_visited=1000000)
import operator
mask = 1<<65-1
bin = [ '__or__', '__and__', '__xor__', '__lshift__', '__rshift__', '__add__', '__sub__', '__mul__' ]
cmp = ['__lt__', '__le__', '__eq__', '__ne__', '__ge__', '__gt__']
class ForkException(Exception):
    pass
class UnequalException(Exception):
    pass
class NUtil:
    def assert_equal(self, other):
        if((self == other).as_long() != 1):
            raise UnequalException
class NFree(NUtil):
    def __init__(self, free):
        if(isinstance(free,str)):
            free = BitVec(free,64)
        self.v = free
    def bv(self):
        return self.v
    def invert(self):
        return NFree(~self.v)
    def as_long(self):
        s = Solver()
        r = BitVec("result",64)
        s.add(r== self.v)
        if(s.check() != sat):
            raise ForkException
        m = s.model()
        res  = m[r].as_long()
        s.add(r!= res)
        if(s.check()!= unsat):
            raise ForkException
            return res
    def If(self,a,b):
        return NFree(If(Extract(0,0,self.v) == 1, a.bv(), b.bv())) 

    def extract(self,a,b):
        return NFree(Extract(a,b,self.v))
    for i in bin:
        a = """
def FOO(self, other):
        return NFree(operator.FOO(self.bv(),other.bv()))""".replace("FOO",i)
        exec(a)
    for i in cmp:
        a = """
def FOO(self, other):
        return NFree(If(operator.FOO(self.bv(), other.bv()), BitVecVal(1,64), BitVecVal(0,64))) """.replace("FOO",i)
        exec(a)

class NValue(NUtil):
    def __init__(self, value, size = 64):
        self.value = value
        self.mask = (1 << (size + 1)) - 1
        self.size = size
    def bv(self):
        return BitVecVal(self.value,self.size)
    def __invert__(self):
        return NValue(mask ^ self.value)
    def as_long(self):
        return self.value
    def extract(self,a,b):
        return NValue((self.value & (1<< (a + 1)) - 1) >> b, a-b+1)
    def If(self, a,b):
        if((self.value & 1) != 0):
            return a
        else:
            return b
    for i in bin:
        a = """
def FOO(self, other): 
     if(isinstance(other,(int,long))):
        other = NValue(other)
     if(isinstance(other,NValue)):
        return NValue(operator.FOO(self.value,other.value) & self.mask)
     else:
        return NFree(operator.FOO(self.bv(), other.bv()))""".replace("FOO",i)
        exec(a)

    for i in cmp:
        a = """
def FOO(self, other):
        if(isinstance(other,NValue)):
           if(operator.FOO(self.value, other.value)):
             return NValue(1,64)
           else:
             return NValue(0,64)
        else:
           return NFree(If(operator.FOO(self.bv(), other.bv()),
                           BitVecVal(1,64), BitVecVal(0,64)))""".replace("FOO",i)
        exec(a)
            


