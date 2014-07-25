#!/usr/bin/env python2
# -*- coding: utf-8 -*-
from threeval_expr import NFree, NValue, ForkException
from capstone_leaky import *
from capstone_leaky.x86 import *
import json
from input import Input
import sys

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




    initsp = 0x1000
    def __init__(self):
        self.instcount = 0 
        self.regs = {}
        self.mem = {}# TODO: Handle memory overlap
        self.carry = NFree('initialcarry')
        self.uniq = 1
        for i in self.registernames:
            self.regs[i] = NFree("init_"+self.registernames[i])
            self.regs[X86_REG_RSP] = NValue(self.initsp) # We use a special initial stack value
    def free_mem(self):
        for addr in self.mem:
            self.mem[addr] = NFree('init_mem' + hex(addr))
    def solve_memoperand(self,op):
        value = NValue(op.disp)
        if(op.index != 0):
            if(op.scale == 1):
                value = value + self.regs[op.index]
            else:
                value = value + NValue(op.scale,64) * self.regs[op.index]
        if(op.base != 0):
#            if(op.base == X86_REG_RIP):
#                value = value + self.ip
#            else:
                value = value + self.regs[op.base]
        return value
    def readmem(self, addr):
        a = addr.as_long()
        if(a in self.mem):
            return self.mem[addr.as_long()]
        else:
            print "Undefined memory address", hex(a)
            raise UndefinedMemoryError()
    def writemem(self, addr,v):
        self.mem[addr.as_long()] = v
    def writeoperand(self,operand, expr):
        if operand.type == X86_OP_REG:
            if operand.reg in self.registernames:
                self.regs[operand.reg] = (expr)
            elif operand.reg in self.regs_32:
                self.regs[self.regs_32[operand.reg]] = (expr & NValue(0xFFFFFFFF))
            elif operand.reg in self.regs_8:
                real_reg = self.regs_8[operand.reg]
                self.regs[real_reg] = ((expr & NValue(0xFF,64)) | (self.regs[real_reg] & ~NValue(0xFF,64)))
            else:
                raise MiscError()
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            self.writemem(addr,expr)
        else:
            raise UnsupportedException()
    def readreg(self,reg):
        if reg in self.registernames:
            return self.regs[reg]
        elif reg in self.regs_32:
            return self.readreg(self.regs_32[reg]) & NValue(0xFFFFFFFF)
        elif reg in self.regs_8:
            return self.readreg(self.regs_8[reg]) &NValue(0xFF)
        else:
            print reg
            raise MiscError()
        
    def readoperand(self,operand):
        if operand.type == X86_OP_REG:
            return self.readreg(operand.reg)
        elif operand.type == X86_OP_IMM:
            return NValue(operand.imm) 
        elif operand.type == X86_OP_MEM:
            addr = self.solve_memoperand(operand.mem)
            return self.readmem(addr)

        else: 
            raise UnsupportedException()
    def resflags(self,expr):
        self.ZF = ( NValue(0) == expr)
        self.SF = ( expr.extract(63,63) == NValue(1,1))


    def cond_a(self):
        return (~self.carry) & (~self.ZF) != NValue(0) # And(Not(self.carry), Not(self.ZF)) # We
        # need better logical conjectures
    def cond_b(self):
        return self.carry
        
    def checkfunction(self,ins,ip,offset, is_test_driver=False):
        self.ip = ip
        while True:
            self.instcount+=1
            if(self.instcount % 2000 == 0):
                print self.instcount, " instructions" 
            if self.ip not in ins:
                raise UndefinedInstrException()
            i = ins[self.ip]       
            nextip = self.ip + i.size
            m = i.mnemonic
