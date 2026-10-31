"""Retail Enemy creation/registration/manager saved-successor experiments.

Reuse the read-only native provider bootstrap before its startup spawn. Add
small synthetic ECL routines to the lookup table; original decoding, creation,
full Enemy updates, intrusive linking and coroutine dispatch all execute.
"""
import argparse,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser();parser.add_argument('--output',type=pathlib.Path,default=ROOT/'tests/native/enemy-birth-order-original.json');args=parser.parse_args()
provider=ROOT/'scripts/reverse/native-enemy-provider.py'
source=provider.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]
def word(v):return struct.pack('<i',v)
def floating(v):return struct.pack('<f',v)
def string(v):
 raw=v.encode('ascii')+b'\0';raw+=bytes((-len(raw))%4);return word(len(raw))+raw
def ins(op,payload=(),time=0):
 raw=b''.join(payload);return struct.pack('<iHHHBBI',time,op,16+len(raw),0,63,len(payload),0)+raw
def spawn(name,time=0):return ins(257,[string(name),floating(0),floating(100),word(100),word(0),word(0)],time)
def hold(time=0):return ins(83,[word(100000)],time)
def pos(x):return ins(300,[floating(x),floating(100)])
def async_(name,time=0):return ins(15,[string(name)],time)
def scpt(routines):
 rows=list(routines.items());names=b''.join(n.encode('ascii')+b'\0' for n,_ in rows);start=(36+len(rows)*4+len(names)+3)&~3
 chunks=[b'ECLH'+bytes(12)+b''.join(code)+ins(10) for _,code in rows];out=bytearray(start+sum(map(len,chunks)));out[:4]=b'SCPT';struct.pack_into('<HH',out,4,1,0);struct.pack_into('<H',out,16,len(rows));out[36+len(rows)*4:36+len(rows)*4+len(names)]=names
 offset=start
 for i,chunk in enumerate(chunks):struct.pack_into('<I',out,36+i*4,offset);out[offset:offset+len(chunk)]=chunk;offset+=len(chunk)
 return bytes(out)
base_routines={
 'ProbeChild':[ins(304,[floating(0),floating(2)]),hold()],
 'ProbeSuccessor':[hold()],
 'ProbeTail':[spawn('ProbeChild',1),hold(1)],
 'ProbeCtorParent':[spawn('ProbeCtorChild'),hold()],
 'ProbeCtorChild':[spawn('ProbeChild'),hold()],
 'ProbeAsync':[async_('ProbeAsyncA'),async_('ProbeAsyncB'),hold()],
 'ProbeAsyncA':[pos(10),hold()],
 'ProbeAsyncB':[async_('ProbeAsyncC'),pos(20),hold()],
 'ProbeAsyncC':[pos(30),hold()],
 'ProbeAsyncLate':[async_('ProbeAsyncA',1),hold(1)],
 'ProbeAnimatedChild':[ins(258,[word(0)]),ins(259,[word(0),word(0)]),hold()],
 'ProbeAnimatedParent':[ins(258,[word(0)]),ins(259,[word(0),word(1)]),spawn('ProbeAnimatedChild'),hold()],
 'ProbeBackground':[ins(258,[word(0)]),ins(259,[word(0),word(0)]),ins(402,[word(143)]),hold()],
 'ProbeBackground270':[pos(80),ins(270,[string('ProbeBackground'),floating(5),floating(7),floating(30),word(1200),word(1000),word(2)]),hold()],
 'ProbeBackground271':[pos(80),ins(271,[string('ProbeBackground'),floating(5),floating(7),floating(30),word(1200),word(1000),word(2)]),hold()],
}
scenarios=[('empty-birth',['ProbeChild'],False),('tail-spawn',['ProbeTail'],True),('existing-successor',['ProbeTail','ProbeSuccessor'],True),
 ('constructor-recursion',['ProbeCtorParent'],False),('async-head-insertion',['ProbeAsync'],True),('async-tail-insertion',['ProbeAsyncLate'],True),
 ('direct-guard-reentry',['ProbeChild'],False),('retail-main-prime',['main'],False),('retail-main-unprimed',['main'],False),
 ('animated-constructor',['ProbeAnimatedParent'],True),('background270',['ProbeBackground270'],True),('background271',['ProbeBackground271'],True),('background271-boss-gate',['ProbeBackground271'],True)]
