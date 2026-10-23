import assert from 'node:assert/strict';

const SCHEMA='eagler-touhou/touch-double-tap-scenes/1';
const MAX_BUDGET_TICKS=120;
const MAX_SCENE_TICKS=256;

class SceneIncomplete extends Error{}

function incomplete(reason,evidence){return{checkStatus:'incomplete',reason,evidence};}
function missingIO(io){return['controls','directTouchBatch','step','observe','readActions'].filter(name=>typeof io?.[name]!=='function');}
function isIOTimeout(error){return error?.name==='TimeoutError'||error?.code==='ETIMEDOUT'||error?.timeout===true||
  (typeof error?.message==='string'&&/^Timeout(?::|\s)/i.test(error.message));}
function validMask(value){return Number.isSafeInteger(value)&&value>0&&value<=0x80000000&&Number.isInteger(Math.log2(value));}
function validateInputMasks(masks){
  for(const name of ['fire','focus','bomb','escape'])assert.ok(validMask(masks?.[name]),`Double-tap Bomb scene requires a single-bit ${name} mask`);
  assert.equal(new Set(['fire','focus','bomb','escape'].map(name=>masks[name])).size,4,'STG action masks must be distinct');
}
function point(value,label){
  assert.ok(Number.isFinite(value?.x)&&value.x>=0&&value.x<=1&&Number.isFinite(value?.y)&&value.y>=0&&value.y<=1,
    `${label} must be a normalized Runtime-canvas point`);
  return{x:value.x,y:value.y};
}
function validateOptions({launchOptions,gesture,geometry,budgets,inputMasks}={}){
  validateInputMasks(inputMasks);
  assert.equal(typeof launchOptions?.doubleTapBombEnabled,'boolean','Double-tap Bomb scene requires the actual startup option receipt');
  assert.ok(typeof launchOptions.source==='string'&&launchOptions.source.length>0,'Double-tap Bomb option receipt needs its launch source');
  assert.ok(Number.isFinite(gesture?.firstTapMaxMs)&&gesture.firstTapMaxMs>0,'Title must declare the first-tap duration limit');
  assert.ok(Number.isFinite(gesture?.interTapMaxMs)&&gesture.interTapMaxMs>0,'Title must declare the inter-tap duration limit');
  assert.ok(Number.isInteger(gesture?.bombPulseTicks)&&gesture.bombPulseTicks>0&&gesture.bombPulseTicks<=MAX_BUDGET_TICKS,
    'Title must declare the bounded native touch Bomb pulse length');
  assert.ok(typeof gesture.source==='string'&&gesture.source.length>0,'Title gesture timing needs a production source anchor');
  const primaryId=geometry?.primary?.id,secondaryId=geometry?.secondary?.id;
  assert.ok(Number.isSafeInteger(primaryId)&&primaryId>=0&&Number.isSafeInteger(secondaryId)&&secondaryId>=0&&primaryId!==secondaryId,
    'Double-tap Bomb scene requires distinct primary movement and secondary gesture pointer IDs');
  const points={primaryStart:point(geometry.primary.start,'Primary movement start'),
    primaryMove:point(geometry.primary.move,'Primary movement target'),
    primaryBombMove:point(geometry.primary.bombMove,'Primary movement target during Bomb'),
    secondary:point(geometry.secondary.point,'Secondary double-tap point')};
  assert.ok(Number.isFinite(geometry.movementEpsilon)&&geometry.movementEpsilon>0,
    'Title must declare a native movement observation tolerance');
  for(const key of ['baselineTicks','movementSettlingTicks','triggerTicks','pulseDrainTicks'])
    assert.ok(Number.isInteger(budgets?.[key])&&budgets[key]>0&&budgets[key]<=MAX_BUDGET_TICKS,
      `Double-tap Bomb scene requires a bounded ${key}`);
  assert.ok(typeof budgets.movementSettlingSource==='string'&&budgets.movementSettlingSource.length>0,
    'Movement settling budget needs its product input-boundary source');
  assert.ok(budgets.pulseDrainTicks>=gesture.bombPulseTicks+1,
    'Pulse drain budget must include the declared pulse and one clear consumer sample');
  const totalBudgetTicks=budgets.baselineTicks+budgets.movementSettlingTicks+1+budgets.triggerTicks+
    budgets.movementSettlingTicks+budgets.pulseDrainTicks;
  assert.ok(totalBudgetTicks<=MAX_SCENE_TICKS,`Double-tap Bomb scene exceeds its ${MAX_SCENE_TICKS}-tick total budget`);
  return{primaryId,secondaryId,points,movementEpsilon:geometry.movementEpsilon,totalBudgetTicks};
}
function positionOf(observation,label){
  const position=observation?.position;
  assert.ok(Number.isFinite(position?.x)&&Number.isFinite(position?.y),`${label} needs the native player position`);
  return{x:position.x,y:position.y};
}
function inventoryOf(action,label){
  const inventory=action?.inventory;
  for(const field of ['bombsRemaining','bombsUsed','bombsUsedStage'])
    assert.ok(Number.isSafeInteger(inventory?.[field])&&inventory[field]>=0,`${label} needs native ${field}`);
  return{bombsRemaining:inventory.bombsRemaining,bombsUsed:inventory.bombsUsed,bombsUsedStage:inventory.bombsUsedStage};
}
function assertNoBomb(action,inventory,label,bombMask){
  assert.equal(Boolean(action.consumed.current&bombMask),false,`${label}: native consumer received Bomb input`);
  assert.equal(action.bomb.active,false,`${label}: native Bomb became active`);
  assert.equal(action.bomb.triggered,false,`${label}: native Bomb was triggered`);
  assert.deepEqual(inventoryOf(action,label),inventory,`${label}: native Bomb inventory changed`);
}
function pointer(type,id,p){return{type,id,x:p.x,y:p.y};}

