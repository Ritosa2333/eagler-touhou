// Private native restoration fixture. Production BrowserRuntime is unchanged.
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..');
const out=resolve(root,'web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer');
const dir=resolve(out,'spectator-stage');mkdirSync(dir,{recursive:true});
const sdk=resolve(process.env.TH15_EMSDK);
const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
if(!compiler)throw Error('Set TH15_EMSDK');
const dependency=resolve(process.env.EAGLER_COMMON_NETPLAY_ROOT);
const build=JSON.parse(readFileSync(resolve(out,'build.json')));
const objects=Object.keys(build.sources).filter(p=>p!=='web/cpp/sdl/BrowserRuntime.cpp').map(p=>resolve(out,'objects',p.replaceAll('/','_')+'.o'));
let source=readFileSync(resolve(root,'web/cpp/sdl/BrowserRuntime.cpp'),'utf8');
const fastForwardAnchor='const bool fast_forwarding=playback.FastForwarding();';
if(!source.includes(fastForwardAnchor))throw Error('Missing passive-follow pacing anchor');
source=source.replace(fastForwardAnchor,()=>
 'const auto spectator_lab_position=playback.Position();\n  '+fastForwardAnchor);
source=source.replace('app->audio_device.spectator_silent(playback.Paused()||playback.Seeking());',
 'EM_ASM({Module.spectatorLabPlayback=({ticks:$0,elapsed:$1,fastForwarding:!!$2,position:$3,edge:$4});},playback.Position()-spectator_lab_position,emscripten_get_now()-begin,fast_forwarding,playback.Position(),playback.Edge());\n  app->audio_device.spectator_silent(playback.Paused()||playback.Seeking());');
source=source.replace('// Rankings and stored names belong to each browser, after the common ending fence.',
 'EM_ASM({Module.spectatorLabTrace?.($0);},frame+1);\n // Rankings and stored names belong to each browser, after the common ending fence.');
source+='\n'+readFileSync(resolve(import.meta.dirname,'spectator-stage-lab.inc'),'utf8');
source=source.replace(/#include "([^"]+)"/g,(_,p)=>'#include "'+resolve(root,'web/cpp/sdl',p).replaceAll('\\','/')+'"');
writeFileSync(resolve(dir,'fixture.cpp'),source);
const flags=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH15_DEVELOPMENT_HARNESS=0','-DTH_ENABLE_THPRAC=0','-DTH15_MULTIPLAYER=1','--use-port=sdl3','--use-port=sdl3_ttf','-I'+resolve(dependency,'include'),'-I'+resolve(root,'third_party/eagler-common/include'),'-I'+resolve(root,'web/cpp/multiplayer'),'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS=_malloc,_free'];
writeFileSync(resolve(dir,'link.rsp'),[...flags,...objects,resolve(dir,'fixture.cpp'),'-o',resolve(dir,'th15.mjs')].map(v=>JSON.stringify(v)).join('\n'));
const result=spawnSync('python',[compiler,'@'+resolve(dir,'link.rsp')],{cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),TEMP:dir,TMP:dir},encoding:'utf8',windowsHide:true});
if(result.status)throw Error(result.stdout+result.stderr);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const fixtureWasm=readFileSync(resolve(dir,'th15.wasm'));
const fixtureLoader=readFileSync(resolve(dir,'th15.mjs'));
const productionLoader=readFileSync(resolve(out,build.kind==='th15-sdl3-application-release'?'th15.mjs':'th15-application.mjs'));
writeFileSync(resolve(dir,'build.json'),JSON.stringify({schema:'eagler-th15-spectator-stage-fixture/1',game:'th15',variant:'multiplayer',
 privateSpectatorFixture:true,builtAt:new Date().toISOString(),productionWasm:build.sha256,
 productionLoaderSha256:sha(productionLoader),productionBuiltAt:build.builtAt,
 sha256:sha(fixtureWasm),loaderSha256:sha(fixtureLoader),fixtureSourceSha256:sha(readFileSync(resolve(dir,'fixture.cpp'))),
 commonDependency:dependency,commonProtocolSha256:sha(readFileSync(resolve(dependency,'include/eagler/netplay/SpectatorStageProtocol.hpp')))},null,2)+'\n');
console.log(JSON.stringify({status:'PASS',fixture:dir,wasm:sha(fixtureWasm),loader:sha(fixtureLoader),productionWasm:build.sha256}));
