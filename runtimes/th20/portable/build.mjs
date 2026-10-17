const builtAt=new Date().toISOString();
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,statSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=resolve(fileURLToPath(new URL('../',import.meta.url))),game=process.argv.includes('--th08')?'th08':process.argv.includes('--th10')?'th10':'th20',root=resolve(workspace,game+'_web'),presentationLab=process.argv.includes('--presentation-lab'),profile=presentationLab?'presentation-lab':'sdl3',out=resolve(root,'artifacts',profile);mkdirSync(out,{recursive:true});
const sdk=process.env.EMSDK??(existsSync(resolve(workspace,'tools/emsdk'))?resolve(workspace,'tools/emsdk'):resolve(workspace,'../toolchains/emsdk'));
const emcc=[resolve(sdk,'install/emscripten/emcc.py'),resolve(sdk,'upstream/emscripten/emcc.py')].find(existsSync);
if(!emcc)throw Error('Install the pinned Emscripten SDK first (tools/download-emscripten.py).');
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:process.env.EMCC_CORES??'4'};
const python=process.env.TH_PYTHON??'python';
const run=(args)=>new Promise((done,reject)=>{const p=spawn(python,[emcc,...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';p.stdout.on('data',x=>{log+=x;process.stdout.write(x);});p.stderr.on('data',x=>{log+=x;process.stderr.write(x);});p.on('error',reject);p.on('exit',code=>code?reject(Error('emcc failed '+code+'\n'+log)):done());});
const thcrap=process.env.TH_ENABLE_THCRAP!=='0';
const audioInclude=resolve(process.env.EAGLER_COMMON_ROOT??resolve(workspace,'../../common'),'include');
// Phase A ships the ImGui/thprac infrastructure only. Phase D activates the
// practice runtime/overlay call sites; without the flag every guarded call site
// compiles out and the build is the vanilla baseline.
const thprac=process.env.TH_ENABLE_THPRAC==='1'||process.argv.includes('--thprac');
const imgui=resolve(root,'cpp/third_party/imgui');
const common=['-O2','-g0','-fno-strict-aliasing','-ffp-contract=off','-DTH_SDL3=1','-DTH_NATIVE_PLATFORM=1','-DIMGUI_DISABLE_WIN32_FUNCTIONS','--use-port=sdl3','--use-port=sdl3_ttf','-I'+resolve(workspace,'portable/sdl'),'-I'+imgui,...(game==='th20'?['-I'+resolve(root,'cpp/shim'),'-I'+resolve(workspace,'source_reconstruction')]:[]),...(thcrap?['-DTH_ENABLE_THCRAP=1']:[]),...(thprac?['-DTH_ENABLE_THPRAC=1']:[])];
if(presentationLab)common.push('-DTH_PRESENTATION_AUDIT=1');
const excluded=new Set(game==='th10'?['LegacyBridge.cpp','LegacyCallbacks.cpp','Exports.cpp','Freestanding.cpp',thcrap?'LocalizationStub.cpp':'Localization.cpp']:game==='th08'?['RuntimeExports.cpp']:[]);
const sources=game==='th20'?[]:readdirSync(resolve(root,'cpp/game')).filter(n=>n.endsWith('.cpp')&&!excluded.has(n)).map(n=>'cpp/game/'+n);
if(game==='th20'){
 // TH20 keeps its game code in source_reconstruction/ (in-place port, guarded by
 // TH_SDL3 ifdefs). Oracle/cpu-compare/test translation units stay out of the wasm build.
 // verify/backend_smoke/replay_format_probe are host-side CLI tools with their own
 // main() and CMake-injected SHA defines, not game runtime; excluded from the wasm link.
 const recon=resolve(workspace,'source_reconstruction'),skip=/oracle|test|diagnostics|audit|link_probe|cpu_compare|archive[\\/]verify\.cpp|backend_smoke\.cpp|replay_format_probe\.cpp/i;
 const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):e.name.endsWith('.cpp')&&!skip.test(relative(recon,resolve(dir,e.name)))?[resolve(dir,e.name)]:[]);
 sources.push(...walk(recon));
 // Module headers reference each other with bare quoted includes (the native
 // CMake wires these per-target); expose every module dir plus native_recovered.
 for(const dir of readdirSync(recon,{withFileTypes:true}))if(dir.isDirectory())common.push('-I'+resolve(recon,dir.name));
 common.push('-I'+resolve(workspace,'native_recovered'));
}
sources.push(...readdirSync(resolve(root,'cpp/platform')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/platform/'+n));
sources.push(...readdirSync(resolve(root,'cpp/sdl')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/sdl/'+n));
sources.push(...['imgui.cpp','imgui_draw.cpp','imgui_freetype.cpp','imgui_tables.cpp','imgui_widgets.cpp'].map(n=>'cpp/third_party/imgui/'+n));
// Phase C: the practice config/runtime and the thprac overlay. They compile
// alongside Phase A's ImGui translation units (the generated .inc is textually
// included by PracticeRuntime.cpp); the thprac flag below only advertises the
// feature until Phase D wires the overlay into the game loop.
for(const source of ['cpp/game/PracticeConfig.cpp','cpp/game/PracticeRuntime.cpp','cpp/sdl/ThpracUi.cpp'])
 if(existsSync(resolve(root,source))&&!sources.includes(source))sources.push(source);
const shared=resolve(workspace,'portable/sdl'),numeric=resolve(workspace,'portable/numeric'),input=resolve(workspace,'portable/input'),renderer=resolve(shared,'Renderer.cpp');
common.push('-I'+audioInclude);
const audioHeaders=[resolve(audioInclude,'eagler/audio/WebSfx.hpp')];
function headers(dir){return readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headers(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);}
const hash=createHash('sha256');for(const path of [...headers(resolve(root,'cpp')),...(game==='th20'?headers(resolve(workspace,'source_reconstruction')):[]),...headers(shared),...headers(numeric),...headers(input),...audioHeaders].sort())hash.update(path).update(readFileSync(path));
// th20 game code genuinely throws (archive/verify paths, D3DX loader errors)
// and uses dynamic_cast once, so the wasm build keeps exceptions and RTTI.
const flags=[...common,game==='th20'?'-std=c++20':'-std=c++17',...(game==='th20'?['-fexceptions','-frtti','-include',resolve(root,'cpp/shim/msvc_compat.h'),'-I'+resolve(workspace,'include'),'-I'+resolve(root,'cpp'),'-msimd128','-msse','-msse2','-mavx']:['-fno-exceptions','-fno-rtti'])],prefix=JSON.stringify([flags,hash.digest('hex')]);
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
async function compile(source,name,c=false){const object=resolve(objects,name+'.o'),key=createHash('sha256').update(prefix).update(readFileSync(source)).digest('hex');if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;
 await run([...(c?[...common,'-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5']:flags),'-c',source,'-o',object]);writeFileSync(object+'.key',key);return object;
}
console.log('Build '+game+' C++ / SDL3 / Emscripten');
// Populate SDL's port cache once before parallel translation units use it.
const rendererObject=await compile(renderer,'shared_renderer');
const soft=resolve(root,game==='th10'?'cpp/rebuild/third_party/softfloat.c':'cpp/third_party/softfloat.c'),softObject=await compile(soft,'softfloat',true);
const outputs=new Array(sources.length);let next=0,done=0;
// th20 Phase-1 acceptance is "every TU compiles"; a single failing TU should not
// hide the remaining error surface, so th20 workers keep going and failures are
// reported (and thrown) after the sweep. th08/th10 keep fail-fast semantics.
const failures=[];
const buildJobs=Math.max(1,Math.min(16,Number.parseInt(process.env.TH_BUILD_JOBS||'4',10)||4));
await Promise.all(Array.from({length:buildJobs},async()=>{while(next<sources.length){const i=next++;try{outputs[i]=await compile(resolve(root,sources[i]),sources[i].replaceAll(/[\\/:.]/g,'_'));}catch(e){if(game!=='th20')throw e;failures.push(sources[i]);console.error('FAILED '+sources[i]);}if(++done%40===0)console.log(done+'/'+sources.length+' translation units');}}));
if(failures.length){console.error(failures.length+'/'+sources.length+' translation units failed:');for(const f of failures)console.error('  '+f);throw Error(failures.length+' translation units failed to compile');}
const output=resolve(out,game+'-sdl.mjs');
const hostImports=[];
const library=resolve(out,'browser-services.js');writeFileSync(library,'addToLibrary({\n'+hostImports.map(i=>`${JSON.stringify(i.name)}: function() { return Module['services'][${JSON.stringify(i.module)}][${JSON.stringify(i.name)}].apply(null, arguments); }`).join(',\n')+'\n});\n');
// th20 links ~470 objects; the Windows spawn command-line limit requires a
// response file (emcc reads @file argument lists).
const linkObjects=[...outputs,rendererObject,softObject];
let linkInputs=linkObjects;
if(game==='th20'){const rsp=resolve(out,'link-objects.rsp');writeFileSync(rsp,linkObjects.map(p=>JSON.stringify(p.replaceAll('\\','/'))).join('\n'));linkInputs=['@'+rsp];}
// TH20 hosts two retail archives in MEMFS (th20.dat 151 MiB via the managed
// provider, thbgm.dat 430 MiB as a runtime resource) on top of the game's own
// working set; its memory ceiling is 2 GiB instead of the shared 1 GiB.
await run([...flags,'--emit-symbol-map','--js-library',library,'-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=67108864',game==='th20'?'-sMAXIMUM_MEMORY=2147483648':'-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...(process.env.TH_DEBUG_NAMES==='1'?['-g']:[]),...linkInputs,'-o',output]);
const wasm=readFileSync(output.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm),sha=x=>createHash('sha256').update(x).digest('hex');
const sourceFiles=[...sources.map(p=>resolve(root,p)),...headers(resolve(root,'cpp')),...(game==='th20'?headers(resolve(workspace,'source_reconstruction')):[]),...headers(shared),...headers(numeric),...headers(input),...audioHeaders,renderer,soft,resolve(workspace,'portable',game+'-services.json'),...(existsSync(resolve(root,'cpp/game/THPRAC-LICENSE.txt'))?[resolve(root,'cpp/game/THPRAC-LICENSE.txt')]:[]),fileURLToPath(import.meta.url)].sort();
const inventory=Object.fromEntries(sourceFiles.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]));
const sdkMetadata=resolve(sdk,'touhou-sdk.json');
const toolchain=existsSync(sdkMetadata)?JSON.parse(readFileSync(sdkMetadata)):{emsdkRoot:relative(workspace,sdk).replaceAll('\\','/'),layout:'external'};
const report={builtAt,game,kind:'cpp-sdl3',profile,diagnostic:presentationLab,version:game==='th10'?'3.5.1-sdl3':game==='th08'?'3.4.0-sdl3':'1.0.0-sdl3',features:{thprac,languages:thcrap,focusHitbox:false},...{architecture:{loop:'cpp-original-cadence-skip-expired-single-tick',audio:'miniaudio-sdl3',renderer:'cpp-gles-semantic-batched',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',files:'sdl-io-idbfs',fonts:'sdl3-ttf',input:'cpp-sdl',launcher:'eagler-touhou/1'}},sdlVersion:'3.4.2',sources,sourceFiles:inventory,sharedSources:['Renderer.cpp','Renderer.hpp','Shaders.hpp','GraphicsState.hpp','AssetPixelFormat.hpp','RenderCommands.hpp','LegacyGraphics.hpp','ExactFloat.hpp','MotionTrack.hpp'],bytes:wasm.length,sha256:sha(wasm),loaderSha256:sha(readFileSync(output)),imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),toolchain};
writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({game,bytes:wasm.length,sha256:report.sha256,output},null,2));
