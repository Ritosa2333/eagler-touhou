import test from 'node:test';
import assert from 'node:assert/strict';
import {createKeyboardActionInput} from './keyboard-action-input.mjs';

const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0};
function fixture(){
  const held=new Set(),samples=[],events=[];
  const input=createKeyboardActionInput({key:async(code,down)=>{if(down)held.add(code);else held.delete(code);events.push({code,down});return{acknowledged:true};},
    step:async()=>samples.push([...held])},{pulseTicks:3});return{held,samples,events,input};
}
test('physical action IO holds real keys and releases edge pulses after the declared logical ticks',async()=>{
  const f=fixture();await f.input.controls({...neutral,fireEnabled:true,focusEnabled:true,bombSerial:1});await f.input.step(4);
  assert.deepEqual(f.samples,[['ShiftLeft','KeyZ','KeyX'],['ShiftLeft','KeyZ','KeyX'],['ShiftLeft','KeyZ','KeyX'],['ShiftLeft','KeyZ']]);
  await f.input.controls({...neutral,bombSerial:1,escapeSerial:1});await f.input.step(4);
  assert.deepEqual(f.samples.slice(4),[['Escape'],['Escape'],['Escape'],[]]);
  await f.input.dispose();assert.equal(f.held.size,0);assert.equal(f.input.evidence.disposed,true);
});
test('unchanged serials never create duplicate physical presses, and arbitrary resets fail',async()=>{
  const f=fixture();await f.input.controls({...neutral,bombSerial:1});await f.input.step(4);await f.input.controls({...neutral,bombSerial:1});
  assert.equal(f.events.filter(row=>row.code==='KeyX'&&row.down).length,1);
  await assert.rejects(f.input.controls(neutral),/advance by one/);await f.input.dispose();
});
test('physical cleanup releases other owners after one key release fails',async()=>{
  const released=[];const input=createKeyboardActionInput({step:async()=>{},key:async(code,down)=>{
    if(!down){released.push(code);if(code==='ShiftLeft')throw Error('release failed');}return{acknowledged:true};}},{pulseTicks:3});
  await input.controls({...neutral,focusEnabled:true,fireEnabled:true});await assert.rejects(input.dispose(),AggregateError);
  assert.deepEqual(released,['ShiftLeft','KeyZ']);
});
test('custom layouts keep canonical action records and dispatch the chosen physical codes',async()=>{
  const events=[];const input=createKeyboardActionInput({step:async()=>{},key:async(code,down)=>{events.push({code,down});return{acknowledged:true};}},
    {pulseTicks:3,codes:{KeyZ:'KeyF',KeyX:'KeyG',ShiftLeft:'KeyT',Escape:'KeyP'}});
  await input.controls({...neutral,fireEnabled:true,focusEnabled:true,bombSerial:1,escapeSerial:1});await input.step(3);await input.dispose();
  assert.deepEqual(events.filter(row=>row.down).map(row=>row.code),['KeyT','KeyF','KeyG','KeyP']);
  assert.deepEqual(input.evidence.events.filter(row=>row.down).map(row=>[row.code,row.physical]),
    [['ShiftLeft','KeyT'],['KeyZ','KeyF'],['KeyX','KeyG'],['Escape','KeyP']]);
  assert.equal(events.length,8);
});
