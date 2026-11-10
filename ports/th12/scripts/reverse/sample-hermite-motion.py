"""Original325/326 dispatcher and movement prefix, no candidate math."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));rows=[]
for op,duration,args in [(325,10,[20.,-30.,50.,160.,-10.,40.]),
                         (326,10,[20.,-30.,50.,160.,-10.,40.]),
                         (325,60,[-2.,18.,-999999.,200.,7.,-4.]),
                         (325,1,[20.,30.,40.,50.,60.,70.]),
                         (325,0,[20.,30.,40.,50.,60.,70.]),
                         (326,7,[0.,0.,-999999.,-999999.,20.,-15.])]:
  mu=Uc(UC_ARCH_X86,UC_MODE_32)
  for a,n in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x40000)]:mu.mem_map(a,n)
  mu.mem_write(0x400000,pe.get_memory_mapped_image());mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  state,context,runner,instruction,stack,stop=0x2000000,0x2010000,0x2012000,0x2020000,0x100d000,0x100f000
  put=lambda a,f,*v:mu.mem_write(a,struct.pack('<'+f,*v))
  get=lambda a,f:list(struct.unpack('<'+f,mu.mem_read(a,struct.calcsize('<'+f))))
  put(0x4b2ed0,'f',1);put(0x4d52d4,'I',1);put(state+0x174c,'I',runner);put(runner+4,'I',context);put(context+4,'I',instruction)
  put(state+0x68,'3f',10,100,0);put(state+0x9c,'3f',3,-7,0);put(state+0x34,'3f',13,93,0)
  data=struct.pack('<i6f',duration,*args);mu.mem_write(instruction,struct.pack('<iHHHBBI',0,op,len(data)+16,0,63,7,0)+data)
  put(stack+0x34,'I',state);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EAX,op);mu.reg_write(UC_X86_REG_EBX,state);mu.reg_write(UC_X86_REG_ESI,runner)
  mu.emu_start(0x416831,0x41962b,count=100000)
  if mu.reg_read(UC_X86_REG_EIP)!=0x41962b:raise RuntimeError('Native325 dispatch budget')
  frames=[]
  for tick in range(max(4,duration+2)):
    put(state+0x16b8,'I',0);put(stack,'II',stop,state);mu.reg_write(UC_X86_REG_ESP,stack)
    mu.emu_start(0x413840,0x413ac5,count=100000)
    if mu.reg_read(UC_X86_REG_EIP)!=0x413ac5:raise RuntimeError('Native motion budget')
    frames.append({'tick':tick+1,'absolute':get(state+0x68,'2f'),'relative':get(state+0x9c,'2f'),'position':get(state+0x34,'2f')})
  rows.append({'op':op,'duration':duration,'args':args,'absolute':[10,100],'relative':[3,-7],'frames':frames})
out=ROOT/'tests/fixtures/hermite-motion-original.json';out.write_text(json.dumps({'schema':'th12-original-hermite-enemy-motion/1','originalExeSha256':EXE_SHA256,'provider':'Original325/326 dispatch416831,406340 and movement413840..413ac5; no hooks','wholeGameOracle':False,'rows':rows},indent=2)+'\n',encoding='utf8')
print(json.dumps({'cases':len(rows),'frames':sum(len(r['frames']) for r in rows)}))
