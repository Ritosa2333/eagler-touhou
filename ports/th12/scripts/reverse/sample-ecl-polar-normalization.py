"""Actual ECL81 angle normalization and polar-output capture.

Constructed context/Enemy from the explicit combat CPU host; all ECL operand
reads,4646e0 normalization,469270 FSINCOS/multiply/stores run in original EXE.
No game/resource/replay writes, candidate math or GPU execution.
"""
import json, math, pathlib, struct, types
ROOT=pathlib.Path(__file__).resolve().parents[2]
source=ROOT/'scripts/reverse/combat-probe.py'
namespace={'__file__':str(source),'__name__':'native_ecl_polar'}
exec(compile(source.read_text(encoding='utf-8').split('loss=[]')[0],str(source),'exec'),namespace)
n=types.SimpleNamespace(**namespace)
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX
F=lambda value:struct.unpack('<f',struct.pack('<f',value))[0]
def ins(op,args=(),mask=0):
 raw=struct.pack('<'+'I'*len(args),*args)
 return struct.pack('<iHHHBBI',0,op,16+len(raw),mask,63,len(args),0)+raw
cases=[('retail-MBoss-at2',-5.595968723297119,64),('ordinary',.7,64),
 ('zero',0,64),('positive-pi',F(math.pi),64),('negative-pi',-F(math.pi),64),
 ('pi-next-float',struct.unpack('<f',struct.pack('<I',0x40490fdc))[0],64),
 ('negative-pi-next-float',-struct.unpack('<f',struct.pack('<I',0x40490fdc))[0],64),
 ('positive-tau',F(math.tau),48),('negative-tau',-F(math.tau),48),
 ('positive-two-wraps',F(math.tau*2+.35),64),('negative-two-wraps',F(-math.tau*2-.35),64),
 ('negative-length',-5.595968723297119,-64),('zero-length',-5.595968723297119,0)]
rows=[];trace=[]
def hook(mu,address,size,data):
 if address==0x468880:trace.append({'entry':hex(address),'kind':'normalize'})
 elif address==0x469270:trace.append({'entry':hex(address),'kind':'polar'})
n.v.hook_add(UC_HOOK_CODE,hook,begin=0x468880,end=0x468880)
n.v.hook_add(UC_HOOK_CODE,hook,begin=0x469270,end=0x469270)
for name,angle,length in cases:
 n.reset();angle,length=F(angle),F(length);trace.clear()
 n.put(n.ES,'I',n.VT);n.put(n.VT,'I',0x4154d0);n.put(n.ES+0x174c,'I',n.RUNNER);n.put(n.RUNNER+4,'I',n.CTX)
 code=ins(81,[n.bits(0),n.bits(4),n.bits(angle),n.bits(length)],3)+ins(83,[n.bits(100000)])+ins(10)
 n.v.mem_write(n.CODE,code);n.put(n.CTX,'fI',0,n.CODE)
 n.put(n.CTX+0x1008,'I',256);n.put(n.CTX+0x1014,'I',n.ES);n.put(n.CTX+0x101c,'I',63)
 n.call(0x467170,[n.bits(1)],{UC_X86_REG_EAX:n.CTX})
 output=n.get(n.CTX+8,'2f');assert [r['kind'] for r in trace]==['normalize','polar']
 rows.append({'name':name,'input':{'angle':angle,'length':length},'output':output,'trace':trace.copy()})
out=ROOT/'tests/fixtures/ecl-polar-normalization-native.json'
out.write_text(json.dumps({'schema':'th12-native-ecl-polar-normalization/1','originalExeSha256':n.SHA,
 'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,
 'boundary':'Actual467170 ECL81,468880 normalize call,4646e0 and469270 using literal operands in constructed context. No math/operand/normalization helpers replaced. Original combat host presentation fixtures are installed but not invoked by this math-only code.','records':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(rows),'retail':rows[0]}))
