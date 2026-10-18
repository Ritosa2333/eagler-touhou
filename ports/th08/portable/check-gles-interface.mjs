// Audit the active, preprocessed game/platform sources, not inactive #else
// fixtures. A source grep alone would wrongly count original test adapters.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
const root=process.cwd(),sdk=resolve(root,'tools/emsdk');
for(const game of ['th08','th10']) {
 const out=resolve(root,game+'_web/artifacts/architecture-candidate/validation');mkdirSync(out,{recursive:true});
 const cpp=resolve(root,'artifacts',game+'-graphics-active.cpp'),output=cpp+'.ii';
 const paths=game==='th08'?['cpp/game/AnmRenderer.cpp','cpp/platform/BrowserRuntime.cpp','cpp/sdl/GraphicsHost.cpp']:['cpp/game/AnmRenderer.cpp','cpp/game/AnmMesh.cpp','cpp/game/AnmProjection.cpp','cpp/game/Presentation.cpp','cpp/platform/Graphics.cpp','cpp/platform/GraphicsExports.cpp','cpp/platform/ApplicationFrames.cpp','cpp/platform/ApplicationPresentation.cpp','cpp/sdl/GraphicsHost.cpp'];
 writeFileSync(cpp,paths.map(p=>'#include "'+resolve(root,game+'_web',p).replaceAll('\\','/')+'"').join('\n')+'\n#include "'+resolve(root,'portable/sdl/Renderer.cpp').replaceAll('\\','/')+'"\n');
 const run=spawnSync('python',[resolve(sdk,'install/emscripten/emcc.py'),'-E','-std=c++17','-DTH_NATIVE_PLATFORM=1','-DTH_SDL3=1','--use-port=sdl3','-I'+resolve(root,'portable/sdl'),cpp,'-o',output],{env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten')},encoding:'utf8',windowsHide:true,maxBuffer:32*1024*1024});
 assert.equal(run.status,0,run.stderr);
 const violations=[],seen=new Set();let current='';
 for(const line of readFileSync(output,'utf8').split(/\r?\n/)) {
  const marker=line.match(/^# \d+ "([^"]+)"/);if(marker){current=marker[1].replaceAll('\\\\','/').replaceAll('\\','/');continue;}
  if(!current.includes(game+'_web/cpp/')&&!current.includes('/portable/sdl/'))continue;
  seen.add(current);
  if(/\b(?:render_state|stage_state|texture_stage|sampler_state|set_render)\s*\(|\bset_stage\s*\([^,)]*,|\.(?:fvf|render\[|stage\[)|\blegacy::/.test(line))violations.push({file:current,line:line.trim()});
 }
 assert(seen.size>20);assert.deepEqual(violations,[],'No legacy graphics-state dispatch in the production preprocessed source');
 const build=JSON.parse(readFileSync(resolve(root,game+'_web/artifacts/sdl3/build.json')));
 assert(build.imports.every(i=>!/^th(?:08|10)_(?:device|graphics)$/.test(i.module)));
 const report={passed:true,game,wasm:build.sha256,files:[...seen].sort(),violations,checks:['preprocessed production owners','no numeric graphics-state dispatch or FVF parsing','no legacy JS graphics/device import']};
 writeFileSync(resolve(out,'gles-interface.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({passed:true,game,files:seen.size}));
}
