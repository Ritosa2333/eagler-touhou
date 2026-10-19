// Root-scheduled continuous live 3P Lunatic/challenge or separate Extra/challenge run. No stage skips or native mutation.
// Real production package, managed Host input, real relay admission and native loop.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {spawn,spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync,createReadStream,statSync} from 'node:fs';
import {pipeline} from 'node:stream';
import {resolve,dirname,extname,relative,isAbsolute,sep} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {createHash,randomUUID} from 'node:crypto';
import {performance} from 'node:perf_hooks';

const root=resolve(import.meta.dirname,'../..');
function option(name,fallback){const i=process.argv.indexOf(name);if(i<0)return fallback;assert.ok(process.argv[i+1]&&!process.argv[i+1].startsWith('--'),'Missing '+name);return process.argv[i+1];}
const bombStress=process.argv.includes('--bomb-stress'),bombSeconds=Number(option('--bomb-seconds','300'));
const minutes=Number(option('--minutes','90')),cpu=Number(option('--cpu','1')),duration=bombStress?bombSeconds:minutes*60;
const players=3,segmentSeconds=10,smokeSeconds=Number(option('--smoke-seconds','0'));
const extra=process.argv.includes('--extra'),challenge=!bombStress,difficulty=extra?4:3,expectedStages=bombStress?[1]:extra?[7]:[1,2,3,4,5,6];
const initialStage=expectedStages[0],initialMusic=(initialStage-1)*2+1,runKind=bombStress?'ordinary-bomb':extra?'extra':'lunatic';
assert.ok(!bombStress||(!extra&&!smokeSeconds),'Bomb stress is a separate ordinary room, not a full-course or setup smoke run');
assert.ok(Number.isFinite(bombSeconds)&&bombSeconds>=60&&bombSeconds<=600);
assert.ok(Number.isFinite(minutes)&&minutes>=10&&minutes<=90);
assert.ok(smokeSeconds===0||(smokeSeconds>=15&&smokeSeconds<=120));
const musicRoot=resolve(option('--music-root',process.env.TH11_MUSIC_ROOT||resolve(root,'../../games/web-content/th11/music')));
const routeOption=option('--route','rtc'),route=routeOption==='direct'?'rtc':routeOption;
const profile='stable';
const gpuDiagnostics=process.argv.includes('--gpu-diagnostics');
const latency=Number(option('--latency-ms','50')),jitter=Number(option('--jitter-ms','5'));
const prediction=12,maxGap=Number(option('--max-gap-ms','100'));

