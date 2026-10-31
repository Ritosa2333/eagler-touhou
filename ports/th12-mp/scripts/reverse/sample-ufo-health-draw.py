"""Original front136/137 solid-rectangle vertex audit, no GPU/game ticks.

Retail bind runs unchanged. The child factory only writes an inert handle;
137 is separately bound and given its explicit parent for geometry capture.
"""
import json, pathlib, struct, types
ROOT = pathlib.Path(__file__).resolve().parents[2]
source = ROOT / 'scripts/reverse/sample-ufo-custom-draw.py'
namespace = {'__file__': str(source), '__name__': 'native_ufo_health'}
exec(compile(source.read_text(encoding='utf-8').split('rows=[]')[0], str(source), 'exec'), namespace)
module = types.SimpleNamespace(**namespace)
from native_anm_resource import install_resource
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP

class HealthNative(module.Native):
 def hook(self, mu, at, size, user):
  if self.stubs[at][0] != 'draw-primitive-up': return super().hook(mu, at, size, user)
  sp = mu.reg_read(UC_X86_REG_ESP)
  device, primitive, count, pointer, stride = self.get(sp+4, '5I')
  assert primitive == 5 and count == 2 and stride == 20
  vertices = [[*self.get(pointer+i*stride, '4f'), self.get(pointer+i*stride+16, 'I')[0]] for i in range(4)]
  self.events.append({'kind':'draw-primitive-up', 'primitive':primitive, 'primitiveCount':count, 'vertices':vertices})
  mu.reg_write(UC_X86_REG_EIP, self.get(sp, 'I')[0]); mu.reg_write(UC_X86_REG_ESP, sp+24); mu.reg_write(UC_X86_REG_EAX, 0)

rows = []
for health in [1.0, .5]:
 n = HealthNative(); n.mu.mem_map(0x4000000, 0x800000)
 bank, white, font, child = 0x4000000, 0x4500000, 0x4700000, module.VM+0x500
 install_resource(n.mu, (ROOT/'local/retail/front.anm').read_bytes(), bank)
 install_resource(n.mu, (ROOT/'local/retail/ascii.anm').read_bytes(), white)
 n.put(0x4b43b8, 'I', font); n.put(font+0x18fb4, 'I', white); n.put(0x4b2ed0, 'f', 1)
 n.put(0x4ce568, 'II', 0x1234, 0); n.put(0x4ce560, 'II', 0xbeef, 0)
 def child_factory(mu, at, size, user):
  sp=mu.reg_read(UC_X86_REG_ESP); handle, script, parent=n.get(sp+4,'3I')
  assert script == 137 and parent == module.VM
  n.put(handle,'I',0); mu.reg_write(UC_X86_REG_EAX,0)
  mu.reg_write(UC_X86_REG_EIP,n.get(sp,'I')[0]); mu.reg_write(UC_X86_REG_ESP,sp+16)
 n.mu.hook_add(UC_HOOK_CODE, child_factory, begin=0x461720, end=0x461720)
 def call(at,args=(),eax=0,ecx=0):
  n.put(module.SP,'I'*(len(args)+1),module.STOP,*args)
  n.mu.reg_write(UC_X86_REG_ESP,module.SP); n.mu.reg_write(UC_X86_REG_EAX,eax); n.mu.reg_write(UC_X86_REG_ECX,ecx)
  n.mu.emu_start(at,module.STOP,count=300000)
  if n.mu.reg_read(UC_X86_REG_EIP)!=module.STOP: raise RuntimeError('Original draw/bind incomplete')
 call(0x454d10,[module.VM,136],ecx=bank)
 call(0x454d10,[child,137],ecx=bank)
 n.put(module.VM+0x430,'3f',264.25,144.125,0); n.put(module.VM+0x40,'f',health)
 n.put(child+0x3c,'I',module.VM)
 for vm,script in [(module.VM,136),(child,137)]:
  state={'script':script,'health':health,'position':n.get(vm+0x424,'3f'),'engine':n.get(vm+0x430,'3f'),
   'offset':n.get(vm+0x43c,'3f'),'size':n.get(vm+0x58,'2f'),'scale':n.get(vm+0x40,'2f'),
   'rotation':n.get(vm+0x2c,'f')[0],'colorARGB':n.get(vm+0x3bc,'I')[0],'layer':n.get(vm+0x20,'i')[0],
   'parent':None if script==136 else {'position':[0,-28,0],'engine':[264.25,144.125,0],'scale':[health,1],'rotation':0}}
  n.events.clear(); before=n.get(0x4ce568,'II')+n.get(0x4ce560,'II')
  call(0x45c900,[module.MANAGER],eax=vm)
  draws=[e for e in n.events if e['kind']=='draw-primitive-up']; assert len(draws)==1
  after=n.get(0x4ce568,'II')+n.get(0x4ce560,'II')
  rows.append({'state':state,'vertices':draws[0]['vertices'],'rngUnchanged':before==after})
out=ROOT/'tests/fixtures/ufo-health-draw-native.json'
out.write_text(json.dumps({'schema':'th12-native-ufo-health-draw/1','originalExeSha256':module.EXE_SHA256,
 'gpuExecuted':False,'wholeGameOracle':False,'boundary':'Retail454d10 front136/137 then actual45c900/45ce60.137 factory writes inert handle; independent137 bind has explicitly assigned parent pointer. D3D only captured. No textures/game ticks executed.','records':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(rows),'vertices':sum(len(r['vertices']) for r in rows)}))
