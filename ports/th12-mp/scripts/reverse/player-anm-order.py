"""Read-only original weapon animation allocation and primary callback ordering.

Uses original ANM logic/resources/list scheduling. Host allocation is a fixture;
no renderer, original files, game golden or replay is written.
"""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
records=[]
for scenario in ['reimuB','sanaeBLeaves']:
 v=Uc(UC_ARCH_X86,UC_MODE_32)
 for base,size in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x1000000),(0x3000000,0x1000000),(0x5000000,0x1000000)]:v.mem_map(base,size)
 v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f)
 PLAYER=0x2000000;ORBS=0x2200000;POS=0x2300000;OUT=POS+100;EFFECTS=0x2400000;MANAGER=0x3000000;SP=0x100e000;STOP=0x100f000
 def put(a,f,*values):v.mem_write(a,struct.pack('<'+f,*values))
 def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
 put(0x4ce8cc,'I',MANAGER);put(0x4b4514,'I',PLAYER);put(0x4b43d4,'I',EFFECTS);put(0x4b2ed0,'f',1);put(0x4d52d4,'I',1);put(0x4ce568,'II',0x1234,0);put(PLAYER+0x97c,'fff',20,300,0);put(POS,'ffffff',20,300,0,0,0,0)
 pl=install_resource(v,(ROOT/'local/retail/pl00.anm').read_bytes(),0x5000000,1);bullet=install_resource(v,(ROOT/'local/retail/bullet.anm').read_bytes(),0x5800000,2)
 put(PLAYER+16,'I',pl['resource']);put(EFFECTS+16,'I',bullet['resource'])
 heap=0x2800000;frame=-1;events=[]
 def native_hook(v,a,size,data):
  global heap
  sp=v.reg_read(UC_X86_REG_ESP)
  if a in [0x46c9ea,0x46d04a]:
   amount=get(sp+4,'I')[0];out=heap;heap+=(amount+15)&~15;target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4);v.reg_write(UC_X86_REG_EAX,out);v.reg_write(UC_X86_REG_EIP,target)
  elif a in [0x46c941,0x46ca4f]:
   target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4);v.reg_write(UC_X86_REG_EIP,target)
  elif a==0x454d10:events.append({'frame':frame,'event':'bind','vm':get(sp+4,'I')[0],'script':get(sp+8,'i')[0],'bank':get(v.reg_read(UC_X86_REG_ECX),'H')[0],'rng':get(0x4ce568,'II')})
  elif a==0x4103f0:
   vm=v.reg_read(UC_X86_REG_ECX);leaf=get(vm+0x478,'I')[0];events.append({'frame':frame,'event':'leafUpdate','id':get(vm,'I')[0],'age':get(leaf+0xc8,'i')[0],'rng':get(0x4ce568,'II')})
 for a in [0x46c9ea,0x46d04a,0x46c941,0x46ca4f,0x454d10,0x4103f0]:v.hook_add(UC_HOOK_CODE,native_hook,begin=a,end=a)
 def call(a,args=(),registers=None):
  put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
  for reg,value in (registers or {}).items():v.reg_write(reg,value)
  try:v.emu_start(a,STOP,count=2000000)
  except Exception as e:raise RuntimeError(f'{scenario}: native {a:08x} EIP={v.reg_read(UC_X86_REG_EIP):08x}') from e
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native budget exceeded')
 def nodes():
  n=get(MANAGER+0x8856b8,'I')[0];result=[]
  while n:
   vm,n=get(n,'II');leaf=get(vm+0x478,'I')[0];result.append({'id':get(vm,'I')[0],'script':get(vm+0x3ea,'h')[0],'clock':get(vm+0x6c,'i')[0],'custom':get(vm+0x488,'I')[0],'leafAge':get(leaf+0xc8,'i')[0] if leaf else None,'leafHeading':get(leaf+0xc0,'f')[0] if leaf else None})
   if len(result)>100:raise RuntimeError('Unexpected list size')
  return result
 births=[];frames=[]
 if scenario=='reimuB':
  for index in range(8):
   before=get(0x4ce568,'II');orb=ORBS+index*0xb4;call(0x407ea0,[index],{UC_X86_REG_EAX:PLAYER+0x97c,UC_X86_REG_ESI:orb});births.append({'index':index,'before':before,'after':get(0x4ce568,'II'),'list':nodes()})
  frames.append({'frame':-1,'rng':get(0x4ce568,'II'),'list':nodes()})
  frame=0;call(0x460fe0,registers={UC_X86_REG_EDI:MANAGER});frames.append({'frame':0,'rng':get(0x4ce568,'II'),'list':nodes()})
 else:
  for _ in range(2):call(0x40fbe0,[OUT,5,POS])
  frames.append({'frame':-1,'rng':get(0x4ce568,'II'),'list':nodes()})
  for frame in range(18):
   if frame==1:call(0x40fbe0,[OUT,5,POS])
   call(0x460fe0,registers={UC_X86_REG_EDI:MANAGER});frames.append({'frame':frame,'rng':get(0x4ce568,'II'),'list':nodes()})
 records.append({'scenario':scenario,'births':births,'frames':frames,'events':events})
out=ROOT/'local/reverse/player-anm-order.json';out.write_text(json.dumps({'schema':'th12-native-player-anm-order/1','originalExeSha256':EXE_SHA256,'partial':True,'wholeGameOracle':False,'hostFixtures':['46c9ea/46d04a synthetic zero initialized heap;46c941/46ca4f free no-op','GPU-free original pl00/bullet ANM tables; no logic allocation/registration/bind/tick intercepted'],'records':records},indent=2)+'\n');print(out)
