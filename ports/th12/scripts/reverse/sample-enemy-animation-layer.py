"""Actual Enemy birth/ECL259/452 layer defaults, Primary head and drawing.

Uses the native provider's constructed manager/resource host bootstrap only;
executes original Enemy constructor, dispatcher, ANM factory/list/tick and
layer draw functions. GPU submission45c900 is recorded and returned. This is
partial CPU evidence, never an accepted replay or whole-render golden.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]
def word(v): return struct.pack('<i',v)
def ins(op,args=()):
    data=b''.join(args);return struct.pack('<iHHHBBI',0,op,len(data)+16,0,63,len(args),0)+data
def program(code):
    name=b'LayerProbe\0';start=(40+len(name)+3)&~3;body=b'ECLH'+bytes(12)+b''.join(code)
    raw=bytearray(start+len(body));raw[:4]=b'SCPT';struct.pack_into('<H',raw,4,1);struct.pack_into('<H',raw,16,1)
    struct.pack_into('<I',raw,36,start);raw[40:40+len(name)]=name;raw[start:]=body;return bytes(raw),start
cases=[]
for offset in [None,-8,-7,0,4,23,24]:
    saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','1234']
    ns={'__file__':str(PROVIDER),'__name__':'native_enemy_animation_layer'}
    try: exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
    finally: sys.argv=saved
    v,put,get,call=(ns[k] for k in ['v','put','get','call'])
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_EDI,UC_X86_REG_ESP
    anm=get(0x4ce8cc,'I')[0];draws=[];factory=[];registration=[]
    def capture_vm(pointer):
        bank=get(pointer+0x3f8,'I')[0]
        name=next((name for name,address in ns['resource_by_name'].items() if address==bank),None)
        return {'id':get(pointer,'I')[0],'asset':name,'script':get(pointer+0x3ea,'h')[0],
                'sprite':get(pointer+0x3e4,'h')[0],'layer':get(pointer+0x20,'i')[0],
                'flags':get(pointer+0x47c,'I')[0],'enginePosition':list(get(pointer+0x430,'3f'))}
    def hook(mu,address,size,unused):
        sp=mu.reg_read(UC_X86_REG_ESP)
        if address==0x41c6a0: factory.append({'requestedLayer':mu.reg_read(UC_X86_REG_EAX),'script':get(sp+8,'i')[0]})
        elif address in [0x461250,0x4612d0]: registration.append({'entry':hex(address),'vm':capture_vm(mu.reg_read(UC_X86_REG_EBX))})
        elif address==0x45c900:
            draws.append(capture_vm(mu.reg_read(UC_X86_REG_EAX)))
            target=get(sp,'I')[0];mu.reg_write(UC_X86_REG_ESP,sp+8);mu.reg_write(UC_X86_REG_EAX,1);mu.reg_write(UC_X86_REG_EIP,target)
    from unicorn.x86_const import UC_X86_REG_EIP
    for address in [0x41c6a0,0x461250,0x4612d0,0x45c900]:v.hook_add(UC_HOOK_CODE,hook,begin=address,end=address)
    code=[ins(258,[word(2)])]
    if offset is not None: code.append(ins(452,[word(offset)]))
    code += [ins(259,[word(0),word(5)]),ins(259,[word(1),word(15)]),
             ins(259,[word(2),word(16)]),ins(83,[word(100000)])]
    base=0x4300000;raw,header=program(code);v.mem_write(base,raw);ns['routines']['LayerProbe']=(base+40,base+header)
    put(ns['PROGRAM']+8,'I',len(ns['routines']))
    for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
    request=0x2070000;put(request,'fffiiiII',25,100,0,0,0,100,0,0)
    enemy=call(0x412990,base+40,request);state=enemy+0x1040;handles=list(get(state+0xe0,'3I'))
    def chain():
        pointer=get(anm+0x8856b8,'I')[0];rows=[]
        while pointer:
            vm,pointer=get(pointer,'II');rows.append(capture_vm(vm))
        return rows
    born=chain();before=len(draws);result=call(0x413220)
    empty_manager_draw={'returned':result,'submissionCount':len(draws)-before}
    v.reg_write(UC_X86_REG_EDI,anm);call(0x460fe0)
    # Only layers with actual scheduled4611d0 callbacks are invoked. Layers26
    # and27 are absent from the native scheduler, unlike the noop Enemy draw21.
    scheduled={0:4,1:6,2:8,3:9,4:11,5:13,6:16,7:17,8:18,9:19,10:20,11:23,
               12:25,13:26,14:28,15:30,16:34,17:36,18:38,19:41,20:42,
               21:50,22:51,23:52,24:63,25:64,28:14,29:39,30:66,31:65}
    for layer in sorted(set(n['layer'] for n in born),key=lambda n:scheduled.get(n,100)):
        if layer not in scheduled: continue
        v.reg_write(UC_X86_REG_EAX,layer);v.reg_write(UC_X86_REG_EDI,anm);call(0x4611d0)
    cases.append({'offset':offset,'syntheticEclHex':raw.hex(),'enemyLayerOffset':get(state+0x238,'i')[0],
                  'slotHandles':handles,'birthPrimaryChain':born,'factory':factory,
                  'registrations':registration,'enemyManagerDraw21':empty_manager_draw,'draws':draws})
output=ROOT/'tests/fixtures/enemy-animation-layer-native.json'
output.write_text(json.dumps({'schema':'th12-native-enemy-animation-layer/1','originalExeSha256':ns['SHA'],
    'stgenm01Sha256':hashlib.sha256((ROOT/'local/retail/stgenm01.anm').read_bytes()).hexdigest(),
    'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,
    'boundary':'Actual412990/413230 Enemy birth, ECL258/259/452,41c6a0/454d10/4612d0 registration,460fe0 Primary27 and4611d0 registered-layer draw. Constructed manager/resource host bootstrap, passive Player hurt/audio boundaries.45c900 is captured before GPU submission. Actual413220 scheduler draw21 is invoked separately and makes no submissions.',
    'cases':cases},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'cases':[{'offset':c['offset'],'slotLayers':[next(n['layer'] for n in c['birthPrimaryChain'] if n['id']==h) for h in c['slotHandles']],
     'primaryScripts':[n['script'] for n in c['birthPrimaryChain']],'managerDraw':c['enemyManagerDraw21'],'draws':[(d['script'],d['layer']) for d in c['draws']]} for c in cases]}))
