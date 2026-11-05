// Offline test analysis only. This file is never imported by the game runtime.
// Usage: node portable/multiplayer/analyze-motion-cadence.mjs [options] 1-frontier.json ...
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';

const HELP=`Usage: node portable/multiplayer/analyze-motion-cadence.mjs [options] <1-frontier.json> ...
  --output <file>          Save full JSON; stdout contains compact JSON lines.
  --edge-ms <ms>           Trim each end of the common measured interval (default 250).
  --epsilon <pixels>       Position equality tolerance (default 0.0001).
  --large-step-px <pixels> Descriptive large displacement threshold (default 8).
  --large-step-factor <n>  Excess over endpoint-model displacement (default 2.5).
  --window-rows <n>        Original rows around the strongest signal, 1..20 (default 20).
  --help

Signals are descriptive, not a stutter verdict. Network waiting and zero logical
progress remain in the body. Touch release does not stop bullet simulation.
Only a known end/stopped marker and declared measurement edges are filtered.
`;
const finite=value=>typeof value==='number'&&Number.isFinite(value);
const at=(array,index)=>Array.isArray(array)&&finite(array[index])?array[index]:null;
const flagged=value=>value===true||value===1;
const vector=(array,index)=>[at(array,index),at(array,index+1)];
const point=p=>p.every(finite);
const subtract=(a,b)=>[a[0]-b[0],a[1]-b[1]];
const length=p=>Math.hypot(p[0],p[1]);
const distance=(a,b)=>length(subtract(a,b));
const dot=(a,b)=>a[0]*b[0]+a[1]*b[1];
const round=value=>finite(value)?Math.round(value*1e6)/1e6:null;
const association=()=>({withCorrections:0,withoutCorrections:0,unknown:0});
const corrected=(a,b)=>a&&a.generation===b.generation&&finite(a.corrections)&&
  finite(b.corrections)&&b.corrections>=a.corrections?b.corrections-a.corrections:null;
