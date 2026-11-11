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
test('release and charge release cannot leave automatic fire held',()=>{
 const fire=new FireControl(),dt=1000/60;
 for(let frame=0;frame<30;frame++)fire.sample(true,false,frame*dt);
 for(let frame=30;frame<90;frame++)assert.equal(fire.sample(false,false,frame*dt),false);
 assert.equal(fire.sample(false,true,90*dt),true);
 for(let frame=91;frame<130;frame++)assert.equal(fire.sample(false,false,frame*dt),false);
 fire.sample(true,false,130*dt);fire.reset();
 for(let frame=131;frame<160;frame++)assert.equal(fire.sample(false,false,frame*dt),false);
});
test('a short selection press survives polling and the other confirmation animation',()=>{
 const fire=new FireControl();
 const output=Array.from({length:70},(_,frame)=>fire.selection(frame===0,false));
 assert.deepEqual(output.slice(0,4),[false,false,false,false]);
 assert.ok(output.slice(4,64).every(Boolean));
 assert.ok(output.slice(64).every(value=>!value));
 assert.equal(fire.sample(false,false,70*1000/60),false,'selection confirmation does not leak into combat');
});
