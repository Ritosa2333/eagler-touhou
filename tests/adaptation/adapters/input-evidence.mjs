import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {assertControlledScene,assertControlledSceneStimulus,assertControlledSceneRequest} from './scene.mjs';
import {assertMenuStartupEvidence} from './menu-startup.mjs';
import {normalizeDialogueTouchManifest,normalizeDialogueTouchObservation} from './dialogue.mjs';

const INPUT_CHECKS=Object.freeze({
  'shared-original-menu-dialogue-input':{adaptationId:'INPUT-07',supportingIds:['INPUT-01','INPUT-04'],kind:'menu-dialogue-input'},
  'shared-touch-menu-dialogue-gestures':{adaptationId:'TOUCH-12',kind:'touch-menu-dialogue'},
  'shared-touch-dialogue-confirm':{adaptationId:'TOUCH-13',kind:'touch-fresh-confirm'},
  'shared-launcher-gamepad-mapping':{adaptationId:'INPUT-03',kind:'launcher-gamepad-mapping'},
  'shared-launcher-touch-layout':{adaptationId:'TOUCH-17',kind:'launcher-touch-layout'},
  'shared-launcher-touch-magnifier':{adaptationId:'TOUCH-19',kind:'launcher-touch-magnifier'},
});
const MENU_SCHEMA='eagler-touhou/original-menu-input-scenes/1';
const SKIP_SCHEMA='eagler-touhou/loaded-dialogue-skip-scenes/1';
const CONFIRM_SCHEMA='eagler-touhou/touch-dialogue-confirm-scenes/1';
const GAMEPAD_SCHEMA='eagler-touhou/launcher-gamepad-scenes/1';
const LAYOUT_SCHEMA='eagler-touhou/launcher-touch-layout-scenes/1';
const MAGNIFIER_SCHEMA='eagler-touhou/launcher-touch-magnifier-scenes/1';

