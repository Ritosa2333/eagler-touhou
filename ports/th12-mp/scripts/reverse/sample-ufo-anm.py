"""Original retail UFO sub-animation ticks, excluding graph/GPU reconstruction."""
import hashlib,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_INVALID
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
cases=[('front',80,[(1,7),(2,11),(3,17)],45),('front',81,[(1,8),(2,12)],45),
 ('bullet',204,[(60,2),(100,1)],124),('bullet',205,[(60,2),(100,1)],124),
 ('bullet',206,[(60,2),(100,1)],124),
 ('bullet',211,[(1,7),(41,1)],48),('bullet',211,[(1,8),(41,1)],48),('bullet',211,[(1,9),(41,1)],48)]
for name,script,interrupts,ticks in cases:
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
 mu.mem_map(0x1000000,0x10000);mu.mem_map(0x2000000,0x600000)
 obj,resource,manager,stack,stop=0x2000000,0x2010000,0x2500000,0x100e000,0x100f000
 raw=(ROOT/'local/retail'/f'{name}.anm').read_bytes();install_resource(mu,raw,resource)
 ascii_resource,ascii_manager=0x2300000,0x2520000
 install_resource(mu,(ROOT/'local/retail/ascii.anm').read_bytes(),ascii_resource)
 put=lambda at,fmt,*v:mu.mem_write(at,struct.pack('<'+fmt,*v))
 read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
 put(0x4ce8cc,'I',manager);put(0x4b43b8,'I',ascii_manager);put(ascii_manager+0x18fb4,'I',ascii_resource);put(0x4b2ed0,'f',1);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0)
 mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
 def hook(uc,address,size,data):
  if address==0x46d04a:
   sp=mu.reg_read(UC_X86_REG_ESP);amount=read(sp+4,'I')[0]
   if amount>0x10000:raise RuntimeError('Geometry allocation fixture budget')
   mu.reg_write(UC_X86_REG_EAX,manager+0x4000);mu.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4)
 def invalid(uc,access,address,size,value,data):
  print(f'{name}:{script} original read {address:x} from {mu.reg_read(UC_X86_REG_EIP):x}',flush=True);return False
 mu.hook_add(UC_HOOK_CODE,hook);mu.hook_add(UC_HOOK_MEM_INVALID,invalid)
 def call(at,args):
  put(stack,'I'*(len(args)+1),stop,*args);mu.reg_write(UC_X86_REG_ESP,stack)
  if at==0x454d10:mu.reg_write(UC_X86_REG_ECX,resource)
  mu.emu_start(at,stop,count=200000)
  if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Native ANM budget {name}:{script}')
 call(0x454d10,[obj,script]);frames=[]
 for tick in range(ticks):
  for when,code in interrupts:
   if when==tick:put(obj+0x3c4,'h',code)
  if tick:call(0x455630,[obj])
  count=read(obj+0x3fc,'i')[0];vertices=read(obj+0x478,'I')[0]
  vertex_words=read(vertices,'I'*(count*14)) if vertices and 0<count<=64 else []
  frames.append({'tick':tick,'clock':read(obj+0x6c,'i')[0],'scriptPosition':read(obj+0x424,'3f'),
   'offset':read(obj+0x43c,'3f'),'rotation':read(obj+0x24,'3f'),'scale':read(obj+0x40,'2f'),
   'uV':read(obj+0x60,'2f'),'primaryARGB':read(obj+0x3bc,'I')[0],'secondaryARGB':read(obj+0x3c0,'I')[0],
   'sprite':read(obj+0x3e4,'h')[0],'layer':read(obj+0x20,'i')[0],'flags':read(obj+0x47c,'I')[0],
   'ended':read(obj+0x3f0,'I')[0]==0,'geometryCount':count,'geometryVertexWords':vertex_words,
   'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II')})
 records.append({'resource':name+'.anm','resourceSha256':hashlib.sha256(raw).hexdigest(),'scriptOrdinal':script,'interrupts':interrupts,'frames':frames})
out=ROOT/'artifacts/reverse/ufo-anm-original.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-ufo-anm-samples/1','originalExeSha256':EXE_SHA256,
 'provider':'Actual454d10/454b80/455630 using unmodified retail ANM and GPU-free resource records; no child factory/candidate interpreter.',
 'fixtureCallbacks':['46d04a malloc returns a mapped bounded geometry buffer; original allocator/heap is not executed.'],
 'wholeGameOracle':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'scripts':len(records),'frames':sum(len(r['frames']) for r in records)}))
