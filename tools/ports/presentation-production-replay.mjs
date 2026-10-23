import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstat,readFile,stat} from 'node:fs/promises';
import {extname,isAbsolute,resolve} from 'node:path';
import {isDeepStrictEqual} from 'node:util';
import {assessReplayReadEvidence} from '../../common/testkit/presentation-lab/replay-pair.mjs';

const RUN_SCHEMA='eagler-presentation/production-replay-capture/1';
const READ_SCHEMA='eagler-presentation/production-replay-reads/1';
const REPLAY_TRACE_SCHEMA=2,REPLAY_TRACE_CAPACITY=256,REPLAY_TRACE_WORDS=36;
const MAX_REPLAY_BYTES=16*1024*1024,MAX_READ_BYTES=128*1024*1024;
const STATUS_FIELDS=Object.freeze([...Array.from({length:22},(_,index)=>index),25,26,27,28]);
const SHA=/^[a-f0-9]{64}$/u;
const isRecord=value=>value!==null&&typeof value==='object'&&!Array.isArray(value);
const same=(left,right)=>isDeepStrictEqual(left,right);
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const rawBytes=value=>Buffer.byteLength(JSON.stringify(value));
const FRAME_CAPTURE_CONTRACT=Object.freeze({runSchema:RUN_SCHEMA,readSchema:READ_SCHEMA,
  replayTraceSchema:REPLAY_TRACE_SCHEMA,replayTraceCapacity:REPLAY_TRACE_CAPACITY,
  replayTraceWords:REPLAY_TRACE_WORDS,maxReplayBytes:MAX_REPLAY_BYTES,maxReadBytes:MAX_READ_BYTES,
  statusFields:STATUS_FIELDS});

function identityBytesMatch(actual,expected,label){
  assert.ok(isRecord(actual),`${label} identity is missing`);
  assert.ok(isRecord(expected),`Expected ${label} identity is required`);
  assert.equal(actual.bytes,expected.bytes,`${label} byte count differs`);
  assert.match(actual.sha256??'',SHA,`${label} SHA-256 is invalid`);
  assert.equal(actual.sha256,expected.sha256,`${label} SHA-256 differs`);
  if(expected.path!==undefined&&actual.path!==undefined)
    assert.equal(resolve(actual.path),resolve(expected.path),`${label} path differs`);
}

/** Read and pin an operator-selected Replay without following links or accepting a changing file. */
export async function readProductionReplayInput(path){
  assert.equal(typeof path,'string','--presentation-replay must be a path');
  assert.ok(isAbsolute(path),'--presentation-replay must be an absolute path');
  const absolute=resolve(path);
  assert.equal(extname(absolute).toLowerCase(),'.rpy','--presentation-replay must name a .rpy file');
  const before=await lstat(absolute);
  assert.ok(before.isFile()&&!before.isSymbolicLink(),'Replay input must be a regular non-symlink file');
  assert.ok(before.size>0&&before.size<=MAX_REPLAY_BYTES,'Replay input must be 1..16 MiB');
  const bytes=await readFile(absolute),after=await lstat(absolute),info=await stat(absolute);
  assert.ok(after.isFile()&&!after.isSymbolicLink(),'Replay input changed to a link while being read');
  assert.equal(bytes.length,before.size,'Replay input changed size while being read');
  assert.equal(after.dev,before.dev,'Replay input changed device while being read');
  assert.equal(after.ino,before.ino,'Replay input was replaced while being read');
  assert.equal(after.mtimeMs,before.mtimeMs,'Replay input changed while being read');
  assert.equal(after.ctimeMs,before.ctimeMs,'Replay input changed while being read');
  assert.equal(info.size,bytes.length,'Replay input stat differs from bytes read');
  return {path:absolute,bytes:bytes.length,sha256:hash(bytes),contents:bytes};
}

