import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,readFileSync,rmSync,writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname,join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';

const analyser=join(dirname(fileURLToPath(import.meta.url)),'analyze-motion-cadence.mjs');
function row(index,options={}){
  const o={time:index*8,commit:index+1,draw:index+1,epoch:10,frame:10,center:0,auth:null,
    slot:7,life:1,state:1,script:4,sprite:9,generation:1,scene:2,loading:0,nativeMode:0,
    alpha:1,renderOnly:1,active:0,completed:0,high:1,previousValid:false,
    admission:0,retry:0,correcting:0,yielded:0,yields:0,corrections:0,resimulated:0,
    scale:1,missing:0,...options};
  const motion=Array(108).fill(0),net=Array(15).fill(0),driver=Array(17).fill(0);
  motion[0]=1;motion[1]=o.commit;motion[2]=index+1;motion[3]=o.draw;motion[4]=o.epoch;
  motion[5]=motion[6]=o.alpha;motion[7]=o.renderOnly;motion[8]=o.active;
  motion[9]=o.slot;motion[10]=o.life;motion[11]=24;
  const b=84,auth=o.auth??o.center;
  motion[b]=1;motion[b+1]=2;motion[b+2]=o.slot;motion[b+3]=o.life;
  motion[b+4]=10;motion[b+5]=o.state;motion[b+6]=o.script;motion[b+7]=o.sprite;
  motion[b+8]=auth;motion[b+9]=0;motion[b+10]=auth-1;motion[b+11]=0;
  motion[b+12]=9;motion[b+13]=+o.previousValid;motion[b+14]=o.center;motion[b+15]=0;
  motion[b+16]=o.center;motion[b+17]=0;motion[b+20]=1;motion[b+21]=1;motion[b+22]=255;
  net[3]=o.frame;net[9]=o.admission;net[10]=o.generation;net[14]=o.nativeMode;
  driver[3]=o.corrections;driver[4]=o.resimulated;
  return {time:o.time,ms:2,logicalFrame:o.frame,scene:o.scene,loading:o.loading,
    progress:[o.frame,o.frame,o.correcting,o.yielded,o.yields,0],net,driver,motion,
    cadence:[o.scale,o.missing,o.retry,0,o.completed,o.high,o.active,o.alpha]};
}
const endpoint=(index,options={})=>row(index,{renderOnly:0,completed:1,...options});
function analyse(rows){
  const dir=mkdtempSync(join(tmpdir(),'th08-motion-analysis-'));
  try{
    const input=join(dir,'1-frontier.json'),output=join(dir,'analysis.json');
    writeFileSync(input,JSON.stringify({frames:1000,passed:true,production:false,
      peers:[{seat:0,timeOrigin:0,end:1000,timing:{native:{mode:0,delay:0}},rows}]}));
    execFileSync(process.execPath,[analyser,'--edge-ms','0','--output',output,input],
      {encoding:'utf8',timeout:10000,maxBuffer:4*1024*1024});
    const report=JSON.parse(readFileSync(output,'utf8'));
    return {report,peer:report.files[0].peers[0]};
  }finally{rmSync(dir,{recursive:true,force:true});}
}

test('committed alpha1 endpoint holds are measured even without a previous endpoint model',()=>{
  const {report,peer}=analyse([endpoint(0),row(1),row(2),row(3),
    endpoint(4,{center:3,frame:11,epoch:11})]);
  const h=peer.endpointHolds;
  assert.equal(report.observationRevision,2);
  assert.equal(h.runs,1);assert.equal(h.heldCommitPairs,3);assert.equal(h.censoredRuns,0);
  assert.equal(h.observedHeldMs.max,24);assert.equal(h.releaseDelayMs.max,32);
  assert.equal(h.releaseStepPx.max,3);assert.equal(h.forwardBudgetCandidateHolds.runs,1);
  assert.equal(h.gateDisabledReadonlyAlpha1HeldCommitPairs,3);
  assert.equal(h.missingEndpointModelHeldCommitPairs,3);
  assert.equal(h.observedWaitEvidence.noneObserved,3);
  assert.equal(peer.signals.centerHold.count,3);assert.equal(peer.signals.centerHoldWhileModelMoves.count,0);
  assert.equal(h.changedCenterCommitPairs,1);assert.equal(h.observedStableCommitPairs,4);
  assert.equal(h.examples[0].budgetCauseProven,false);
  assert.equal(h.examples[0].endpointKind,'highRefreshSemanticForwardBudgetCandidate');
  assert.equal(report.rawWindow.signal,'alpha1EndpointHeldAcrossCommits');
  assert.equal(report.rawWindow.rows[0].calculationEpoch,10);
  assert.equal(report.rawWindow.rows[0].renderOnly,0);
  assert.equal(report.rawWindow.rows[0].cadence.completedForward,1);
});

test('uncommitted callbacks do not add holds; repeated and newly drawn commits remain separate',()=>{
  const start=endpoint(0);
  const duplicate={...row(1,{commit:1,draw:1}),motion:[...start.motion]};
  const {peer}=analyse([start,duplicate,row(2,{commit:2,draw:1}),row(3,{commit:3,draw:2}),
    endpoint(4,{commit:4,draw:3,center:3})]);
  const h=peer.endpointHolds;
  assert.equal(peer.commits.same,1);assert.equal(h.heldCommitPairs,2);
  assert.equal(h.reusedDrawHeldCommitPairs,1);assert.equal(h.redrawnHeldCommitPairs,1);
  assert.equal(h.observedHeldMs.max,24);assert.equal(h.releaseDelayMs.max,32);
});