function demand(condition,message){assert.ok(condition,message);}
function int(value){return Number.isSafeInteger(value);}
function same(a,b,message){assert.deepEqual(a,b,message);}
function equal(a,b,message){assert.equal(a,b,message);}
function validRuntime(value,identity,label){
  demand(int(value?.generation)&&value.generation>0&&int(value?.pointer)&&value.pointer>0&&
    int(value?.epoch)&&value.epoch>=0,`${label}: actual Runtime identity is incomplete`);
  same(value,identity,`${label}: native observation belongs to another Runtime instance or loop epoch`);
}
function assertUniqueTarget(driver,entry,product,adaptationId){
  equal(driver?.schema,'eagler-touhou/runtime-desktop-probes/1','Evidence is not a Runtime desktop report');
  demand(Array.isArray(driver.targets)&&driver.targets.length===1,'Input check needs one selected Runtime target');
  const target=driver.targets[0];
  equal(target.game,product.game,'Runtime target belongs to another product');
  equal(target.variant,product.variant,'Runtime target belongs to another product variant');
  demand(Array.isArray(target.checks)&&target.checks.filter(item=>item?.id===entry.id).length===1&&
    target.checks.includes(entry),'Input claim is not the unique check in the selected target');
  const allowed=new Set([adaptationId,...(INPUT_CHECKS[entry.id]?.supportingIds??[])]);
  demand(Array.isArray(entry.adaptationIds)&&entry.adaptationIds.includes(adaptationId)&&
    new Set(entry.adaptationIds).size===entry.adaptationIds.length&&entry.adaptationIds.every(id=>allowed.has(id)),
    `Input check must identify ${adaptationId} with declared supporting IDs`);
}
function runtimeContract(product,capability){
  const adapter=resolveAdaptationAdapter(product.game,product.variant);
  demand(adapter,`No registered input adapter for ${product.game}/${product.variant}`);
  demand(adapter[capability],`No registered ${capability} contract for ${product.game}/${product.variant}`);
  return adapter;
}
function assertTrialBoundary(trial,ingress,product,label,kind){
  equal(trial?.checkStatus,'passed',`${label}: scene did not pass`);
  equal(trial.ingress,ingress,`${label}: input ingress differs from the requested trial`);
  if(kind!==undefined)equal(trial.kind,kind,`${label}: scene kind is mismatched`);
  demand(int(trial.epoch)&&trial.epoch>0,`${label}: selected Runtime epoch is missing`);
  equal(trial.transition?.epoch,trial.epoch,`${label}: transition belongs to another Runtime epoch`);
  equal(trial.clock?.mode,'native-manual',`${label}: native consumer was not observed under the selected manual clock`);
  if(trial.transition?.state?.game!==undefined)equal(trial.transition.state.game,product.game,`${label}: transition state belongs to another game`);
  const binding=trial.frameBinding;
  demand(binding&&typeof binding.url==='string',`${label}: actual frame-to-native binding is missing`);
  let url;try{url=new URL(binding.url);}catch{throw new Error(`${label}: actual Runtime frame URL is invalid`);}
  equal(Number(url.searchParams.get('runtimeEpoch')),trial.epoch,`${label}: frame URL belongs to another Host epoch`);
  equal(binding.hostEpoch,trial.epoch,`${label}: frame binding belongs to another Host epoch`);
  validRuntime(binding.nativeRuntime,binding.nativeRuntime,label);
  same(trial.frameBindingAfter,binding,`${label}: frame-to-native binding changed during the trial`);
}
function assertDialogueControlledWorld(trial,adapter,label){
  if(!adapter.controlledScene?.controller)return;
  const scene=trial.controlledScene;
  demand(scene?.acceptance&&Array.isArray(scene.timeline)&&scene.timeline.length>0,`${label}: controlled dialogue raw boundaries are missing`);
  let previous=scene.acceptance;assertControlledScene(previous);let requests=0;
  for(const event of scene.timeline){
    if(event.kind==='request'){
      requests++;same(event.effects,adapter.controlledScene.requestEffects.dialogue,`${label}: request effect differs from native title contract`);
      same(event.before,previous,`${label}: request rebaselined the controlled native state`);
      assertControlledSceneRequest(event.after,event.before,{kind:'dialogue'});equal(event.result?.accepted,true,`${label}: native request was not accepted`);previous=event.after;
    }else{
      equal(event.kind,'tick',`${label}: undeclared controlled native boundary`);
      assertControlledSceneStimulus(event.receipt,previous,{kind:'dialogue'});previous=event.receipt;
    }
    equal(previous.game,adapter.game,`${label}: controller belongs to another title`);
    same(previous.runtime,trial.frameBinding.nativeRuntime,`${label}: controller belongs to another native Runtime`);
  }
  equal(requests,1,`${label}: expected exactly one native Dialogue request`);
}
function assertTitleObservation(row,epoch,label){
  demand(typeof row?.label==='string'&&row.label.length>0,`${label}: title observation label is absent`);
  const value=row?.value;
  equal(value?.available,true,`${label}: native title observer is unavailable`);
  equal(value.scene,'title',`${label}: Runtime did not remain in the title scene`);
  validRuntime(value.runtime,epoch,label);
  const title=value.title;
  for(const field of ['screen','cursor','phase','stateTimer2'])demand(int(title?.[field])&&title[field]>=0,`${label}: native title ${field} is missing`);
  demand(typeof title.confirmationReady==='boolean'&&int(title.input?.current)&&int(title.input?.previous),
    `${label}: native title input consumer fields are missing`);
  return title;
}
function eventByLabel(evidence,label){
  const rows=evidence.events.filter(row=>row?.label===label);
  equal(rows.length,1,`${label}: expected one raw input event`);
  return rows[0];
}
function assertMappedEvent(evidence,label,ingress,action,down){
  const row=eventByLabel(evidence,label);
  same(row.operation,{action,down},`${label}: raw operation differs from the declared native action`);
  equal(row.receipt?.acknowledged,true,`${label}: transport did not acknowledge the actual input`);
  equal(row.receipt?.source,ingress,`${label}: transport belongs to another input source`);
  equal(row.receipt?.action,action,`${label}: transport delivered another action`);
  equal(row.receipt?.down,down,`${label}: transport delivered another input edge`);
}
function assertTouchEvent(evidence,label,type,id){
  const row=eventByLabel(evidence,label),receipt=row.receipt;
  equal(row.operation?.type,type,`${label}: pointer operation type is mismatched`);
  equal(row.operation?.id,id,`${label}: pointer operation identity is mismatched`);
  equal(receipt?.acknowledged,true,`${label}: browser touch transport did not acknowledge the event`);
  equal(receipt?.source,'touch',`${label}: event did not use the Runtime touch path`);
  equal(receipt?.type,type,`${label}: touch receipt type is mismatched`);
  equal(receipt?.id,id,`${label}: touch receipt identity is mismatched`);
  demand(Number.isFinite(receipt.startedAtMs)&&Number.isFinite(receipt.completedAtMs)&&
    receipt.completedAtMs>=receipt.startedAtMs,`${label}: touch receipt lacks ordered transport time`);
}
function assertNativeEdge(title,mask,label){
  demand((title.input.current&mask)!==0&& (title.input.previous&mask)===0,
    `${label}: no fresh matching edge reached the native title consumer`);
}
function assertMenuTrial(trial,ingress,adapter,product){
  const label=`${product.game}/${product.variant} original menu ${ingress}`;
  assertTrialBoundary(trial,ingress,product,label);
  const evidence=trial.evidence,contract=adapter.originalMenu;
  equal(evidence?.schema,MENU_SCHEMA,`${label}: menu scene evidence schema is absent`);
  equal(evidence.ingress,ingress,`${label}: evidence ingress is mismatched`);
  same(evidence.expected,contract.expected,`${label}: evidence uses another title menu expectation`);
  const masks=ingress==='touch'?{...contract.inputMasks,cancel:contract.touchCancelMask}:contract.inputMasks;
  same(evidence.inputMasks,masks,`${label}: input masks differ from the registered title owner`);
  same(evidence.sources,contract.sources,`${label}: native title source anchors differ from the adapter`);
  same(evidence.geometry,contract.geometry,`${label}: menu geometry differs from the registered title contract`);
  if(contract.startup){
    assertMenuStartupEvidence(evidence.startup,contract.startup,contract.expected,trial.frameBinding.nativeRuntime,contract.inputMasks.confirm);
    if(evidence.startup.observations.some(row=>row.label==='startup-confirm-consumer')){
      if(ingress==='touch'){
        assertTouchEvent(evidence,'startup-confirm-down','down',contract.geometry.pointerId);
        assertTouchEvent(evidence,'startup-confirm-up','up',contract.geometry.pointerId);
      }else{
        assertMappedEvent(evidence,'startup-confirm-down',ingress,'confirm',true);
        assertMappedEvent(evidence,'startup-confirm-up',ingress,'confirm',false);
      }
    }
  }else equal(evidence.startup,undefined,`${label}: startup navigation was not declared by the title`);
  demand(Array.isArray(evidence.observations)&&evidence.observations.length>=5,`${label}: native title observations are incomplete`);
  demand(int(evidence.totalBudgetTicks)&&evidence.totalBudgetTicks>0&&evidence.totalBudgetTicks<=256,
    `${label}: bounded native title interaction evidence is absent`);
  equal(evidence.cleanup?.released,true,`${label}: title input owner was not released`);
  const observed=new Map();
  for(const row of evidence.observations){
    const title=assertTitleObservation(row,trial.frameBinding.nativeRuntime,label);
    demand(!observed.has(row.label),`${label}: duplicate native observation ${row.label}`);
    observed.set(row.label,title);
  }
  const start=observed.get('title-menu-ready'),startConsumer=observed.get('select-start-menu-entry-consumer');
  const directionConsumer=observed.get(ingress==='touch'?'single-menu-swipe-held-consumer':'single-menu-down-consumer');
  const cancelConsumer=observed.get('return-from-difficulty-menu-consumer');
  demand(start&&startConsumer&&directionConsumer&&cancelConsumer,`${label}: title consumer transitions are missing`);
  equal(start.screen,contract.expected.startScreen,`${label}: trial did not begin on the original title menu`);
  equal(start.cursor,contract.expected.startCursor,`${label}: title trial began from another cursor`);
  assertNativeEdge(startConsumer,contract.inputMasks.confirm,`${label} confirm`);
  if(contract.expected.confirmTransitionPhase!==undefined&&startConsumer.screen===contract.expected.startScreen){
    equal(startConsumer.phase,contract.expected.confirmTransitionPhase,`${label}: confirm used an undeclared transition phase`);
    const waits=evidence.observations.filter(row=>row.label.startsWith('wait-real-difficulty-menu'));
    demand(waits.length>0&&waits.length<=contract.budgets.transitionTicks,`${label}: missing bounded native difficulty transition`);
    for(const row of waits)if(row.value.title.screen!==contract.expected.difficultyScreen){
      equal(row.value.title.screen,contract.expected.startScreen,`${label}: transition crossed an unrelated menu`);
      equal(row.value.title.phase,contract.expected.confirmTransitionPhase,`${label}: transition changed its native phase`);
    }
    equal(waits.at(-1).value.title.screen,contract.expected.difficultyScreen,`${label}: transition never reached native difficulty`);
  }else equal(startConsumer.screen,contract.expected.difficultyScreen,`${label}: confirm did not enter the real difficulty menu`);
  const cursorBefore=evidence.difficultyCursorBefore,cursorAfter=evidence.difficultyCursorAfter;
  demand(int(cursorBefore)&&int(cursorAfter)&&cursorBefore!==cursorAfter,`${label}: native menu cursor did not move`);
  assertNativeEdge(directionConsumer,contract.inputMasks.down,`${label} down`);
  equal(directionConsumer.screen,contract.expected.difficultyScreen,`${label}: direction left the difficulty menu`);
  equal(directionConsumer.cursor,cursorAfter,`${label}: cursor receipt does not match the native observation`);
  if(ingress==='touch'){
    const holdTicks=contract.geometry?.swipeHoldTicks;
    demand(int(holdTicks)&&holdTicks>1,`${label}: title owner did not declare its held-swipe consumer window`);
    equal(evidence.geometry?.swipeHoldTicks,holdTicks,`${label}: evidence does not retain the registered held-swipe window`);
    demand(evidence.totalBudgetTicks>=holdTicks,`${label}: total interaction budget is shorter than the held-swipe observation window`);
    for(let tick=1;tick<holdTicks;tick++){
      const stable=observed.get(`single-menu-swipe-held-stability-${tick}`);
      demand(stable,`${label}: held swipe stability observation ${tick}/${holdTicks-1} is missing`);
      equal(stable.screen,contract.expected.difficultyScreen,`${label}: held swipe left the real difficulty menu`);
      equal(stable.cursor,cursorAfter,`${label}: held swipe repeated a native cursor movement`);
    }
  }
  if(ingress!=='touch'){
    const up=observed.get('single-menu-up-consumer');
    demand(up,`${label}: reverse menu navigation was not observed`);
    assertNativeEdge(up,contract.inputMasks.up,`${label} up`);
    equal(up.cursor,cursorBefore,`${label}: up did not restore the starting difficulty selection`);
    for(const action of ['confirm','down','up','cancel']){
      assertMappedEvent(evidence,`${action==='confirm'?'select-start-menu-entry':action==='cancel'?'return-from-difficulty-menu':`single-menu-${action}`}-down`,ingress,action,true);
      assertMappedEvent(evidence,`${action==='confirm'?'select-start-menu-entry':action==='cancel'?'return-from-difficulty-menu':`single-menu-${action}`}-up`,ingress,action,false);
    }
  }else{
    const g=contract.geometry;
    for(const [name,type,id] of [
      ['select-start-menu-entry-down','down',g.pointerId],['select-start-menu-entry-up','up',g.pointerId],
      ['single-menu-swipe-down','down',g.pointerId],['single-menu-swipe-move','move',g.pointerId],['single-menu-swipe-up','up',g.pointerId],
      ['return-from-difficulty-menu-first-down','down',g.pointerId],['return-from-difficulty-menu-second-down','down',g.cancelPointerId],
      ['return-from-difficulty-menu-first-up','up',g.pointerId],['return-from-difficulty-menu-second-up','up',g.cancelPointerId],
    ])assertTouchEvent(evidence,name,type,id);
  }
  assertNativeEdge(cancelConsumer,masks.cancel,`${label} cancel`);
  const returned=[...observed.values()].at(-1);
  equal(returned.screen,contract.expected.startScreen,`${label}: cancel did not return to the title start menu`);
  equal(returned.cursor,contract.expected.startCursor,`${label}: returned title cursor is not restored`);
}

