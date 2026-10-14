// Preserve each formal build attempt and its exact resulting binary identity.
// Root schedules this command serially; build.mjs owns four bounded workers.
import {spawn,spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync,copyFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..');
const at=process.argv.indexOf('--output');
if(at<0||!process.argv[at+1])throw Error('A fresh --output is required');
const out=resolve(process.argv[at+1]),ordinary=process.argv.includes('--ordinary');
const lto=process.argv.includes('--lto');
const simdExperiment=process.argv.includes('--simd-experiment');
if(existsSync(resolve(out,'receipt.json')))throw Error('Existing receipt; use a new output directory');
mkdirSync(out,{recursive:true});
const hash=b=>createHash('sha256').update(b).digest('hex');
const git=path=>{const x=spawnSync('git',['-C',path,'rev-parse','HEAD'],{encoding:'utf8',windowsHide:true});if(x.status!==0)throw Error(x.stderr);return x.stdout.trim();};
const report={schema:'th11-formal-web-build/1',startedAt:new Date().toISOString(),completedAt:null,
 passed:false,root,output:out,variant:ordinary?'ordinary':'multiplayer',flags:{optimization:lto?'O3':'O2',linkTime:lto,simdExperiment,fastMath:false,fpContract:'off',developmentHarness:0,jobs:4},
 command:[process.execPath,'portable/build.mjs',...(ordinary?[]:['--multiplayer']),...(lto?['--lto']:[]),...(simdExperiment?['--simd-experiment']:[])],exitCode:null,
 head:git(root),commonHead:git(resolve(root,'third_party/eagler-common')),artifacts:{}};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(report,null,2)+'\n');
save();let stdout='',stderr='';
try{
 await new Promise((accept,reject)=>{
  const child=spawn(process.execPath,report.command.slice(1),{cwd:root,env:{...process.env,TH_BUILD_JOBS:'4',EMCC_CORES:'4'},windowsHide:true,stdio:['ignore','pipe','pipe']});
  child.stdout.on('data',b=>{stdout+=b;process.stdout.write(b);});
  child.stderr.on('data',b=>{stderr+=b;process.stderr.write(b);});
  child.once('error',reject);child.once('exit',(code,signal)=>{report.exitCode=code;report.signal=signal;accept();});
 });
 if(report.exitCode!==0)throw Error('Formal build failed; original logs retained');
 const built=resolve(root,'th11_web/artifacts',ordinary?'sdl3':'multiplayer');
 for(const name of ['build.json','th11-sdl.mjs','th11-sdl.wasm','th11-sdl.mjs.symbols']){
  const source=resolve(built,name),bytes=readFileSync(source);copyFileSync(source,resolve(out,name));
  report.artifacts[name]={sha256:hash(bytes),bytes:bytes.length};
 }
 const manifest=JSON.parse(readFileSync(resolve(out,'build.json'),'utf8'));
 if(manifest.sha256!==report.artifacts['th11-sdl.wasm'].sha256||manifest.loaderSha256!==report.artifacts['th11-sdl.mjs'].sha256)throw Error('Build identity mismatch');
 if(manifest.symbolMap?.sha256!==report.artifacts['th11-sdl.mjs.symbols'].sha256)throw Error('Release symbol map identity mismatch');
 report.sourceDigest=manifest.sourceDigest;report.passed=true;
}catch(error){report.error=error?.stack||String(error);process.exitCode=1;}
finally{
 writeFileSync(resolve(out,'stdout.log'),stdout);writeFileSync(resolve(out,'stderr.log'),stderr);
 report.completedAt=new Date().toISOString();save();console.log('Formal build receipt: '+resolve(out,'receipt.json'));
}
