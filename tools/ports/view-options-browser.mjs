import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runViewOptionsScene,decodeViewOptions,assertViewOptionsRestart} from './view-options-scene.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {readPracticeSnapshotRaw} from '../../tests/adaptation/adapters/practice.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';

/** Existing formal Launcher session, production input, Shell options and loop. */
export async function runLauncherViewOptions({page,frame,target,adapter,initialSetting,relaunch,output,save}){
  const flow=resolveTitleFlow(target),observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
  const until=async(label,predicate)=>{let sample;const end=Date.now()+20000;do{sample=await observe();if(predicate(sample))return sample;await page.waitForTimeout(50);}while(Date.now()<end);throw Error(label+': '+JSON.stringify(sample));};
  const press=async code=>{await frame.locator('canvas').focus();await page.keyboard.press(code,{delay:90});await page.waitForTimeout(140);};
  const scene=()=>frame.evaluate(controlValidationScene,validationSceneRequest(target));
  let activeClock=false,opened=false;
  const read=async()=>{const raw=await frame.evaluate(readPracticeSnapshotRaw,{runtime:adapter.runtime,snapshot:adapter.viewOptions.snapshot});
    return{raw,view:decodeViewOptions(raw),scene:assertControlledSceneWorld(await scene()),
      identity:await frame.evaluate(runtime=>({url:location.href,timeOrigin:performance.timeOrigin,app:window[runtime.global][runtime.app]}),adapter.runtime)};};
  try{
    const evidence=await runViewOptionsScene({policy:adapter.viewOptions,read,save,step:async count=>{for(let i=0;i<count;i++)await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));},
      open:async always=>{
        let setting=initialSetting;
        if(opened){
          for(const key of ['ArrowRight','ShiftLeft'])await page.keyboard.up(key);
          if(activeClock){await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}));activeClock=false;}
          const next=await relaunch({always,previousEpoch:Number(new URL(frame.url()).searchParams.get('runtimeEpoch'))});frame=next.frame;setting=next.setting;
        }else assert.equal(always,false);
        opened=true;if(await page.locator('#touchHelp').isVisible())await page.locator('#touchHelpClose').click();
        const fire=page.locator('#touchFire');if(await fire.getAttribute('aria-pressed')==='true')await fire.click();
        const arm=assertControlledSceneArmed(await frame.evaluate(controlValidationScene,validationSceneRequest({...target,action:'arm'})));
        await flow.enterGameplay({observe,waitUntil:until,press,lane:'desktop'});assertControlledSceneWorld(await scene());
        const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));assert.equal(clock.mode,'native-manual');activeClock=true;
        return{...setting,epoch:Number(new URL(frame.url()).searchParams.get('runtimeEpoch')),arm,clock};
      },
      key:async(code,down)=>{await frame.locator('canvas').focus();await page.keyboard[down?'down':'up'](code);},
      capture:async name=>{const before=await read(),path=resolve(output,`view-${name}.png`);await frame.locator('canvas').screenshot({path});const after=await read(),bytes=await readFile(path);
        return{name,path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),beforeFrame:before.view.frame,afterFrame:after.view.frame,identity:before.identity,view:before.view};},
      release:async()=>{for(const key of ['ArrowRight','ShiftLeft'])await page.keyboard.up(key);},
    });
    await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}));activeClock=false;
    const restart=evidence.restart={before:await observe()};await press('Escape');restart.paused=await until('view scene native pause',s=>s.pause.active);
    await page.waitForTimeout(250);restart.frozen=await observe();
    await flow.restartFromPause({press,waitUntil:until});
    const key=restart.before.capabilities.restartFrame;assert.ok(key);
    restart.reset=await until('view scene native restart resets world',s=>flow.isGameplayReady(s,'desktop')&&s.frames[key]<restart.before.frames[key]);
    restart.advanced=await until('view scene restarted world advances',s=>flow.isGameplayReady(s,'desktop')&&s.frames[key]>restart.reset.frames[key]);
    restart.identity=await frame.evaluate(runtime=>({url:location.href,timeOrigin:performance.timeOrigin,app:window[runtime.global][runtime.app]}),adapter.runtime);
    assertViewOptionsRestart(evidence);
    await save(evidence);return evidence;
  }finally{for(const key of ['ArrowRight','ShiftLeft','Escape','KeyR'])await page.keyboard.up(key);
    if(activeClock)await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}));}
}
