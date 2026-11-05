"""Original three laser factories/manager updates with original bullet ANM.

Inputs are synthetic parameter buffers built by original ECL handlers in
combat-probe.py. Host heap/audio only are fixtures; player is placed far away.
"""
import collections,json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
profiles=json.loads((ROOT/'artifacts/reverse/combat/native-probe.json').read_text())['laserProfiles']
records=[]
for row,clear_method in [(p,m) for p in profiles for m in ['none','all','rectangle','circle']]:
 kind=row['factory']['kind'];v=Uc(UC_ARCH_X86,UC_MODE_32)
 for a,n in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
  (0x1000000,0x10000),(0x2000000,0x1000000),(0x3000000,0x1000000),
  (0x4000000,0x1000000),(0x5000000,0x1000000)]:v.mem_map(a,n)
 v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f);v.reg_write(UC_X86_REG_MXCSR,0x1f80)
 LM,PLAYER,SHT,ENEMIES,PARAM,POS,SP,STOP=0x2000000,0x2010000,0x2020000,0x2030000,0x2040000,0x2050000,0x100e000,0x100f000
 def put(a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
 def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
 def ret(pop=0,value=0):
  sp=v.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4+pop);v.reg_write(UC_X86_REG_EAX,value);v.reg_write(UC_X86_REG_EIP,target)
 heap=0x3000000;intercepts=collections.Counter();events=[];frame=-1
 def hook(v,a,n,data):
  global heap
  sp=v.reg_read(UC_X86_REG_ESP)
  if a in [0x46c9ea,0x46d04a]:
   amount=get(sp+4,'I')[0];out=heap;heap+=(amount+15)&~15;intercepts[hex(a)]+=1;ret(value=out)
  elif a in [0x46ca4f,0x46c941]:intercepts[hex(a)]+=1;ret()
  elif a in [0x453e20,0x453d90,0x453f10]:intercepts[hex(a)]+=1;ret(4 if a==0x453e20 else 0)
  elif a in [0x454ee0,0x454d10,0x455630,0x437a80,0x437d00,0x4391c0]:
   events.append({'frame':frame,'entry':hex(a),'vm':get(sp+4,'I')[0] if a==0x455630 else None})
 v.hook_add(UC_HOOK_CODE,hook)
 def call(a,args=(),regs=None):
  put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
  for r,x in (regs or {}).items():v.reg_write(r,x)
  try:v.emu_start(a,STOP,count=3000000)
  except Exception as e:raise RuntimeError(f'kind{kind} {a:x} native failed @ {v.reg_read(UC_X86_REG_EIP):x}') from e
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native instruction budget reached')
  return v.reg_read(UC_X86_REG_EAX)
 put(0x4b2ed0,'f',1);put(0x4d52d4,'I',1);put(0x4d52dc,'I',1);put(0x4ce8cc,'I',0x4000000);put(0x4ce568,'II',0x1234,0)
 put(0x4b4514,'I',PLAYER);put(0x4b43dc,'I',ENEMIES);put(PLAYER+0xa28,'i',1);put(PLAYER+0xa2c,'I',SHT);put(SHT+4,'f',2)
 put(PLAYER+0x97c,'3f',5000,5000,0);put(PLAYER+0x9cc,'6f',4999,4999,0,5001,5001,0)
 resource=install_resource(v,(ROOT/'local/retail/bullet.anm').read_bytes(),0x5000000,6)
 call(0x427fa0,regs={UC_X86_REG_EDI:LM})
 # Registration fills this sentinel tail and resource. The scheduler registration
 # is outside this local direct-callback experiment; the constructor is native.
 put(LM+0x464,'I',LM+0x10);put(LM+0x488,'I',resource['resource']);put(0x4b43d4,'I',0x2060000);put(0x2060010,'I',resource['resource'])
 v.mem_write(PARAM,bytes.fromhex(row['factory']['profileHex']))
 result=call(0x428450,[kind],{UC_X86_REG_EDI:PARAM})
 laser=get(LM+0x18,'I')[0]
 if not laser:raise RuntimeError('Native factory did not append laser')
 def snap():
  nodes=[];p=get(LM+0x18,'I')[0]
  while p:
   node_kind={0x4a0654:0,0x4a0704:1,0x4a06ac:2}[get(p,'I')[0]]
   vm0={0:0x63c,1:0x660,2:0x634}[node_kind];vm1={0:0xaf0,1:0xb14,2:0xae8}[node_kind]
   nodes.append({'address':hex(p),'factoryKind':node_kind,'id':get(p+0x80,'I')[0],'phase':get(p+0xc,'i')[0],
    'retirePending':get(p+0x7c,'B')[0],'cancellable':get(p+0x7d,'B')[0],
    'age':get(p+0x18,'i')[0],'position':get(p+0x50,'3f'),'angleLengthWidthSpeed':get(p+0x68,'4f'),
    'headOffset':get(p+0x78,'f')[0],'vmClocks':[get(p+vm0+0x6c,'i')[0],get(p+vm1+0x6c,'i')[0]],
    'vmEndedFlags':[get(p+vm0+0x47c,'I')[0],get(p+vm1+0x47c,'I')[0]]})
   p=get(p+8,'I')[0]
   if len(nodes)>300:raise RuntimeError('Invalid laser linked list')
  return {'frame':frame,'count':get(LM+0x468,'i')[0],'rng':get(0x4ce568,'II'),'nodes':nodes}
 frames=[snap()]
 for frame in range(75):
  if frame==12 and clear_method!='none':
   put(POS,'3f',20,0,0);put(POS+16,'3f',30,20,0)
   if clear_method=='all':call(0x428750,regs={UC_X86_REG_EDI:0,UC_X86_REG_EBX:0})
   elif clear_method=='rectangle':call(0x4285f0,[0],{UC_X86_REG_EDI:POS,UC_X86_REG_ESI:POS+16})
   elif clear_method=='circle':call(0x4286f0,[struct.unpack('<I',struct.pack('<f',10))[0],0,0],{UC_X86_REG_ESI:POS})
   frames.append({'afterClear':clear_method,**snap()})
  call(0x428270,regs={UC_X86_REG_EDI:LM});frames.append(snap())
 records.append({'eclOpcode':row['opcode'],'factoryKind':kind,'clearMethod':clear_method,'factoryReturn':result,
  'profileHex':row['factory']['profileHex'],'frames':frames,'events':events,'intercepts':dict(intercepts)})
