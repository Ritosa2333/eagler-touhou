import assert from 'node:assert/strict';
import {createHash,webcrypto} from 'node:crypto';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import test from 'node:test';
import {runInNewContext} from 'node:vm';
import {captureReplayPairRun} from '../../common/testkit/presentation-lab/replay-pair.mjs';
import {assessProductionReplayEvidence,captureProductionReplayInFrame,readProductionReplayInput} from './presentation-production-replay.mjs';

const hash=value=>createHash('sha256').update(value).digest('hex');
const scope={game:'th08',variant:'normal'};
const normalWasm=hash('normal wasm'),diagnosticWasm=hash('diagnostic wasm');
const packageIdentity={game:'th08',variant:'normal',packageSha256:hash('normal package'),wasmSha256:normalWasm,
  manifestSha256:hash('normal manifest'),sourceBuild:{profile:'production'}};
const dataIdentity={path:'D:/inputs/th08.dat',bytes:15,sha256:hash('private th08 DATA')};
const replayIdentity={path:'D:/inputs/demo0.rpy',bytes:14,sha256:hash('selected Replay')};
const epoch=41,targetId='th08-normal',requiredTicks=3;
const statusFields=[...Array.from({length:22},(_,index)=>index),25,26,27,28];
const navigation={origin:'http://127.0.0.1:8123',epoch,runtimeUrl:`http://127.0.0.1:8123/runtime/${targetId}/index.html?gameGeneration=runtime-fixture&runtimeEpoch=${epoch}`,
  gameGeneration:'runtime-fixture',runtimeVariant:'normal'};

async function makeDiagnosticRun(){
  let tick=0,serial=0;
  const rows={events:[],routes:[],uploads:[],errors:[]};
  const checkpoint=()=>({schema:'eagler-presentation/checkpoint/1',
    state:{complete:true,groups:Array.from({length:17},(_,index)=>({id:`group-${index}`,digest:tick*100+index}))},
    runtime:{status:Array.from({length:29},(_,index)=>index===22?900+tick:index===23?901+tick:index===24?902+tick:tick+index),
      trace:Array.from({length:68},(_,index)=>index===0?tick:index+40)},
    replay:{available:true,status:Array.from({length:10},(_,index)=>index===0?1:tick+index),trace:{available:true,schema:2,
      nextSequence:tick+1,firstAvailable:1,count:tick,capacity:256,
      latest:tick?Array.from({length:36},(_,index)=>index===0?tick:index+70):[]}},
    audio:{auditWords:Array.from({length:25},(_,index)=>tick+index),sfx:{available:true,enabled:true,sequence:serial,
      counts:{events:rows.events.length,routes:rows.routes.length,uploads:0,errors:0},dropped:{events:0,routes:0,uploads:0,errors:0}}}});
  const readAudioRowsSince=since=>({available:true,sequence:serial,enabled:true,
    counts:{events:rows.events.length,routes:rows.routes.length,uploads:0,errors:0},
    dropped:{events:0,routes:0,uploads:0,errors:0},
    ...Object.fromEntries(Object.entries(rows).map(([key,value])=>[key,key==='errors'?[]:value.filter(row=>row.serial>since)]))});
  const controller={freeze(){},step(){
    tick++;const eventSerial=++serial;
    rows.events.push({serial:eventSerial,logicalIndex:tick,voiceKey:'normal-replay'});
    rows.routes.push({serial:++serial,eventSerial,logicalIndex:tick,direct:true,fallback:false,fallbackStarted:false,pending:false});
  },preview(){return{status:'drawn'};}};
  const run=await captureReplayPairRun({controller,role:'baseline',ticks:requiredTicks,identity:{build:{game:'th08',wasm:diagnosticWasm,
      diagnostic:true,instrumented:true,profile:'presentation-lab',productionBuild:{available:true,diagnostic:false,
      sourceMatchesCurrent:true,binaryMatchesDescriptor:true,wasmSha256:normalWasm}},data:{bytes:dataIdentity.bytes,sha256:dataIdentity.sha256}},
    replayIdentity:{bytes:replayIdentity.bytes,sha256:replayIdentity.sha256},epoch:3,scope,
    readCheckpoint:checkpoint,readAudioRowsSince,expectedAuthorityGroups:17,yieldEvery:requiredTicks});
  run.firstFrame={status:'passed',schema:'th08/first-frame-receipt/1',epoch:run.epoch,event:'first-frame',
    origin:'http://127.0.0.1:8123',nativePresentCount:3};
  return run;
}

