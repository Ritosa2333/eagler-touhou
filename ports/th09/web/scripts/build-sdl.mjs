import {printingRuntimeInputs, printRuntimeInputs, headerInputs} from '../../../../tools/build/runtime-inputs.mjs';
// TH09's authored C++ presentation harness; no original executable, address
// dispatch, analysis candidates, CPU emulator or Windows ABI is linked.
import {spawn} from 'node:child_process';
import {compilerArguments} from '../../../../tools/build/compiler-arguments.mjs';
const compilerArgumentsSource=fileURLToPath(new URL('../../../../tools/build/compiler-arguments.mjs',import.meta.url));
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const buildArgs=process.argv.slice(2);
const emsdkArgIndex=buildArgs.findIndex(arg=>arg==='--emsdk');
const emsdkArgEquals=buildArgs.find(arg=>arg.startsWith('--emsdk='));
let requestedEmsdk=emsdkArgEquals?.slice('--emsdk='.length);
if(emsdkArgIndex>=0){
  if(!buildArgs[emsdkArgIndex+1]||buildArgs[emsdkArgIndex+1].startsWith('--'))throw Error('Missing value for --emsdk');
  requestedEmsdk=buildArgs[emsdkArgIndex+1];
}
if(requestedEmsdk==='')throw Error('Missing value for --emsdk');
const release=buildArgs.includes('--release');
const builtAt=new Date().toISOString();
const profiling=process.env.TH09_PROFILE==='1';
const audioSimd=process.env.TH09_AUDIO_SIMD==='1';
if(release&&profiling)throw Error('Profiling symbols are diagnostic-only');
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..'),commonRoot=resolve(process.env.EAGLER_COMMON_ROOT??resolve(workspace,'../../common'));
if(!existsSync(resolve(commonRoot,'include/eagler/netplay/SessionChannel.hpp')))throw Error('Initialize the pinned eagler-common submodule before building TH09');
const sdkCandidates=[requestedEmsdk,process.env.TH09_EMSDK,process.env.EMSDK,resolve(workspace,'tools/emsdk')].filter(Boolean);
const sdk=sdkCandidates.find(path=>existsSync(resolve(path,'.emscripten'))&&['install','upstream'].some(layout=>existsSync(resolve(path,layout,'emscripten/emcc.py'))&&existsSync(resolve(path,layout,'emscripten/tools')))) ?? (printingRuntimeInputs ? resolve(workspace,'tools/emsdk') : null);
if(!sdk)throw Error('Emscripten SDK not found. Pass --emsdk PATH or set TH09_EMSDK/EMSDK to the Emscripten 6.0.9 root (it must contain .emscripten and install/ or upstream/ emscripten files).');
const emcc=['install','upstream'].map(layout=>resolve(sdk,layout,'emscripten/emcc.py')).find(existsSync);
const out=resolve(root,process.env.TH09_OUTPUT||(release?'artifacts/sdl-release':'artifacts/sdl3')),objects=resolve(out,'objects');if(!printingRuntimeInputs)mkdirSync(objects,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:process.env.EMCC_CORES??'4'};
const common=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-I'+resolve(commonRoot,'include'),'-I'+resolve(root,'cpp/sdl'),'-I'+resolve(root,'cpp/multiplayer'),'-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH09_DEVELOPMENT_HARNESS='+Number(!release),'--use-port=sdl3','--use-port=sdl3_ttf'];
const run=args=>new Promise((accept,reject)=>{const child=spawn(process.env.TH_PYTHON??process.env.PYTHON??'python',[emcc,...compilerArguments(args,{compiler:emcc,output:out})],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';for(const stream of [child.stdout,child.stderr])stream.on('data',b=>{log+=b;process.stdout.write(b);});child.on('error',reject);child.on('exit',code=>code?reject(Error('TH09 Emscripten build failed '+code+'\n'+log)):accept());});
const source=[];for(const dir of ['cpp/game','cpp/sdl','cpp/multiplayer'])for(const name of readdirSync(resolve(root,dir)).filter(n=>n.endsWith('.cpp')).sort())source.push(resolve(root,dir,name));for(const name of ['BrowserPeerTransport','NetplayCore','NetplayProtocol','NetplaySession','SessionChannel','RollbackJournal'])source.push(resolve(commonRoot,'src/netplay',name+'.cpp'));source.push(resolve(workspace,'portable/sdl/Renderer.cpp'));
const allHeaders=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?allHeaders(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...allHeaders(resolve(root,'cpp')),...allHeaders(resolve(workspace,'portable/sdl')),...allHeaders(resolve(workspace,'portable/input')),...allHeaders(resolve(commonRoot,'include'))].sort();
await printRuntimeInputs({workspace,root,sources:source,headers,flags:common});
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),headerHash=createHash('sha256');for(const name of headers)headerHash.update(name).update(readFileSync(name));const settings=JSON.stringify([common,headerHash.digest('hex')]);
// Keep SIMD opt-in and confined to the audio translation unit. The linker
// detects its target features; game logic and SDL ports keep their old flags.
const audioSource=resolve(root,'cpp/sdl/AudioDevice.cpp');
const sourceFlags=file=>audioSimd&&file===audioSource?['-msimd128']:[];
async function compile(file){const flags=sourceFlags(file),name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_'),object=resolve(objects,name+'.o'),key=sha(settings+(flags.length?JSON.stringify(flags):'')+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,...flags,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
// The shared unit warms SDL's cached ports before independent compilations.
const shared=await compile(source.pop()),outputs=new Array(source.length);let cursor=0,done=0;
const requestedJobs=Number(process.env.TH09_BUILD_JOBS||process.env.TH_BUILD_JOBS||4);
if(!Number.isInteger(requestedJobs)||requestedJobs<1)throw Error('TH_BUILD_JOBS must be a positive integer');
const buildJobs=Math.min(4,requestedJobs);
if(!Number.isInteger(buildJobs)||buildJobs<1||buildJobs>4)throw Error('TH09_BUILD_JOBS must be 1..4');
await Promise.all(Array.from({length:buildJobs},async()=>{while(cursor<source.length){const index=cursor++;outputs[index]=await compile(source[index]);if(++done%25===0)console.log(done+'/'+source.length+' source files');}}));
const loader=resolve(out,release?'th09.mjs':'th09-presentation.mjs');
await run([...common,...(profiling?['--profiling-funcs']:[]),'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...outputs,shared,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm);
if(release&&WebAssembly.Module.exports(module).some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'))throw Error('Development entry point in release');
writeFileSync(resolve(out,'build.json'),JSON.stringify({kind:release?'th09-native-web-release-candidate':'th09-presentation-harness',builtAt,completeGame:false,audioSimd,compilerFlags:common,sourceCompilerFlags:Object.fromEntries(source.filter(file=>sourceFlags(file).length).map(file=>[relative(workspace,file).replaceAll('\\','/'),sourceFlags(file)])),sha256:sha(wasm),bytes:wasm.length,imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),sources:Object.fromEntries([...source,...headers,compilerArgumentsSource,fileURLToPath(import.meta.url),resolve(workspace,'portable/sdl/Renderer.cpp')].map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]))},null,2)+'\n');
console.log(JSON.stringify({loader,bytes:wasm.length,sha256:sha(wasm)}));
