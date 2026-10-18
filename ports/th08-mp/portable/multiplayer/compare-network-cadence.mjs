// Test-only analysis. Does not launch a browser or change either input report.
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';

const usage='Usage: node compare-network-cadence.mjs baseline.json candidate.json [--output comparison.json]';
const MAX_SEGMENTS=64,MAX_WINDOWS=16,MAX_NOTICES=16;
const finite=value=>typeof value==='number'&&Number.isFinite(value);
const integer=value=>Number.isSafeInteger(value)&&value>=0;
const pick=(value,keys)=>Object.fromEntries(keys.filter(key=>value?.[key]!==undefined).map(key=>[key,value[key]]));
const equal=(a,b)=>a!=null&&b!=null?JSON.stringify(a)===JSON.stringify(b):null;
const canonical=value=>Array.isArray(value)&&value.length>0&&value.every(integer)?value:null;
const delta=(after,before,key)=>finite(after?.[key])&&finite(before?.[key])?after[key]-before[key]:null;
const native=row=>row?.net?{mode:row.net[14]??null,delay:row.net[12]??null}:null;
const unique=values=>[...new Map(values.map(value=>[JSON.stringify(value),value])).values()];
const logicalFrame=row=>row?.logicalFrame??row?.net?.[3];
const progress=row=>Array.isArray(row?.progress)&&row.progress.length===6?row.progress:null;
const validProgress=row=>row?.progress==null||(progress(row)!==null&&row.progress.every(value=>integer(value)&&value<=0xffffffff)&&
  [0,1].includes(row.progress[2])&&[0,1].includes(row.progress[3])&&
  row.progress[0]===logicalFrame(row)&&row.progress[1]===row.net?.[3]);