function normalRead(checkpoint){
  const values=statusFields.map(field=>checkpoint.runtime.status[field]);
  const trace=checkpoint.replay.trace;
  return{schema:'eagler-presentation/production-replay-read/1',navigation:{...navigation},
    runtime:{status:{fields:[...statusFields],values},trace:[...checkpoint.runtime.trace]},
    replay:{status:[...checkpoint.replay.status],trace:{available:trace.available,schema:trace.schema,
      nextSequence:trace.nextSequence,firstAvailable:trace.firstAvailable,count:trace.count,capacity:trace.capacity,
      latest:[...trace.latest]}}};
}

function makeProductionRun(diagnostic){
  const start=normalRead(diagnostic.start),samples=diagnostic.samples.map(sample=>({logicalTick:sample.logicalTick,
    before:normalRead(diagnostic.rawReadEvidence.pairs.find(pair=>pair.boundary===`tick/${sample.logicalTick}/before`).samples[0]),
    afterStep:normalRead(diagnostic.rawReadEvidence.pairs.find(pair=>pair.boundary===`tick/${sample.logicalTick}/after-step`).samples[0]),
    replayTraceSequence:sample.replayTraceSequence}));
  const pairs=[{boundary:'start',samples:[structuredClone(start),structuredClone(start)]},
    ...samples.flatMap(sample=>[
      {boundary:`tick/${sample.logicalTick}/before`,samples:[structuredClone(sample.before),structuredClone(sample.before)]},
      {boundary:`tick/${sample.logicalTick}/after-step`,samples:[structuredClone(sample.afterStep),structuredClone(sample.afterStep)]},
    ])];
  const retainedBytes=pairs.reduce((sum,pair)=>sum+Buffer.byteLength(JSON.stringify(pair)),0);
  return{schema:'eagler-presentation/production-replay-capture/1',role:'production',status:'passed',scope:{...scope,targetId},epoch,
    navigation:{...navigation},hostPage:{url:'http://127.0.0.1:8123/',origin:'http://127.0.0.1:8123'},
    identity:{build:{...packageIdentity,profile:'production',diagnostic:false},data:{...dataIdentity}},
    inputIdentity:{...replayIdentity},replayIdentity:{bytes:replayIdentity.bytes,sha256:replayIdentity.sha256},
    replayImport:{method:'normal-runtime-shell-write-read',path:'replay/th8_01.rpy',bytes:replayIdentity.bytes,sha256:replayIdentity.sha256},
    capture:{method:'normal-runtime-shell-native-exports',requestedTicks:requiredTicks,completedTicks:requiredTicks,
      warmupTicks:120,readRepetitions:2,logicalTickMethod:'sdl_loop_tick(1/60,17)'},start,samples,
    rawReadEvidence:{schema:'eagler-presentation/production-replay-reads/1',epoch,repetitions:2,byteLimit:128*1024*1024,
      dropped:0,retainedBytes,pairs}};
}

async function fixture(){
  const diagnostic=await makeDiagnosticRun();
  return{diagnostic,production:makeProductionRun(diagnostic)};
}

function assess({production,diagnostic},overrides={}){
  return assessProductionReplayEvidence({productionRun:production,runs:[diagnostic],requiredTicks,scope,
    expectedBuild:packageIdentity,expectedData:dataIdentity,expectedReplay:replayIdentity,...overrides});
}

test('normal Runtime raw reads compare exactly with the diagnostic Replay boundary while excluding audit-only fields',async()=>{
  const evidence=await fixture();
  assert.equal(evidence.diagnostic.status,'passed');
  assert.notEqual(evidence.diagnostic.start.runtime.status[22],evidence.production.start.runtime.status.values[22]??-1);
  const report=assess(evidence);
  assert.equal(report.status,'passed',JSON.stringify(report.issues));
  assert.equal(report.complete,true);assert.equal(report.comparedTicks,requiredTicks);
  assert.deepEqual(report.sharedFields.excludedDiagnosticStatusFields,[22,23,24]);
  assert.equal(report.rawReads.production.readCount,(1+2*requiredTicks)*2);
});

test('metadata-only evidence and a diagnostic run relabeled as production are rejected',async()=>{
  const evidence=await fixture();
  evidence.production.rawReadEvidence.pairs=[];evidence.production.rawReadEvidence.retainedBytes=0;
  assert.notEqual(assess(evidence).status,'passed');
  const clone={...evidence,production:structuredClone(evidence.diagnostic)};
  clone.production.role='production';
  assert.notEqual(assess(clone).status,'passed');
});

test('manual diagnostic Replay uses its actual epoch and reads without manufacturing a first-frame event',async()=>{
  const evidence=await fixture();
  evidence.diagnostic.firstFrame={status:'incomplete',epoch:evidence.diagnostic.epoch,firstFrame:null};
  const report=assess(evidence);
  assert.equal(report.status,'passed',report.issues.join('; '));
  assert.equal(report.comparedTicks,requiredTicks);
  evidence.production.navigation.epoch++;
  assert.equal(assess(evidence).status,'failed','Normal Runtime navigation binding remains mandatory');
});

