import test from 'node:test';
import assert from 'node:assert/strict';
import {commandHandler,installManagedShell,runtimeOptions} from '../web/managed.mjs';
import {userPath,validateUserFile} from '../web/storage.mjs';
import {encodeContent,decodeContent} from '../web/data.mjs';
import {inspectReplay} from '../web/replay.mjs';
import {createTouchInput} from '../web/input.mjs';
function mock(){const values=new Map(),calls=[];let running=false;const store={list:()=>[...values].map(([path,bytes])=>({path,size:bytes.length})),read:path=>{if(!values.has(path))throw Error('missing');return values.get(path);},write:(path,bytes)=>values.set(path,bytes),remove:path=>values.delete(path),sync:async()=>{}};return {calls,store,running:()=>running,resources:async r=>calls.push(['resources',r]),languagePack:async p=>calls.push(['pack',p]),configure:async value=>calls.push(['configure',value]),launch:async()=>{running=true;},sync:async()=>{},key:m=>calls.push(['key',m]),clearKeyboard(){},cancelTouch(){},touch:m=>calls.push(['touch',m]),pointer:m=>calls.push(['pointer',m]),practicePointer(){}};}
test('managed protocol: authenticate source, origin and navigation epoch; ordered ACKs',async()=>{
 const api=mock(),listeners=new Map(),messages=[],parent={postMessage:m=>messages.push(m)},environment={parent,location:{href:'https://example.test/runtime/th05.html?runtimeEpoch=9',origin:'https://example.test'},addEventListener:(k,v)=>listeners.set(k,v),removeEventListener:k=>listeners.delete(k)};
 const shell=installManagedShell(api,environment);shell.ready();assert.equal(messages[0].saveRoot,'/savesth05');
 const payload={protocol:'eagler-touhou/1',game:'th05',epoch:9,command:'configure',request:'a',music:'none',options:{thpracEnabled:false}};
 const send=(data,source=parent,origin='https://example.test')=>listeners.get('message')({data,source,origin});
 send({...payload,epoch:8});send(payload,{});send(payload,parent,'https://foreign.test');await shell.idle();assert.equal(api.calls.length,0);
 send(payload);send({protocol:payload.protocol,game:'th05',epoch:9,command:'launch',request:'b'});await shell.idle();assert.deepEqual(messages.slice(1).map(m=>[m.request,m.ok]),[['a',true],['b',true]]);
 send({...payload,request:'c'});await shell.idle();assert.equal(messages.at(-1).ok,false);shell.close();assert.equal(listeners.has('message'),false);
});
test('managed protocol: atomic content identity and file traversal boundaries',async()=>{
 const bytes=await encodeContent(new Map([['kaiki1.dat',new Uint8Array([1])],['kaiki2.dat',new Uint8Array([2])],['zun.com',new Uint8Array([3])],['sfx/effects.pcm',new Uint8Array([4])]]));assert.equal((await decodeContent(bytes.buffer)).size,4);bytes[bytes.length-1]^=1;await assert.rejects(decodeContent(bytes.buffer),/digest/);
 for(const path of ['../score.dat','/savesth06/score.dat','replay/../score.dat','replay/../../a.rpy','replay\\a.rpy','/data/kaiki1.dat','replay/a/b/c.rpy'])assert.throws(()=>userPath(path));assert.equal(userPath('/savesth05/replay/archive/replay00.rpy'),'replay/archive/replay00.rpy');
 const api=mock(),handle=commandHandler(api);await handle({command:'write',path:'score.dat',bytes:Array(1500).fill(0)});assert.equal((await handle({command:'read',path:'score.dat'})).bytes.length,1500);await assert.rejects(handle({command:'write',path:'score.dat',bytes:[0]}),/score/);await handle({command:'remove',path:'score.dat'});assert.deepEqual((await handle({command:'list'})).files,[]);
});
test('managed practice: one binary forwards disabled/enabled and independent interface language',async()=>{
 const api=mock(),handle=commandHandler(api);for(const enabled of [false,true,false])await handle({command:'configure',music:'none',language:'ja',options:{thpracEnabled:enabled,thpracLocale:'en-US'}});
 assert.deepEqual(api.calls.filter(c=>c[0]==='configure').map(c=>[c[1].options.thpracEnabled,c[1].options.thpracLocale]),[[false,'en-US'],[true,'en-US'],[false,'en-US']]);
 await handle({command:'configure',music:'none',runtimePack:{language:'lang_zh-hans'}});assert.equal(api.calls.at(-1)[1].language,'lang_zh-hans');
 for(const invalid of [{touchSensitivity:99},{touchSensitivity:301},{thpracEnabled:'yes'},{netplayPlayer:1},{thpracLocale:'fr-FR'}])assert.throws(()=>runtimeOptions(invalid));
});
test('managed touch: sensitivity, independent key owners, serial edges and cancellation',()=>{
 const sent=[],motions=[],options={touchEnabled:true,touchSensitivity:200,touchMovementMode:'touch-unlimited',touchFocusMode:'two-finger'};
 const touch=createTouchInput({send:(...v)=>sent.push(v),position:()=>({x:224,y:376}),options:()=>options,move:(...v)=>motions.push(v)});
 touch.pointer({type:'down',id:1,x:50,y:50});touch.pointer({type:'move',id:1,x:60,y:45});touch.tick();assert.deepEqual(motions,[[20,-10,true]]);
 const state={fireEnabled:true,focusEnabled:false,joystickX:0,joystickY:0,bombSerial:1,escapeSerial:0};touch.controls(state);touch.tick();touch.controls(state);touch.tick();touch.tick();assert.equal(sent.filter(v=>v[0]==='KeyX'&&v[1]).length,1);touch.cancel();assert.ok(sent.some(v=>v[0]==='KeyZ'&&!v[1]));
});

