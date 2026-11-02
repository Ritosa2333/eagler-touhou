"""Actual Bullet21 appearance reset, ANM transition and viewport retirement."""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
HELPER=ROOT/'scripts/reverse/sample-laser-phase-clear.py'
exec(compile(HELPER.read_text(encoding='utf-8').split('records=[]')[0],str(HELPER),'exec'))
records=[]
for name,teleport in [('ordinary-morph',False),('offscreen-new-spawn',True)]:
 saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','4660','--native-bullets']
 ns={'__file__':str(PROVIDER),'__name__':'native_bullet_morph_animation'}
 try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
 finally:sys.argv=saved
 v,put,get,call=(ns[n] for n in ['v','put','get','call'])
 from unicorn.x86_const import UC_X86_REG_ESI
 code=[ins(300,[floating(20),floating(90)]),ins(500,[word(0)]),
  ins(502,[word(0),word(5),word(4)]),ins(505,[word(0),floating(0),floating(0)]),
  ins(507,[word(0),word(1)]),ins(508,[word(0),word(-1),word(-1)])]
 for i,kind,c,d in [(0,0x1000,10,0),(1,0x800,7,8),(2,0x1000,20,0),
                    (3,0x1000000,7,10),(4,0x1000,30,0)]:
  code.append(ins(509,[word(0),word(i),word(0),word(kind),word(c),word(d),floating(-999999),floating(-999999)]))
 code.extend([ins(501,[word(0)]),ins(83,[word(100000)])])
 raw,header=program(code);base=0x4300000;v.mem_write(base,raw)
 ns['routines']['ClearProbe']=(base+40,base+header);put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):
  put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
 req=0x2070000;put(req,'fffiiiII',0,0,0,0,0,10000,0,0);call(0x412990,base+40,req)
 pointer=next(iter(ns['bullet_ids']))
 def capture(tick):
  sprite=get(pointer+0x3fc,'I')[0]
  return {'inputTick':tick,'phase':get(pointer+0x532,'H')[0],
   'typeColor':list(get(pointer+0x9f4,'HH')),'age':get(pointer+0x4e8,'i')[0],
   'animationTime':get(pointer+8+0x6c,'i')[0],
   'script':get(pointer+8+0x3ea,'h')[0],'sprite':get(pointer+8+0x3e4,'h')[0],
   'integers':list(get(pointer+8+0x3fc,'6i')),
   'dimensions':list(get(sprite+0x38,'f')+get(sprite+0x34,'f')) if sprite else None,
   'rng':list(get(0x4ce568,'II'))}
 frames=[capture(-1)]
 for tick in range(36):
  if teleport and tick==10:put(pointer+0x4bc,'fff',20,-71,0)
  v.reg_write(UC_X86_REG_ESI,ns['BULLETS']);call(0x40a020)
  frames.append(capture(tick))
 records.append({'name':name,'teleportAt10':teleport,'syntheticEclHex':raw.hex(),'frames':frames})
out=ROOT/'tests/fixtures/bullet-morph-animation-original.json'
out.write_text(json.dumps({'schema':'th12-original-bullet-morph-animation/1',
 'originalExeSha256':ns['SHA'],'wholeGameOracle':False,'gpuExecuted':False,
 'scope':'Actual ECL emitter,40aa60 appearance800/1000000 including402520 reset before454ee0,40a020/4099e0 and real bullet ANM/RNG. Far/passive Player collision return boundary, no Player/Enemy/registered Primary scheduler ticks. Offscreen case sets the declared physical position to(20,-71) before input10 on both sides.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':[(r['name'],[(f['inputTick'],f['typeColor'],f['phase'],f['animationTime']) for f in r['frames'] if f['inputTick'] in [9,10,16,17,18,30]]) for r in records]}))
