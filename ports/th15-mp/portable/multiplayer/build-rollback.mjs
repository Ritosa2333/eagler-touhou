// Relink one private entry translation unit against the production objects.
// The game, journal, allocator and network adapter use the release build inputs.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,existsSync,renameSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),repository=resolve(root,'../..');
// Generic/inline fixtures keep following their release metadata. Scalar hooks
// require the same explicit opt-in as the production build.
const scalarHooks=process.argv.includes('--journal-scalar-hooks');
const explicitInline=process.argv.includes('--journal-inline-dirty-blocks');
const explicitGeneric=process.argv.includes('--journal-native-filter');
assert.ok(Number(scalarHooks)+Number(explicitInline)+Number(explicitGeneric)<=1,'Select only one journal guard');
const out=resolve(root,'web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer');
const dir=resolve(out,'rollback');mkdirSync(dir,{recursive:true});
const sdk=resolve(process.env.TH15_EMSDK||resolve(repository,'toolchains/emsdk'));
const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
assert.ok(compiler,'Set TH15_EMSDK to the release toolchain');
const common=resolve(process.env.EAGLER_COMMON_NETPLAY_ROOT||resolve(repository,'common'));
const build=JSON.parse(readFileSync(resolve(out,'build.json'),'utf8'));
assert.equal(build.kind,'th15-sdl3-application-release');assert.equal(build.features.multiplayer,true);
assert.equal(build.netplay?.instrumentation,0x52424a31);
assert.equal(typeof build.netplay.inlineDirtyBlocks,'boolean');assert.equal(build.netplay.dirtyGuardAbi,0x52424431);
const inlineDirtyBlocks=build.netplay.inlineDirtyBlocks;
if(build.netplay.scalarHooks!==undefined||build.netplay.scalarHookAbi!==undefined){
 assert.equal(typeof build.netplay.scalarHooks,'boolean');assert.equal(build.netplay.scalarHookAbi,1);
}
assert.equal(build.netplay.scalarHooks??false,scalarHooks,'Scalar fixture opt-in must match production metadata (--journal-scalar-hooks)');
assert.ok(!(inlineDirtyBlocks&&scalarHooks),'Scalar hooks and inline dirty guards are separate profiles');
if(explicitInline)assert.equal(inlineDirtyBlocks,true,'Inline fixture opt-in must match production metadata');
if(explicitGeneric)assert.equal(inlineDirtyBlocks,false,'Generic fixture opt-in must match production metadata');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
for(const [name,digest] of Object.entries({...build.sources,...build.headers}))assert.equal(sha(readFileSync(resolve(root,name))),digest,'Release source changed: '+name);
const objects=Object.keys(build.sources).filter(p=>p!=='web/cpp/sdl/BrowserRuntime.cpp').map(p=>resolve(out,'objects',p.replaceAll('/','_')+'.o'));
for(const file of objects)assert.ok(existsSync(file),'Missing release object '+file);
let source=readFileSync(resolve(root,'web/cpp/sdl/BrowserRuntime.cpp'),'utf8');
source+='\n'+readFileSync(resolve(import.meta.dirname,'spectator-stage-lab.inc'),'utf8');
source+='\n'+readFileSync(resolve(import.meta.dirname,'rollback-lab.inc'),'utf8');
source=source.replace(/#include "([^"]+)"/g,(_,p)=>'#include "'+resolve(root,'web/cpp/sdl',p).replaceAll('\\','/')+'"');
writeFileSync(resolve(dir,'fixture.cpp'),source);
const flags=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
 '-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH15_DEVELOPMENT_HARNESS=0','-DTH15_PRESENTATION_LAB=0',
 '-DTH_ENABLE_THPRAC=0','-DTH15_MULTIPLAYER=1','-DTH15_WRITE_JOURNAL=1','-DEAGLER_WRITE_JOURNAL=1','--use-port=sdl3','--use-port=sdl3_ttf',
 '-I'+resolve(common,'include'),'-I'+resolve(root,'web/cpp/multiplayer'),
 '--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker',
 '-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824',
 '-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAP32,HEAPU32',
 '-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2',
 '-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS=_malloc,_free'];
writeFileSync(resolve(dir,'link.rsp'),[...flags,...objects,resolve(dir,'fixture.cpp'),'-o',resolve(dir,'th15.mjs')].map(v=>JSON.stringify(v)).join('\n')+'\n');
const result=spawnSync(process.env.TH_PYTHON||process.env.PYTHON||'python',[compiler,'@'+resolve(dir,'link.rsp')],
 {cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,EMCC_CORES:'1'},encoding:'utf8',windowsHide:true});
if(result.error||result.status)throw result.error||Error(result.stdout+result.stderr);
const instrument=resolve(repository,'tools/rollback/instrument.mjs');
const guard=['--native-filter',...(inlineDirtyBlocks?['--inline-dirty-blocks']:[]),...(scalarHooks?['--scalar-hooks']:[])];
for(const args of [[instrument,...guard,resolve(dir,'th15.wasm'),resolve(dir,'th15.journal.wasm')],[instrument,'--verify',...guard,resolve(dir,'th15.journal.wasm')]]){
 const checked=spawnSync(process.execPath,args,{stdio:['ignore','pipe','inherit'],encoding:'utf8',windowsHide:true});if(checked.error||checked.status)throw checked.error||Error('Fixture instrumentation failed');
 process.stdout.write(checked.stdout);
 if(args.includes('--verify')){
  const verified=JSON.parse(checked.stdout);
  assert.equal(verified.scalarHooks??false,scalarHooks,'Verified scalar profile differs from production metadata');
  assert.equal(verified.inlineDirtyBlocks??false,inlineDirtyBlocks,'Verified inline profile differs from production metadata');
  if(scalarHooks)assert.equal(verified.scalarHookAbi,1);
 }
}
renameSync(resolve(dir,'th15.journal.wasm'),resolve(dir,'th15.wasm'));
const report={schema:'eagler-th15-rollback-fixture/1',builtAt:new Date().toISOString(),productionWasm:build.sha256,
 productionBuiltAt:build.builtAt,productionObjects:objects.length,entrySourceSha256:sha(Buffer.from(source)),
 wasmSha256:sha(readFileSync(resolve(dir,'th15.wasm'))),loaderSha256:sha(readFileSync(resolve(dir,'th15.mjs'))),
 instrumentation:0x52424a31,nativeFilter:true,inlineDirtyBlocks,dirtyGuardAbi:build.netplay.dirtyGuardAbi,scalarHooks,
 ...(build.netplay.scalarHookAbi!==undefined?{scalarHookAbi:build.netplay.scalarHookAbi}:{}),
 oracle:'independent exact-input WASM, semantic scalar trace and native framebuffer bytes'};
writeFileSync(resolve(dir,'build.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({status:'PASS',fixture:dir,...report}));
