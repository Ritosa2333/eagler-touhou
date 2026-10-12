"""Observe retail41a9f0 target eligibility with isolated list fixtures."""
import hashlib,json,pathlib,struct,sys
root=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_FPCW,UC_X86_REG_ESP,UC_X86_REG_EDI,UC_X86_REG_EIP,UC_X86_REG_EAX
exe=root.parent/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32)
mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
heap=0x2000000;mu.mem_map(heap,0x20000)
def put(address,fmt,*values):mu.mem_write(address,struct.pack('<'+fmt,*values))
manager=heap;node=heap+0x1000;enemy=heap+0x2000;position=heap+0x5000;stack=heap+0xf000;stop=heap+0x10000
put(0x4b43dc,'I',manager);put(manager+0x68,'I',node);put(node,'II',enemy,0)
put(position,'fff',0,0,0);put(enemy+0x1074,'fff',20,40,0)
mu.reg_write(UC_X86_REG_FPCW,0x7f)
rows=[]
for flags in [0,0x10,1,0x20,0x2000000,0x4000000]:
  put(enemy+0x26f8,'I',flags);put(stack,'If',stop,256)
  mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDI,position)
  mu.emu_start(0x41a9f0,stop,count=1000)
  if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Target selector did not return')
  rows.append({'flags':flags,'accepted':mu.reg_read(UC_X86_REG_EAX)==enemy})
out=root/'artifacts/reverse/player-targeting-native.json'
out.write_text(json.dumps({'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'function':'41a9f0','scope':'Isolated original selector, without game input/RNG/target replacement','samples':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps(rows))
