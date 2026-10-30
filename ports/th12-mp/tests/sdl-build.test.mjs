import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtempSync,mkdirSync,writeFileSync,rmSync} from 'node:fs';
import {join,dirname,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {sdlBuildInputs,verifySdlBuild} from '../scripts/sdl-build-inputs.mjs';

function fixture(t){
  const temporary=mkdtempSync(join(tmpdir(),'th12-sdl-build-')),root=join(temporary,'ports/th12');
  t.after(()=>rmSync(temporary,{recursive:true,force:true}));
  const put=(path,bytes='source')=>{const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,bytes);};
  for(const path of ['portable/sdl/Application.cpp','src/game/Main.cpp','CMakeLists.txt',
    'third_party/stb_vorbis/stb_vorbis.h','portable/sdl/scripts/gen-cp932.py',
    'scripts/build-sdl.mjs','scripts/sdl-build-inputs.mjs','scripts/toolchain.mjs',
    '../../common/browser/peer.mjs','../../common/contracts/shared.mjs','../../common/include/eagler/Shared.hpp','../../common/src/Shared.cpp','../../common/third_party/imgui/imgui.cpp'])put(path);
  const artifacts={};
  for(const name of ['touhou12.mjs','touhou12.wasm']){
    const bytes=Buffer.from('compiled '+name);put('site/runtime/sdl/'+name,bytes);
    artifacts[name]={bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};
  }
  return {root,put,build:{schema:'th12-sdl-build/1',platform:'web',sources:sdlBuildInputs(root),artifacts}};
}

test('SDL packaging binds shared compiler inputs and detects newly added translation units',t=>{
  const f=fixture(t);verifySdlBuild(f.root,f.build);
  f.put('../../common/include/eagler/Shared.hpp','changed header');
  assert.throws(()=>verifySdlBuild(f.root,f.build),/编译输入已改变/);
  f.put('../../common/include/eagler/Shared.hpp');verifySdlBuild(f.root,f.build);
  f.put('portable/sdl/NewScene.cpp');
  assert.throws(()=>verifySdlBuild(f.root,f.build),/编译输入已改变/);
});

for(const name of ['touhou12.mjs','touhou12.wasm'])test('SDL packaging rejects replaced compiler output: '+name,t=>{
  const f=fixture(t);f.put('site/runtime/sdl/'+name,'replaced executable');
  assert.throws(()=>verifySdlBuild(f.root,f.build),/编译产物身份不符/);
});
