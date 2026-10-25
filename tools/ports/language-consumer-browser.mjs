import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {executeObservationFlowSteps} from '../../tests/adaptation/adapters/flow-steps.mjs';
import {normalizeLanguageConsumerState} from './language-consumer-state.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {languageObservationRequest,readLanguageObservation} from '../../tests/adaptation/adapters/language.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld,assertControlledSceneStimulus} from '../../tests/adaptation/adapters/scene.mjs';
import {dialogueTouchSceneRequest,readDialogueTouchWords,normalizeDialogueTouchManifest,normalizeDialogueTouchObservation,requestDialogueTouchMessage} from '../../tests/adaptation/adapters/dialogue.mjs';
import {touchActionsRequest,readTouchActionState} from '../../tests/adaptation/adapters/actions.mjs';
import {runLoadedDialogueSkipScene} from './loaded-dialogue-skip-scenes.mjs';
import {assessLanguageScene,assessMessageTextRasters} from './language-contract.mjs';
import {resolveLanguageTextSource} from './language-text-sources.mjs';
import {pressObservedMenuKey} from './native-menu-key.mjs';

/** A title's existing native scene and navigation interfaces share one live language instance. */
export async function runLanguageConsumers({page,frame,target,adapter,expected,language,original,sources,output,label,save}){
  const profile=adapter.language,statistics=profile.consumers.statistics,e={schema:'eagler-touhou/language-consumers/1',navigation:[],cleanupErrors:[]};
  const identity=()=>frame.evaluate(runtime=>({url:location.href,timeOrigin:performance.timeOrigin,app:globalThis[runtime.global][runtime.app]}),adapter.runtime);
  const ui=async()=>{const raw=await frame.evaluate(readNativeSnapshotRaw,{runtime:adapter.runtime,snapshot:profile.uiSnapshot}),state=normalizeLanguageConsumerState(raw,profile);
    assert.ok(e.navigation.length<512,'Language navigation observation overflow');e.navigation.push({raw,state});return state;};
  const wait=async(name,predicate,timeoutMs=20000,read=ui)=>{const end=Date.now()+timeoutMs;let state;do{state=await read();if(predicate(state))return state;await page.waitForTimeout(50);}while(Date.now()<end);throw Error(name+': '+JSON.stringify(state));};
  e.keyInputs=[];
  const press=async code=>{await frame.locator('canvas').focus();
    try{const receipt=await pressObservedMenuKey({now:Date.now,read:ui,down:key=>page.keyboard.down(key),up:key=>page.keyboard.up(key),wait:ms=>page.waitForTimeout(ms)},
        {code,mask:profile.navigation.masks[code],inputField:profile.navigation.inputField,transitionFields:profile.navigation.transitionFields});
      e.keyInputs.push(receipt);await wait('native menu key release',state=>(state[profile.navigation.inputField]&receipt.mask)===0);
    }catch(error){e.keyFailure=error.evidence;throw error;}};
  const trace=async()=>(await frame.evaluate(readLanguageObservation,languageObservationRequest(target))).trace;
  const image=async name=>{const path=resolve(output,`${label}-${name}.png`);await frame.locator('canvas').screenshot({path});const bytes=await readFile(path);
    return{path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),identity:await identity()};};
  let manual=false,failure;
  try{
    e.identity=await identity();
    await executeObservationFlowSteps({flow:statistics.enter,observe:ui,press,waitUntil:wait});
    const baseline=Object.fromEntries(statistics.samples.map(sample=>[sample.key,sample.originalHex]));
    e.statistics={state:await ui(),trace:await trace()};
    e.statistics.assessment=assessLanguageScene({trace:e.statistics.trace,profile:{samples:statistics.samples},expected,baseline,original});
    assert.equal(e.statistics.assessment.status,'observed',e.statistics.assessment.reason);
    e.statistics.image=await image('statistics');await save(e);
    await executeObservationFlowSteps({flow:statistics.exit,observe:ui,press,waitUntil:wait});
    const sceneRequest=validationSceneRequest(target),flow=resolveTitleFlow(target),observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
    e.arm=assertControlledSceneArmed(await frame.evaluate(controlValidationScene,validationSceneRequest({...target,action:'arm'})));
    await flow.enterGameplay({observe,press,lane:'desktop',waitUntil:(name,predicate)=>wait(name,predicate,20000,observe)});
    e.clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));assert.equal(e.clock.mode,'native-manual');manual=true;
    let previous=assertControlledSceneWorld(await frame.evaluate(controlValidationScene,sceneRequest));e.controlled={initial:previous,receipts:[]};
    const step=async count=>{for(let i=0;i<count;i++){
      await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));
      const next=await frame.evaluate(controlValidationScene,sceneRequest);assertControlledSceneStimulus(next,previous,{kind:'dialogue'});previous=next;e.controlled.receipts.push(next);
    }};
    e.settle=[];let ready=false;
    for(let i=0;i<profile.consumers.dialogue.maxReadyTicks;i++){
      const state=await frame.evaluate(readTouchActionState,touchActionsRequest(target));e.settle.push(state);
      if(state.complete&&state.lifeState===adapter.deathbomb.lifeStates.alive&&state.consumed.current===0){ready=true;break;}
      await step(1);
    }
    assert.equal(ready,true,'Dialogue text scene did not reach its ordinary neutral player consumer');
    const {contract}=dialogueTouchSceneRequest(target),neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
    const controls=async value=>{assert.equal(value.focusEnabled,false);assert.equal(value.joystickX,0);assert.equal(value.joystickY,0);
      const button=page.locator('#touchFire');assert.equal(await button.isVisible(),true);
      if((await button.getAttribute('aria-pressed')==='true')!==value.fireEnabled)await button.click();
      return{acknowledged:true,source:'Launcher #touchFire',fireEnabled:await button.getAttribute('aria-pressed')==='true'};};
    const nativeObserve=async()=>{const raw=await frame.evaluate(readDialogueTouchWords,{contract,operation:'observation'}),state=normalizeDialogueTouchObservation(raw,contract);
      if(!e.dialogueCapture&&state.available&&state.dialogue.present&&state.dialogue.opcode===contract.waitOpcode&&
        state.dialogue.pausedFrames>=profile.consumers.dialogue.captureWaitTicks){
        const visible=await ui();
        if(Math.max(...profile.consumers.dialogue.textAlphaFields.map(field=>visible[field]))>=profile.consumers.dialogue.minimumTextAlpha){
          e.dialogueCapture={raw,state,ui:visible,trace:await trace(),identity:await identity(),image:await image('dialogue')};await save(e);
        }
      }return state;};
    const result=await runLoadedDialogueSkipScene({
      controls,readManifest:async()=>{
        const raw=await frame.evaluate(readDialogueTouchWords,{contract,operation:'manifest'}),manifest=normalizeDialogueTouchManifest(raw,contract),observation=await nativeObserve();
        e.dialogueManifestRaw=raw;assert.equal(manifest.available,true,manifest.reason);
        return{...manifest,fixture:observation.fixture,fixtureSource:contract.observation.source};
      },observe:nativeObserve,readActions:()=>frame.evaluate(readTouchActionState,touchActionsRequest(target)),
      requestMessage:async entry=>{e.beforeMessageTraceOrdinal=(await trace()).totalObservations;return frame.evaluate(requestDialogueTouchMessage,{contract,entry});},
      step,
      send:async operation=>{assert.equal(operation.action,'skip');await frame.locator('canvas').focus();await page.keyboard[operation.down?'down':'up']('ControlLeft');
        return{acknowledged:true,source:'keyboard',action:operation.action,down:operation.down};},
    },{ingress:'keyboard',expected:{variant:target.variant,stage:contract.stage,touchContext:contract.touchContext,gameplayTouchContext:contract.gameplayTouchContext},
      waitOpcode:contract.waitOpcode,consumerLatencyTicks:contract.consumerLatencyTicks,inputMasks:{fire:adapter.actions.inputMasks.fire,confirm:contract.observation.confirmMask,skip:contract.skipMask},
      gesture:contract.skipGesture,controls:{neutral,heldFire:{...neutral,fireEnabled:true}},sources:{manifest:contract.manifest.source,consumer:contract.sources.confirmConsumer},
      budgets:{baselineTicks:8,openTicks:90,waitTicks:60,minimumWaitTicks:profile.consumers.dialogue.captureWaitTicks,skipTicks:8,source:contract.sources.confirmConsumer}});
    e.dialogue=result;assert.equal(result.checkStatus,'passed',result.reason);assert.ok(e.dialogueCapture,'Real dialogue produced no captured native Wait');
    const manifest=result.evidence.manifest,resolveSource=asset=>resolveLanguageTextSource(sources,{language,original,asset}),
      source=resolveSource(manifest.source.message.asset),eclSource=resolveSource(manifest.source.ecl.asset);
    for(const [actual,input]of [[manifest.source.message,source],[manifest.source.ecl,eclSource]]){
      assert.ok(input);assert.equal(actual.byteLength,input.bytes);assert.equal(actual.fnv64,input.fnv64);
    }
    e.dialogueSource={asset:manifest.source.message.asset,input:source,expected:adapter.parseDialogueText(await readFile(source.path),result.evidence.candidate.entry)};
    e.dialogueAssessment=assessMessageTextRasters({trace:e.dialogueCapture.trace,expected:e.dialogueSource.expected,scene:profile.consumers.dialogue.gameScene,
      after:e.beforeMessageTraceOrdinal});
    assert.equal(e.dialogueAssessment.status,'observed',e.dialogueAssessment.reason);assert.deepEqual(await identity(),e.identity);await save(e);return e;
  }catch(error){failure=error;e.failure=String(error.stack||error);throw error;}
  finally{
    for(const code of ['KeyZ','KeyX','ArrowUp','ArrowDown','ControlLeft'])try{await page.keyboard.up(code);}catch(error){e.cleanupErrors.push(String(error));}
    try{const fire=page.locator('#touchFire');if(await fire.isVisible()&&await fire.getAttribute('aria-pressed')==='true')await fire.click();}catch(error){e.cleanupErrors.push(String(error));}
    if(manual)try{e.clockEnd=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}));}catch(error){e.cleanupErrors.push(String(error));}
    await save(e);if(e.cleanupErrors.length&&!failure)throw Error('Language consumer cleanup failed');
  }
}
