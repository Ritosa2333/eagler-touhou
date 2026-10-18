import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {spawnSync} from 'node:child_process';
const sdk=resolve('tools/emsdk'),out='th08_web/artifacts/sdl3/ogg-check.mjs';
const built=spawnSync('python',[sdk+'/install/emscripten/emcc.py','portable/check-ogg-decoder.cpp','-O2','-std=c++17','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_RUNTIME_METHODS=FS','-o',out],{env:{...process.env,EM_CONFIG:sdk+'/.emscripten',EMSDK:sdk},encoding:'utf8',windowsHide:true});
assert.equal(built.status,0,built.stdout+built.stderr);
const create=(await import(pathToFileURL(resolve(out)))).default;let exports;
const m=await create({instantiateWasm(imports,done){WebAssembly.instantiate(readFileSync(out.replace('.mjs','.wasm')),imports).then(({instance,module})=>{exports=instance.exports;done(instance,module);});return {};}}),rows=JSON.parse(readFileSync('th08_web/assets/sdl-native/music-verification.json'));
for(const row of rows){m.FS.writeFile('/track.ogg',readFileSync('th08_web/assets/sdl-native/music/'+row.file));const result=exports.check(row.frames,row.loopFrame);assert.equal(result,0,row.file+' decoder/loop check '+result);console.log(row.file+' decoded fully; exact loop seek verified');}
writeFileSync('th08_web/artifacts/architecture-candidate/validation/ogg-decoder.json',JSON.stringify({passed:true,tracks:rows.length,checks:['all frames decode with production Vorbis/miniaudio in Wasm','44.1 kHz stereo','original loop start and end','loop-crossing samples match independently sought segments']},null,2));