/** Run the normal TH08 Shell Replay path inside an already launched production frame. */
export async function captureProductionReplay({frame,target,packageIdentity,dataIdentity,replayIdentity,
  expectedOrigin,outerUrl,epoch=1,ticks=600,replayPath='replay/th8_01.rpy',firstFrame=null}={}){
  assert.ok(frame&&typeof frame.evaluate==='function','A selected normal Runtime frame is required');
  assert.ok(target?.game==='th08'&&target?.variant==='normal'&&typeof target.id==='string',
    'Production Replay capture is registered for th08 normal');
  assert.ok(isRecord(packageIdentity)&&packageIdentity.game==='th08'&&packageIdentity.variant==='normal',
    'Verified normal Runtime package identity is required');
  assert.ok(packageIdentity.sourceBuild?.profile==='production','Runtime package is not bound to its normal source build');
  assert.ok(isRecord(dataIdentity)&&Number.isSafeInteger(dataIdentity.bytes)&&dataIdentity.bytes>0&&SHA.test(dataIdentity.sha256??''),
    'Verified private DATA identity is required');
  assert.ok(isRecord(replayIdentity)&&isAbsolute(replayIdentity.path)&&Number.isSafeInteger(replayIdentity.bytes)&&
    replayIdentity.bytes>0&&SHA.test(replayIdentity.sha256??'')&&Buffer.isBuffer(replayIdentity.contents),
  'Read-only Replay input identity is required');
  assert.ok(Number.isSafeInteger(ticks)&&ticks>=1&&ticks<=3600,'Production Replay ticks must be 1..3600');
  assert.ok(Number.isSafeInteger(epoch)&&epoch>0,'A positive Runtime navigation epoch is required');
  const hostUrl=new URL(outerUrl),origin=new URL(expectedOrigin).origin;
  assert.equal(hostUrl.origin,origin,'Production host page differs from the verified fixture origin');
  const current=await readProductionReplayInput(replayIdentity.path);
  assert.equal(current.bytes,replayIdentity.bytes,'Replay file changed before production capture');
  assert.equal(current.sha256,replayIdentity.sha256,'Replay file hash changed before production capture');
  const run=await frame.evaluate(captureProductionReplayInFrame,{
    game:target.game,targetId:target.id,epoch,replayPath,replayBytes:replayIdentity.bytes,
    replaySha256:replayIdentity.sha256,ticks,expectedOrigin:origin,captureContract:FRAME_CAPTURE_CONTRACT,
  });
  const after=await readProductionReplayInput(replayIdentity.path);
  assert.equal(after.bytes,replayIdentity.bytes,'Replay input changed during production capture');
  assert.equal(after.sha256,replayIdentity.sha256,'Replay input changed during production capture');
  run.identity={build:{game:packageIdentity.game,variant:packageIdentity.variant,packageSha256:packageIdentity.packageSha256,
      wasmSha256:packageIdentity.wasmSha256,manifestSha256:packageIdentity.manifestSha256??null,
      profile:packageIdentity.sourceBuild.profile,diagnostic:false,sourceBuild:packageIdentity.sourceBuild,
      packageBuild:packageIdentity.packageBuild},data:{path:dataIdentity.path,bytes:dataIdentity.bytes,sha256:dataIdentity.sha256}};
  run.inputIdentity={path:replayIdentity.path,bytes:replayIdentity.bytes,sha256:replayIdentity.sha256};
  run.firstFrame=firstFrame;
  run.hostPage={url:hostUrl.href,origin:hostUrl.origin};
  return run;
}

