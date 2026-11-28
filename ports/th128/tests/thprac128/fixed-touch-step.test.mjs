import test from'node:test';import assert from'node:assert/strict';import{functionSource}from'../web-input/play-harness.mjs';
for(const action of ['pause','restart'])test('whole current fixed-step caller stops before World when touch opens '+action,async()=>{
 let nativeCalls=0;const state={active:true,worldMode:true,paused:false,handoff:false,module:{_th128_world_tick(){nativeCalls++;throw Error('World tick ran after touch changed its owner');}}},menu={pauseOpen:false};
 state.webInput={sampleTouch(){if(action==='pause'){state.paused=true;menu.pauseOpen=true;}else{state.active=false;state.handoff=true;}}};
 const run=new Function('state','menu','pendingOriginalEndingRequest','async '+functionSource('advanceSerial')+';return advanceSerial;')(state,menu,()=>null);
 await run();assert.equal(nativeCalls,0);
});
