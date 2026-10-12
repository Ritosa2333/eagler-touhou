"""Original ECL275 dispatch and script69's four immediate laser children."""
import pathlib,json,sys,hashlib,base64
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
sys.path.insert(0,str(ROOT.parent/'tools/python'));sys.path.insert(0,str(PROVIDER.parent))
exec(compile((ROOT/'scripts/reverse/sample-last-enemy-id.py').read_text().split('birth_cases =')[0],'helpers','exec'))
fixture=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
fixture.update(stage=6,character=2,shot=1,difficulty=1,inputOnly=False,replay=None)
fixture['initial'].update(score=0,power=400,pointValue=2000000,rank=0,x=0,y=51200,nativeGlobals=[0,0,0,0])
path=ROOT/'artifacts/reverse/enemy-anm-irq-input.json';path.write_text(json.dumps(fixture))
sys.argv=[str(PROVIDER),'--frames','0','--input',str(path),'--seed','1234']
ns={'__file__':str(PROVIDER),'__name__':'enemy_anm_irq'}
text=PROVIDER.read_text();exec(compile(text.split('if args.routine not in routines:')[0],str(PROVIDER),'exec'),ns)
v,put,get,call=(ns[k] for k in ['v','put','get','call'])
raw,entries=scpt([('main',[ins(258,[i(2)]),ins(259,[i(0),i(69)]),ins(277,[i(0),f(1.5707963705062866)]),ins(402,[i(797)]),ins(83,[i(8)]),ins(275,[i(0),i(1)]),ins(83,[i(12)]),ins(275,[i(0),i(2)]),ins(83,[i(100000)])])])
base=0x4300000;v.mem_write(base,raw)
for name,(string_at,header) in entries.items():ns['routines'][name]=(base+string_at,base+header)
put(ns['PROGRAM']+8,'I',len(ns['routines']))
for index,(_, (string_at,header)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+index*8,'II',string_at,header)
put(0x4ce568,'II',1234,0);request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
enemy=call(0x412990,base+entries['main'][0],request)
from unicorn.x86_const import UC_X86_REG_EDX,UC_X86_REG_EDI
def snapshot(tick):
 v.reg_write(UC_X86_REG_EDX,get(0x4ce8cc,'I')[0]);root=call(0x461920,get(enemy+0x1120,'I')[0]);nodes=[]
 if root:
  pointers=[root];child=get(root+0x14,'I')[0]
  while child:pointers.append(get(child,'I')[0]);child=get(child+4,'I')[0]
  for at in pointers:nodes.append({'script':get(at+0x3ea,'H')[0],'clock':get(at+0x6c,'i')[0],'pending':get(at+0x3c4,'h')[0],'sprite':get(at+0x3e4,'h')[0],'scale':get(at+0x40,'ff'),'primary':get(at+0x3bc,'I')[0]})
 return {'tick':tick,'rng':get(0x4ce568,'II'),'nodes':nodes}
rows=[snapshot(-1)]
for tick in range(42):
 ns['frame']=tick;call(0x413190,ns['MANAGER']);v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0);rows.append(snapshot(tick))
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Controlled original ECL259/277/275,461970 and original stgenm06 script69/64..67, actual Enemy18 and Primary27. Passive damage/player/audio/resource/allocator host boundaries. No replay or GPU.','seed':1234,'character':2,'shot':1,'difficulty':1,'initial':fixture['initial'],'mainBase64':base64.b64encode(raw).decode(),'rows':rows}
(ROOT/'tests/fixtures/enemy-anm-irq-original.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps({'rows':len(rows),'children':rows[0]['nodes'],'active':rows[13]['nodes']}))
