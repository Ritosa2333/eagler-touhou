"""Pinned, explicit TH12 spell subsystem sampling; no original files are modified."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_INVALID
from unicorn.x86_const import *
exe=TITLE.parent/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;SPELL=0x2000000;REPLAY=SPELL+0x10000;STATS=SPELL+0x20000;BOMB=SPELL+0x50000;FONT=SPELL+0x60000;STD=SPELL+0x80000;BULLETS=SPELL+0x100000;ENEMIES=SPELL+0x700000;ENEMY=SPELL+0x710000;PLAYER=SPELL+0x720000;HUD=SPELL+0x730000;TEXT=SPELL+0x740000;VM=SPELL+0x750000;NODE1=SPELL+0x751000;CHILD1=SPELL+0x752000;NODE2=SPELL+0x753000;CHILD2=SPELL+0x754000;CODE=SPELL+0x760000;STOP=0x3000000
mu.mem_map(STACK,0x10000);mu.mem_map(SPELL,0x800000);mu.mem_map(STOP,4096);mu.reg_write(UC_X86_REG_FPCW,0x37f)
def put(a,fmt,*v):mu.mem_write(a,struct.pack('<'+fmt,*v))
def get(a,fmt):return struct.unpack('<'+fmt,mu.mem_read(a,struct.calcsize('<'+fmt)))
def ret(n=0,value=None):
 sp=mu.reg_read(UC_X86_REG_ESP);address=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+4+n);mu.reg_write(UC_X86_REG_EIP,address)
 if value is not None:mu.reg_write(UC_X86_REG_EAX,value)
results=[];interrupts=[];bindings=[];sprites=[];native_presentation=False
def hook(uc,a,size,data):
 sp=mu.reg_read(UC_X86_REG_ESP)
 if a==0x4615a0:
  bindings.append({'bank':get(sp+4,'I')[0],'script':get(sp+12,'i')[0],'layer':mu.reg_read(UC_X86_REG_EAX)})
  put(get(sp+8,'I')[0],'I',42);ret(16,42)
 elif a==0x461920:ret(4,VM)
 elif a==0x461970:interrupts.append(mu.reg_read(UC_X86_REG_EBX));ret(4,0)
 elif a==0x461a70:ret(4,0)
 elif a in [0x460800,0x453d90]:ret(0,0)
 elif a==0x420f90 and not native_presentation:results.append({'failed':mu.reg_read(UC_X86_REG_EAX),'bonus':get(sp+4,'i')[0]});ret(4,0)
 elif a==0x454b80:sprites.append(mu.reg_read(UC_X86_REG_ECX));ret(0,0)
 elif a in [0x461d80,0x461d40]:ret(0,0)
mu.hook_add(UC_HOOK_CODE,hook)
def invalid(uc,access,address,size,value,data):
 print('unmapped '+hex(address)+' at '+hex(mu.reg_read(UC_X86_REG_EIP))+' ESP='+hex(mu.reg_read(UC_X86_REG_ESP)),flush=True)
 return False
mu.hook_add(UC_HOOK_MEM_INVALID,invalid)
def call(a,eax=TEXT,ecx=3,edx=0,esi=PLAYER,edi=SPELL,ebp=0,ebx=1,args=()):
 sp=STACK+0xf000;put(sp,'I'*(len(args)+1),STOP,*args)
 for r,v in [(UC_X86_REG_ESP,sp),(UC_X86_REG_EAX,eax),(UC_X86_REG_ECX,ecx),(UC_X86_REG_EDX,edx),(UC_X86_REG_ESI,esi),(UC_X86_REG_EDI,edi),(UC_X86_REG_EBP,ebp),(UC_X86_REG_EBX,ebx)]:mu.reg_write(r,v)
 mu.emu_start(a,STOP,count=1000000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError(hex(a)+' budget exceeded')
 return mu.reg_read(UC_X86_REG_EAX)
def reset(mode=1,difficulty=1,stage=1,flags=0):
 mu.mem_write(SPELL,bytes(0xc0));mu.mem_write(STATS,bytes(0x20000));mu.mem_write(BOMB,bytes(0x524));mu.mem_write(ENEMY,bytes(0x2800));mu.mem_write(PLAYER,bytes(0xc59c));results.clear();interrupts.clear();bindings.clear();sprites.clear()
 for a,p in [(0x4b43cc,SPELL),(0x4b4518,REPLAY),(0x4b451c,STATS),(0x4b43c4,BOMB),(0x4b43b8,FONT),(0x4b43c0,STD),(0x4b43c8,BULLETS),(0x4b43dc,ENEMIES),(0x4b4514,PLAYER),(0x4b43e4,HUD)]:put(a,'I',p)
 put(REPLAY+16,'i',mode);put(0x4b0ca8,'i',difficulty);put(0x4b0cb0,'i',stage);put(0x4b0cb8,'i',0);put(0x4b0c90,'i',0);put(0x4b0c94,'i',0);put(0x4b2ed0,'f',1);put(FONT+0x18fb4,'I',101);put(0x4cee70,'I',102);put(BULLETS+0x4debdc,'I',103);put(ENEMIES+28,'I',ENEMY);put(ENEMIES+72,'I',104);put(ENEMIES+76,'I',105);put(HUD+0x6d44,'I',106);put(ENEMY+0x1074,'3f',40,128,0);put(PLAYER+0x980,'f',300);put(SPELL+124,'I',flags);mu.mem_write(TEXT,b'Research\0')
 put(VM+16,'2I',CHILD1,NODE2);put(NODE2,'2I',CHILD2,0);put(CHILD1+0x3ea,'H',125);put(CHILD2+0x3ea,'H',126)
def snap():return {'flags':get(SPELL+124,'I')[0],'age':get(SPELL+40,'i')[0],'previousAge':get(SPELL+36,'i')[0],'cardId':get(SPELL+120,'i')[0],'bonus':get(SPELL+128,'i')[0],'initialBonus':get(SPELL+132,'i')[0],'limit':get(SPELL+136,'i')[0],'serial':get(SPELL+144,'i')[0],'ring':get(SPELL+172,'2f')}
starts=[]
for difficulty,stage,flags,bomb in [(0,1,0,0),(1,1,0,0),(3,7,0,0),(4,7,0,0),(2,3,127,0),(1,1,0,1)]:
 reset(difficulty=difficulty,stage=stage,flags=flags);put(BOMB+60,'i',bomb);call(0x40e060,args=(2400,));starts.append({'difficulty':difficulty,'stage':stage,'flagsBefore':flags,'bombActive':bool(bomb),'bindings':bindings.copy(),'after':snap()})
attempts=[]
for mode in [0,1]:
 for before in [0,99998,99999]:
  reset(mode=mode);local=STATS+0x66c+3*144;total=STATS+0x1aa24+3*144;put(local+132,'i',before);put(total+132,'i',before);call(0x40e060,args=(2400,));attempts.append({'mode':mode,'before':before,'localAttempts':get(local+132,'i')[0],'allAttempts':get(total+132,'i')[0]})
stage_bindings=[]
for stage in range(1,8):
 for stage_frame in [0,24]:
  reset(stage=stage);put(0x4b0cb8,'i',stage_frame);call(0x40e060,args=(2400,));stage_bindings.append({'stage':stage,'stageFrame':stage_frame,'bindings':bindings.copy()})
ticks=[]
for limit,survival in [(2400,False),(2100,False),(2400,True)]:
 reset();call(0x40e060,args=(limit,));
 if survival:call(0x4127a0)
 samples=[snap()]
 for frame in range(limit+1):call(0x40db40);samples.append(snap())
 ticks.append({'limit':limit,'survival':survival,'samples':samples})
loss=[]
for cause in ['bomb','miss','timeout']:
 for age in [0,59,60,61,300]:
  for survival in [False,True]:
   for bomb in [0,1]:
    reset();call(0x40e060,args=(2400,));put(SPELL+40,'i',age);put(BOMB+60,'i',bomb)
    if survival:call(0x4127a0)
    if cause=='bomb':call(0x406fd0)
    elif cause=='miss':
     sp=STACK+0xe000;mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_ESI,PLAYER);mu.reg_write(UC_X86_REG_EBP,0);mu.reg_write(UC_X86_REG_EBX,1);mu.emu_start(0x4382f3,0x438328,count=10000)
    else:
     put(ENEMY+0x2648,'i',100);put(ENEMY+0x12b0,'i',120)
     for i in range(8):put(ENEMY+0x270c+i*16,'i',-1)
     put(ENEMY+0x270c,'2i',0,120);call(0x41a6f0,ecx=ENEMY)
    after=snap();call(0x40db40);loss.append({'cause':cause,'age':age,'survival':survival,'bombActive':bool(bomb),'after':after,'nextTick':snap()})
ends=[]
for flags,bonus,score,mode in [(3,4000000,0,1),(3,123457,10,1),(1,0,10,1),(3,4000000,999800000,1),(3,0,0,1),(11,4000000,0,1),(3,4000000,0,0)]:
 reset(mode=mode);put(SPELL+124,'I',flags);put(SPELL+128,'i',bonus);put(SPELL+120,'i',3);put(0x4b0c44,'i',score);local=STATS+0x66c+3*144;total=STATS+0x1aa24+3*144;put(local+128,'i',99998);put(total+128,'i',99999);call(0x40e5e0)
 ends.append({'flagsBefore':flags,'bonus':bonus,'scoreBefore':score,'mode':mode,'afterFlags':get(SPELL+124,'I')[0],'score':get(0x4b0c44,'i')[0],'results':results.copy(),'localCaptures':get(local+128,'i')[0],'allCaptures':get(total+128,'i')[0]})
hover=[]
for flags,y in [(3,95.99),(3,96),(7,128),(7,128.01)]:
 reset();call(0x40e060,args=(2400,));put(SPELL+124,'I',flags);put(SPELL+40,'i',119);put(PLAYER+0x980,'f',y);put(ENEMY+0x1074,'2f',80,256);call(0x40db40);hover.append({'flagsBefore':flags,'playerY':y,'interrupts':interrupts.copy(),'after':snap()})
names=[];b=(TITLE/'local/retail/stage01.ecl').read_bytes();count=struct.unpack_from('<H',b,16)[0];table=36+struct.unpack_from('<H',b,6)[0];offsets=struct.unpack_from('<'+'I'*count,b,table)
for begin in offsets:
 p=begin+16
 while p+16<=len(b):
  op,size=struct.unpack_from('<HH',b,p+4)
  if size<16 or size%4 or p+size>len(b):break
  if op in [422,428,437,438,439]:
   instruction=b[p:p+size];mu.mem_write(CODE,instruction);sp=STACK+0x8000;put(sp+24,'I',CODE);mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EDI,CODE);mu.emu_start(0x41887c,0x4188d5,count=10000);length=struct.unpack_from('<I',instruction,28)[0];raw=bytes(mu.mem_read(sp+0x28c,length));names.append({'opcode':op,'encrypted':instruction[32:32+length].hex(),'decrypted':raw.hex(),'name':raw.split(b'\0')[0].decode('shift_jis')})
  p+=size
  if p in offsets:break
presentation=[];native_presentation=True
for failed,bonus in [(0,4000000),(0,123457),(0,0),(1,0)]:
 reset();call(0x420f90,eax=failed,esi=HUD,args=(bonus,));presentation.append({'failed':failed,'bonus':bonus,'bindings':bindings.copy(),'sprites':sprites.copy()})
out=TITLE/'local/reverse/spell-probe.json';out.write_text(json.dumps({'schema':'th12-research-spell-probe/1','exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'wholeGameOracle':False,'fixtures':['ANM bind/find/delete/interrupt use dummy handles and ring children125/126','460800 text and453d90 sound no-op','420f90 capture presentation records arguments except explicit native presentation cases','454b80 sprite bind and461d40/80 visibility callbacks are no-op for presentation cases','miss probes execute verified native4382f3..438328 basic block rather than full player death'],'banks':{'101':'FontManager bank','102':'spell text bank','103':'bullet bank','104':'EnemyManager main stage bank','105':'EnemyManager alternate stage bank','106':'HUD front bank'},'starts':starts,'attempts':attempts,'stageBindings':stage_bindings,'ticks':ticks,'loss':loss,'ends':ends,'hover':hover,'names':names,'presentation':presentation},indent=2,ensure_ascii=False)+'\n',encoding='utf8');print(out)
