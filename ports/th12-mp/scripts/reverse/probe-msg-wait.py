"""Original MSG control-clock fixture; only sound is intercepted, never logic."""
import pathlib,sys,struct,hashlib,json
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_FPCW,UC_X86_REG_MXCSR
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest();assert digest=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));word=lambda v:struct.pack('<I',v&0xffffffff)
def instruction(time,op,payload=b''):return struct.pack('<HBB',time,op,len(payload))+payload
rows=[]
for mode in ('wait','pressed-z','pressed-enter','held-z','held-control','control-disabled'):
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
 STACK,VM,CODE,STOP=0x1000000,0x2000000,0x3000000,0x100f000
 for address in (STACK,VM,CODE):mu.mem_map(address,0x10000)
 def return_sound(uc,address,size,user):
  sp=uc.reg_read(UC_X86_REG_ESP);target=struct.unpack('<I',uc.mem_read(sp,4))[0];uc.reg_write(UC_X86_REG_EIP,target);uc.reg_write(UC_X86_REG_ESP,sp+4)
 mu.hook_add(UC_HOOK_CODE,return_sound,begin=0x453d90,end=0x453d90)
 mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
 mu.mem_write(0x4b2ed0,struct.pack('<f',1))
 mu.mem_write(VM+0x18,struct.pack('<ii f II',-1,0,0,0x4b2ed0,1));mu.mem_write(VM+0x2c,struct.pack('<ii f II',-1,0,0,0x4b2ed0,1));mu.mem_write(VM+0x64,word(CODE));mu.mem_write(VM+0x90,word(1))
 mu.mem_write(CODE,instruction(0,10,b'\0' if mode=='control-disabled' else b'\1')+instruction(0,11,word(5))+instruction(1,0))
 samples=[]
 for tick in range(9):
  held=0x200 if mode in ('held-control','control-disabled') and tick==2 else 1 if mode=='held-z' and tick==2 else 0
  pressed=1 if mode=='pressed-z' and tick==2 else 0x80000 if mode=='pressed-enter' and tick==2 else 0
  mu.mem_write(0x4d49d0,word(held));mu.mem_write(0x4d49dc,word(pressed));sp=STACK+0xe000;mu.mem_write(sp,word(STOP)+word(VM));mu.reg_write(UC_X86_REG_ESP,sp)
  mu.emu_start(0x41fcc0,STOP,count=100000)
  if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('MSG instruction budget exceeded')
  samples.append({'tick':tick,'return':mu.reg_read(UC_X86_REG_EAX),'clock':struct.unpack('<i',mu.mem_read(VM+0x1c,4))[0],'wait':struct.unpack('<i',mu.mem_read(VM+0x30,4))[0],'pc':struct.unpack('<I',mu.mem_read(VM+0x64,4))[0]-CODE})
  if samples[-1]['return']==0xffffffff:break
 rows.append({'mode':mode,'samples':samples})
out=TITLE/'artifacts/reverse/msg/wait-samples.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps({'exeSha256':digest,'entry':'0x41fcc0','scope':'MSG control-clock only; sound no-op','wholeGameOracle':False,'cases':rows},indent=2)+'\n');print(json.dumps(rows,indent=2))
