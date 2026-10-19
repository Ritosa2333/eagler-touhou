// Private font input is embedded only in the local test artifact, never a release.
import {existsSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.EMSDK;
if(!sdk)throw Error('Set EMSDK to the Emscripten SDK');
const compiler=['upstream/emscripten/emcc.py','install/emscripten/emcc.py'].map(name=>resolve(sdk,name)).find(existsSync);
if(!compiler)throw Error('Emscripten compiler is unavailable');
const font=process.env.EAGLER_TEST_FONT||(process.env.WINDIR&&resolve(process.env.WINDIR,'Fonts/msgothic.ttc'));
if(!font||!existsSync(font))throw Error('Set EAGLER_TEST_FONT to a private Gothic font file');
const output=resolve(root,'artifacts/validation-runtime-fonts');mkdirSync(output,{recursive:true});
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG||resolve(sdk,'.emscripten')};
const args=[compiler,'tests/runtime-font-raster-test.cpp','-O2','-UNDEBUG','-std=c++17','-sDEFAULT_TO_CXX=1','--use-port=sdl3','--use-port=sdl3_ttf','-Iinclude','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','--embed-file',font.replaceAll('\\','/')+'@/msgothic.ttc','-o',resolve(output,'font-test.cjs')];
const compiled=spawnSync(process.env.TH_PYTHON||'python',args,{cwd:root,env,stdio:'inherit',windowsHide:true});
if(compiled.status!==0)process.exit(compiled.status||1);
const tested=spawnSync(process.execPath,['-e',`process.on('uncaughtException',error=>{console.error(error.message);process.exit(1)});require(${JSON.stringify(resolve(output,'font-test.cjs'))})`],{cwd:root,stdio:'inherit',windowsHide:true});
process.exitCode=tested.status===0?0:tested.status||1;
