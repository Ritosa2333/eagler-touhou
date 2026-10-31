"""Original Bullet21 movement/query ordering. No replay/golden writes.

The original collision and state branches execute. Only the downstream real miss
callback is intercepted; this is not a full-world replay oracle.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
sys.path.insert(0,str(ROOT/'scripts/reverse'))
from native_anm_resource import check_original,EXE_SHA256
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
from unicorn.x86_const import *
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
mu=Uc(UC_ARCH_X86,UC_MODE_32)
for address,size in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x30000)]:mu.mem_map(address,size)
mu.mem_write(0x400000,pe.get_memory_mapped_image())
mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
PLAYER,STAGE,SIZE,POINT,SP,STOP=0x2000000,0x2020000,0x2010000,0x2010010,0x100e000,0x100f000
def put(at,fmt,*values):mu.mem_write(at,struct.pack('<'+fmt,*values))
def get(at,fmt):return list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
miss_calls=[]
def hook(uc,address,size,user):
 if address==0x438370:
  miss_calls.append(address);sp=mu.reg_read(UC_X86_REG_ESP);ret=get(sp,'I')[0]
  mu.reg_write(UC_X86_REG_ESP,sp+4);mu.reg_write(UC_X86_REG_EIP,ret)
mu.hook_add(UC_HOOK_CODE,hook,begin=0x438370,end=0x438370)
put(0x4b4514,'I',PLAYER);put(0x4b43e4,'I',STAGE)
# Separate synthetic 4099e0 branch audit. It does not claim these are the late
# replay's actual bullet states. No motion or collision instructions are hooked.
BULLET=0x2011000
tail_calls=[];graze_calls=[]
query_positions=[];audit_events=[]
def tail_hook(uc,address,size,user):
 sp=mu.reg_read(UC_X86_REG_ESP);ret=get(sp,'I')[0]
 if address==0x455630:
  tail_calls.append(get(sp+4,'I')[0]);mu.reg_write(UC_X86_REG_EAX,0)
 else:graze_calls.append(get(sp+4,'I')[0])
 audit_events.append('anm-tail' if address==0x455630 else 'graze')
 mu.reg_write(UC_X86_REG_ESP,sp+8);mu.reg_write(UC_X86_REG_EIP,ret)
for address in [0x455630,0x4391c0]:mu.hook_add(UC_HOOK_CODE,tail_hook,begin=address,end=address)
def observe(uc,address,size,user):
 if address==0x437810:
  query_positions.append(get(mu.reg_read(UC_X86_REG_ECX),'ff'));audit_events.append('query')
 elif address==0x438370:audit_events.append('miss')
for address in [0x437810,0x438370]:mu.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
def observe_interrupt(uc,access,address,size,value,user):
 audit_events.append('interrupt:'+str(value&65535))
# State1 writes the pending IRQ directly; state2 writes through40d6e0.
mu.hook_add(UC_HOOK_MEM_WRITE,observe_interrupt,begin=BULLET+8+0x3c4,end=BULLET+8+0x3c5)
put(0x4b2ed0,'f',1)
audit=[]
for label,phase,x,vx,age,int0,delay,install_record in [
 ('spawning-first-query-hit',2,1,4,8,1,0,False),
 ('spawning-first-query-ineligible',2,1,4,7,1,0,False),
 ('spawning-two-query-graze',2,-12,4,8,1,0,False),
 ('spawning-second-query-hit',2,-7,4,8,1,0,False),
 ('alive-positive-delay-word',1,0,0,100,0,5,False),
 ('alive-extension200-positive-word',1,0,0,100,0,0,True),
 ('alive-negative-delay-word',1,0,0,100,0,-5,False),
 ('alive-zero-delay-control',1,0,0,100,0,0,False),
]:
 mu.mem_write(PLAYER,bytes(0xc59c));mu.mem_write(STAGE,bytes(0x7000));mu.mem_write(BULLET,bytes(0x9f8))
 put(PLAYER+0x9cc,'ff',-1,119);put(PLAYER+0x9d8,'ff',1,121);put(PLAYER+0xa28,'i',1)
 put(BULLET,'Ii',2,delay);put(BULLET+0x532,'H',phase);put(BULLET+0x4bc,'fff',x,120,0)
 put(BULLET+0x4c8,'fff',vx,0,0);put(BULLET+0x4dc,'ff',4,4);put(BULLET+0x4e8,'i',age)
 put(BULLET+0x404,'i',int0);put(BULLET+0x524,'i',-1);put(BULLET+0x520,'i',10)
 if install_record:put(BULLET+0x550,'ffiiII',-999999,-999999,5,-999999,0x200,0)
 put(SP,'II',STOP,BULLET);mu.reg_write(UC_X86_REG_ESP,SP)
 before=len(miss_calls);before_graze=len(graze_calls);before_tail=len(tail_calls)
 before_queries=len(query_positions);before_events=len(audit_events)
 mu.emu_start(0x4099e0,STOP,count=100000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native bullet update budget exceeded')
 audit.append({'case':label,'initial':{'phase':phase,'x':x,'y':120,'vx':vx,'age':age,'anmInt0':int0,'word4':delay,'extension200':install_record},
  'after':{'phase':get(BULLET+0x532,'H')[0],'position':get(BULLET+0x4bc,'ff'),'word4':get(BULLET+4,'i')[0],'interrupt':get(BULLET+8+0x3c4,'h')[0]},
  'missCallbacks':len(miss_calls)-before,'grazeCallbacks':len(graze_calls)-before_graze,'anmTailCalls':len(tail_calls)-before_tail,
  'queryPositions':query_positions[before_queries:],'events':audit_events[before_events:]})
audit_out={'schema':'th12-original-bullet-collision-branch-audit/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,
 'source':'Original4099e0 including40aa60 and437810; downstream438370 miss/4391c0 graze callbacks captured;455630 ANM tail no-op. Synthetic fields are explicit, no candidate-derived claim of full state.',
 'rows':audit}
(ROOT/'artifacts/reverse/bullet-collision-branch-original.json').write_text(json.dumps(audit_out,indent=2)+'\n')
# This new fixture contains only pinned EXE samples and explicit host boundaries.
(ROOT/'tests/fixtures/bullet-collision-order-original.json').write_text(json.dumps(audit_out,indent=2)+'\n')
print(json.dumps({'syntheticBranchAudit':audit}))
