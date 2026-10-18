// Predeclared offline information-budget experiment. No browser/game/subprocess/timers.
// Run only after root authorizes the single offline execution.
// Usage: node analyze-information-pacing-shadow.mjs [plan.json [new-evidence.json]]
// The plan, analyzer hash and expected input hashes are recorded before raw parsing.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';

const project=path.resolve(fileURLToPath(new URL('../../../',import.meta.url)));
const self=fileURLToPath(import.meta.url);
const LOCKED_POLICY={"logicalHz":60,"historyFrames":8,"safeReserveFrames":1,"networkScaleMinimum":1,"networkScaleMaximum":2,"phaseScaleMinimum":0.98,"phaseScaleMaximum":1.02,"rampMsPerScaleUnit":50,"legacyOutstandingStart":4,"legacyOutstandingRampFrames":3,"formalStartFrontier":180,"formalEndFrontier":1800,"historyGaps":32,"minimumHistoryGaps":16,"conditionalQuantile":0.9,"minimumConditionalSurvivors":4,"sampleGapCensorMs":50,"nativeCallbackCensorMs":10,"rafRequestWaitCensorMs":50,"readBracketCensorMs":5,"clockEqualityToleranceMs":0.00001,"maxSamplesPerPeer":16384,"maxInputBytes":41943040,"maxOutputBytes":16777216};
const LOCKED_INPUTS=[{"id":"relay39_burst_response","path":"th09_web/artifacts/multiplayer-tests/rollback4x-chain-release-input-batch-headed-relay-burst-response-01-report.json","bytes":21525676,"sha256":"09bb173bee138e05916d0557c0a42d5797c6be200e4825e97e42695063582376","identityWitness":"docs/multiplayer/rollback-chain-release-burst-response-evidence.json","identityWitnessSha256":"b254a712a15f79205c96fc0e3517e7e9e5dc9a52db7cc4ba21478c95e554bac9","expectedParameters":{"limit":1800,"delay":39,"jitter":5,"slow":4,"release":true,"fallback":true,"inputMode":"wall-clock","rafDiagnostics":true,"networkTrace":false,"impairmentSite":"relay","burstAt":20000,"burstFor":500,"burstDelay":120}},{"id":"relay125_steady","path":"th09_web/artifacts/multiplayer-tests/rollback4x-chain-release-input-batch-headed-relay125-01-report.json","bytes":28866220,"sha256":"d03833c48742160afa7956a70b4aa82f135069005e3b4b38422a6b3c08bca770","identityWitness":"docs/multiplayer/rollback-chain-release-relay125-transport-evidence.json","identityWitnessSha256":"57942083c992c6fbf19a09092370541e41a3cf403e9ab8a7f218bfd0ae182972","expectedParameters":{"limit":1800,"delay":125,"jitter":5,"slow":4,"release":true,"fallback":true,"inputMode":"wall-clock","rafDiagnostics":true,"networkTrace":false,"impairmentSite":"relay","burstAt":0,"burstFor":0,"burstDelay":0}}];
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const finite=value=>typeof value==='number'&&Number.isFinite(value);
const integer=value=>Number.isSafeInteger(value)&&value>=0;
const array=value=>Array.isArray(value)?value:[];
const clamp=(value,lo,hi)=>Math.max(lo,Math.min(hi,value));
const MASK={
 readBracketUnavailable:1,sampleGap:2,nativeWork:4,replayWorkOrDebt:8,
 correctionStarted:16,rafRequestWait:32,lifecycleNotVisible:64,loopInterruptedOrPaused:128,
 rafDiagnosticInvalid:256,readBracketWide:512,lifecycleUnavailable:1024,
 loopStateUnavailable:2048,clockOrder:4096
};
const ESTIMATE=['left-censored','current-clock-unavailable','insufficient-history','conditional-tail-unsupported','supported'];
const WARNING=['none','predicted-bridgeable','predicted-beyond-ramp','overdue-unknown'];
const QUALIFICATION=['unclassified','observed-fields-unflagged','local-observation-contaminated','right-censored-no-next-A','clock-unavailable'];
const GEOMETRY=['unknown','unnecessary-warning-proxy','model-bridgeable','beyond-ramp-model','interval-overlap-unknown'];
const COHORT=['prefix','body','tail','formal-boundary-unavailable'];
const COLUMNS=[
 'sampleIndex','markerMs','aReadLowerMs','aReadUpperMs','F','A','B','Q','Qfull','R','next','ok','nativeWorkMs',
 'networkScaleMilli','effectiveScaleMilli','targetFromObservedB','gapId','ageLowerMs','ageUpperMs','ageNominalMs',
 'eligibleHistoryCount','conditionalSurvivors','possibleSurvivors','definiteSurvivors','estimateStatus',
 'residualNominalMs','residualLowerMs','residualUpperMs','historicalP90GapNominalMs','historicalMaxGapNominalMs',
 'overdueNominal','overdueDefinite','holdQMs','rampQMs','holdQfullMs','rampQfullMs','holdSafeMs','rampSafeMs',
 'localObservationMask','runningGapObservationMask','prebrakeCandidate','warningCode','closureQualification',
 'geometricNextProgressClass','nextAProgressIndex','nextAProgressDelayLowerMs','nextAProgressDelayUpperMs',
 'originalBThresholdLeadLowerMs','originalBThresholdLeadUpperMs','originalNetworkBrakeLeadLowerMs',
 'originalNetworkBrakeLeadUpperMs','positiveLeadModelOpportunity','retrospectiveModelOpportunityWithoutWarning',
 'cohort','rafRequestWaitMs','observationGapMs','readBracketMs','historyOldestGapId','historyNewestGapId'
];
const GAP_COLUMNS=[
 'gapId','A','startSampleIndex','closeSampleIndex','startProgressLowerMs','startProgressUpperMs',
 'closeProgressLowerMs','closeProgressUpperMs','durationLowerMs','durationUpperMs','durationNominalMs',
 'closingDeltaA','leftCensored','rightCensored','wholeGapObservationMask','trainingAdmitted',
 'firstWarningIndex','warningPointCount','firstObservedBThresholdIndex','firstObservedNetworkBrakeIndex',
 'firstWarningQualification','firstWarningGeometry','usefulPrethresholdModelOpportunity',
 'firstRetrospectiveOpportunityIndex','warningCohortOrGapStartCohort'
];

