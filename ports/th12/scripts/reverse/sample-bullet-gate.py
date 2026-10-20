"""Native4067e0 then40c480, d0 timer profile only; no hooks."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));rows=[]
for duration in [-2,0,1,2,5,30,120]:
  for rate in [.5,1.,1.5]:
    mu=Uc(UC_ARCH_X86,UC_MODE_32)
    for a,n in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x10000)]:mu.mem_map(a,n)
    mu.mem_write(0x400000,pe.get_memory_mapped_image());mu.reg_write(UC_X86_REG_FPCW,0x37f)
    put=lambda a,f,*v:mu.mem_write(a,struct.pack('<'+f,*v))
    get=lambda a,f:list(struct.unpack('<'+f,mu.mem_read(a,struct.calcsize('<'+f))))
    obj,stack,stop=0x2000000,0x100e000,0x100f000;put(0x4b2ed0,'f',rate)
    put(stack,'2I',stop,duration&0xffffffff);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EAX,obj+0x970)
    mu.emu_start(0x4067e0,stop,count=10000);put(obj+0x528,'I',0x400);frames=[]
    for tick in range(max(4,int(max(0,duration)/rate)+2)):
      active=get(obj+0x528,'I')[0]
      result=0
      if active&0x400:
        put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDI,obj)
        mu.emu_start(0x40c480,stop,count=10000)
        if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native400 budget')
        result=mu.reg_read(UC_X86_REG_EAX)
      frames.append({'tick':tick+1,'remaining':get(obj+0x974,'i')[0],'value':get(obj+0x978,'f')[0],'active':get(obj+0x528,'I')[0],'completed':result})
    rows.append({'duration':duration,'rate':rate,'frames':frames})
out=ROOT/'tests/fixtures/bullet-gate-original.json';out.write_text(json.dumps({'schema':'th12-original-bullet-gate/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'provider':'Original4067e0 and40c480 with Record.d0; no candidate/ANM/geometry hooks','rows':rows},indent=2)+'\n',encoding='utf8')
print(json.dumps({'cases':len(rows),'frames':sum(len(r['frames']) for r in rows)}))
