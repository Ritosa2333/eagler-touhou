import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assertTouchUnlimitedResultEvidence} from '../../../tools/ports/touch-unlimited-result-scenes.mjs';

export function assessUnlimitedResultEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-touch-unlimited-result')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'ordinary-unlimited-result',checks:['TOUCH-10']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');
    const adapter=resolveAdaptationAdapter(product.game,product.variant),observation=entry.observation,e=observation.evidence;
    assert.equal(observation.clock.mode,'native-manual');assert.equal(observation.transition.pagehide,true);
    assert.equal(e.preparation.runtimeEpoch,observation.epoch);
    assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===observation.epoch&&row.ready));
    const configs=target.launchOptionsByEpoch.filter(row=>row.epoch===observation.epoch);assert.equal(configs.length,1);
    assert.equal(configs[0].options.touchMovementMode,'touch-unlimited');assert.equal(configs[0].acknowledgement.ok,true);
    assert.equal(e.states.baseline.controlledWorld,false);assert.equal(e.states.baseline.armed,true);
    assert.equal(e.replay.path,adapter.replay.finalizer.path);
    const bytes=readFileSync(e.replay.artifact);assert.equal(bytes.length,e.replay.bytes);
    assert.equal(createHash('sha256').update(bytes).digest('hex'),e.replay.sha256);
    const contract={...product,sources:adapter.runBoundaries.sources,
      nonzeroTarget:state=>({x:state.playerPosition.x+adapter.runBoundaries.touchGeometry.probeDelta.x,y:state.playerPosition.y+adapter.runBoundaries.touchGeometry.probeDelta.y})};
    const parsed=adapter.parseReplayMetadata(bytes,{expectedSha256:e.replay.sha256});
    assertTouchUnlimitedResultEvidence(e,{contract,parsed});result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
