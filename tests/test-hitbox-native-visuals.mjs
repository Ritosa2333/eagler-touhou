/** Private workspace component acceptance. Runs stock ANM assets through each
 * title's real native interpreter beside the display adapter. This proves
 * visual state/geometry and redraw/RNG invariants, not a browser pixel pass. */
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync} from 'node:fs';
import {readFile,mkdtemp,rm,stat} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,join,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
import {workspacePath,workspaceRoot} from '../lib/workspace-layout.mjs';
const fixtures=resolve(import.meta.dirname,'native');
const assets={th09:process.env.EAGLER_TH09_HITBOX_ANM,th15:process.env.EAGLER_TH15_HITBOX_ANM,th20:process.env.EAGLER_TH20_HITBOX_ANM};
for(const [game,path] of Object.entries(assets))assert.ok(path&&(await stat(path)).isFile(),`Supply EAGLER_${game.toUpperCase()}_HITBOX_ANM with the original extracted animation resource`);
const sdk=process.env.EMSDK||resolve(workspaceRoot(),'toolchains/emsdk');
const wasiSdk=process.env.EAGLER_WASI_SDK||resolve(workspaceRoot(),'toolchains/wasi-sdk-34.0-x86_64-windows');
const emcc=['upstream','install'].map(layout=>resolve(sdk,layout,'emscripten/emcc.py')).find(existsSync);
assert.ok(emcc,'Emscripten compiler is required');
const work=await mkdtemp(join(tmpdir(),'eagler-hitbox-visuals-'));
function run(file,args,options={}){return execFileSync(file,args,{encoding:'utf8',windowsHide:true,timeout:180000,maxBuffer:8*1024*1024,...options});}
try{
 const game15=workspacePath('th15','th15_web/cpp/game'),exe=resolve(work,process.platform==='win32'?'th15.exe':'th15');
 const units=['AnmManager','AnmRegistry','AnmInterpreter','AnmResource','AnmUpdate','AnmVariables','AnmInterpolators','AnmVisualState','AnmCoordinates','AnmStrip','AnmGeometry','AnmTrail','AnmOverlay','AnmSceneEffects','Interpolation'];
 run(process.env.CXX||'g++',['-std=c++17','-O2','-I',game15,resolve(fixtures,'hitbox-th15.cpp'),...units.map(unit=>resolve(game15,unit+'.cpp')),'-o',exe]);
 console.log(run(exe,[resolve(assets.th15)]).trim());
 const game09=workspacePath('th09','th09_web/cpp/game'),wasm=resolve(work,'th09.wasm');
 run(resolve(wasiSdk,'bin',process.platform==='win32'?'clang++.exe':'clang++'),['-std=c++17','-O2','-fno-exceptions','-I',game09,resolve(fixtures,'hitbox-th09.cpp'),...['AnmResource','AnmExecutor','Rng','Timer','GameMath'].map(unit=>resolve(game09,unit+'.cpp')),'-o',wasm]);
 const wasi=new WASI({version:'preview1',args:['check', '/assets/'+resolve(assets.th09).replaceAll('\\','/').split('/').at(-1)],env:{},preopens:{'/assets':resolve(assets.th09,'..')},returnOnExit:true});
 const instance=await WebAssembly.instantiate(await readFile(wasm),wasi.getImportObject());assert.equal(wasi.start(instance.instance),0);
 const root=workspacePath('th20'),program=resolve(work,'th20.cjs');
 const includes=['th20_web/cpp/shim','th20_web/cpp','th20_web/cpp/sdl','portable/sdl','include','native_recovered'].flatMap(path=>['-I',resolve(root,path)]);
 run(process.env.TH_PYTHON||'python',[emcc,'-std=c++20','-O2','-ffp-contract=off','-DTH_SDL3=1','-DTH_NATIVE_PLATFORM=1','-msimd128','-msse','-msse2','-mavx','-include',resolve(root,'th20_web/cpp/shim/msvc_compat.h'),...includes,resolve(fixtures,'hitbox-th20.cpp'),...['sprite_renderer/anm_vm','sprite_renderer/binding','sprite_renderer/animation','ecl_vm/math'].map(path=>resolve(root,'source_reconstruction',path+'.cpp')),'-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-o',program],{env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten')}});
 console.log(run(process.execPath,[program,resolve(assets.th20)]).trim());
 console.log(JSON.stringify({passed:true,scope:'Native animation components with actual stock resources',resources:Object.fromEntries(await Promise.all(Object.entries(assets).map(async([game,path])=>[game,createHash('sha256').update(await readFile(path)).digest('hex')])))}));
}finally{assert.equal(dirname(resolve(work)),resolve(tmpdir()));await rm(work,{recursive:true,force:true});}
