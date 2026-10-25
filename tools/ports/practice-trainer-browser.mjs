import assert from 'node:assert/strict';
import {runPracticeTrainerTrial} from './practice-trainer-scene.mjs';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {controlValidationScene,validationSceneRequest} from '../../tests/adaptation/adapters/scene.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {requestDeathbombHit,deathbombRequest} from '../../tests/adaptation/adapters/deathbomb.mjs';

/** Reuse the normal Practice journey, physical input and production tick. */
export async function runPracticeTrainerBrowser({page,frame,target,adapter,trial,arm,press,save}){
  const contract=adapter.practice.trainer;
  let optionsSetup;
  if(trial.options?.length){
    const ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:adapter.practice.options});await press('F12');
    for(const option of trial.options){const row=await ui.locate(option.semantic),item=row.state.controls.find(c=>c.semantic===option.semantic);
      if(item.value!==option.value)await ui.click(option.semantic);}
    optionsSetup=await ui.read();await press('F12');
  }
  if(trial.keys.length){
    await press('Backspace');
    for(const key of trial.keys)await press(key);
    await press('Backspace');
  }
  const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));
  assert.equal(clock.mode,'native-manual');
  const request=snapshot=>practiceSnapshotRequest({runtime:adapter.runtime,contract:{snapshot}});
  const read=async()=>({
    raw:await frame.evaluate(readNativeSnapshotRaw,request(contract.snapshot)),
    uiRaw:await frame.evaluate(readNativeSnapshotRaw,request(adapter.practice.uiSnapshot)),
    tracker:await frame.evaluate(readNativeSnapshotRaw,request(contract.tracker)).then(raw=>{assert.equal(raw.available,true);return raw.words;}),
    scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target)),
    identity:await frame.evaluate(runtime=>({app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),adapter.runtime),
  });
  try{return await runPracticeTrainerTrial({arm,read,save,optionsSetup,
    readReady:async()=>({raw:await frame.evaluate(readNativeSnapshotRaw,request(contract.snapshot))}),
    tick:()=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'})),
    hit:()=>frame.evaluate(requestDeathbombHit,deathbombRequest(target)),
    key:async(code,down)=>{await frame.locator('canvas').focus();await page.keyboard[down?'down':'up'](code);},
  },{contract,trial});}
  finally{await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}));}
}
