import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..'),common=resolve(root,'../../common');
const compiler=process.env.EAGLER_WASI_CXX;assert(compiler,'Set EAGLER_WASI_CXX to WASI clang++');
for(const variant of ['th15','th15-mp']){
 const title=resolve(root,'..',variant),out=resolve(title,'artifacts/touch-shield');mkdirSync(out,{recursive:true});
 for(const fixture of ['touch-shield-check','touch-bomb-check','verify']){
  const wasm=resolve(out,fixture+'.wasm');
  const r=spawnSync(compiler,['--target=wasm32-wasip1','-O2','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-mexec-model=reactor','-Wl,--no-entry','-I'+resolve(common,'include'),resolve(title,'portable/input',fixture+'.cpp'),'-o',wasm],{encoding:'utf8',windowsHide:true});
  assert.equal(r.status,0,r.error?.message||r.stdout+r.stderr);
  const wasi=new WASI({version:'preview1',args:[],env:{}}),{instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);assert.equal(instance.exports.verify(),0,variant+'/'+fixture);console.log('PASS',variant,fixture);
 }
 const source=readFileSync(resolve(title,'web/cpp/sdl/BrowserRuntime.cpp'),'utf8');
 assert(!source.includes('retain_motion'));
 writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,variant,checks:['three shield layers','death preserves held gesture','release and pause cancel gesture','dialogue','deathbomb','motion representation'],physicalDevice:'not run'},null,2)+'\n');
}
