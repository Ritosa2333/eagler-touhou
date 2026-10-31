import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import path from 'node:path';import {spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';
import {decodeSpellRealTime}from'../src/spell-presentation.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(root,'artifacts/spell-wall-time-test.mjs');
const exports=['reset','start','clock_ms','tick','end','encoded','display_ticks','flags','replay'];
const build=spawnSync(python,[compiler,'tests/fixtures/spell-wall-time.cpp','-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(exports.map(name=>'_'+name))],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
assert.equal(build.status,0,build.stderr||build.stdout);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/spell-wall-time-original.json',import.meta.url)));
test('manual spell real-time output agrees with original QPC, 16.7ms quantization and encoding',async()=>{
 const c=await create();assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const record of native.cases){c._reset();c._start();c._clock_ms(1000);for(let i=0;i<120;i++)c._tick();c._end();c._clock_ms(1000+record.milliseconds);
  assert.equal(c._encoded(),record.encoded,`${record.milliseconds}ms`);assert.deepEqual(decodeSpellRealTime(c._encoded()),record.decoded);assert.equal(c._display_ticks(),record.serialResult);assert.equal(c._flags(),record.flagsAfter);}
});
test('paused observations retain wall start and replay playback never replaces recorded real time',async()=>{
 const c=await create();c._reset();c._start();for(const frame of native.pausedObservations){if(!frame.active)c._end();c._clock_ms(frame.milliseconds);assert.equal(c._flags(),frame.flags|2);if(!frame.active)assert.equal(c._encoded(),frame.encoded);}
 c._reset();c._replay(native.cases[7].encoded);c._start();c._clock_ms(1000);for(let i=0;i<120;i++)c._tick();c._end();c._clock_ms(9999999);assert.equal(c._encoded(),native.cases[7].encoded);assert.equal(c._display_ticks(),121);
 c._reset();c._start();c._clock_ms(Number.NaN);c._end();c._clock_ms(1000);assert.equal(c._encoded(),-1,'missing monotonic clock is not replaced with ticks');
});