function runtimeObservation(value,epoch,contract,label){
  demand(value&&typeof value==='object',`${label}: raw Runtime observation is missing`);
  const decoded=normalizeDialogueTouchObservation({available:true,words:value.rawWords},contract);
  for(const [key,expected] of Object.entries(decoded))same(value[key],expected,`${label}: ${key} differs from the native Dialogue words`);
  validRuntime(value.runtime,epoch,label);
  if(value.scene){
    equal(value.scene.variant,contract.variant,`${label}: native scene variant differs from the selected product`);
    equal(value.scene.stage,contract.stage,`${label}: native scene stage differs from the registered dialogue owner`);
  }
  if(value.fixture){
    equal(value.fixture.used,true,`${label}: controlled scene receipt is absent`);
    equal(value.fixture.gateEnabled,true,`${label}: native fixture gate is inactive`);
    equal(value.fixture.writeLocked,true,`${label}: disposable write lock is absent`);
    equal(value.fixture.loadScope,false,`${label}: an active persistent load scope was observed`);
    equal(value.fixture.committedBytes,0,`${label}: scene wrote persistent bytes`);
  }
  return value;
}
function validateDialogueManifest(evidence,trial,adapter,label){
  const contract=adapter.dialogueTouchScene,manifest=evidence.manifest;
  equal(manifest?.schema,'eagler-touhou/dialogue-touch-manifest/1',`${label}: loaded dialogue manifest schema is absent`);
  const decoded=normalizeDialogueTouchManifest({available:true,words:manifest.rawWords},contract);
  for(const [key,expected] of Object.entries(decoded))same(manifest[key],expected,`${label}: ${key} differs from the loaded native source words`);
  equal(manifest.available,true,`${label}: no actual loaded dialogue source was observed`);
  equal(manifest.source?.variant,trial.transition?.state?.variant??contract.variant,`${label}: manifest belongs to another product variant`);
  equal(manifest.source?.stage,contract.stage,`${label}: manifest belongs to another stage owner`);
  validRuntime(manifest.runtime,trial.frameBinding.nativeRuntime,`${label} manifest`);
  equal(manifest.fixture?.used,true,`${label}: manifest has no controlled-scene receipt`);
  equal(manifest.fixture?.gateEnabled,true,`${label}: manifest has no native fixture gate`);
  equal(manifest.fixture?.writeLocked,true,`${label}: manifest is not write locked`);
  equal(manifest.fixture?.loadScope,false,`${label}: manifest reports an active load scope`);
  equal(manifest.fixture?.committedBytes,0,`${label}: manifest reports persistent writes`);
  for(const key of ['ecl','message']){
    demand(typeof manifest.source?.[key]?.asset==='string'&&manifest.source[key].asset.length>0&&
      int(manifest.source[key].byteLength)&&manifest.source[key].byteLength>0&&/^[0-9a-f]{16}$/iu.test(manifest.source[key].fnv64??''),
      `${label}: loaded ${key} byte identity is incomplete`);
  }
  const candidate=evidence.candidate;
  demand(int(candidate?.entry)&&int(candidate?.ecl?.timeline)&&int(candidate?.ecl?.time)&&int(candidate?.ecl?.referenceCount)&&
    candidate.ecl.referenceCount>0&&int(candidate?.message?.waitOffset)&&int(candidate?.message?.timeoutFrames)&&
    candidate.message.timeoutFrames>0&&int(candidate?.message?.waitTime),`${label}: selected ECL/MessageProgram candidate is incomplete`);
  demand(manifest.candidates?.some(row=>row.entry===candidate.entry&&row.ecl?.timeline===candidate.ecl.timeline&&
    row.ecl?.time===candidate.ecl.time&&row.ecl?.referenceCount===candidate.ecl.referenceCount&&
    row.message?.waitOffset===candidate.message.waitOffset&&row.message?.timeoutFrames===candidate.message.timeoutFrames),
  `${label}: selected wait is not a live entry from the loaded ECL/MSG manifest`);
  return candidate;
}
function dialogueRows(evidence){
  const values=[];
  for(const row of evidence.baseline??[])if(row?.observation)values.push(row.observation);
  for(const name of ['open','dialogueOpen','wait','noTapWait','queuedConfirmSamples','confirmSamples','skipSamples'])
    if(Array.isArray(evidence[name]))values.push(...evidence[name]);
  return values;
}
function validateDialogueSequences(evidence,trial,adapter,label){
  const contract=adapter.dialogueTouchScene,rows=dialogueRows(evidence);
  demand(rows.length>0,`${label}: no native Dialogue observations were retained`);
  const seen=new Map();let previous=null;
  for(const [index,value] of rows.entries()){
    runtimeObservation(value,trial.frameBinding.nativeRuntime,contract,`${label} observation ${index+1}`);
    demand(value.dialogue&&typeof value.dialogue.present==='boolean'&&typeof value.dialogue.consumer?.valid==='boolean'&&
      int(value.dialogue.consumer.sequence),`${label}: actual Dialogue consumer receipt is missing`);
    if(!value.dialogue.present&&!value.dialogue.consumer.valid)continue;
    demand(value.dialogue.consumer.valid&&value.dialogue.consumer.sequence>0,
      `${label}: active Dialogue consumer sequence is missing`);
    const seq=value.dialogue.consumer.sequence;
    const prior=seen.get(seq);
    if(prior){same(value.dialogue,prior,`${label}: a repeated consumer sequence changed its raw Dialogue state`);continue;}
    if(previous!==null)equal(seq,previous+1,`${label}: native Dialogue consumer sequence skipped or repeated`);
    seen.set(seq,value.dialogue);previous=seq;
  }
  return rows;
}
function assertFireBaseline(evidence,adapter,label){
  demand(Array.isArray(evidence.baseline)&&evidence.baseline.length>0,`${label}: held Fire baseline is missing`);
  const mask=adapter.actions.inputMasks.fire;
  demand(evidence.baseline.some(row=>row?.action?.complete===true&&row.action.phase==='consumer-run'&&
    int(row.action.sequence)&&row.action.consumed?.current!==undefined&&(row.action.consumed.current&mask)!==0&&
    row.observation?.dialogue?.present===false&&row.observation?.touch?.fireEnabled===true),
  `${label}: held Fire did not reach the native gameplay consumer before Dialogue`);
}
function acceptedRequest(evidence,candidate,label){
  equal(evidence.request?.entry,candidate.entry,`${label}: the production Dialogue request used another loaded message entry`);
  const request=evidence.request?.result;
  demand(request===1||request?.accepted===true||request?.status==='accepted'||request?.status===1,
    `${label}: production TimelineActions did not accept the loaded message entry`);
}
function assertSkipTrial(trial,ingress,adapter,product){
  assertDialogueControlledWorld(trial,adapter,`${product.game} Dialogue ${ingress}`);
  const label=`${product.game}/${product.variant} dialogue Skip ${ingress}`;
  assertTrialBoundary(trial,ingress,product,label,'hold-skip');
  const evidence=trial.evidence;
  equal(evidence?.schema,SKIP_SCHEMA,`${label}: loaded-dialogue Skip evidence schema is absent`);
  equal(evidence.ingress,ingress,`${label}: loaded-dialogue ingress is mismatched`);
  const candidate=validateDialogueManifest(evidence,trial,adapter,label);
  same(evidence.expected,{variant:adapter.dialogueTouchScene.variant,stage:adapter.dialogueTouchScene.stage,
    touchContext:adapter.dialogueTouchScene.touchContext,gameplayTouchContext:adapter.dialogueTouchScene.gameplayTouchContext},
  `${label}: dialogue context differs from the registered owner`);
  assertFireBaseline(evidence,adapter,label);acceptedRequest(evidence,candidate,label);
  const rows=validateDialogueSequences(evidence,trial,adapter,label);
  demand(evidence.open?.some(row=>row.dialogue?.present===true&&row.dialogue.message===candidate.entry&&
    row.dialogue.instructionOffset===candidate.message.waitOffset&&row.dialogue.opcode===adapter.dialogueTouchScene.waitOpcode),
  `${label}: real MessageProgram entry did not reach its native Wait`);
  demand(evidence.wait?.some(row=>row.dialogue?.present===true&&row.dialogue.message===candidate.entry&&
    row.dialogue.instructionOffset===candidate.message.waitOffset&&row.dialogue.pausedFrames>=row.dialogue.minimumWait),
  `${label}: selected Wait did not reach its actual minimum-input interval`);
  const skipMask=adapter.dialogueTouchScene.skipMask,confirmMask=adapter.dialogueTouchScene.observation.confirmMask;
  const consumed=evidence.skipSamples?.find(row=>row.dialogue?.consumer?.valid===true&&
    (row.dialogue.currentInput&skipMask)!==0);
  demand(consumed,`${label}: Skip never reached the actual Dialogue consumer`);
  equal(consumed.dialogue.currentInput&confirmMask,0,`${label}: Skip also produced a confirmation input`);
  equal(consumed.dialogue.freshConfirmEdge,false,`${label}: Skip produced a fresh confirm edge`);
  demand(!consumed.dialogue.present||consumed.dialogue.message!==candidate.entry||
    consumed.dialogue.instructionOffset!==candidate.message.waitOffset,
  `${label}: native consumer did not advance the loaded MessageProgram cursor`);
  demand(evidence.skipSamples.some(row=>!row.dialogue?.present||!(row.dialogue.currentInput&skipMask)),
    `${label}: native Skip release was not observed`);
  if(ingress==='touch'){
    demand(Number.isFinite(evidence.gesture?.skipHoldMs)&&evidence.gesture.skipHoldMs>0&&
      Number.isFinite(evidence.holdDurationMs)&&evidence.holdDurationMs>=evidence.gesture.skipHoldMs,
    `${label}: physical touch hold did not meet the registered Skip gesture interval`);
    const down=evidence.inputEvents?.find(row=>row.label==='long-touch-Skip-down'),up=evidence.inputEvents?.find(row=>row.label==='long-touch-Skip-up');
    demand(down&&up&&down.operation?.id===up.operation?.id&&down.receipt?.type==='down'&&up.receipt?.type==='up'&&
      down.receipt?.id===up.receipt?.id&&down.receipt?.source==='touch'&&up.receipt?.source==='touch'&&
      Number.isFinite(down.receipt.completedAtMs)&&Number.isFinite(up.receipt.startedAtMs)&&up.receipt.startedAtMs>=down.receipt.completedAtMs,
    `${label}: long touch does not have a matching physical pointer down/up receipt`);
    equal(evidence.cleanup?.pointerReleased,true,`${label}: touch Skip pointer was not released`);
  }else{
    assertMappedEvent({events:evidence.inputEvents},'native-Skip-key-down',ingress,'skip',true);
    assertMappedEvent({events:evidence.inputEvents},'native-Skip-key-up',ingress,'skip',false);
    equal(evidence.cleanup?.skipReleased,true,`${label}: mapped Skip key was not released`);
  }
  equal(evidence.cleanup?.controlsNeutral,true,`${label}: touch controls were not neutralized`);
  demand(!evidence.cleanup?.controlsError&&!evidence.cleanup?.skipError&&!evidence.cleanup?.pointerError,
    `${label}: input cleanup reported an error`);
  demand(rows.length>0,`${label}: Dialogue evidence window is empty`);
}
function assertShortConfirmTrial(trial,adapter,product){
  assertDialogueControlledWorld(trial,adapter,`${product.game} Dialogue touch confirm`);
  const label=`${product.game}/${product.variant} touch short confirm`;
  assertTrialBoundary(trial,'touch',product,label,'short-confirm');
  const evidence=trial.evidence;
  equal(evidence?.schema,CONFIRM_SCHEMA,`${label}: touch Dialogue confirm evidence schema is absent`);
  const contract=adapter.dialogueTouchScene,candidate=validateDialogueManifest(evidence,trial,adapter,label);
  same(evidence.expected,{variant:contract.variant,stage:contract.stage,touchContext:contract.touchContext,
    gameplayTouchContext:contract.gameplayTouchContext},`${label}: scene context differs from registered Dialogue owner`);
  assertFireBaseline(evidence,adapter,label);acceptedRequest(evidence,candidate,label);
  const rows=validateDialogueSequences(evidence,trial,adapter,label);
  demand(evidence.dialogueOpen?.some(row=>row.dialogue?.present===true&&row.dialogue.message===candidate.entry&&
    row.dialogue.instructionOffset===candidate.message.waitOffset&&row.dialogue.opcode===contract.waitOpcode),
  `${label}: real MessageProgram did not reach its confirmation Wait`);
  demand(evidence.noTapWait?.some(row=>row.dialogue?.present===true&&row.dialogue.message===candidate.entry&&
    row.dialogue.instructionOffset===candidate.message.waitOffset&&row.dialogue.freshConfirmEdge===false),
  `${label}: no-touch control did not remain at the same Wait`);
  const batch=evidence.touchBatch,events=batch?.events;
  equal(batch?.acknowledged,true,`${label}: Runtime did not acknowledge the real touch tap`);
  demand(Array.isArray(events)&&events.length===2,`${label}: touch tap does not contain exactly one down/up pair`);
  const [down,up]=events;
  demand(down?.acknowledged===true&&down.type==='down'&&up?.acknowledged===true&&up.type==='up'&&
    down.id===up.id&&Number.isFinite(down.startedAtMs)&&Number.isFinite(up.completedAtMs)&&up.completedAtMs>=down.startedAtMs,
  `${label}: trusted transport did not retain the matching touch pointer lifecycle`);
  demand(Number.isFinite(evidence.tapDurationMs)&&evidence.tapDurationMs>=0&&
    evidence.tapDurationMs<contract.gesture.maxTapDurationMs,
  `${label}: touch tap duration exceeds the registered confirmation gesture`);
  const confirms=evidence.confirmSamples??[];
  demand(confirms.length>=contract.gesture.confirmPulseTicks,`${label}: native confirm pulse samples are incomplete`);
  const edges=confirms.filter(row=>row.dialogue?.freshConfirmEdge===true);
  equal(edges.length,1,`${label}: touch tap did not produce exactly one native confirmation edge`);
  const edge=edges[0].dialogue;
  equal(edge.consumer?.valid,true,`${label}: confirmation is not from the actual Dialogue consumer`);
  demand((edge.currentInput&contract.observation.confirmMask)!==0&&
    (edge.previousInput&contract.observation.confirmMask)===0,`${label}: native confirm edge bits are missing`);
  demand(!edge.present||edge.message!==candidate.entry||edge.instructionOffset!==candidate.message.waitOffset,
    `${label}: actual confirm edge did not advance the loaded MessageProgram cursor`);
  equal(evidence.cleanup?.controlsNeutral,true,`${label}: touch controls were not neutralized`);
  demand(Array.isArray(evidence.cleanup?.errors)&&evidence.cleanup.errors.length===0,
    `${label}: touch confirm cleanup reported an error`);
  demand(rows.length>0,`${label}: no native Runtime observation was retained`);
}
function assertRestore(trial,product,previousEpoch){
  equal(trial?.checkStatus,'passed','Gameplay restoration trial did not pass');
  equal(trial?.kind,'restore-gameplay','Final trial is not the Gameplay restoration step');
  demand(int(trial.epoch)&&trial.epoch>previousEpoch,'Gameplay restoration did not launch a new Runtime epoch');
  equal(trial.transition?.epoch,trial.epoch,'Gameplay restoration transition belongs to another epoch');
  const state=trial.transition?.state;
  demand(state&&typeof state==='object','Gameplay restoration omitted its native Runtime state');
  const readiness=resolveTitleFlow({game:product.game,variant:product.variant}).gameplayReadiness(state,'desktop');
  equal(readiness.ready,true,`Gameplay was not restored in the new epoch: ${readiness.reasons.join(', ')}`);
}
function assertInputGroup(entry,product,touch){
  const adapter=runtimeContract(product,'originalMenu');
  demand(adapter.dialogueTouchScene&&adapter.actions,`No dialogue/action owner is registered for ${product.game}/${product.variant}`);
  const trials=entry.observation?.trials;
  const required=touch?3:4;
  demand(Array.isArray(trials)&&trials.length===required+1,`${entry.id}: required ingress trials and final restore are incomplete`);
  const trialEpochs=trials.map((trial,index)=>{
    demand(int(trial?.epoch)&&trial.epoch>0,`${entry.id}: trial ${index+1} has no selected Runtime epoch`);return trial.epoch;
  });
  for(let index=1;index<trialEpochs.length;index++)demand(trialEpochs[index]>trialEpochs[index-1],`${entry.id}: Runtime epochs are not strictly ordered`);
  if(touch){
    assertMenuTrial(trials[0],'touch',adapter,product);
    assertShortConfirmTrial(trials[1],adapter,product);
    assertSkipTrial(trials[2],'touch',adapter,product);
  }else{
    assertMenuTrial(trials[0],'keyboard',adapter,product);
    assertMenuTrial(trials[1],'gamepad',adapter,product);
    assertSkipTrial(trials[2],'keyboard',adapter,product);
    assertSkipTrial(trials[3],'gamepad',adapter,product);
  }
  assertRestore(trials.at(-1),product,trialEpochs.at(-2));
}

