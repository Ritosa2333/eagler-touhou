import {spawnSync} from 'node:child_process';
import {readdirSync,existsSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.TH15_EMSDK;
if(!sdk)throw Error('TH15_EMSDK required');
const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
const out=resolve(root,'th15_web/artifacts/thprac-check');mkdirSync(out,{recursive:true});
const sources=readdirSync(resolve(root,'th15_web/cpp/game')).filter(p=>p.endsWith('.cpp')).map(p=>resolve(root,'th15_web/cpp/game',p));
const target=resolve(out,'menus.cjs'),response=resolve(out,'menus.rsp.utf-8');
// Mechanical compiler response file keeps the full typed game link below the
// Windows command-line limit. No SDL/native platform stubs replace menu owners.
writeFileSync(response,['-O1','-std=c++17','-sDEFAULT_TO_CXX=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=2097152',resolve(root,'portable/check-th15-thprac-menus.cpp'),...sources,'-o',target].map(v=>JSON.stringify(v)).join('\n'));
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),TEMP:out,TMP:out};
for(const [command,args] of [['python',[compiler,'@'+response]],[process.execPath,[target]]]){
 const result=spawnSync(command,args,{cwd:root,env,windowsHide:true,stdio:'inherit'});
 if(result.error)throw result.error;if(result.status!==0)throw Error('Menu regression failed: '+result.status);
}