test('managed touch: menu tap/swipe/back and free-direction movement',()=>{
 const keys=[],motions=[],options={touchEnabled:true,touchMovementMode:'joystick-free',touchFocusMode:'hold-button',touchSensitivity:150};let scene=0;
 const touch=createTouchInput({send:(...v)=>keys.push(v),position:()=>({x:224,y:376}),options:()=>options,scene:()=>scene,move:(...v)=>motions.push(v)});
 touch.pointer({type:'down',id:1,x:30,y:30});touch.pointer({type:'up',id:1,x:30,y:30});touch.tick();touch.tick();assert.ok(keys.some(([key,down])=>key==='KeyZ'&&down));
 touch.pointer({type:'down',id:1,x:30,y:30});touch.pointer({type:'move',id:1,x:30,y:80});touch.tick();touch.pointer({type:'up',id:1,x:30,y:80});touch.tick();assert.ok(keys.some(([key,down])=>key==='ArrowDown'&&down));
 touch.pointer({type:'down',id:1,x:30,y:30});touch.pointer({type:'down',id:2,x:60,y:30});touch.pointer({type:'up',id:1,x:30,y:30});touch.pointer({type:'up',id:2,x:60,y:30});touch.tick();assert.ok(keys.some(([key,down])=>key==='Escape'&&down));
 touch.cancel();scene=4;keys.length=0;touch.controls({fireEnabled:false,focusEnabled:false,joystickX:20000,joystickY:10000,bombSerial:0,escapeSerial:0});touch.tick();assert.equal(motions.length,1);assert.equal(motions[0][0]/motions[0][1],2);assert.equal(motions[0][2],false);assert.deepEqual(keys,[]);
});
test('managed protocol: malformed input and launch ordering return failures',async()=>{
 const handle=commandHandler(mock());for(const value of [{command:'launch'},{command:'configure',music:'midi'},{command:'keyboard',code:'KeyZ',down:1},{command:'direct-touch',type:'move',id:NaN,x:0,y:0},{command:'touch-controls'},{command:'unknown'}])await assert.rejects(handle(value));
});
test('managed Replay: validate the native frame stream and touch journal',()=>{
 const bytes=new Uint8Array(340+4+4+8+28+12),view=new DataView(bytes.buffer);bytes.set(new TextEncoder().encode('RPY '));view.setInt32(4,1,true);view.setInt32(36,3,true);view.setInt32(40,3,true);view.setInt32(44,1,true);bytes[53]=1;view.setInt32(340,2,true);let at=348;bytes.set(new TextEncoder().encode('TCH5'),at);view.setUint32(at+4,1,true);at+=8;view.setUint32(at,1,true);at+=4;view.setUint32(at,1,true);view.setFloat32(at+4,16,true);view.setFloat32(at+8,-32,true);
 assert.equal(inspectReplay(bytes).touch,true);validateUserFile('replay/replay00.rpy',bytes);view.setFloat32(at+8,Infinity,true);assert.throws(()=>inspectReplay(bytes),/touch motion/);assert.throws(()=>inspectReplay(bytes.subarray(0,344)),/Truncated/);
});
