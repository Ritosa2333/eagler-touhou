import assert from 'node:assert/strict';

const SCHEMA='eagler-touhou/loaded-dialogue-skip-scenes/1';
const MAX_TICKS=180;
class SceneIncomplete extends Error{}
function incomplete(reason,evidence){return{checkStatus:'incomplete',reason,evidence};}
function timeout(error){return error?.name==='TimeoutError'||error?.code==='ETIMEDOUT'||error?.timeout===true;}
function validMask(value){return Number.isSafeInteger(value)&&value>0&&value<=0x80000000&&Number.isInteger(Math.log2(value));}
function point(value){return Number.isFinite(value?.x)&&value.x>=0&&value.x<=1&&Number.isFinite(value?.y)&&value.y>=0&&value.y<=1;}
function validate(options={}){
  assert.ok(['keyboard','gamepad','touch'].includes(options.ingress),`Unsupported dialogue Skip ingress ${String(options.ingress)}`);
  assert.equal(options.expected?.variant,'normal','Dialogue Skip adapter must explicitly declare its normal Runtime variant');
  assert.ok(Number.isSafeInteger(options.expected?.stage)&&options.expected.stage>=0,'Dialogue Skip adapter must declare its stage');
  assert.ok(Number.isSafeInteger(options.expected?.touchContext)&&options.expected.touchContext>=0,'Dialogue Skip adapter must declare the active modal context');
  assert.ok(Number.isSafeInteger(options.expected?.gameplayTouchContext)&&options.expected.gameplayTouchContext>=0,
    'Dialogue Skip adapter must declare its pre-dialogue gameplay touch context');
  for(const name of ['fire','confirm','skip'])assert.ok(validMask(options.inputMasks?.[name]),`Dialogue adapter must declare a single-bit ${name} mask`);
  assert.notEqual(options.inputMasks.confirm,options.inputMasks.skip,'Dialogue confirm and Skip masks must be distinct');
  assert.ok(Number.isSafeInteger(options.waitOpcode)&&options.waitOpcode>=0&&options.waitOpcode<=255,'Dialogue adapter must declare its MessageProgram Wait opcode');
  assert.ok(Number.isSafeInteger(options.consumerLatencyTicks)&&options.consumerLatencyTicks>=0&&options.consumerLatencyTicks<=8,
    'Dialogue adapter must declare command-to-modal consumer latency');
  assert.ok(typeof options.sources?.manifest==='string'&&options.sources.manifest.length>0,'Dialogue scene needs the loaded-script manifest source');
  assert.ok(typeof options.sources?.consumer==='string'&&options.sources.consumer.length>0,'Dialogue scene needs the native consumer source');
  assert.ok(typeof options.gesture?.source==='string'&&options.gesture.source.length>0,'Dialogue Skip needs its production gesture source');
  if(options.ingress==='touch')assert.ok(Number.isFinite(options.gesture?.skipHoldMs)&&options.gesture.skipHoldMs>0&&options.gesture.skipHoldMs<=5000,
    'Touch adapter must declare a bounded long-press Skip threshold');
  const b=options.budgets??{};
  for(const name of ['baselineTicks','openTicks','waitTicks','skipTicks'])
    assert.ok(Number.isSafeInteger(b[name])&&b[name]>0&&b[name]<=MAX_TICKS,`Dialogue Skip needs a bounded ${name}`);
  assert.ok(typeof b.source==='string'&&b.source.length>0,'Dialogue Skip budgets need their production source anchor');
  assert.ok(baselineControls(options.controls),'Dialogue Skip needs held-Fire and neutral touch-control snapshots');
  if(options.ingress==='touch'){
    assert.ok(Number.isSafeInteger(options.pointer?.id)&&options.pointer.id>=0,'Long-press touch Skip needs a stable pointer id');
    assert.ok(point(options.pointer?.point),'Long-press touch Skip needs a normalized canvas point');
  }
  const total=b.baselineTicks+b.openTicks+b.waitTicks+2*options.consumerLatencyTicks+b.skipTicks+6;
  assert.ok(total<=MAX_TICKS,`Dialogue Skip exceeds its ${MAX_TICKS}-tick shared bound`);
  return total;
}
function baselineControls(value){return typeof value?.heldFire?.fireEnabled==='boolean'&&value.heldFire.fireEnabled===true&&
  typeof value?.neutral?.fireEnabled==='boolean'&&value.neutral.fireEnabled===false;}
