"""Execute retail 41b740/41bb40/41bd20 against native laser objects.

The original callbacks, laser construction/clear and bullet ANM execute unchanged.
40a250 is the explicit bullet-allocation boundary: capture the exact Shooter at
40b5e0, but do not allocate a second subsystem. No whole-game oracle claim.
"""
import collections,json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
profiles=json.loads((ROOT/'artifacts/reverse/combat/native-probe.json').read_text())['laserProfiles']
def laser(kind=0,position=(12.,120.,0.),initial=(-20.,95.,0.),angle=.3,length=42.,speed=2.,age=9,phase=2):
 return dict(kind=kind,position=list(position),initial=list(initial),angle=angle,length=length,speed=speed,age=age,phase=phase)
def bullet(vm=True,phase=1,marker=1,position=(-10.,120.,0.),angle=.7,speed=2.,initial=(-35.,88.,0.)):
 return dict(vmAlive=vm,phase=phase,marker=marker,position=list(position),sourceAngle=angle,sourceSpeed=speed,initialPosition=list(initial))
cases=[]
for selector in [3,4,5]:
 cases.append(dict(name=f'{selector}-empty',selector=selector,lasers=[],bullets=[],integers=[9,0,0,0],floats=[16.,2.,0.,0.]))
 if selector==4:
  for name,inputs in [('one',[bullet()]),('many',[bullet(position=(-20,90,0),speed=0),bullet(angle=-.7,speed=5),bullet(position=(15,180,0),angle=4.,speed=-1)]),
   ('filter',[bullet(vm=False),bullet(phase=0),bullet(phase=2),bullet(phase=3),bullet(marker=0),bullet(marker=2),bullet(marker=-1),bullet()]),
   ('end-slots',[dict(slot=0,**bullet()),dict(slot=1999,**bullet(position=(25,190,0),angle=-2.))])]:
   cases.append(dict(name=f'4-{name}',selector=4,lasers=[],bullets=inputs,integers=[0]*4,floats=[0.]*4))
 else:
  for kind in [0,1,2]:
   cases.append(dict(name=f'{selector}-one-kind{kind}',selector=selector,lasers=[laser(kind)],bullets=[],integers=[9,0,0,0],floats=[16.,2.,0.,0.]))
  cases.append(dict(name=f'{selector}-many',selector=selector,lasers=[laser(0,length=29,age=8),laser(1,angle=-1.,length=33.,age=9,phase=3),laser(2,angle=1.2,length=50.,age=10)],bullets=[],integers=[9,0,0,0],floats=[16.,2.,0.,0.]))
  for length in [0.,14.,14.001,16.,16.001,28.,28.001]:
   cases.append(dict(name=f'{selector}-length{length}',selector=selector,lasers=[laser(length=length,angle=0)],bullets=[],integers=[0]*4,floats=[16.,2.,0.,0.]))
  cases.append(dict(name=f'{selector}-skip-and-negative',selector=selector,lasers=[laser(length=28,age=-1),laser(length=-1,age=9),laser(length=0,age=10)],bullets=[],integers=[9,0,0,0],floats=[24.,-.5,0.,0.]))

