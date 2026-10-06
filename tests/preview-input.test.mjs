import test from 'node:test';import assert from 'node:assert/strict';
import {PreviewInput,supportedReplay,validateTouchSnapshot} from '../src/preview-input.mjs';
const snapshot=(extra={})=>({fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100,...extra});

test('keyboard and touch cancellation preserve the other input source',()=>{
  const input=new PreviewInput();input.key('KeyZ',true);input.key('KeyZ',true,'host');input.key('KeyZ',false,'host');
  assert.equal(input.sample(0).held,1);
  input.touch(snapshot({focusEnabled:true,joystickX:1}));input.cancelTouch();assert.equal(input.sample(0).held,1);
  input.touch(snapshot({focusEnabled:true}));input.clearKeyboard();assert.equal(input.sample(0).held,8);
  input.clear();assert.deepEqual(input.sample(0),{held:0,pressed:0});
});
test('host Bomb serial survives render delays and is consumed by exactly one logic sample',()=>{
  const input=new PreviewInput();input.touch(snapshot({bombSerial:1}));input.touch(snapshot({bombSerial:1}));
  assert.deepEqual(input.sample(0),{held:2,pressed:2});assert.deepEqual(input.sample(2),{held:0,pressed:0});
  input.touch(snapshot({bombSerial:1}));assert.equal(input.sample(0).pressed,0);
  input.touch(snapshot({bombSerial:2}));input.clear();input.touch(snapshot({bombSerial:2}));assert.equal(input.sample(0).pressed,0);
  input.touch(snapshot({bombSerial:3}));assert.equal(input.sample(2).pressed,2);
});
test('paused snapshots baseline serials without injecting buttons after resume',()=>{
  const input=new PreviewInput();assert.equal(input.touch(snapshot({bombSerial:3,escapeSerial:1,fireEnabled:true}),false),true);
  assert.deepEqual(input.sample(0),{held:0,pressed:0});assert.equal(input.touch(snapshot({bombSerial:3,escapeSerial:1}),true),false);
});
test('gamepad d-pad maps to the same movement bits as keyboard',()=>{
  const buttons=Array.from({length:16},()=>({pressed:false}));buttons[12].pressed=buttons[14].pressed=true;
  assert.equal(new PreviewInput().sample(0,{connected:true,axes:[0,0],buttons}).held,80);
});
test('a gamepad Bomb held across pause cannot manufacture a fresh Bomb on resume',()=>{
  const input=new PreviewInput(),pad={connected:true,axes:[0,0],buttons:[{pressed:false},{pressed:true}]};
  assert.equal(input.sample(0,pad).pressed,2);input.clear();assert.equal(input.sample(0,pad).pressed,0);
  pad.buttons[1].pressed=false;input.sample(0,pad);pad.buttons[1].pressed=true;assert.equal(input.sample(0,pad).pressed,2);
});
test('Extra cannot be previewed as the ordinary first stage',()=>{
  assert.equal(supportedReplay({difficulty:4,stages:[{number:1}]}),false);
  assert.equal(supportedReplay({difficulty:3,stages:[{number:1}]}),true);
  assert.equal(supportedReplay({difficulty:1,stages:[{number:2}]}),false);
});
test('malformed host touch input is rejected before changing state',()=>{
  for(const value of [{bombSerial:NaN},{escapeSerial:-1},{joystickX:Infinity},{joystickY:2},{fireEnabled:1},{touchSensitivity:500}])assert.throws(()=>validateTouchSnapshot(snapshot(value)));
});
