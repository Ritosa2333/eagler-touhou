import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,statSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=resolve(fileURLToPath(new URL('../',import.meta.url))),game=process.argv.includes('--th08')?'th08':'th10',root=resolve(workspace,game+'_web'),out=resolve(root,'artifacts/sdl3');mkdirSync(out,{recursive:true});
const sdk=process.env.EMSDK??(existsSync(resolve(workspace,'tools/emsdk'))?resolve(workspace,'tools/emsdk'):resolve(workspace,'../toolchains/emsdk'));
const emcc=[resolve(sdk,'install/emscripten/emcc.py'),resolve(sdk,'upstream/emscripten/emcc.py')].find(existsSync);
if(!emcc)throw Error('Install the pinned Emscripten SDK first (tools/download-emscripten.py).');
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const python=process.env.TH_PYTHON??'python';
const run=(args)=>new Promise((done,reject)=>{const p=spawn(python,[emcc,...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';p.stdout.on('data',x=>{log+=x;process.stdout.write(x);});p.stderr.on('data',x=>{log+=x;process.stderr.write(x);});p.on('error',reject);p.on('exit',code=>code?reject(Error('emcc failed '+code+'\n'+log)):done());});
const imgui=resolve(root,'cpp/third_party/imgui');
const common=['-O2','-g0','-fno-strict-aliasing','-ffp-contract=off','-DTH_SDL3=1','-DTH_NATIVE_PLATFORM=1','-DIMGUI_DISABLE_WIN32_FUNCTIONS','--use-port=sdl3','--use-port=sdl3_ttf','-I'+resolve(workspace,'portable/sdl'),'-I'+imgui];
const presentationLab=process.argv.includes('--presentation-lab');
if(presentationLab){if(game!=='th08')throw Error('Presentation lab currently supports TH08 only');common.push('-DTH_PRESENTATION_AUDIT=1');}
// thcrap-style offline language pack for th08. ON by default, matching the
// always-on thprac convention; TH_ENABLE_THCRAP=0 builds the strict Japanese
// regression baseline (LocalizationStub, no /thcrap/th08/ override reads).
const thcrap=game==='th08'&&(process.env.TH_ENABLE_THCRAP??'1')!=='0';
const excluded=new Set(game==='th10'?['LegacyBridge.cpp','LegacyCallbacks.cpp','Exports.cpp','Freestanding.cpp']:['RuntimeExports.cpp',thcrap?'LocalizationStub.cpp':'Localization.cpp']);
const sources=readdirSync(resolve(root,'cpp/game')).filter(n=>n.endsWith('.cpp')&&!excluded.has(n)).map(n=>'cpp/game/'+n);
sources.push(...readdirSync(resolve(root,'cpp/platform')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/platform/'+n));
sources.push(...readdirSync(resolve(root,'cpp/sdl')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/sdl/'+n));
if(game==='th08')sources.push(...['imgui.cpp','imgui_draw.cpp','imgui_freetype.cpp','imgui_tables.cpp','imgui_widgets.cpp'].map(n=>'cpp/third_party/imgui/'+n));
const shared=resolve(workspace,'portable/sdl'),numeric=resolve(workspace,'portable/numeric'),input=resolve(workspace,'portable/input'),renderer=resolve(shared,'Renderer.cpp');
function headers(dir){return readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headers(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);}
const capturedInputs=new Map();
function observed(path){const bytes=readFileSync(path),digest=createHash('sha256').update(bytes).digest('hex');if(capturedInputs.has(path)&&capturedInputs.get(path)!==digest)throw Error('Source changed during build; rebuild: '+path);capturedInputs.set(path,digest);return bytes;}
const hash=createHash('sha256');for(const path of [...headers(resolve(root,'cpp')),...headers(shared),...headers(numeric),...headers(input)].sort())hash.update(path).update(observed(path));
const flags=[...common,...(thcrap?['-DTH_ENABLE_THCRAP=1']:[]),'-std=c++17','-fno-exceptions','-fno-rtti'],prefix=JSON.stringify([flags,hash.digest('hex')]);
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
async function compile(source,name,c=false){const object=resolve(objects,name+'.o'),key=createHash('sha256').update(prefix).update(observed(source)).digest('hex');if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;
 await run([...(c?[...common,'-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5']:flags),'-c',source,'-o',object]);writeFileSync(object+'.key',key);return object;
}
console.log('Build '+game+' C++ / SDL3 / Emscripten'+(game==='th08'?' / TH_ENABLE_THCRAP='+(thcrap?'ON':'OFF'):''));
// Populate SDL's port cache once before parallel translation units use it.
const rendererObject=await compile(renderer,'shared_renderer');
const soft=resolve(root,game==='th10'?'cpp/rebuild/third_party/softfloat.c':'cpp/third_party/softfloat.c'),softObject=await compile(soft,'softfloat',true);
const outputs=new Array(sources.length);let next=0,done=0;
const buildJobs=Math.max(1,Math.min(16,Number.parseInt(process.env.TH_BUILD_JOBS||'4',10)||4));
await Promise.all(Array.from({length:buildJobs},async()=>{while(next<sources.length){const i=next++;outputs[i]=await compile(resolve(root,sources[i]),sources[i].replaceAll('/','_'));if(++done%40===0)console.log(done+'/'+sources.length+' translation units');}}));
const output=resolve(out,game+'-sdl.mjs');
const hostImports=[];
const library=resolve(out,'browser-services.js');writeFileSync(library,'addToLibrary({\n'+hostImports.map(i=>`${JSON.stringify(i.name)}: function() { return Module['services'][${JSON.stringify(i.module)}][${JSON.stringify(i.name)}].apply(null, arguments); }`).join(',\n')+'\n});\n');
await run([...flags,'--emit-symbol-map','--js-library',library,'-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=67108864','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...outputs,rendererObject,softObject,'-o',output]);
const wasm=readFileSync(output.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm),sha=x=>createHash('sha256').update(x).digest('hex');
const sourceFiles=[...sources.map(p=>resolve(root,p)),...headers(resolve(root,'cpp')),...headers(shared),...headers(numeric),...headers(input),renderer,soft,resolve(workspace,'portable',game+'-services.json'),...(game==='th08'?[resolve(root,'cpp/game/THPRAC-LICENSE.txt')]:[]),fileURLToPath(import.meta.url)].sort();
const inventory=Object.fromEntries(sourceFiles.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]));
for(const [path,expected] of capturedInputs)if(sha(readFileSync(path))!==expected)throw Error('Source changed during compilation; do not publish this mixed build. Rebuild: '+path);
const sdkMetadata=resolve(sdk,'touhou-sdk.json');
const toolchain=existsSync(sdkMetadata)?JSON.parse(readFileSync(sdkMetadata)):{emsdkRoot:relative(workspace,sdk).replaceAll('\\','/'),layout:'external'};
const report={game,kind:'cpp-sdl3',version:game==='th10'?'3.5.1-sdl3':'3.4.1-sdl3',features:{thprac:game==='th08',languages:thcrap,focusHitbox:false},...{architecture:{loop:'cpp-original-cadence-skip-expired-single-tick',audio:'miniaudio-sdl3',renderer:'cpp-gles-semantic-batched',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',files:'sdl-io-idbfs',fonts:'sdl3-ttf',input:'cpp-sdl',launcher:'eagler-touhou/1'}},sdlVersion:'3.4.2',sources,sourceFiles:inventory,sharedSources:['Renderer.cpp','Renderer.hpp','Shaders.hpp','GraphicsState.hpp','AssetPixelFormat.hpp','RenderCommands.hpp','LegacyGraphics.hpp','ExactFloat.hpp','MotionTrack.hpp'],bytes:wasm.length,sha256:sha(wasm),loaderSha256:sha(readFileSync(output)),imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),toolchain};
writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({game,bytes:wasm.length,sha256:report.sha256,output},null,2));
