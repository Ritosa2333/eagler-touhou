"""Actual TH12 STD12 camera/update sampling, with explicit host boundaries."""
import pathlib,sys,struct,json,math,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
for name,ticks in [('stage01',6001),('stage02',1201),('stage03',1201),('stage05',2801),('stage07',1201)]:
 v=Uc(UC_ARCH_X86,UC_MODE_32)
 for base,size in [(0,4096),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x40000)]:v.mem_map(base,size)
 v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f)
 STAGE,CODE,CONFIG,HEADER,HEAP,SP,STOP=0x2000000,0x2010000,0x2030000,0x2032000,0x2034000,0x100e000,0x100f000
 def put(a,f,*values):v.mem_write(a,struct.pack('<'+f,*values))
 def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
 def ret(n=0,value=0):
  sp=v.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4+n);v.reg_write(UC_X86_REG_EAX,value);v.reg_write(UC_X86_REG_EIP,target)
 def hook(v,a,size,data):
  if a==0x46c6bc:
   sp=v.reg_read(UC_X86_REG_ESP);dest,source=get(sp+4,'2I');xyz=get(source,'fff');length=math.sqrt(sum(n*n for n in xyz));put(dest,'fff',*(n/length if length else 0 for n in xyz));ret(8,dest)
  elif a==0x46c9ea:ret(0,HEAP)
  elif a==0x46ca4f:ret()
 for a in [0x46c6bc,0x46c9ea,0x46ca4f]:v.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
 def call(a,args=(),registers=None):
  put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
  for r,value in (registers or {}).items():v.reg_write(r,value)
  try:v.emu_start(a,STOP,count=200000)
  except Exception as e:raise RuntimeError(f'{name} EIP={v.reg_read(UC_X86_REG_EIP):08x}') from e
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native budget')
 put(CONFIG+0x590,'I',0x4000);call(0x430be0,registers={UC_X86_REG_ECX:CONFIG});v.mem_write(STAGE+0x360c,bytes(v.mem_read(0x4ced1c,0x118)))
 std=(ROOT/'local/retail'/f'{name}.std').read_bytes();start=struct.unpack_from('<I',std,8)[0];v.mem_write(CODE,std[start:]);put(STAGE+0x10,'I',HEADER);put(STAGE+0x1c,'I',CODE);put(STAGE+0x4c,'I',CODE);put(STAGE+0x38,'iifII',-1,0,0,0x4b2ed0,1);put(STAGE+0x35bc,'I',1);put(0x4b2ed0,'f',1);put(0x4b43c0,'I',STAGE)
 samples=[]
 for frame in range(ticks):
  call(0x403ec0,registers={UC_X86_REG_ECX:STAGE})
  if name=='stage01' or frame in [0,1,299,599,689,690,739,740,741,969,970,1199,1200,2563,2564,2565,2799,2800]:
   samples.append({'frame':frame,'clock':get(STAGE+0x3c,'i')[0],'callbackCount':get(STAGE+0x35d8,'i')[0],'pcOffset':get(STAGE+0x4c,'I')[0]-CODE,'position':get(STAGE+0x360c,'fff'),'direction':get(STAGE+0x3618,'fff'),'up':get(STAGE+0x3624,'fff'),'eyeOffset':get(STAGE+0x3630,'fff'),'offset':get(STAGE+0x3648,'fff'),'velocity':get(STAGE+0x36fc,'fff'),'fov':get(STAGE+0x3654,'f')[0],'fogRange':get(STAGE+0x3708,'ff'),'fogBytes':get(STAGE+0x3710,'ffff'),'fogArgb':get(STAGE+0x3720,'I')[0],'shakeMode':get(STAGE+0x20,'B')[0],'shakeAge':get(STAGE+0x28,'i')[0]})
 records.append({'name':name,'ticks':ticks,'stdSha256':hashlib.sha256(std).hexdigest(),'samples':samples})
out=ROOT/'local/reverse/stage-logic-probe.json';out.write_text(json.dumps({'schema':'th12-native-stage-logic/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'fixtures':['D3DXVec3Normalize uses explicit host arithmetic; not native DLL code','objectCount0 omits STD object ANM ticks; embedded8 VMs have null scripts','synthetic malloc/free; original4cea94=0 prevents background mesh construction; no GPU execution'],'records':records},indent=2)+'\n');print(out)
