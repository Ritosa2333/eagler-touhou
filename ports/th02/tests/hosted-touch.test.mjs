import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createHostedTouch} from '../web/hosted-touch.mjs';
import {packTouch,unpackTouch} from '../web/touch-input.js';

function harness(mode='touch'){
  let scene=0,now=0;
  const options={touchEnabled:true,touchMovementMode:mode,touchSensitivity:150,touchFocusMode:'two-finger',doubleTapBombEnabled:true};
  const panel=[];
  const touch=createHostedTouch({options:()=>options,scene:()=>scene,panelPointer:p=>panel.push(p),now:()=>now});
  const point=(type,id,x=100,y=200)=>touch.pointer({type,id,x,y});
  const controls=state=>touch.controls({fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,...state});
  return {touch,options,panel,point,controls,scene:value=>{scene=value;},clock:value=>{now=value;}};
}

test('direct touch confirms, navigates and backs out of native menus',()=>{
  const h=harness();
  h.point('down',1);h.point('up',1);
  assert.equal(h.touch.sample(),32);
  h.point('down',1);h.point('move',1,100,235);h.point('up',1);
  assert.equal(h.touch.sample(),2);
  h.point('down',1);h.point('down',2);h.point('up',1);h.point('up',2);
  assert.equal(h.touch.sample(),128);
  h.scene(2);
  h.point('down',1);h.point('move',1,120,200);h.point('up',1);
  assert.deepEqual(h.panel.map(p=>p.type),['down','move','up']);
  assert.equal(h.touch.sample(),0);
});

test('direct touch packs each move once, accepts two-finger focus and releases on cancel',()=>{
  const h=harness('touch-unlimited');h.scene(1);
  h.point('down',1);h.point('move',1,115,190);h.point('down',2);
  assert.equal(h.touch.sample(),64);
  assert.deepEqual(unpackTouch(packTouch(0,h.touch.motion())),{active:true,unlimited:true,begin:true,x:360,y:-240});
  assert.deepEqual(unpackTouch(packTouch(0,h.touch.motion())),{active:true,unlimited:true,x:0,y:0});
  h.touch.cancel();
  assert.equal(h.touch.sample(),0);
  assert.equal(packTouch(0,h.touch.motion()),0);
});

test('limited drag sends radial deltas and drops the pending target on release',()=>{
  const h=harness();h.scene(1);
  h.point('down',1);h.point('move',1,116,203);
  assert.deepEqual(unpackTouch(packTouch(0,h.touch.motion())),{active:true,unlimited:false,radial:true,begin:true,x:384,y:72});
  h.point('up',1,116,203);
  assert.equal(packTouch(0,h.touch.motion()),0);
});

test('joystick menus, Fire, Bomb and Esc share the regular touch controls',()=>{
  const h=harness('joystick-free');
  h.controls({joystickY:20000});
  assert.equal(h.touch.sample(),2);
  h.controls({fireEnabled:true});
  assert.equal(h.touch.sample(),32);
  h.scene(1);
  h.controls({fireEnabled:true,bombSerial:1,escapeSerial:1,joystickX:20000});
  assert.equal(h.touch.sample(),32|16|128);
  assert.equal(h.touch.sample(),32);
  assert.equal(unpackTouch(packTouch(0,h.touch.motion())).x,49);
  h.controls({fireEnabled:true,bombSerial:1,escapeSerial:1});
  assert.equal(h.touch.sample(),32);
  assert.equal(h.touch.motion(),undefined);
  h.point('down',1);h.point('up',1);h.touch.sample();h.clock(200);h.point('down',1);
  assert.equal(h.touch.sample(),32|16);
});
