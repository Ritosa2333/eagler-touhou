import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assertTouchDeathbombEvidence} from '../../../tools/ports/touch-deathbomb-scenes.mjs';

export function assessRuntimeDeathbombEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-touch-deathbomb')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-deathbomb-window',checks:['TOUCH-09']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const matches=driver.targets.filter(target=>target.game===product.game&&target.variant===product.variant);
    assert.equal(matches.length,1);const target=matches[0];
    assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
    assert.equal(entry.status,'passed');
    const adapter=resolveAdaptationAdapter(product.game,product.variant);
    assert.ok(adapter?.deathbomb&&adapter.actions,'Register native Deathbomb and action semantics');
    const trials=entry.observation?.trials;assert.equal(trials?.length,3,'Deathbomb requires keyboard/touch last-window and late-input trials');
    assert.equal(new Set(trials.map(trial=>trial.epoch)).size,3,'Deathbomb trials must use fresh isolated epochs');
    for(const [source,timing] of [['keyboard','last-window'],['touch','last-window'],['touch','late']]){
      const matching=trials.filter(trial=>trial.source===source&&trial.timing===timing);assert.equal(matching.length,1);
      const trial=matching[0];assert.equal(trial.checkStatus,'passed');assert.equal(trial.clock?.mode,'native-manual');
      assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===trial.epoch&&row.ready===true),'Deathbomb native world was not ready');
      assert.equal(trial.evidence.source,source);assert.equal(trial.evidence.timing,timing);
      for(const operation of [...trial.evidence.touch,...trial.evidence.controls,...trial.evidence.keys,...trial.evidence.cleanup]){
        const receipt=operation.result;
        if(receipt?.epoch!==undefined)assert.equal(receipt.epoch,trial.epoch,'Deathbomb input receipt belongs to another epoch');
        if(receipt?.game!==undefined)assert.equal(receipt.game,product.game);
        if(receipt?.physicalReceipt?.epoch!==undefined)assert.equal(receipt.physicalReceipt.epoch,trial.epoch);
      }
      assertTouchDeathbombEvidence(trial.evidence,{contract:{...adapter.deathbomb,inputMasks:adapter.actions.inputMasks}});
    }
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