const correcting=row=>progress(row)?.[2]===1;
const authoredAnchor=row=>validProgress(row)&&!correcting(row);
const sameEpoch=(a,b)=>(a?.stage??null)===(b?.stage??null)&&(a?.net?.[10]??null)===(b?.net?.[10]??null);
const unconfirmedFrames=row=>{
  const frame=logicalFrame(row),confirmed=row?.net?.[5];
  return finite(frame)&&finite(confirmed)?Math.max(0,frame-(confirmed===0xffffffff||confirmed===-1?0:confirmed+1)):null;
};
function distribution(values){
  const sorted=values.filter(value=>finite(value)&&value>=0).sort((a,b)=>a-b);
  const at=fraction=>sorted[Math.max(0,Math.ceil(sorted.length*fraction)-1)]??null;
  return {count:sorted.length,min:sorted[0]??null,
    mean:sorted.length?sorted.reduce((sum,value)=>sum+value,0)/sorted.length:null,
    p50:at(.5),p95:at(.95),p99:at(.99),max:sorted.at(-1)??null,
    over50:sorted.filter(value=>value>50).length,over100:sorted.filter(value=>value>100).length};
}
function metrics(rows){
  if(rows.length<2)return {samples:rows.length,validCadence:false,reason:'Fewer than two callback samples'};
  if(rows.some(row=>!row||typeof row!=='object'))return {samples:rows.length,validCadence:false,reason:'Invalid callback row'};
  const first=rows[0],last=rows.at(-1),ticks=[],callbackGaps=[],advanceGaps=[],presentGaps=[];
  let lastAdvance=null,lastPresent=null,clockBreaks=0,logicalRegressions=0,enemyRegressions=0,presentRegressions=0;
  let authoredHoldLogicalFrames=0,cursorRegressions=0,cursorRegressedFrames=0,partialEnemyRegressions=0,epochChanges=0;
  let previousAnchor=authoredAnchor(first)?first:null;
  for(let i=1;i<rows.length;i++){
    const previous=rows[i-1],row=rows[i],dt=row.time-previous.time;
    if(!finite(dt)||dt<=0){++clockBreaks;lastAdvance=lastPresent=null;continue;}
    callbackGaps.push(dt);
    const dc=row.net?.[3]-previous.net?.[3];
    if(finite(dc)&&dc<0){++cursorRegressions;cursorRegressedFrames-=dc;}
    if(!sameEpoch(row,previous))++epochChanges;
    const df=logicalFrame(row)-logicalFrame(previous);
    if(!finite(df)||df<0||!sameEpoch(row,previous)){++logicalRegressions;lastAdvance=null;}
    else{
      ticks.push(df);
      if(df>0){
        if(lastAdvance!==null)advanceGaps.push(row.time-lastAdvance);
        lastAdvance=row.time;
        if(authoredAnchor(row)&&authoredAnchor(previous)&&row.enemyFrame===previous.enemyFrame)authoredHoldLogicalFrames+=df;
      }
    }
    if((correcting(row)||correcting(previous))&&finite(row.enemyFrame)&&finite(previous.enemyFrame)&&row.enemyFrame<previous.enemyFrame)
      ++partialEnemyRegressions;
    if(authoredAnchor(row)){
      if(previousAnchor&&finite(row.enemyFrame)&&finite(previousAnchor.enemyFrame)&&row.enemyFrame<previousAnchor.enemyFrame)++enemyRegressions;
      previousAnchor=row;
    }
    const dp=row.graphics?.[5]-previous.graphics?.[5];
    if(finite(dp)&&dp<0){++presentRegressions;lastPresent=null;}
    else if(dp>0){
      if(lastPresent!==null)presentGaps.push(row.time-lastPresent);
      lastPresent=row.time;
    }
  }
  const wallMs=last.time-first.time,frames=logicalFrame(last)-logicalFrame(first);
  const invalidProgress=rows.filter(row=>!validProgress(row)).length;
  const valid=clockBreaks===0&&logicalRegressions===0&&invalidProgress===0&&finite(wallMs)&&wallMs>0&&finite(frames)&&frames>=0;
  const observed=rows.slice(1),available=observed.filter(row=>progress(row)!==null);
  const initialProgress=progress(first),finalProgress=progress(last);
  const callbackTotalMs=observed.reduce((sum,row)=>sum+(finite(row.ms)?row.ms:0),0);
  return {samples:rows.length,callbacks:rows.length-1,validCadence:valid,
    wallMs:finite(wallMs)?wallMs:null,netFrame:{first:first.net?.[3]??null,last:last.net?.[3]??null},
    logicalFrame:{first:logicalFrame(first)??null,last:logicalFrame(last)??null},
    logicalFrames:valid?frames:null,logicalHz:valid?frames*1000/wallMs:null,
    enemyFrame:{first:first.enemyFrame??null,last:last.enemyFrame??null},
    authoredFrames:enemyRegressions||!authoredAnchor(first)||!authoredAnchor(last)?null:last.enemyFrame-first.enemyFrame,
    zeroTickCallbacks:ticks.filter(value=>value===0).length,
    multiTickCallbacks:ticks.filter(value=>value>1).length,
    maxLogicalTicksPerCallback:ticks.length?ticks.reduce((maximum,value)=>Math.max(maximum,value),0):null,
    ticksPerCallback:distribution(ticks),authoredHoldLogicalFrames,
    predictionBacklogFrames:distribution(rows.map(unconfirmedFrames)),
    callbackTotalMs,callbackMsPerLogicalFrame:valid&&frames>0?callbackTotalMs/frames:null,
    callbackMs:distribution(rows.slice(1).map(row=>row.ms)),
    callbackGapMs:distribution(callbackGaps),advanceGapMs:distribution(advanceGaps),presentGapMs:distribution(presentGaps),
    corrections:finite(last.driver?.[3]-first.driver?.[3])?last.driver[3]-first.driver[3]:null,
    resimulated:finite(last.driver?.[4]-first.driver?.[4])?last.driver[4]-first.driver[4]:null,
    progress:{sources:unique(rows.map(row=>row.logicalFrame==null?'legacy net[3]':'logicalFrame')),
      availableSamples:available.length,invalidSamples:invalidProgress,
      correctingCallbacks:available.length?available.filter(correcting).length:null,
      budgetYieldCallbacks:available.length?available.filter(row=>progress(row)[3]===1).length:null,
      correctionYields:initialProgress&&finalProgress&&epochChanges===0&&finalProgress[4]>=initialProgress[4]?finalProgress[4]-initialProgress[4]:null,
      correctingCallbackMs:distribution(available.filter(correcting).map(row=>row.ms)),
      budgetYieldCallbackMs:distribution(available.filter(row=>progress(row)[3]===1).map(row=>row.ms)),
      scope:'Post-callback flags; all intervening callbacks contribute to total cadence/cost. Missing legacy flags are unknown.'},
    clockBreaks,logicalRegressions,cursorRegressions,cursorRegressedFrames,epochChanges,
    enemyRegressions,partialEnemyRegressions,presentRegressions};
}
function splitTimeline(rows){
  const segments=[],events=[];let current=null,previous=null,previousAnchor=null,invalidRows=0;
  const record=(kind,index,extra={})=>events.push({kind,row:index,...extra});
  for(let index=0;index<rows.length;index++){
    const row=rows[index];
    if(!row||!finite(row.time)||!integer(row.enemyFrame)||!integer(row.scene)||!integer(row.net?.[3])||
       !integer(logicalFrame(row))||!validProgress(row)){
      ++invalidRows;record('invalid-row-metadata',index);current=previous=previousAnchor=null;continue;
    }
    const generation=row.net?.[10]??null,stage=row.stage??null,loading=row.loading??null;
    const reasons=[];
    if(previous){
      if(row.scene!==previous.scene)reasons.push('scene-change');
      if(loading!==(previous.loading??null))reasons.push('loading-change');
      if(generation!==(previous.net?.[10]??null))reasons.push('generation-change');
      if(stage!==(previous.stage??null))reasons.push('stage-change');
      if(authoredAnchor(row)&&previousAnchor&&row.enemyFrame<previousAnchor.enemyFrame)reasons.push('enemy-frame-regression');
      if(logicalFrame(row)<logicalFrame(previous))reasons.push('logical-frame-regression');
      if((progress(row)===null)!==(progress(previous)===null))reasons.push('progress-schema-change');
      if(row.time<=previous.time)reasons.push('non-increasing-time');
    }
    if(!current||reasons.length){
      for(const reason of reasons)record(reason,index,{fromEnemyFrame:previous.enemyFrame,toEnemyFrame:row.enemyFrame});
      current={id:segments.length,scene:row.scene,generation,stage,loading,reason:reasons.length?reasons:['start'],
        start:index,end:index,rows:[],anchors:[]};segments.push(current);previousAnchor=null;
    }
    if(authoredAnchor(row)){current.anchors.push({row,index:current.rows.length});previousAnchor=row;}
    current.rows.push(row);current.end=index;previous=row;
  }
  const eligible=segments.filter(segment=>(segment.loading===0||segment.loading===false)&&
    segment.anchors.length>=2&&segment.anchors.at(-1).row.enemyFrame>segment.anchors[0].row.enemyFrame);
  return {segments,eligible,events,invalidRows};
}
function segmentDescription(segment){
  const first=segment.rows[0],last=segment.rows.at(-1);
  return {id:segment.id,scene:segment.scene,stage:segment.stage,generation:segment.generation,loading:segment.loading,
    reason:segment.reason,samples:segment.rows.length,rowRange:[segment.start,segment.end],
    enemyFrame:[segment.anchors[0]?.row.enemyFrame??null,segment.anchors.at(-1)?.row.enemyFrame??null],
    authoredAnchorSamples:segment.anchors.length,correctingSamples:segment.rows.filter(correcting).length,
    observedEnemyFrame:[first.enemyFrame,last.enemyFrame],logicalFrame:[logicalFrame(first),logicalFrame(last)],
    netFrame:[first.net[3],last.net[3]],wallMs:last.time-first.time};
}
function timingEvidence(report,peer,index){
  const startup=report.startupTiming?.[index]??null,timing=peer.timing??startup;
  const host=timing?.host??startup?.host,cal=timing?.calibration??startup?.calibration;
  const policy=['mode','delay','automatic','predictionLimit','reserve'];
  return {entryRequested:host?pick(host.requested,policy):null,entryEffective:host?pick(host.effective,policy):null,
    hostErrors:host?.errors?.slice(0,MAX_NOTICES)??[],
    startupNative:startup?.native??null,afterNative:native(peer.after)??timing?.native??null,
    bodyNativeStates:unique((peer.rows??[]).map(native).filter(Boolean)),
    calibration:cal&&cal[1]?{phase:cal[1],committed:cal[1]===5,players:cal[4],localPlayer:cal[5],mode:cal[6],
      automatic:!!cal[7],delay:cal[8],fullDelay:cal[9],P:cal[1]===5?cal[10]:null}:null,
    predictionLimitSource:'Entry effective policy only; net[13] is the capture frontier, not native H'};
}
function inputEvidence(report,peer,index,seat){
  const imp=peer.impairment??{},before=peer.before?.impairment,touch=peer.touch??{};
  const bodyKeys=['matched','scheduled','sent','dropped','canceled','controlInputs','overflow','errors'];
  const body=Object.fromEntries(bodyKeys.map(key=>[key,delta(imp,before,key)]));
  const delivered=delta(imp,before,'deliveredDelayTotalMs');
  const startup=report.startupImpairment?.[index]??before??{};
  const idle=report.idleInput===true||(report.wallClockTouch===true&&Array.isArray(report.touchSeats)&&!report.touchSeats.includes(seat));
  return {scope:'Counters cover the complete measured body or phase, not the authored intersection',
    stimulus:peer.wallClockTouch?'native-touch':idle?'idle-request':peer.identity?.fixtureBuild?'fixture-capture':'unrecorded',
    configuredInjection:{scope:imp.scope??report.scope??null,oneWayMs:imp.oneWayMs??report.oneWayMs??null,jitterMs:imp.jitterMs??report.jitterMs??null},
    measuredInput:body,measuredDeliveredMeanMs:body.sent>0&&delivered!==null?delivered/body.sent:null,
    runInputTotals:pick(imp,['matched','sent','controlInputs','overflow','errors','maxDeliveredDelayMs','maxTimerOverrunMs',
      'spikeDelayed','spikeControlDelayed','spikeStartMs','spikeEndMs']),
    startupCalibration:pick(startup,['calibrationMatched','calibrationProbes','calibrationEchoes','calibrationSent',
      'calibrationControlBypassed','calibrationControlPolicy','calibrationMaxDeliveredDelayMs','calibrationMaxTimerOverrunMs']),
    touch:pick(touch,['source','sourceHz','events','scheduled','coalesced','pathPixels','minX','maxX','timerActive','tapeSha256']),
    nativeRoutes:unique((peer.rows??[]).map(row=>row?.driver?.[12]).filter(value=>value!==undefined))};
}
function analyzeRun(path){
  const report=JSON.parse(readFileSync(path,'utf8'));
  if(!report||!Array.isArray(report.peers))throw Error('Expected a full measurement report with peers: '+path);
  const seats=new Set(),peers=report.peers.map((peer,index)=>{
    if(!peer||typeof peer!=='object')throw Error('Invalid peer '+index+' in '+path);
    const seat=integer(peer.seat)?peer.seat:index;
    if(seats.has(seat))throw Error('Duplicate peer seat '+seat+' in '+path);
    seats.add(seat);
    const rows=Array.isArray(peer.rows)?peer.rows:[],timeline=splitTimeline(rows);
    const browser=report.browsers?.find(value=>value.seat===seat)??report.browsers?.[index]??null;
    const throttled=Array.isArray(report.slowSeats)?report.slowSeats.includes(seat):null;
    const notices=[];
    if(!integer(peer.seat))notices.push('Seat inferred from peer array index');
    if(!rows.length)notices.push('No measured callback rows');
    if(rows.some(row=>row?.progress==null))notices.push('Rows without progress use the legacy atomic-correction assumption for authored anchors; correction/yield flags are unknown');
    if(timeline.invalidRows)notices.push('Invalid row metadata prevents a complete timeline');
    if(timeline.events.some(event=>event.kind==='enemy-frame-regression'))
      notices.push('Enemy counter regressed: possible stage/restart/rebirth boundary; repeated ranges cannot be automatically paired');
    if(timeline.events.some(event=>event.kind==='scene-change'))notices.push('Scene changes are split; no window crosses them');
    if(timeline.segments.some(segment=>segment.loading===null))notices.push('Missing loading state: those segments are not eligible for authored matching');
    if(timeline.segments.length>MAX_SEGMENTS)notices.push('Too many segments for a bounded comparison; automatic matching disabled');
    const timing=timingEvidence(report,peer,index);
    if(timing.bodyNativeStates.length>1)notices.push('Native mode/delay changed during the measured body');
    const afterFrame=logicalFrame(peer.after),afterConfirmed=peer.after?.net?.[5];
    const canonicalReady=!!peer.after&&validProgress(peer.after)&&!correcting(peer.after)&&integer(afterFrame)&&
      afterFrame===peer.after.net?.[3]&&integer(afterConfirmed)&&afterConfirmed!==0xffffffff&&afterConfirmed>=afterFrame-1;
    if(!canonicalReady)notices.push('After state is incomplete or has invalid progress; canonical agreement is not a completed-state witness');
    return {seat,rows,timeline,canonical:canonicalReady?canonical(peer.canonical):null,description:{
      seat,identity:pick(peer.identity,['game','variant','wasm','wasmSha256','fixtureBuild']),
      cpu:{configuredRate:throttled===null?null:throttled?report.cpuRate:1,runRate:report.cpuRate??null,throttledSeat:throttled,
        scope:'CDP rate configured by the harness, not a measured hardware speed'},
      browser:browser?{version:browser.version,actualGL:browser.actualGL,nativeGpuVerified:browser.nativeGpuVerified}:null,
      timing,input:inputEvidence(report,peer,index,seat),
      body:metrics(rows),after:{netFrame:peer.after?.net?.[3]??null,logicalFrame:logicalFrame(peer.after)??null,
        correcting:progress(peer.after)?.[2]??null,callbackYielded:progress(peer.after)?.[3]??null,
        correctionYields:progress(peer.after)?.[4]??null,correctionEnd:progress(peer.after)?.[5]??null,
        confirmedThrough:peer.after?.net?.[5]??null,driverFailed:peer.after?.driver?.[2]??null,
        channelError:peer.after?.driver?.[15]??null,canonicalReady,canonical:canonical(peer.canonical)},
      harnessSummary:pick(report.summary?.[index],['logicalFps','frames','elapsedMs','presentGapMs','advanceGapMs','callbackMs',
        'maxLogicalTicksPerCallback','multiTickCallbacks','corrections','resimulated','profileAvailable',
        'logicalCounterValid','logicalFrame','netCursor','progress','predictionBacklogFrames']),
      timeline:{segmentCount:timeline.segments.length,eligibleSegments:timeline.eligible.length,invalidRows:timeline.invalidRows,
        events:timeline.events.slice(0,MAX_NOTICES),omittedEvents:Math.max(0,timeline.events.length-MAX_NOTICES),
        segments:timeline.segments.slice(0,MAX_SEGMENTS).map(segmentDescription)},notices}};
  });
  const allCanonical=peers.length>=2&&peers.length===report.players&&peers.every(peer=>peer.canonical);
  const afterAgreement=allCanonical?peers.every(peer=>equal(peer.canonical,peers[0].canonical)):null;
  return {report,peers,description:{path,passed:report.passed??null,error:report.error??null,
    runtimeTiming:report.runtimeTiming??null,performancePolicy:report.mode??null,production:report.production??null,
    players:report.players??null,requestedFrames:report.frames??null,
    options:pick(report,['cpuRate','slowSeats','difficulty','denseBullets','wallClockTouch','idleInput','touchSeats','touchHz',
      'limit60','oneWayMs','jitterMs','browserChannel','headed','nativeGpu']),
    admission:report.admission?{serial:report.admission.serial,room:pick(report.admission.room,
      ['playerCount','difficulty','adonisMode','inputDelay','inputDelayAuto','predictionLimit','predictionReserve'])}:null,
    afterCanonicalAgreementAcrossPeers:afterAgreement,peers:peers.map(peer=>peer.description)}};
}
const compatible=(a,b)=>a.scene===b.scene&&(a.stage===null||b.stage===null||a.stage===b.stage)&&
  (a.generation===null||b.generation===null||a.generation===b.generation);
