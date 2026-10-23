import test from 'node:test';
import assert from 'node:assert/strict';
import {PresentationLabControllerCore} from './controller-core.mjs';
import {historyReportSummary} from './report-core.mjs';
import {captureReplayPairRun} from './replay-pair.mjs';

test('history exports an index while the current window keeps its original raw proof',()=>{
  const report={schema:'test/audit/1',tick:12,label:'motion',objects:[{key:'player/1',values:[99],fields:[{id:'x',status:'interpolated',samples:[1,2]}]}],counts:{interpolated:1},
    nativeFrames:{samples:[1,2]},nativeLifecycle:{events:[3]},readVerification:{receipts:[4]},images:[5],
    futureRawField:{bytes:[6]}};
  const summary=historyReportSummary(report);
  assert.equal(summary.retention,'history-summary');assert.equal(summary.rawEvidenceRetained,false);
  assert.deepEqual(summary.counts,report.counts);assert.equal(summary.tick,12);
  assert.deepEqual(summary.objects,[{key:'player/1',fields:[{id:'x',status:'interpolated'}]}]);
  for(const key of ['nativeFrames','nativeLifecycle','readVerification','images','futureRawField'])assert.equal(key in summary,false);
  assert.deepEqual(report.nativeFrames.samples,[1,2]);assert.deepEqual(report.readVerification.receipts,[4]);
});

function fixture() {
  let ticks=0,draws=0,stopped=0,fault=false,generation=0,token=null;
  const driver={
    describe:()=>({driverApiVersion:1,game:'fixture',adapterVersion:'fixture/1',nativeAbi:'fixture/1',features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true}}),
    freeze:()=>{stopped++;return token??=Object.freeze({sessionEpoch:1,generation:++generation});},
    resume:value=>{assert.equal(value,token);token=null;},
    advanceOneTick:()=>{ticks++;return {status:'advanced',advancedTicks:1};},
    drawOnly:({alpha})=>{assert.ok(alpha>=0&&alpha<=1);draws++;return {status:'drawn'};},
    setInput:()=>{},clearInput:()=>{},close:()=>{},
  };
  const record=reference=>({tick:ticks,reference,records:[]});
  const observer={
    observationApiVersion:1,scanSchema:'test/scan/1',sessionSchema:'test/session/1',
    enable:()=>{},readReferences:()=>({previous:record(0),current:record(1)}),readObservation:()=>record(null),
    readStateEvidence:()=>[ticks],readTiming:()=>({ticks}),readTrace:()=>[ticks],readScene:()=>({ticks}),
    worldFrozen:()=>false,readGate:()=>true,setNegativeControl:value=>{fault=value;},
    captureImage:(_frame,alpha)=>({alpha}),
    analyze:input=>({schema:'test/audit/1',tick:input.current.tick,objects:[],purity:true}),
    compact:report=>report,mergeIssues:()=>[],
  };
  const controller=new PresentationLabControllerCore({driver,observer,identity:{wasm:'fixture'}});
  return {controller,counts:()=>({ticks,draws,stopped,fault})};
}

test('alpha preview is draw-only and restores the negative control',()=>{
  const {controller,counts}=fixture();const frame=controller.preview(.5,{negativeControl:true});
  assert.equal(frame.status,'drawn');assert.deepEqual(frame.records,[]);
  assert.deepEqual(counts(),{ticks:0,draws:1,stopped:1,fault:false});
});

test('paired Replay consumes the real shared controller preview receipt',async()=>{
  const {controller,counts}=fixture();
  const checkpoint=()=>{const tick=counts().ticks;return {schema:'eagler-presentation/checkpoint/1',
    state:{complete:true,groups:Array.from({length:17},(_,id)=>({id:`owner-${id}`,digest:tick}))},
    runtime:{status:[2,tick],trace:[tick]},replay:{available:true,status:[1,tick],
      trace:{available:true,schema:2,nextSequence:tick+1,firstAvailable:1,count:tick,capacity:256,
        latest:tick?Array.from({length:36},(_,i)=>i===0?tick:0):[]}},
    audio:{auditWords:[1,0],sfx:{available:true,enabled:true,sequence:0,
      counts:{events:0,routes:0,uploads:0,errors:0},dropped:{events:0,routes:0,uploads:0,errors:0}}}};};
  const run=await captureReplayPairRun({controller,role:'controlled-extra-draw',ticks:1,epoch:1,
    identity:{build:{game:'fixture',wasm:'a'.repeat(64)},data:{bytes:1,sha256:'b'.repeat(64)}},
    replayIdentity:{bytes:1,sha256:'a'.repeat(64)},scope:{game:'fixture',variant:'normal'},
    readCheckpoint:checkpoint,readAudioRowsSince:()=>({available:true,events:[],routes:[],uploads:[],errors:[],
      dropped:{events:0,routes:0,uploads:0,errors:0}}),expectedAuthorityGroups:17});
  assert.equal(run.samples.length,1);
  assert.equal(counts().ticks,1);assert.equal(counts().draws,3);
  assert.ok(run.samples[0].drawReceipts.every(row=>row.status==='drawn'&&row.unchanged));
});
test('fast stepping executes every complete fixed tick',()=>{
  const {controller,counts}=fixture();controller.step(60);assert.equal(counts().ticks,60);
  assert.throws(()=>controller.step(0),/1\.\.600/);
});
test('sweep samples stable alpha endpoints without advancing simulation',()=>{
  const {controller,counts}=fixture();const report=controller.sweep({images:true});
  assert.equal(counts().ticks,0);assert.equal(counts().draws,8);assert.equal(report.images.length,5);
  assert.equal(report.readVerification.receipts.length,9);
  assert.equal(report.readVerification.receipts.flatMap(item=>item.reads).length,18);
  assert.deepEqual(report.readVerification.receipts.map(item=>item.alpha),[null,0,.25,.5,.75,1,.5,.5,1]);
});
test('sweep retains every second actual state read rather than manufacturing duplicate receipts',()=>{
  const {controller}=fixture();let reads=0,analyzed;
  controller.observer.readStateEvidence=()=>({sequence:++reads});
  controller.observer.analyze=input=>{analyzed=input;return {objects:[],purity:false};};
  const report=controller.sweep();
  assert.equal(reads,18);
  assert.deepEqual(report.readVerification.receipts.flatMap(item=>item.reads.map(read=>read.sequence)),Array.from({length:18},(_,index)=>index+1));
  assert.equal(analyzed.stateBefore.sequence,1);
  assert.deepEqual(analyzed.statesAfter.map(read=>read.sequence),Array.from({length:17},(_,index)=>index+2));
});
