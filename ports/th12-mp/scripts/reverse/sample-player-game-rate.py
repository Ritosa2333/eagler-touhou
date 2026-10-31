"""Actual436ba0 movement and state/invulnerability timers at retail slow rates."""
import pathlib,json,sys,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
bootstrap=PROVIDER.read_text().split('SPAWN=0x2070000;')[0]
rows=[]
from itertools import product
for loadout in range(6):
 fixture=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
 fixture['character'],fixture['shot']=divmod(loadout,2)
 fixture['initial'].update(power=400)
 input_path=ROOT/'artifacts/reverse/player-game-rate-input.json';input_path.write_text(json.dumps(fixture))
 sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--input',str(input_path)]
 ns={'__file__':str(PROVIDER),'__name__':'player_rate_original'}
 exec(compile(bootstrap,str(PROVIDER),'exec'),ns)
 v,put,get,call=(ns[k] for k in ['v','put','get','call']);player=ns['PLAYER']
 from unicorn.x86_const import UC_X86_REG_EAX
 for rate,focus,direction in product([1,.5,.25],[0,8],[0x10,0x40,0x50,0xa0]):
  put(0x4b2ed0,'f',rate);put(ns['MANAGER']+0x70,'i',1)
  put(player+0xa28,'i',1);put(player+0x988,'ii',111,38400);put(player+0x97c,'fff',111/128,300,0)
  for timer,value in [(0xa30,120),(0xa44,120),(0xc400,120)]:
   v.reg_write(UC_X86_REG_EAX,player+timer);call(0x4067e0,value)
  put(player+0x825c,'f',1);put(player+0xc598,'i',0);put(player+0xa14,'ii',0,0)
  def snap():return {'fixed':list(get(player+0x988,'ii')),'stateAge':get(player+0xa34,'i')[0],'totalAge':get(player+0xa48,'i')[0],'invulnerability':get(player+0xc404,'i')[0],'focused':get(player+0xc598,'i')[0]}
  frames=[snap()];held=focus|direction
  for tick in range(8):put(0x4d49d0,'I',held);call(0x436ba0,player);frames.append(snap())
  rows.append({'loadout':loadout,'rate':rate,'held':held,'frames':frames})
 print(json.dumps({'loadout':loadout,'cases':len(rows)}),flush=True)
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Controlled actual Player16, retail SHT/ANM, live enemy-count1, no shot/Bomb/collision. State/total/invulnerability clocks initially120, position(111,38400). Host resource/heap/audio boundaries; not replay/GPU.','rows':rows}
(ROOT/'tests/fixtures/player-game-rate-original.json').write_text(json.dumps(out,indent=2)+'\n')
