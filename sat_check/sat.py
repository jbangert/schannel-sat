#!/usr/bin/env python2
# -*- coding: utf-8 -*-
from inst_adapter import *
from threeval_expr import NFree, NValue, ForkException
import pickle, subprocess,os
from memory import Memory
from capstone_leaky import *
from capstone_leaky.x86 import *
from IPython.core import ultratb
import json
from input import Input

import sys
import traceback
#sys.excepthook = ultratb.FormattedTB(mode='Context',
#     color_scheme='Linux', call_pdb=1)
 
#from IPython import embed
import ipdb
import pprint
pp = pprint.PrettyPrinter(indent=4)

def info(type, value, tb):
    traceback.print_exception(type, value, tb)
    print
    ipdb.pm()

sys.excepthook = info
debug = 'DEBUG' in os.environ
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
               X86_REG_BL : X86_REG_RBX,
               X86_REG_R8B : X86_REG_R8,
               X86_REG_R9B : X86_REG_R9,
               X86_REG_R10B : X86_REG_R10,  
               X86_REG_R11B : X86_REG_R11,
               X86_REG_R12B : X86_REG_R12,
               X86_REG_R13B : X86_REG_R13,
               X86_REG_R14B : X86_REG_R14,
               X86_REG_R15B : X86_REG_R15}




    initsp = 0xffff0000
    def __init__(self):
        self.instcount = 0 
        self.regs = {}
        self.xmmregs = []
        self.mem =  Memory(7)
        self.carry = NFree('initialcarry')
        self.uniq = 1
        self.breakpoints = {}
        for i in self.registernames:
            self.regs[i] = NFree("init_"+self.registernames[i])
            self.regs[X86_REG_RSP] = NValue(self.initsp) # We use a special initial stack value
        for i in range(32):
            self.xmmregs.append(NFree("initxmm_"+str(i),128))
    def breakpoint(self,addr):
        self.breakpoints[addr] = True
    def print_state(self):
        for x in self.registernames:
            val = self.readreg(x)
            if(isinstance(val,NValue)):
                print self.registernames[x], val.as_long()
        
    def operandsize(self,i, idx):
        op = i.operands[idx]
        if(op.type == X86_OP_IMM or op.type == X86_OP_MEM):
            return i.op_size * 8
        elif op.type == X86_OP_REG:
            if op.reg in self.registernames:
                return 64
            elif(op.reg >= X86_REG_XMM0 and op.reg <=  X86_REG_XMM31):
                return 128
            elif op.reg in self.regs_32:
                return 32
            elif op.reg in self.regs_8:
                return 8
            else:
                raise MiscError()
    def solve_memoperand(self,op):
        value = NValue(op.disp)
        if(op.index != 0):
            if(op.scale == 1):
                value = value + self.regs[op.index]
            else:
                value = value + NValue(op.scale,64) * self.regs[op.index]
        if(op.base != 0):
            if(op.base == X86_REG_RIP):
                value = value + self.ip
            else:
                value = value + self.regs[op.base]
        return value
    def writeoperand(self,i,idx,expr):
        assert(expr.size == self.operandsize(i,idx))
        operand = i.operands[idx]
        if operand.type == X86_OP_REG:
            if operand.reg in self.registernames:
                # if operand.reg == X86_REG_R14:
                #     if isinstance(self.regs[X86_REG_R14], NFree):
                #         print "Free"
                #     else:
                #         print self.regs[X86_REG_R14].as_long()
                self.regs[operand.reg] = (expr)
            elif operand.reg in self.regs_32:
                self.regs[self.regs_32[operand.reg]] = expr.zeroext(32)
            elif operand.reg in self.regs_8:
                real_reg = self.regs_8[operand.reg]
                self.regs[real_reg] = ((expr & NValue(0xFF,64)) | (self.regs[real_reg] & ~NValue(0xFF,64)))
            elif operand.reg >= X86_REG_XMM0 and operand.reg <=  X86_REG_XMM31: 
                return self.xmmregs[operand.reg- X86_REG_XMM0]
            else:
                raise MiscError()
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            self.mem.write(addr,expr, i.op_size * 8)
        else:
            raise UnsupportedException()
    def readreg(self,reg):
        if reg in self.registernames:
            return self.regs[reg]
        elif reg in self.regs_32:
            return self.readreg(self.regs_32[reg]).extract(31,0)
        elif reg in self.regs_8:
            return self.readreg(self.regs_8[reg]).extract(7,0)
        elif reg >= X86_REG_XMM0 and reg <=  X86_REG_XMM31: 
            return self.xmmregs[reg- X86_REG_XMM0]
        else:
            print reg
            raise MiscError()
        
    def readoperand(self,i,idx):
        operand = i.operands[idx]
        if operand.type == X86_OP_REG:
            return self.readreg(operand.reg)
        elif operand.type == X86_OP_IMM:
            return NValue(operand.imm,self.operandsize(i,idx)) 
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            return self.mem.read(addr, i.op_size * 8)
        else: 
            raise UnsupportedException()
    # def writeop128(self,operand,expr):
    #     if operand.type == X86_OP_REG:
    #         if(operand.reg >= X86_REG_XMM0 and operand.reg <=  X86_REG_XMM31):
    #             self.xmmregs[operand.reg - X86_REG_XMM0] = expr
    #         else:
    #             raise MiscError()
    #     elif operand.type == X86_OP_MEM:
    #         addr = self.solve_memoperand(operand.mem)
    #         self.mem.write(addr, expr.extract(63,0))
    #         self.mem.write(addr + NValue(8), expr.extract(127,64))
    #     else: 
    #         raise MiscError()
    # def readop128(self,operand):
    #     if operand.type == X86_OP_REG:
    #         if(operand.reg >= X86_REG_XMM0 and operand.reg <=  X86_REG_XMM31):
    #             return self.xmmregs[operand.reg - X86_REG_XMM0]
    #         else:
    #             raise MiscError()
    #     elif operand.type == X86_OP_MEM:
    #         addr = self.solve_memoperand(operand.mem)
    #         lsh = self.mem.read(addr).zeroext(64)
    #         msh = self.mem.read(addr + NValue(8)).zeroext(64)
            return lsh | (msh << NValue(64,128))
    def resflags(self,expr):
        self.ZF =  (expr == 0)
        self.SF = ( expr.extract(expr.size-1,expr.size-1) == NValue(1,1))


    def cond_a(self):
        return (~self.carry) & (~self.ZF) != NValue(0) # And(Not(self.carry), Not(self.ZF)) # We
        # need better logical conjectures
    def cond_b(self):
        return self.carry
    def cond_eq(self):
        return self.ZF
    def load_data_segments(self, input):
        for k in input.rodata:
            self.mem.write(NValue(k), NValue(ord(input.rodata[k])),8)
    def checkfunction(self,ins,ip,offset, is_test_driver=False):
        self.ip = ip
        while True:
            self.instcount+=1
            if(self.instcount % 10000 == 0):
                print self.instcount, " instructions" , hex(self.ip + offset)
            if self.ip not in ins:
                raise UndefinedInstrException()
            i = ins[self.ip]       
            nextip = self.ip + i.size
            m = i.mnemonic
            
            if(self.ip + offset in self.breakpoints):
                ipdb.set_trace()
            if debug:
                print hex(self.ip + offset), i.mnemonic, " ", i.op_str
            if m == "push":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] - 8
                self.mem.write(self.regs[X86_REG_RSP], self.readoperand(i,0), i.op_size*8)
            elif m == "pop":
                self.writeoperand(i,0,  self.mem.read(self.regs[X86_REG_RSP], i.op_size*8))
                self.regs[X86_REG_RSP] =  self.regs[X86_REG_RSP] + 8
                #TODO: model pushing
            elif m == "ret":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] + 8
                break
            elif m == "call":
                if(is_test_driver):
                    self.mem.make_free()
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] -8
                rspval = self.regs[X86_REG_RSP]
                self.mem.write(rspval, NValue(nextip),64)
                self.checkfunction(ins,self.readoperand(i,0).as_long(), offset)
                rspval.assert_equal(self.regs[X86_REG_RSP] -8 )
                self.mem.read(rspval,64).assert_equal(NValue(nextip))
            elif m == "jmp":
                nextip = self.readoperand(i,0).as_long()
            elif m == "je":
                if(self.ZF.as_long() != 0):
                    nextip = self.readoperand(i,0).as_long()
            elif m == "jne":
                if(self.ZF.as_long() == 0):
                    nextip = self.readoperand(i,0).as_long()
            elif m == "and":
                self.carry = NValue(0)
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, a & b)
                self.resflags(a&b)
            elif m == "test":
                self.carry = NValue(0)
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = a & b
                self.resflags(c)
            elif m == "cmp":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = a - b 
                self.carry = b>a
                self.resflags(c)
            elif m == "nop":
                True
            elif m == "add":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = a + b 
                self.carry = c < a 
                self.writeoperand(i,0,c)  
                self.resflags(c)
            elif m == "adc":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = self.carry
                out = a + b +c
                self.carry = out < a | out < (a+b)
                self.writeoperand(i,0,out)
                self.resflags(c)
            elif m == "sub":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = a - b 
                self.carry = b>a
                self.writeoperand(i,0,c)
                self.resflags(c)
            elif m == "sbb":
                
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                c = self.carry #MyIf(self.carry, 1, 0)
                out = a - b - c
                self.carry =  (c + b) > a
                self.writeoperand(i,0,out)
                self.resflags(c)
            elif m == "lea":
                if(i.operands[1].type != X86_OP_MEM):
                    print "LEA with operand type", i.operands[1].type
                    raise MiscError()
                r = self.solve_memoperand(i.operands[1].mem)
                if(i.op_size == 4):
                    r = r.extract(31,0)
                self.writeoperand(i,0,r) #TODO: Use
                #a different function for solve_memoperand
            elif m == "mul":
                self.regs[X86_REG_EDX] = NFree("unk" + str(self.uniq))
                self.regs[X86_REG_EAX] = NFree("unk" + str(self.uniq+1))
                self.uniq+=2      
            elif m == "imul":
                self.regs[X86_REG_EDX] = NFree("unk" + str(self.uniq))
                self.regs[X86_REG_EAX] = NFree("unk" + str(self.uniq+1))
                self.uniq+=2
            elif m == "xor":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                x = i.operands[0]
                y = i.operands[1]
                shortcut = False
                if(x.type == y.type):
                    if(x.type == X86_OP_REG):
                        shortcut = (x.reg == y.reg)
                if(shortcut):
                    self.resflags(NValue(0))
                    self.writeoperand(i,0, NValue(0, self.operandsize(i,0)))
                else:
                    self.resflags(a^b)
                    self.writeoperand(i,0, a ^ b)
                self.carry = NValue(0)
            elif m == "mov":
                self.writeoperand(i,0, self.readoperand(i,1))
            elif m == "movsxd":
                self.writeoperand(i,0, NFree("unknown_" + str(self.uniq)))
                self.uniq += 1
            elif m == "movzx":
                self.writeoperand(i,0, self.readoperand(i,1)) 
            elif m == "shr":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                x = a.LshR(b)
                self.resflags(x)
                self.writeoperand(i,0, x)   
            elif m == "shl":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                x = a << b
                self.resflags(x)
                self.writeoperand(i,0, x)   
                
            elif m == "setb":
                # XXX: ignore operand size?
                self.writeoperand(i,0, self.carry.If(NValue(1,8), NValue(0,8)))
            elif m == "cmovb":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, self.cond_b().If(b,a ))
            elif m == "cmovnb"  or m == "cmovae":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, (~self.cond_b()).If(b,a))
            elif m == "cmovne":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, (~self.cond_eq()).If(b,a))
                
            elif m == "cmova":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, self.cond_a().If(b,a ))
            elif m == "cmovna" or m == "cmovbe":
                a = self.readoperand(i,0)
                b = self.readoperand(i,1)
                self.writeoperand(i,0, (~self.cond_a()).If(b,a ))
                
                
            elif m == "rep stosq":
                while(self.regs[X86_REG_RCX].as_long() != 0):
                    self.regs[X86_REG_RCX] -= 1
