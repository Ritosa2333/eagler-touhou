import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {targets,inspectInputs,root} from './inputs.mjs';

// Source/build wiring complements the shared service's rendered PCM tests.
for(const target of targets())test(`${target.game} ${target.variant} compiles its immediate SFX route`,()=>{
 const inputs=inspectInputs(target),owner=resolve(root,target.directory);
 const routes=inputs.translationUnits.map(path=>resolve(owner,path))
  .map(path=>({path,source:readFileSync(path,'utf8')}))
  .filter(({source})=>source.includes('#include <eagler/audio/WebSfx.hpp>'));
 assert(routes.length,`${target.directory}: no shared SFX output in compiled sources`);
 assert(routes.some(({source})=>source.includes('web_sfx::upload(')&&source.includes('web_sfx::play(')),
  `${target.directory}: sound upload and playback must reach shared output`);
 if(target.recipe.kind==='node')assert(inputs.inputFiles.includes(resolve(root,'common/include/eagler/audio/WebSfx.hpp')),
  `${target.directory}: shared output header must participate in build identity`);
 else assert.match(readFileSync(resolve(owner,'CMakeLists.txt'),'utf8'),/EAGLER_COMMON_ROOT.*\/cmake\/EaglerCommon\.cmake/);
});
