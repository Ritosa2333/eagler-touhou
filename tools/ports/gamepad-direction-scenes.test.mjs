import test from 'node:test';
import assert from 'node:assert/strict';
import {runGamepadDirections,assertGamepadDirections} from './gamepad-direction-scenes.mjs';
const movement={directionMasks:{ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128}},observation={inputSemantics:'consumed'};
async function fixture(){
  let tick=0,x=100,y=100,vector={mode:'buttons',x:0,y:0};
  const result=await runGamepadDirections({
    set:async value=>{vector=value;return{acknowledged:true,source:'production-gamepad',epoch:1,vector:{...value},
      bits:(value.x<0?64:value.x>0?128:0)|(value.y<0?16:value.y>0?32:0)};},
    step:async()=>{tick++;x+=vector.x;y+=vector.y;},
    observe:async()=>({localSeat:0,playing:true,position:{x,y},capabilities:{restartFrame:'world'},frames:{world:tick},input:{inputSemantics:'consumed',
      value:(vector.x<0?64:vector.x>0?128:0)|(vector.y<0?16:vector.y>0?32:0)}}),
  });return result;
}
const options={movement,observation,isGameplayReady:value=>value.playing,epoch:1};
test('one shared gamepad scene covers Dpad and axes in all eight directions with persistent release',async()=>assertGamepadDirections(await fixture(),options));
for(const [name,mutate] of Object.entries({
  'missing diagonal':e=>e.trials.pop(),
  'missing neutral arming':e=>e.neutral.bits=64,
  'held control suppressed before arming':e=>e.trials[0].press.bits=0,
  'incorrect native direction':e=>e.trials[0].held[3].input.value=128,
  'wrong movement sign':e=>e.trials[0].held[3].position.x+=10,
  'stale native tick':e=>e.trials[0].held[2].frames.world--,
  'drift after release':e=>e.trials[0].released[6].position.x++,
  'stuck native input':e=>e.trials[0].released[6].input.value=64,
  'another seat':e=>e.trials[0].held[0].localSeat=1,
  'wrong input semantics':e=>e.trials[0].held[0].input.inputSemantics='queued-for-next-movement',
  'not in gameplay':e=>e.trials[0].held[0].playing=false,
}))test(`gamepad directions reject ${name}`,async()=>{const e=await fixture();mutate(e);assert.throws(()=>assertGamepadDirections(e,options));});
