import test from'node:test';import assert from'node:assert/strict';
import{TouchInput}from'../../site/runtime/upstream-touch-input.mjs';
const state=(change={})=>({key:'game1',kind:'game',instance:1,gestureScope:1,ready:true,x:0,y:51200,fast:576,slow:256,...change});
function create(mode='touch-unlimited'){const input=new TouchInput();input.configure({touchEnabled:true,touchMovementMode:mode,touchSensitivity:100});return input;}
test('upstream direct target accumulates sensitivity in TH128 raw units and limited mode keeps target',()=>{
 for(const[mode,want]of[['touch',1],['touch-unlimited',2]]){const t=create(mode),s=state();t.pointer({id:1,type:'down',x:100,y:100},s,0);t.pointer({id:1,type:'move',x:110,y:90},s,20);assert.deepEqual(t.sample(s).motion,{mode:want,x:1280,y:49920});t.pointer({id:1,type:'up',x:110,y:90},s,40);assert.equal(t.sample(s).motion,null);}
});
test('upstream two-finger focus and free joystick preserve analog magnitude instead of keyboard disc',()=>{
 const t=create('joystick-free'),s=state();t.controls({fireEnabled:true,focusEnabled:false,joystickX:16384,joystickY:-32767,bombSerial:0,escapeSerial:0},s);
 t.pointer({id:1,type:'down',x:100,y:100},s,0);t.pointer({id:2,type:'down',x:150,y:100},s,0);
 const sample=t.sample(s);assert.equal(sample.held,9);assert.deepEqual(sample.motion,{mode:3,x:16384,y:-32767});
 t.cancel();assert.equal(t.sample(s).held,1);assert.equal(t.sample(s).motion,null);
});
test('upstream menu swipe and two-finger tap reach original direction and cancel bits',()=>{
 const t=create(),s=state({key:'menu',kind:'menu',ready:false});t.pointer({id:1,type:'down',x:100,y:100},s,0);t.pointer({id:1,type:'move',x:126,y:100},s,20);assert.equal(t.sample(s).pressed,128);t.pointer({id:1,type:'up',x:126,y:100},s,40);assert.equal(t.sample(s).pressed,0);
 t.pointer({id:1,type:'down',x:100,y:100},s,100);t.pointer({id:2,type:'down',x:150,y:100},s,100);t.pointer({id:1,type:'up',x:100,y:100},s,150);t.pointer({id:2,type:'up',x:150,y:100},s,160);assert.equal(t.sample(s).pressed,2);
});
test('cancelled menu gesture never becomes confirm and replay refuses touch fire or movement',()=>{
 const t=create(),s=state({key:'menu',kind:'menu',ready:false});t.pointer({id:1,type:'down',x:1,y:1},s,0);t.pointer({id:2,type:'down',x:2,y:2},s,0);t.pointer({id:1,type:'cancel',x:1,y:1},s,10);t.pointer({id:2,type:'up',x:2,y:2},s,20);assert.equal(t.sample(s).pressed,0);
 const replay=state({key:'replay',kind:'replay'});t.controls({fireEnabled:true,focusEnabled:true,joystickX:32767,joystickY:0,bombSerial:0,escapeSerial:0},replay);t.pointer({id:3,type:'down',x:100,y:100},replay,100);t.pointer({id:3,type:'move',x:300,y:300},replay,120);assert.deepEqual(t.sample(replay),{held:0,pressed:0,motion:null});
});
test('respawn rebases a held gesture and a new launch discards stale target',()=>{
 const t=create(),s=state();t.pointer({id:1,type:'down',x:100,y:100},s,0);t.pointer({id:1,type:'move',x:120,y:100},s,20);assert.equal(t.sample(s).motion.x,2560);
 const next=state({key:'game2',instance:2,x:512,y:40000});assert.deepEqual(t.sample(next).motion,{mode:2,x:512,y:40000});
 const fresh=state({key:'new',instance:3,gestureScope:3});assert.equal(t.sample(fresh).motion,null);
});