/** Browser-side capture. It deliberately uses only normal Runtime exports and the real Shell file API. */
export async function captureProductionReplayInFrame({game,targetId,epoch,replayPath,replayBytes,replaySha256,ticks,expectedOrigin,captureContract}){
  const {runSchema,readSchema,replayTraceSchema,replayTraceCapacity,replayTraceWords,maxReplayBytes,maxReadBytes,statusFields}=
    captureContract??{};
  if(!runSchema||!readSchema||!Number.isSafeInteger(replayTraceSchema)||!Number.isSafeInteger(replayTraceCapacity)||
     !Number.isSafeInteger(replayTraceWords)||!Number.isSafeInteger(maxReplayBytes)||!Number.isSafeInteger(maxReadBytes)||
     !Array.isArray(statusFields))throw new Error('Production Replay browser capture contract is missing');
  const fail=message=>{throw new Error(message);};
  const runtime=window[`__${game}Runtime`],core=runtime?.core,app=runtime?.app;
  const required=['status','trace','sdl_replay_trace_control_v1','sdl_replay_trace_status_v1','sdl_loop_tick',
    'sdl_loop_stop','sdl_loop_pause','sdl_key','allocate','deallocate'];
  if(!runtime||!core||!Number.isInteger(app)||app<=0)fail('Normal Runtime Shell is unavailable');
  for(const name of required)if(typeof core[name]!=='function')fail(`Normal Runtime export ${name} is unavailable`);
  if(!core.memory?.buffer)fail('Normal Runtime WASM memory is unavailable');
  const actualUrl=new URL(location.href),actualEpoch=Number(actualUrl.searchParams.get('runtimeEpoch'));
  if(actualUrl.origin!==expectedOrigin||location.origin!==expectedOrigin)fail('Runtime navigation origin differs from the verified fixture');
  if(!actualUrl.pathname.startsWith(`/runtime/${targetId}/`))fail('Runtime frame belongs to another selected package');
  if(actualEpoch!==epoch)fail('Runtime frame epoch differs from the selected navigation');
  if(game!=='th08'||typeof replayPath!=='string'||!replayPath.startsWith('replay/')||!replayPath.endsWith('.rpy'))
    fail('Production Replay capture requires the TH08 normal Runtime Replay path');

  const imported=await runtime.command({command:'read',path:replayPath});
  if(!Array.isArray(imported?.bytes)||imported.bytes.length<1||imported.bytes.length>maxReplayBytes)
    fail('Normal Shell did not return the imported Replay bytes');
  const importedBytes=new Uint8Array(imported.bytes);
  const importedDigest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',importedBytes)),
    value=>value.toString(16).padStart(2,'0')).join('');
  if(importedBytes.length!==replayBytes||importedDigest!==replaySha256)
    fail('Normal Shell Replay readback differs from the selected input');

  // Quiesce the ordinary RAF loop, then use the same production one-tick Shell path as the Lab.
  core.sdl_loop_stop();core.sdl_loop_pause(0);
  const status=field=>core.status(app,field);
  const step=()=>{
    const result=core.sdl_loop_tick(app,1/60,17);
    if(result!==0)fail(`Normal Runtime logical tick failed: ${result}`);
  };
  const nativeKey=(code,down)=>{
    const bytes=new TextEncoder().encode(code+'\0'),pointer=core.allocate(bytes.length);
    if(!Number.isInteger(pointer)||pointer<=0)fail('Normal Runtime keyboard allocation failed');
    try{new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);core.sdl_key(pointer,down?1:0);}
    finally{core.deallocate(pointer);}
  };
  const press=code=>{nativeKey(code,true);for(let i=0;i<3;i++)step();nativeKey(code,false);for(let i=0;i<40;i++)step();};
  const menuNavigation=[];
  for(let i=0;i<12&&status(9)!==4;i++){
    press('ArrowDown');menuNavigation.push({scene:status(0),screen:status(8),cursor:status(9)});
  }
  if(status(9)!==4)fail('Normal title menu did not reach the Replay cursor');
  if(core.sdl_replay_trace_control_v1(1,1)!==1)fail('Normal Replay consumer trace could not be enabled');
  for(let i=0;i<5&&status(0)!==2;i++){
    press('KeyZ');menuNavigation.push({scene:status(0),screen:status(8),cursor:status(9)});
  }
  if(status(0)!==2)fail('Normal title menu did not enter a game scene');
  const clear=core.sdl_keys_clear;
  if(typeof clear==='function')clear();
  else{nativeKey('KeyZ',false);nativeKey('ArrowDown',false);}
  for(let i=0;i<120;i++)step();
  if(status(12)!==1||status(20)!==1)fail('Selected Replay is not active after the 120-tick warmup');
  const warmupTracePtr=core.trace(app),warmupTrace=Array.from(new Uint32Array(core.memory.buffer,warmupTracePtr,68));
  if((warmupTrace[4]&8)===0)fail('Selected game scene is not consuming Replay playback');
  if(core.sdl_replay_trace_control_v1(1,1)!==1)fail('Normal Replay trace reset failed at the capture boundary');

  const navigation={origin:location.origin,epoch:actualEpoch,runtimeUrl:location.href,
    gameGeneration:actualUrl.searchParams.get('gameGeneration'),runtimeVariant:'normal'};
  if(navigation.gameGeneration!=='runtime-fixture')fail('Runtime frame is not the verified normal fixture generation');
  const read=()=>{
    const statusValues=statusFields.map(field=>status(field));
    const tracePointer=core.trace(app),memory=core.memory.buffer;
    if(!Number.isInteger(tracePointer)||tracePointer<=0||tracePointer+68*4>memory.byteLength)fail('Normal trace export returned an invalid pointer');
    const runtimeStatus={fields:[...statusFields],values:statusValues};
    const replayStatus=statusValues.slice(12,22);
    const replayRead=field=>core.sdl_replay_trace_status_v1(field)>>>0;
    const schema=replayRead(29),nextSequence=replayRead(30),firstAvailable=replayRead(31),count=replayRead(32),capacity=replayRead(33);
    if(schema!==replayTraceSchema||capacity!==replayTraceCapacity||count>capacity||firstAvailable>nextSequence||
       nextSequence-firstAvailable!==count)fail('Normal Replay consumer trace metadata is invalid');
    const latest=[];
    if(count){const start=34+(count-1)*replayTraceWords;for(let i=0;i<replayTraceWords;i++)latest.push(replayRead(start+i));
      if(latest[0]!==((nextSequence-1)>>>0))fail('Normal Replay trace latest sample is not the newest sequence');}
    return {schema:'eagler-presentation/production-replay-read/1',navigation:{...navigation},
      runtime:{status:runtimeStatus,trace:Array.from(new Uint32Array(memory,tracePointer,68))},
      replay:{status:replayStatus,trace:{available:true,schema,nextSequence,firstAvailable,count,capacity,latest}}};
  };
  const stableRead=boundary=>{
    const samples=[read(),read()];
    if(JSON.stringify(samples[0])!==JSON.stringify(samples[1]))fail(`${boundary}: two actual normal Runtime reads changed`);
    return {boundary,samples};
  };
  const startPair=stableRead('start'),start=startPair.samples[0],pairs=[startPair],samples=[];
  let statusResult='passed',issue=null,sequenceBefore=start.replay.trace.nextSequence;
  for(let index=0;index<ticks;index++){
    const beforePair=stableRead(`tick/${index+1}/before`),before=beforePair.samples[0];pairs.push(beforePair);
    if(before.replay.trace.nextSequence!==sequenceBefore){statusResult='failed';issue=`tick ${index+1}: Replay trace changed before the requested step`;break;}
    step();
    const afterPair=stableRead(`tick/${index+1}/after-step`),afterStep=afterPair.samples[0];pairs.push(afterPair);
    const sequenceAfter=afterStep.replay.trace.nextSequence;
    if(sequenceAfter!==sequenceBefore+1){statusResult='failed';issue=`tick ${index+1}: exactly one normal Replay trace sample was not appended`;break;}
    const activeIndex=afterStep.runtime.status.fields.indexOf(20);
    if(afterStep.replay.status[0]!==1||activeIndex<0||afterStep.runtime.status.values[activeIndex]!==1){
      statusResult='incomplete';issue=`tick ${index+1}: normal Replay playback left the active game`;break;
    }
    samples.push({logicalTick:index+1,before,afterStep,replayTraceSequence:sequenceAfter});
    sequenceBefore=sequenceAfter;
  }
  const rawReadEvidence={schema:readSchema,epoch:actualEpoch,repetitions:2,byteLimit:maxReadBytes,dropped:0,pairs};
  rawReadEvidence.retainedBytes=pairs.reduce((sum,pair)=>sum+new TextEncoder().encode(JSON.stringify(pair)).byteLength,0);
  if(rawReadEvidence.retainedBytes>maxReadBytes){statusResult='failed';issue='Raw production Replay reads exceed the retention bound';}
  return {schema:runSchema,role:'production',status:statusResult,scope:{game,variant:'normal',targetId},epoch:actualEpoch,
    navigation,fixture:null,replayIdentity:{bytes:importedBytes.length,sha256:importedDigest},
    replayImport:{method:'normal-runtime-shell-write-read',path:replayPath,bytes:importedBytes.length,sha256:importedDigest},
    capture:{method:'normal-runtime-shell-native-exports',requestedTicks:ticks,completedTicks:samples.length,
      menuNavigation,warmupTicks:120,logicalTickMethod:'sdl_loop_tick(1/60,17)',readRepetitions:2},
    start,samples,rawReadEvidence,issues:issue?[issue]:[]};
}

