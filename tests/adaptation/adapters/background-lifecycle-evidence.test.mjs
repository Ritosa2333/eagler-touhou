import test from 'node:test';
import assert from 'node:assert/strict';
import {assertPointerCancellation,assertHeldTouchLifecycle,assertGamepadReconnect} from './background-lifecycle-evidence.mjs';
const product={game:'th08',variant:'normal'};
function pointerFixture(){
  let frame=0;const state=(x,y)=>({localSeat:0,playing:true,position:{x,y},input:{value:0},capabilities:{restartFrame:'world'},frames:{world:frame++}});
  const before=state(100,100),moving=Array.from({length:4},(_,i)=>state(101+i,99-i)),cancelled=Array.from({length:4},()=>state(104,96));
  const commands=['pointerdown','pointermove','pointercancel'].map((type,i)=>({source:'production-dom-pointer',fields:{control:'directSurface',type,id:9812,x:i?.7:.5,y:i?.7:.75},
    barrier:{protocol:'eagler-touhou/1',game:'th08',epoch:1,request:String(i),ok:true}}));
  return{epoch:1,fixture:{game:'th08',epoch:1},clock:{mode:'native-manual'},before,moving,cancelled,commands};
}
const options={product,isGameplayReady:state=>state.playing};
test('pointer cancellation proves actual movement followed by four later stationary native ticks',()=>assertPointerCancellation(pointerFixture(),options));
for(const [name,mutate] of Object.entries({
  'no effective movement':e=>e.moving.forEach(row=>row.position={...e.before.position}),
  'movement after cancellation':e=>e.cancelled[0].position.x++,
  'stale final sample':e=>e.cancelled[3].frames.world--,
  'wrong DOM event':e=>e.commands[2].fields.type='cancel',
  'missing ordered acknowledgement':e=>e.commands[2].barrier.ok=false,
  'another instance acknowledgement':e=>e.commands[2].barrier.epoch=2,
  'scene not ready':e=>e.moving[0].playing=false,
}))test(`pointer lifecycle rejects ${name}`,()=>{const e=pointerFixture();mutate(e);assert.throws(()=>assertPointerCancellation(e,options));});
function touchFixture(){
  const neutral={focusEnabled:false,heldFire:false,functionDown:false,joystickX:0,joystickY:0};
  return{fixture:{game:'th08',epoch:1,focus:'hold-button'},down:{fields:{control:'focus',type:'pointerdown'},
    barrier:{protocol:'eagler-touhou/1',game:'th08',epoch:1,request:'focus',ok:true}},held:{input:{value:4}},
    before:{focusEnabled:true},hidden:{...neutral},restored:{...neutral}};
}
const touchOptions={game:'th08',epoch:1,focusMask:4};
test('held touch must clear in the native background interval and remain cleared on return',()=>assertHeldTouchLifecycle(touchFixture(),touchOptions));
for(const [name,mutate] of Object.entries({
  'focus never reached native':e=>e.held.input.value=0,
  'focus remains while hidden':e=>e.hidden.focusEnabled=true,
  'focus returns on restore':e=>e.restored.focusEnabled=true,
  'fire owner remains':e=>e.restored.heldFire=true,
  'function owner remains':e=>e.hidden.functionDown=true,
  'joystick owner remains':e=>e.restored.joystickX=1,
}))test(`held touch lifecycle rejects ${name}`,()=>{const e=touchFixture();mutate(e);assert.throws(()=>assertHeldTouchLifecycle(e,touchOptions));});
function reconnectFixture(){
  const state=(value,x)=>({input:{value},position:{x,y:100}}),receipt=bits=>({epoch:1,bits});
  return{epoch:1,neutral:receipt(0),before:state(0,100),left:receipt(64),held:state(64,96),
    disconnect:{...receipt(0),device:{connected:false}},released:state(0,92),reconnectHeld:{...receipt(0),device:{connected:true,buttons:{14:1}}},
    suppressed:state(0,92),rearm:receipt(0),right:receipt(128),resumed:state(128,96),release:receipt(0),final:state(0,100)};
}
const reconnectOptions={movement:{directionMasks:{ArrowLeft:64,ArrowRight:128}}};
test('gamepad reconnect requires release, held suppression and working input after a neutral sample',()=>assertGamepadReconnect(reconnectFixture(),reconnectOptions));
for(const [name,mutate] of Object.entries({
  'device never disconnected':e=>e.disconnect.device.connected=true,
  'input survives disconnect':e=>e.released.input.value=64,
  'held reconnect activates':e=>e.reconnectHeld.bits=64,
  'movement survives reconnect':e=>e.suppressed.position.x--,
  'reconnect never works':e=>e.resumed.input.value=0,
  'post reconnect stuck input':e=>e.final.input.value=128,
  'receipt from another instance':e=>e.right.epoch=2,
}))test(`gamepad reconnect rejects ${name}`,()=>{const e=reconnectFixture();mutate(e);assert.throws(()=>assertGamepadReconnect(e,reconnectOptions));});
