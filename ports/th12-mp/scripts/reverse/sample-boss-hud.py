"""TH12 1.00b Boss HUD ratio/name/star tick and rectangle call arguments.

Runs retail41e439..41e727 and41f33d..41f450 unchanged. ANM factory/queued
interrupt and GPU rectangle submission are explicit capture fixtures. No HP
arithmetic, threshold comparison, visibility, script-selection or rectangle
calculation is intercepted. This is not a whole HUD/ANM/GPU/whole-game Oracle.
"""
import json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);PE=pefile.PE(str(EXE))

class Native:
 def __init__(self,displayed=0,thresholds=None):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32)
  self.mu.mem_map(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
  self.mu.mem_write(0x400000,PE.get_memory_mapped_image())
  self.mu.mem_map(0x1000000,0x10000);self.mu.mem_map(0x2000000,0x200000)
  self.hud,self.manager,self.enemy,self.player,self.resource,self.stack=0x2000000,0x2020000,0x2030000,0x2040000,0x2100000,0x100d000
  self.next_handle=1;self.handles={};self.events=[];self.rectangles=[]
  self.put(self.hud+0x6d44,'I',self.resource)
  self.put(self.hud+0x6ce8,'2f',displayed,displayed)
  self.put(self.hud+0x6cf0,'i',500)
  self.put(self.player+0x97c,'3f',0,400,0)
  for i,(fraction,argb) in enumerate(thresholds or [[0,0],[0,0],[0,0],[0,0]]):
   self.put(self.hud+0x6cf8+i*8,'fI',fraction,argb)
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  self.mu.hook_add(UC_HOOK_CODE,self.hook)
 def put(self,p,fmt,*x):self.mu.mem_write(p,struct.pack('<'+fmt,*x))
 def get(self,p,fmt):return list(struct.unpack('<'+fmt,self.mu.mem_read(p,struct.calcsize('<'+fmt))))
 def ret(self,pop=0):
  sp=self.mu.reg_read(UC_X86_REG_ESP);target=self.get(sp,'I')[0]
  self.mu.reg_write(UC_X86_REG_EIP,target);self.mu.reg_write(UC_X86_REG_ESP,sp+4+pop)
 def hook(self,mu,address,size,data):
  sp=mu.reg_read(UC_X86_REG_ESP)
  if address==0x4615a0:
   resource,out,script,flags=self.get(sp+4,'4I');handle=self.next_handle;self.next_handle+=1
   event={'kind':'bind','handle':handle,'script':script,'layer':mu.reg_read(UC_X86_REG_EAX),
    'bank':'front','resource':hex(resource),'registrationFlags':flags,'parentECX':mu.reg_read(UC_X86_REG_ECX)}
   self.handles[handle]=event;self.events.append(event)
   self.put(out,'I',handle);mu.reg_write(UC_X86_REG_EAX,0x2110000+handle*0x500);self.ret(16)
  elif address==0x461970:
   handle=self.get(sp+4,'I')[0];code=mu.reg_read(UC_X86_REG_EBX)
   self.events.append({'kind':'interrupt','handle':handle,'code':code,'effectiveFixtureHandle':handle in self.handles})
   self.ret(4)
  elif address==0x451f30:
   bounds=self.get(mu.reg_read(UC_X86_REG_EDI),'4f')
   self.rectangles.append({'bounds':bounds,'argb':mu.reg_read(UC_X86_REG_EBX)})
   self.ret()
 def execute(self,start,stop,registers):
  # These are basic-block segments of larger functions. Their stack locals are
  # mapped directly; no fake prologue/return is claimed or needed at stop.
  self.mu.mem_write(self.stack,bytes(0x100))
  self.mu.reg_write(UC_X86_REG_ESP,self.stack)
  for reg,value in registers.items():self.mu.reg_write(reg,value)
  self.mu.emu_start(start,stop,count=500000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Retail segment {start:x} instruction budget')
 def state(self):
  return {'displayed':self.get(self.hud+0x6ce8,'f')[0],'target':self.get(self.hud+0x6cec,'f')[0],
   'bossLife':self.get(self.hud+0x6cf0,'i')[0],'remainingSpells':self.get(self.hud+0x6cf4,'i')[0],
   'thresholds':[[self.get(self.hud+0x6cf8+i*8,'f')[0],self.get(self.hud+0x6cfc+i*8,'I')[0]] for i in range(4)],
   'nameHandle':self.get(self.hud+0x6c78,'I')[0],'starHandles':self.get(self.hud+0x6c7c,'10I'),
   'flags':self.get(self.hud+0x6d18,'I')[0]}
 def tick(self,**input):
  values={'stage':1,'checkpoint':24,'health':1000,'maxHealth':1000,'remainingSpells':3,
   'managerPresent':True,'bossPresent':True,'managerFlags':0,'dialogue':0,'player':[0,400],**input}
  self.put(0x4b0cb0,'i',values['stage']);self.put(0x4b0cb8,'i',values['checkpoint'])
  self.put(self.manager+0x1c,'I',self.enemy if values['bossPresent'] else 0)
  self.put(self.manager+0x3c,'I',values['managerFlags'])
  self.put(self.enemy+0x2648,'2i',values['health'],values['maxHealth'])
  self.put(self.hud+0x6cf4,'i',values['remainingSpells']);self.put(self.hud+0x6d30,'I',values['dialogue'])
  self.put(self.player+0x97c,'2f',*values['player'])
  self.events=[];self.rectangles=[];before=self.state()
  self.execute(0x41e439,0x41e727,{UC_X86_REG_EDI:self.hud,UC_X86_REG_ECX:self.manager if values['managerPresent'] else 0,
   UC_X86_REG_EDX:self.player,UC_X86_REG_EBX:0,UC_X86_REG_EAX:0})
  after=self.state();self.draw()
  return {'input':values,'before':before,**after,'events':list(self.events),'rectangles':list(self.rectangles)}
 def draw(self):
  self.rectangles=[]
  self.execute(0x41f33d,0x41f450,{UC_X86_REG_ESI:self.hud,UC_X86_REG_EBX:0,UC_X86_REG_EDI:0})

records=[]
def scenario(name,steps,displayed=0,thresholds=None):
 n=Native(displayed,thresholds);frames=[]
 for frame,values in enumerate(steps):frames.append({'frame':frame,**n.tick(**values)})
 records.append({'name':name,'frames':frames})
scenario('appearance-full-from-zero',[{} for _ in range(45)])
scenario('appearance-fraction-from-zero',[{'health':333} for _ in range(17)])
scenario('damage-immediate-and-recovery',[{'health':hp} for hp in [1000,1000,250,200,800,800]],.6)
scenario('zero-hp-keeps-live-boss-name',[{'health':0},{'health':100}],.5)
scenario('remaining-spells-rebind-and-outro',[{'remainingSpells':n} for n in [3,3,1,0,10,2]],.4)
for stage in range(1,8):
 for checkpoint in [23,24]:scenario(f'name-stage{stage}-checkpoint{checkpoint}',[{'stage':stage,'checkpoint':checkpoint}],.4)
thresholds=[[.15,0xffff2222],[.4,0xff22ff22],[.65,0xff2222ff],[.9,0xffdddd22]]
for name,hidden in [('no-manager',{'managerPresent':False}),('no-boss',{'bossPresent':False}),
 ('manager-hide-bit0',{'managerFlags':1}),('dialogue',{'dialogue':1})]:
 scenario('hidden-'+name,[{},hidden,{}],.6,thresholds)
scenario('upper-left-hysteresis',[{'player':p} for p in [[0,400],[-80,0],[-80,0],[80,0],[0,400],[-1,20],[0,20],[1,20]]],.7)

# Direct draw-only ratios retain the exact four threshold comparisons and
# colors from the retail code, independent of appearance interpolation.
for ratio in [0,.025,.15,.333,.4,.65,1]:
 n=Native(ratio,thresholds);n.draw()
 records.append({'name':f'draw-ratio-{ratio}','drawOnly':True,'frames':[{'frame':0,'input':{'displayed':ratio,'thresholds':thresholds},
  **n.state(),'events':[],'rectangles':list(n.rectangles)}]})

for r in records:
 for f in r['frames']:
  if not all(math.isfinite(f[k]) for k in ['displayed','target']):raise RuntimeError('Nonfinite original ratio')
  if not all(math.isfinite(x) for rectangle in f['rectangles'] for x in rectangle['bounds']):raise RuntimeError('Nonfinite original rectangle')
assert records[0]['frames'][0]['displayed']>0
assert records[0]['frames'][-1]['displayed']==1
assert records[2]['frames'][2]['displayed']==records[2]['frames'][2]['target']

out=ROOT/'tests/fixtures/boss-hud-original.json'
metadata={'schema':'th12-original-boss-hud/1','originalExeSha256':EXE_SHA256,
 'wholeGameOracle':False,'wholeHudOracle':False,'gpuExecuted':False,
 'provider':'Retail41e439..41e727 ratio/name/star tick and41f33d..41f450 HP rectangle construction; no arithmetic or branch hooks.',
 'entryRegisters':{'tick':{'EDI':'HUD','ECX':'EnemyManager or0','EDX':'Player'},'draw':{'ESI':'HUD'},'ESP':'mapped segment-local stack'},
 'fieldOffsets':{'displayed':'HUD+6ce8','target':'HUD+6cec','bossLife':'HUD+6cf0','remainingSpells':'HUD+6cf4',
  'thresholdPairs':'HUD+6cf8,+6d00,+6d08,+6d10','nameHandle':'HUD+6c78','starHandles':'HUD+6c7c..6ca0',
  'hudFlags':'HUD+6d18','dialogue':'HUD+6d30','stage':'global4b0cb0','checkpoint':'global4b0cb8',
  'bossPointer':'EnemyManager+1c','managerFlags':'EnemyManager+3c','healthMax':'Enemy+2648/+264c'},
 'rectangleLayout':['left','top','right','bottom'],
 'fixtureCallbacks':['4615a0 captures originalfront bank/script/layer/registration flags, writes deterministic pseudo handle and returns mapped dummy VM pointer; real ANM binding/tick not executed',
 '461970 captures queued IRQ code and pseudo handle; real lookup, children and ANM interrupt handling not executed',
 '451f30 captures originalEDI four-float rectangle andEBX ARGB without GPU submission'],
 'limitations':['Synthetic valid HUD/EnemyManager/Boss/Player objects and mapped stack locals; surrounding HUD callback and real application scheduler are not executed',
 'Checkpoint23/24 and remaining count0..10 are synthetic inputs; stage/checkpoint branches are original',
 'Returned handles are fixture identities, not native allocator values; interruption events with handle0 are requested calls, not a demonstrated live VM interruption',
 'No font/name texture, star ANM, blend-state, device, pixel output, full Boss removal or complete HUD lifecycle Oracle'],
 'records':records}
out.write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'scenarios':len(records),'frames':sum(len(r['frames']) for r in records),
 'rectangles':sum(len(f['rectangles']) for r in records for f in r['frames']),
 'names':[{ 'stage':f['input']['stage'],'checkpoint':f['input']['checkpoint'],
  'scripts':[e['script'] for e in f['events'] if e['kind']=='bind' and e['script']>=118]} for r in records if r['name'].startswith('name-') for f in r['frames']]}))
