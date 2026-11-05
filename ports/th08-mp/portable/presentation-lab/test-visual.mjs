import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {mkdirSync,existsSync} from 'node:fs';
const root=resolve(import.meta.dirname,'../..'),out=resolve(root,'artifacts/presentation-lab/unit');
const sdk=process.env.EMSDK||resolve(root,'../../toolchains/emsdk');
const emcc=resolve(sdk,'upstream/emscripten/emcc.py');
if(!existsSync(emcc))throw Error('Set EMSDK to the pinned SDK');
mkdirSync(out,{recursive:true});
execFileSync(process.env.TH_PYTHON||'python',[emcc,'-O2','-std=c++17','-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti',
    resolve(import.meta.dirname,'test-visual.cpp'),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',resolve(out,'test-visual.cjs')],
    {cwd:root,env:{...process.env,EMSDK:sdk,EM_CONFIG:resolve(sdk,'.emscripten')},stdio:'inherit',timeout:90000});
execFileSync(process.execPath,[resolve(out,'test-visual.cjs')],{stdio:'inherit',timeout:30000});
