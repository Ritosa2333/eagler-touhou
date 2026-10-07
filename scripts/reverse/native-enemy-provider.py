"""Independent retail Enemy/ECL tick provider with an explicit synthetic world.

Executes original constructors, EnemyManager update, ECL VM/dispatcher and motion.
Host-only file/resource loaders and audio output are intercepted. Optional native
Player, bullet and item/UFO/Bomb/bonus/HUD managers execute their actual updates.
Damage/collision can be enabled or explicitly suppressed for a passive fixture. This
is a partial subsystem provider, never an accepted full-game replay golden.
"""
import argparse,collections,hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from native_anm_resource import install_resource,check_original
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
from unicorn.x86_const import *
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
parser=argparse.ArgumentParser();parser.add_argument('--frames',type=int,default=120);parser.add_argument('--routine',default='main');parser.add_argument('--seed',type=int,default=42);parser.add_argument('--input',type=pathlib.Path);parser.add_argument('--native-bullets',action='store_true');parser.add_argument('--native-player',action='store_true');parser.add_argument('--native-items',action='store_true');parser.add_argument('--active-collision',action='store_true');parser.add_argument('--without-anm',action='store_true');parser.add_argument('--initial-health',type=int,default=10000);parser.add_argument('--damage-fixture',type=int,default=0);parser.add_argument('--output',type=pathlib.Path,default=TITLE/'artifacts/reverse/native-enemy-frames.json');args=parser.parse_args()
if args.native_player and (not args.input or args.without_anm):raise SystemExit('Native player requires a fixture input and original ANM logic')
if args.active_collision and (not args.native_player or args.damage_fixture):raise SystemExit('Active collision requires original player and forbids synthetic damage fixture')
if args.native_items and not args.native_player:raise SystemExit('Native items require original player')
exe=WORKSPACE/'th12/th12.exe'
if hashlib.sha256(exe.read_bytes()).hexdigest()!=SHA:raise SystemExit('Wrong retail EXE version')
pe=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
fixture=json.loads(args.input.read_text(encoding='utf-8')) if args.input else None
if fixture:args.seed=fixture['seed']
for base,size in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x100000),(0x2000000,0x1000000),(0x3000000,0x1000000),(0x4000000,0x400000),(0x5000000,0x1000000),(0x6000000,0x2000000)]:v.mem_map(base,size)
v.mem_write(0x400000,pe.get_memory_mapped_image())
def put(a,f,*values):v.mem_write(a,struct.pack('<'+f,*values))
def get(a,f):return struct.unpack('<'+f,v.mem_read(a,struct.calcsize(f)))
def read_string(a):
 data=bytearray()
 for _ in range(4096):
  byte=v.mem_read(a+len(data),1)[0]
  if not byte:return data.decode('ascii')
  data.append(byte)
 raise RuntimeError('Unterminated retail string')
MANAGER,PLAYER,STAGE,HUD,DIALOGUE,BOMB,BULLETS,PROGRAM=0x2000000,0x2010000,0x2020000,0x2030000,0x2040000,0x2050000,0x2100000,0x2060000
SP,STOP=0x10f0000,0x10ff000
for address,value in [(0x4b43dc,MANAGER),(0x4b4514,PLAYER),(0x4b43e4,STAGE),(0x4b43cc,HUD),(0x4b43c4,DIALOGUE),(0x4b43c8,BULLETS),(0x4b4518,BOMB),(0x4ce8cc,0x5000000)]:put(address,'I',value)
put(0x4b43d4,'I',0x2092000) # Native effect factory resource holder; callbacks execute normally.
put(0x4ce89c,'I',0x2080000)
STAGE_INFO,STAGE_NAMES=0x2081000,0x2094000
put(0x4b452c,'I',STAGE_INFO)
for index,name in enumerate(['stage01.ecl','stage01.std','stage01.anm','enemy.anm','stgenm01.anm','stage01','st01_00a.msg','st01_00b.msg','st01_01a.msg','st01_01b.msg','st01_02a.msg','st01_02b.msg','st01logo.anm']):
 address=STAGE_NAMES+index*128;v.mem_write(address,name.encode('ascii')+b'\0');put(STAGE_INFO+index*4,'I',address)