#                    print "store ", expr(self.regs[X86_REG_RDI])
                    self.mem.write(self.regs[X86_REG_RDI], self.regs[X86_REG_RAX],64)
                    self.regs[X86_REG_RDI] += 8
            elif m == "rep movsq":
                while(self.regs[X86_REG_RCX].as_long() != 0):
                    #TODO handle direction flag
                    self.regs[X86_REG_RCX] -= 1
#                    print "store ", expr(self.regs[X86_REG_RDI])
#                    print "load ", expr(self.regs[X86_REG_RSI])
                    self.mem.write(self.regs[X86_REG_RDI], self.mem.read(self.regs[X86_REG_RSI],64),64)
                    self.regs[X86_REG_RDI] += 8
                    self.regs[X86_REG_RSI] += 8
            elif m == "movdqa" or m == "movaps" or m == "movups" : # Only microarchitectural differences
              #  embed()
                self.writeoperand(i,0,self.readoperand(i,1))
       
            elif m == "clc":
                self.carry = NValue(0,64)
            elif m == "rcl" and  len(i.operands)== 1:
                x = self.readoperand(i,0) 
                oldcarry = self.carry
                self.carry = (x.extract(63,63) == NValue(1,1))
                self.writeoperand(i,0, (x << 1) | oldcarry )
            else:
                print "unknown ", m, " ", i.op_str
                
            self.ip = nextip
def read_process(filename):
    ex = os.path.dirname(os.path.realpath(__file__))+ "/inst_adapter.py"
    json = subprocess.check_output([ex, filename])
    return pickle.loads(json)
if __name__ == "__main__":
    symbol = "_Z15fp_mul_comba_16P6fp_intS0_S0_"
    x = read_process(sys.argv[1]) #input_wrapper(Input(sys.argv[1])) # 
    m = X86Machine()
    if debug:
        for z in os.environ['DEBUG'].split():
            m.breakpoint(int(z,0))
    m.load_data_segments(x)
    m.checkfunction(x.ins,x.sym['main'], x.offset,True )
