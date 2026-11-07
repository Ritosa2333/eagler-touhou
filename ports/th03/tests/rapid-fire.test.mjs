import {test} from 'node:test';
import assert from 'node:assert/strict';
import {FireControl} from '../web/fire-control.js';

test('rapid fire pulses by default, while disabled uses the original held key',()=>{
  const fire=new FireControl(),step=1000/60;
  assert.deepEqual(Array.from({length:8},(_,n)=>fire.sample(true,false,n*step)),[true,false,false,false,true,false,false,false]);
  assert.deepEqual(Array.from({length:8},(_,n)=>fire.sample(true,false,(n+8)*step,false)),Array(8).fill(true));
  assert.equal(fire.sample(false,false,16*step,false),false);
  assert.equal(fire.sample(false,true,17*step,false),true);
  assert.equal(fire.sample(true,false,18*step,true),true);
});
