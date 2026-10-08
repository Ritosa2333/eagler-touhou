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
from native_crt_host import CrtHost
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
from unicorn.x86_const import *
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
parser=argparse.ArgumentParser();parser.add_argument('--frames',type=int,default=120);parser.add_argument('--routine',default='main');parser.add_argument('--seed',type=int,default=42);parser.add_argument('--input',type=pathlib.Path);parser.add_argument('--native-bullets',action='store_true');parser.add_argument('--native-player',action='store_true');parser.add_argument('--native-items',action='store_true');parser.add_argument('--active-collision',action='store_true');parser.add_argument('--without-anm',action='store_true');parser.add_argument('--initial-health',type=int,default=10000);parser.add_argument('--damage-fixture',type=int,default=0);parser.add_argument('--omit-animation-capture',action='store_true');parser.add_argument('--prime-creation-guard',action='store_true');parser.add_argument('--trace-hurt-queries',nargs=2,type=int);parser.add_argument('--trace-rng-ticks',nargs=2,type=int);parser.add_argument('--capture-text-output',action='store_true');parser.add_argument('--trace-source-births',nargs=2,type=int);parser.add_argument('--output',type=pathlib.Path,default=TITLE/'artifacts/reverse/native-enemy-frames.json');parser.add_argument('--native-lasers',action='store_true');parser.add_argument('--instruction-budget',type=int,default=2000000);parser.add_argument('--trace-laser-visual-ticks',nargs=2,type=int);parser.add_argument('--trace-anm-vm-address',type=lambda value:int(value,0));parser.add_argument('--trace-cancel-ticks',nargs=2,type=int);parser.add_argument('--native-camera-effects',action='store_true');parser.add_argument('--omit-world-frame-capture',action='store_true');parser.add_argument('--trace-msg-ticks',nargs=2,type=int);args=parser.parse_args()
if args.native_player and (not args.input or args.without_anm):raise SystemExit('Native player requires a fixture input and original ANM logic')
if args.active_collision and (not args.native_player or args.damage_fixture):raise SystemExit('Active collision requires original player and forbids synthetic damage fixture')
if args.native_items and not args.native_player:raise SystemExit('Native items require original player')
if args.native_camera_effects and not args.native_items:raise SystemExit('Native camera effects require original item/Bomb/world managers')
if args.native_lasers and not (args.native_player and args.native_items and args.native_bullets and not args.without_anm):raise SystemExit('Native lasers require original Player/Item/Bullet/ANM updates')
if not 1<=args.instruction_budget<=200000000:raise SystemExit('Native instruction budget must be between1 and200000000')
exe=WORKSPACE/'th12/th12.exe'
if hashlib.sha256(exe.read_bytes()).hexdigest()!=SHA:raise SystemExit('Wrong retail EXE version')
pe=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
fixture=json.loads(args.input.read_text(encoding='utf-8')) if args.input else None
if fixture and fixture.get('inputOnly') and not args.native_player:raise SystemExit('Input-only replay fixtures require original native player execution')
if fixture and fixture.get('stage')!=1:raise SystemExit('This partial native provider currently binds stage1 resources only')
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
if args.native_camera_effects:
 # Selected-manager unpaused application context. Camera4527f0 explicitly
 # requires a non-null GameScene4b44e8 before it advances or consumes RNG.
 # This supplies only the declared startup environment/flags, not a camera
 # update, random value, geometry or original application scene constructor.
 put(0x4b44e8,'I',0x20d0000);put(0x20d0060,'I',0)
STAGE_INFO,STAGE_NAMES=0x2081000,0x2094000
put(0x4b452c,'I',STAGE_INFO)
for index,name in enumerate(['stage01.ecl','stage01.std','stage01.anm','enemy.anm','stgenm01.anm','stage01','st01_00a.msg','st01_00b.msg','st01_01a.msg','st01_01b.msg','st01_02a.msg','st01_02b.msg','st01logo.anm']):
 address=STAGE_NAMES+index*128;v.mem_write(address,name.encode('ascii')+b'\0');put(STAGE_INFO+index*4,'I',address)
SOUND_BUFFER,SOUND_VTABLE,SOUND_SET_PAN,SOUND_GET_STATUS,SOUND_STOP=0x2090000,0x2091000,0x10fe000,0x10fe010,0x10fe020
put(0x4d1068,'I',SOUND_BUFFER);put(SOUND_BUFFER,'I',SOUND_VTABLE)
for offset,method in [(0x24,SOUND_GET_STATUS),(0x40,SOUND_SET_PAN),(0x48,SOUND_STOP)]:put(SOUND_VTABLE+offset,'I',method)
put(0x4d1380,'I',SOUND_BUFFER) # Sound 54, continuous summoned-UFO pan, same host-only interface.
v.mem_write(SOUND_SET_PAN,b'\x31\xc0\xc2\x08\x00') # Sound 21 buffer pan method; this + pan argument, stdcall RET 8.
# IDirectSoundBuffer output methods only. 423020 queries status then stops each
# buffer during the actual native GameOver scene transition. The CPU sound
# queue updates in that function remain native; this silent host has no playing
# buffer and makes no claim about audible output.
v.mem_write(SOUND_GET_STATUS,b'\x8b\x44\x24\x08\xc7\x00\x00\x00\x00\x00\x31\xc0\xc2\x08\x00')
v.mem_write(SOUND_STOP,b'\x31\xc0\xc2\x04\x00')
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
heap_allocations=[];heap_frees=[];heap_regions={'heap':(0x3000000,0x4000000),'ecl':(0x4000000,0x4400000),'anmManager':(0x5000000,0x6000000),'resources':(0x6000000,0x8000000)}
anm_vm_writes=[]
camera_update_counts=collections.Counter()
camera_draw_reset_count=0
friendly_ids={};next_friendly_id=1;query_stack=[]
laser_ids={}
native_polar_inputs={}
source_request=None
collision_query=None
laser_line_query=None

