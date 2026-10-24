"""Pin TH12 1.00b and capture real CPU vertex generation, without GPU draws.

No candidate geometry runs here. The original UFO ANM postprocessor, flat ANM
quad builder, and curve-laser vtable draw fill the compared vertices. Resource
records omit texture objects; host allocation/audio/GPU submission are fixtures.
"""
import hashlib,json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE)
PE=pefile.PE(str(EXE));RAW=(ROOT/'local/retail/bullet.anm').read_bytes()

class Native:
 def __init__(self):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32)
  for a,n in [(0,0x10000),(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
   (0x1000000,0x10000),(0x2000000,0x1000000),(0x3000000,0x1000000),
   (0x4000000,0x1000000),(0x5000000,0x1000000)]:self.mu.mem_map(a,n)
  self.mu.mem_write(0x400000,PE.get_memory_mapped_image())
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  self.heap=0x3000000;self.draws=[];self.label=''
  self.resource=install_resource(self.mu,RAW,0x5000000,0)
  for a,f,x in [(0x4b2ed0,'f',[1]),(0x4ce8cc,'I',[0x4000000]),(0x4ce568,'II',[0x1234,0]),
   (0x4ce560,'II',[0xbeef,0]),(0x4d52d4,'I',[1]),(0x4d52dc,'I',[1])]:self.put(a,f,*x)
  self.put(0x4cee34,'I',0x2070000);self.put(0x2070000+0xcc,'4I',0,0,10000,10000)
  self.put(0x4000000+0x8356a4,'I',0x4800000)
  # Execute the original constructor's constant quad/RHW initialization block.
  # Its preceding 4096 VM construction and following D3D creation are outside
  # this independent draw probe. The block leaves the x87 stack balanced.
  self.mu.reg_write(UC_X86_REG_ESP,0x100e000)
  self.mu.reg_write(UC_X86_REG_EDI,0x4000000)
  self.mu.emu_start(0x45e02c,0x45e0cb,count=1000)
  self.mu.hook_add(UC_HOOK_CODE,self.hook)
  # Execute retail sprite record normalization as well as its later bind.
  # Texture dimensions are header dimensions in this GPU-free resource fixture.
  for i in range(self.resource['spriteCount']):
   self.call(0x460610,regs={UC_X86_REG_EAX:i,UC_X86_REG_EDX:self.resource['resource'],
    UC_X86_REG_EBX:self.resource['sprites']+i*72})
 def put(self,a,f,*x):self.mu.mem_write(a,struct.pack('<'+f,*x))
 def get(self,a,f):return list(struct.unpack('<'+f,self.mu.mem_read(a,struct.calcsize('<'+f))))
 def ret(self,pop=0,value=0):
  sp=self.mu.reg_read(UC_X86_REG_ESP);target=self.get(sp,'I')[0]
  self.mu.reg_write(UC_X86_REG_ESP,sp+4+pop);self.mu.reg_write(UC_X86_REG_EAX,value);self.mu.reg_write(UC_X86_REG_EIP,target)
 def vertices(self,p,count):
  out=[]
  for i in range(count):
   w=self.get(p+i*28,'7I');f=self.get(p+i*28,'7f')
   out.append([*f[:4],w[4],*f[5:]])
  return out
 def state(self,p):
  flags=self.get(p+0x47c,'I')[0]
  sprite=self.get(p+0x3f4,'I')[0]
  return {'position':self.get(p+0x424,'3f'),'engine':self.get(p+0x430,'3f'),
   'offset':self.get(p+0x43c,'3f'),'rotation':self.get(p+0x24,'3f'),
   'scale':self.get(p+0x40,'2f'),'uvScroll':self.get(p+0x60,'2f'),
   'uvScale':self.get(p+0x50,'2f'),'uvCorners':self.get(p+0x7c,'8f'),
   'dimensions':self.get(p+0x58,'2f'),'resourceDimensions':[self.get(sprite+0x38,'f')[0],self.get(sprite+0x34,'f')[0]],'integers':self.get(p+0x3fc,'4i'),
   'primary':self.get(p+0x3bc,'I')[0],'secondary':self.get(p+0x3c0,'I')[0],
   'sprite':self.get(p+0x3e4,'h')[0],'flags':flags,'flags2':self.get(p+0x480,'I')[0],
   'layer':self.get(p+0x20,'i')[0],'mode':(flags>>23)&31,'blend':(flags>>5)&7,
   'anchorX':(flags>>19)&3,'anchorY':(flags>>21)&3}
 def hook(self,mu,a,n,data):
  sp=mu.reg_read(UC_X86_REG_ESP)
  if a in [0x46c9ea,0x46d04a]:
   size=self.get(sp+4,'I')[0];out=self.heap;self.heap+=(size+15)&~15
   if self.heap>=0x4000000:raise RuntimeError('Native heap fixture exhausted')
   self.ret(value=out)
  elif a in [0x46ca4f,0x46c941,0x453d90,0x453f10,0x459cf0,0x45a3c0]:self.ret()
  elif a==0x453e20:self.ret(4)
  elif a==0x45a4a0:
   # Execute the original quad -> six-vertex TriangleList copy, not a guessed
   # strip conversion. Only its subsequent device submission is omitted.
   manager=mu.reg_read(UC_X86_REG_EAX)
   self.quad_buffer=self.get(manager+0x8356a4,'I')[0]
  elif a==0x45a52a:
   self.draws.append({'label':self.label,'primitive':'triangle-list','vertices':self.vertices(self.quad_buffer,6)})
  elif a==0x45c3a0:
   # UFO and curve geometry has already been generated before this GPU helper.
   vm=mu.reg_read(UC_X86_REG_EAX);p,count=self.get(sp+4,'II')
   self.draws.append({'label':self.label,'primitive':'triangle-strip','vertices':self.vertices(p,count),
    'vm':self.state(vm)})
   self.ret(8)
 def call(self,a,args=(),regs=None):
  sp=0x100e000;stop=0x100f000;self.put(sp,'I'*(len(args)+1),stop,*args)
  self.mu.reg_write(UC_X86_REG_ESP,sp)
  for r,x in (regs or {}).items():self.mu.reg_write(r,x)
  try:self.mu.emu_start(a,stop,count=3000000)
  except Exception as e:raise RuntimeError(f'Native {a:x} failed at {self.mu.reg_read(UC_X86_REG_EIP):x}') from e
  if self.mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native instruction budget')
  return self.mu.reg_read(UC_X86_REG_EAX)

records=[]
for name,fill,irq,age,parent in [('empty',0,0,60,False),('quarter',.25,0,60,False),
 ('half',.5,0,60,True),('full',1,0,60,True),('full-yellow',1,2,60,False),
 ('full-red',1,2,62,False),('full-green',1,2,64,False),('full-cyan',1,2,66,False),
 ('fade',.75,1,70,False)]:
 n=Native();vm=0x2100000;p=0x2110000
 n.call(0x454d10,[vm,206],{UC_X86_REG_ECX:n.resource['resource']})
 n.put(vm+0x430,'3f',224,116,2);n.put(vm+0x424,'3f',3,4,1);n.put(vm+0x43c,'3f',2,1,3)
 if parent:
  n.put(vm+0x3c,'I',p);n.put(p+0x424,'3f',11,12,1);n.put(p+0x430,'3f',31,21,2);n.put(p+0x43c,'3f',2,3,4)
 for tick in range(1,age+1):
  n.put(vm+0x24,'f',float(struct.unpack('<f',struct.pack('<f',6.2831854820251465*fill))[0]))
  if tick==60 and irq:n.put(vm+0x3c4,'h',irq)
  n.call(0x455630,[vm])
 n.label=name;n.call(0x45c900,[0x4000000],{UC_X86_REG_EAX:vm})
 state=n.state(vm);origin=[state['position'][i]+state['engine'][i]+state['offset'][i] for i in range(3)]
 if parent:origin=[origin[i]+n.get(p+0x424,'3f')[i]+n.get(p+0x430,'3f')[i]+n.get(p+0x43c,'3f')[i] for i in range(3)]
 records.append({'kind':'ufo206','name':name,'fill':fill,'interrupt':irq,'age':age,
  'state':state,'origin':origin,'vertices':n.draws[0]['vertices']})

profiles=json.loads((ROOT/'artifacts/reverse/combat/native-probe.json').read_text())['laserProfiles']
for row in profiles:
 kind=row['factory']['kind'];n=Native();lm,player,sht,params=0x2000000,0x2010000,0x2020000,0x2040000
 n.put(0x4b4514,'I',player);n.put(0x4b43dc,'I',0x2030000);n.put(player+0xa28,'i',1);n.put(player+0xa2c,'I',sht);n.put(sht+4,'f',2)
 n.put(player+0x97c,'3f',5000,5000,0);n.put(player+0x9cc,'6f',4999,4999,0,5001,5001,0)
 n.call(0x427fa0,regs={UC_X86_REG_EDI:lm});n.put(lm+0x464,'I',lm+0x10);n.put(lm+0x488,'I',n.resource['resource'])
 n.put(0x4b43d4,'I',0x2060000);n.put(0x2060010,'I',n.resource['resource'])
 n.mu.mem_write(params,bytes.fromhex(row['factory']['profileHex']));n.call(0x428450,[kind],{UC_X86_REG_EDI:params})
 obj=n.get(lm+0x18,'I')[0];vm=obj+{0:0x63c,1:0x660,2:0x634}[kind]
 draw={0:0x429af0,1:0x42b100,2:0x42cca0}[kind]
 for tick in range(41):
  n.call(0x428270,regs={UC_X86_REG_EDI:lm})
  if tick not in [0,1,8,20,40]:continue
  n.draws=[];n.label=f'laser{kind}-age{tick+1}';n.call(draw,regs={UC_X86_REG_ECX:obj})
  nodes=[]
  if kind==2:
   count=n.get(obj+0x470,'i')[0];base=n.get(obj+0xf9c,'I')[0]
   nodes=[n.get(base+i*20,'5f') for i in range(count)]
  state=n.state(vm)
  records.append({'kind':'curve' if kind==2 else 'straight','name':n.label,'laserKind':kind,
   'laserPosition':n.get(obj+0x50,'3f'),'laserAngle':n.get(obj+0x68,'f')[0],
   'laserWidth':n.get(obj+0x70,'f')[0],'curveDrawWidth':n.get(obj+0x464,'f')[0] if kind==2 else None,
   'laserLength':n.get(obj+0x6c,'f')[0],'headOffset':n.get(obj+0x78,'f')[0],
   'age':n.get(obj+0x18,'i')[0],'nodes':nodes,'state':state,'draws':n.draws})
 if kind in [0,1]:
  for head in [0,.01,32]:
   n.put(obj+0x78,'f',head);n.draws=[];n.label=f'laser{kind}-head{head}'
   n.call(draw,regs={UC_X86_REG_ECX:obj})
   records.append({'kind':'straight','name':n.label,'laserKind':kind,'laserPosition':n.get(obj+0x50,'3f'),
    'laserAngle':n.get(obj+0x68,'f')[0],'laserWidth':n.get(obj+0x70,'f')[0],
    'laserLength':n.get(obj+0x6c,'f')[0],'headOffset':head,'age':n.get(obj+0x18,'i')[0],
    'state':n.state(vm),'draws':n.draws})
  for width in [18,0,-6]:
   n.put(vm+0x58,'f',width);n.draws=[];n.label=f'laser{kind}-vmwidth{width}'
   n.call(draw,regs={UC_X86_REG_ECX:obj})
   records.append({'kind':'straight','name':n.label,'laserKind':kind,'laserPosition':n.get(obj+0x50,'3f'),
    'laserAngle':n.get(obj+0x68,'f')[0],'laserWidth':n.get(obj+0x70,'f')[0],
    'laserLength':n.get(obj+0x6c,'f')[0],'headOffset':32,'age':n.get(obj+0x18,'i')[0],
    'state':n.state(vm),'draws':n.draws})
 # Exercise original curve join construction on wrapped and sharp bends.
 if kind==2:
  # Larger synthetic arrays need independent sufficiently sized buffers. The
  # factory profile allocates only two nodes; reusing that allocation after
  # changing count would overwrite the source history during native drawing.
  n.put(obj+0xf9c,'I',0x2080000);n.put(obj+0xfa0,'I',0x2090000)
  angles_cases=[('wrap-positive',[3.1,-3.1]),('wrap-negative',[-3.1,3.1]),
   ('bend-positive',[0,1.2]),('bend-negative',[0,-.8]),('sharp',[0,2.9]),
   ('long-bend',[0,1.2,-.8,2.9]),('long-wrap',[3.1,-3.1,3.05,-3.05]),
   ('eight',[0,.2,.4,.7,1.2,1.9,2.7,-3.1]),
   ('twelve',[-3.1,3.1,2.5,1.8,.8,0,-.8,-1.8,-2.5,-3.1,3.1,2.7])]
  for name,angles in angles_cases:
   base=n.get(obj+0xf9c,'I')[0];n.put(obj+0x470,'i',len(angles))
   nodes=[[i*20-40,100+i*15,0,a,3] for i,a in enumerate(angles)]
   for i,x in enumerate(nodes):n.put(base+i*20,'5f',*x)
   n.draws=[];n.label='curve-'+name;n.call(draw,regs={UC_X86_REG_ECX:obj})
   records.append({'kind':'curve','name':n.label,'laserKind':2,'laserWidth':n.get(obj+0x70,'f')[0],
    'curveDrawWidth':n.get(obj+0x464,'f')[0],'nodes':nodes,'state':n.state(vm),'draws':n.draws})
  # Original custom draw ignores collision width, scroll, and VM diffuse color.
  n.put(obj+0x70,'f',60);n.put(vm+0x60,'2f',.25,.5);n.put(vm+0x3bc,'I',0x808899aa)
  n.draws=[];n.label='curve-uses-configuration-width-and-white';n.call(draw,regs={UC_X86_REG_ECX:obj})
  records.append({'kind':'curve','name':n.label,'laserKind':2,'laserWidth':60,
   'curveDrawWidth':n.get(obj+0x464,'f')[0],'nodes':nodes,'state':n.state(vm),'draws':n.draws})

out=ROOT/'tests/fixtures/custom-geometry-original.json'
out.write_text(json.dumps({'schema':'th12-native-custom-geometry/1','originalExeSha256':EXE_SHA256,
 'resourceSha256':hashlib.sha256(RAW).hexdigest(),'wholeGameOracle':False,'gpuExecuted':False,
 'provider':'Real460610 sprite normalization,455630 mode13,429af0/42b100/42cca0,45af10/459e50 CPU vertices,45a4a0 batch copy and45e02c quad constants.',
 'fixtures':['GPU-free original sprite records; no texture handles','host heap allocation/free/audio',
 '459cf0 blend-state and45a3c0 GPU flush return;45a4a0 original six-vertex batch copy;45c3a0 GPU submission captured after original CPU generation',
 'synthetic UFO poses/fill, synthetic far-away Player, ECL-created original laser profile buffers; synthetic2/4/8/12-node curve arrays with sufficient separate mapped source/output buffers',
 'wide synthetic viewport prevents screen culling; manager global translation/color modulation disabled'],
 'limitations':['GPU not executed; no filtering/pixel comparison','synthetic mapped history arrays test original continuous draw math; factory/manager lifecycle is tested separately'],
 'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'cases':len(records),'vertices':sum(len(r.get('vertices',[]))+sum(len(d['vertices']) for d in r.get('draws',[])) for r in records)}))