const bucket=delta=>delta===null?'unknown':delta>0?'withCorrections':'withoutCorrections';
const waiting=sample=>flagged(sample.raw.net?.[9])||flagged(sample.raw.cadence?.[2]);
const evidenceNames=['correctionYield','correctionWork','admissionWait','correctionAndAdmission','pendingUnknown','noneObserved','unknown'];
const evidenceCounts=()=>Object.fromEntries(evidenceNames.map(name=>[name,0]));
const observedFlag=value=>value===true||value===1?true:value===false||value===0?false:null;
function counterDelta(a,b,key){
  return a&&a.generation===b.generation&&finite(a[key])&&finite(b[key])&&b[key]>=a[key]?b[key]-a[key]:null;
}
function waitEvidence(a,b){
  const admissionWait=observedFlag(b.raw.net?.[9]),retryPending=observedFlag(b.raw.cadence?.[2]);
  const correcting=observedFlag(b.correcting),callbackYielded=observedFlag(b.budgetYielded);
  const correctionYieldDelta=counterDelta(a,b,'correctionYields'),resimulationDelta=counterDelta(a,b,'resimulated');
  const correctionStartDelta=corrected(a,b);
  const yielded=callbackYielded===true||correctionYieldDelta>0;
  const correction=yielded||correcting===true||resimulationDelta>0||correctionStartDelta>0;
  const complete=[admissionWait,retryPending,correcting,callbackYielded].every(value=>value!==null);
  const category=correction&&admissionWait===true?'correctionAndAdmission':
    yielded?'correctionYield':correction?'correctionWork':admissionWait===true?'admissionWait':
    retryPending===true?'pendingUnknown':complete?'noneObserved':'unknown';
  return {category,admissionWait,retryPending,correcting,callbackYielded,
    intervalScale:at(b.raw.cadence,0)??at(b.raw.pacing,0),missingRemoteInputs:at(b.raw.cadence,1),
    correctionYieldDelta,resimulationDelta,correctionStartDelta,flagsComplete:complete};
}
function distribution(values){
  const sorted=values.filter(finite).sort((a,b)=>a-b),n=sorted.length;
  return n?{count:n,min:round(sorted[0]),p50:round(sorted[Math.floor((n-1)*.5)]),
    p95:round(sorted[Math.floor((n-1)*.95)]),max:round(sorted[n-1])}:{count:0,min:null,p50:null,p95:null,max:null};
}
function options(argv){
  const result={inputs:[],output:null,edgeMs:250,epsilon:.0001,largeStepPx:8,largeStepFactor:2.5,windowRows:20};
  const names={'--output':'output','--edge-ms':'edgeMs','--epsilon':'epsilon',
    '--large-step-px':'largeStepPx','--large-step-factor':'largeStepFactor','--window-rows':'windowRows'};
  let positional=false;
  for(let index=0;index<argv.length;++index){
    const arg=argv[index];
    if(positional){result.inputs.push(resolve(arg));continue;}
    if(arg==='--'){positional=true;continue;}
    if(arg==='--help'||arg==='-h'){result.help=true;continue;}
    if(!arg.startsWith('--')){result.inputs.push(resolve(arg));continue;}
    const equals=arg.indexOf('='),name=equals<0?arg:arg.slice(0,equals),key=names[name];
    if(!key)throw Error('Unknown option '+name);
    const value=equals<0?argv[++index]:arg.slice(equals+1);
    if(value===undefined||value===''||value.startsWith('--'))throw Error('Missing value for '+name);
    result[key]=key==='output'?resolve(value):Number(value);
  }
  if(result.help)return result;
  if(!result.inputs.length)throw Error('Provide at least one 1-frontier.json file. Use --help.');
  for(const key of ['edgeMs','epsilon','largeStepPx','largeStepFactor','windowRows'])
    if(!finite(result[key])||result[key]<0)throw Error('Invalid '+key);
  if(result.largeStepPx<=0||result.largeStepFactor<=1)throw Error('Large-step thresholds must be > 0 pixels and > 1 factor.');
  if(!Number.isInteger(result.windowRows)||result.windowRows<1||result.windowRows>20)throw Error('window-rows must be an integer from 1 to 20.');
  const pathKey=path=>process.platform==='win32'?path.toLowerCase():path;
  if(result.output&&result.inputs.some(path=>pathKey(path)===pathKey(result.output)))throw Error('--output must not overwrite an input file.');
  return result;
}
function readReport(path){
  const text=readFileSync(path,'utf8').replace(/^\uFEFF/,'');
  let report,nonFiniteTokens=0;
  try{report=JSON.parse(text);}catch(original){
    // Python json.dumps may emit Infinity for unused touch min/max values.
    // Replace only bare non-finite tokens; quoted strings remain unchanged.
    const normalized=text.replace(/("(?:[^"\\]|\\.)*")|(?:-?Infinity|NaN)(?=\s*[,}\]])/g,
      (match,quoted)=>{if(quoted)return quoted;++nonFiniteTokens;return 'null';});
    if(!nonFiniteTokens)throw original;
    report=JSON.parse(normalized);
  }
  if(!Array.isArray(report.peers))throw Error(path+': expected report.peers[].rows');
  return {report,nonFiniteTokens};
}
function decode(raw,index,origin){
  const m=raw.motion,o=84;
  // During a yielded correction net[3] is a historical replay cursor. Only
  // the persistent frontier can select the logical end of the measurement.
  // Pixel samples still come exclusively from the probe's committed snapshot.
  const logicalFrame=finite(raw.logicalFrame)?raw.logicalFrame:at(raw.progress,0)??at(raw.net,3);
  const frameSource=finite(raw.logicalFrame)?'logicalFrame':at(raw.progress,0)!==null?'progress[0]':'net[3]';
  const sample={raw,index,time:finite(raw.time)?raw.time:null,
    epoch:finite(origin)&&finite(raw.time)?origin+raw.time:null,
    frame:logicalFrame,frameSource,cursor:at(raw.net,3),
    correcting:at(raw.progress,2),budgetYielded:at(raw.progress,3),correctionYields:at(raw.progress,4),
    generation:at(raw.net,10),nativeMode:at(raw.net,14),corrections:at(raw.driver,3),resimulated:at(raw.driver,4),
    commit:at(m,1),draw:at(m,3),calculationEpoch:at(m,4),presentationAlpha:at(m,5),alpha:at(m,6),
    renderOnly:at(m,7),interpolationActive:at(m,8),excluded:null,
    supported:Array.isArray(m)&&m.length>=108&&m[0]===1&&m[11]===24};
  const b={valid:at(m,o),kind:at(m,o+1),slot:at(m,o+2),life:at(m,o+3),
    age:at(m,o+4),state:at(m,o+5),script:at(m,o+6),sprite:at(m,o+7),
    auth:vector(m,o+8),previous:vector(m,o+10),previousAge:at(m,o+12),
    previousValid:at(m,o+13)===1,vm:vector(m,o+14),center:vector(m,o+16),
    shake:vector(m,o+18),quads:at(m,o+20),vertexAlpha:at(m,o+22)};
  b.usable=sample.supported&&b.valid===1&&b.kind===2&&Number.isInteger(b.slot)&&
    b.slot>=0&&b.slot<1537&&finite(b.life)&&finite(b.age)&&finite(b.state)&&finite(b.script)&&
    b.quads===1&&[b.auth,b.vm,b.center,b.shake].every(point)&&m[9]===b.slot&&m[10]===b.life;
  b.endpoints=b.previousValid&&point(b.previous)&&finite(b.previousAge)&&finite(sample.alpha)&&
    sample.alpha>=0&&sample.alpha<=1;
  b.span=b.endpoints?b.age-b.previousAge:null;
  b.phase=b.endpoints?b.previousAge+b.span*sample.alpha:null;
  b.model=b.endpoints?b.previous.map((value,axis)=>value+(b.auth[axis]-value)*sample.alpha):null;
  b.unshaken=point(b.center)&&point(b.shake)?subtract(b.center,b.shake):null;
  sample.bullet=b;
  return sample;
}
function bounds(peers,report){
  const starts=[],ends=[],stops=[];
  let synchronized=peers.length>0;
  for(const peer of peers){
    const timed=peer.samples.filter(sample=>finite(sample.time));
    peer.first=timed[0]?.time??null;peer.last=timed.at(-1)?.time??null;
    peer.target=finite(peer.raw.end)?peer.raw.end:finite(report.frames)?report.frames:null;
    peer.stop=timed.find(sample=>flagged(sample.raw.stopped)||
      (finite(peer.target)&&finite(sample.frame)&&sample.frame>=peer.target))?.time??null;
    if(!finite(peer.origin)||!finite(peer.first)||!finite(peer.last))synchronized=false;
    else{
      starts.push(peer.origin+peer.first);ends.push(peer.origin+peer.last);
      if(finite(peer.stop))stops.push(peer.origin+peer.stop);
    }
  }
  return {synchronized,start:synchronized?Math.max(...starts):null,
    end:synchronized?Math.min(...ends):null,stop:synchronized&&stops.length?Math.min(...stops):null};
}
function excluded(sample,peer,shared,settings){
  if(!finite(sample.time))return 'invalidTimestamp';
  const time=shared.synchronized?sample.epoch:sample.time;
  const start=shared.synchronized?shared.start:peer.first,end=shared.synchronized?shared.end:peer.last;
  const stop=shared.synchronized?shared.stop:peer.stop;
  if(flagged(sample.raw.stopped)||(finite(stop)&&time>=stop)||
    (finite(peer.target)&&finite(sample.frame)&&sample.frame>=peer.target))return 'stopped';
  if(flagged(sample.raw.edge)||time<start+settings.edgeMs||time>end-settings.edgeMs)return 'edge';
  if(!finite(sample.frame)||!Number.isInteger(sample.generation))return 'missingNetIdentity';
  if(!sample.supported)return 'missingOrUnsupportedMotion';
  if(!finite(sample.commit)||sample.commit<1)return 'noCommittedProbe';
  return null;
}
function sameIdentity(a,b){
  return a.generation===b.generation&&a.raw.scene===b.raw.scene&&
    ['slot','life','state','script'].every(key=>a.bullet[key]===b.bullet[key]);
}
function aligned(a,b,cosine,epsilon){
  const product=length(a)*length(b);
  return product>epsilon*epsilon&&dot(a,b)/product>=cosine;
}
function velocity(b){
  return b.endpoints&&b.span>0?subtract(b.auth,b.previous).map(value=>value/b.span):null;
}
function endpointKind(sample,mode){
  if(sample.alpha!==1)return null;
  const high=observedFlag(sample.raw.cadence?.[5]);
  const semantic=sample.renderOnly===0&&sample.interpolationActive===0&&sample.presentationAlpha===1;
  if(high===true&&semantic&&at(sample.raw.cadence,4)>0&&(sample.nativeMode??mode)===0&&
     sample.raw.scene===2&&sample.raw.loading===0)
    return 'highRefreshSemanticForwardBudgetCandidate';
  if(high===true&&semantic)return 'highRefreshSemanticEndpoint';
  if(sample.renderOnly===1)return 'readonlyAlpha1';
  return 'otherAlpha1';
}
function analyseEndpointHolds(peer,segments,settings,consider){
  const result={observedStablePairMs:0,observedStableCommitPairs:0,changedCenterCommitPairs:0,alpha1EndpointSamples:0,highRefreshSemanticForwardBudgetCandidates:0,
    runs:0,heldCommitPairs:0,censoredRuns:0,alpha1HeldCommitPairs:0,
    gateDisabledReadonlyAlpha1HeldCommitPairs:0,redrawnHeldCommitPairs:0,reusedDrawHeldCommitPairs:0,
    unknownDrawHeldCommitPairs:0,sameCalculationEpochHeldCommitPairs:0,changedCalculationEpochHeldCommitPairs:0,
    unknownCalculationEpochHeldCommitPairs:0,forwardProgressHeldCommitPairs:0,authoritativeMovedHeldCommitPairs:0,
    missingEndpointModelHeldCommitPairs:0,intervalScaleAboveOneHeldCommitPairs:0,missingRemoteInputHeldCommitPairs:0,
    observedWaitEvidence:evidenceCounts(),byEndpointKind:{},examples:[]};
  const durations=[],releaseDelays=[],releaseSteps=[],candidateDurations=[],candidateReleaseDelays=[];
  const mode=peer.raw.timing?.native?.mode??null;
  const finish=(run,release=null)=>{
    if(!run)return;
    const heldMs=run.last.time-run.start.time,releaseMs=release?release.time-run.start.time:null;
    const releaseStep=release?distance(release.bullet.unshaken,run.start.bullet.unshaken):null;
    ++result.runs;result.heldCommitPairs+=run.pairs;if(!release)++result.censoredRuns;
    durations.push(heldMs);if(release){releaseDelays.push(releaseMs);releaseSteps.push(releaseStep);}
    const kind=result.byEndpointKind[run.kind]??={runs:0,heldCommitPairs:0,totalObservedHeldMs:0,releasedRuns:0};
    ++kind.runs;kind.heldCommitPairs+=run.pairs;kind.totalObservedHeldMs+=heldMs;if(release)++kind.releasedRuns;
    if(run.kind==='highRefreshSemanticForwardBudgetCandidate'){
      candidateDurations.push(heldMs);if(release)candidateReleaseDelays.push(releaseMs);
    }
    const example={endpointRow:run.start.index,lastHeldRow:run.last.index,releaseRow:release?.index??null,
      endpointKind:run.kind,budgetCauseProven:false,endpointTimeMs:run.start.time,lastHeldTimeMs:run.last.time,
      observedHeldMs:heldMs,releaseDelayMs:releaseMs,releaseStepPx:releaseStep,heldCommitPairs:run.pairs,
      observedWaitEvidence:run.evidence,endpointEvidence:run.endpointEvidence,
      generation:run.start.generation,scene:run.start.raw.scene,slot:run.start.bullet.slot,
      life:run.start.bullet.life,state:run.start.bullet.state,script:run.start.bullet.script,sprite:run.start.bullet.sprite};
    result.examples.push(example);
    result.examples.sort((a,b)=>b.observedHeldMs-a.observedHeldMs);
    if(result.examples.length>8)result.examples.length=8;
    consider(peer,run.last,{signal:'alpha1EndpointHeldAcrossCommits',details:example,priority:2,score:heldMs});
  };
  for(const segment of segments){
    let previous=null,seed=null,run=null;
    for(const sample of segment){
      const kind=endpointKind(sample,mode);
      if(kind)++result.alpha1EndpointSamples;
      if(kind==='highRefreshSemanticForwardBudgetCandidate')++result.highRefreshSemanticForwardBudgetCandidates;
      const sameSprite=previous&&previous.bullet.sprite===sample.bullet.sprite;
      if(sameSprite){
        ++result.observedStableCommitPairs;result.observedStablePairMs+=sample.time-previous.time;
        if(distance(previous.bullet.unshaken,sample.bullet.unshaken)>settings.epsilon)++result.changedCenterCommitPairs;
      }
      else {finish(run);run=null;seed=null;}
      if(sameSprite&&seed&&distance(seed.sample.bullet.unshaken,sample.bullet.unshaken)<=settings.epsilon){
        if(!run)run={start:seed.sample,last:seed.sample,kind:seed.kind,pairs:0,
          evidence:evidenceCounts(),endpointEvidence:seed.evidence};
        const evidence=waitEvidence(previous,sample);
        ++run.pairs;run.last=sample;++run.evidence[evidence.category];++result.observedWaitEvidence[evidence.category];
        if(evidence.intervalScale>1+1e-6)++result.intervalScaleAboveOneHeldCommitPairs;
        if(evidence.missingRemoteInputs>0)++result.missingRemoteInputHeldCommitPairs;
        if(sample.alpha===1)++result.alpha1HeldCommitPairs;
        if(sample.alpha===1&&sample.renderOnly===1&&sample.interpolationActive===0)
          ++result.gateDisabledReadonlyAlpha1HeldCommitPairs;
        if(finite(previous.draw)&&finite(sample.draw)&&sample.draw>previous.draw)++result.redrawnHeldCommitPairs;
        else if(finite(previous.draw)&&sample.draw===previous.draw)++result.reusedDrawHeldCommitPairs;
        else ++result.unknownDrawHeldCommitPairs;
        if(!finite(previous.calculationEpoch)||!finite(sample.calculationEpoch))++result.unknownCalculationEpochHeldCommitPairs;
        else if(sample.calculationEpoch===previous.calculationEpoch)++result.sameCalculationEpochHeldCommitPairs;
        else ++result.changedCalculationEpochHeldCommitPairs;
        if(sample.frame>previous.frame)++result.forwardProgressHeldCommitPairs;
        if(distance(sample.bullet.auth,previous.bullet.auth)>settings.epsilon)++result.authoritativeMovedHeldCommitPairs;
        if(!sample.bullet.endpoints||!previous.bullet.endpoints)++result.missingEndpointModelHeldCommitPairs;
      }else{
        if(sameSprite)finish(run,sample);
        run=null;
        seed=kind?{sample,kind,evidence:waitEvidence(previous,sample)}:null;
      }
      previous=sample;
    }
    finish(run);
  }
  const totalHeldMs=durations.reduce((sum,value)=>sum+value,0);
  result.observedStablePairMs=round(result.observedStablePairMs);
  result.totalObservedHeldMs=round(totalHeldMs);
  result.changedCenterCommitsPerObservedStableSecond=result.observedStablePairMs>0?round(result.changedCenterCommitPairs*1000/result.observedStablePairMs):null;
  result.runsPerObservedStableSecond=result.observedStablePairMs>0?round(result.runs*1000/result.observedStablePairMs):null;
  result.heldFractionOfObservedStableMs=result.observedStablePairMs>0?round(totalHeldMs/result.observedStablePairMs):null;
  result.observedHeldMs=distribution(durations);result.releaseDelayMs=distribution(releaseDelays);
  result.releaseStepPx=distribution(releaseSteps);
  result.forwardBudgetCandidateHolds={runs:candidateDurations.length,observedHeldMs:distribution(candidateDurations),
    releaseDelayMs:distribution(candidateReleaseDelays)};
  for(const value of Object.values(result.byEndpointKind))value.totalObservedHeldMs=round(value.totalObservedHeldMs);
  return result;
}
function analysePeer(peer,shared,settings,consider){
  const counts={rawRows:peer.samples.length,eligibleRows:0,filtered:{},committedSamples:0,
    invalidBulletSamples:0,stableSegments:0,stableBulletSamples:0,singletonSegments:0,
    identityBoundaries:0,unobservedCommitBoundaries:0,nonIncreasingTimeBoundaries:0,spriteChangePairs:0};
  const commits={adjacentCallbackPairs:0,baselineCallbacks:0,same:0,one:0,multiple:0,regressed:0,
    positiveDifferenceSum:0,unobservedIntermediateCommits:0,maximumDifference:0};
  const callbackGaps=[],commitGaps=[],segments=[];
  let previousCallback=null,previousCommit=null,segment=[];
  const close=()=>{if(segment.length)segments.push(segment);segment=[];};
  for(const sample of peer.samples){
    sample.excluded=excluded(sample,peer,shared,settings);
    if(sample.excluded){
      counts.filtered[sample.excluded]=(counts.filtered[sample.excluded]??0)+1;
      previousCallback=previousCommit=null;close();continue;
    }
    ++counts.eligibleRows;
    if(previousCallback&&sample.generation===previousCallback.generation){
      ++commits.adjacentCallbackPairs;
      const delta=sample.commit-previousCallback.commit;
      if(delta===0)++commits.same;
      else if(delta===1)++commits.one;
      else if(delta>1){++commits.multiple;commits.unobservedIntermediateCommits+=delta-1;}
      else ++commits.regressed;
      if(delta>0){commits.positiveDifferenceSum+=delta;commits.maximumDifference=Math.max(commits.maximumDifference,delta);}
      if(sample.time>previousCallback.time)callbackGaps.push(sample.time-previousCallback.time);
    }else ++commits.baselineCallbacks;
    previousCallback=sample;
    if(previousCommit&&sample.generation===previousCommit.generation&&sample.commit===previousCommit.commit)continue;
    ++counts.committedSamples;
    if(previousCommit&&sample.generation===previousCommit.generation&&sample.commit>previousCommit.commit&&sample.time>previousCommit.time)
      commitGaps.push(sample.time-previousCommit.time);
    if(!sample.bullet.usable){++counts.invalidBulletSamples;close();previousCommit=sample;continue;}
    if(segment.length){
      const previous=segment.at(-1);
      if(!sameIdentity(previous,sample)){++counts.identityBoundaries;close();}
      else if(sample.commit-previous.commit!==1){++counts.unobservedCommitBoundaries;close();}
      else if(sample.time<=previous.time){++counts.nonIncreasingTimeBoundaries;close();}
    }
    segment.push(sample);previousCommit=sample;
  }
  close();
  const endpoints={validSamples:0,invalidSamples:0,ageMinusPreviousAge:{zero:0,one:0,other:0},
    byState:[],otherAgeDifferences:[],sameAge:{samples:0,exactSame:0,withinEpsilon:0,different:0,
      movingSpawnState:0,movingOtherState:0,maximumDistancePx:0}};
  const otherAges=new Map(),spansByState=new Map(),ageSpans=[],phaseSteps=[],centerSteps=[],centerSpeeds=[],holdRuns=[],phaseEpsilon=1e-6;
  const cohorts={stableCommitPairs:association(),phasePairs:association(),centerPairs:association(),endpointModelPairs:association()};
  const signals=Object.fromEntries(['sameAgeDifferentEndpoints','sameAgeMovingSpawnState','phaseRegression','phaseHold','phaseAdvance',
    'centerHold','centerHoldWithLiveEndpoints','centerHoldWithStationaryEndpoints','centerHoldWhileModelMoves',
    'releaseAfterHeldCommits','largeCenterStep','largeStepBeyondEndpointModel','reverseAgainstEndpointModel',
    'reverseAlongForwardEndpoints','centerTurnFollowingEndpointModel','centerDirectionChange']
    .map(name=>[name,{count:0,...association(),whileNetworkWaiting:0,observedWaitEvidence:evidenceCounts()}]));
  const add=(name,a,b,details=null,priority=0,score=0)=>{
    const signal=signals[name]??={count:0,...association(),whileNetworkWaiting:0,observedWaitEvidence:evidenceCounts()};
    ++signal.count;++signal[bucket(corrected(a,b))];if(waiting(b))++signal.whileNetworkWaiting;
    ++signal.observedWaitEvidence[waitEvidence(a,b).category];
    if(priority)consider(peer,b,{signal:name,details,priority,score});
  };
  const cohort=(name,a,b)=>{++cohorts[name][bucket(corrected(a,b))];};
  for(const run of segments){
    if(run.length<2){++counts.singletonSegments;continue;}
    ++counts.stableSegments;counts.stableBulletSamples+=run.length;
    let holds=0,lastCenterStep=null,lastModelStep=null;
    for(let index=0;index<run.length;++index){
      const sample=run[index],b=sample.bullet,previous=index?run[index-1]:null;
      if(b.endpoints){
        ++endpoints.validSamples;ageSpans.push(b.span);
        if(!spansByState.has(b.state))spansByState.set(b.state,{state:b.state,zero:0,one:0,other:0});
        ++spansByState.get(b.state)[b.span===0?'zero':b.span===1?'one':'other'];
        if(b.span===0)++endpoints.ageMinusPreviousAge.zero;
        else if(b.span===1)++endpoints.ageMinusPreviousAge.one;
        else {++endpoints.ageMinusPreviousAge.other;otherAges.set(b.span,(otherAges.get(b.span)??0)+1);}
        if(b.span===0){
          const d=distance(b.auth,b.previous);++endpoints.sameAge.samples;
          if(b.auth[0]===b.previous[0]&&b.auth[1]===b.previous[1])++endpoints.sameAge.exactSame;
          if(d<=settings.epsilon)++endpoints.sameAge.withinEpsilon;
          else{
            ++endpoints.sameAge.different;
            // BulletUpdate intentionally decrements active_time before moving
            // states 2..4, then ticks it at the end: age can hold during motion.
            const spawnState=b.state>=2&&b.state<=4;
            ++endpoints.sameAge[spawnState?'movingSpawnState':'movingOtherState'];
            add(spawnState?'sameAgeMovingSpawnState':'sameAgeDifferentEndpoints',
              previous,sample,{distancePx:d},spawnState?0:4,d);
          }
          endpoints.sameAge.maximumDistancePx=Math.max(endpoints.sameAge.maximumDistancePx,d);
        }
      }else ++endpoints.invalidSamples;
      if(!previous)continue;
      const a=previous.bullet,dt=sample.time-previous.time;
      cohort('stableCommitPairs',previous,sample);
      if(a.endpoints&&b.endpoints){
        cohort('phasePairs',previous,sample);
        const delta=b.phase-a.phase;phaseSteps.push(delta);
        if(delta<-phaseEpsilon)add('phaseRegression',previous,sample,{phaseBefore:a.phase,phaseAfter:b.phase,delta},6,-delta);
        else if(Math.abs(delta)<=phaseEpsilon)add('phaseHold',previous,sample);
        else add('phaseAdvance',previous,sample);
      }
      if(a.sprite!==b.sprite){
        ++counts.spriteChangePairs;
        if(holds)holdRuns.push(holds);holds=0;lastCenterStep=lastModelStep=null;continue;
      }
      cohort('centerPairs',previous,sample);
      const step=subtract(b.unshaken,a.unshaken),d=length(step),modelStep=a.model&&b.model?subtract(b.model,a.model):null;
      centerSteps.push(d);centerSpeeds.push(d*1000/dt);
      if(d<=settings.epsilon){
        ++holds;add('centerHold',previous,sample);
        if(a.endpoints&&b.endpoints){
          const live=distance(a.auth,a.previous)>settings.epsilon||distance(b.auth,b.previous)>settings.epsilon;
          if(live)add('centerHoldWithLiveEndpoints',previous,sample,{heldCommitPairs:holds},holds>=2?1:0,holds);
          else add('centerHoldWithStationaryEndpoints',previous,sample);
        }
      }else{
        if(holds){holdRuns.push(holds);add('releaseAfterHeldCommits',previous,sample,{heldCommitPairs:holds,stepPx:d});holds=0;}
        if(d>=settings.largeStepPx)add('largeCenterStep',previous,sample,{stepPx:d,elapsedMs:dt});
      }
      if(modelStep){
        cohort('endpointModelPairs',previous,sample);
        const expected=length(modelStep);
        if(d<=settings.epsilon&&expected>settings.epsilon)
          add('centerHoldWhileModelMoves',previous,sample,{modelStepPx:expected},5,expected);
        if(d>=settings.largeStepPx&&d>expected*settings.largeStepFactor+settings.epsilon)
          add('largeStepBeyondEndpointModel',previous,sample,{stepPx:d,modelStepPx:expected,elapsedMs:dt},3,d);
        if(d>settings.epsilon&&expected>settings.epsilon&&aligned(step,modelStep.map(v=>-v),.5,settings.epsilon))
          add('reverseAgainstEndpointModel',previous,sample,{stepPx:d,modelStepPx:expected},5,d);
      }
      const va=velocity(a),vb=velocity(b);
      if(va&&vb&&aligned(va,vb,.98,settings.epsilon)&&
        aligned(step,vb.map(v=>-v),.5,settings.epsilon))
        add('reverseAlongForwardEndpoints',previous,sample,{stepPx:d,phaseBefore:a.phase,phaseAfter:b.phase},4,d);
      if(lastCenterStep&&aligned(step,lastCenterStep.map(v=>-v),.5,settings.epsilon)){
        const ordinaryTurn=modelStep&&lastModelStep&&aligned(modelStep,lastModelStep.map(v=>-v),.5,settings.epsilon);
        add(ordinaryTurn?'centerTurnFollowingEndpointModel':'centerDirectionChange',previous,sample);
      }
      lastCenterStep=d>settings.epsilon?step:null;
      lastModelStep=modelStep&&length(modelStep)>settings.epsilon?modelStep:null;
    }
    if(holds)holdRuns.push(holds);
  }
  endpoints.otherAgeDifferences=[...otherAges].sort((a,b)=>b[1]-a[1]).slice(0,8).map(([difference,count])=>({difference,count}));
  endpoints.byState=[...spansByState.values()].sort((a,b)=>a.state-b.state);
  endpoints.otherDistinctDifferences=otherAges.size;
  endpoints.sameAge.maximumDistancePx=round(endpoints.sameAge.maximumDistancePx);
  const warnings=[];
  if(!counts.eligibleRows)warnings.push('No eligible probe rows; inspect missing motion, time alignment and edge filters.');
  else if(!counts.stableBulletSamples)warnings.push('No continuous valid bullet segment with at least two observed consecutive commits.');
  if(!shared.synchronized)warnings.push('Missing peer timeOrigin/rows; only each peer own edges and known stop were applied.');
  return {seat:peer.seat,nativeMode:peer.raw.timing?.native?.mode??null,nativeDelay:peer.raw.timing?.native?.delay??null,
    counts,window:{clock:shared.synchronized?'epoch-ms':'peer-performance-ms',
    start:shared.synchronized?shared.start:peer.first,end:shared.synchronized?shared.end:peer.last,
    knownStop:shared.synchronized?shared.stop:peer.stop,edgeMs:settings.edgeMs},
    commits,callbackGapMs:distribution(callbackGaps),observedCommitGapMs:distribution(commitGaps),
    stableSegmentLengths:distribution(segments.filter(run=>run.length>=2).map(run=>run.length)),
    endpoints,ageSpan:distribution(ageSpans),renderPhaseStep:distribution(phaseSteps),
    unshakenCenterStepPx:distribution(centerSteps),unshakenCenterSpeedPxPerSecond:distribution(centerSpeeds),
    heldCommitPairsPerRun:distribution(holdRuns),endpointHolds:analyseEndpointHolds(peer,segments,settings,consider),
    cohorts,signals,warnings};
}
function rawWindow(best,settings){
  if(!best)return {selected:false,reason:'No qualifying signal selected; this is not proof of smoothness.',rows:[]};
  const {peer,sample,event}=best,all=peer.samples;
  const start=Math.max(0,Math.min(sample.index-Math.floor(settings.windowRows/2),all.length-settings.windowRows));
  return {selected:true,source:peer.source,seat:peer.seat,sourceRow:sample.index,
    signal:event.signal,details:event.details,
    rows:all.slice(start,start+settings.windowRows).map(s=>{
      const b=s.bullet;
      return {row:s.index,timestampMs:s.time,epochMs:s.epoch,netFrame:s.cursor,
        logicalFrame:s.frame,logicalFrameSource:s.frameSource,correcting:s.correcting,budgetYielded:s.budgetYielded,
        generation:s.generation,
        commit:s.commit,draw:s.draw,calculationEpoch:s.calculationEpoch,presentationAlpha:s.presentationAlpha,
        renderOnly:s.renderOnly,interpolationActive:s.interpolationActive,
        valid:b.valid,slot:b.slot,life:b.life,state:b.state,script:b.script,sprite:b.sprite,vertexAlpha:b.vertexAlpha,
        age:b.age,previousAge:b.previousAge,previousValid:b.previousValid,alpha:s.alpha,
        renderPhase:b.phase,authoritative:b.auth,previous:b.previous,vm:b.vm,center:b.center,
        shake:b.shake,centerWithoutShake:b.unshaken,corrections:s.corrections,
        correctionDelta:corrected(all[s.index-1],s),networkWaiting:waiting(s),
        observedWaitEvidence:waitEvidence(all[s.index-1],s),correctionYields:s.correctionYields,resimulated:s.resimulated,
        cadence:{scale:at(s.raw.cadence,0),missingRemoteInputs:at(s.raw.cadence,1),
          retryPending:at(s.raw.cadence,2),debtSeconds:at(s.raw.cadence,3),completedForward:at(s.raw.cadence,4),
          highRefresh:at(s.raw.cadence,5),interpolate:at(s.raw.cadence,6),presentationAlpha:at(s.raw.cadence,7)},
        effectiveScale:at(s.raw.cadence,0)??at(s.raw.pacing,0),excluded:s.excluded};
    })};
}
function main(){
  const settings=options(process.argv.slice(2));
  if(settings.help){process.stdout.write(HELP);return;}
  let best=null;
  const consider=(peer,sample,event)=>{
    if(!best||event.priority>best.event.priority||(event.priority===best.event.priority&&event.score>best.event.score))
      best={peer,sample,event};
  };
  const files=settings.inputs.map(source=>{
    const {report,nonFiniteTokens}=readReport(source);
    const peers=report.peers.map((raw,index)=>({raw,source,index,seat:raw.seat??index,origin:raw.timeOrigin,
      samples:(Array.isArray(raw.rows)?raw.rows:[]).map((row,n)=>decode(row,n,raw.timeOrigin))}));
    const shared=bounds(peers,report);
    return {source,measurementPassed:report.passed??null,production:report.production??null,
      runtimeTiming:report.runtimeTiming??null,oneWayMs:report.oneWayMs??null,jitterMs:report.jitterMs??null,
      cpuRate:report.cpuRate??null,nonFiniteTokensReplaced:nonFiniteTokens,
      peers:peers.map(peer=>analysePeer(peer,shared,settings,consider))};
  });
  const {inputs,output,help,...thresholds}=settings;
  const result={schema:'th08-motion-cadence-analysis-v1',observationRevision:2,thresholds,
    rules:[
      'Input rows are observer callback timestamps, not exact GPU scanout timestamps.',
      'Common measured interval uses peer timeOrigin; any peer reaching its target cuts off natural stopped-input tails.',
      'Logical end/progress uses row.logicalFrame, then progress[0], then legacy net[3]. Raw-window netFrame preserves the actual replay cursor.',
      'Display samples use only committed probe snapshots and real commit ordinals; historical cursor advances never create presentations.',
      'Network waiting, repeated logical frames and zero movement are retained. Touch release is not a game-input stop.',
      'Bullet segments require same generation/scene/slot/life/state/script, one valid quad and consecutive real commit ordinals.',
      'Sprite changes only break center comparisons. Invalid previous endpoints are excluded from age/phase models.',
      'Age-span and endpoint counts include stable segments only. Other age differences list at most eight values.',
      'Age is an owner timer, not a native-frame counter. Spawn states 2..4 intentionally hold age while moving; these are not anomaly candidates.',
      'Corrections are cumulative driver[3] differences between observations; association does not establish causation.',
      'Legacy whileNetworkWaiting/networkWaiting combine net[9] admission waiting with retry_pending, which also includes CPU correction yields; they are mixed evidence, not a network-cause verdict.',
      'observedWaitEvidence separates admissionWait, correctionYield/work, both, and pendingUnknown. Admission can await remote/local input or a lifecycle barrier; network transport and peer CPU are not distinguishable here.',
      'intervalScaleAboveOneHeldCommitPairs reports sampled network clock braking separately; scale/missing-input association still does not isolate transport from peer CPU.',
      'noneObserved means no reported admission/correction evidence, not proof of sufficient CPU or network capacity; unknown marks incomplete flags, and retry_pending alone is pendingUnknown.',
      'endpointHolds compares consecutive committed geometry of the same owner and sprite, regardless of PreviousValid. Missing endpoint models never erase a geometric hold.',
      'Only high-refresh, native mode0, semantic alpha1, completed-forward samples are forward budget candidates. No explicit budget_spent/present-reason probe exists, so budgetCauseProven remains false.',
      'Endpoint hold durations run from the endpoint observer timestamp through the last held commit: a lower bound. releaseDelayMs ends at the observed release commit; censored runs have no observed release.',
      'Endpoint hold rate and fraction use observed stable same-owner/sprite commit-pair time, not the entire game duration. Examples retain the eight longest runs.',
      'CalculationEpoch is a draw/update epoch, not logical progress or bullet identity; normal changes do not split owner continuity. Same versus changed epoch held pairs are reported separately.',
      'A probe observes one submitted bullet quad, not GPU scanout or occlusion. Stationary/transparent bullets can legitimately hold; raw vertex alpha and authoritative endpoint changes remain evidence, not a verdict.',
      'Large steps, holds and direction changes are descriptive. Expected stationary motion and turns are separate; no CPU benefit is inferred.',
      'The single raw window preserves original numeric samples and zero-based source row indices; derived summary distributions are rounded.'
    ],files,rawWindow:rawWindow(best,settings)};
  const text=JSON.stringify(result,null,2)+'\n';
  if(output){mkdirSync(dirname(output),{recursive:true});writeFileSync(output,text);}
  process.stdout.write(JSON.stringify({type:'configuration',schema:result.schema,thresholds,output})+'\n');
  for(const file of files)for(const peer of file.peers)
    process.stdout.write(JSON.stringify({type:'summary',source:file.source,measurementPassed:file.measurementPassed,
      seat:peer.seat,nativeMode:peer.nativeMode,nativeDelay:peer.nativeDelay,counts:peer.counts,commits:peer.commits,
      ageSpan:peer.endpoints.ageMinusPreviousAge,ageSpanByState:peer.endpoints.byState,sameAge:peer.endpoints.sameAge,
      phaseStep:peer.renderPhaseStep,centerStep:peer.unshakenCenterStepPx,heldRuns:peer.heldCommitPairsPerRun,endpointHolds:peer.endpointHolds,
      cohorts:peer.cohorts,signals:peer.signals,warnings:peer.warnings})+'\n');
  const {rows,...window}=result.rawWindow;
  process.stdout.write(JSON.stringify({type:'rawWindow',...window})+'\n');
  for(const row of rows)process.stdout.write(JSON.stringify({type:'sample',...row})+'\n');
}
try{main();}catch(error){process.stderr.write('motion-cadence: '+error.message+'\n');process.exitCode=1;}
