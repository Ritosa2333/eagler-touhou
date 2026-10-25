import test from 'node:test';
import assert from 'node:assert/strict';
import {runNativeForegroundOperation} from './runtime-desktop-lifecycle.mjs';
const pause=()=>new Promise(resolve=>setImmediate(resolve));
test('native startup restores only the verified owned window and preserves the actual operation result',async()=>{
  let ready=false,finish;const evidence={samples:[],restores:[]};
  const result=await runNativeForegroundOperation({evidence,operation:()=>new Promise(resolve=>{finish=resolve;}),
    readState:async()=>({owned:true,ready}),restore:async()=>{ready=true;return{ready:true,targetId:'owned'};},
    pause:async()=>{finish(42);await pause();}});
  assert.equal(result,42);assert.equal(evidence.restores.length,1);assert.equal(evidence.restores[0].targetId,'owned');
});
test('startup rejects a foreign native page without activating it',async()=>{
  let activated=0;
  await assert.rejects(runNativeForegroundOperation({evidence:{samples:[],restores:[]},operation:()=>new Promise(()=>{}),
    readState:async()=>({owned:false,ready:false}),restore:async()=>{activated++;},pause}),/original native page/);
  assert.equal(activated,0);
});
test('repeated foreground loss is bounded and cannot become a passing startup',async()=>{
  const evidence={samples:[],restores:[]};
  await assert.rejects(runNativeForegroundOperation({evidence,operation:()=>new Promise(()=>{}),readState:async()=>({owned:true,ready:false}),
    restore:async()=>({ready:true}),pause}),/repeatedly changed/);
  assert.equal(evidence.restores.length,2);
});
test('startup failure remains a failure after a successful foreground restore',async()=>{
  const evidence={samples:[],restores:[]};
  await assert.rejects(runNativeForegroundOperation({evidence,operation:async()=>{await pause();throw Error('native launch failed');},
    readState:async()=>({owned:true,ready:false}),restore:async()=>({ready:true}),pause}),/native launch failed/);
});
