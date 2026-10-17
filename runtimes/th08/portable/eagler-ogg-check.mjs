import assert from 'node:assert/strict';
import {existsSync,readFileSync,mkdirSync} from 'node:fs';
import {join,resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';

const root=resolve('.'),sdk=process.env.EMSDK??resolve(root,'tools/emsdk'),emcc=resolve(sdk,'install/emscripten/emcc.py');
const out=resolve(root,'th08_web/artifacts/sdl3/eagler-ogg-check.mjs');mkdirSync(resolve(root,'th08_web/artifacts/sdl3'),{recursive:true});
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(root,'th10_web/artifacts/eagler-emconfig'),EMSDK:sdk};
const built=spawnSync(process.env.TH_PYTHON??'python',[emcc,'portable/eagler-ogg-check.cpp','-O2','-std=c++17','-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_RUNTIME_METHODS=FS','-o',out],{env,encoding:'utf8',windowsHide:true});
assert.equal(built.status,0,built.stdout+built.stderr);
const create=(await import(pathToFileURL(out))).default;let wasmExports;
const runtime=await create({instantiateWasm(imports,done){WebAssembly.instantiate(readFileSync(out.replace('.mjs','.wasm')),imports).then(({instance,module})=>{wasmExports=instance.exports;done(instance,module);});return {};}});
const layout=JSON.parse(readFileSync(resolve(root,'th08_web/assets/sdl-native/music-layout.json'),'utf8'));
const fixtureCandidates=[process.env.EAGLER_OGG_FIXTURE,resolve(root,'th08_web/assets-ogg/bgm-ogg'),resolve(root,'../../games/web-content/th08/bgm-ogg')].filter(Boolean);
const fixture=fixtureCandidates.find(existsSync);assert.ok(fixture,'Set EAGLER_OGG_FIXTURE to the private th08 bgm-ogg directory.');
try{runtime.FS.mkdir('/bgm-ogg');}catch{}
const first=layout[0],firstPath=join(fixture,'th08_01.ogg');assert.ok(existsSync(firstPath),firstPath);
assert.equal(wasmExports.check_late_missing(),0,'missing OGG must remain retryable');
const firstBytes=readFileSync(firstPath);runtime.FS.writeFile('/bgm-ogg/late.ogg',firstBytes);
assert.equal(wasmExports.check_late(Math.floor(first.length/4),Math.floor(first.loop/4)),0,'late OGG retry');
for(const row of layout){const name=row.name.replace(/\.wav$/i,'.ogg'),path=join(fixture,name);assert.ok(existsSync(path),path);runtime.FS.writeFile('/bgm-ogg/track.ogg',readFileSync(path));assert.equal(wasmExports.check(Math.floor(row.length/4),Math.floor(row.loop/4)),0,name+' stream/full/seek/loop');}
const host=readFileSync(resolve(root,'th08_web/cpp/sdl/AudioHost.cpp'),'utf8');
for(const required of ['sdl_ogg_decode_mode','sdl_music_resource_changed','sdl_music_stats','/bgm-ogg/','id==1000&&!audio.music','create_pcm'])assert.ok(host.includes(required),'AudioHost contract: '+required);
console.log(JSON.stringify({passed:true,game:'th08',tracks:layout.length,checks:['native stream decoder and full PCM data-source APIs are PCM-identical','random seek and tail reads are exact','loop point crosses tail without discontinuity','decoder retry succeeds after late FS delivery'],contractChecks:['AudioHost exports none/resource-change/stats controls','none gates only BGM handle 1000 while create_pcm SFX path remains']},null,2));
