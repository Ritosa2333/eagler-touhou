"""Controlled local BossCard6 comparison; not a replay checkpoint/oracle."""
import json,pathlib,sys,struct
ROOT=pathlib.Path(__file__).resolve().parents[2]
bomb_case=True
active_case=True
label='death-shooting-gate'
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
sys.path.insert(0,str(PROVIDER.parent))
exec(compile((ROOT/'scripts/reverse/sample-last-enemy-id.py').read_text().split('birth_cases =')[0], 'helpers', 'exec'))
fixture=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
fixture.update(stage=6,seed=1234,replay=None,scope='Controlled stationary local BossCard6 fixture, no replay checkpoint')
fixture['initial'].update(power=400,score=0,pointValue=2000000,rank=0,x=10240,y=51200,nativeGlobals=[0,0,0,0])
bootstrap=ROOT/'artifacts/reverse/s6-boss-death-shooting-local-input.json';bootstrap.write_text(json.dumps(fixture))
sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--native-lasers','--native-camera-effects','--capture-text-output','--output',str(ROOT/'artifacts/reverse/s6-boss-death-shooting-local-diagnostic.json'),'--instruction-budget','20000000','--input',str(bootstrap)]
ns={'__file__':str(PROVIDER),'__name__':'local_card1_original'}
sys.argv.extend(['--trace-rng-ticks','379','390'])
if active_case:sys.argv.extend(['--active-collision'])
exec(compile(PROVIDER.read_text().split('SPAWN=0x2070000;')[0],str(PROVIDER),'exec'),ns)
text=PROVIDER.read_text();exec(compile(text[text.index('def capture_bullets():'):text.index('def capture_lasers():')],str(PROVIDER),'exec'),ns)
exec(compile(text[text.index('def camera_nodes():'):text.index('def capture_message_event():')],str(PROVIDER),'exec'),ns)
v,put,get,call=(ns[k] for k in ['v','put','get','call'])
raw,entries=scpt([('main',[ins(402,[i(0xa0)]),ins(256,[string('BossDeathFixture'),f(0),f(128),i(13200),i(0),i(0)]),ins(83,[i(100000)])]),('BossDeathFixture',[ins(258,[i(2)]),ins(262,[i(0),i(0)]),ins(400,[f(64),f(64)]),ins(401,[f(64),f(64)]),ins(411,[i(13200)]),ins(300,[f(0),f(128)]),ins(422,[i(99),i(7200),i(500000),string('fixture')]),ins(11,[string('BossDead')]),ins(83,[i(100000)])])])
(ROOT/'artifacts/reverse/s6-boss-death-shooting-local-main.ecl').write_bytes(raw)
base=0x4300000;v.mem_write(base,raw);
for name,(text,header) in entries.items():ns['routines'][name]=(base+text,base+header)
put(ns['PROGRAM']+8,'I',len(ns['routines']))
for index,(_, (text,header)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+index*8,'II',text,header)
put(0x4ce568,'II',1234,0);request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
root_enemy=call(0x412990,base+entries['main'][0],request)
node=get(ns['MANAGER']+0x68,'I')[0]
while node:
 enemy,next_node=get(node,'II')
 if get(enemy+0x2648,'i')[0]==13200:break
 node=next_node
else:raise RuntimeError('Controlled Boss not registered')
from unicorn.x86_const import UC_X86_REG_EDI,UC_X86_REG_EAX,UC_X86_REG_ESI
def snapshot(tick):return {'tick':tick,'rng':list(get(0x4ce568,'II')),'position':list(get(enemy+0x1074,'2f')),'angle':get(enemy+0x10c4,'f')[0],'health':get(enemy+0x2648,'i')[0],'graze':get(0x4b0cdc,'i')[0],'flags':get(enemy+0x26f8,'I')[0],'animation':get(enemy+0x126c,'i')[0],'score':get(0x4b0c44,'i')[0],'bombActive':get(ns['bomb_manager']+0x3c,'I')[0],'rate':get(0x4b2ed0,'f')[0],'shotClock':get(ns['PLAYER']+0xc424,'i')[0],'playerAge':get(ns['PLAYER']+0xa30,'ii'),'age':get(enemy+0x12b0,'i')[0],'spellAge':get(ns['bonus_manager']+0x28,'i')[0],'serialTicks':get(ns['bonus_manager']+0x90,'i')[0],'spellFlags':get(ns['bonus_manager']+0x7c,'I')[0]}
rows=[snapshot(-1)]
for tick in range(340):
 ns['frame']=tick;put(0x4d49d0,'I',1)
 v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x4610d0)
 ns['tick_camera_effects']() if 'tick_camera_effects' in ns else None
 call(0x436ba0,ns['PLAYER']);v.reg_write(UC_X86_REG_EAX,ns['bomb_manager']);call(0x406ce0)
 call(0x413190,ns['MANAGER']);v.reg_write(UC_X86_REG_EAX,ns['ufo_manager']);call(0x44a2d0)
 v.reg_write(UC_X86_REG_EDI,ns['laser_manager']);call(0x428270)
 v.reg_write(UC_X86_REG_ESI,ns['BULLETS']);call(0x40a020)
 call(0x425c20,ns['item_manager']);v.reg_write(UC_X86_REG_EDI,ns['bonus_manager']);call(0x40db40)
 v.reg_write(UC_X86_REG_EDI,ns['stage_manager']);call(0x41e020)
 v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0)
 rows.append(snapshot(tick))
 if tick in [0,59,60,119,120,121,200,279,280,281,320,339]:rows[-1].update(animations=ns['capture_animations']())
 if tick%60==0:print(json.dumps({k:v for k,v in rows[-1].items() if k not in ['bullets','animations']}),flush=True)
(ROOT/f'artifacts/reverse/s6-boss-death-shooting-local-native-{label}.json').write_text(json.dumps({'wholeGameOracle':False,'collisionAndDamageExcluded':False,'rows':rows,'events':ns['events']}))






import hashlib,base64
output={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Controlled protected main, retail BossDead, actual continuous Sanae B shooting, Player16/Enemy18/ANM/Spell with resource/heap/audio/text host boundaries; no replay/GPU claim.','initial':fixture['initial'],'character':fixture['character'],'shot':fixture['shot'],'difficulty':fixture['difficulty'],'seed':1234,'mainBase64':base64.b64encode(raw).decode(),'rows':[{k:x[k] for k in ['tick','rng','rate','score','shotClock','serialTicks','spellAge']} for x in rows]}
(ROOT/'tests/fixtures/slow-player-damage-original.json').write_text(json.dumps(output,indent=2)+'\n')