records=[]
for case in cases:
 v=Uc(UC_ARCH_X86,UC_MODE_32)
 for a,n in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
  (0x1000000,0x10000),(0x2000000,0x1000000),(0x3000000,0x1000000),
  (0x4000000,0x1000000),(0x5000000,0x1000000),(0x7000000,0x600000)]:v.mem_map(a,n)
 v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f);v.reg_write(UC_X86_REG_MXCSR,0x1f80)
 LM,PLAYER,SHT,ENEMIES,PARAM,ENEMY,SP,STOP,BM=0x2000000,0x2010000,0x2020000,0x2030000,0x2040000,0x2050000,0x100e000,0x100f000,0x7000000
 def put(a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
 def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
 def ret(pop=0,value=0):
  sp=v.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4+pop);v.reg_write(UC_X86_REG_EAX,value);v.reg_write(UC_X86_REG_EIP,target)
 heap=0x3000000;events=[];emissions=[];intercepts=collections.Counter();capture=False;clear_entries={};factory_entries={};rng_values=[]
 def decode_shooter(p):
  return {'position':get(p+4,'3f'),'angle':get(p+16,'f')[0],'spacing':get(p+20,'f')[0],'speed':get(p+24,'f')[0],'slow':get(p+28,'f')[0],
   'type':get(p,'H')[0],'color':get(p+2,'H')[0],'count':get(p+0x1f8,'h')[0],'layers':get(p+0x1fa,'h')[0],'mode':get(p+0x1fc,'h')[0],
   'flags':get(p+0x200,'I')[0],'fireSound':get(p+0x204,'i')[0],'transformSound':get(p+0x208,'i')[0],'first':get(p+0x20c,'i')[0],
   'records':[dict(zip(['a','b','c','d','kind','concurrent'],get(p+0x24+i*24,'ffiiII'))) for i in range(18)]}
 def hook(v,a,n,data):
  nonlocal_dummy=None
  global heap
  sp=v.reg_read(UC_X86_REG_ESP)
  if a in [0x46c9ea,0x46d04a]:
   amount=get(sp+4,'I')[0];out=heap;heap+=(amount+15)&~15;intercepts[hex(a)]+=1;ret(value=out)
  elif a in [0x46ca4f,0x46c941]:intercepts[hex(a)]+=1;ret()
  elif a in [0x453e20,0x453d90,0x453f10]:
   if capture and a==0x453e20:events.append({'kind':'sound','id':v.reg_read(UC_X86_REG_EAX),'x':get(sp+4,'f')[0]})
   intercepts[hex(a)]+=1;ret(4 if a==0x453e20 else 0)
  elif a==0x40a250:intercepts[hex(a)]+=1;ret(20,0)
  elif capture:
   if a==0x40b5e0:
    emissions.append(decode_shooter(v.reg_read(UC_X86_REG_ESI)));events.append({'kind':'emit','index':len(emissions)-1,'rng':get(0x4ce568,'II')})
   elif a==0x464440:events.append({'kind':'rng32','rng':get(0x4ce568,'II')})
   elif a==0x454d10:events.append({'kind':'bind','script':get(sp+8,'i')[0]})
   elif a in clear_entries:events.append({'kind':'clear','serial':get(v.reg_read(UC_X86_REG_ECX)+0x80,'I')[0],'convert':get(sp+4,'I')[0],'force':get(sp+8,'I')[0]})
 v.hook_add(UC_HOOK_CODE,hook)
 def call(a,args=(),regs=None):
  put(SP,'I'*(len(args)+1),STOP,*args);v.reg_write(UC_X86_REG_ESP,SP)
  for r,x in (regs or {}).items():v.reg_write(r,x)
  try:v.emu_start(a,STOP,count=3000000)
  except Exception as e:raise RuntimeError(f"{case['name']} native @ {v.reg_read(UC_X86_REG_EIP):x}") from e
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native instruction budget reached')
  return v.reg_read(UC_X86_REG_EAX)
 put(0x4b2ed0,'f',1);put(0x4d52d4,'I',1);put(0x4d52dc,'I',1);put(0x4ce8cc,'I',0x4000000);put(0x4ce568,'II',0x1234,0)
 put(0x4b4514,'I',PLAYER);put(0x4b43dc,'I',ENEMIES);put(0x4b43c8,'I',BM);put(PLAYER+0xa28,'i',1);put(PLAYER+0xa2c,'I',SHT);put(SHT+4,'f',2)
 put(PLAYER+0x97c,'3f',5000,5000,0);put(PLAYER+0x9cc,'6f',4999,4999,0,5001,5001,0)
 resource=install_resource(v,(ROOT/'local/retail/bullet.anm').read_bytes(),0x5000000,6)
 put(BM+0x4debdc,'I',resource['resource'])
 call(0x427fa0,regs={UC_X86_REG_EDI:LM});put(LM+0x464,'I',LM+0x10);put(LM+0x488,'I',resource['resource']);put(0x4b43d4,'I',0x2060000);put(0x2060010,'I',resource['resource'])
 for row in case['lasers']:
  profile=next(p for p in profiles if p['factory']['kind']==row['kind']);row['profileHex']=profile['factory']['profileHex'];v.mem_write(PARAM,bytes.fromhex(row['profileHex']))
  call(0x428450,[row['kind']],{UC_X86_REG_EDI:PARAM});p=get(LM+0x464,'I')[0]
  put(p+0x50,'3f',*row['position']);put(p+0x454,'3f',*row['initial']);put(p+0x68,'2f',row['angle'],row['length']);put(p+0x74,'f',row['speed']);put(p+0x40,'i',row['age']);put(p+0xc,'i',row['phase'])
  clear_entries[get(get(p,'I')[0]+0x14,'I')[0]]=row['kind']
 for i,row in enumerate(case['bullets']):
  p=BM+0x64+row.get('slot',i)*0x9f8
  put(p,'I',int(row['vmAlive']));put(p+0x532,'H',row['phase']);put(p+0x4bc,'3f',*row['position']);put(p+0x628,'ffi',row['sourceAngle'],row['sourceSpeed'],row['marker']);put(p+0x640,'2f',*row['initialPosition'][:2])
 put(ENEMY+0x23c,'4i',*case['integers']);put(ENEMY+0x24c,'4f',*case['floats'])
 rng_before=get(0x4ce568,'II');capture=True;native_return=call({3:0x41b740,4:0x41bb40,5:0x41bd20}[case['selector']],regs={UC_X86_REG_ECX:ENEMY});capture=False
 states=[];p=get(LM+0x18,'I')[0]
 while p:
  states.append(dict(kind={0x4a0654:0,0x4a0704:1,0x4a06ac:2}[get(p,'I')[0]],serial=get(p+0x80,'I')[0],phase=get(p+0xc,'i')[0],position=get(p+0x50,'3f'),initialPosition=get(p+0x454,'3f'),angleLengthWidthSpeed=get(p+0x68,'4f'),protection=get(p+0x44c,'i')[0],cursor=get(p+0x438,'i')[0]))
  p=get(p+8,'I')[0]
 records.append({'input':case,'nativeReturn':native_return,'rngBefore':rng_before,'rngAfter':get(0x4ce568,'II'),'emissions':emissions,'events':events,'lasers':states,
  'shooter':decode_shooter(ENEMY+(0x48c if case['selector']==3 else 0x1318)) if case['selector']!=4 else None,'intercepts':dict(intercepts)})
 assert native_return==0
 assert all(math.isfinite(e[k]) for e in emissions for k in ['angle','speed','spacing','slow'])
out=ROOT/'tests/native/laser-enemy-callback-original.json'
out.write_text(json.dumps({'schema':'th12-laser-enemy-callback-native/1','originalExeSha256':EXE_SHA256,'partial':True,'wholeGameOracle':False,
 'scope':'original EnemyState callbacks3/4/5, synthetic zero/one/many native laser instances and BulletManager slots; original laser factory/clear/bulletANM',
 'fixtures':['host malloc/free and audio output','native laser manager constructor plus registered sentinel/resource without scheduler','original bullet ANM decoded into native resource tables','40a250 bullet allocation return0: actual 40b5e0 aim/count/layer/sound remains original; emitted Shooter buffers captured','far-away synthetic player has valid SHT geometry'],
 'records':records},indent=2)+'\n')
print(json.dumps({'output':str(out),'records':len(records),'emissions':sum(len(r['emissions']) for r in records),'spawns':sum(len(r['lasers']) for r in records if r['input']['selector']==4),'rng32':sum(sum(e['kind']=='rng32' for e in r['events']) for r in records)},indent=2))
