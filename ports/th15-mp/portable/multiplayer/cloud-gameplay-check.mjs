// Inputs stay in RUNNER_TEMP. Artifact uploads must not include this directory.
// Frozen real-game observations use the existing production performance driver.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,openSync,closeSync,writeSync,renameSync,rmSync,appendFileSync,existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {runCloudCommand} from '../../../../tools/th15-cloud/cloud-command-runner.mjs';
import os from 'node:os';
const root=resolve(import.meta.dirname,'../../../..');
const out=resolve(root,'artifacts/th15-cloud-gameplay');mkdirSync(out,{recursive:true});
const inputs=resolve(process.env.RUNNER_TEMP||os.tmpdir(),'th15-cloud-inputs');mkdirSync(inputs,{recursive:true});
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const readJson=p=>JSON.parse(readFileSync(p,'utf8'));
const report={schema:'th15-cloud-gameplay/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),host:{platform:process.platform,arch:process.arch,cpu:os.cpus()[0]?.model,logicalCpus:os.cpus().length,node:process.version},requests:[],commands:[],inputs:{},cases:[],status:'running',sixtyHzAccepted:false,scope:{softwareRendererOnly:true,userIntelUhdValidated:false,fullCampaign:false,retailFilesPublished:false}};
const save=()=>writeFileSync(resolve(out,'summary.json'),JSON.stringify(report,null,2)+'\n');
function run(name,program,args,{cwd=root,env={},timeout=600000,required=true}={}){
 const log=resolve(out,name+'.log');
 const result=runCloudCommand(program,args,{cwd,env:{...process.env,...env},log,timeout});
 const row={name,...result};report.commands.push(row);save();
 if(!result.cleanup.verified)throw Error('Owned process cleanup not established: '+name);
 if(required&&(result.error||result.exitCode!==0))throw Error(name+' failed; see '+log);return row;
}
function publicUrl(value){const u=new URL(value);if(u.protocol!=='https:'||u.username||u.password||!/(^|\.)touhou\.vip$/.test(u.hostname))throw Error('Unsupported resource origin '+u.origin);return u;}
async function download(record,target){
 const url=publicUrl(record.url),attempt={url:url.href,target:record.target,expectedBytes:record.bytes,expectedSha256:record.sha256??null};report.requests.push(attempt);save();
 assert.ok(Number.isSafeInteger(record.bytes)&&record.bytes>0&&record.bytes<=128*1024*1024,'Invalid declared input size');
 const path=resolve(inputs,target),partial=path+'.partial';mkdirSync(dirname(path),{recursive:true});
 if(existsSync(path)){const bytes=readFileSync(path);if(bytes.length===record.bytes&&(!record.sha256||sha(bytes)===record.sha256)){attempt.reused=true;return {path,bytes:bytes.length,sha256:sha(bytes),url:url.href};}throw Error('Conflicting existing input '+target);}
 let fd;
 try{
  const response=await fetch(url,{signal:AbortSignal.timeout(180000),redirect:'error'});attempt.httpStatus=response.status;
  if(!response.ok){await response.body?.cancel();throw Error('HTTP '+response.status);}
  fd=openSync(partial,'wx');const hash=createHash('sha256');let count=0;const reader=response.body.getReader();
  for(;;){const next=await reader.read();if(next.done)break;count+=next.value.length;if(count>record.bytes){await reader.cancel();throw Error('Response exceeds declared input size');}hash.update(next.value);writeSync(fd,next.value);}
  closeSync(fd);fd=undefined;const digest=hash.digest('hex');attempt.bytes=count;attempt.sha256=digest;
  assert.equal(count,record.bytes,'Input byte length');if(record.sha256)assert.equal(digest,record.sha256,'Input SHA-256');
  renameSync(partial,path);attempt.verified=true;save();return {path,bytes:count,sha256:digest,url:url.href};
 }catch(error){if(fd!==undefined)closeSync(fd);rmSync(partial,{force:true});attempt.error=String(error);save();throw error;}
}
async function acquire(){
 run('resource-metadata',process.execPath,[resolve(import.meta.dirname,'cloud-resource-metadata.mjs')],{timeout:150000});
 const metadata=readJson(resolve(root,'artifacts/th15-resource-metadata/report.json'));
 for(const [target,name] of [['/th15.dat','th15.dat'],['/msgothic.ttc','msgothic.ttc'],['/unifont.otf','unifont.otf']]){
  for(const site of metadata.sites){const record=site.declaredInputs?.find(f=>f.target===target);if(!record)continue;
   try{report.inputs[name]=await download(record,name);break;}catch{} }
 }
 for(const site of metadata.sites){const source=site.multiplayerRuntimeResources;if(!source)continue;
  const cp=source.resources?.resources?.find(f=>f.path==='/fonts/cp932.bin');if(!cp)continue;
  try{report.inputs.cp932=await download({url:new URL(cp.url,source.url).href,target:cp.path,bytes:cp.bytes},'fonts/cp932.bin');break;}catch{} }
 save();
 if(!report.inputs['th15.dat'])throw Error('BLOCKED: no declared TH15 archive was downloaded and verified');
 return {TH15_TEST_ARCHIVE:report.inputs['th15.dat'].path,TH15_TEST_FONT:report.inputs['msgothic.ttc']?.path??'',TH15_TEST_UNICODE_FONT:report.inputs['unifont.otf']?.path??'',EAGLER_FONT_ROOT:resolve(inputs,'fonts')};
}
try{
 const env=await acquire();report.status='archive-verified';save();
 if(process.argv.includes('--inputs-only')){
  if(process.env.GITHUB_ENV)for(const [key,value] of Object.entries(env))appendFileSync(process.env.GITHUB_ENV,key+'='+value+'\n');
 }else{
  const port=resolve(root,'ports/th15-mp'),binary=resolve(port,'web/artifacts/sdl-multiplayer/visual-gameplay.cjs');
  run('real-archive-opacity-oracle',process.execPath,[binary,env.TH15_TEST_ARCHIVE],{timeout:180000,env});
  report.oracleExecuted=true;save();
  if(!report.inputs['msgothic.ttc']||!report.inputs['unifont.otf']||!report.inputs.cp932)throw Error('BLOCKED: browser input fonts are not complete');
  assert.ok(process.env.EAGLER_CHROME_PATH,'Set EAGLER_CHROME_PATH');
  run('build-rollback-fixture',process.execPath,[resolve(port,'portable/multiplayer/build-rollback.mjs')],{timeout:300000,env});
  report.correctness=[];
  for(const [name,flags] of [['rollback',[]],['lifecycle',['--lifecycle-only']],['presentation',['--presentation-toggle']]]){
   const destination=resolve(out,'correctness-'+name);
   const command=run('correctness-'+name,process.execPath,[resolve(port,'portable/multiplayer/rollback-browser.mjs'),...flags,'--output='+destination],{env,timeout:1200000,required:false});
   const resultPath=resolve(destination,'report.json');const result=existsSync(resultPath)?readJson(resultPath):null;
   report.correctness.push({name,exitCode:command.exitCode,status:result?.status??'no-pass-report',groups:result?.reports?.length??0,
    report:existsSync(resultPath)?{path:resultPath,sha256:sha(readFileSync(resultPath))}:null});save();
  }
  report.correctnessPassed=report.correctness.every(c=>c.exitCode===0&&c.status==='PASS')&&report.correctness.reduce((n,c)=>n+c.groups,0)===7;
  if(!report.correctnessPassed)throw Error('BLOCKED: current candidate failed seven-group correctness gate; no performance acceptance');
  const baseline=resolve(root,'.cloud-baseline');assert.ok(existsSync(resolve(baseline,'ports/th15-mp')),'Missing frozen baseline checkout');
  for(const [name,repo] of [['baseline',baseline],['candidate',root]]){
   run('package-'+name,process.execPath,[resolve(repo,'ports/th15-mp/portable/package-eagler.mjs'),'--multiplayer'],{cwd:repo,env});
  }
  const cases=[];for(const players of [2,3])for(const mode of [1,0,2])for(const variant of ['baseline','candidate'])cases.push({players,mode,variant});
  for(const entry of cases){
   const name=entry.players+'p-mode'+entry.mode+'-'+entry.variant,caseOut=resolve(out,name),repo=entry.variant==='baseline'?baseline:root;
   const result=run(name,process.execPath,[resolve(port,'portable/multiplayer/performance-browser.mjs'),'--players='+entry.players,'--mode='+entry.mode,'--duration=15','--cpu-throttle=4','--renderer=swiftshader','--runtime='+resolve(repo,'ports/th15-mp/build-eagler-multiplayer'),'--output='+caseOut],{env,timeout:300000,required:false});
   const final=resolve(caseOut,'receipt.final.json');const receipt=existsSync(final)?readJson(final):null;
   report.cases.push({...entry,exitCode:result.exitCode,error:result.error,receipt:existsSync(final)?{path:final,sha256:sha(readFileSync(final))}:null,
    status:receipt?.status??'no-final-receipt',valid:result.exitCode===0&&receipt?.observationPassed===true&&receipt?.saveValidationPassed===true,
    metrics:receipt?.results?.map(r=>({seat:r.seat,logicHz:r.metrics.logicHz,presentationHz:r.metrics.presentationHz,callback:r.metrics.callbackWall,presentationHolds:r.metrics.presentWindowHolds,frontierHolds:r.metrics.frontierWindowHolds,rollback:r.metrics.rollbackDelta})),errors:receipt?.errors??[]});save();
  }
  report.status='matrix-completed';if(report.cases.some(c=>!c.valid))process.exitCode=1;
  run('diagnostic-2p-mode0',process.execPath,[resolve(port,'portable/multiplayer/performance-browser.mjs'),'--players=2','--mode=0','--duration=15','--cpu-throttle=4','--renderer=swiftshader','--diagnostic=all','--runtime='+resolve(root,'ports/th15-mp/build-eagler-multiplayer'),'--output='+resolve(out,'diagnostic-2p-mode0')],{env,timeout:300000,required:false});
 }
}catch(error){report.status=String(error).includes('BLOCKED:')?'blocked':'failed';report.error=String(error);process.exitCode=1;}
finally{report.completedAt=new Date().toISOString();save();console.log(JSON.stringify({out,status:report.status,oracleExecuted:report.oracleExecuted??false,cases:report.cases.length,sixtyHzAccepted:false,error:report.error}));}
