import test from 'node:test';
import assert from 'node:assert/strict';
import {mountLauncherSession} from '../../site/runtime/launcher-session.mjs';
import {mountWebInput} from '../../site/runtime/web-input.mjs';
import {environment} from '../web-input/surface.mjs';
import {fakeWindow,envelope,flush} from './environment.mjs';
test('a parent keyboard lease clears without releasing a physical game key',async()=>{
 const env=environment(),bridge=fakeWindow(),held=new Set(),owner=Symbol('launcher');let input;
 env.window.addEventListener('keydown',event=>{if(input.acceptKeyDown(event))held.add(event.code);});
 env.window.addEventListener('keyup',event=>{if(input.acceptKeyUp(event))held.delete(event.code);});
 input=mountWebInput({...env});
 const session=mountLauncherSession({window:bridge.win,document:bridge.win.document,location:bridge.win.location,hooks:{
  configure:async()=>{},launch:async()=>{},keyboard({down,code}){const keys=new Set(input.readKeySource(owner));if(down)keys.add(code);else keys.delete(code);input.setKeySource(owner,[...keys]);},
  keyboardClear(){input.clearKeySource(owner);},exit(){input.clearKeySource(owner);}
 }});
 session.markReady();bridge.send(envelope('configure','cfg',{music:'ogg'}));await flush();bridge.send(envelope('launch','launch'));await flush();
 env.key('keydown','KeyZ');bridge.send(envelope('keyboard','down',{down:true,code:'KeyZ'}));await flush();bridge.send(envelope('keyboard-clear','clear'));await flush();
 assert(held.has('KeyZ'));env.key('keyup','KeyZ');assert.equal(held.size,0);await session.dispose();input.dispose();
});
test('the Launcher can hold and release both Ctrl codes through the web input owner',async()=>{
 const env=environment(),bridge=fakeWindow(),held=new Set(),owner=Symbol('launcher');
 const input=mountWebInput({...env});
 env.window.addEventListener('keydown',event=>{if(input.acceptKeyDown(event))held.add(event.code);});
 env.window.addEventListener('keyup',event=>{if(input.acceptKeyUp(event))held.delete(event.code);});
 const session=mountLauncherSession({window:bridge.win,hooks:{configure(){},launch(){},keyboard({down,code}){
  input.setKeySource(owner,down?[code]:[]);
 },keyboardClear(){input.clearKeySource(owner);},exit(){input.clearKeySource(owner);}}});
 session.markReady();bridge.send(envelope('configure','cfg',{music:'ogg'}));await flush();bridge.send(envelope('launch','launch'));await flush();
 try{
  for(const code of ['ControlLeft','ControlRight']){
   bridge.send(envelope('keyboard',code+'-down',{down:true,code}));await flush();assert.deepEqual([...held],[code]);
   bridge.send(envelope('keyboard',code+'-up',{down:false,code}));await flush();assert.deepEqual([...held],[]);
  }
 }finally{await session.dispose();input.dispose();}
});
test('parent exit cancels its own key without cancelling an independent touch pointer',async()=>{
 const env=environment(),bridge=fakeWindow(),held=new Set(),owner=Symbol('launcher');let input;
 env.window.addEventListener('keydown',event=>{if(input.acceptKeyDown(event))held.add(event.code);});
 env.window.addEventListener('keyup',event=>{if(input.acceptKeyUp(event))held.delete(event.code);});
 input=mountWebInput({...env});
 const session=mountLauncherSession({window:bridge.win,document:bridge.win.document,location:bridge.win.location,hooks:{
  configure:async()=>{},launch:async()=>{},keyboard({down,code}){input.setKeySource(owner,down?[code]:[]);},
  keyboardClear(){input.clearKeySource(owner);},exit(){input.clearKeySource(owner);}
 }});
 session.markReady();bridge.send(envelope('configure','cfg',{music:'ogg'}));await flush();bridge.send(envelope('launch','launch'));await flush();
 const button=env.find(input.layer,'data-web-action','freeze');env.emit(button,'pointerdown',1);
 bridge.send(envelope('keyboard','down',{down:true,code:'KeyX'}));await flush();bridge.send(envelope('exit','exit'));await flush();assert(held.has('KeyX'));
 env.emit(button,'pointerup',1);assert.equal(held.size,0);input.dispose();
});