function requireIdentity(value,label,expected){
  if(value?.available===false)throw new SceneIncomplete(value.reason??`${label}: native dialogue observation unavailable`);
  const runtime=value?.runtime;
  for(const key of ['generation','pointer','epoch'])if(!Number.isSafeInteger(runtime?.[key])||
    (key==='epoch'?runtime[key]<0:runtime[key]<=0))
    throw new SceneIncomplete(`${label}: native Runtime ${key} identity is missing`);
  if(expected)assert.deepEqual(runtime,expected,`${label}: Runtime identity changed during dialogue Skip`);
  assert.equal(value.scene?.variant,'normal',`${label}: Runtime variant changed`);
  const fixture=value.fixture;
  assert.equal(fixture?.used,true,`${label}: controlled-scene receipt is missing`);
  assert.equal(fixture?.gateEnabled,true,`${label}: native validation gate is not active`);
  assert.equal(fixture?.writeLocked,true,`${label}: disposable write lock is missing`);
  assert.equal(fixture?.loadScope,false,`${label}: fixture load scope is active`);
  assert.equal(fixture?.committedBytes,0,`${label}: fixture committed persistent bytes`);
  return runtime;
}
function validateManifest(manifest,options){
  if(manifest?.available===false)throw new SceneIncomplete(manifest.reason??'Loaded dialogue source is unavailable');
  if(manifest?.available!==true)throw new SceneIncomplete('Loaded dialogue manifest did not confirm its source availability');
  assert.equal(manifest?.schema,'eagler-touhou/dialogue-touch-manifest/1','Dialogue manifest schema is unsupported');
  assert.equal(manifest.source?.variant,options.expected.variant,'Loaded dialogue source has the wrong Runtime variant');
  assert.equal(manifest.source?.stage,options.expected.stage,'Loaded dialogue source has the wrong stage');
  const runtime=manifest.runtime;for(const key of ['generation','pointer','epoch'])
    if(!Number.isSafeInteger(runtime?.[key])||(key==='epoch'?runtime[key]<0:runtime[key]<=0))
      throw new SceneIncomplete(`Loaded dialogue manifest lacks Runtime ${key} identity`);
  assert.equal(manifest.fixture?.used,true,'Loaded dialogue manifest lacks its controlled-scene receipt');
  assert.equal(manifest.fixture?.gateEnabled,true,'Loaded dialogue manifest lacks the active native validation gate');
  assert.equal(manifest.fixture?.writeLocked,true,'Loaded dialogue manifest lacks the disposable write lock');
  assert.equal(manifest.fixture?.loadScope,false,'Loaded dialogue manifest reports an active file load scope');
  assert.equal(manifest.fixture?.committedBytes,0,'Loaded dialogue manifest reports persistent writes');
  for(const key of ['ecl','message']){
    const source=manifest.source?.[key];
    assert.ok(typeof source?.asset==='string'&&source.asset.length>0,`Loaded ${key} asset identity is missing`);
    assert.ok(Number.isSafeInteger(source?.byteLength)&&source.byteLength>0,`Loaded ${key} byte length is missing`);
    assert.match(source?.fnv64??'',/^[0-9a-f]{16}$/i,`Loaded ${key} source hash is missing`);
  }
  assert.ok(Number.isSafeInteger(manifest.source?.difficultyMask)&&manifest.source.difficultyMask>0,'Loaded ECL difficulty mask is missing');
  assert.ok(Array.isArray(manifest.candidates)&&manifest.candidates.length>0,'No live ECL reference reaches a confirm Wait');
  for(const row of manifest.candidates){
    assert.ok(Number.isSafeInteger(row?.entry)&&row.entry>=0,'Manifest candidate entry is invalid');
    assert.ok(Number.isSafeInteger(row?.ecl?.timeline)&&row.ecl.timeline>=0&&Number.isSafeInteger(row.ecl?.time)&&row.ecl.time>=0,
      'Manifest candidate lacks its ECL source location');
    assert.ok(Number.isSafeInteger(row?.ecl?.referenceCount)&&row.ecl.referenceCount>0,'Manifest candidate has no live ECL reference');
    assert.ok((row.ecl.difficultyBits&manifest.source.difficultyMask)!==0,'Manifest candidate is inactive at the current difficulty');
    assert.ok(Number.isSafeInteger(row?.message?.waitOffset)&&row.message.waitOffset>=0,'Manifest candidate lacks a real MSG Wait offset');
    assert.ok(Number.isSafeInteger(row?.message?.waitTime)&&row.message.waitTime>=0,'Manifest candidate lacks its initial Wait time');
    assert.ok(Number.isSafeInteger(row?.message?.timeoutFrames)&&row.message.timeoutFrames>0,'Manifest candidate lacks a bounded automatic timeout');
  }
  return manifest.candidates.slice().sort((a,b)=>b.message.timeoutFrames-a.message.timeoutFrames||a.entry-b.entry)[0];
}
function pointerReceipt(receipt,type,id,label){
  if(receipt?.available===false)throw new SceneIncomplete(receipt.reason??`${label}: touch transport unavailable`);
  if(receipt?.acknowledged!==true||receipt.source!=='touch'||receipt.type!==type||receipt.id!==id||
     !Number.isFinite(receipt.startedAtMs)||!Number.isFinite(receipt.completedAtMs)||receipt.completedAtMs<receipt.startedAtMs)
    throw new SceneIncomplete(`${label}: touch transport receipt is missing, mismatched or malformed`);
  return receipt;
}