function bounds(segment){return [segment.anchors[0].row.enemyFrame,segment.anchors.at(-1).row.enemyFrame];}
function overlap(segment,low,high){const [first,last]=bounds(segment);return Math.min(last,high)>Math.max(first,low);}
function repeatedRange(timeline,segment,low,high){
  // Do not use a generation number to guess which occurrence of the same
  // authored counter is comparable when stage identity is absent.
  return timeline.eligible.some(other=>other!==segment&&other.scene===segment.scene&&
    (other.stage===null||segment.stage===null||other.stage===segment.stage)&&overlap(other,low,high));
}
function comparePeers(baseline,candidate){
  const notices=[],windows=[],rejected=[];
  if(baseline.timeline.segments.length>MAX_SEGMENTS||candidate.timeline.segments.length>MAX_SEGMENTS)
    return {seat:baseline.seat,status:'unmatched',notices:['Segment limit exceeded; no automatic pairing'],windows:[]};
  for(const a of baseline.timeline.eligible)for(const b of candidate.timeline.eligible){
    if(!compatible(a,b))continue;
    const [af,al]=bounds(a),[bf,bl]=bounds(b),low=Math.max(af,bf),high=Math.min(al,bl);
    if(high<=low)continue;
    const identity={scene:a.scene,baselineSegment:a.id,candidateSegment:b.id,enemyFrame:[low,high]};
    if(repeatedRange(baseline.timeline,a,low,high)||repeatedRange(candidate.timeline,b,low,high)){
      rejected.push({...identity,reason:'Repeated authored range in the same scene; stage/restart occurrence is ambiguous'});continue;
    }
    const observed=new Set(b.anchors.map(anchor=>anchor.row.enemyFrame));
    const shared=[...new Set(a.anchors.map(anchor=>anchor.row.enemyFrame).filter(frame=>frame>=low&&frame<=high&&observed.has(frame)))];
    if(shared.length<2){
      rejected.push({...identity,reason:'Fewer than two identical non-correcting authored anchors; no boundary interpolation'});continue;
    }
    const first=shared[0],last=shared.at(-1);
    const cut=segment=>{
      const start=segment.anchors.find(anchor=>anchor.row.enemyFrame===first).index;
      const end=segment.anchors.find(anchor=>anchor.row.enemyFrame===last).index;
      // Anchor selection excludes half-corrected worlds; cost selection must
      // retain every original callback between those stable endpoints.
      return {rows:segment.rows.slice(start,end+1),range:[segment.start+start,segment.start+end]};
    };
    const leftCut=cut(a),rightCut=cut(b),left=metrics(leftCut.rows),right=metrics(rightCut.rows);
    if(!left.validCadence||!right.validCadence){
      rejected.push({...identity,reason:'Invalid callback clock or logical frontier inside the proposed window'});continue;
    }
    windows.push({scene:a.scene,stage:a.stage===b.stage?a.stage:null,
      enemyFrame:{first,last,span:last-first,sharedObservedAnchors:shared.length},
      baselineSegment:a.id,candidateSegment:b.id,baselineRows:leftCut.range,candidateRows:rightCut.range,
      baseline:left,candidate:right});
  }
  if(!windows.length)notices.push('No unambiguous common authored window; do not substitute full-body metrics as an equal-workload comparison');
  if(rejected.length)notices.push('Some overlapping authored ranges were rejected; see rejectedWindows');
  if(windows.length>MAX_WINDOWS)notices.push('Only the first '+MAX_WINDOWS+' matched windows are shown; no aggregate merges them');
  if(!equal(baseline.description.cpu,candidate.description.cpu))notices.push('CPU throttle configurations differ');
  if(!equal(baseline.description.input.configuredInjection,candidate.description.input.configuredInjection))notices.push('Application-send impairment configurations differ');
  if(baseline.description.input.stimulus!==candidate.description.input.stimulus)notices.push('Input stimulus categories differ');
  const leftHash=baseline.description.input.touch.tapeSha256,rightHash=candidate.description.input.touch.tapeSha256;
  if(leftHash&&rightHash&&leftHash!==rightHash)notices.push('Wall-clock touch tapes differ; matching authored counters does not prove identical player state');
  return {seat:baseline.seat,status:windows.length?'matched-counter-windows':'unmatched',
    afterCanonicalEqualBetweenRuns:equal(baseline.canonical,candidate.canonical),
    notices,matchedWindowCount:windows.length,windows:windows.slice(0,MAX_WINDOWS),
    rejectedWindowCount:rejected.length,rejectedWindows:rejected.slice(0,MAX_NOTICES)};
}
function main(){
  const positional=[];let output=null;
  for(let i=2;i<process.argv.length;i++){
    const arg=process.argv[i];
    if(arg==='--help'||arg==='-h'){console.log(usage);return;}
    if(arg==='--output'){
      if(output||!process.argv[i+1]||process.argv[i+1].startsWith('--'))throw Error(usage);
      output=resolve(process.argv[++i]);
    }else if(arg.startsWith('--'))throw Error('Unknown option '+arg+'\n'+usage);
    else positional.push(resolve(arg));
  }
  if(positional.length!==2)throw Error(usage);
  const pathKey=value=>process.platform==='win32'?value.toLowerCase():value;
  if(output&&positional.some(path=>pathKey(path)===pathKey(output)))throw Error('Output must not overwrite an input report');
  const baseline=analyzeRun(positional[0]),candidate=analyzeRun(positional[1]);
  const peerComparisons=[],unpaired=[];
  for(const left of baseline.peers){
    const right=candidate.peers.find(peer=>peer.seat===left.seat);
    if(right)peerComparisons.push(comparePeers(left,right));
    else unpaired.push({side:'baseline',seat:left.seat});
  }
  for(const right of candidate.peers)if(!baseline.peers.some(peer=>peer.seat===right.seat))unpaired.push({side:'candidate',seat:right.seat});
  const workloadKeys=['players','production','difficulty','denseBullets','mode','checkpointSpan','backgroundIndex',
    'bombLazy','shotSparse','recordSparse','titleSpellsLazy','worldInstancing','textureCoalesce','skipResimVisual','skipResimGeometry'];
  const workloadDifferences=workloadKeys.filter(key=>baseline.report[key]!==candidate.report[key]).map(key=>
    ({field:key,baseline:baseline.report[key]??null,candidate:candidate.report[key]??null}));
  const comparison={schema:'th08-network-cadence-comparison/2',createdAt:new Date().toISOString(),
    method:{selection:'Same scene, compatible recorded generation/stage, non-loading segment; first non-Correcting arrival at shared observed enemyFrame endpoints',
      boundaries:'No interpolation or concatenation across scene/stage/generation/reset boundaries; stable authored regressions and repeated ranges are rejected',
      metrics:'logicalFrame (legacy net[3] fallback) owns forward cadence/backlog. Raw net[3] is the historical cursor. Every intervening partial-correction callback is included; boundary callback work and censored edge waits are excluded. Present uses the original graphics counter.',
      identity:'enemyFrame is app.game.enemies.state.frames. Stage and player respawn/life state are not recorded in older rows',
      limitations:'Counter alignment does not prove identical bullets, RNG, player paths or death/respawn state; canonical equality is reported separately',
      interpretation:'Measured values only. No automatic improvement percentage or performance promise. CPU throttling is not a phone model; application-send delay is not wire RTT'},
    baseline:baseline.description,candidate:candidate.description,peerComparisons,unpaired,workloadDifferences,
    notices:[...(baseline.report.passed===true?[]:['Baseline report did not pass']),
      ...(candidate.report.passed===true?[]:['Candidate report did not pass']),
      ...(unpaired.length?['Peer seats differ between reports']:[]),
      ...(workloadDifferences.length?['Workload/fixture settings differ; see workloadDifferences']:[])]};
  const text=JSON.stringify(comparison,null,2)+'\n';
  if(output){
    mkdirSync(dirname(output),{recursive:true});writeFileSync(output,text);
    console.log(JSON.stringify({output,baselinePassed:baseline.description.passed,candidatePassed:candidate.description.passed,
      canonicalAgreement:{baseline:baseline.description.afterCanonicalAgreementAcrossPeers,candidate:candidate.description.afterCanonicalAgreementAcrossPeers},
      peers:peerComparisons.map(peer=>({seat:peer.seat,status:peer.status,matchedWindows:peer.matchedWindowCount??0,
        rejectedWindows:peer.rejectedWindowCount??0,notices:peer.notices})),notices:comparison.notices},null,2));
  }else process.stdout.write(text);
}
try{main();}catch(error){console.error('compare-network-cadence: '+error.message);process.exitCode=1;}
