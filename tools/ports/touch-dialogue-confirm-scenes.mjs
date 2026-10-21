import assert from 'node:assert/strict';

const SCHEMA='eagler-touhou/touch-dialogue-confirm-scenes/1';
const MAX_TICKS=240;

class SceneIncomplete extends Error{}
function incomplete(reason,evidence){return{checkStatus:'incomplete',reason,evidence};}
function missingIO(io){return['readManifest','observe','requestMessage','controls','directTouchBatch','step','readActions']
  .filter(name=>typeof io?.[name]!=='function');}
function timeoutError(error){return error?.name==='TimeoutError'||error?.code==='ETIMEDOUT'||error?.timeout===true;}
function bounded(value,label,max=MAX_TICKS){
  assert.ok(Number.isSafeInteger(value)&&value>0&&value<=max,`${label} must be a positive bounded tick count`);return value;
}
function validPoint(point){return Number.isFinite(point?.x)&&point.x>=0&&point.x<=1&&Number.isFinite(point?.y)&&point.y>=0&&point.y<=1;}
function identityOf(observation,label,expected){
  if(observation?.available===false)throw new SceneIncomplete(observation.reason??`${label}: native observation is unavailable`);
  const runtime=observation?.runtime;
  for(const key of ['generation','pointer','epoch'])assert.ok(Number.isSafeInteger(runtime?.[key])&&runtime[key]>=0,
    `${label}: Runtime ${key} identity is missing`);
  if(expected){
    assert.deepEqual(runtime,expected,`${label}: Runtime instance or frame-loop epoch changed`);
  }
  assert.equal(observation.fixture?.used,true,`${label}: controlled-scene receipt is missing`);
  assert.equal(observation.fixture?.gateEnabled,true,`${label}: native validation gate is not active`);
  assert.equal(observation.fixture?.writeLocked,true,`${label}: disposable write lock is not active`);
  assert.equal(observation.fixture?.loadScope,false,`${label}: fixture write load scope is active`);
  assert.equal(observation.fixture?.committedBytes,0,`${label}: fixture committed persistent bytes`);
  return runtime;
}
function validateManifest(manifest,options){
  if(manifest?.available===false)throw new SceneIncomplete(manifest.reason??'No real ECL-referenced confirm entry is available');
  assert.equal(manifest?.schema,'eagler-touhou/dialogue-touch-manifest/1','Dialogue fixture returned an unknown manifest schema');
  assert.equal(manifest.source?.variant,options.expected.variant,'Dialogue confirm source differs from the adapter-declared Runtime variant');
  assert.equal(manifest.source?.stage,options.expected?.stage,'Manifest source is not the adapter-declared stage');
  for(const key of ['generation','pointer','epoch'])assert.ok(Number.isSafeInteger(manifest.runtime?.[key])&&manifest.runtime[key]>=0,
    `Manifest Runtime ${key} identity is missing`);
  assert.equal(manifest.fixture?.used,true,'Manifest lacks the controlled-scene receipt');
  assert.equal(manifest.fixture?.gateEnabled,true,'Manifest lacks the active native validation gate');
  assert.equal(manifest.fixture?.writeLocked,true,'Manifest lacks the disposable write lock');
  assert.equal(manifest.fixture?.loadScope,false,'Manifest reports an active write load scope');
  assert.equal(manifest.fixture?.committedBytes,0,'Manifest reports committed persistent bytes');
  for(const source of ['ecl','message']){
    const record=manifest.source?.[source];
    assert.ok(typeof record?.asset==='string'&&record.asset.length>0,`Manifest ${source} asset receipt is missing`);
    assert.ok(Number.isSafeInteger(record?.byteLength)&&record.byteLength>0,`Manifest ${source} byte length is missing`);
    assert.match(record?.fnv64??'',/^[0-9a-f]{16}$/i,`Manifest ${source} loaded-byte digest is missing`);
  }
  assert.ok(Number.isSafeInteger(manifest.source?.difficultyMask)&&manifest.source.difficultyMask>0,
    'Manifest lacks the active ECL difficulty mask');
  const candidates=manifest.candidates;
  assert.ok(Array.isArray(candidates)&&candidates.length>0,'Loaded ECL has no referenced entry with an initial confirm Wait');
  for(const candidate of candidates){
    assert.ok(Number.isSafeInteger(candidate?.entry)&&candidate.entry>=0,'Candidate entry index is invalid');
    assert.ok(Number.isSafeInteger(candidate?.ecl?.timeline)&&candidate.ecl.timeline>=0,'Candidate ECL timeline is missing');
    assert.ok(Number.isSafeInteger(candidate?.ecl?.time)&&candidate.ecl.time>=0,'Candidate ECL time is missing');
    assert.ok(Number.isSafeInteger(candidate?.ecl?.offset)&&candidate.ecl.offset>=0,'Candidate ECL source offset is missing');
    assert.ok(Number.isSafeInteger(candidate?.ecl?.referenceCount)&&candidate.ecl.referenceCount>0,'Candidate has no live ECL reference');
    assert.ok((candidate.ecl.difficultyBits&manifest.source.difficultyMask)!==0,'Candidate is not enabled by the active difficulty mask');
    assert.ok(Number.isSafeInteger(candidate?.message?.waitOffset)&&candidate.message.waitOffset>=0,'Candidate has no initial confirm Wait offset');
    assert.ok(Number.isSafeInteger(candidate?.message?.waitTime)&&candidate.message.waitTime>=0,'Candidate Wait time is missing');
    assert.ok(Number.isSafeInteger(candidate?.message?.timeoutFrames)&&candidate.message.timeoutFrames>0,'Candidate Wait timeout is missing');
    assert.ok(Number.isSafeInteger(candidate?.message?.instructionCount)&&candidate.message.instructionCount>0,'Candidate MSG instruction count is missing');
  }
  return candidates.slice().sort((a,b)=>b.message.timeoutFrames-a.message.timeoutFrames||a.entry-b.entry)[0];
}
function validateOptions(options){
  assert.equal(options?.expected?.variant,'normal','The adapter must declare the normal Runtime variant');
  assert.ok(Number.isSafeInteger(options.expected.stage)&&options.expected.stage>=0,'The adapter must declare its gameplay stage index');
  assert.ok(Number.isSafeInteger(options.expected.touchContext)&&options.expected.touchContext>=0,'Declare the active modal touch context');
  assert.ok(Number.isSafeInteger(options.consumerLatencyTicks)&&options.consumerLatencyTicks>=0&&options.consumerLatencyTicks<=8,
    'Declare the command-to-modal consumer latency');
  assert.ok(typeof options.consumerLatencySource==='string'&&options.consumerLatencySource.length>0,
    'Modal consumer latency needs its production source');
  assert.ok(Number.isSafeInteger(options.inputMasks?.fire)&&options.inputMasks.fire>0,'The adapter must declare the native gameplay Fire mask');
  assert.ok(Number.isSafeInteger(options.inputMasks?.confirm)&&options.inputMasks.confirm>0&&
    options.inputMasks.confirm<=0x80000000&&Number.isInteger(Math.log2(options.inputMasks.confirm)),
    'The adapter must declare a single native Dialogue confirm bit');
  assert.ok(Number.isSafeInteger(options.waitOpcode)&&options.waitOpcode>=0&&options.waitOpcode<=0xff,
    'The adapter must declare the native MessageProgram confirmation-Wait opcode');
  assert.ok(Number.isSafeInteger(options.pointer?.id)&&options.pointer.id>=0,'Touch confirm needs a stable pointer id');
  assert.ok(validPoint(options.pointer?.point),'Touch confirm needs a normalized canvas point');
  assert.ok(Number.isFinite(options.gesture?.maxTapDurationMs)&&options.gesture.maxTapDurationMs>0,
    'Touch tap duration needs its title-declared positive threshold');
  assert.ok(typeof options.gesture?.source==='string'&&options.gesture.source.length>0,'Touch tap timing needs its source anchor');
  bounded(options.gesture.confirmPulseTicks,'Native touch-confirm pulse');
  assert.equal(typeof options.controls?.heldFire?.fireEnabled,'boolean','A complete held-Fire touch-control snapshot is required');
  assert.equal(options.controls.heldFire.fireEnabled,true,'Held-Fire trial must begin with Fire enabled');
  assert.equal(typeof options.controls?.neutral?.fireEnabled,'boolean','A complete neutral cleanup snapshot is required');
  for(const key of ['baselineTicks','dialogueOpenTicks','waitTicks'])bounded(options.budgets?.[key],`Dialogue scene ${key}`);
  assert.ok(typeof options.budgets?.source==='string'&&options.budgets.source.length>0,'Dialogue tick budgets need an adapter source anchor');
  const total=options.budgets.baselineTicks+options.budgets.dialogueOpenTicks+options.budgets.waitTicks+
    1+options.consumerLatencyTicks+options.gesture.confirmPulseTicks+1;
  assert.ok(total<=MAX_TICKS,`Dialogue scene exceeds its ${MAX_TICKS}-tick shared bound`);
  return total;
}
function sameEntryWait(observation,candidate,waitOpcode){
  const d=observation?.dialogue;
  return d?.present===true&&d.message===candidate.entry&&d.instructionOffset===candidate.message.waitOffset&&d.opcode===waitOpcode;
}
function freshEdge(observation){return observation?.dialogue?.freshConfirmEdge===true;}
function pointer(type,id,point){return{type,id,x:point.x,y:point.y};}

