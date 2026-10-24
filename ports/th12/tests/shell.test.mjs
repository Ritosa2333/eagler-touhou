import test from 'node:test';import assert from 'node:assert/strict';
import {createCommandHandler,installShell} from '../src/shell.mjs';
function mock(){
  const calls=[],state={running:false};
  const api={running:()=>state.running,music:x=>calls.push(['music',x]),start:async()=>calls.push(['start']),key:(...args)=>calls.push(['key',...args]),keyboardClear:()=>calls.push(['keyboardClear']),touchCancel:()=>calls.push(['touchCancel']),touch:()=>{},pointer:()=>{},store:{list:async()=>[],read:async()=>new Uint8Array([7]),write:async()=>{},remove:async()=>calls.push(['remove']),sync:async()=>calls.push(['sync'])}};
  return {api,calls,state,handle:createCommandHandler(api)};
}
test('preview launch requires successful configuration and rejects concurrent running launches',async()=>{
  const m=mock();await assert.rejects(m.handle({command:'launch'}));
  await assert.rejects(m.handle({command:'configure',music:'none',resources:[{}]}));assert.deepEqual(m.calls,[]);
  await assert.rejects(m.handle({command:'configure',music:'none',options:{touchEnabled:true}}));assert.deepEqual(m.calls,[]);
  await m.handle({command:'configure',music:'none'});await m.handle({command:'launch'});
  assert.deepEqual(m.calls,[['music',false],['start']]);m.state.running=true;
  await assert.rejects(m.handle({command:'launch'}));await assert.rejects(m.handle({command:'configure',music:'ogg'}));
});
test('shell routes Escape and keyboard/touch clear independently, and syncs removal',async()=>{
  const m=mock();await m.handle({command:'keyboard',code:'Escape',down:true});await m.handle({command:'keyboard-clear'});await m.handle({command:'touch-cancel'});await m.handle({command:'remove',path:'/savesth12/replay/th12_test.rpy'});
  assert.deepEqual(m.calls,[['key','Escape',true],['keyboardClear'],['touchCancel'],['remove'],['sync']]);
  await assert.rejects(m.handle({command:'keyboard',code:'KeyZ',down:'false'}));await assert.rejects(m.handle({command:'direct-touch',type:'move',id:1,x:NaN,y:0}));
});
test('iframe bridge rejects stale epoch, foreign origin and wrong source without side effects',async()=>{
  const m=mock(),messages=[];let listener;
  const environment={location:{href:'http://localhost/game?runtimeEpoch=12',origin:'http://localhost'},parent:{postMessage:x=>messages.push(x)},addEventListener:(type,cb)=>{listener=cb;}};
  installShell(m.api,environment);assert.equal(messages[0].event,'ready');
  const data={protocol:'eagler-touhou/1',game:'th12',epoch:12,command:'keyboard',code:'KeyZ',down:true,request:'key'};
  for(const changes of [{data:{...data,epoch:11}},{origin:'http://other'},{source:{}}])await listener({data,source:environment.parent,origin:environment.location.origin,...changes});
  assert.deepEqual(m.calls,[]);assert.equal(messages.length,1);
  await listener({data,source:environment.parent,origin:environment.location.origin});assert.deepEqual(m.calls,[['key','KeyZ',true]]);assert.equal(messages[1].ok,true);assert.equal(messages[1].epoch,12);
});