function assertLauncherBoundary(observation,entry,product,schema,label){
  equal(entry.status,'passed',`${label}: Runtime check status is not passed`);
  equal(observation?.target?.game,product.game,`${label}: Runtime observation belongs to another product`);
  equal(observation?.target?.variant,product.variant,`${label}: Runtime observation belongs to another variant`);
  demand(int(observation.target.epoch)&&observation.target.epoch>0,`${label}: actual Runtime epoch is missing`);
  equal(observation.transition?.epoch,observation.target.epoch,`${label}: transition belongs to another Runtime epoch`);
  equal(observation.clock?.mode,'native-manual',`${label}: native consumer clock is not identified`);
  equal(observation?.schema,schema,`${label}: production scene evidence schema is absent`);
  equal(observation.game,product.game,`${label}: scene evidence belongs to another product`);
  equal(observation.variant,product.variant,`${label}: scene evidence belongs to another variant`);
}
function assertContractEcho(observation,adapter,label){
  same(observation.contractEvidence?.inputMasks,adapter.actions.inputMasks,`${label}: action masks differ from the product adapter`);
  same(observation.contractEvidence?.movement,adapter.movement,`${label}: movement owner differs from the product adapter`);
  // The native controller is a title-owned prerequisite, never a summary-only
  // exemption from the zero-hazard world. Re-evaluate each captured tick.
  if(adapter.controlledScene?.controller){
    const scene=observation.controlledScene;
    demand(scene?.acceptance&&Array.isArray(scene.receipts)&&scene.receipts.length>0,
      `${label}: native controlled-world receipts are missing`);
    let previous=null;
    for(const receipt of [scene.acceptance,...scene.receipts]){
      equal(receipt.game,adapter.game,`${label}: controlled world belongs to another game`);
      equal(receipt.variant,adapter.variant,`${label}: controlled world belongs to another variant`);
      same(receipt.controllerPolicy,adapter.controlledScene.controller,`${label}: controller policy differs from the registered title`);
      same(receipt.writePolicy,adapter.controlledScene.writePolicy,`${label}: write overlay policy differs from the registered title`);
      assertControlledScene(receipt,previous);previous=receipt;
    }
  }
}
function sampleDevices(sample,first,second,label){
  demand(Array.isArray(sample?.devices)&&sample.devices.length>=2,`${label}: journal did not identify both connected devices`);
  const rows=sample.devices.filter(row=>[first.key,second.key].includes(row?.key));
  equal(rows.length,2,`${label}: selected/non-selected device identities are missing from the production journal`);
  demand(rows.every(row=>int(row.index)),`${label}: gamepad API index is absent from the mapper journal`);
  const selected=rows.find(row=>row.key===sample.selectedKey);
  equal(selected?.index,sample.selectedIndex,`${label}: selected device index differs from the live mapper journal`);
}
function assertInactiveControllerStimulus(e,tick,name,selectedKey,otherKey,label){
  const stimulus=e.stimuli?.find(row=>row.label===`${name}:nonselected-active`);
  demand(stimulus,`${label}: raw non-selected controller stimulus ${name} is missing`);
  equal(stimulus.key,otherKey,`${label}: negative stimulus came from the selected controller`);
  equal(stimulus.active,true,`${label}: non-selected controller was not activated`);
  demand(stimulus.control?.kind==='button'||stimulus.control?.kind==='axis',`${label}: negative stimulus control kind is absent`);
  demand(int(stimulus.control?.index)&&stimulus.control.index>=0,`${label}: negative stimulus control index is absent`);
  equal(stimulus.receipt?.key,otherKey,`${label}: fixture did not apply the control to the named other device`);
  equal(stimulus.receipt?.index,stimulus.control.index,`${label}: fixture changed the negative control index`);
  if(stimulus.control.kind==='button')equal(stimulus.receipt?.pressed,true,`${label}: other-device button was not actually pressed`);
  else equal(stimulus.receipt?.value,stimulus.value,`${label}: other-device axis value was not actually applied`);
  demand(stimulus.control.kind==='button'?stimulus.value===true:Number.isFinite(stimulus.value)&&stimulus.value!==0,
    `${label}: non-selected control value is not an active input`);
  equal(stimulus.nativeConsumerSequence,tick.after.sequence,
    `${label}: non-selected stimulus is not associated with the same native consumer sample`);
  demand(tick.samples.some(sample=>sample.selectedKey===selectedKey&&sample.bits===0),
    `${label}: selected controller was not neutral in the same native sample window`);
  equal(tick.after.consumed.fire,false,`${label}: non-selected controller reached the native fire consumer`);
}
function assertGamepadEvidence(entry,product){
  const adapter=runtimeContract(product,'actions');demand(adapter.movement,`No native movement contract is registered for ${product.game}/${product.variant}`);
  const observation=entry.observation,label=`${product.game}/${product.variant} Launcher gamepad mapping`;
  assertLauncherBoundary(observation,entry,product,GAMEPAD_SCHEMA,label);assertContractEcho(observation,adapter,label);
  const e=observation;
  demand(int(e.mappingSeat)&&e.mappingSeat>=0&&int(e.nativeSeat)&&e.nativeSeat>=0&&e.mappingSeat===e.nativeSeat,
    `${label}: Launcher mapping seat and registered native seat do not match`);
  const ids=e.identities;
  demand(Array.isArray(ids)&&ids.length===2&&ids.every(row=>typeof row.key==='string'&&row.key.length>0&&int(row.index)),
    `${label}: two connected gamepad identities were not retained`);
  demand(ids[0].key!==ids[1].key&&ids[0].index!==ids[1].index,`${label}: gamepad identities or live indexes are not distinct`);
  const fixtureDevices=observation.deviceFixture?.devices;
  demand(Array.isArray(fixtureDevices)&&fixtureDevices.length>=2&&ids.every(id=>fixtureDevices.some(row=>row.key===id.key)),
    `${label}: connected-device fixture receipt does not identify both scene controllers`);
  equal(e.reloaded,true,`${label}: selected controller mapping was not reloaded from storage`);
  const ticks=e.ticks;
  demand(Array.isArray(ticks)&&ticks.length>=8,`${label}: selected/non-selected mapper journal is incomplete`);
  for(let i=0;i<ticks.length;i++){
    const tick=ticks[i];
    demand(typeof tick.label==='string'&&int(tick.before?.sequence)&&int(tick.after?.sequence)&&
      tick.after.sequence===tick.before.sequence+1,`${label}: mapper sample is not paired with one actual native consumer sequence`);
    demand(typeof tick.after.consumed?.fire==='boolean',`${label}: native fire consumer state is absent`);
    demand(Array.isArray(tick.samples)&&tick.samples.length>0,`${label}: actual production mapper sample is absent`);
    for(const sample of tick.samples){
      equal(sample.seat,e.mappingSeat,`${label}: mapper sample belongs to another seat`);
      demand(typeof sample.selectedKey==='string'&&int(sample.selectedIndex)&&int(sample.bits),`${label}: mapper sample fields are incomplete`);
      sampleDevices(sample,ids[0],ids[1],`${label} ${tick.label}`);
    }
  }
  const byLabel=new Map(ticks.map(tick=>[tick.label,tick]));
  for(const name of ['nonselected-device-button','nonselected-device-axis','first-device-unselected-after-switch','first-device-unselected-axis-after-switch'])
    demand(byLabel.has(name),`${label}: missing non-selected controller negative sample ${name}`);
  for(const name of ['nonselected-device-button','nonselected-device-axis','first-device-unselected-after-switch','first-device-unselected-axis-after-switch']){
    const tick=byLabel.get(name);
    const expectedSelected=name.startsWith('first-device-unselected')?ids[1].key:ids[0].key;
    const other=ids.find(id=>id.key!==expectedSelected);
    assertInactiveControllerStimulus(e,tick,name,expectedSelected,other.key,label);
  }
  const operations=e.operations;
  demand(Array.isArray(operations),`${label}: native action and reorder operations are absent`);
  const op=(kind,name)=>operations.find(row=>row.kind===kind&&row.label===name);
  for(const [kind,name] of [['button','first-device-shoot'],['axis','first-device-right-axis'],
    ['button','second-device-shoot'],['axis','second-device-right-axis'],
    ['button','reordered-device-shoot'],['button','reloaded-device-shoot']])
    demand(op(kind,name),`${label}: missing selected-device operation ${kind}/${name}`);
  demand(operations.some(row=>row.kind==='index-reorder'),`${label}: selected device index reorder was not recorded`);
  for(const operation of operations.filter(row=>row.kind==='button')){
    const source=operation.sample;
    demand(source&&ids.some(id=>id.key===operation.key),`${label}: selected button is not tied to a connected device`);
    equal(source.selectedKey,operation.key,`${label}: selected button journal belongs to another device`);
    demand(int(source.seat)&&source.seat===e.mappingSeat&&source.bits>0,`${label}: selected button mapper sample is absent`);
    const held=ticks.find(row=>row.after.sequence===operation.held),released=ticks.find(row=>row.after.sequence===operation.released);
    demand(held&&released,`${label}: selected button edges have no native consumer samples`);
    equal(held.after.consumed.fire,true,`${label}: selected button did not reach native fire`);
    equal(released.after.consumed.fire,false,`${label}: selected button release did not clear native fire`);
  }
  for(const operation of operations.filter(row=>row.kind==='axis')){
    demand(ids.some(id=>id.key===operation.key)&&operation.sample?.selectedKey===operation.key&&
      operation.sample?.seat===e.mappingSeat&&operation.sample?.bits>0&&
      Number.isFinite(operation.before?.x)&&Number.isFinite(operation.after?.x)&&operation.after.x>operation.before.x,
    `${label}: selected axis sample did not move the actual native player`);
  }
  for(const name of ['nonselected-device-axis','first-device-unselected-axis-after-switch']){
    const operation=operations.find(row=>row.kind==='unselected-axis'&&row.label===name);
    demand(operation,`${label}: non-selected axis did not retain its native position comparison`);
    same(operation.after,operation.before,`${label}: non-selected axis moved the native player`);
    const tick=byLabel.get(name);
    demand(operation.sequence===tick.after.sequence,`${label}: non-selected axis position comparison belongs to another native tick`);
  }
  const reorder=operations.find(row=>row.kind==='index-reorder');
  demand(reorder.before!==reorder.after&&reorder.selectedKey===e.persisted?.selectedKey&&e.persisted?.index===reorder.after,
    `${label}: selected persistent identity did not survive the gamepad index reorder and reload`);
  return true;
}
function assertLayoutEvidence(entry,product){
  const adapter=runtimeContract(product,'actions'),observation=entry.observation,label=`${product.game}/${product.variant} Launcher touch layout`;
  assertLauncherBoundary(observation,entry,product,LAYOUT_SCHEMA,label);assertContractEcho(observation,adapter,label);
  const orientations=observation.orientations;
  demand(Array.isArray(orientations)&&orientations.length===2,`${label}: independent portrait and landscape storage records are missing`);
  const byName=new Map(orientations.map(row=>[row.orientation,row]));
  demand(byName.has('landscape')&&byName.has('portrait'),`${label}: both layout orientations were not tested`);
  const values={};
  for(const orientation of ['landscape','portrait']){
    const row=byName.get(orientation),stored=row?.stored,applied=row?.applied;
    demand(stored&&typeof stored.key==='string'&&stored.key.length>0&&stored.value?.profiles?.[orientation]?.controls?.[observation.control],
      `${label}: ${orientation} profile was not independently persisted`);
    const placement=stored.value.profiles[orientation].controls[observation.control];
    demand(Number.isFinite(placement.x)&&Number.isFinite(placement.y)&&Number.isFinite(placement.scale),
      `${label}: ${orientation} persisted placement is incomplete`);
    demand(applied?.available===true&&applied.target==='player'&&applied.orientation===orientation&&
      applied.storageKey===stored.key,`${label}: ${orientation} profile was not applied to the selected Player DOM`);
    same(applied.placement,placement,`${label}: ${orientation} DOM uses another stored placement`);
    demand(applied.safeRect?.width>0&&applied.safeRect?.height>0&&applied.controlRect?.width>0&&
      applied.controlRect?.height>0&&Number.isFinite(applied.renderedScale),`${label}: ${orientation} actual DOM geometry is incomplete`);
    const actualX=(applied.controlRect.left+applied.controlRect.width/2-applied.safeRect.left)/applied.safeRect.width;
    const actualY=(applied.controlRect.top+applied.controlRect.height/2-applied.safeRect.top)/applied.safeRect.height;
    const epsilon=1/Math.min(applied.safeRect.width,applied.safeRect.height);
    demand(Math.abs(actualX-placement.x)<=epsilon&&Math.abs(actualY-placement.y)<=epsilon&&
      Math.abs(applied.renderedScale-placement.scale)<=1e-6,`${label}: ${orientation} applied DOM position or scale differs from its stored profile`);
    values[orientation]=placement;
  }
  equal(byName.get('landscape').stored.key,byName.get('portrait').stored.key,`${label}: orientations came from different storage owners`);
  demand(values.landscape.x!==values.portrait.x||values.landscape.y!==values.portrait.y||values.landscape.scale!==values.portrait.scale,
    `${label}: portrait and landscape profiles are not independent`);
  const later=byName.get('portrait').stored.value.profiles.landscape?.controls?.[observation.control];
  same(later,values.landscape,`${label}: saving portrait overwrote the stored landscape profile`);
  return true;
}
function assertTrustedTouchEvents(events,types,label){
  demand(Array.isArray(events),`${label}: browser touch event journal is missing`);
  for(const type of types){
    const rows=events.filter(row=>row?.type===type&&row.pointerType==='touch');
    demand(rows.length>0,`${label}: trusted ${type} event is missing`);
    demand(rows.every(row=>row.isTrusted===true&&int(row.pointerId)),`${label}: touch event was untrusted or lost its pointer identity`);
  }
}
function assertMagnifierEvidence(entry,product){
  const adapter=runtimeContract(product,'actions'),observation=entry.observation,label=`${product.game}/${product.variant} touch magnifier`;
  assertLauncherBoundary(observation,entry,product,MAGNIFIER_SCHEMA,label);assertContractEcho(observation,adapter,label);
  const e=observation,gestures=e.gestures;
  demand(Array.isArray(gestures)&&gestures.length===2,`${label}: before/after zoom pointer trials are incomplete`);
  const plain=gestures.find(row=>row.label==='magnifier-off'),zoomed=gestures.find(row=>row.label==='magnifier-on');
  demand(plain&&zoomed,`${label}: native target samples do not straddle magnifier activation`);
  for(const gesture of [plain,zoomed]){
    demand(gesture.rect?.width>0&&gesture.rect?.height>0&&Number.isFinite(gesture.rect.left)&&Number.isFinite(gesture.rect.top),
      `${label}: actual Runtime screen rectangle is missing`);
    const native=gesture.native;
    demand(native?.before?.source==='native-touch-trace-v2'&&native?.armed?.source===native.before.source&&
      native?.moved?.source===native.before.source&&native.before.raw&&native.armed.raw&&native.moved.raw&&
      int(native.before.sequence)&&native.armed?.sequence===native.before.sequence+1&&native.moved?.sequence===native.armed.sequence+1,
    `${label}: actual native touch consumer sequence or registered trace identity is missing`);
    demand(Number.isFinite(native.before.target?.x)&&Number.isFinite(native.before.target?.y)&&
      Number.isFinite(native.armed.target?.x)&&Number.isFinite(native.armed.target?.y)&&
      Number.isFinite(native.moved.target?.x)&&Number.isFinite(native.moved.target?.y),
    `${label}: actual native target positions are incomplete`);
    const dx=native.moved.target.x-native.armed.target.x,dy=native.moved.target.y-native.armed.target.y;
    demand((dx!==0||dy!==0)&&gesture.delta?.x===dx&&gesture.delta?.y===dy,`${label}: trusted movement did not change the native target`);
    assertTrustedTouchEvents(gesture.events,['pointerdown','pointermove','pointerup'],`${label} ${gesture.label}`);
    for(const row of gesture.events.filter(item=>item?.pointerType==='touch'&&['pointerdown','pointermove','pointerup'].includes(item.type)))
      demand(row.surfaceOwner==='player'&&row.surfaceTarget==='touchDirectSurface',
        `${label}: trusted Pointer did not target the direct-touch surface inside the launched Player`);
    const move=gesture.events.find(row=>row.type==='pointermove');
    equal(move?.clientX,gesture.screen?.move?.clientX,`${label}: screen x does not match the delivered browser Pointer`);
    equal(move?.clientY,gesture.screen?.move?.clientY,`${label}: screen y does not match the delivered browser Pointer`);
  }
  const comparison=e.comparison,zoom=comparison?.pinch?.zoom;
  equal(zoom?.active,true,`${label}: production magnifier did not activate`);
  equal(zoom?.target,'player',`${label}: zoom transform is not attached to the selected Player`);
  demand(Number.isFinite(zoom?.scale)&&zoom.scale>1,`${label}: pinch did not increase the actual Player scale`);
  assertTrustedTouchEvents(comparison?.pinch?.events,['pointerdown','pointermove','pointerup'],`${label} two-finger pinch`);
  for(const row of (comparison?.pinch?.events??[]).filter(item=>item?.pointerType==='touch'&&['pointerdown','pointermove','pointerup'].includes(item.type)))
    demand(row.surfaceOwner==='player'&&row.surfaceTarget==='touchDirectSurface',
      `${label}: trusted pinch Pointer did not target the direct-touch surface inside the launched Player`);
  for(const type of ['pointerdown','pointermove','pointerup']){
    const rows=comparison.pinch.events.filter(row=>row.type===type&&row.pointerType==='touch');
    demand(new Set(rows.map(row=>row.pointerId)).size>=2,`${label}: pinch did not retain two distinct touch identities`);
  }
  demand(plain.screen.move.clientX!==zoomed.screen.move.clientX||plain.screen.move.clientY!==zoomed.screen.move.clientY,
    `${label}: zoom did not change the absolute screen coordinates for the same normalized input`);
  equal(Math.fround(plain.delta.x),Math.fround(zoomed.delta.x),`${label}: same normalized input changed native x movement under zoom`);
  equal(Math.fround(plain.delta.y),Math.fround(zoomed.delta.y),`${label}: same normalized input changed native y movement under zoom`);
  same(comparison.plainDelta,plain.delta,`${label}: plain native movement summary differs from its raw trace`);
  same(comparison.magnifiedDelta,zoomed.delta,`${label}: magnified native movement summary differs from its raw trace`);
  demand(e.browserEvidence?.trustedCdpTouch===true,`${label}: browser evidence does not identify its trusted CDP input path`);
  return true;
}

