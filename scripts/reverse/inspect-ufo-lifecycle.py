"""Read-only native UFO lifecycle/HUD probe, not restored game source.

Builds a controlled original selected-manager world through the independent
provider, then executes original UFO/Enemy/item/ANM functions. Fixture writes
are stated per case; host file loaders and audio output remain boundaries.
No candidate game module, original file or accepted golden is changed.
"""
import contextlib,copy,io,json,pathlib,runpy,sys
TITLE=pathlib.Path(__file__).resolve().parents[2]
out=TITLE/'artifacts/reverse/ufo-lifecycle-inspection.json'
sys.argv=[str(TITLE/'scripts/reverse/native-enemy-provider.py'),'--frames','0','--input',str(TITLE/'artifacts/reverse/world-input-demo2.json'),'--native-player','--native-items','--native-bullets','--output',str(TITLE/'artifacts/reverse/ufo-lifecycle-bootstrap.json')]
with contextlib.redirect_stdout(io.StringIO()):loaded=runpy.run_path(sys.argv[0])
g=loaded['call'].__globals__;v=g['v'];put=g['put'];get=g['get'];call=g['call']
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDI,UC_X86_REG_ESI,UC_X86_REG_ESP
manager=g['ufo_manager'];enemy_manager=g['MANAGER'];stage=g['stage_manager'];player=g['PLAYER']
# Restore actual retail PE/globals and synthetic heap/ANM memory between cases.
# The original ECL bytes at 0x4000000 are read-only and shared by all fixtures.
regions=[(0x400000,(g['pe'].OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x2000000,0x2000000),(0x5000000,0x3000000)]
memory=[(a,bytes(v.mem_read(a,n))) for a,n in regions];registers=v.context_save()
saved={k:copy.deepcopy(g[k]) for k in ['heap','resource_base','resource_by_name','births','bullet_ids','next_bullet_id','last_spawn_request']}
trace=[]
def observe(v,address,size,unused):
 row={'entry':hex(address),'frame':g['frame']}
 if address==0x4214b0:row.update({'stage':get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0],'newSlot':get(v.reg_read(UC_X86_REG_ESP)+8,'i')[0],'rotation':get(stage+0x6788,'i')[0]})
 elif address==0x455630:
  vm=get(v.reg_read(UC_X86_REG_ESP)+4,'I')[0];irq=get(vm+0x3c4,'h')[0]
  if not irq:return
  row.update({'vm':hex(vm),'script':get(vm+0x3ea,'H')[0],'irq':irq})
 elif address==0x421a60:row['rotationBefore']=get(stage+0x6788,'i')[0]
 elif address==0x414af0:row['enemyId']=get(v.reg_read(UC_X86_REG_ESI)+0x27b4,'I')[0]
 trace.append(row)
for a in [0x4214b0,0x421a60,0x455630,0x414af0,0x44a890,0x44af00]:v.hook_add(UC_HOOK_CODE,observe,begin=a,end=a)
def reset():
 for a,data in memory:v.mem_write(a,data)
 v.context_restore(registers)
 for k,value in saved.items():g[k]=copy.deepcopy(value)
 g['events'].clear();g['intercepts'].clear();trace.clear();g['frame']=0
def snapshot():
 enemy=get(manager+0x34,'I')[0]
 items=g['capture_items']()
 for item in items:
  ptr=g['item_manager']+0x14+item['slot']*0x9d8;item.update({'hintHandle':get(ptr+0x968,'I')[0],'primaryScriptClock':get(ptr+0x3ea,'H')+get(ptr+0x6c,'i'),'secondaryScriptClock':get(ptr+0x4b4+0x3ea,'H')+get(ptr+0x4b4+0x6c,'i')})
 return {'inventory':get(0x4b0c4c,'7i'),'pivStored':get(0x4b0c78,'i')[0],'hudRotation':get(stage+0x6788,'i')[0],'manager':{'age':get(manager+0x14,'i')[0],'summoned':get(manager+0x2c,'i')[0],'kind':get(manager+0x30,'i')[0],'enemyId':get(manager+0x38,'I')[0],'anmIds':get(manager+0x3c,'3I'),'buckets':get(manager+0x48,'ii'),'fill':get(manager+0x50,'f')[0],'rewardDisplay':get(manager+0x54,'4f')+get(manager+0x64,'ii')},'enemy':{'id':get(enemy+0x27b4,'I')[0],'position':get(enemy+0x1074,'ff'),'health':get(enemy+0x2648,'i')[0],'healthFlags':get(enemy+0x265c,'I')[0],'flags':get(enemy+0x26f8,'I')[0],'private0':get(enemy+0x127c,'i')[0]} if enemy else None,'items':items,'animations':g['capture_animations'](),'rng':get(0x4ce568,'H')+get(0x4ce56c,'I')}