/**
 * Verify one real launcher-to-player double-tap trial. The launch option is
 * supplied by the caller; this scene never changes Runtime configuration.
 * `directTouchBatch` accepts ordered pointer events and checkpoint sentinels.
 * A checkpoint advances the declared native clock ticks and returns every
 * post-consumer action and position sample before the next pointer event.
 */
export async function runTouchDoubleTapBombScene(io,options={}){
  const evidence={schema:SCHEMA,launchOptions:options.launchOptions?structuredClone(options.launchOptions):null,
    gesture:options.gesture?structuredClone(options.gesture):null,geometry:options.geometry?structuredClone(options.geometry):null,
    budgets:options.budgets?structuredClone(options.budgets):null,inputMasks:options.inputMasks?structuredClone(options.inputMasks):null,
    controls:[],touchBatches:[],samples:[],positionChecks:[],bombObservation:null,cleanup:null};
  const missing=missingIO(io);
  if(missing.length)return incomplete(`Double-tap Bomb IO is missing ${missing.join(', ')}`,evidence);
  if(!options.inputMasks||!options.launchOptions||!options.gesture||!options.geometry||!options.budgets)
    return incomplete('Double-tap Bomb scene requires launch option, title gesture, geometry, and bounded wait declarations.',evidence);
  const config=validateOptions(options);
  evidence.budgets.totalSceneTicks=config.totalBudgetTicks;
  const enabled=options.launchOptions.doubleTapBombEnabled;
  const masks=options.inputMasks;
  const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
  const evidenceSamples=evidence.samples;
  let result=null,failure=null,cleanupError=null,primaryMayBeHeld=false;
  let previousSequence=null,baseline=null,baselineInventory=null,stage=null;
  const callIO=async(label,call)=>{
    try{return await call();}
    catch(error){
      evidence.ioErrors??=[];evidence.ioErrors.push({label,name:error?.name??null,message:String(error?.message??error)});
      if(isIOTimeout(error))throw new SceneIncomplete(`${label}: Runtime IO timed out before the required observation was received`);
      throw error;
    }
  };
  const recordConsumer=(action,observation,label)=>{
    if(action?.available===false)throw new SceneIncomplete(action.reason??'Native STG action observer is unavailable');
    if(action?.complete!==true)throw new SceneIncomplete(`${label}: no completed player-consumer sample was observed`);
    assert.equal(action.phase,'consumer-run',`${label}: expected an unpaused native player-consumer sample`);
    assert.ok(Number.isSafeInteger(action.sequence)&&action.sequence>0,`${label}: consumer sample needs a sequence`);
    if(previousSequence!==null)assert.equal(action.sequence,previousSequence+1,`${label}: player-consumer sequence skipped or repeated`);
    previousSequence=action.sequence;
    if(stage!==null)assert.equal(action.stage,stage,`${label}: native stage changed during the gesture`);
    else stage=action.stage;
    const row={label,action:structuredClone(action),runtime:{position:observation?.position??null,
      pause:observation?.pause??null,phase:observation?.phase??null}};
    evidenceSamples.push(row);
    return row;
  };
  const tickForConsumer=async(label,maxTicks)=>{
    for(let tick=0;tick<maxTicks;tick++){
      await callIO(`${label}-tick-step`,()=>io.step(1));
      const action=await callIO(`${label}-native-action-observation`,()=>io.readActions());
      const observation=await callIO(`${label}-Runtime-position-observation`,()=>io.observe());
      if(action?.available===false)throw new SceneIncomplete(action.reason??'Native STG action observer is unavailable');
      if(action?.complete!==true){
        evidenceSamples.push({label:`${label}-incomplete-${tick+1}`,action:structuredClone(action??null),
          runtime:{position:observation?.position??null,pause:observation?.pause??null,phase:observation?.phase??null}});
        continue;
      }
      return recordConsumer(action,observation,label);
    }
    throw new SceneIncomplete(`${label}: no completed player-consumer sample within ${maxTicks} declared ticks`);
  };
  const neutralize=async(label)=>{
    const acknowledgement=await callIO(`${label}-neutral-controls`,()=>io.controls({...neutral}));
    evidence.controls.push({label,controls:{...neutral},acknowledgement:acknowledgement??null});
    if(acknowledgement?.acknowledged!==true)throw new SceneIncomplete(`${label}: Runtime did not acknowledge neutral touch controls`);
  };
  const touchBatch=async(label,operations)=>{
    const batch=await callIO(`${label}-direct-touch-batch`,()=>io.directTouchBatch(operations.map(item=>({...item}))));
    evidence.touchBatches.push({label,operations:structuredClone(operations),batch:structuredClone(batch??null)});
    if(batch?.available===false)throw new SceneIncomplete(batch.reason??`${label}: direct touch batch is unavailable`);
    if(batch?.acknowledged!==true||!Array.isArray(batch.events))
      throw new SceneIncomplete(`${label}: direct touch batch did not return acknowledged per-event transport receipts`);
    const expected=operations.map((operation,index)=>({operation,index})).filter(row=>row.operation.type!=='checkpoint');
    if(batch.events.length!==expected.length)throw new SceneIncomplete(`${label}: direct touch transport receipt count differs from submitted pointer events`);
    for(let index=0;index<expected.length;index++){
      const {operation,index:operationIndex}=expected[index],receipt=batch.events[index];
      if(receipt?.acknowledged!==true||receipt.operationIndex!==operationIndex||receipt.type!==operation.type||receipt.id!==operation.id||
        !Number.isFinite(receipt.startedAtMs)||!Number.isFinite(receipt.completedAtMs)||receipt.completedAtMs<receipt.startedAtMs)
        throw new SceneIncomplete(`${label}: a pointer transport receipt is missing, reordered, or malformed`);
      if(index>0&&receipt.startedAtMs<batch.events[index-1].completedAtMs)
        throw new SceneIncomplete(`${label}: pointer transport times do not follow the submitted FIFO order`);
    }
    for(const operation of operations.filter(item=>item.type==='checkpoint')){
      const checkpoint=batch.checkpoints?.find(item=>item?.label===operation.label);
      if(!checkpoint||!Array.isArray(checkpoint.samples)||checkpoint.samples.length!==operation.ticks)
        throw new SceneIncomplete(`${label}: checkpoint ${operation.label} did not return all ${operation.ticks} declared samples`);
      for(let tick=0;tick<checkpoint.samples.length;tick++){
        const sample=checkpoint.samples[tick];
        recordConsumer(sample?.action,sample?.observation,`${operation.label}-tick-${tick+1}`);
      }
    }
    return batch;
  };
  try{
    await neutralize('neutral-baseline');
    baseline=(await tickForConsumer('initial-native-baseline',options.budgets.baselineTicks)).action;
    baselineInventory=inventoryOf(baseline,'Initial native Bomb baseline');
    assert.ok(baselineInventory.bombsRemaining>0,'Double-tap Bomb scene needs at least one native Bomb in inventory');
    assert.equal(baseline.bomb.active,false,'Double-tap Bomb scene requires no active native Bomb at baseline');
    assert.equal(baseline.bomb.triggered,false,'Double-tap Bomb scene requires no pending native Bomb at baseline');
    const beforeMovement=positionOf(evidenceSamples.at(-1).runtime,'Initial native sample');

    primaryMayBeHeld=true;
    const primaryBatch=await touchBatch('primary-movement-owner',[
      pointer('down',config.primaryId,config.points.primaryStart),
      pointer('move',config.primaryId,config.points.primaryMove),
      {type:'checkpoint',label:'primary-movement-settle',ticks:options.budgets.movementSettlingTicks},
    ]);
    const movementSamples=internalCheckpoint(primaryBatch,'primary-movement-settle').samples;
    const movedSample=movementSamples.find(sample=>{
      const position=positionOf(sample.observation,'Primary movement sample');
      return Math.hypot(position.x-beforeMovement.x,position.y-beforeMovement.y)>config.movementEpsilon;
    });
    assert.ok(movedSample,'The primary movement finger did not move the native player within its declared settling budget');
    const afterMovement=positionOf(movementSamples.at(-1).observation,'Final primary movement sample');
    evidence.positionChecks.push({label:'primary-finger-before-gesture',before:beforeMovement,after:afterMovement,
      primaryId:config.primaryId,transport:primaryBatch.events});

    const tapPoint=config.points.secondary,secondId=config.secondaryId;
    const firstTapStartOperation=0,firstTapEndOperation=1,secondTapStartOperation=3,secondTapEndOperation=4;
    const gestureOperations=[
      pointer('down',secondId,tapPoint),pointer('up',secondId,tapPoint),
      {type:'checkpoint',label:'single-tap',ticks:1},
      pointer('down',secondId,tapPoint),pointer('up',secondId,tapPoint),
      {type:'checkpoint',label:'double-tap-consumer',ticks:options.budgets.triggerTicks},
      pointer('move',config.primaryId,config.points.primaryBombMove),
      {type:'checkpoint',label:'primary-movement-during-bomb',ticks:options.budgets.movementSettlingTicks},
      {type:'checkpoint',label:'bomb-pulse-drain',ticks:options.budgets.pulseDrainTicks},
    ];
    const batch=await touchBatch('single-then-double-tap-bounded-group',gestureOperations);
    const single=internalCheckpoint(batch,'single-tap').samples[0].action;
    const singleInventory=inventoryOf(single,'Single-tap observation');
    assertNoBomb(single,baselineInventory,'Single tap',masks.bomb);
    assert.deepEqual(singleInventory,baselineInventory,'Single tap changed native Bomb inventory');

    const tapReceipts=batch.events;
    const byOperation=new Map(tapReceipts.map(row=>[row.operationIndex,row]));
    const firstDown=byOperation.get(firstTapStartOperation),firstUp=byOperation.get(firstTapEndOperation);
    const secondDown=byOperation.get(secondTapStartOperation),secondUp=byOperation.get(secondTapEndOperation);
    if(!firstDown||!firstUp||!secondDown||!secondUp)throw new SceneIncomplete('Double-tap group lacks the four pointer transport times.');
    const firstTapElapsed=firstUp.completedAtMs-firstDown.startedAtMs;
    const interTapMinElapsed=secondDown.startedAtMs-firstUp.completedAtMs;
    const interTapMaxElapsed=secondDown.completedAtMs-firstUp.startedAtMs;
    const secondTapElapsed=secondUp.completedAtMs-secondDown.startedAtMs;
    evidence.gestureTiming={firstTapElapsedMs:firstTapElapsed,firstTapMaxMs:options.gesture.firstTapMaxMs,
      interTapMinElapsedMs:interTapMinElapsed,interTapMaxElapsedMs:interTapMaxElapsed,
      interTapMaxMs:options.gesture.interTapMaxMs,secondTapElapsedMs:secondTapElapsed,source:options.gesture.source,
      clock:'Runtime performance.now() brackets the direct-touch RPC; TouchController receives SDL_GetTicks() during dispatch'};
    if(firstTapElapsed<0||interTapMinElapsed<0||interTapMaxElapsed<interTapMinElapsed||secondTapElapsed<0||
      firstTapElapsed>options.gesture.firstTapMaxMs||secondTapElapsed>options.gesture.firstTapMaxMs||
      interTapMaxElapsed>options.gesture.interTapMaxMs)
      throw new SceneIncomplete('The observed pointer transport exceeded the title-declared double-tap timing window.');

    const afterDouble=internalCheckpoint(batch,'double-tap-consumer').samples.map(sample=>sample.action);
    const usedDelta=action=>inventoryOf(action,'Double-tap consumer').bombsUsed-baselineInventory.bombsUsed;
    if(enabled){
      const triggerIndex=afterDouble.findIndex(action=>action?.complete===true&&action.bomb?.triggered===true&&
        action.bomb?.active===true&&Boolean(action.consumed.current&masks.bomb)&&usedDelta(action)===1);
      if(triggerIndex<0){
        if(afterDouble.some(action=>action?.available===false||action?.complete!==true))
          throw new SceneIncomplete('Double-tap case did not produce enough native consumer observations to assess Bomb activation.');
        assert.fail(`Enabled double-tap did not produce exactly one native Bomb within ${options.budgets.triggerTicks} consumer ticks`);
      }
      for(const action of afterDouble.slice(0,triggerIndex))assertNoBomb(action,baselineInventory,
        'Before native double-tap activation',masks.bomb);
      const trigger=afterDouble[triggerIndex];
      assert.equal(inventoryOf(trigger,'Native Bomb trigger').bombsRemaining,baselineInventory.bombsRemaining-1,
        'Double-tap did not consume one native Bomb inventory unit');
      assert.equal(inventoryOf(trigger,'Native Bomb trigger').bombsUsedStage,baselineInventory.bombsUsedStage+1,
        'Double-tap did not increment native stage Bomb use exactly once');
      for(const action of afterDouble.slice(triggerIndex)){
        if(action?.complete!==true)continue;
        assert.equal(usedDelta(action),1,'Double-tap caused more than one native Bomb use');
        assert.equal(inventoryOf(action,'Double-tap consumer').bombsRemaining,baselineInventory.bombsRemaining-1,
          'Double-tap consumed more than one native Bomb');
        assert.equal(inventoryOf(action,'Double-tap consumer').bombsUsedStage,baselineInventory.bombsUsedStage+1,
          'Double-tap repeated a native stage Bomb use');
      }
      const beforeBombMove=positionOf(internalCheckpoint(batch,'double-tap-consumer').samples.at(-1).observation,
        'Position before movement during Bomb');
      const movementDuringSamples=internalCheckpoint(batch,'primary-movement-during-bomb').samples;
      const movement=movementDuringSamples.find(sample=>sample.action.bomb.active&&(()=>{
        const position=positionOf(sample.observation,'Movement during native Bomb');
        return Math.hypot(position.x-beforeBombMove.x,position.y-beforeBombMove.y)>config.movementEpsilon;
      })());
      assert.ok(movement,'The primary movement finger did not produce native movement while Bomb remained active within its declared settling budget');
      const movedDuringBomb=positionOf(movement.observation,'Movement during native Bomb');
      evidence.positionChecks.push({label:'primary-finger-during-native-bomb',before:beforeBombMove,after:movedDuringBomb,
        primaryId:config.primaryId,bombActive:movement.action.bomb.active,
        settlingSamples:movementDuringSamples.map(sample=>({sequence:sample.action.sequence,frame:sample.action.frame,
          bombActive:sample.action.bomb.active,position:positionOf(sample.observation,'Movement during native Bomb')}))});

      const pulseSamples=[...afterDouble.slice(triggerIndex+1),...movementDuringSamples.map(sample=>sample.action),
        ...internalCheckpoint(batch,'bomb-pulse-drain').samples.map(sample=>sample.action)].filter(action=>action?.complete===true);
      assert.ok(pulseSamples.length>=options.gesture.bombPulseTicks+1,
        'The bounded wait did not include the full input pulse plus a clear native consumer sample');
      for(const action of pulseSamples)assert.equal(usedDelta(action),1,'Native Bomb inventory changed again while the first Bomb remained active');
      assert.equal(Boolean(pulseSamples.at(-1).consumed.current&masks.bomb),false,
        'The native Bomb input pulse remained held after its declared tick budget');
      evidence.bombObservation={expectedActivationCount:1,observedBombsUsedDelta:1,triggerSequence:trigger.sequence,
        bombActiveAtFinalObservation:internalCheckpoint(batch,'bomb-pulse-drain').samples.at(-1).action.bomb.active,
        pulseTicks:options.gesture.bombPulseTicks,observedPostTriggerConsumers:pulseSamples.length,
        boundedWaitTicks:options.budgets.triggerTicks+options.budgets.movementSettlingTicks+options.budgets.pulseDrainTicks,
        totalSceneTicks:config.totalBudgetTicks,
        completion:'native Bomb remains governed by its observed active state; the assertion waits only through the declared input pulse and checks stable inventory'};
    }else{
      const rows=[single,...afterDouble,...internalCheckpoint(batch,'primary-movement-during-bomb').samples.map(sample=>sample.action),
        ...internalCheckpoint(batch,'bomb-pulse-drain').samples.map(sample=>sample.action)];
      for(const action of rows){
        if(action?.complete!==true)throw new SceneIncomplete('Disabled double-tap case lacks a completed native consumer observation.');
        assertNoBomb(action,baselineInventory,'Disabled double-tap case',masks.bomb);
      }
      evidence.bombObservation={expectedActivationCount:0,observedBombsUsedDelta:0,
        boundedWaitTicks:options.budgets.triggerTicks+options.budgets.movementSettlingTicks+options.budgets.pulseDrainTicks,
        totalSceneTicks:config.totalBudgetTicks,
        completion:'all sampled native consumers retained the baseline Bomb state'};
    }
    result={checkStatus:'passed',reason:null,evidence};
  }catch(error){
    if(error instanceof SceneIncomplete)result=incomplete(error.message,evidence);
    else failure=error;
  }finally{
    if(result)evidence.sceneOutcome={checkStatus:result.checkStatus,reason:result.reason};
    const cleanupErrors=[];
    try{
      if(primaryMayBeHeld){
        const batch=await callIO('final-pointer-release',()=>io.directTouchBatch([pointer('up',config.primaryId,config.points.primaryBombMove),
          pointer('up',config.secondaryId,config.points.secondary)]));
        const expectedIds=new Set([config.primaryId,config.secondaryId]);
        evidence.cleanup={...(evidence.cleanup??{}),pointerReleased:batch?.acknowledged===true&&batch.events?.length===2&&
          batch.events.every(row=>row?.acknowledged===true&&row.type==='up'&&expectedIds.delete(row.id))&&expectedIds.size===0};
        if(evidence.cleanup.pointerReleased!==true)cleanupErrors.push(new Error('Runtime did not acknowledge release of both scene pointers'));
      }else evidence.cleanup={...(evidence.cleanup??{}),pointerReleased:true};
    }catch(error){cleanupErrors.push(error);}
    try{
      const acknowledgement=await callIO('final-neutral-controls',()=>io.controls({...neutral}));
      evidence.controls.push({label:'final-neutral-cleanup',controls:{...neutral},acknowledgement:acknowledgement??null});
      evidence.cleanup={...(evidence.cleanup??{}),controlsNeutral:acknowledgement?.acknowledged===true};
      if(acknowledgement?.acknowledged!==true)cleanupErrors.push(new Error('Runtime did not acknowledge final neutral controls'));
    }catch(error){cleanupErrors.push(error);}
    evidence.cleanup={...(evidence.cleanup??{}),errors:cleanupErrors.map(error=>error.message)};
    if(cleanupErrors.length)cleanupError=cleanupErrors.length===1?cleanupErrors[0]:new AggregateError(cleanupErrors,'Double-tap Bomb scene cleanup failed');
  }
  if(failure){failure.evidence=evidence;failure.touchDoubleTapBombEvidence=evidence;if(cleanupError)failure.touchDoubleTapBombCleanupError=cleanupError.message;throw failure;}
  if(cleanupError){const outcome=result?`${result.checkStatus}${result.reason?` (${result.reason})`:''}`:'no scene verdict';
    const error=new Error(`Double-tap Bomb scene ${outcome}; cleanup failed: ${cleanupError.message}`,{cause:cleanupError});
    error.touchDoubleTapBombEvidence=evidence;throw error;}
  return result;
}

function internalCheckpoint(batch,label){
  const checkpoint=batch.checkpoints?.find(item=>item?.label===label);
  if(!checkpoint||!Array.isArray(checkpoint.samples))throw new SceneIncomplete(`Direct-touch batch omitted checkpoint ${label}`);
  return checkpoint;
}
