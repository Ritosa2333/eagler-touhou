"""Actual Player16 respawn at normal/half/quarter rate, across six retail SHTs.

Controlled state/timer inputs; immutable original instructions execute the
transition and damage-source creation. No replay or GPU equivalence claim.
"""
import pathlib,json,sys,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
bootstrap=PROVIDER.read_text().split('SPAWN=0x2070000;')[0]
rows=[]
for loadout in range(6):
 fixture=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
 fixture['character'],fixture['shot']=divmod(loadout,2)
 fixture['initial'].update(power=400)
 input_path=ROOT/'artifacts/reverse/player-respawn-rate-input.json';input_path.write_text(json.dumps(fixture))
 sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--input',str(input_path)]
 ns={'__file__':str(PROVIDER),'__name__':'respawn_rate_original'}
 exec(compile(bootstrap,str(PROVIDER),'exec'),ns)
 v,put,get,call=(ns[k] for k in ['v','put','get','call']);player=ns['PLAYER']
 from unicorn.x86_const import UC_X86_REG_EAX
 for rate in [1,.5,.25]:
  for age in [29,30]:
   put(0x4b2ed0,'f',rate);put(ns['MANAGER']+0x70,'i',1)
   put(0x4b0c98,'i',1);put(0x4b0ca0,'i',2);put(0x4b0cd4,'i',400)
   put(player+0xa28,'i',2);put(player+0x988,'ii',111,38400);put(player+0x97c,'fff',111/128,300,0)
   for timer,value in [(0xa30,age),(0xa44,120),(0xc400,120)]:
    v.reg_write(UC_X86_REG_EAX,player+timer);call(0x4067e0,value)
   put(player+0x825c,'f',1);put(player+0xc598,'i',0);put(player+0xa14,'ii',0,0)
   def snap():return {'fixed':list(get(player+0x988,'ii')),'state':get(player+0xa28,'i')[0],
    'stateAge':get(player+0xa34,'i')[0],'stateTime':get(player+0xa38,'f')[0],
    'totalAge':get(player+0xa48,'i')[0],'invulnerability':get(player+0xc404,'i')[0],
    'rate':get(0x4b2ed0,'f')[0]}
   frames=[snap()];put(0x4d49d0,'I',0);call(0x436ba0,player);frames.append(snap())
   rows.append({'loadout':loadout,'rate':rate,'age':age,'frames':frames})
 print(json.dumps({'loadout':loadout,'cases':len(rows)}),flush=True)
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle':False,'scope':__doc__,'rows':rows}
(ROOT/'tests/fixtures/player-respawn-rate-original.json').write_text(json.dumps(out,indent=2)+'\n')
