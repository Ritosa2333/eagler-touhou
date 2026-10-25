import assert from 'node:assert/strict';

/** Shared stage restart: the game rebuilds its world inside the same native app. */
export async function runMusicGameplayRestart(io,contract){
  const evidence={schema:'eagler-touhou/music-gameplay-restart/1',contract,inputs:[]};
  try{
    evidence.before=await io.waitUntil('world ready before music restart',sample=>sample.ready&&sample.frame>=contract.minimumFrame);
    evidence.inputs.push(await io.press(contract.pauseKey));
    evidence.paused=await io.waitUntil('game paused before music restart',sample=>sample.paused);
    await io.wait(250);evidence.settled=await io.read();
    evidence.inputs.push(await io.press(contract.restartKey));
    evidence.after=await io.waitUntil('world rebuilt after music restart',sample=>sample.ready&&!sample.paused&&sample.frame<evidence.before.frame&&
      sample.audio.source===contract.expectedSource&&(contract.mode==='ogg'?sample.audio.playing&&sample.audio.cursor<evidence.before.audio.cursor:
        sample.audio.opens>evidence.before.audio.opens));
    evidence.output=await io.collectOutput();evidence.outputState=await io.read();evidence.heldAfter=await io.readHeld();
    assertMusicGameplayRestart(evidence,contract);return evidence;
  }catch(error){error.evidence=evidence;throw error;}
}

export function assertMusicGameplayRestart(e,contract){
  assert.equal(e.schema,'eagler-touhou/music-gameplay-restart/1');assert.deepEqual(e.contract,contract);
  assert.ok(Number.isSafeInteger(contract.minimumFrame)&&contract.minimumFrame>0&&contract.source?.trim());
  const before=e.before,after=e.after;assert.ok(before.ready&&after.ready&&!before.paused&&!after.paused);
  assert.ok(before.identity.game&&Number.isSafeInteger(before.identity.epoch)&&before.identity.epoch>0&&Number.isSafeInteger(before.identity.app)&&before.identity.app>0&&
    Number.isFinite(before.identity.timeOrigin)&&before.identity.url);
  assert.ok(Number.isSafeInteger(before.protocol.first)&&before.protocol.first>0&&Number.isSafeInteger(before.protocol.exit)&&before.protocol.exit>=0);
  assert.ok(before.frame>=contract.minimumFrame&&after.frame>=0&&after.frame<before.frame,'The world did not actually restart');
  assert.equal(e.paused.paused,true);assert.equal(e.settled.paused,true);assert.equal(e.paused.frame,e.settled.frame);
  for(const sample of [before,e.paused,e.settled,after,e.outputState]){
    assert.ok(Number.isSafeInteger(sample.frame)&&sample.frame>=0);assert.ok(Number.isFinite(sample.audio.cursor)&&sample.audio.cursor>=0);
    assert.equal(typeof sample.paused,'boolean');assert.equal(typeof sample.audio.playing,'boolean');
    assert.deepEqual(sample.identity,before.identity,'Native game restart changed the Runtime/app owner');
    assert.deepEqual(sample.protocol,before.protocol,'Native game restart exited or relaunched the Runtime');
    assert.equal(sample.stage,contract.stage);assert.equal(sample.audio.source,contract.expectedSource);
  }
  assert.ok(e.outputState.ready&&!e.outputState.paused&&e.outputState.frame>after.frame,'Restarted world did not continue');
  assert.deepEqual(e.inputs.map(input=>input.code),[contract.pauseKey,contract.restartKey]);
  for(const input of e.inputs){
    assert.equal(input.source,'Host keyboard protocol');
    for(const response of [input.down,input.up]){assert.equal(response.protocol,'eagler-touhou/1');assert.equal(response.ok,true);assert.equal(response.game,before.identity.game);
      assert.equal(response.epoch,before.identity.epoch);assert.ok(response.request);}
    assert.notEqual(input.down.request,input.up.request);
  }
  assert.deepEqual(e.heldAfter,{pause:false,restart:false});
  if(contract.mode==='ogg')assert.ok(after.audio.cursor<before.audio.cursor,'Game restart retained the old OGG cursor');
  else{
    assert.equal(contract.mode,'midi');assert.ok(e.outputState.audio.opens>before.audio.opens,'Game restart did not reopen its SMF');
    for(const sample of [before,e.outputState])for(const field of ['opens','noteOns'])assert.ok(Number.isSafeInteger(sample.audio[field])&&sample.audio[field]>=0);
    assert.ok(e.outputState.audio.noteOns>before.audio.noteOns,'No new MIDI notes followed the native restart');
  }
}