function commonRead(read,label,epoch,navigation){
  const issues=[];
  if(!isRecord(read)||read.schema!=='eagler-presentation/production-replay-read/1')return {issues:[`${label}: normal Runtime read schema is missing`],value:null};
  if(read.navigation?.epoch!==epoch||!same(read.navigation,navigation))issues.push(`${label}: Runtime navigation or epoch changed`);
  const fields=read.runtime?.status?.fields,values=read.runtime?.status?.values;
  if(!Array.isArray(fields)||!Array.isArray(values)||!same(fields,STATUS_FIELDS)||values.length!==fields.length||
      values.some(value=>!Number.isSafeInteger(value)||value< -0x80000000||value>0x7fffffff))
    issues.push(`${label}: shared normal status fields are missing or malformed`);
  if(!Array.isArray(read.runtime?.trace)||read.runtime.trace.length!==68||
      read.runtime.trace.some(value=>!Number.isSafeInteger(value)||value<0||value>0xffffffff))
    issues.push(`${label}: 68-word normal trace is missing or malformed`);
  if(!Array.isArray(read.replay?.status)||read.replay.status.length!==10||
      read.replay.status.some(value=>!Number.isSafeInteger(value)||value< -0x80000000||value>0x7fffffff))
    issues.push(`${label}: ten common Replay status fields are missing or malformed`);
  const trace=read.replay?.trace;
  if(!isRecord(trace)||trace.available!==true||trace.schema!==REPLAY_TRACE_SCHEMA||trace.capacity!==REPLAY_TRACE_CAPACITY||
      !Number.isSafeInteger(trace.nextSequence)||!Number.isSafeInteger(trace.firstAvailable)||!Number.isSafeInteger(trace.count)||
      trace.count<0||trace.count>trace.capacity||trace.nextSequence-trace.firstAvailable!==trace.count||
      !Array.isArray(trace.latest)||(trace.count===0?trace.latest.length!==0:trace.latest.length!==REPLAY_TRACE_WORDS)||
      (trace.count>0&&(trace.latest[0]>>>0)!==((trace.nextSequence-1)>>>0))||
      trace.latest.some(value=>!Number.isSafeInteger(value)||value<0||value>0xffffffff))
    issues.push(`${label}: Replay consumer trace metadata or latest 36-word sample is invalid`);
  return {issues,value:issues.length?null:{runtime:{status:{fields,values},trace:read.runtime.trace},
    replay:{status:read.replay.status,trace}}};
}

