import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';

const root=path.resolve(import.meta.dirname,'..'),dir=path.join(root,'artifacts/anm-unmatched-interrupt-tests');
const sdk=[process.env.EMSDK,path.resolve(root,'../tools/emsdk'),path.resolve(root,'../../tools/emsdk')]
 .filter(Boolean).find(p=>fs.existsSync(path.join(p,'upstream/emscripten/emcc.py')));
const compiler=process.env.EMCC_PYTHON_SCRIPT||(sdk&&path.join(sdk,'upstream/emscripten/emcc.py'));
assert.ok(compiler&&fs.existsSync(compiler),'Activate Emscripten and set EMSDK or EMCC_PYTHON_SCRIPT');
const bundled=sdk&&path.join(sdk,'python/3.13.3_64bit/python.exe');
const python=process.env.EMSDK_PYTHON||process.env.PYTHON||(bundled&&fs.existsSync(bundled)?bundled:'python');
const output=path.join(dir,'core.mjs'),temp=path.join(dir,'tmp');fs.mkdirSync(temp,{recursive:true});
const built=spawnSync(python,[compiler,'tests/fixtures/anm-unmatched-interrupt.cpp','src/game/AnmLogic.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_FUNCTIONS=["_irq_load","_irq_setup","_irq_tick","_irq_snapshot","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('fixtures/anm-unmatched-interrupt-original.json',import.meta.url)));
const f32=v=>Array.isArray(v)?v.map(f32):Math.fround(v);
function compare(c,expected,label){
 const actual=JSON.parse(c.UTF8ToString(c._irq_snapshot()));
 for(const field of ['clock','pc','savedPC','pendingInterrupt'])
  assert.equal(actual[field],expected[field],label+' '+field);
 // Compare VM execution/visibility flags. Native's transform-dirty bit8 is
 // an existing presentation-cache marker, separate from these IRQ gates;
 // retain its raw value in the fixture without treating it as CPU control.
 assert.equal((actual.flags&0x80042001)>>>0,(expected.flags&0x80042001)>>>0,label+' IRQ flags');
 for(const field of ['time','savedTime','scale','rotation','uv','float0','gameRate'])
  assert.deepEqual(f32(actual[field]),f32(expected[field]),label+' '+field);
 for(const field of ['gameSeedCounter','displaySeedCounter'])
  assert.deepEqual(actual[field],expected[field],label+' '+field);
 assert.equal(actual.unsupported,0,label+' unsupported');
}
test('unmatched original ANM IRQ skips instructions while rate-scaled interpolation and velocity still advance',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.records.length,72);
 for(const row of original.records){
  const c=await create(),bytes=Buffer.from(row.syntheticBankHex,'hex'),at=c._malloc(bytes.length);
  c.HEAPU8.set(bytes,at);assert.equal(c._irq_load(at,bytes.length),1);c._free(at);
  assert.equal(c._irq_setup(row.delta,+row.fixedRate),1);
  const label=[row.handler,row.delta,row.initialState,row.fixedRate].join('/');
  compare(c,row.frames[0],label+' bind');
  for(let i=0;i<row.actions.length;i++){
   const a=row.actions[i];c._irq_tick(a.interrupt,+a.paused);compare(c,row.frames[i+1],label+' tick'+i);
  }
 }
});
