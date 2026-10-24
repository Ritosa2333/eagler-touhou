"""Original UFO HUD/native draw call capture, with explicit GPU/font fixtures.

No candidate runtime, golden, replay file or original game file is modified.
"""
import itertools,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,HUD,FONT,UFO,ENEMY,LIST,DUMMY,RESOURCE,STOP=0x1000000,0x2000000,0x2020000,0x2050000,0x2060000,0x2070000,0x2080000,0x2100000,0x100f000
mu.mem_map(STACK,0x10000);mu.mem_map(HUD,0x600000)
raw=(ROOT/'local/retail/front.anm').read_bytes();install_resource(mu,raw,RESOURCE)
put=lambda at,fmt,*v:mu.mem_write(at,struct.pack('<'+fmt,*v))
read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
calls=[];capture_ticks=False
def text(at):return bytes(mu.mem_read(at,128)).split(b'\0',1)[0].decode('ascii')
def ret(cleanup=0):
 sp=mu.reg_read(UC_X86_REG_ESP);mu.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
def hook(uc,address,size,data):
 sp=mu.reg_read(UC_X86_REG_ESP)
 if address==0x4615a0:
  bank,handle,script,flags=read(sp+4,'4I');layer=mu.reg_read(UC_X86_REG_EAX);put(handle,'I',DUMMY);mu.reg_write(UC_X86_REG_EAX,DUMMY)
  calls.append({'kind':'factory-fixture','bank':bank,'script':script,'layer':layer,'flags':flags});ret(16)
 elif address==0x461920:mu.reg_write(UC_X86_REG_EAX,DUMMY);ret(4)
 elif address==0x455630 and capture_ticks:
  obj=read(sp+4,'I')[0];calls.append({'kind':'original-tick','physicalSlot':(obj-HUD-0x54b8)//0x4b4 if obj!=DUMMY else 'pulse-fixture','interrupt':read(obj+0x3c4,'h')[0]})
 elif address==0x454b80 and capture_ticks:
  obj=mu.reg_read(UC_X86_REG_EAX);calls.append({'kind':'original-sprite-bind','physicalSlot':(obj-HUD-0x54b8)//0x4b4,'sprite':mu.reg_read(UC_X86_REG_ECX)})
 elif address in [0x4020a0,0x4021a0]:
  pos=mu.reg_read(UC_X86_REG_EDI if address==0x4020a0 else UC_X86_REG_EDX)
  call={'kind':'font-draw-fixture','entry':hex(address),'position':read(pos,'3f'),'font':read(FONT+0x18f98,'i')[0],
   'colorARGB':read(FONT+0x18f80,'I')[0],'scale':read(FONT+0x18f84,'2f'),'drawFlag':read(FONT+0x18f9c,'i')[0],'alignment':read(FONT+0x18fa4,'2i')}
  if address==0x4020a0:
   fmt=text(read(sp+4,'I')[0]);call['format']=fmt
   call['arguments']=read(sp+8,'d') if fmt=='*%1.1f' else read(sp+8,'2i' if fmt=='%2d:%.2d' else 'i')
  else:call['groupedNumber']=mu.reg_read(UC_X86_REG_ECX)
  calls.append(call);ret()
 elif address==0x412530:mu.reg_write(UC_X86_REG_EAX,ENEMY);ret()
mu.hook_add(UC_HOOK_CODE,hook);mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
put(0x4b2ed0,'f',1);put(0x4ce8cc,'I',0x2090000);put(0x4b43e4,'I',HUD);put(0x4b43b8,'I',FONT);put(0x4b43dc,'I',LIST);put(LIST+0x68,'I',LIST+0x100);put(LIST+0x100,'II',ENEMY,0);put(ENEMY+0x27b4,'I',42);put(ENEMY+0x1074,'3f',40,128,0);put(ENEMY+0x2648,'2i',1234,1900)
put(FONT+0x18f80,'I',0xffffffff);put(FONT+0x18f84,'2f',1,1);put(FONT+0x18fa4,'2i',1,1);put(HUD+0x6d44,'I',RESOURCE)
def call(address,args=(),ecx=0,ebx=0,eax=0):
 sp=STACK+0xe000;put(sp,'I'*(len(args)+1),STOP,*(n&0xffffffff for n in args))
 for reg,value in [(UC_X86_REG_ESP,sp),(UC_X86_REG_ECX,ecx),(UC_X86_REG_EBX,ebx),(UC_X86_REG_EAX,eax)]:mu.reg_write(reg,value)
 mu.emu_start(address,STOP,count=300000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original function exceeded budget '+hex(address))
def pose(obj):return {'position':read(obj+0x424,'3f'),'extraOffset':read(obj+0x43c,'3f'),'scale':read(obj+0x40,'2f'),'sprite':read(obj+0x3e4,'h')[0],'alpha':read(obj+0x3bf,'B')[0],'clock':read(obj+0x6c,'i')[0]}
slots=[HUD+0x54b8+n*0x4b4 for n in [1,2,3]]
inventory=[]
for rotation,selected in itertools.product(range(3),[-1,0,1,2]):
 capture_ticks=False
 for obj in slots:call(0x454d10,(obj,80),ecx=RESOURCE)
 put(HUD+0x6788,'i',rotation);put(0x4b0c4c,'3i',1,2,3);calls.clear();capture_ticks=True
 call(0x4214b0,(HUD,selected));inventory.append({'rotation':rotation,'selectedLogicalSlot':selected,'calls':calls.copy(),'poses':[pose(obj) for obj in slots]})
rotation_records=[]
for rotation in range(3):
 put(HUD+0x6788,'i',rotation);calls.clear();call(0x421a60)
 rotation_records.append({'before':rotation,'after':read(HUD+0x6788,'i')[0],'calls':calls.copy(),'poses':[pose(obj) for obj in slots]})
blink=[]
for rotation,on in itertools.product(range(3),[0,1]):
 put(HUD+0x6788,'i',rotation);calls.clear();call(0x421690,(on,))
 blink.append({'rotation':rotation,'on':on,'calls':calls.copy(),'sprites':[pose(obj)['sprite'] for obj in slots]})
capture_ticks=False;draw_records=[]
for age,display in [(19,0),(20,0),(59,0),(60,0),(60,1),(60,4),(60,5),(60,9),(60,10),(60,119),(60,120)]:
 mu.mem_write(UFO,bytes(0x100));put(UFO+0x14,'i',age);put(UFO+0x28,'i',720);put(UFO+0x34,'2I',ENEMY,42);put(UFO+0x44,'I',DUMMY)
 put(UFO+0x48,'2if',12,34,.75);put(UFO+0x54,'3f',264,144,0);put(UFO+0x60,'f',6);put(UFO+0x64,'2i',display,1234560)
 calls.clear();call(0x44a4f0,ebx=UFO);call(0x44a5d0,ebx=UFO)
 draw_records.append({'age':age,'displayFrames':display,'calls':calls.copy(),'healthScale':read(DUMMY+0x40,'f')[0],'healthEnginePosition':read(DUMMY+0x430,'3f')})
OUT=ROOT/'artifacts/reverse/ufo-presentation-original.json';OUT.parent.mkdir(parents=True,exist_ok=True)
OUT.write_text(json.dumps({'schema':'th12-original-ufo-presentation-samples/1','originalExeSha256':EXE_SHA256,
 'wholeGameOracle':False,'wholePresentationOracle':False,'gpuExecuted':False,
 'provider':'Original4214b0/421a60/421690 and44a4f0/44a5d0; embedded front80 uses actual454d10/454b80/455630 and retail bytecode.',
 'fixtureCallbacks':['4615a0 pulse factory and461920 lookup return dummy VM; pulse script81 itself is not executed in this call capture.',
 '4020a0/4021a0 record native arguments and FontManager fields without formatting/drawing.',
 '412530 lookup returns the one synthetic live enemy with matching ID.'],
 'inventory':inventory,'rotation':rotation_records,'blink':blink,'draw':draw_records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(OUT),'inventoryCases':len(inventory),'rotationCases':len(rotation_records),'blinkCases':len(blink),'drawCases':len(draw_records)}))