records=[]
for name,roots,prime in scenarios:
 saved_argv=sys.argv;sys.argv=[str(provider),'--frames','0'];ns={'__file__':str(provider),'__name__':'th12_native_birth_research'}
 try:exec(compile(source,str(provider),'exec'),ns)
 finally:sys.argv=saved_argv
 v,put,get,call=ns['v'],ns['put'],ns['get'],ns['call'];MANAGER=ns['MANAGER'];routines=ns['routines'];base=0x4300000
 raw=scpt(base_routines);v.mem_write(base,raw);count=struct.unpack_from('<H',raw,16)[0];names_at=36+count*4
 for i in range(count):
  end=raw.index(0,names_at);routine=raw[names_at:end].decode('ascii');routines[routine]=(base+names_at,base+struct.unpack_from('<I',raw,36+i*4)[0]);names_at=end+1
 lookup=ns['PROGRAM']+0x100;put(ns['PROGRAM']+8,'I',len(routines));put(ns['PROGRAM']+0x8c,'I',lookup)
 for i,(routine,(text,header)) in enumerate(sorted(routines.items())):put(lookup+i*8,'II',text,header)
 # Native manager timer initializer layout. Existing provider leaves pointer0;
 # it is immaterial to saved-successor traversal but tracked separately below.
 put(MANAGER+0x50,'iifII',-1,0,0.,0x4b2ed0,1)
 if name.startswith('background'):
  put(0x4ceaec,'ff',16.,-9.)
  if name.endswith('boss-gate'):put(MANAGER+0x1c,'I',1)
 from unicorn import UC_HOOK_CODE
 from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_ESI,UC_X86_REG_EAX
 phase='construct';events=[]
 def trace(v,address,size,data):
  if address==0x413840:
   state=get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0];events.append(dict(phase=phase,kind='attempt',enemy=hex(state-0x1040),flags=get(state+0x16b8,'I')[0]))
  elif address==0x413860:
   state=v.reg_read(UC_X86_REG_ESI);events.append(dict(phase=phase,kind='fullUpdate',enemy=hex(state-0x1040)))
  elif address==0x412b38:
   # EBP is the newly constructed Enemy. ID was assigned at412acb before link.
   from unicorn.x86_const import UC_X86_REG_EBP
   enemy=v.reg_read(UC_X86_REG_EBP);events.append(dict(phase=phase,kind='register',enemy=hex(enemy),id=get(enemy+0x27b4,'I')[0]))
 for address in [0x413840,0x413860,0x412b38]:v.hook_add(UC_HOOK_CODE,trace,begin=address,end=address)
 def location(pc):
  candidates=[(name,header) for name,(_,header) in routines.items() if header+16<=pc]
  if not pc or not candidates:return None
  name,header=max(candidates,key=lambda x:x[1]);return dict(routine=name,instructionOffset=pc-header-16)
 def capture():
  out=[];node=get(MANAGER+0x68,'I')[0]
  while node:
   enemy,next_node=get(node,'II');state=enemy+0x1040;contexts=[];thread=enemy+0x1030
   while thread:
    context=get(thread,'I')[0];contexts.append(dict(id=get(context+0x1020,'i')[0],location=location(get(context+4,'I')[0]),time=get(context,'f')[0]));thread=get(thread+4,'I')[0]
   out.append(dict(address=hex(enemy),id=get(enemy+0x27b4,'I')[0],routine=ns['births'][enemy]['routine'],position=get(state+0x34,'ff'),z=get(state+0x3c,'f')[0],age=get(state+0x270,'i')[0],hpImmunity=get(state+0x1694,'i')[0],flags=get(state+0x16b8,'I')[0],privateVariables=get(state+0x23c,'12I'),contexts=contexts));node=next_node
  return dict(chain=out,managerClock=get(MANAGER+0x50,'iif'))
 spawn_request=0x2070000;put(spawn_request,'fffiiiII',0,100,0,0,0,100,0,0);put(spawn_request+0x20,'12I',*range(1,13));frames=[];created=[]
 for routine in roots:created.append(call(0x412990,routines[routine][0],spawn_request))
 frames.append(dict(step='afterBirth',**capture()))
 if name=='direct-guard-reentry':
  phase='directReentry';returned=call(0x413840,created[0]+0x1040);frames.append(dict(step=phase,nativeReturn=returned,**capture()))
 elif name=='retail-main-prime':prime=True
 if prime:
  phase='prime';call(0x413190,MANAGER);frames.append(dict(step=phase,**capture()))
 for i in range(4):phase=f'pass{i}';call(0x413190,MANAGER);frames.append(dict(step=phase,**capture()))
 names_by_address={e['address']:e['routine'] for f in frames for e in f['chain']}
 for event in events:event['routine']=names_by_address.get(event['enemy'],ns['births'].get(int(event['enemy'],16),{}).get('routine'))
 records.append(dict(name=name,roots=roots,prime=prime,syntheticEclHex=raw.hex() if name.startswith('retail-') is False else None,frames=frames,events=events))
 assert all(e['routine'] is not None for e in events)
args.output.parent.mkdir(parents=True,exist_ok=True)
args.output.write_text(json.dumps(dict(schema='th12-enemy-birth-order-native/1',originalExeSha256=ns['SHA'],partial=True,wholeGameOracle=False,
 scope='real412990/413230/413840/413190/468d90 using synthetic ECL and original ANM; empty/tail/successor/recursive constructor/direct guard/async and retail startup',
 fixtures=['read-only native-enemy-provider bootstrap before startup spawn','synthetic ECL code lives in research memory only; original binary/resources unmodified','host malloc/free/file loader/audio plus original provider passive damage/body boundary','manager clock timer initialized explicitly to valid retail rate pointer; original provider leaves this pointerzero'],records=records),indent=2)+'\n')
print(json.dumps(dict(output=str(args.output),scenarios=len(records),updateAttempts=sum(sum(e['kind']=='attempt' for e in r['events']) for r in records),fullUpdates=sum(sum(e['kind']=='fullUpdate' for e in r['events']) for r in records)),indent=2))