/**
 * Run TOUCH-13 against a real loaded MessageProgram. The host must expose only
 * ECL-referenced entries and must open through the production TimelineActions
 * route. This helper never writes message, timer, instruction or input state.
 */
export async function runTouchDialogueFreshConfirmScene(io,options={}){
  const evidence={schema:SCHEMA,expected:options.expected??null,gesture:options.gesture??null,budgets:options.budgets??null,
    manifest:null,candidate:null,controls:[],baseline:[],request:null,dialogueOpen:[],noTapWait:[],touchBatch:null,
    confirmSamples:[],cleanup:null};
  const missing=missingIO(io);if(missing.length)return incomplete(`Dialogue confirm IO is missing ${missing.join(', ')}`,evidence);
  if(!options.expected||!options.inputMasks||!options.pointer||!options.gesture||!options.controls||!options.budgets)
    return incomplete('Dialogue confirm needs adapter identity, input masks, tap geometry, touch controls and bounded budgets.',evidence);
  const totalBudgetTicks=validateOptions(options);evidence.totalBudgetTicks=totalBudgetTicks;
  let result=null,failure=null,cleanupErrors=[],pointerMayBeDown=false,baselineFireObserved=false,runtimeIdentity=null;
  let previousSequence=null,previousDialogueSequence=null;
  const callIO=async(label,call)=>{
    try{return await call();}catch(error){
      evidence.ioErrors??=[];evidence.ioErrors.push({label,name:error?.name??null,message:String(error?.message??error)});
      if(timeoutError(error))throw new SceneIncomplete(`${label}: Runtime IO timed out before the required native observation`);
      throw error;
    }
  };
  const observe=async(label)=>{
    const value=await callIO(label,()=>io.observe());
    const identity=identityOf(value,label,runtimeIdentity);if(!runtimeIdentity)runtimeIdentity=identity;
    assert.equal(value.scene?.stage,options.expected.stage,`${label}: native stage differs from the adapter declaration`);
    assert.equal(value.scene?.variant,'normal',`${label}: native variant changed during the dialogue scene`);
    const consumer=value.dialogue?.consumer;
    if(!consumer||typeof consumer.valid!=='boolean'||!Number.isSafeInteger(consumer.sequence))
      throw new SceneIncomplete(`${label}: no actual modal-consumer receipt`);
    if(value.dialogue.present&&!consumer.valid)throw new SceneIncomplete(`${label}: active modal did not consume input`);
    if(consumer.valid){
      if(previousDialogueSequence!==null&&consumer.sequence!==previousDialogueSequence+1)
        throw new SceneIncomplete(`${label}: modal consumer sequence skipped or repeated`);
      previousDialogueSequence=consumer.sequence;
    }
    return value;
  };
  const completedAction=async(label)=>{
    const action=await callIO(label,()=>io.readActions());
    if(action?.available===false)throw new SceneIncomplete(action.reason??`${label}: native player consumer is unavailable`);
    if(action?.complete!==true)throw new SceneIncomplete(`${label}: no completed native player-consumer sample was observed`);
    assert.equal(action.phase,'consumer-run',`${label}: expected a normal gameplay consumer sample`);
    assert.ok(Number.isSafeInteger(action.sequence)&&action.sequence>0,`${label}: player-consumer sequence is missing`);
    if(previousSequence!==null)assert.equal(action.sequence,previousSequence+1,`${label}: player-consumer sequence skipped or repeated`);
    previousSequence=action.sequence;
    assert.equal(action.stage,options.expected.stage,`${label}: native player stage changed`);
    return action;
  };
  const setControls=async(label,value)=>{
    const receipt=await callIO(label,()=>io.controls(structuredClone(value)));
    evidence.controls.push({label,controls:structuredClone(value),receipt:structuredClone(receipt??null)});
    if(receipt?.acknowledged!==true)throw new SceneIncomplete(`${label}: Runtime did not acknowledge the touch-control snapshot`);
  };
  try{
    const manifest=await callIO('read-loaded-dialogue-manifest',()=>io.readManifest());evidence.manifest=structuredClone(manifest??null);
    if(manifest?.available===false)throw new SceneIncomplete(manifest.reason??'Loaded ECL and MessageProgram do not expose a usable confirm entry');
    const candidate=validateManifest(manifest,options);evidence.candidate=structuredClone(candidate);
    await setControls('enable-held-touch-Fire',options.controls.heldFire);

    for(let tick=0;tick<options.budgets.baselineTicks;tick++){
      await callIO('held-Fire-native-step',()=>io.step(1));
      const action=await completedAction('held-Fire-player-consumer');const observation=await observe('held-Fire-runtime-observation');
      const consumed=action.consumed?.current;
      assert.ok(Number.isSafeInteger(consumed),`Held-Fire sample ${tick+1} lacks the consumed native input`);
      if(consumed&options.inputMasks.fire)baselineFireObserved=true;
      evidence.baseline.push({action:structuredClone(action),observation:structuredClone(observation)});
      if(baselineFireObserved)break;
    }
    assert.equal(baselineFireObserved,true,'Touch controls did not produce a consumed native gameplay Fire sample before opening Dialogue');
    const beforeOpen=evidence.baseline.at(-1).observation;
    assert.equal(beforeOpen.dialogue?.present,false,'Dialogue was already active before the fixture request');
    assert.equal(beforeOpen.touch?.fireEnabled,true,'Touch Fire was not held at the gameplay-to-dialogue boundary');

    const request=await callIO('open-production-dialogue-entry',()=>io.requestMessage(candidate.entry));
    evidence.request={entry:candidate.entry,result:structuredClone(request??null)};
    const accepted=request===1||request?.status==='accepted'||request?.status===1;
    if(!accepted)throw new SceneIncomplete(`The production Dialogue owner rejected ECL-referenced entry ${candidate.entry}`);
    pointerMayBeDown=false;

    let waitObservation=null;
    for(let tick=0;tick<options.budgets.dialogueOpenTicks;tick++){
      await callIO('dialogue-open-native-step',()=>io.step(1));const observation=await observe('dialogue-open-observation');
      evidence.dialogueOpen.push(structuredClone(observation));
      assert.equal(observation.dialogue?.present,true,'The production Dialogue owner did not open the requested entry');
      assert.equal(observation.dialogue?.message,candidate.entry,'The production Dialogue owner opened a different MessageProgram entry');
      assert.equal(freshEdge(observation),false,'Held gameplay Fire created a fresh Dialogue confirm edge while opening');
      assert.equal(observation.touch?.fireEnabled,true,'Touch Fire configuration changed during Dialogue open');
      if(sameEntryWait(observation,candidate,options.waitOpcode)&&observation.dialogue.timer>=candidate.message.waitTime){
        waitObservation=observation;break;
      }
    }
    if(!waitObservation)throw new SceneIncomplete('The selected real MessageProgram entry did not reach its initial confirm Wait within the declared open budget');

    let waitTicks=0;
    while(waitObservation.dialogue.pausedFrames<waitObservation.dialogue.minimumWait&&waitTicks<options.budgets.waitTicks){
      const remaining=candidate.message.timeoutFrames-waitObservation.dialogue.pausedFrames;
      if(remaining<=1)throw new SceneIncomplete('The selected MessageProgram Wait can time out before its native minimum-confirm interval');
      await callIO('wait-for-native-confirm-eligibility',()=>io.step(1));waitObservation=await observe('native-confirm-eligibility-observation');
      evidence.noTapWait.push(structuredClone(waitObservation));++waitTicks;
      assert.equal(freshEdge(waitObservation),false,'A no-touch wait step produced a Dialogue confirm edge');
      if(!sameEntryWait(waitObservation,candidate,options.waitOpcode))throw new SceneIncomplete('MessageProgram advanced before touch input while waiting for confirm eligibility');
    }
    if(waitObservation.dialogue.pausedFrames<waitObservation.dialogue.minimumWait)
      throw new SceneIncomplete('Dialogue did not reach its native minimum-confirm interval within the declared wait budget');

    const noTapStart=waitObservation.dialogue;
    if(candidate.message.timeoutFrames-noTapStart.pausedFrames<=1+options.gesture.confirmPulseTicks)
      throw new SceneIncomplete('The candidate Wait lacks enough source-declared time for the no-touch control and full touch pulse');
    await callIO('prove-no-touch-does-not-advance',()=>io.step(1));const noTap=await observe('no-touch-control-observation');
    evidence.noTapWait.push(structuredClone(noTap));
    assert.equal(freshEdge(noTap),false,'Dialogue advanced from an unexpected confirmation edge without a touch gesture');
    assert.equal(sameEntryWait(noTap,candidate,options.waitOpcode),true,'Dialogue left its real confirmation Wait before any touch input');
    waitObservation=noTap;

    pointerMayBeDown=true;
    const operations=[pointer('down',options.pointer.id,options.pointer.point),pointer('up',options.pointer.id,options.pointer.point)];
    const batch=await callIO('send-real-dialogue-touch-tap',()=>io.directTouchBatch(operations));evidence.touchBatch=structuredClone(batch??null);
    if(batch?.available===false||batch?.acknowledged!==true||!Array.isArray(batch.events)||batch.events.length!==2)
      throw new SceneIncomplete('Runtime did not acknowledge both real touch events for the Dialogue tap');
    const [down,up]=batch.events;
    if(!down?.acknowledged||down.type!=='down'||down.id!==options.pointer.id||
       !up?.acknowledged||up.type!=='up'||up.id!==options.pointer.id||
       !Number.isFinite(down.startedAtMs)||!Number.isFinite(up.completedAtMs)||up.completedAtMs<down.startedAtMs)
      throw new SceneIncomplete('Dialogue tap transport receipts are missing, malformed or reordered');
    const tapDurationMs=up.completedAtMs-down.startedAtMs;evidence.tapDurationMs=tapDurationMs;
    if(tapDurationMs>=options.gesture.maxTapDurationMs)
      throw new SceneIncomplete('Touch transport exceeded the adapter-declared Dialogue tap duration');
    pointerMayBeDown=false;

    evidence.queuedConfirmSamples=[];
    for(let tick=0;tick<options.consumerLatencyTicks;tick++){
      await callIO(`modal-confirm-input-queued-${tick+1}`,()=>io.step(1));
      const observation=await observe(`modal-confirm-before-consumer-${tick+1}`);
      evidence.queuedConfirmSamples.push(structuredClone(observation));
      assert.equal(freshEdge(observation),false,'Touch confirm arrived before its declared modal consumer boundary');
      assert.equal(sameEntryWait(observation,candidate,options.waitOpcode),true,'Dialogue left its Wait before consuming the touch edge');
    }
    let ownerRetired=false;
    for(let tick=0;tick<options.gesture.confirmPulseTicks+1;tick++){
      await callIO(`dialogue-confirm-pulse-${tick+1}-step`,()=>io.step(1));
      const observation=await observe(`dialogue-confirm-pulse-${tick+1}-observation`);
      evidence.confirmSamples.push(structuredClone(observation));
      const dialogue=observation.dialogue;
      if(!dialogue||typeof dialogue.present!=='boolean'||!Number.isSafeInteger(dialogue.currentInput)||!Number.isSafeInteger(dialogue.previousInput))
        throw new SceneIncomplete('Native Dialogue consumer input or modal state was not observed after the tap');
      const edge=freshEdge(observation),currentHasConfirm=Boolean(dialogue.currentInput&options.inputMasks.confirm);
      if(tick===0){
        assert.equal(dialogue.consumer.valid,true,'The confirmation must be read from the actual modal consumer');
        assert.equal(currentHasConfirm,true,'Real touch tap did not reach the native Dialogue confirm input');
        assert.equal(edge,true,'Real touch tap did not create one fresh native Dialogue confirm edge');
        const advanced=!dialogue.present||dialogue.message!==waitObservation.dialogue.message||dialogue.instructionOffset!==waitObservation.dialogue.instructionOffset;
        assert.equal(advanced,true,'Native Dialogue owner consumed the touch edge without advancing its MessageProgram cursor');
      }else if(ownerRetired||!dialogue.present||observation.touch.context!==options.expected.touchContext){
        // Gameplay Fire can resume after the modal retires. It is not another
        // Dialogue consumer; a later modal must not inherit a fresh old edge.
        if(dialogue.present)assert.equal(edge,false,'An expired confirmation pulse reached a later modal owner');
      }else if(tick<options.gesture.confirmPulseTicks){
        assert.equal(currentHasConfirm,true,`Native touch confirm pulse ended before its declared ${options.gesture.confirmPulseTicks} ticks`);
        assert.equal(edge,false,'A held touch-confirm pulse produced more than one native press edge');
      }else{
        assert.equal(currentHasConfirm,false,'Native touch confirm pulse remained held after its declared tick budget');
        assert.equal(edge,false,'Touch-confirm release created an extra native press edge');
      }
      if(!dialogue.present||observation.touch.context!==options.expected.touchContext){
        ownerRetired=true;evidence.ownerRetiredAtTick??=tick+1;
      }
    }
    result={checkStatus:'passed',reason:null,evidence};
  }catch(error){
    if(error instanceof SceneIncomplete)result=incomplete(error.message,evidence);else failure=error;
  }finally{
    try{
      if(pointerMayBeDown){
        const point=options.pointer?.point,id=options.pointer?.id;
        if(validPoint(point)&&Number.isSafeInteger(id)){
          const receipt=await callIO('release-dialogue-touch-pointer',()=>io.directTouchBatch([pointer('up',id,point)]));
          if(receipt?.acknowledged!==true||receipt.events?.length!==1||receipt.events[0]?.type!=='up'||receipt.events[0]?.id!==id)
            cleanupErrors.push(new Error('Runtime did not acknowledge Dialogue pointer release'));
        }else cleanupErrors.push(new Error('Dialogue pointer cleanup metadata is invalid'));
      }
    }catch(error){cleanupErrors.push(error);}
    try{
      if(options.controls?.neutral){const receipt=await callIO('neutralize-touch-Fire',()=>io.controls(structuredClone(options.controls.neutral)));
        if(receipt?.acknowledged!==true)cleanupErrors.push(new Error('Runtime did not acknowledge neutral touch controls'));
        evidence.cleanup={controlsNeutral:receipt?.acknowledged===true};
      }
    }catch(error){cleanupErrors.push(error);}
    evidence.cleanup={...(evidence.cleanup??{}),errors:cleanupErrors.map(error=>error.message)};
  }
  if(failure){failure.evidence=evidence;failure.touchDialogueConfirmEvidence=evidence;throw failure;}
  if(cleanupErrors.length){const error=new Error(`Dialogue confirm scene ${result?.checkStatus??'has no verdict'}; cleanup failed: ${cleanupErrors.map(item=>item.message).join('; ')}`);
    error.evidence=evidence;error.touchDialogueConfirmEvidence=evidence;throw error;}
  return result;
}
