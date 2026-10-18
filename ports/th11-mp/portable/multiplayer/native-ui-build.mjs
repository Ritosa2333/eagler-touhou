import {readFileSync,writeFileSync,mkdirSync,copyFileSync,existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {execFileSync,spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {responseFileContents} from '../response-file.mjs';
const here=import.meta.dirname,root=resolve(here,'../..');
const index=process.argv.indexOf('--phase'),phase=index<0?'after':process.argv[index+1];
if(!/^[a-zA-Z0-9_-]+$/.test(phase))throw Error('Invalid diagnostic phase');
const out=resolve(root,'artifacts/multiplayer-ui/20261009',phase);mkdirSync(out,{recursive:true});
const sdk=process.env.EMSDK||resolve(root,'../../toolchains/emsdk');
const emcc=['upstream/emscripten/emcc.py','install/emscripten/emcc.py'].map(x=>resolve(sdk,x)).find(existsSync);
const env={...process.env,EMSDK:sdk,EM_CONFIG:resolve(sdk,'.emscripten'),EMCC_CORES:'4'};
const sha=x=>createHash('sha256').update(x).digest('hex');
const evidence={phase,diagnostic:true,transport:false,originalExecutable:false,head:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),fixtureSha256:sha(readFileSync(resolve(here,'native-ui-fixture.cpp'))),sources:{},builds:{}};
for(const mp of (process.argv.includes('--mp-only')?[true]:[false,true])){
 const name=mp?'multiplayer':'ordinary';
 const report=JSON.parse(readFileSync(resolve(root,'th11_web/artifacts',mp?'multiplayer':'sdl3','build.json')));
 const thprac=!mp&&!!report.features?.thprac;
 const plan=JSON.parse(execFileSync(process.execPath,['portable/build.mjs',...(mp?['--multiplayer']:[]),...(thprac?['--thprac']:[]),'--print-plan'],{cwd:root,encoding:'utf8'}));
 const flags=plan.flags.map(x=>x==='-DTH11_DEVELOPMENT_HARNESS=0'?'-DTH11_DEVELOPMENT_HARNESS=1':x);
 const run=args=>{const rsp=resolve(out,name+'.rsp.utf-8');writeFileSync(rsp,responseFileContents(args));const p=spawnSync('python',[emcc,'@'+rsp],{cwd:root,env,encoding:'utf8',windowsHide:true,timeout:600000});writeFileSync(resolve(out,name+'-build.log'),(p.stdout||'')+(p.stderr||''));if(p.error||p.status)throw p.error||Error(p.stdout+p.stderr);};
 if(report.variant!==plan.variant||report.diagnostic||(mp&&report.features?.thprac))throw Error('Diagnostic requires matching production variant and features');
 for(const [file,expected]of Object.entries(report.sourceFiles)){const source=resolve(root,file);const bytes=readFileSync(source);evidence.sources[file]={sha256:sha(bytes),productionBuildSha256:expected};if(sha(bytes)!==expected)throw Error('Rebuild production cache before diagnostic link: '+file);if(/cpp\/game\//.test(file)||file.endsWith('Application.cpp')){const target=resolve(out,'sources',file);mkdirSync(dirname(target),{recursive:true});copyFileSync(source,target);}}
 const objects=plan.sources.filter(x=>!x.endsWith('/Application.cpp')).map(x=>resolve(plan.outputDirectory,'objects',x.replaceAll('\\','_').replaceAll('/','_').replaceAll(':','_').replaceAll('.','_')+'.o'));
 for(const p of objects)if(!existsSync(p))throw Error('Missing cache '+p);
 const app=resolve(out,name+'-application.o');run([...flags,'-c',resolve(here,'native-ui-fixture.cpp'),'-o',app]);
 const output=resolve(out,name+'.mjs');
 run([...flags,...plan.linkFlags,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=2097152','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8','-sEXPORTED_FUNCTIONS=_malloc,_free','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',...objects,app,'-o',output]);
 evidence.builds[name]={flags,features:report.features,objects:objects.length,sha256:sha(readFileSync(output.replace('.mjs','.wasm')))};console.log('Built '+name+' '+output);
}
copyFileSync(resolve(here,'native-ui-fixture.cpp'),resolve(out,'fixture.cpp'));
writeFileSync(resolve(out,'build-evidence.json'),JSON.stringify(evidence,null,2));
