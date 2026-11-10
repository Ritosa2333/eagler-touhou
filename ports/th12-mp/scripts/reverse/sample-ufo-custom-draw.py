"""TH12 original effect1/2 custom CPU vertices and captured D3D submissions.

No GPU execution, candidate math, resource edits, or full-world Oracle claim.
The VM and callback data are explicit synthetic inputs. Original410590/410840,
459a60,45d430/45d5d0 and angle helpers execute; only D3D methods are intercepted.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);PE=pefile.PE(str(EXE))
MANAGER,DEVICE,VTABLE,VM,DATA,SP,STOP=0x3000000,0x2000000,0x2001000,0x2002000,0x2003000,0x100e000,0x100f000
METHODS={0xe4:('render-state',3),0x10c:('texture-stage-state',4),0x114:('sampler-state',4),0x164:('fvf',2),0x14c:('draw-primitive-up',5)}
class Native:
 def __init__(self):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32)
  for base,size in [(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x10000),(MANAGER,0x900000)]:self.mu.mem_map(base,size)
  self.mu.mem_write(0x400000,PE.get_memory_mapped_image())
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  self.events=[];self.stubs={}
  self.put(0x4ce8cc,'I',MANAGER);self.put(0x4ce8f0,'I',DEVICE);self.put(DEVICE,'I',VTABLE)
  self.put(MANAGER+0x8856b0,'I',MANAGER+0x600000)
  self.put(MANAGER+0x4b5640,'B',255);self.put(MANAGER+0x4b5646,'B',255);self.put(MANAGER+0x4b5647,'B',1)
  for index,(offset,method) in enumerate(METHODS.items()):
   stub=0x100d000+index*16;self.stubs[stub]=method;self.put(VTABLE+offset,'I',stub)
   self.mu.hook_add(UC_HOOK_CODE,self.hook,begin=stub,end=stub)
 def put(self,at,fmt,*values):self.mu.mem_write(at,struct.pack('<'+fmt,*values))
 def get(self,at,fmt):return list(struct.unpack('<'+fmt,self.mu.mem_read(at,struct.calcsize('<'+fmt))))
 def hook(self,mu,at,size,user):
  name,count=self.stubs[at];sp=mu.reg_read(UC_X86_REG_ESP);args=self.get(sp+4,'I'*count)
  row={'kind':name,'args':args[1:]}
  if name=='draw-primitive-up':
   primitive,primitives,pointer,stride=args[1:]
   count=primitives+2 if primitive==6 else primitives+1
   if stride!=20:raise RuntimeError('Unexpected original custom vertex stride')
   row['primitive']=primitive;row['primitiveCount']=primitives;row['stride']=stride
   row['vertices']=[[*self.get(pointer+i*stride,'4f'),self.get(pointer+i*stride+16,'I')[0]] for i in range(count)]
  self.events.append(row);ret=self.get(sp,'I')[0]
  mu.reg_write(UC_X86_REG_ESP,sp+4+len(args)*4);mu.reg_write(UC_X86_REG_EAX,0);mu.reg_write(UC_X86_REG_EIP,ret)
 def draw(self,address):
  self.put(SP,'I',STOP);self.mu.reg_write(UC_X86_REG_ESP,SP);self.mu.reg_write(UC_X86_REG_ECX,VM)
  before=[*self.get(0x4ce568,'II'),*self.get(0x4ce560,'II')]
  self.mu.emu_start(address,STOP,count=100000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native custom draw budget')
  after=[*self.get(0x4ce568,'II'),*self.get(0x4ce560,'II')]
  return {'events':self.events,'rngUnchanged':before==after,'cacheAfter':self.get(MANAGER+0x4b5640,'8B')}
rows=[]
for blend in [0,1]:
 for radius,center,edge in [(0,0x80ffffff,0xffffff),(8,0xb000ff80,0x00108020),(32,0x30ff4080,0x00ff4080),(128,0xffffffff,0xffffff)]:
  n=Native();n.put(VM+0x478,'I',DATA);n.put(VM+0x47c,'I',blend<<5);n.put(VM+0x3bc,'I',0xffffffff)
  n.put(DATA,'ff',224.5,128.25);n.put(DATA+0x20,'f',radius);n.put(DATA+0x24,'II',center,edge)
  rows.append({'effect':2,'entry':'0x410840','inputs':{'center':[224.5,128.25],'radius':radius,'centerARGB':center,'edgeARGB':edge,'segments':32,'startAngle':0,'blend':blend},**n.draw(0x410840)})
for count in [2,8,16]:
 n=Native();n.put(VM+0x478,'I',DATA);n.put(VM+0x3bc,'I',0xff00ff00);n.put(DATA+0xc8,'I',count)
 points=[[100.25+i*3,200.5-i*.25] for i in range(count)];colors=[(((i+1)*255//count)<<24)|0x00ff00 for i in range(count)]
 for i,(point,color) in enumerate(zip(points,colors)):n.put(DATA+i*8,'ff',*point);n.put(DATA+0x80+i*4,'I',color)
 rows.append({'effect':1,'entry':'0x410590','inputs':{'points':points,'colorsARGB':colors,'blend':0},**n.draw(0x410590)})
result={'schema':'th12-original-ufo-custom-draw/1','originalExeSha256':EXE_SHA256,'gpuExecuted':False,'wholeGameOracle':False,
 'boundary':'Explicit callback data/VM; CPU custom geometry/material logic runs natively. D3D9 vtable methods alone are captured. No textures or candidate geometry loaded.',
 'vertexLayout':['x:f32','y:f32','z:f32','rhw:f32','argb:u32'],'fvf':0x44,'rows':rows}
out=ROOT/'tests/fixtures/ufo-custom-draw-original.json';out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'scenes':len(rows),'vertices':sum(len(e.get('vertices',[])) for r in rows for e in r['events']),'rngUnchanged':all(r['rngUnchanged'] for r in rows),'output':str(out)}))