def source_fields(pointer):
 return {'slot':(pointer-PLAYER-0x8988)//0x74,'radius':get(pointer,'f')[0],'radialGrowth':get(pointer+4,'f')[0],'rotation':get(pointer+8,'f')[0],'rotationGrowth':get(pointer+12,'f')[0],'width':get(pointer+16,'f')[0],'height':get(pointer+20,'f')[0],'position':get(pointer+0x18,'fff'),'heading':get(pointer+0x3c,'f')[0],'timer':get(pointer+0x4c,'ii'),'damage':get(pointer+0x60,'i')[0],'accumulatedDamage':get(pointer+0x64,'i')[0],'limit':get(pointer+0x68,'i')[0],'cadence':get(pointer+0x6c,'i')[0],'flags':get(pointer+0x70,'I')[0]}

def allocate_bytes(amount,source='crtHeap'):
 global heap
 allocation=heap;end=allocation+((max(amount,1)+15)&~15)
 if end>heap_regions['heap'][1]:raise RuntimeError('Synthetic host heap exhausted before allocation '+hex(allocation)+'..'+hex(end))
 heap=end
 heap_allocations.append({'inputTick':frame,'source':source,'address':hex(allocation),'endExclusive':hex(end),'requestedBytes':amount,'alignedBytes':end-allocation,'nativeInstruction':hex(v.reg_read(UC_X86_REG_EIP)),'caller':hex(get(v.reg_read(UC_X86_REG_ESP),'I')[0])})
 return allocation

crt_host=CrtHost(v,pe,allocate_bytes) if args.native_items else None
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
 global heap,next_bullet_id,next_friendly_id,last_spawn_request,resource_base,source_request,collision_query,laser_line_query
 if args.trace_cancel_ticks and args.trace_cancel_ticks[0]<=frame<=args.trace_cancel_ticks[1] and address in (0x40cf60,0x40caa0,0x40c8b0,0x428670,0x4286f0,0x428750,0x42a800,0x42a1f0,0x42bd90,0x42b750,0x42e3e0,0x42df00):
  sp=v.reg_read(UC_X86_REG_ESP);row={'frame':frame,'kind':'nativeCancelEntry','entry':hex(address),'caller':hex(get(sp,'I')[0]),'stackWords':get(sp+4,'6I'),'registers':{name:hex(v.reg_read(register)) for name,register in [('eax',UC_X86_REG_EAX),('ecx',UC_X86_REG_ECX),('edx',UC_X86_REG_EDX),('esi',UC_X86_REG_ESI),('edi',UC_X86_REG_EDI)]},'spellFlagsBonus':get(bonus_manager+0x7c,'II') if args.native_items else None}
  if address==0x40cf60:row['convert']=get(sp+4,'I')[0]
  row['stackCodeWords']=[hex(word) for word in get(sp,'32I') if 0x400000<=word<0x498000]
  if address in (0x42a800,0x42a1f0,0x42bd90,0x42b750,0x42e3e0,0x42df00):
   pointer=v.reg_read(UC_X86_REG_ECX);row['nativeLaser']={'address':hex(pointer),'vtable':hex(get(pointer,'I')[0]),'nativeField80':get(pointer+0x80,'I')[0],'position':get(pointer+0x50,'fff'),'geometry':get(pointer+0x68,'4f'),'phase':get(pointer+0xc,'i')[0]}
  events.append(row)
 if address==0x412720 and args.trace_msg_ticks and args.trace_msg_ticks[0]<=frame<=args.trace_msg_ticks[1]:
  stage=get(0x4b43e4,'I')[0];msg=get(stage+0x6d30,'I')[0];events.append({'frame':frame,'kind':'nativeMsgGateRead','entry':'0x412720','caller':hex(get(v.reg_read(UC_X86_REG_ESP),'I')[0]),'message':hex(msg),'gate8c':get(msg+0x8c,'i')[0] if msg else None,'held':get(0x4d49d0,'I')[0],'pressed':get(0x4d49dc,'I')[0]})
 if address==0x452a60:
  sp=v.reg_read(UC_X86_REG_ESP);caller=get(sp,'I')[0];events.append({'frame':frame,'kind':'nativeCameraEffectRequested','entry':'0x452a60','caller':hex(caller),'wrapperCaller':hex(get(sp+36,'I')[0]) if caller==0x4529e9 else None,'rawParameters':get(sp+4,'7I'),'stackCodeWords':[hex(word) for word in get(sp,'32I') if 0x400000<=word<0x498000]})
 if args.active_collision and args.trace_rng_ticks and args.trace_rng_ticks[0]<=frame<=args.trace_rng_ticks[1]:
  if address in (0x437810,0x437980):
   sp=v.reg_read(UC_X86_REG_ESP);position=v.reg_read(UC_X86_REG_ECX if address==0x437810 else UC_X86_REG_EAX)
   offset=position-BULLETS-0x64-0x4bc
   if 0<=offset<2000*0x9f8 and offset%0x9f8==0:
    bullet=position-0x4bc
    collision_query={'frame':frame,'kind':'nativeProjectileCollision','entry':hex(address),'caller':hex(get(sp,'I')[0]),'sourceHostileSlot':offset//0x9f8,'position':get(position,'ff'),'playerPosition':get(PLAYER+0x97c,'ff'),'playerBounds':get(PLAYER+0x9cc,'6f'),'bulletFlags':get(bullet+0x528,'I')[0],'bulletAge':get(bullet+0x4e8,'i')[0],'bulletTypeColor':get(bullet+0x9f4,'HH')}
    if address==0x437810:collision_query['hitbox']=get(v.reg_read(UC_X86_REG_EAX),'ff')
    else:collision_query['radius']=get(sp+4,'f')[0]
  elif address in (0x409b5a,0x409e5f) and collision_query:
   collision_query['result']=v.reg_read(UC_X86_REG_EAX);events.append(collision_query);collision_query=None
  elif address==0x4391c0:
   sp=v.reg_read(UC_X86_REG_ESP);position=get(sp+4,'I')[0]
   events.append({'frame':frame,'kind':'nativeGrazeAward','entry':'0x4391c0','caller':hex(get(sp,'I')[0]),'position':get(position,'fff') if position else None,'grazeBefore':get(0x4b0cdc,'i')[0],'pivBefore':get(0x4b0c78,'i')[0]})
  if address==0x437a80 and get(v.reg_read(UC_X86_REG_ESP),'I')[0]==0x42b029:
   sp=v.reg_read(UC_X86_REG_ESP);object_pointer=v.reg_read(UC_X86_REG_EDI)
   laser_line_query={'frame':frame,'kind':'nativeTimedLaserLineCollision','entry':'0x437a80','caller':'0x42b029','object':hex(object_pointer),'nativeField80':get(object_pointer+0x80,'I')[0],'origin':get(v.reg_read(UC_X86_REG_EAX),'fff'),'angleTransverseLongitudinal':get(sp+4,'fff'),'playerPosition':get(PLAYER+0x97c,'fff'),'playerHalfExtents':get(PLAYER+0x9e4,'fff'),'ageBefore':get(object_pointer+0x18,'i')[0]}
  elif address==0x42b029 and laser_line_query:
   laser_line_query['result']=v.reg_read(UC_X86_REG_EAX);events.append(laser_line_query);laser_line_query=None
 if address==0x4390f0 and args.trace_source_births and args.trace_source_births[0]<=frame<=args.trace_source_births[1]:
  sp=v.reg_read(UC_X86_REG_ESP);position,radius,growth,duration,damage=get(sp+4,'Iffii');offset=position-PLAYER-0xa58-0x14
  source_request={'frame':frame,'kind':'sourceCircleRequested','entry':'0x4390f0','caller':hex(get(sp,'I')[0]),'position':get(position,'fff'),'radius':radius,'growth':growth,'duration':duration,'damage':damage,'parentFriendly':None}
  if 0<=offset<256*0x78 and offset%0x78==0:
   slot=offset//0x78;shot=position-0x14;source_request['parentFriendly']={'slot':slot,'id':friendly_ids.get(slot),'position':get(position,'fff'),'age':get(shot+4,'i')[0],'phase':get(shot+0x48,'i')[0],'speed':get(shot+0x2c,'f')[0],'angle':get(shot+0x30,'f')[0]}
  events.append(source_request)
 if address==0x4391ab and source_request and source_request['frame']==frame:
  pointer=v.reg_read(UC_X86_REG_ESI);events.append({'frame':frame,'kind':'sourceCircleCreated','entry':'0x4391ab','request':source_request,'source':source_fields(pointer)});source_request=None
 if address in (0x44e660,0x44e8e0) and args.capture_text_output:
  # Output-only boundary audited against the entire native wrappers. Their
  # writes target GDI pixels/font objects and D3D surfaces, not gameplay state.
  # ActualSpell,CRT460800 formatting,4606b0 layout and VM updates still run.
  sp=v.reg_read(UC_X86_REG_ESP);parameters=get(sp+4,'6I');text_pointer=v.reg_read(UC_X86_REG_ECX) if address==0x44e660 else parameters[4];raw=bytearray()
  for index in range(4096):
   byte=v.mem_read(text_pointer+index,1)[0]
   if not byte:break
   raw.append(byte)
  else:raise RuntimeError('Unterminated native dynamic text')
  record={'frame':frame,'kind':'textTextureOutput','entry':hex(address),'style':v.reg_read(UC_X86_REG_EAX),'textHex':raw.hex(),'sourceRect':get(parameters[0],'4i'),'rawStackWords':parameters,'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0]}
  if address==0x44e660:record.update({'offset':v.reg_read(UC_X86_REG_EDX),'fontHeight':parameters[1],'foregroundColor':parameters[2],'outlineColor':parameters[3],'spacing':parameters[5]})
  events.append(record);intercepts['text_texture_output_'+hex(address)]+=1;return_from_call(24);return
 if address in (0x4643d0,0x464440) and args.trace_rng_ticks and args.trace_rng_ticks[0]<=frame<=args.trace_rng_ticks[1]:
  sp=v.reg_read(UC_X86_REG_ESP);rng=v.reg_read(UC_X86_REG_ESI)
  events.append({'frame':frame,'kind':'rngCall','entry':hex(address),'rngObject':hex(rng),'counterBefore':get(rng+4,'I')[0],'caller':hex(get(sp,'I')[0]),'stackCodeWords':[hex(word) for word in get(sp,'32I') if 0x400000<=word<0x498000],'ebx':hex(v.reg_read(UC_X86_REG_EBX)),'edi':hex(v.reg_read(UC_X86_REG_EDI))})
 if address==0x40d640:
  destination=v.reg_read(UC_X86_REG_ECX);offset=destination-BULLETS-0x64-0x4c8
  if offset>=0 and offset%0x9f8==0 and offset//0x9f8<2000:
   native_polar_inputs[destination-0x4c8]={'frame':frame,'angle':get(v.reg_read(UC_X86_REG_ESP)+4,'f')[0],'speed':get(v.reg_read(UC_X86_REG_ESP)+8,'f')[0],'instruction':'0x40d640'}
  return
 if address==0x43967b:
  projectile=v.reg_read(UC_X86_REG_EDI);slot=(projectile-PLAYER-0xa58)//0x78
  if not 0<=slot<256:raise RuntimeError('Native friendly allocation left the physical pool')
  friendly_ids[slot]=next_friendly_id;events.append({'frame':frame,'kind':'friendlyCreated','id':next_friendly_id,'slot':slot,'instruction':'0x43967b'});next_friendly_id+=1;return
 if address==0x439ed0 and args.trace_hurt_queries and args.trace_hurt_queries[0]<=frame<=args.trace_hurt_queries[1]:
  sp=v.reg_read(UC_X86_REG_ESP)
  if get(sp,'I')[0]==0x413d90:
   state=v.reg_read(UC_X86_REG_EBX);query_stack.append({'frame':frame,'kind':'nativeHurtQueryTrace','enemy':get(state-0x1040+0x27b4,'I')[0],'healthBefore':get(state+0x1608,'i')[0],'position':get(get(sp+4,'I')[0],'ff'),'bounds':get(get(sp+8,'I')[0],'ff'),'scanSlots':[],'sourceScans':[]})
 if address==0x439f70 and query_stack:
  projectile=v.reg_read(UC_X86_REG_EDI)-0x18;phase=get(projectile+0x48,'i')[0]
  if phase and len(query_stack[-1]['scanSlots'])<256:query_stack[-1]['scanSlots'].append({'slot':(projectile-PLAYER-0xa58)//0x78,'id':friendly_ids.get((projectile-PLAYER-0xa58)//0x78),'phase':phase,'age':get(projectile+4,'i')[0],'hit':get(projectile+0x58,'i')[0]})
 if address==0x43a21c and query_stack:
  pointer=v.reg_read(UC_X86_REG_EBX)
  if get(pointer+0x70,'I')[0]&1:query_stack[-1]['sourceScans'].append({'sourceBefore':source_fields(pointer),'accepted':False})
 if address in (0x43a3c4,0x43a3d1,0x43a3f6) and query_stack and query_stack[-1]['sourceScans']:
  pointer=v.reg_read(UC_X86_REG_EBX);record=query_stack[-1]['sourceScans'][-1]
  if record['sourceBefore']['slot']==(pointer-PLAYER-0x8988)//0x74:
   if address==0x43a3c4:record['nativeCircleSquaredDistance']=get(v.reg_read(UC_X86_REG_ESP)+0x2c,'f')[0]
   elif address==0x43a3d1:record['accepted']=True
   else:record['sourceAfter']=source_fields(pointer)
 if address in (SOUND_SET_PAN,SOUND_GET_STATUS,SOUND_STOP):
  intercepts[{SOUND_SET_PAN:'sound_buffer_set_pan',SOUND_GET_STATUS:'sound_buffer_get_status',SOUND_STOP:'sound_buffer_stop'}[address]]+=1;return
 if address==0x433710:
  events.append({'frame':frame,'kind':'nativeGameOverSceneEntered','entry':'0x433710','playerState':get(PLAYER+0xa28,'i')[0],'playerStateAge':get(PLAYER+0xa34,'i')[0],'lifeBombCounters':get(0x4b0c98,'iiii')})
  # The selected gameplay-manager fixture has no initialized application/menu
  # scene. Stop at the actual native transition, before entering that absent
  # environment. This boundary is not a completed gameplay input tick.
  v.emu_stop();return
 if address==0x412990:last_spawn_request=get(get(v.reg_read(UC_X86_REG_ESP)+8,'I')[0],'fffiiiII')
 if address==0x413230:births[v.reg_read(UC_X86_REG_EDI)]={'routine':read_string(get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0]),'frame':frame,'request':last_spawn_request}
 if address==0x413d90 and args.active_collision:
  state=v.reg_read(UC_X86_REG_EBX);damage=v.reg_read(UC_X86_REG_EAX)
  if query_stack:
   query=query_stack.pop();query['damage']=damage;events.append(query)
  if damage:events.append({'frame':frame,'kind':'nativeHurtQuery','enemy':get(state-0x1040+0x27b4,'I')[0],'birth':births.get(state-0x1040),'damage':damage,'healthBefore':get(state+0x1608,'i')[0],'hpImmunity':get(state+0x1694,'i')[0]})
 if address==0x45fe60:
  name=pathlib.PureWindowsPath(read_string(v.reg_read(UC_X86_REG_EBX))).name
  if name not in resource_by_name:
   raw=(TITLE/'local/retail'/name).read_bytes();resource=install_resource(v,raw,resource_base,v.reg_read(UC_X86_REG_ECX));resource_by_name[name]=resource['resource'];resource_base=(resource['end']+4095)&~4095;files.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()})
  intercepts['host_anm_resource_loader']+=1;return_from_call(result=resource_by_name[name]);return
 if address==0x463c10:
  name=pathlib.PureWindowsPath(read_string(v.reg_read(UC_X86_REG_EAX))).name
  if name=='scoreth12.dat':
   # Explicit clean-profile host boundary: do not import the user's local save.
   # The actual43cf90 constructor takes the native missing-file path, including
   #43ceb0's defaults/RNG and43d140's header allocation. Spell history writes
   # later execute normally against that constructed4b451c object.
   intercepts['host_score_file_absent']+=1;return_from_call(8,0);return
  raw=(TITLE/'local/retail'/name).read_bytes();allocation=allocate_bytes(len(raw),'retailFile:'+name);v.mem_write(allocation,raw);files.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()});intercepts['host_file_loader']+=1;return_from_call(8,allocation);return
 if address in (0x46c9ea,0x46d04a):
  amount=get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0];allocation=allocate_bytes(amount,'nativeAllocator:'+hex(address))
  intercepts['allocator']+=1;return_from_call(result=allocation);return
 if address in (0x46ca4f,0x46c941):
  sp=v.reg_read(UC_X86_REG_ESP);heap_frees.append({'inputTick':frame,'entry':hex(address),'caller':hex(get(sp,'I')[0]),'pointer':hex(get(sp+4,'I')[0]),'reclaimed':False});intercepts['host_free']+=1;return_from_call();return
 if address in external:
  intercepts[hex(address)]+=1
  if address==0x439ed0 and args.damage_fixture:
   state=v.reg_read(UC_X86_REG_EBX);events.append({'frame':frame,'kind':'damageFixtureQuery','healthBefore':get(state+0x1608,'i')[0],'hpImmunityBefore':get(state+0x1694,'i')[0],'bodyImmunityBefore':get(state+0x16a8,'i')[0],'flags':get(state+0x16b8,'I')[0],'returnedDamage':args.damage_fixture})
  return_from_call(external[address],args.damage_fixture if address==0x439ed0 else 0);return
 if address==0x40b5e0:
  shooter=v.reg_read(UC_X86_REG_ESI);events.append({'frame':frame,'kind':'bulletEmitter','rawShooterHex':bytes(v.mem_read(shooter,0x214)).hex()});return
 if address in (0x40aa16,0x40aa0a):
  bullet=v.reg_read(UC_X86_REG_EBX)-0x9f8;bullet_ids[bullet]=next_bullet_id;next_bullet_id+=1;events.append({'frame':frame,'kind':'bulletCreated','id':bullet_ids[bullet],'slot':(bullet-BULLETS-0x64)//0x9f8,'position':get(bullet+0x4bc,'fff'),'velocity':get(bullet+0x4c8,'fff'),'velocityInput':native_polar_inputs.get(bullet),'speed':get(bullet+0x4d4,'f')[0],'angle':get(bullet+0x4d8,'f')[0],'typeColor':get(bullet+0x9f4,'HH'),'state':get(bullet+0x532,'H')[0],'flags':get(bullet+0x528,'I')[0],'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0]});return
 if address==0x4273f0:
  sp=v.reg_read(UC_X86_REG_ESP);kind,position,angle,speed=get(sp+4,'IIff')
  source_offset=position-BULLETS-0x64-0x4bc
  source_slot=source_offset//0x9f8 if 0<=source_offset<2000*0x9f8 and source_offset%0x9f8==0 else None
  events.append({'frame':frame,'kind':'itemSpawn','type':kind,'position':get(position,'fff'),'angle':angle,'speed':speed,'caller':hex(get(sp,'I')[0]),'positionPointer':hex(position),'sourceHostileSlot':source_slot})
  if not args.native_items:intercepts['item_creation']+=1;return_from_call(16)
  return
 if address in [0x412990,0x413840,0x467170,0x4154d0,0x4694f0]:last_calls.append(hex(address))
# Hook only named native host boundaries; unrelated original instructions do
# not cross into Python. This preserves native logic and speeds long traces.
for address in sorted(set(external)|{SOUND_SET_PAN,SOUND_GET_STATUS,SOUND_STOP,0x433710,0x46c9ea,0x46d04a,0x46c941,0x46ca4f,0x412990,0x413230,0x413840,0x413d90,0x43967b,0x439ed0,0x439f70,0x40d640,0x4643d0,0x464440,0x467170,0x4154d0,0x4694f0,0x40b5e0,0x40aa16,0x40aa0a,0x4273f0,0x45fe60,0x463c10,0x44e660,0x44e8e0,0x4390f0,0x4391ab,0x43a21c,0x43a3c4,0x43a3d1,0x43a3f6,0x437810,0x437980,0x409b5a,0x409e5f,0x4391c0}):v.hook_add(UC_HOOK_CODE,code_hook,begin=address,end=address)
for address in (0x437a80,0x42b029):v.hook_add(UC_HOOK_CODE,code_hook,begin=address,end=address)
for address in (0x40cf60,0x40caa0,0x40c8b0,0x428670,0x4286f0,0x428750,0x42a800,0x42a1f0,0x42bd90,0x42b750,0x42e3e0,0x42df00):v.hook_add(UC_HOOK_CODE,code_hook,begin=address,end=address)
v.hook_add(UC_HOOK_CODE,code_hook,begin=0x452a60,end=0x452a60)
v.hook_add(UC_HOOK_CODE,code_hook,begin=0x412720,end=0x412720)
resource_fields={0x4b0c44:'scoreStored',0x4b0c48:'power',0x4b0c78:'pivStored',0x4b0ccc:'rank'}
def resource_write(v,access,address,size,value,unused):
 if address in resource_fields and size==4:
  previous=get(address,'I')[0]
  if previous!=value&0xffffffff:events.append({'frame':frame,'kind':'resourceWrite','field':resource_fields[address],'instruction':hex(v.reg_read(UC_X86_REG_EIP)),'before':struct.unpack('<i',struct.pack('<I',previous))[0],'after':struct.unpack('<i',struct.pack('<I',value&0xffffffff))[0]})
v.hook_add(UC_HOOK_MEM_WRITE,resource_write,begin=0x4b0c44,end=0x4b0ccc)
def anm_vm_write(v,access,address,size,value,unused):
 vm=args.trace_anm_vm_address
 if vm is None:return
 fields=[(0x3c4,2,'interrupt'),(0x3e4,2,'sprite'),(0x3e6,2,'bank'),(0x3ea,2,'script'),(0x3ec,4,'scriptBase'),(0x3f0,4,'instruction')]
 names=[name for offset,width,name in fields if address<vm+offset+width and address+size>vm+offset]
 if names:anm_vm_writes.append({'inputTick':frame,'vm':hex(vm),'fields':names,'address':hex(address),'bytes':size,'beforeHex':bytes(v.mem_read(address,size)).hex(),'valueHex':hex(value),'instruction':hex(v.reg_read(UC_X86_REG_EIP)),'stackCodeWords':[hex(word) for word in get(v.reg_read(UC_X86_REG_ESP),'32I') if 0x400000<=word<0x498000]})
if args.trace_anm_vm_address is not None:v.hook_add(UC_HOOK_MEM_WRITE,anm_vm_write,begin=args.trace_anm_vm_address+0x3c4,end=args.trace_anm_vm_address+0x3f3)
def call(address,*values,budget=2000000):
 budget=max(budget,args.instruction_budget)
 put(SP,'I',STOP)
 for index,value in enumerate(values):put(SP+4+index*4,'I',value)
 v.reg_write(UC_X86_REG_ESP,SP)
 try:v.emu_start(address,STOP,count=budget)
 except Exception as error:
  fault={'schema':'th12-native-provider-fault/1','partial':True,'wholeGameOracle':False,'originalExeSha256':SHA,'replay':fixture.get('replay') if fixture else None,'replaySha256':fixture.get('replaySha256') if fixture else None,'inputTick':frame,'entry':hex(address),'error':str(error),'registers':{name:hex(v.reg_read(register)) for name,register in [('eip',UC_X86_REG_EIP),('eax',UC_X86_REG_EAX),('ebx',UC_X86_REG_EBX),('ecx',UC_X86_REG_ECX),('edx',UC_X86_REG_EDX),('esi',UC_X86_REG_ESI),('edi',UC_X86_REG_EDI),('ebp',UC_X86_REG_EBP),('esp',UC_X86_REG_ESP)]},'stackWords':get(v.reg_read(UC_X86_REG_ESP),'8I'),'managerPointers':{hex(pointer):hex(get(pointer,'I')[0]) for pointer in [0x4b43cc,0x4b4518,0x4b451c,0x4ce8cc]},'recentNativeCalls':list(last_calls),'scope':'Unmodified native failure boundary; no partial trace is accepted as a whole-game oracle.'}
  fault_output=args.output.with_suffix('.fault.json');fault_output.parent.mkdir(parents=True,exist_ok=True);fault_output.write_text(json.dumps(fault,indent=2)+'\n',encoding='utf-8')
  if isinstance(globals().get('frames'),list) and 'write_trace' in globals():write_trace(fault)
  raise RuntimeError(f'{error}; fixtureFrame={frame}; EIP={v.reg_read(UC_X86_REG_EIP):08x}; stack={get(v.reg_read(UC_X86_REG_ESP),"IIIIIIII")}; recent native calls={list(last_calls)}; fault={fault_output}') from error
 if v.reg_read(UC_X86_REG_EIP)==0x433710 and events and events[-1]['kind']=='nativeGameOverSceneEntered':
  boundary=events[-1]
  if isinstance(globals().get('frames'),list) and 'write_trace' in globals():write_trace(scene_boundary=boundary)
  print(json.dumps({'output':str(args.output),'sceneBoundary':boundary,'wholeGameOracle':False}));raise SystemExit(0)
 if v.reg_read(UC_X86_REG_EIP)!=STOP:
  boundary={'schema':'th12-native-provider-budget-boundary/1','partial':True,'wholeGameOracle':False,'originalExeSha256':SHA,'replay':fixture.get('replay') if fixture else None,'replaySha256':fixture.get('replaySha256') if fixture else None,'inputTick':frame,'entry':hex(address),'error':'Native instruction budget exhausted before function returned','instructionBudget':budget,'eip':hex(v.reg_read(UC_X86_REG_EIP)),'recentNativeCalls':list(last_calls),'scope':'Probe resource limit, not evidence that the original game failed. The incomplete tick is excluded from the captured native prefix.'}
  boundary['registers']={name:hex(v.reg_read(register)) for name,register in [('eax',UC_X86_REG_EAX),('ebx',UC_X86_REG_EBX),('ecx',UC_X86_REG_ECX),('edx',UC_X86_REG_EDX),('esi',UC_X86_REG_ESI),('edi',UC_X86_REG_EDI),('ebp',UC_X86_REG_EBP),('esp',UC_X86_REG_ESP)]}
  boundary['stackWords']=get(v.reg_read(UC_X86_REG_ESP),'16I')
  if 0x455630<=v.reg_read(UC_X86_REG_EIP)<0x458730:
   try:
    vm=v.reg_read(UC_X86_REG_EDI);base,pc=get(vm+0x3ec,'II')
    boundary['anmInterruptContext']={'vm':hex(vm),'bankScriptSprite':get(vm+0x3e6,'H')+get(vm+0x3ea,'H')+get(vm+0x3e4,'H'),'clock':get(vm+0x6c,'i')[0],'pendingInterrupt':get(vm+0x3c4,'H')[0],'scriptBase':hex(base),'instruction':hex(pc),'scanPointer':hex(v.reg_read(UC_X86_REG_ESI)),'flags':get(vm+0x47c,'II'),'baseBytes':bytes(v.mem_read(base,32)).hex(),'scanBytes':bytes(v.mem_read(v.reg_read(UC_X86_REG_ESI),32)).hex()}
   except Exception as inspection_error:boundary['anmInterruptContextError']=str(inspection_error)
  boundary_output=args.output.with_suffix('.budget.json');boundary_output.parent.mkdir(parents=True,exist_ok=True);boundary_output.write_text(json.dumps(boundary,indent=2)+'\n',encoding='utf-8')
  if isinstance(globals().get('frames'),list) and 'write_trace' in globals():write_trace(fault=boundary)
  raise RuntimeError('Native instruction budget exceeded at '+hex(v.reg_read(UC_X86_REG_EIP))+'; inputTick='+str(frame)+'; boundary='+str(boundary_output))
 return v.reg_read(UC_X86_REG_EAX)
if args.routine not in routines:raise SystemExit('Unknown original routine')
if args.native_items:
 if call(0x46ea5c,1)!=1 or call(0x47562a)!=1:raise RuntimeError('Original CRT heap/thread initialization failed')
 # Original floating formatter initializer475a4b and pointer-table encoding
 #47c20c (called by472d6b) must both run before46cad8 can format doubles.
 call(0x475a4b,0);call(0x47c20c)
 score_data=call(0x43d050,budget=50000000)
 if not score_data or get(0x4b451c,'I')[0]!=score_data:raise RuntimeError('Original score/history constructor failed')
 # Original replay-stage restore43c7e9..43c7f3 resets the RNG seed/counter
 # after pre-game constructors; clean score defaults must not advance input0.
 put(0x4ce568,'H',args.seed&65535);put(0x4ce56c,'I',0)
 # Application startup42ea3e..42ea54 loads text.anm bank0 and publishes
 # global4cee70 before Spell/MSG CPU managers run. With that startup binding
 # absent, MSG41f900 dereferences resource0, whose mapped FS/SEH bytes are
 # stack pointers, and constructs an invalid ANM script. Execute the real
 # resource-loader boundary and bind its actual returned resource, not a
 # fabricated script or a skipped interrupt.
 v.reg_write(UC_X86_REG_EBX,0x4a07a0);v.reg_write(UC_X86_REG_ECX,0)
 text_resource=call(0x45fe60);put(0x4cee70,'I',text_resource)
 if not text_resource:raise RuntimeError('Original text ANM startup resource loading failed')
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
if args.native_lasers:
 # Actual4281c0 allocates the manager, registers4283d0 at priority20 and loads
 # bullet.anm. Its native427fd0 registration publishes the list sentinel and
 # resource holder; no synthetic laser nodes or collision results are supplied.
 laser_manager=call(0x4281c0)
 if not laser_manager or get(0x4b44f4,'I')[0]!=laser_manager:raise RuntimeError('Original laser manager initialization failed')
 if get(laser_manager+0x464,'I')[0]!=laser_manager+0x10:raise RuntimeError('Original laser manager sentinel registration failed')
SPAWN=0x2070000;put(SPAWN,'fffiiiII',0,0,0,0,0,args.initial_health,0,0) # Retail startup 40f0f8: request health 0x2710.
root_enemy=call(0x412990,routines[args.routine][0],SPAWN)
# Retail Stage10 creates main before input0. Its Enemy18 encounter on input0
# only clears the constructor's20000 guard; there is no extra startup traversal.
# Explicit priming remains available solely for older passive fixtures.
if args.prime_creation_guard:call(0x413190,MANAGER)
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

def capture_lasers():
 result=[];pointer=get(laser_manager+0x18,'I')[0]
 while pointer:
  kind={0x4a0654:0,0x4a0704:1,0x4a06ac:2}.get(get(pointer,'I')[0])
  if kind is None:raise RuntimeError('Unrecognized original laser vtable')
  # Timed42ad74 replaces the base+80 serial with the requested lookup ID.
  # Retain that actual raw field and a separate observed-object identity;
  # never equate it with the candidate manager's independent serial number.
  if pointer not in laser_ids:laser_ids[pointer]={'id':len(laser_ids)+1,'firstObservedInputTick':frame}
  row={'observedObject':laser_ids[pointer],'address':hex(pointer),'nativeField80':get(pointer+0x80,'I')[0],'kind':kind,'phase':get(pointer+0xc,'i')[0],'position':get(pointer+0x50,'fff'),'geometry':get(pointer+0x68,'4f'),'headOffset':get(pointer+0x78,'f')[0],'age':get(pointer+0x14,'iif'),'retirePending':get(pointer+0x7c,'B')[0],'cancellable':get(pointer+0x7d,'B')[0]}
  if args.trace_laser_visual_ticks and args.trace_laser_visual_ticks[0]<=frame<=args.trace_laser_visual_ticks[1]:
   vms=[]
   for role,offset in [('main',{0:0x63c,1:0x660,2:0x634}[kind]),('cap',{0:0xaf0,1:0xb14,2:0xae8}[kind])]:
    vm=pointer+offset;color=get(vm+0x3bc,'I')[0]
    vms.append({'role':role,'flags':get(vm+0x47c,'II'),'blendBits':(get(vm+0x47c,'I')[0]>>5)&7,'alphaByte':color>>24,'primaryColor':color,'layer':get(vm+0x20,'i')[0],'bankScriptSprite':get(vm+0x3e6,'H')+get(vm+0x3ea,'H')+get(vm+0x3e4,'H'),'spriteSize':get(vm+0x58,'ff'),'scale':get(vm+0x40,'ff'),'rotation':get(vm+0x24,'fff'),'enginePosition':get(vm+0x430,'fff'),'scriptPosition':get(vm+0x424,'fff'),'clock':get(vm+0x6c,'i')[0]})
   row['embeddedAnimation']=vms
  result.append(row)
  pointer=get(pointer+8,'I')[0]
  if len(result)>256:raise RuntimeError('Invalid original laser manager list')
 return result
def capture_player():
 return {'position':get(PLAYER+0x97c,'ff'),'fixedPosition':get(PLAYER+0x988,'ii'),'state':get(PLAYER+0xa28,'i')[0],'stateAge':get(PLAYER+0xa34,'i')[0],'invulnerability':get(PLAYER+0xc404,'i')[0],'shootAge':get(PLAYER+0xc424,'i')[0],'bodyAnimationTime':get(PLAYER+0x14+0x6c,'i')[0],'lifeAndBombCounters':get(0x4b0c98,'iiii'),'power':get(0x4b0c48,'i')[0],'speedsFixed':get(PLAYER+0x990,'iiii'),'shtMoveFloats':get(get(PLAYER+0xa2c,'I')[0]+0x10,'ffff'),'movementFactor':get(PLAYER+0x825c,'f')[0],'direction':get(PLAYER+0xa1c,'i')[0],'deltaFixed':get(PLAYER+0xa14,'ii')}
def capture_items():
 result=[]
 for slot in range(2664):
  item=item_manager+0x14+slot*0x9d8;state,kind=get(item+0x9b0,'ii')
  #4277d8 writes the delayed-collection timer at9c0;425c5c decrements it.
  #9b8 retains the originally spawned kind (e.g.9 for a cancelled-bullet star).
  if state:result.append({'slot':slot,'state':state,'type':kind,'position':get(item+0x96c,'ff'),'velocity':get(item+0x978,'ff'),'age':get(item+0x98c,'i')[0],'colorTimer':get(item+0x9a0,'i')[0],'attractionSpeed':get(item+0x9bc,'f')[0],'birthKind':get(item+0x9b8,'i')[0],'delay':get(item+0x9c0,'i')[0]})
 return result
def capture_sources():
 result=[]
 for slot in range(128):
  source=PLAYER+0x8988+slot*0x74;flags=get(source+0x70,'I')[0]
  if flags&1:result.append({'slot':slot,'radius':get(source,'f')[0],'radialGrowth':get(source+4,'f')[0],'rotation':get(source+8,'f')[0],'rotationGrowth':get(source+12,'f')[0],'width':get(source+16,'f')[0],'height':get(source+20,'f')[0],'position':get(source+0x18,'fff'),'heading':get(source+0x3c,'f')[0],'timer':get(source+0x4c,'ii'),'damage':get(source+0x60,'i')[0],'accumulatedDamage':get(source+0x64,'i')[0],'limit':get(source+0x68,'i')[0],'cadence':get(source+0x6c,'i')[0],'flags':flags})
 return result

def camera_nodes():
 scheduler=get(0x4ce89c,'I')[0];node=get(scheduler+0x18,'I')[0];seen=set()
 while node:
  if node in seen:raise RuntimeError('Invalid original update scheduler list')
  seen.add(node);owner,next_node=get(node,'II')
  if get(owner,'i')[0]==14:yield owner
  node=next_node

def tick_camera_effects():
 # Traverse the original registered update list at its actual priority14
 # point. Nodes created by Bomb17 start next tick. The original callback
 # return7 protocol calls its destructor (+10), which unlinks its real
 # update/render registrations through452f00; return0 directly unlinks.
 for node in camera_nodes():
  callback=get(node+8,'I')[0]
  if not callback or not(get(node+4,'I')[0]&2):continue
  v.reg_write(UC_X86_REG_ECX,get(node+0x20,'I')[0]);result=call(callback)
  camera_update_counts[hex(callback)]+=1
  if result==7:
   destructor=get(node+0x10,'I')[0]
   if destructor:v.reg_write(UC_X86_REG_ECX,get(node+0x20,'I')[0]);call(destructor)
  elif result==0:
   v.reg_write(UC_X86_REG_ECX,node);v.reg_write(UC_X86_REG_EDX,get(0x4ce89c,'I')[0]);call(0x462890)
  elif result!=1:raise RuntimeError('Unsupported actual priority14 scheduler callback result '+str(result))

def capture_camera_effects():
 rows=[]
 for node in camera_nodes():
  data=get(node+0x20,'I')[0]
  rows.append({'node':hex(node),'callback':hex(get(node+8,'I')[0]),'flags':get(node+4,'I')[0],'object':hex(data),'rawObjectHex':bytes(v.mem_read(data,0x44)).hex(),'timer':get(data+0x30,'iif')})
 return {'shake':get(0x4cee04,'ff'),'nodes':rows}

def capture_message_event():
 stage=get(0x4b43e4,'I')[0];msg=get(stage+0x6d30,'I')[0]
 row={'frame':frame,'kind':'nativeMsgFrame','stageManager':hex(stage),'message':hex(msg),'held':get(0x4d49d0,'I')[0],'pressed':get(0x4d49dc,'I')[0],'released':get(0x4d49e0,'I')[0]}
 if msg:
  pc=get(msg+0x64,'I')[0];origin=next((a for a in heap_allocations if a['source'].startswith('retailFile:') and int(a['address'],16)<=pc<int(a['endExclusive'],16)),None)
  row.update({'entry':get(msg,'i')[0],'clock':get(msg+0x18,'iif'),'wait':get(msg+0x2c,'iif'),'gate8c':get(msg+0x8c,'i')[0],'flags90':get(msg+0x90,'I')[0],'pc':hex(pc),'pcFile':origin['source'][len('retailFile:'):] if origin else None,'pcOffset':pc-int(origin['address'],16) if origin else None,'instructionHeader':get(pc,'HBB'),'textLineCounters':get(msg+0x94,'iii')})
 return row

def reset_camera_at_draw42():
 global camera_draw_reset_count
 # Actual CPU-only prefix of upper ANM Draw42: fldz/fst clears the world
 # camera offsets and ANMManager+b0/b4 before drawing upper layers. End at
 #460e62, before the GPU-facing4611d0 call. Restore the pushed EDI/ESP as
 # the original460e6c epilogue would; no random/state substitute is written.
 saved_sp=v.reg_read(UC_X86_REG_ESP);saved_edi=v.reg_read(UC_X86_REG_EDI)
 v.emu_start(0x460e40,0x460e62,count=20)
 if v.reg_read(UC_X86_REG_EIP)!=0x460e62:raise RuntimeError('Original Camera Draw42 reset prefix did not reach its GPU boundary')
 v.reg_write(UC_X86_REG_ESP,saved_sp);v.reg_write(UC_X86_REG_EDI,saved_edi);camera_draw_reset_count+=1
def capture_friendly():
 result=[]
 for slot in range(256):
  projectile=PLAYER+0xa58+slot*0x78;phase=get(projectile+0x48,'i')[0]
  if not phase:continue
  result.append({'id':friendly_ids.get(slot),'slot':slot,'phase':phase,'age':get(projectile+4,'i')[0],'position':get(projectile+0x14,'ff'),'velocity':get(projectile+0x20,'ff'),'speed':get(projectile+0x2c,'f')[0],'angle':get(projectile+0x30,'f')[0],'damage':get(projectile+0x60,'i')[0],'hitbox':get(projectile+0x68,'ff'),'targetID':get(projectile+0x64,'I')[0],'hitLastFrame':get(projectile+0x58,'i')[0]})
 return result
def capture_economy():
 return {'scoreStored':get(0x4b0c44,'i')[0],'power':get(0x4b0c48,'i')[0],'pivStored':get(0x4b0c78,'i')[0],'maxPivStored':get(0x4b0cf4,'i')[0],'rank':get(0x4b0ccc,'i')[0],'lifeBombCounters':get(0x4b0c98,'iiii'),'ufoColors':get(0x4b0c4c,'iii'),'ufoCountState':get(0x4b0c58,'ii'),'ufoAge':get(ufo_manager+0x14,'i')[0],'ufoKind':get(ufo_manager+0x30,'i')[0],'ufoEnemy':get(ufo_manager+0x38,'I')[0],'ufoFill':get(ufo_manager+0x50,'f')[0],'ufoBuckets':get(ufo_manager+0x48,'ii'),'spellFlagsBonus':get(bonus_manager+0x7c,'II'),'bombState':get(bomb_manager+0x3c,'i')[0]}

def write_trace(fault=None,scene_boundary=None):
 # A failed tick is never exported as a completed frame. Retain the preceding
 # actual native prefix for diagnosis, explicitly bound to its stop boundary.
 completed=len(frames)-1
 data={'schema':'th12-native-enemy-provider/1','originalExeSha256':SHA,'retailInputs':files,'scope':'original native selected managers with host-only loaders and audio output boundaries; startup/world completeness remains partial','partial':True,'wholeGameOracle':False,'requestedFrames':args.frames,'completedFrames':completed,'requestedTraceCompleted':fault is None and completed==args.frames,'fault':fault,'routine':args.routine,'initialHealth':args.initial_health,'damageFixture':args.damage_fixture,'requestedSeed':args.seed,'rngInitialized':True,'nativePlayerExecuted':args.native_player,'nativeItemsUfoSpellMsgExecuted':args.native_items,'bombInputEnabled':args.native_items,'activeCollisionExecuted':args.active_collision,'nativeAnmExecuted':not args.without_anm,'animationSnapshotsCaptured':not(args.without_anm or args.omit_animation_capture),'nativeBulletUpdateExecuted':args.native_bullets,'scoreHistoryInitialCondition':{'constructor':'0x43d050','fileBoundary':'scoreth12.dat absent, original default initialization','replayRngRestore':'0x43c7e9..0x43c7f3'} if args.native_items else None,'itemFields':{'birthKind':'0x9b8','delay':'0x9c0'},'fixtureInput':str(args.input) if args.input else None,'replay':fixture.get('replay') if fixture else None,'replaySha256':fixture.get('replaySha256') if fixture else None,'inputAddresses':{'held':'0x4d49d0','previousHeld':'0x4d49d4','pressed':'0x4d49dc','released':'0x4d49e0'},'creationGuardPrimed':args.prime_creation_guard,'intercepts':dict(intercepts),'frames':frames,'events':[event for event in events if fault is None or event['frame']<fault['inputTick']]}
 if scene_boundary:
  data['requestedTraceCompleted']=False
  data['stoppedAtSceneBoundary']=True
  data['sceneBoundary']=scene_boundary
  data['events']=[event for event in events if event['frame']<scene_boundary['frame']]
 if crt_host:data['crtCpuHost']=crt_host.metadata()
 data['nativeLaserUpdateExecuted']=args.native_lasers
 data['nativeCameraUpdateExecuted']=args.native_camera_effects
 data['cameraScope']={'enabled':args.native_camera_effects,'constructor':'0x452a60','priority':14,'registeredList':'[4ce89c]+18; node owner callback+8 and userdata+20','callbacksExecuted':dict(camera_update_counts),'retirement':'Original return7 destructor callback; original462890 for return0','applicationContext':{'address':'0x20d0000','flagsAt60':0,'syntheticUnpausedStartup':True} if args.native_camera_effects else None,'scope':'Original registered camera CPU callbacks/RNG and destructor lifecycle; application startup, pause conditions and draw output remain partial. No synthetic random burn.'}
 data['cameraDrawResetBoundary']={'executed':args.native_camera_effects,'nativeInstructions':['0x460e40','0x460e62 exclusive'],'calls':camera_draw_reset_count,'gpuExecuted':False,'scope':'Actual Draw42 CPU zero-write prefix only, before4611d0. CPU frame snapshots show offsets after that reset; drawing and visible displacement are not an image oracle.'}
 data['messageDiagnosticScope']={'inputTickRange':args.trace_msg_ticks,'scope':'Actual frame-end MSG timer/PC/flags and412720 gate reads, with original replay held/pressed/released values. No timer, gate or message instruction is altered.'}
 data['textAnmStartupBinding']={'enabled':args.native_items,'source':'42ea3e..42ea54: text.anm bank0 returned by45fe60 -> global4cee70','resource':hex(text_resource) if args.native_items else None,'scope':'Original CPU ANM resource layout and scripts loaded at a host texture-free boundary. Application startup itself remains partial.'}
 data['instructionBudgetPerCall']=args.instruction_budget
 data['worldFrameCaptureScope']={'enabled':not args.omit_world_frame_capture,'actualInputTicksExecuted':executed_input_ticks,'completedSnapshots':len(frames)-1,'scope':'Optional event-only CPU diagnostics execute the same original replay inputs and schedule, but omitted snapshots cannot be compared as a native frame prefix.'}
 data['hostHeapAudit']={'scope':'Bounded disjoint host arena; allocations never reuse freed storage. Native allocator and retail file loader share the same checked allocator. Free calls are recorded but do not reclaim host storage.','regions':{name:[hex(a),hex(b)] for name,(a,b) in heap_regions.items()},'allocationCount':len(heap_allocations),'highWaterExclusive':hex(heap),'remainingBytes':heap_regions['heap'][1]-heap,'overlapsFixedRegions':False,'allocations':heap_allocations,'nativeFreeCalls':heap_frees}
 if args.trace_anm_vm_address is not None:data['anmVmWriteAudit']={'vm':hex(args.trace_anm_vm_address),'scope':'Original CPU writes observed without alteration to resource selection, interrupt and script pointers.','writes':anm_vm_writes}
 data['laserScope']={'enabled':args.native_lasers,'constructor':'0x4281c0' if args.native_lasers else None,'registrar':'0x427fd0' if args.native_lasers else None,'update':'0x428270' if args.native_lasers else None,'priority':20 if args.native_lasers else None,'scope':'Original manager and object vtable updates, embedded ANM and Player line collision. Direct callback fixture has an unpaused world; whole application pause/dialogue scheduler and drawing remain outside scope.'}
 data['laserStateCapture']={'enabled':args.native_lasers,'kinds':{'0':'moving','1':'timed','2':'curve'},'fields':['observed object identity','nativeField80','kind','phase','position','angle/length/width/speed','headOffset','age previous/current/value','retirePending','cancellable'],'nativeField80':'Moving/Curve serial; Timed lookup ID, overwritten by42ad74. Candidate serial IDs are a separate namespace.','curveNodesCaptured':False,'gpuExecuted':False,'scope':'Observed original CPU object state. Observation IDs cannot detect a birth retiring before the first captured frame. Curve node geometry and draw vertices are not captured, and the current world comparator has no laser-field acceptance claim.'}
 data['laserEmbeddedAnimationCapture']={'inputTickRange':args.trace_laser_visual_ticks,'enabled':bool(args.native_lasers and args.trace_laser_visual_ticks),'scope':'Actual Main/Cap embedded VM state, including flags/blend/alpha/layer/resource selection/pose. No GPU execution, draw vertices or pixel oracle.'}
 data['audioOutputBoundary']={'audibleOutputExecuted':False,'scope':'Host-only sound buffers with native queue updates retained. Silent IDirectSoundBuffer GetStatus, SetPan and Stop responses; no audio oracle.','methods':{'vtable+0x24':'GetStatus returns no playing buffer','vtable+0x40':'SetPan returns success','vtable+0x48':'Stop returns success'}}
 data['nativeGameOverSceneBoundary']=next((event for event in events if event['kind']=='nativeGameOverSceneEntered'),None)
 data['dynamicTextOutputBoundary']={'enabled':args.capture_text_output,'functions':['0x44e660','0x44e8e0'] if args.capture_text_output else [],'scope':'Records actual CPU formatting/layout wrapper arguments; GDI rasterization, CPU glyph-pixel transforms and D3D texture upload suppressed. No font pixel/size comparison and no visual oracle. Actual Spell/Enemy/ECL/ANM/RNG logic retained.'}
 args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
# Probe bootstrap consumers execute the initialization prefix only. Validate
# the requested trace length at the actual loop boundary, not during bootstrap.
if args.frames<0 or (fixture and args.frames>len(fixture['inputs'])):raise SystemExit('Requested trace exceeds supplied replay inputs')
executed_input_ticks=0
frames=[{'frame':-1,'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0],'enemies':capture(),'animations':capture_animations() if not (args.without_anm or args.omit_animation_capture) else [],'player':capture_player() if args.native_player else None}]
for frame in range(args.frames):
 if fixture:
  # Original43b994 saves previous held before43ba02/43ba1b/43ba35 write
  # the three replay words.49d8 is not the pressed mask.
  sample=fixture['inputs'][frame];input_mask=0xffffffff if args.native_items else 0xfffffffd
  put(0x4d49d4,'I',get(0x4d49d0,'I')[0]);put(0x4d49d0,'I',sample['held']&input_mask)
  put(0x4d49dc,'I',sample.get('pressed',0)&input_mask);put(0x4d49e0,'I',sample.get('released',0)&input_mask)
  if not args.native_player:put(PLAYER+0x97c,'fff',sample['xFixed']/128,sample['yFixed']/128,0);put(0x4b0c48,'i',sample['power']);put(0x4b0ccc,'i',sample['rank'])
 if not args.without_anm:v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x4610d0)
 if args.native_camera_effects:tick_camera_effects()
 if args.native_player:call(0x436ba0,PLAYER)
 if args.native_items:v.reg_write(UC_X86_REG_EAX,bomb_manager);call(0x406ce0)
 call(0x413190,MANAGER)
 if args.native_items:v.reg_write(UC_X86_REG_EAX,ufo_manager);call(0x44a2d0)
 if args.native_lasers:v.reg_write(UC_X86_REG_EDI,laser_manager);call(0x428270)
 if args.native_bullets:v.reg_write(UC_X86_REG_ESI,BULLETS);call(0x40a020)
 if args.native_items:
  call(0x425c20,item_manager);v.reg_write(UC_X86_REG_EDI,bonus_manager);call(0x40db40);v.reg_write(UC_X86_REG_EDI,stage_manager);call(0x41e020)
 if not args.without_anm:v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0)
 if args.native_camera_effects:reset_camera_at_draw42()
 executed_input_ticks+=1
 if args.trace_msg_ticks and args.trace_msg_ticks[0]<=frame<=args.trace_msg_ticks[1]:events.append(capture_message_event())
 if args.omit_world_frame_capture:continue
 frames.append({'frame':frame,'rngSeed':get(0x4ce568,'H')[0],'rngCalls':get(0x4ce56c,'I')[0],'enemies':capture(),'bullets':capture_bullets() if args.native_bullets else [],'animations':capture_animations() if not (args.without_anm or args.omit_animation_capture) else [],'player':capture_player() if args.native_player else None,'items':capture_items() if args.native_items else [],'sources':capture_sources() if args.native_player else [],'friendly':capture_friendly() if args.native_player else [],'economy':capture_economy() if args.native_items else None})
 if args.native_lasers:frames[-1]['lasers']=capture_lasers()
 if args.native_camera_effects:frames[-1]['cameraEffects']=capture_camera_effects()
write_trace();print(args.output)
