const builtAt=new Date().toISOString();
import {spawn} from 'node:child_process';
import {readFileSync,readdirSync,mkdirSync,existsSync,writeFileSync,unlinkSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=fileURLToPath(new URL('../',import.meta.url));
const workspace=resolve(root,'..');
const sdk=resolve(process.env.TH15_EMSDK||resolve(workspace,'tools/emsdk'));
const emcc=['install','upstream'].map(layout=>resolve(sdk,layout,'emscripten/emcc.py')).find(existsSync);
if(!emcc)throw Error('Emscripten compiler not found in TH15_EMSDK');
const release=process.argv.includes('--release');
const thprac=process.argv.includes('--thprac');
const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const out=resolve(root,process.env.TH15_OUTPUT||(release?'artifacts/sdl-release':'artifacts/sdl-application'));
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const common=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1',`-DTH15_DEVELOPMENT_HARNESS=${release?0:1}`,'--use-port=sdl3','--use-port=sdl3_ttf'];
common.push(`-DTH_ENABLE_THPRAC=${thprac?1:0}`);
if(thprac)common.push('-DIMGUI_DISABLE_WIN32_FUNCTIONS','-I'+resolve(root,'cpp/third_party/imgui'));
// The full object list can exceed Windows' 32K process command limit after extraction.
// Emscripten expands UTF-8 response files before parsing the same compiler flags.
const run=args=>{
 const response=args.length>64?resolve(objects,'link.rsp.utf-8'):null;
 if(response)writeFileSync(response,args.map(a=>JSON.stringify(a)).join('\n')+'\n');
 return new Promise((resolveRun,reject)=>{const p=spawn('python',[emcc,...(response?['@'+response]:args)],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';for(const s of [p.stdout,p.stderr])s.on('data',b=>{log+=b;process.stdout.write(b);});p.on('error',reject);p.on('exit',code=>code?reject(Error('TH15 SDL build failed '+code+'\n'+log)):resolveRun());}).finally(()=>{if(response)unlinkSync(response);});
};
const files=dir=>readdirSync(resolve(root,dir),{withFileTypes:true}).flatMap(e=>e.isDirectory()?files(dir+'/'+e.name):e.name.endsWith('.cpp')?[resolve(root,dir,e.name)]:[]);
const source=[...files('cpp/game'),...files('cpp/sdl'),...(release?[]:[resolve(root,'tests/sdl/application-exports.cpp')]),resolve(workspace,'portable/sdl/Renderer.cpp')];
if(thprac)source.push(...['imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp'].map(p=>resolve(root,'cpp/third_party/imgui',p)));
const headerFiles=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headerFiles(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...headerFiles(resolve(root,'cpp')),...headerFiles(resolve(workspace,'portable/sdl')),...headerFiles(resolve(workspace,'portable/input'))].sort();
const sha=b=>createHash('sha256').update(b).digest('hex');
const headerHash=sha(headers.map(p=>p+sha(readFileSync(p))).join('\n'));
const settings=JSON.stringify([common,headerHash]);
async function compile(file){const name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_'),object=resolve(objects,name+'.o'),key=sha(settings+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
const objectsBuilt=[];let cursor=0;await Promise.all(Array.from({length:4},async()=>{while(cursor<source.length){const i=cursor++;objectsBuilt[i]=await compile(source[i]);}}));
const loader=resolve(out,release?'th15.mjs':'th15-application.mjs');
const exports=['_malloc','_free',...(release?[]:["_application_initialize","_application_test_alpha_pixels","_application_render_scale","_application_pause_state","_application_test_finish_name","_application_test_game_over","_application_test_death","_application_step","_application_error","_application_state","_application_checkpoint_state","_application_manual_state","_application_title_select","_application_complete_stage","_application_transition_state","_application_music","_application_pixels","_application_graphics_stats","_application_close","_application_keyboard_key","_application_test_protection"])];
await run([...common,...(!release&&process.argv.includes('--profile')?['--profiling-funcs']:[]),'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS='+exports.join(','),...objectsBuilt,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm'));const report={builtAt,kind:release?'th15-sdl3-application-release':'th15-sdl3-application-integration-development',completeGame:release&&target.completeGame===true,features:{thprac,languages:true},bytes:wasm.length,sha256:sha(wasm),architecture:{loop:'cpp-fixed-60hz-bounded-catchup-with-display-presentation',presentation:'immutable-draw-transaction-with-owner-field-interpolation',eaglerCommon:'8316c4f861dedb67e1e0e7be75ddcf0b90f63448',audio:'miniaudio-sdl3',input:'cpp-sdl3-continuous-recorded-touch',files:'sdl-io-idbfs',launcher:'eagler-touhou/1',renderer:'shared portable/sdl/Renderer.cpp via SDL3/WebGL2',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',fonts:'compiled SDL3 FontDevice with original measured RGB coverage; real title/run/pause/ending application; remaining unsupported services fail explicitly'},headers:Object.fromEntries(headers.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))])),sources:Object.fromEntries(source.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]))};writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({loader,features:{thprac,languages:true},bytes:wasm.length,sha256:report.sha256}));
