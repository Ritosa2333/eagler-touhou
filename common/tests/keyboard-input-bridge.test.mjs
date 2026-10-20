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
test('a host-reported repeat rebuilds native ownership after a runtime reset',()=>{
  const f=fixture();
  const physical=(code,down,repeat=false)=>f.keyboard.event({code,key:code==='KeyZ'?'z':'Shift',keyCode:code==='KeyZ'?90:16,
    location:code==='ShiftLeft'?1:0,repeat},down,'native');
  assert.equal(physical('KeyZ',true),true);assert.deepEqual(f.events,[['KeyZ',true]]);
  // The runtime-side reset (between-stage loading barrier, focus handshakes).
  f.keyboard.clear();assert.deepEqual(f.events,[['KeyZ',true],['KeyZ',false]]);
  // The browser still reports the key physically held via auto-repeat.
  assert.equal(physical('KeyZ',true,true),true,'host-attested repeat must rebuild');
  assert.deepEqual(f.events,[['KeyZ',true],['KeyZ',false],['KeyZ',true]]);
  // And the rebuilt owner releases normally once the key is let go.
  assert.equal(physical('KeyZ',false),true);
  assert.deepEqual(f.events,[['KeyZ',true],['KeyZ',false],['KeyZ',true],['KeyZ',false]]);
});
test('virtual and mapper repeats never rebuild cleared ownership',()=>{
  const f=fixture();
  assert.equal(f.keyboard.event({code:'KeyZ',key:'z',keyCode:90,location:0,repeat:true,inputSource:'virtual'},true,'hosted'),false);
  assert.deepEqual(f.events,[],'first virtual repeat with no owner reports nothing');
  f.keyboard.event({code:'KeyZ',key:'z',keyCode:90,location:0},true,'hosted');assert.deepEqual(f.events,[['KeyZ',true]]);
  f.keyboard.clear();
  assert.equal(f.keyboard.event({code:'KeyZ',key:'z',keyCode:90,location:0,repeat:true,inputSource:'virtual'},true,'hosted'),false);
  assert.deepEqual(f.events,[['KeyZ',true],['KeyZ',false]],'virtual repeat cannot re-arm cleared input');
});
