"""Actual ECL309/311 synchronous geometry and combine at float boundaries."""
import json,pathlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
HELPER=ROOT/'scripts/reverse/inspect-enemy-motion.py'
ns={'__file__':str(HELPER),'__name__':'native_ecl_circle_interpolation_sync'}
exec(compile(HELPER.read_text(encoding='utf-8').split('quant=[]')[0],str(HELPER),'exec'),ns)
records=[]
for relative in [False,True]:
 for old_x,angle,radius in [
  (2.5,1.5533431768417358,144.0),
  (-2.52,1.5533431768417358,144.0),
  (-4.53,3.1415927410125732,4.53000020980835),
  (0.0,.33,3.0),
  (100.0,0.0,0.0),
 ]:
  for enabled in [False,True]:
   v=ns['vm_new']();put,get=ns['put'],ns['get'];state=ns['STATE']
   put(v,state+0x34,'fff',old_x,128,0)
   offset=0x9c if relative else 0x68
   put(v,state+(0x68 if relative else 0x9c),'fff',0,128 if relative else 0,0)
   put(v,state+offset,'fff',0,0,0)
   put(v,state+offset+0xc,'fff',0,0 if relative else 128,0)
   put(v,state+offset+0x18,'ffff',.034906585,angle,radius,0)
   put(v,state+offset+0x30,'I',int(enabled))
   before={key:ns['movement'](v,offset) for key,offset in [('absolute',0x68),('relative',0x9c),('combined',0x34)]}
   ns['dispatch'](v,311 if relative else 309,[120,9,-.034906585,64.0,0.0])
   after={key:ns['movement'](v,offset) for key,offset in [('absolute',0x68),('relative',0x9c),('combined',0x34)]}
   records.append({'relative':relative,'before':before,'after':after})
out=ROOT/'tests/fixtures/ecl-circle-interpolation-sync-original.json'
out.write_text(json.dumps({'schema':'th12-original-ecl-circle-interpolation-sync/1',
 'originalExeSha256':ns['SHA'],'wholeGameOracle':False,
 'scope':'Actual309/311 parameter dispatch416e03..41962b,464eb0 geometry and413700/465320 combine. Controlled current Motion structures and literals, FPCW037f/MXCSR1f80. No Player, full Enemy tick, ANM, GPU or replay acceptance.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(records),
 'replayBoundary':[(r['relative'],r['before']['combined']['position'][0],r['after']['combined']['position'][0]) for r in records[:4]]}))
