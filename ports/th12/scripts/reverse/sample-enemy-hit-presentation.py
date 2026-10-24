"""Original ECL453 and the Enemy4143b0..41457e damage-color/sound block.

Native constructors/binding execute; the hit flag, flash timer, remaining phase
HP and spell flags are explicit controlled inputs. The original drawing state
and453e20 request boundary are observed, not the GPU/audio device or hurt query.
"""
import json, pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT/'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]
sys.argv = [str(PROVIDER), '--frames', '1', '--seed', '1234']
ns = {'__file__':str(PROVIDER), '__name__':'native_enemy_hit_presentation'}
exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
v,put,get,call = (ns[n] for n in ['v','put','get','call'])
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *

def word(n): return struct.pack('<i',n)
def ins(op,args=()):
    raw=b''.join(args); return struct.pack('<iHHHBBI',0,op,len(raw)+16,0,63,len(args),0)+raw
def program(code):
    name=b'HitProbe\0'; start=(40+len(name)+3)&~3; body=b'ECLH'+bytes(12)+b''.join(code)
    raw=bytearray(start+len(body));raw[:4]=b'SCPT';struct.pack_into('<H',raw,4,1)
    struct.pack_into('<H',raw,16,1);struct.pack_into('<I',raw,36,start)
    raw[40:40+len(name)]=name;raw[start:]=body;return bytes(raw),start
base=0x4300000
code=[ins(258,[word(2)]),ins(259,[word(0),word(7)]),ins(83,[word(100000)])]
raw,header=program(code);v.mem_write(base,raw);ns['routines']['HitProbe']=(base+40,base+header)
put(ns['PROGRAM']+8,'I',len(ns['routines']))
for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):
    put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
request=0x2070000;put(request,'fffiiiII',25,100,0,0,0,1000,0,0)
enemy=call(0x412990,base+40,request);state=enemy+0x1040
handle=get(state+0xe0,'I')[0];v.reg_write(UC_X86_REG_EDX,get(0x4ce8cc,'I')[0])
vm=call(0x461920,handle)
if not vm: raise RuntimeError('Actual Enemy slot0 not bound')
sounds=[]
v.hook_add(UC_HOOK_CODE,lambda mu,address,size,data:mu.emu_stop(),
           begin=0x41457e,end=0x41457e)
# Observe EAX before the provider's silent audio return overwrites it. Keep
# the same declared device boundary, with its original RET4 cleanup.
del ns['external'][0x453e20]
def sound(mu,address,size,data):
    sounds.append({'id':mu.reg_read(UC_X86_REG_EAX),
                   'x':get(mu.reg_read(UC_X86_REG_ESP)+4,'f')[0]})
    ns['return_from_call'](4)
v.hook_add(UC_HOOK_CODE,sound,begin=0x453e20,end=0x453e20)
rows=[]
cases=[]
for flags in [0,0x100000,0x40000000,0x40000000|0x100000,
              0x20000000,0x20000000|0x100000,0x400000,0x400000|0x100000]:
    for phase_hp in [99,100,199,200,499,500,899,900]:
        for spell_flags in [0,1,9]:
            for age in [0,1]: cases.append((flags,phase_hp,spell_flags,age,0,-1))
for hit_sound in [-1,0,37]:
    for flash in [0,1,4]:
        for flags in [0,0x100000,0x40000000|0x100000]:
            cases.append((flags,90,1,0,flash,hit_sound))
for flags,phase_hp,spell_flags,age,flash,hit_sound in cases:
    put(state+0x16b8,'I',flags);put(state+0x1684,'i',flash)
    put(state+0x168c,'i',hit_sound);put(state+0x270,'i',age)
    put(enemy+0x2650,'i',phase_hp);put(ns['HUD']+0x7c,'I',spell_flags)
    put(vm+0x47c,'I',0x10001);put(vm+0x3c0,'I',0x12345678)
    put(ns['STAGE']+0x6c08,'I',0x10001);sounds.clear()
    # Original block receives the stack locals from its enclosing Enemy tick.
    put(ns['SP']+0xc,'I',state+0xe0);put(ns['SP']+0x10,'I',state+0x34)
    v.reg_write(UC_X86_REG_ESP,ns['SP']);v.reg_write(UC_X86_REG_EBX,state)
    v.emu_start(0x4143b0,ns['STOP'],count=100000)
    if v.reg_read(UC_X86_REG_EIP)!=0x41457e:raise RuntimeError('Original color block incomplete')
    rows.append({'initial':{'flags':flags,'phaseLife':phase_hp,'spellFlags':spell_flags,
                            'phaseAge':age,'flash':flash,'hitSound':hit_sound},
                 'after':{'flash':get(state+0x1684,'i')[0],
                          'vmFlags':get(vm+0x47c,'I')[0],
                          'secondaryColor':get(vm+0x3c0,'I')[0],
                          'stageVmFlags':get(ns['STAGE']+0x6c08,'I')[0],
                          'sounds':list(sounds)}})
out=ROOT/'tests/fixtures/enemy-hit-presentation-original.json'
out.write_text(json.dumps({'schema':'th12-original-enemy-hit-presentation/1',
    'originalExeSha256':ns['SHA'],'wholeGameOracle':False,'gpuExecuted':False,
    'boundary':'Actual Enemy/ANM birth followed by original4143b0..41457e block with controlled accepted-hit flags/timer/phase health. Sound453e20 is observed at request boundary; Enemy hurt query, full scheduler, GPU and audible output are outside this fixture.',
    'cases':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'cases':len(rows),'soundCases':sum(bool(r['after']['sounds']) for r in rows)}))
