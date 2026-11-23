import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createHash} from 'node:crypto';
import {readManagedProgram,readManagedProgramBytes} from '../runtime/managed-program.mjs';

const gen='a'.repeat(64),coreId='1'.repeat(64);
const dir=`http://127.0.0.1:4900/runtime/th128/multiplayer/${gen}/`,page=dir+'th128.html?runtimeVariant=multiplayer&managedData=1';
const data=new TextEncoder().encode('program bytes');
const pin=path=>({path,bytes:data.length,sha256:createHash('sha256').update(data).digest('hex')});
function receipt(){return {schema:'th128mp-program/1',coreGeneration:coreId,files:{
  core:pin(`runtime/builds/${coreId}/core.mjs`),wasm:pin(`runtime/builds/${coreId}/core.wasm`),type9:pin('runtime/bullet-type9-profile.json'),
  peer:pin('runtime/transport/author-peer.mjs'),peerWasm:pin('runtime/transport/author-peer.wasm'),
}};}
test('managed MP code receipts are bound to the room Runtime generation rather than the single-player Core generation',async()=>{
  const request=[];
  const fetchImpl=async (url,init)=>{request.push(url.href);assert.equal(init.redirect,'error');return new Response(JSON.stringify(receipt()));};
  const program=await readManagedProgram(page,{fetchImpl});
  assert.equal(program.generation,gen);assert.notEqual(program.generation,coreId);
  assert.equal(program.core.url,new URL(receipt().files.core.path,dir).pathname);
  assert.equal(program.peerWasm.url,new URL(receipt().files.peerWasm.path,dir).pathname);
  assert.deepEqual(request,[dir+'runtime/program.json']);assert.ok(Object.isFrozen(program));assert.ok(Object.isFrozen(program.peerWasm));
  const verified=await readManagedProgramBytes(program.peerWasm,page,{fetchImpl:async url=>{assert.equal(url.origin,new URL(dir).origin);return new Response(data);}});
  assert.deepEqual(verified,data);
  await assert.rejects(readManagedProgramBytes(program.peerWasm,page,{fetchImpl:async()=>new Response(new Uint8Array(data.length))}),/SHA-256/);
});

test('managed MP receipts cannot accept aliases, foreign builds, raw URLs or a truncated binary',async()=>{
  const spec=receipt(),reply=body=>async()=>new Response(JSON.stringify(body));
  await assert.rejects(readManagedProgram(page.replace(gen,'current'),{fetchImpl:reply(spec)}),/generation/);
  await assert.rejects(readManagedProgram(page.replace('multiplayer&','normal&'),{fetchImpl:reply(spec)}),/multiplayer/);
  for(const path of ['//example.com/core.mjs','runtime/transport/../other.mjs',`runtime/builds/${'f'.repeat(64)}/core.mjs`]){
    spec.files.core.path=path;
    await assert.rejects(readManagedProgram(page,{fetchImpl:reply(spec)}),/program path/);
  }
  const program=await readManagedProgram(page,{fetchImpl:reply(receipt())});
  await assert.rejects(readManagedProgramBytes(program.peerWasm,page,{fetchImpl:async()=>new Response(data.slice(1))}),/length/);
});

test('co-op manifest pins the battle wasm separately from the ordinary title/STD core and will not substitute either',async()=>{
 const make=()=>{const p=receipt(),id='d'.repeat(64);p.schema='th128mp-program/2';p.coopGeneration=id;p.peerGeneration='9'.repeat(64);for(const [k,ext] of [['peer','mjs'],['peerWasm','wasm']])p.files[k]=pin('runtime/transport/'+p.peerGeneration+'/author-peer.'+ext);p.files.coopCore=pin(`runtime/coop/${id}/coop-core.mjs`);p.files.coopWasm={...pin(`runtime/coop/${id}/coop-core.wasm`),sha256:id};return p;};
 const get=p=>readManagedProgram(page,{fetchImpl:async()=>new Response(JSON.stringify(p))});const p=await get(make());assert.equal(p.gameplay,'same-stage-coop');assert.notEqual(p.coopWasm.sha256,p.wasm.sha256);
 for(const mutate of [m=>{delete m.files.coopCore;},m=>m.files.coopWasm=m.files.wasm,m=>m.files.coopWasm.sha256='7'.repeat(64),m=>m.schema='th128mp-program/1',m=>delete m.peerGeneration,m=>m.files.peer=receipt().files.peer,m=>m.files.peerWasm=receipt().files.peerWasm]){const x=make();mutate(x);await assert.rejects(get(x));}
});