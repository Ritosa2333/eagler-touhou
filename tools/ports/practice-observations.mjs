import assert from 'node:assert/strict';

export function practiceBuildFeature(manifest,product){
  assert.equal(typeof manifest.features?.thprac,'boolean','Current practice acceptance requires an explicit boolean package feature');
  const enabled=manifest.features.thprac;
  if(enabled)assert.equal(product.features.thprac,true,'Package practice feature exceeds product support');
  return enabled;
}

export function practiceGameplayReady(state){
  const frame=state?.game==='th08'?state.worldProgress?.counters?.enemySceneFrame:state?.frames?.simulation;
  return ['stage','match'].includes(state?.phase?.name)&&state?.pause?.active===false&&
    state?.localSeat===0&&Number.isSafeInteger(frame)&&frame>=0;
}
