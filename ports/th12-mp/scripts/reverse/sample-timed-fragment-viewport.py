"""Actual42b750 fragment birth viewport gate; controlled Timed laser inputs."""
import pathlib,json,sys,hashlib,math
ROOT=pathlib.Path(__file__).resolve().parents[2]
helpers=(ROOT/'scripts/reverse/sample-laser-phase-clear.py').read_text().split('records=[]')[0]
exec(compile(helpers,'laser_helpers','exec'))
rows=[]
for axis in ['x','y']:
 for target in ([-225,-224,-223.99998474121094,-209,-208,-207.99998474121094,207.99998474121094,208,224] if axis=='x' else [-33,-32,-31.999998092651367,-17,-16,-15.999999046325684,463.9999694824219,464,480,507]):
  sys.argv=[str(PROVIDER),'--frames','0','--seed','1234'];ns={'__file__':str(PROVIDER),'__name__':'timed_fragment_viewport'}
  exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
  v,put,get,call=(ns[k] for k in ['v','put','get','call']);lm=call(0x4281c0)
  origin=(target-48,128) if axis=='x' else (0,target-48);center=(origin[0]+32,origin[1]) if axis=='x' else (origin[0],origin[1]+32)
  angle=0 if axis=='x' else 1.5707963705062866
  code=[ins(300,[floating(origin[0]),floating(origin[1])]),ins(500,[word(0)]),ins(502,[word(0),word(7),word(2)]),ins(505,[word(0),floating(0),floating(0)]),ins(504,[word(0),floating(angle),floating(0)]),ins(600,[word(0),floating(128),floating(128),floating(16),floating(16)]),ins(601,[word(0),word(8),word(16),word(60),word(15),word(0)]),ins(603,[word(0),word(1)]),ins(83,[word(100000)])]
  raw,header=program(code);base=0x4300000;v.mem_write(base,raw);ns['routines']['ClearProbe']=(base+40,base+header)
  put(ns['PROGRAM']+8,'I',len(ns['routines']))
  for i,(_, (text,h)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+i*8,'II',text,h)
  request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0);call(0x412990,base+40,request)
  center_at=0x2070100;put(center_at,'fff',*center,0)
  from unicorn.x86_const import UC_X86_REG_ESI
  v.reg_write(UC_X86_REG_ESI,center_at);returned=call(0x4286f0,int.from_bytes(floating(20),'little'),0,1)
  nodes=[];node=get(lm+0x18,'I')[0]
  while node:
   nodes.append({'kind':{0x4a0654:0,0x4a0704:1}[get(node,'I')[0]],'position':get(node+0x50,'fff'),'geometry':get(node+0x68,'4f')});node=get(node+8,'I')[0]
  rows.append({'axis':axis,'target':target,'origin':origin,'center':center,'angle':angle,'returned':returned,'nodes':nodes})
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Actual ECL603 factory and4286f0/42b750 virtual circle clear/fragment constructor. Controlled parameter/location inputs and original ANM. Existing resource/heap/audio/item-request host boundaries; no replay/GPU.','rows':rows}
(ROOT/'tests/fixtures/timed-fragment-viewport-original.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps([{'axis':r['axis'],'target':r['target'],'count':len(r['nodes'])} for r in rows]))
