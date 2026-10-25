import assert from 'node:assert/strict';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {decodePracticeUiSnapshot} from '../../tests/adaptation/adapters/practice-ui.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';

export function assertPracticeInputOptions(e,contract){
  const fields=contract.consumerFields;
  assert.equal(e.schema,'eagler-touhou/practice-input-options/1');assertControlledSceneArmed(e.arm);
  assert.deepEqual(e.trials.map(t=>t.id),contract.filters.map(f=>f.id));const identity=e.trials[0].on.samples[0].identity;
  assert.ok(Number.isSafeInteger(identity.app)&&identity.app>=0&&Number.isFinite(identity.documentTimeOrigin)&&new URL(identity.url).protocol==='https:');
  for(const [index,trial] of e.trials.entries())for(const [name,enabled] of [['on',true],['off',false]]){
    const spec=contract.filters[index],rows=trial[name].samples;assert.ok(rows.length>=6&&rows.length<=32);let prior;
    for(const [i,row] of rows.entries()){
      assert.deepEqual(row.identity,identity);const native=decodeNativeControls(row.raw),ui=decodePracticeUiSnapshot(row.uiRaw),w=row.raw.words;
      assert.equal(ui.inputGeneration,native.generation);assert.equal(ui.renderedGeneration,native.renderedGeneration);
      assert.equal(native.open,false);assert.equal(w[fields.inGame],1);assert.equal(w[spec.stateWord],Number(enabled));assertControlledSceneWorld(row.scene);
      assert.equal(row.scene.requestGeneration,e.arm.requestGeneration);
      if(prior){assert.equal(w[fields.sequence],prior[fields.sequence]+1);assert.equal(w[fields.frame],prior[fields.frame]+1);}prior=w;
      const physical=Boolean(ui.keyboard.down[spec.vk>>>5]&(1<<(spec.vk&31)));
      assert.equal(physical,Boolean(spec.key),'Native physical key path differs from the declared control');
      if(i>=contract.consumerLatencyTicks+1)assert.equal(w[fields.current]&spec.mask,enabled?spec.enabledMask:spec.disabledMask,'Native player consumed the wrong filtered input');
    }
  }
  assert.deepEqual(e.cleanupErrors,[]);return{complete:false,filters:contract.filters.map(f=>f.id),scope:'Actual native input consumers and physical positive controls'};
}

export async function runPracticeOptionsBrowser({page,frame,target,adapter,arm,press,save}){
  const contract=adapter.practice.options,e={schema:'eagler-touhou/practice-input-options/1',arm,trials:[],cleanupErrors:[]};
  const controls=createNativeUiControls({page,frame,runtime:adapter.runtime,contract,save:async ui=>{e.ui=ui;await save(e);}});
  const clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action}));
  let manual=false,held=null,failure;
  const begin=async()=>{const state=await clock('begin');assert.equal(state.mode,'native-manual');manual=true;};
  const end=async()=>{if(manual){await clock('end');manual=false;}};
  const key=async(code,down)=>{await frame.locator('canvas').focus();await page.keyboard[down?'down':'up'](code);held=down?code:null;};
  const read=async()=>{const row=await controls.read();return{raw:row.raw,identity:row.identity,
    uiRaw:await frame.evaluate(readNativeSnapshotRaw,practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice,kind:'ui'})),
    scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target))};};
  try{
    await begin();for(let n=0;n<600;n++){const state=await controls.read();if(state.raw.words[contract.consumerFields.life]===0)break;await clock('tick');}
    assert.equal((await controls.read()).raw.words[contract.consumerFields.life],0,'Advanced input arena never became playable');await end();
    for(const spec of contract.filters){
      const trial={id:spec.id};e.trials.push(trial);
      for(const [name,enabled] of [['on',true],['off',false]]){
        await press('F12');await controls.until('Advanced menu not open',s=>s.open&&s.generation===s.renderedGeneration);
        const before=await controls.locate(spec.semantic),item=before.state.controls.find(c=>c.semantic===spec.semantic);
        if(Boolean(item.value)!==enabled)await controls.click(spec.semantic);
        await controls.until('Native input option value did not change',s=>s.controls.some(c=>c.semantic===spec.semantic&&Boolean(c.value)===enabled));
        await press('F12');await controls.until('Advanced menu not closed',s=>!s.open);await begin();
        trial[name]={samples:[]};if(spec.key)await key(spec.key,true);
        for(let n=0;n<6;n++){await clock('tick');trial[name].samples.push(await read());}
        if(held)await key(held,false);for(let n=0;n<3;n++)await clock('tick');await end();await save(e);
      }
    }
    e.assessment=assertPracticeInputOptions(e,contract);await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{if(held)await key(held,false);await end();}catch(error){e.cleanupErrors.push(String(error));}
    await save(e);if(e.cleanupErrors.length&&!failure)throw Error('Advanced input cleanup failed');}
}
