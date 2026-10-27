import test from 'node:test';
import assert from 'node:assert/strict';
import {TouchInput} from '../src/touch-input.mjs';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
const game=(instance='1:1',extra={})=>({key:'game:'+instance,kind:'game',instance,ready:true,x:0,y:20000,fast:4,slow:2,...extra});
const snapshot=(focusEnabled=true)=>({fireEnabled:true,focusEnabled,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0});
function input(mode='toggle-button'){const touch=new TouchInput();touch.configure({touchEnabled:true,touchFocusMode:mode,touchMovementMode:'touch'});touch.controls(snapshot(),game());return touch;}

test('toggle focus survives repeated player respawns without another host snapshot',()=>{
  const touch=input();
  for(const instance of ['1:1','1:2','1:3'])assert.equal(touch.sample(game(instance)).held&8,8);
  touch.controls(snapshot(false),game('1:3'));assert.equal(touch.sample(game('1:3')).held&8,0);
});
test('respawn clears stale drag targets and Bomb pulses while retaining toggle focus',()=>{
  const touch=input();touch.pointer({id:1,type:'down',x:100,y:200},game(),0);
  touch.pointer({id:1,type:'move',x:120,y:200},game(),10);touch.pending=2;
  const next=game('1:2');assert.deepEqual(touch.sample(next),{held:9,pressed:0,motion:null});
  assert.equal(touch.points.size,0);assert.equal(touch.primary,null);
  touch.pointer({id:2,type:'down',x:100,y:200},next,20);touch.pointer({id:2,type:'move',x:120,y:200},next,30);
  assert.equal(touch.sample(next).motion.x,1920,'new drag uses selected slow-speed ratio');
});
for(const kind of ['menu','replay'])test(`entering ${kind} still clears toggled focus`,()=>{
  const touch=input();touch.sample({key:kind+':1',kind,instance:'',ready:true});
  assert.equal(touch.focus,false);assert.equal(touch.sample(game('2:1')).held&8,0);
});
test('explicit cancellation and disabling touch still release toggled focus',()=>{
  const touch=input();touch.cancel();assert.equal(touch.sample(game()).held&8,0);
  touch.controls(snapshot(),game());touch.configure({touchEnabled:false});assert.equal(touch.focus,false);
});
test('pause and confirmation retain the toggle but release contacts, Bomb and axes',()=>{
  const touch=input();touch.pointer({id:1,type:'down',x:100,y:200},game(),0);
  touch.pending=2;touch.axes=[1,1];touch.cancel({preserveFocus:true});
  for(const scene of ['pause','pause-confirm','pause']){
    const state={key:scene,kind:'menu',paused:true,instance:'1:1',ready:false};
    assert.deepEqual(touch.sample(state),{held:0,pressed:0,motion:null});
    assert.equal(touch.focus,true);assert.equal(touch.points.size,0);
  }
  touch.cancel({preserveFocus:true});assert.deepEqual(touch.sample(game()),{held:9,pressed:0,motion:null});
});
test('leaving a pause for title or another run releases toggle focus',()=>{
  for(const state of [{key:'title',kind:'menu',instance:'',ready:true},game('2:1')]){
    const touch=input();touch.sample({key:'pause',kind:'menu',paused:true,instance:'1:1',ready:false});
    touch.sample(state);assert.equal(touch.focus,false);
  }
});
for(const mode of ['hold-button','two-finger'])test(`${mode} releases on pause even when preserving toggle focus`,()=>{
  const touch=input(mode);touch.cancel({preserveFocus:true});assert.equal(touch.focus,false);
});
for(const mode of ['hold-button','two-finger'])test(`${mode} does not acquire a persistent focus latch on respawn`,()=>{
  const touch=input(mode);assert.equal(touch.sample(game('1:2')).held&8,0);
});

const live=(instance='1:0',extra={})=>game(instance,{gestureScope:'1:1',...extra});
for(const mode of ['touch','touch-unlimited'])test(`${mode}: held contacts survive local death and rebase at respawn`,()=>{
 const touch=input('two-finger');touch.configure({touchMovementMode:mode});
 touch.pointer({id:'host:1',type:'down',x:100,y:200},live(),0);
 touch.pointer({id:'host:2',type:'down',x:180,y:200},live(),1);
 touch.pointer({id:'host:1',type:'move',x:120,y:200},live(),2);
 assert(touch.sample(live()).motion.x>0);
 touch.pending=2;touch.axes=[.5,.5];
 const dead=live('1:1',{ready:false,x:7000,y:45000});
 assert.equal(touch.sample(dead).motion,null);assert.equal(touch.pending,0);assert.equal(touch.points.size,2);
 touch.pointer({id:'host:1',type:'move',x:160,y:240},dead,3);
 const respawn=live('1:1',{x:0,y:50000});
 assert.deepEqual(touch.sample(respawn).motion,{mode:mode==='touch'?1:2,x:0,y:50000});
 touch.pointer({id:'host:1',type:'move',x:170,y:250},respawn,4);
 const gain=mode==='touch'?96:192;
 assert.deepEqual(touch.sample(respawn).motion,{mode:mode==='touch'?1:2,x:10*gain,y:50000+10*gain});
 assert.equal(touch.sample(respawn).held&8,8,'two fingers still own focus');
 touch.pointer({id:'host:2',type:'up',x:180,y:200},respawn,5);
 touch.pointer({id:'host:1',type:'cancel',x:170,y:250},respawn,6);
 assert.equal(touch.points.size,0);assert.equal(touch.sample(respawn).motion,null);
});

test('rollback across local death rebases the existing pointer without acquiring a new contact',()=>{
 const touch=input();touch.pointer({id:1,type:'down',x:100,y:200},live(),0);
 for(const [instance,x] of [['1:1',1000],['1:0',2000],['1:1',3000]]){
  const state=live(instance,{x,y:30000});const sample=touch.sample(state);
  assert.equal(touch.points.size,1);assert.equal(sample.motion.x,x);assert.equal(sample.motion.y,30000);
 }
 touch.sample(live('2:1',{gestureScope:'2:1'}));assert.equal(touch.points.size,0,'another stage does not reuse a drag target');
});

test('actual shell keeps gameplay gestures continuous through other deaths and spell boundaries',()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
 const from=source.indexOf('function touchState(){'),to=source.indexOf('\nfunction pointerPosition',from);
 let localMisses=0;const context={model:{state:{scene:'playing'}},ready:true,busy:false,session:{stage:6},runtimeOptions:{netplayPlayer:1},
  hud:()=>[0,10,200],readJSON:()=>({active:false}),core:{_th12_player_instance:()=>localMisses,_th12_player_state:()=>1,_th12_player_speed:()=>4},
  profile:{settings:{keyMap:{focus:'ShiftLeft'}}},keyDown:()=>false};
 runInNewContext(source.slice(from,to)+';globalThis.state=touchState;',context);
 const touch=input();const initial=context.state();touch.pointer({id:'host:1',type:'down',x:100,y:200},initial,0);
 touch.pointer({id:'host:1',type:'move',x:120,y:200},initial,1);const motion=touch.sample(initial).motion;
 for(let i=0;i<20;i++){
  // A different pilot's miss and an ECL phase reset leave the local identity intact.
  const next=context.state();assert.equal(next.key,initial.key);assert.deepEqual(touch.sample(next).motion,motion);assert.equal(touch.points.size,1);
 }
 localMisses++;const next=context.state();assert.notEqual(next.key,initial.key);
 assert.equal(touch.sample(next).motion.x,1280);assert.equal(touch.points.size,1);
});
