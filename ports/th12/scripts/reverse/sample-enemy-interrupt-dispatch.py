"""Actual413840/41a6f0 distinction between first and second interrupt checks."""
import pathlib,json,sys,hashlib,base64
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
exec(compile((ROOT/'scripts/reverse/sample-last-enemy-id.py').read_text().split('birth_cases =')[0],'scpt_helpers','exec'))
rows=[]
for damage,pending in [(0,False),(10,False),(10,True),(80,True)]:
 sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--damage-fixture',str(damage),'--input',str(ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json')]
 ns={'__file__':str(PROVIDER),'__name__':'interrupt_dispatch_original'}
 exec(compile(PROVIDER.read_text().split('SPAWN=0x2070000;')[0],str(PROVIDER),'exec'),ns)
 v,put,get,call=(ns[k] for k in ['v','put','get','call'])
 # The damage result is a declared fixture boundary, not replay evidence.
 put(ns['bonus_manager']+0x7c,'I',35 if pending else 0);put(ns['bonus_manager']+0x28,'i',100)
 raw,entries=scpt([('main',[ins(402,[i(0xa0)]),ins(256,[string('Probe'),f(0),f(128),i(100),i(0),i(0)]),ins(83,[i(100000)])]),
  ('Probe',[ins(258,[i(2)]),ins(262,[i(0),i(0)]),ins(402,[i(0x82)]),ins(411,[i(100)]),ins(414,[i(0),i(0),i(3),string('After')]),ins(83,[i(100000)])]),
  ('After',[ins(411,[i(100)]),ins(42,[i(-10000)],mask=1),ins(43,[i(-9985)],mask=1),ins(83,[i(100000)])])])
 base=0x4300000;v.mem_write(base,raw)
 for name,(text,header) in entries.items():ns['routines'][name]=(base+text,base+header)
 put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for index,(_, (text,header)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+index*8,'II',text,header)
 put(0x4ce568,'II',1234,0);request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
 root=call(0x412990,base+entries['main'][0],request)
 node=get(ns['MANAGER']+0x68,'I')[0]
 while node:
  enemy,next_node=get(node,'II')
  if get(enemy+0x2648,'i')[0]==100:break
  node=next_node
 else:raise RuntimeError('Missing Probe')
 def snap(tick):return {'tick':tick,'life':get(enemy+0x2648,'i')[0],'age':get(enemy+0x12b0,'i')[0],'marker':get(enemy+0x127c,'I')[0],'timedOut':bool(get(enemy+0x26f8,'I')[0]&0x800000),'rng':get(0x4ce568,'II')}
 frames=[snap(-1)]
 for tick in range(8):ns['frame']=tick;call(0x413190,ns['MANAGER']);frames.append(snap(tick))
 rows.append({'damage':damage,'pending':pending,'frames':frames,'mainBase64':base64.b64encode(raw).decode()})
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Actual Enemy birth/update/ECL/interrupt and RNG. Controlled query return0/10/80 through existing439ed0 fixture boundary, pending flag35 and age100. No Player/Bomb/ANM scheduler passes; no replay/GPU. Host resource/heap/audio boundaries retained.','rows':rows}
(ROOT/'tests/fixtures/enemy-interrupt-dispatch-original.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps({'cases':len(rows),'frames':rows}))
