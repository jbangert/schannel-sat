from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
import json
from capstone_leaky import *
from capstone_leaky.x86 import *
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
    def json(self):
        return json.dumps(self.ins)
