import test,{before} from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
import {virtualGamepadLayouts,virtualGamepad,virtualButton,virtualNeutral} from '../fixtures/virtual-gamepads.mjs';
let createGamepadMappingModel,gamepadActions;
before(async()=>{const result=await build({entryPoints:[fileURLToPath(new URL('../../src/launcher/gamepad-mapping.mts',import.meta.url))],bundle:true,write:false,format:'esm',platform:'node'});({createGamepadMappingModel,gamepadActions}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64')));});
function fixture(layout,index=0){
  const pad=virtualGamepad(layout,index),saved=new Map(),storage={getItem:k=>saved.get(k)??null,setItem:(k,v)=>saved.set(k,v)};
  let pads=Array.from({length:index+1},(_,n)=>n===index?pad:null);
  const options={storage,getGamepads:()=>pads},model=createGamepadMappingModel(options);model.refresh();const key=model.getSnapshot().devices[0].key;model.select(0,key);model.setEnabled(true);model.sample(0,true);
  return {model,pad,key,options,setPads:next=>{pads=next;}};
}
for(const [name,type] of Object.entries(virtualGamepadLayouts))test(`${type.label}: discovery, independent actions, neutral gating and profile reload`,()=>{
  const f=fixture(name,3);assert.equal(f.model.getSnapshot().devices[0].index,3);
  if(type.mapping==='standard'){virtualButton(f.pad,0,1);assert.equal(f.model.sample(0,true),1);virtualNeutral(f.pad,name);assert.equal(f.model.sample(0,true),0);}
  else{virtualButton(f.pad,0,1);assert.equal(f.model.sample(0,true),0,'raw layouts must start unbound');virtualNeutral(f.pad,name);}
  // Each semantic action is learned on a different available input in turn.
  for(const [n,action] of gamepadActions.entries()){
    const button=n%f.pad.buttons.length;
    f.model.learn(f.key,action);f.model.refresh();virtualButton(f.pad,button,1);f.model.refresh();
    assert.deepEqual(f.model.getSnapshot().profiles[f.key].bindings[action],[{kind:'button',index:button}]);
    assert.equal(f.model.sample(0,true),0,'learning/held edits do not arm input');
    virtualNeutral(f.pad,name);f.model.sample(0,true);virtualButton(f.pad,button,1);
    assert.ok(f.model.sample(0,true)&(1<<n),action);virtualNeutral(f.pad,name);assert.equal(f.model.sample(0,true),0);
  }
  const restored=createGamepadMappingModel(f.options);assert.deepEqual(restored.getSnapshot().profiles[f.key],f.model.getSnapshot().profiles[f.key]);
  const shootButton=restored.getSnapshot().profiles[f.key].bindings.shoot[0].index;
  virtualButton(f.pad,shootButton,1);assert.equal(restored.sample(0,true),0,'a restored profile starts suspended');virtualNeutral(f.pad,name);restored.sample(0,true);virtualButton(f.pad,shootButton,1);assert.ok(restored.sample(0,true)&1);
});
test('two identical controllers retain their seat and custom profile when the lower-index device disappears',()=>{
  const a=virtualGamepad('xbox',0),b=virtualGamepad('xbox',2);let pads=[a,null,b];
  const model=createGamepadMappingModel({storage:null,getGamepads:()=>pads});model.refresh();const [left,right]=model.getSnapshot().devices.map(d=>d.key);
  model.select(0,left);model.select(1,right);model.setEnabled(true);model.bind(right,'shoot',[{kind:'button',index:3}]);model.sample(0,true);model.sample(1,true);
  a.connected=false;pads=[null,null,b];model.refresh();
  assert.equal(model.getSnapshot().devices[0].key,right,'the surviving device must not inherit the disconnected device identity');
  virtualButton(b,3,1);assert.equal(model.sample(0,true),0);assert.equal(model.sample(1,true),1);
  virtualButton(b,3,0);a.connected=true;pads=[a,null,b];model.refresh();model.sample(0,true);model.sample(1,true);
  virtualButton(a,0,1);virtualButton(b,3,1);assert.equal(model.sample(0,true),1);assert.equal(model.sample(1,true),1);
});
test('analog button triggers honor the selected threshold even when browser pressed is already true',()=>{
  const f=fixture('xbox');f.model.bind(f.key,'bomb',[{kind:'button',index:7}]);f.model.setThreshold(f.key,.8);f.model.sample(0,true);
  virtualButton(f.pad,7,.6);assert.equal(f.model.sample(0,true),0);
  virtualButton(f.pad,7,.9);assert.equal(f.model.sample(0,true),2);
  virtualButton(f.pad,7,.3);assert.equal(f.model.sample(0,true),0);
});
test('positive/negative sticks and resting-negative triggers learn independently and cannot rearm after disconnect while held',()=>{
  const f=fixture('axisTriggers');
  for(const [action,index,value] of [['left',0,-1],['right',0,1],['up',1,-1],['down',1,1],['focus',2,1],['bomb',5,1]]){
    f.model.learn(f.key,action);f.model.refresh();f.pad.axes[index]=value;f.model.refresh();virtualNeutral(f.pad,'axisTriggers');f.model.sample(0,true);
    f.pad.axes[index]=value;assert.ok(f.model.sample(0,true)&(1<<gamepadActions.indexOf(action)),action);virtualNeutral(f.pad,'axisTriggers');assert.equal(f.model.sample(0,true),0);
  }
  f.pad.axes[0]=-1;assert.equal(f.model.sample(0,true),64);f.pad.connected=false;assert.equal(f.model.sample(0,true),0);f.model.refresh();f.pad.connected=true;f.model.refresh();assert.equal(f.model.sample(0,true),0);
  virtualNeutral(f.pad,'axisTriggers');f.model.sample(0,true);f.pad.axes[0]=-1;assert.equal(f.model.sample(0,true),64);
});
test('four devices with holes keep assignments separate and unavailable APIs are safe',()=>{
  const pads=[virtualGamepad('xbox',0),null,virtualGamepad('dualsense',2),virtualGamepad('arcade',3),virtualGamepad('pro',4)];
  const model=createGamepadMappingModel({storage:null,getGamepads:()=>pads});model.refresh();const devices=model.getSnapshot().devices;
  model.select(0,devices[1].key);model.select(1,devices[3].key);model.setEnabled(true);model.sample(0,true);model.sample(1,true);
  virtualButton(pads[0],0,1);assert.equal(model.sample(0,true),0);assert.equal(model.sample(1,true),0);
  virtualButton(pads[2],0,1);virtualButton(pads[4],1,1);assert.equal(model.sample(0,true),1);assert.equal(model.sample(1,true),2);
  for(const getGamepads of [undefined,()=>{throw Error('Permission denied');},()=>[null]]){
    const unavailable=createGamepadMappingModel({storage:null,getGamepads});unavailable.refresh();unavailable.setEnabled(true);assert.equal(unavailable.sample(0,true),0);
  }
});
