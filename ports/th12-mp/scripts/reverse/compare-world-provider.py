"""Compare selected original managers with a candidate active-world capture.

This is a diagnostic, never an accepted/updated golden. Frame numbers in the
report are zero-based native input ticks (candidate frame is tick + 1). Native
and candidate enemies are paired by original birth routine/occurrence; items
and projectiles use physical pool slots when exported. Projectile age resets
are compared as lifecycle changes, never interpreted as a new pool occupant.
"""
import argparse,collections,json,pathlib
TITLE=pathlib.Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser()
p.add_argument('--native',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-native-active-demo2.json')
p.add_argument('--candidate',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-candidate-active-demo2.json')
p.add_argument('--output',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-active-comparison.json')
p.add_argument('--compare-lasers',action='store_true',help='Compare captured CPU laser objects by first observed input tick, kind and linked-list occurrence; never compare GPU output')
args=p.parse_args();native=json.loads(args.native.read_text());candidate=json.loads(args.candidate.read_text())
candidate_fixture=candidate.get('fixtureExecution')
if isinstance(candidate_fixture,dict) and 'mask' in candidate_fixture and candidate_fixture['mask']!=0:
 raise SystemExit('Active manager comparison rejects candidate diagnostic fixtureExecution.mask='+str(candidate_fixture['mask']))
for where,snapshot in [('initial',candidate.get('initial',{}))]+[('inputTick'+str(index),row) for index,row in enumerate(candidate['ticks'])]:
 if 'fixtureFlags' in snapshot and snapshot['fixtureFlags']!=0:
  raise SystemExit('Active manager comparison rejects candidate '+where+' fixtureFlags='+str(snapshot['fixtureFlags']))
if native.get('worldFrameCaptureScope',{}).get('enabled') is False:
 raise SystemExit('Event-only native diagnostics have no comparable gameplay frame prefix')
if native.get('replaySha256') and candidate.get('replaySha256') and native['replaySha256']!=candidate['replaySha256']:
 raise SystemExit('Native and candidate captures use different replay inputs')
for flag in ['nativePlayerExecuted','nativeItemsUfoSpellMsgExecuted','activeCollisionExecuted','nativeAnmExecuted','nativeBulletUpdateExecuted']:
 if not native.get(flag):raise SystemExit('Active manager comparison requires '+flag)
if args.compare_lasers:
 if not native.get('nativeLaserUpdateExecuted'):raise SystemExit('Laser comparison requires actual native Laser20 execution')
 if any('lasers' not in frame for frame in native['frames'][1:]):raise SystemExit('Laser comparison requires every native CPU snapshot to capture laser objects')
 if any('lasers' not in frame or 'nodes' not in frame['lasers'] for frame in candidate['ticks']):raise SystemExit('Laser comparison requires every candidate snapshot to capture typed laser nodes')
first={};examples=[];counts=collections.Counter();excluded={'main','MapleEnemy','Logo','LogoEnemy','ShipShadow','ShipShadowEnemy'}
def record(tick,field,a,b,actor=None):
 row={'inputTick':tick,'candidateFrame':tick+1,'field':field,'native':a,'candidate':b}
 if actor is not None:row['actor']=actor
 counts[field]+=1
 if field not in first:first[field]=row
 if len(examples)<100:examples.append(row)
def scalar(tick,field,a,b,actor=None):
 if a!=b:record(tick,field,a,b,actor)
def vector(tick,field,a,b,actor=None,tolerance=.00004):
 if len(a)!=len(b) or any(abs(x-y)>tolerance for x,y in zip(a,b)):record(tick,field,a,b,actor)
def enemy_keys(frames,native_side):
 keys={};occurrences=collections.Counter()
 for frame in frames:
  for e in frame['enemies']:
   if e['id'] in keys:continue
   if native_side:name=e['birth']['routine']
   else:
    ctx=next((x for x in frame['ecl'] if x['enemy']==e['id']),{})
    name=(ctx.get('calls') or [ctx])[0].get('routine','')
   keys[e['id']]=(name,occurrences[name]);occurrences[name]+=1
 return keys
nk=enemy_keys(native['frames'],True);ck=enemy_keys([candidate['initial']]+candidate['ticks'],False)
def laser_keys(frames,native_side):
 # Separate ID namespaces: native Timed+80 is a lookup ID, whereas Moving/
 # Curve+80 is a serial. Candidate manager serials must not be equated to
 # either raw field. First live snapshots give only observed births; objects
 # born and retired between snapshots remain outside this pairing scope.
 keys={};occurrences=collections.Counter()
 for frame in frames:
  tick=frame['frame'] if native_side else frame['frame']-1
  objects=frame.get('lasers',[]) if native_side else frame.get('lasers',{}).get('nodes',[])
  for laser in objects:
   identity=laser['observedObject']['id'] if native_side else laser['serial']
   if identity in keys:continue
   birth=(tick,laser['kind']);keys[identity]=(*birth,occurrences[birth]);occurrences[birth]+=1
 return keys
if args.compare_lasers:
 nlk=laser_keys(native['frames'],True);clk=laser_keys([candidate['initial']]+candidate['ticks'],False)
friendly_native_ids={};friendly_native_previous={};friendly_candidate_ids={};nfcount=0
hostile_candidate_ids={}
hostile_candidate_births={};hostile_native_births={event['id']:event['frame'] for event in native.get('events',[]) if event.get('kind')=='bulletCreated'}
friendly_pairing_methods=set()
hostile_pairing_methods=set()
def friendly_candidate_fields(b):
 w=b['weapon'];offset=1 if len(w)>=13 else 0
 return {'phase':w[0],'age':b['age'],'position':b['position'],'damage':w[3+offset],'hitbox':w[4+offset:6+offset],'hitLastFrame':w[2]}
frames=0;alignment_failure=None;graze_frames_compared=0
for a,b in zip(native['frames'][1:],candidate['ticks']):
 tick=a['frame'];np=a['player'];cp=b['player'];ne=a['economy'];ce=b['economy'];r=ce['resources']
 if b['frame']!=tick+1:
  # Natural candidate GameOver stops its tick clock. Keep the actually aligned
  # prefix and expose the boundary; never advance, relabel or shift a frame.
  alignment_failure={'inputTick':tick,'expectedCandidateFrame':tick+1,'actualCandidateFrame':b['frame']};record(tick,'clock.frame',tick+1,b['frame']);break
 frames+=1
 if args.compare_lasers:
  nl={nlk[x['observedObject']['id']]:x for x in a['lasers']};cl={clk[x['serial']]:x for x in b['lasers']['nodes']}
  scalar(tick,'laser.count',len(nl),len(cl));scalar(tick,'laser.observedActors',sorted(nl),sorted(cl))
  for key in sorted(nl.keys()&cl.keys()):
   x,y=nl[key],cl[key]
   for field in ['kind','phase','retirePending']:scalar(tick,'laser.'+field,x[field],y[field],key)
   scalar(tick,'laser.cancellable',bool(x['cancellable']),bool(y['cancellable']),key)
   if x['kind']==1:scalar(tick,'laser.timedLookupId',x['nativeField80'],y['lookupId'],key)
   vector(tick,'laser.position',x['position'],y['position'],key)
   vector(tick,'laser.geometry',x['geometry']+[x['headOffset']],y['geometry'],key)
   scalar(tick,'laser.ageInteger',x['age'][:2],y['age'][:2],key)
   vector(tick,'laser.ageFloat',[x['age'][2]],[y['age'][2]],key)
 scalar(tick,'rng.seed',a['rngSeed'],b['rng']['seed']);scalar(tick,'rng.calls',a['rngCalls'],b['rng']['calls'])
 for field,av,bv in [('player.fixedPosition',np['fixedPosition'],[cp['xFixed'],cp['yFixed']]),('player.state',np['state'],cp['state']),('player.stateAge',np['stateAge'],cp['stateAge']),('player.invulnerability',np['invulnerability'],cp['invulnerability']),('player.shootAge',np['shootAge'],cp['fireFrame'])]:scalar(tick,field,av,bv)
 for name,index in [('power',0),('scoreStored',3),('pivStored',4),('maxPivStored',5),('rank',10)]:scalar(tick,'economy.'+name,ne[name],r[index])
 # Historical native captures omitted this field. Its absence is unobserved,
 # never a zero graze count; candidate Snapshot exports it under player.graze.
 if 'graze' in ne and 'graze' in cp:
  scalar(tick,'economy.graze',ne['graze'],cp['graze']);graze_frames_compared+=1
 scalar(tick,'economy.lifeBomb',ne['lifeBombCounters'],[r[6],r[8],r[7],r[9]])
 scalar(tick,'ufo.colors',ne['ufoColors'],ce['ufoColors']);scalar(tick,'bomb.active',bool(ne['bombState']),bool(cp['bombState']['active']))
 # Decorative enemies stay excluded from combat, but the main controller's
 # ECL clock is essential to detecting a startup tick added before input0.
 for key in sorted({nk[e['id']] for e in a['enemies'] if nk[e['id']][0]=='main'}):
  x=next(e for e in a['enemies'] if nk[e['id']]==key);y=next((e for e in b['enemies'] if ck[e['id']]==key),None)
  if y is None:record(tick,'controller.actors',key,None);continue
  nc=sorted((z['context']['routine'],z['context']['instructionOffset'],z['time']) for z in x['contexts'] if z['context'])
  cc=sorted((z['routine'],z['byteOffset'],z['time']) for z in b['ecl'] if z['enemy']==y['id'] and z['routine'])
  scalar(tick,'controller.contexts',nc,cc,key)
 na={nk[e['id']]:e for e in a['enemies'] if nk[e['id']][0] not in excluded};ca={ck[e['id']]:e for e in b['enemies'] if ck[e['id']][0] not in excluded}
 scalar(tick,'enemy.actors',sorted(na),sorted(ca))
 for key in sorted(na.keys()&ca.keys()):
  x,y=na[key],ca[key];scalar(tick,'enemy.life',x['life'],y['life'],key);scalar(tick,'enemy.hpImmunity',x['hpImmunity'],y['immunity'],key);vector(tick,'enemy.position',x['position'],y['position'],key,tolerance=.00002)
  nc=sorted((z['context']['routine'],z['context']['instructionOffset'],z['time']) for z in x['contexts'] if z['context']);cc=sorted((z['routine'],z['byteOffset'],z['time']) for z in b['ecl'] if z['enemy']==y['id'] and z['routine']);scalar(tick,'enemy.contexts',nc,cc,key)
 ni={i['slot']:i for i in a['items']};ci={i[1]:i for i in b['items']};scalar(tick,'item.slots',sorted(ni),sorted(ci))
 for slot in sorted(ni.keys()&ci.keys()):
  x,y=ni[slot],ci[slot]
  for field,av,bv in [('type',x['type'],y[2]),('state',x['state'],y[3]),('age',x['age'],y[8]),('delay',x['delay'],y[10])]:scalar(tick,'item.'+field,av,bv,slot)
  # 425c20 reads9a0 only in the changing-token13..15 branch. The native
  # aggregate constructor4278xx can retain a previous token's unused timer;
  # this behavioral comparison does not require copying that stale memory.
  if x['type']==y[2] and 13<=x['type']<=15:scalar(tick,'item.colorTimer',x['colorTimer'],y[9],slot)
  for field,av,bv in [('position',x['position'],y[4:6]),('velocity',x['velocity'],y[6:8]),('attractionSpeed',[x['attractionSpeed']],[y[13]])]:vector(tick,'item.'+field,av,bv,slot)
 nf={}
 for shot in a['friendly']:
  if shot.get('id') is not None:
   nf[shot['id']]=shot;continue
  slot=shot['slot'];previous=friendly_native_previous.get(slot)
  # Enemy18 may reuse a just-retired slot for an explosion fragment with
  # age0. Its next Player16 tick moves0->1; that is not another birth.
  if previous is None or shot['age']<previous['age']:nfcount+=1;friendly_native_ids[slot]=nfcount
  nf[friendly_native_ids[slot]]=shot
 friendly_native_previous={x['slot']:x for x in a['friendly']}
 cf={}
 for shot in sorted(b['bullets'],key=lambda shot:shot['id']):
  if not shot['friendly']:continue
  if shot['id'] not in friendly_candidate_ids:friendly_candidate_ids[shot['id']]=len(friendly_candidate_ids)+1
  cf[friendly_candidate_ids[shot['id']]]=friendly_candidate_fields(shot)
 friendly_candidate=[shot for shot in b['bullets'] if shot['friendly']]
 if all(isinstance(shot.get('physicalSlot'),int) and 0<=shot['physicalSlot']<256 for shot in friendly_candidate):
  nf={shot['slot']:shot for shot in a['friendly']}
  cf={shot['physicalSlot']:friendly_candidate_fields(shot) for shot in friendly_candidate}
  friendly_pairing_methods.add('physicalSlot')
 else:friendly_pairing_methods.add('creationOrdinalFallback')
 scalar(tick,'friendly.count',len(nf),len(cf));scalar(tick,'friendly.actors',sorted(nf),sorted(cf))
 for key in sorted(nf.keys()&cf.keys()):
  x,y=nf[key],cf[key]
  for field in ['phase','age','damage','hitLastFrame']:scalar(tick,'friendly.'+field,x[field],y[field],key)
  for field in ['position','hitbox']:vector(tick,'friendly.'+field,x[field],y[field],key)
 nh={shot['id']:shot for shot in a['bullets']};ch={}
 for shot in b['bullets']:
  if shot['friendly']:continue
  if shot['id'] not in hostile_candidate_ids:hostile_candidate_ids[shot['id']]=len(hostile_candidate_ids)+1;hostile_candidate_births[shot['id']]=tick
  ch[hostile_candidate_ids[shot['id']]]=shot
 hostile_candidate=[shot for shot in b['bullets'] if not shot['friendly']]
 if all(isinstance(shot.get('physicalSlot'),int) and 0<=shot['physicalSlot']<2000 for shot in hostile_candidate):
  nh={shot['slot']:shot for shot in a['bullets']};ch={shot['physicalSlot']:shot for shot in hostile_candidate}
  if len(nh)!=len(a['bullets']) or len(ch)!=len(hostile_candidate):raise SystemExit('Hostile physical slots must be unique within a native/candidate frame')
  hostile_pairing_methods.add('physicalSlot')
 else:hostile_pairing_methods.add('creationOrdinalFallback')
 scalar(tick,'hostile.count',len(nh),len(ch));scalar(tick,'hostile.actors',sorted(nh),sorted(ch))
 for key in sorted(nh.keys()&ch.keys()):
  x,y=nh[key],ch[key]
  if x['id'] in hostile_native_births:scalar(tick,'hostile.birthInputTick',hostile_native_births[x['id']],hostile_candidate_births[y['id']],key)
  for field,av,bv in [('phase',x['state'],y['hostile'][2]),('age',x['age'],y['age']),('collisionDelay',x['collisionDelay'],y['hostile'][8]),('cursor',x['extensionCursor'],y['hostile'][11]),('typeColor',x['typeColor'],[y['type'],y['color']])]:scalar(tick,'hostile.'+field,av,bv,key)
  for field,av,bv in [('position',x['position'],y['position']),('velocity',x['velocity'],y['hostile'][:2]),('polar',[x['angle'],x['speed']],y['motion'])]:vector(tick,'hostile.'+field,av,bv,key)
out={'schema':'th12-native-world-comparison/2','partial':True,'wholeGameOracle':False,'framesCompared':frames,'friendlyPairing':sorted(friendly_pairing_methods),'hostilePairing':sorted(hostile_pairing_methods),'nativeCreationGuardPrimed':native.get('creationGuardPrimed'),'replay':candidate.get('replay'),'replaySha256':candidate.get('replaySha256'),'candidateWasmSha256':candidate.get('wasmSha256'),'originalExeSha256':native.get('originalExeSha256'),'replayInputAlignmentValidated':bool(candidate.get('replaySha256') and candidate.get('replaySha256')==native.get('replaySha256') and not native.get('creationGuardPrimed')),'floatTolerance':.00004,'excludedEnemyRoutines':sorted(excluded),'pairingLimitations':'Projectiles use physical slots when exported; older captures fall back to creation ordinals. Native hostile birth hooks are checked against the candidate first live snapshot, which cannot observe births retiring in the same tick. Main controller ECL is compared; full animation scene and STD remain excluded. Laser CPU state comparison is explicitly opt-in and carries its own observation identity boundary.','firstDifferenceByField':first,'differenceCounts':dict(counts),'examples':examples}
out['alignmentFailure']=alignment_failure
out['candidateFixtureExecutionScope']={'metadata':candidate_fixture,'snapshotFlagsCaptured':any('fixtureFlags' in row for row in [candidate.get('initial',{})]+candidate['ticks']),'scope':'Any explicitly declared nonzero diagnostic mask or snapshot fixtureFlags is rejected. Legacy captures without these fields retain their original unreported scope; their absence alone is not proof of normal-world mode.'}
out['nativeTraceBoundary']={'requestedFrames':native.get('requestedFrames'),'completedFrames':native.get('completedFrames',max(len(native['frames'])-1,0)),'lastCompletedInputTick':native['frames'][-1]['frame'] if native['frames'] else None,'requestedTraceCompleted':native.get('requestedTraceCompleted'),'fault':native.get('fault'),'sceneBoundary':native.get('sceneBoundary',native.get('nativeGameOverSceneBoundary')),'scope':'Only actually completed native gameplay-manager ticks are compared. A native scene transition or candidate GameOver clock stop does not supply a whole replay result.'}
out['itemSemanticFieldScope']={'colorTimer':'Compared only for matching changing-token kinds13..15, the425c20 branch that reads9a0. Aggregate-item constructors can retain unused prior-slot timer bytes; these are not behavioral state.','delay':'Native9c0 is compared as exported, including delayed-point lifecycle.','remainingFields':'Type, state, age, position, velocity and attraction speed retain their existing comparisons.'}
out['grazeComparisonScope']={'nativeCapture':native.get('grazeStateCapture'),'nativeField':'economy.graze','candidateField':'player.graze','framesCompared':graze_frames_compared,'scope':'Compare only aligned frames where both direct integer fields exist. Missing fields in historical captures are skipped, never interpreted as zero; PIV is not used to infer graze.'}
out['nativeLaserExecutionScope']={'executed':native.get('nativeLaserUpdateExecuted',False),'providerScope':native.get('laserScope'),'stateCapture':native.get('laserStateCapture'),'laserFieldsCompared':args.compare_lasers,'pairing':'First observed input tick, kind and occurrence in that tick/kind; independent native observed-object and candidate serial namespaces. Objects born and retired before a snapshot are unobservable; multiple same-kind births assume corresponding linked-list order.' if args.compare_lasers else None,'comparedFields':['kind','phase','position','angle/length/width/speed/headOffset','integer and float age','retirePending','cancellable','Timed lookup ID'] if args.compare_lasers else [],'scope':'Actual native laser updates can contribute to shared RNG, graze, PIV and collision fields when enabled. Opt-in laser comparisons cover observed CPU lifecycle/geometry state only, never embedded ANM, Curve nodes, draw vertices or pixels. A capture without native laser updates cannot establish a discrepancy caused by laser graze.'}
out['nativeCameraExecutionScope']={'executed':native.get('nativeCameraUpdateExecuted',False),'providerScope':native.get('cameraScope'),'drawResetBoundary':native.get('cameraDrawResetBoundary'),'cameraFieldsCompared':False,'scope':'Registered native priority14 camera updates can consume shared game RNG after Bomb creation. Captures omitting this priority cannot establish a complete Bomb RNG discrepancy. Camera displacement and visual output are not compared here.'}
out['nativeHostEnvironmentScope']={'textAnmStartupBinding':native.get('textAnmStartupBinding'),'nativeStageResources':native.get('nativeStageResources'),'replayHeaderInitialConditionScope':native.get('replayHeaderInitialConditionScope'),'nativeReplayManagerScope':native.get('nativeReplayManagerScope'),'hostHeapAudit':{key:native.get('hostHeapAudit',{}).get(key) for key in ['allocationCount','highWaterExclusive','remainingBytes','overlapsFixedRegions']},'scope':'Startup bindings, original stage resources, replay-header initial conditions and bounded host arena are explicitly recorded when captured. Historical captures retain the environment in which they actually executed. Missing historical metadata never establishes full ReplayManager execution.'}
out['replayInputAlignmentValidated']=out['replayInputAlignmentValidated'] and alignment_failure is None
args.output.write_text(json.dumps(out,indent=2)+'\n')
earliest=sorted(first.values(),key=lambda row:row['inputTick'])[:6]
print(json.dumps({'output':str(args.output),'framesCompared':frames,'wholeGameOracle':False,'fieldsWithDifferences':len(first),'earliestDifferences':earliest},indent=2))