test('readonly alpha1 and low-refresh semantic endpoints are never declared budget candidates',()=>{
  const readonly=analyse([row(0),row(1),row(2,{center:2})]).peer.endpointHolds;
  assert.equal(readonly.runs,1);assert.equal(readonly.highRefreshSemanticForwardBudgetCandidates,0);
  assert.equal(readonly.examples[0].endpointKind,'readonlyAlpha1');
  const low=analyse([endpoint(0,{high:0}),endpoint(1,{high:0}),endpoint(2,{high:0,center:2})]).peer.endpointHolds;
  assert.equal(low.runs,1);assert.equal(low.forwardBudgetCandidateHolds.runs,0);
  assert.equal(low.examples[0].endpointKind,'otherAlpha1');
});

test('same-owner hold runs stop at slot, life, generation, scene, state, script and sprite boundaries',()=>{
  for(const change of [{slot:8},{life:2},{generation:2},{scene:3},{state:2},{script:5},{sprite:10}]){
    const {peer}=analyse([endpoint(0),row(1,change),row(2,{...change,center:3})]);
    assert.equal(peer.endpointHolds.runs,0,JSON.stringify(change));
    assert.equal(peer.endpointHolds.heldCommitPairs,0,JSON.stringify(change));
  }
});

test('admission, correction and unknown retry evidence are separate without claiming a network cause',()=>{
  const cases=[
    ['correctionYield',{correcting:1,yielded:1,yields:1,retry:1},true],
    ['correctionWork',{correcting:1},false],
    ['correctionWork',{corrections:1,resimulated:4},false],
    ['admissionWait',{admission:1,retry:1},true],
    ['correctionAndAdmission',{admission:1,correcting:1,yielded:1,yields:1,retry:1},true],
    ['pendingUnknown',{retry:1},true],
    ['noneObserved',{},false],
    ['unknown',{unknown:true},false]
  ];
  for(const [category,options,legacyWaiting] of cases){
    const held=row(1,options);
    if(options.unknown){held.progress=null;held.net[9]=null;}
    const {peer}=analyse([endpoint(0),held]);
    assert.equal(peer.endpointHolds.observedWaitEvidence[category],1,category);
    assert.equal(peer.signals.centerHold.observedWaitEvidence[category],1,category);
    assert.equal(peer.signals.centerHold.whileNetworkWaiting,+legacyWaiting,category);
    assert.equal(peer.endpointHolds.censoredRuns,1,category);
  }
});

test('calculation epoch changes do not masquerade as new bullet ownership',()=>{
  const {peer}=analyse([endpoint(0),row(1),endpoint(2,{epoch:11,frame:11,auth:4}),
    row(3,{epoch:11,frame:11,center:3,auth:4})]);
  const h=peer.endpointHolds;
  assert.equal(h.runs,1);assert.equal(h.heldCommitPairs,2);
  assert.equal(h.sameCalculationEpochHeldCommitPairs,1);
  assert.equal(h.changedCalculationEpochHeldCommitPairs,1);
  assert.equal(h.forwardProgressHeldCommitPairs,1);assert.equal(h.authoritativeMovedHeldCommitPairs,1);
  assert.equal(h.observedHeldMs.max,16);assert.equal(h.releaseDelayMs.max,24);
});

test('incomplete mode/cadence or loading evidence cannot produce a forward budget candidate',()=>{
  for(const options of [{nativeMode:2},{loading:1},{scene:3},{active:1},{high:0}]){
    const {peer}=analyse([endpoint(0,options),row(1,{scene:options.scene??2,loading:options.loading??0})]);
    assert.equal(peer.endpointHolds.highRefreshSemanticForwardBudgetCandidates,0,JSON.stringify(options));
    assert.equal(peer.endpointHolds.forwardBudgetCandidateHolds.runs,0,JSON.stringify(options));
  }
  const start=endpoint(0);start.cadence[4]=null;
  assert.equal(analyse([start,row(1)]).peer.endpointHolds.highRefreshSemanticForwardBudgetCandidates,0);
});

test('sampled clock braking is reported independently of admission/correction evidence',()=>{
  const {report,peer}=analyse([endpoint(0,{scale:1.5,missing:3}),
    row(1,{scale:1.5,missing:3}),row(2,{center:3,scale:1.5,missing:3})]);
  const h=peer.endpointHolds;
  assert.equal(h.intervalScaleAboveOneHeldCommitPairs,1);assert.equal(h.missingRemoteInputHeldCommitPairs,1);
  assert.equal(h.observedWaitEvidence.noneObserved,1);
  assert.equal(h.examples[0].endpointEvidence.intervalScale,1.5);
  assert.equal(report.rawWindow.rows[1].cadence.scale,1.5);
  assert.equal(report.rawWindow.rows[1].observedWaitEvidence.admissionWait,false);
});

test('non-alpha1 held geometry stays visible in the report without being called an alpha1 lock',()=>{
  const {peer}=analyse([endpoint(0),row(1,{alpha:.5,active:1}),row(2,{alpha:.75,active:1,center:2})]);
  const h=peer.endpointHolds;
  assert.equal(h.heldCommitPairs,1);assert.equal(h.alpha1HeldCommitPairs,0);
  assert.equal(h.gateDisabledReadonlyAlpha1HeldCommitPairs,0);
});

test('missing commits and invalid geometry do not create bridged hold runs',()=>{
  const skipped=analyse([endpoint(0),row(1,{commit:3}),row(2,{commit:4,center:2})]).peer;
  assert.equal(skipped.endpointHolds.runs,0);
  assert.equal(skipped.commits.unobservedIntermediateCommits,1);
  const invalid=row(1);invalid.motion[84+20]=0;
  const broken=analyse([endpoint(0),invalid,row(2,{center:2})]).peer;
  assert.equal(broken.endpointHolds.runs,0);assert.equal(broken.counts.invalidBulletSamples,1);
});
