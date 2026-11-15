import {test, beforeAll as before} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let createTh09LocalInput,Th09LocalKeyboard,KeyboardMapping,createGamepadMappingModel,createPlayerGamepad,wasdKeyboardAliases;
before(async()=>{const result=await build({stdin:{resolveDir:fileURLToPath(new URL('../../',import.meta.url)),contents:`export * from './src/launcher/th09-local-input.mts';export * from './src/launcher/keyboard-mapping.mts';export * from './src/launcher/gamepad-mapping.mts';export * from './app/services/player-gamepad.ts';export {wasdKeyboardAliases} from './src/launcher/keyboard-preferences.mts';`},bundle:true,write:false,format:'esm',platform:'node'});
  ({createTh09LocalInput,Th09LocalKeyboard,KeyboardMapping,createGamepadMappingModel,createPlayerGamepad,wasdKeyboardAliases}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64')));});
const storage=()=>{const saved=new Map();return {getItem:k=>saved.get(k)??null,setItem:(k,v)=>saved.set(k,v)};};
const context=()=>({target:{},game:'th09',epoch:1,launched:true,ready:true,spectator:false,protocol:'eagler-touhou/1',targetOrigin:'https://local.test'});
const key=(code,repeat=false)=>({code,key:'',keyCode:0,location:0,type:'keydown',repeat});
test('original local keyboard halves keep independent Fire, diagonal movement and immediate held release',()=>{
  const model=createTh09LocalInput(storage()),owner=new Th09LocalKeyboard(model),ctx=context();model.setEnabled(true);
  owner.map(key('KeyZ'),true,ctx);owner.map(key('ArrowDown'),true,ctx);
  assert.equal(owner.sample(0,ctx),1);assert.equal(owner.sample(1,ctx),1);
  owner.map(key('KeyZ'),false,ctx);assert.equal(owner.sample(0,ctx),0);assert.equal(owner.sample(1,ctx),1);
  owner.map(key('ArrowDown'),false,ctx);owner.map(key('KeyR'),true,ctx);owner.map(key('Numpad9'),true,ctx);
  assert.equal(owner.sample(0,ctx),16|64);assert.equal(owner.sample(1,ctx),16|128);
  owner.map(key('KeyR'),false,ctx);owner.map(key('Numpad9'),false,ctx);assert.equal(owner.sample(0,ctx),0);assert.equal(owner.sample(1,ctx),0);
});
test('unseen short strokes survive one capture but observed holds and lifecycle cancellation do not gain a tail',()=>{
  const model=createTh09LocalInput(null),owner=new Th09LocalKeyboard(model),ctx=context();model.setEnabled(true);
  owner.map(key('ArrowDown'),true,ctx);owner.map(key('ArrowDown'),false,ctx);assert.equal(owner.sample(1,ctx),1);assert.equal(owner.sample(1,ctx),0);
  owner.map(key('KeyT'),true,ctx);owner.map(key('KeyT'),false,ctx);assert.equal(owner.sample(0,ctx),16);assert.equal(owner.sample(0,ctx),0);
  owner.map(key('KeyZ'),true,ctx);owner.sample(0,ctx);owner.clear();owner.map(key('KeyZ',true),true,ctx);assert.equal(owner.sample(0,ctx),0);
  owner.map(key('KeyZ'),false,ctx);owner.map(key('KeyZ'),true,ctx);assert.equal(owner.sample(0,ctx),1);
  assert.equal(owner.sample(0,{...ctx,epoch:2}),0);
});
test('per-player presets, conflict rejection, named layouts and device assignments survive reload separately from general keys',()=>{
  const saved=storage(),model=createTh09LocalInput(saved);model.setEnabled(true);
  assert.equal(model.selectLayout(0,'wasd'),null);assert.equal(model.selectLayout(1,'arrows'),null);
  assert.equal(model.bind(1,'shoot','KeyW'),'KeyW');assert.equal(model.setDevice(0,'gamepad','pad-a'),null);
  assert.equal(model.bind(1,'shoot','KeyW'),null);assert.equal(model.setDevice(0,'keyboard'),'KeyW');
  assert.equal(model.setDevice(1,'gamepad','pad-a'),'gamepad');assert.equal(model.setDevice(1,'gamepad','pad-b'),null);
  model.setDevice(0,'keyboard');model.bind(0,'shoot','KeyQ');const id=model.saveLayout(0,'左手布局');assert.ok(id.startsWith('saved-'));
  model.selectLayout(0,'original');assert.equal(model.selectLayout(0,id),null);assert.deepEqual(model.getSnapshot().players[0].bindings.shoot,['KeyQ']);
  const restored=createTh09LocalInput(saved);assert.equal(restored.getSnapshot().enabled,true);assert.equal(restored.getSnapshot().players[1].gamepad,'pad-b');assert.deepEqual(restored.getSnapshot().players[0].bindings.shoot,['KeyQ']);
});
test('local versus bypasses general WASD aliases and falls back to native when its own profile is disabled',()=>{
  const model=createTh09LocalInput(null),owner=new KeyboardMapping(()=>wasdKeyboardAliases,model),ctx=context();
  assert.equal(owner.map(key('KeyH'),true,'physical','th09-local',ctx),null);
  model.setEnabled(true);assert.deepEqual(owner.map(key('KeyH'),true,'physical','th09-local',ctx),[]);assert.equal(owner.sampleLocal(0,ctx),128);assert.equal(owner.sampleLocal(1,ctx),0);
  owner.clear();assert.equal(owner.map(key('KeyW'),true,'physical','standard',ctx)[0].code,'ArrowUp');
});

test('local WASD samples J/K and either Focus key without transferring input to the other player',()=>{
  const model=createTh09LocalInput(null),owner=new Th09LocalKeyboard(model),ctx=context();model.setEnabled(true);model.selectLayout(0,'wasd');
  for(const [code,bit]of [['KeyJ',1],['KeyK',2],['KeyL',4],['Space',4]]){
    owner.map(key(code),true,ctx);assert.equal(owner.sample(0,ctx),bit);assert.equal(owner.sample(1,ctx),0);
    owner.map(key(code),false,ctx);assert.equal(owner.sample(0,ctx),0);
  }
  owner.map(key('KeyL'),true,ctx);owner.map(key('Space'),true,ctx);owner.sample(0,ctx);
  owner.map(key('KeyL'),false,ctx);assert.equal(owner.sample(0,ctx),4);owner.map(key('Space'),false,ctx);assert.equal(owner.sample(0,ctx),0);
});

test('local preset migration preserves saved layouts and keeps old bindings if the new keys collide',()=>{
  for(const conflict of [false,true]){
    const saved=storage(),model=createTh09LocalInput(saved);model.setEnabled(true);model.selectLayout(0,'wasd');
    const key='eagler-touhou-th09-local-input-v1',value=JSON.parse(saved.getItem(key));
    Object.assign(value.players[0].bindings,{shoot:['KeyF'],bomb:['KeyG'],focus:['ShiftLeft']});
    value.players[0].saved=[{id:'saved-old',name:'旧布局',bindings:structuredClone(value.players[0].bindings)}];
    if(conflict){value.players[1].selected='custom';value.players[1].bindings.shoot=['KeyJ'];}
    saved.setItem(key,JSON.stringify(value));const restored=createTh09LocalInput(saved).getSnapshot();
    assert.equal(restored.enabled,true);assert.equal(restored.players[0].selected,conflict?'custom':'wasd');
    assert.deepEqual(restored.players[0].bindings.shoot,[conflict?'KeyF':'KeyJ']);
    assert.deepEqual(restored.players[0].saved[0].bindings.shoot,['KeyF']);assert.deepEqual(restored.players[1].bindings,value.players[1].bindings);
  }
});
test('two controllers and keyboard/controller pairs use local assignments without changing general controller settings',()=>{
  const pads=[0,1].map(index=>({id:'Pad '+index,index,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0]}));
  const gamepad=createGamepadMappingModel({storage:null,getGamepads:()=>pads});gamepad.refresh();const [a,b]=gamepad.getSnapshot().devices.map(d=>d.key);
  const local=createTh09LocalInput(null);local.setEnabled(true);local.setDevice(0,'gamepad',a);local.setDevice(1,'gamepad',b);
  const ctx=context(),win=new EventTarget(),doc=Object.assign(new EventTarget(),{visibilityState:'visible',hasFocus:()=>true});
  ctx.target.__eaglerGamepadInputBridge={schema:'eagler-touhou/gamepad-input/1',protocol:ctx.protocol,game:ctx.game,epoch:ctx.epoch,origin:ctx.targetOrigin};
  let mode='th09-local';ctx.target.__eaglerKeyboardInputBridge={profile:()=>mode};
  const service=createPlayerGamepad({runtime:{getInputContext:()=>ctx,subscribe:()=>()=>{}},model:gamepad,th09Local:local,window:win,document:doc,playerOpen:()=>true});
  const port=ctx.target.__eaglerGamepadMapping;assert.equal(port.enabled(),true);port.sample(0);port.sample(1);
  pads[0].buttons[0]={pressed:true,value:1};pads[1].buttons[1]={pressed:true,value:1};assert.equal(port.sample(0),1);assert.equal(port.sample(1),2);
  local.setDevice(0,'keyboard');assert.equal(port.sample(0),0);pads[1].buttons[1]={pressed:false,value:0};port.sample(1);pads[1].buttons[1]={pressed:true,value:1};assert.equal(port.sample(1),2);
  assert.equal(gamepad.getSnapshot().enabled,false);assert.deepEqual(gamepad.getSnapshot().seats,['','']);mode='standard';assert.equal(port.enabled(),false);assert.equal(port.sample(1),-1);
  mode='th09-local';local.setEnabled(false);assert.equal(port.enabled(),false);assert.equal(port.sample(0),-1);service.dispose();
});
