#!/usr/bin/env python2
from z3 import *
from capstone_leaky import *
from capstone_leaky.x86 import *
import json
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection

set_option(max_args=10000000, max_lines=1000000, max_depth=10000000, max_visited=1000000)

from IPython import embed
import pprint
pp = pprint.PrettyPrinter(indent=4)

class UnsupportedException(Exception):
    pass
class UndefinedExpression(Exception):
    pass
class UndefinedMemoryError(Exception):
    pass
class UndefinedInstrException(Exception):
    pass
class MiscError(Exception):
    pass
class ForkException(Exception):
    pass
def always_equal(e1, e2):
    s = Solver()
    s.add(e1!=e2)
    print "Checking: " , e1 != e2
    return s.check() == unsat

def expr(expr):
    s = Solver()
    r = BitVec("result",64)
    s.add(r == expr)
    #print "Checking: " ,r == expr
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
        self.md.detail = True
        for i in self.md.disasm(self.code,0): # Some bug if we dont disassemble from 0
#            print hex(i.address),i.mnemonic
#            print json.dumps(i)
            self.ins[i.address] = i
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
    regs_32 = {X86_REG_EAX : X86_REG_RAX,
               X86_REG_ECX : X86_REG_RCX,
               X86_REG_EDX : X86_REG_RDX,
               X86_REG_EBX : X86_REG_RBX,
               X86_REG_ESP : X86_REG_RSP,
               X86_REG_EBP : X86_REG_RBP,
               X86_REG_ESI : X86_REG_RSI, 
               X86_REG_EDI : X86_REG_RDI,

               X86_REG_R8D : X86_REG_R8,
               X86_REG_R9D : X86_REG_R9,
               X86_REG_R10D : X86_REG_R10,  
               X86_REG_R11D : X86_REG_R11,
               X86_REG_R12D : X86_REG_R12,
               X86_REG_R13D : X86_REG_R13,
               X86_REG_R14D : X86_REG_R14,
               X86_REG_R15D : X86_REG_R15}
    regs_8 = {X86_REG_AL : X86_REG_RAX,
               X86_REG_CL : X86_REG_RCX,
               X86_REG_DL : X86_REG_RDX,
               X86_REG_BL : X86_REG_RBX}


    
    regsize = 64
    initsp = 0x1000
    def __init__(self):
        self.regs = {}
        # self.mem = Array('mem',BitVecSort(64),BitVecSort(64))
        self.mem = {}
        self.carry = Bool('initialcarry')
        self.uniq = 1
        for i in self.registernames:
            self.regs[i] = BitVec("init_"+self.registernames[i],self.regsize)
            self.regs[X86_REG_RSP] = BitVecVal(self.initsp,64) # We use a special initial stack
            # pointer
    def solve_memoperand(self,op):
        value = BitVecVal(op.disp,64)
        if(op.index != 0):
            if(op.scale == 1):
                value = value + self.regs[op.index]
            else:
                value = value + BitVecVal(op.scale,64) * self.regs[op.index]
        if(op.base != 0):
            value = value + self.regs[op.base]
        return expr(value)
    def writeoperand(self,operand, expr):
        if operand.type == X86_OP_REG:
            if operand.reg in self.registernames:
                self.regs[operand.reg] = expr
            elif operand.reg in self.regs_32:
                self.regs[self.regs_32[operand.reg]] = expr & BitVecVal(0xFFFFFFFF,64)
            elif operand.reg in self.regs_8:
                real_reg = self.regs_8[operand.reg]
                self.regs[real_reg] = (expr & BitVecVal(0xFF,64)) | (self.regs[real_reg] & ~BitVecVal(0xFF,64))
            else:
                raise MiscError()
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            self.mem[addr] = expr
        else:
            raise UnsupportedException()
    def readoperand(self,operand):
        if operand.type == X86_OP_REG:
            if operand.reg in self.registernames:
                return self.regs[operand.reg]
            elif operand.reg in self.regs_32:
                return self.regs[self.regs_32[operand.reg]] & BitVecVal(0xFFFFFFFF,64)
            elif operand.reg in self.regs_8:
                self.regs[self.regs_8[operand.reg]] & BitVecVal(0xFF,64)
            else:
                print operand.reg
                raise MiscError()
        elif operand.type == X86_OP_IMM:
            return BitVecVal(operand.imm,64) 
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            if(addr in self.mem):
                return self.mem[addr]
            else:
                print "Undefined memory address", hex(addr)
                raise UndefinedMemoryError()
        else: 
            raise UnsupportedException()
    def checkfunction(self,ins,ip,offset):
        while True:
            
            if ip not in ins:
                raise UndefinedInstrException()
            i = ins[ip]       
            nextip = ip + i.size
            m = i.mnemonic
            print hex(ip + offset), i.mnemonic, " ", i.op_str
            if m == "push":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] - BitVecVal(8,64)
                self.mem[expr(self.regs[X86_REG_RSP])] = self.readoperand(i.operands[0])
            elif m == "pop":
                self.writeoperand(i.operands[0],  self.mem[expr(self.regs[X86_REG_RSP])])
                self.regs[X86_REG_RSP] =  self.regs[X86_REG_RSP] + BitVecVal(8,64)
                #TODO: model pushing
            elif m == "ret":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] + BitVecVal(8,64)
                break
            elif m == "call":
                oldrsp = self.regs[X86_REG_RSP]
                rspval = expr(self.regs[X86_REG_RSP])
                self.mem[rspval] = BitVecVal(nextip,64)
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] -BitVecVal(8,64)
                
                self.checkfunction(ins,expr(self.readoperand(i.operands[0])), offset)
                if not always_equal(oldrsp, self.regs[X86_REG_RSP]):
                    raise MiscError()
                if not always_equal(self.mem[rspval],BitVecVal(nextip , 64)):
                    raise MiscError()
            elif m == "add":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a + b 
                self.carry = c < a 
                self.writeoperand(i.operands[0],c)
            elif m == "sub":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a - b 
                self.carry = b>a
                self.writeoperand(i.operands[0],c)
            elif m == "lea":
                if(i.operands[1].type != X86_OP_MEM):
                    print "LEA with operand type", i.operands[1].type
                    raise MiscError()
                self.writeoperand(i.operands[0],self.solve_memoperand(i.operands[1].mem))
            elif m == "mul":
                self.regs[X86_REG_EDX] = BitVec("unk" + str(self.uniq),64)
                self.regs[X86_REG_EAX] = BitVec("unk" + str(self.uniq+1),64)
                self.uniq+=2
            elif m == "xor":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], a ^ b)
                self.carry = False
            elif m == "mov":
                self.writeoperand(i.operands[0], self.readoperand(i.operands[1]))
            elif m == "rep stosq":
                while(expr(self.regs[X86_REG_RCX]) != 0):
                    self.regs[X86_REG_RCX] -= BitVecVal(1,64)
                    print "store ", hex(expr(self.regs[X86_REG_RDI]))
                    self.mem[expr(self.regs[X86_REG_RDI])] = self.regs[X86_REG_RAX]
                    self.regs[X86_REG_RDI] += BitVecVal(8,64)
            else:
                print "unknown ", m
                
            ip = nextip

symbol = "_Z15fp_mul_comba_16P6fp_intS0_S0_"
x = Input("rsa")
m = X86Machine()
m.checkfunction(x.ins,x.sym['_Z8run_multv'], x.offset )
