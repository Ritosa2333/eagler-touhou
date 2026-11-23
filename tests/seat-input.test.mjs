import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createSeatInput,TH128_HELD} from '../runtime/seat-input.mjs';
import {createMatchClock} from '../runtime/match-clock.mjs';

test('per-seat source aggregation releases only the source that ended',()=>{
  const input=createSeatInput({allowedMask:763});
  input.key('ShiftLeft',true);input.key('ShiftRight',true);input.key('ArrowLeft',true);
  input.touch(4,TH128_HELD.ArrowLeft);input.touch(5,TH128_HELD.KeyZ);
  input.key('ShiftLeft',false);input.key('ArrowLeft',false);
  assert.equal(input.read(),8|64|1);
  input.touch(4,0);assert.equal(input.read(),8|1);
  input.clear();assert.equal(input.read(),0);
  assert.equal(input.key('Escape',true),false);
  assert.throws(()=>input.touch(5,4),/native mask/);
});

test('C and left Ctrl share rapid fire without early release; right Ctrl is not bound',()=>{
 const input=createSeatInput({allowedMask:763});
 assert.equal(input.key('ControlRight',true),false);assert.equal(input.read(),0);
 for(const code of ['KeyC','ControlLeft'])assert.equal(input.key(code,true),true,code);
 assert.equal(input.read(),512);
 assert.equal(input.key('ControlRight',false),false);input.key('KeyC',false);assert.equal(input.read(),512);
 input.key('ControlLeft',false);assert.equal(input.read(),0);
 assert.equal(createSeatInput({allowedMask:251}).key('ControlLeft',true),false);
});
test('two same-machine players sample only their assigned standard controller index',()=>{
  const buttons=()=>Array.from({length:16},()=>({pressed:false,value:0}));
  const a={index:0,mapping:'standard',connected:true,axes:[-.95,0],buttons:buttons()},
    b={index:1,mapping:'standard',connected:true,axes:[0,.98],buttons:buttons()};
  a.buttons[0].pressed=true;b.buttons[2].pressed=true;b.buttons[4].value=1;
  const one=createSeatInput({allowedMask:763,controllerIndex:0}),two=createSeatInput({allowedMask:763,controllerIndex:1});
  assert.equal(one.read([a,b]),64|1);assert.equal(two.read([a,b]),32|512|8);
  b.connected=false;assert.equal(two.read([a,b]),0);
  assert.equal(one.read([a,b]),64|1);
});

test('a physical key tap between native samples is retained once, and blur drops pending edges',()=>{
  const input=createSeatInput({allowedMask:763});
  input.key('ArrowRight',true);input.key('ArrowRight',false);
  assert.equal(input.read(),128);assert.equal(input.read(),0);
  input.key('KeyX',true);input.key('KeyX',false);input.clear();
  assert.equal(input.read(),0);
});

test('a completed focused tap survives repeatedly stalled presentation until its first real sample',()=>{
  const input=createSeatInput({allowedMask:763}),queued=[];
  const clock=createMatchClock({stepFrame(){queued.push(input.read());return {advanced:true};}});
  clock.advance(0,true);input.key('ArrowRight',true);input.key('ArrowRight',false);input.key('ArrowLeft',true);
  for(const time of [1023,2046]){assert.equal(clock.advance(time,true).stalled,true);input.presentationStall();}
  assert.deepEqual(queued,[]);
  clock.advance(2063,true);clock.advance(2080,true);assert.deepEqual(queued,[128,0]);
  input.key('ArrowRight',true);input.key('ArrowRight',false);input.presentationStall();input.clear();
  assert.equal(input.read(),0);
});

test('a focused key that finishes after the stalled presentation belongs to the same single edge',()=>{
  const input=createSeatInput({allowedMask:763});
  input.key('ArrowRight',true);input.presentationStall();input.key('ArrowRight',false);
  assert.equal(input.read(),128);assert.equal(input.read(),0);
  input.key('ArrowLeft',true);input.presentationStall();input.read();input.key('ArrowLeft',false);
  assert.equal(input.read(),0,'a previous unsampled hold expires at the first actual sample');
});

test('another seated canvas taking focus releases holds but does not erase the previous seat completed tap',()=>{
  const input=createSeatInput({allowedMask:763});
  input.key('ArrowLeft',true);input.key('ArrowLeft',false);
  input.key('ArrowRight',true);input.key('KeyX',true);input.key('KeyX',false);input.touch(7,128);
  input.focusLost();
  assert.equal(input.read(),64);assert.equal(input.read(),0);
  input.key('ArrowLeft',true);input.key('ArrowLeft',false);input.focusLost();input.clear();assert.equal(input.read(),0);
});
