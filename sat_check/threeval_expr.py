
import operator
NOSAT= True

if not NOSAT:
    from z3 import *
    set_option(max_args=100, max_lines=100, max_depth=5, max_visited=100)
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
if not NOSAT:
    class NFree(NUtil):
        def __init__(self, free,size = 64):
            if(isinstance(free,str)):
                free = BitVec(free,size)
                self.size = free.size()
                self.v = free
	def bv(self):
	    return self.v
	def __invert__(self):
	    return NFree(~self.v)
	def as_long(self):
	    s = Solver()
	    r = BitVec("result",self.size)
	    s.add(r== self.v)
            print "Solving for", self.v
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
	    return NFree(Extract(a,b,self.v), a-b + 1)
	def concat(self,i):
	    return NFree(Concat(self.v, i.bv()), self.size + i.size)
	def zeroext(self,bits):
	    return NFree(ZeroExt(bits,self.v))
	for i in bin:
	        a = """
	def FOO(self, other):
	        if isinstance(other,(int,long)):
	           other = NValue(other)
	#        print (self.bv(), "FOO", other.bv())
	        return NFree(operator.FOO(self.bv(),other.bv()))""".replace("FOO",i)
	        exec(a)
        for i in cmp:
	        a = """
	def FOO(self, other):
	        if isinstance(other,(int,long)):
	               other = NValue(other,self.size)
	        return NFree(If(operator.FOO(self.bv(), other.bv()), BitVecVal(1,64), BitVecVal(0,64))) """.replace("FOO",i)
                exec(a)
else:	
	class NFree(NUtil):
	    def __init__(self,free,size = 64):
	        if(isinstance(free,str)):
	            self.size = size
	        else:
	            self.size = free.size
	    def bv(self):
	        return self
	    def as_long(self):
	        raise ForkException
	    def If(self,a,b):
	        return NFree("",a.size)
	    def extract(self,a,b):
	        return NFree("",a-b+1)
	    def concat(self,i):
	        return NFree("",self.size + i.size)
	    def zeroext(self,bits):
	        return NFree("",self.size + bits)
            def __invert__(self):
                return NFree("",self.size)
	    for i in bin:
	        a = """
def FOO(self, other):
        if isinstance(other,(int,long)):
           other = NValue(other)
#        print (self.bv(), "FOO", other.bv())
        assert(self.size == other.size)
        return NFree("", self.size )""".replace("FOO",i)
                exec(a)
	    for i in cmp:
	        a = """
def FOO(self, other):
        if isinstance(other,(int,long)):
               other = NValue(other,self.size)
        return NFree("", 64) """.replace("FOO",i)
	        exec(a)
	        
class NValue(NUtil):
    def __init__(self, value, size = 64):
        self.value = value
        self.mask = (1 << (size + 1)) - 1
        self.size = size
    if not NOSAT:
        def bv(self):
            return BitVecVal(self.value,self.size)
    else:
        def bv(self):
            return NFree("",self.size)
    def __invert__(self):
        return NValue(mask ^ self.value)
    def as_long(self):
        return self.value
    def extract(self,a,b):
        return NValue((self.value & (1<< (a + 1)) - 1) >> b, a-b+1)  
    def zeroext(self,bits):
        return NValue(self.value, self.size + bits)
    def If(self, a,b):
        if((self.value & 1) != 0):
            return a
        else:
            return b
    if not NOSAT:
	    def concat(self,other):
	        if(isinstance(other,NValue)):
	            return NValue(self.value << other.size | other.value,other.size + self.size)
	        else:
	            return NFree(BitVecVal(self.value << other.size, self.size + other.size) | other.bv())
    else:
        def concat(self,other):
            if(isinstance(other,NValue)):
                return NValue(self.value << other.size | other.value,other.size + self.size)
            else:
                return NFree("", other.size + self.size)


    for i in bin:
        a = """
def FOO(self, other): 
     if(isinstance(other,(int,long))):
        other = NValue(other)
     if(isinstance(other,NValue)):
#        print "sweet operation"
        return NValue(operator.FOO(self.value,other.value) & self.mask, self.size)
     else:
        return NFree(operator.FOO(self.bv(), other.bv()))""".replace("FOO",i)
        exec(a)
    if not NOSAT:
        for i in cmp:
            a = """
def FOO(self, other):
        if isinstance(other,(int,long)):
           other = NValue(other)
        if(isinstance(other,NValue)):
#           print "sweet comparison"
           if(operator.FOO(self.value, other.value)):
             return NValue(1,64)
           else:
             return NValue(0,64)
        else:
           return NFree(If(operator.FOO(self.bv(), other.bv()),
                           BitVecVal(1,64), BitVecVal(0,64)))""".replace("FOO",i)
            exec(a)
    else:
        for i in cmp:
            a = """
def FOO(self, other):
        if isinstance(other,(int,long)):
           other = NValue(other)
        if(isinstance(other,NValue)):
#           print "sweet comparison"
           if(operator.FOO(self.value, other.value)):
             return NValue(1,64)
           else:
             return NValue(0,64)
        else:
           return NFree("",64)""".replace("FOO",i)
            exec(a)
        
            


