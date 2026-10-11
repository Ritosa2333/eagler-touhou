"""Actual Player16 successful deathbomb resumes439a40 in the same pass."""
import pathlib,json,sys,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
bootstrap=PROVIDER.read_text().split('SPAWN=0x2070000;')[0]
rows=[]
for loadout in range(6):
 for shot_age in [0,3,10]:
  fixture=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
  fixture['character'],fixture['shot']=divmod(loadout,2)
  fixture['initial'].update(power=400,x=111,y=38400)
  inp=ROOT/'artifacts/reverse/deathbomb-shooting-input.json';inp.write_text(json.dumps(fixture))
  sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets','--native-lasers','--active-collision','--input',str(inp)]
  ns={'__file__':str(PROVIDER),'__name__':'deathbomb_shooting_original'}
  exec(compile(bootstrap,str(PROVIDER),'exec'),ns)
  text=PROVIDER.read_text();exec(compile(text[text.index('def capture_friendly():'):text.index('def capture_player_event(')],str(PROVIDER),'exec'),ns)
  v,put,get,call=(ns[k] for k in ['v','put','get','call']);player=ns['PLAYER']
  from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESI
  put(ns['MANAGER']+0x70,'i',1);put(player+0xc404,'i',0)
  v.reg_write(UC_X86_REG_ESI,player);call(0x438370)
  for at,value in [(0xa30,2),(0xa44,120),(0xc420,shot_age)]:
   v.reg_write(UC_X86_REG_EAX,player+at);call(0x4067e0,value)
  def snap():return {'position':get(player+0x988,'ii'),'state':get(player+0xa28,'i')[0],'stateAge':get(player+0xa34,'i')[0],'shotClock':get(player+0xc424,'i')[0],'rng':get(0x4ce568,'II'),'stock':get(0x4b0ca0,'i')[0],'focused':get(player+0xc598,'i')[0],'friendly':ns['capture_friendly']()}
  initial_options=[get(player+0x82bc+index*0xe4,'ii')+get(player+0x8334+index*0xe4,'i') for index in range(4)]
  frames=[snap()]
  for tick in range(8):put(0x4d49d0,'I',11 if tick==0 else 9);call(0x436ba0,player);frames.append(snap())
  rows.append({'loadout':loadout,'shotAge':shot_age,'initialOptions':initial_options,'frames':frames})
 print(json.dumps({'loadout':loadout,'cases':len(rows)}),flush=True)
out={'originalExeSha256':ns['SHA'],'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Controlled actual438370 then436ba0, retail six SHT/ANM, power400, position(111,38400), stateAge2 and shotAge0/3/10, live enemy count1. Player16 only; no Bomb17/Enemy18/ANM27 or replay/GPU. Resource/heap/audio host boundaries retained.','rows':rows}
(ROOT/'tests/fixtures/deathbomb-shooting-original.json').write_text(json.dumps(out,indent=2)+'\n')
