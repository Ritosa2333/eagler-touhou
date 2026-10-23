import test from 'node:test';
import assert from 'node:assert/strict';
import {assertPausedWorldStable,assertSynchronizedPause} from './room-lifecycle.mjs';

const paused = () => [0,1,2].map(localSeat=>({frame: 400, observation: {localSeat,
  phase: {name:'paused'}, pause: {active:true}, frames: {simulation:240}}}));

test('network clock advances without advancing the paused world',()=>{
  const before=paused(),after=paused();
  after.forEach(state=>state.frame+=100);
  assert.doesNotThrow(()=>assertPausedWorldStable(before,after));
});
test('one participant continuing to simulate fails the shared pause',()=>{
  const before=paused(),after=paused();after[2].observation.frames.simulation++;
  assert.throws(()=>assertPausedWorldStable(before,after),/continued to simulate/);
});
test('missing or incorrectly attributed seats cannot establish synchronized pause',()=>{
  assert.throws(()=>assertPausedWorldStable(paused(),paused().slice(0,2)),/disappeared/);
  const wrong=paused();wrong[1].observation.localSeat=0;
  assert.throws(()=>assertPausedWorldStable(paused(),wrong),/another seat/);
});
test('a result screen or missing world clock cannot establish native pause',()=>{
  const results=paused();results[0].observation.phase.name='game_over';
  assert.throws(()=>assertPausedWorldStable(paused(),results),/not the pause screen/);
  const missing=paused();delete missing[1].observation.frames.simulation;
  assert.throws(()=>assertPausedWorldStable(paused(),missing),/missing native world clock/);
  const failed=paused();failed[1].failed=true;
  assert.throws(()=>assertPausedWorldStable(paused(),failed),/native room is failed/);
});
test('th08 pause compares its native enemy clock and player position independently of transport',()=>{
  const th08=()=>paused().slice(0,2).map((snapshot,seat)=>({...snapshot,observation:{...snapshot.observation,
    game:'th08',variant:'multiplayer',frames:{multiplayer:400},worldProgress:{counters:{stage:1,enemySceneFrame:250},
      state:{paused:true},localPlayer:{seat,position:{x:192,y:384}}}}}));
  const before=th08(),after=th08();after[0].observation.frames.multiplayer+=30;
  assert.doesNotThrow(()=>assertSynchronizedPause(before));
  const differentStage=th08();differentStage[1].observation.worldProgress.counters.stage=2;
  assert.throws(()=>assertSynchronizedPause(differentStage),/different stages/);
  assert.doesNotThrow(()=>assertPausedWorldStable(before,after));
  after[1].observation.worldProgress.localPlayer.position.x++;
  assert.throws(()=>assertPausedWorldStable(before,after),/continued to simulate/);
});
