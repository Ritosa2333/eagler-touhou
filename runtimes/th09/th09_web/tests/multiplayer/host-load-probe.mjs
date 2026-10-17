// Optional host contention evidence for multiplayer diagnostics.
// Samples are retained regardless of game performance; this is not a result filter.
import {cpus} from 'node:os';
import {spawn} from 'node:child_process';
import {performance} from 'node:perf_hooks';

export function startHostLoadProbe(options={}){
 const integer=(name,fallback,min,max)=>{
  const value=options[name]??fallback;
  if(!Number.isInteger(value)||value<min||value>max)throw Error('Invalid host-load option '+name);
  return value;
 };
 const cpuIntervalMs=integer('cpuIntervalMs',1000,250,60000);
 const processIntervalMs=integer('processIntervalMs',5000,1000,60000);
 const limits={cpuSamples:integer('maxCpuSamples',4096,2,65536),
  processSamples:integer('maxProcessSamples',1024,1,8192),
  processRows:integer('maxProcessRows',32768,1,262144),
  rowsPerProcessSample:integer('maxRowsPerProcessSample',512,1,4096),
  stdoutChars:262144,knownProcesses:32,marks:128,errors:64};
 const startedAtUtcMs=Date.now(),startedAt=performance.now();
 const cpuSamples=[],processSamples=[],knownProcesses=[],marks=[],errors=[];
 const overflow={cpuSamples:false,processSamples:false,processRows:false,processRowsPerSample:false,stdout:false,knownProcesses:false,marks:false,errors:false};
 const counts={cpuAttempts:0,processMessages:0,processRowsSeen:0,processRowsStored:0,stdoutBytes:0,stderrBytes:0};
 const processSampler={platform:process.platform,ownNodePid:process.pid,helperPid:null,status:'starting',
  exitCode:null,signal:null,stopRequested:false,stopConfirmed:false};
 let previousCpu=null,cpuTimer=null,helper=null,stdoutBuffer='',discardingLine=false,stopped=false,endedAtUtcMs=null,stopPromise=null;
 let finishClosed;const closed=new Promise(resolve=>{finishClosed=resolve;});
 const elapsed=()=>performance.now()-startedAt;
 const error=kind=>{
  if(errors.length<limits.errors)errors.push({atMs:elapsed(),kind});else overflow.errors=true;
 };
 const mark=label=>{
  if(marks.length<limits.marks)marks.push({atMs:elapsed(),utcMs:Date.now(),label:String(label).slice(0,120)});
  else overflow.marks=true;
 };
 const registerKnownProcess=(pid,role)=>{
  if(!Number.isInteger(pid)||pid<=0){error('invalid-known-process-pid');return;}
  if(knownProcesses.some(row=>row.pid===pid))return;
  if(knownProcesses.length>=limits.knownProcesses){overflow.knownProcesses=true;return;}
  knownProcesses.push({pid,role:String(role).slice(0,80),registeredAtMs:elapsed(),registeredUtcMs:Date.now()});
 };
 const sampleCpu=kind=>{
  ++counts.cpuAttempts;
  try{
   const values=cpus();let idleMs=0,totalMs=0;
   for(const {times} of values){idleMs+=times.idle;totalMs+=times.user+times.nice+times.sys+times.idle+times.irq;}
   const now=elapsed(),prior=previousCpu;
   const deltaIdleMs=prior?idleMs-prior.idleMs:null,deltaTotalMs=prior?totalMs-prior.totalMs:null;
   const comparable=!!prior&&values.length===prior.logicalCpus&&deltaTotalMs>0&&deltaIdleMs>=0&&deltaIdleMs<=deltaTotalMs;
   const row={kind,atMs:now,utcMs:Date.now(),logicalCpus:values.length,idleMs,totalMs,
    intervalMs:prior?now-prior.atMs:null,deltaIdleMs,deltaTotalMs,
    busyFraction:comparable?1-deltaIdleMs/deltaTotalMs:null,
    counterDiscontinuity:!!prior&&(values.length!==prior.logicalCpus||deltaTotalMs<0||deltaIdleMs<0||deltaIdleMs>deltaTotalMs)};
   previousCpu=row;
   if(cpuSamples.length<limits.cpuSamples)cpuSamples.push(row);else overflow.cpuSamples=true;
  }catch{error('os-cpus-failed');}
 };
 const consumeLine=line=>{
  if(!line.trim())return;++counts.processMessages;
  try{
   const value=JSON.parse(line);
   if(value.version!==1||!Array.isArray(value.rows)||!Number.isFinite(value.utcMs)||
      !Number.isFinite(value.durationMs)||!Number.isInteger(value.matched))throw Error('schema');
   if(value.failed)error('get-process-failed');
   if(value.truncated)overflow.processRowsPerSample=true;
   counts.processRowsSeen+=value.rows.length;
   if(processSamples.length>=limits.processSamples){overflow.processSamples=true;return;}
   const rows=[];
   for(const item of value.rows){
    if(!Array.isArray(item)||item.length!==3||!Number.isInteger(item[0])||typeof item[1]!=='string'||
       item[1].length>120||(item[2]!==null&&(!Number.isFinite(item[2])||item[2]<0)))throw Error('row');
    if(counts.processRowsStored>=limits.processRows){overflow.processRows=true;continue;}
    rows.push(item);++counts.processRowsStored;
   }
   processSamples.push({receivedAtMs:elapsed(),receivedUtcMs:Date.now(),sourceUtcMs:value.utcMs,
    sourceDurationMs:value.durationMs,matchingCount:value.matched,sourceRowOverflow:!!value.truncated,
    retainedRowOverflow:rows.length!==value.rows.length,failed:!!value.failed,rows});
  }catch{error('process-sampler-json-or-schema');}
 };
 const onStdout=chunk=>{
  counts.stdoutBytes+=chunk.length;
  // The producer emits one bounded JSON record per line. Do not retain an
  // unlimited partial line if the helper fails to honor that protocol.
  for(const [index,part] of chunk.toString('utf8').split('\n').entries()){
   if(index){if(!discardingLine)consumeLine(stdoutBuffer);stdoutBuffer='';discardingLine=false;}
   if(discardingLine)continue;
   if(stdoutBuffer.length+part.length>limits.stdoutChars){overflow.stdout=true;stdoutBuffer='';discardingLine=true;}
   else stdoutBuffer+=part;
  }
 };
 const snapshot=()=>({
  version:1,startedAtUtcMs,endedAtUtcMs,snapshotAtMs:elapsed(),stopped,cpuIntervalMs,processIntervalMs,
  complete:stopped&&processSampler.stopConfirmed,
  valid:stopped&&processSampler.stopConfirmed&&!errors.length&&!Object.values(overflow).some(Boolean)&&
   cpuSamples.length>=2&&(process.platform!=='win32'||processSamples.length>0),
  scope:'Read-only host CPU counters and candidate process CPU counters. No performance samples are excluded or reweighted.',
  limitations:[
   'Aggregate CPU occupancy does not establish single-core availability, CPU frequency, thermal state, GPU load, or a stable benchmark environment.',
   'Process names are candidates for contention, not proof that a process is compiling. Known harness/relay/helper PIDs are labeled separately; other node processes remain unlabeled candidates.',
   'Process CPU is cumulative lifetime CPU seconds. A first sighting is not CPU consumed during this run; PID reuse or counter reset can break a delta.',
   'The 5 s process snapshots miss short tasks that start and exit between snapshots. Timer delays and source/receive times are retained.',
   'This observer has overhead. Its own PowerShell helper is intentionally not a candidate process, but its work is included in aggregate CPU counters.',
   'Overflow and sampling failures make this evidence incomplete; they are never a reason to silently remove a game result.'
  ],
  processRowFields:['pid','name','cpuSeconds'],processSampler:{...processSampler},limits:{...limits},
  counts:{...counts},overflow:{...overflow},cpuSamples:cpuSamples.slice(),processSamples:processSamples.slice(),
  knownProcesses:knownProcesses.slice(),marks:marks.slice(),errors:errors.slice()
 });
 registerKnownProcess(process.pid,'harness-node');mark('probe-start');sampleCpu('initial');
 cpuTimer=setInterval(()=>sampleCpu('periodic'),cpuIntervalMs);cpuTimer.unref?.();
 const killOwnedHelper=()=>{
  if(helper&&helper.exitCode===null&&helper.signalCode===null){try{helper.kill();}catch{error('owned-sampler-stop-failed');}}
 };
 const onParentExit=()=>{if(helper&&helper.exitCode===null&&helper.signalCode===null){try{helper.kill();}catch{}}};
 if(process.platform==='win32'){
  // Never read CommandLine, Path, StartInfo, environment, or private files.
  // No Stop-Process/taskkill is used. Only our own spawned helper can be stopped.
  const script=String.raw`
$ErrorActionPreference='Stop'
$parentId=__PARENT_PID__
$limit=__ROW_LIMIT__
$pattern='^(cl|clang(?:\+\+|-cl|-[0-9]+)?|gcc|g\+\+|cc|cc1(?:plus)?|ld(?:\.lld)?|lld(?:-link)?|link|wasm-(?:ld|opt)|ninja|make|mingw32-make|cmake|MSBuild|dotnet|csc|vbc|VBCSCompiler|sccache|ccache|node|python(?:[0-9.]+)?|rustc|cargo|javac|java|gradle|tsc|esbuild|swc)$'
while(Get-Process -Id $parentId -ErrorAction SilentlyContinue){
 $began=[DateTimeOffset]::UtcNow.ToUnixTimeMilliseconds()
 $watch=[System.Diagnostics.Stopwatch]::StartNew()
 $rows=New-Object 'System.Collections.Generic.List[object]'
 $matched=0;$failed=$false
 try{
  Get-Process -ErrorAction SilentlyContinue | ForEach-Object {
   if($_.Id -ne $parentId -and $_.ProcessName -match $pattern){
    ++$matched
    if($rows.Count -lt $limit){
     $cpu=$null;try{if($null -ne $_.CPU){$cpu=[double]$_.CPU}}catch{}
     $rows.Add(@([int]$_.Id,[string]$_.ProcessName,$cpu))
    }
   }
  }
 }catch{$failed=$true}
 [Console]::Out.WriteLine((@{version=1;utcMs=$began;durationMs=$watch.Elapsed.TotalMilliseconds;matched=$matched;truncated=($matched -gt $rows.Count);failed=$failed;rows=@($rows.ToArray())} | ConvertTo-Json -Depth 5 -Compress))
 Start-Sleep -Milliseconds __PROCESS_INTERVAL__
}
`.replace('__PARENT_PID__',String(process.pid)).replace('__ROW_LIMIT__',String(limits.rowsPerProcessSample)).replace('__PROCESS_INTERVAL__',String(processIntervalMs));
  try{
   helper=spawn('powershell.exe',['-NoLogo','-NoProfile','-NonInteractive','-Command',script],{
    windowsHide:true,stdio:['ignore','pipe','pipe']});
   processSampler.helperPid=helper.pid??null;
   if(Number.isInteger(helper.pid))registerKnownProcess(helper.pid,'host-load-sampler');
   helper.once('spawn',()=>{processSampler.status='running';});
   helper.stdout.on('data',onStdout);
   helper.stderr.on('data',chunk=>{if(!counts.stderrBytes)error('process-sampler-stderr');counts.stderrBytes+=chunk.length;});
   helper.once('error',event=>{processSampler.status='error';error('process-sampler-spawn-'+String(event.code||'unknown').slice(0,40));finishClosed();});
   helper.once('close',(code,signal)=>{
    if(stdoutBuffer.trim()&&!discardingLine)consumeLine(stdoutBuffer);stdoutBuffer='';
    processSampler.exitCode=code;processSampler.signal=signal;processSampler.status='closed';
    processSampler.stopConfirmed=true;
    if(!processSampler.stopRequested)error('process-sampler-exited-before-stop');finishClosed();
   });
   process.once('exit',onParentExit);
  }catch{processSampler.status='error';processSampler.stopConfirmed=true;error('process-sampler-spawn-failed');finishClosed();}
 }else{processSampler.status='unsupported-platform';processSampler.stopConfirmed=true;error('process-sampler-windows-only');finishClosed();}
 const waitForClose=async milliseconds=>{
  let timer;
  try{return await Promise.race([closed.then(()=>true),new Promise(resolve=>{timer=setTimeout(()=>resolve(false),milliseconds);})]);}
  finally{clearTimeout(timer);}
 };
 const stop=()=>{
  if(stopPromise)return stopPromise;
  stopPromise=(async()=>{
   clearInterval(cpuTimer);cpuTimer=null;sampleCpu('final');stopped=true;mark('probe-stop');
   processSampler.stopRequested=true;killOwnedHelper();
   if(!await waitForClose(2000)){killOwnedHelper();if(!await waitForClose(2000))error('owned-sampler-stop-timeout');}
   process.removeListener('exit',onParentExit);endedAtUtcMs=Date.now();
   return snapshot();
  })();return stopPromise;
 };
 return {mark,registerKnownProcess,snapshot,stop};
}
