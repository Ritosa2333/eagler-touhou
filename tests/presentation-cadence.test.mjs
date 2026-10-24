import test from 'node:test';import assert from 'node:assert/strict';
import {needsPresentationInterpolation as needs,PresentationCadence,PresentationLimiter,needsPresentationCapture} from '../src/presentation-cadence.mjs';
test('real75/90/120/144Hz frames retain high refresh interpolation',()=>{
  for(const rate of [75,90,120,144])assert.equal(needs(1000/rate),true);
});
test('60Hz and overloaded frames do not build or interpolate unused endpoints',()=>{
  for(const rate of [60,50,43,30,15])assert.equal(needs(1000/rate),false);
});
test('cadence switches immediately after stalls and recovers without changing the logical clock',()=>{
  const samples=[8.33,8.33,33.33,50,16.67,8.33];
  assert.deepEqual(samples.map(n=>needs(n)),[true,true,false,false,false,true]);
  assert.equal(needs(16.67,1000/15),true,'slow replay can still present intermediate frames');
});
test('invalid or suspended frame intervals never request interpolation',()=>{
  for(const n of [0,-1,NaN,Infinity])assert.equal(needs(n),false);
});
test('intermittent fast callbacks under load do not oscillate interpolation on and off',()=>{
  const cadence=new PresentationCadence();
  assert.deepEqual([8,8,8,8,25,8,16,8,8,8,8].map(n=>cadence.observe(n)),[false,false,false,true,false,false,false,false,false,false,true]);
});
test('60 Hz presentation retains fractional refresh phase on 60 through 240 Hz displays',()=>{
  for(const hz of [60,75,90,120,144,165,240]){
    const limiter=new PresentationLimiter();let count=0;
    for(let i=0;i<hz*60;i++)if(limiter.allows(i*1000/hz))count++;
    assert.equal(count,3600,'refresh '+hz);
  }
});
test('presentation limiter skips suspended deadlines and resumes unlocked rendering immediately',()=>{
  const limiter=new PresentationLimiter();assert.equal(limiter.allows(0),true);
  assert.equal(limiter.allows(8),false);assert.equal(limiter.allows(1000),true);
  assert.equal(limiter.allows(1001),false);
  assert.equal(limiter.allows(1002,false),true);assert.equal(limiter.allows(1003,false),true);
  assert.equal(limiter.allows(1004,true),true);assert.equal(limiter.allows(1005,true),false);
});
test('catch-up preserves every logic tick and captures the endpoints required by interpolation',()=>{
  const step=1000/60;
  for(const interpolate of [false,true]){
    let remaining=step*5+.1,ticks=0,captures=[];
    while(remaining>=step){remaining-=step;ticks++;if(needsPresentationCapture(remaining,step,interpolate))captures.push(ticks);}
    assert.equal(ticks,5);assert.deepEqual(captures,interpolate?[4,5]:[5]);
  }
});