/** Open a real ECL-referenced loaded MessageProgram entry and prove Skip at its native Wait. */
export async function runLoadedDialogueSkipScene(io,options={}){
  const evidence={schema:SCHEMA,ingress:options.ingress??null,expected:structuredClone(options.expected??null),gesture:structuredClone(options.gesture??null),
    budgets:structuredClone(options.budgets??null),manifest:null,candidate:null,controls:[],baseline:[],open:[],wait:[],inputEvents:[],skipSamples:[],cleanup:{}};
  const required=['readManifest','observe','requestMessage','controls','readActions','step','send'];
  if(options.ingress==='touch')required.push('waitPhysical');
  const missing=required.filter(name=>typeof io?.[name]!=='function');
  if(missing.length)return incomplete(`Loaded dialogue Skip IO is missing ${missing.join(', ')}`,evidence);
  if(!options.expected||!options.inputMasks||!options.controls||!options.budgets||!options.sources||!options.gesture)
    return incomplete('Loaded dialogue Skip requires title identity, masks, controls, gesture and bounded source declarations.',evidence);
  const total=validate(options);evidence.totalBudgetTicks=total;
  let runtime=null,result=null,failure=null,heldTouch=false;const heldInputs=new Set();let actionSequence=null,dialogueSequence=null;
  const call=async(label,fn)=>{try{return await fn();}catch(error){evidence.ioErrors??=[];evidence.ioErrors.push({label,name:error?.name??null,message:String(error?.message??error)});
    if(timeout(error))throw new SceneIncomplete(`${label}: native dialogue IO timed out`);throw error;}};
  const observe=async(label)=>{
    const value=await call(label,()=>io.observe());runtime=requireIdentity(value,label,runtime);
    if(value.scene.stage!==options.expected.stage)throw new SceneIncomplete(`${label}: native stage differs from the declared dialogue source`);
    const dialogue=value.dialogue;
    if(!dialogue||typeof dialogue.present!=='boolean'||typeof dialogue.consumer?.valid!=='boolean'||!Number.isSafeInteger(dialogue.consumer.sequence))
      throw new SceneIncomplete(`${label}: native Dialogue consumer receipt is missing`);
    if(dialogue.present&&!dialogue.consumer.valid)throw new SceneIncomplete(`${label}: active Dialogue did not publish a valid actual consumer receipt`);
    if(dialogue.present&&value.touch?.context!==options.expected.touchContext)
      throw new SceneIncomplete(`${label}: TouchController did not enter the adapter-declared Dialogue input context`);
    if(dialogue.consumer.valid){
      if(dialogueSequence!==null&&dialogue.consumer.sequence!==dialogueSequence+1)
        throw new SceneIncomplete(`${label}: Dialogue consumer sequence skipped or repeated`);
      dialogueSequence=dialogue.consumer.sequence;
    }
    return value;
  };
  const action=async(label)=>{
    const value=await call(label,()=>io.readActions());
    if(value?.available===false)throw new SceneIncomplete(value.reason??`${label}: native player consumer unavailable`);
    if(value?.complete!==true)throw new SceneIncomplete(`${label}: no completed native player-consumer sample`);
    if(value.phase!=='consumer-run')throw new SceneIncomplete(`${label}: native player consumer is not running`);
    if(!Number.isSafeInteger(value.sequence)||value.sequence<=0)throw new SceneIncomplete(`${label}: player consumer sequence is missing`);
    if(actionSequence!==null&&value.sequence!==actionSequence+1)throw new SceneIncomplete(`${label}: player consumer sequence skipped or repeated`);
    actionSequence=value.sequence;return value;
  };
  const setControls=async(label,value)=>{
    const ack=await call(label,()=>io.controls(structuredClone(value)));evidence.controls.push({label,controls:structuredClone(value),acknowledgement:structuredClone(ack??null)});
    if(ack?.acknowledged!==true)throw new SceneIncomplete(`${label}: Runtime did not acknowledge the touch-control snapshot`);
  };
  const send=async(label,operation)=>{
    const receipt=await call(label,()=>io.send({source:options.ingress,...operation}));evidence.inputEvents.push({label,operation:structuredClone(operation),receipt:structuredClone(receipt??null)});
    if(options.ingress==='touch')return pointerReceipt(receipt,operation.type,operation.id,label);
    if(receipt?.available===false)throw new SceneIncomplete(receipt.reason??`${label}: input transport unavailable`);
    if(receipt?.acknowledged!==true||receipt.source!==options.ingress||receipt.action!==operation.action||receipt.down!==operation.down)
      throw new SceneIncomplete(`${label}: mapped input receipt does not match the requested ${options.ingress} Skip event`);
    return receipt;
  };
  const stepObserve=async(label)=>{await call(`${label}-step`,()=>io.step(1));return observe(`${label}-observation`);};
  try{
    const manifest=await call('read-loaded-dialogue-manifest',()=>io.readManifest());evidence.manifest=structuredClone(manifest??null);
    const candidate=validateManifest(manifest,options);evidence.candidate=structuredClone(candidate);runtime=structuredClone(manifest.runtime);
    await setControls('enable-touch-Fire-for-production-dialogue-request',options.controls.heldFire);
    let fire=false;
    for(let tick=0;tick<options.budgets.baselineTicks;tick++){
      await call('held-Fire-player-step',()=>io.step(1));const sample=await action('held-Fire-player-consumer');
      const observation=await observe('held-Fire-native-observation');
      evidence.baseline.push({action:structuredClone(sample),observation:structuredClone(observation)});
      assert.ok(Number.isSafeInteger(sample.consumed?.current),'Player consumer sample lacks the consumed native input');
      if(sample.consumed.current&options.inputMasks.fire){fire=true;break;}
      assert.equal(observation.dialogue.present,false,'Dialogue was already active before its controlled fixture request');
    }
    assert.equal(fire,true,'Touch controls did not produce a real native gameplay Fire sample before opening Dialogue');
    const before=evidence.baseline.at(-1).observation;
    assert.equal(before.dialogue.present,false,'Dialogue was already active at the fixture request boundary');
    assert.equal(before.touch?.context,options.expected.gameplayTouchContext,
      'The held-Fire baseline is not in the declared pre-dialogue context');
    assert.equal(before.touch?.fireEnabled,true,'The actual Runtime did not retain the declared held-Fire touch control');
    const requested=await call('request-loaded-production-dialogue-entry',()=>io.requestMessage(candidate.entry));
    evidence.request={entry:candidate.entry,result:structuredClone(requested??null)};
    if(requested?.accepted!==true&&requested?.status!=='accepted'&&requested!==1)
      throw new SceneIncomplete(`Production Dialogue owner rejected loaded ECL entry ${candidate.entry}`);

    let waitState=null;
    for(let tick=0;tick<options.budgets.openTicks;tick++){
      const value=await stepObserve('open-real-message');evidence.open.push(structuredClone(value));
      const d=value.dialogue;
      assert.equal(d.present,true,'Production Dialogue owner did not open the requested real entry');
      assert.equal(d.message,candidate.entry,'Production Dialogue owner opened a different entry');
      assert.equal(d.freshConfirmEdge,false,'Held gameplay Fire created an unexpected Dialogue confirm edge');
      assert.equal(value.touch?.fireEnabled,true,'Touch Fire owner changed while the production Dialogue was opening');
      if(d.instructionOffset===candidate.message.waitOffset&&d.opcode===options.waitOpcode&&d.timer>=candidate.message.waitTime){waitState=value;break;}
    }
    if(!waitState)throw new SceneIncomplete('Real MessageProgram entry did not reach its initial Wait within the declared open budget');
    if(waitState.dialogue.skippable!==true)throw new SceneIncomplete('Selected real MessageProgram Wait is not skippable under the native Dialogue state');
    let waited=0;
    while(waitState.dialogue.pausedFrames<waitState.dialogue.minimumWait&&waited<options.budgets.waitTicks){
      if(candidate.message.timeoutFrames-waitState.dialogue.pausedFrames<=1)throw new SceneIncomplete('MessageProgram Wait times out before native Skip becomes eligible');
      waitState=await stepObserve('wait-for-native-dialogue-skip-eligibility');evidence.wait.push(structuredClone(waitState));++waited;
      assert.equal(waitState.dialogue.present,true,'MessageProgram retired before the native Skip trial');
      assert.equal(waitState.dialogue.message,candidate.entry,'MessageProgram changed before the native Skip trial');
      assert.equal(waitState.dialogue.instructionOffset,candidate.message.waitOffset,'MessageProgram left its Wait before Skip input');
      assert.equal(waitState.dialogue.freshConfirmEdge,false,'No-input wait unexpectedly generated a confirm edge');
    }
    assert.equal(waitState.touch?.fireEnabled,true,'Touch Fire owner changed before the native Skip trial');
    if(waitState.dialogue.pausedFrames<waitState.dialogue.minimumWait)
      throw new SceneIncomplete('Dialogue did not reach its native minimum-input interval within the declared wait budget');
    evidence.wait.push(structuredClone(waitState));
    let holdStartedAt=null,holdEndedAt=null;
    if(options.ingress==='touch'){
      const p=options.pointer,id=p.id;
      const down=await send('long-touch-Skip-down',{type:'down',id,x:p.point.x,y:p.point.y});heldTouch=true;holdStartedAt=down.completedAtMs;
      const waitedPhysical=await call('hold-pointer-past-native-Skip-threshold',()=>io.waitPhysical(options.gesture.skipHoldMs));
      if(!Number.isFinite(waitedPhysical?.startedAtMs)||!Number.isFinite(waitedPhysical?.completedAtMs)||
         waitedPhysical.startedAtMs<holdStartedAt||waitedPhysical.completedAtMs-waitedPhysical.startedAtMs<options.gesture.skipHoldMs)
        throw new SceneIncomplete('Touch hold did not provide a real transport-time interval past the declared Skip threshold');
    }else{
      await send('native-Skip-key-down',{action:'skip',down:true});heldInputs.add('skip');
    }
    for(let tick=0;tick<options.consumerLatencyTicks;tick++){
      const queued=await stepObserve(`skip-input-queued-${tick+1}`);evidence.skipSamples.push(structuredClone(queued));
      assert.equal(queued.dialogue.currentInput&options.inputMasks.skip,0,'Skip reached the Dialogue consumer before its declared input latency');
      assert.equal(queued.dialogue.instructionOffset,candidate.message.waitOffset,'MessageProgram advanced before the Skip consumer boundary');
      assert.equal(queued.dialogue.freshConfirmEdge,false,'Queued Skip input created a Dialogue confirm edge');
    }
    let skipConsumed=null;
    for(let tick=0;tick<options.budgets.skipTicks;tick++){
      const value=await stepObserve(`native-Skip-consumer-${tick+1}`);evidence.skipSamples.push(structuredClone(value));
      if(value.dialogue.currentInput&options.inputMasks.skip){skipConsumed=value;break;}
      assert.equal(value.dialogue.instructionOffset,candidate.message.waitOffset,'MessageProgram advanced without a consumed Skip bit');
      assert.equal(value.dialogue.freshConfirmEdge,false,'Waiting for Skip created a Dialogue confirm edge');
    }
    if(!skipConsumed)throw new SceneIncomplete('Declared Skip input did not reach the actual Dialogue consumer within the bounded tick window');
    const skipped=skipConsumed.dialogue;
    if(!skipped.consumer.valid)throw new SceneIncomplete('Native Dialogue did not validate the Skip-consuming update');
    assert.ok(Number.isSafeInteger(skipped.currentInput),'Dialogue Skip sample lacks actual consumed input bits');
    assert.notEqual(skipped.currentInput&options.inputMasks.skip,0,'Long press or Skip key did not reach the actual Dialogue consumer');
    assert.equal(skipped.currentInput&options.inputMasks.confirm,0,'Skip gesture also produced a confirm input');
    assert.equal(skipped.freshConfirmEdge,false,'Skip gesture produced a confirm edge');
    const advanced=skipped.present===false||skipped.message!==candidate.entry||skipped.instructionOffset!==candidate.message.waitOffset;
    assert.equal(advanced,true,'Dialogue consumed Skip without advancing the real MessageProgram cursor');
    if(skipped.present)assert.equal(skipped.message,candidate.entry,'Skip moved to a different loaded dialogue entry');
    if(options.ingress==='touch'){
      const p=options.pointer,id=p.id;
      const up=await send('long-touch-Skip-up',{type:'up',id,x:p.point.x,y:p.point.y});holdEndedAt=up.startedAtMs;heldTouch=false;
      evidence.cleanup.pointerReleased=true;
      evidence.holdDurationMs=holdEndedAt-holdStartedAt;
      if(evidence.holdDurationMs<options.gesture.skipHoldMs)
        throw new SceneIncomplete('Touch pointer-up arrived before the title-declared long-press threshold');
    }else{
      await send('native-Skip-key-up',{action:'skip',down:false});heldInputs.delete('skip');
      evidence.cleanup.skipReleased=true;
    }
    let releaseObserved=false;
    for(let tick=0;tick<options.consumerLatencyTicks+2;tick++){
      const released=await stepObserve(`native-Skip-release-${tick+1}`);evidence.skipSamples.push(structuredClone(released));
      if(!released.dialogue.present||released.touch?.context!==options.expected.touchContext){releaseObserved=true;break;}
      assert.equal(released.dialogue.freshConfirmEdge,false,'Skip release created a Dialogue confirm edge');
      assert.equal(released.dialogue.currentInput&options.inputMasks.confirm,0,'Skip release produced a Dialogue confirm input');
      if(!(released.dialogue.currentInput&options.inputMasks.skip)){releaseObserved=true;break;}
    }
    if(!releaseObserved)throw new SceneIncomplete('Native Dialogue did not observe the released Skip owner within its declared consumer latency');
    result={checkStatus:'passed',reason:null,evidence};
  }catch(error){if(error instanceof SceneIncomplete)result=incomplete(error.message,evidence);else failure=error;}
  finally{
    if(heldTouch)try{const p=options.pointer;const receipt=await io.send({source:'touch',type:'up',id:p.id,x:p.point.x,y:p.point.y});
      evidence.cleanup.pointerReleased=receipt?.acknowledged===true&&receipt.type==='up'&&receipt.id===p.id;}catch(error){evidence.cleanup.pointerError=String(error?.message??error);}
    for(const actionName of heldInputs)try{const receipt=await io.send({source:options.ingress,action:actionName,down:false});
      evidence.cleanup[`${actionName}Released`]=receipt?.acknowledged===true&&receipt.action===actionName&&receipt.down===false;}catch(error){evidence.cleanup[`${actionName}Error`]=String(error?.message??error);}
    try{const receipt=await io.controls(structuredClone(options.controls.neutral));evidence.cleanup.controlsNeutral=receipt?.acknowledged===true;
      if(!evidence.cleanup.controlsNeutral)evidence.cleanup.controlsError='Runtime did not acknowledge neutral touch controls';}
    catch(error){evidence.cleanup.controlsError=String(error?.message??error);}
  }
  if(failure){failure.evidence=evidence;failure.loadedDialogueSkipEvidence=evidence;throw failure;}
  const cleanupFailed=Object.entries(evidence.cleanup).some(([key,value])=>key.endsWith('Error')||value===false);
  if(cleanupFailed){const error=new Error('Loaded dialogue Skip scene did not release every input owner');error.evidence=evidence;throw error;}
  return result;
}
