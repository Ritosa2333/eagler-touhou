import {printingRuntimeInputs,printRuntimeInputs,headerInputs} from '../../../../tools/build/runtime-inputs.mjs';
import {spawn} from 'node:child_process';
import {readFileSync,readdirSync,mkdirSync,existsSync,writeFileSync,unlinkSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=fileURLToPath(new URL('../',import.meta.url));
const workspace=resolve(root,'..');
const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT||resolve(workspace,'../../common'));
const sdk=resolve(process.env.TH17_EMSDK||process.env.EMSDK||resolve(workspace,'../../toolchains/emsdk'));
const emcc=['install','upstream'].map(layout=>resolve(sdk,layout,'emscripten/emcc.py')).find(existsSync);
if(!emcc&&!printingRuntimeInputs)throw Error('Set TH17_EMSDK or EMSDK to an Emscripten SDK');
const release=process.argv.includes('--release');
const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const out=resolve(root,process.env.TH17_OUTPUT||(release?'artifacts/sdl-release':'artifacts/sdl-application'));
const objects=resolve(out,'objects');if(!printingRuntimeInputs)mkdirSync(objects,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
// Keep strict floating-point semantics; whole-program optimization can inline
// the small animation/render interfaces without changing replay arithmetic.
const optimize=process.env.TH17_OPTIMIZE!=='baseline';
const common=['-I'+resolve(commonRoot,'include'),...(optimize?['-O3','-flto']:['-O2']),'-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1',`-DTH17_DEVELOPMENT_HARNESS=${release?0:1}`,'--use-port=sdl3'];
// The full object list can exceed Windows' 32K process command limit after extraction.
// Emscripten expands UTF-8 response files before parsing the same compiler flags.
const run=args=>{
 const response=args.length>64?resolve(objects,'link.rsp.utf-8'):null;
 if(response)writeFileSync(response,args.map(a=>JSON.stringify(a)).join('\n')+'\n');
 return new Promise((resolveRun,reject)=>{const p=spawn(process.env.TH_PYTHON||process.env.PYTHON||'python',[emcc,...(response?['@'+response]:args)],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';for(const s of [p.stdout,p.stderr])s.on('data',b=>{log+=b;process.stdout.write(b);});p.on('error',reject);p.on('exit',code=>code?reject(Error('TH17 SDL build failed '+code+'\n'+log)):resolveRun());}).finally(()=>{if(response)unlinkSync(response);});
};
const files=dir=>readdirSync(resolve(root,dir),{withFileTypes:true}).flatMap(e=>e.isDirectory()?files(dir+'/'+e.name):e.name.endsWith('.cpp')?[resolve(root,dir,e.name)]:[]);
const source=[...files('cpp/game'),...files('cpp/sdl'),...(release?[]:[resolve(root,'tests/sdl/application-exports.cpp')]),resolve(workspace,'portable/sdl/Renderer.cpp')];
const headerFiles=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headerFiles(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...headerFiles(resolve(root,'cpp')),...headerFiles(resolve(workspace,'portable/sdl')),...headerFiles(resolve(workspace,'portable/input')),...headerInputs(resolve(commonRoot,'include/eagler/input'),resolve(commonRoot,'browser'))].sort();
await printRuntimeInputs({workspace,root,sources:source,headers,flags:common});
const sha=b=>createHash('sha256').update(b).digest('hex');
const headerHash=sha(headers.map(p=>p+sha(readFileSync(p))).join('\n'));
const settings=JSON.stringify([common,headerHash]);
async function compile(file){const name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_'),object=resolve(objects,name+'.o'),key=sha(settings+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
await run([...common,'-x','c++','-fsyntax-only','-']);
const objectsBuilt=[];let cursor=0;await Promise.all(Array.from({length:4},async()=>{while(cursor<source.length){const i=cursor++;objectsBuilt[i]=await compile(source[i]);}}));
const loader=resolve(out,release?'th17.mjs':'th17-application.mjs');
const exports=['_malloc','_free'];
await run([...common,...(!release&&process.argv.includes('--profile')?['--profiling-funcs']:[]),'--no-entry','-lexports.js','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS='+exports.join(','),...objectsBuilt,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm'));const report={builtAt:new Date().toISOString(),features:{thprac:false,languages:false,focusHitbox:false},compilerOptions:common,kind:release?'th17-sdl3-application-release':'th17-sdl3-application-integration-development',completeGame:release&&target.completeGame===true,bytes:wasm.length,sha256:sha(wasm),architecture:{loop:'cpp-fixed-60hz-bounded-catchup',audio:'miniaudio-sdl3',input:'cpp-sdl3-continuous-recorded-touch',files:'sdl-io-idbfs',launcher:'eagler-touhou/1',renderer:'shared portable/sdl/Renderer.cpp via SDL3/WebGL2',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',fonts:'compiled SDL3 FontDevice with original measured RGB coverage; complete title/run/pause/ending application'},headers:Object.fromEntries(headers.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))])),sources:Object.fromEntries(source.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]))};writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({loader,bytes:wasm.length,sha256:report.sha256}));
