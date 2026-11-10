"""Actual profile1 4526e0 callbacks, mixed profile8 order and pause behavior.

Original constructor, scheduler list, timer, RNG, destructor and Draw42 reset
writes execute. The selected-manager startup is an explicit fixture; no replay
acceptance, GPU or mathematical callback replacements are used.
"""
import json,pathlib,runpy,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
saved=sys.argv
sys.argv=[str(ROOT/'scripts/reverse/native-enemy-provider.py'),'--frames','0','--native-player',
    '--native-items','--native-camera-effects','--omit-animation-capture',
    '--input',str(ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-marisa-a.json'),
    '--output',str(ROOT/'artifacts/reverse/camera-pulse-bootstrap.json')]
try:host=runpy.run_path(sys.argv[0])
finally:sys.argv=saved
v,put,get,call=(host[k] for k in ['v','put','get','call'])
from unicorn.x86_const import UC_X86_REG_ESP
plans=[
 {'name':'retail-ecl417-fade','ticks':34,'births':{0:[[1,30,12,0,0]]}},
 {'name':'reimu-b-orb-burst','ticks':12,'births':{0:[[1,8,6,6,0]]}},
 {'name':'signed-amplitude-rise','ticks':22,'births':{0:[[1,17,-7,12,0]]}},
 {'name':'zero-duration-retires','ticks':3,'births':{0:[[1,0,8,4,0]]}},
 {'name':'negative-duration-retires','ticks':3,'births':{0:[[1,-1,8,4,0]]}},
 {'name':'profile1-ignores-application-pause','ticks':15,'births':{0:[[1,20,8,4,0]]},
  'flags':{1:1,2:2,3:4,4:16,5:32,6:64,7:0x77,8:0x80},'stopping':11},
 {'name':'mixed-newest-first','ticks':18,'births':{0:[[8,3,2,8,4],[1,8,6,6,0]],3:[[8,7,1,3,2]],5:[[1,6,-3,5,0]]},
  'flags':{6:0x77,7:0x77}},
 {'name':'pulse-fractional-timer','ticks':25,'births':{0:[[1,20,11,-5,0]]},
  'rates':{1:.25,2:.5,3:0,4:1.25,5:2,6:.999,7:1.001,8:.99,9:1.01}},
]
records=[]
for plan in plans:
 assert not list(host['camera_nodes']())
 put(0x4ce568,'II',42,0);frames=[]
 for tick in range(plan['ticks']):
  births=plan['births'].get(tick,[])
  for birth in births:call(0x4529a0,*(value&0xffffffff for value in birth),70)
  flags=plan.get('flags',{}).get(tick,0);stopping=tick>=plan.get('stopping',10**9)
  put(0x20d0060,'I',flags);put(0x4ce55c,'I',int(stopping))
  rate=plan.get('rates',{}).get(tick,1.)
  for owner in host['camera_nodes']():
   data=get(owner+0x20,'I')[0];put(get(data+0x3c,'I')[0],'f',rate)
  host['tick_camera_effects']();snapshot=host['capture_camera_effects']()
  frames.append({'tick':tick,'births':births,'flags':flags,'stopping':stopping,'rate':get(0x4b2ed0,'f')[0],
   'rng':list(get(0x4ce568,'II')),'offsets':snapshot['shake'],
   'nodes':[{'callback':n['callback'],'timer':n['timer'][1:]} for n in snapshot['nodes']]})
  old_sp=v.reg_read(UC_X86_REG_ESP);v.emu_start(0x460e40,0x460e62);v.reg_write(UC_X86_REG_ESP,old_sp)
 put(0x4ce55c,'I',1);host['tick_camera_effects']();put(0x4ce55c,'I',0)
 records.append({'name':plan['name'],'frames':frames})
output=ROOT/'tests/fixtures/camera-pulse-original.json'
output.write_text(json.dumps({'schema':'th12-original-camera-pulse/1','originalExeSha256':host['SHA'],
 'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,'seed':42,
 'boundary':'Actual4529a0/452a60 birth, registered4526e0(profile1)/4527f0(profile8),464a80 timer,main RNG,452960 destructor and460e40..460e62 Draw42 reset writes. Selected-manager unpaused startup host fixture, original application flags explicitly varied. No other managers advanced, no game instructions replaced.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'cases':len(records),'frames':sum(len(r['frames'])for r in records)}))
