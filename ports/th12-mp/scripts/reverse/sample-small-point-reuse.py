"""Actual4273f0 type9 pool reuse and425c20 attraction overwrite.

Idle slot's previous +9bc is explicit fixture data. Original constructor,
timer and movement/automatic attraction execute; ANM/sound/HUD submissions
alone are inert host boundaries. No candidate/resources/replays are written.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
sys.path.insert(0,str(ROOT/'scripts/reverse'))
import pefile
from native_anm_resource import check_original,EXE_SHA256
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
from unicorn.x86_const import *
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);pe=pefile.PE(str(EXE))
MANAGER,PLAYER,UFO,SHT,POS,SP,STOP=0x2100000,0x2000000,0x2010000,0x2070000,0x2080000,0x100d000,0x100f000
records=[]
for kind in [9,1]:
 v=Uc(UC_ARCH_X86,UC_MODE_32)
 for base,size in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),
                   (0x2000000,0x100000),(MANAGER,0x670000),(0x2800000,0x500000)]:v.mem_map(base,size)
 v.mem_write(0x400000,pe.get_memory_mapped_image())
 def put(at,fmt,*values):v.mem_write(at,struct.pack('<'+fmt,*values))
 def get(at,fmt):return list(struct.unpack('<'+fmt,v.mem_read(at,struct.calcsize('<'+fmt))))
 for at,pointer in [(0x4b44f0,MANAGER),(0x4b4514,PLAYER),(0x4b4534,UFO),(0x4b43e4,0x2030000),
                    (0x4b43dc,0x2040000),(0x4b43c8,0x2800000)]:put(at,'I',pointer)
 put(0x2800000+0x4debdc,'I',0x2020000);put(0x4b2ed0,'f',1)
 put(PLAYER+0xa2c,'I',SHT);put(SHT+8,'f',5);put(PLAYER+0xa28,'i',1)
 put(PLAYER+0x97c,'3f',150,400,0)
 for off,half in [(0xc444,30),(0xc45c,50),(0xc474,30)]:put(PLAYER+off,'6f',150-half,400-half,0,150+half,400+half,0)
 item=MANAGER+(0x17afd4 if kind==9 else 0x14)
 stale=struct.unpack('<f',struct.pack('<I',0x41399994))[0]
 put(item+0x9bc,'f',stale);put(POS,'3f',-180,100,0)
 writes=[];intercepts={};tick=-1
 def intercept(mu,address,size,user):
  pops={0x43e250:8,0x453e20:4,0x453d90:0,0x4385b0:4,0x454d10:8,0x4061e0:8,
   0x455630:4,0x461970:4,0x461a70:4,0x402520:0,0x41ce60:12,0x41cf40:12,0x420f90:4}
  if address not in pops:return
  intercepts[hex(address)]=intercepts.get(hex(address),0)+1
  sp=mu.reg_read(UC_X86_REG_ESP);mu.reg_write(UC_X86_REG_EIP,get(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4+pops[address])
 def write(mu,access,address,size,value,user):
  if address<=item+0x9bc<address+size:
   writes.append({'tick':tick,'pc':hex(mu.reg_read(UC_X86_REG_EIP)),'word':value&0xffffffff})
 v.hook_add(UC_HOOK_CODE,intercept);v.hook_add(UC_HOOK_MEM_WRITE,write,begin=item+0x9bc,end=item+0x9bf)
 v.reg_write(UC_X86_REG_FPCW,0x37f);v.reg_write(UC_X86_REG_MXCSR,0x1f80)
 def call(at,args):
  put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
  v.emu_start(at,STOP,count=1000000)
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original Item call budget')
 bits=lambda f:struct.unpack('<I',struct.pack('<f',f))[0]
 def state():return {'state':get(item+0x9b0,'i')[0],'type':get(item+0x9b4,'i')[0],
  'position':get(item+0x96c,'2f'),'velocity':get(item+0x978,'2f'),'age':get(item+0x98c,'i')[0],
  'attractionSpeed':get(item+0x9bc,'f')[0],'delay':get(item+0x9c0,'i')[0]}
 call(0x4273f0,[kind,POS,bits(-1.5707963705062866),bits(.6000000238418579)])
 assert v.reg_read(UC_X86_REG_EAX)==item
 birth=state();birth_writes=writes.copy();frames=[]
 for tick in range(26 if kind==9 else 2):
  call(0x425c20,[MANAGER]);frames.append({'tick':tick,**state()})
 records.append({'kind':kind,'staleAttractionSpeed':stale,'spawn':{'position':[-180,100],
  'angle':-1.5707963705062866,'speed':.6000000238418579},'player':{'position':[150,400],
  'character':0,'state':1,'pickupHalfWidth':30,'nearHalfWidth':30,'attractionSpeed':5},
  'birth':birth,'birthWrites':birth_writes,'frames':frames,'attractionWrites':writes,'intercepts':intercepts})
out=ROOT/'tests/fixtures/small-point-reuse-native.json'
out.write_text(json.dumps({'schema':'th12-native-small-point-reuse/1','originalExeSha256':EXE_SHA256,
 'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,
 'boundary':'Actual4273f0/425c20 and movement/timer helpers. Explicit idle slot+9bc and synthetic Player/SHT/rectangle fields. ANM/sound/HUD calls intercepted; no visual/ANM RNG or fullgame claim. Memory writes identify actual attraction field stores.','records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(records),'smallPointBirth':records[0]['birth'],
 'birthAttractionWrites':records[0]['birthWrites'],'ordinaryBirth':records[1]['birth'],
 'firstAbove':next((f for f in records[0]['frames'] if f['state']==3),None)}))
