import assert from 'node:assert/strict';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {decodePracticeUiSnapshot} from '../../tests/adaptation/adapters/practice-ui.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld,assertControlledSceneOwner} from '../../tests/adaptation/adapters/scene.mjs';
import {requestDeathbombHit,deathbombRequest} from '../../tests/adaptation/adapters/deathbomb.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';

export function assertPracticeLifecycleOption(e,options,kind){
  const c=options.lifecycle,fields=c.fields,consumer=options.consumerFields;
  assert.equal(e.schema,'eagler-touhou/practice-lifecycle-option/1');assert.equal(e.kind,kind);assert.deepEqual(e.cleanupErrors,[]);assertControlledSceneArmed(e.arm);
  const before=e.before,w=before.raw.words,identity=before.identity;assert.ok(e.samples.length>2&&e.samples.length<=700);
  assertControlledSceneWorld(before.scene);
  for(const row of [before,...e.samples]){decodeNativeControls(row.raw);assert.deepEqual(row.identity,identity);
    const ui=decodePracticeUiSnapshot(row.uiRaw);for(const vk of [c.retryVirtualKey,c.pauseVirtualKey])assert.equal(ui.keyboard.down[vk>>>5]&(1<<(vk&31)),0,'Lifecycle was assisted by a physical menu key');
    assert.equal(row.scene.writeReceipt.locked,true);assert.equal(row.scene.writeReceipt.committedBytes,0);
    if(row.scene.active){assertControlledSceneOwner(row.scene);assert.equal(row.scene.enemies,before.scene.enemies);assert.equal(row.scene.lasers,0);assert.ok(row.scene.bullets<=1);}
    else assert.equal(kind,'fast-retry','Only the observed restart may retire the disposable world');
  }
  if(kind==='one-key-death'){
    const disabled=e.disabledTitle;assert.equal(disabled.before.raw.words[consumer.inGame],0);
    for(const row of [disabled.before,disabled.after,e.activated.before,e.activated.after])assert.deepEqual(row.state,decodeNativeControls(row.raw));
    for(const key of ['life','lives','bombs'])assert.equal(disabled.after.raw.words[fields[key]],disabled.before.raw.words[fields[key]]);
    const item=disabled.before.state.controls.find(item=>item.semantic===c.deathControl),afterItem=disabled.after.state.controls.find(item=>item.semantic===c.deathControl);
    assert.equal(item.enabled,false);assert.equal(afterItem.enabled,false);assert.equal(afterItem.activations,item.activations);
    assert.equal(disabled.kind,'disabled-click');assert.equal(disabled.semantic,c.deathControl);
    assert.equal(e.activated.before.state.controls.find(item=>item.semantic===c.deathControl).enabled,true);
    assert.equal(e.activated.after.state.controls.find(item=>item.semantic===c.deathControl).activations,
      e.activated.before.state.controls.find(item=>item.semantic===c.deathControl).activations+1);
    assert.deepEqual(disabled.before.identity,identity);assert.ok(w[fields.lives]>0&&w[fields.bombs]>0);
    const action=e.activated.after.raw.words;assert.equal(action[fields.life],c.deathState);assert.equal(action[fields.lives],0);assert.equal(action[fields.bombs],0);
    assert.equal(e.samples.at(-1).raw.words[fields.gameOver],1,'One-key death never reached the native game-over consumer');
  }else{
    assert.equal(kind,'fast-retry');assert.equal(w[fields.fastRetry],1);assert.equal(e.hit.accepted,true);
    assert.ok(e.samples.some(row=>row.raw.words[fields.retryCountdown]>0),'The actual hit never armed retry');
    assert.ok(e.samples.some(row=>row.runtime.pause?.active===true),'Automatic retry did not enter native pause');
    const last=e.samples.at(-1),current=last.raw.words;
    assert.ok(current[consumer.frame]<w[consumer.frame],'Automatic retry did not rebuild the actual world');
    assert.equal(last.runtime.worldProgress.comparable,true);assert.equal(last.runtime.pause.active,false);
    assert.equal(last.scene.active,false,'The old disposable world must retire with the restart');
    for(const key of ['lives','bombs'])assert.equal(current[fields[key]],w[fields[key]]);
  }
  return{complete:true,kind,source:'Native UI action, guarded collision, lifecycle and resource consumers'};
}

/** A negative title click shares the same document/app as the later live click. */
export async function preparePracticeDeathTitle({page,frame,target,adapter,save}){
  const options=adapter.practice.options,c=options.lifecycle,clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action}));
  const advance=()=>clock('tick'),ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:options,advance});
  const input=createPracticeUiInput({page,frame,adapter,advance});let manual=false;
  try{assert.equal((await clock('begin')).mode,'native-manual');manual=true;await input.press(c.menuKey);await ui.until('Title Advanced menu not open',s=>s.open);
    const result=await ui.clickDisabled(c.deathControl);await input.press(c.menuKey);await ui.until('Title Advanced menu not closed',s=>!s.open);await save(result);return result;
  }finally{if(manual)await clock('end');}
}

export async function runPracticeLifecycleOption({page,frame,target,adapter,kind,arm,disabledTitle,save}){
  const options=adapter.practice.options,c=options.lifecycle,consumer=options.consumerFields;
  const e={schema:'eagler-touhou/practice-lifecycle-option/1',kind,arm,disabledTitle,samples:[],cleanupErrors:[]};let manual=false,failure;
  const clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action}));
  const advance=()=>clock('tick'),ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:options,advance});
  const input=createPracticeUiInput({page,frame,adapter,advance}),read=async()=>({...await ui.read(),
    uiRaw:await frame.evaluate(readNativeSnapshotRaw,practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice,kind:'ui'})),
    scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target)),runtime:await frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target))});
  try{
    assert.equal((await clock('begin')).mode,'native-manual');manual=true;
    for(let n=0;n<600;n++){const r=await ui.read();if(r.raw.words[consumer.life]===c.aliveState)break;await advance();}
    assert.equal((await ui.read()).raw.words[consumer.life],c.aliveState);
    if(kind==='fast-retry'){
      await input.press(c.menuKey);await ui.click(c.retryControl);await input.press(c.menuKey);await ui.until('Retry Advanced menu not closed',s=>!s.open);
      e.before=await read();assertControlledSceneWorld(e.before.scene);e.hit=await frame.evaluate(requestDeathbombHit,deathbombRequest(target));
    }else{
      e.before=await read();assertControlledSceneWorld(e.before.scene);await input.press(c.menuKey);e.activated=await ui.click(c.deathControl);
      await input.press(c.menuKey);await ui.until('Death Advanced menu not closed',s=>!s.open);
    }
    for(let n=0;n<180;n++){
      await advance();const row=await read();e.samples.push(row);
      if(kind==='one-key-death'?row.raw.words[c.fields.gameOver]===1:
        row.raw.words[consumer.frame]<e.before.raw.words[consumer.frame]&&row.runtime.worldProgress?.comparable===true&&!row.runtime.pause?.active)break;
    }
    e.nativeInput=input.evidence;e.assessment=assertPracticeLifecycleOption(e,options,kind);await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{if(manual)await clock('end');}catch(error){e.cleanupErrors.push(String(error));}await save(e);
    if(e.cleanupErrors.length&&!failure)throw Error('Practice lifecycle option cleanup failed');}
}
