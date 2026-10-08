"""Actual three laser classes × raw circle flags and radius edge cases.

Uses the passive provider host bootstrap only, then original factories,
eight Laser20 visits and4286f0. Item requests are captured4273f0; no shader,
replay acceptance, copied candidate geometry or callback replacement.
"""
import json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
helpers=(ROOT/'scripts/reverse/sample-laser-phase-clear.py').read_text(encoding='utf-8').split('records=[]')[0]
exec(compile(helpers,str(ROOT/'scripts/reverse/sample-laser-phase-clear.py'),'exec'))
from unicorn.x86_const import UC_X86_REG_EDI,UC_X86_REG_ESI
records=[]
for kind in range(3):
 for radius in [0,8,20,24]:
  for flags in range(4):
   saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','4660']
   ns={'__file__':str(PROVIDER),'__name__':'native_laser_circle_flags'}
   try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
   finally:sys.argv=saved
   v,put,get,call=(ns[k] for k in ['v','put','get','call'])
   lm=call(0x4281c0)
   code=[ins(300,[floating(94.4),floating(144)]),ins(500,[word(0)]),ins(502,[word(0),word(7),word(6)]),
    ins(505,[word(0),floating(0),floating(0)]),ins(504,[word(0),floating(0),floating(0)]),
    ins(600,[word(0),floating(96),floating(96),floating(1000),floating(16)]),
    ins(601,[word(0),word(8),word(8),word(140),word(10),word(9)]),ins(508,[word(0),word(-1),word(-1)]),
    ins([602,603,611][kind],[word(0),word(1)] if kind==1 else [word(0)]),ins(83,[word(100000)])]
   raw,header=program(code);base=0x4300000;v.mem_write(base,raw);ns['routines']['ClearProbe']=(base+40,base+header)
   put(ns['PROGRAM']+8,'I',len(ns['routines']))
   for index,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+index*8,'II',text,routine)
   request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0);call(0x412990,base+40,request)
   for tick in range(8):v.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
   start=len(ns['events']);center=0x2070100;put(center,'fff',94.4,144,0)
   v.reg_write(UC_X86_REG_ESI,center)
   returned=call(0x4286f0,int.from_bytes(floating(radius),'little'),flags,1)
   items=[e for e in ns['events'][start:] if e.get('kind')=='itemSpawn']
   nodes=[];node=get(lm+0x18,'I')[0]
   while node:
    nodes.append({'kind':{0x4a0654:0,0x4a0704:1,0x4a06ac:2}[get(node,'I')[0]],'phase':get(node+0xc,'i')[0],
     'position':list(get(node+0x50,'3f')),'geometry':list(get(node+0x68,'4f')),'retirePending':get(node+0x7c,'B')[0]})
    node=get(node+8,'I')[0]
   records.append({'kind':kind,'radius':radius,'flags':flags,'syntheticEclHex':raw.hex(),
    'itemSpawns':items,'returned':returned,'nodes':nodes})
output=ROOT/'tests/fixtures/laser-circle-flags-original.json'
output.write_text(json.dumps({'schema':'th12-original-laser-circle-flags/1','originalExeSha256':ns['SHA'],
 'wholeGameOracle':False,'gpuExecuted':False,'scope':'Actual Enemy/ECL factories,8 original Laser20 updates,4286f0 and Moving42a1f0/Timed42b750/Curve42df00 virtual circle methods. Constructed passive host; original bullet ANM executes, item requests recorded4273f0 without Item allocation. No GPU or whole-replay acceptance.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'cases':len(records),'counts':[(r['kind'],r['radius'],r['flags'],len(r['itemSpawns']),r['returned']) for r in records]}))
