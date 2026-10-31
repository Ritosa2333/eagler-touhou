"""Capture retail ANM mode9 vertices, including the actual spell ring scripts.

The retail VM and CPU postprocessor execute. Only GPU submission, allocation,
and device state are supplied by the independent custom-geometry fixture.
"""
import hashlib,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts/reverse'))
source=ROOT/'scripts/reverse/sample-custom-geometry.py'
scope={'__file__':str(source),'__name__':'native_spell_ring_fixture'}
exec(source.read_text(encoding='utf8').split('\nrecords=[]',1)[0],scope)
Native=scope['Native'];EXE_SHA256=scope['EXE_SHA256'];RAW=scope['RAW']
ECX=scope['UC_X86_REG_ECX'];EAX=scope['UC_X86_REG_EAX']
records=[]
for script in [125,126]:
 for age in [1,8,34,60,80,100]:
  n=Native();vm=0x2100000
  n.call(0x454d10,[vm,script],{ECX:n.resource['resource']})
  n.put(vm+0x430,'3f',224,160,2)
  n.put(vm+0x424,'3f',3,4,1);n.put(vm+0x43c,'3f',2,1,3)
  for _ in range(age):n.call(0x455630,[vm])
  n.label=f'spell-ring{script}-age{age}'
  n.call(0x45c900,[0x4000000],{EAX:vm})
  s=n.state(vm)
  records.append({'name':n.label,'script':script,'age':age,'state':s,
   'origin':[s['position'][i]+s['engine'][i]+s['offset'][i] for i in range(3)],
   'vertices':n.draws[0]['vertices'] if n.draws else []})
# Separate poses exercise count, negative radius, rotation, repeating UV,
# immediate parent translation, and secondary diffuse in the same retail path.
for name,count,repeat,radius,thickness,rotation,parent,secondary in [
 ('three',3,1,64,8,0,False,False),('five-repeat',5,3,32,12,3.1,False,False),
 ('negative-radius',8,-2,-16,4,-3.1,False,False),
 ('parent-secondary',32,2,100,20,.7,True,True)]:
 n=Native();vm=0x2100000;p=0x2110000
 n.call(0x454d10,[vm,125],{ECX:n.resource['resource']})
 # ANM79 allocates the retail vertex buffer for the initial32 pairs; keep all
 # synthetic counts within that allocation and set the VM's count explicitly.
 n.put(vm+0x3fc,'2i',count,repeat);n.put(vm+0x40,'2f',thickness,radius)
 n.put(vm+0x2c,'f',rotation);n.put(vm+0x60,'2f',.25,.125)
 n.put(vm+0x430,'3f',224,160,2);n.put(vm+0x424,'3f',3,4,1);n.put(vm+0x43c,'3f',2,1,3)
 if parent:
  n.put(vm+0x3c,'I',p);n.put(p+0x424,'3f',11,12,1);n.put(p+0x430,'3f',31,21,2);n.put(p+0x43c,'3f',2,3,4)
 if secondary:
  n.put(vm+0x47c,'I',n.get(vm+0x47c,'I')[0]|0x10000);n.put(vm+0x3c0,'I',0x80ab37ff)
 # Execute the normal VM update after clearing scale/rotation interpolation.
 # Script velocity still advances rotation; the sampled state records that
 # resulting angle, rather than assuming the synthetic input pose survives.
 for offset in [0x1e0,0x1a4]:n.put(vm+offset,'i',0)
 n.call(0x455630,[vm])
 n.label=name;n.call(0x45c900,[0x4000000],{EAX:vm});s=n.state(vm)
 origin=[s['position'][i]+s['engine'][i]+s['offset'][i] for i in range(3)]
 if parent:origin=[origin[i]+n.get(p+0x424,'3f')[i]+n.get(p+0x430,'3f')[i]+n.get(p+0x43c,'3f')[i] for i in range(3)]
 records.append({'name':name,'script':125,'age':1,'state':s,'origin':origin,
  'vertices':n.draws[0]['vertices'] if n.draws else []})
out=ROOT/'tests/fixtures/spell-ring-geometry-native.json'
out.write_text(json.dumps({'schema':'th12-native-spell-ring/1','originalExeSha256':EXE_SHA256,
 'resourceSha256':hashlib.sha256(RAW).hexdigest(),'wholeGameOracle':False,'gpuExecuted':False,
 'provider':'Original454d10/455630 mode9 CPU vertices and45c900/45c3a0 submission.',
 'fixtures':['GPU-free retail bullet.anm sprite records normalized by460610',
 'independent mapped VM, synthetic engine/offset/parent positions and secondary color',
 'host allocation and GPU/device-state boundaries as sample-custom-geometry.py'],
 'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'cases':len(records),'vertices':sum(len(r['vertices']) for r in records),
 'tau64':Native().get(0x4a3cd0,'d')[0]}))