function diagnosticCommon(value,label){
  const issues=[];
  if(!isRecord(value)||!isRecord(value.runtime)||!Array.isArray(value.runtime.status)||value.runtime.status.length!==29||
      !Array.isArray(value.runtime.trace)||value.runtime.trace.length!==68||!isRecord(value.replay)||
      !Array.isArray(value.replay.status)||value.replay.status.length!==10||!isRecord(value.replay.trace))
    return {issues:[`${label}: diagnostic checkpoint omits common status, trace, or Replay fields`],value:null};
  if(value.runtime.status.some(word=>!Number.isSafeInteger(word)||word< -0x80000000||word>0x7fffffff))
    issues.push(`${label}: diagnostic Runtime status words are malformed`);
  if(value.runtime.trace.some(word=>!Number.isSafeInteger(word)||word<0||word>0xffffffff))
    issues.push(`${label}: diagnostic Runtime trace words are malformed`);
  if(value.replay.status.some(word=>!Number.isSafeInteger(word)||word< -0x80000000||word>0x7fffffff))
    issues.push(`${label}: diagnostic Replay status words are malformed`);
  // Status 22-24 are compiled only into the diagnostic audit build and have no production counterpart.
  const status={fields:[...STATUS_FIELDS],values:STATUS_FIELDS.map(field=>value.runtime.status[field])};
  const replayTrace=value.replay.trace;
  if(replayTrace.available!==true||replayTrace.schema!==REPLAY_TRACE_SCHEMA||replayTrace.capacity!==REPLAY_TRACE_CAPACITY||
     !Number.isSafeInteger(replayTrace.nextSequence)||!Number.isSafeInteger(replayTrace.firstAvailable)||
     !Number.isSafeInteger(replayTrace.count)||replayTrace.count<0||replayTrace.count>replayTrace.capacity||
     replayTrace.nextSequence-replayTrace.firstAvailable!==replayTrace.count||!Array.isArray(replayTrace.latest)||
     replayTrace.latest.length!==(replayTrace.count===0?0:REPLAY_TRACE_WORDS)||
     replayTrace.latest.some(word=>!Number.isSafeInteger(word)||word<0||word>0xffffffff)||
     (replayTrace.count>0&&(replayTrace.latest[0]>>>0)!==((replayTrace.nextSequence-1)>>>0)))
    issues.push(`${label}: diagnostic Replay consumer trace metadata or sample is malformed`);
  return {issues,value:{runtime:{status,trace:value.runtime.trace},replay:{status:value.replay.status,
    trace:{available:replayTrace.available,schema:replayTrace.schema,nextSequence:replayTrace.nextSequence,
      firstAvailable:replayTrace.firstAvailable,count:replayTrace.count,capacity:replayTrace.capacity,latest:replayTrace.latest}}}};
}

function normalizeExpectedBuild(expectedBuild){
  const source=expectedBuild?.runtime??expectedBuild;
  assert.ok(isRecord(source),'Expected production build identity is required');
  const result={game:source.game,variant:source.variant,packageSha256:source.packageSha256,
    wasmSha256:source.wasmSha256,manifestSha256:source.manifestSha256??null};
  assert.equal(result.game,'th08','Production Replay comparator is registered for th08');
  assert.equal(result.variant,'normal','Production Replay comparator requires the normal Runtime');
  assert.match(result.packageSha256??'',SHA,'Expected Runtime package SHA-256 is required');
  assert.match(result.wasmSha256??'',SHA,'Expected normal execution WASM SHA-256 is required');
  if(result.manifestSha256!==null)assert.match(result.manifestSha256,SHA,'Expected Runtime manifest SHA-256 is invalid');
  return result;
}

