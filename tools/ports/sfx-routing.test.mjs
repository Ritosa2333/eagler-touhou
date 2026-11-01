import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {targets,inspectInputs,root} from './inputs.mjs';

// Source/build wiring complements the shared service's rendered PCM tests.
const selected=process.env.EAGLER_PORT_CHECK_GAMES?JSON.parse(process.env.EAGLER_PORT_CHECK_GAMES):null;
for(const target of targets().filter(target=>!selected||selected.includes(target.game)))test(`${target.game} ${target.variant} compiles its SFX output route`,()=>{
 const inputs=inspectInputs(target),owner=resolve(root,target.directory);
 if(target.game==='th05'){
  assert(inputs.translationUnits.includes('portable/Platform.cpp'));
  const platform=readFileSync(resolve(owner,'portable/Platform.cpp'),'utf8');
  if(target.variant==='multiplayer'){
   assert(inputs.translationUnits.includes('portable/Netplay.cpp'));
   assert.match(platform,/void PlaySound\(int index\)\{if\(coop::QueueSound\(index\)\)return;EM_ASM\(\{Module\.eaglerAudio\?\.sound\(\$0\)/);
   const netplay=readFileSync(resolve(owner,'portable/Netplay.cpp'),'utf8');
   assert.match(netplay,/through=std::min\(Confirmed\(\),int\(nextFrame\)-1\)/);
   assert.match(netplay,/int\(it->first\)<=through/);
   assert.match(netplay,/if\(event.kind==0\)web::PlaySound\(event.index\)/);
   assert.match(netplay,/it=audio.erase\(it\)/);
  }else assert.match(platform,/void PlaySound\(int index\)\{EM_ASM\(\{Module\.eaglerAudio\?\.sound\(\$0\)/);
  const audio=readFileSync(resolve(owner,'web/audio.mjs'),'utf8');
  assert.match(audio,/sfx=createSfx\(ctx\(\),files\)/);assert.match(audio,/sound\(index\).*sfx\?\.trigger\(index\)/);
  assert.match(audio,/pump\(\)\{sfx\?\.pump\(\)/);return;
 }
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
