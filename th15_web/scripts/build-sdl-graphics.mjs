import {spawn} from 'node:child_process';
import {readFileSync,readdirSync,mkdirSync,existsSync,writeFileSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=fileURLToPath(new URL('../',import.meta.url));
const workspace=resolve(root,'..');
const sdk=resolve(workspace,'tools/emsdk');
const release=process.argv.includes('--release');
const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const out=resolve(root,process.env.TH15_OUTPUT||('artifacts/sdl-graphics'));
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'4'};
const common=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1',`-DTH15_DEVELOPMENT_HARNESS=${release?0:1}`,'--use-port=sdl3'];
const run=args=>new Promise((resolveRun,reject)=>{const p=spawn('python',[resolve(sdk,'install/emscripten/emcc.py'),...args],{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';for(const s of [p.stdout,p.stderr])s.on('data',b=>{log+=b;process.stdout.write(b);});p.on('error',reject);p.on('exit',code=>code?reject(Error('TH15 SDL build failed '+code+'\n'+log)):resolveRun());});
const files=dir=>readdirSync(resolve(root,dir),{withFileTypes:true}).flatMap(e=>e.isDirectory()?files(dir+'/'+e.name):e.name.endsWith('.cpp')?[resolve(root,dir,e.name)]:[]);
const source=[...files('cpp/game'),resolve(root,'cpp/sdl/GraphicsDevice.cpp'),resolve(root,'cpp/sdl/FontDevice.cpp'),resolve(root,'cpp/sdl/GraphicsImages.cpp'),resolve(root,'cpp/sdl/PresentationFrame.cpp'),resolve(root,'tests/sdl/graphics-exports.cpp'),resolve(workspace,'portable/sdl/Renderer.cpp')];
const headerFiles=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headerFiles(resolve(dir,e.name)):/\.(h|hpp|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headers=[...headerFiles(resolve(root,'cpp')),...headerFiles(resolve(workspace,'portable/sdl')),...headerFiles(resolve(workspace,'portable/input'))].sort();
const sha=b=>createHash('sha256').update(b).digest('hex');
const headerHash=sha(headers.map(p=>p+sha(readFileSync(p))).join('\n'));
const settings=JSON.stringify([common,headerHash]);
async function compile(file){const name=relative(workspace,file).replaceAll('\\','_').replaceAll('/','_'),object=resolve(objects,name+'.o'),key=sha(settings+sha(readFileSync(file)));if(existsSync(object)&&existsSync(object+'.key')&&readFileSync(object+'.key','utf8')===key)return object;await run([...common,'-c',file,'-o',object]);writeFileSync(object+'.key',key);return object;}
const objectsBuilt=[];let cursor=0;await Promise.all(Array.from({length:4},async()=>{while(cursor<source.length){const i=cursor++;objectsBuilt[i]=await compile(source[i]);}}));
const loader=resolve(out,'th15-graphics.mjs');
await run([...common,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS='+['_graphics_topology','_graphics_dirty','_graphics_world_quad','_malloc','_free','_graphics_initialize','_graphics_texture','_graphics_quad','_graphics_clear','_graphics_pixels','_graphics_stats','_graphics_error','_graphics_frame','_graphics_capture','_graphics_resize','_graphics_text','_graphics_texture_data','_graphics_alpha','_graphics_upload_png','_graphics_clear_texture','_graphics_clear_depth','_graphics_depth_quad','_graphics_screen_initialize','_graphics_screen_pass','_graphics_screen_error','_graphics_screen_paint','_graphics_screen_close','_graphics_screen_capture','_graphics_screen_capture_pending','_graphics_screen_capture_finish','_graphics_screen_capture_texture','_graphics_read_texture','_graphics_texture_format'].join(','),...objectsBuilt,'-o',loader]);
const wasm=readFileSync(loader.replace('.mjs','.wasm'));const report={kind:'th15-sdl3-graphics-comparison-fixture',completeGame:false,bytes:wasm.length,sha256:sha(wasm),architecture:{renderer:'shared portable/sdl/Renderer.cpp via SDL3/WebGL2',graphicsInterface:'semantic-state-texture-matrix',vertexUpload:'web-bufferData-direct-game-batches-cached-vao',fonts:'compiled SDL3 FontDevice with original measured RGB coverage; complete UI not yet connected'},headers:Object.fromEntries(headers.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))])),sources:Object.fromEntries(source.map(p=>[relative(workspace,p).replaceAll('\\','/'),sha(readFileSync(p))]))};writeFileSync(resolve(out,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({loader,bytes:wasm.length,sha256:report.sha256}));
