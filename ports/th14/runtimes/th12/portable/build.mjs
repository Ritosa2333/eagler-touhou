// Frozen-source SDL3/Emscripten build for the TH11 portable runtime.
// Mirrors th10/th20 portable/build.mjs: compile cpp/game + cpp/sdl + the shared
// Renderer, link a raw-exported ESM loader, and write an attested build.json.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=resolve(fileURLToPath(new URL('../',import.meta.url))),root=resolve(workspace,'th11_web'),out=resolve(root,'artifacts/sdl3');mkdirSync(out,{recursive:true});
// TH11 ships no tools/emsdk; prefer an explicit EMSDK, then the shared TH08 SDK.
const sdk=process.env.EMSDK??(existsSync(resolve(workspace,'tools/emsdk'))?resolve(workspace,'tools/emsdk'):resolve(workspace,'../th08/tools/emsdk'));
const emcc=[resolve(sdk,'install/emscripten/emcc.py'),resolve(sdk,'upstream/emscripten/emcc.py')].find(existsSync);
if(!emcc)throw Error('TH11 build needs an Emscripten SDK (set EMSDK or provide th08/tools/emsdk).');
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const python=process.env.TH_PYTHON??'python';
const run=args=>new Promise((done,reject)=>{const p=spawn(python,[emcc,...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';p.stdout.on('data',x=>{log+=x;process.stdout.write(x);});p.stderr.on('data',x=>{log+=x;process.stderr.write(x);});p.on('error',reject);p.on('exit',code=>code?reject(Error('emcc failed '+code+'\n'+log)):done());});
const common=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH11_DEVELOPMENT_HARNESS=0','--use-port=sdl3','--use-port=sdl3_ttf'];
const files=dir=>readdirSync(resolve(root,dir),{withFileTypes:true}).flatMap(e=>e.isDirectory()?files(dir+'/'+e.name):e.name.endsWith('.cpp')?[resolve(root,dir,e.name)]:[]);
const source=[...files('cpp/game'),...files('cpp/sdl'),resolve(workspace,'portable/sdl/Renderer.cpp')];
const headerFiles=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headerFiles(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...headerFiles(resolve(root,'cpp')),...headerFiles(resolve(workspace,'portable/sdl')),...headerFiles(resolve(workspace,'portable/input'))].sort();
const sha=b=>createHash('sha256').update(b).digest('hex');
const headerHash=sha(headers.map(p=>p+sha(readFileSync(p))).join('\n'));
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
const settings=JSON.stringify([common,headerHash]);
async function compile(file){const name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_').replaceAll(':','_').replaceAll('.','_'),object=resolve(objects,name+'.o'),key=sha(settings+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
console.log('Build th11 C++ / SDL3 / Emscripten');
const objectsBuilt=[];let cursor=0,done=0;await Promise.all(Array.from({length:4},async()=>{while(cursor<source.length){const i=cursor++;objectsBuilt[i]=await compile(source[i]);if(++done%40===0)console.log(done+'/'+source.length+' translation units');}}));
const loader=resolve(out,'th11-sdl.mjs');
const exported=['_malloc','_free','_th11_validate_file','_th11_save_scores','_th11_save_replay','_th11_initialize','_th11_music_enabled','_th11_audio_statistics','_th11_return_title','_th11_restart','_th11_error','_th11_frame','_th11_phase','_th11_pause','_th11_resume','_th11_loop_start','_th11_loop_stop','_th11_loop_pause','_th11_key','_th11_keys_clear','_th11_touch','_th11_touch_cancel','_th11_touch_options','_th11_touch_controls','_th11_touch_stick'];
await run([...common,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS='+exported.join(','),...objectsBuilt,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm);
const sourceFiles=[...source.map(p=>resolve(workspace,relative(workspace,p))),...headers,fileURLToPath(import.meta.url)].sort();
const inventory=Object.fromEntries(sourceFiles.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]));
const sdkMetadata=resolve(sdk,'touhou-sdk.json');
const toolchain=existsSync(sdkMetadata)?JSON.parse(readFileSync(sdkMetadata)):{emsdkRoot:relative(workspace,sdk).replaceAll('\\','/'),layout:'external'};
const report={game:'th11',kind:'cpp-sdl3',profile:'sdl3',diagnostic:false,version:'1.0.0-sdl3',features:{thprac:false,languages:true,focusHitbox:false},architecture:{loop:'cpp-fixed-60hz-bounded-catchup',audio:'cpp-miniaudio-vorbis-sdl3-stream',renderer:'cpp-gles-semantic-batched',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',files:'sdl-io-idbfs',fonts:'cpp-original-baked-glyphs-argb4444',input:'cpp-sdl',launcher:'eagler-touhou/1'},sdlVersion:'3.4.2',sources:source.map(p=>relative(workspace,p).replaceAll('\\','/')),sourceFiles:inventory,sharedSources:['Renderer.cpp','Renderer.hpp','Shaders.hpp','GraphicsState.hpp','AssetPixelFormat.hpp','RenderCommands.hpp','LegacyGraphics.hpp','MotionTrack.hpp','TouchController.hpp'],bytes:wasm.length,sha256:sha(wasm),loaderSha256:sha(readFileSync(loader)),imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),toolchain};
writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({game:'th11',bytes:wasm.length,sha256:report.sha256,output:loader},null,2));
