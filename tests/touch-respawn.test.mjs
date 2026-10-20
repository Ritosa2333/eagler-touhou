import test from 'node:test';
import assert from 'node:assert/strict';
import {TouchInput} from '../src/touch-input.mjs';
const game=(instance='1:1',extra={})=>({key:'game:'+instance,kind:'game',instance,ready:true,x:0,y:20000,fast:4,slow:2,...extra});
const snapshot=(focusEnabled=true)=>({fireEnabled:true,focusEnabled,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0});
function input(mode='toggle-button'){const touch=new TouchInput();touch.configure({touchEnabled:true,touchFocusMode:mode,touchMovementMode:'touch'});touch.controls(snapshot(),game());return touch;}

test('toggle focus survives repeated player respawns without another host snapshot',()=>{
  const touch=input();
  for(const instance of ['1:1','1:2','1:3'])assert.equal(touch.sample(game(instance)).held&8,8);
  touch.controls(snapshot(false),game('1:3'));assert.equal(touch.sample(game('1:3')).held&8,0);
});
test('respawn clears stale drag targets and Bomb pulses while retaining toggle focus',()=>{
  const touch=input();touch.pointer({id:1,type:'down',x:100,y:200},game(),0);
  touch.pointer({id:1,type:'move',x:120,y:200},game(),10);touch.pending=2;
  const next=game('1:2');assert.deepEqual(touch.sample(next),{held:9,pressed:0,motion:null});
  assert.equal(touch.points.size,0);assert.equal(touch.primary,null);
  touch.pointer({id:2,type:'down',x:100,y:200},next,20);touch.pointer({id:2,type:'move',x:120,y:200},next,30);
  assert.equal(touch.sample(next).motion.x,1920,'new drag uses selected slow-speed ratio');
});
for(const kind of ['menu','replay'])test(`entering ${kind} still clears toggled focus`,()=>{
  const touch=input();touch.sample({key:kind+':1',kind,instance:'',ready:true});
  assert.equal(touch.focus,false);assert.equal(touch.sample(game('2:1')).held&8,0);
});
test('explicit cancellation and disabling touch still release toggled focus',()=>{
  const touch=input();touch.cancel();assert.equal(touch.sample(game()).held&8,0);
  touch.controls(snapshot(),game());touch.configure({touchEnabled:false});assert.equal(touch.focus,false);
});
for(const mode of ['hold-button','two-finger'])test(`${mode} does not acquire a persistent focus latch on respawn`,()=>{
  const touch=input(mode);assert.equal(touch.sample(game('1:2')).held&8,0);
});