def append(color):v.reg_write(UC_X86_REG_ESI,0x4b0c4c);v.reg_write(UC_X86_REG_EDI,color);call(0x422e80)
def ufo_tick():v.reg_write(UC_X86_REG_EAX,manager);call(0x44a2d0)
def summon(kind=1):
 put(0x4b0c4c,'7i',*[kind if kind>0 else 1,kind if kind>0 else 2,kind if kind>0 else 3,3,3,0,kind]);v.reg_write(UC_X86_REG_ESI,manager);v.reg_write(UC_X86_REG_EAX,kind);call(0x44a8a0)
def finish_case(name,fixture,before,extra=None):
 return {'name':name,'fixtureWrites':fixture,'before':before,'after':snapshot(),'calls':list(trace),'events':copy.deepcopy(g['events']),'extra':extra}
cases=[]
reset();steps=[]
for color in [1,2,2,3,1]:append(color);steps.append(snapshot())
cases.append(finish_case('inventory_mismatch_hud_rotation',{'appendColors':[1,2,2,3,1]},None,steps))
reset()
for color in [1,2,3]:append(color)
before=snapshot();ufo_tick();cases.append(finish_case('third_token_manager19_summon',{'appendColors':[1,2,3]},before))
reset();summon();before=snapshot();put(stage+0x6d30,'i',1);ufo_tick();flagged=snapshot();call(0x413190,enemy_manager);call(0x413190,enemy_manager);ufo_tick()
cases.append(finish_case('dialogue_forced_death_callback',{'dialogue':1,'managerTicksAfterFlag':2},before,{'afterUfoFlag':flagged}))
for age,boss in [(419,False),(420,False),(421,False),(100,True)]:
 reset();summon();put(manager+0x14,'i',age);put(manager+0x18,'f',float(age));enemy=get(manager+0x34,'I')[0];put(enemy+0x127c,'i',123)
 if boss:put(enemy_manager+0x1c,'I',enemy)
 before=snapshot();ufo_tick();cases.append(finish_case('escape_signal_'+str(age)+('_boss' if boss else ''),{'ufoAge':age,'private0':123,'bossSlot0Nonzero':boss},before))
reset();summon();before=snapshot();v.reg_write(UC_X86_REG_ECX,manager);call(0x449f50);cases.append(finish_case('direct_reset_retains_enemy',{'call':'449f50(ECX=manager)'},before,{'nativeEnemiesAfterReset':g['capture']()}))
reset();summon();put(manager+0x48,'ii',3,5);put(manager+0x50,'f',.75);before=snapshot();path=[]
for tick in range(830):
 g['frame']=tick
 call(0x413190,enemy_manager);ufo_tick();v.reg_write(UC_X86_REG_ESI,g['BULLETS']);call(0x40a020);call(0x425c20,g['item_manager']);v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0)
 if tick%60==0 or not get(manager+0x38,'I')[0]:path.append({'inputTick':tick,'state':snapshot()})
 if not get(manager+0x38,'I')[0]:break
cases.append(finish_case('natural_escape_without_hurt',{'kind':1,'buckets':[3,5],'fill':.75,'playerPosition':[0,400],'nativePlayerTickOmitted':True,'hurtQueriesSuppressed':True,'schedules':['Enemy18','UFO19','Bullet21','Item22','ANM27']},before,path))
reset();summon();before=snapshot();pos=g['SPAWN'];put(pos,'fff',0,128,0)
for i in range(16):call(0x4273f0,13,pos,0,0)
returned=call(0x4273f0,13,pos,0,0);cases.append(finish_case('full_token_pool',{'spawnTokenType':13,'spawnCount':17},before,{'lastReturn':hex(returned),'poolEnd':hex(g['item_manager']+0x17afd4),'sideCounter':get(g['item_manager']+0x666fe0,'i')[0]}))
for colors in [(1,1),(2,2),(3,3),(1,2),(1,3),(2,3)]:
 for token in [10,11,12]:
  reset();put(0x4b0c4c,'7i',colors[0],colors[1],0,2,2,0,0);put(g['SPAWN'],'fff',0,200,0);item=call(0x4273f0,token,g['SPAWN'],0,0);before=snapshot();call(0x425c20,g['item_manager']);shown=snapshot();call(0x425c20,g['item_manager']);persisted=snapshot();put(0x4b0c4c,'7i',1,1,1,3,3,0,1);call(0x425c20,g['item_manager'])
  cases.append(finish_case('token_hint_'+str(colors)+'_'+str(token),{'twoColors':colors,'token':token,'afterTwoTicksSetCount3':True},before,{'shown':shown,'persisted':persisted}))
