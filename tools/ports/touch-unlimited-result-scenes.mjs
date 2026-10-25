import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';

const SCHEMA='eagler-touhou/touch-unlimited-result-scenes/1';
const REQUIRED_IO=['setNeutralTarget','setUnlimitedTarget','step','readState','readTouchTrace',
  'requestStageTransition','requestResultTransition','saveReplay','readReplayBytes'];
const MAX_WAIT_TICKS=120;
const EXPECTED_DROP_RATE=100;

class SceneIncomplete extends Error {}

function unavailable(value,label){
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label} is unavailable`);
  if(!value||value.available!==true)throw new SceneIncomplete(`${label} has no available receipt`);
  return value;
}
function finitePoint(value,label){
  if(!Number.isFinite(value?.x)||!Number.isFinite(value?.y))throw new SceneIncomplete(`${label} needs finite native coordinates`);
  return{x:value.x,y:value.y};
}
function requireSource(actual,expected,label){
  if(typeof actual!=='string'||!actual.trim()||typeof expected!=='string'||!expected.trim())
    throw new SceneIncomplete(`${label} has no declared source identity`);
  assert.equal(actual,expected,`${label} source does not match the title contract`);
}
function identityOf(state,preparation,contract,label){
  unavailable(state,`${label} Runtime state`);
  assert.equal(state.game,contract.game,`${label} Runtime game differs from the selected product`);
  assert.equal(state.variant,contract.variant,`${label} Runtime variant differs from the selected product`);
  assert.equal(state.runtimeEpoch,preparation.runtimeEpoch,`${label} crossed Runtime epochs`);
  assert.deepEqual(state.storageIdentity,preparation.storageIdentity,`${label} crossed normal storage identities`);
  assert.equal(state.runId,preparation.runId,`${label} crossed gameplay run identities`);
  requireSource(state.identitySource,contract.sources?.runtimeIdentity,`${label} Runtime/storage identity`);
  assert.equal(state.normal,true,`${label} is not an ordinary normal run`);
  assert.equal(state.practice,false,`${label} entered a practice run`);
  assert.equal(state.replay,false,`${label} entered Replay playback`);
  if(typeof state.cheatMovementUsed!=='boolean')throw new SceneIncomplete(`${label} has no read-only run marker`);
  requireSource(state.runIdSource,contract.sources?.runId,`${label} ordinary run identity`);
  requireSource(state.markerSource,contract.sources?.runMarker,`${label} run marker`);
  return state;
}
function validPreparation(preparation,contract){
  if(!preparation||preparation.available!==true||preparation.status!=='ready')
    throw new SceneIncomplete('A prepared ordinary Runtime receipt is required');
  assert.equal(preparation.game,contract.game,'Prepared Runtime game differs from the selected product');
  assert.equal(preparation.variant,contract.variant,'Prepared Runtime variant differs from the selected product');
  assert.equal(preparation.normal,true,'Preparation must identify an ordinary normal run');
  assert.equal(preparation.isolatedStorage,true,'Preparation must use isolated normal storage');
  for(const key of ['runtimeEpoch','runId'])if(preparation[key]===undefined||preparation[key]===null)
    throw new SceneIncomplete(`Preparation has no ${key} identity`);
  if(preparation.storageIdentity===undefined||preparation.storageIdentity===null)
    throw new SceneIncomplete('Preparation has no normal storage identity');
  requireSource(preparation.identitySource,contract.sources?.runtimeIdentity,'Prepared Runtime/storage identity');
  if(preparation.startReceipt?.available!==true||preparation.startReceipt?.status!=='passed')
    throw new SceneIncomplete('The normal gameplay start receipt is unavailable');
}
function traceCursor(trace,label){
  unavailable(trace,label);
  if(!Number.isSafeInteger(trace.nextSequence)||trace.nextSequence<1)
    throw new SceneIncomplete(`${label} has no next sequence boundary`);
  if(trace.dropped===true)throw new SceneIncomplete(`${label} reports dropped trace rows`);
  return trace.nextSequence-1;
}
function verifyTraceRow(row,{afterSequence,state,target,contract,label}){
  if(!row||!Number.isSafeInteger(row.sequence)||!Number.isSafeInteger(afterSequence))
    throw new SceneIncomplete(`${label} has no native trace sequence`);
  if(row.sequence!==afterSequence+1)throw new SceneIncomplete(`${label} trace sequence has a gap or stale row`);
  assert.equal(row.runId,state.runId,`${label} row belongs to another run`);
  assert.deepEqual(row.runtimeEpoch,state.runtimeEpoch,`${label} row belongs to another Runtime epoch`);
  assert.deepEqual(row.storageIdentity,state.storageIdentity,`${label} row belongs to another storage identity`);
  assert.deepEqual(row.stage,state.stage,`${label} row belongs to another stage`);
  assert.equal(row.mode,'unlimited',`${label} was not consumed in unlimited mode`);
  assert.equal(row.gameplayTick,true,`${label} was not consumed by a gameplay tick`);
  requireSource(row.source,contract.sources?.touchTrace,`${label} movement row`);
  const before=finitePoint(row.before,`${label} movement row`),after=finitePoint(row.after,`${label} movement row`),actualTarget=finitePoint(row.target,`${label} movement row`);
  assert.deepEqual(actualTarget,target,`${label} movement row did not consume the requested target`);
  return{sequence:row.sequence,runId:row.runId,stage:row.stage,runtimeEpoch:row.runtimeEpoch,storageIdentity:structuredClone(row.storageIdentity),before,after,target:actualTarget,
    mode:row.mode,gameplayTick:row.gameplayTick,source:row.source};
}
async function nextMotion(io,{afterSequence,state,target,contract,label,maxWaitTicks}){
  let cursor=afterSequence;
  for(let tick=0;tick<maxWaitTicks;tick++){
    const stepped=await io.step(1);
    if(stepped?.available!==true)throw new SceneIncomplete(stepped?.reason??`${label} logical step has no completed-tick receipt`);
    if(stepped.steps!==undefined&&stepped.steps!==1)throw new SceneIncomplete(`${label} did not advance exactly one logical tick`);
    const batch=unavailable(await io.readTouchTrace({afterSequence:cursor}),`${label} movement trace`);
    if(batch.dropped===true)throw new SceneIncomplete(`${label} movement trace reports dropped rows`);
    if(!Array.isArray(batch.rows))throw new SceneIncomplete(`${label} movement trace has no row list`);
    if(batch.firstAvailableSequence!==undefined&&batch.firstAvailableSequence>cursor+1)
      throw new SceneIncomplete(`${label} movement trace no longer covers the requested sequence`);
    if(batch.rows.length){
      const row=verifyTraceRow(batch.rows[0],{afterSequence:cursor,state,target,contract,label});
      if(batch.rows.length>1&&batch.rows[1].sequence!==row.sequence+1)
        throw new SceneIncomplete(`${label} movement trace contains a sequence gap`);
      return row;
    }
    const next=traceCursor(batch,`${label} movement trace`);
    if(next!==cursor)throw new SceneIncomplete(`${label} movement trace advanced without returning rows`);
  }
  throw new SceneIncomplete(`${label} had no consumed movement row within ${maxWaitTicks} logical ticks`);
}
function validateTransitionReceipt(receipt,source,label){
  unavailable(receipt,`${label} request`);
  assert.equal(receipt.accepted,true,`${label} request was rejected`);
  requireSource(receipt.source,source,`${label} request`);
  return structuredClone(receipt);
}
async function waitForState(io,{preparation,contract,predicate,label,maxWaitTicks}){
  for(let tick=0;tick<maxWaitTicks;tick++){
    const state=identityOf(await io.readState(),preparation,contract,label);
    if(predicate(state))return state;
    const stepped=await io.step(1);
    if(stepped?.available!==true)throw new SceneIncomplete(stepped?.reason??`${label} logical step has no completed-tick receipt`);
    if(stepped.steps!==undefined&&stepped.steps!==1)throw new SceneIncomplete(`${label} did not advance exactly one logical tick`);
  }
  throw new SceneIncomplete(`${label} was not observed within ${maxWaitTicks} logical ticks`);
}
function bytesOf(value,label){
  const bytes=value?.bytes??value;
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label} is unavailable`);
  if(!(Buffer.isBuffer(bytes)||bytes instanceof Uint8Array))throw new SceneIncomplete(`${label} did not return original bytes`);
  return Buffer.from(bytes);
}

