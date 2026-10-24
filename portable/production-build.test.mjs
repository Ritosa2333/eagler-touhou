import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,mkdirSync,writeFileSync,copyFileSync,readdirSync,readFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {validateProductionBuild} from './production-build.mjs';
const empty=Uint8Array.from([0,97,115,109,1,0,0,0]);
const build={profile:'sdl3',diagnostic:false,exports:[]};
function exported(name){
  const text=new TextEncoder().encode(name);
  return Uint8Array.from([...empty,1,4,1,96,0,0,3,2,1,0,7,text.length+4,1,text.length,...text,0,0,10,4,1,2,0,11]);
}
test('production accepts the exact binary export attestation',()=>{
  validateProductionBuild(build,empty);
  const binary=exported('th11_frame');
  validateProductionBuild({...build,exports:WebAssembly.Module.exports(new WebAssembly.Module(binary))},binary);
  assert.throws(()=>validateProductionBuild(build,binary),/attestation/);
});
test('all diagnostic profiles and unclassified builds are rejected',()=>{
  for(const profile of ['sdl3-diagnostics','presentation-lab',undefined])
    assert.throws(()=>validateProductionBuild({...build,profile},empty),/rejects diagnostic/);
  for(const diagnostic of [true,undefined])
    assert.throws(()=>validateProductionBuild({...build,diagnostic},empty),/rejects diagnostic/);
});
test('an actual diagnostic export cannot be hidden behind a production profile',()=>{
  for(const name of ['audit_state','presentation_lab_tick','th11_probe_frame','th11_presentation_state']){
    const binary=exported(name),exports=WebAssembly.Module.exports(new WebAssembly.Module(binary));
    assert.throws(()=>validateProductionBuild({...build,exports},binary),/diagnostic export/);
    assert.throws(()=>validateProductionBuild(build,binary),/attestation/);
  }
});
test('the real packager rejects a diagnostic build before writing output',()=>{
  const root=mkdtempSync(resolve(tmpdir(),'th11-package-profile-'));
  const portable=resolve(root,'portable'),binary=resolve(root,'web/artifacts/input'),out=resolve(root,'output');
  for(const dir of [portable,binary,out])mkdirSync(dir,{recursive:true});
  for(const name of ['package-eagler.mjs','production-build.mjs'])copyFileSync(resolve(import.meta.dirname,name),resolve(portable,name));
  writeFileSync(resolve(out,'keep.txt'),'existing output');
  writeFileSync(resolve(binary,'build.json'),JSON.stringify({...build,profile:'sdl3-diagnostics',diagnostic:true}));
  writeFileSync(resolve(binary,'th11-sdl.wasm'),empty);
  const result=spawnSync(process.execPath,[resolve(portable,'package-eagler.mjs')],{encoding:'utf8',windowsHide:true,
    env:{...process.env,TH11_OUTPUT:binary,EAGLER_RUNTIME_OUTPUT:out,EAGLER_FONT_ROOT:root}});
  assert.equal(result.status,1);assert.match(result.stderr,/Production packaging rejects diagnostic/);
  assert.deepEqual(readdirSync(out),['keep.txt']);assert.equal(readFileSync(resolve(out,'keep.txt'),'utf8'),'existing output');
});
