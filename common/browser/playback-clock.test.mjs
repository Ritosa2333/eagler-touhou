import test from 'node:test';
import assert from 'node:assert/strict';
import {PlaybackClock} from './playback-clock.mjs';

function run(rate, hz) {
  const clock = new PlaybackClock({now:()=>0}); clock.speed(rate);
  let frame = 0, hash = 17;
  const input = Array.from({length:2000}, (_,i)=>(i*13)%31);
  const tick = {position:()=>frame, step:()=>{hash=(Math.imul(hash,31)+input[frame++])>>>0;}};
  for(let t=0;t<=hz;t++)clock.tick(t*1000/hz,tick);
  return {frame,hash};
}
test('display cadence and speed preserve the same input order at a fixed logical frame',()=>{
  for(const rate of [.5,1,2,4,8]) {
    const a=run(rate,60), b=run(rate,144);
    assert.equal(a.frame,60*rate);assert.deepEqual(b,a);
  }
});
test('paused seeking stops at the exact target, remains paused and never keeps accelerating',()=>{
  const clock=new PlaybackClock({maxSteps:20,now:()=>0});let frame=0;
  const tick={position:()=>frame,step:()=>{frame++;}};
  clock.pause(true);clock.seek(53);
  for(let t=0;t<100;t++)clock.tick(t*17,tick);
  assert.equal(frame,53);assert.equal(clock.target,null);assert.equal(clock.paused,true);
  clock.pause(false);clock.tick(2000,tick);clock.tick(3000,tick);
  assert.equal(frame,68); // bounded 250ms normal-speed debt, not seek speed
});
test('seeking yields to rendering and invalid positions/rates fail before stepping',()=>{
  let now=0,frame=0;const clock=new PlaybackClock({budgetMs:8,now:()=>now});clock.seek(500);
  assert.equal(clock.tick(0,{position:()=>frame,step:()=>{now+=3;frame++;}}),3);
  assert.equal(frame,3);assert.equal(clock.target,500);
  for(const rate of [-1,NaN,Infinity,3])assert.throws(()=>clock.speed(rate));
  for(const frame of [-1,NaN,1.5])assert.throws(()=>clock.seek(frame));
});
