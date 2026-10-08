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
args=p.parse_args();native=json.loads(args.native.read_text());candidate=json.loads(args.candidate.read_text())
if native.get('worldFrameCaptureScope',{}).get('enabled') is False:
 raise SystemExit('Event-only native diagnostics have no comparable gameplay frame prefix')
if native.get('replaySha256') and candidate.get('replaySha256') and native['replaySha256']!=candidate['replaySha256']:
 raise SystemExit('Native and candidate captures use different replay inputs')
for flag in ['nativePlayerExecuted','nativeItemsUfoSpellMsgExecuted','activeCollisionExecuted','nativeAnmExecuted','nativeBulletUpdateExecuted']:
 if not native.get(flag):raise SystemExit('Active manager comparison requires '+flag)
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
friendly_native_ids={};friendly_native_previous={};friendly_candidate_ids={};nfcount=0
hostile_candidate_ids={}
hostile_candidate_births={};hostile_native_births={event['id']:event['frame'] for event in native.get('events',[]) if event.get('kind')=='bulletCreated'}
friendly_pairing_methods=set()
hostile_pairing_methods=set()
def friendly_candidate_fields(b):
 w=b['weapon'];offset=1 if len(w)>=13 else 0
 return {'phase':w[0],'age':b['age'],'position':b['position'],'damage':w[3+offset],'hitbox':w[4+offset:6+offset],'hitLastFrame':w[2]}
frames=0;alignment_failure=None
for a,b in zip(native['frames'][1:],candidate['ticks']):
 tick=a['frame'];np=a['player'];cp=b['player'];ne=a['economy'];ce=b['economy'];r=ce['resources']
 if b['frame']!=tick+1:
  # Natural candidate GameOver stops its tick clock. Keep the actually aligned
  # prefix and expose the boundary; never advance, relabel or shift a frame.
  alignment_failure={'inputTick':tick,'expectedCandidateFrame':tick+1,'actualCandidateFrame':b['frame']};record(tick,'clock.frame',tick+1,b['frame']);break
 frames+=1
 scalar(tick,'rng.seed',a['rngSeed'],b['rng']['seed']);scalar(tick,'rng.calls',a['rngCalls'],b['rng']['calls'])
 for field,av,bv in [('player.fixedPosition',np['fixedPosition'],[cp['xFixed'],cp['yFixed']]),('player.state',np['state'],cp['state']),('player.stateAge',np['stateAge'],cp['stateAge']),('player.invulnerability',np['invulnerability'],cp['invulnerability']),('player.shootAge',np['shootAge'],cp['fireFrame'])]:scalar(tick,field,av,bv)
 for name,index in [('power',0),('scoreStored',3),('pivStored',4),('maxPivStored',5),('rank',10)]:scalar(tick,'economy.'+name,ne[name],r[index])
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
out={'schema':'th12-native-world-comparison/2','partial':True,'wholeGameOracle':False,'framesCompared':frames,'friendlyPairing':sorted(friendly_pairing_methods),'hostilePairing':sorted(hostile_pairing_methods),'nativeCreationGuardPrimed':native.get('creationGuardPrimed'),'replay':candidate.get('replay'),'replaySha256':candidate.get('replaySha256'),'candidateWasmSha256':candidate.get('wasmSha256'),'originalExeSha256':native.get('originalExeSha256'),'replayInputAlignmentValidated':bool(candidate.get('replaySha256') and candidate.get('replaySha256')==native.get('replaySha256') and not native.get('creationGuardPrimed')),'floatTolerance':.00004,'excludedEnemyRoutines':sorted(excluded),'pairingLimitations':'Projectiles use physical slots when exported; older captures fall back to creation ordinals. Native hostile birth hooks are checked against the candidate first live snapshot, which cannot observe births retiring in the same tick. Main controller ECL is compared, but full animation scene, STD and hostile lasers are not compared here.','firstDifferenceByField':first,'differenceCounts':dict(counts),'examples':examples}
out['alignmentFailure']=alignment_failure
out['nativeTraceBoundary']={'requestedFrames':native.get('requestedFrames'),'completedFrames':native.get('completedFrames',max(len(native['frames'])-1,0)),'lastCompletedInputTick':native['frames'][-1]['frame'] if native['frames'] else None,'requestedTraceCompleted':native.get('requestedTraceCompleted'),'fault':native.get('fault'),'sceneBoundary':native.get('sceneBoundary',native.get('nativeGameOverSceneBoundary')),'scope':'Only actually completed native gameplay-manager ticks are compared. A native scene transition or candidate GameOver clock stop does not supply a whole replay result.'}
out['itemSemanticFieldScope']={'colorTimer':'Compared only for matching changing-token kinds13..15, the425c20 branch that reads9a0. Aggregate-item constructors can retain unused prior-slot timer bytes; these are not behavioral state.','delay':'Native9c0 is compared as exported, including delayed-point lifecycle.','remainingFields':'Type, state, age, position, velocity and attraction speed retain their existing comparisons.'}
out['nativeLaserExecutionScope']={'executed':native.get('nativeLaserUpdateExecuted',False),'providerScope':native.get('laserScope'),'stateCapture':native.get('laserStateCapture'),'laserFieldsCompared':False,'scope':'Actual native laser updates can contribute to shared RNG, graze, PIV and collision fields when enabled. Laser geometry itself has no comparison/visual acceptance claim here. A capture without native laser updates cannot establish a discrepancy caused by laser graze.'}
out['nativeCameraExecutionScope']={'executed':native.get('nativeCameraUpdateExecuted',False),'providerScope':native.get('cameraScope'),'drawResetBoundary':native.get('cameraDrawResetBoundary'),'cameraFieldsCompared':False,'scope':'Registered native priority14 camera updates can consume shared game RNG after Bomb creation. Captures omitting this priority cannot establish a complete Bomb RNG discrepancy. Camera displacement and visual output are not compared here.'}
out['nativeHostEnvironmentScope']={'textAnmStartupBinding':native.get('textAnmStartupBinding'),'hostHeapAudit':{key:native.get('hostHeapAudit',{}).get(key) for key in ['allocationCount','highWaterExclusive','remainingBytes','overlapsFixedRegions']},'scope':'Startup bindings and bounded host arena are explicitly recorded. Historical captures retain the environment in which they actually executed.'}
out['replayInputAlignmentValidated']=out['replayInputAlignmentValidated'] and alignment_failure is None
args.output.write_text(json.dumps(out,indent=2)+'\n')
earliest=sorted(first.values(),key=lambda row:row['inputTick'])[:6]
print(json.dumps({'output':str(args.output),'framesCompared':frames,'wholeGameOracle':False,'fieldsWithDifferences':len(first),'earliestDifferences':earliest},indent=2))
