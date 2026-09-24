// TH09's authored C++ presentation harness; no original executable, address
// dispatch, analysis candidates, CPU emulator or Windows ABI is linked.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const release=process.argv.includes('--release');
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..');
const sdkCandidates=[process.env.TH09_EMSDK,resolve(workspace,'tools/emsdk'),resolve(workspace,'../th08/tools/emsdk')].filter(Boolean);
const sdk=sdkCandidates.find(path=>existsSync(resolve(path,'install/emscripten/emcc.py'))&&existsSync(resolve(path,'.emscripten')));
if(!sdk)throw Error('Emscripten SDK not found; set TH09_EMSDK to an installed emsdk directory');
const out=resolve(root,process.env.TH09_OUTPUT||(release?'artifacts/sdl-release':'artifacts/sdl3')),objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const common=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH09_DEVELOPMENT_HARNESS='+Number(!release),'--use-port=sdl3','--use-port=sdl3_ttf'];
const run=args=>new Promise((accept,reject)=>{const child=spawn('python',[resolve(sdk,'install/emscripten/emcc.py'),...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';for(const stream of [child.stdout,child.stderr])stream.on('data',b=>{log+=b;process.stdout.write(b);});child.on('error',reject);child.on('exit',code=>code?reject(Error('TH09 Emscripten build failed '+code+'\n'+log)):accept());});
const source=[];for(const dir of ['cpp/game','cpp/sdl'])for(const name of readdirSync(resolve(root,dir)).filter(n=>n.endsWith('.cpp')).sort())source.push(resolve(root,dir,name));source.push(resolve(workspace,'portable/sdl/Renderer.cpp'));
const allHeaders=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?allHeaders(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...allHeaders(resolve(root,'cpp')),...allHeaders(resolve(workspace,'portable/sdl')),...allHeaders(resolve(workspace,'portable/input'))].sort();
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),headerHash=createHash('sha256');for(const name of headers)headerHash.update(name).update(readFileSync(name));const settings=JSON.stringify([common,headerHash.digest('hex')]);
async function compile(file){const name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_'),object=resolve(objects,name+'.o'),key=sha(settings+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
// The shared unit warms SDL's cached ports before independent compilations.
const shared=await compile(source.pop()),outputs=new Array(source.length);let cursor=0,done=0;
await Promise.all(Array.from({length:4},async()=>{while(cursor<source.length){const index=cursor++;outputs[index]=await compile(source[index]);if(++done%25===0)console.log(done+'/'+source.length+' source files');}}));
const loader=resolve(out,release?'th09.mjs':'th09-presentation.mjs');
await run([...common,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...outputs,shared,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm')),module=new WebAssembly.Module(wasm);
if(release&&WebAssembly.Module.exports(module).some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'))throw Error('Development entry point in release');
writeFileSync(resolve(out,'build.json'),JSON.stringify({kind:release?'th09-native-web-release-candidate':'th09-presentation-harness',completeGame:false,sha256:sha(wasm),bytes:wasm.length,imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),sources:Object.fromEntries([...source,...headers,resolve(workspace,'portable/sdl/Renderer.cpp')].map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]))},null,2)+'\n');
console.log(JSON.stringify({loader,bytes:wasm.length,sha256:sha(wasm)}));

