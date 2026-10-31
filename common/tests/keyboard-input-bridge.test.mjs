import test from 'node:test';
import assert from 'node:assert/strict';
import {createBrowserKeyboard} from '../browser/directory-keyboard.mjs';
const identity={protocol:'eagler-touhou/1',game:'th08',epoch:7,origin:'https://launcher.invalid'};
function fixture(){
  const target={},events=[];let ready=true,clears=0;
  const keyboard=createBrowserKeyboard({send:(code,down)=>events.push([code,down]),hostContext:{target,...identity,ready:()=>ready,reset:()=>{clears++;keyboard.clear();}}});
  return {target,keyboard,events,get clears(){return clears;},setReady:v=>ready=v};
}
const key=(code,down=true)=>({...identity,command:'keyboard',code,key:code==='KeyZ'?'z':'',keyCode:0,location:0,down});

test('numeric and editing SDL queries follow the canonical key lifetime',()=>{
  for(const [code,scan] of [['Digit0',39],['Digit7',36],['Numpad0',98],['Numpad9',97],
    ['Minus',45],['Period',55],['NumpadSubtract',86],['NumpadDecimal',99],['Delete',76],['End',77]]){
    const f=fixture();f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,
      supports:()=>true,map:(e,down)=>[{code:e.code,key:'',keyCode:0,location:0,down}],
      clear:()=>[{code,key:'',keyCode:0,location:0,down:false}]};
    const bridge=f.target.__eaglerKeyboardInputBridge;
    f.keyboard.mapNative({code},true);assert.equal(bridge.scancodeState(scan),1,code);
    f.keyboard.clear();assert.equal(bridge.scancodeState(scan),0,code);
  }
});

test('local two-player words are synchronous, scoped to the advertised context and cleared at context changes',()=>{
  const target={},events=[];let mode='standard',clears=0;
  const keyboard=createBrowserKeyboard({hostContext:{target,...identity,ready:()=>true,profile:()=>mode},send:(code,down)=>events.push([code,down])});
  target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,map:()=>[],clear:()=>{clears++;return [];},sampleLocal:seat=>seat===0?16:2};
  const bridge=target.__eaglerKeyboardInputBridge;
  assert.equal(bridge.sampleLocal(0),-1);mode='th09-local';assert.equal(bridge.sampleLocal(0),16);assert.equal(bridge.sampleLocal(1),2);assert.equal(clears,1);
  mode='standard';assert.equal(bridge.sampleLocal(1),-1);assert.equal(clears,2);
  assert.deepEqual(events,[]);keyboard.clear();assert.equal(clears,3);
});
test('the mapped input is visible synchronously, with a separate canonical SDL query',()=>{
  const f=fixture();let source,profile;
  f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,
    map:(raw,down,s,p)=>{source=s;profile=p;return [{code:'KeyZ',key:'z',keyCode:90,location:0,down}];},
    clear:()=>[{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}]};
  assert.equal(f.keyboard.mapNative({code:'KeyK'},true),true);
  assert.deepEqual(f.events,[['KeyZ',true]]);assert.equal(source,'physical');assert.equal(profile,'standard');
  assert.equal(f.target.__eaglerKeyboardInputBridge.scancodeState(29),1);
  assert.equal(f.target.__eaglerKeyboardInputBridge.scancodeState(7),0);
  assert.equal(f.target.__eaglerKeyboardInputBridge.submit({...identity,command:'keyboard-clear'}),true);
  assert.equal(f.clears,1);assert.deepEqual(f.events,[['KeyZ',true],['KeyZ',false]]);
});
test('missing or stale mapping leaves the original native keyboard available',()=>{
  const f=fixture();assert.equal(f.keyboard.mapNative({code:'KeyZ'},true),false);
  f.keyboard.event({code:'KeyZ'},true);assert.deepEqual(f.events,[['KeyZ',true]]);
  f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,epoch:6,supports:()=>true,map:()=>{throw Error('stale callback');},clear:()=>[]};
  assert.equal(f.keyboard.mapNative({code:'KeyK'},true),false);
  assert.equal(f.target.__eaglerKeyboardInputBridge.submit(key('KeyZ')),false);
});
test('source identity and rejected input do not produce raw-key duplicates',()=>{
  const f=fixture();const observed=[];
  f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,
    supports:()=>true,
    map:(raw,down,source)=>{observed.push(source);return [];},clear:()=>[]};
  assert.equal(f.target.__eaglerKeyboardInputBridge.submit({...key('KeyK'),inputSource:'physical'}),true);
  assert.equal(f.target.__eaglerKeyboardInputBridge.submit(key('KeyC')),true);
  assert.deepEqual(observed,['physical','virtual']);assert.deepEqual(f.events,[]);
  f.setReady(false);assert.equal(f.keyboard.mapNative({code:'KeyK'},true),false);assert.equal(observed.length,2);
});
test('invalid mapper output throws before any state is published and is never retried',()=>{
  const f=fixture();f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,map:()=>[{code:'KeyZ',down:true}],clear:()=>[]};
  assert.throws(()=>f.keyboard.mapNative({code:'KeyK'},true),/Invalid canonical/);assert.deepEqual(f.events,[]);
});
test('gamepad snapshots preserve native ownership until a matching frontend port is bound',()=>{
  const f=fixture(),bridge=f.target.__eaglerGamepadInputBridge;
  assert.equal(bridge.sample(0),-1);
  let seat,clears=0,bits=128;
  const port={schema:'eagler-touhou/gamepad-input/1',...identity,enabled:()=>bits>=0,sample:s=>{seat=s;return bits;},clear:()=>{clears++;}};
  f.target.__eaglerGamepadMapping={...port,epoch:6};assert.equal(bridge.sample(0),-1);
  f.target.__eaglerGamepadMapping=port;assert.equal(bridge.sample(1),128);assert.equal(seat,1);assert.equal(bridge.owned(),true);
  f.keyboard.clear();assert.equal(clears,1);
  f.setReady(false);assert.equal(bridge.sample(0),0);assert.equal(clears,2);
  f.setReady(true);bits=-1;assert.equal(bridge.sample(0),-1);assert.equal(bridge.owned(),false);
  bits=4096;assert.equal(bridge.sample(0),0,'invalid actions cannot leak into game bits');
});