assert.ok([1,4].includes(cpu),'CPU must be 1 or 4');
assert.ok(['rtc','relay'].includes(route),'route: rtc/direct or relay');
assert.ok(['stable','spike'].includes(profile),'profile: stable or spike');
assert.ok(latency>=0&&latency<=200&&jitter>=0&&jitter<=25&&maxGap>=30&&maxGap<=1000);
const launcher=resolve(option('--launcher-root',process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../eagler-touhou')));
const runtime=resolve(option('--runtime',process.env.TH11_MP_PACKAGE||resolve(root,'build-eagler-multiplayer')));
const archive=resolve(process.env.TH11_TEST_ARCHIVE||resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
const diagnostics=resolve(option('--playwright-root',process.env.TH11_PLAYWRIGHT_ROOT||resolve(root,'../../eagler-touhou/th11/tools/diagnostics')));
const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/fullgame-3p-'+runKind+'-'+route+'-'+cpu+'x-'+stamp)));
assert.ok(!existsSync(resolve(out,'receipt.json')),'Never overwrite an earlier good or bad receipt');
mkdirSync(out,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const fileIdentity=path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:hash(bytes)};};
const pause=ms=>new Promise(r=>setTimeout(r,ms));
const report={schema:'th11-rollback-full-game/1',startedAt:new Date().toISOString(),completedAt:null,
  status:'running',passed:false,root,launcher,runtime,archive,out,scope:{desktop:true,independentChromeProcesses:3,
    managedHostInput:true,realNativeLoop:true,actualGpu:true,pixelMotion:false,fullWorldOracle:false,continuousGeneration:true,stageSkip:false,nativeStateMutation:false,
    launcherUi:false,phone:false,performanceObserver:true},settings:{duration,cpu,routeRequested:routeOption,profile,
    oneWaySendDelayMs:latency,jitterPlusMinusMs:jitter,prediction,maxGapMs:maxGap,minutes,smokeSeconds,players,
    difficulty,runKind,initialStage,expectedStages,challenge,bombStress,bombSeconds,gpuDiagnostics,music:'progressive-ogg',musicRoot},
  stages:[],errors:[],http:[],cleanup:[],browser:[],processIds:[],network:[],results:[],segments:[],musicTransfers:[],snapshotErrors:[],failureSnapshots:[],
  stageVisits:[[],[],[]],failures:[],fullRunComplete:false,smokeOnly:smokeSeconds>0};
if(gpuDiagnostics){report.scope.diagnosticOnly=true;report.gpuDiagnosticPolicy={
  resourceCounts:'Successful WebGL create returns minus observed delete calls; not driver live memory or GPU completion.',
  allocationOnly:true,drawCallsWrapped:false,readbacksAdded:false,maximumContexts:8,maximumEvents:16,
  logging:'This run only: enable-logging=stderr, log-level=0, gpu_process_host/gpu_watchdog_thread vmodule=1'};}
report.validationWindow={phase:'setup',openedMs:performance.now(),closedMs:null,cleanupStartedMs:null};
report.ownedCleanupActions=[];
function classifyError(error){
  error.checkerMs??=performance.now();error.observedPhase??=report.validationWindow.phase;
  error.validationClassification=error.blocking===false?'explicit-external':'blocking';
  // The product labels peers P1..P3 for zero-based seats 0..2. Only a real,
  // already-issued close of that exact peer can explain this narrow message.
  const match=/^Error: RTC peer P([1-3]) (?:disconnected|(?:control|input) channel (?:closed|error))$/.exec(String(error.error||'').split('\n')[0]);
  let shellSource=false;try{shellSource=new URL(error.location?.url).pathname==='/runtime/shell.mjs';}catch{}
  const closed=match&&report.ownedCleanupActions.find(a=>
    (a.action==='close-owned-browser'||(a.action==='close-owned-transport'&&a.result?.performed===true))&&
    a.seat===Number(match[1])-1&&a.requestedMs<=error.checkerMs);
  if(error.kind==='console-error'&&error.source?.classification==='fixture-origin'&&shellSource&&
     error.observedPhase==='cleanup'&&report.validationWindow.closedMs!==null&&closed){
    error.validationClassification='expected-owned-teardown';error.ownedCloseAction=closed.id;
  }
  return error;
}
function recordError(error){report.errors.push(classifyError(error));}
const blockingErrors=()=>report.errors.map(classifyError).filter(e=>e.blocking!==false&&e.validationClassification!=='expected-owned-teardown');
report.lifecycle=[];
report.lifecyclePolicy={oncePerEmitterEvent:true,pidMeaning:'Browser process PID from SystemInfo, not a renderer PID',
  clock:'Checker performance.now and UTC at event callback; event arrival is not OS process-exit time'};
function recordLifecycle(kind,seat,browserPid,url=null){
  const checkerMs=performance.now(),observedPhase=report.validationWindow.phase;
  const owned=report.ownedCleanupActions.find(a=>a.action==='close-owned-browser'&&a.seat===seat&&a.requestedMs<=checkerMs);
  const expected=kind!=='page-crash'&&!!owned&&
    ((observedPhase==='cleanup'&&report.validationWindow.closedMs!==null)||(owned.reason==='watchdog'&&!!report.watchdog));
  const entry={sequence:report.lifecycle.length+1,kind,seat,browserPid:browserPid??null,url,
    checkerMs,utc:new Date().toISOString(),observedPhase,expectedOwnedClose:expected,ownedCloseAction:expected?owned.id:null};
  report.lifecycle.push(entry);
  if(!expected){
    report.firstUnexpectedLifecycle??={...entry};
    recordError({...entry,kind:'browser-lifecycle',lifecycleEvent:kind,
      error:'Unexpected '+kind+' on seat '+seat+' (browser PID '+(browserPid??'unavailable')+')',blocking:true});
    const first=blockingErrors()[0];
    report.firstFailureEvidence??={checkerMs:first.checkerMs,kind:first.kind,seat:first.seat,
      lifecycleEvent:first.lifecycleEvent??null,error:first.error??null};
    firstFailure??=Error('First blocking browser evidence: '+JSON.stringify(report.firstFailureEvidence));
    report.error??=firstFailure.stack||String(firstFailure);
  }
  // At most four terminal event records per seat. Preserve them immediately,
  // without attempting any evaluate in a closing target or hiding a first error.
  try{save();}catch(error){report.cleanup.push({action:'save-lifecycle-evidence',sequence:entry.sequence,error:String(error)});}
}
const save=()=>{
  report.errorSummary={total:report.errors.length,blocking:blockingErrors().length,validationBlocking:blockingErrors().length,
    sourceBlocking:report.errors.filter(e=>e.blocking!==false).length,
    expectedTeardown:report.errors.filter(e=>e.validationClassification==='expected-owned-teardown').length,
    lateBlocking:blockingErrors().filter(e=>e.observedPhase==='cleanup').length,
    explicitlyExternal:report.errors.filter(e=>e.blocking===false).length};
  writeFileSync(resolve(out,'receipt.json'),JSON.stringify(report,null,2)+'\n');
};
function requireNoBlockingErrors(){const errors=blockingErrors();if(errors.length)throw Error('Browser/server error: '+JSON.stringify(errors.slice(-5)));}
function browserErrorSource(kind,sourceUrl,pageUrl,frameUrl=null){
  let sourceOrigin=null,hostname=null,protocol=null,pageOrigin=null;
  try{const u=new URL(sourceUrl);sourceOrigin=u.origin;hostname=u.hostname;protocol=u.protocol;}catch{}
  try{pageOrigin=new URL(pageUrl).origin;}catch{}
  const external=['request-failed','console-error'].includes(kind)&&['http:','https:'].includes(protocol)&&hostname==='me.kis.v2.scr.kaspersky-labs.com';
  return {blocking:!external,source:{url:sourceUrl||null,origin:sourceOrigin,hostname,pageUrl,frameUrl,
    classification:external?'known-external-kaspersky':sourceOrigin&&sourceOrigin===pageOrigin?'fixture-origin':'unattributed'}};
}
report.errorPolicy={externalHost:'me.kis.v2.scr.kaspersky-labs.com',
  onlyExplicitRequestOrConsoleSource:true,unknownConsoleBlocks:true,pageErrorsAlwaysBlock:true,retainAllErrors:true};
const snapshotTimeoutMs=15000,freezeTimeoutMs=5000,segmentMaxRecords=64,segmentMaxBytes=64*1024;
report.snapshotPolicy={parallelPeerCapture:true,primitiveStringTransfer:true,retainUnpersistedSegment:true,
  segmentMaxRecords,segmentMaxBytes,segmentWholeStringify:false,chunkOrderValidated:true,
  snapshotTimeoutMs,freezeTimeoutMs,
  checkerTransferMeaning:'Request to string result, including browser capture/serialization, scheduling and transfer; not wire-only time'};
async function bounded(promise,ms,label){let timer;try{return await Promise.race([promise,new Promise((_,reject)=>{
  timer=setTimeout(()=>reject(Error(label+' timed out after '+ms+' ms')),ms);
})]);}finally{clearTimeout(timer);}}
// Runs inside the Host document. Only the small metadata is serialized at
// capture; immutable rows stay in the exchanged arrays until the file ACK.
function browserSegmentTransfer(request){
  const limit=64*1024,maxRecords=64,names=['raf','transport','motion','touchEvents'],encoder=new TextEncoder();
  const wire=(metadata,json)=>{const header=JSON.stringify(metadata),result=header+'\n'+json;
    if(encoder.encode(header).byteLength+1+metadata.payloadBytes>limit)throw Error('Segment block exceeds 64 KiB');
    return result;};
  if(request.action==='begin'){
    if(window.__fullPendingSegment||window.__fullSegmentTransfer)throw Error('Previous segment has not been written');
    const began=performance.now(),r=document.querySelector('iframe').contentWindow,
      probe=r.__rollbackAcceptance.segment(),touch=window.__fullTouch.segment();
    window.__fullPendingSegment={probe,touch};
    const {raf,transport,motion,...probeMetadata}=probe,{events,...touchMetadata}=touch;
    const serializeAt=performance.now(),json=JSON.stringify({probe:probeMetadata,touch:touchMetadata}),
      serializeMs=performance.now()-serializeAt,byteAt=performance.now(),payloadBytes=encoder.encode(json).byteLength,
      byteMs=performance.now()-byteAt,freezeAt=performance.now(),frozen=JSON.parse(json);
    const rows=[raf,transport,motion,events],counts=rows.map(a=>a.length),id=(window.__fullSegmentSerial=(window.__fullSegmentSerial||0)+1);
    window.__fullPendingSegment={probe:{...frozen.probe,raf,transport,motion},touch:{...frozen.touch,events}};
    window.__fullSegmentTransfer={id,rows,counts,section:0,offset:0,sequence:0,complete:false};
    return wire({kind:'segment-begin',id,names,counts,recordTotal:counts.reduce((a,b)=>a+b,0),
      timeOrigin:performance.timeOrigin,browserCaptureStartedMs:began,browserCaptureMs:serializeAt-began,
      browserSerializeMs:serializeMs,browserByteCountMs:byteMs,browserMetadataFreezeMs:performance.now()-freezeAt,payloadBytes},json);
  }
  const state=window.__fullSegmentTransfer;
  if(!state||state.id!==request.id)throw Error('Segment token mismatch');
  if(request.action==='ack'){
    if(!state.complete||request.sequence!==state.sequence)throw Error('Cannot release incomplete segment');
    window.__fullPendingSegment=null;window.__fullSegmentTransfer=null;return JSON.stringify({id:state.id,released:true});
  }
  if(request.action!=='chunk'||request.sequence!==state.sequence||state.complete)throw Error('Segment sequence mismatch');
  while(state.section<names.length&&state.offset===state.counts[state.section]){++state.section;state.offset=0;}
  if(state.section===names.length){
    state.complete=true;const sequence=state.sequence++;
    return wire({kind:'segment-end',id:state.id,sequence,counts:state.counts,recordTotal:state.counts.reduce((a,b)=>a+b,0),
      browserSerializeMs:0,browserByteCountMs:0,payloadBytes:2},'[]');
  }
  const section=state.section,offset=state.offset,parts=[];let payloadBytes=2,browserSerializeMs=0,browserByteCountMs=0;
  while(parts.length<maxRecords&&state.offset<state.counts[section]){
    const start=performance.now(),row=JSON.stringify(state.rows[section][state.offset]);
    browserSerializeMs+=performance.now()-start;
    const byteAt=performance.now(),bytes=encoder.encode(row).byteLength;browserByteCountMs+=performance.now()-byteAt;
    // Reserve room for the bounded header. Never split or truncate a record.
    if(bytes+2>limit-2048)throw Error('Single segment record exceeds the 64 KiB block budget');
    if(payloadBytes+bytes+(parts.length?1:0)>limit-2048)break;
    payloadBytes+=bytes+(parts.length?1:0);parts.push(row);++state.offset;
  }
  if(!parts.length)throw Error('Segment block made no progress');
  const start=performance.now(),json='['+parts.join(',')+']';browserSerializeMs+=performance.now()-start;
  return wire({kind:'segment-block',id:state.id,sequence:state.sequence++,section:names[section],sectionIndex:section,
    offset,count:parts.length,sectionCount:state.counts[section],browserSerializeMs,browserByteCountMs,payloadBytes},json);
}
function decodeSegmentWire(snapshot,wire){
  snapshot.wires.push(wire);assert.equal(typeof wire,'string','Segment response must be one string');
  const split=wire.indexOf('\n');assert.ok(split>0,'Segment metadata separator missing');
  const start=performance.now(),metadata=JSON.parse(wire.slice(0,split)),json=wire.slice(split+1),data=JSON.parse(json),
    parseMs=performance.now()-start,byteAt=performance.now(),bytes=Buffer.byteLength(json),wireBytes=Buffer.byteLength(wire),
    byteMs=performance.now()-byteAt,t=snapshot.timing;
  assert.ok(wireBytes<=segmentMaxBytes,'Segment wire exceeds 64 KiB');assert.equal(bytes,metadata.payloadBytes,'Segment payload byte count changed');
  t.checkerParseMs+=parseMs;t.checkerParseMaxMs=Math.max(t.checkerParseMaxMs,parseMs);t.checkerByteCountMs+=byteMs;
  t.browserSerializeMs+=metadata.browserSerializeMs;t.browserSerializeMaxMs=Math.max(t.browserSerializeMaxMs,metadata.browserSerializeMs);
  t.browserByteCountMs+=metadata.browserByteCountMs;t.browserStringBytes+=bytes;t.wireBytes+=wireBytes;
  return {metadata,data};
}
async function captureSegments(){
  const snapshots=pages.map((page,seat)=>({page,seat,wires:[],counts:[0,0,0,0],nextSequence:0,done:false,
    timing:{checkerRequestedMs:performance.now(),checkerTransferMs:0,checkerTransferMaxMs:0,checkerParseMs:0,
      checkerParseMaxMs:0,checkerByteCountMs:0,browserSerializeMs:0,browserSerializeMaxMs:0,browserByteCountMs:0,
      browserStringBytes:0,wireBytes:0,blocks:0,sections:[]}}));
  async function request(snapshot,action){
    const began=performance.now(),left=snapshotTimeoutMs-(began-snapshot.timing.checkerRequestedMs);
    assert.ok(left>0,'Complete segment transfer exceeded deadline on peer '+snapshot.seat);
    try{return await bounded(snapshot.page.evaluate(browserSegmentTransfer,action),left,'segment '+action.action+' peer '+snapshot.seat);}
    finally{const ms=performance.now()-began;snapshot.timing.checkerTransferMs+=ms;snapshot.timing.checkerTransferMaxMs=Math.max(snapshot.timing.checkerTransferMaxMs,ms);}
  }
  await Promise.all(snapshots.map(async snapshot=>{
    try{
      const {metadata:m,data}=decodeSegmentWire(snapshot,await request(snapshot,{action:'begin'}));
      assert.equal(m.kind,'segment-begin');assert.deepEqual(m.names,['raf','transport','motion','touchEvents']);
      assert.equal(m.counts.length,4);for(const n of m.counts)assert.ok(Number.isSafeInteger(n)&&n>=0&&n<=20000);
      assert.equal(m.recordTotal,m.counts.reduce((a,b)=>a+b,0));
      snapshot.id=m.id;snapshot.expected=m.counts;snapshot.expectedTotal=m.recordTotal;
      snapshot.data={probe:{...data.probe,raf:[],transport:[],motion:[]},touch:{...data.touch,events:[]}};
      snapshot.rows=[snapshot.data.probe.raf,snapshot.data.probe.transport,snapshot.data.probe.motion,snapshot.data.touch.events];
      Object.assign(snapshot.timing,{timeOrigin:m.timeOrigin,browserCaptureStartedMs:m.browserCaptureStartedMs,
        browserCaptureMs:m.browserCaptureMs,browserMetadataFreezeMs:m.browserMetadataFreezeMs,recordTotal:m.recordTotal,sections:m.names.map((section,i)=>({section,expected:m.counts[i],received:0,blocks:0}))});
    }catch(error){snapshot.error=error;}
  }));
  // One small block per peer per round, allowing their real game loops to run
  // between requests. All records and collection-time bad frames are retained.
  while(snapshots.some(s=>!s.error&&!s.done))await Promise.all(snapshots.filter(s=>!s.error&&!s.done).map(async snapshot=>{
    try{
      const {metadata:m,data}=decodeSegmentWire(snapshot,await request(snapshot,{action:'chunk',id:snapshot.id,sequence:snapshot.nextSequence}));
      assert.equal(m.id,snapshot.id);assert.equal(m.sequence,snapshot.nextSequence++);assert.ok(Array.isArray(data));
      if(m.kind==='segment-end'){
        assert.deepEqual(data,[]);assert.deepEqual(m.counts,snapshot.expected);assert.deepEqual(snapshot.counts,snapshot.expected);
        assert.equal(m.recordTotal,snapshot.expectedTotal);snapshot.done=true;snapshot.timing.complete=true;return;
      }
      assert.equal(m.kind,'segment-block');const section=snapshot.counts.findIndex((n,i)=>n<snapshot.expected[i]);
      assert.equal(m.sectionIndex,section);assert.equal(m.section,snapshot.timing.sections[section]?.section);
      assert.equal(m.offset,snapshot.counts[section]);assert.equal(m.sectionCount,snapshot.expected[section]);
      assert.equal(m.count,data.length);assert.ok(m.count>0&&m.count<=segmentMaxRecords);
      assert.ok(m.offset+m.count<=m.sectionCount);snapshot.rows[section].push(...data);snapshot.counts[section]+=m.count;
      ++snapshot.timing.blocks;++snapshot.timing.sections[section].blocks;snapshot.timing.sections[section].received+=m.count;
    }catch(error){snapshot.error=error;}
  }));
  for(const snapshot of snapshots){snapshot.timing.checkerReceivedMs=performance.now();snapshot.timing.checkerCaptureWallMs=snapshot.timing.checkerReceivedMs-snapshot.timing.checkerRequestedMs;}
  return snapshots;
}
async function captureSnapshots(kind){
  // Catch each peer separately: one failed renderer must not discard successful
  // snapshots from peers whose segment buffers have already been exchanged.
  return Promise.all(pages.map(async(page,seat)=>{
    const timing={checkerRequestedMs:performance.now()};
    try{
      const wire=await bounded(page.evaluate(kind=>{
        const began=performance.now(),r=document.querySelector('iframe')?.contentWindow;
        let data;
        if(kind!=='failure')throw Error('Normal segments must use bounded blocks');
        data={host:window.host?.snapshot?.(),probe:r?.__rollbackAcceptance?.result(),
          touch:window.__fullTouch,unpersistedSegment:window.__fullPendingSegment||null};
        const serializedAt=performance.now(),json=JSON.stringify(data),serializedDone=performance.now();
        const bytes=new TextEncoder().encode(json).byteLength,bytesDone=performance.now();
        const metadata={kind,timeOrigin:performance.timeOrigin,browserCaptureStartedMs:began,
          browserCaptureMs:serializedAt-began,browserSerializeMs:serializedDone-serializedAt,
          browserByteCountMs:bytesDone-serializedDone,browserStringBytes:bytes,browserStringCodeUnits:json.length};
        return JSON.stringify(metadata)+'\n'+json;
      },kind),snapshotTimeoutMs,kind+' snapshot peer '+seat);
      timing.checkerReceivedMs=performance.now();timing.checkerTransferMs=timing.checkerReceivedMs-timing.checkerRequestedMs;
      assert.equal(typeof wire,'string','Snapshot must cross Playwright as a single primitive string');
      return {seat,wire,timing};
    }catch(error){timing.checkerReceivedMs=performance.now();timing.checkerTransferMs=timing.checkerReceivedMs-timing.checkerRequestedMs;return {seat,timing,error};}
  }));
}
function decodeSnapshot(snapshot){
  const start=performance.now(),split=snapshot.wire.indexOf('\n');
  assert.ok(split>0,'Snapshot metadata separator missing');
  const metadata=JSON.parse(snapshot.wire.slice(0,split)),json=snapshot.wire.slice(split+1),data=JSON.parse(json);
  snapshot.timing.checkerParseMs=performance.now()-start;
  Object.assign(snapshot.timing,metadata);
  const bytesAt=performance.now(),bytes=Buffer.byteLength(json);
  snapshot.timing.checkerByteCountMs=performance.now()-bytesAt;
  assert.equal(bytes,metadata.browserStringBytes,'Snapshot string byte count changed');
  return {data,json};
}
function writeSnapshotFile(file,encoded,timing){
  const start=performance.now();writeFileSync(file,encoded);timing.checkerWriteMs=performance.now()-start;
  const hashAt=performance.now(),sha256=hash(encoded);timing.checkerHashMs=performance.now()-hashAt;
  return sha256;
}
const mark=(name,extra={})=>{report.stages.push({name,checkerMs:performance.now(),...extra});save();};
save();
let relay,server;const browsers=[],contexts=[],pages=[],cdps=[],relayHeartbeats=[];
let relayLog='',relayLogOverflow=false,firstFailure=null;
const totalBudgetMs=(300+(smokeSeconds||duration))*1000;
const watchdog=setTimeout(()=>{
  report.watchdog={exceeded:true,totalBudgetMs,checkerMs:performance.now(),error:'Overall bounded browser gate timed out'};
  firstFailure??=Error(report.watchdog.error);report.error??=firstFailure.stack||String(firstFailure);
  report.status='failed';report.passed=false;process.exitCode=1;save();
  // Closing only the owned browsers rejects pending evaluate calls in a wedged
  // renderer. Never kill-by-name or touch an unrelated browser/relay process.
  if(relay&&relay.exitCode===null)relay.kill();
  void Promise.allSettled(browsers.map((browser,seat)=>{
    const action={id:report.ownedCleanupActions.length+1,seat,action:'close-owned-browser',
      reason:'watchdog',requestedMs:performance.now()};
    report.ownedCleanupActions.push(action);
    return browser.close().then(()=>{action.completedMs=performance.now();action.ok=true;},
      error=>{action.completedMs=performance.now();action.error=String(error);});
  }));
},totalBudgetMs);
report.totalBudgetMs=totalBudgetMs;
function gitIdentity(path){const run=args=>{const r=spawnSync('git',['-C',path,...args],{encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});return {exitCode:r.status,stdout:r.stdout?.trim(),stderr:r.stderr?.trim()};};return {path,head:run(['rev-parse','HEAD']),status:run(['status','--porcelain=v1','--untracked-files=normal'])};}
function blocked(message){const e=Error(message);e.blocked=true;throw e;}
function percentile(values,p){if(!values.length)return null;const a=[...values].sort((a,b)=>a-b);return a[Math.min(a.length-1,Math.floor((a.length-1)*p))];}
function distribution(values){return {count:values.length,p50:percentile(values,.5),p95:percentile(values,.95),p99:percentile(values,.99),
  max:values.length?Math.max(...values):null,over33:values.filter(x=>x>33.4).length,over50:values.filter(x=>x>50).length,over100:values.filter(x=>x>100).length};}
const RB={version:0,allows:1,canCapture:2,correcting:3,rollbackFrame:4,predictionFrontier:5,captureFrame:6,committedThrough:7,
  records:8,journalFrames:9,restores:10,historyTicks:11,peakDepth:12,peakBytes:13,peakBlocks:14,networkWait:15,cpuYield:16,
  barrierFrame:17,touchCached:18,touchFrame:19,revision:20,presents:21,publishedExclusive:22,error:23};
report.rollbackStatus=RB;
report.sceneStatus={version:0,phase:1,stage:2,frame:3,endingExists:4,endingActive:5,endingFlags:6,
  messageKind:7,endingFrames:8,endingTime:9,pauseState:10,pauseCompleted:11};
report.staffRequirement={required:!extra&&!bombStress,minimumProgressTransitions:2,exactMessageKind:2,
  meaning:'Three peers each observe active staff.msg and two advances after fully confirmed native callbacks; Extra has no authored Staff phase'};

// Installed before WASM instantiation, in each real document. This changes only
// transport scheduling / observation, never packet bytes or native input state.
function installGpuResourceProbe(){
  const stats=globalThis.__th11GpuResourceProbe={version:1,timeOrigin:performance.timeOrigin,contexts:[],events:[],errors:[]};
  const contexts=new WeakMap(),original=HTMLCanvasElement.prototype.getContext;
  const fail=message=>{if(stats.errors.length<8)stats.errors.push({at:performance.now(),message});};
  HTMLCanvasElement.prototype.getContext=function(...args){
    const gl=Reflect.apply(original,this,args);
    if(!gl||!['webgl','webgl2','experimental-webgl'].includes(String(args[0]))||contexts.has(gl))return gl;
    if(stats.contexts.length>=8){fail('WebGL context observation capacity exceeded');return gl;}
    const row={id:stats.contexts.length+1,kind:String(args[0]),createdAt:performance.now(),resources:{}};
    contexts.set(gl,row);stats.contexts.push(row);
    // Weak identity records never retain a GL object. Repeated delete calls
    // preserve original behavior and do not invent a second retirement.
    for(const kind of ['Buffer','Texture','Framebuffer','Renderbuffer','Shader','Program','VertexArray','Query','Sampler','TransformFeedback']){
      const create=gl['create'+kind],destroy=gl['delete'+kind];if(typeof create!=='function'||typeof destroy!=='function')continue;
      const owned=new WeakSet(),deleted=new WeakSet(),count=row.resources[kind]={created:0,deleted:0,outstanding:0,peakOutstanding:0,nullCreates:0,unknownDeletes:0};
      gl['create'+kind]=function(...values){const object=Reflect.apply(create,this,values);
        if(object){owned.add(object);++count.created;++count.outstanding;count.peakOutstanding=Math.max(count.peakOutstanding,count.outstanding);}
        else ++count.nullCreates;return object;};
      gl['delete'+kind]=function(object){const result=Reflect.apply(destroy,this,[object]);
        if(object&&!deleted.has(object)){deleted.add(object);if(owned.has(object)){++count.deleted;--count.outstanding;}else ++count.unknownDeletes;}return result;};
    }
    for(const name of ['webglcontextlost','webglcontextrestored'])this.addEventListener(name,event=>{
      if(stats.events.length>=16){fail('WebGL context event capacity exceeded');return;}
      stats.events.push({kind:name,context:row.id,at:performance.now(),utc:new Date().toISOString(),
        statusMessage:String(event.statusMessage||''),nativePhase:globalThis.core?.th11_phase?.()??null});
      // Do not prevent, restore, change attributes, or replace the shell handler.
    });
    return gl;
  };
}
function installProbe(config){
  const state={timeOrigin:performance.timeOrigin,phase:'idle',start:0,end:0,raf:[],transport:[],motion:[],tail:[],
    overflow:[],errors:[],lastMotion:0,current:null,serial:0,wireSerial:0,queued:0,queuedBytes:0,maxQueued:0,
    maxQueuedBytes:0,realDelay:config.latency,profile:config.profile,attached:false,settleStarted:0,
    terminal:null,staff:{samples:0,progressTransitions:0,first:null,last:null,lastProgress:null}};
  const timers=new Set(),orderedDue=new WeakMap(),originalRaf=requestAnimationFrame.bind(globalThis);
  let random=(0x713311+config.seat*1009)>>>0;
  const push=(list,value,max=20000)=>{if(list.length<max)list.push(value);else if(!state.overflow.includes(list===state.raf?'raf':list===state.transport?'transport':'motion'))state.overflow.push(list===state.raf?'raf':list===state.transport?'transport':'motion');};
  function rnd(){random=(Math.imul(random,1664525)+1013904223)>>>0;return random/4294967296;}
  function bytes(data){if(data instanceof ArrayBuffer)return new Uint8Array(data.slice(0));if(ArrayBuffer.isView(data))return new Uint8Array(data.buffer.slice(data.byteOffset,data.byteOffset+data.byteLength));return null;}
  function signature(b){let v=2166136261;for(let i=0;i<b.length;++i)v=Math.imul(v^b[i],16777619)>>>0;return v;}
  function send(original,socket,data,kind,ordered){
    const payload=bytes(data);if(!payload)return original.call(socket,data);
    const now=performance.now(),measured=state.phase==='measure',elapsed=now-state.start;
    let extra=0;
    if(false&&measured&&state.profile==='spike'){
      const span=config.duration*1000;
      if(elapsed>=span*.32&&elapsed<span*.32+250)extra=150;
      if(elapsed>=span*.66&&elapsed<span*.66+350)extra=300;
    }
    const delay=state.realDelay>0?Math.max(0,state.realDelay+(rnd()*2-1)*config.jitter+extra):0;
    let due=now+delay;
    if(ordered){due=Math.max(due,orderedDue.get(socket)||0);orderedDue.set(socket,due+.001);}
    const id=++state.wireSerial,entry={id,kind,phase:state.phase,scheduled:now,due,requestedDelay:delay,extra,
      size:payload.length,signature:signature(payload),actual:null,lateness:null,readyState:socket.readyState};
    push(state.transport,entry);
    if(++state.queued>1024||(state.queuedBytes+=payload.length)>4*1024*1024){state.errors.push('bounded transport injection queue overflow');throw Error('acceptance transport queue overflow');}
    state.maxQueued=Math.max(state.maxQueued,state.queued);state.maxQueuedBytes=Math.max(state.maxQueuedBytes,state.queuedBytes);
    const deliver=()=>{state.queued--;state.queuedBytes-=payload.length;
      const actual=performance.now(),delivery={...entry,recordKind:'delivery',actual,lateness:actual-due};
      // Scheduling records never change after publication. Completion (including
      // cancellation/failure) is a separate immutable record with the same id.
      try{if(socket.readyState!=='open'&&socket.readyState!==1){delivery.cancelled='channel closed before due';return;}original.call(socket,payload);}
      catch(e){delivery.error=String(e);state.errors.push(String(e));}
      finally{push(state.transport,delivery);}};
    if(!delay&&due<=now){deliver();return;}
    const timer=setTimeout(()=>{timers.delete(timer);deliver();},Math.max(0,due-now));timers.add(timer);
  }
  if(globalThis.WebSocket){const original=WebSocket.prototype.send;WebSocket.prototype.send=function(data){return send(original,this,data,'ws',true);};}
  if(config.route==='relay')Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});
  if(globalThis.RTCDataChannel){const original=RTCDataChannel.prototype.send;RTCDataChannel.prototype.send=function(data){return send(original,this,data,'rtc:'+this.label,this.ordered);};}
  globalThis.requestAnimationFrame=function(callback){
    const requested=performance.now(),id=++state.serial;
    return originalRaf(stamp=>{
      const row={id,requested,stamp,entry:performance.now(),exit:null,nativeMs:null,nativeTicks:null,
        observerMs:0,net:null,rollback:null,phase:state.phase};
      const previous=state.current;state.current=row;
      try{return callback(stamp);}finally{row.exit=performance.now();state.current=previous;
        if(row.nativeMs!==null&&['measure','drain','disconnect'].includes(row.phase))push(state.raf,row);}
    });
  };
  const words=(name,count)=>{const core=globalThis.core;if(!core?.[name])return [];const p=core[name]();return p?Array.from(new Uint32Array(core.memory.buffer,p,count)):[];};
  const nativeText=name=>{const core=globalThis.core;if(!core?.[name])return '';const p=core[name]();if(!p)return '';const bytes=new Uint8Array(core.memory.buffer);let end=p;while(end<bytes.length&&bytes[end])++end;return new TextDecoder().decode(bytes.subarray(p,Math.min(end,p+4096)));};
  function small(){const a=globalThis.Module?.SDL3?.audioContext;return {timeOrigin:state.timeOrigin,now:performance.now(),phase:state.phase,nativePhase:globalThis.core?.th11_phase?.(),net:words('th11_mp_status',24),
    game:words('th11_mp_game_status',56),staffEvidence:{...state.staff},terminalEvidence:state.terminal,
    ...(config.gpuDiagnostics?{gpuDiagnostics:globalThis.__th11GpuResourceProbe??null}:{}),
    hostExits:(parent.host?.events||[]).filter(m=>m.event==='exit').slice(-4),hostErrors:(parent.host?.events||[]).filter(m=>m.event==='error').slice(-4),ui:words('th11_mp_replay_ui_status',12),memoryBytes:globalThis.core?.memory?.buffer.byteLength,scene:words('th11_mp_scene_status',12),audioContext:a?{state:a.state,currentTime:a.currentTime,sampleRate:a.sampleRate}:null,
    rollback:words('th11_mp_rollback_status',24),audio:words('th11_audio_statistics',9),sfx:words('th11_sfx_statistics',120),bomb:config.bombStress?words('th11_mp_bomb_status',24):null,error:nativeText('th11_mp_error')||nativeText('th11_error'),
    route:globalThis.__eaglerPeerTransport?.route||null,disconnected:!!globalThis.__eaglerPeerTransport?.disconnected,
    shellError:document.querySelector('#error')?.textContent||'',queued:state.queued,overflow:state.overflow,probeErrors:state.errors};}
  globalThis.__rollbackAcceptance={
    state,small,
    attach(){
      if(state.attached)return;state.attached=true;
      const prior=Module.onGameFrame;
      Module.onGameFrame=(ok,ms,count)=>{
        const shellEntry=performance.now(),row=state.current;
        if(row){row.nativeMs=ms;row.nativeTicks=count;row.ok=ok;}
        // The shell can stop/Clear netplay inside its own hook. Preserve the
        // actual confirmed terminal transaction before it clears the live ABI.
        if(ok&&globalThis.core?.th11_phase?.()===4){
          const net=words('th11_mp_status',24),rollback=words('th11_mp_rollback_status',24),scene=words('th11_mp_scene_status',12);
          if(net[0]===1&&net[1]&&net[5]===3&&rollback[0]===1&&scene[0]===1&&scene[1]===4&&scene[3]>0&&
             net[4]!==0xffffffff&&net[4]+1>=scene[3]&&net[3]>=scene[3]&&!net[23]&&!rollback[3]&&!rollback[23]){
            state.terminal={at:performance.now(),callback:row?.id??null,source:'before-runtime-shell-hook',
              net,rollback,scene,audio:words('th11_audio_statistics',9),sfx:words('th11_sfx_statistics',120),fullyConfirmed:true,retired:!!net[13]};
          }
        }
        prior?.(ok,ms,count);
        if(row)row.shellHookMs=performance.now()-shellEntry;
        const entry=performance.now();
        if(!['measure','drain','disconnect'].includes(state.phase))return;
        const net=words('th11_mp_status',24),rollback=words('th11_mp_rollback_status',24);
        const phase=globalThis.core?.th11_phase?.(),scene=words('th11_mp_scene_status',12);
        // Ending is confirmed P0, outside rollback capture. Observe real complete
        // callbacks, never infer Staff from music, hash or an Ending screenshot.
        if(scene.length===12&&scene[0]===1&&phase===5&&scene[1]===5&&scene[4]&&scene[5]&&
           (scene[6]&2)&&scene[7]===2&&net[4]!==0xffffffff&&net[4]+1>=scene[3]&&!rollback[3]&&!rollback[23]){
          const point={at:performance.now(),generation:net[12],frame:scene[3],confirmed:net[4],scene};
          const proof=state.staff;++proof.samples;if(!proof.first)proof.first=point;
          if(proof.lastProgress&&point.generation===proof.lastProgress.generation&&
             scene[8]>proof.lastProgress.scene[8]&&scene[9]>proof.lastProgress.scene[9]){
            ++proof.progressTransitions;proof.lastProgress=point;
          }else if(!proof.lastProgress)proof.lastProgress=point;
          proof.last=point;
        }
        if(row){row.net=net;row.rollback=rollback;row.nativePhase=phase;row.stage=state.lastStage??0;}
        const now=performance.now();
        if(phase!==1||state.bombStrict||now-state.lastMotion>=100){
          const game=words('th11_mp_game_status',56),sample={at:now,callback:row?.id??null,net,rollback,game,scene,audio:words('th11_audio_statistics',9),bomb:config.bombStress?words('th11_mp_bomb_status',24):null,memoryBytes:globalThis.core?.memory?.buffer.byteLength};
          if(state.bombStrict&&sample.bomb?.[2]){
            const watch=state.bombLifecycle;++watch.samples;
            if(phase===1){++watch.stageSamples;
              for(let seat=0;seat<3;++seat){const p=8+seat*16,b=12+seat*4;
                watch.maxElapsed[seat]=Math.max(watch.maxElapsed[seat],sample.bomb[b+2]);
                if((game[p]!==1||game[p+1]!==0||game[p+2]!==1||game[2]!==1||sample.bomb[b+3])&&watch.violations.length<8)
                  watch.violations.push({frame:game[4],seat,game:[...game.slice(p,p+16)],bomb:[...sample.bomb]});
              }
            }
          }
          state.lastMotion=now;state.lastStage=game[2];if(row)row.stage=game[2];push(state.motion,sample,6000);
          // Exact snapshots are observed only after native correction has completed
          // and every input through this post-tick state is confirmed.
          if(game.length===56&&game[3]!==1&&net[4]!==0xffffffff&&net[4]+1>=game[4]&&
             !rollback[3]&&!rollback[23]&&rollback[0]===1)
            {state.tail.push(sample);if(state.tail.length>240)state.tail.shift();}
        }
        if(row)row.observerMs+=performance.now()-entry;
      };
      const transport=globalThis.__eaglerPeerTransport;
      if(transport&&!transport.__rollbackObserved){
        const receive=transport.receiveBinary;
        transport.receiveBinary=function(data){const b=bytes(data);if(b)push(state.transport,{kind:'receive',phase:state.phase,at:performance.now(),size:b.length,signature:signature(b)});return receive.call(this,data);};
        transport.__rollbackObserved=true;
      }
    },
    start(){state.phase='measure';state.start=performance.now();state.end=state.start+config.duration*1000;state.lastMotion=0;state.segmentStart=state.start;},
    segment(){const end=performance.now(),result={...state,start:state.segmentStart,end,tail:[],current:null};
      state.raf=[];state.transport=[];state.motion=[];state.segmentStart=end;return result;},
    drain(){state.phase='drain';state.realDelay=0;state.settleStarted=performance.now();return small();},
    stop(){state.phase='settled';return small();},
    lifecycle(){state.phase='disconnect';return small();},
    result(){return state;},
    close(){for(const timer of timers)clearTimeout(timer);timers.clear();},
  };
}

