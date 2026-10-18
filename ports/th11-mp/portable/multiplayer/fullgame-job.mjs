// Dedicated background owner for rollback-full-game.mjs. No arbitrary command,
// shell, automatic retry, second browser attempt, or kill-by-name is supported.
// Usage: node portable/multiplayer/fullgame-job.mjs --job-dir <fresh-dir>
//   --runtime <package> --launcher-root <companion> --chrome <installed-browser>
//   --minutes 90 [--extra] [--cpu 1] [--route rtc] [--smoke-seconds 15]
import assert from 'node:assert/strict';
import {spawn,spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,openSync,closeSync,renameSync,statSync,existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {performance} from 'node:perf_hooks';

const root=resolve(import.meta.dirname,'../..'),self=import.meta.filename;
const runner=resolve(import.meta.dirname,'rollback-full-game.mjs');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const identity=path=>({path,sha256:sha(readFileSync(path))});
const now=()=>new Date().toISOString();
const json=(path,value,flag='w')=>writeFileSync(path,JSON.stringify(value,null,2)+'\n',{flag});
const file=path=>assert.ok(statSync(path).isFile(),'Required file: '+path);
const directory=path=>assert.ok(statSync(path).isDirectory(),'Required directory: '+path);
// Raw stderr bytes: fixed prefix plus a ring containing the newest suffix.
// No per-line parsing, unbounded concatenation or logging in the game runner.
function boundedBrowserStderr(){
  const headLimit=2*1024*1024,tailLimit=6*1024*1024;
  const head=Buffer.allocUnsafe(headLimit),tail=Buffer.allocUnsafe(tailLimit);
  let totalBytes=0,headBytes=0,tailBytes=0,next=0;
  const status={streamEnded:false,childClosed:false,forcedStop:false,streamError:null};
  return {status,
    push(chunk){
      totalBytes+=chunk.length;
      const n=Math.min(chunk.length,headLimit-headBytes);
      if(n){chunk.copy(head,headBytes,0,n);headBytes+=n;}
      const rest=chunk.subarray(n);if(!rest.length)return;
      if(rest.length>=tailLimit){rest.copy(tail,0,rest.length-tailLimit);tailBytes=tailLimit;next=0;return;}
      const first=Math.min(rest.length,tailLimit-next);rest.copy(tail,next,0,first);
      if(first<rest.length)rest.copy(tail,0,first);
      next=(next+rest.length)%tailLimit;tailBytes=Math.min(tailLimit,tailBytes+rest.length);
    },
    summary(){const discardedBytes=totalBytes-headBytes-tailBytes;return {
      schema:'th11-fullgame-browser-stderr/1',debugNamespace:'pw:browser',headLimit,tailLimit,
      totalBytes,retainedBytes:headBytes+tailBytes,headBytes,tailBytes,discardedBytes,truncated:discardedBytes>0,
      layout:'stderr.log contains raw head then raw tail; omitted middle has no inserted marker',
      tailSourceOffset:totalBytes-tailBytes,complete:status.streamEnded&&status.childClosed&&!status.forcedStop&&!status.streamError,
      ...status};},
    write(fd){
      if(headBytes)writeFileSync(fd,head.subarray(0,headBytes));
      const start=tailBytes===tailLimit?next:0,first=Math.min(tailBytes,tailLimit-start);
      if(first)writeFileSync(fd,tail.subarray(start,start+first));
      if(first<tailBytes)writeFileSync(fd,tail.subarray(0,tailBytes-first));
    },
  };
}
const valueOptions=new Set(['--job-dir','--runtime','--launcher-root','--chrome','--minutes','--smoke-seconds',
  '--cpu','--route','--latency-ms','--jitter-ms','--max-gap-ms','--music-root','--playwright-root','--gpu']);
function argumentsFor(tokens){
  const options={};
  for(let i=0;i<tokens.length;++i){
    const name=tokens[i];
    assert.ok(name==='--extra'||name==='--gpu-diagnostics'||valueOptions.has(name),'Unsupported argument: '+name);
    assert.ok(!(name in options),'Duplicate argument: '+name);
    if(name==='--extra'||name==='--gpu-diagnostics'){options[name]=true;continue;}
    const value=tokens[++i];
    assert.ok(typeof value==='string'&&value.length&&!value.startsWith('--')&&!value.includes('\0'),'Missing/invalid '+name);
    options[name]=value;
  }
  for(const name of ['--job-dir','--runtime','--launcher-root','--chrome'])assert.ok(options[name],'Required '+name);
  assert.ok('--minutes' in options||'--smoke-seconds' in options,'Explicit --minutes or --smoke-seconds is required');
  const number=(name,fallback,min,max)=>{
    const n=Number(options[name]??fallback);
    assert.ok(Number.isFinite(n)&&n>=min&&n<=max,'Invalid '+name);return n;
  };
  const minutes=number('--minutes',90,10,90),smoke=number('--smoke-seconds',0,0,120);
  assert.ok(smoke===0||smoke>=15,'--smoke-seconds must be 0 or 15..120');
  assert.ok('--minutes' in options||smoke>0,'A full run requires explicit --minutes');
  const cpu=number('--cpu',1,1,4);assert.ok(cpu===1||cpu===4,'--cpu must be 1 or 4');
  const route=options['--route']??'rtc';assert.ok(['rtc','direct','relay'].includes(route),'Invalid --route');
  const gpu=options['--gpu']??'hardware';assert.ok(['hardware','swiftshader'].includes(gpu),'Invalid --gpu');
  const job=resolve(options['--job-dir']),run=resolve(job,'run');
  const paths={
    '--runtime':resolve(options['--runtime']),'--launcher-root':resolve(options['--launcher-root']),
    '--chrome':resolve(options['--chrome']),
    '--music-root':resolve(options['--music-root']??process.env.TH11_MUSIC_ROOT??resolve(root,'../../games/web-content/th11/music')),
    '--playwright-root':resolve(options['--playwright-root']??process.env.TH11_PLAYWRIGHT_ROOT??resolve(root,'../../eagler-touhou/th11/tools/diagnostics')),
  };
  const args=['--minutes',String(minutes),'--smoke-seconds',String(smoke),'--cpu',String(cpu),'--route',route,'--gpu',gpu,
    '--latency-ms',String(number('--latency-ms',50,0,200)),'--jitter-ms',String(number('--jitter-ms',5,0,25)),
    '--max-gap-ms',String(number('--max-gap-ms',100,30,1000)),
    ...Object.entries(paths).flatMap(([k,v])=>[k,v]),...(options['--extra']?['--extra']:[]),
    ...(options['--gpu-diagnostics']?['--gpu-diagnostics']:[]),'--output',run];
  return {job,run,args,minutes,smoke,paths};
}
function preflight(parsed,archive){
  file(self);file(runner);file(archive);file(parsed.paths['--chrome']);directory(parsed.paths['--music-root']);
  for(const name of ['manifest.json','runtime-files.json','th11-sdl.wasm','th11-sdl.mjs','shell.mjs'])file(resolve(parsed.paths['--runtime'],name));
  const manifest=JSON.parse(readFileSync(resolve(parsed.paths['--runtime'],'manifest.json'),'utf8'));
  assert.equal(manifest.product,'th11mp');assert.equal(manifest.variant,'multiplayer');
  for(const name of ['services/netplay/netplay-relay.mjs','launcher/web/lib/contracts/product-catalog.mjs',
    'launcher/web/.cache/build/browser/assets/launcher/multiplayer-relay-url.mjs',
    'launcher/web/.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs'])file(resolve(parsed.paths['--launcher-root'],name));
  file(resolve(parsed.paths['--playwright-root'],'node_modules/playwright/package.json'));
  // Reject silently drifting timeout semantics when the owned runner changes.
  assert.ok(readFileSync(runner,'utf8').includes('const totalBudgetMs=(300+(smokeSeconds||duration))*1000;'),
    'Runner watchdog formula changed; review this dedicated owner before launch');
}
async function launch(tokens){
  const parsed=argumentsFor(tokens);
  const archive=resolve(process.env.TH11_TEST_ARCHIVE??resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
  preflight(parsed,archive);
  assert.ok(!existsSync(parsed.job),'Job directory must be fresh: '+parsed.job);
  mkdirSync(dirname(parsed.job),{recursive:true});mkdirSync(parsed.job);
  const config={schema:'th11-fullgame-job/1',createdAt:now(),root,job:parsed.job,run:parsed.run,archive,
    requestedArguments:tokens,inputArguments:['--job-dir',parsed.job,...parsed.args.slice(0,-2)],
    runnerArguments:parsed.args,minutes:parsed.minutes,smokeSeconds:parsed.smoke,
    runnerWatchdogMs:((parsed.smoke||parsed.minutes*60)+300)*1000,
    cleanupGraceMs:45000,terminationBudgetMs:15000,runner:identity(runner),owner:identity(self)};
  const configFile=resolve(parsed.job,'config.json');json(configFile,config,'wx');
  const digest=sha(readFileSync(configFile));
  const output=openSync(resolve(parsed.job,'worker.stdout.log'),'wx'),error=openSync(resolve(parsed.job,'worker.stderr.log'),'wx');
  const record={createdAt:now(),launcherPid:process.pid,workerPid:null,status:'starting',configSha256:digest,job:parsed.job};
  try{
    const worker=spawn(process.execPath,[self,'--worker',parsed.job,digest],{
      cwd:root,env:process.env,detached:true,windowsHide:true,shell:false,stdio:['ignore',output,error]});
    await new Promise((accept,reject)=>{worker.once('spawn',accept);worker.once('error',reject);});
    record.workerPid=worker.pid;record.status='spawned';worker.unref();
  }catch(error){record.status='launch-failed';record.error=String(error);process.exitCode=1;}
  finally{closeSync(output);closeSync(error);json(resolve(parsed.job,'launch.json'),record,'wx');}
  console.log(JSON.stringify({...record,jobStatus:resolve(parsed.job,'job.json'),receipt:resolve(parsed.run,'receipt.json')}));
}
async function work(job,digest){
  job=resolve(job);assert.match(digest,/^[0-9a-f]{64}$/);
  const configBytes=readFileSync(resolve(job,'config.json'));assert.equal(sha(configBytes),digest,'Configuration changed');
  const config=JSON.parse(configBytes);
  assert.equal(config.schema,'th11-fullgame-job/1');assert.equal(config.root,root);assert.equal(config.job,job);
  assert.equal(config.owner.path,self);assert.equal(config.owner.sha256,identity(self).sha256,'Worker source changed before start');
  assert.equal(config.runner.path,runner);assert.equal(config.runner.sha256,identity(runner).sha256,'Runner changed before start');
  const parsed=argumentsFor(config.inputArguments);
  assert.equal(parsed.job,job);assert.equal(parsed.run,config.run);
  assert.deepEqual(parsed.args,config.runnerArguments,'Only validated full-game arguments are allowed');
  assert.equal(config.runnerWatchdogMs,((parsed.smoke||parsed.minutes*60)+300)*1000);
  assert.equal(config.cleanupGraceMs,45000);assert.equal(config.terminationBudgetMs,15000);
  // This one-way claim is never removed. Replaying a worker cannot start a second run.
  json(resolve(job,'worker.claim.json'),{pid:process.pid,startedAt:now(),configSha256:digest},'wx');
  const started=performance.now(),startedWall=Date.now();
  const browserStderr=boundedBrowserStderr();
  const hardBudget=config.runnerWatchdogMs+config.cleanupGraceMs+config.terminationBudgetMs;
  const state={schema:config.schema,status:'starting',workerPid:process.pid,runnerPid:null,startedAt:now(),
    completedAt:null,heartbeatAt:null,elapsedMs:0,runnerWatchdogMs:config.runnerWatchdogMs,
    terminationAt:new Date(startedWall+hardBudget-config.terminationBudgetMs).toISOString(),
    hardDeadline:new Date(startedWall+hardBudget).toISOString(),hardBudgetMs:hardBudget,
    exitCode:null,signal:null,timedOut:false,passed:false,smokeSetupPassed:false,progress:null,cleanup:[]};
  const statusFile=resolve(job,'job.json');
  function save(){
    state.heartbeatAt=now();state.elapsedMs=Math.round(performance.now()-started);
    state.stderrCapture=browserStderr.summary();
    try{json(statusFile+'.tmp',state);renameSync(statusFile+'.tmp',statusFile);}
    catch(error){console.error('Job status write failed: '+String(error));}
  }
  function progress(){
    try{
      const receipt=JSON.parse(readFileSync(resolve(config.run,'receipt.json'),'utf8'));
      state.progress={readAt:now(),status:receipt.status,completedAt:receipt.completedAt,passed:receipt.passed,
        fullRunComplete:receipt.fullRunComplete,smokeOnly:receipt.smokeOnly,smokeSetupPassed:receipt.smokeSetupPassed,
        progress:receipt.progress??null,segments:receipt.segments?.length??0,stageVisits:receipt.stageVisits??[],
        lastStage:receipt.stages?.at(-1)??null,failures:receipt.failures?.length??0,error:receipt.error??null};
      return receipt;
    }catch{return null;} // The unmodified runner may be between truncate and write.
  }
  save();
  let child=null,heartbeat=null,deadline=null,terminal=null,finish=null,output=null,error=null;
  const live=()=>child&&child.pid&&child.exitCode===null&&child.signalCode===null;
  function terminate(reason){
    if(state.terminationReason)return;
    state.terminationReason=reason;state.timedOut=reason==='hard-timeout';state.status='terminating';save();
    if(!live()){finish?.();return;}
    clearTimeout(terminal);
    terminal=setTimeout(()=>{state.cleanupIncomplete=!!live();finish?.();},
      Math.max(0,Math.min(config.terminationBudgetMs,hardBudget-(performance.now()-started))));
    // The runner must still be our active ChildProcess. No saved/stale PID lookup.
    const pid=child.pid,entry={at:now(),pid,reason};state.cleanup.push(entry);
    try{
      if(process.platform==='win32'){
        const taskkill=resolve(process.env.SystemRoot||'C:/Windows','System32/taskkill.exe');
        const result=spawnSync(taskkill,['/PID',String(pid),'/T','/F'],{
          windowsHide:true,shell:false,timeout:10000,encoding:'utf8',maxBuffer:65536});
        entry.method='owned-active-tree-taskkill';entry.exitCode=result.status;entry.signal=result.signal;
        entry.stdout=result.stdout;entry.stderr=result.stderr;if(result.error)entry.error=String(result.error);
        if(result.status!==0&&live())entry.directFallback=child.kill('SIGKILL');
      }else{entry.method='owned-active-process-group';process.kill(-pid,'SIGKILL');}
    }catch(error){entry.error=String(error);if(live()){
      try{entry.directFallback=child.kill('SIGKILL');}catch(e){entry.fallbackError=String(e);}
    }}
    save();
  }
  const interrupted=()=>terminate('SIGTERM'),interruptKey=()=>terminate('SIGINT');
  process.on('SIGTERM',interrupted);process.on('SIGINT',interruptKey);
  try{
    preflight(parsed,config.archive);assert.ok(!existsSync(config.run),'Never reuse an earlier run directory');
    output=openSync(resolve(job,'stdout.log'),'wx');error=openSync(resolve(job,'stderr.log'),'wx');
    const finished=new Promise(accept=>{finish=accept;});
    const childEnv={...process.env,TH11_TEST_ARCHIVE:config.archive};
    // Windows treats environment keys case-insensitively. Replace any inherited
    // spelling instead of accidentally retaining DEBUG=* or pw:protocol.
    for(const key of Object.keys(childEnv))if(key.toLowerCase()==='debug')delete childEnv[key];
    childEnv.DEBUG='pw:browser';
    child=spawn(process.execPath,[runner,...config.runnerArguments],{cwd:root,
      env:childEnv,windowsHide:true,shell:false,
      detached:process.platform!=='win32',stdio:['ignore',output,'pipe']});
    child.stderr.on('data',chunk=>browserStderr.push(chunk));
    child.stderr.once('end',()=>{browserStderr.status.streamEnded=true;});
    child.stderr.once('error',e=>{browserStderr.status.streamError=String(e);});
    child.once('spawn',()=>{state.runnerPid=child.pid;state.status='running';save();});
    child.once('error',e=>{state.error=String(e);finish();});
    child.once('exit',(code,signal)=>{state.exitCode=code;state.signal=signal;});
    // exit can precede the last pipe data. close means stdio has finished too.
    child.once('close',(code,signal)=>{state.exitCode=code;state.signal=signal;browserStderr.status.childClosed=true;finish();});
    heartbeat=setInterval(()=>{progress();save();},5000);
    deadline=setTimeout(()=>terminate('hard-timeout'),Math.max(0,hardBudget-config.terminationBudgetMs-(performance.now()-started)));
    terminal=setTimeout(()=>{state.timedOut=true;state.cleanupIncomplete=!!live();finish();},
      Math.max(0,hardBudget-(performance.now()-started)));
    await finished;
    const receipt=progress();
    state.receiptComplete=!!receipt?.completedAt;
    state.passed=!state.terminationReason&&!state.timedOut&&state.exitCode===0&&state.receiptComplete&&receipt.passed===true;
    state.smokeSetupPassed=!state.terminationReason&&!state.timedOut&&state.exitCode===0&&state.receiptComplete&&
      receipt.smokeOnly===true&&receipt.smokeSetupPassed===true&&receipt.status==='smoke-complete';
    state.status=state.timedOut?'timed-out':state.terminationReason?'interrupted':state.passed?'passed':state.smokeSetupPassed?'smoke-complete':'failed';
    if(!state.passed&&!state.smokeSetupPassed)process.exitCode=1;
  }catch(e){
    state.error=e?.stack||String(e);process.exitCode=1;
    if(live())terminate('worker-error');
    state.status='failed';
  }finally{
    clearInterval(heartbeat);clearTimeout(deadline);clearTimeout(terminal);
    process.removeListener('SIGTERM',interrupted);process.removeListener('SIGINT',interruptKey);
    if(child?.stderr&&!child.stderr.readableEnded){browserStderr.status.forcedStop=true;child.stderr.destroy();}
    try{
      if(error!==null)browserStderr.write(error);
      json(resolve(job,'stderr-capture.json'),{...browserStderr.summary(),savedAt:now(),
        runnerPid:state.runnerPid,exitCode:state.exitCode,signal:state.signal});
      if(browserStderr.status.streamError)throw Error('Runner stderr read failed: '+browserStderr.status.streamError);
    }catch(e){state.stderrCaptureError=String(e);state.error??=String(e);
      state.status='failed';state.passed=false;state.smokeSetupPassed=false;process.exitCode=1;}
    if(output!==null)closeSync(output);if(error!==null)closeSync(error);
    if(child&&live()){state.cleanupIncomplete=true;child.unref();}
    state.completedAt=now();save();console.log(JSON.stringify(state));
  }
}
try{
  const args=process.argv.slice(2);
  if(args[0]==='--worker'){assert.equal(args.length,3,'Invalid internal worker invocation');await work(args[1],args[2]);}
  else await launch(args);
}catch(error){console.error(error?.stack||String(error));process.exitCode=1;}