function compareRawProductionReads(run,requiredTicks){
  const issues=[],raw=run.rawReadEvidence,expected=[];
  if(!isRecord(raw)||raw.schema!==READ_SCHEMA||!Array.isArray(raw.pairs))
    return {issues:['Production raw repeated reads are missing'],pairCount:0,readCount:0};
  if(raw.epoch!==run.epoch||raw.repetitions!==2||raw.dropped!==0||raw.byteLimit!==MAX_READ_BYTES||
      !Number.isSafeInteger(raw.retainedBytes)||raw.retainedBytes>MAX_READ_BYTES)
    issues.push('Production raw read identity, repetition count, or retention bound is invalid');
  expected.push({boundary:'start',value:run.start});
  for(const [index,sample] of (run.samples??[]).entries()){
    expected.push({boundary:`tick/${index+1}/before`,value:sample.before},
      {boundary:`tick/${index+1}/after-step`,value:sample.afterStep});
  }
  if((run.samples?.length??0)!==requiredTicks)issues.push(`Production sample window does not contain ${requiredTicks} logical ticks`);
  if(raw.pairs.length!==expected.length)issues.push(`Production raw read boundary count differs: expected ${expected.length}, found ${raw.pairs.length}`);
  let retained=0;
  for(const [index,pair] of raw.pairs.entries()){
    retained+=rawBytes(pair);const binding=expected[index];
    if(!binding||pair?.boundary!==binding.boundary){issues.push(`Production raw read boundary ${index} is missing, duplicated, or out of order`);continue;}
    if(!Array.isArray(pair.samples)||pair.samples.length!==2){issues.push(`${pair.boundary}: two actual reads were not retained`);continue;}
    const values=pair.samples.map((sample,readIndex)=>commonRead(sample,`${pair.boundary} read ${readIndex+1}`,run.epoch,run.navigation));
    issues.push(...values.flatMap(result=>result.issues));
    if(values.some(result=>!result.value))continue;
    if(!same(values[0].value,values[1].value))issues.push(`${pair.boundary}: two actual normal Runtime reads changed`);
    if(!same(pair.samples[0],binding.value))issues.push(`${pair.boundary}: raw read differs from the retained production sample`);
  }
  if(retained!==raw.retainedBytes)issues.push('Production raw read byte count differs from the retained data');
  return {issues,pairCount:raw.pairs.length,readCount:raw.pairs.reduce((sum,pair)=>sum+(pair.samples?.length??0),0),retainedBytes:retained};
}

function badResult(issues,status='failed'){
  return {schema:'eagler-presentation/production-replay-assessment/1',status,complete:false,comparedTicks:0,
    firstDivergence:null,sharedFields:{runtimeStatus:[...STATUS_FIELDS],runtimeTraceWords:68,replayStatusFields:10,replayTraceWords:36,
      excludedDiagnosticStatusFields:[22,23,24]},issues};
}

