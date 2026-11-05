"""Native combat branch probes with explicit presentation fixtures.

This is a local synthetic subsystem experiment, not a replay/full-game oracle.
No original binary/resource, runtime source, or golden is written.
"""
import hashlib,json,math,pathlib,struct,sys,collections
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
exe=ROOT.parent/'th12/th12.exe';assert hashlib.sha256(exe.read_bytes()).hexdigest()==SHA
pe=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
for a,n in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
            (0x1000000,0x10000),(0x2000000,0x1000000),(0x3000000,0x1000000)]:v.mem_map(a,n)
v.mem_write(0x400000,pe.get_memory_mapped_image())
v.reg_write(UC_X86_REG_FPCW,0x37f);v.reg_write(UC_X86_REG_MXCSR,0x1f80)
PLAYER,SPELL,BOMB,HUD,ENEMIES,BULLETS,VM,NODE=0x2000000,0x2020000,0x2030000,0x2040000,0x2050000,0x2100000,0x2600000,0x2610000
ES,RUNNER,CTX,CODE,VT,LM,LASER,POS=0x2700000,0x2710000,0x2720000,0x2730000,0x2740000,0x2750000,0x2760000,0x2770000
SP,STOP,HEAP=0x100e000,0x100f000,0x3000000
def put(a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
def bits(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def ret(pop=0,value=0):
 sp=v.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0]
 v.reg_write(UC_X86_REG_ESP,sp+4+pop);v.reg_write(UC_X86_REG_EAX,value);v.reg_write(UC_X86_REG_EIP,target)
events=[];intercepts=collections.Counter();factory=[];record_factory=True;trace=[]
def hook(v,a,n,data):
 global HEAP
 sp=v.reg_read(UC_X86_REG_ESP)
 if a in [0x4154d0,0x41962b,0x468bea,0x438370,0x4381e0,0x406bf0,0x4089c0]:trace.append(hex(a))
 if a in [0x46c9ea,0x46d04a]:
  amount=get(sp+4,'I')[0];out=HEAP;HEAP+=(amount+15)&~15;intercepts[hex(a)]+=1;ret(value=out)
 elif a in [0x46ca4f,0x46c941]:intercepts[hex(a)]+=1;ret()
 elif a==0x4615a0:
  put(get(sp+8,'I')[0],'I',0);intercepts[hex(a)]+=1;ret(16)
 elif a==0x461920:intercepts[hex(a)]+=1;ret(4)
 elif a==0x4621c0:intercepts[hex(a)]+=1;ret(value=VM)
 elif a==0x461250:
  put(NODE,'II',VM,0);intercepts[hex(a)]+=1;ret(value=NODE)
 elif a in [0x461970,0x461a70]:intercepts[hex(a)]+=1;ret(4)
 elif a==0x454d10:intercepts[hex(a)]+=1;ret(8)
 elif a in [0x453d90]:intercepts[hex(a)]+=1;ret()
 elif a==0x453e20:intercepts[hex(a)]+=1;ret(4)
 elif a in [0x41ce60,0x41cf40]:intercepts[hex(a)]+=1;ret(12)
 elif a==0x452a60:intercepts[hex(a)]+=1;ret(28)
 elif a==0x428450 and record_factory:
  kind=get(sp+4,'I')[0];p=v.reg_read(UC_X86_REG_EDI)
  factory.append({'kind':kind,'profileHex':bytes(v.mem_read(p,[0x1e8,0x208,0x1e0][kind])).hex()});ret(4,0x10001)
v.hook_add(UC_HOOK_CODE,hook)
def call(a,args=(),regs=None,stop=STOP):
 put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
 for r,x in (regs or {}).items():v.reg_write(r,x)
 v.emu_start(a,stop,count=2000000)
 if v.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Native budget {a:x} @ {v.reg_read(UC_X86_REG_EIP):x}')
 return v.reg_read(UC_X86_REG_EAX)
def reset(age=60,flags=3):
 for a,n in [(PLAYER,0xc59c),(SPELL,0xc0),(BOMB,0x524),(HUD,0x7000),(ENEMIES,0x1000),(VM,0x4b4),(ES,0x1800),(RUNNER,0x100),(CTX,0x1100),(LM,0x490),(LASER,0x1000)]:v.mem_write(a,bytes(n))
 for a,p in [(0x4b4514,PLAYER),(0x4b43cc,SPELL),(0x4b43c4,BOMB),(0x4b43e4,HUD),(0x4b43dc,ENEMIES),(0x4b43c8,BULLETS),(0x4b44f4,LM)]:put(a,'I',p)
 put(0x4b2ed0,'f',1);put(0x4b0c90,'ii',2,0);put(0x4b0c48,'i',0);put(0x4b0cd0,'ii',400,100);put(0x4b0c98,'iii',3,0,2)
 put(PLAYER+0x97c,'fff',0,240,0);put(PLAYER+0xa28,'i',1);put(SPELL+0x28,'i',age);put(SPELL+0x7c,'Ii',flags,4000000)
 put(HUD+0x6d44,'I',VM);put(0x4ce8cc,'I',0x2800000);put(0x4d49d0,'I',0)
 trace.clear()
def snapshot():
 return {'playerState':get(PLAYER+0xa28,'i')[0],'stateTimer':get(PLAYER+0xa34,'i')[0],
         'invulPrevious':get(PLAYER+0xc400,'i')[0],'invulCurrent':get(PLAYER+0xc404,'i')[0],
         'spellFlags':get(SPELL+0x7c,'I')[0],'spellBonus':get(SPELL+0x80,'i')[0],
         'bombActive':get(BOMB+0x3c,'i')[0],'bombs':get(0x4b0ca0,'i')[0],'lives':get(0x4b0c98,'i')[0]}
loss=[]
for age in [0,59,60,61]:
 for survival in [False,True]:
  for action in ['wait','deathbomb','confirm']:
   reset(age,3|(8 if survival else 0));before=snapshot()
   call(0x438370,regs={UC_X86_REG_ESI:PLAYER});collision=snapshot()
   if action=='deathbomb':
    put(0x4d49d0,'I',2)
    # Execute the complete original state dispatch/deathbomb prefix and stop
    # before shared source/body/fire processing, not before Bomb startup.
    call(0x436ba0,[PLAYER],stop=0x436f90)
   elif action=='confirm':call(0x4381e0,regs={UC_X86_REG_ESI:PLAYER})
   loss.append({'spellAge':age,'survival':survival,'action':action,'before':before,
                'collision':collision,'after':snapshot(),'trace':trace.copy()})

def ins(op,args=()):return struct.pack('<iHHHBBI',0,op,16+len(args)*4,0,255,len(args),0)+struct.pack('<'+'I'*len(args),*args)
def ecl(op,args):
 put(ES,'I',VT);put(VT,'I',0x4154d0);put(ES+0x174c,'I',RUNNER);put(RUNNER+4,'I',CTX)
 v.mem_write(CODE,ins(op,args)+ins(83,[10000])+ins(10))
 put(CTX,'fI',0,CODE);put(CTX+0x1008,'I',256);put(CTX+0x1014,'I',ES);put(CTX+0x101c,'I',255)
 trace.clear();value=call(0x467170,[bits(1)],{UC_X86_REG_EAX:CTX})
 return {'opcode':op,'args':args,'return':value,'trace':trace.copy(),'pcOffset':get(CTX+4,'I')[0]-CODE}
opcode700=[]
for arg in [1,2,3,4,100]:reset();opcode700.append(ecl(700,[arg]))

setters=[]
for op,args,offsets in [(600,[0,bits(25),bits(100),bits(10),bits(6)],[0x490,0x494,0x498,0x660]),
 (601,[0,2,3,20,5,7],[0x670,0x674,0x678,0x67c,0x680]),
 (529,[6],[0x1768]),(530,[1],[0x176c]),(531,[1],[0x1770])]:
 reset();row=ecl(op,args);row['fields']={hex(o):get(ES+o,'I')[0] for o in offsets};setters.append(row)
for op,args,offsets in [(604,[123,bits(20),bits(120)],[0x50,0x54,0x58]),
 (605,[123,bits(30),bits(180)],[0x460,0x464,0x468]),
 (606,[123,bits(1)],[0x74]),(607,[123,bits(11)],[0x70]),
 (608,[123,bits(.3)],[0x68]),(609,[123,bits(3)],[0x470])]:
 reset();put(LM+0x18,'I',LASER);put(LASER+0x80,'I',123)
 row=ecl(op,args);row['fields']={hex(o):get(LASER+o,'I')[0] for o in offsets};setters.append(row)
profiles=[]
for op,args in [(602,[0]),(603,[0,123]),(611,[0])]:
 reset();ecl(500,[0]);ecl(600,[0,bits(25),bits(100),bits(10),bits(6)]);ecl(601,[0,2,3,20,5,7]);
 row=ecl(op,args);row['factory']=factory[-1];profiles.append(row)

geometry=[]
for kind,fn in [(0,0x42aa60),(1,0x42c080),(2,0x42e5b0)]:
 for angle in [0,math.pi/2]:
  for x,y in [(-1,0),(0,0),(1,0),(50,4.99),(50,5),(50,5.01),(99,0),(100,0),(101,0),(50,20)]:
   reset();put(LASER+0x50,'3f',0,0,0);put(LASER+0x68,'3f',angle,100,10);put(POS,'3f',x,y,0)
   result=call(fn,[POS,bits(0)],{UC_X86_REG_ECX:LASER})
   geometry.append({'factoryKind':kind,'function':hex(fn),'angle':angle,'point':[x,y],'radius':0,'result':result})

for r in loss:
 assert r['collision']['playerState']==4 and r['collision']['invulCurrent']==6
 if r['spellAge']>=60:assert not(r['collision']['spellFlags']&2) and r['collision']['spellBonus']==0
 if r['action']=='deathbomb':assert r['after']['playerState']==1 and r['after']['bombs']==1 and r['after']['lives']==3
 if r['action']=='confirm':assert r['after']['playerState']==2 and r['after']['lives']==2 and r['after']['invulCurrent']==180
for r in opcode700:assert r['trace']==['0x4154d0','0x41962b','0x468bea','0x468bea'] and r['pcOffset']==40
assert [r['factory']['kind'] for r in profiles]==[0,1,2]
out=ROOT/'artifacts/reverse/combat/native-probe.json'
out.write_text(json.dumps({'schema':'th12-native-combat-probe/1','originalExeSha256':SHA,
 'wholeGameOracle':False,'partial':True,'scope':'Original ECL VM/high dispatcher, collision function, confirmed Miss, original SanaeA deathbomb prefix, laser query geometry',
 'fixtures':['ANM 4615a0/461920/461970/461a70/454d10 use null handle or no-op; no ANM RNG claimed',
             '4621c0/461250 return synthetic VM/node for SanaeA presentation only',
             '452a60 camera effect registration and 41ce60/41cf40 HUD counters are no-op',
             '453d90/453e20 sound output no-op; host heap malloc/free fixture',
             'Laser factory records original ECL-built parameter buffer, does not allocate/tick a laser',
             'Deathbomb executes original Player state prefix, Bomb dispatch/start, bonus loss and bomb stock subtraction; stop at436f90 before shared player tail'],
 'intercepts':dict(intercepts),'loss':loss,'opcode700':opcode700,'eclSetters':setters,
 'laserProfiles':profiles,'laserGeometry':geometry},indent=2)+'\n')
print(json.dumps({'output':str(out),'lossCases':len(loss),'opcode700':opcode700,
 'deathbombAge60':[r for r in loss if r['spellAge']==60 and not r['survival'] and r['action']=='deathbomb'],
 'laserCases':len(geometry)},indent=2))