test('hosted physical rejection never creates a raw keyboard owner',()=>{
 for(const rejected of [{isComposing:true},{launcherOwnsFocus:true}])for(const code of ['ArrowUp','ShiftLeft','KeyZ']){
  const f=fixture();let state=false;
  f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,
   supports:()=>true,map:(e,down)=>{if(down&&e.altKey)return null;state=down;return [{code:e.code,key:'',keyCode:0,location:0,down}];},clear:()=>[]};
  assert.equal(f.keyboard.mapped(),true);
  assert.equal(f.keyboard.event({code,...rejected,inputSource:'physical'},true,'hosted'),false);
  assert.deepEqual(f.events,[],'rejected DOWN cannot be published through raw fallback');
  f.keyboard.event({code},false,'hosted');assert.equal(state,false);assert.ok(f.events.every(([,down])=>!down));
  f.keyboard.event({code},true,'hosted');f.keyboard.event({code},false,'hosted');
  assert.deepEqual(f.events.slice(-2),[[code,true],[code,false]],'the next ordinary press and release still work');
 }
});

test('bound but unavailable and unsupported mapped input never falls back to hosted raw keys',()=>{
 const f=fixture();f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,
  supports:()=>true,map:()=>null,clear:()=>[]};
 assert.equal(f.keyboard.event({code:'KeyZ',altKey:true},true,'hosted'),false);assert.deepEqual(f.events,[]);
 f.setReady(false);assert.equal(f.keyboard.event({code:'ArrowUp'},true,'hosted'),false);assert.deepEqual(f.events,[]);
 delete f.target.__eaglerKeyboardMapping;assert.equal(f.keyboard.mapped(),false);
 assert.equal(f.keyboard.event({code:'ArrowUp'},true,'hosted'),true);f.keyboard.event({code:'ArrowUp'},false,'hosted');
 assert.deepEqual(f.events,[['ArrowUp',true],['ArrowUp',false]],'an absent mapper retains standalone/legacy input');
});


test('attaching a mapper retires raw native and hosted owners before the next mapped UP',()=>{
 for(const source of ['native','hosted']){
  const f=fixture(),code='ArrowUp';f.keyboard.event({code},true,source);assert.deepEqual(f.events,[[code,true]]);
  f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,
   map:(e,down)=>[{code:e.code,key:'',keyCode:0,location:0,down}],clear:()=>[]};
  assert.equal(f.keyboard.mapped(),true);assert.deepEqual(f.events,[[code,true],[code,false]]);
  assert.equal(f.keyboard.mapNative({code},false),true);assert.deepEqual(f.events,[[code,true],[code,false]]);
 }
});

test('detach, stale identity and replacement retire the original mapped owner exactly once',()=>{
 for(const change of ['detach','stale','replace']){
  const f=fixture(),code='ShiftRight';let clears=0;
  const previous={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,
   map:(e,down)=>[{code,key:'',keyCode:0,location:2,down}],clear:()=>{clears++;return [{code,key:'',keyCode:0,location:2,down:false}];}};
  f.target.__eaglerKeyboardMapping=previous;f.keyboard.mapNative({code:'KeyL'},true);assert.deepEqual(f.events,[[code,true]]);
  if(change==='detach')delete f.target.__eaglerKeyboardMapping;
  if(change==='stale')f.target.__eaglerKeyboardMapping={...previous,epoch:identity.epoch+1};
  if(change==='replace')f.target.__eaglerKeyboardMapping={...previous,map:(e,down)=>[{code:'KeyX',key:'x',keyCode:88,location:0,down}],clear:()=>[]};
  f.keyboard.mapNative({code:'KeyL'},false);assert.equal(clears,1,change);assert.deepEqual(f.events,[[code,true],[code,false]],change);
  f.keyboard.mapped();assert.equal(clears,1,'stable ownership does not repeat retirement');
  assert.equal(f.target.__eaglerKeyboardInputBridge.keyDown(code),false);
 }
});

test('a held mapped or virtual key survives unrelated input while the same mapper stays bound',()=>{
 const f=fixture();let clears=0;
 f.target.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',...identity,supports:()=>true,
  map:(e,down)=>[{code:e.code,key:'',keyCode:0,location:0,down}],clear:()=>{clears++;return [];}};
 f.keyboard.mapNative({code:'KeyZ'},true);f.keyboard.event({code:'ShiftLeft'},true,'hosted');
 for(let n=0;n<120;++n){f.keyboard.mapNative({code:'ArrowUp'},true);f.keyboard.mapNative({code:'ArrowUp'},false);f.keyboard.mapped();}
 assert.equal(f.target.__eaglerKeyboardInputBridge.keyDown('KeyZ'),true);assert.equal(f.target.__eaglerKeyboardInputBridge.keyDown('ShiftLeft'),true);assert.equal(clears,0);
 f.keyboard.mapNative({code:'KeyZ'},false);f.keyboard.event({code:'ShiftLeft'},false,'hosted');
 assert.equal(f.target.__eaglerKeyboardInputBridge.keyDown('KeyZ'),false);assert.equal(f.target.__eaglerKeyboardInputBridge.keyDown('ShiftLeft'),false);
});
