// Compile the real SDL host in every feature combination used by the Lab.
import {execFileSync} from 'node:child_process';
import {existsSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.EMSDK;
if(!sdk)throw Error('Set EMSDK');
const compiler=['install','upstream'].map(part=>resolve(sdk,part,'emscripten/emcc.py')).find(existsSync);
if(!compiler)throw Error('Emscripten compiler not found');
const env={...process.env,TH_ENABLE_THPRAC:'0',TH11_DIAGNOSTICS:'0',TH11_PRESENTATION_LAB:'0',EM_CONFIG:process.env.EM_CONFIG||resolve(sdk,'.emscripten')};
for(const options of [[],['--thprac'],['--presentation-lab']]){
  const input=JSON.parse(execFileSync(process.execPath,[resolve(root,'portable/build.mjs'),'--print-inputs',...options],{env,encoding:'utf8',windowsHide:true}));
  execFileSync(process.env.TH_PYTHON||'python',[compiler,...input.compilerFlags,'-fsyntax-only',resolve(root,'web/cpp/sdl/Application.cpp')],{env,cwd:root,stdio:'inherit',windowsHide:true});
  console.log('PASS actual SDL host:',options.join(' ')||'thprac off');
}