function quantile(values,q){
 if(!values.length)return null;
 const sorted=values.slice().sort((a,b)=>a-b);
 return sorted[Math.max(0,Math.ceil(q*sorted.length)-1)];
}
function distribution(values){
 const good=values.filter(finite);
 return {observed:good.length,missing:values.length-good.length,min:good.length?Math.min(...good):null,
  p50:quantile(good,.5),p90:quantile(good,.9),p99:quantile(good,.99),max:good.length?Math.max(...good):null};
}
function counts(values,names){
 const result=Object.fromEntries(names.map(name=>[name,0]));
 for(const value of values)result[names[value]??'unknown']=(result[names[value]??'unknown']??0)+1;
 return result;
}
function maskCounts(values){
 return Object.fromEntries(Object.entries(MASK).map(([name,bit])=>[name,values.filter(value=>(value&bit)!==0).length]));
}
function withinProject(relative){
 assert.equal(typeof relative,'string');
 const resolved=path.resolve(project,relative),rel=path.relative(project,resolved);
 assert.ok(rel&&!rel.startsWith('..'+path.sep)&&rel!=='..'&&!path.isAbsolute(rel),'Path must stay in the title worktree');
 return resolved;
}
function boundedErrors(errors){
 return {count:errors.length,sha256:sha(JSON.stringify(errors)),items:errors.slice(0,64),omitted:Math.max(0,errors.length-64)};
}
function holdBudget(frames,effective,p){
 return frames*(1000/p.logicalHz)*effective;
}
function rampBudget(frames,network,effective,p){
 if(frames<=0)return 0;
 const dt=1000/p.logicalHz,cap=p.networkScaleMaximum,slew=p.rampMsPerScaleUnit;
 const start=Math.max(network,effective);
 if(start>=cap)return frames*dt*cap;
 // Keep the end-observed phase floor until the rising network advice overtakes it.
 const plateau=Math.max(0,effective-network)*slew;
 const plateauFrames=plateau/(dt*effective);
 if(frames<=plateauFrames)return frames*dt*effective;
 const remaining=frames-plateauFrames;
 const rampFrames=(slew/dt)*Math.log(cap/start);
 if(remaining<=rampFrames)return plateau+slew*start*Math.expm1(remaining*dt/slew);
 return plateau+slew*(cap-start)+(remaining-rampFrames)*dt*cap;
}
function classifyDelay(lower,upper,hold,ramp){
 if(![lower,upper,hold,ramp].every(finite)||lower<0||upper<lower)return 0;
 if(upper<=hold)return 1;
 if(lower>hold&&upper<=ramp)return 2;
 if(lower>ramp)return 3;
 return 4;
}
function contracts(p){
 let checks=0;
 const near=(actual,expected)=>{assert.ok(Math.abs(actual-expected)<1e-9);++checks;};
 const dt=1000/p.logicalHz;
 near(rampBudget(0,1,1,p),0);
 near(holdBudget(4,1,p),4*dt);
 near(rampBudget(4,2,2,p),8*dt);
 near(rampBudget((p.rampMsPerScaleUnit/dt)*Math.log(2),1,1,p),50);
 near(rampBudget(1/(dt*1.02),1,1.02,p),1);
 assert.ok(rampBudget(4,1,1,p)>holdBudget(4,1,p));++checks;
 for(const [lo,hi,h,r,want] of [[5,10,16,30,1],[17,20,16,30,2],[31,35,16,30,3],[15,20,16,30,4],[null,20,16,30,0]]){
  assert.equal(classifyDelay(lo,hi,h,r),want);++checks;
 }
 near(quantile([1,2,3,4,5,6,7,8,9,10],.9),9);
 return {passed:true,checks,scope:'Deterministic model/interval contracts only, run before raw reads.'};
}
function interval(previous,current){
 return previous?.readValid&&current?.readValid&&current.readHi>=previous.readLo
  ?{lo:previous.readLo,hi:current.readHi,mid:(previous.readLo+current.readHi)/2}:null;
}
function lead(event,decisionAt){
 return event&&finite(decisionAt)?[event.lo-decisionAt,event.hi-decisionAt]:[null,null];
}
function upperBound(rows,at){
 let lo=0,hi=rows.length;
 while(lo<hi){const mid=(lo+hi)>>>1;if(rows[mid].at<=at)lo=mid+1;else hi=mid;}
 return lo;
}
function timelineContext(peer,started){
 const diag=peer.rafDiagnostics,life=[],control=[],errors=[];
 let prior=-Infinity;
 for(const [index,row] of array(peer.lifecycle).entries()){
  if(!Array.isArray(row)||!finite(row[0])||row[0]<prior){errors.push({kind:'lifecycle-order-or-shape',index});continue;}
  prior=row[0];life.push({at:row[0]-started,event:row[1],visible:row[2]==='visible'});
 }
 prior=-Infinity;let running=false,pause=null,epoch=0;
 for(const [index,row] of array(diag?.controls).entries()){
  if(!row||!finite(row.enteredAt)||row.enteredAt<prior){errors.push({kind:'control-order-or-shape',index});continue;}
  prior=row.enteredAt;
  if(row.name==='_th09_loop_start'){running=true;++epoch;}
  else if(row.name==='_th09_loop_stop')running=false;
  else if(row.name==='_th09_loop_pause')pause=finite(row.argument)?row.argument:null;
  else continue;
  control.push({at:row.enteredAt-started,name:row.name,argument:row.argument,running,pause,epoch});
 }
 return {errors,flags(from,to){
  let flags=0;
  if(!finite(from)||!finite(to)||to<from)return MASK.clockOrder;
  const li=upperBound(life,from)-1,ls=li>=0?life[li]:null;
  if(!ls||errors.some(error=>error.kind.startsWith('lifecycle')))flags|=MASK.lifecycleUnavailable;
  else if(!ls.visible||ls.event==='freeze'||ls.event==='pagehide')flags|=MASK.lifecycleNotVisible;
  for(let i=li+1;i<life.length&&life[i].at<=to;++i)
   if(!life[i].visible||life[i].event==='freeze'||life[i].event==='pagehide')flags|=MASK.lifecycleNotVisible;
  const ci=upperBound(control,from)-1,cs=ci>=0?control[ci]:null;
  if(!cs||cs.pause===null||errors.some(error=>error.kind.startsWith('control')))flags|=MASK.loopStateUnavailable;
  else if(!cs.running||cs.pause!==0)flags|=MASK.loopInterruptedOrPaused;
  for(let i=ci+1;i<control.length&&control[i].at<=to;++i)
   if(control[i].name!=='_th09_loop_pause'||control[i].argument!==0)flags|=MASK.loopInterruptedOrPaused;
  return flags;
 }};
}
function readPoints(peer,p){
 const m=peer.measure,samples=array(m?.samples),pacing=array(m?.networkPacing),errors=[];
 if(!finite(m?.started)||m.started<=0)errors.push({kind:'missing-page-clock-anchor'});
 if(samples.length<2||samples.length>p.maxSamplesPerPeer)errors.push({kind:'sample-count',count:samples.length});
 const byIndex=new Map();
 for(const [position,row] of pacing.entries()){
  if(!Array.isArray(row)||row.length!==7||!row.every(integer)||row[1]!==1||row[0]>=samples.length){
   errors.push({kind:'pacing-row-shape',position});continue;
  }
  if(byIndex.has(row[0]))errors.push({kind:'duplicate-pacing-index',position,index:row[0]});
  else byIndex.set(row[0],row);
 }
 for(let i=0;i<samples.length;++i)if(!byIndex.has(i))errors.push({kind:'missing-pacing-index',index:i});
 const frames=new Map(),duplicateFrames=new Set();
 for(const frame of array(peer.rafDiagnostics?.frames)){
  if(!integer(frame?.sampleBefore)||frame.sampleAfter!==frame.sampleBefore+1||!finite(frame.sampleAt))continue;
  if(frames.has(frame.sampleBefore))duplicateFrames.add(frame.sampleBefore);else frames.set(frame.sampleBefore,frame);
 }
 const context=timelineContext(peer,m?.started??0),points=[];
 for(let i=0;i<samples.length;++i){
  const s=samples[i],net=byIndex.get(i);
  if(!Array.isArray(s)||s.length!==14||!s.every(finite)||s[2]<0||!s.slice(3,13).every(integer)){
   errors.push({kind:'sample-shape',index:i});continue;
  }
  if(!net)continue;
  const [unused,version,B,n,e,F,A]=net,next=s[3],prev=points.at(-1);
  if(F<next||B!==Math.max(F-A,0)||B>p.historyFrames)errors.push({kind:'frontier-contract',index:i,F,A,B,next});
  if(n<1000||n>2000||e<980||e>2000||e>Math.max(n,1020)+1||(n>1000&&e<n-1))
   errors.push({kind:'scale-contract',index:i,n,e});
  if(prev&&(s[0]<=prev.marker||F<prev.F||A<prev.A||s[5]<prev.corrections||s[6]<prev.resimulated||s[7]<prev.captures))
   errors.push({kind:'nonmonotone-clock-frontier-or-counter',index:i});
  const frame=frames.get(i),marker=s[0],readLo=frame?frame.sampleAt-m.started:null,readHi=frame?.afterSampleAt-m.started;
  const readValid=!!frame&&!duplicateFrames.has(i)&&finite(readLo)&&finite(readHi)&&readHi>=readLo&&
   Math.abs(readLo-marker)<=p.clockEqualityToleranceMs;
  const request=integer(frame?.rafIndex)?array(peer.rafDiagnostics?.requests)[frame.rafIndex]:null;
  const rafWait=finite(request?.enteredAt)&&finite(request?.requestedAt)?request.enteredAt-request.requestedAt:null;
  const point={i,marker,readLo:readValid?readLo:null,readHi:readValid?readHi:null,readValid,F,A,B,
   Q:Math.max(0,p.historyFrames-B),Qfull:Math.max(0,A+p.historyFrames-F),R:F-next,next,ok:s[1],work:s[2],
   n,e,network:n/1000,effective:e/1000,corrections:s[5],resimulated:s[6],captures:s[7],
   phase:s[12],rafTimestamp:s[13],rafWait,sampleGap:prev?marker-prev.marker:null,readWidth:readValid?readHi-readLo:null,
   target:1+clamp((B-p.legacyOutstandingStart)/p.legacyOutstandingRampFrames,0,1),mask:0};
  if(!readValid)point.mask|=MASK.readBracketUnavailable;
  if(prev&&point.sampleGap>p.sampleGapCensorMs)point.mask|=MASK.sampleGap;
  if(point.work>p.nativeCallbackCensorMs)point.mask|=MASK.nativeWork;
  if(point.R>0||(prev&&point.resimulated>prev.resimulated))point.mask|=MASK.replayWorkOrDebt;
  if(prev&&point.corrections>prev.corrections)point.mask|=MASK.correctionStarted;
  if(finite(rafWait)&&rafWait>p.rafRequestWaitCensorMs)point.mask|=MASK.rafRequestWait;
  if(peer.rafDiagnostics?.valid!==true)point.mask|=MASK.rafDiagnosticInvalid;
  if(readValid&&point.readWidth>p.readBracketCensorMs)point.mask|=MASK.readBracketWide;
  if(readValid&&prev?.readValid&&readLo<prev.readLo)point.mask|=MASK.clockOrder;
  point.mask|=context.flags(prev?.readValid?prev.readLo:marker,readValid?readHi:marker);
  points.push(point);
 }
 return {points,errors,contextErrors:context.errors,sampleCount:samples.length,pacingCount:pacing.length,
  duplicateReadFrameIndices:[...duplicateFrames],frameReadCoverage:points.filter(point=>point.readValid).length};
}
function estimate(history,gap,point,p){
 const e={status:0,count:history.length,survivors:0,possible:0,definite:0,ageLo:null,ageHi:null,ageNominal:null,
  residual:null,residualLo:null,residualHi:null,p90:quantile(history.map(g=>g.nominal),p.conditionalQuantile),
  maximum:history.length?Math.max(...history.map(g=>g.nominal)):null,overdueNominal:false,overdueDefinite:false,
  oldestGap:history[0]?.id??null,newestGap:history.at(-1)?.id??null};
 if(gap.leftCensored)return e;
 if(!gap.startEvent||!point.readValid){e.status=1;return e;}
 e.ageLo=Math.max(0,point.readHi-gap.startEvent.hi);
 e.ageHi=Math.max(0,point.readHi-gap.startEvent.lo);
 e.ageNominal=Math.max(0,point.readHi-gap.startEvent.mid);
 const selected=history.filter(g=>g.nominal>e.ageNominal);
 e.survivors=selected.length;e.possible=history.filter(g=>g.upper>e.ageLo).length;
 e.definite=history.filter(g=>g.lower>e.ageHi).length;
 e.overdueNominal=history.length>0&&e.ageNominal>e.maximum;
 e.overdueDefinite=history.length>0&&e.ageLo>Math.max(...history.map(g=>g.upper));
 if(history.length<p.minimumHistoryGaps){e.status=2;return e;}
 if(selected.length<p.minimumConditionalSurvivors){e.status=3;return e;}
 e.status=4;
 e.residual=quantile(selected.map(g=>g.nominal-e.ageNominal),p.conditionalQuantile);
 e.residualLo=quantile(selected.map(g=>Math.max(0,g.lower-e.ageHi)),p.conditionalQuantile);
 e.residualHi=quantile(selected.map(g=>Math.max(0,g.upper-e.ageLo)),p.conditionalQuantile);
 return e;
}
function newGap(id,point,event,left){
 return {id,A:point.A,startIndex:point.i,startEvent:event,leftCensored:left,rightCensored:true,closeIndex:null,
  endEvent:null,closingDeltaA:null,lower:null,upper:null,nominal:null,mask:0,trainingAdmitted:false,warningIndices:[],
  firstTarget:null,firstBrake:null,thresholdEvent:null,brakeEvent:null,firstRetrospectiveOpportunity:null};
}
function closeGap(gap,point,event,deltaA,history,p){
 gap.closeIndex=point.i;gap.endEvent=event;gap.closingDeltaA=deltaA;gap.rightCensored=false;gap.mask|=point.mask;
 if(gap.startEvent&&event){
  gap.lower=Math.max(0,event.lo-gap.startEvent.hi);gap.upper=Math.max(0,event.hi-gap.startEvent.lo);
  gap.nominal=Math.max(0,event.mid-gap.startEvent.mid);
 }
 gap.trainingAdmitted=!gap.leftCensored&&gap.mask===0&&finite(gap.lower)&&finite(gap.upper)&&finite(gap.nominal)&&gap.nominal>0;
 if(gap.trainingAdmitted){
  history.push({id:gap.id,lower:gap.lower,upper:gap.upper,nominal:gap.nominal,deltaA});
  if(history.length>p.historyGaps)history.shift();
 }
}
function spans(points,accept,label){
 const result=[];let span=null;
 for(let i=0;i<points.length;++i){
  const point=points[i],active=accept(point);
  if(active&&!span){span={firstIndex:i,endIndexExclusive:null,firstObservedMs:point.readHi??point.marker,
   onsetInterval:i?interval(points[i-1],point):null,leftCensored:i===0,rightCensored:true,maxB:point.B};}
  if(active&&span)span.maxB=Math.max(span.maxB,point.B);
  if(!active&&span){span.endIndexExclusive=i;span.endObservedMs=point.readHi??point.marker;span.rightCensored=false;result.push(span);span=null;}
 }
 if(span){span.endIndexExclusive=points.length;span.endObservedMs=points.at(-1)?.readHi??points.at(-1)?.marker??null;result.push(span);}
 return {kind:label,spans:result,scope:'Observed end-of-callback state. Onsets are observation intervals; not exact Pump target/application times.'};
}
function analyzePeer(peer,side,p){
 const read=readPoints(peer,p);
 const sourceValidation={valid:read.errors.length===0,errors:boundedErrors(read.errors),
  sampleCount:read.sampleCount,pacingCount:read.pacingCount,validReadBracketCount:read.frameReadCoverage,
  duplicateReadFrameIndices:read.duplicateReadFrameIndices,contextErrors:boundedErrors(read.contextErrors)};
 if(!sourceValidation.valid)return {side,status:'source-validation-failed',sourceValidation};
 const points=read.points,history=[],gaps=[];
 const start=points.findIndex(point=>point.F>=p.formalStartFrontier),end=points.findIndex(point=>point.F>=p.formalEndFrontier);
 const boundariesAvailable=start>=0&&end>=start;
 const cohort=index=>!boundariesAvailable?3:index<start?0:index<end?1:2;
 let gap=null;
 for(let i=0;i<points.length;++i){
  const point=points[i],prev=points[i-1];
  if(!gap){gap=newGap(0,point,null,true);gaps.push(gap);}
  else if(point.A>prev.A){
   const event=interval(prev,point);
   closeGap(gap,point,event,point.A-prev.A,history,p);
   gap=newGap(gaps.length,point,event,false);gaps.push(gap);
  }
  gap.mask|=point.mask;point.gapId=gap.id;point.gapMask=gap.mask;point.cohort=cohort(i);
  if(point.target>1&&gap.firstTarget===null){gap.firstTarget=i;gap.thresholdEvent=i?interval(prev,point):null;}
  if(point.n>1000&&gap.firstBrake===null){gap.firstBrake=i;gap.brakeEvent=i&&prev.n<=1000?interval(prev,point):null;}
  point.estimate=estimate(history,gap,point,p);
  point.holdQ=holdBudget(point.Q,point.effective,p);point.rampQ=rampBudget(point.Q,point.network,point.effective,p);
  point.holdQfull=holdBudget(point.Qfull,point.effective,p);point.rampQfull=rampBudget(point.Qfull,point.network,point.effective,p);
  const safe=Math.max(0,point.Qfull-p.safeReserveFrames);
  point.holdSafe=holdBudget(safe,point.effective,p);point.rampSafe=rampBudget(safe,point.network,point.effective,p);
  point.prebrake=point.B>0&&point.A<point.F&&point.Q>p.safeReserveFrames&&point.target===1&&point.n===1000&&
   gap.firstTarget===null&&gap.firstBrake===null;
  point.warning=0;
  if(point.prebrake&&gap.mask===0&&point.readValid){
   const e=point.estimate;
   if(e.status===4&&e.residual>point.holdSafe)point.warning=e.residual<=point.rampSafe?1:2;
   else if(e.status===3&&finite(e.p90)&&e.ageNominal>e.p90)point.warning=3;
  }
  if(point.warning)gap.warningIndices.push(i);
 }
 // Closure classification cannot feed back into estimates or warning decisions.
 for(const point of points){
  const g=gaps[point.gapId],nextEvent=g.endEvent;
  const delay=nextEvent&&point.readValid?[Math.max(0,nextEvent.lo-point.readHi),Math.max(0,nextEvent.hi-point.readHi)]:[null,null];
  point.delayLo=delay[0];point.delayHi=delay[1];
  point.geometry=classifyDelay(...delay,point.holdSafe,point.rampSafe);
  point.qualification=g.rightCensored?3:(!point.readValid||!nextEvent)?4:g.mask!==0?2:1;
  point.thresholdLead=lead(g.thresholdEvent,point.readHi);
  point.brakeLead=lead(g.brakeEvent,point.readHi);
  point.positiveLeadOpportunity=point.prebrake&&point.estimate.count>=p.minimumHistoryGaps&&!g.leftCensored&&
   point.qualification===1&&point.geometry===2&&
   g.firstTarget!==null&&point.i<g.firstTarget&&finite(point.thresholdLead[0])&&point.thresholdLead[0]>0;
  point.unwarnedOpportunity=point.positiveLeadOpportunity&&point.warning===0;
  if(point.positiveLeadOpportunity&&g.firstRetrospectiveOpportunity===null)g.firstRetrospectiveOpportunity=point.i;
 }
 const summarize=(selected,selectedGaps)=>{
  const firstWarnings=selectedGaps.filter(g=>g.warningIndices.length).map(g=>points[g.warningIndices[0]]);
  const qualified=firstWarnings.filter(point=>point.qualification===1);
  return {samples:selected.length,gapsAssigned:selectedGaps.length,
   stateCounts:{actualAlreadyCoversFrontier:selected.filter(point=>point.A>=point.F).length,
    noPredictionRoom:selected.filter(point=>point.Q===0).length,noOneFrameReserve:selected.filter(point=>point.Q<=p.safeReserveFrames).length,
    targetFromObservedBAboveOne:selected.filter(point=>point.target>1).length,observedNetworkScaleAboveOne:selected.filter(point=>point.n>1000).length,
    prebrakeCandidates:selected.filter(point=>point.prebrake).length},
   pointWarnings:counts(selected.map(point=>point.warning),WARNING),
   estimateStates:counts(selected.map(point=>point.estimate.status),ESTIMATE),
   localObservationFlags:maskCounts(selected.map(point=>point.mask)),
   firstWarningPerGap:{count:firstWarnings.length,byWarning:counts(firstWarnings.map(point=>point.warning),WARNING),
    closureQualification:counts(firstWarnings.map(point=>point.qualification),QUALIFICATION),
    qualifiedGeometry:counts(qualified.map(point=>point.geometry),GEOMETRY),
    usefulPrethresholdModelOpportunities:firstWarnings.filter(point=>point.positiveLeadOpportunity).length,
    unnecessaryWarningProxy:qualified.filter(point=>point.geometry===1).length,
    originalThresholdLeadLowerMs:distribution(firstWarnings.map(point=>point.thresholdLead[0])),
    originalNetworkBrakeLeadLowerMs:distribution(firstWarnings.map(point=>point.brakeLead[0]))},
   gapAccounting:{closed:selectedGaps.filter(g=>!g.rightCensored).length,rightCensored:selectedGaps.filter(g=>g.rightCensored).length,
    leftCensored:selectedGaps.filter(g=>g.leftCensored).length,admittedToTraining:selectedGaps.filter(g=>g.trainingAdmitted).length,
    excludedFromTraining:selectedGaps.filter(g=>!g.trainingAdmitted).length,
    wholeGapFlags:maskCounts(selectedGaps.map(g=>g.mask)),
    noWarnings:selectedGaps.filter(g=>!g.warningIndices.length).length,
    retrospectiveModelOpportunityWithNoWarning:selectedGaps.filter(g=>!g.warningIndices.length&&g.firstRetrospectiveOpportunity!==null).length},
   distributions:{B:distribution(selected.map(point=>point.B)),Q:distribution(selected.map(point=>point.Q)),
    R:distribution(selected.map(point=>point.R)),safeHoldMs:distribution(selected.map(point=>point.holdSafe)),
    safeRampMs:distribution(selected.map(point=>point.rampSafe)),
    additionalSafeModelMs:distribution(selected.map(point=>point.rampSafe-point.holdSafe)),
    observationGapMs:distribution(selected.map(point=>point.sampleGap)),
    readBracketMs:distribution(selected.map(point=>point.readWidth)),
    nominalAgeMs:distribution(selected.map(point=>point.estimate.ageNominal))}};
 };
 const assigned=g=>cohort(g.warningIndices[0]??g.startIndex);
 const rows=points.map(point=>{
  const e=point.estimate,g=gaps[point.gapId];
  return [point.i,point.marker,point.readLo,point.readHi,point.F,point.A,point.B,point.Q,point.Qfull,point.R,point.next,point.ok,point.work,
   point.n,point.e,point.target,point.gapId,e.ageLo,e.ageHi,e.ageNominal,e.count,e.survivors,e.possible,e.definite,e.status,
   e.residual,e.residualLo,e.residualHi,e.p90,e.maximum,+e.overdueNominal,+e.overdueDefinite,
   point.holdQ,point.rampQ,point.holdQfull,point.rampQfull,point.holdSafe,point.rampSafe,
   point.mask,point.gapMask,+point.prebrake,point.warning,point.qualification,point.geometry,g.closeIndex,
   point.delayLo,point.delayHi,...point.thresholdLead,...point.brakeLead,+point.positiveLeadOpportunity,+point.unwarnedOpportunity,
   point.cohort,point.rafWait,point.sampleGap,point.readWidth,e.oldestGap,e.newestGap];
 });
 for(const row of rows)assert.equal(row.length,COLUMNS.length);
 const gapRows=gaps.map(g=>{
  const first=g.warningIndices.length?points[g.warningIndices[0]]:null;
  return [g.id,g.A,g.startIndex,g.closeIndex,g.startEvent?.lo??null,g.startEvent?.hi??null,
   g.endEvent?.lo??null,g.endEvent?.hi??null,g.lower,g.upper,g.nominal,g.closingDeltaA,
   +g.leftCensored,+g.rightCensored,g.mask,+g.trainingAdmitted,first?.i??null,g.warningIndices.length,
   g.firstTarget,g.firstBrake,first?.qualification??null,first?.geometry??null,first?+first.positiveLeadOpportunity:0,
   g.firstRetrospectiveOpportunity,assigned(g)];
 });
 for(const row of gapRows)assert.equal(row.length,GAP_COLUMNS.length);
 return {side,status:'analyzed',sourceValidation,
  clock:{name:'Page performance.now()',timesRelativeToMeasureStarted:true,measureStarted:peer.measure.started,
   timeOrigin:null,timeOriginAvailability:'The selected old RAF snapshots do not export timeOrigin; no cross-clock subtraction is performed.'},
  boundary:{complete:boundariesAvailable,startSampleIndex:start<0?null:start,endSampleIndexExclusive:end<0?null:end,
   bodyRange:boundariesAvailable?[start,end]:null,requestedFrontierRange:[p.formalStartFrontier,p.formalEndFrontier],
   rule:'First native F crossings; full original order and terminal/tail remain in point rows.'},
  unmeasured:{pumpTiming:true,receiveBudgetExit:true,remainingQueuePerPump:true,browserPerInputArrival:true,remoteProducerTiming:true,
   cause:'Information-source-unidentified; unflagged observed fields do not isolate a network cause.'},
  sourceRafState:{valid:peer.rafDiagnostics?.valid??null,clock:peer.rafDiagnostics?.clock??null,
   overflow:peer.rafDiagnostics?.overflow??null,counts:peer.rafDiagnostics?.counts??null},
  evidenceQualificationScope:'Whole-report RAF validity and timeline structure/order errors are retrospective evidence gates, not reconstructed online signals. Future A gap durations do not feed forward warning calculations.',
  summaries:{all:summarize(points,gaps),...Object.fromEntries(COHORT.map((name,id)=>[name,
   summarize(points.filter(point=>point.cohort===id),gaps.filter(g=>assigned(g)===id))]))},
  originalBrakeObservations:{thresholdFromObservedB:spans(points,point=>point.target>1,'B threshold'),
   networkScaleAboveOne:spans(points,point=>point.n>1000,'Network scale')},
  finalOpenGapId:gaps.at(-1)?.id??null,finalEligibleTrainingGapIds:history.map(g=>g.id),
  pointRows:rows,gapRows};
}
function analyzeInput(input,p,plan){
 const full=withinProject(input.path),meta={expected:{path:input.path,bytes:input.bytes,sha256:input.sha256}};
 try{
  const stat=fs.statSync(full);assert.ok(stat.isFile());assert.ok(stat.size<=p.maxInputBytes,'Raw exceeds declared bound');
  const bytes=fs.readFileSync(full);meta.actual={bytes:bytes.length,sha256:sha(bytes)};
  assert.equal(bytes.length,input.bytes,'Raw size differs from predeclared input');
  assert.equal(meta.actual.sha256,input.sha256,'Raw SHA differs from predeclared input');
  // Hash/size validation precedes JSON parsing and all sample inspection.
  const raw=JSON.parse(bytes.toString('utf8')),parameterErrors=[];
  for(const [key,expected] of Object.entries(input.expectedParameters))
   if(raw.parameters?.[key]!==expected)parameterErrors.push({key,expected,actual:raw.parameters?.[key]??null});
  if(raw.identity?.wasm!==plan.sourceContract.wasmSha256)
   parameterErrors.push({key:'identity.wasm',expected:plan.sourceContract.wasmSha256,actual:raw.identity?.wasm??null});
  if(!Array.isArray(raw.results)||raw.results.length!==2)parameterErrors.push({key:'results',expected:'two complete peer results'});
  if(parameterErrors.length)return {id:input.id,status:'input-contract-invalid',integrity:meta,errors:parameterErrors};
  const peers=raw.results.map((peer,side)=>analyzePeer(peer,side,p));
  return {id:input.id,status:peers.every(peer=>peer.status==='analyzed')?'analyzed':'peer-validation-failed',integrity:meta,
   runtimePassed:raw.passed??null,parameters:raw.parameters,
   sourceIdentity:{wasm:raw.identity.wasm,harness:raw.identity.harness??null,loader:raw.identity.loader??null},
   peers};
 }catch(error){
  return {id:input.id,status:'input-unavailable-or-invalid',integrity:meta,error:String(error?.message??error)};
 }
}
function main(){
 const args=process.argv.slice(2);assert.ok(args.length<=2,'Usage: node analyze-information-pacing-shadow.mjs [plan.json [new-evidence.json]]');
 const planPath=args[0]?path.resolve(args[0]):withinProject('docs/multiplayer/rollback-information-pacing-shadow-plan.json');
 const planBytes=fs.readFileSync(planPath),plan=JSON.parse(planBytes.toString('utf8'));
 assert.equal(plan.schema,'th09-information-pacing-shadow-plan-v1');assert.equal(plan.planId,'information-budget-shadow-01');
 assert.deepEqual(plan.policy,LOCKED_POLICY,'Policy changed: retain v1 evidence and create a new preregistration instead of retuning this run');
 assert.deepEqual(plan.inputs,LOCKED_INPUTS,'Input set changed after declaration');
 const out=args[1]?path.resolve(args[1]):withinProject(plan.output.path);
 assert.ok(!fs.existsSync(out),'Evidence is create-only; output already exists');
 assert.ok(!plan.inputs.some(input=>withinProject(input.path)===out)&&out!==planPath&&out!==self,'Output overlaps an input');
 const preReadIdentity={plan:{path:planPath,bytes:planBytes.length,sha256:sha(planBytes)},
  analyzer:{path:self,sha256:sha(fs.readFileSync(self))},expectedInputs:plan.inputs.map(input=>({id:input.id,path:input.path,bytes:input.bytes,sha256:input.sha256}))};
 const contractChecks=contracts(plan.policy);
 const startedAt=new Date().toISOString();
 const results=[];
 for(const input of plan.inputs)results.push(analyzeInput(input,plan.policy,plan));
 const evidence={schema:'th09-information-pacing-shadow-evidence-v1',planId:plan.planId,
  execution:{startedAt,completedAt:new Date().toISOString(),status:'offline-analysis-complete',rawInputIntegrityCheckedBeforeParsing:true},
  preReadIdentity,originalPlan:plan,contractChecks,
  analysisValid:results.every(result=>result.status==='analyzed'),
  interpretation:'A preregistered observational budget model. No counterfactual FPS/latency, no network-root-cause label, no game or production control change.',
  dictionaries:{pointColumns:COLUMNS,gapColumns:GAP_COLUMNS,estimateStatus:ESTIMATE,warningCode:WARNING,
   closureQualification:QUALIFICATION,geometricNextProgressClass:GEOMETRY,cohort:COHORT,localObservationMask:MASK},
  results};
 const output=JSON.stringify(evidence)+'\n',outputBytes=Buffer.byteLength(output);
 assert.ok(outputBytes<=plan.policy.maxOutputBytes,'Evidence exceeds declared size bound; no partial/truncated result written');
 fs.writeFileSync(out,output,{flag:'wx'});
 console.log(JSON.stringify({output:out,bytes:outputBytes,sha256:sha(output),analysisValid:evidence.analysisValid,
  contractChecks:contractChecks.checks,cases:results.map(result=>({id:result.id,status:result.status,
   peers:result.peers?.map(peer=>({side:peer.side,status:peer.status,summary:peer.summaries?.all?.firstWarningPerGap??null}))??[]}))}));
 if(!evidence.analysisValid)process.exitCode=1;
}
if(process.argv[1]&&path.resolve(process.argv[1])===self)main();
export {rampBudget,holdBudget,classifyDelay,estimate,analyzePeer};