function canonical(sample){return {frame:sample.game[4],generation:sample.net[12],values:[...sample.game.slice(1,5),...sample.game.slice(8)]};}
function analysis(data){
  const rows=data.raf.filter(r=>r.phase==='measure'&&r.entry>=data.start&&r.entry<=data.end);
  const motion=data.motion.filter(r=>r.at>=data.start&&r.at<=data.end);
  const gaps=values=>values.slice(1).map((v,i)=>v-values[i]);
  let lastPresent=null,highFrame=null;const presents=[],frontiers=[];
  for(const r of rows){const rb=r.rollback;
    if(rb?.length===24){if(lastPresent!==null&&rb[21]>lastPresent)presents.push(r.exit);lastPresent=rb[21];}
    if(r.net?.length===24){if(highFrame!==null&&r.net[3]>highFrame)frontiers.push(r.exit);highFrame=Math.max(highFrame??0,r.net[3]);}
  }
  const elapsed=rows.length>1?rows.at(-1).exit-rows[0].entry:0;
  const logical=rows.length>1?Math.max(0,rows.at(-1).net[3]-rows[0].net[3]):0;
  const first=rows[0]?.rollback||[],last=rows.at(-1)?.rollback||[];
  const deltas={restores:(last[10]??0)-(first[10]??0),historyTicks:(last[11]??0)-(first[11]??0),
    presentations:(last[21]??0)-(first[21]??0)};
  const moves=[0,1,2].map(seat=>{const coords=motion.filter(s=>s.game.length===56).map(s=>[s.game[17+seat*16],s.game[18+seat*16]]);
    return {seat,samples:coords.length,distinct:new Set(coords.map(c=>c.join(','))).size};});
  const audio=motion.filter(r=>r.audio?.length===9).map(r=>r.audio);
  const audioSummary={samples:audio.length,first:audio[0]||[],last:audio.at(-1)||[],errors:audio.filter(a=>a[3]!==0),
    mixedFramesDelta:audio.length?(audio.at(-1)[2]-audio[0][2])>>>0:0,queueFrames:distribution(audio.map(a=>a[4])),musicIncluded:true};
  const measuredWire=data.transport.filter(e=>e.phase==='measure'&&e.recordKind==='delivery'&&e.actual!==null&&e.actual!==undefined);
  const edgeIntervals=times=>gaps([data.start,...times.filter(t=>t>=data.start&&t<=data.end),data.end]);
  return {callbacks:rows.length,elapsedMs:elapsed,logicalTicks:logical,logicHz:elapsed?logical*1000/elapsed:0,
    rafRequestToEntry:distribution(rows.map(r=>r.entry-r.requested)),rafTimestampToEntry:distribution(rows.map(r=>r.entry-r.stamp)),
    callbackEntryIntervals:distribution(gaps(rows.map(r=>r.entry))),callbackWall:distribution(rows.map(r=>r.exit-r.entry)),
    nativeReportedWall:distribution(rows.map(r=>r.nativeMs)),observerWall:distribution(rows.map(r=>r.observerMs)),
    existingShellHookWall:distribution(rows.map(r=>r.shellHookMs??0)),
    presentCount:presents.length,frontierCount:frontiers.length,
    presentIntervals:distribution(gaps(presents)),newFrontierIntervals:distribution(gaps(frontiers)),
    presentWindowHolds:distribution(edgeIntervals(presents)),frontierWindowHolds:distribution(edgeIntervals(frontiers)),
    windowEdgeMeaning:'Start/end intervals are censored lower bounds; presentation counter changes are observed at callback exit, not GPU scanout.',
    moves,rollbackDelta:deltas,audio:audioSummary,
    networkWaitCallbacks:rows.filter(r=>r.rollback?.[15]).length,cpuYieldCallbacks:rows.filter(r=>r.rollback?.[16]).length,
    stages:[...new Set(rows.map(r=>r.stage))],nativePhases:[...new Set(rows.map(r=>r.nativePhase))],
    callbackNoProgressMs:distribution(edgeIntervals(rows.map(r=>r.entry))),
    injectionTimerLateness:distribution(measuredWire.map(e=>e.lateness)),actualSendDelay:distribution(measuredWire.map(e=>e.actual-e.scheduled)),
    sourceClock:{timeOrigin:data.timeOrigin,note:'All page deltas use this document performance clock; never subtract peer clocks.'},
    rollbackFirst:first,rollbackLast:last,overflow:data.overflow,probeErrors:data.errors,
    maxQueued:data.maxQueued,maxQueuedBytes:data.maxQueuedBytes};
}