/** Recompute complete Runtime input claims from their raw native and browser evidence. */
export function assessRuntimeInputEvidence({driver,entry,product}={}){
  const definition=INPUT_CHECKS[entry?.id];
  if(!definition)return{available:false,valid:false,reason:`No Runtime input assessor for ${entry?.id??'unknown check'}`,kind:null};
  const result={available:true,valid:false,reason:null,kind:definition.kind,checks:[definition.adaptationId]};
  try{
    demand(product&&typeof product.game==='string'&&['normal','multiplayer'].includes(product.variant),
      'Selected product identity is missing');
    equal(entry?.id,Object.keys(INPUT_CHECKS).find(id=>INPUT_CHECKS[id]===definition),'Input check identity is mismatched');
    assertUniqueTarget(driver,entry,product,definition.adaptationId);
    equal(entry.status,'passed','Runtime input check status is not passed');
    if(entry.id==='shared-original-menu-dialogue-input')assertInputGroup(entry,product,false);
    else if(entry.id==='shared-touch-menu-dialogue-gestures')assertInputGroup(entry,product,true);
    else if(entry.id==='shared-touch-dialogue-confirm'){
      const adapter=runtimeContract(product,'dialogueTouchScene'),trials=entry.observation?.trials;
      demand(Array.isArray(trials)&&trials.length===2,'Fresh confirm requires its native trial and final gameplay restoration');
      assertShortConfirmTrial(trials[0],adapter,product);
      assertRestore(trials[1],product,trials[0].epoch);
    }
    else if(entry.id==='shared-launcher-gamepad-mapping')assertGamepadEvidence(entry,product);
    else if(entry.id==='shared-launcher-touch-layout')assertLayoutEvidence(entry,product);
    else assertMagnifierEvidence(entry,product);
    result.valid=true;
  }catch(error){result.reason=String(error?.message??error);}
  return result;
}
