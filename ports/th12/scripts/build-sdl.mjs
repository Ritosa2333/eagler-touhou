// Compile the shared SDL3 frontend. Does not execute the resulting application.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {emscripten} from './toolchain.mjs';
import {sdlBuildInputs} from './sdl-build-inputs.mjs';
const root=path.resolve(import.meta.dirname,'..'),web=process.argv.includes('--web');
const option=(name,fallback)=>process.argv.find(value=>value.startsWith(name+'='))?.slice(name.length+1)??fallback;
const build=path.resolve(option('--build-dir',path.join(root,'artifacts',web?'sdl-web':'sdl-desktop')));
const cmake=process.env.CMAKE??(process.platform==='win32'&&fs.existsSync('C:/mingw64/bin/cmake.exe')?'C:/mingw64/bin/cmake.exe':'cmake');
const practice=process.env.TH_ENABLE_THPRAC!=='0'&&process.env.TH12_THPRAC!=='0';
const env={...process.env};const args=['-S',root,'-B',build,'-G',option('--generator','Ninja'),'-DCMAKE_BUILD_TYPE=Release','-DBUILD_TESTING=OFF','-DTH12_BUILD_SDL_HOST=ON','-DTH12_ENABLE_THPRAC='+(practice?'ON':'OFF'),'-DTH12_FETCH_SDL_DEPS=ON'];
const sources=sdlBuildInputs(root);
if(process.env.EAGLER_COMMON_ROOT)args.push('-DEAGLER_COMMON_ROOT='+path.resolve(process.env.EAGLER_COMMON_ROOT));
fs.mkdirSync(build,{recursive:true});
if(web){
  const sdk=emscripten(root);env.EMSDK=sdk.sdk;env.EMSDK_PYTHON=sdk.python;env.EM_CONFIG=env.EM_CONFIG??path.join(sdk.sdk,'.emscripten');
  env.EM_CACHE=env.EM_CACHE??path.join(build,'emscripten-cache');env.EMCC_TEMP_DIR=path.join(build,'emscripten-temp');
  fs.mkdirSync(env.EMCC_TEMP_DIR,{recursive:true});env.TMP=env.TEMP=env.EMCC_TEMP_DIR;
  args.push('-DCMAKE_TOOLCHAIN_FILE='+path.join(path.dirname(sdk.compiler),'cmake/Modules/Platform/Emscripten.cmake'),'-DPython3_EXECUTABLE='+sdk.python);
}
for(const [variable,value]of Object.entries({FETCHCONTENT_SOURCE_DIR_TH12_SDL3:process.env.TH12_SDL3_SOURCE,FETCHCONTENT_SOURCE_DIR_TH12_FREETYPE:process.env.TH12_FREETYPE_SOURCE,CMAKE_PREFIX_PATH:process.env.TH12_CMAKE_PREFIX}))if(value)args.push('-D'+variable+'='+path.resolve(value));
function run(args){const result=spawnSync(cmake,args,{cwd:root,env,stdio:'inherit',windowsHide:true});if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);}
run(args);run(['--build',build,'--config','Release','--target','touhou12','--parallel',option('--jobs',process.env.TH_BUILD_JOBS??'6')]);
if(JSON.stringify(sdlBuildInputs(root))!==JSON.stringify(sources))throw Error('SDL 源码在编译期间改变，请重新编译');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const artifacts={};
if(web){
  const output=path.join(root,'site/runtime/sdl');fs.mkdirSync(output,{recursive:true});
  for(const name of ['touhou12.mjs','touhou12.wasm']){const bytes=fs.readFileSync(path.join(build,name));fs.writeFileSync(path.join(output,name),bytes);artifacts[name]={bytes:bytes.length,sha256:sha(bytes)};}
  fs.writeFileSync(path.join(output,'build.json'),JSON.stringify({schema:'th12-sdl-build/1',platform:'web',features:{thprac:practice,languages:true},builtAt:new Date().toISOString(),sourceSha256:sha(JSON.stringify(sources)),sources,artifacts},null,2)+'\n');
}
console.log(JSON.stringify({platform:web?'web':'desktop',build,applicationExecuted:false,testsExecuted:false,artifacts}));
