import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/anm-external-scale');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const names=['malloc','free','external_load','external_begin','external_interrupt','external_scale','external_tick','external_snapshot'];
const result=spawnSync(python,[compiler,'tests/fixtures/anm-external-scale.cpp','src/game/AnmLogic.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const createFixture=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('./fixtures/anm-external-scale-original.json',import.meta.url)));
const snapshot=core=>[...new Float32Array(core.HEAPU8.buffer,core._external_snapshot(),9)];
async function run(row){
 const core=await createFixture();
 const raw=row.syntheticBankHex?Buffer.from(row.syntheticBankHex,'hex'):fs.readFileSync(path.join(root,'local/retail/bullet.anm'));
 assert.equal(createHash('sha256').update(raw).digest('hex'),row.resourceSha256);
 const p=core._malloc(raw.length);core.HEAPU8.set(raw,p);assert.equal(core._external_load(p,raw.length),1);core._free(p);
 assert.equal(core._external_begin(row.script),1);
 for(const frame of row.frames){
  if(frame.tick===1&&row.interrupt)core._external_interrupt(row.interrupt);
  if(frame.externalScale)core._external_scale(...frame.externalScale);
  if(frame.tick)core._external_tick();
  const got=snapshot(core),label=row.name+' tick'+frame.tick;
  assert.deepEqual(got.slice(0,2),frame.scale,label+' scale');
  assert.deepEqual(got.slice(2,7),[frame.clock,frame.layer,frame.primary>>>24,+frame.ended,0],label+' lifecycle');
 }
 return core;
}
test('original zero-duration interpolation releases external scale, while negative durations retain native time0',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of original.records.filter(row=>row.syntheticBankHex))await run(row);
});
test('retail bullet script42 IRQ2 preserves caller laser length after its zero-duration scale interpolator',async()=>{
 const row=original.records.find(row=>row.name==='retail-bullet42-irq2'),core=await run(row);
 assert.deepEqual(snapshot(core).slice(0,2),row.frames.at(-1).externalScale);
});