/**
 * Verify unlimited-touch run semantics against a prepared ordinary Runtime.
 * IO only supplies real controls, bounded logical steps, read-only observations,
 * production transition requests and the production Replay finalizer.
 */
export async function runTouchUnlimitedResultSceneGroup(io,{contract,preparation,slot=1,name='TCH10',maxWaitTicks=8}={}){
  const evidence={schema:SCHEMA,product:contract?{game:contract.game,variant:contract.variant}:null,
    preparation:preparation?structuredClone(preparation):null,states:{},steps:[],transitions:[],result:null,replay:null,cleanup:null};
  const missing=REQUIRED_IO.filter(key=>typeof io?.[key]!=='function');
  if(typeof contract?.inspectReplay!=='function')missing.push('contract.inspectReplay');
  if(typeof contract?.nonzeroTarget!=='function')missing.push('contract.nonzeroTarget');
  if(!contract?.sources||typeof contract.sources!=='object')missing.push('contract.sources');
  if(missing.length)return{schema:SCHEMA,checkStatus:'incomplete',reason:`Touch-unlimited IO is missing ${missing.join(', ')}`,evidence};
  if(!Number.isSafeInteger(maxWaitTicks)||maxWaitTicks<1||maxWaitTicks>MAX_WAIT_TICKS)
    throw new RangeError(`Touch-unlimited maxWaitTicks must be between 1 and ${MAX_WAIT_TICKS}`);

  let result=null,failure=null,cleanupError=null;
  try{
    validPreparation(preparation,contract);
    assert.ok(Number.isSafeInteger(slot)&&slot>=1, 'Replay slot must be a positive safe integer');
    assert.equal(typeof name,'string');assert.ok(name.trim(),'Replay name must be nonempty');
    const initialNeutral=await io.setNeutralTarget();
    if(initialNeutral?.available!==true)throw new SceneIncomplete(initialNeutral?.reason??'Initial neutral input has no acknowledgement');
    assert.notEqual(initialNeutral.accepted,false,'Runtime rejected initial neutral input');

    const baseline=identityOf(await io.readState(),preparation,contract,'Initial gameplay');
    evidence.states.baseline=structuredClone(baseline);
    assert.equal(baseline.scene,'gameplay','Touch-unlimited scene must start in gameplay');
    assert.equal(baseline.cheatMovementUsed,false,'A fresh ordinary run must not start with the unlimited marker');
    const position=finitePoint(baseline.playerPosition,'Initial gameplay state');
    const traceBefore=await io.readTouchTrace({afterSequence:null});
    const initialSequence=traceCursor(traceBefore,'Initial movement trace');
    evidence.traceInitialSequence=initialSequence;

    const zeroTarget=typeof contract.stationaryTarget==='function'?
      finitePoint(contract.stationaryTarget(baseline),'Stationary target'):position;
    assert.deepEqual(zeroTarget,position,'The stationary control target must equal the observed player position');
    const zeroReceipt=unavailable(await io.setUnlimitedTarget({target:zeroTarget,requestId:'touch-unlimited-zero'}),
      'Stationary unlimited-touch control');
    assert.equal(zeroReceipt.accepted,true,'Runtime did not acknowledge the stationary unlimited-touch target');
    const zeroTargetActual=finitePoint(zeroReceipt.target,'Stationary target receipt');
    assert.deepEqual(zeroTargetActual,zeroTarget,'Stationary target receipt differs from the requested target');
    const zeroRow=await nextMotion(io,{afterSequence:initialSequence,state:baseline,target:zeroTargetActual,contract,
      label:'Stationary unlimited-touch control',maxWaitTicks});
    assert.deepEqual(zeroRow.before,zeroRow.after,'Stationary unlimited-touch input moved the player');
    const afterZero=identityOf(await io.readState(),preparation,contract,'Stationary movement result');
    evidence.states.afterZero=structuredClone(afterZero);
    assert.equal(afterZero.scene,'gameplay');assert.equal(afterZero.cheatMovementUsed,false,
      'A zero-displacement unlimited-touch sample must not set the run marker');
    assert.deepEqual(afterZero.playerPosition,zeroRow.after,'Stationary movement trace differs from the native player position');
    evidence.steps.push({kind:'zero-displacement-unlimited',receipt:structuredClone(zeroReceipt),row:zeroRow,
      marker:afterZero.cheatMovementUsed,stage:afterZero.stage,runId:afterZero.runId});

    const movingTarget=finitePoint(contract.nonzeroTarget(afterZero),'Nonzero target');
    assert.ok(movingTarget.x!==afterZero.playerPosition.x||movingTarget.y!==afterZero.playerPosition.y,
      'The nonzero touch target must differ from the observed player position');
    const movingReceipt=unavailable(await io.setUnlimitedTarget({target:movingTarget,requestId:'touch-unlimited-nonzero'}),
      'Nonzero unlimited-touch control');
    assert.equal(movingReceipt.accepted,true,'Runtime did not acknowledge the nonzero unlimited-touch target');
    const movingTargetActual=finitePoint(movingReceipt.target,'Nonzero target receipt');
    assert.deepEqual(movingTargetActual,movingTarget,'Nonzero target receipt differs from the requested target');
    const movingRow=await nextMotion(io,{afterSequence:zeroRow.sequence,state:afterZero,target:movingTargetActual,contract,
      label:'Nonzero unlimited-touch control',maxWaitTicks});
    assert.ok(movingRow.after.x!==movingRow.before.x||movingRow.after.y!==movingRow.before.y,
      'Unlimited-touch movement was not nonzero at the gameplay consumer');
    const afterMove=identityOf(await io.readState(),preparation,contract,'Nonzero movement result');
    evidence.states.afterMove=structuredClone(afterMove);
    assert.equal(afterMove.scene,'gameplay');assert.equal(afterMove.cheatMovementUsed,true,
      'A consumed nonzero unlimited-touch movement did not set the run marker');
    assert.deepEqual(afterMove.playerPosition,movingRow.after,'Movement trace differs from the native player position');
    evidence.steps.push({kind:'nonzero-unlimited',receipt:structuredClone(movingReceipt),row:movingRow,
      marker:afterMove.cheatMovementUsed,stage:afterMove.stage,runId:afterMove.runId});

    const stageRequest=validateTransitionReceipt(await io.requestStageTransition({runId:afterMove.runId,stage:afterMove.stage}),
      contract.sources.stageTransition,'Production stage transition');
    const afterStage=await waitForState(io,{preparation,contract,label:'Next gameplay stage',maxWaitTicks,
      predicate:state=>state.scene==='gameplay'&&!Object.is(state.stage,afterMove.stage)});
    evidence.states.afterStage=structuredClone(afterStage);
    assert.equal(afterStage.cheatMovementUsed,true,'The run marker was cleared across the stage transition');
    if(afterStage.resultLag!==null&&afterStage.resultLag!==undefined)
      throw new SceneIncomplete('Result data appeared before the production Result transition');
    evidence.transitions.push({kind:'stage',request:stageRequest,from:afterMove.stage,to:afterStage.stage,
      runId:afterStage.runId,marker:afterStage.cheatMovementUsed});

    const resultRequest=validateTransitionReceipt(await io.requestResultTransition({runId:afterStage.runId,stage:afterStage.stage}),
      contract.sources.resultTransition,'Production Result transition');
    const resultState=await waitForState(io,{preparation,contract,label:'Production Result',maxWaitTicks,
      predicate:state=>state.scene==='result'&&state.resultLag?.available===true});
    evidence.states.result=structuredClone(resultState);
    assert.equal(resultState.cheatMovementUsed,true,'The Result belongs to a run without the unlimited marker');
    const resultLag=resultState.resultLag;
    if(!resultLag||resultLag.available!==true||!Number.isFinite(resultLag.value))
      throw new SceneIncomplete('The Result high-score processing-drop value is unavailable');
    requireSource(resultLag.source,contract.sources.resultLag,'Result high-score processing-drop value');
    assert.equal(resultLag.value,EXPECTED_DROP_RATE,'Production Result did not record 100% processing drop');
    evidence.transitions.push({kind:'result',request:resultRequest,runId:resultState.runId,stage:resultState.stage});
    evidence.result={runId:resultState.runId,stage:resultState.stage,marker:resultState.cheatMovementUsed,
      lag:resultLag.value,source:resultLag.source};

    const saved=unavailable(await io.saveReplay({slot,name}), 'Production Replay finalizer');
    assert.equal(saved.saved,true,'Production Replay finalizer did not save the recording');
    requireSource(saved.source,contract.sources.replayFinalizer,'Production Replay finalizer');
    if(typeof saved.path!=='string'||!saved.path.trim())throw new SceneIncomplete('Replay finalizer returned no product-owned path');
    const replayRead=unavailable(await io.readReplayBytes({path:saved.path}),'Saved Replay read');
    assert.deepEqual(replayRead.storageIdentity,preparation.storageIdentity,'Saved Replay was read from another storage identity');
    const raw=bytesOf(replayRead,'Saved Replay');
    const sha256=createHash('sha256').update(raw).digest('hex');
    if(saved.sha256!==undefined)assert.equal(saved.sha256,sha256,'Saved Replay finalizer digest differs from original stored bytes');
    const parsed=unavailable(await contract.inspectReplay(raw,{expectedSha256:sha256}),'Independent Replay parser');
    requireSource(parsed.source,contract.sources.replayParser,'Independent Replay parser');
    assert.equal(parsed.sha256,sha256,'Independent Replay parser returned a different byte identity');
    if(typeof parsed.metadata?.lag!=='number'||!Number.isFinite(parsed.metadata.lag))
      throw new SceneIncomplete('Independent Replay parser did not expose the stored processing-drop field');
    assert.equal(parsed.metadata.practice,false,'Saved Replay is not an ordinary normal-game recording');
    assert.equal(parsed.metadata.lag,EXPECTED_DROP_RATE,'Saved Replay did not store 100% processing drop');
    evidence.replay={path:saved.path,method:saved.source,bytes:raw.length,sha256,
      ...(replayRead.artifact?{artifact:replayRead.artifact}:{}),
      storageIdentity:structuredClone(replayRead.storageIdentity),metadataLag:parsed.metadata.lag,
      practice:parsed.metadata.practice,parser:parsed.source};
    result={schema:SCHEMA,checkStatus:'passed',reason:null,evidence};
  }catch(error){
    if(error instanceof SceneIncomplete)result={schema:SCHEMA,checkStatus:'incomplete',reason:error.message,evidence};
    else failure=error;
  }finally{
    try{
      const neutral=await io.setNeutralTarget();
      if(neutral?.available===false)throw new Error(neutral.reason??'Neutral input cleanup was unavailable');
      if(neutral?.accepted===false)throw new Error('Runtime rejected neutral input cleanup');
      evidence.cleanup={neutralized:true,receipt:neutral??null};
    }catch(error){cleanupError=error;evidence.cleanup={neutralized:false,reason:error?.message??String(error)};}
  }
  if(failure&&cleanupError)throw new AggregateError([failure,cleanupError],'Touch-unlimited scene and input cleanup both failed');
  if(failure){failure.evidence=evidence;throw failure;}
  if(cleanupError)throw cleanupError;
  return result;
}

