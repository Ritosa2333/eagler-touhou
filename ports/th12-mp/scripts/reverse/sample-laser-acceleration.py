"""Original laser4/8 initialization and virtual acceleration callbacks."""
import pathlib,sys,struct,json,math
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));rows=[]
cases=[(0,4,10,.15,-999.,1.),(2,4,10,.15,.5,1.),(0,4,0,.2,.4,1.),
       (2,4,5,-.2,999.,.5),(0,4,4,.3,-999.,1.5),
       (2,8,10,.15,.12,1.),(2,8,5,-.2,-.25,.5),(2,8,4,.3,.9,1.5),
       (2,8,0,.15,.1,1.),(0,8,5,.15,.1,1.)]
for kind,extension,duration,a,b,rate in cases:
  mu=Uc(UC_ARCH_X86,UC_MODE_32)
  for addr,n in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x10000)]:mu.mem_map(addr,n)
  mu.mem_write(0x400000,pe.get_memory_mapped_image());mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  obj,player,stack,stop=0x2000000,0x2003000,0x100e000,0x100f000
  put=lambda at,f,*v:mu.mem_write(at,struct.pack('<'+f,*v))
  get=lambda at,f:list(struct.unpack('<'+f,mu.mem_read(at,struct.calcsize('<'+f))))
  put(0x4b2ed0,'f',rate);put(0x4b4514,'I',player);put(player+0x97c,'3f',22,200,0)
  angle=struct.unpack('<f',struct.pack('<f',.33))[0]
  put(obj+0x50,'3f',12,100,0);put(obj+0x5c,'3f',math.cos(angle)*2.5,math.sin(angle)*2.5,0)
  put(obj+0x68,'f',.33);put(obj+0x74,'f',2.5);put(obj+0x638,'i',-1)
  put(obj+(0x47c if kind==2 else 0x484),'ffiIIi',a,b,duration,0,extension,0)
  def call(addr):
    put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_ECX,obj)
    mu.emu_start(addr,stop,count=100000)
    if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native laser extension budget')
  def snapshot():
    timer=obj+(0xb8 if extension==4 else 0xec)
    return {'speed':get(obj+0x74,'f')[0],'angle':get(obj+0x68,'f')[0],
            'velocity':get(obj+0x5c,'3f'),'active':get(obj+0x430,'I')[0],
            'cursor':get(obj+0x42c,'i')[0],'timer':get(timer+4,'if')}
  call(0x428b00 if kind==0 else 0x42d030);frames=[snapshot()]
  for _ in range(24):
    if get(obj+0x430,'I')[0]&extension:
      callback=0x429520 if extension==4 and kind==0 else 0x42d890 if extension==4 else 0x42d800 if kind==2 else 0x427e30
      call(callback)
    frames.append(snapshot())
  rows.append({'kind':kind,'extension':extension,'duration':duration,'a':a,'b':b,'rate':rate,'frames':frames})
out=ROOT/'tests/fixtures/laser-acceleration-original.json';out.write_text(json.dumps({'schema':'th12-original-laser-acceleration/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'provider':'Original428b00/42d030 startup,429520/42d890/42d800/427e30 virtual callbacks; no hooks','rows':rows},indent=2)+'\n',encoding='utf8')
print(json.dumps({'cases':len(rows),'snapshots':sum(len(r['frames']) for r in rows)}))
