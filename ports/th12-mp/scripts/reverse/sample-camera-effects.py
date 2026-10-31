"""Controlled original profile8 constructor and real registered priority14.

No other game updates or accepted replay golden; execution and retirement use
the provider's original callbacks/allocator. Draw42 executes only the original
four float reset writes, stopping before any draw submission.
"""
import json, pathlib, runpy, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
prior = sys.argv
sys.argv = [str(ROOT / 'scripts/reverse/native-enemy-provider.py'), '--frames', '0',
    '--native-player', '--native-items', '--native-camera-effects', '--omit-animation-capture',
    '--input', str(ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-marisa-a.json'),
    '--output', str(ROOT / 'artifacts/reverse/camera-effects-bootstrap.json')]
try: native = runpy.run_path(sys.argv[0])
finally: sys.argv = prior
v, put, get, call = (native[k] for k in ['v', 'put', 'get', 'call'])
from unicorn.x86_const import UC_X86_REG_ESP
rows = []
plans = [
    {'name': 'marisa-a-full', 'ticks': 335, 'births': {0: [3,60,240,30]}},
    {'name': 'sanae-b-full', 'ticks': 75, 'births': {0: [4,1,60,10]}},
    {'name': 'overlap-newest-first', 'ticks': 15, 'births': {0:[3,2,4,3],3:[7,1,2,3],5:[1,1,0,2]}},
    {'name': 'pause-gates-and-stop', 'ticks': 24, 'births': {0:[3,2,30,3]},
     'flags': {1:1,2:2,3:4,4:0x10,5:0x20,6:0x40,7:0x80,8:0x200}, 'stopping': 18},
    {'name': 'fractional-native-timer', 'ticks': 20, 'births': {0:[5,5,10,5]},
     'rates': {1:.25,2:.5,3:0,4:1.25,5:2,6:.999,7:1.001,8:.99,9:1.01}},
]
for plan in plans:
    assert not list(native['camera_nodes']())
    put(0x4ce568,'II',42,0)
    frames=[]
    for tick in range(plan['ticks']):
        born=plan['births'].get(tick)
        if born: call(0x4529a0,8,*born,70)
        flags=plan.get('flags',{}).get(tick,0)
        stopping=int(tick>=plan.get('stopping',10**9))
        rate=plan.get('rates',{}).get(tick,1.)
        put(0x20d0060,'I',flags)
        put(0x4ce55c,'I',stopping)
        for node in native['camera_nodes']():
            data=get(node+0x20,'I')[0]
            put(get(data+0x3c,'I')[0],'f',rate)
        native['tick_camera_effects']()
        camera=native['capture_camera_effects']()
        frames.append({'tick':tick,'born':born,'flags':flags,'stopping':bool(stopping),
                       'rate':get(0x4b2ed0,'f')[0], 'rng':list(get(0x4ce568,'II')),
                       'offsets':camera['shake'], 'timers':[node['timer'][1:] for node in camera['nodes']]})
        prior_stack=v.reg_read(UC_X86_REG_ESP)
        v.emu_start(0x460e40,0x460e62)
        v.reg_write(UC_X86_REG_ESP,prior_stack)
    # Explicit original shutdown for plans that intentionally retain live
    # callbacks after their finite sampling window.
    put(0x4ce55c,'I',1);native['tick_camera_effects']();put(0x4ce55c,'I',0)
    rows.append({'name':plan['name'],'frames':frames})
out=ROOT/'tests/fixtures/camera-effects-original.json'
out.write_text(json.dumps({'schema':'th12-controlled-original-camera-effects/1',
    'originalExeSha256':native['SHA'],'wholeGameOracle':False,'gpuExecuted':False,
    'scope':'Actual4529a0/452a60 birth, registered4527f0 priority14, 452960 destructor; no other managers; Draw42 original460e40..460e62 zero-write block before next frame',
    'initialSeed':42,'rows':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(rows),'frames':sum(len(row['frames'])for row in rows)}))
