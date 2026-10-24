"""Original ECL503/524/525 and602/603/611 parameter construction cases."""
import json,math,pathlib,runpy
ROOT=pathlib.Path(__file__).resolve().parents[2]
# Reuse the explicit synthetic VM/dispatcher and presentation fixtures. This
# runs its invariant checks first; no original/core/replay/golden is written.
probe=runpy.run_path(str(ROOT/'scripts/reverse/combat-probe.py'))
reset,ecl,put,bits=map(probe.__getitem__,['reset','ecl','put','bits'])
ES=probe['ES'];records=[]
for opcode in [602,603,611]:
 for originX in [-999.,-990.,60.]:
  for angle in [0.,math.pi/2,math.pi*3,-math.pi*4]:
   for radius in [0.,24.]:
    reset();ecl(500,[0]);put(ES+0x34,'3f',12,80,0)
    ecl(502,[0,17,4]);ecl(503,[0,bits(4),bits(-7)]);ecl(525,[0,bits(originX),bits(140)]);ecl(524,[0,bits(radius)])
    ecl(600,[0,bits(25),bits(100),bits(10),bits(6)]);ecl(601,[0,2,3,20,5,7])
    # Angle/speed are already-evaluated shooter fields, intentionally supplied
    # here instead of exercising unrelated rank/aim shooter configuration.
    put(ES+0x49c,'f',angle);put(ES+0x4a4,'f',3)
    lookupId=0 if angle==0 else 123
    result=ecl(opcode,[0,lookupId] if opcode==603 else [0])
    records.append({'opcode':opcode,'factoryKind':{602:0,603:1,611:2}[opcode],
      'input':{'enemy':[12,80,0],'offset':[4,-7,0],'origin':[originX,140,0],
       'fixedOrigin':originX>=-990,'angle':angle,'speed':3,'initialOffset':radius,'lookupId':lookupId,
       'type':17,'color':4,'sounds':[22,40],'laser600':[25,100,10,6],'laser601':[2,3,20,5,7]},
      'factory':probe['factory'][-1],'trace':result['trace']})
out=ROOT/'tests/native/laser-factory-original.json'
out.write_text(json.dumps({'schema':'th12-native-laser-factory-parameters/1',
 'originalExeSha256':probe['SHA'],'wholeGameOracle':False,
 'fixtures':['Same synthetic native ECL VM as combat-probe.py;428450 records parameter buffer without allocating','angle/speed supplied as evaluated shooter fields;other parameter setters native;no ANM/RNG claim'],
 'records':records},indent=2)+'\n')
print(json.dumps({'output':str(out),'cases':len(records)}))
