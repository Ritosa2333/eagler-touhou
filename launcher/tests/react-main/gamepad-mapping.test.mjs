import test,{before} from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let createGamepadMappingModel,gamepadActionBits,createPlayerGamepad,createKeyboardPreferences,KeyboardMapping;
before(async()=>{
  const result=await build({stdin:{resolveDir:fileURLToPath(new URL('../../',import.meta.url)),contents:`export * from './src/launcher/gamepad-mapping.mts';export * from './src/launcher/keyboard-preferences.mts';export * from './src/launcher/keyboard-mapping.mts';export * from './app/services/player-gamepad.ts';`},bundle:true,write:false,format:'esm',platform:'node'});
  ({createGamepadMappingModel,gamepadActionBits,createPlayerGamepad,createKeyboardPreferences,KeyboardMapping}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64')));
});
function fixture(mapping='standard'){
  let pads=[{id:'Test pad',index:0,connected:true,mapping,buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]}];
  const saved=new Map(),storage={getItem:k=>saved.get(k),setItem:(k,v)=>saved.set(k,v)};
  const owner=createGamepadMappingModel({storage,getGamepads:()=>pads});owner.refresh();const key=owner.getSnapshot().devices[0].key;
  owner.select(0,key);owner.setEnabled(true);owner.sample(0,true);
  return {owner,key,storage,get pad(){return pads[0];},set pads(value){pads=value;},press(index,down=true){pads[0].buttons[index]={pressed:down,value:+down};}};
}
test('fresh Runtime sampling sees buttons synchronously and axis threshold is configurable',()=>{
  const f=fixture();f.press(0);assert.equal(f.owner.sample(0,true),1);f.press(0,false);
  f.pad.axes[0]=.4;assert.equal(f.owner.sample(0,true),0);f.pad.axes[0]=.6;assert.equal(f.owner.sample(0,true),128);
  f.owner.setThreshold(f.key,.8);f.pad.axes[0]=0;f.owner.sample(0,true);f.pad.axes[0]=.6;assert.equal(f.owner.sample(0,true),0);
});
test('unknown layout learns buttons and resting-negative trigger axes without assuming raw slots',()=>{
  const f=fixture('');f.press(0);assert.equal(f.owner.sample(0,true),0);f.press(0,false);
  f.owner.learn(f.key,'shoot');f.owner.refresh();f.press(7);f.owner.refresh();
  assert.deepEqual(f.owner.getSnapshot().profiles[f.key].bindings.shoot,[{kind:'button',index:7}]);
  assert.equal(f.owner.sample(0,true),0,'learning press cannot become game input');f.press(7,false);f.owner.sample(0,true);f.press(7);assert.equal(f.owner.sample(0,true),1);
  f.press(7,false);f.pad.axes[2]=-1;f.owner.learn(f.key,'bomb');f.owner.refresh();f.pad.axes[2]=.8;f.owner.refresh();
  assert.deepEqual(f.owner.getSnapshot().profiles[f.key].bindings.bomb,[{kind:'axis',index:2,rest:-1,value:1}]);
});
test('blur, spectator, disconnect and reconfiguration require neutral before rearming',()=>{
  const f=fixture();f.press(0);assert.equal(f.owner.sample(0,true),1);f.owner.suspend();assert.equal(f.owner.sample(0,true),0);
  f.press(0,false);f.owner.sample(0,true);f.press(0);assert.equal(f.owner.sample(0,true),1);
  assert.equal(f.owner.sample(0,false),0);assert.equal(f.owner.sample(0,true),0);
  f.pad.connected=false;assert.equal(f.owner.sample(0,true),0);f.pad.connected=true;assert.equal(f.owner.sample(0,true),0);
  f.press(0,false);f.owner.sample(0,true);f.press(0);assert.equal(f.owner.sample(0,true),1);
});
test('mapping persists per device layout and assignment cannot duplicate a physical device',()=>{
  const f=fixture();f.owner.bind(f.key,'shoot',[{kind:'button',index:3}]);
  const restored=createGamepadMappingModel({storage:f.storage,getGamepads:()=>[f.pad]});restored.sample(0,true);f.press(3);assert.equal(restored.sample(0,true),1);
  f.owner.select(1,f.key);assert.deepEqual(f.owner.getSnapshot().seats,['',f.key]);
  const raw={...f.pad,mapping:''};const different=createGamepadMappingModel({storage:f.storage,getGamepads:()=>[raw]});assert.equal(different.sample(0,true),0,'a raw layout must not reuse a standard profile');
});
test('keyboard-like Gamepad fallback remains outside physical gamepad mapping',()=>{
  const model=createGamepadMappingModel({storage:null,getGamepads:()=>[{id:'Bluetooth keyboard',index:0,connected:true,mapping:'standard',buttons:[],axes:[]}]});model.refresh();assert.deepEqual(model.getSnapshot().devices,[]);
});
test('disabled frontend mapping explicitly returns the original native owner',()=>{
  const f=fixture();f.owner.setEnabled(false);f.press(0);assert.equal(f.owner.sample(0,true),-1);
});
test('service validates Runtime identity and retires its old port on epoch replacement',()=>{
  const f=fixture(),window=new EventTarget(),document=Object.assign(new EventTarget(),{visibilityState:'visible',hasFocus:()=>true});
  const identity={protocol:'eagler-touhou/1',game:'th09',epoch:1,targetOrigin:'https://example.test',launched:true,spectator:false};
  const target={__eaglerGamepadInputBridge:{schema:'eagler-touhou/gamepad-input/1',...identity,origin:identity.targetOrigin}};
  let context={...identity,target};const listeners=new Set(),runtime={getInputContext:()=>context,subscribe:fn=>{listeners.add(fn);return()=>listeners.delete(fn);}};
  const service=createPlayerGamepad({runtime,model:f.owner,window,document,playerOpen:()=>true});const old=target.__eaglerGamepadMapping;
  old.sample(0);f.press(0);assert.equal(old.sample(0),1);window.dispatchEvent(new Event('blur'));assert.equal(old.sample(0),0);
  const replacement={__eaglerGamepadInputBridge:target.__eaglerGamepadInputBridge};context={...context,target:replacement};for(const fn of listeners)fn();assert.equal(target.__eaglerGamepadMapping,undefined);assert.ok(replacement.__eaglerGamepadMapping,'same-epoch window replacement must bind its own port');
  context={...context,epoch:2,target:{}};for(const fn of listeners)fn();assert.equal(replacement.__eaglerGamepadMapping,undefined);assert.equal(old.sample(0),0);service.dispose();
});
test('custom keyboard source overrides original function and survives reload',()=>{
  const saved=new Map(),storage={getItem:k=>saved.get(k),setItem:(k,v)=>saved.set(k,v)},preferences=createKeyboardPreferences(storage);
  preferences.bind('bomb','KeyZ');const restored=createKeyboardPreferences(storage),mapper=new KeyboardMapping(restored.getSnapshot),ctx={target:{},game:'th06',epoch:1,launched:true,spectator:false};
  const changes=mapper.map({type:'keydown',code:'KeyZ',key:'z',keyCode:90},true,'physical','standard',ctx);
  assert.deepEqual(changes.map(e=>e.code),['KeyX']);preferences.reset();assert.equal(preferences.getSnapshot().KeyK,'KeyZ');
});
test('Arrow keys and WASD presets preserve a custom layout across switching and reload',()=>{
  const saved=new Map(),storage={getItem:k=>saved.get(k),setItem:(k,v)=>saved.set(k,v)};
  const model=createKeyboardPreferences(storage);
  assert.equal(model.getLayoutSnapshot().selected,'wasd');
  model.selectLayout('arrows');assert.deepEqual(model.getSnapshot(),{});
  model.bind('shoot','KeyF');const id=model.saveLayout('我的键位');assert.ok(id);
  model.selectLayout('wasd');assert.equal(model.getSnapshot().KeyK,'KeyZ');
  model.selectLayout(id);assert.equal(model.getSnapshot().KeyF,'KeyZ');assert.equal(model.getSnapshot().KeyK,undefined);
  const reloaded=createKeyboardPreferences(storage);assert.equal(reloaded.getLayoutSnapshot().selected,id);assert.equal(reloaded.getSnapshot().KeyF,'KeyZ');
  reloaded.selectLayout('arrows');reloaded.selectLayout('custom');assert.equal(reloaded.getSnapshot().KeyF,'KeyZ');
  reloaded.reset();assert.deepEqual(reloaded.getSnapshot(),{},'reset follows the Arrow keys preset used as the custom layout base');
});
test('saved layouts are snapshots and legacy keyboard preferences migrate without losing aliases',()=>{
  const saved=new Map([['eagler-touhou-keyboard-mapping-v1',JSON.stringify({version:1,aliases:{KeyF:'KeyZ'}})]]),storage={getItem:k=>saved.get(k),setItem:(k,v)=>saved.set(k,v)};
  const model=createKeyboardPreferences(storage);assert.equal(model.getLayoutSnapshot().selected,'custom');
  const id=model.saveLayout('自用');model.bind('bomb','KeyG');model.selectLayout(id);assert.equal(model.getSnapshot().KeyG,undefined);
  model.deleteLayout(id);assert.equal(model.getLayoutSnapshot().saved.length,0);assert.equal(model.getSnapshot().KeyF,'KeyZ');
  assert.equal(model.selectLayout('missing'),false);
  model.selectLayout('arrows');assert.deepEqual(JSON.parse(saved.get('eagler-touhou-keyboard-mapping-v1')).aliases,{});
});
test('a failed layout write is explicit while the current page remains usable',()=>{
  const model=createKeyboardPreferences({getItem:()=>null,setItem(){throw Error('quota');}});
  model.bind('shoot','KeyF');assert.equal(model.getSnapshot().KeyF,'KeyZ');assert.equal(model.getLayoutSnapshot().persisted,false);assert.equal(model.saveLayout('自用'),null);
});