#            print hex(self.ip + offset), i.mnemonic, " ", i.op_str
            if m == "push":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] - 8
                self.writemem(self.regs[X86_REG_RSP], self.readoperand(i.operands[0]))
            elif m == "pop":
                self.writeoperand(i.operands[0],  self.readmem(self.regs[X86_REG_RSP]))
                self.regs[X86_REG_RSP] =  self.regs[X86_REG_RSP] + 8
                #TODO: model pushing
            elif m == "ret":
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] + 8
                break
            elif m == "call":
                rspval = self.regs[X86_REG_RSP]
                if(is_test_driver):
                    self.free_mem()
                self.writemem(rspval, NValue(nextip))
                self.regs[X86_REG_RSP] = self.regs[X86_REG_RSP] -8

                self.checkfunction(ins,self.readoperand(i.operands[0]).as_long(), offset)
                rspval.assert_equal(self.regs[X86_REG_RSP])
                self.readmem(rspval).assert_equal(NValue(nextip))
            elif m == "jmp":
                nextip = self.readoperand(i.operands[0])
            elif m == "je":
                if(self.ZF.as_long() != 0):
                    nextip = self.readoperand(i.operands[0]).as_long()
            elif m == "jne":
                if(self.ZF.as_long() == 0):
                    nextip = self.readoperand(i.operands[0]).as_long()
            elif m == "and":
                self.carry = NValue(0)
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], a & b)
                self.resflags(a&b)
            elif m == "test":
                self.carry = NValue(0)
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a & b
            elif m == "cmp":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a - b 
                self.carry = b>a
                self.resflags(c)
            elif m == "nop":
                True
            elif m == "add":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a + b 
                self.carry = c < a 
                self.writeoperand(i.operands[0],c)  
                self.resflags(c)
            elif m == "adc":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = self.carry
                out = a + b +c
                self.carry = out < a | out < (a+b)
                self.writeoperand(i.operands[0],out)
                self.resflags(c)
            elif m == "sub":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = a - b 
                self.carry = b>a
                self.writeoperand(i.operands[0],c)
                self.resflags(c)
            elif m == "sbb":
                
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                c = self.carry #MyIf(self.carry, 1, 0)
                out = a - b - c
                self.carry =  (c + b) > a
                self.writeoperand(i.operands[0],out)
                self.resflags(c)
            elif m == "lea":
                if(i.operands[1].type != X86_OP_MEM):
                    print "LEA with operand type", i.operands[1].type
                    raise MiscError()
                self.writeoperand(i.operands[0],self.solve_memoperand(i.operands[1].mem)) #TODO: Use
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
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                if(a==b):
                    self.resflags(NValue(0))
                    self.writeoperand(i.operands[0], NValue(0))
                else:
                    self.resflags(a^b)
                    self.writeoperand(i.operands[0], a ^ b)
                self.carry = NValue(0)
            elif m == "mov":
                self.writeoperand(i.operands[0], self.readoperand(i.operands[1]))
            elif m == "movzx":
                self.writeoperand(i.operands[0], self.readoperand(i.operands[1])) 
            elif m == "shr":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.resflags(a>>b)
                self.writeoperand(i.operands[0], a >> b)           
            elif m == "setb":
                self.writeoperand(i.operands[0], self.carry)
            elif m == "cmovb":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], self.cond_b().If(b,a ))
            elif m == "cmovnb"  or m == "cmovae":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], (~self.cond_b()).If(b,a))
            elif m == "cmova":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], self.cond_a().If(b,a ))
            elif m == "cmovna" or m == "cmovbe":
                a = self.readoperand(i.operands[0])
                b = self.readoperand(i.operands[1])
                self.writeoperand(i.operands[0], (~self.cond_a()).If(b,a ))
                
                
            elif m == "rep stosq":
                while(self.regs[X86_REG_RCX].as_long() != 0):
                    self.regs[X86_REG_RCX] -= 1
#                    print "store ", expr(self.regs[X86_REG_RDI])
                    self.writemem(self.regs[X86_REG_RDI], self.regs[X86_REG_RAX])
                    self.regs[X86_REG_RDI] += 8
            elif m == "rep movsq":
                while(self.regs[X86_REG_RCX].as_long() != 0):
                    #TODO handle direction flag
                    self.regs[X86_REG_RCX] -= 1
#                    print "store ", expr(self.regs[X86_REG_RDI])
#                    print "load ", expr(self.regs[X86_REG_RSI])
                    self.writemem(self.regs[X86_REG_RDI], self.readmem(self.regs[X86_REG_RSI]))
                    self.regs[X86_REG_RDI] += 8
                    self.regs[X86_REG_RSI] += 8
            elif m == "movdqa" or m == "movaps": # Only microarchitectural differences
                embed
                self.writeoperand(i.operands[0],self.readoperand(i.operands[1]))
            elif m == "clc":
                self.carry = False
            elif m == "rcl" and  len(i.operands)== 1:
                x = self.readoperand(i.operands[0])
                oldcarry = self.carry
                self.carry = (x.extract(63,63) == NValue(1,1))
                self.writeoperand(i.operands[0], (x << 1) | oldcarry )
            else:
                print "unknown ", m, " ", i.op_str
                
            self.ip = nextip

symbol = "_Z15fp_mul_comba_16P6fp_intS0_S0_"
x = Input(sys.argv[1])
m = X86Machine()

m.checkfunction(x.ins,x.sym['main'], x.offset,True )
