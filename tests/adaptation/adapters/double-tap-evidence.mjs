import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assertTouchDoubleTapBombEvidence} from '../../../tools/ports/touch-double-tap-scenes.mjs';

export function assessRuntimeDoubleTapEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-touch-double-tap-bomb')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-double-tap-bomb',checks:['TOUCH-11']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(target=>target.game===product.game&&target.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
    assert.equal(entry.status,'passed');const adapter=resolveAdaptationAdapter(product.game,product.variant);
    const trials=entry.observation.trials;assert.equal(trials.length,2);assert.notEqual(trials[0].epoch,trials[1].epoch);
    for(const enabled of [false,true]){
      const matches=trials.filter(trial=>trial.evidence.launchOptions.doubleTapBombEnabled===enabled);assert.equal(matches.length,1);
      const trial=matches[0];assert.equal(trial.checkStatus,'passed');assert.equal(trial.clock.mode,'native-manual');
      const launch=target.launchOptionsByEpoch.filter(launch=>launch.epoch===trial.epoch);assert.equal(launch.length,1);
      assert.equal(launch[0].options.doubleTapBombEnabled,enabled);assert.equal(trial.evidence.launchOptions.epoch,trial.epoch);
      assert.equal(trial.transition.epoch,trial.epoch);
      assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===trial.epoch&&row.ready===true));
      assertTouchDoubleTapBombEvidence(trial.evidence,{gesture:adapter.doubleTapBomb,inputMasks:adapter.actions.inputMasks,bombResource:adapter.actions.bombResource});
    }
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
