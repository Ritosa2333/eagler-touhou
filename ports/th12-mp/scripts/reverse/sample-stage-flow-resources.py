"""Selected retail scene constructor/continue/RNG boundary samples, no game run.

The fixture samples actual 421dde resource initialization,421eb1 power,
421f6d rank,4230a0 continue digit and43c730 new-stage seed/counter boundary.
It does not claim to execute native scene loading or all120 clear callbacks.
"""
import hashlib,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
EXE=ROOT.parent/'th12/th12.exe'
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
assert hashlib.sha256(EXE.read_bytes()).hexdigest()==SHA
pe=pefile.PE(str(EXE));mu=Uc(UC_ARCH_X86,UC_MODE_32)
mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
mu.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,SCENE,REPLAY,HEADER=0x1000000,0x2000000,0x2001000,0x2002000
mu.mem_map(STACK,0x10000);mu.mem_map(SCENE,0x10000)
def put(p,f,*v):mu.mem_write(p,struct.pack('<'+f,*v))
def get(p,f):return list(struct.unpack('<'+f,mu.mem_read(p,struct.calcsize('<'+f))))
def execute(address,stop):
 put(STACK+0xe000,'I',STACK+0xf000);mu.reg_write(UC_X86_REG_ESP,STACK+0xe000)
 mu.emu_start(address,stop,count=100000)
 assert mu.reg_read(UC_X86_REG_EIP)==stop
def hook(uc,address,size,user):
 if address not in [0x46c9ea,0x477420]:return
 sp=uc.reg_read(UC_X86_REG_ESP)
 if address==0x46c9ea:uc.reg_write(UC_X86_REG_EAX,HEADER)
 else:
  target,value,count=get(sp+4,'3I');uc.mem_write(target,bytes([value&255])*count)
  uc.reg_write(UC_X86_REG_EAX,target)
 uc.reg_write(UC_X86_REG_EIP,get(sp,'I')[0]);uc.reg_write(UC_X86_REG_ESP,sp+4)
mu.hook_add(UC_HOOK_CODE,hook)
continues=[]
for stage in range(1,7):
 for difficulty in range(4):
  for count in [0,8,9]:
   mu.mem_write(0x4b0c40,bytes(0xb8));mu.mem_write(SCENE,bytes(0x1000))
   put(0x4b0cb0,'i',stage);put(0x4b0ca8,'i',difficulty)
   put(0x4b0cc4,'i',count);put(0x4b0ce0,'I',8)
   put(0x4b0cd0,'2i',400,100);put(0x4b0c48,'i',100)
   put(0x4b43e4,'I',0);put(0x4b0c44,'i',1234567);put(0x4b0cdc,'i',1234)
   put(0x4b0c9c,'i',3);put(0x4b0ca4,'i',2);put(0x4b0ccc,'i',123)
   put(0x4b0cd8,'i',9);put(0x4b0c4c,'5i',1,2,3,1,2)
   mu.reg_write(UC_X86_REG_FPCW,0x37f)
   mu.reg_write(UC_X86_REG_EAX,0x4b0c40);execute(0x4230a0,STACK+0xf000)
   mu.reg_write(UC_X86_REG_EBP,0);mu.reg_write(UC_X86_REG_EBX,2)
   mu.reg_write(UC_X86_REG_EDI,SCENE);mu.reg_write(UC_X86_REG_ECX,difficulty)
   execute(0x421dde,0x421ea4)
   execute(0x421eb1,0x421f25);execute(0x421f6d,0x421fa1)
   continues.append({'stage':stage,'difficulty':difficulty,'beforeCount':count,
    'count':get(0x4b0cc4,'i')[0],'scoreStored':get(0x4b0c44,'i')[0],
    'power':get(0x4b0c48,'i')[0],'lives':get(0x4b0c98,'i')[0],
    'bombs':get(0x4b0ca0,'i')[0],'piv':get(0x4b0c78,'i')[0],
    'graze':get(0x4b0cdc,'i')[0],'lifeFragments':get(0x4b0c9c,'i')[0],
    'bombFragments':get(0x4b0ca4,'i')[0],'rank':get(0x4b0ccc,'i')[0],
    'extendThreshold':get(0x4b0cd8,'i')[0],'inventory':get(0x4b0c4c,'5i')})
boundaries=[]
for stage in range(1,8):
 mu.mem_write(REPLAY,bytes(0x1000));put(0x4b4518,'I',REPLAY)
 put(0x4b0cb0,'i',stage);put(REPLAY+0x10,'i',0)
 put(0x4ce568,'H',0x3456);put(0x4ce56c,'I',986577);put(0x4cee4c,'I',0)
 execute(0x43c730,STACK+0xf000)
 boundaries.append({'stage':stage,'seed':get(0x4ce568,'H')[0],
  'calls':get(0x4ce56c,'I')[0],'recordedSeed':get(HEADER+2,'H')[0]})
out=ROOT/'tests/fixtures/stage-flow-resources-original.json'
out.write_text(json.dumps({'originalExeSha256':SHA,
 'scope':'Selected resource initialization/continue arithmetic and recording RNG boundary only; not complete scene progression',
 'wholeGameOracle':False,'hooks':['46c9ea memory allocation only','477420 memory set only'],
 'continue':continues,'stageRngBoundary':boundaries},indent=2)+'\n',encoding='utf-8')
print(out)
