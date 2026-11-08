import test from 'node:test';
import assert from 'node:assert/strict';
import {hostedPoint,createPointerRouter} from '../web/pointer.mjs';
import {createTouchInput} from '../web/input.mjs';
test('dialogue long press skips with Ctrl and releases without a menu back action',()=>{
 let time=0,scene=13;const keys=[],moves=[];
 const touch=createTouchInput({send:(...key)=>keys.push(key),position:()=>({x:200,y:300}),options:()=>({touchEnabled:true,touchMovementMode:'touch',touchSensitivity:100}),move:(...point)=>moves.push(point),scene:()=>scene,clock:()=>time});
 touch.pointer({type:'down',id:1,x:100,y:100});touch.tick();assert.deepEqual(keys,[]);
 time=300;touch.tick();assert.deepEqual(keys,[['ControlLeft',true]]);assert.deepEqual(moves,[]);
 touch.pointer({type:'up',id:1,x:100,y:100});touch.tick();assert.deepEqual(keys,[['ControlLeft',true],['ControlLeft',false]]);
 touch.pointer({type:'down',id:2,x:100,y:100});scene=4;touch.pointer({type:'move',id:2,x:120,y:100});touch.tick();assert.equal(moves.at(-1)[0],20);
});
test('hosted touch fractions map through portrait and landscape letterboxing',()=>{
 for(const [width,height] of [[390,844],[844,390],[640,480]]){
  const center=hostedPoint({id:-1,type:'down',x:.5,y:.5},width,height);assert.equal(center.x,320);assert.equal(center.y,240);
  const scale=Math.min(width/640,height/480),move=hostedPoint({id:-1,type:'move',x:.5+40*scale/width,y:.5-20*scale/height},width,height);
  assert.ok(Math.abs(move.x-360)<1e-8);assert.ok(Math.abs(move.y-220)<1e-8);
 }
});
test('native panels own touches; passive panel swipes replay their original down',()=>{
 const events=[];let route=1,cancelled=0;
 const router=createPointerRouter({_th05_pointer:()=>route,_th05_pointer_cancel:()=>cancelled++},{pointer:event=>events.push(event)});
 const down={id:2,type:'down',x:100,y:100},move={id:2,type:'move',x:120,y:100},up={...move,type:'up'};
 for(const event of [down,move,up])router.pointer(event);assert.deepEqual(events,[]);
 route=2;router.pointer(down);route=3;router.pointer(move);route=0;router.pointer(up);assert.deepEqual(events,[down,move,up]);
 events.length=0;route=2;router.pointer(down);route=4;router.pointer(up);assert.deepEqual(events,[]);
 router.cancel();assert.equal(cancelled,1);
});

test('auto-fire leaves cold title and character confirmation edges available',()=>{
 let scene=2;const keys=[];const input=createTouchInput({send:(...key)=>keys.push(key),position:()=>({x:200,y:300}),options:()=>({touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:150}),move(){},scene:()=>scene});
 input.controls({fireEnabled:true,focusEnabled:false,joystickX:0,joystickY:0,bombSerial:0,escapeSerial:0});input.tick();assert.deepEqual(keys,[]);
 for(const active of [2,3]){scene=active;input.pointer({type:'down',id:1,x:200,y:200});input.pointer({type:'up',id:1,x:200,y:200});input.tick();input.tick();assert.deepEqual(keys.slice(-2),[['KeyZ',true],['KeyZ',false]]);}
 scene=4;input.tick();assert.deepEqual(keys.at(-1),['KeyZ',true]);scene=14;input.tick();assert.deepEqual(keys.at(-1),['KeyZ',false]);input.tick();assert.deepEqual(keys.at(-1),['KeyZ',false]);scene=2;input.tick();
});

test('an ending long press supplies Ctrl only while the stationary finger is held',()=>{
 let time=0,scene=5;const keys=[];
 const input=createTouchInput({send:(...key)=>keys.push(key),options:()=>({touchEnabled:true,touchMovementMode:'touch'}),position:()=>({x:100,y:100}),scene:()=>scene,clock:()=>time});
 input.pointer({type:'down',id:1,x:100,y:100});
 time=499;input.tick();assert.deepEqual(keys,[]);
 time=500;input.tick();assert.deepEqual(keys,[['ControlLeft',true]]);
 input.pointer({type:'up',id:1,x:100,y:100});input.tick();
 assert.deepEqual(keys,[['ControlLeft',true],['ControlLeft',false]]);
 input.pointer({type:'down',id:2,x:100,y:100});
 input.pointer({type:'move',id:2,x:130,y:100});
 time=1100;input.tick();assert.ok(keys.slice(2).every(([key])=>key!=='ControlLeft'));
 input.cancel();scene=4;input.pointer({type:'down',id:3,x:100,y:100});
 time=1700;scene=5;input.tick();
 assert.ok(keys.slice(2).every(([key])=>key!=='ControlLeft'),'a finger held before the ending cannot skip it');
});
