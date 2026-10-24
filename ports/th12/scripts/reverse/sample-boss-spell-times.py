"""Retail Boss countdown and spell-result font submissions, without GPU/GDI.

Runs the arithmetic and draw segments unchanged. Font4015c0 and direct
ANM45c900 only capture calls; controlled mapped objects provide their inputs.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))

class Native:
 def __init__(self):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32)
  self.mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);self.mu.mem_write(0x400000,pe.get_memory_mapped_image())
  self.mu.mem_map(0x1000000,0x10000);self.mu.mem_map(0x2000000,0x400000)
  self.stack,self.stop=0x100d000,0x100f000
  self.hud,self.font,self.enemy,self.manager,self.spell,self.vm=0x2000000,0x2020000,0x2040000,0x2050000,0x2060000,0x2070000
  self.calls=[];self.draws=[];self.lookup=True
  for address,value in [(0x4b43b8,self.font),(0x4b43e4,self.hud),(0x4b43dc,self.manager),(0x4b43cc,self.spell),(0x4ce8cc,0x2300000)]:self.put(address,'I',value)
  self.put(0x4b2ed0,'f',1);self.put(0x4ce568,'2I',0x1234,0);self.put(0x4ce560,'2I',0xbeef,0)
  self.put(self.font+0x18f80,'I',0xffffffff);self.put(self.font+0x18f84,'2f',1,1);self.put(self.font+0x18fa4,'2i',1,1)
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.reg_write(UC_X86_REG_MXCSR,0x1f80);self.mu.hook_add(UC_HOOK_CODE,self.hook)
 def put(self,p,fmt,*values):self.mu.mem_write(p,struct.pack('<'+fmt,*values))
 def read(self,p,fmt):return list(struct.unpack('<'+fmt,self.mu.mem_read(p,struct.calcsize('<'+fmt))))
 def string(self,p):return bytes(self.mu.mem_read(p,64)).split(b'\0')[0].decode('ascii')
 def ret(self,cleanup=0):
  sp=self.mu.reg_read(UC_X86_REG_ESP);self.mu.reg_write(UC_X86_REG_EIP,self.read(sp,'I')[0]);self.mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
 def hook(self,mu,address,size,data):
  sp=mu.reg_read(UC_X86_REG_ESP)
  if address==0x461920:mu.reg_write(UC_X86_REG_EAX,self.vm if self.lookup else 0);self.ret(4)
  elif address==0x461a70:self.ret(4)
  elif address==0x45c900:self.draws.append(hex(mu.reg_read(UC_X86_REG_EAX)-self.hud));self.ret(4)
  elif address==0x453d90:self.ret()
  elif address==0x4015c0:
   fmt=self.string(self.read(sp+4,'I')[0]);args=[] if '%' not in fmt else self.read(sp+8,'i')
   self.calls.append({'format':fmt,'arguments':args,'text':fmt%tuple(args),'font':self.read(self.font+0x18f98,'i')[0],
    'position':self.read(mu.reg_read(UC_X86_REG_EBX),'3f'),'scale':self.read(self.font+0x18f84,'2f'),
    'colorARGB':self.read(self.font+0x18f80,'I')[0],'alignment':self.read(self.font+0x18fa4,'2i'),'drawFlag':self.read(self.font+0x18f9c,'i')[0]});self.ret()
 def execute(self,start,stop,registers):
  self.mu.reg_write(UC_X86_REG_ESP,self.stack)
  for reg,value in registers.items():self.mu.reg_write(reg,value)
  self.mu.emu_start(start,stop,count=500000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'{start:x} native budget')
 def call(self,start,args=(),**registers):
  self.put(self.stack,'I'*(len(args)+1),self.stop,*args);self.mu.reg_write(UC_X86_REG_ESP,self.stack)
  for name,value in registers.items():self.mu.reg_write(globals()['UC_X86_REG_'+name],value)
  self.mu.emu_start(start,self.stop,count=500000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=self.stop:raise RuntimeError(f'{start:x} native call budget')
 def seeds(self):return self.read(0x4ce568,'2I')+self.read(0x4ce560,'2I')
 def pose(self,obj):
  return {'sprite':self.read(obj+0x3e4,'h')[0],'position':self.read(obj+0x424,'3f'),
   'scale':self.read(obj+0x40,'2f'),'primary':self.read(obj+0x3bc,'I')[0],
   'flags':self.read(obj+0x47c,'I')[0]}

countdowns=[]
for age,limit in [(0,2400),(1,2400),(58,2400),(59,2400),(60,2400),(2399,2400),(2400,2400),(0,6001),(0,6000)]:
 n=Native();n.put(n.enemy+0x2710,'i',limit);n.put(n.enemy+0x12b0,'i',age)
 n.execute(0x41a7d3,0x41a834,{UC_X86_REG_ECX:n.enemy,UC_X86_REG_ESI:0})
 seconds,hundredths=n.read(n.hud+0x6d38,'2i');n.put(n.manager+0x1c,'I',n.enemy);n.put(n.hud+0x4f0c,'I',0xffabcdef)
 n.put(n.stack+0x100,'I',n.hud);seeds=n.seeds()
 n.execute(0x41f795,0x41f88a,{UC_X86_REG_EBP:n.stack+0xf8,UC_X86_REG_EDI:0,UC_X86_REG_EBX:1})
 countdowns.append({'age':age,'limit':limit,'seconds':seconds,'hundredths':hundredths,'calls':n.calls,'embeddedDraws':n.draws,'rngUnchanged':seeds==n.seeds()})

gates=[]
for name,manager,boss,flags,dialogue,seconds in [('missing-manager',False,True,0,0,40),('missing-boss',True,False,0,0,40),('manager-hidden',True,True,1,0,40),('dialogue',True,True,0,1,40),('unset-timer',True,True,0,0,-1)]:
 n=Native();n.put(0x4b43dc,'I',n.manager if manager else 0);n.put(n.manager+0x1c,'I',n.enemy if boss else 0);n.put(n.manager+0x3c,'I',flags)
 n.put(n.hud+0x6d30,'i',dialogue);n.put(n.hud+0x6d38,'i',seconds);n.put(n.stack+0x100,'I',n.hud)
 n.execute(0x41f795,0x41f8c4,{UC_X86_REG_EBP:n.stack+0xf8,UC_X86_REG_EDI:0,UC_X86_REG_EBX:1})
 gates.append({'name':name,'calls':n.calls,'embeddedDraws':n.draws})

results=[]
for ticks,seconds,hundredths,alpha,valid in [(121,2,1,255,True),(3601,58,83,128,True),(60000,999,99,255,True),(121,2,1,0,True),(121,2,1,255,False)]:
 n=Native();encoded=((seconds+hundredths+22)*1000+(seconds+66)%1000)*100+(hundredths+33)%100
 n.put(n.spell+0x94,'i',ticks);n.put(n.spell+0xa8,'i',encoded+(1 if not valid else 0));n.put(n.hud+0x6cb8,'i',1);n.put(n.hud+0x6cb4,'I',42)
 n.put(n.vm+0x3bf,'B',alpha);n.put(n.stack+0x100,'I',n.hud);seeds=n.seeds()
 n.execute(0x41f11a,0x41f33d,{UC_X86_REG_ESI:n.hud,UC_X86_REG_EDI:n.font,UC_X86_REG_EBP:n.stack+0xf8,UC_X86_REG_EBX:0})
 results.append({'ticks':ticks,'seconds':seconds,'hundredths':hundredths,'encoded':encoded+(1 if not valid else 0),'valid':valid,'alpha':alpha,'calls':n.calls,'rngUnchanged':seeds==n.seeds()})

animations=[]
for script in [3,4,78]:
 n=Native();bank='front' if script==78 else 'ascii';resource=install_resource(n.mu,(ROOT/'local/retail'/f'{bank}.anm').read_bytes(),0x2100000)
 n.call(0x454d10,[n.vm,script],ECX=resource['resource']);frames=[]
 for tick in range(181):
  if tick:n.call(0x455630,[n.vm])
  if tick in [0,1,2,15,30,45,60,90,120,150,180]:frames.append({'tick':tick,**n.pose(n.vm)})
 animations.append({'bank':bank,'script':script,'frames':frames})

timerTracks=[]
for secondsTrack in [[40,39,39,10,9,9,5,4,4,3,0,44]]:
 n=Native();resource=install_resource(n.mu,(ROOT/'local/retail/ascii.anm').read_bytes(),0x2100000)
 for i in range(2):n.call(0x454d10,[n.hud+0x4b50+i*0x4b4,3+i],ECX=resource['resource'])
 n.put(n.manager+0x1c,'I',n.enemy);n.put(n.hud+0x6d40,'i',-1);frames=[]
 for tick,seconds in enumerate(secondsTrack):
  for i in range(2):n.call(0x455630,[n.hud+0x4b50+i*0x4b4])
  n.put(n.hud+0x6d38,'2i',seconds,17)
  n.execute(0x41e279,0x41e439,{UC_X86_REG_EDI:n.hud,UC_X86_REG_ECX:n.manager})
  frames.append({'tick':tick,'seconds':seconds,'digits':[n.pose(n.hud+0x4b50+i*0x4b4) for i in range(2)]})
 timerTracks.append({'frames':frames})
out=ROOT/'tests/fixtures/boss-spell-times-original.json'
out.write_text(json.dumps({'schema':'th12-original-boss-spell-times/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'gpuExecuted':False,
 'provider':'Actual41a7d3..41a834 countdown arithmetic;41f795..41f88a countdown draw;41f11a..41f33d result draw and41d0b0 decoder;retail454d10/455630 ascii3/4 front78.',
 'limits':['Mapped synthetic valid HUD/Enemy/Spell/Font objects; actual font4015c0 submissions are captured before glyph expansion.','45c900 captures two direct embedded VM draw identities; no GPU submission.','No full HUD callback, wall-clock measurement, replay scene or original whole-game scheduling is asserted.'],
 'countdowns':countdowns,'gates':gates,'results':results,'animations':animations,'timerTracks':timerTracks},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'countdowns':len(countdowns),'results':len(results)}))
