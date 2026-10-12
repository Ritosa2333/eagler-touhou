"""Observe retail41a9f0 target eligibility with isolated list fixtures."""
import hashlib,json,pathlib,struct,sys
root=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_FPCW,UC_X86_REG_ESP,UC_X86_REG_EDI,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_EDX
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
alternative=heap+0x6000;projectile=heap+0x9000
put(node,'II',enemy,node+0x20);put(node+0x20,'II',alternative,0)
put(enemy+0x1074,'fff',-80,100,0);put(enemy+0x27b4,'I',91)
put(alternative+0x1074,'fff',80,100,0);put(alternative+0x27b4,'I',92)
updates=[]
for flags in [1,0x20,0x2000000,0x4000000]:
  mu.mem_write(projectile,bytes(0x78));put(enemy+0x26f8,'I',flags)
  put(projectile+4,'i',10);put(projectile+0x14,'fff',0,300,0)
  put(projectile+0x2c,'ff',8,-1.5707963705062866);put(projectile+0x48,'i',1)
  put(projectile+0x54,'I',enemy);put(projectile+0x64,'I',91)
  frames=[]
  for tick in range(2):
    put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDX,projectile)
    mu.emu_start(0x43a480,stop,count=10000)
    if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Homing update did not return')
    read=lambda address,fmt:struct.unpack('<'+fmt,mu.mem_read(address,struct.calcsize('<'+fmt)))
    frames.append({'tick':tick,'speed':read(projectile+0x2c,'f')[0],'angle':read(projectile+0x30,'f')[0],'targetPointerPresent':bool(read(projectile+0x54,'I')[0]),'targetId':read(projectile+0x64,'I')[0]})
    put(projectile+4,'i',11)
  updates.append({'retainedTargetFlags':flags,'frames':frames})
out.write_text(json.dumps({'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'function':'41a9f0/43a480','scope':'Isolated original selector and homing update, with two controlled enemy-list entries; no native code or return replacement','samples':rows,'invalidRetainedTarget':updates},indent=2)+'\n',encoding='utf-8')
print(json.dumps(rows))
print(json.dumps(updates))
