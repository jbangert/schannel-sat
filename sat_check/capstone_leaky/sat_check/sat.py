#!/usr/bin/env python2
from z3 import *
from capstone import *
from capstone.x86 import *
import json
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection


from IPython import embed
import pprint
pp = pprint.PrettyPrinter(indent=4)

class UnsupportedException(Exception):
    pass
class UndefinedExpression(Exception):
    pass
class UndefinedMemoryExpression(Exception):
    pass
class UndefinedInstrException(Exception):
    pass
def EvalExpression(expr):
    s = Solver()
    r = BitVec("result",64)
    s.add(r == expr)
    s.check()
    m = s.model()
    res = m[r].as_long()
    s.add (r != res)
    if(s.check() != unsat):
        raise ForkException
    return res    
class Input:
    def __init__(self,filename):
        self.ins = {}
        self.sym = {}
        elf = ELFFile(open(filename,'rb'))
        s = elf.get_section_by_name(b'.text')
        
        if not s:
            print ("No code found")
            return
        self.code = s.data()
        self.offset = s['sh_addr']
        self.md = Cs(CS_ARCH_X86, CS_MODE_64)
        for i in self.md.disasm(self.code,0): # Some bug if we dont disassemble from 0
#            print hex(i.address),i.mnemonic
            print json.dumps(i)
            self.ins[i.address] = True
        s = elf.get_section_by_name(b'.symtab')
        if s:
            if isinstance(s, SymbolTableSection):
                for sym in s.iter_symbols():
                    self.sym[sym.name] = sym['st_value']  - self.offset
class X86Machine:
    registernames = { X86_REG_RAX : "RAX", 
                      X86_REG_RCX : "RCX",
                      X86_REG_RDX : "RDX", 
                      X86_REG_RBX : "RBX",
                      X86_REG_RSP : "RSP",
                      X86_REG_RBP : "RBP",
                      X86_REG_RSI : "RSI",
                      X86_REG_RDI : "RDI",
                      X86_REG_R8 : "R8",
                      X86_REG_R9 : "R9",
                      X86_REG_R10 : "R10",
                      X86_REG_R11 : "R11",
                      X86_REG_R12 : "R12",
                      X86_REG_R13 : "R13",
                      X86_REG_R14 : "R14",
                      X86_REG_R15 : "R15"}
    
    regsize = 64
    initsp = 0x1000
    def __init__(self):
        self.regs = {}
        self.mem = Array('mem',BitVecSort(64),BitVecSort(64))
        for i in self.registernames:
            self.regs[i] = BitVec("init_"+self.registernames[i],self.regsize)
            self.regs["RSP"] = BitVecVal(self.initsp,64) # We use a special initial stack pointer
    def writeoperand(self,operand, expr):
        if operand.type == X86_OP_REG:
            self.regs[operand.reg] = expr
        elif operand.type == X86_OP_MEM:
            self.mem[solve_memoperand(operand.mem)] = expr
        else:
            raise UnsupportedException()
    def readoperand(self,operand):
        if operand.type == X86_OP_REG:
            return self.regs[operand.reg]
        elif operand.type == X86_OP_IMM:
            return BitVecVal(operand.imm,64) 
        elif operand.type == X86_OP_MEM:
            addr = solve_memoperand(operand.mem)
            if(addr in self.mem):
                return self.mem[addr]
            else:
                raise UndefinedMemoryException()
        else: 
            raise UnsupportedException()
    def checkfunction(self,ins,ip):
        while True:
            
            if ip not in ins:
                raise UndefinedInstrException()
            i = ins[ip]
            print "foo"
#            print i.size
            m = i.mnemonic

symbol = "_Z15fp_mul_comba_16P6fp_intS0_S0_"
x = Input("rsa")
m = X86Machine()
m.checkfunction(x.ins,x.sym['_Z8run_multv'] )
