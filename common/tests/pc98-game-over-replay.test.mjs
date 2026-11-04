import test from 'node:test';
import assert from 'node:assert/strict';
import {createExternalReplayHost as th02} from '../../ports/th02/web/replay/external-host.js';
import {createExternalReplayHost as th04} from '../../ports/th04/web/replay/external-host.js';

for(const [game,create] of [['th02',th02],['th04',th04]])test(`${game}: game over resumes after replay memory is captured`,async()=>{
  const original=WebAssembly.Suspending;
  WebAssembly.Suspending=class {constructor(fn){return fn;}};
  let resolveSnapshot,finished=false,resumed=false,observations=0;
  const snapshot=new Promise(resolve=>{resolveSnapshot=resolve;});
  const host=create({mode:'record',boundaryMode:'sleep',inspect:()=>snapshot,
    observe:()=>({active:true,stage:0,finish:++observations===1?'game-over':null}),
    onEnd:result=>{finished=result.timeline.reason==='game-over';}});
  try{
    const module={},imports={env:{initTimeoutSyncSleep:()=>{},__asyncjs__syncSleep:()=>{}}};
    host.attach(module,imports);imports.env.initTimeoutSyncSleep();
    const guest=imports.env.__asyncjs__syncSleep(0,true).then(()=>{resumed=true;});
    await new Promise(resolve=>setTimeout(resolve,30));
    assert.equal(host.status.held,true);assert.equal(resumed,false);
    resolveSnapshot({memoryHash:'snapshot'});await guest;
    assert.equal(finished,true);assert.equal(host.status.held,false);assert.equal(resumed,true);
    await imports.env.__asyncjs__syncSleep(0,true);
  }finally{host.dispose();WebAssembly.Suspending=original;}
});
