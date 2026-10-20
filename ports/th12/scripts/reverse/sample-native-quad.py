"""Actual ANM mode0/1 quad corners and screen-offset/rounding order.

Only controlled VM pose/scale/anchors and camera offsets are inputs. Original
45c900, 45a570/45af10, 459e50 and45a4a0 execute. No GPU or whole-game claim.
"""
import json, pathlib, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts/reverse'))
source=ROOT/'scripts/reverse/sample-custom-geometry.py'
scope={'__file__':str(source),'__name__':'native_quad_fixture'}
exec(source.read_text(encoding='utf-8').split('\nrecords=[]',1)[0],scope)
Native=scope['Native'];EAX=scope['UC_X86_REG_EAX'];ECX=scope['UC_X86_REG_ECX']
rows=[]
cases=[(100.5,200.5,16,14,.3,1),(244.25,160.75,30,30,.625,1.1),
       (-12.5,-7.5,16,14,1,1),(100.75,201.125,16,14,.3,.3)]
for mode in [0,1]:
 for anchor in [0,1,2]:
  for case in cases:
   for offset in [(0,0),(.4,0),(3.25,-2.5)]:
    for rotation in ([0] if mode==0 else [0,.35]):
     n=Native();vm=0x2100000
     n.call(0x454d10,[vm,42],{ECX:n.resource['resource']})
     n.put(vm+0x47c,'I',(mode<<23)|(anchor<<19)|(anchor<<21)|3|8)
     n.put(vm+0x3bc,'I',0xffffffff)
     n.put(vm+0x424,'3f',0,0,0);n.put(vm+0x43c,'3f',0,0,0)
     n.put(vm+0x430,'3f',case[0],case[1],0)
     n.put(vm+0x58,'2f',*case[2:4]);n.put(vm+0x40,'2f',*case[4:])
     n.put(vm+0x24,'3f',0,0,rotation)
     n.put(0x4000000+0xb0,'2f',*offset)
     rng_before=n.get(0x4ce568,'2I')
     n.draws=[];n.call(0x45c900,[0x4000000],{EAX:vm})
     state=n.state(vm)
     rows.append({'mode':mode,'anchor':anchor,'position':state['engine'][:2],
                  'dimensions':state['dimensions'],'scale':state['scale'],
                  'rotation':state['rotation'][2],'offset':n.get(0x4000000+0xb0,'2f'),
                  'vertices':n.draws[0]['vertices'] if n.draws else [],
                  'rngBefore':rng_before,'rngAfter':n.get(0x4ce568,'2I')})
alpha_rows=[]
for mode in [0,1]:
 n=Native();vm=0x2100000
 n.call(0x454d10,[vm,42],{ECX:n.resource['resource']})
 n.put(vm+0x47c,'I',(mode<<23)|0x10000|3|8)
 n.put(vm+0x430,'3f',244,160,0);n.put(vm+0x424,'3f',0,0,0);n.put(vm+0x43c,'3f',0,0,0)
 n.put(vm+0x58,'2f',16,14);n.put(vm+0x40,'2f',1,1);n.put(vm+0x24,'3f',0,0,0)
 n.put(vm+0x3c0,'I',0xffffffff)
 for primary in [0x00ffffff,0x01ffffff]:
  n.put(vm+0x3bc,'I',primary);n.draws=[];n.call(0x45c900,[0x4000000],{EAX:vm})
  alpha_rows.append({'mode':mode,'primary':primary,'secondary':0xffffffff,
                     'flags':n.get(vm+0x47c,'I')[0],'drawCount':len(n.draws)})
out=ROOT/'tests/fixtures/native-quad-original.json'
out.write_text(json.dumps({'schema':'th12-controlled-native-quad/1',
    'originalExeSha256':scope['EXE_SHA256'],'wholeGameOracle':False,'gpuExecuted':False,
    'scope':'Actual45c900→45a570/45af10→459e50→45a4a0 with controlled VM pose/scale/anchors and ANM screen offset; GPU/resource host boundaries as sample-custom-geometry.py; mode0 output includes original D3D half-pixel adjustment.',
    'rows':rows,'alphaRows':alpha_rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'cases':len(rows),'output':str(out)}))