try{
  mark('preflight');
  const musicCatalogFile=resolve(root,'web/cpp/game/MusicCatalog.hpp');
  const musicText=readFileSync(musicCatalogFile,'utf8'),musicTracks=new Map();
  for(const m of musicText.matchAll(/\{(\d+),"(th(?:10|11)_\d+)"\}/g))musicTracks.set(Number(m[1]),m[2]+'.ogg');
  assert.equal(musicTracks.size,18,'Read actual native MusicCatalog, including result cue');
  const musicFiles=new Map();for(const [cue,name] of musicTracks){const path=resolve(musicRoot,name);
    if(!existsSync(path))blocked('Full audio gate requires actual progressive OGG asset: '+path);
    musicFiles.set(name,{cue,...fileIdentity(path)});}
  report.musicCatalog=fileIdentity(musicCatalogFile);report.musicFiles=Array.from(musicFiles,([name,value])=>({name,...value}));
  const relayFile=resolve(launcher,'services/netplay/netplay-relay.mjs');
  const catalogFile=resolve(launcher,'launcher/web/lib/contracts/product-catalog.mjs');
  const relayUrlFile=resolve(launcher,'launcher/web/.cache/build/browser/assets/launcher/multiplayer-relay-url.mjs');
  const optionsFile=resolve(launcher,'launcher/web/.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs');
  for(const file of [relayFile,catalogFile,relayUrlFile,optionsFile,resolve(diagnostics,'node_modules/playwright/package.json')])
    if(!existsSync(file))blocked('Missing migrated dependency: '+file);
  const catalog=await import(pathToFileURL(catalogFile)),policy=catalog.PRODUCT_GAMES.th11.multiplayer;
  const timing=catalog.multiplayerInputTimingPolicy(policy);report.policy={inputTiming:policy.inputTiming,resolved:timing};
  // The same actual contract is imported by the unmodified relay service.
  // Never admit a P0 room then secretly substitute a rollback runtime.
  if(!timing.rollback||timing.predictionLimit!==prediction)blocked('Actual TH11 relay product contract must permit Direct mode and P12; current '+JSON.stringify(report.policy));
  const {buildMultiplayerGameplayRelayUrl}=await import(pathToFileURL(relayUrlFile));
  const {buildMultiplayerRuntimeOptions}=await import(pathToFileURL(optionsFile));
  for(const file of [archive,resolve(runtime,'manifest.json'),resolve(runtime,'runtime-files.json'),resolve(runtime,'th11-sdl.wasm')])
    if(!existsSync(file))blocked('Missing built production fixture: '+file);
  const manifest=JSON.parse(readFileSync(resolve(runtime,'manifest.json'))),inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json')));
  assert.equal(manifest.product,'th11mp');assert.equal(manifest.variant,'multiplayer');
  const wasm=fileIdentity(resolve(runtime,'th11-sdl.wasm'));assert.equal(wasm.sha256,manifest.execution.sha256,'Actual WASM agrees with untouched manifest');
  report.identity={game:gitIdentity(root),common:gitIdentity(resolve(root,'../../common')),launcher:gitIdentity(launcher),
    archive:fileIdentity(archive),manifest:fileIdentity(resolve(runtime,'manifest.json')),inventory:fileIdentity(resolve(runtime,'runtime-files.json')),
    wasm,loader:fileIdentity(resolve(runtime,'th11-sdl.mjs')),shell:fileIdentity(resolve(runtime,'shell.mjs')),
    relay:fileIdentity(relayFile),catalog:fileIdentity(catalogFile),runtimeOptions:fileIdentity(optionsFile),relayUrl:fileIdentity(relayUrlFile),runner:fileIdentity(import.meta.filename),host:fileIdentity(resolve(import.meta.dirname,'browser-host.html'))};
  const pinned=new Map([report.identity.wasm,report.identity.loader,report.identity.shell,report.identity.manifest,report.identity.inventory,
    report.identity.relay,report.identity.catalog,report.identity.runtimeOptions,report.identity.relayUrl,report.identity.runner,report.identity.host,report.musicCatalog,...musicFiles.values()].map(f=>[f.path,f.sha256]));
  const require=createRequire(resolve(diagnostics,'package.json')),{chromium}=require('playwright');
  const browserTemp=resolve(out,'browser-temp');mkdirSync(browserTemp,{recursive:true});
  // Scope temporary profiles/downloads to this owned test process and its children.
  // The machine's C: temp volume can be full; never clean another user's files.
  process.env.TEMP=browserTemp;process.env.TMP=browserTemp;process.env.TMPDIR=browserTemp;
  report.browserTemp=browserTemp;
  const chrome=option('--chrome',process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE||chromium.executablePath());
  if(!existsSync(chrome))blocked('Installed desktop Chrome executable missing; pass --chrome (no download/install): '+chrome);
  report.browserExecutable=fileIdentity(chrome);report.playwright=require('playwright/package.json').version;
  const gpu=option('--gpu','hardware');assert.ok(['hardware','swiftshader'].includes(gpu));report.settings.gpuRequested=gpu;
  // Child owns a separate event loop. Heartbeats are evidence of timer lateness,
  // not proof of per-packet forwarding; no cross-process clock subtraction.
  const relayBootstrap="let prior=performance.now();setInterval(()=>{const now=performance.now();process.send?.({kind:'heartbeat',now,late:now-prior-50});prior=now;},50).unref();await import("+JSON.stringify(pathToFileURL(relayFile).href)+");";
  relay=spawn(process.execPath,['--input-type=module','--eval',relayBootstrap],{cwd:dirname(relayFile),windowsHide:true,
    env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',
      EAGLER_NETPLAY_RELAY_DELAY_MS:'0',EAGLER_NETPLAY_RELAY_JITTER_MS:'0'},
    stdio:['ignore','pipe','pipe','ipc']});
  report.relayProcess={pid:relay.pid,entry:relayFile};
  report.relaySegments=[];report.relayHeartbeatTotal=0;
  relay.on('message',m=>{if(m?.kind==='heartbeat'){++report.relayHeartbeatTotal;if(relayHeartbeats.length<10000)relayHeartbeats.push(m);else relayLogOverflow=true;}});
  const relayOutput=b=>{if(relayLog.length+b.length>4*1024*1024){relayLogOverflow=true;return;}relayLog+=b.toString();};
  relay.stdout.on('data',relayOutput);relay.stderr.on('data',relayOutput);
  const deadline=Date.now()+15000;
  while(!/listening ws:\/\/127\.0\.0\.1:\d+/.test(relayLog)){if(relay.exitCode!==null||Date.now()>deadline)throw Error('Relay startup failed: '+relayLog);await pause(25);}
  const relayUrl=relayLog.match(/listening (ws:\/\/127\.0\.0\.1:\d+)/)[1];
  const html=readFileSync(resolve(import.meta.dirname,'browser-host.html'));
  const mime={'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html','.json':'application/json','.wasm':'application/wasm','.css':'text/css'};
  server=createServer((req,res)=>{
    const transfer={url:req.url,started:performance.now(),expectedBytes:null,readBytes:0,finished:false,closed:false};
    report.http.push(transfer);
    res.on('finish',()=>{transfer.finished=true;transfer.finishedAt=performance.now();});
    res.on('close',()=>{transfer.closed=true;transfer.closedAt=performance.now();});
    try{
      const pathname=new URL(req.url,'http://localhost').pathname;res.setHeader('Cache-Control','no-store');
      if(pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
      if(pathname==='/favicon.ico'){res.writeHead(204);res.end();return;}
      let file,contentType;
      if(pathname.startsWith('/music/')){const name=decodeURIComponent(pathname.slice(7)),f=musicFiles.get(name);
        if(f){file=f.path;contentType='audio/ogg';}}
      else if(pathname==='/data/th11')file=archive;
      else if(pathname.startsWith('/runtime/')){
        const name=decodeURIComponent(pathname.slice(9)),candidate=resolve(runtime,name),rel=relative(runtime,candidate);
        if(!isAbsolute(rel)&&rel!== '..'&&!rel.startsWith('..'+sep)&&
          (name==='runtime-files.json'||Object.hasOwn(inventory.files,name)))file=candidate;
      }
      if(!file||!existsSync(file)){res.writeHead(404);res.end('Unknown fixture');return;}
      transfer.file=file;transfer.expectedBytes=statSync(file).size;
      res.setHeader('Content-Type',contentType||mime[extname(file)]||'application/octet-stream');
      res.setHeader('Content-Length',String(transfer.expectedBytes));
      const stream=createReadStream(file);stream.on('data',chunk=>{transfer.readBytes+=chunk.length;});
      pipeline(stream,res,error=>{if(error){transfer.error=String(error);
        recordError({kind:'server-transfer',url:req.url,error:String(error),checkerMs:performance.now()});}});
    }catch(error){
      if(res.headersSent)res.destroy(error);else{res.writeHead(500);res.end(String(error));}
      recordError({kind:'server',url:req.url,error:String(error),checkerMs:performance.now()});
    }
  });
  await new Promise((yes,no)=>{server.once('error',no);server.listen(0,'127.0.0.1',yes);});
  server.keepAliveTimeout=65000;server.headersTimeout=70000;
  report.httpTimeouts={keepAliveMs:server.keepAliveTimeout,headersMs:server.headersTimeout};
  const base='http://127.0.0.1:'+server.address().port,roomCode=randomUUID().replaceAll('-','').slice(0,12),room='th11mp-'+roomCode;
  mark('launch-independent-browsers');
  for(let seat=0;seat<players;++seat){
    const args=['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding',
      ...(gpuDiagnostics?['--enable-logging=stderr','--log-level=0','--vmodule=gpu_process_host=1,gpu_watchdog_thread=1']:[]),
      '--autoplay-policy=no-user-gesture-required',...(gpu==='swiftshader'?['--enable-unsafe-swiftshader']:
        process.platform==='win32'?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-gpu'])];
    const ignoreDefaultArgs=['--mute-audio'];
    const browser=await chromium.launch({headless:true,executablePath:chrome,args,ignoreDefaultArgs});browsers.push(browser);
    let browserPid=null;
    browser.once('disconnected',()=>recordLifecycle('browser-disconnected',seat,browserPid));
    const connection=await browser.newBrowserCDPSession();
    const processes=await connection.send('SystemInfo.getProcessInfo'),system=await connection.send('SystemInfo.getInfo');
    browserPid=processes.processInfo.find(p=>p.type==='browser')?.id;
    assert.ok(browserPid,'Chrome process identity available');report.processIds.push(browserPid);
    report.browser.push({seat,version:browser.version(),pid:browserPid,args,ignoreDefaultArgs,audioOutputMutedByPlaywright:false,gpu:system.gpu,processes:processes.processInfo});
    const context=await browser.newContext({viewport:{width:660,height:510},serviceWorkers:'block'});contexts.push(context);
    context.once('close',()=>recordLifecycle('context-close',seat,browserPid));
    if(gpuDiagnostics)await context.addInitScript(installGpuResourceProbe);
    await context.addInitScript(installProbe,{seat,route,latency,jitter,duration,profile,bombStress,gpuDiagnostics});
    const page=await context.newPage();pages.push(page);
    page.once('close',()=>recordLifecycle('page-close',seat,browserPid,page.url()));
    page.on('requestfailed',request=>{
      let frameUrl=null,frameError=null;try{frameUrl=request.frame().url();}catch(error){frameError=String(error);}
      recordError({seat,kind:'request-failed',url:request.url(),method:request.method(),failure:request.failure(),
        checkerMs:performance.now(),frameError,...browserErrorSource('request-failed',request.url(),page.url(),frameUrl)});
    });
    page.on('pageerror',e=>recordError({seat,kind:'pageerror',error:String(e),stack:e.stack||null,
      checkerMs:performance.now(),...browserErrorSource('pageerror',null,page.url()),frameUrls:page.frames().map(f=>f.url())}));
    page.once('crash',()=>recordLifecycle('page-crash',seat,browserPid,page.url()));
    page.on('console',m=>{if(m.type()==='error'){const location=m.location();
      recordError({seat,kind:'console-error',error:m.text(),location,checkerMs:performance.now(),
        ...browserErrorSource('console-error',location.url,page.url()),frameUrls:page.frames().map(f=>f.url())});
    }});
    const cdp=await context.newCDPSession(page);cdps.push(cdp);
    await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpu});report.browser[seat].cpuThrottle={requested:cpu,cdpAcknowledged:true};
    await page.goto(base);await page.evaluate(()=>host.open());
    await page.waitForFunction(()=>host.ready()||host.events.some(m=>m.event==='error'),null,{timeout:120000});
    const startup=await page.evaluate(()=>({ready:host.ready(),errors:host.events.filter(m=>m.event==='error')}));
    assert.ok(startup.ready&&!startup.errors.length,'Managed initialization failed: '+JSON.stringify(startup));
  }
  assert.equal(new Set(report.processIds).size,players,'Three independently launched Chrome browser PIDs required');
  const ids=[0,1,2].map(s=>'th11_full_'+s+'_'+randomUUID().slice(0,8)),selections=bombStress?[0,2,4]:[0,3,5];
  for(let seat=0;seat<players;++seat)await pages[seat].evaluate(async([url,id,seat,selection,difficulty,challenge])=>{
    // Existing host.lobby hardcodes Normal; request the actual selected difficulty.
    const target=new URL(url);target.searchParams.delete('run');target.searchParams.set('lobby',id);
    target.searchParams.set('intent',seat===0?'create':'join');target.searchParams.set('member','member_'+id);
    target.searchParams.set('players','3');target.searchParams.set('difficulty',String(difficulty));
    target.searchParams.set('challengeMode',challenge?'1':'0');
    const socket=new WebSocket(target),state={socket,id,messages:[],room:null};host.lobbyState=state;
    socket.addEventListener('message',e=>{const m=JSON.parse(String(e.data));state.messages.push(m);if(m.room)state.room=m.room;});
    socket.addEventListener('error',()=>state.error='Lobby connection error');
    socket.addEventListener('close',e=>state.error='Lobby closed '+e.code+' '+e.reason);
    state.until=async predicate=>{const deadline=performance.now()+15000;while(!predicate()){
      if(state.error||performance.now()>deadline)throw Error(state.error||'Lobby timeout');await new Promise(r=>setTimeout(r,10));}};
    await state.until(()=>state.room);socket.send(JSON.stringify({type:'take-seat',seat,loadout:selection,ready:false}));
    await state.until(()=>state.room.seats[seat]?.clientId===id);
  },[relayUrl+'/?room='+room+'&run=1',ids[seat],seat,selections[seat],difficulty,challenge]);
  await Promise.all(pages.map(p=>p.evaluate(()=>host.readyLobby())));
  report.startRoom=await pages[0].evaluate(async()=>{
    const s=host.lobbyState;await s.until(()=>s.room.seats.filter(Boolean).every(p=>p.ready));
    s.socket.send(JSON.stringify({type:'start',adonisMode:0,inputDelayAuto:false,inputDelay:0,predictionReserve:2,predictionLimit:12}));
    await s.until(()=>s.messages.some(m=>m.type==='start')||s.messages.some(m=>m.type==='error'));
    const start=s.messages.find(m=>m.type==='start');if(!start)throw Error(JSON.stringify(s.messages.filter(m=>m.type==='error')));return start;
  });
  assert.equal(report.startRoom.room.adonisMode,0);assert.equal(report.startRoom.room.inputDelay,0);
  assert.equal(report.startRoom.room.predictionLimit,prediction,'Relay and native prediction policy must agree');
  const approved=report.startRoom.room;
  assert.equal(approved.playerCount,players);assert.equal(approved.difficulty,difficulty);
  assert.equal(approved.inputDelayAuto,false);assert.equal(approved.challengeMode,challenge);
  const active=approved.seats.slice(0,approved.playerCount);assert.equal(active.length,players);
  active.forEach((s,i)=>{assert.ok(s);assert.equal(s.clientId,ids[i]);assert.equal(s.loadout,selections[i]);
    assert.ok(Number.isInteger(s.loadout)&&s.loadout>=0&&s.loadout<policy.loadouts.length);});
  const loadouts=active.map(s=>{const {character,shot}=policy.loadouts[s.loadout];return {character,shot};});
  report.fixtureSeed={value:1234,source:'fixed deterministic acceptance seed; raw relay snapshot has no seed'};
  report.runtimeOptions=[];
  mark('real-relay-admitted');
  for(let seat=0;seat<players;++seat){
    const url=buildMultiplayerGameplayRelayUrl(relayUrl,{product:'th11mp',roomCode,runId:report.startRoom.serial,role:{player:seat},memberId:'member_'+ids[seat]});
    const options={...buildMultiplayerRuntimeOptions({url,player:seat,playerCount:approved.playerCount,
      seed:report.fixtureSeed.value,difficulty:approved.difficulty,inputDelay:approved.inputDelay,
      inputDelayAuto:approved.inputDelayAuto,predictionReserve:approved.predictionReserve,
      adonisMode:approved.adonisMode,predictionLimit:approved.predictionLimit,challengeMode:approved.challengeMode,
      spectator:false,spectatorId:'',spectatorCount:approved.spectatorCount,iceServers:[],loadouts},policy),
      touchEnabled:true,touchMovementMode:'touch',touchSensitivity:100,doubleTapBombEnabled:false};
    report.runtimeOptions.push(options);
    const initial=[0,initialMusic].map(cue=>{const name=musicTracks.get(cue),file=musicFiles.get(name);return {path:'/music/'+name,url:base+'/music/'+name,bytes:file.bytes};});
    await pages[seat].evaluate(async({options,resources})=>{
      await host.request('configure',{language:'ja',music:'ogg',options,resources});
      await host.launch();document.querySelector('iframe').contentWindow.__rollbackAcceptance.attach();
    },{options,resources:initial});
  }
  async function smalls(){return Promise.all(pages.map(p=>p.evaluate(()=>document.querySelector('iframe').contentWindow.__rollbackAcceptance.small())));}
  const capabilities=await smalls();
  for(const s of capabilities){
    assert.equal(s.rollback.length,24,'Built package must export real rollback status ABI');
    assert.equal(s.scene.length,12,'Built package must export real scene status ABI');
    assert.equal(s.scene[0],1,'Require agreed scene ABI version 1');
  }
  const warmDeadline=Date.now()+45000;let warm;
  while(true){warm=await smalls();
    for(const s of warm)assert.ok(!s.error&&!s.shellError&&!s.probeErrors.length,JSON.stringify(s));
    if(warm.every(s=>s.net[2]&&s.net[3]>=125&&s.rollback[0]===1&&s.rollback[1]&&s.rollback[2]&&!s.rollback[3]))break;
    if(Date.now()>warmDeadline)throw Error('Stable rollback-capable world did not activate: '+JSON.stringify(warm));await pause(50);
  }
  report.webgl=await Promise.all(pages.map(p=>p.evaluate(()=>{
    const canvas=document.querySelector('iframe').contentDocument.querySelector('canvas'),gl=canvas?.getContext('webgl2');
    if(!gl)return {available:false};const ext=gl.getExtension('WEBGL_debug_renderer_info');return {available:true,
      renderer:gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER),vendor:gl.getParameter(ext?ext.UNMASKED_VENDOR_WEBGL:gl.VENDOR)};
  })));
  report.warmup=warm;for(const s of warm){assert.equal(s.route,route);assert.equal(s.net[7],0);}
  async function routes(){return Promise.all(pages.map(p=>p.evaluate(async()=>{
    const r=document.querySelector('iframe').contentWindow,t=r.__eaglerPeerTransport,out={route:t?.route,native:Array.from(new Uint32Array(r.core.memory.buffer,r.core.th11_mp_status(),24))[20],pairs:[]};
    for(const [id,peer] of t?.peers||[]){const stats=await peer.pc.getStats();let selected;
      for(const s of stats.values())if(s.type==='transport'&&s.selectedCandidatePairId)selected=stats.get(s.selectedCandidatePairId);
      if(!selected)for(const s of stats.values())if(s.type==='candidate-pair'&&s.state==='succeeded'&&s.nominated)selected=s;
      if(selected)out.pairs.push({peer:id,pair:selected,local:stats.get(selected.localCandidateId),remote:stats.get(selected.remoteCandidateId)});
    }return out;
  })));}
  report.network=await routes();
  for(const r of report.network){assert.equal(r.route,route);assert.equal(r.native,route==='rtc'?1:2);
    if(route==='rtc'){assert.equal(r.pairs.length,2,'Every 3P RTC peer pair must be observed');for(const pair of r.pairs){assert.notEqual(pair.local?.candidateType,'relay');assert.notEqual(pair.remote?.candidateType,'relay');}}}

  report.firstGeneration=warm[0].net[12];
  assert.ok(warm.every(s=>s.net[12]===report.firstGeneration&&s.net[5]===3));
  const loadedMusic=pages.map(()=>new Set([musicTracks.get(0),musicTracks.get(initialMusic)]));
  async function deliverMusic(states){
    for(let seat=0;seat<players;++seat){const cues=new Set([states[seat].audio[5]]);
      const stage=states[seat].game[2];if(stage>=1&&stage<=7){cues.add((stage-1)*2+1);cues.add((stage-1)*2+2);}
      for(const cue of cues){const name=musicTracks.get(cue);if(!name||loadedMusic[seat].has(name))continue;
        const file=musicFiles.get(name),began=performance.now();
        // Normal progressive Host resource write wakes the native pending cue.
        await pages[seat].evaluate(resource=>host.request('resources',{resources:[resource]}),
          {path:'/music/'+name,url:base+'/music/'+name,bytes:file.bytes});
        loadedMusic[seat].add(name);report.musicTransfers.push({seat,cue,name,bytes:file.bytes,sha256:file.sha256,checkerMs:began,completedMs:performance.now()});
      }
    }
  }
  await deliverMusic(warm);
  report.validationWindow.phase='validation';report.validationWindow.measurementStartedMs=performance.now();
  mark('continuous-run-start');
  await Promise.all(pages.map((page,seat)=>page.evaluate(({seat,bombStress})=>{
    const r=document.querySelector('iframe').contentWindow,probe=r.__rollbackAcceptance;
    const t=window.__fullTouch={start:performance.now(),submitted:0,acks:0,lateSlots:0,errors:[],events:[],
      serial:0,bombSerial:0,escapeSerial:0,bombPulses:[],steer:{x:0,y:0},pointAt:0,lastPoint:null,
      active:true,mode:null,phase:null,index:1,maxEventCount:8000};
    function event(row){if(t.events.length<8000)t.events.push(row);else if(!t.errors.includes('touch event buffer overflow'))t.errors.push('touch event buffer overflow');}
    const send=(command,fields={},planned=performance.now())=>{
      const id='full-touch-'+seat+'-'+(++t.serial),sent=performance.now();++t.submitted;
      event({id,command,planned,sent,lateness:sent-planned,...(command==='direct-touch'?{point:{x:fields.x,y:fields.y,type:fields.type}}:command==='touch-controls'?{controls:{...fields.controls}}:{})});
      r.postMessage({protocol:'eagler-touhou/1',game:'th11',epoch:1,request:id,command,...fields},location.origin);return id;
    };
    window.addEventListener('message',e=>{if(e.source!==r||e.origin!==location.origin)return;
      const m=e.data;if(typeof m?.request==='string'&&m.request.startsWith('full-touch-'+seat+'-')){++t.acks;const pulse=t.bombPulses.find(p=>p.request===m.request);if(pulse){pulse.ackAt=performance.now();pulse.ackOk=!!m.ok;}if(!m.ok){if(t.errors.length<100)t.errors.push(m.error);else t.errorOverflow=true;}}});
    const controls=fire=>({controls:{fireEnabled:fire,focusEnabled:false,bombSerial:t.bombSerial,escapeSerial:t.escapeSerial,joystickX:0,joystickY:0}});
    const word=(name,count)=>Array.from(new Uint32Array(r.core.memory.buffer,r.core[name](),count));
    t.bombClock=()=>{const n=word('th11_mp_status',24),b=word('th11_mp_rollback_status',24);
      return {at:performance.now(),generation:n[12],next:n[3],capture:b[6],phase:r.core.th11_phase(),configured:!!n[1],error:!!(n[23]||b[23])};};
    t.pulseBomb=()=>{
      if(!bombStress||t.mode!=='battle'||t.bombPulses.length)throw Error('Bomb pulse requires its one normal Stage input edge');
      const game=word('th11_mp_game_status',56),bomb=word('th11_mp_bomb_status',24),p=8+seat*16;
      if(!bomb[2]||game[p]!==1||game[p+1]||game[p+2]!==1||!game[p+6]||game[p+4]<game[p+6]||bomb[13+seat*4])
        throw Error('Bomb pulse seat is not alive, powered and inactive');
      const pulse={seat,before:t.bombClock(),serial:++t.bombSerial,ackAt:null,ackOk:null};
      pulse.request=send('touch-controls',controls(true));t.bombPulses.push(pulse);return pulse;
    };
    const point=elapsed=>{
      if(!bombStress)return {x:(224+(seat-1)*24+42*Math.sin(elapsed*.0007+seat*.8))/640,y:(386+24*Math.sin(elapsed*.00051+seat))/480};
      const dt=t.pointAt?Math.min(30,Math.max(0,elapsed-t.pointAt)):0;t.pointAt=elapsed;
      const old=t.lastPoint||{x:(224+(seat-1)*24)/640,y:386/480};
      return t.lastPoint={x:Math.max(.02,Math.min(.98,old.x+t.steer.x*dt*.00025)),
        y:Math.max(.02,Math.min(.98,old.y+t.steer.y*dt*.0004))};
    };
    function enter(mode){if(t.mode===mode)return;
      if(t.mode==='battle'){send('touch-cancel');send('touch-controls',controls(false));}
      t.mode=mode;
      if(mode==='battle'){send('touch-controls',controls(true));send('direct-touch',{type:'down',id:101+seat,...point(performance.now()-t.start)});}
    }
    t.finish=()=>{enter('stopped');t.active=false;};
    t.segment=()=>{const rows=t.events;t.events=[];return {start:t.start,submitted:t.submitted,acks:t.acks,lateSlots:t.lateSlots,errors:[...t.errors,...(t.errorOverflow?['touch error overflow']:[])],events:rows,active:t.active};};
    probe.start();
    const step=1000/120;
    function pump(){if(!t.active)return;const now=performance.now(),phase=r.core.th11_phase();
      t.phase=phase;enter(phase===1?'battle':'menu');
      const slot=Math.max(t.index,Math.floor((now-t.start)/step));t.lateSlots+=Math.max(0,slot-t.index);
      if(phase===1)send('direct-touch',{type:'move',id:101+seat,...point(now-t.start)},t.start+slot*step);
      t.index=slot+1;setTimeout(pump,Math.max(0,t.start+t.index*step-performance.now()));
    }
    pump();
  },{seat,bombStress})));
  // Histograms have a fixed 0.5 ms resolution and a capped overflow bin.
  // Exact maxima and every raw segment remain available separately.
  const aggregate=new Map(),lastChain=['','',''],lastSegmentStates=[null,null,null],audioSeen=[new Set(),new Set(),new Set()];
  const audioWatch=pages.map(()=>({active:false,lastCounter:0,lastChangedMs:0,observations:0,progressEvents:0,maxStallMs:0}));
  const coverage=pages.map(()=>({restores:0,historyTicks:0,movedPlayers:[false,false,false]}));
  const aggregateFailures=new Set(),keyEvidence=[];
  report.keyInputContract={minimumCaptureFrames:2,minimumSimulationFrames:2,edgeTimeoutMs:6000,
    extraSlotReadyFrames:12,terminalRelease:'release is sent; after a confirmed terminal transaction there is no next input frame'};
  report.keyInputSummary={count:0,errors:0,heldMaxMs:0,releasedMaxMs:0,terminalReleases:0};
  report.audioProgressGate={continuousStageStallMs:3000,networkWaitExempt:false,meaning:'PCM mix/SDL queue progress; neither per-SFX correctness nor audible-output proof'};
  function accumulator(){return {count:0,sum:0,max:0,over50:0,over100:0,hist:Array(2002).fill(0)};}
  function add(acc,value){if(!Number.isFinite(value)||value<0)return;++acc.count;acc.sum+=value;acc.max=Math.max(acc.max,value);
    if(value>50)++acc.over50;if(value>100)++acc.over100;++acc.hist[Math.min(2001,Math.floor(value*2))];}
  function summary(acc){function p(q){if(!acc.count)return null;let n=0;const wanted=Math.ceil(acc.count*q);for(let i=0;i<acc.hist.length;++i){n+=acc.hist[i];if(n>=wanted)return i===2001?'>1000.5':i*.5;}}
    return {count:acc.count,mean:acc.count?acc.sum/acc.count:null,max:acc.max|| (acc.count?0:null),p95:p(.95),p99:p(.99),over50:acc.over50,over100:acc.over100,histogramResolutionMs:.5};}
  function updateAggregate(seat,data){
    let prior=lastSegmentStates[seat];
    for(const row of data.raf){
      if(prior?.nativePhase===1&&(row.nativePhase!==1||row.stage!==prior.stage)){
        const previous=aggregate.get(seat+':'+prior.stage);
        if(previous){previous.lastPresent=null;previous.lastFrontier=null;previous.priorPresent=null;previous.highFrame=null;}
      }
      if(row.nativePhase!==1||row.stage<1||row.stage>7){prior=row;continue;}
      const key=seat+':'+row.stage;
      let a=aggregate.get(key);if(!a){a={seat,stage:row.stage,native:accumulator(),rafEntryLag:accumulator(),callback:accumulator(),
        presentHold:accumulator(),frontierHold:accumulator(),networkWaitCallbacks:0,cpuYieldCallbacks:0,
        peakDepth:0,peakBytes:0,firstFrame:row.net[3],lastFrame:row.net[3],lastPresent:null,lastFrontier:null,highFrame:null,priorPresent:null};aggregate.set(key,a);}
      add(a.native,row.nativeMs);add(a.rafEntryLag,row.entry-row.stamp);
      if(prior?.nativePhase===1&&prior.stage===row.stage)add(a.callback,row.entry-prior.entry);
      if(row.rollback[15])++a.networkWaitCallbacks;if(row.rollback[16])++a.cpuYieldCallbacks;
      a.peakDepth=Math.max(a.peakDepth,row.rollback[12]);a.peakBytes=Math.max(a.peakBytes,row.rollback[13]);a.lastFrame=row.net[3];
      if(a.lastPresent===null)a.lastPresent=row.entry;
      if(a.lastFrontier===null)a.lastFrontier=row.entry;
      // Keep exact open-interval maxima across segment boundaries. Menus and
      // pauses end continuity; a 150 ms hold cannot become two passing 75 ms holds.
      a.presentHold.max=Math.max(a.presentHold.max,row.exit-a.lastPresent);
      a.frontierHold.max=Math.max(a.frontierHold.max,row.exit-a.lastFrontier);
      if(a.priorPresent!==null&&row.rollback[21]>a.priorPresent){add(a.presentHold,row.exit-a.lastPresent);a.lastPresent=row.exit;}
      if(a.highFrame!==null&&row.net[3]>a.highFrame){add(a.frontierHold,row.exit-a.lastFrontier);a.lastFrontier=row.exit;}
      a.highFrame=Math.max(a.highFrame??0,row.net[3]);a.priorPresent=row.rollback[21];prior=row;
    }
    lastSegmentStates[seat]=prior;
    for(const sample of data.motion)if(sample.audio?.[7]>0&&!sample.audio[8])audioSeen[seat].add(sample.audio[5]);
  }
  function perfSummary(){return [...aggregate.values()].map(a=>({seat:a.seat,stage:a.stage,firstFrame:a.firstFrame,lastFrame:a.lastFrame,
    native:summary(a.native),rafTimestampToEntry:summary(a.rafEntryLag),callbackIntervals:summary(a.callback),
    presentIntervals:summary(a.presentHold),frontierIntervals:summary(a.frontierHold),
    networkWaitCallbacks:a.networkWaitCallbacks,cpuYieldCallbacks:a.cpuYieldCallbacks,peakDepth:a.peakDepth,peakBytes:a.peakBytes}));}
  let segmentIndex=0,lastDrain=performance.now(),runStarted=performance.now(),lastProgress=0;
  let lastStates=warm,lastStageChange=runStarted,observedEnding=false,observedResults=false,common=null,lastEndingCapture=0;
  report.staffEvidence={required:!extra,directSceneAbi:true,naturalEndingToResults:false,periodicScreenshots:[],peers:[],passed:false,
    limitation:extra?'Extra ends through its original compact Results without Ending/Staff':'Read-only native state proves script phase/progress; screenshots remain visual evidence'};
  async function drainSegment(label='running'){
    const index=++segmentIndex,keys=keyEvidence.splice(0),snapshots=await captureSegments();
    let captureFailure=null;const preserved=[];
    for(const snapshot of snapshots){
      const {seat,timing}=snapshot,name='segment-'+String(index).padStart(4,'0')+'-peer-'+seat+'.json',file=resolve(out,name);
      try{
        if(snapshot.error)throw snapshot.error;
        assert.ok(snapshot.done&&snapshot.timing.complete,'Incomplete segment cannot enter analysis');
        const {data}=snapshot,analysisAt=performance.now();
        updateAggregate(seat,data.probe);const metrics=analysis(data.probe);
        coverage[seat].restores+=Math.max(0,metrics.rollbackDelta.restores);
        coverage[seat].historyTicks+=Math.max(0,metrics.rollbackDelta.historyTicks);
        metrics.moves.forEach((m,i)=>{if(m.distinct>1)coverage[seat].movedPlayers[i]=true;});
        timing.checkerAnalysisMs=performance.now()-analysisAt;
        const payload={schema:'th11-fullgame-segment/1',index,seat,label,generation:report.firstGeneration,
          previousSha256:lastChain[seat],...data,metrics,snapshotTiming:timing,...(seat===0?{keyInputs:keys}:{})};
        const encodeAt=performance.now(),encoded=JSON.stringify(payload)+'\n';timing.checkerEncodeMs=performance.now()-encodeAt;
        const digest=writeSnapshotFile(file,encoded,timing);lastChain[seat]=digest;preserved.push(seat);
        // The receipt owns post-write timing; the immutable raw file cannot
        // include its own completed write/hash measurements.
        report.segments.push({index,seat,label,file:name,sha256:digest,snapshotTiming:timing,from:data.probe.start,to:data.probe.end,callbacks:metrics.callbacks,
          stages:metrics.stages,nativePhases:metrics.nativePhases,nativeMax:metrics.nativeReportedWall.max,presentMax:metrics.presentWindowHolds.max,
          firstFrame:data.probe.raf[0]?.net?.[3]??null,lastFrame:data.probe.raf.at(-1)?.net?.[3]??null});
        if(data.probe.overflow.length||data.probe.errors.length||data.touch.errors.length)throw Error('Bounded observer/input buffer integrity failure on seat '+seat);
        if(metrics.audio.errors.length)throw Error('Audio mix/SDL failure on seat '+seat);
        // Retain every long hold; a later completed course cannot erase it.
        if(metrics.nativePhases.length===1&&metrics.nativePhases[0]===1&&metrics.stages.length===1&&(metrics.nativeReportedWall.max>maxGap||metrics.presentWindowHolds.max>maxGap||metrics.newFrontierIntervals.max>maxGap))
          report.failures.push({kind:'performance',index,seat,nativeMax:metrics.nativeReportedWall.max,presentHold:metrics.presentWindowHolds.max,frontierMax:metrics.newFrontierIntervals.max,phases:metrics.nativePhases});
      }catch(error){
        captureFailure??=error;report.snapshotErrors.push({kind:'segment',index,seat,error:String(error),snapshotTiming:timing});
        if(snapshot.wires.length&&!preserved.includes(seat)){
          // Preserve every received block even if reconstruction/analysis fails.
          try{const raw=name.replace(/\.json$/,'.snapshot-wire.txt');const sha256=writeSnapshotFile(resolve(out,raw),snapshot.wires.join('\n')+'\n',timing);
            report.snapshotErrors.at(-1).raw={file:raw,sha256};preserved.push(seat);}
          catch(preserveError){report.cleanup.push({seat,action:'preserve-segment-wire',error:String(preserveError)});}
        }
      }
    }
    await Promise.all(preserved.map(async seat=>{
      try{const snapshot=snapshots[seat];if(!snapshot.done)return;
        const ack=JSON.parse(await bounded(pages[seat].evaluate(browserSegmentTransfer,{action:'ack',id:snapshot.id,sequence:snapshot.nextSequence}),freezeTimeoutMs,'release persisted segment peer '+seat));
        assert.equal(ack.id,snapshot.id);assert.equal(ack.released,true);}
      catch(error){captureFailure??=error;report.snapshotErrors.push({kind:'segment-release',index,seat,error:String(error)});}
    }));
    if(captureFailure){firstFailure??=captureFailure;report.failedSegmentKeyInputs={index,keys};save();throw captureFailure;}
    const heartbeatFile='relay-heartbeats-segment-'+String(index).padStart(4,'0')+'.json';
    const heartbeatData=JSON.stringify(relayHeartbeats.splice(0))+'\n';writeFileSync(resolve(out,heartbeatFile),heartbeatData);
    report.relaySegments.push({index,file:heartbeatFile,sha256:hash(heartbeatData)});
    report.performance=perfSummary();
    for(const a of report.performance){const key=a.seat+':'+a.stage;
      if(!aggregateFailures.has(key)&&(a.native.max>maxGap||a.presentIntervals.max>maxGap||a.frontierIntervals.max>maxGap)){
        aggregateFailures.add(key);report.failures.push({kind:'performance',scope:'continuous-stage-aggregate',seat:a.seat,stage:a.stage,
          nativeMax:a.native.max,presentHold:a.presentIntervals.max,frontierMax:a.frontierIntervals.max});}}
    report.audioHealth=audioWatch;report.rollbackMovementCoverage=coverage;report.progress={elapsedMs:performance.now()-runStarted,segmentIndex,lastStates,memoryMeaning:'WebAssembly memory reserved buffer byteLength; not live heap or a leak counter'};
    writeFileSync(resolve(out,'performance.json'),JSON.stringify(report.performance,null,2)+'\n');save();lastDrain=performance.now();
  }
  function terminalProof(x){
    const t=x.terminalEvidence;
    return !!(t?.fullyConfirmed&&t.source==='before-runtime-shell-hook'&&t.net?.[1]&&t.net[5]===players&&
      t.net[12]===report.firstGeneration&&t.scene?.[0]===1&&t.scene[1]===4&&t.scene[3]>0&&
      t.net[4]!==0xffffffff&&t.net[4]+1>=t.scene[3]&&t.net[3]>=t.scene[3]&&
      !t.net[23]&&t.rollback?.[0]===1&&!t.rollback[3]&&!t.rollback[23]&&t.audio?.[3]===0);
  }
  const successfulHostExit=x=>x.hostExits?.some(e=>e.code===0&&e.status==='success'&&!e.returnToMenu);
  const sfxHealth=Array.from({length:players},()=>({observations:0,first:null,last:null,terminalUsed:false}));
  function checkSfx(x,seat){
    // AudioDevice::close rebuilds Impl. A normal shell exit therefore uses the
    // last live sample captured before that hook; never merge two Impl lives.
    const terminal=x.audio?.[0]!==1&&terminalProof(x)?x.terminalEvidence:null;
    if(terminal)assert.equal(terminal.audio[0],1,'Terminal SFX proof must precede AudioDevice close');
    const value=terminal?.sfx||x.sfx,generation=terminal?.net?.[12]??x.net[12],watch=sfxHealth[seat];
    assert.equal(generation,report.firstGeneration,'SFX lifecycle generation');
    assert.equal(value?.length,120,'SFX ABI required');assert.equal(value[0],1);assert.equal(value[1],120);assert.equal(value[7],56);
    assert.equal(value[3],0,'ma_sound_start failure total');assert.equal(value[4],0,'Last native SFX start error');
    assert.equal(value[6],0,'Invalid native SFX voice call');
    assert.ok(value.slice(64,120).every(n=>n===0),'Per-ID native SFX start failure');
    // The bounded run cannot generate 2^32 starts. A decrease is an unexplained
    // Impl reset and breaks continuous evidence, rather than a new baseline.
    if(watch.last)for(const index of [2,...Array.from({length:56},(_,id)=>8+id)])
      assert.ok(value[index]>=watch.last.value[index],'SFX counters reset inside one observed audio life: '+seat+':'+index);
    const sample={checkerMs:performance.now(),generation,frame:terminal?.scene?.[3]??x.game[4],
      source:terminal?'confirmed-terminal-before-shell':'low-frequency-small',audio:terminal?.audio||x.audio,value:[...value]};
    watch.first??=sample;watch.last=sample;++watch.observations;watch.terminalUsed||=!!terminal;
    watch.successDelta=(value[2]-watch.first.value[2])>>>0;
    report.sfxHealth=sfxHealth;
  }
  function checkStates(states,{allowExited=false}={}){
    for(let seat=0;seat<players;++seat){const x=states[seat];
      assert.ok(!x.error&&!x.shellError&&!x.probeErrors.length&&!x.rollback[23]&&!x.hostErrors?.length,JSON.stringify({seat,...x}));
      checkSfx(x,seat);
      if(!x.net[1]){
        assert.ok(allowExited&&x.nativePhase===4&&terminalProof(x),'Cleared session needs a witnessed confirmed terminal transaction: '+JSON.stringify({seat,...x}));
        audioWatch[seat].active=false;
        continue;
      }
      assert.equal(x.net[12],report.firstGeneration,'A restarted generation cannot count as one complete run');
      assert.ok(x.net[3]<240000,'Stop before Replay capacity can masquerade as completion');
      assert.equal(x.net[5],3);assert.equal(x.audio[3],0,'Audio failure latch');
      assert.equal(x.scene.length,12,'Scene ABI remains available');assert.equal(x.scene[0],1);
      assert.equal(x.scene[1],x.nativePhase);assert.equal(x.scene[2],x.game[2]);assert.equal(x.scene[3],x.game[4]);
      if(extra)assert.notEqual(x.nativePhase,5,'Extra must retain its original no-Ending lifecycle');
      if(x.nativePhase!==4)assert.ok(!x.disconnected&&!x.net[13],'Unexpected retirement/disconnect');
      if(x.nativePhase!==4){assert.equal(x.audio[0],1,'Audio device ready');assert.equal(x.audioContext?.state,'running','Full audio gate requires a running AudioContext');}
      const watch=audioWatch[seat],now=performance.now();
      if(x.nativePhase===1&&x.audio[0]===1&&x.audioContext?.state==='running'){
        ++watch.observations;
        if(!watch.active||x.audio[2]!==watch.lastCounter){
          if(watch.active)++watch.progressEvents;
          watch.active=true;watch.lastCounter=x.audio[2];watch.lastChangedMs=now;
        }else{
          const stalled=now-watch.lastChangedMs;watch.maxStallMs=Math.max(watch.maxStallMs,stalled);
          assert.ok(stalled<3000,'PCM stalled during continuous Stage: '+JSON.stringify({seat,stalledMs:stalled,...x}));
        }
      }else watch.active=false;
      const visit=report.stageVisits[seat],stage=x.game[2];
      if(x.nativePhase===1&&visit.at(-1)?.stage!==stage){
        assert.equal(stage,expectedStages[visit.length],'Actual selected course stages must be visited in order, without a skip');
        visit.push({stage,frame:x.game[4],confirmed:x.net[4],checkerMs:performance.now()});lastStageChange=performance.now();
      }
    }
    requireNoBlockingErrors();
  }
  const inputClock=()=>pages[0].evaluate(()=>{
    const r=document.querySelector('iframe').contentWindow,n=Array.from(new Uint32Array(r.core.memory.buffer,r.core.th11_mp_status(),24)),
      b=Array.from(new Uint32Array(r.core.memory.buffer,r.core.th11_mp_rollback_status(),24));
    return {at:performance.now(),configured:!!n[1],generation:n[12],next:n[3],capture:b[6],error:!!(n[23]||b[23]),
      phase:r.core.th11_phase(),terminalEvidence:r.__rollbackAcceptance.state.terminal};
  });
  async function waitInputFrames(start,frames=2,allowTerminal=false){
    const deadline=performance.now()+6000;
    while(true){
      const value=await inputClock();
      if(allowTerminal&&terminalProof(value))return {start,end:value,terminal:true,wallMs:value.at-start.at};
      assert.ok(value.configured&&value.generation===report.firstGeneration&&!value.error,'Input lifecycle changed: '+JSON.stringify({start,value}));
      if(value.capture>=start.capture+frames&&value.next>=start.next+frames)
        return {start,end:value,terminal:false,wallMs:value.at-start.at,captureFrames:value.capture-start.capture,simulationFrames:value.next-start.next};
      if(performance.now()>deadline)throw Error('Host key did not cross actual input/simulation frames: '+JSON.stringify({start,value,frames}));
      await pause(8);
    }
  }
  async function hostKey(code,down){
    let timer;
    try{return await Promise.race([pages[0].evaluate(([code,down])=>host.key(code,down),[code,down]),
      new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error('Host keyboard ACK timed out: '+code+' '+down)),6000);})]);}
    finally{clearTimeout(timer);}
  }
  async function keyTap(code,{allowTerminal=false}={}){
    const entry={code,checkerStartedMs:performance.now(),allowTerminal};
    let failure=null;
    try{
      await hostKey(code,true);
      entry.held=await waitInputFrames(await inputClock(),2,allowTerminal);
    }catch(error){failure=error;entry.error=String(error);}
    // Always release through Host, but preserve the first failure if cleanup
    // also fails. Failed edge evidence is retained just like successful taps.
    try{await hostKey(code,false);}catch(error){entry.releaseError=String(error);failure??=error;}
    if(!failure)try{entry.released=await waitInputFrames(await inputClock(),2,allowTerminal);}
      catch(error){failure=error;entry.error=String(error);}
    entry.checkerCompletedMs=performance.now();
    keyEvidence.push(entry);assert.ok(keyEvidence.length<1000,'Undrained key evidence overflow');
    const summary=report.keyInputSummary;++summary.count;
    summary.heldMaxMs=Math.max(summary.heldMaxMs,entry.held?.wallMs??0);
    summary.releasedMaxMs=Math.max(summary.releasedMaxMs,entry.released?.wallMs??0);
    if(entry.released?.terminal)++summary.terminalReleases;
    report.lastKeyInput=entry;
    if(failure){++summary.errors;throw failure;}
    return entry;
  }
  async function waitExtraSlotsReady(){
    if(extra)await waitInputFrames(await inputClock(),12);
  }
  async function runBombStress(){
    const result=report.bombStress={completed:false,selectionIndices:[0,2,4],powerInjected:false,
      beginsAtStage:1,pulses:[],proofs:[],replays:[],lifecycle:[],coverageError:null,
      scope:'Three ordinary Bombs, actual power pickup, observed lifecycle and two confirmed Pause boundaries; not the full course'};
    const deadline=runStarted+bombSeconds*1000,view=new DataView(new ArrayBuffer(4));
    const number=bits=>{view.setUint32(0,bits,true);return view.getFloat32(0,true);};
    const actor=(state,seat)=>{const p=8+seat*16;return {active:state.game[p],ghost:state.game[p+1],life:state.game[p+2],
      lives:state.game[p+3],power:state.game[p+4],step:state.game[p+6],x:number(state.game[p+9]),y:number(state.game[p+10])};};
    const fail=message=>{result.coverageError={message,checkerMs:performance.now(),states:lastStates};
      throw Error('Bomb coverage incomplete: '+message);};
    const live=(state,seat)=>{const p=actor(state,seat);return p.active===1&&!p.ghost&&p.life===1;};
    const available=state=>state.bomb?.length===24&&state.bomb[0]===1&&state.bomb[1]===24&&state.bomb[2]===1;
    let nextConfirm=0,preparing=true;
    async function service(){
      lastStates=await smalls();checkStates(lastStates);await deliverMusic(lastStates);
      if(performance.now()>deadline)fail('Bounded ordinary Stage 1 budget exhausted');
      if(lastStates.some(x=>x.nativePhase!==1&&x.nativePhase!==2))fail('Ordinary life/scene left the requested Stage window');
      if(lastStates.some(x=>x.game[2]!==1))fail('Stage changed before complete Bomb evidence');
      if(preparing&&performance.now()>=nextConfirm&&lastStates.every(x=>x.nativePhase===1)){
        // This is the same real Host dialogue input as the full-course runner.
        await keyTap('Enter');nextConfirm=performance.now()+1800;
      }
      if((preparing||lastStates.every(x=>x.nativePhase===2))&&performance.now()-lastDrain>=segmentSeconds*1000)
        await drainSegment('ordinary-bomb');
      return lastStates;
    }
    async function steer(states,collector){
      await Promise.all(pages.map((page,seat)=>{
        const p=actor(states[seat],seat),target={x:(seat-1)*100,y:seat===collector?96:350};
        const value={x:Math.abs(target.x-p.x)>10?Math.sign(target.x-p.x):0,y:Math.abs(target.y-p.y)>10?Math.sign(target.y-p.y):0};
        return page.evaluate(value=>{window.__fullTouch.steer=value;},value);
      }));
    }
    let collector=0;
    while(true){
      const states=await service(),own=states.map((x,i)=>actor(x,i));
      if(own.every((p,i)=>live(states[i],i)&&p.step>0&&p.power>=p.step)){
        await steer(states,-1);
        if(own.every(p=>p.y>300)&&states.every(available))break;
      }else{
        if(collector<0||!live(states[collector],collector)||own[collector].power>=own[collector].step)
          collector=own.findIndex((p,i)=>live(states[i],i)&&p.step>0&&p.power<p.step);
        await steer(states,collector);
      }
      result.preparation={checkerMs:performance.now(),collector,actors:own};
      await pause(100);
    }
    await drainSegment('bomb-power-ready');await service();
    preparing=false;result.powerReady=lastStates;result.sfxBefore=lastStates.map(x=>[...x.sfx]);
    // Stop target drift while preserving continuous fire and the managed pointer.
    await Promise.all(pages.map(page=>page.evaluate(()=>{
      window.__fullTouch.steer={x:0,y:0};const s=document.querySelector('iframe').contentWindow.__rollbackAcceptance.state;
      s.bombLifecycle={samples:0,stageSamples:0,maxElapsed:[0,0,0],violations:[]};s.bombStrict=true;
    })));
    result.strictObserver='Full game56 and Bomb24 only during this bounded critical window, charged to rAF observerMs; samples do not prove every sub-tick';
    const seatClock=seat=>pages[seat].evaluate(()=>window.__fullTouch.bombClock());
    async function waitSeat(seat,start,frames){
      const stop=Math.min(deadline,performance.now()+6000);
      while(true){const now=await seatClock(seat);
        if(!now.configured||now.generation!==report.firstGeneration||now.error||now.phase!==1)fail('Bomb input lifecycle changed on seat '+seat);
        if(now.capture>=start.capture+frames&&now.next>=start.next+frames)return now;
        if(performance.now()>stop)fail('Bomb input capture/forward did not advance on seat '+seat);await pause(8);
      }
    }
    for(let seat=0;seat<players;++seat){
      await service();
      const pulse=await pages[seat].evaluate(()=>window.__fullTouch.pulseBomb());result.pulses.push(pulse);
      const ackDeadline=performance.now()+6000;
      while(!pulse.ackAt){
        Object.assign(pulse,await pages[seat].evaluate(()=>({...window.__fullTouch.bombPulses[0]})));
        if(performance.now()>ackDeadline)fail('Bomb managed input ACK timed out on seat '+seat);if(!pulse.ackAt)await pause(8);
      }
      assert.equal(pulse.ackOk,true,'Managed Bomb request failed');
      pulse.afterAck=await seatClock(seat);
      pulse.afterSpacing=await waitSeat(seat,pulse.afterAck,2);
    }
    // Do not cancel a touch or enter Pause until all three actual 3-capture
    // pulses have expired. Capture indices are each seat's own clock.
    for(let seat=0;seat<players;++seat)result.pulses[seat].afterRelease=await waitSeat(seat,result.pulses[seat].afterAck,4);
    async function pauseProof(label,active){
      const before=await smalls();checkStates(before);const afterFrame=Math.max(...before.map(x=>x.net[3]));
      await keyTap('Escape');const stop=Math.min(deadline,performance.now()+10000);
      while(true){
        await service();
        const tails=await Promise.all(pages.map(page=>page.evaluate(()=>JSON.stringify(
          document.querySelector('iframe').contentWindow.__rollbackAcceptance.state.tail.slice(-120)))).map(p=>p.then(JSON.parse)));
        const maps=tails.map(rows=>new Map(rows.filter(x=>x.game[4]>afterFrame&&x.game[3]===2&&x.bomb?.[2]&&x.bomb[8]===2&&
          x.net[12]===report.firstGeneration&&x.bomb[3]===report.firstGeneration&&
          x.bomb[4]===x.bomb[5]&&x.bomb[5]===x.bomb[6]&&x.bomb[7]!==0xffffffff&&x.bomb[7]+1>=x.bomb[4]&&
          !x.bomb[10]&&x.bomb[11]===0xffffffff&&!x.rollback[8]&&!x.rollback[9]&&!x.rollback[3])
          .map(x=>[x.net[12]+':'+x.game[4],x])));
        const common=[...maps[0].keys()].reverse().find(key=>maps.every(m=>m.has(key)));
        if(common){
          const samples=maps.map(m=>m.get(common));
          for(const sample of samples){
            assert.deepEqual(canonical(sample),canonical(samples[0]),'Confirmed Bomb game56/hash mismatch');
            assert.deepEqual(sample.bomb.slice(3,7),samples[0].bomb.slice(3,7),'Confirmed Bomb frame identity');
            assert.deepEqual(sample.bomb.slice(12),samples[0].bomb.slice(12),'Confirmed Bomb bodies mismatch');
            for(let seat=0;seat<players;++seat){
              if(!live(sample,seat))fail('Death or Ghost interrupted confirmed '+label+' seat '+seat);
              assert.equal(sample.bomb[12+seat*4],1);assert.equal(sample.bomb[13+seat*4],active);
              assert.equal(sample.bomb[15+seat*4],0,'Bomb native error');
            }
          }
          const proof={label,afterFrame,key:common,samples};result.proofs.push(proof);return proof;
        }
        if(performance.now()>stop)fail('No equal fully confirmed Pause snapshot for '+label);await pause(30);
      }
    }
    async function archives(label){
      const records=await Promise.all(pages.map(page=>page.evaluate(async()=>{
        const r=document.querySelector('iframe').contentWindow,size=r.core.th11_mp_replay_size(),ptr=r.core.th11_mp_replay_data();
        if(!size||!ptr)throw Error('No real confirmed Replay archive');
        const bytes=new Uint8Array(r.core.memory.buffer,ptr,size).slice(),{inspectMultiplayerReplay}=await import('/runtime/multiplayer-replay.mjs');
        const manifest=await(await fetch('/runtime/manifest.json')).json(),
          buildWords=Array.from({length:4},(_,i)=>parseInt(manifest.execution.sha256.slice(i*8,i*8+8),16)>>>0);
        const validate=value=>{const p=r.core.malloc(value.length);if(!p)return false;
          try{r.Module.HEAPU8.set(value,p);return r.core.th11_mp_replay_validate(p,value.length)===1;}finally{r.core.free(p);}};
        return JSON.stringify({metadata:inspectMultiplayerReplay(bytes,{validate,buildWords}),bytes:Array.from(bytes)});
      }).then(JSON.parse)));
      const payloads=[];
      for(let seat=0;seat<players;++seat){
        const record=records[seat],m=record.metadata,bytes=Uint8Array.from(record.bytes),v=new DataView(bytes.buffer);
        assert.equal(m.playerCount,3);assert.equal(m.difficulty,3);assert.equal(m.challenge,false);assert.equal(m.completed,false);
        assert.equal(m.cheat,false);assert.equal(m.timingMode,0);assert.deepEqual(m.chapters.map(c=>c.stage),[1]);assert.deepEqual(m.loadouts,loadouts);
        const base=40+v.getUint32(24,true)+v.getUint32(28,true)*8;payloads.push(bytes.subarray(base));
        const pulses=[];
        for(let player=0;player<players;++player){
          const request=result.pulses[player],begin=Math.max(0,request.before.capture-1),end=request.afterRelease.capture+1,frames=[];
          assert.ok(m.frameCount>end,'Confirmed archive covers pulse and release');
          for(let f=begin;f<=end;++f)if(v.getUint16(base+(f*players+player)*12,true)&2)frames.push(f);
          assert.equal(frames.length,3,'Exactly 3 captured Bomb button frames on seat '+player);
          assert.deepEqual(frames,[frames[0],frames[0]+1,frames[0]+2],'Bomb input pulse must be contiguous');
          assert.ok(frames[0]>begin&&frames[2]<end,'Bomb pulse has actual release frames on both sides');
          pulses.push({player,begin,end,frames});
        }
        const file=label+'-peer-'+seat+'.rpy';writeFileSync(resolve(out,file),bytes);
        result.replays.push({label,seat,file,sha256:hash(bytes),bytes:bytes.length,metadata:m,pulses});
      }
      const prefix=Math.min(...payloads.map(x=>x.length));
      for(const bytes of payloads.slice(1))assert.deepEqual(bytes.subarray(0,prefix),payloads[0].subarray(0,prefix),'Confirmed Bomb input histories differ');
      result.confirmedPrefixFrames=prefix/(players*12);
    }
    const overlapDeadline=Math.min(deadline,performance.now()+6000);
    while(true){await service();
      if(lastStates.every(x=>available(x)&&[0,1,2].every(s=>x.bomb[13+s*4]===1)))break;
      if(performance.now()>overlapDeadline)fail('Three actual Bomb active windows did not overlap');await pause(20);
    }
    await pauseProof('overlap',1);await archives('bomb-overlap');
    lastStates=await smalls();checkStates(lastStates);
    result.sfxAtOverlap=lastStates.map(x=>[...x.sfx]);
    for(let seat=0;seat<players;++seat)for(const id of [38,49])
      assert.ok(((result.sfxAtOverlap[seat][8+id]-result.sfxBefore[seat][8+id])>>>0)>0,'Expected native aggregated SFX start ID '+id);
    const resume=result.resume={checkerStartedMs:performance.now(),key:null,transitions:[]};
    // MP Escape is accepted in state 32 or 33, then state 34 spends more
    // than 11 exact paused ticks closing. A 2-held/2-release tap is only
    // input evidence; it does not prove the asynchronous menu has closed.
    // Send one edge only: retrying Escape after Resume could pause again.
    const resumeState=states=>states.map((x,seat)=>({seat,phase:x.nativePhase,pauseState:x.scene[10],
      generation:x.net[12],next:x.net[3],capture:x.rollback[6],confirmed:x.net[4],
      bombValid:available(x),world:x.bomb?.[4]}));
    resume.before=resumeState(lastStates);
    resume.key=await keyTap('Escape');
    const resumeDeadline=Math.min(deadline,performance.now()+10000);let resumeSignature='';
    while(true){
      const resumed=await service(),states=resumeState(resumed);
      resume.last={checkerMs:performance.now(),states};
      const signature=JSON.stringify(states.map(x=>[x.phase,x.pauseState]));
      if(signature!==resumeSignature){resume.transitions.push(resume.last);resumeSignature=signature;
        assert.ok(resume.transitions.length<=64,'Unexpected repeated Resume state transitions');}
      if(resumed.every(x=>x.nativePhase===1&&available(x))){
        resume.completedMs=performance.now();resume.waitAfterTapMs=resume.completedMs-resume.key.checkerCompletedMs;
        break;
      }
      if(resumed.some(x=>x.nativePhase===2&&![32,33,34].includes(x.scene[10])))
        fail('Real Host Resume entered an unexpected Pause menu state');
      if(performance.now()>resumeDeadline)fail('Real Host Resume did not return to Stage within 10 seconds after one Escape');
      await pause(30);
    }
    const from=Math.max(...lastStates.map(x=>x.net[3]));result.resumeFrame=from;
    while(true){
      await service();
      const watches=await Promise.all(pages.map(page=>page.evaluate(()=>({...document.querySelector('iframe').contentWindow.__rollbackAcceptance.state.bombLifecycle}))));
      result.lifecycle=watches;
      if(watches.some(x=>x.violations.length))fail('Observed death/Ghost/native error interrupted natural Bomb completion');
      if(lastStates.every(x=>available(x)&&x.net[3]>=from+200&&[0,1,2].every(s=>!x.bomb[13+s*4])))break;
      await pause(50);
    }
    const ended=await pauseProof('natural-end',0);await archives('bomb-natural-end');
    result.simulatedFramesAfterResume=ended.samples.map(x=>x.game[4]-from);
    assert.ok(result.lifecycle.every(x=>x.stageSamples>0),'Critical Stage observer had real samples');
    assert.ok(ended.samples.every(x=>x.bomb[13]===0&&x.bomb[14]>=181&&x.bomb[15]===0),
      'Confirmed inactive selection 0 completed tick(current=180); predicted maxElapsed is diagnostic only');
    await Promise.all(pages.map(page=>page.evaluate(()=>{document.querySelector('iframe').contentWindow.__rollbackAcceptance.state.bombStrict=false;})));
    result.completed=true;result.completedAt=new Date().toISOString();mark('ordinary-bomb-coverage-complete');
  }
  if(bombStress){await runBombStress();}else{
  let nextStoryEdge=runStarted+1800;
  while(true){
    lastStates=await smalls();checkStates(lastStates);await deliverMusic(lastStates);
    if(lastStates.every(s=>s.nativePhase===5)){if(!observedEnding){observedEnding=true;mark('natural-ending-entered',{states:lastStates});
      for(let seat=0;seat<players;++seat)await pages[seat].screenshot({path:resolve(out,'ending-peer-'+seat+'.png')});}}
    if(observedEnding&&lastStates.every(s=>s.nativePhase===5)&&performance.now()-lastEndingCapture>20000){
      const name='ending-'+String(Math.floor((performance.now()-runStarted)/1000)).padStart(5,'0')+'.png';
      await pages[0].screenshot({path:resolve(out,name)});report.staffEvidence.periodicScreenshots.push({file:name,states:lastStates});lastEndingCapture=performance.now();
    }
    if(lastStates.every(s=>s.nativePhase===3)){
      observedResults=true;report.staffEvidence.naturalEndingToResults=observedEnding;
      report.staffEvidence.peers=lastStates.map((s,seat)=>({seat,...s.staffEvidence}));
      report.staffEvidence.passed=!extra&&report.staffEvidence.peers.every(s=>s.progressTransitions>=2);
      if(extra)assert.ok(lastStates.every(s=>s.scene[11]===1),'Extra native Results must be completed, not a wipe');
      else assert.ok(observedEnding&&report.staffEvidence.passed,'Every peer must observe real Staff with two advances before Results');
      break;
    }
    if(lastStates.some(s=>s.nativePhase===4))throw Error('Session finished before natural Ending/result evidence');
    const elapsed=performance.now()-runStarted;
    if(smokeSeconds&&elapsed>=smokeSeconds*1000){report.smokeSetupPassed=true;break;}
    if(elapsed>duration*1000)throw Error('Full-run wall-clock budget exhausted without complete results');
    if(performance.now()-lastStageChange>15*60*1000&&report.progressWarning!==lastStates[0].game[2]){
      report.progressWarning=lastStates[0].game[2];mark('slow-stage-progress-warning',{states:lastStates});}
    // Real Enter edges advance authored dialogue. Holding Shoot alone is not a
    // reliable pressed edge; no ECL/MSG program counter or timing is modified.
    if(performance.now()>=nextStoryEdge&&lastStates.every(s=>s.nativePhase===1||s.nativePhase===5)){
      await keyTap('Enter');nextStoryEdge=performance.now()+(observedEnding?900:1800);
    }
    if(performance.now()-lastDrain>=segmentSeconds*1000)await drainSegment();
    if(performance.now()-lastProgress>=2000){writeFileSync(resolve(out,'progress.json'),JSON.stringify({schema:'th11-fullgame-progress/1',
      at:new Date().toISOString(),elapsedMs:elapsed,segmentIndex,status:'running',observedEnding,states:lastStates,
      stageVisits:report.stageVisits,errorCount:report.errors.length,performanceFailureCount:report.failures.length})+'\n');lastProgress=performance.now();}
    await pause(200);
  }
  if(!smokeSeconds){
    assert.ok(observedResults&&(extra||observedEnding&&report.staffEvidence.passed),'Must complete the selected original course lifecycle');
    for(const visits of report.stageVisits)assert.deepEqual(visits.map(x=>x.stage),expectedStages);
    mark('natural-results-entered',{states:lastStates});
    for(let seat=0;seat<players;++seat)await pages[seat].screenshot({path:resolve(out,'results-peer-'+seat+'.png')});
    // Menus are confirmed P0 transactions. Keep all peers producing input while
    // matching the same observed post-tick state, without modifying confirmed.
    const settleDeadline=Date.now()+10000;
    while(!common){
      const tails=await Promise.all(pages.map(p=>p.evaluate(()=>document.querySelector('iframe').contentWindow.__rollbackAcceptance.state.tail)));
      const maps=tails.slice(1).map(t=>new Map(t.filter(s=>s.game[3]===3).map(s=>[s.net[12]+':'+s.game[4],s])));
      for(const first of tails[0].filter(s=>s.game[3]===3)){const others=maps.map(m=>m.get(first.net[12]+':'+first.game[4]));if(others.every(Boolean)){
        for(const other of others)assert.deepEqual(canonical(first),canonical(other),'Three peers differ at same fully confirmed results frame');
        common={frame:first.game[4],generation:first.net[12],samples:[first,...others]};}}
      if(common)break;if(Date.now()>settleDeadline)throw Error('No common confirmed three-peer results frame');await pause(50);
    }
    report.confirmedBoundary=common;
    // Complete the original name and replay menus with normal P0 keys.
    // A fresh profile can require eight letters plus End; an existing name can
    // already select End. State inspection controls the number of real taps.
    const inReplaySlots=s=>extra?s.scene[10]===23:s.ui[5]===15&&s.ui[6]===2;
    const inReplayName=s=>extra?s.scene[10]===24:s.ui[5]===15&&s.ui[6]===3;
    let menuDeadline=Date.now()+30000;
    while(true){lastStates=await smalls();checkStates(lastStates);
      if(extra?lastStates[0].scene[10]===22:inReplaySlots(lastStates[0]))break;
      if(Date.now()>menuDeadline)throw Error('Original Results name menu did not reach Replay choices');
      await keyTap('KeyZ');if(performance.now()-lastDrain>=segmentSeconds*1000)await drainSegment('result-name');
    }
    if(extra){
      // The original compact menu starts at Return(0), Save(1), disabled Retry(2).
      // Use real P0 Down/Confirm. The separate PauseMenu owner is not TitleMenu15.
      assert.equal(lastStates[0].scene[11],1,'Extra compact menu must belong to a cleared run');
      await keyTap('ArrowDown');await keyTap('KeyZ');menuDeadline=Date.now()+15000;
      while(true){lastStates=await smalls();checkStates(lastStates);if(inReplaySlots(lastStates[0]))break;
        if(Date.now()>menuDeadline||lastStates.some(s=>s.nativePhase===4))throw Error('Extra Replay Save selection did not reach native slots23');
        await pause(100);}
      await waitExtraSlotsReady();
      report.extraResultFlow={compactChoicesObserved:true,slotsObserved:true,completed:true};
    }
    const before=(await pages[0].evaluate(()=>host.request('list'))).files.map(x=>x.path);
    // Menu index i saves file i+1. Native API slot0 means automatic free-path
    // selection, not menu index0; production protects named files from autosave.
    const emptySlot=Array.from({length:25},(_,i)=>i).find(i=>!before.includes('replay/th11_'+String(i+1).padStart(2,'0')+'.rpy'));
    assert.notEqual(emptySlot,undefined,'Fresh isolated profile must have an empty original Replay slot');
    for(let i=0;i<emptySlot;++i)await keyTap('ArrowDown');
    await keyTap('KeyZ');menuDeadline=Date.now()+30000;
    let sawReplayName=false;
    while(true){lastStates=await smalls();checkStates(lastStates);
      if(inReplayName(lastStates[0]))sawReplayName=true;
      if(sawReplayName&&inReplaySlots(lastStates[0]))break;
      if(Date.now()>menuDeadline)throw Error('Original Replay name save did not finish');
      await keyTap('KeyZ');if(performance.now()-lastDrain>=segmentSeconds*1000)await drainSegment('replay-name');
    }
    const listing=await pages[0].evaluate(()=>host.request('list'));
    report.nativeReplayFiles=listing.files;
    const savedPath='replay/th11_'+String(emptySlot+1).padStart(2,'0')+'.rpy';
    assert.ok(!before.includes(savedPath)&&listing.files.some(f=>f.path===savedPath),'Native Replay Save created selected real file');
    // Validate both the file produced by the original Save menu and the live
    // confirmed archive. A successful export cannot stand in for a corrupt file.
    const inspectReplay=(page,path=null)=>page.evaluate(async path=>{
      const r=document.querySelector('iframe').contentWindow;
      const bytes=Uint8Array.from(path?(await host.request('read',{path})).bytes:host.exportReplay());
      const {inspectMultiplayerReplay}=await import('/runtime/multiplayer-replay.mjs');
      const manifest=await(await fetch('/runtime/manifest.json')).json(),buildWords=Array.from({length:4},(_,i)=>parseInt(manifest.execution.sha256.slice(i*8,i*8+8),16)>>>0);
      const validate=value=>{const p=r.core.malloc(value.length);if(!p)return false;try{r.Module.HEAPU8.set(value,p);return r.core.th11_mp_replay_validate(p,value.length)===1;}finally{r.core.free(p);}};
      const metadata=inspectMultiplayerReplay(bytes,{validate,buildWords});return {metadata,bytes:Array.from(bytes)};
    },path);
    const checkReplay=result=>{
      assert.equal(result.metadata.playerCount,3);assert.equal(result.metadata.difficulty,difficulty);assert.equal(result.metadata.challenge,true);
      assert.equal(result.metadata.completed,true);assert.equal(result.metadata.cheat,false);assert.equal(result.metadata.timingMode,0);
      assert.deepEqual(result.metadata.chapters.map(x=>x.stage),expectedStages);
    };
    const saved=await inspectReplay(pages[0],savedPath);checkReplay(saved);
    const savedBytes=Uint8Array.from(saved.bytes),savedFile=resolve(out,'native-saved-complete.rpy');writeFileSync(savedFile,savedBytes);
    report.savedReplay={path:savedPath,file:savedFile,sha256:hash(savedBytes),bytes:savedBytes.length,metadata:saved.metadata,sawReplayName,
      readBeforeSync:true};
    await pages[0].evaluate(()=>host.request('sync'));
    const persisted=await pages[0].evaluate(path=>host.request('read',{path}),savedPath);
    assert.deepEqual(Uint8Array.from(persisted.bytes),savedBytes,'Host sync must not replace the original menu-saved Replay');
    report.savedReplay.syncPreservedNativeBytes=true;
    report.replays=[];
    for(let seat=0;seat<players;++seat){
      const result=await inspectReplay(pages[seat]);checkReplay(result);
      const bytes=Uint8Array.from(result.bytes),file=resolve(out,'complete-peer-'+seat+'.rpy');writeFileSync(file,bytes);
      report.replays.push({seat,file,sha256:hash(bytes),bytes:bytes.length,metadata:result.metadata});
    }
    // Compare every seat's actual confirmed input payload through the common
    // prefix. Final archive lengths may differ by a few confirmed menu frames.
    const payloads=report.replays.map(r=>{const b=readFileSync(r.file),v=new DataView(b.buffer,b.byteOffset,b.byteLength);return b.subarray(40+v.getUint32(24,true)+v.getUint32(28,true)*8);});
    const prefix=Math.min(...payloads.map(b=>b.length));for(const b of payloads.slice(1))assert.deepEqual(b.subarray(0,prefix),payloads[0].subarray(0,prefix),'Confirmed input histories differ');
    report.inputHistoryEqualThroughFrames=prefix/(3*12);
    await waitExtraSlotsReady();
    await keyTap('KeyX',{allowTerminal:!extra});
    if(extra){
      const backDeadline=Date.now()+15000;
      while(true){lastStates=await smalls();checkStates(lastStates);if(lastStates[0].scene[10]===22)break;
        if(Date.now()>backDeadline)throw Error('Extra Replay slots Back did not reach compact choices22');await pause(100);}
      // X first selects Return without activating it; a separate Z confirms.
      // This is the authored two-step compact menu behavior, not runtime retire.
      await keyTap('KeyX');await keyTap('KeyZ',{allowTerminal:true});report.extraResultFlow.returnRequested=true;
    }
    const exitDeadline=Date.now()+15000;
    while(true){lastStates=await smalls();checkStates(lastStates,{allowExited:true});
      if(lastStates.every(s=>s.nativePhase===4&&terminalProof(s)&&successfulHostExit(s)))break;
      if(Date.now()>exitDeadline)throw Error('Native Replay Save Back lacks confirmed terminal + successful Host exit for all three participants: '+JSON.stringify(lastStates));await pause(100);}
    report.terminalEvidence=lastStates.map((s,seat)=>({seat,native:s.terminalEvidence,hostExits:s.hostExits}));
    report.fullRunComplete=true;report.exitStates=lastStates;mark(extra?'extra-normal-exit':'full-game-normal-exit');
  }
  }
  await Promise.all(pages.map(p=>p.evaluate(()=>window.__fullTouch.finish())));
  await pause(250);await drainSegment(smokeSeconds?'smoke-end':'complete');
  report.final=await smalls();
  for(let seat=0;seat<players;++seat){const x=report.final[seat];
    assert.ok(!x.error&&!x.shellError&&!x.probeErrors.length&&!x.rollback[23]&&!x.hostErrors?.length,JSON.stringify({seat,...x}));
    checkSfx(x,seat);
  }
  report.performance=perfSummary();
  report.musicSeenPlaying=audioSeen.map(s=>[...s]);report.musicLoaded=loadedMusic.map(s=>[...s]);
  for(let seat=0;seat<players;++seat){
    const evidence=report.segments.filter(s=>s.seat===seat);
    if(!evidence.length)report.failures.push({kind:'missing-segments',seat});
    const requiredCues=report.stageVisits[seat].map(v=>(v.stage-1)*2+1);
    for(const cue of requiredCues)if(!audioSeen[seat].has(cue))report.failures.push({kind:'stage-bgm-not-observed-playing',seat,cue});
    if(!audioWatch[seat].progressEvents)report.failures.push({kind:'pcm-no-progress-evidence',seat,watch:audioWatch[seat]});
    if(!(coverage[seat].restores>0&&coverage[seat].historyTicks>0))report.failures.push({kind:'rollback-path-not-exercised',seat,coverage:coverage[seat]});
    if(!coverage[seat].movedPlayers.every(Boolean))report.failures.push({kind:'actual-movement-not-observed',seat,coverage:coverage[seat]});
    const touch=await pages[seat].evaluate(()=>{const t=window.__fullTouch;return {submitted:t.submitted,acks:t.acks,errors:t.errors,lateSlots:t.lateSlots};});
    if(touch.acks!==touch.submitted||touch.errors.length)report.failures.push({kind:'touch-integrity',seat,touch});
  }
  for(const [path,digest] of pinned)assert.equal(hash(readFileSync(path)),digest,'Fixture changed during run: '+path);
  requireNoBlockingErrors();
  report.sourceStable=true;report.audioStatsErrorLatchClear=true;
  report.sceneStatusAvailable=report.final.every(s=>s.scene.length===12&&s.scene[0]===1);report.audioLimit='PCM progress, clear mix/SDL latch and sampled same-life SFX start return codes do not prove each event played, audible output, finite PCM or live heap stability';
  report.sfxStartFailuresClear=sfxHealth.every(w=>w.observations>0&&w.last.value[3]===0&&w.last.value[6]===0);
  report.audioHealth=audioWatch;report.rollbackMovementCoverage=coverage;
  if(gpuDiagnostics){report.gpuDiagnosticCollectionComplete=report.final.every(s=>
    s.gpuDiagnostics?.contexts.length>0&&!s.gpuDiagnostics.errors.length);
    if(!report.gpuDiagnosticCollectionComplete)report.failures.push({kind:'gpu-diagnostic-coverage'});}
  report.performancePassed=!gpuDiagnostics&&!report.failures.some(f=>f.kind==='performance'||f.kind==='audio-context-not-running')&&
    report.performance.every(a=>a.native.max<=maxGap&&a.presentIntervals.max<=maxGap&&a.frontierIntervals.max<=maxGap);
  if(relayLogOverflow)throw Error('Relay log/heartbeat coverage overflow');
  const candidatePassed=!gpuDiagnostics&&!smokeSeconds&&(bombStress?report.bombStress?.completed:report.fullRunComplete)&&report.failures.length===0,
    smokeSetupPassed=!!smokeSeconds&&!report.failures.some(f=>f.kind!=='performance');
  report.validationOutcome={passed:candidatePassed,smokeSetupPassed,
    status:smokeSeconds?(smokeSetupPassed?'smoke-complete':'smoke-failed'):candidatePassed?'passed':'failed',
    exitCode:smokeSeconds?(smokeSetupPassed?0:1):candidatePassed?0:1};
  report.validationWindow.closedMs=performance.now();report.validationWindow.phase='validation-complete';
  report.status='validation-complete';report.passed=false;
}catch(error){
  firstFailure??=error;report.passed=false;
  report.failureContext??={at:new Date().toISOString(),lastProgress:report.progress||null,lastHealthySegments:report.segments.slice(-6),stageVisits:report.stageVisits};
  report.status=firstFailure.blocked&&!report.watchdog?'blocked':'failed';report.error??=firstFailure.stack||String(firstFailure);
  process.exitCode=firstFailure.blocked&&!report.watchdog?2:1;
  // Freeze only our own input producer and native loop before large snapshots.
  // Do not clear transport timers/buffers, synthesize releases or replace the
  // first failure with any secondary freeze/serialization/cleanup exception.
  try{save();}catch(saveError){report.cleanup.push({action:'save-first-failure',error:String(saveError)});}
  await Promise.all(pages.map(async(page,seat)=>{
    const start=performance.now();
    try{const state=await bounded(page.evaluate(()=>{
      const touch=window.__fullTouch,r=document.querySelector('iframe')?.contentWindow;
      if(touch){touch.active=false;touch.frozenAt=performance.now();}
      r?.core?.th11_loop_stop?.();
      return {at:performance.now(),timeOrigin:performance.timeOrigin,touchActive:touch?.active??null,
        submitted:touch?.submitted??null,acks:touch?.acks??null,pendingSegment:!!window.__fullPendingSegment};
    }),freezeTimeoutMs,'freeze failure peer '+seat);
      report.cleanup.push({seat,action:'freeze-failure-input-and-runtime',checkerMs:performance.now()-start,state});
    }catch(freezeError){report.cleanup.push({seat,action:'freeze-failure-input-and-runtime',error:String(freezeError),checkerMs:performance.now()-start});}
  }));
  const snapshots=await captureSnapshots('failure');
  for(const snapshot of snapshots){
    const {seat,timing}=snapshot;
    try{
      if(snapshot.error)throw snapshot.error;
      const {json}=decodeSnapshot(snapshot),file='peer-'+seat+'-failure.json';
      const sha256=writeSnapshotFile(resolve(out,file),json+'\n',timing);
      report.failureSnapshots.push({seat,file,sha256,snapshotTiming:timing});
    }catch(preserveError){
      report.cleanup.push({seat,action:'preserve-failure',error:String(preserveError),snapshotTiming:timing});
      if(snapshot.wire)try{const file='peer-'+seat+'-failure.snapshot-wire.txt',sha256=writeSnapshotFile(resolve(out,file),snapshot.wire,timing);
        report.failureSnapshots.push({seat,file,sha256,decodeFailed:true,snapshotTiming:timing});}
      catch(rawError){report.cleanup.push({seat,action:'preserve-failure-wire',error:String(rawError)});}
    }
  }
}finally{
  report.validationWindow.closedMs??=performance.now();report.validationWindow.phase='cleanup';
  report.validationWindow.cleanupStartedMs=performance.now();
  mark('cleanup-start');
  for(let seat=0;seat<pages.length;++seat){
    try{await bounded(pages[seat].evaluate(()=>{const r=document.querySelector('iframe')?.contentWindow;r?.core?.th11_loop_stop();r?.__rollbackAcceptance?.close();}),freezeTimeoutMs,'stop owned runtime peer '+seat);}
    catch(e){report.cleanup.push({seat,action:'stop-owned-runtime',error:String(e)});}
  }
  // Validation and final evidence are already closed. Close each owned
  // transport explicitly before browser shutdown can broadcast a cascade.
  await Promise.all(pages.map(async(page,seat)=>{
    let action;
    try{
      const requestedMs=performance.now();
      const operation=page.evaluate(()=>{
        const t=document.querySelector('iframe')?.contentWindow?.__eaglerPeerTransport;
        const snapshot=()=>t?{present:true,route:t.route,localPlayer:t.localPlayer,closed:!!t.closed,
          disconnected:!!t.disconnected,error:t.error||null}: {present:false};
        const before=snapshot(),at=performance.now();let performed=false;
        if(t&&typeof t.close==='function'&&!t.closed){t.close();performed=true;}
        return {timeOrigin:performance.timeOrigin,at,completedAt:performance.now(),performed,before,after:snapshot()};
      });
      // Record only after the real evaluate request was issued, not an intent.
      action={id:report.ownedCleanupActions.length+1,seat,action:'close-owned-transport',requestedMs};
      report.ownedCleanupActions.push(action);
      action.result=await bounded(operation,freezeTimeoutMs,'close owned transport peer '+seat);
      action.completedMs=performance.now();action.ok=true;report.cleanup.push({...action});
    }catch(e){
      if(action){action.completedMs=performance.now();action.error=String(e);report.cleanup.push({...action});}
      recordError({seat,kind:'owned-transport-close-failed',error:String(e),blocking:true});
    }
  }));
  for(let seat=0;seat<browsers.length;++seat){
    const action={id:report.ownedCleanupActions.length+1,seat,action:'close-owned-browser',requestedMs:performance.now()};
    report.ownedCleanupActions.push(action);
    try{await browsers[seat].close();action.completedMs=performance.now();action.ok=true;report.cleanup.push({...action});}
    catch(e){action.completedMs=performance.now();action.error=String(e);report.cleanup.push({...action});}
  }
  if(server){server.closeAllConnections?.();await Promise.race([new Promise(r=>server.close(r)),pause(3000)]);}
  if(relay){relay.kill();await Promise.race([new Promise(r=>relay.once('exit',r)),pause(3000)]);report.cleanup.push({action:'stop-owned-relay',pid:relay.pid,exitCode:relay.exitCode,signal:relay.signalCode});}
  writeFileSync(resolve(out,'relay.log'),relayLog);writeFileSync(resolve(out,'relay-heartbeats.json'),JSON.stringify(relayHeartbeats)+'\n');
  report.relayHeartbeat={count:relayHeartbeats.length,lateness:distribution(relayHeartbeats.map(x=>x.late)),clock:'relay child performance.now; not comparable to peer timeOrigins',logOverflow:relayLogOverflow};
  clearTimeout(watchdog);
  const lateErrors=blockingErrors();
  if(!firstFailure&&lateErrors.length){firstFailure=Error('Blocking browser/server errors remained after validation: '+JSON.stringify(lateErrors.slice(-5)));report.error=firstFailure.stack;}
  if(firstFailure||report.watchdog){report.status=firstFailure?.blocked&&!report.watchdog?'blocked':'failed';report.passed=false;report.smokeSetupPassed=false;process.exitCode=firstFailure?.blocked&&!report.watchdog?2:1;}
  else if(report.validationOutcome){report.status=report.validationOutcome.status;report.passed=report.validationOutcome.passed;
    report.smokeSetupPassed=report.validationOutcome.smokeSetupPassed;process.exitCode=report.validationOutcome.exitCode;}
  report.validationWindow.phase='closed';
  report.completedAt=new Date().toISOString();save();
  console.log('TH11 rollback full game '+report.status+': '+resolve(out,'receipt.json'));
  if(firstFailure)console.error(String(firstFailure));
}
