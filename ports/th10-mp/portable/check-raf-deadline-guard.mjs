// Dedicated, opt-in policy test. This runner never builds the game or opens a
// browser. Every invocation creates a new evidence directory, including failures.
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {
  closeSync,existsSync,mkdirSync,openSync,readFileSync,writeFileSync,
} from 'node:fs';
import {basename,dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {WASI} from 'node:wasi';

const runnerPath=fileURLToPath(import.meta.url);
const root=resolve(dirname(runnerPath),'..');
let requestedOutput;
for(let index=2;index<process.argv.length;++index){
  if(process.argv[index]!=='--output-dir'||requestedOutput||!process.argv[index+1])
    throw Error('Usage: node portable/check-raf-deadline-guard.mjs [--output-dir NEW_DIRECTORY]');
  requestedOutput=process.argv[++index];
}
const stamp=new Date().toISOString().replace(/[:.]/g,'-');
const output=resolve(requestedOutput??resolve(root,
  'artifacts/rollback-smoothness/raf-deadline-guard-unit-'+stamp+'-'+process.pid));
mkdirSync(dirname(output),{recursive:true});
mkdirSync(output); // Deliberately reject an existing run; never overwrite evidence.
const write=(name,bytes)=>{
  const path=resolve(output,name);
  mkdirSync(dirname(path),{recursive:true});
  writeFileSync(path,bytes,{flag:'wx'});
  return path;
};
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const common=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'../../common'));
const inputEntries=[
  ['portable/check-raf-deadline-guard.mjs',runnerPath],
  ['portable/check-raf-deadline-guard.cpp',resolve(root,'portable/check-raf-deadline-guard.cpp')],
  ['web/cpp/sdl/RafDeadlineGuard.hpp',resolve(root,'web/cpp/sdl/RafDeadlineGuard.hpp')],
  // The policy test does not compile ApplicationHost. Preserve the adapter
  // paired with this result so a later actual-game test can prove its identity.
  ['web/cpp/sdl/ApplicationHost.cpp',resolve(root,'web/cpp/sdl/ApplicationHost.cpp')],
  ['web/cpp/multiplayer/NetworkFrameSchedule.hpp',
    resolve(root,'web/cpp/multiplayer/NetworkFrameSchedule.hpp')],
  ['portable/sdl/PresentationCadence.hpp',resolve(root,'portable/sdl/PresentationCadence.hpp')],
  ['portable/sdl/FrameCadence.hpp',resolve(root,'portable/sdl/FrameCadence.hpp')],
  ['eagler-common/include/eagler/netplay/PendingFrameSchedule.hpp',
    resolve(common,'include/eagler/netplay/PendingFrameSchedule.hpp')],
  ['eagler-common/include/eagler/netplay/FrameBudget.hpp',
    resolve(common,'include/eagler/netplay/FrameBudget.hpp')],
];
const receipt={
  schema:'th10-raf-deadline-guard-unit-receipt/1',
  startedAt:new Date().toISOString(),finishedAt:null,passed:false,
  scope:'TH10 RAF guard policy with production presentation/debt helpers; no ApplicationHost compilation, browser, renderer, transport, game correctness, performance or physical-display proof',
  root,output,node:process.version,platform:process.platform,architecture:process.arch,
  compiler:null,test:null,sourceBefore:[],sourceAfter:[],sourceUnchanged:false,
  artifacts:[],error:null,
};
const wasmPath=resolve(output,'raf-deadline-guard.wasm');
let primaryError=null;
try{
  for(const [name,path] of inputEntries){
    const bytes=readFileSync(path);
    const snapshot=write('source/'+name,bytes);
    receipt.sourceBefore.push({name,path,snapshot,bytes:bytes.length,sha256:digest(bytes)});
  }
  const executable=process.platform==='win32'?'clang++.exe':'clang++';
  const sdk=process.env.WASI_SDK_PATH??[
    resolve(root,'../toolchains/wasi-sdk-34.0-x86_64-windows'),
    resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows'),
  ].find(path=>existsSync(resolve(path,'bin',executable)));
  if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
  const compiler=resolve(sdk,'bin',executable);
  if(!existsSync(compiler))throw Error('WASI SDK compiler does not exist: '+compiler);
  const args=[
    '--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-I'+resolve(root,'portable'),'-I'+resolve(common,'include'),
    resolve(root,'portable/check-raf-deadline-guard.cpp'),'-o',wasmPath,
  ];
  const compiled=spawnSync(compiler,args,{
    cwd:root,windowsHide:true,encoding:'utf8',maxBuffer:8*1024*1024,
    timeout:120000,stdio:['ignore','pipe','pipe'],
  });
  write('compiler.stdout.txt',compiled.stdout??'');
  write('compiler.stderr.txt',compiled.stderr??'');
  receipt.compiler={executable:compiler,args,status:compiled.status,signal:compiled.signal,
    error:compiled.error?String(compiled.error):null};
  if(compiled.error||compiled.status!==0)
    throw Error('Policy compilation failed; inspect compiler.stderr.txt.');

  let stdoutFd,stderrFd,exitCode=null;
  try{
    stdoutFd=openSync(resolve(output,'test.stdout.json'),'wx');
    stderrFd=openSync(resolve(output,'test.stderr.txt'),'wx');
    const wasi=new WASI({
      version:'preview1',args:[basename(wasmPath)],env:{},returnOnExit:true,
      stdin:0,stdout:stdoutFd,stderr:stderrFd,
    });
    const {instance}=await WebAssembly.instantiate(readFileSync(wasmPath),{
      wasi_snapshot_preview1:wasi.wasiImport,
    });
    exitCode=wasi.start(instance);
  }finally{
    if(stdoutFd!==undefined)closeSync(stdoutFd);
    if(stderrFd!==undefined)closeSync(stderrFd);
    receipt.test={exitCode,report:null};
  }
  const report=JSON.parse(readFileSync(resolve(output,'test.stdout.json'),'utf8').trim());
  receipt.test.report=report;
  if(exitCode!==0||report.schema!=='th10-raf-deadline-guard-unit/1'||
     report.passed!==true||report.failures!==0||!(report.checks>0)||
     !Array.isArray(report.caseNames)||report.caseNames.length!==report.cases)
    throw Error('Policy assertions failed; inspect test.stdout.json and test.stderr.txt.');
}catch(error){
  primaryError=error instanceof Error?error.message:String(error);
}finally{
  try{
    for(const [name,path] of inputEntries){
      const bytes=readFileSync(path);
      receipt.sourceAfter.push({name,path,bytes:bytes.length,sha256:digest(bytes)});
    }
    receipt.sourceUnchanged=receipt.sourceBefore.length===inputEntries.length&&
      receipt.sourceAfter.every((entry,index)=>
        receipt.sourceBefore[index].name===entry.name&&
        receipt.sourceBefore[index].sha256===entry.sha256);
    if(!receipt.sourceUnchanged&&!primaryError)primaryError='Source changed during the policy test.';
  }catch(error){
    receipt.sourceUnchanged=false;
    if(!primaryError)primaryError='Source readback failed: '+String(error);
  }
  for(const name of ['raf-deadline-guard.wasm','compiler.stdout.txt',
    'compiler.stderr.txt','test.stdout.json','test.stderr.txt']){
    const path=resolve(output,name);
    if(existsSync(path)){
      const bytes=readFileSync(path);
      receipt.artifacts.push({name,path,bytes:bytes.length,sha256:digest(bytes)});
    }
  }
  receipt.finishedAt=new Date().toISOString();
  receipt.error=primaryError;
  receipt.passed=!primaryError&&receipt.sourceUnchanged&&receipt.test?.report?.passed===true;
  const receiptPath=write('receipt.json',JSON.stringify(receipt,null,2)+'\n');
  console.log(JSON.stringify({
    passed:receipt.passed,receipt:receiptPath,scope:receipt.scope,
    cases:receipt.test?.report?.cases??null,checks:receipt.test?.report?.checks??null,
    sourceUnchanged:receipt.sourceUnchanged,error:receipt.error,
  },null,2));
  if(!receipt.passed)process.exitCode=1;
}
