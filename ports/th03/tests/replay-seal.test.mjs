import assert from 'node:assert/strict';
import {createExternalReplayHost} from '../web/replay/external-host.js';

// Exercise the real JSPI continuation with a stand-in for Node versions that
// do not yet expose WebAssembly.Suspending.
const original=WebAssembly.Suspending;
WebAssembly.Suspending=class {constructor(fn){return fn;}};
try{
  let finish=false,completed=false,releaseSnapshot;
  const snapshot=new Promise(resolve=>{releaseSnapshot=resolve;});
  const host=createExternalReplayHost({mode:'record',observe:()=>({active:true,currentActive:true,finish:finish?'clear':null}),
    inspect:()=>snapshot,onEnd:()=>{completed=true;}});
  const imports={env:{initTimeoutSyncSleep:()=>{},__asyncjs__syncSleep:()=>{}}},module={};
  host.attach(module,imports);imports.env.initTimeoutSyncSleep();
  await imports.env.__asyncjs__syncSleep(0,true);
  finish=true;
  let resumed=false;
  const continuation=imports.env.__asyncjs__syncSleep(0,true).then(()=>{resumed=true;});
  await new Promise(resolve=>setTimeout(resolve,20));
  assert.equal(resumed,false,'the game must wait for the replay snapshot');
  releaseSnapshot({});
  await Promise.race([continuation,new Promise((_,reject)=>setTimeout(()=>reject(Error('game remained paused after replay snapshot')),1000))]);
  assert.equal(completed,true);assert.equal(host.status.held,false);
  host.dispose();
  console.log('PASS TH03 automatic replay sealing releases the game');
}finally{WebAssembly.Suspending=original;}
