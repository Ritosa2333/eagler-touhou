"""Pinned synthetic Replay input and stage/main birth timing probes.

No replay file is opened. This uses constructed headers/input triples and
actual43b950/43c730/43c590. The separate birth experiment reuses the native
Enemy provider bootstrap and supplies a tiny synthetic ECL routine; scheduler
priorities are established by original registration instructions, not an
assumed frame offset. This is a partial startup probe, not a whole-game runner.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);PE=pefile.PE(str(EXE))

class ReplayNative:
 def __init__(self,triples,stage=1):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32);self.mu.mem_map(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
  self.mu.mem_write(0x400000,PE.get_memory_mapped_image());self.mu.mem_map(0x1000000,0x10000);self.mu.mem_map(0x2000000,0x40000)
  self.manager,self.header,self.input,self.fps,self.game,self.player=0x2000000,0x2001000,0x2002000,0x2003000,0x2004000,0x2010000
  self.stack,self.stop,self.stage=0x100e000,0x100f000,stage;self.events=[]
  self.put(0x4b44e8,'I',self.game);self.put(0x4b4518,'I',self.manager);self.put(0x4b4514,'I',self.player)
  self.put(self.manager+0x1c,'I',0x2005000);self.put(0x2005000+0x5c,'3i',2,1,3)
  self.put(0x4b0cb0,'i',stage);self.put(0x4b0cd0,'2i',400,100);self.put(0x4b2ed0,'f',1)
  self.put(self.manager+0x10,'i',1);self.put(self.manager+0x1d8,'i',stage)
  self.put(self.manager+stage*36+0xa8,'6I',self.input,self.input,self.fps,self.fps,self.header,0)
  self.put(self.header,'HHIIihHi4h3i3i4i',stage,0x3456,len(triples),len(triples)*6+2,1234567,143,0x7777,12345000,
   -1,4,2,2,0,0,0,42,768,49920,314,3,1,9876)
  self.mu.mem_write(self.input,b''.join(struct.pack('<3H',*x) for x in triples));self.mu.mem_write(self.fps,bytes([60,59,58,57]))
  self.put(0x4d49d0,'5I',0x40,0x20,0x12345678,0x80,0x10)
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.hook_add(UC_HOOK_CODE,self.hook)
 def put(self,p,fmt,*v):self.mu.mem_write(p,struct.pack('<'+fmt,*v))
 def read(self,p,fmt):return list(struct.unpack('<'+fmt,self.mu.mem_read(p,struct.calcsize('<'+fmt))))
 def hook(self,mu,address,size,data):
  if address==0x432850:
   # End-marker recognition is original. Pause/menu construction is outside
   # this input probe and captured at its entry, with no rewritten branch.
   self.events.append('replay-end-menu');sp=mu.reg_read(UC_X86_REG_ESP)
   mu.reg_write(UC_X86_REG_EIP,self.read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4)
 def call(self,address):
  self.put(self.stack,'I',self.stop);self.mu.reg_write(UC_X86_REG_ESP,self.stack);self.mu.reg_write(UC_X86_REG_ECX,self.manager)
  self.mu.emu_start(address,self.stop,count=100000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=self.stop:raise RuntimeError(f'Replay{address:x} budget')
 def constructor_restore(self):
  # Replay bytes have already been decoded into mapped records. Execute the
  # original selected-stage restore segment before callback allocation.
  self.mu.reg_write(UC_X86_REG_ESP,self.stack);self.mu.reg_write(UC_X86_REG_EBP,self.manager)
  self.mu.emu_start(0x43b1fb,0x43b31b,count=100000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=0x43b31b:raise RuntimeError('Constructor restore budget')
 def state(self):
  return {'stageCursor':self.read(self.manager+self.stage*36+0xbc,'i')[0],
   'totalCursor':self.read(self.manager+0x1d0,'i')[0],
   'inputByteOffset':self.read(self.manager+self.stage*36+0xac,'I')[0]-self.input,
   'fpsByteOffset':self.read(self.manager+self.stage*36+0xb4,'I')[0]-self.fps,
   'heldPreviousRepeatPressedReleased':self.read(0x4d49d0,'5I'),
   'fps':self.read(self.manager+0x1cc,'B')[0],'playerFixed':self.read(self.player+0x988,'2i'),
   'playerPosition':self.read(self.player+0x97c,'2f'),'focus':self.read(self.player+0xc598,'i')[0],
   'scorePowerPiv':self.read(0x4b0c44,'2i')+self.read(0x4b0c78,'i'),
   'lifeBomb':self.read(0x4b0c98,'4i'),'rank':self.read(0x4b0ccc,'i')[0],
   'cc4ExtendGraze':self.read(0x4b0cc4,'i')+self.read(0x4b0cd8,'2i'),
   'rngSeedCounter':self.read(0x4ce568,'2I'),'events':list(self.events)}

records=[]
def input_case(name,triples,restore=False,cursor=0,absent_game=False):
 n=ReplayNative(triples);steps=[]
 if restore:
  n.constructor_restore();steps.append({'step':'selected-stage-constructor43b1fb',**n.state()})
  n.call(0x43c730);steps.append({'step':'restore43c730',**n.state()})
  n.call(0x43b950);steps.append({'step':'negative-cursor-before-stage0',**n.state()})
  n.call(0x43c590);steps.append({'step':'stage-ready43c590',**n.state()})
 else:n.put(n.manager+n.stage*36+0xbc,'i',cursor)
 if absent_game:n.put(0x4b44e8,'I',0)
 for tick in range(len(triples)+1):
  before=n.state();n.call(0x43b950);steps.append({'step':'read'+str(tick),'before':before,**n.state()})
 records.append({'name':name,'triples':triples,'restore':restore,'steps':steps})
input_case('startup-zero-then-recorded-edges',[[0x81,0x81,0],[0x40,0x40,0x81],[0,0,0x40]],True)
for name,words in [('only-held-ffff',[0xffff,0,0]),('held-pressed-ffff',[0xffff,0xffff,0]),
 ('pressed-released-ffff',[0,0xffff,0xffff]),('all-ffff',[0xffff,0xffff,0xffff])]:input_case(name,[words])
input_case('negative-stage-cursor',[[1,1,0]],cursor=-1)
input_case('no-game-manager',[[1,1,0]],absent_game=True)

# Execute the true birth/first two Enemy18 visits with the input callback in
# between. This is manual dispatch of a known subset, not the full scheduler.
provider=ROOT/'scripts/reverse/native-enemy-provider.py'
source=provider.read_text(encoding='utf8').split('if args.routine not in routines:')[0]
saved_argv=sys.argv;sys.argv=[str(provider),'--frames','1'];ns={'__file__':str(provider),'__name__':'replay_startup_probe'}
try:exec(compile(source,str(provider),'exec'),ns)
finally:sys.argv=saved_argv
v,put,get,call=ns['v'],ns['put'],ns['get'],ns['call'];manager=ns['MANAGER'];player=ns['PLAYER']
def ins(op,payload=(),time=0):
 raw=b''.join(payload);return struct.pack('<iHHHBBI',time,op,16+len(raw),0,63,len(payload),0)+raw
f=lambda value:struct.pack('<f',value)
code=b'ECLH'+bytes(12)+ins(300,[f(0),f(100)])+ins(300,[f(11),f(100)],1)+ins(300,[f(22),f(100)],2)+ins(83,[struct.pack('<i',100000)],2)+ins(10)
name_ptr,code_ptr=0x4300000,0x4300100;v.mem_write(name_ptr,b'ReplayMain\0');v.mem_write(code_ptr,code)
ns['routines']['ReplayMain']=(name_ptr,code_ptr);lookup=ns['PROGRAM']+0x100
put(ns['PROGRAM']+8,'I',len(ns['routines']))
for i,(_,pair) in enumerate(sorted(ns['routines'].items())):put(lookup+i*8,'2I',*pair)
put(manager+0x50,'iifII',-1,0,0.,0x4b2ed0,1)
replay,header,stream,fps,game=0x20f0000,0x20f1000,0x20f2000,0x20f3000,0x20e0000
put(0x4b4518,'I',replay);put(0x4b44e8,'I',game);put(replay+0x10,'i',1);put(replay+0x1d8,'i',1)
put(replay+36+0xa8,'6I',stream,stream,fps,fps,header,0)
put(replay+0x1c,'I',0x20f4000);put(0x20f4000+0x5c,'3i',0,0,1)
put(header,'HHIIihHi4h3i3i4i',1,0x3456,3,20,1234567,143,0x7777,12345000,-1,4,2,2,0,0,0,42,768,49920,314,3,1,9876)
put(stream,'9H',0x81,0x81,0,0x40,0x40,0x81,0,0,0x40);v.mem_write(fps,bytes([60,59]))
put(0x4d49d0,'5I',0,0,0,0,0)
v.reg_write(UC_X86_REG_ESP,ns['SP']);v.reg_write(UC_X86_REG_EBP,replay)
v.emu_start(0x43b1fb,0x43b31b,count=100000)
assert v.reg_read(UC_X86_REG_EIP)==0x43b31b
call(0x43c730);call(0x43c590)
request=0x2070000;put(request,'fffiiiII',0,100,0,0,0,10000,0,0)
enemy=call(0x412990,name_ptr,request);startup=[]
def capture(step):
 context=get(enemy+4,'I')[0]
 return {'step':step,'enemyId':get(enemy+0x27b4,'I')[0],'position':list(get(enemy+0x1074,'2f')),
  'age':get(enemy+0x12b0,'i')[0],'flags':get(enemy+0x26f8,'I')[0],
  'contextTime':get(context,'f')[0],'contextByteOffset':get(context+4,'I')[0]-code_ptr-16,
  'input':list(get(0x4d49d0,'5I')),'inputCursor':get(replay+36+0xbc,'i')[0],
  'playerFixed':list(get(player+0x988,'2i')),'focus':get(player+0xc598,'i')[0],
  'power':get(0x4b0c48,'i')[0],'rank':get(0x4b0ccc,'i')[0],'rng':list(get(0x4ce568,'2I'))}
startup.append(capture('main-synchronous-birth'))
for index in range(3):
 v.reg_write(UC_X86_REG_ECX,replay);call(0x43b950);startup.append(capture('input'+str(index)+'-priority11'))
 call(0x413190,manager);startup.append(capture('enemy-priority18-after-input'+str(index)))
assert startup[2]['age']==startup[0]['age'] and startup[2]['position']==startup[0]['position']
assert startup[4]['position'][0]==11 and startup[6]['position'][0]==22
out=ROOT/'tests/fixtures/replay-input-order-original.json'
out.write_text(json.dumps({'schema':'th12-original-replay-input-order/1','originalExeSha256':EXE_SHA256,
 'wholeGameOracle':False,'originalReplayLoaded':False,'originalFilesModified':False,'gpuExecuted':False,
 'priorities':{'stage':'42200a/42200f ->422bd0 priority10','input':'43b356/43b35b ->43c520 priority11','player':16,'enemy':18},
 'provider':'Actual selected-stage43b1fb..43b31b restore segment,43b950/43c730/43c590 synthetic objects; actual412990/413840/413190 synthetic ECL with native Enemy bootstrap.',
 'fixtureCallbacks':['Input-only432850 captures requested replay-end menu without executing UI.','Birth experiment inherits native-enemy-provider host allocator/resource/audio/passive damage boundaries.'],
 'limitations':['Manual known callback subset; full StageGame initialization, Player16, audio, draw, disk IO and whole scheduler excluded.',
  'Synthetic input triples are not a user replay; no deleted replay bytes, copied stale replay, or accepted golden is read.',
  'header+44 is actual graze4b0cdc, restored at selected-stage replay construction43b316; stage-transition43c730/43c590 alone keep the existing global value.'],
 'headerNativeGlobals':{'offset56':'global4b0cc4 continue/score low digit; increment path not sampled','offset60':'global4b0cd8 score-extend threshold index','offset64':'Player+c598 focus','offset68':'global4b0cdc graze; selected-stage constructor restore'},
 'records':records,'startup':startup},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'inputCases':len(records),'startupStates':len(startup)}))
