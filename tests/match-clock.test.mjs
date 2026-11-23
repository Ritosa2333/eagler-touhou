import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMatchClock} from '../runtime/match-clock.mjs';

const step=1000/60;
test('clock keeps sixty logical ticks on high refresh and discards background or paused time',()=>{
  const options=[],clock=createMatchClock({stepFrame:request=>{options.push(request);return {advanced:true};}});
  for(let i=0;i<=240;i++)clock.advance(i*step/2,true);
  assert.equal(options.length,120);assert.ok(options.every(request=>request.allowPrediction===false));
  assert.equal(clock.advance(3000,true).stalled,true);assert.equal(options.length,120);
  clock.advance(3018,false);clock.input();assert.equal(options.length,120);
  clock.advance(3034,true);assert.equal(options.length,120);
  assert.equal(clock.advance(3052,true).advanced,1);
});

test('an arriving packet can consume time already due; prediction is a bounded fallback one step later',()=>{
  let confirmed=false;const calls=[];
  const clock=createMatchClock({stepFrame({allowPrediction}){
    calls.push(allowPrediction);
    return {advanced:confirmed||allowPrediction,reason:confirmed||allowPrediction?undefined:'waiting-input'};
  }});
  clock.advance(0,true);assert.equal(clock.advance(step,true).advanced,0);assert.deepEqual(calls,[false]);
  confirmed=true;assert.equal(clock.input().advanced,1);confirmed=false;
  assert.equal(clock.advance(2*step,true).advanced,0);
  assert.equal(clock.advance(3*step,true).advanced,1);assert.deepEqual(calls,[false,false,false,true,false]);
  assert.equal(clock.advance(3*step+200,true).advanced,2,'large debt is capped at three steps and one remains reserved');
  assert.equal(clock.input().advanced,0,'network input callbacks cannot run speculative frames');
});
