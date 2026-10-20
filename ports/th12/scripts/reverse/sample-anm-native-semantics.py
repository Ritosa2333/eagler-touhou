"""Native ANM child offsets, direct-child interrupts and white sprite fallback.

Executes the pinned retail functions; synthetic scripts isolate reviewed rules.
No candidate simulator, graphics hooks or accepted replay/golden are involved.
"""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
def cmd(op,fmt='',values=(),time=0,mask=0):
 data=struct.pack('<'+fmt,*values);return struct.pack('<HHhH',op&65535,len(data)+8,time,mask)+data
def bank(scripts):
 h=bytearray(64+8*len(scripts));struct.pack_into('<I',h,0,7);struct.pack_into('<H',h,6,len(scripts));off=len(h)
 for i,s in enumerate(scripts):struct.pack_into('<iI',h,64+i*8,i,off);off+=len(s)
 return bytes(h)+b''.join(scripts)
def environment(raw):
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
 mu.mem_map(0x1000000,0x10000);mu.mem_map(0x2000000,0x1000000)
 manager,res,out,stack,stop=0x2000000,0x2a00000,0x2b00000,0x100e000,0x100f000
 put=lambda at,fmt,*v:mu.mem_write(at,struct.pack('<'+fmt,*v))
 read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
 install_resource(mu,raw,res);ascii_res=0x2c00000;install_resource(mu,(ROOT/'local/retail/ascii.anm').read_bytes(),ascii_res)
 put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0)
 put(0x4b43b8,'I',out+0x1000);put(out+0x1000+0x18fb4,'I',ascii_res)
 mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
 def call(at,args=(),registers=None):
  put(stack,'I'*(len(args)+1),stop,*(n&0xffffffff for n in args));mu.reg_write(UC_X86_REG_ESP,stack)
  for reg,value in (registers or {}).items():mu.reg_write(reg,value)
  mu.emu_start(at,stop,count=300000)
  if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Native ANM budget {at:x}')
 def objects():
  head=read(manager+0x8856b8,'I')[0];result=[]
  while head:
   obj,nxt=read(head,'II');result.append(obj);head=nxt
   if len(result)>20:raise RuntimeError('Native list budget')
  return result
 def snapshot():
  result=[]
  for obj in objects():
   children=[];at=read(obj+0x14,'I')[0]
   while at:
    c,nxt=read(at,'II');children.append(read(c+0x3ea,'h')[0]);at=nxt
    if len(children)>20:raise RuntimeError('Native child list budget')
   result.append({'script':read(obj+0x3ea,'h')[0],'clock':read(obj+0x6c,'i')[0],
    'pendingInterrupt':read(obj+0x3c4,'h')[0],'scale':read(obj+0x40,'2f'),'float0':read(obj+0x40c,'f')[0],
    'position':read(obj+0x424,'3f'),'engine':read(obj+0x430,'3f'),'offset':read(obj+0x43c,'3f'),
    'attached':bool(read(obj+0x18,'I')[0]),'children':children})
  return {'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II'),'primary':result}
 return mu,put,read,call,objects,snapshot,res,out,ascii_res

hold=cmd(63);end=cmd(-1)
scripts=[]
for script in range(5):
 birth=(cmd(88,'i',[1])+cmd(88,'i',[2])) if script==0 else cmd(88,'i',[3]) if script==1 else b''
 on7=cmd(7,'ff',[10004,10011],mask=3)+(cmd(88,'i',[4]) if script==0 else b'')+cmd(50,'ff',[script+2,script+3])+hold
 scripts.append(birth+hold+cmd(64,'i',[7])+on7+end)
graph=bank(scripts);interrupts=[]
for immediate,target_script in [(False,0),(True,0),(False,1),(True,1)]:
 mu,put,read,call,objects,snapshot,res,out,_=environment(graph)
 call(0x4615a0,[res,out,0,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:0})
 before=snapshot();target=next(obj for obj in objects() if read(obj+0x3ea,'h')[0]==target_script)
 handle=read(target,'I')[0]
 call(0x4619e0 if immediate else 0x461970,[handle],{UC_X86_REG_EBX:7})
 after=snapshot();call(0x460fe0,registers={UC_X86_REG_EDI:0x2000000})
 interrupts.append({'immediate':immediate,'targetScript':target_script,'scriptBankHex':graph.hex(),'before':before,'after':after,'afterPrimary':snapshot()})

offsets=[]
for opcode in [96,97]:
 raw=bank([cmd(48,'fff',[10,20,0])+cmd(opcode,'iff',[1,10011,10011],mask=6)+hold+end,
  cmd(7,'ff',[10004,10011],mask=3)+cmd(48,'fff',[3,4,0])+hold+end])
 mu,put,read,call,objects,snapshot,res,out,_=environment(raw)
 pos=out+0x100;put(pos,'3f',12,34,0);call(0x4615a0,[res,out,0,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:pos})
 offsets.append({'opcode':opcode,'scriptBankHex':raw.hex(),'snapshot':snapshot()})

white=[]
for sprite in [-1,-2,-999]:
 raw=bank([cmd(3,'i',[sprite])+hold+end]);mu,put,read,call,objects,snapshot,res,out,ascii_res=environment(raw)
 call(0x4615a0,[res,out,0,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:0});obj=objects()[0]
 selected=read(obj+0x3f4,'I')[0];table=read(ascii_res+0x118,'I')[0]
 white.append({'requestedSprite':sprite,'scriptBankHex':raw.hex(),'sprite':read(obj+0x3e4,'h')[0],
  'usesAsciiRecord':selected==table+264*72,'scriptBankRetained':read(obj+0x3f8,'I')[0]==res,
  'spriteSize':read(obj+0x58,'2f'),'snapshot':snapshot()})
rotations=[]
for angle,velocity in [(6.283185482,(0,0,0)),(-6.283185482,(0,0,0)),(10,(0,0,0)),(10,(.1,0,0)),(10,(0,.1,0)),(10,(0,0,.1))]:
 raw=bank([cmd(53,'fff',velocity)+hold+end]);mu,put,read,call,objects,snapshot,res,out,_=environment(raw)
 call(0x4615a0,[res,out,0,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:0});obj=objects()[0]
 put(obj+0x24,'3f',angle,angle,angle);call(0x455630,[obj])
 rotations.append({'scriptBankHex':raw.hex(),'angleWritten':angle,'velocity':velocity,'rotationAfterTick':read(obj+0x24,'3f')})
report={'schema':'th12-original-anm-native-semantics/1','originalExeSha256':EXE_SHA256,
 'provider':'Actual4615a0/4621c0/461250/461720/461840/461970/4619e0/460fe0/454d10/454b80/455630; synthetic raw scripts; GPU-free resource record tables; no logic hooks.',
 'wholeGameOracle':False,'wholePresentationOracle':False,'gpuExecuted':False,
 'interrupts':interrupts,'offsets':offsets,'whiteSprites':white,'externalRotations':rotations}
path=ROOT/'tests/fixtures/anm-native-semantics-original.json';path.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(path),'interruptCases':len(interrupts),'offsetCases':len(offsets),'whiteSpriteCases':len(white)}))
