"""Pinned retail spell text submissions and GUI ANM poses; no GPU/GDI execution.

40def0 executes its native gates, font writes, coordinates and record lookup.
454d10/455630 execute unchanged retail ascii1/2 and text74 bytecode. 460800
and4606b0 execute title alignment and wrapper geometry, stopping at44e660.
"""
import hashlib,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);PE=pefile.PE(str(EXE))

class Native:
 def __init__(self):
  self.mu=Uc(UC_ARCH_X86,UC_MODE_32)
  self.mu.mem_map(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
  self.mu.mem_write(0x400000,PE.get_memory_mapped_image())
  self.mu.mem_map(0x1000000,0x10000);self.mu.mem_map(0x2000000,0x400000)
  self.stack,self.stop=0x100e000,0x100f000
  self.font,self.spell,self.stats,self.obj,self.manager,self.text=0x2000000,0x2020000,0x2030000,0x2260000,0x2300000,0x2310000
  self.calls=[];self.title_calls=[];self.lookup=True
  self.put(0x4b43b8,'I',self.font);self.put(0x4b451c,'I',self.stats)
  self.put(0x4ce8cc,'I',self.manager);self.put(0x4b2ed0,'f',1)
  self.put(0x4ce568,'2I',0x1234,0);self.put(0x4ce560,'2I',0xbeef,0)
  self.mu.reg_write(UC_X86_REG_FPCW,0x37f);self.mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
  self.mu.hook_add(UC_HOOK_CODE,self.hook)
 def put(self,p,fmt,*values):self.mu.mem_write(p,struct.pack('<'+fmt,*values))
 def read(self,p,fmt):return list(struct.unpack('<'+fmt,self.mu.mem_read(p,struct.calcsize('<'+fmt))))
 def cstring(self,p):
  data=bytearray()
  for _ in range(1024):
   value=self.mu.mem_read(p,1)[0];p+=1
   if not value:return bytes(data)
   data.append(value)
  raise ValueError('Original string exceeds fixture bound')
 def ret(self,cleanup=0):
  sp=self.mu.reg_read(UC_X86_REG_ESP)
  self.mu.reg_write(UC_X86_REG_EIP,self.read(sp,'I')[0]);self.mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
 def hook(self,mu,address,size,data):
  sp=mu.reg_read(UC_X86_REG_ESP)
  if address==0x461920:mu.reg_write(UC_X86_REG_EAX,self.obj if self.lookup else 0);self.ret(4)
  elif address==0x4015c0:
   fmt=self.cstring(self.read(sp+4,'I')[0]).decode('ascii')
   args=[] if fmt=='$' else self.read(sp+8,'2i' if fmt=='%.2d/%.2d' else 'i')
   self.calls.append({'format':fmt,'arguments':args,'text':fmt%tuple(args),
    'font':self.read(self.font+0x18f98,'i')[0],'position':self.read(mu.reg_read(UC_X86_REG_EBX),'3f'),
    'colorARGB':self.read(self.font+0x18f80,'I')[0],'scale':self.read(self.font+0x18f84,'2f'),
    'alignment':self.read(self.font+0x18fa4,'2i'),'drawFlag':self.read(self.font+0x18f9c,'i')[0]})
   self.ret()
  elif address==0x46cad8:
   # Fixture titles are literal CP932 strings without formatting directives.
   destination,source=self.read(sp+4,'2I');raw=self.cstring(source)
   mu.mem_write(destination,raw+b'\0');mu.reg_write(UC_X86_REG_EAX,len(raw));self.ret()
  elif address==0x44e660:
   args=self.read(sp+4,'6I')
   self.title_calls.append({'style':mu.reg_read(UC_X86_REG_EAX),'textHex':self.cstring(mu.reg_read(UC_X86_REG_ECX)).hex(),
    'offset':mu.reg_read(UC_X86_REG_EDX),'sourceRect':self.read(args[0],'4i'),'fontHeight':args[1],
    'foregroundColor':args[2],'outlineColor':args[3],'spacing':args[5]})
   self.ret(24)
 def call(self,address,args=(),**registers):
  self.put(self.stack,'I'*(len(args)+1),self.stop,*args)
  self.mu.reg_write(UC_X86_REG_ESP,self.stack)
  for name,value in registers.items():self.mu.reg_write(globals()['UC_X86_REG_'+name],value)
  self.mu.emu_start(address,self.stop,count=300000)
  if self.mu.reg_read(UC_X86_REG_EIP)!=self.stop:raise RuntimeError(f'Native {address:x} budget')
 def font_defaults(self):
  # Actual constructor tail, avoiding allocation/unused VM constructor calls.
  self.mu.reg_write(UC_X86_REG_ESP,self.stack);self.mu.reg_write(UC_X86_REG_ESI,self.font)
  self.mu.emu_start(0x401026,0x401070,count=1000)
 def seeds(self):return self.read(0x4ce568,'2I')+self.read(0x4ce560,'2I')
 def pose(self):
  flags=self.read(self.obj+0x47c,'I')[0]
  return {'position':self.read(self.obj+0x424,'3f'),'scale':self.read(self.obj+0x40,'2f'),
   'primary':self.read(self.obj+0x3bc,'I')[0],'sprite':self.read(self.obj+0x3e4,'h')[0],
   'layer':self.read(self.obj+0x20,'i')[0],'flags':flags,'visible':flags&3==3,
   'ended':self.read(self.obj+0x3f0,'I')[0]==0}

text_records=[]
cases=[('active-full',3,4000000,[12,3,88,77],255,True),('active-short',3,123457,[1,0,88,77],255,True),
 ('failed',1,4000000,[12,3,88,77],255,True),('inactive',0,4000000,[12,3,88,77],255,True),
 ('no-history-handle',3,4000000,[12,3,88,77],255,False),('zero-alpha',3,4000000,[12,3,88,77],0,True),
 ('dim-alpha',7,4000000,[12,3,88,77],32,True),('partial-alpha',3,4000000,[12,3,88,77],128,True),
 ('large-history',3,1,[99999,99998,111,22],255,True),('zero-bonus',3,0,[0,0,88,77],255,True),
 ('negative-bonus',3,-123457,[7,2,88,77],255,True),('loadout-selection',3,90000000,[31,13,99,66],255,True)]
for name,flags,bonus,records,alpha,lookup in cases:
 n=Native();n.font_defaults();n.lookup=lookup;n.put(n.spell+0x1c,'I',42)
 n.put(n.spell+0x78,'2i',3,flags);n.put(n.spell+0x80,'i',bonus);n.put(n.obj+0x3bf,'B',alpha)
 player,subtype=(2,1) if name=='loadout-selection' else (0,0)
 n.put(0x4b0c90,'i',player);n.put(0x4b0c94,'i',subtype)
 record=n.stats+0x66c+(player*2+subtype)*0x45f4+3*0x90
 n.put(record+0x80,'2i',records[1],records[0]);n.put(n.stats+0x1aa24+3*0x90+0x80,'2i',records[3],records[2])
 before=n.seeds();n.call(0x40def0,EDI=n.spell);assert before==n.seeds()
 text_records.append({'name':name,'input':{'flags':flags,'bonusDisplayed':bonus,'records':records,'historyAlpha':alpha,
  'historyHandlePresent':lookup,'player':player,'subtype':subtype},'calls':n.calls,'rngUnchanged':True})

animations=[]
for bank,script in [('ascii',1),('ascii',2),('text',74)]:
 for ending in [None,3,2,1]:
  n=Native();raw=(ROOT/'local/retail'/f'{bank}.anm').read_bytes()
  resource=install_resource(n.mu,raw,0x2100000)
  n.call(0x454d10,[n.obj,script],ECX=resource['resource']);frames=[]
  for tick in range(166):
   if tick==130 and ending is not None:n.put(n.obj+0x3c4,'h',ending)
   if tick:n.call(0x455630,[n.obj])
   if tick in [0,1,15,30,59,60,75,89,90,91,105,120,129,130,131,134,138,145,159,160,165]:
    frames.append({'tick':tick,**n.pose()})
  animations.append({'bank':bank,'script':script,'interrupt':ending,'interruptTick':130,
   'resourceSha256':hashlib.sha256(raw).hexdigest(),'frames':frames})

titles=[]
for title in ['星蓮船','宝塔「レイディアントトレジャー」','ABCD','空']:
 n=Native();resource=install_resource(n.mu,(ROOT/'local/retail/text.anm').read_bytes(),0x2100000)
 n.call(0x454d10,[n.obj,74],ECX=resource['resource'])
 record=n.read(n.obj+0x3f4,'I')[0];n.put(record+8,'I',0x2320000);n.put(0x2320000,'I',0x2321000)
 raw=title.encode('cp932');n.mu.mem_write(n.text,raw+b'\0');before=n.seeds()
 n.call(0x460800,[0xffffff,0,0,0,n.text],ESI=n.obj);assert before==n.seeds()
 titles.append({'title':title,'titleHex':raw.hex(),'calls':n.title_calls,'rngUnchanged':True})

out=ROOT/'tests/fixtures/spell-presentation-original.json'
out.write_text(json.dumps({'schema':'th12-original-spell-presentation/1','originalExeSha256':EXE_SHA256,
 'provider':'Actual40def0,454d10/455630 and460800->4606b0; retail resources, synthetic mapped Spell/Font/Stats objects.',
 'wholeGameOracle':False,'wholePresentationOracle':False,'gdiExecuted':False,'gpuExecuted':False,
 'fixtureCallbacks':['461920 returns the fixture history VM or0; no actual registration chain lookup.',
  '4015c0 captures native format, arguments, coordinates and FontManager fields; formatting is Python; glyph submission not executed.',
  '46cad8 copies fixture literal CP932 title to formatter output; CRT format/locale startup excluded.',
  '44e660 captures GDI wrapper arguments; GDI rasterization and D3D upload excluded.'],
 'limits':['Only three root ANM scripts, not the full ANM scheduler.','Display uses current loadout capture/attempt counters; persistent save history is not loaded by this probe.',
  'Title source extent666x36 follows44e85b..44e882; actual font/outline pixels remain unverified.'],
 'fontDefaultEntry':'401026..401070 native constructor tail; white,scale1,align1',
 'drawCallback':{'entry':'40e050->40def0','priority':10,'bonusPosition':[261,51,0],'failurePosition':[275,51,0],
  'historyPosition':[356,51,0],'format':['%8d','$','%.2d/%.2d'],'alphaSource':'ascii2 primaryARGB high byte'},
 'text':text_records,'animations':animations,'titles':titles},indent=2,ensure_ascii=False)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'textCases':len(text_records),'animationTracks':len(animations),
 'animationFrames':sum(len(r['frames']) for r in animations),'titleCases':len(titles)}))
