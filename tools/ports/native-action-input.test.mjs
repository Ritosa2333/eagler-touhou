import test from 'node:test';
import assert from 'node:assert/strict';
import {createNativeActionInput} from './native-action-input.mjs';
import {createGamepadActionInput} from './gamepad-action-input.mjs';
const neutral={focusEnabled:false,fireEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0};
test('gamepad transport shares held actions and releases a single Bomb/pause pulse after native ticks',async()=>{
  const held=new Set(),samples=[],events=[];
  const input=createGamepadActionInput({button:async(index,down)=>{
    events.push([index,down]);if(down)held.add(index);else held.delete(index);return{acknowledged:true};
  },step:async()=>samples.push([...held])},{pulseTicks:3});
  await input.controls({...neutral,focusEnabled:true,fireEnabled:true,bombSerial:1,escapeSerial:1});
  await input.step(4);assert.deepEqual(samples,[[4,0,1,9],[4,0,1,9],[4,0,1,9],[4,0]]);
  await input.controls({...neutral,bombSerial:1,escapeSerial:1});await input.dispose();
  assert.equal(input.evidence.source,'production-gamepad');assert.equal(held.size,0);
  for(const index of [0,1,4,9])assert.equal(events.filter(row=>row[0]===index&&row[1]).length,1);
});
test('failed input acknowledgement still releases the dispatched owner',async()=>{
  const events=[];const input=createNativeActionInput({step:async()=>{},set:async(action,down)=>{
    events.push([action,down]);return{acknowledged:!down};
  }},{pulseTicks:3,source:'fixture'});
  await assert.rejects(input.controls({...neutral,fireEnabled:true}),/not acknowledged/);await input.dispose();
  assert.deepEqual(events,[['fire',true],['fire',false]]);
});
