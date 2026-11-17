import test from'node:test';import assert from'node:assert/strict';import{functionSource}from'../web-input/play-harness.mjs';
test('practice identity excludes unrelated title warmup but retains gameplay bytes',async()=>{
 const module={},builds=new WeakMap([[module,{artifacts:{'core.wasm':{sha256:'a'.repeat(64)}}}]]),fn=new Function('checkpointModuleBuilds','crypto','TextEncoder','async '+functionSource('thpracReplayResourceIdentity')+';return thpracReplayResourceIdentity')(builds,crypto,TextEncoder);
 const base=new Map([['runtime/st_a1_1.ecl',Promise.resolve(new Uint8Array([1,2,3]))],['runtime/pl00.anm',Promise.resolve(new Uint8Array([4,5,6]))]]),warm=new Map([...base,['runtime/title.anm',Promise.resolve(new Uint8Array([7]))],['runtime/title_v.anm',Promise.resolve(new Uint8Array([8]))]]);
 assert.deepEqual(await fn(module,base),await fn(module,warm));const changed=new Map(base);changed.set('runtime/pl00.anm',Promise.resolve(new Uint8Array([4,5,7])));assert.notDeepEqual(await fn(module,base),await fn(module,changed));
});
test('late runtime readiness does not consume route practice opening token',()=>{
 let opens=0;const env={thpracRouteShown:false,thpracSettings:{enabled:true},practiceMenu:{open(){++opens;return false;}},menu:{launching:false},state:{launcherExited:false,stageCatalog:{},module:{},handoff:false,pendingCarryToken:0,nativePausePending:null},thprac128RuntimeSupported:()=>false,find:()=>({disabled:false})};
 const fn=new Function(...Object.keys(env),functionSource('syncThpracRouteMenu')+';return syncThpracRouteMenu')(...Object.values(env));fn({page:2,ready:true});fn({page:2,ready:true});assert.equal(opens,0);
});
