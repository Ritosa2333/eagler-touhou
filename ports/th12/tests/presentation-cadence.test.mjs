import test from 'node:test';import assert from 'node:assert/strict';
import {needsPresentationInterpolation as needs,PresentationCadence} from '../src/presentation-cadence.mjs';
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
