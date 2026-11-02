"""Actual ECL603/512/513/445 phase clear, original factories and cancel ANM.

Constructed Enemy/manager/resource host and passive Player hurt/audio; item
requests are recorded at4273f0 rather than running an Item manager. No laser,
ECL, ANM, math or RNG instruction is replaced. No replay/GPU acceptance.
"""
import hashlib, json, math, pathlib, struct, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP=PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]
def word(v): return struct.pack('<i',v)
def floating(v): return struct.pack('<f',v)
def ins(op,args=(),time=0):
    raw=b''.join(args);return struct.pack('<iHHHBBI',time,op,len(raw)+16,0,63,len(args),0)+raw
def program(code):
    name=b'ClearProbe\0';start=(40+len(name)+3)&~3;body=b'ECLH'+bytes(12)+b''.join(code)
    raw=bytearray(start+len(body));raw[:4]=b'SCPT';struct.pack_into('<H',raw,4,1);struct.pack_into('<H',raw,16,1)
    struct.pack_into('<I',raw,36,start);raw[40:40+len(name)]=name;raw[start:]=body;return bytes(raw),start
records=[]
for name,commands,protected in [
    ('phase-clear-after-circle',[512,445],False),
    ('phase-clear-only',[445],False),
    ('phase-clear-repeat',[445,445],False),
    ('circle-only',[512],False),
    ('protected-circle',[512],True),
    ('protected-no-items-circle',[513],True),
    ('protected-phase-clear',[445],True),
]:
    saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','4660']
    ns={'__file__':str(PROVIDER),'__name__':'native_laser_phase_clear'}
    try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
    finally:sys.argv=saved
    v,put,get,call=(ns[k] for k in ['v','put','get','call'])
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDI,UC_X86_REG_ESP
    lm=call(0x4281c0);events=[]
    def snapshot_lasers():
        node=get(lm+0x18,'I')[0];rows=[]
        while node:
            rows.append({'kind':{0x4a0654:0,0x4a0704:1,0x4a06ac:2}[get(node,'I')[0]],
                         'phase':get(node+0xc,'i')[0],'position':list(get(node+0x50,'3f')),
                         'geometry':list(get(node+0x68,'4f')),'protection':get(node+0x44c,'i')[0],
                         'offscreenGrace':list(get(node+0x438,'iif'))})
            node=get(node+8,'I')[0]
        return rows
    def hook(mu,address,size,unused):
        sp=mu.reg_read(UC_X86_REG_ESP)
        if address in [0x428670,0x4286f0]:
            events.append({'entry':hex(address),'caller':hex(get(sp,'I')[0]),
                           'arguments':list(get(sp+4,'fii')) if address==0x4286f0 else [],
                           'lasers':snapshot_lasers()})
        elif address in [0x42a800,0x42bd90]:
            events.append({'entry':hex(address),'caller':hex(get(sp,'I')[0]),
                           'arguments':list(get(sp+4,'ii'))})
    for address in [0x428670,0x4286f0,0x42a800,0x42bd90]:v.hook_add(UC_HOOK_CODE,hook,begin=address,end=address)
    code=[ins(300,[floating(94.4),floating(144)]),ins(500,[word(0)]),
          ins(502,[word(0),word(7),word(6)]),ins(505,[word(0),floating(8),floating(8)]),
          ins(600,[word(0),floating(480),floating(480),floating(0),floating(16)]),
          ins(601,[word(0),word(20),word(8),word(140),word(10),word(9)]),
          ins(508,[word(0),word(-1),word(-1)])]
    if protected:code.append(ins(509,[word(0),word(0),word(0),word(0x200),word(10),word(-999999),floating(-999999),floating(-999999)]))
    for angle,identifier in [(0,1),(math.pi,2)]:
        code += [ins(504,[word(0),floating(angle),floating(0)]),ins(603,[word(0),word(identifier)])]
    code += [ins(op,[floating(16)] if op in [512,513] else [],1 if protected else 0) for op in commands]
    code.append(ins(83,[word(100000)],1 if protected else 0))
    raw,header=program(code);base=0x4300000;v.mem_write(base,raw);ns['routines']['ClearProbe']=(base+40,base+header)
    put(ns['PROGRAM']+8,'I',len(ns['routines']))
    for index,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+index*8,'II',text,routine)
    put(0x4ce568,'H',0x1234);put(0x4ce56c,'I',0)
    start_events=len(ns['events']);request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
    enemy=call(0x412990,base+40,request)
    if protected:
        # Input0 clears Enemy's actual constructor guard, and the original
        # Laser20 update processes ETEX200 to establish cancellation protection.
        # On input1 ECL's delayed clear executes before the next Laser20 visit.
        call(0x413190,ns['MANAGER']);v.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
        call(0x413190,ns['MANAGER'])
    items=[e for e in ns['events'][start_events:] if e.get('kind')=='itemSpawn']
    state={'rng':list(get(0x4ce568,'II')),'lasers':snapshot_lasers()}
    anm=get(0x4ce8cc,'I')[0];pointer=get(anm+0x8856b8,'I')[0];effects=[]
    while pointer:
        vm,pointer=get(pointer,'II')
        effects.append({'script':get(vm+0x3ea,'h')[0],'layer':get(vm+0x20,'i')[0],
                        'sprite':get(vm+0x3e4,'h')[0],'flags':get(vm+0x47c,'I')[0],
                        'rotation':list(get(vm+0x24,'3f')),
                        'enginePosition':list(get(vm+0x430,'3f'))})
    final=state
    if protected:
        v.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
        v.reg_write(UC_X86_REG_EDI,anm);call(0x460fe0)
        final={'rng':list(get(0x4ce568,'II')),'lasers':snapshot_lasers()}
    records.append({'name':name,'commands':commands,'protectedExtension':protected,'inputTicksBeforeCapture':2 if protected else 0,'syntheticEclHex':raw.hex(),
                    'events':events,'itemSpawns':items,'effects':effects,'birth':state,'final':final})
output=ROOT/'tests/fixtures/laser-phase-clear-native.json'
output.write_text(json.dumps({'schema':'th12-original-laser-phase-clear/1','originalExeSha256':ns['SHA'],
    'bulletAnmSha256':hashlib.sha256((ROOT/'local/retail/bullet.anm').read_bytes()).hexdigest(),
    'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,
    'boundary':'Actual Enemy412990/ECL300,603,512,513,445 dispatch,428670/4286f0 and laser virtual clearing/factories, original bullet ANM and RNG. Constructed manager/resource host; passive Player hurt/audio, item requests captured4273f0 without Item allocation. Protected variants use actual ETEX200/Laser20 before a delayed time1 ECL clear. Captures before the next Laser20 visit or registered ANM Primary27/Item22 updates. No game math/RNG/helper replaced.',
    'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'cases':[{'name':r['name'],'items':len(r['itemSpawns']),'rng':r['birth']['rng'],
     'effects':len(r['effects']),'laserKinds':[n['kind'] for n in r['birth']['lasers']]} for r in records]}))