test('tampered data, Replay identity, production build, navigation, epoch, and window are rejected',async()=>{
  const cases=[
    ['DATA',({production})=>{production.identity.data.sha256=hash('different DATA');}],
    ['Replay input',({production})=>{production.inputIdentity.sha256=hash('different Replay');}],
    ['WASM',({diagnostic})=>{diagnostic.identity.build.productionBuild.wasmSha256=hash('wrong wasm');}],
    ['mode',({production})=>{production.navigation.runtimeVariant='multiplayer';}],
    ['navigation generation',({production})=>{production.navigation.gameGeneration='presentation-lab';}],
    ['navigation epoch',({production})=>{production.navigation.epoch++;}],
    ['capture window',({production})=>{production.capture.requestedTicks--; }],
    ['trace data',({production})=>{production.samples[1].afterStep.runtime.trace[3]++;}],
    ['missing actual read',({production})=>{production.rawReadEvidence.pairs[1].samples.pop();}],
  ];
  for(const [name,mutate] of cases){
    const evidence=await fixture();mutate(evidence);
    const report=assess(evidence);
    assert.notEqual(report.status,'passed',`${name} mutation was accepted`);
  }
});

test('selected Replay files must be absolute regular .rpy inputs and preserve their raw identity',async t=>{
  const directory=await mkdtemp(join(tmpdir(),'production-replay-input-'));
  t.after(()=>rm(directory,{recursive:true,force:true}));
  const path=join(directory,'selected.rpy'),contents=Buffer.from('replay bytes');await writeFile(path,contents);
  const identity=await readProductionReplayInput(path);
  assert.equal(identity.path,resolve(path));assert.equal(identity.bytes,contents.length);
  assert.equal(identity.sha256,hash(contents));assert.deepEqual(identity.contents,contents);
  await assert.rejects(readProductionReplayInput('relative.rpy'),/absolute/);
  await assert.rejects(readProductionReplayInput(join(directory,'selected.txt')),/\.rpy/);
});

test('browser-side Replay capture runs from its serialized source in an isolated page realm',async()=>{
  const contents=Buffer.from('selected replay bytes');
  let logicalSteps=0,traceCount=0;
  const memory=new ArrayBuffer(69*Uint32Array.BYTES_PER_ELEMENT);
  new Uint32Array(memory)[5]=8;
  const traceStatus=field=>{
    const nextSequence=traceCount+1,count=Math.min(traceCount,256),firstAvailable=nextSequence-count;
    if(field===29)return 2;
    if(field===30)return nextSequence;
    if(field===31)return firstAvailable;
    if(field===32)return count;
    if(field===33)return 256;
    if(field===34+(count-1)*36&&count)return traceCount;
    return 0;
  };
  const core={memory:{buffer:memory},status:(_app,field)=>field===0?2:field===9?4:field===12||field===20?1:0,trace:()=>4,
    sdl_replay_trace_control_v1:()=>{traceCount=0;return 1;},sdl_replay_trace_status_v1:traceStatus,
    sdl_loop_tick:()=>{logicalSteps++;traceCount++;new Uint32Array(memory)[1]=logicalSteps;return 0;},
    sdl_loop_stop(){},sdl_loop_pause(){},sdl_key(){},allocate:()=>4,deallocate(){}};
  const runtime={core,app:1,command:async()=>({bytes:Array.from(contents)})};
  const frameGlobals={window:{__th08Runtime:runtime},location:{origin:navigation.origin,href:navigation.runtimeUrl},
    URL,Uint8Array,ArrayBuffer,TextEncoder,crypto:webcrypto};
  const capture=runInNewContext(`(${captureProductionReplayInFrame.toString()})`,frameGlobals);
  const run=await capture({game:'th08',targetId,epoch,replayPath:'replay/th8_01.rpy',replayBytes:contents.length,
    replaySha256:hash(contents),ticks:2,expectedOrigin:navigation.origin,captureContract:{
      runSchema:'eagler-presentation/production-replay-capture/1',readSchema:'eagler-presentation/production-replay-reads/1',
      replayTraceSchema:2,replayTraceCapacity:256,replayTraceWords:36,maxReplayBytes:16*1024*1024,
      maxReadBytes:128*1024*1024,statusFields,
    }});
  assert.equal(run.status,'passed',run.issues.join('; '));
  assert.equal(run.capture.completedTicks,2);
  assert.equal(run.start.replay.trace.count,0);
  assert.equal(run.start.replay.trace.latest.length,0);
  assert.equal(logicalSteps,122,'the normal capture performs its 120-tick warmup before the requested window');
});