/** Recompute the ordinary-run marker and native Result from retained observations. */
export function assertTouchUnlimitedResultEvidence(e,{contract,parsed}={}){
  assert.equal(e.schema,SCHEMA);assert.deepEqual(e.product,{game:contract.game,variant:contract.variant});
  const p=e.preparation;validPreparation(p,contract);
  assert.deepEqual(Object.keys(e.states),['baseline','afterZero','afterMove','afterStage','result']);
  for(const [label,state] of Object.entries(e.states))identityOf(state,p,contract,label);
  const {baseline,afterZero,afterMove,afterStage,result}=e.states;
  for(const state of [baseline,afterZero,afterMove,afterStage])assert.equal(state.scene,'gameplay');
  assert.equal(baseline.cheatMovementUsed,false);assert.equal(afterZero.cheatMovementUsed,false);
  for(const state of [afterMove,afterStage,result])assert.equal(state.cheatMovementUsed,true);
  assert.deepEqual(e.steps.map(row=>row.kind),['zero-displacement-unlimited','nonzero-unlimited']);
  const zero=e.steps[0],moving=e.steps[1],target=finitePoint(contract.nonzeroTarget(afterZero),'Independent movement target');
  assert.equal(zero.receipt.available,true);assert.equal(zero.receipt.accepted,true);assert.deepEqual(zero.receipt.target,baseline.playerPosition);
  verifyTraceRow(zero.row,{afterSequence:e.traceInitialSequence,state:baseline,target:baseline.playerPosition,contract,label:'Stationary control'});
  assert.deepEqual(zero.row.before,zero.row.after);assert.deepEqual(afterZero.playerPosition,zero.row.after);
  assert.equal(moving.receipt.available,true);assert.equal(moving.receipt.accepted,true);assert.deepEqual(moving.receipt.target,target);
  verifyTraceRow(moving.row,{afterSequence:zero.row.sequence,state:afterZero,target,contract,label:'Nonzero control'});
  assert.deepEqual(moving.row.before,afterZero.playerPosition);assert.notDeepEqual(moving.row.before,moving.row.after);
  assert.deepEqual(moving.row.after,target);assert.deepEqual(afterMove.playerPosition,moving.row.after);
  assert.notEqual(afterStage.stage,afterMove.stage);assert.equal(afterStage.resultLag,null);
  assert.deepEqual(e.transitions.map(row=>row.kind),['stage','result']);
  validateTransitionReceipt(e.transitions[0].request,contract.sources.stageTransition,'Stage');
  validateTransitionReceipt(e.transitions[1].request,contract.sources.resultTransition,'Result');
  assert.equal(e.transitions[0].from,afterMove.stage);assert.equal(e.transitions[0].to,afterStage.stage);
  assert.equal(result.scene,'result');assert.equal(result.resultLag.available,true);assert.equal(result.resultLag.value,EXPECTED_DROP_RATE);
  requireSource(result.resultLag.source,contract.sources.resultLag,'Result lag');
  requireSource(e.replay.method,contract.sources.replayFinalizer,'Replay finalizer');
  requireSource(parsed.source,contract.sources.replayParser,'Replay parser');assert.equal(parsed.sha256,e.replay.sha256);
  assert.equal(parsed.metadata.practice,false);assert.equal(parsed.metadata.lag,EXPECTED_DROP_RATE);
  assert.deepEqual(e.replay.storageIdentity,p.storageIdentity);
  assert.equal(e.cleanup.neutralized,true);assert.equal(e.cleanup.receipt.available,true);assert.equal(e.cleanup.receipt.accepted,true);
}
