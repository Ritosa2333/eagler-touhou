"""Original MovingLaser partial head cancellation and next Laser20 update.

Factories, circle vtable clearing, embedded ANM and game RNG execute normally.
Only the initial head-offset is a controlled property input. Item requests are
observed at the existing provider boundary; this is not a whole replay oracle.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
helpers=(ROOT/'scripts/reverse/sample-laser-phase-clear.py').read_text(encoding='utf-8').split('records=[]')[0]
exec(compile(helpers,str(ROOT/'scripts/reverse/sample-laser-phase-clear.py'),'exec'))
from unicorn.x86_const import UC_X86_REG_EDI,UC_X86_REG_ESI
records=[]
for initial_offset in [0,.01,1,17,50]:
 for radius in [8,24,40]:
  saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','4660']
  ns={'__file__':str(PROVIDER),'__name__':'native_moving_laser_prefix_clear'}
  try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
  finally:sys.argv=saved
  v,put,get,call=(ns[k] for k in ['v','put','get','call'])
  lm=call(0x4281c0)
  code=[ins(300,[floating(94.4),floating(144)]),ins(500,[word(0)]),
    ins(502,[word(0),word(7),word(6)]),ins(505,[word(0),floating(1),floating(1)]),
    ins(504,[word(0),floating(0),floating(0)]),
    ins(600,[word(0),floating(96),floating(96),floating(1000),floating(16)]),
    ins(508,[word(0),word(-1),word(-1)]),ins(602,[word(0)]),ins(83,[word(100000)])]
  raw,header=program(code);base=0x4300000;v.mem_write(base,raw)
  ns['routines']['ClearProbe']=(base+40,base+header);put(ns['PROGRAM']+8,'I',len(ns['routines']))
  for index,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):
   put(ns['PROGRAM']+0x100+index*8,'II',text,routine)
  request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
  call(0x412990,base+40,request);node=get(lm+0x18,'I')[0];put(node+0x78,'f',initial_offset)
  def capture():
   return {'phase':get(node+0xc,'i')[0],'position':list(get(node+0x50,'3f')),
    'geometry':list(get(node+0x68,'5f')),'maxLength':get(node+0x464,'f')[0],
    'retirePending':get(node+0x7c,'B')[0]}
  center=0x2070100;put(center,'fff',94.4,144,0);v.reg_write(UC_X86_REG_ESI,center)
  count=call(0x4286f0,int.from_bytes(floating(radius),'little'),1,1)
  after=capture();v.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
  records.append({'initialOffset':initial_offset,'radius':radius,'removed':count,
    'afterClear':after,'afterTick':capture()})
out=ROOT/'tests/fixtures/moving-laser-prefix-clear-original.json'
out.write_text(json.dumps({'schema':'th12-original-moving-laser-prefix-clear/1',
 'originalExeSha256':ns['SHA'],'wholeGameOracle':False,'gpuExecuted':False,
 'scope':'Actual Enemy/ECL602 factory,4286f0/42a1f0 Moving virtual partial circle clear and next428270/429610 update with real bullet ANM. Controlled initial+78 offset, captured item request boundary; no input/golden changes.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(records),
 'headOffsets':[(r['initialOffset'],r['radius'],r['afterClear']['geometry'][-1],r['afterTick']['geometry'][-1]) for r in records]}))
