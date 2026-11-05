"""Explicit native TH12 Sanae B and player damage samples; not a game golden."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
exe=WORKSPACE/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;PLAYER=0x2000000;BULLET=PLAYER+0x20000;SPEC=PLAYER+0x30000;POS=PLAYER+0x40000;BOX=POS+16;ANM=PLAYER+0x50000;BOMB=PLAYER+0x60000;BOSS=PLAYER+0x70000;STOP=0x3000000
mu.mem_map(STACK,0x10000);mu.mem_map(PLAYER,0x80000);mu.mem_map(STOP,4096);mu.reg_write(UC_X86_REG_FPCW,0x37f)
def put(a,fmt,*v):mu.mem_write(a,struct.pack('<'+fmt,*v))
def get(a,fmt):return struct.unpack('<'+fmt,mu.mem_read(a,struct.calcsize('<'+fmt)))
def bits(f):return struct.unpack('<I',struct.pack('<f',f))[0]
def ret(n=0,value=None):
 sp=mu.reg_read(UC_X86_REG_ESP);address=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+4+n);mu.reg_write(UC_X86_REG_EIP,address)
 if value is not None:mu.reg_write(UC_X86_REG_EAX,value)
emissions=[];interrupts=[];clears=[]
def hook(uc,a,size,data):
 if a==0x439630:
  sp=mu.reg_read(UC_X86_REG_ESP);p=get(sp+4,'I')[0];q=mu.reg_read(UC_X86_REG_ECX)
  emissions.append({'position':get(q,'3f'),'profileHex':bytes(mu.mem_read(p,52)).hex(),'angle':get(p+20,'f')[0]});ret(8,0)
 elif a==0x461970:interrupts.append(mu.reg_read(UC_X86_REG_EBX));ret(4,0)
 elif a==0x406de0:ret(4,0) # Other bomb subsystem contributes no damage in this isolated fixture.
 elif a in (0x461a70,):ret(4,0)
 elif a==0x461c50:ret(0,ANM)
 elif a==0x4615a0:
  sp=mu.reg_read(UC_X86_REG_ESP);dest=get(sp+8,'I')[0];put(dest,'I',7);ret(16,7)
 elif a==0x40cd00:
  sp=mu.reg_read(UC_X86_REG_ESP);p=mu.reg_read(UC_X86_REG_EAX);q=get(sp+4,'I')[0];clears.append({'position':get(p,'3f'),'size':get(q,'2f'),'convert':get(sp+8,'i')[0]});ret(8,0)
 elif a==0x4285f0:ret(4,0)
mu.hook_add(UC_HOOK_CODE,hook)
def call(a,eax=PLAYER,ecx=PLAYER,edx=BULLET,esi=BULLET,edi=PLAYER,args=()):
 sp=STACK+0xf000;put(sp,'I'*(len(args)+1),STOP,*args)
 for r,v in [(UC_X86_REG_ESP,sp),(UC_X86_REG_EAX,eax),(UC_X86_REG_ECX,ecx),(UC_X86_REG_EDX,edx),(UC_X86_REG_ESI,esi),(UC_X86_REG_EDI,edi)]:mu.reg_write(r,v)
 mu.emu_start(a,STOP,count=1000000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native budget reached at '+hex(a))
 return mu.reg_read(UC_X86_REG_EAX)
def reset():
 mu.mem_write(PLAYER,bytes(0xc59c));mu.mem_write(BULLET,bytes(120));put(0x4b4514,'I',PLAYER);put(0x4b2ed0,'f',1);put(PLAYER+0xa30,'2i',0,1);mu.mem_write(SPEC,bytes(52));put(SPEC+29,'b',4)
def query(x=0,y=120,w=24,h=24):put(POS,'3f',x,y,0);put(BOX,'2f',w,h);return call(0x439ed0,args=(POS,BOX))
extra=[]
for seed in [0,1,0xbeef,0xffff]:
 reset();put(0x4ce568,'H',seed);put(BULLET+20,'3f',32,120,0);emissions.clear();call(0x43a950,args=(POS,));extra.append({'seed':seed,'finalSeed':get(0x4ce568,'H')[0],'emissions':emissions.copy()})
particle=[];reset();put(BULLET+20,'3f',0,120,0);put(BULLET+44,'f',3);put(BULLET+72,'i',1)
for frame in range(34):
 call(0x43aa50);source=PLAYER+0x8988;particle.append({'tick':frame,'speed':get(BULLET+44,'f')[0],'phase':get(BULLET+72,'i')[0],'sourceRadius':get(source,'f')[0],'sourceGrowth':get(source+4,'f')[0],'sourceTicks':get(source+80,'i')[0],'sourceDamage':get(source+96,'i')[0],'sourceFlags':get(source+112,'I')[0]})
source=[];reset();call(0x4390f0,args=(POS,bits(12),bits(1.4),15,4));put(PLAYER+0x8988+24,'3f',0,120,0)
for tick in range(16):
 p=PLAYER+0x8988;source.append({'tick':tick,'radius':get(p,'f')[0],'previous':get(p+76,'i')[0],'ticks':get(p+80,'i')[0],'flags':get(p+112,'I')[0],'damage':query(),'outsideWithHugeEnemy':query(45,120,400,400),'boundary':query(get(p,'f')[0],120,1,1)})
 sp=STACK+0xe000;mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EDI,PLAYER);mu.emu_start(0x436f90,0x43709e,count=100000)
rect=[];cadence=[]
for kind in [0,2,4]:
 for age in range(9):
  reset();b=PLAYER+0xa58;put(SPEC+29,'b',kind);put(b,'2i',age-1,age);put(b+20,'3f',0,120,0);put(b+72,'i',1);put(b+96,'i',15);put(b+104,'2f',24,24);put(b+116,'I',SPEC);cadence.append({'type':kind,'age':age,'damage':query(),'phase':get(b+72,'i')[0]})
 for x,y,w,h in [(24,120,24,24),(24.01,120,24,24),(0,96,24,24),(0,95.99,24,24),(100,120,200,4),(0,143,4,4),(0,145,4,4)]:
  reset();b=PLAYER+0xa58;put(SPEC+29,'b',kind);put(b,'2i',3,4);put(b+20,'3f',0,120,0);put(b+72,'i',1);put(b+96,'i',15);put(b+104,'2f',24,24);put(b+116,'I',SPEC);rect.append({'type':kind,'x':x,'y':y,'width':w,'height':h,'damage':query(x,y,w,h)})
marisa_b=[];put(0x4b43cc,'I',BOSS)
for age in [0,29,30,31,60,89,90,159,160,161]:
 put(BOMB+24,'i',age);put(BOMB+28,'f',age);clears.clear();call(0x407a30,ecx=BOMB)
 for y in [-1,0,0.01,120,224,400,448,449]:
  put(POS,'3f',1000,y,0);damage=call(0x4079d0,edx=BOMB,args=(POS,));marisa_b.append({'age':age,'x':1000,'y':y,'damage':damage,'clearRectangles':clears.copy()})
out=TITLE/'local/reverse/player-weapons-probe.json';out.write_text(json.dumps({'schema':'th12-research-player-weapons/1','exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'provider':'Original x86 under Unicorn','wholeGameOracle':False,'fixtures':['439630 records six particles without ANM allocation','461970/461a70 ANM interrupt/delete no-op','461c50/4615a0 ANM dummy handle','406de0 other bomb damage returns zero','40cd00 records clear rectangle and 4285f0 laser clear no-op'],'extraCallback1':extra,'particleUpdate':particle,'sources':source,'rectangles':rect,'projectileCadence':cadence,'marisaBBomb':marisa_b},indent=2)+'\n');print(out)
