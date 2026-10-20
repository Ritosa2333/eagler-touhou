"""Actual40dd50 wall-time observer/quantization with a controlled QPC host.

4508b0 and all original CRT arithmetic run unchanged. Only the platform
QueryPerformanceCounter output is supplied. No logical ticks stand in for time.
"""
import json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
SOURCE=ROOT/'scripts/reverse/sample-boss-spell-times.py'
ns={'__file__':str(SOURCE),'__name__':'native_spell_wall_time'}
exec(compile(SOURCE.read_text(encoding='utf8').split('countdowns=[]')[0],str(SOURCE),'exec'),ns)
Native=ns['Native']
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *

def setup():
 n=Native();n.now=0;n.qpc=0x100fe00
 n.put(0x4980d4,'I',n.qpc);n.put(0x4cf408,'q',1000000)
 n.put(0x4cf410,'q',0);n.put(0x4cf448,'d',0)
 n.put(0x4b44e8,'I',0x2080000);n.put(0x4b4518,'I',0x2078000)
 n.put(0x4b0cb0,'i',1);n.put(0x2078000+0x24,'I',0x2090000)
 def qpc(mu,address,size,data):
  sp=mu.reg_read(UC_X86_REG_ESP);target=n.read(sp+4,'I')[0]
  n.put(target,'q',round(n.now*1000));mu.reg_write(UC_X86_REG_EAX,1);n.ret(4)
 n.mu.hook_add(UC_HOOK_CODE,qpc,begin=n.qpc,end=n.qpc)
 return n

rows=[]
for duration in [0,8,8.35,8.36,16.7,25.05,1000,2345,58345,999999,1000000]:
 n=setup();n.now=1000;n.put(n.spell+0x7c,'I',3);n.put(n.spell+0x90,'i',121)
 n.call(0x40dd50);began=n.read(n.spell+0x98,'d')[0]
 n.now+=duration;n.put(n.spell+0x7c,'I',n.read(n.spell+0x7c,'I')[0]&~1)
 n.call(0x40dd50);encoded=n.read(n.spell+0xa8,'i')[0]
 n.call(0x41d0b0,[n.stack+0x100],EBX=n.stack+0x104)
 rows.append({'milliseconds':duration,'startSeconds':began,'serialResult':n.read(n.spell+0x94,'i')[0],
  'encoded':encoded,'decoded':n.read(n.stack+0x100,'2i'),'flagsAfter':n.read(n.spell+0x7c,'I')[0],
  'recorded':n.read(0x2090000+0x4c,'i')[0]})

# Original wall observer runs at the application presentation boundary,
# independently of logical update callbacks. Active paused observations do
# not rebase the start. The elapsed duration includes these host intervals.
n=setup();observations=[]
for ms,active,pause in [(1000,True,0),(2000,True,1),(6000,True,1),(7000,False,0)]:
 n.now=ms;n.put(0x2080000+0x60,'I',pause);flags=n.read(n.spell+0x7c,'I')[0]
 n.put(n.spell+0x7c,'I',(flags|1) if active else flags&~1);n.call(0x40dd50)
 observations.append({'milliseconds':ms,'active':active,'applicationFlags':pause,
  'startSeconds':n.read(n.spell+0x98,'d')[0],'flags':n.read(n.spell+0x7c,'I')[0],
  'encoded':n.read(n.spell+0xa8,'i')[0]})

out=ROOT/'tests/fixtures/spell-wall-time-original.json'
out.write_text(json.dumps({'schema':'th12-original-spell-wall-time/1','originalExeSha256':ns['EXE_SHA256'],
 'wholeGameOracle':False,'gpuExecuted':False,
 'scope':'Actual40dd50,4508b0 QPC seconds and CRT quantization/encoding, plus41d0b0 decode. Only platform QueryPerformanceCounter is a deterministic host. Spell active/serial and presentation timestamps are controlled inputs, not a full application scheduler or pause oracle.',
 'roundingSeconds':n.read(0x4a3e60,'d')[0],'midpointSeconds':n.read(0x4a3e58,'d')[0],
 'cases':rows,'pausedObservations':observations},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'cases':rows,'pausedObservations':observations}))
