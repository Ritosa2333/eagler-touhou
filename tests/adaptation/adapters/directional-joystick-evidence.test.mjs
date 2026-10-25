import test from 'node:test';
import assert from 'node:assert/strict';
import {assertDirectionalJoystickEvidence} from './directional-joystick-evidence.mjs';

const product={game:'th08',variant:'normal'},movement={directionMasks:{ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128},directionSource:'fixture native enum',
  inputBoundary:'queued-for-next-movement',keyboardReleasePolicy:{observationSource:'fixture native input',observationPhase:'post-update'}};
const options={product,movement,observation:{inputSemantics:'queued-for-next-movement'},isGameplayReady:state=>state.playing===true};
function fixture(){
  let frame=0,request=0;
  const state=(value,x=100,y=100)=>({...product,localSeat:0,playing:true,position:{x,y},frames:{world:++frame},capabilities:{restartFrame:'world'},
    input:{value,inputSemantics:movement.inputBoundary,source:movement.keyboardReleasePolicy.observationSource,sampleBoundary:movement.keyboardReleasePolicy.observationPhase}});
  const receipt=()=>({protocol:'eagler-touhou/1',game:'th08',epoch:2,request:String(++request),ok:true,acknowledged:true});
  const e={mode:'joystick',epoch:2,pagehide:true,start:state(0),actions:[]};
  for(const [x,y] of [[-1,0],[1,0],[0,-1],[0,1],[-1,-1],[1,1],[-1,1],[1,-1]]){
    const mask=(x<0?64:x>0?128:0)|(y<0?16:y>0?32:0);
    e.actions.push({kind:'joystick-direction',x,y,input:{joystickX:x*32767,joystickY:y*32767},press:receipt(),release:receipt(),
      before:state(0),held:state(mask,100+x*4,100+y*4),released:state(0,100+x*5,100+y*5),
      settled:Array.from({length:3},()=>state(0,100+x*5,100+y*5))});
  }
  e.neutral=state(0);return e;
}
test('directional joystick proof requires all eight native direction masks and persistent recentering',()=>{
  assertDirectionalJoystickEvidence(fixture(),options);
});
test('consumed observations use their own contract rather than the movement scheduling name',()=>{
  const e=fixture();
  for(const state of [e.start,e.neutral,...e.actions.flatMap(row=>[row.before,row.held,row.released,...row.settled])])state.input.inputSemantics='consumed';
  const synchronous={...options,movement:{...movement,inputBoundary:'synchronous-consumption'},observation:{inputSemantics:'consumed'}};
  assertDirectionalJoystickEvidence(e,synchronous);
  e.actions[0].held.input.inputSemantics='synchronous-consumption';
  assert.throws(()=>assertDirectionalJoystickEvidence(e,synchronous));
});
for(const [name,mutate] of Object.entries({
  'missing diagonal':e=>e.actions.pop(),
  'wrong native bit':e=>e.actions[0].held.input.value=128,
  'wrong physical direction':e=>e.actions[0].held.position.x=104,
  'drift on the other axis':e=>e.actions[0].held.position.y=101,
  'residual movement after recenter':e=>e.actions[0].settled[1].position.x++,
  'input returns after recenter':e=>e.actions[0].settled[2].input.value=64,
  'stale world observation':e=>e.actions[0].held.frames.world=e.actions[0].before.frames.world,
  'wrong input phase':e=>e.actions[0].held.input.sampleBoundary='before-update',
  'another player seat':e=>e.actions[0].held.localSeat=1,
  'command sent to old Runtime':e=>e.actions[0].press.epoch=1,
  'missing stable frames':e=>e.actions[0].settled.pop(),
  'unchanged position without game running':e=>e.actions[0].held.playing=false,
}))test(`directional joystick proof rejects ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertDirectionalJoystickEvidence(e,options));});
