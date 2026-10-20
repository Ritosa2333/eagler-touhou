"""Actual ANM mode16 draws: retail UFO204/205 and token hint211.

Only D3D submission is intercepted. Retail bind/tick, vertex formation and
material execute in the pinned EXE. No game/RPY/assets are modified.
"""
import json,pathlib,struct,types
ROOT=pathlib.Path(__file__).resolve().parents[2]
source=ROOT/'scripts/reverse/sample-ufo-custom-draw.py'
namespace={'__file__':str(source),'__name__':'native_ufo_outline'}
exec(compile(source.read_text(encoding='utf-8').split('rows=[]')[0],str(source),'exec'),namespace)
module=types.SimpleNamespace(**namespace)
from native_anm_resource import install_resource
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EIP,UC_X86_REG_ESP
rows=[]
for script,interrupts,frame in [(204,[],60),(205,[],60),(211,[(1,7)],12),(211,[(1,8)],25),(211,[(1,9)],40),(211,[(1,10)],21)]:
 n=module.Native();n.mu.mem_map(0x4000000,0x800000)
 bank=0x4000000;white=0x4500000;font=0x4700000
 install_resource(n.mu,(ROOT/'local/retail/bullet.anm').read_bytes(),bank)
 install_resource(n.mu,(ROOT/'local/retail/ascii.anm').read_bytes(),white)
 n.put(0x4b43b8,'I',font);n.put(font+0x18fb4,'I',white)
 n.put(0x4b2ed0,'f',1);n.put(0x4ce568,'II',0x1234,0);n.put(0x4ce560,'II',0xbeef,0)
 def call(address,args=(),eax=0,ecx=0):
  n.put(module.SP,'I'*(len(args)+1),module.STOP,*args)
  n.mu.reg_write(UC_X86_REG_ESP,module.SP);n.mu.reg_write(UC_X86_REG_EAX,eax);n.mu.reg_write(UC_X86_REG_ECX,ecx)
  n.mu.emu_start(address,module.STOP,count=300000)
  if n.mu.reg_read(UC_X86_REG_EIP)!=module.STOP:raise RuntimeError('Original draw/tick incomplete')
 call(0x454d10,[module.VM,script],ecx=bank)
 for tick in range(1,frame+1):
  for when,irq in interrupts:
   if tick==when:n.put(module.VM+0x3c4,'h',irq)
  call(0x455630,[module.VM])
 for rotation,scale in [(None,None),(0.375,1.25)]:
  n.put(module.VM+0x430,'3f',264.25,144.125,0)
  if rotation is not None:n.put(module.VM+0x2c,'f',rotation)
  if scale is not None:n.put(module.VM+0x40,'2f',scale,9.0) # Native mode16 uses X scale only.
  state={'script':script,'frame':frame,'rotation':n.get(module.VM+0x2c,'f')[0],
    'radius':n.get(module.VM+0x58,'f')[0],'scale':n.get(module.VM+0x40,'2f'),
    'count':n.get(module.VM+0x3fc,'i')[0],'primary':n.get(module.VM+0x3bc,'I')[0],
    'layer':n.get(module.VM+0x20,'i')[0],'flags':n.get(module.VM+0x47c,'I')[0],
    'position':[264.25,144.125]}
  n.events.clear();before=n.get(0x4ce568,'II')+n.get(0x4ce560,'II')
  call(0x45c900,[module.MANAGER],eax=module.VM)
  after=n.get(0x4ce568,'II')+n.get(0x4ce560,'II')
  draws=[e for e in n.events if e['kind']=='draw-primitive-up']
  if len(draws)!=1 or draws[0]['primitive']!=3:raise RuntimeError('Mode16 did not submit one line strip')
  rows.append({'state':state,'vertices':draws[0]['vertices'],'primitive':draws[0]['primitive'],
    'primitiveCount':draws[0]['primitiveCount'],'rngUnchanged':before==after})
out=ROOT/'tests/fixtures/ufo-outline-draw-native.json'
out.write_text(json.dumps({'schema':'th12-native-ufo-outline-draw/1','originalExeSha256':module.EXE_SHA256,
 'gpuExecuted':False,'wholeGameOracle':False,'boundary':'Real retail454d10/455630 and45c900/45d710 with explicit engine position and optional rotation/X/Y scale; D3D calls only captured. Geometry uses unmodified ANM commands.','records':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(rows),'vertices':sum(len(r['vertices']) for r in rows)}))
