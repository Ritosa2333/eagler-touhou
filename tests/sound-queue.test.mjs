import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import{spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';
import{readCoreSoundEvents}from'../src/sound-events.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),folder=path.join(root,'artifacts/sound-queue-tests');fs.mkdirSync(folder,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const built=spawnSync(python,[compiler,'tests/fixtures/sound-queue.cpp','src/game/SoundQueue.cpp','-std=c++20','-O2','-o',path.join(folder,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_reset","_request","_snapshot","_events","_events_ptr","_events_stride"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});assert.equal(built.status,0,built.stderr||built.stdout);
const create=(await import(pathToFileURL(path.join(folder,'core.mjs')).href)).default,original=JSON.parse(fs.readFileSync(new URL('fixtures/sound-queue-original.json',import.meta.url)));
const snapshot=c=>{let p=c._snapshot(),end=p;while(c.HEAPU8[end])end++;return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,end)));};
test('sound grouping,12/128 capacity,stop and integer pan mean match original queue instructions',async()=>{
  for(const row of original.rows){const c=await create();c._reset();for(const args of row.requests)c._request(...args);assert.deepEqual(snapshot(c),row.slots,row.name);const wrapped={HEAPU8:c.HEAPU8,_th12_sound_events:c._events,_th12_sound_events_ptr:c._events_ptr,_th12_sound_events_stride:c._events_stride};const events=readCoreSoundEvents(wrapped);assert.deepEqual(events.filter(e=>!e.stop).map(e=>({id:e.id,pan:e.pan})),row.played,row.name);for(let i=0;i<10;i++)assert.deepEqual(readCoreSoundEvents(wrapped),events);assert.deepEqual(snapshot(c),row.slots,'drawing must not clear pending sound requests');}
});
test('original sound IDs resolve all60 resources rather than substituting a generic bullet sound',()=>{
  assert.deepEqual(original.sounds.map(s=>s.id),Array.from({length:60},(_,i)=>i));for(const[id,file]of [[0,'se_plst00.wav'],[2,'se_pldead00.wav'],[3,'se_enep00.wav'],[21,'se_lazer02.wav'],[44,'se_graze.wav'],[48,'se_cardget.wav'],[54,'se_ufoalert.wav']])assert.equal(original.sounds[id].file,file);assert.equal(original.sounds[54].loop,true);
});
test('sound ABI reader rejects malformed buffers before reading outside memory',()=>{
  assert.equal(readCoreSoundEvents({}),null);assert.throws(()=>readCoreSoundEvents({_th12_sound_events:()=>13,_th12_sound_events_ptr:()=>0,_th12_sound_events_stride:()=>16,HEAPU8:new Uint8Array(8)}),/记录/);
});
