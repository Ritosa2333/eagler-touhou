import test from 'node:test';
import assert from 'node:assert/strict';
import {installGpuDiagnostics} from '../../sdl-runtime/gpu-diagnostics.mjs';
test('release modules cannot enable GPU experiments',()=>{
 const host={};assert.equal(installGpuDiagnostics({},null,{},host),null);
 assert.equal(host.th15GpuDiagnostics,undefined);
});
test('explicit profiles, bounded observations, and unavailable GPU identity',()=>{
 const calls=[],host={navigator:{userAgent:'fixture'}},handlers={};
 const core={_sdl_test_gpu_flags:n=>calls.push(['flags',n]),_sdl_test_capture_copies:n=>calls.push(['copies',n])};
 const canvas={getContext:name=>{assert.equal(name,'webgl2');return null;},addEventListener:(n,f)=>handlers[n]=f};
 const api=installGpuDiagnostics(core,canvas,{version:'test'},host);
 assert.deepEqual(calls,[],'installation must not change rendering');
 api.configure({copies:false,flush:true,resetState:true});
 assert.deepEqual(calls,[['flags',3],['copies',0]]);
 assert.throws(()=>api.configure({flush:1}));assert.throws(()=>api.configure({finish:true}));
 for(let i=0;i<100;i++)api.mark('observation '+i);
 handlers.webglcontextlost();const report=api.report();
 assert.equal(report.events.length,64);assert.equal(report.gpu,null);
 assert.equal(report.events.at(-1).kind,'context-lost');assert.equal(report.statistics,null);
 assert.deepEqual(report.profile,{copies:false,flush:true,resetState:true});
});