for piv in [2000000,39950000]:
 reset();put(0x4b0c4c,'7i',1,1,1,3,3,0,1);put(0x4b0c78,'i',piv);put(g['SPAWN'],'fff',0,200,0);item=call(0x4273f0,14,g['SPAWN'],0,0);before=snapshot();v.reg_write(UC_X86_REG_EAX,1);v.reg_write(UC_X86_REG_ECX,item);call(0x4270b0);cases.append(finish_case('full_inventory_collect_'+str(piv),{'stockCount':3,'pivStored':piv,'maxPivStored':40000000,'collectedTokenColor0Based':1},before))
reset();put(g['SPAWN'],'fff',0,128,0);item=call(0x4273f0,13,g['SPAWN'],0,0);before=snapshot();put(stage+0x6d30,'i',1);call(0x425c20,g['item_manager']);cases.append(finish_case('dialogue_token_expiry_clock',{'tokenType':13,'dialogue':1,'ageInitially0':True},before))
reset();summon();put(manager+0x48,'ii',3,5);put(manager+0x50,'f',.75)
for tick in range(62):g['frame']=tick;call(0x413190,enemy_manager);ufo_tick()
before=snapshot();enemy=get(manager+0x34,'I')[0];put(enemy+0x2648,'i',0);call(0x413190,enemy_manager);ufo_tick();cases.append(finish_case('mature_health_zero_without_hurt',{'kind':1,'after62TicksSetHealth0':True,'buckets':[3,5],'fill':.75,'hurtQueriesSuppressed':True},before))
reset();summon();put(g['SPAWN'],'fff',0,128,0)
for i in range(16):call(0x4273f0,13,g['SPAWN'],0,0)
put(manager+0x48,'ii',3,5);put(manager+0x50,'f',.75);before=snapshot();call(0x44af00);neighbor=g['item_manager']+0x17afd4
cases.append(finish_case('aggregate_reward_full_token_pool',{'kind':1,'buckets':[3,5],'fill':.75,'occupiedTokenSlots':16,'directNativeDeathRewardCallback':True},before,{'adjacentSmallPointSlot0':{'stateType':get(neighbor+0x9b0,'ii'),'aggregateKindBucketsMultiplier':get(neighbor+0x9c8,'iiif')}}))
for card,pending,bomb,invuln,player_flag in [(0,False,False,0,0),(93,False,False,120,0),(93,False,True,0,0),(93,False,True,0,4),(103,False,True,0,0),(93,True,False,0,0),(0,True,False,0,0),(0,False,False,0,4)]:
 reset();summon()
 for tick in range(62):g['frame']=tick;call(0x413190,enemy_manager);ufo_tick()
 enemy=get(manager+0x34,'I')[0];x,y=get(enemy+0x1074,'ff');source=player+0x8988
 put(source,'4f',100,0,0,0);put(source+0x18,'fff',x,y,0);put(source+0x4c,'iifII',0,1,1,0x4b2ed0,1);put(source+0x60,'5i',80,0,999999,4,3)
 put(player+0xa30,'ii',61,62) # Native hurt query rejects a paused Player timer.
 put(player+0xc404,'i',invuln);put(player+0xc414,'I',player_flag);put(g['bonus_manager']+0x78,'II',card,(1 if card else 0)|(32 if pending else 0));put(g['bomb_manager']+0x3c,'i',1 if bomb else 0)
 before=snapshot();suppressed=g['external'].pop(0x439ed0);call(0x413190,enemy_manager);g['external'][0x439ed0]=suppressed
 cases.append(finish_case('ufo_source_spell_'+str((card,pending,bomb,invuln,player_flag)),{'nativeCircleSourceDamage':80,'nativeCircleSourceRadius':100,'nativeSourceCadence':4,'nativeSourceTimer':[0,1],'playerTimer':[61,62],'spellId':card,'spellPendingBomb':pending,'bombActiveField':bomb,'playerInvulnerability':invuln,'playerFlags':player_flag,'sourceHurtFunctionExecuted':True},before,{'healthDelta':before['enemy']['health']-get(enemy+0x2648,'i')[0],'sourceAccumulatedDamage':get(source+0x64,'i')[0]}))
result={'schema':'th12-ufo-lifecycle-inspection/1','originalExeSha256':g['SHA'],'method':'Actual original x86 execution plus manually reviewed angr pseudocode; this probe is not itself a decompiler','partial':True,'wholeGameOracle':False,'hostBoundaries':'Original constructors/ECL/ANM/item logic execute. Host allocation/file loaders and audio output are intercepted. Most cases are controlled state injections, not uninterrupted replay observations. Natural-escape fixture suppresses hurt queries and omits Player16/Bomb17/HUD25.','cases':cases}
out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8');print(out)
