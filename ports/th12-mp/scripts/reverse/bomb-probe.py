"""Explicit TH12 Bomb/source/effect probes; external visual allocation is a fixture."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
exe=TITLE.parent/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;PLAYER=0x2000000;BOMB=PLAYER+0x10000;ANM=PLAYER+0x20000;BOSS=PLAYER+0x30000;POS=PLAYER+0x40000;BOX=POS+64;HEAP=PLAYER+0x50000;STOP=0x3000000
mu.mem_map(STACK,0x10000);mu.mem_map(PLAYER,0x100000);mu.mem_map(STOP,4096);mu.reg_write(UC_X86_REG_FPCW,0x37f)
def put(a,fmt,*v):mu.mem_write(a,struct.pack('<'+fmt,*v))
def get(a,fmt):return struct.unpack('<'+fmt,mu.mem_read(a,struct.calcsize('<'+fmt)))
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def ret(n=0,v=None):
 sp=mu.reg_read(UC_X86_REG_ESP);a=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+4+n);mu.reg_write(UC_X86_REG_EIP,a)
 if v is not None:mu.reg_write(UC_X86_REG_EAX,v)
circles=[];effects=[];rectangles=[];rng_calls=0;orb_target=0
def hook(uc,a,size,data):
 global rng_calls
 sp=mu.reg_read(UC_X86_REG_ESP)
 if a==0x461920:ret(4,ANM if get(sp+4,'I')[0] else 0)
 elif a in [0x453d90,0x461970,0x453e20]:ret(4 if a!=0x453d90 else 0,0)
 elif a==0x46c9ea:ret(0,0) # Screen shake allocation omitted.
 elif a==0x46d04a:ret(0,HEAP)
 elif a==0x40fbe0:
  out,kind,p=get(sp+4,'3I');effects.append({'kind':kind,'position':get(p,'6f')});put(out,'I',1);ret(12,out)
 elif a==0x40caa0:
  p=mu.reg_read(UC_X86_REG_EBX);circles.append({'position':get(p,'3f'),'radius':get(sp+4,'f')[0],'convert':get(sp+8,'i')[0],'skipProtected':get(sp+12,'i')[0]});ret(12,0)
 elif a==0x4286f0:ret(12,0)
 elif a==0x40cd00:
  p=mu.reg_read(UC_X86_REG_EAX);q=get(sp+4,'I')[0];rectangles.append({'position':get(p,'3f'),'size':get(q,'2f'),'convert':get(sp+8,'i')[0]});ret(8,0)
 elif a==0x4285f0:ret(4,0)
 elif a==0x406de0:ret(4,0)
 elif a==0x410020:ret(16,0) # Mesh coordinate builder is not needed for the RNG/query probe.
 elif a==0x41a9f0:ret(4,orb_target)
 elif a==0x464440:rng_calls+=1
mu.hook_add(UC_HOOK_CODE,hook)
def call(a,eax=BOMB,ecx=ANM,edx=POS,esi=BOMB,edi=BOMB,ebx=BOMB,args=()):
 sp=STACK+0xf000;put(sp,'I'*(len(args)+1),STOP,*args)
 for r,v in [(UC_X86_REG_ESP,sp),(UC_X86_REG_EAX,eax),(UC_X86_REG_ECX,ecx),(UC_X86_REG_EDX,edx),(UC_X86_REG_ESI,esi),(UC_X86_REG_EDI,edi),(UC_X86_REG_EBX,ebx)]:mu.reg_write(r,v)
 mu.emu_start(a,STOP,count=1000000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError(hex(a)+' instruction budget reached')
 return mu.reg_read(UC_X86_REG_EAX)
def reset(age=0):
 global rng_calls
 mu.mem_write(PLAYER,bytes(0xc59c));mu.mem_write(BOMB,bytes(0x524));mu.mem_write(ANM,bytes(0x500));mu.mem_write(HEAP,bytes(0x800));put(0x4b4514,'I',PLAYER);put(0x4b43cc,'I',BOSS);put(0x4b2ed0,'f',1);put(0x4ce568,'H',0);put(BOMB+0x508,'3f',20,300,0);put(BOMB+24,'i',age);put(BOMB+28,'f',age);put(BOMB+64,'I',7);put(ANM+64,'2f',100,32);put(POS,'6f',20,300,0,0,0,0);circles.clear();effects.clear();rectangles.clear();rng_calls=0
sb=[]
for age in [0,1,29,30,179,180,199,200,249,250,279,280,399,400]:
 reset(age);call(0x408f00);sources=[]
 for i in range(128):
  p=PLAYER+0x8988+i*0x74
  if get(p+112,'I')[0]&1:sources.append({'position':get(p+24,'3f'),'radius':get(p,'f')[0],'growth':get(p+4,'f')[0],'ticks':get(p+80,'i')[0],'damage':get(p+96,'i')[0],'flags':get(p+112,'I')[0]})
 sb.append({'age':age,'circles':circles.copy(),'sources':sources,'effects':effects.copy(),'rng32CallsWithoutLeafConstructor':rng_calls})
reset();put(ANM+0x478,'I',HEAP);call(0x4105e0);leaf=[{'age':get(HEAP+200,'i')[0],'seed':get(0x4ce568,'H')[0],'rng32Calls':rng_calls,'angle':get(HEAP+192,'f')[0]}]
for frame in range(17):
 result=call(0x4103f0);leaf.append({'tick':frame,'result':result,'age':get(HEAP+200,'i')[0],'seed':get(0x4ce568,'H')[0],'rng32Calls':rng_calls,'angle':get(HEAP+192,'f')[0]})
ma=[]
for age in [0,1,4,300,301]:
 for x,y,w,h in [(20,300,24,24),(20,120,24,24),(36,332,0,0),(36.01,332.01,0,0),(84,300,0,0),(84.01,300.01,0,0),(148,268,0,0),(148.01,268.01,0,0),(20,-180,0,0),(20,-180.01,0,0),(220,120,400,10)]:
  reset(age);put(BOMB+20,'i',age-1);put(POS,'3f',x,y,0);put(BOX,'2f',w,h);damage=call(0x4073b0,eax=POS,ecx=BOMB,esi=BOX);ma.append({'age':age,'x':x,'y':y,'width':w,'height':h,'damage':damage})
reset();call(0x407580);ma_clear=rectangles.copy()
reset();call(0x46a250);ra_emitter=[]
for tick in range(302):
 effects.clear();result=call(0x46a300);ra_emitter.append({'tick':tick,'age':get(HEAP+16,'i')[0],'offsets':get(HEAP+32,'4f'),'effects':effects.copy(),'result':result})
 if result:break
ra_beams=[]
for offset in [0,18,36,-36]:
 reset();put(POS,'6f',20+offset,300,-1.5707963705062866,20,300,0);call(0x469750);p=PLAYER+0x8988
 samples=[]
 for tick in range(35):
  samples.append({'tick':tick,'head':get(HEAP+0x330,'2f'),'angle':get(HEAP+0x34c,'f')[0],'age':get(HEAP+0x3f8,'i')[0],'sourcePosition':get(p+24,'2f'),'sourceAngle':get(p+8,'f')[0],'sourceSize':get(p+16,'2f'),'sourceFlags':get(p+112,'I')[0]})
  result=call(0x469d40)
  if result:break
 ra_beams.append({'offset':offset,'samples':samples})
ra_manager=[]
for columns in [0,17]:
 reset();mesh=HEAP+0x1000;put(BOMB+0x504,'I',mesh);put(mesh+4,'i',columns);put(mesh+16,'2I',HEAP+0x2000,HEAP+0x9000);put(PLAYER+0x97c,'3f',20,300,0);call(0x46a970)
 ra_manager.append({'meshColumns':columns,'rng32Calls':rng_calls,'seed':get(0x4ce568,'H')[0],'clearRectangles':rectangles.copy(),'speedMultiplier':get(PLAYER+0x825c,'f')[0]})
rb=[]
for index,target in [(0,None),(7,None),(0,[40,120])]:
 reset();orb=HEAP;orb_target=HEAP+0x2000 if target else 0;put(0x4b43dc,'I',orb_target);put(PLAYER+0x97c,'3f',20,300,0)
 if target:put(orb_target+0x1074,'3f',*target,0)
 put(orb,'I',7);put(orb+16,'3f',20,300,0);put(orb+32,'3f',index*.7853981852531433,0,.04908738657832146);put(orb+52,'I',1);put(orb+132,'i',1);put(orb+136,'2i',-1,0);put(orb+148,'I',0x4b2ed0);put(orb+152,'I',1);put(orb+172,'i',index)
 samples=[]
 for tick in range(200):
  call(0x407b80,args=(orb,));samples.append({'tick':tick,'position':get(orb+4,'2f'),'center':get(orb+16,'2f'),'heading':get(orb+32,'f')[0],'radius':get(orb+36,'f')[0],'speed':get(orb+28,'f')[0],'delta':get(orb+156,'2f'),'flags':get(orb+52,'I')[0],'age':get(orb+140,'i')[0]})
 rb.append({'index':index,'target':target,'samples':samples})
reset();orb=HEAP;put(orb,'I',7);put(orb+4,'3f',20,300,0);p=PLAYER+0x8988;put(p+112,'I',3);put(orb+176,'I',p);call(0x408030,edi=orb);q=p+0x74
rb_explosion={'circles':circles.copy(),'newSource':{'radius':get(q,'f')[0],'growth':get(q+4,'f')[0],'ticks':get(q+80,'i')[0],'damage':get(q+96,'i')[0],'flags':get(q+112,'I')[0]},'oldSourceFlags':get(p+112,'I')[0],'orbActive':get(orb+132,'i')[0]}
sa=[]
for x,y in [(-1000,-1),(0,-.01),(1000,0),(1000,.01),(0,224),(0,449)]:
 reset();put(POS,'3f',x,y,0);sa.append({'x':x,'y':y,'damage':call(0x408c10,eax=POS)})
reset();call(0x408c30);sa_clear=rectangles.copy();sa_timing=[];reset()
for age in [0,239,240,241]:
 put(BOMB+24,'i',age);put(BOMB+28,'f',age);rectangles.clear();result=call(0x408b90);sa_timing.append({'age':age,'result':result,'clearRectangles':rectangles.copy(),'mainHandle':get(BOMB+64,'I')[0],'iframes':get(PLAYER+0xc404,'i')[0]})
out=TITLE/'local/reverse/bomb-probe.json';out.write_text(json.dumps({'schema':'th12-research-bomb-probe/1','exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'wholeGameOracle':False,'fixtures':['461920 dummy VM with sx100/sy32; no real ANM advancement','screen shake allocation46c9ea returns null','40fbe0 records leaves/beams without constructor','40caa0/4286f0 records circle/laser clear','malloc46d04a supplies fixture storage','406de0 unrelated Bomb direct damage suppressed','410020 mesh coordinates no-op; supplied column count tests game RNG loop','Reimu B orbit fields are explicit native4083c0/407ea0 initialization; target helper supplies optional eligible fixed enemy'],'sanaeB':sb,'leaf':leaf,'marisaADamage':ma,'marisaAClear':ma_clear,'reimuAEmitter':ra_emitter,'reimuABeams':ra_beams,'reimuAManager':ra_manager,'reimuBOrbs':rb,'reimuBExplosion':rb_explosion,'sanaeADamage':sa,'sanaeAClear':sa_clear,'sanaeATiming':sa_timing},indent=2)+'\n');print(out)