SOUND_BUFFER,SOUND_VTABLE,SOUND_SET_PAN=0x2090000,0x2091000,0x10fe000
put(0x4d1068,'I',SOUND_BUFFER);put(SOUND_BUFFER,'I',SOUND_VTABLE);put(SOUND_VTABLE+0x40,'I',SOUND_SET_PAN)
put(0x4d1380,'I',SOUND_BUFFER) # Sound 54, continuous summoned-UFO pan, same host-only interface.
v.mem_write(SOUND_SET_PAN,b'\x31\xc0\xc2\x08\x00') # Sound 21 buffer pan method; this + pan argument, stdcall RET 8.
put(0x4b2ed0,'f',1);put(0x4d52d4,'I',1);put(0x4d52dc,'I',1);v.reg_write(UC_X86_REG_MXCSR,0x1f80);v.reg_write(UC_X86_REG_FPCW,0x037f)
v.reg_write(UC_X86_REG_FPTAG,0xffff);v.reg_write(UC_X86_REG_FPSW,0)
put(0x4b0ca8,'i',1);put(0x4b0cb0,'i',1);put(0x4b0c48,'i',100);put(0x4b0cd0,'ii',400,100)
put(0x4ce568,'H',args.seed&65535);put(0x4ce56c,'I',0)
put(BULLETS+0x10,'I',BULLETS+0x64);put(BULLETS+0x64+2000*0x9f8+0x532,'H',5)
put(PLAYER+0x97c,'fff',0,400,0);put(PLAYER+0xa28,'i',1);put(MANAGER+0x64,'I',PROGRAM);put(MANAGER+0x74,'I',1)
if fixture:
 put(0x4b0ca8,'i',fixture['difficulty']);put(0x4b0cac,'i',fixture['stage']);put(0x4b0c90,'ii',fixture['character'],fixture['shot'])
 put(0x4b0c48,'i',fixture['initial']['power']);put(0x4b0ccc,'i',fixture['initial']['rank']);put(0x4b0c78,'i',fixture['initial']['pointValue'])
 put(0x4b0cf0,'i',get(0x4b30e8+fixture['difficulty']*4,'i')[0]*100);put(0x4b0cf4,'i',get(0x4b30fc+fixture['difficulty']*4,'i')[0]*100) # Actual retail tables, 421e0d..421e59.
 put(PLAYER+0x97c,'fff',fixture['initial']['x']/128,fixture['initial']['y']/128,0)
# Populate the original loader's sorted name/header lookup table from raw retail
# SCPT metadata. Binary search and all VM instruction decoding execute natively.
routines={};files=[];cursor=0x4000000
for name in ['default.ecl','stage01.ecl']:
 data=(TITLE/'local/retail'/name).read_bytes();base=cursor;v.mem_write(base,data);cursor+=(len(data)+4095)&~4095
 count=struct.unpack_from('<H',data,16)[0];table=36+struct.unpack_from('<H',data,6)[0];names=table+count*4
 for index in range(count):
  header=base+struct.unpack_from('<I',data,table+index*4)[0];end=data.index(0,names);routine=data[names:end].decode('ascii');routines[routine]=(base+names,header);names=end+1
 files.append({'file':name,'sha256':hashlib.sha256(data).hexdigest()})
resource_by_name={}
if not args.without_anm:
 check_original(exe);resource_base=0x6000000
 anm_names=['enemy.anm','stgenm01.anm','bullet.anm']+([f"pl{fixture['character']:02}.anm",'ascii.anm'] if args.native_player else [])
 for index,name in enumerate(anm_names):
  raw=(TITLE/'local/retail'/name).read_bytes();resource=install_resource(v,raw,resource_base,index);resource_base=(resource['end']+4095)&~4095
  resource_by_name[name]=resource['resource']
  if index==0:put(MANAGER+0x44,'I',resource['resource'])
  if index==1:put(MANAGER+0x48,'I',resource['resource'])
  # Native412c60 assigns slot0 from BulletManager+4debdc;412dd0 loads
  # ECL ANIM banks starting at slot1. They are distinct resource namespaces.
  if index==2:put(MANAGER+0x40,'I',resource['resource']);put(BULLETS+0x4debdc,'I',resource['resource']);put(0x2092000+0x10,'I',resource['resource'])
  if name=='ascii.anm':put(0x4b43b8,'I',0x20b0000);put(0x20b0000+0x18fb4,'I',resource['resource'])
  files.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()})
