import test from 'node:test';
import assert from 'node:assert/strict';
import {createGamepadActionInput,GAMEPAD_ACTION_BUTTONS} from '../../../tools/ports/gamepad-action-input.mjs';
import {assertGamepadActionJournal} from './gamepad-input-evidence.mjs';
async function fixture(){
  const buttons=Array(17).fill(0),bits={fire:1,bomb:2,focus:4,escape:8};
  const input=createGamepadActionInput({step:async()=>{},button:async(button,down)=>{
    buttons[button]=Number(down);return{acknowledged:true,source:'production-gamepad',epoch:2,button,down,
      device:{connected:true,buttons:[...buttons],axes:[0,0,0,0]},bits:Object.entries(GAMEPAD_ACTION_BUTTONS).reduce((mask,[action,index])=>mask|(buttons[index]?bits[action]:0),0)};
  }},{pulseTicks:3});
  const controls={focusEnabled:false,fireEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0};
  for(const fields of [{focusEnabled:true},{focusEnabled:false,fireEnabled:true},{fireEnabled:false,bombSerial:1},{escapeSerial:1},{escapeSerial:2}]){
    Object.assign(controls,fields);await input.controls(controls);await input.step(4);
  }
  await input.dispose();return JSON.parse(JSON.stringify(input.evidence));
}
const options={epoch:2,pulseTicks:3};
test('native gamepad journal requires source buttons, held mapping, one Bomb and two pause edges',async()=>assertGamepadActionJournal(await fixture(),options));
for(const [name,mutate] of Object.entries({
  'duplicate Bomb':e=>e.events.push({...e.events.find(row=>row.action==='bomb'&&row.down)}),
  'mapper omitted focus':e=>e.events[0].receipt.bits=0,
  'wrong button':e=>e.events[0].button=5,
  'unreleased action':e=>e.events.pop(),
  'stuck physical control':e=>e.events.at(-1).receipt.device.buttons[9]=1,
  'wrong pulse duration':e=>e.events.find(row=>row.action==='bomb'&&!row.down).afterTick++,
  'foreign Runtime':e=>e.events[0].receipt.epoch=1,
  'unacknowledged operation':e=>e.events[0].receipt.acknowledged=false,
}))test(`native gamepad proof rejects ${name}`,async()=>{const e=await fixture();mutate(e);assert.throws(()=>assertGamepadActionJournal(e,options));});
