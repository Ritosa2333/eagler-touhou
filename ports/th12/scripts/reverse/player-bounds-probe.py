"""Original ANM quad geometry and Player439b10 offscreen decisions.

The bounds-only fixture supplies four explicit geometry vertices. The ANM
fixture executes native geometry helpers on synthetic VM state, not a candidate.
"""
from pathlib import Path
import json,struct,sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
mu=Uc(UC_ARCH_X86,UC_MODE_32)
for at,size in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x1000000)]:mu.mem_map(at,size)
mu.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,STOP,PLAYER,MANAGER,VM,LIST,SPEC,OUT=0x100e000,0x100f000,0x2000000,0x2100000,0x2a00000,0x2a10000,0x2a20000,0x2a30000
def put(at,f,*v):mu.mem_write(at,struct.pack('<'+f,*v))
def get(at,f):return list(struct.unpack('<'+f,mu.mem_read(at,struct.calcsize('<'+f))))
def call(at,args=(),registers=None):
 put(STACK,'I'*(len(args)+1),STOP,*args);mu.reg_write(UC_X86_REG_ESP,STACK)
 for reg,value in (registers or {}).items():mu.reg_write(reg,value)
 mu.emu_start(at,STOP,count=400000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError(f'Budget at {mu.reg_read(UC_X86_REG_EIP):x}')
def ret(pop=0,value=0):
 sp=mu.reg_read(UC_X86_REG_ESP);at=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+4+pop);mu.reg_write(UC_X86_REG_EAX,value);mu.reg_write(UC_X86_REG_EIP,at)
mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
put(0x4ce8cc,'I',MANAGER);put(0x4b2ed0,'f',1)
geometry=[]
for mode in [0,1]:
 for anchorX in [0,1,2]:
  for anchorY in [0,1,2]:
   for angle in ([0] if mode==0 else [0,.25,-1.5707963705062866]):
    mu.mem_write(VM,bytes(0x600));flags=3|(mode<<23)|(anchorX<<19)|(anchorY<<21)
    put(VM+0x47c,'I',flags);put(VM+0x58,'ff',16,48);put(VM+0x40,'ff',1.25,.75)
    put(VM+0x424,'fff',2.25,-3.75,0);put(VM+0x430,'fff',224,16,0);put(VM+0x43c,'fff',1.5,2.5,0);put(VM+0x2c,'f',angle)
    call(0x45cdf0,registers={UC_X86_REG_EAX:VM,UC_X86_REG_ECX:OUT})
    geometry.append({'mode':mode,'anchorX':anchorX,'anchorY':anchorY,'angle':angle,'position':[227.75,14.75],'dimensions':[20,36],'vertices':[get(OUT+i*12,'fff') for i in range(4)]})

vertices=[];deletes=[];debug=[]
def hook(uc,at,size,data):
 if at==0x45cdf0:
  dest=mu.reg_read(UC_X86_REG_ECX)
  for i,(x,y) in enumerate(vertices):put(dest+i*12,'fff',x,y,0)
  ret()
 elif at==0x461a70:deletes.append(get(mu.reg_read(UC_X86_REG_ESP)+4,'I')[0]);ret(4)
 elif at==0x406de0:ret(4)
 elif at==0x461970:ret(4)
 elif at==0x461c50:ret(value=VM)
 elif at==0x4615a0:
  put(get(mu.reg_read(UC_X86_REG_ESP)+8,'I')[0],'I',7);ret(16,7)
 elif at in [0x439c43,0x439cf7,0x439e21,0x439e5c]:debug.append({'at':hex(at),'vertices':get(mu.reg_read(UC_X86_REG_ESP)+0x20,'12f')})
mu.hook_add(UC_HOOK_CODE,hook)
mu.ctl_flush_tb() # Geometry was already translated before this bounds-only hook.
cases=[]
for kind in [0,2,3,4]:
 for age in [9,10,11]:
  for x,y in [(31,100),(32,100),(416,100),(415.9,100),(224,15),(224,16),(224,464),(224,463.9),(-1000,-1000)]:
   mu.mem_write(PLAYER,bytes(0xd000));mu.mem_write(VM,bytes(0x600));mu.mem_write(SPEC,bytes(64));mu.mem_write(MANAGER+0x8856b8,bytes(16))
   put(MANAGER+0x8856b8,'I',LIST);put(LIST,'II',VM,0);put(VM,'I',77)
   shot=PLAYER+0xa58;put(shot,'ii',age-1,age);put(shot+0x48,'i',1);put(shot+0x4c,'I',77);put(shot+0x74,'I',SPEC)
   put(SPEC+0x1d,'B',kind);put(shot+0x14,'fff',0,-64,0)
   vertices=[(x,y)]*4;deletes.clear();debug.clear();call(0x439b10,args=(PLAYER,))
   cases.append({'type':kind,'age':age,'vertices':vertices.copy(),'shotPosition':[0,-64],'active':get(shot+0x48,'i')[0]!=0,'deleted':bool(deletes),'debug':debug.copy()})
damage=[]
put(0x4b4514,'I',PLAYER)
for kind in [0,2]:
 for enemyY,shotY,shotHeight in [(ey,176 if kind==2 else ey,448 if kind==2 else 24) for ey in [-64,-32,-12,-11.99,0,12,24,48,448,460,464,500]]+[(ey,sy,448 if kind==2 else 48) for ey in [-12,-11.99,-9.3,0,12] for sy in [-56,-36,-32.92,-24,-12,0,12,36]]:
  mu.mem_write(PLAYER,bytes(0xd000));mu.mem_write(SPEC,bytes(64));mu.mem_write(VM,bytes(0x600))
  shot=PLAYER+0xa58;put(PLAYER+0xa30,'ii',3,4);put(shot,'ii',3,4);put(shot+0x14,'fff',0,shotY,0)
  put(shot+0x48,'i',1);put(shot+0x60,'i',15);put(shot+0x68,'ff',24,shotHeight);put(shot+0x74,'I',SPEC);put(SPEC+0x1d,'B',kind)
  put(OUT,'fff',0,enemyY,0);put(OUT+16,'ff',24,24)
  call(0x439ed0,args=(OUT,OUT+16))
  damage.append({'type':kind,'enemyY':enemyY,'damage':mu.reg_read(UC_X86_REG_EAX),'shotY':shotY,'shotHeight':shotHeight,'enemyHeight':24})
for row in cases:row.pop('debug',None)
out=ROOT/'tests/fixtures/player-bounds-original.json'
out.write_text(json.dumps({'schema':'th12-native-player-bounds/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'geometry':geometry,'bounds':cases,'damage':damage,'fixtures':['439b10 geometry boundary fixture intercepts45cdf0 only','bounds fixture records461a70 removal without another manager','geometry fixture executes45cdf0 and its native mode0/1 helpers','439ed0 geometry fixture intercepts406de0 Bomb damage and ANM bind/remove only']},indent=2)+'\n',encoding='utf-8')
print(f'{len(geometry)} geometry, {len(cases)} original bounds cases: {out.name}')
