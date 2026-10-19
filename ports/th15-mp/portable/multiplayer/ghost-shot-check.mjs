import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..'),sdk=resolve(process.env.TH15_EMSDK||resolve(root,'tools/emsdk'));
const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
const out=resolve(root,'web/artifacts/sdl-multiplayer'),build=JSON.parse(readFileSync(resolve(out,'build.json')));
const objects=Object.keys(build.sources).filter(p=>p.startsWith('web/cpp/game/')).map(p=>resolve(out,'objects',p.replaceAll('/','_')+'.o'));
const loader=resolve(out,'ghost-shot-check.cjs'),rsp=resolve(out,'ghost-shot-check.rsp');
const flags=['-O2','-std=c++17','-DTH_NATIVE_PLATFORM=1','-DTH15_MULTIPLAYER=1','-DTH_ENABLE_THPRAC=0','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','--use-port=sdl3','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=8388608','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824'];
writeFileSync(rsp,[...flags,...objects,resolve(import.meta.dirname,'ghost-shot-check.cpp'),'-o',loader].map(a=>JSON.stringify(a)).join('\n'));
for(const [command,args]of [['python',[compiler,'@'+rsp]],[process.execPath,[loader,resolve(process.env.TH15_TEST_ARCHIVE)]]]){const r=spawnSync(command,args,{cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),TEMP:out,TMP:out},encoding:'utf8',windowsHide:true});if(r.status)throw Error(r.stdout+r.stderr);if(r.stdout)console.log(r.stdout.trim());}