lookup=PROGRAM+0x100;put(PROGRAM+8,'I',len(routines));put(PROGRAM+0x8c,'I',lookup)
for index,(name,(address,header)) in enumerate(sorted(routines.items())):put(lookup+index*8,'II',address,header)
heap=0x3000000;intercepts=collections.Counter();events=[];frame=-1;last_calls=collections.deque(maxlen=12);births={};bullet_ids={};next_bullet_id=1;last_spawn_request=None
def return_from_call(pop=0,result=0):
 sp=v.reg_read(UC_X86_REG_ESP);ret=get(sp,'I')[0];v.reg_write(UC_X86_REG_EAX,result);v.reg_write(UC_X86_REG_ESP,sp+4+pop);v.reg_write(UC_X86_REG_EIP,ret)
# All stack cleanup sizes are read from the corresponding original callee RET.
external={0x461920:4,0x4615a0:16,0x461970:4,0x461a70:4,0x461e30:0,0x461f40:4,0x461250:0,0x4621c0:0,0x454df0:4,0x454ee0:0,0x453e20:4,0x453d90:0,0x453f10:0,0x454d10:8,0x43e250:8,0x4061e0:8,0x455630:4,0x439ed0:8,0x437810:0,0x437980:4,0x4391c0:4}
if not args.without_anm:
 for address in list(external):
  if address in [0x461920,0x4615a0,0x461970,0x461a70,0x461e30,0x461f40,0x461250,0x4621c0,0x454df0,0x454ee0,0x454d10,0x455630,0x4061e0]:del external[address]
if args.active_collision:
 for address in [0x439ed0,0x437810,0x437980,0x4391c0]:del external[address]
