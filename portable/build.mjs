import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,statSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=resolve(fileURLToPath(new URL('../',import.meta.url))),game=process.argv.includes('--th08')?'th08':'th10',root=resolve(workspace,game+'_web');
const presentationLab=process.argv.includes('--presentation-lab'),multiplayer=process.argv.includes('--multiplayer'),printPlan=process.argv.includes('--print-plan');
const fixtures=process.argv.includes('--multiplayer-fixtures');
if(fixtures&&(!multiplayer||game!=='th10'))throw Error('TH10 fixture builds require --multiplayer.');
if(presentationLab&&multiplayer)throw Error('Presentation Lab and multiplayer are separate build variants.');
const profile=fixtures?'multiplayer-fixtures':multiplayer?'multiplayer':presentationLab?'presentation-lab':'sdl3',out=resolve(root,'artifacts',profile);
if(!printPlan)mkdirSync(out,{recursive:true});
const netplayRoot=resolve(workspace,'third_party/eagler-common');
const sdk=process.env.EMSDK??(existsSync(resolve(workspace,'tools/emsdk'))?resolve(workspace,'tools/emsdk'):resolve(workspace,'../toolchains/emsdk'));
const emcc=[resolve(sdk,'install/emscripten/emcc.py'),resolve(sdk,'upstream/emscripten/emcc.py')].find(existsSync);
if(!emcc&&!printPlan)throw Error('Install the pinned Emscripten SDK first (tools/download-emscripten.py).');
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const python=process.env.TH_PYTHON??'python';
const run=(args)=>new Promise((done,reject)=>{const p=spawn(python,[emcc,...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';p.stdout.on('data',x=>{log+=x;process.stdout.write(x);});p.stderr.on('data',x=>{log+=x;process.stderr.write(x);});p.on('error',reject);p.on('exit',code=>code?reject(Error('emcc failed '+code+'\n'+log)):done());});
const thcrap=process.env.TH_ENABLE_THCRAP!=='0';
// Phase A ships the ImGui/thprac infrastructure only. Phase D activates the
// practice runtime/overlay call sites; without the flag every guarded call site
// compiles out and the build is the vanilla baseline.
const thprac=process.env.TH_ENABLE_THPRAC==='1'||process.argv.includes('--thprac');
const imgui=resolve(root,'cpp/third_party/imgui');
const common=['-O2','-g0','-fno-strict-aliasing','-ffp-contract=off','-DTH_SDL3=1','-DTH_NATIVE_PLATFORM=1','-DIMGUI_DISABLE_WIN32_FUNCTIONS','--use-port=sdl3','--use-port=sdl3_ttf','-I'+resolve(workspace,'portable/sdl'),'-I'+imgui,...(thcrap?['-DTH_ENABLE_THCRAP=1']:[]),...(thprac?['-DTH_ENABLE_THPRAC=1']:[])];
if(presentationLab)common.push('-DTH_PRESENTATION_AUDIT=1');
const excluded=new Set(game==='th10'?['LegacyBridge.cpp','LegacyCallbacks.cpp','Exports.cpp','Freestanding.cpp',thcrap?'LocalizationStub.cpp':'Localization.cpp']:['RuntimeExports.cpp']);
const sources=readdirSync(resolve(root,'cpp/game')).filter(n=>n.endsWith('.cpp')&&!excluded.has(n)).map(n=>'cpp/game/'+n);
sources.push(...readdirSync(resolve(root,'cpp/platform')).filter(n=>n.endsWith('.cpp')&&!(multiplayer&&n==='WorldPlayer.cpp')).map(n=>'cpp/platform/'+n));
sources.push(...readdirSync(resolve(root,'cpp/sdl')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/sdl/'+n));
if(multiplayer){
 common.push('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_ENABLE_NETPLAY=1','-I'+resolve(root,'cpp/multiplayer'),'-I'+resolve(netplayRoot,'include'));
 sources.push(...readdirSync(resolve(root,'cpp/multiplayer')).filter(n=>n.endsWith('.cpp')).map(n=>'cpp/multiplayer/'+n));
 sources.push(...['NetplayProtocol','NetplayCore','NetplaySession','SessionChannel','RollbackJournal','BrowserPeerTransport','WebSocketTransport'].map(n=>relative(root,resolve(netplayRoot,'src/netplay',n+'.cpp')).replaceAll('\\','/')));
}
sources.push(...['imgui.cpp','imgui_draw.cpp','imgui_freetype.cpp','imgui_tables.cpp','imgui_widgets.cpp'].map(n=>'cpp/third_party/imgui/'+n));
// Phase C: the practice config/runtime and the thprac overlay. They compile
// alongside Phase A's ImGui translation units (the generated .inc is textually
// included by PracticeRuntime.cpp); the thprac flag below only advertises the
// feature until Phase D wires the overlay into the game loop.
for(const source of ['cpp/game/PracticeConfig.cpp','cpp/game/PracticeRuntime.cpp','cpp/sdl/ThpracUi.cpp'])
 if(!sources.includes(source))sources.push(source);
const shared=resolve(workspace,'portable/sdl'),numeric=resolve(workspace,'portable/numeric'),input=resolve(workspace,'portable/input'),renderer=resolve(shared,'Renderer.cpp');
function headers(dir){return readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?(e.name==='multiplayer'&&!multiplayer?[]:headers(resolve(dir,e.name))):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);}
const flags=[...common,'-std=c++17','-fno-exceptions','-fno-rtti'],linkFlags=multiplayer?['-lwebsocket']:[];
// Test fixture setup is linked only into a diagnostic binary in a separate
// output directory. No fixture mutation exports enter the normal MP Runtime.
if(fixtures)sources.push('../portable/multiplayer/FixtureExports.cpp');
if(printPlan){console.log(JSON.stringify({game,profile,variant:multiplayer?'multiplayer':'normal',outputDirectory:out,flags,linkFlags,sources},null,2));process.exit(0);}
const netplayHeaders=multiplayer?headers(resolve(netplayRoot,'include')):[];
const capturedInputs=new Map();
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
function observed(path){const bytes=readFileSync(path),value=digest(bytes);if(capturedInputs.has(path)&&capturedInputs.get(path)!==value)throw Error('Source changed during build: '+path);capturedInputs.set(path,value);return bytes;}
observed(fileURLToPath(import.meta.url));
const hash=createHash('sha256');for(const path of [...headers(resolve(root,'cpp')),...headers(shared),...headers(numeric),...headers(input),...netplayHeaders].sort())hash.update(path).update(observed(path));
const prefix=JSON.stringify([flags,hash.digest('hex')]);
// Same flags/native translation units; share the object cache, never the
// linked binary, manifest or source inventory. Build variants sequentially.
const objects=resolve(root,'artifacts',fixtures?'multiplayer':profile,'objects');mkdirSync(objects,{recursive:true});
async function compile(source,name,c=false){const object=resolve(objects,name+'.o'),key=createHash('sha256').update(prefix).update(observed(source)).digest('hex');if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;
 await run([...(c?[...common,'-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5']:flags),'-c',source,'-o',object]);writeFileSync(object+'.key',key);return object;
}
console.log('Build '+game+' C++ / SDL3 / Emscripten');
// Populate SDL's port cache once before parallel translation units use it.
const rendererObject=await compile(renderer,'shared_renderer');
const soft=resolve(root,game==='th10'?'cpp/rebuild/third_party/softfloat.c':'cpp/third_party/softfloat.c'),softObject=await compile(soft,'softfloat',true);
const outputs=new Array(sources.length);let next=0,done=0;
await Promise.all(Array.from({length:4},async()=>{while(next<sources.length){const i=next++;outputs[i]=await compile(resolve(root,sources[i]),sources[i].replaceAll('/','_'));if(++done%40===0)console.log(done+'/'+sources.length+' translation units');}}));
const output=resolve(out,game+'-sdl.mjs');
const hostImports=[];
const library=resolve(out,'browser-services.js');writeFileSync(library,'addToLibrary({\n'+hostImports.map(i=>`${JSON.stringify(i.name)}: function() { return Module['services'][${JSON.stringify(i.module)}][${JSON.stringify(i.name)}].apply(null, arguments); }`).join(',\n')+'\n});\n');
await run([...flags,...linkFlags,'--emit-symbol-map','--js-library',library,'-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=67108864','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...outputs,rendererObject,softObject,'-o',output]);
const wasm=readFileSync(output.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm),sha=x=>createHash('sha256').update(x).digest('hex');
const sourceFiles=[...sources.map(p=>resolve(root,p)),...headers(resolve(root,'cpp')),...headers(shared),...headers(numeric),...headers(input),...netplayHeaders,renderer,soft,resolve(workspace,'portable',game+'-services.json'),...(existsSync(resolve(root,'cpp/game/THPRAC-LICENSE.txt'))?[resolve(root,'cpp/game/THPRAC-LICENSE.txt')]:[]),fileURLToPath(import.meta.url)].sort();
const inventory=Object.fromEntries(sourceFiles.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(observed(p))]));
for(const [path,expected] of capturedInputs)if(digest(readFileSync(path))!==expected)throw Error('Source changed during compilation; do not publish mixed build: '+path);
const sdkMetadata=resolve(sdk,'touhou-sdk.json');
const toolchain=existsSync(sdkMetadata)?JSON.parse(readFileSync(sdkMetadata)):{emsdkRoot:relative(workspace,sdk).replaceAll('\\','/'),layout:'external'};
const report={game,kind:'cpp-sdl3',profile,variant:multiplayer?'multiplayer':'normal',diagnostic:presentationLab,version:game==='th10'?'3.5.1-sdl3':'3.4.0-sdl3',features:{thprac,languages:thcrap,focusHitbox:false},...{architecture:{loop:'cpp-original-cadence-skip-expired-single-tick',audio:'miniaudio-sdl3',renderer:'cpp-gles-semantic-batched',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',files:'sdl-io-idbfs',fonts:'sdl3-ttf',input:'cpp-sdl',launcher:'eagler-touhou/1'}},sdlVersion:'3.4.2',sources,sourceFiles:inventory,sharedSources:['Renderer.cpp','Renderer.hpp','Shaders.hpp','GraphicsState.hpp','AssetPixelFormat.hpp','RenderCommands.hpp','LegacyGraphics.hpp','ExactFloat.hpp','MotionTrack.hpp'],bytes:wasm.length,sha256:sha(wasm),loaderSha256:sha(readFileSync(output)),imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),toolchain};
if(fixtures)report.diagnostic=true;
report.sourceDigest=digest(JSON.stringify(inventory));
writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({game,profile,bytes:wasm.length,sha256:report.sha256,output},null,2));
