"""Explicit TH12 original hostile-bullet motion/collision probes, no golden edits."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
exe=WORKSPACE/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;BULLET=0x2000000;PLAYER=BULLET+0x10000;SHT=BULLET+0x20000;STOP=0x3000000
mu.mem_map(STACK,0x10000);mu.mem_map(BULLET,0x30000);mu.mem_map(STOP,4096)
mu.reg_write(UC_X86_REG_FPCW,0x37f)
def put(offset,fmt,*values):mu.mem_write(offset,struct.pack('<'+fmt,*values))
def get(offset,fmt):return struct.unpack('<'+fmt,mu.mem_read(offset,struct.calcsize('<'+fmt)))
def ret(cleanup=0):
    sp=mu.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup);mu.reg_write(UC_X86_REG_EIP,target)
emissions=[];fixture_animation=False
def hook(uc,address,size,data):
    if address==0x453d90:ret() # sound fixture only
    elif address==0x438370:ret() # geometric collision probe records return, not a real player miss
    elif address==0x455630 and fixture_animation:
        mu.reg_write(UC_X86_REG_EAX,0);ret(4) # Full state-1 logic only, renderer intentionally absent.
    elif address==0x40b5e0:
        p=mu.reg_read(UC_X86_REG_ESI);emissions.append({'type':get(p,'h')[0],'color':get(p+2,'h')[0],'position':get(p+4,'3f'),'angleSpread':get(p+16,'2f'),'speedSlow':get(p+24,'2f'),'countsMode':get(p+0x1f8,'3h'),'flags':get(p+0x200,'I')[0],'first':get(p+0x20c,'i')[0]});ret()
mu.hook_add(UC_HOOK_CODE,hook)
def call(addr,eax=BULLET,ecx=BULLET,edx=BULLET,esi=BULLET,args=()):
    sp=STACK+0xf000;put(sp,'I'*(len(args)+1),STOP,*args)
    for reg,value in [(UC_X86_REG_ESP,sp),(UC_X86_REG_EAX,eax),(UC_X86_REG_ECX,ecx),(UC_X86_REG_EDX,edx),(UC_X86_REG_ESI,esi)]:mu.reg_write(reg,value)
    mu.emu_start(addr,STOP,count=1000000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('original instruction budget exceeded at '+hex(addr))
    return mu.reg_read(UC_X86_REG_EAX)
def initialize(records,x=0,y=120,angle=0,speed=3):
    import math
    mu.mem_write(BULLET,bytes(0x9f8));put(BULLET+0x4bc,'3f',x,y,0);put(BULLET+0x4c8,'3f',math.cos(angle)*speed,math.sin(angle)*speed,0);put(BULLET+0x4d4,'2f',speed,angle);put(BULLET+0x544,'i',-1)
    for i,r in enumerate(records):put(BULLET+0x550+i*24,'ffiiII',r['a'],r['b'],r['c'],r['d'],r['kind'],int(r.get('concurrent',False)))
    call(0x40aa60)
def snapshot():return {'position':get(BULLET+0x4bc,'2f'),'velocity':get(BULLET+0x4c8,'2f'),'speed':get(BULLET+0x4d4,'f')[0],'angle':get(BULLET+0x4d8,'f')[0],'active':get(BULLET+0x528,'I')[0],'cursor':get(BULLET+0x548,'i')[0]}
put(0x4b2ed0,'f',1);put(0x4b4514,'I',PLAYER);mu.mem_write(PLAYER,bytes(0xc59c));put(PLAYER+0xa2c,'I',SHT);put(PLAYER+0xa28,'i',1);put(PLAYER+0x97c,'2f',0,0);put(SHT+4,'f',2)
motion=[]
for kind,a,b,c,d,x,y,angle,speed,fn in [
 (4,.053333335,-999999,30,-999999,0,120,0,1,0x40b6f0),
 (4,-.083333336,-999999,30,-999999,0,120,0,3,0x40b6f0),
 (8,.01,.02,10,0,0,120,0,3,0x40b920),
 (1,0,0,0,0,0,120,0,3,0x40b670),
 (256,-999999,-999999,1,29,195,120,0,3,0x40bef0),
 (256,2,-999999,2,15,195,-5,.3,3,0x40bef0),
]:
    records=[{'a':a,'b':b,'c':c,'d':d,'kind':kind,'concurrent':True}];initialize(records,x,y,angle,speed);samples=[snapshot()]
    for frame in range(40 if kind in [1,4,8] else 1):
        # Retail 4099e0 calls each motion callback only while its active bit is set.
        if get(BULLET+0x528,'I')[0]&kind:call(fn)
        samples.append(snapshot())
    motion.append({'record':records[0],'initial':{'x':x,'y':y,'angle':angle,'speed':speed},'nativeFunction':hex(fn),'samples':samples})
records=[{'a':.75,'b':2.,'c':50923526,'d':13,'kind':524288},{'a':.42,'b':.12,'c':2,'d':0,'kind':1048576}];emissions.clear();initialize(records);split={'records':records,'emissions':emissions.copy(),'after':snapshot()}
sequences=[];fixture_animation=True
for kind,duration in [(4096,3),(1024,3),(4096,60)]:
    records=[{'a':-999999,'b':-999999,'c':duration,'d':0,'kind':kind},{'a':.5,'b':-999999,'c':2,'d':0,'kind':4}]
    initialize(records,speed=0);put(BULLET,'I',2);put(BULLET+0x532,'h',1);put(BULLET+0x404,'i',1);samples=[snapshot()]
    for frame in range(duration+8):call(0x4099e0,args=(BULLET,));samples.append(snapshot())
    sequences.append({'records':records,'initial':{'x':0,'y':120,'angle':0,'speed':0},'nativeFunction':'0x4099e0 state-1 branch with ANM update fixture','samples':samples})
fixture_animation=False
collision=[]
for shape,size in [('rectangle',4),('rectangle',10),('circle',14)]:
    for x,y in [(0,0),(2.9,0),(3,0),(3.01,0),(2.9,2.9),(5,0),(6,0),(6.01,0),(14,0),(15,0),(25,0),(25.01,0),(41,0),(43,0)]:
        put(BULLET+0x4bc,'3f',x,y,0);put(BULLET+0x4dc,'2f',size,size);put(PLAYER+0x9cc,'2f',-1,-1);put(PLAYER+0x9d8,'2f',1,1)
        value=call(0x437810,eax=BULLET+0x4dc,ecx=BULLET+0x4bc) if shape=='rectangle' else call(0x437980,eax=BULLET+0x4bc,args=(struct.unpack('<I',struct.pack('<f',size))[0],))
        collision.append({'shape':shape,'size':size,'playerHit':2,'x':x,'y':y,'result':value})
appearances=[]
for i in range(31):
    addr=0x4af280+i*0xd0;appearances.append({'type':i,'script':get(addr,'i')[0],'hitbox':get(addr+0xc4,'f')[0],'group':get(addr+0xc8,'i')[0],'effect':get(addr+0xcc,'i')[0]})
out=TITLE/'local/reverse/bullet-probe.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps({'schema':'th12-research-bullet-probe/1','wholeGameDeterminism':False,'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'provider':'original x86 motion/dispatch/collision instructions under Unicorn 2.1.4','fixtures':['453d90 sound no-op','438370 player miss no-op for geometric query','40b5e0 records child emitter without allocating bullets','455630 ANM update no-op for state-1 sequencing probes'],'motion':motion,'sequences':sequences,'split':split,'collision':collision,'appearances':appearances},indent=2)+'\n')
print(out)
