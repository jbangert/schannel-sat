#!/usr/bin/python2
import pickle

class wrap_mem:
    def __init__(self,m):
        self.base = m.base
        self.disp = m.disp
        self.index = m.index
        self.scale = m.scale
class wrap_oper:
    def __init__(self,o):
        self.type = o.type
        self.reg = o.reg
        self.imm = o.imm
        self.mem = wrap_mem(o.mem)
class wrap_inst:
    def __init__(self, c):
        self.offset = c.offset
        self.size = c.size
        self.mnemonic = c.mnemonic
        self.op_str = c.op_str
        self.op_size = c.op_size
        self.operands = map(lambda x: wrap_oper(x),c.operands)
class input_wrapper:
    def __init__(self, o):
        self.sym = o.sym
        self.offset = o.offset
        self.ins = my_dictionary = {k: wrap_inst(v) for k, v in o.ins.iteritems()}
if __name__ == "__main__":
    from input import Input
    import sys
    x= input_wrapper(Input(sys.argv[1]))
    print pickle.dumps(x)
