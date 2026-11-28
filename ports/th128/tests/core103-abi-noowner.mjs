import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {pathToFileURL} from 'node:url';
const home=path.resolve(process.argv[2]),build=JSON.parse(fs.readFileSync(path.join(home,'result.json'),'utf8'));
const request=JSON.parse(fs.readFileSync(path.join(home,'request.json'),'utf8'));
assert.equal(build.passed,true);assert.equal(build.coreOrdinal,103);
const messages=[],{default:create}=await import(pathToFileURL(path.join(home,'run/runtime/core.mjs')));
const module=await create({wasmBinary:fs.readFileSync(path.join(home,'run/runtime/core.wasm')),print:s=>messages.push(['stdout',s]),printErr:s=>messages.push(['stderr',s])});
for(const name of request.exports)assert.equal(typeof module[name],'function','actual produced runtime missing export '+name);
assert(module.HEAPU8 instanceof Uint8Array);
const pointer=module._malloc(32);assert(pointer>0&&pointer+32<=module.HEAPU8.length);
try{const expected=Uint8Array.from({length:32},(_,n)=>n*7&255);module.HEAPU8.set(expected,pointer);assert.deepEqual(module.HEAPU8.slice(pointer,pointer+32),expected);}finally{module._free(pointer);}
const owner=JSON.parse(module.UTF8ToString(module._th128_world_normal_postrun_state()));
assert.equal(owner.present,false);assert.equal(owner.generation,0);
assert.equal(module._th128_world_pause_active(),0);
const rejects=[
 ()=>module._th128_world_normal_postrun_main_prepare(0),
 ()=>module._th128_world_normal_postrun_main_commit(0),
 ()=>module._th128_world_normal_postrun_profile_ack(0,0,0),
 ()=>module._th128_world_normal_postrun_main_ack(0,0,0),
 ()=>module._th128_world_pause_commit(2)
];
for(const call of rejects)assert.equal(call(),0,'unowned native action must reject');
const after=JSON.parse(module.UTF8ToString(module._th128_world_normal_postrun_state()));assert.equal(after.present,false);assert.equal(after.generation,0);
assert.deepEqual(messages,[]);
console.log(JSON.stringify({passed:true,coreOrdinal:103,generation:build.generation,actualExports:request.exports.length,Factories:1,WorldStarts:0,
 unownedActionsRejected:rejects.length,postrunOwnersMinted:0,browser:0,storageWrites:0,sourceStarts:0,scope:'actual produced103instantiate/export/memory and noownerrejection only;428componentbehavior631 separatelytested,notoriginalgameplay'}));