def code_hook(v,address,size,unused):
 global heap,next_bullet_id,last_spawn_request,resource_base
 if address==SOUND_SET_PAN:intercepts['sound_buffer_set_pan']+=1;return
 if address==0x412990:last_spawn_request=get(get(v.reg_read(UC_X86_REG_ESP)+8,'I')[0],'fffiiiII')
 if address==0x413230:births[v.reg_read(UC_X86_REG_EDI)]={'routine':read_string(get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0]),'frame':frame,'request':last_spawn_request}
 if address==0x413d90 and args.active_collision:
  state=v.reg_read(UC_X86_REG_EBX);damage=v.reg_read(UC_X86_REG_EAX)
  if damage:events.append({'frame':frame,'kind':'nativeHurtQuery','enemy':get(state-0x1040+0x27b4,'I')[0],'birth':births.get(state-0x1040),'damage':damage,'healthBefore':get(state+0x1608,'i')[0],'hpImmunity':get(state+0x1694,'i')[0]})
 if address==0x45fe60:
  name=pathlib.PureWindowsPath(read_string(v.reg_read(UC_X86_REG_EBX))).name
  if name not in resource_by_name:
   raw=(TITLE/'local/retail'/name).read_bytes();resource=install_resource(v,raw,resource_base,v.reg_read(UC_X86_REG_ECX));resource_by_name[name]=resource['resource'];resource_base=(resource['end']+4095)&~4095;files.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()})
  intercepts['host_anm_resource_loader']+=1;return_from_call(result=resource_by_name[name]);return
 if address==0x463c10:
  name=pathlib.PureWindowsPath(read_string(v.reg_read(UC_X86_REG_EAX))).name
  raw=(TITLE/'local/retail'/name).read_bytes();allocation=heap;heap+=(len(raw)+15)&~15;v.mem_write(allocation,raw);files.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()});intercepts['host_file_loader']+=1;return_from_call(8,allocation);return
 if address in (0x46c9ea,0x46d04a):
  amount=get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0];allocation=heap;heap+=(amount+15)&~15
  if heap>=0x4000000:raise RuntimeError('Synthetic host heap exhausted')
  intercepts['allocator']+=1;return_from_call(result=allocation);return
 if address in (0x46ca4f,0x46c941):intercepts['host_free']+=1;return_from_call();return
 if address in external:
  intercepts[hex(address)]+=1
  if address==0x439ed0 and args.damage_fixture:
   state=v.reg_read(UC_X86_REG_EBX);events.append({'frame':frame,'kind':'damageFixtureQuery','healthBefore':get(state+0x1608,'i')[0],'hpImmunityBefore':get(state+0x1694,'i')[0],'bodyImmunityBefore':get(state+0x16a8,'i')[0],'flags':get(state+0x16b8,'I')[0],'returnedDamage':args.damage_fixture})
  return_from_call(external[address],args.damage_fixture if address==0x439ed0 else 0);return
 if address==0x40b5e0:
  shooter=v.reg_read(UC_X86_REG_ESI);events.append({'frame':frame,'kind':'bulletEmitter','rawShooterHex':bytes(v.mem_read(shooter,0x214)).hex()});return
 if address in (0x40aa16,0x40aa0a):
  bullet=v.reg_read(UC_X86_REG_EBX)-0x9f8;bullet_ids[bullet]=next_bullet_id;next_bullet_id+=1;events.append({'frame':frame,'kind':'bulletCreated','id':bullet_ids[bullet],'slot':(bullet-BULLETS-0x64)//0x9f8,'position':get(bullet+0x4bc,'fff'),'velocity':get(bullet+0x4c8,'fff'),'speed':get(bullet+0x4d4,'f')[0],'angle':get(bullet+0x4d8,'f')[0],'typeColor':get(bullet+0x9f4,'HH'),'state':get(bullet+0x532,'H')[0],'flags':get(bullet+0x528,'I')[0],'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0]});return
 if address==0x4273f0:
  sp=v.reg_read(UC_X86_REG_ESP);kind,position,angle,speed=get(sp+4,'IIff');events.append({'frame':frame,'kind':'itemSpawn','type':kind,'position':get(position,'fff'),'angle':angle,'speed':speed})
  if not args.native_items:intercepts['item_creation']+=1;return_from_call(16)
  return
 if address in [0x412990,0x413840,0x467170,0x4154d0,0x4694f0]:last_calls.append(hex(address))
# Hook only named native host boundaries; unrelated original instructions do
# not cross into Python. This preserves native logic and speeds long traces.
for address in sorted(set(external)|{SOUND_SET_PAN,0x46c9ea,0x46d04a,0x46c941,0x46ca4f,0x412990,0x413230,0x413840,0x413d90,0x467170,0x4154d0,0x4694f0,0x40b5e0,0x40aa16,0x40aa0a,0x4273f0,0x45fe60,0x463c10}):v.hook_add(UC_HOOK_CODE,code_hook,begin=address,end=address)
resource_fields={0x4b0c44:'scoreStored',0x4b0c48:'power',0x4b0c78:'pivStored',0x4b0ccc:'rank'}
def resource_write(v,access,address,size,value,unused):
 if address in resource_fields and size==4:
  previous=get(address,'I')[0]
  if previous!=value&0xffffffff:events.append({'frame':frame,'kind':'resourceWrite','field':resource_fields[address],'instruction':hex(v.reg_read(UC_X86_REG_EIP)),'before':struct.unpack('<i',struct.pack('<I',previous))[0],'after':struct.unpack('<i',struct.pack('<I',value&0xffffffff))[0]})
v.hook_add(UC_HOOK_MEM_WRITE,resource_write,begin=0x4b0c44,end=0x4b0ccc)
def call(address,*values,budget=2000000):
 put(SP,'I',STOP)
 for index,value in enumerate(values):put(SP+4+index*4,'I',value)
 v.reg_write(UC_X86_REG_ESP,SP)
 try:v.emu_start(address,STOP,count=budget)
 except Exception as error:raise RuntimeError(f'{error}; fixtureFrame={frame}; EIP={v.reg_read(UC_X86_REG_EIP):08x}; stack={get(v.reg_read(UC_X86_REG_ESP),"IIIIIIII")}; recent native calls={list(last_calls)}') from error
 if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native instruction budget exceeded at '+hex(v.reg_read(UC_X86_REG_EIP)))
 return v.reg_read(UC_X86_REG_EAX)
if args.routine not in routines:raise SystemExit('Unknown original routine')
if args.native_items:
 item_manager=call(0x425b10,budget=50000000);ufo_manager=call(0x44a230);bonus_manager=call(0x40daa0);bomb_manager=call(0x406b20)
 if not item_manager or not ufo_manager or not bonus_manager or not bomb_manager:raise RuntimeError('Original item/UFO/bonus/bomb initialization failed')
 stage_manager=call(0x41df40)
 if not stage_manager:raise RuntimeError('Original HUD/stage initialization failed')
 call(0x41d560) # Original HUD VM setup, including front80 rotating stock slots.
if args.native_player:
 call(0x4359a0,PLAYER);v.reg_write(UC_X86_REG_EDI,PLAYER)
 if call(0x435ae0)!=0:raise RuntimeError('Original player initialization failed')
 call(0x436100,PLAYER)
 # State/resources from the replay header are initial conditions, not supplied
 # per-tick player position or damage results. All subsequent movement is native.
 put(PLAYER+0x97c,'fff',fixture['initial']['x']/128,fixture['initial']['y']/128,0);put(PLAYER+0x988,'ii',fixture['initial']['x'],fixture['initial']['y'])
 for address,key in [(0x4b0c48,'power'),(0x4b0c98,'lives'),(0x4b0c9c,'lifeFragments'),(0x4b0ca0,'bombs'),(0x4b0ca4,'bombFragments')]:put(address,'i',fixture['initial'].get(key,0))
 if args.native_items:
  put(0x4b0c78,'i',fixture['initial']['pointValue']);put(0x4b0c44,'i',fixture['initial'].get('score',0));put(0x4b0c4c,'iii',*fixture['initial'].get('ufoColors',[0,0,0]))
SPAWN=0x2070000;put(SPAWN,'fffiiiII',0,0,0,0,0,args.initial_health,0,0) # Retail startup 40f0f8: request health 0x2710.
root_enemy=call(0x412990,routines[args.routine][0],SPAWN)
# Native creation already calls the complete first Enemy tick and sets its
# once-per-frame bit. Prime the original manager once solely to clear that bit;
# subsequent fixture tick 0 is the next fresh native update, like candidate tick.
call(0x413190,MANAGER)
def context_location(pc):
 candidates=[(name,header) for name,(_,header) in routines.items() if header+16<=pc]
 if not pc or not candidates:return None
 name,header=max(candidates,key=lambda x:x[1]);return {'routine':name,'instructionOffset':pc-header-16}
def capture():
 result=[];node=get(MANAGER+0x68,'I')[0]
 while node:
  enemy,next_node=get(node,'II');state=enemy+0x1040;ctx=get(enemy+4,'I')[0]
  contexts=[];thread=enemy+0x1030
  while thread:
   thread_context=get(thread,'I')[0];location=context_location(get(thread_context+4,'I')[0]);contexts.append({'id':get(thread_context+0x1020,'i')[0],'context':location,'time':get(thread_context,'f')[0],'localsHex':bytes(v.mem_read(thread_context+8,64)).hex()});thread=get(thread+4,'I')[0]
  result.append({'id':get(enemy+0x27b4,'I')[0],'birth':births.get(enemy),'position':get(state+0x34,'ff'),'absolute':get(state+0x68,'ff'),'relative':get(state+0x9c,'ff'),'angle':get(state+0x84,'f')[0],'speed':get(state+0x80,'f')[0],'relativeAngle':get(state+0xb8,'f')[0],'relativeSpeed':get(state+0xb4,'f')[0],'radius':get(state+0x88,'f')[0],'relativeRadius':get(state+0xbc,'f')[0],'life':get(enemy+0x2648,'i')[0],'flags':get(enemy+0x26f8,'I')[0],'age':get(state+0x270,'i')[0],'hpImmunity':get(state+0x1694,'i')[0],'bodyImmunity':get(state+0x16a8,'i')[0],'offscreenExtent':get(state+0x15ec,'ff'),'context':context_location(get(ctx+4,'I')[0]),'contextTime':get(ctx,'f')[0],'contexts':contexts,'localsHex':bytes(v.mem_read(ctx+8,64)).hex(),'privateVariablesHex':bytes(v.mem_read(state+0x23c,48)).hex()});node=next_node
 return result
def capture_bullets():
 result=[]
 for bullet,identifier in bullet_ids.items():
  state=get(bullet+0x532,'H')[0]
  if not state:continue
  result.append({'id':identifier,'slot':(bullet-BULLETS-0x64)//0x9f8,'position':get(bullet+0x4bc,'ff'),'velocity':get(bullet+0x4c8,'ff'),'speed':get(bullet+0x4d4,'f')[0],'angle':get(bullet+0x4d8,'f')[0],'typeColor':get(bullet+0x9f4,'HH'),'state':state,'age':get(bullet+0x4e8,'i')[0],'collisionDelay':get(bullet+4,'i')[0],'flags':get(bullet+0x528,'I')[0],'extensionCursor':get(bullet+0x548,'i')[0],'animationTime':get(bullet+8+0x6c,'i')[0],'hitbox':get(bullet+0x4dc,'ff')})
 return result
def capture_animations():
 result=[]
 for list_offset,priority in [(0x8856b8,27),(0x8856c0,8)]:
  node=get(0x5000000+list_offset,'I')[0]
  while node:
   animation,next_node=get(node,'II')
   result.append({'id':get(animation,'I')[0],'priority':priority,'bankScriptSprite':get(animation+0x3e6,'H')[0:1]+get(animation+0x3ea,'H')[0:1]+get(animation+0x3e4,'H')[0:1],'clock':get(animation+0x6c,'i')[0],'flags':get(animation+0x47c,'II'),'anchor':get(animation+0x430,'fff'),'position':get(animation+0x424,'fff'),'rotation':get(animation+0x24,'fff'),'scale':get(animation+0x40,'ff'),'spriteSize':get(animation+0x58,'ff'),'primaryColor':get(animation+0x3bc,'I')[0],'interrupt':get(animation+0x3c4,'H')[0]});node=next_node
 return result
def capture_player():
 return {'position':get(PLAYER+0x97c,'ff'),'fixedPosition':get(PLAYER+0x988,'ii'),'state':get(PLAYER+0xa28,'i')[0],'stateAge':get(PLAYER+0xa34,'i')[0],'invulnerability':get(PLAYER+0xc404,'i')[0],'shootAge':get(PLAYER+0xc424,'i')[0],'bodyAnimationTime':get(PLAYER+0x14+0x6c,'i')[0],'lifeAndBombCounters':get(0x4b0c98,'iiii'),'power':get(0x4b0c48,'i')[0],'speedsFixed':get(PLAYER+0x990,'iiii'),'shtMoveFloats':get(get(PLAYER+0xa2c,'I')[0]+0x10,'ffff'),'movementFactor':get(PLAYER+0x825c,'f')[0],'direction':get(PLAYER+0xa1c,'i')[0],'deltaFixed':get(PLAYER+0xa14,'ii')}
def capture_items():
 result=[]
 for slot in range(2664):
  item=item_manager+0x14+slot*0x9d8;state,kind=get(item+0x9b0,'ii')
  if state:result.append({'slot':slot,'state':state,'type':kind,'position':get(item+0x96c,'ff'),'velocity':get(item+0x978,'ff'),'age':get(item+0x98c,'i')[0],'colorTimer':get(item+0x9a0,'i')[0],'attractionSpeed':get(item+0x9bc,'f')[0],'delay':get(item+0x9b8,'i')[0]})
 return result
def capture_sources():
 result=[]
 for slot in range(128):
  source=PLAYER+0x8988+slot*0x74;flags=get(source+0x70,'I')[0]
  if flags&1:result.append({'slot':slot,'radius':get(source,'f')[0],'radialGrowth':get(source+4,'f')[0],'rotation':get(source+8,'f')[0],'rotationGrowth':get(source+12,'f')[0],'width':get(source+16,'f')[0],'height':get(source+20,'f')[0],'position':get(source+0x18,'fff'),'heading':get(source+0x3c,'f')[0],'timer':get(source+0x4c,'ii'),'damage':get(source+0x60,'i')[0],'accumulatedDamage':get(source+0x64,'i')[0],'limit':get(source+0x68,'i')[0],'cadence':get(source+0x6c,'i')[0],'flags':flags})
 return result
def capture_friendly():
 result=[]
 for slot in range(256):
  projectile=PLAYER+0xa58+slot*0x78;phase=get(projectile+0x48,'i')[0]
  if not phase:continue
  result.append({'slot':slot,'phase':phase,'age':get(projectile+4,'i')[0],'position':get(projectile+0x14,'ff'),'velocity':get(projectile+0x20,'ff'),'speed':get(projectile+0x2c,'f')[0],'angle':get(projectile+0x30,'f')[0],'damage':get(projectile+0x60,'i')[0],'hitbox':get(projectile+0x68,'ff'),'targetID':get(projectile+0x64,'I')[0],'hitLastFrame':get(projectile+0x58,'i')[0]})
 return result
def capture_economy():
 return {'scoreStored':get(0x4b0c44,'i')[0],'power':get(0x4b0c48,'i')[0],'pivStored':get(0x4b0c78,'i')[0],'maxPivStored':get(0x4b0cf4,'i')[0],'rank':get(0x4b0ccc,'i')[0],'lifeBombCounters':get(0x4b0c98,'iiii'),'ufoColors':get(0x4b0c4c,'iii'),'ufoCountState':get(0x4b0c58,'ii'),'ufoAge':get(ufo_manager+0x14,'i')[0],'ufoKind':get(ufo_manager+0x30,'i')[0],'ufoEnemy':get(ufo_manager+0x38,'I')[0],'ufoFill':get(ufo_manager+0x50,'f')[0],'ufoBuckets':get(ufo_manager+0x48,'ii'),'spellFlagsBonus':get(bonus_manager+0x7c,'II'),'bombState':get(bomb_manager+0x3c,'i')[0]}
frames=[{'frame':-1,'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0],'enemies':capture(),'animations':capture_animations() if not args.without_anm else [],'player':capture_player() if args.native_player else None}]
for frame in range(args.frames):
 if fixture:
  sample=fixture['inputs'][frame];input_mask=0xffffffff if args.native_items else 0xfffffffd;put(0x4d49d0,'I',sample['held']&input_mask);put(0x4d49d8,'I',sample.get('pressed',0)&input_mask);put(0x4d49dc,'I',sample.get('released',0)&input_mask)
  if not args.native_player:put(PLAYER+0x97c,'fff',sample['xFixed']/128,sample['yFixed']/128,0);put(0x4b0c48,'i',sample['power']);put(0x4b0ccc,'i',sample['rank'])
 if not args.without_anm:v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x4610d0)
 if args.native_player:call(0x436ba0,PLAYER)
 if args.native_items:v.reg_write(UC_X86_REG_EAX,bomb_manager);call(0x406ce0)
 call(0x413190,MANAGER)
 if args.native_items:v.reg_write(UC_X86_REG_EAX,ufo_manager);call(0x44a2d0)
 if args.native_bullets:v.reg_write(UC_X86_REG_ESI,BULLETS);call(0x40a020)
 if args.native_items:
  call(0x425c20,item_manager);v.reg_write(UC_X86_REG_EDI,bonus_manager);call(0x40db40);v.reg_write(UC_X86_REG_EDI,stage_manager);call(0x41e020)
 if not args.without_anm:v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0)
 frames.append({'frame':frame,'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0],'enemies':capture(),'bullets':capture_bullets() if args.native_bullets else [],'animations':capture_animations() if not args.without_anm else [],'player':capture_player() if args.native_player else None,'items':capture_items() if args.native_items else [],'sources':capture_sources() if args.native_player else [],'friendly':capture_friendly() if args.native_player else [],'economy':capture_economy() if args.native_items else None})
args.output.parent.mkdir(parents=True,exist_ok=True)
args.output.write_text(json.dumps({'schema':'th12-native-enemy-provider/1','originalExeSha256':SHA,'retailInputs':files,'scope':'original native selected managers with host-only loaders and audio output boundaries; startup/world completeness remains partial','partial':True,'wholeGameOracle':False,'routine':args.routine,'initialHealth':args.initial_health,'damageFixture':args.damage_fixture,'requestedSeed':args.seed,'rngInitialized':True,'nativePlayerExecuted':args.native_player,'nativeItemsUfoSpellMsgExecuted':args.native_items,'bombInputEnabled':args.native_items,'activeCollisionExecuted':args.active_collision,'nativeAnmExecuted':not args.without_anm,'nativeBulletUpdateExecuted':args.native_bullets,'fixtureInput':str(args.input) if args.input else None,'creationGuardPrimed':True,'intercepts':dict(intercepts),'frames':frames,'events':events},indent=2)+'\n',encoding='utf-8');print(args.output)
