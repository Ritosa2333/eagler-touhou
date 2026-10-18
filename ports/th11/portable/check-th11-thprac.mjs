// Source/resource proof only. This does not attest UI or gameplay lifecycle.
import {spawnSync} from 'node:child_process';
import {commonInclude} from './common-root.mjs';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const data=process.argv[2],upstream=process.argv[3],sdk=process.env.EMSDK;
if(!data||!upstream||!sdk)throw Error('Usage: EMSDK=<sdk> node portable/check-th11-thprac.mjs <th11.data> <thprac-source>');
const compiler=['install/emscripten/emcc.py','upstream/emscripten/emcc.py'].map(p=>resolve(sdk,p)).find(existsSync);
if(!compiler)throw Error('Emscripten SDK compiler missing');
const out=resolve(root,'th11_web/artifacts/thprac-check');mkdirSync(out,{recursive:true});
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG||resolve(sdk,'.emscripten'),TEMP:out,TMP:out};
function run(command,args){const result=spawnSync(command,args,{cwd:root,env,stdio:'inherit',windowsHide:true});if(result.error)throw result.error;if(result.status!==0)throw Error(command+' failed: '+result.status);}
run(process.execPath,['portable/generate-thprac.mjs',resolve(upstream),'--check']);
const files=['PracticePatcher','PracticeConfig','PracticeReplay','StageCompletion','GameEconomy','Archive','ResourceCrypt','Lzss','EclResource','GameResources','AnmResource','ShtResource','StageResource'].map(n=>'th11_web/cpp/game/'+n+'.cpp');
const target=resolve(out,'check.cjs');
const generate=process.argv.includes('--generate-sites');
run(process.env.TH_PYTHON||'python',[compiler,'-I'+commonInclude,'-O2','-std=c++17',...(generate?['-DTH11_PRACTICE_SITE_GENERATION=1']:[]),'-sDEFAULT_TO_CXX=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=134217728','-sSTACK_SIZE=2097152','portable/check-th11-thprac.cpp',...files,'-o',target]);
run(process.execPath,[target,resolve(data)]);
const sites=spawnSync(process.execPath,[target,resolve(data),'--emit-sites'],{cwd:root,env,encoding:'utf8',windowsHide:true});
if(sites.status!==0)throw Error('Instruction-site generation failed: '+sites.stderr);
if(generate){writeFileSync(resolve(root,'th11_web/cpp/game/PracticeSiteChecks.hpp'),sites.stdout);console.log('Generated purple source-site inventory; rerun without --generate-sites for enforcing proof');process.exit(0);}
if(sites.stdout.replaceAll('\r\n','\n').trimEnd()!==readFileSync(resolve(root,'th11_web/cpp/game/PracticeSiteChecks.hpp'),'utf8').replaceAll('\r\n','\n').trimEnd())throw Error('TH11 practice instruction-site CRC inventory drifted');
console.log('Retail patch-site CRC inventory matches all generated source cases');
