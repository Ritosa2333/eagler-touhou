"""Original TH12 movement instructions, actual 308..311 dispatch, and tick geometry.

Stops before animation/collision callbacks. This is partial native evidence, not
a whole-game provider; no candidate rules execute inside the diagnostic VM.
"""
import pathlib,sys,struct,json,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_MXCSR,UC_X86_REG_FPCW
EXE=WORKSPACE/'th12/th12.exe';SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256(EXE.read_bytes()).hexdigest()!=SHA:raise SystemExit('Wrong TH12 version')
pe=pefile.PE(str(EXE));STOP=0x100f000;STATE=0x2000000;CONTEXT=0x2010000;RUNNER=0x2012000;INSTRUCTION=0x2020000;SP=0x100d000
def put(v,a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
def get(v,a,f):return struct.unpack('<'+f,v.mem_read(a,struct.calcsize(f)))
def vm_new():
 v=Uc(UC_ARCH_X86,UC_MODE_32);v.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);v.mem_write(0x400000,pe.get_memory_mapped_image());v.mem_map(0x1000000,0x10000);v.mem_map(0x2000000,0x40000)
 put(v,0x4b2ed0,'f',1);put(v,0x4d52d4,'I',1) # Native CPU SSE2 availability; avoids CRT exception-service plumbing.
 v.reg_write(UC_X86_REG_MXCSR,0x1f80);v.reg_write(UC_X86_REG_FPCW,0x037f)
 put(v,STATE+0x174c,'I',RUNNER);put(v,RUNNER+4,'I',CONTEXT);put(v,CONTEXT+4,'I',INSTRUCTION)
 return v
def dispatch(v,op,args):
 data=b''.join(struct.pack('<f' if isinstance(x,float) else '<i',x) for x in args)
 v.mem_write(INSTRUCTION,struct.pack('<iHHHBBI',0,op,16+len(data),0,63,len(args),0)+data)
 put(v,SP+0x34,'I',STATE);v.reg_write(UC_X86_REG_ESP,SP);v.reg_write(UC_X86_REG_EAX,op);v.reg_write(UC_X86_REG_EBX,STATE);v.reg_write(UC_X86_REG_ESI,RUNNER)
 v.emu_start(0x416d06 if op in [308,310] else 0x416e03,0x41962b,count=100000)
def movement(v,off):
 return {'position':get(v,STATE+off,'fff'),'velocityOrCenter':get(v,STATE+off+0xc,'fff'),'speed':get(v,STATE+off+0x18,'f')[0],'angle':get(v,STATE+off+0x1c,'f')[0],'radius':get(v,STATE+off+0x20,'f')[0],'radiusDelta':get(v,STATE+off+0x24,'f')[0],'flags':get(v,STATE+off+0x30,'I')[0]}
quant=[]
for x in [-1.2345,-.015,-.005,-.0001,0.,.0001,.005,.0099999988,.0099999998,.015,1.2345]:
 v=vm_new();put(v,STATE,'ff',x,-x);put(v,SP,'I',STOP);v.reg_write(UC_X86_REG_ESP,SP);v.reg_write(UC_X86_REG_ESI,STATE);v.emu_start(0x465320,STOP,count=100000);quant.append({'input':x,'output':get(v,STATE,'ff')})
cases=[]
for name,angle,radius,radius_delta,target_speed,target_radius,target_delta,duration,mode,relative in [
 ('ufo_relative_radius60',0.,0.,0.,.034906585,16.,0.,60,0,True),
 ('ufo_relative_wrap60',1.,0.,0.,.034906585,16.,0.,60,0,True),
 ('absolute_radius_and_delta',.33,3.,.125,.06,15.,-.125,10,4,False),
]:
 v=vm_new();put(v,STATE+0x68,'fff',0,128,0);put(v,STATE+0x34,'fff',0,128,0)
 dispatch(v,310 if relative else 308,[angle,.034906585 if relative else .02,radius,radius_delta]);after_set={'absolute':movement(v,0x68),'relative':movement(v,0x9c),'combined':movement(v,0x34)}
 dispatch(v,311 if relative else 309,[duration,mode,target_speed,target_radius,target_delta]);after_interpolation={'absolute':movement(v,0x68),'relative':movement(v,0x9c),'combined':movement(v,0x34)}
 frames=[]
 for tick in range(1,121):
  put(v,STATE+0x16b8,'I',0);put(v,SP,'II',STOP,STATE);v.reg_write(UC_X86_REG_ESP,SP);v.emu_start(0x413840,0x413ac5,count=100000)
  if tick in [1,2,5,10,11,29,30,59,60,61,62,120]:frames.append({'tick':tick,'absolute':movement(v,0x68),'relative':movement(v,0x9c),'combined':movement(v,0x34),'absolutePolarDuration':get(v,STATE+0x358,'i')[0],'relativePolarDuration':get(v,STATE+0x394,'i')[0],'absoluteRadiusDuration':get(v,STATE+0x3d0,'i')[0],'relativeRadiusDuration':get(v,STATE+0x40c,'i')[0]})
 cases.append({'name':name,'args':{'angle':angle,'initialRadius':radius,'initialRadiusDelta':radius_delta,'initialSpeed':.034906585 if relative else .02,'targetSpeed':target_speed,'targetRadius':target_radius,'targetDelta':target_delta,'duration':duration,'mode':mode,'relative':relative},'afterSet':after_set,'afterInterpolation':after_interpolation,'frames':frames})
out=TITLE/'artifacts/reverse/enemy-motion-inspection.json';out.write_text(json.dumps({'schema':'th12-enemy-motion-inspection/1','originalExeSha256':SHA,'scope':'native movement dispatch and full update geometry before callbacks','wholeGameDeterminism':False,'quantization':quant,'cases':cases},indent=2)+'\n',encoding='utf-8');print(out)
