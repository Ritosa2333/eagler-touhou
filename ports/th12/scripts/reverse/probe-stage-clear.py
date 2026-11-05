"""Original stage award arithmetic; excludes stage transition and game execution."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest()
assert digest=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32);v.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);v.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,HUD,STOP=0x1000000,0x2000000,0x100f000;v.mem_map(STACK,0x10000);v.mem_map(HUD,0x10000);v.reg_write(UC_X86_REG_FPCW,0x37f)
def put(at,fmt,*values):v.mem_write(at,struct.pack('<'+fmt,*values))
def get(at,fmt):return list(struct.unpack('<'+fmt,v.mem_read(at,struct.calcsize('<'+fmt))))
bindings=[]
def hook(uc,address,size,user):
 sp=uc.reg_read(UC_X86_REG_ESP);bindings.append({'resource':get(sp+4,'I')[0],'script':get(sp+12,'i')[0],'layer':uc.reg_read(UC_X86_REG_EAX)});put(get(sp+8,'I')[0],'I',17);target=get(sp,'I')[0];uc.reg_write(UC_X86_REG_EAX,17);uc.reg_write(UC_X86_REG_EIP,target);uc.reg_write(UC_X86_REG_ESP,sp+20)
v.hook_add(UC_HOOK_CODE,hook,begin=0x4615a0,end=0x4615a0)
def reset(stage,difficulty,score,power=400,lives=2,bombs=3,piv=1000000):
 v.mem_write(HUD,bytes(0x10000));put(0x4b43e4,'I',HUD);put(HUD+0x6d44,'I',123);put(0x4b0cb0,'i',stage);put(0x4b0ca8,'i',difficulty);put(0x4b0c44,'i',score);put(0x4b0c48,'i',power);put(0x4b0c98,'i',lives);put(0x4b0ca0,'i',bombs);put(0x4b0c78,'i',piv);bindings.clear()
def call(address,stop=STOP):
 sp=STACK+0xe000;put(sp,'I',STOP);v.reg_write(UC_X86_REG_ESP,sp);v.emu_start(address,stop,count=100000)
 if v.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native instruction budget')
base=[]
for stage in range(1,8):
 for score in [0,999950000]:
  reset(stage,1,score);call(0x421350);base.append({'stage':stage,'scoreBefore':score,'scoreAfter':get(0x4b0c44,'i')[0],'displayAward':get(HUD+0x6d48,'i')[0],'flags':get(HUD+0x6d18,'I')[0],'bindings':bindings.copy()})
final=[]
for stage in [6,7]:
 for difficulty in range(5):
  for power,lives,bombs,piv in [(400,2,3,1000000),(137,0,1,1234567)]:
   reset(stage,difficulty,10,power,lives,bombs,piv)
   call(0x420adb if stage==6 else 0x420c1f,0x420bb7 if stage==6 else 0x420c65)
   final.append({'stage':stage,'difficulty':difficulty,'power':power,'lives':lives,'bombs':bombs,'pointValueStored':piv,'scoreBefore':10,'scoreAfter':get(0x4b0c44,'i')[0],'displayAward':v.reg_read(UC_X86_REG_ESI)})
out=TITLE/'tests/fixtures/stage-clear-original.json';out.write_text(json.dumps({'originalExeSha256':digest,'provider':'421350; isolated retail opcode21 final award branch plus421880/40e730','scope':'Stage award arithmetic only; no full MSG or next-stage execution','wholeGameOracle':False,'hooks':['4615a0 records graphical binding only'],'base':base,'final':final},indent=2)+'\n',encoding='utf-8');print(out)