assert len(records)==12
for r in records:
 for f in r['frames']:
  assert f['count']==len(f['nodes'])
  for n in f['nodes']:assert all(math.isfinite(x) for x in n['position']+n['angleLengthWidthSpeed'])
out=ROOT/'artifacts/reverse/combat/laser-lifecycle.json'
out.write_text(json.dumps({'schema':'th12-native-laser-lifecycle/1','originalExeSha256':EXE_SHA256,
 'partial':True,'wholeGameOracle':False,'scope':'3 original factory variants ×4 cancellation modes ×75 manager ticks, embedded original bullet ANM bind/extra/update; synthetic far-away Player',
 'fixtures':['host malloc/free synthetic zero heap and audio output only','manager registered sentinel and bank set from parsed original ANM without scheduler','Player has valid SHT hit width and far-away hit rectangle; no actual Miss/graze expected'],
 'records':records},indent=2)+'\n')
print(json.dumps({'output':str(out),'results':[{'kind':r['factoryKind'],'clear':r['clearMethod'],
 'retiredFrame':next((f['frame'] for f in r['frames'] if not f['nodes']),None),
 'clearNodes':next((f['nodes'] for f in r['frames'] if 'afterClear' in f),None),
 'intercepts':r['intercepts']} for r in records]},indent=2))
