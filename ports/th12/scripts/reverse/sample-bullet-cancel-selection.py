"""Actual40a250 birth and40aa60 transforms select Bullet+524 differently."""
import json,pathlib,sys,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
exec(compile((ROOT/'scripts/reverse/sample-last-enemy-id.py').read_text().split('birth_cases =')[0],'helpers','exec'))
sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--input',str(ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json')]
ns={'__file__':str(PROVIDER),'__name__':'cancel_selection_original'}
exec(compile(PROVIDER.read_text().split('SPAWN=0x2070000;')[0],str(PROVIDER),'exec'),ns)
v,put,get,call=(ns[k] for k in ['v','put','get','call'])
cases=[(kind,4,0,None) for kind in [0,3,17,23,24,26,28,29,30]]
cases += [(kind,0,previous,None) for kind in [29,30] for previous in [-1,6,16]]
cases += [(kind,0,previous,target) for kind,target in [(0,29),(29,29),(29,30),(30,29)] for previous in [0,16]]
rows=[]
for index,(kind,color,previous,target) in enumerate(cases):
 commands=[ins(500,[i(0)]),ins(502,[i(0),i(kind),i(color)]),ins(503,[i(0),i(1),i(1)]),
  ins(504,[i(0),f(0),f(0)]),ins(505,[i(0),f(0),f(0)]),ins(507,[i(0),i(1)])]
 if target is not None:commands.append(ins(509,[i(0),i(0),i(0),i(0x1000000),i(target),i(0),f(0),f(0)]))
 commands.extend([ins(501,[i(0)]),ins(83,[i(100000)])])
 raw,entries=scpt([('main',commands)]);base=0x4300000;v.mem_write(base,raw)
 ns['routines']['main']=(base+entries['main'][0],base+entries['main'][1]);put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for at,(_, (text,header)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+at*8,'II',text,header)
 slot=ns['BULLETS']+0x64
 # Controlled physical-slot input, retaining the prior+524 word while making
 # the slot free. Original40a250 chooses it through its actual saved cursor.
 put(ns['BULLETS']+0x10,'I',slot);put(slot+0x532,'H',0);put(slot+0x524,'i',previous)
 request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
 call(0x412990,base+entries['main'][0],request)
 rows.append({'type':kind,'color':color,'previous':previous,'transform':target,'actualType':get(slot+0x9f4,'H')[0],'selected':get(slot+0x524,'i')[0]})
out=ROOT/'tests/fixtures/bullet-cancel-selection-original.json'
out.write_text(json.dumps({'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle':False,'scope':'Controlled slot+524 and free phase/cursor inputs. Actual40a250/40aa60 via real ECL emission and original ANM birth; no original instruction/RNG replaced. No collision, replay, drawing or full-stage claim. Existing provider heap/resource/audio boundaries retained.','rows':rows},indent=2)+'\n')
print(json.dumps({'cases':len(rows),'class5':[{k:r[k] for k in ['type','previous','transform','selected']} for r in rows if r['type']>=29 or r['transform'] is not None]}))
