"""Explicit native437a80 line hit/graze/state gate samples; no full-game golden."""
import hashlib,json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
exe=ROOT.parent/'th12/th12.exe';assert hashlib.sha256(exe.read_bytes()).hexdigest()==SHA
pe=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
for a,n in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x30000)]:v.mem_map(a,n)
v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f)
PLAYER,HUD,POS,SP,STOP=0x2000000,0x2010000,0x2020000,0x100e000,0x100f000
def put(a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
def f32(x):return struct.unpack('<f',struct.pack('<f',x))[0]
def bits(x):return struct.unpack('<I',struct.pack('<f',x))[0]
misses=0
def hook(v,a,n,data):
 global misses
 if a==0x438370:
  misses+=1;sp=v.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4);v.reg_write(UC_X86_REG_EIP,target)
v.hook_add(UC_HOOK_CODE,hook)
rows=[]
def sample(angle,localX,localY,hit=2,width=16,length=100,state=1,invul=0,flags=0,dialogue=False):
 global misses
 angle=f32(angle);x=f32(math.cos(angle)*localX-math.sin(angle)*localY);y=f32(math.sin(angle)*localX+math.cos(angle)*localY)
 put(0x4b4514,'I',PLAYER);put(0x4b43e4,'I',HUD);put(HUD+0x6d30,'I',1 if dialogue else 0)
 put(PLAYER+0x97c,'3f',x,y,0);put(PLAYER+0x9e4,'2f',hit,hit);put(PLAYER+0xa28,'i',state);put(PLAYER+0xc404,'i',invul);put(PLAYER+0xc414,'I',flags);put(POS,'3f',0,0,0)
 put(SP,'4I',STOP,bits(angle),bits(width),bits(length));v.reg_write(UC_X86_REG_ESP,SP);v.reg_write(UC_X86_REG_EAX,POS);misses=0
 v.emu_start(0x437a80,STOP,count=100000)
 assert v.reg_read(UC_X86_REG_EIP)==STOP
 code=v.reg_read(UC_X86_REG_EAX);assert code in(0,1,2);assert (misses==1)==(code==1)
 rows.append({'angle':angle,'origin':[0,0,0],'transverseSize':width,'longitudinalSize':length,
  'player':{'position':[x,y,0],'hitWidth':hit,'hitHeight':hit,'state':state,'invulnerability':invul,'flags':flags,'dialogue':dialogue},
  'localInput':[localX,localY],'nativeReturn':code,'calls438370':misses})
for angle in [0,math.pi/4,math.pi/2,-math.pi/2,math.pi]:
 for hit in [2,3.5,3]:
  for x,y in [(-hit-.01,0),(-hit,0),(-hit+.01,0),(0,0),(50,0),(100,0),(100+hit,0),(100+hit+.01,0),
   (50,8+hit-.01),(50,8+hit),(50,8+hit+.01),(50,-8-hit),(50,-8-hit-.01),(50,8+hit*16),(50,8+hit*16+.01),
   (-hit*16,0),(-hit*16-.01,0),(100+hit*16,0),(100+hit*16+.01,0),(50,64)]:sample(angle,x,y,hit)
for state in range(5):
 for invul in [0,1,180]:
  for flags in [0,2,4]:
   for dialogue in [False,True]:
    for y in [0,20]:sample(0,50,y,state=state,invul=invul,flags=flags,dialogue=dialogue)
for width in [-5,0,4,6,16,32]:
 for y in [-40,-5,-2,0,2,5,40]:sample(0,50,y,width=width)
out=ROOT/'tests/native/laser-line-original.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-native-laser-line-collision/1','originalExeSha256':SHA,
 'entry':'0x437a80','wholeGameOracle':False,'fixtures':['438370 records call and returns without executing collision visuals/window; state gate/geometry native','Player/optional HUD storage synthetic; native sin/cos and all line query instructions execute'],
 'cases':rows},indent=2)+'\n')
print(json.dumps({'output':str(out),'cases':len(rows),'returnCounts':{c:sum(r['nativeReturn']==c for r in rows) for c in [0,1,2]}}))