/** Recompute production Replay evidence against retained diagnostic runs and the independently expected inputs. */
export function assessProductionReplayEvidence({productionRun,runs,requiredTicks,scope,expectedBuild,expectedData,expectedReplay}={}){
  const issues=[];
  try{
    const build=normalizeExpectedBuild(expectedBuild);
    if(!Number.isSafeInteger(requiredTicks)||requiredTicks<1||requiredTicks>3600)
      return badResult(['requiredTicks must be 1..3600'],'incomplete');
    assert.ok(isRecord(scope)&&scope.game==='th08'&&scope.variant==='normal','A th08 normal comparison scope is required');
    assert.ok(isRecord(productionRun)&&productionRun.schema===RUN_SCHEMA,'A separately captured normal production Replay run is required');
    assert.equal(productionRun.role,'production','A diagnostic run cannot satisfy the production role');
    assert.equal(productionRun.status,'passed','Production Replay capture is not complete and passed');
    assert.deepEqual(productionRun.scope,{game:scope.game,variant:scope.variant,targetId:productionRun.scope?.targetId},
      'Production Replay scope differs from th08 normal');
    assert.ok(typeof productionRun.scope.targetId==='string'&&productionRun.scope.targetId,'Production target id is missing');
    assert.ok(Number.isSafeInteger(productionRun.epoch)&&productionRun.epoch>0,'Production Runtime epoch is missing');
    assert.ok(isRecord(productionRun.navigation)&&typeof productionRun.navigation.runtimeUrl==='string',
      'Production Runtime navigation is missing');
    const runtimeUrl=new URL(productionRun.navigation.runtimeUrl),hostUrl=new URL(productionRun.hostPage?.url??'');
    assert.ok(productionRun.navigation.origin===runtimeUrl.origin&&productionRun.navigation.origin===hostUrl.origin&&
      productionRun.hostPage.origin===hostUrl.origin&&Number(productionRun.navigation.epoch)===productionRun.epoch&&
      /^https?:$/.test(runtimeUrl.protocol)&&['127.0.0.1','localhost','[::1]'].includes(runtimeUrl.hostname)&&
      runtimeUrl.pathname.startsWith(`/runtime/${productionRun.scope.targetId}/`)&&
      productionRun.navigation.gameGeneration==='runtime-fixture',
    'Production Runtime origin, frame URL, or navigation epoch is invalid');
    assert.equal(productionRun.navigation.runtimeVariant,'normal','Production Runtime navigation is not normal');
    assert.equal(productionRun.capture?.method,'normal-runtime-shell-native-exports','Capture did not use the normal Runtime Shell');
    assert.equal(productionRun.capture?.requestedTicks,requiredTicks,'Production requested tick window differs');
    assert.equal(productionRun.capture?.completedTicks,requiredTicks,'Production completed tick window differs');
    assert.equal(productionRun.capture?.warmupTicks,120,'Production Replay warmup differs from the paired diagnostic run');
    assert.equal(productionRun.capture?.readRepetitions,2,'Production Runtime did not retain two actual reads');
    assert.ok(isRecord(productionRun.identity?.build),'Production package identity is missing');
    const actualBuild={game:productionRun.identity.build.game,variant:productionRun.identity.build.variant,
      packageSha256:productionRun.identity.build.packageSha256,wasmSha256:productionRun.identity.build.wasmSha256,
      manifestSha256:productionRun.identity.build.manifestSha256??null};
    assert.deepEqual(actualBuild,build,'Production Runtime build differs from the independently expected package');
    assert.equal(productionRun.identity.build.diagnostic,false,'Production package is marked diagnostic');
    assert.equal(productionRun.identity.build.profile,'production','Production package profile is missing');
    identityBytesMatch(productionRun.identity.data,expectedData,'Private DATA');
    identityBytesMatch(productionRun.inputIdentity,expectedReplay,'Operator-selected Replay input');
    const expectedReplayContent={bytes:expectedReplay?.bytes,sha256:expectedReplay?.sha256};
    identityBytesMatch(productionRun.replayIdentity,expectedReplayContent,'Replay input');
    identityBytesMatch(productionRun.replayImport,expectedReplayContent,'Runtime filesystem Replay readback');
    assert.equal(productionRun.replayImport.method,'normal-runtime-shell-write-read','Replay was not read back through the normal Shell');
    assert.equal(productionRun.replayImport.path,'replay/th8_01.rpy','Normal Shell Replay import path differs');
    assert.equal(productionRun.capture?.logicalTickMethod,'sdl_loop_tick(1/60,17)','Production logical tick method differs');

    const navigation=productionRun.navigation;
    if(!Array.isArray(runs)||runs.length<1)throw new Error('At least one retained diagnostic Replay run is required');
    const baseline=runs.find(run=>run?.role==='baseline');
    if(!baseline)throw new Error('A diagnostic baseline Replay run is required');
    const diagnosticEpochs=new Set();
    for(const [index,run] of runs.entries()){
      const label=`diagnostic run ${index+1}`;
      if(!isRecord(run)||!['baseline','controlled-extra-draw'].includes(run.role)){
        issues.push(`${label}: unsupported diagnostic run role or metadata`);continue;
      }
      if(run.schema!=='eagler-presentation/replay-pair-run/1'||run.status!=='passed')issues.push(`${label}: retained diagnostic raw run is missing or incomplete`);
      if(run.role==='baseline'&&run.capture?.drawAlphas?.length)issues.push(`${label}: baseline unexpectedly includes diagnostic extra draws`);
      if(!Number.isSafeInteger(run.epoch)||run.epoch<1||diagnosticEpochs.has(run.epoch))issues.push(`${label}: diagnostic run epoch is missing or reused`);
      diagnosticEpochs.add(run.epoch);
      if(!same(run.scope,scope))issues.push(`${label}: diagnostic game or mode differs`);
      if(run.capture?.requestedTicks!==requiredTicks||run.capture?.completedTicks!==requiredTicks||run.samples?.length!==requiredTicks)
        issues.push(`${label}: diagnostic Replay window differs from ${requiredTicks} ticks`);
      if(run.identity?.build?.game!=='th08'||run.identity.build.diagnostic!==true||run.identity.build.instrumented!==true)
        issues.push(`${label}: evidence is not an independently instrumented diagnostic build`);
      const pb=run.identity?.build?.productionBuild;
      if(!isRecord(pb)||pb.available!==true||pb.diagnostic!==false||pb.sourceMatchesCurrent!==true||pb.binaryMatchesDescriptor!==true||
         pb.wasmSha256!==build.wasmSha256)
        issues.push(`${label}: diagnostic run is not bound to the expected normal production build`);
      try{identityBytesMatch(run.identity?.data,expectedData,`${label} DATA`);}
      catch(error){issues.push(error.message);}
      try{identityBytesMatch(run.replayIdentity,expectedReplay,`${label} Replay`);}
      catch(error){issues.push(error.message);}
      const authorityGroups=run.readVerification?.expectedAuthorityGroups;
      if(!Number.isSafeInteger(authorityGroups)||authorityGroups<1||run.readVerification?.status!=='passed'||
         run.readVerification?.repetitions!==2||run.readVerification.authorityGroups!==authorityGroups)
        issues.push(`${label}: diagnostic repeated-read verification is missing`);
      const raw=assessReplayReadEvidence(run,{requiredTicks,expectedAuthorityGroups:authorityGroups});
      if(raw.status!=='passed')issues.push(...raw.issues.map(issue=>`${label}: ${issue}`));
      for(const [pairIndex,pair] of (run.rawReadEvidence?.pairs??[]).entries()){
        for(const [readIndex,checkpoint] of (pair.samples??[]).entries()){
          const native=`${label} raw pair ${pairIndex+1} read ${readIndex+1}`;
          const shared=diagnosticCommon(checkpoint,native);issues.push(...shared.issues);
          if(!Array.isArray(checkpoint?.audio?.auditWords)||checkpoint.audio.auditWords.length!==25||
             checkpoint.audio.auditWords.some(word=>!Number.isSafeInteger(word)||word<0||word>0xffffffff))
            issues.push(`${native}: diagnostic audio audit read is malformed`);
        }
      }
    }
    if(issues.length)return badResult(issues);
    const productionReads=compareRawProductionReads(productionRun,requiredTicks);
    issues.push(...productionReads.issues);
    if(issues.length)return badResult(issues);

    const checkpoints=[{label:'Replay start',production:productionRun.start,diagnostic:baseline.start,tick:0}];
    for(let index=0;index<requiredTicks;index++){
      const normal=productionRun.samples[index],diagnostic=baseline.samples[index];
      if(normal.logicalTick!==index+1||diagnostic.logicalTick!==index+1){
        issues.push(`Logical Replay tick ${index+1} is missing, duplicated, or out of order`);break;
      }
      checkpoints.push({label:`tick ${index+1} before`,production:normal.before,diagnostic:diagnostic.before,tick:index+1});
      checkpoints.push({label:`tick ${index+1} after`,production:normal.afterStep,diagnostic:diagnostic.afterStep,tick:index+1});
      if(normal.replayTraceSequence!==normal.afterStep?.replay?.trace?.nextSequence)
        issues.push(`Production tick ${index+1} trace sequence differs from its captured Replay boundary`);
    }
    let firstDivergence=null,comparedTicks=0;
    for(const checkpoint of checkpoints){
      const actual=commonRead(checkpoint.production,checkpoint.label,productionRun.epoch,navigation);
      const reference=diagnosticCommon(checkpoint.diagnostic,`baseline ${checkpoint.label}`);
      if(actual.issues.length||reference.issues.length){issues.push(...actual.issues,...reference.issues);break;}
      if(!same(actual.value,reference.value)){
        firstDivergence={tick:checkpoint.tick,boundary:checkpoint.label,
          production:actual.value,diagnostic:reference.value};
        issues.push(`${checkpoint.label}: production and diagnostic common Replay state differs`);break;
      }
      if(checkpoint.label.endsWith('after'))comparedTicks++;
    }
    if(comparedTicks!==requiredTicks&&!issues.length)issues.push(`Only ${comparedTicks} of ${requiredTicks} logical Replay ticks were compared`);
    return {schema:'eagler-presentation/production-replay-assessment/1',status:issues.length?'failed':'passed',
      complete:issues.length===0,comparedTicks,firstDivergence,
      production:{epoch:productionRun.epoch,targetId:productionRun.scope.targetId,build:actualBuild,data:productionRun.identity.data,
        replay:productionRun.replayIdentity,navigation},diagnostic:{role:baseline.role,epoch:baseline.epoch,
        build:baseline.identity.build.wasm,data:baseline.identity.data,replay:baseline.replayIdentity},
      rawReads:{production:productionReads,diagnosticRuns:runs.map(run=>({role:run.role,epoch:run.epoch,
        readCount:run.rawReadEvidence?.pairs?.reduce((sum,pair)=>sum+(pair.samples?.length??0),0)??0}))},
      sharedFields:{runtimeStatus:[...STATUS_FIELDS],runtimeTraceWords:68,replayStatusFields:10,replayTraceWords:36,
        excludedDiagnosticStatusFields:[22,23,24]},issues};
  }catch(error){issues.push(String(error?.message??error));return badResult(issues);}
}

export const PRODUCTION_REPLAY_SCHEMAS=Object.freeze({run:RUN_SCHEMA,read:READ_SCHEMA,
  artifact:'eagler-presentation/production-replay-artifact/1',assessment:'eagler-presentation/production-replay-assessment/1'});
