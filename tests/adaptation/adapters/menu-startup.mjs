import assert from 'node:assert/strict';

export function assertMenuStartupObservation(value,contract,runtime){
  assert.equal(contract?.schema,'eagler-touhou/title-startup-navigation/1');
  assert.equal(value?.available,true,'Startup requires an actual native title observation');
  for(const key of ['generation','pointer','epoch'])assert.ok(Number.isInteger(runtime?.[key])&&runtime[key]>=(key==='epoch'?0:1),'Startup lacks native Runtime identity');
  assert.deepEqual(value.runtime,runtime,'Startup crossed native Runtime identity');
  assert.ok(contract.nativeScenes.includes(value.nativeScene),'Startup entered an undeclared native application screen');
  assert.equal(value.scene,value.nativeScene===contract.titleScene?'title':'other','Startup scene label differs from native screen');
  const t=value.title;
  assert.ok(contract.screens.includes(t?.screen),'Startup entered an undeclared native menu screen');
  for(const name of ['cursor','phase','stateTimer2'])assert.ok(Number.isInteger(t[name])&&t[name]>=-1,`Startup lacks native ${name}`);
  assert.ok(Number.isInteger(t.startupElapsed)&&t.startupElapsed>=-1,'Startup lacks the real startup draw counter');
  assert.equal(typeof t.confirmationReady,'boolean');
  for(const name of ['current','previous'])assert.ok(Number.isInteger(t.input?.[name])&&t.input[name]>=0&&t.input[name]<=0xffff,'Startup input is invalid');
  return t;
}

/** This is the producer and offline gate; wall time is not inferred from logical ticks. */
export function assertMenuStartupEvidence(evidence,contract,expected,runtime,confirmMask){
  assert.deepEqual(evidence?.contract,contract,'Startup uses another title navigation contract');
  assert.ok(Number.isInteger(contract.ticks)&&contract.ticks>0&&contract.ticks<=256);
  assert.ok(Array.isArray(evidence.observations)&&evidence.observations.length>=1,'Startup has no raw native samples');
  let tick=0,consumer=null;const labels=new Set();
  for(const row of evidence.observations){
    assert.ok(typeof row.label==='string'&&!labels.has(row.label),'Startup sample label is absent or repeated');labels.add(row.label);
    assert.ok(Number.isInteger(row.tick)&&row.tick>=tick&&row.tick<=tick+1&&row.tick<=contract.ticks,'Startup exceeded or skipped its actual tick bound');tick=row.tick;
    const t=assertMenuStartupObservation(row.value,contract,runtime);
    if(row.label==='startup-confirm-consumer'){
      assert.equal(consumer,null,'Startup submitted multiple prompt confirmations');consumer=row;
      assert.equal(t.screen,contract.promptScreen);assert.equal(t.phase,contract.promptTransitionPhase);
      assert.ok((t.input.current&confirmMask)!==0&&(t.input.previous&confirmMask)===0,'Startup confirm was not a fresh native edge');
      const ready=evidence.observations.find(item=>item.label==='startup-ready-prompt');
      assert.ok(ready&&ready.tick<row.tick,'Startup lacks the prior native ready prompt');
      assert.equal(ready.value.nativeScene,contract.titleScene);assert.equal(ready.value.title.screen,contract.promptScreen);
      assert.equal(ready.value.title.phase,contract.promptReadyPhase);assert.equal(ready.value.title.confirmationReady,true);
    }else if(!consumer)assert.equal(t.input.current,0,'Startup received input before its real prompt was ready');
  }
  const first=evidence.observations[0];assert.equal(first.tick,0,'Startup initial observation is missing');
  assert.equal(evidence.ticks,tick,'Startup executed tick count differs from its samples');
  for(const wait of evidence.physicalWaits??[]){
    assert.equal(wait.requestedMs,contract.physicalWaitMs,'Startup changed the real presentation wait');
    assert.ok(Number.isFinite(wait.startedAtMs)&&Number.isFinite(wait.completedAtMs)&&wait.completedAtMs-wait.startedAtMs>=wait.requestedMs,'Startup has no completed physical wait');
    const before=evidence.observations.find(row=>row.label===wait.before),after=evidence.observations.find(row=>row.label===wait.after);
    assert.ok(before&&after,'Startup physical wait lost its native before/after samples');
    assert.equal(before.tick,after.tick);assert.deepEqual(after.value,before.value,'Native title changed while the manual clock was stopped');
    assert.equal(before.value.nativeScene,contract.titleScene);assert.equal(before.value.title.screen,contract.loadingScreen);
    assert.ok(before.value.title.startupElapsed>=contract.minimumStartupFrames,'Physical wait preceded the actual startup drawing boundary');
  }
  assert.ok((evidence.physicalWaits??[]).length<=1,'Startup repeated the physical deadline');
  const last=evidence.observations.at(-1).value;
  assert.equal(last.nativeScene,contract.titleScene);assert.equal(last.title.screen,expected.startScreen);
  assert.equal(last.title.phase,expected.startPhase);assert.equal(last.title.confirmationReady,true);
  assert.equal(last.title.input.current,0,'Startup left a held input behind');
  if(first.value.title.screen!==expected.startScreen)assert.ok(consumer,'Startup reached main without the actual prompt consumer');
  return evidence;
}
