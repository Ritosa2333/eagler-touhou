import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,existsSync,writeFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
const root=resolve(import.meta.dirname,'..'),data=process.argv[2],upstream=process.argv[3],sdk=process.env.TH15_EMSDK;
if(!data||!upstream||!sdk)throw Error('Usage: TH15_EMSDK=<sdk> node portable/check-th15-thprac.mjs <original.dat> <thprac_purple> [--generate-sites]');
const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
if(!compiler)throw Error('Emscripten compiler missing');
const out=resolve(root,'th15_web/artifacts/thprac-check');mkdirSync(out,{recursive:true});
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),TEMP:out,TMP:out};
function run(command,args,capture=false){const result=spawnSync(command,args,{cwd:root,env,windowsHide:true,encoding:'utf8',stdio:capture?'pipe':'inherit'});if(result.error)throw result.error;if(result.status!==0)throw Error(command+' failed: '+result.status+'\n'+(result.stderr||'')+(result.stdout||''));return result.stdout;}
run(process.execPath,['portable/generate-thprac.mjs',resolve(upstream),'--check']);
run(process.execPath,['portable/generate-thprac-tools.mjs',resolve(upstream),'--check']);
run(process.execPath,['portable/check-th15-thprac-native.mjs',resolve(dirname(data),'th15.exe')]);
const sources=['PracticePatcher','PracticeProgram','PracticeConfig','PracticeReplay','Archive','ResourceCrypt','Lzss','EclResource','MusicLayout'].map(n=>'th15_web/cpp/game/'+n+'.cpp');
const compile=(target,generate=false)=>run('python',[compiler,'-O2','-std=c++17','-sDEFAULT_TO_CXX=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=134217728','-sSTACK_SIZE=2097152',...(generate?['-DTH15_PRACTICE_SITE_GENERATION=1']:[]),'portable/check-th15-thprac.cpp',...sources,'-o',target]);
if(process.argv.includes('--generate-sites')){
 const target=resolve(out,'sites.cjs');compile(target,true);
 const sites=run(process.execPath,[target,resolve(data),'--emit-sites'],true);
 // Mechanical code generation, matching TH11's CRC inventory owner.
 writeFileSync(resolve(root,'th15_web/cpp/game/PracticeSiteChecks.hpp'),sites);
}
const target=resolve(out,'check.cjs');compile(target);run(process.execPath,[target,resolve(data)]);
const sites=run(process.execPath,[target,resolve(data),'--emit-sites'],true);
if(sites.trimEnd()!==readFileSync(resolve(root,'th15_web/cpp/game/PracticeSiteChecks.hpp'),'utf8').replaceAll('\r\n','\n').trimEnd())throw Error('Purple TH15 instruction-site inventory drift');
console.log('Purple source extraction and retail instruction-site inventory verified');
const gameplay=resolve(out,'gameplay.cjs');
run('python',[compiler,'-O2','-std=c++17','-sDEFAULT_TO_CXX=1','-sNODERAWFS=1','portable/check-th15-thprac-gameplay.cpp',...['SpellCard','SpellDraw','EnemyInterrupts','ChapterCheckpoint','SessionRuntime'].map(n=>'th15_web/cpp/game/'+n+'.cpp'),'-o',gameplay]);
run(process.execPath,[gameplay]);
run(process.execPath,['portable/check-th15-thprac-menus.mjs']);
const sharedTools=resolve(out,'tools.cjs');
run('python',[compiler,'-O2','-std=c++17','-sDEFAULT_TO_CXX=1','portable/check-th15-thprac-tools.cpp','-o',sharedTools]);
run(process.execPath,[sharedTools]);
