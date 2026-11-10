"""Original Player16 entry clear against real bullet/laser/source entities.

Synthetic SCPT and state ages are declared initial conditions. Entity factories,
ETEX protection, Player16, Enemy18, Laser20, Bullet21 and Primary27 run retail
instructions. No clear, collision, ANM, effect or RNG callback is replaced.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP=PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
def word(x):return struct.pack('<i',x)
def floating(x):return struct.pack('<f',x)
def ins(op,args=()):
 raw=b''.join(args);return struct.pack('<iHHHBBI',0,op,len(raw)+16,0,63,len(args),0)+raw
def ecl_program():
 code=[]
 # Stationary bullets at center, inner/outer-only positions, and far away.
 for x,y,protected in [(20,300,False),(70,300,False),(120,300,False),(-160,50,False),
                        (20,300,True),(40,300,True),(70,300,True),(120,300,True)]:
  code += [ins(300,[floating(x),floating(y)]),ins(500,[word(0)]),
    ins(502,[word(0),word(7),word(0)]),ins(505,[word(0),floating(0),floating(0)]),
    ins(504,[word(0),floating(0),floating(0)]),ins(507,[word(0),word(1)]),
    ins(508,[word(0),word(-1),word(-1)])]
  if protected:code.append(ins(509,[word(0),word(0),word(0),word(0x200),word(100),word(-999999),floating(-999999),floating(-999999)]))
  code.append(ins(501,[word(0)]))
 for kind in range(3):
  for x,protected in [(20,False),(20,True),(160,True)]:
   code += [ins(300,[floating(x),floating(300)]),ins(500,[word(0)]),
     ins(502,[word(0),word(7),word(0)]),ins(505,[word(0),floating(0),floating(0)]),
     ins(504,[word(0),floating(0),floating(0)]),
     ins(600,[word(0),floating(96),floating(96),floating(1000),floating(16)]),
     # Curve shares the first timing word as its history capacity; zero is
     # invalid for its real update. Retain the established8-node factory case.
     ins(601,[word(0),word(8),word(8),word(140),word(10),word(9)]),
     ins(508,[word(0),word(-1),word(-1)])]
   if protected:code.append(ins(509,[word(0),word(0),word(0),word(0x200),word(100),word(-999999),floating(-999999),floating(-999999)]))
   code.append(ins([602,603,611][kind],[word(0),word(1)] if kind==1 else [word(0)]))
 code.append(ins(83,[word(100000)]))
 target=[ins(415,[word(0)]),ins(400,[floating(24),floating(24)]),ins(83,[word(100000)])]
 names=['main','Target'];bodies=[b'ECLH'+bytes(12)+b''.join(code),b'ECLH'+bytes(12)+b''.join(target)]
 data=bytearray(44);data[:4]=b'SCPT';struct.pack_into('<H',data,4,1);struct.pack_into('<H',data,16,2)
 name_offsets=[]
 for name in names:name_offsets.append(len(data));data+=name.encode()+b'\0'
 while len(data)%4:data+=b'\0'
 headers=[]
 for body in bodies:headers.append(len(data));data+=body
 for i,h in enumerate(headers):struct.pack_into('<I',data,36+4*i,h)
 return bytes(data),names,name_offsets,headers
raw,names,name_offsets,headers=ecl_program()
records=[]
for state,age in [(0,0),(0,7),(0,29),(0,30),(0,60),(1,0),(1,29),(1,30),(2,30)]:
 source=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-marisa-a.json').read_text())
 source['initial'].update({'x':-160*128,'y':32*128})
 fixture_path=ROOT/'artifacts/reverse/player-clear-world-input.json';fixture_path.write_text(json.dumps(source),encoding='utf-8')
 saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets',
  '--native-lasers','--active-collision','--instruction-budget','20000000','--input',str(fixture_path),
  '--output',str(ROOT/'artifacts/reverse/player-clear-world-bootstrap.json')]
 ns={'__file__':str(PROVIDER),'__name__':'original_player_clear_world'}
 try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
 finally:sys.argv=saved
 from unicorn import UC_HOOK_CODE
 from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_ESP
 mu,put,get,call=(ns[k] for k in ['v','put','get','call'])
 player,lm,anm=ns['PLAYER'],ns['laser_manager'],0x5000000
 base=0x4300000;mu.mem_write(base,raw)
 for name,text,header in zip(names,name_offsets,headers):ns['routines'][name]=(base+text,base+header)
 put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
 request=0x2070000;put(request,'fffiiiII',0,0,0,0,0,10000,0,0)
 call(0x412990,base+name_offsets[0],request)
 put(request,'fffiiiII',20,300,0,0,0,1000,0,0)
 target=call(0x412990,base+name_offsets[1],request)
 call(0x413190,ns['MANAGER']) # Real first encounter clears constructor guards.
 p=get(lm+0x18,'I')[0];debug_nodes=[]
 while p:
  debug_nodes.append([hex(p),hex(get(p,'I')[0]),hex(get(p+8,'I')[0])]);p=get(p+8,'I')[0]
  if len(debug_nodes)>20:raise RuntimeError('Invalid diagnostic laser list')
 print(json.dumps({'state':state,'age':age,'beforeWarmLasers':debug_nodes}),flush=True)
 for tick in range(8):
  mu.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
  mu.reg_write(UC_X86_REG_ESI,ns['BULLETS']);call(0x40a020)
  mu.reg_write(UC_X86_REG_EDI,anm);call(0x460fe0)
 # Establish saved death origin via the original state2 transition. For the
 # state2 case, delay this callback until the actual observed Player pass.
 put(player+0x97c,'3f',20,300,0);put(player+0x988,'2i',20*128,300*128)
 if state!=2:
  put(player+0xa28,'i',2);mu.reg_write(UC_X86_REG_EAX,player+0xa30);call(0x4067e0,30)
  put(0x4d49d0,'I',0);call(0x436ba0,player)
 put(player+0xa28,'i',state);mu.reg_write(UC_X86_REG_EAX,player+0xa30);call(0x4067e0,age)
 if state==1:put(player+0x97c,'3f',0,400,0);put(player+0x988,'2i',0,400*128)
 put(0x4d49d0,'I',0)
 def snapshot():
  bullets=[]
  for slot in range(8):
   p=ns['BULLETS']+0x64+slot*0x9f8
   if get(p+0x532,'H')[0]:bullets.append({'slot':slot,'phase':get(p+0x532,'H')[0],
    'position':list(get(p+0x4bc,'2f')),'age':get(p+0x4e8,'i')[0],
    'protection':get(p+4,'i')[0],'animation':[get(p+8+0x3ea,'H')[0],get(p+8+0x6c,'i')[0],get(p+8+0x3c4,'H')[0]]})
  lasers=[];p=get(lm+0x18,'I')[0]
  while p:
   lasers.append({'kind':{0x4a0654:0,0x4a0704:1,0x4a06ac:2}[get(p,'I')[0]],
    'phase':get(p+0xc,'i')[0],'position':list(get(p+0x50,'2f')),
    'geometry':list(get(p+0x68,'4f')),'headOffset':get(p+0x78,'f')[0],
    'protection':get(p+0x44c,'i')[0],'retirePending':get(p+0x7c,'B')[0]});p=get(p+8,'I')[0]
  effects=[];p=get(anm+0x8856b8,'I')[0]
  while p:
   vm,p=get(p,'II');script=get(vm+0x3ea,'H')[0]
   if get(vm+0x10,'I')[0]==ns['resource_by_name']['bullet.anm'] and script==4:
    effects.append({'script':script,'clock':get(vm+0x6c,'i')[0],
      'layer':get(vm+0x20,'i')[0],'engine':list(get(vm+0x430,'3f')),
      'position':list(get(vm+0x424,'3f')),'rotation':list(get(vm+0x24,'3f')),
      'scale':list(get(vm+0x40,'2f')),'primary':get(vm+0x3bc,'I')[0]})
  sources=[]
  for slot in range(128):
   p=player+0x8988+slot*0x74
   if get(p+112,'I')[0]&1:sources.append([*get(p+24,'2f'),get(p,'f')[0],get(p+4,'f')[0],
      get(p+76,'i')[0],get(p+80,'i')[0],get(p+96,'i')[0],get(p+112,'I')[0]])
  return {'rng':list(get(0x4ce568,'II')),'bullets':bullets,'lasers':lasers,'effects':effects,
    'sources':sources,'targetLife':get(target+0x2648,'i')[0]}
 before=snapshot();call(0x436ba0,player);after=snapshot()
 call(0x413190,ns['MANAGER'])
 mu.reg_write(UC_X86_REG_EDI,lm);call(0x428270)
 mu.reg_write(UC_X86_REG_ESI,ns['BULLETS']);call(0x40a020)
 mu.reg_write(UC_X86_REG_EDI,anm);call(0x460fe0)
 final=snapshot()
 records.append({'state':state,'age':age,'before':before,'afterPlayer':after,'afterManagers':final})
 print(json.dumps({'state':state,'age':age,'bulletPhases':[(b['slot'],b['phase']) for b in after['bullets']],
  'lasers':len(after['lasers']),'effects':len(after['effects']),'rng':after['rng'],'targetLife':final['targetLife']}),flush=True)
output=ROOT/'tests/fixtures/player-clear-world-original.json'
output.write_text(json.dumps({'schema':'th12-original-player-clear-world/1','originalExeSha256':ns['SHA'],
 'wholeGameOracle':False,'gpuExecuted':False,'diagnosticSuppressionMask':0,'syntheticEclHex':raw.hex(),
 'scope':'Original factories create8 stationary bullets and9 Moving/Timed/Curve lasers, including actualETEX200 protection. '
 '8 original Laser20/Bullet21/Primary27 warm-up passes; real state2 transition at20,300 saves death origin and source. '
 'Controlled player state/age, actual full436ba0 thenEnemy18/Laser20/Bullet21/Primary27. Snapshots include actual '
 'clear effects, cancellation animation, gameRNG, source state and a real damageable Target enemy. '
 'Resource/heap/audio host boundaries retained; Item22/Bonus/HUD/GPU and replay acceptance outside scope.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(output)
