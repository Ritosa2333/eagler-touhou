// Offline only: no browser, gate rerun, or mutation of raw evidence.
import { readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { basename, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
const [input, output] = process.argv.slice(2);
if (!input || !output) throw new Error('usage: node analyze-release-response.mjs raw-report.json evidence.json');
const bytes=readFileSync(input),raw=JSON.parse(bytes);
const sha=b=>createHash('sha256').update(b).digest('hex');
const finite=x=>typeof x==='number'&&Number.isFinite(x);
const dist=values=>{
  const a=values.filter(finite).sort((a,b)=>a-b),n=a.length;
  return {count:n,total:a.reduce((s,x)=>s+x,0),min:n?a[0]:null,
    p50:n?a[Math.floor(n*.5)]:null,p95:n?a[Math.floor(n*.95)]:null,
    p99:n?a[Math.floor(n*.99)]:null,max:n?a.at(-1):null};
};
const ms=values=>({...dist(values),...Object.fromEntries([25,50,100,250].map(n=>['over'+n+'ms',values.filter(x=>finite(x)&&x>n).length]))});
const metricNames=['scheduleToSubmitMs','submitToFirstWireMs','scheduleToFirstWireMs',
  'firstWireToPresentationObservedMs','submitToPresentationObservedMs','scheduleToPresentationObservedMs'];
function cohort(intents,phaseAt){
  const eligible=intents.filter(x=>x.changedSubmittedMask!==false);
  return {planned:intents.length,submitted:intents.filter(x=>x.submittedAtMs!==null).length,
    captured:intents.filter(x=>x.captureFrame!==null).length,presented:intents.filter(x=>x.presentationObservedAtMs!==null).length,
    sameMaskResubmissions:intents.length-eligible.length,
    statuses:Object.fromEntries(['not-submitted','submitted-not-captured','captured-not-presented','presented'].map(s=>[s,intents.filter(x=>x.status===s).length])),
    metricDenominator:eligible.length,
    observationPhases:Object.fromEntries(['submittedAtMs','firstWireAtMs','presentationObservedAtMs'].map(k=>[k,
      Object.fromEntries(['before-formal','formal','after-formal','not-observed'].map(p=>[p,intents.filter(x=>phaseAt(x[k])===p).length]))])),
    conditionalLatencyMs:Object.fromEntries(metricNames.map(k=>[k,{...ms(eligible.map(x=>x[k])),missing:eligible.filter(x=>!finite(x[k])).length,
      slowestIntentIds:eligible.filter(x=>finite(x[k])).sort((a,b)=>b[k]-a[k]).slice(0,5).map(x=>x.id)}]))};
}
const indexed=(rows,names)=>rows.map(row=>Object.fromEntries(names.map((k,i)=>[k,row[i]])));
const relay=raw.relayTransport;
if(!relay||!raw.parameters.responsiveness)throw new Error('Requires relay and responsiveness raw evidence');
const packets=new Map(indexed(relay.packets,relay.columns.packet).map(x=>[x.id,x]));
const events=indexed(relay.events,relay.columns.event);
const scheduled=events.filter(x=>x.phase===relay.columns.phases.indexOf('scheduled'));
const sends=events.filter(x=>x.phase===relay.columns.phases.indexOf('send'));
const sendBySchedule=new Map(sends.map(x=>[x.scheduledId,x]));
const anchor=relay.injection?.anchor?.at;
const relative=t=>finite(t)&&finite(anchor)?t-anchor:null;
const range=a=>{let lo=Infinity,hi=-Infinity;for(const x of a)if(finite(x)){lo=Math.min(lo,x);hi=Math.max(hi,x);}return lo===Infinity?null:[lo,hi];};
const scheduledRows=scheduled.map(x=>{
  const p=packets.get(x.packetId),s=sendBySchedule.get(x.scheduledId);
  return {scheduledId:x.scheduledId,packetId:x.packetId,sender:p.sender,destination:p.destination,
    senderFrame:p.senderFrame,inputCount:p.inputCount,burst:!!x.burst,scheduledAtMsFromRelayAnchor:relative(x.enteredAt),
    plannedDelayMs:x.plannedDelayMs,dueAtMsFromRelayAnchor:relative(x.dueAt),
    sendApiAtMsFromRelayAnchor:s?relative(s.enteredAt):null,sendApiDelayMs:s?s.enteredAt-x.enteredAt:null,
    sendApiTimerLatenessMs:s?s.enteredAt-x.dueAt:null,pendingMessages:x.pendingMessages};
});
const summarizePackets=a=>({scheduled:a.length,sent:a.filter(x=>x.sendApiAtMsFromRelayAnchor!==null).length,
    senderFrameRange:range(a.map(x=>x.senderFrame)),formalSenderFramePackets:a.filter(x=>x.senderFrame>=180&&x.senderFrame<raw.parameters.limit).length,
    scheduledWindowMsFromRelayAnchor:range(a.map(x=>x.scheduledAtMsFromRelayAnchor)),
    sendApiWindowMsFromRelayAnchor:range(a.map(x=>x.sendApiAtMsFromRelayAnchor).filter(finite)),
    plannedDelayMs:ms(a.map(x=>x.plannedDelayMs)),sendApiDelayMs:ms(a.map(x=>x.sendApiDelayMs)),
    sendApiTimerLatenessMs:ms(a.map(x=>x.sendApiTimerLatenessMs)),
    slowestSendApiRows:a.filter(x=>finite(x.sendApiDelayMs)).sort((a,b)=>b.sendApiDelayMs-a.sendApiDelayMs).slice(0,5)});
const burstRows=scheduledRows.filter(x=>x.burst);
const burstSummary=[0,1].map(sender=>({sender,destination:1-sender,...summarizePackets(burstRows.filter(x=>x.sender===sender))}));
const packetSummary=[0,1].map(sender=>{
  const a=scheduledRows.filter(x=>x.sender===sender);
  return {sender,destination:1-sender,all:summarizePackets(a),
    senderFrameBands:Object.fromEntries([
      ['before-formal',x=>x.senderFrame<180],['formal',x=>x.senderFrame>=180&&x.senderFrame<raw.parameters.limit],
      ['after-formal',x=>x.senderFrame>=raw.parameters.limit]
    ].map(([name,accept])=>[name,summarizePackets(a.filter(accept))]))};
});
const burstFrameRange=range(burstRows.map(x=>x.senderFrame));
const sides=raw.results.map((r,side)=>{
  const m=r.measure,p=r.responsiveness,all=m.samples.map((row,index)=>({row,index}));
  const formal=all.filter(x=>x.row[3]>=180&&x.row[3]<raw.parameters.limit),first=formal[0],last=formal.at(-1);
  if(!first||!last)throw new Error('No observed formal sample window for side '+side);
  const responseStart=first.row[0]+m.started-p.startedAt,responseEnd=last.row[0]+m.started-p.startedAt;
  const phaseAt=t=>!finite(t)?'not-observed':t<responseStart?'before-formal':t>responseEnd?'after-formal':'formal';
  const continuousStart=all.find(x=>x.row[3]>=180),continuousEnd=all.find(x=>x.row[3]>=raw.parameters.limit);
  const continuousAvailable=!!continuousStart&&!!continuousEnd&&continuousEnd.index>=continuousStart.index;
  const continuousStartMs=continuousAvailable?continuousStart.row[0]+m.started-p.startedAt:null;
  const continuousEndMs=continuousAvailable?continuousEnd.row[0]+m.started-p.startedAt:null;
  const continuousPhaseAt=t=>!continuousAvailable||!finite(t)?'not-observed':t<continuousStartMs?'before-formal':t>=continuousEndMs?'after-formal':'formal';
  const intents=p.intents.map(x=>({...x,
    scheduleToFirstWireMs:finite(x.firstWireAtMs)?x.firstWireAtMs-x.scheduledAtMs:null,
    submitToPresentationObservedMs:finite(x.submittedAtMs)&&finite(x.presentationObservedAtMs)?x.presentationObservedAtMs-x.submittedAtMs:null}));
  const formalIntents=intents.filter(x=>phaseAt(x.scheduledAtMs)==='formal');
  const beforeFormalIntents=intents.filter(x=>phaseAt(x.scheduledAtMs)==='before-formal');
  const afterFormalIntents=intents.filter(x=>phaseAt(x.scheduledAtMs)==='after-formal');
  const pacing=new Map(m.networkPacing.map(x=>[x[0],x]));
  const pacingSummary=rows=>({callbacks:rows.length,observed:rows.filter(x=>pacing.has(x.index)).length,
    missingRemoteInputs:dist(rows.map(x=>pacing.get(x.index)?.[2])),
    networkScaleMilli:dist(rows.map(x=>pacing.get(x.index)?.[3])),effectiveScaleMilli:dist(rows.map(x=>pacing.get(x.index)?.[4])),
    predictionFrontierRange:range(rows.map(x=>pacing.get(x.index)?.[5])),
    actualContiguousExclusiveRange:range(rows.map(x=>pacing.get(x.index)?.[6])),
    historicalReplayRemainingTicks:dist(rows.map(x=>pacing.has(x.index)?Math.max(0,pacing.get(x.index)[5]-x.row[3]):null)),
    callbacksWithMissingRemote:rows.filter(x=>pacing.get(x.index)?.[2]>0).length,
    scaledCallbacks:rows.filter(x=>pacing.get(x.index)?.[3]>1000).length,waitCallbacks:rows.filter(x=>x.row[1]===2).length});
  const differences=all.map((curr,index)=>{
    if(!index)return null;const prev=all[index-1],a=prev.row,b=curr.row;
    return {sampleIndex:index,previousSampleIndex:index-1,frameBefore:a[3],frameAfter:b[3],
      correctionsDelta:b[5]-a[5],resimulatedTicksDelta:b[6]-a[6],capturesDelta:b[7]-a[7],
      worldStateSavesDelta:finite(m.frameDetails[index]?.[1])&&finite(m.frameDetails[index-1]?.[1])?m.frameDetails[index][1]-m.frameDetails[index-1][1]:null};
  });
  const rollbackSummary=rows=>{
    const d=rows.map(x=>differences[x.index]).filter(Boolean);
    return {observedCallbackDeltas:d.length,firstSampleHasNoPredecessor:rows.some(x=>x.index===0),
      correctionsStarted:d.reduce((s,x)=>s+x.correctionsDelta,0),resimulatedTicks:d.reduce((s,x)=>s+x.resimulatedTicksDelta,0),
      worldStateSaves:dist(d.map(x=>x.worldStateSavesDelta)),
      callbacksWithCorrection:d.filter(x=>x.correctionsDelta>0).length,callbacksWithResimulation:d.filter(x=>x.resimulatedTicksDelta>0).length,
      resimulatedTicksPerCallback:dist(d.map(x=>x.resimulatedTicksDelta)),
      resimulatedTicksPerReplayingCallback:dist(d.filter(x=>x.resimulatedTicksDelta>0).map(x=>x.resimulatedTicksDelta)),
      observedFrameRegressions:d.filter(x=>x.frameAfter<x.frameBefore).length,
      counterAnomalies:d.filter(x=>['correctionsDelta','resimulatedTicksDelta','capturesDelta'].some(k=>!finite(x[k])||x[k]<0)||x.worldStateSavesDelta<0)};
  };
  const timeWindows=[['before-formal',all.slice(0,first.index)],['formal',all.slice(first.index,last.index+1)],['after-formal',all.slice(last.index+1)]];
  const continuousTimeWindows=continuousAvailable?[
    ['before-formal',all.slice(0,continuousStart.index)],['formal',all.slice(continuousStart.index,continuousEnd.index)],
    ['after-formal',all.slice(continuousEnd.index)]
  ]:[];
  let prev;const gaps=[];
  for(const curr of all)if((!prev||curr.row[11]!==prev.row[11])&&(!prev||curr.row[3]>prev.row[3])){
    if(prev){
      const interval=all.slice(prev.index+1,curr.index+1),start=prev.row,end=curr.row;
      gaps.push({fromSample:prev.index,toSample:curr.index,fromFrame:start[3],toFrame:end[3],rafGapMs:end[13]-start[13],
        observationGapMs:end[0]-start[0],responseAgeFromMs:start[0]+m.started-p.startedAt,responseAgeToMs:end[0]+m.started-p.startedAt,
        fromWindow:phaseAt(start[0]+m.started-p.startedAt),toWindow:phaseAt(end[0]+m.started-p.startedAt),
        continuousFromWindow:continuousPhaseAt(start[0]+m.started-p.startedAt),continuousToWindow:continuousPhaseAt(end[0]+m.started-p.startedAt),
        callbacks:interval.length,waitCallbacks:interval.filter(x=>x.row[1]===2).length,
        cppWorkMs:ms(interval.map(x=>x.row[2])),corrections:end[5]-start[5],resimulated:end[6]-start[6],captures:end[7]-start[7],
        saves:m.frameDetails[curr.index][1]-m.frameDetails[prev.index][1],pacing:pacingSummary(interval),
        sampleRows:interval.map(x=>({index:x.index,row:x.row,pacing:pacing.get(x.index),frameDetails:m.frameDetails[x.index]}))});
    }
    prev=curr;
  }
  const unresolved=intents.filter(x=>x.status!=='presented').map(x=>({intent:x,
    location:phaseAt(x.scheduledAtMs),continuousLocation:continuousPhaseAt(x.scheduledAtMs),
    submittedHoldBeforeSupersededMs:finite(x.supersededAtMs)&&finite(x.submittedAtMs)?x.supersededAtMs-x.submittedAtMs:null,
    neighboringIntents:intents.filter(y=>Math.abs(y.id-x.id)<=1),
    neighborClockAnchorMs:finite(x.submittedAtMs)?x.submittedAtMs:x.scheduledAtMs,
    neighboringCaptures:p.captures.filter(y=>Math.abs(y.firstWireAtMs-(finite(x.submittedAtMs)?x.submittedAtMs:x.scheduledAtMs))<50),
    neighboringSamples:all.filter(y=>Math.abs(y.row[0]+m.started-p.startedAt-(finite(x.submittedAtMs)?x.submittedAtMs:x.scheduledAtMs))<35)}));
  const windows=burstFrameRange?[
    ['before-burst-frame-band',burstFrameRange[0]-60,burstFrameRange[0]-1],
    ['burst-sender-frame-band',...burstFrameRange],
    ['after-burst-frame-band',burstFrameRange[1]+1,burstFrameRange[1]+60]
  ].map(([name,from,to])=>({name,frameRange:[from,to],...pacingSummary(formal.filter(x=>x.row[3]>=from&&x.row[3]<=to))})):[];
  const gapSummary=rows=>({rafTimestampGapMs:ms(rows.map(x=>x.rafGapMs)),observationGapMs:ms(rows.map(x=>x.observationGapMs))});
  const summarizeTimeWindow=([name,rows])=>({name,sampleIndices:rows.length?[rows[0].index,rows.at(-1).index]:null,
    responseClockRangeMs:range(rows.map(x=>x.row[0]+m.started-p.startedAt)),frameRange:range(rows.map(x=>x.row[3])),
    callbackWorkMs:ms(rows.map(x=>x.row[2])),
    consecutiveRafTimestampGapMs:ms(rows.slice(1).flatMap((x,i)=>x.row[13]!==rows[i].row[13]?[x.row[13]-rows[i].row[13]]:[])),
    pacing:pacingSummary(rows),rollback:rollbackSummary(rows)});
  return {side,cpuRate:raw.parameters.cpuThrottle?.ratesBySide?.[side],inputDelay:p.inputDelay,
    rawCounts:p.counts,responseValid:p.valid,responseErrors:p.errors,responseObservations:p.observations,
    formalWindow:{frameCriterion:[180,raw.parameters.limit],firstSampleIndex:first.index,lastSampleIndex:last.index,
      measureStartedAt:m.started,responseStartedAt:p.startedAt,browserObservationRange:[m.started+first.row[0],m.started+last.row[0]],
      responseClockRangeMs:[responseStart,responseEnd],cohortRule:'scheduledAtMs within first/last formal sample observations, inclusive; never select by successful capture/presentation'},
    fullSession:cohort(intents,phaseAt),formal:cohort(formalIntents,phaseAt),beforeFormal:cohort(beforeFormalIntents,phaseAt),
    afterFormal:cohort(afterFormalIntents,phaseAt),beforeFormalIntents,formalIntents,afterFormalIntents,unresolved,
    smoothness:raw.summary[side],pacing:{formal:pacingSummary(formal),burstFrameWindows:windows},
    observedTimeWindows:timeWindows.map(summarizeTimeWindow),
    continuousWindow:{available:continuousAvailable,
      rule:'Half-open response-clock interval from the first sample reaching frame >=180 to the first sample reaching frame >=limit; that terminal observation and all later samples stay in after-formal, even if rollback later lowers frame.',
      startSampleIndex:continuousStart?.index??null,firstReachedLimitSampleIndex:continuousEnd?.index??null,
      responseClockRangeMs:continuousAvailable?[continuousStartMs,continuousEndMs]:null,
      boundaryWitness:continuousAvailable?{start:{...continuousStart,pacing:pacing.get(continuousStart.index)},end:{...continuousEnd,pacing:pacing.get(continuousEnd.index)}}:null,
      boundaryElapsedMs:continuousAvailable?continuousEnd.row[0]-continuousStart.row[0]:null,
      boundaryLogicalHz:continuousAvailable?(continuousEnd.row[3]-continuousStart.row[3])*1000/(continuousEnd.row[0]-continuousStart.row[0]):null,
      cohorts:continuousAvailable?Object.fromEntries(['before-formal','formal','after-formal'].map(name=>[name,cohort(intents.filter(x=>continuousPhaseAt(x.scheduledAtMs)===name),continuousPhaseAt)])):null,
      intentIds:continuousAvailable?Object.fromEntries(['before-formal','formal','after-formal'].map(name=>[name,intents.filter(x=>continuousPhaseAt(x.scheduledAtMs)===name).map(x=>x.id)])):null,
      observedTimeWindows:continuousTimeWindows.map(summarizeTimeWindow),
      newStateWindows:continuousTimeWindows.map(([name])=>({name,...gapSummary(gaps.filter(x=>x.continuousFromWindow===name&&x.continuousToWindow===name))})),
      formalWithEndBoundaryNewStateGaps:continuousAvailable?gapSummary(gaps.filter(x=>x.fromSample>=continuousStart.index&&x.toSample<=continuousEnd.index)):null,
      crossingNewStateIntervals:gaps.filter(x=>x.continuousFromWindow!==x.continuousToWindow)},
    legacyFrameFilter:{
      rule:'Original formal/summary frame criterion is preserved. It may leave and re-enter after the first limit observation; skipped callback rows must not become a physical RAF stall.',
      excludedInsideFirstLast:all.slice(first.index,last.index+1).filter(x=>x.row[3]<180||x.row[3]>=raw.parameters.limit),
      selectedAfterFirstLimit:continuousEnd?formal.filter(x=>x.index>=continuousEnd.index):[]},
    rollback:{exactDepthObservable:false,exactDepthFrames:null,
      reason:'The frozen release samples contain cumulative correction/resimulation counters, but no restore target or per-correction replay span. Counter deltas, prediction lead and remaining replay are not rollback depth.',
      initialObservedCounters:all.length?{corrections:all[0].row[5],resimulatedTicks:all[0].row[6],worldStateSaves:m.frameDetails[0]?.[1]}:null,
      fullSession:rollbackSummary(all),formalFrameCriterion:rollbackSummary(formal),
      correctionAndReplayRows:differences.filter(x=>x&&(x.correctionsDelta||x.resimulatedTicksDelta||x.frameAfter<x.frameBefore)).map(x=>({...x,pacing:pacing.get(x.sampleIndex)}))},
    newStateIntervals:{fullSession:gapSummary(gaps),
      windows:timeWindows.map(([name])=>({name,...gapSummary(gaps.filter(x=>x.fromWindow===name&&x.toWindow===name))})),
      crossingWindows:gaps.filter(x=>x.fromWindow!==x.toWindow),
      over50ms:gaps.filter(x=>x.rafGapMs>50||x.observationGapMs>50),
      longestFullSession:[...gaps].sort((a,b)=>b.rafGapMs-a.rafGapMs).slice(0,5),
      terminalObservedWithoutNewState:prev?{fromSample:prev.index,toSample:all.at(-1).index,frame:prev.row[3],
        rafTimestampGapMs:all.at(-1).row[13]-prev.row[13],observationGapMs:all.at(-1).row[0]-prev.row[0],
        fromWindow:phaseAt(prev.row[0]+m.started-p.startedAt),toWindow:phaseAt(all.at(-1).row[0]+m.started-p.startedAt)}:null},
    longestNewStateIntervals:gaps.filter(x=>x.fromFrame>=180&&x.toFrame<raw.parameters.limit).sort((a,b)=>b.rafGapMs-a.rafGapMs).slice(0,5),
    finalIntent:intents.at(-1),rafDiagnosticsValid:r.rafDiagnostics.valid,rafDiagnosticsOverflow:r.rafDiagnostics.overflow,
    audio:r.audio,audioOutput:r.audioOutput,lifecycle:r.lifecycle,linearMemoryBytes:r.memoryBytes};
});
const evidence={schema:2,scope:'Offline interpretation of one frozen release relay run, steady delay or burst; no new acceptance threshold or causal performance attribution.',
  source:{report:basename(input),bytes:bytes.length,sha256:sha(bytes),analyzer:basename(fileURLToPath(import.meta.url)),analyzerSha256:sha(readFileSync(fileURLToPath(import.meta.url)))},
  passed:raw.passed,parameters:raw.parameters,identity:raw.identity,errors:raw.errors,
  final:{sameFrame:raw.sameFrame,comparable:raw.finalComparable,hashes:raw.results.map(x=>x.hash),confirmedChecks:raw.confirmedChecks},
  interpretation:[
    'Response formal cohort uses each page performance clock, offsetting responsiveness.startedAt against measure.started. Metrics condition on non-null observations and changed masks; all missing/status counts remain visible.',
    'Before-formal is observed active warmup and after-formal is the observed tail. Intents are grouped by schedule time; observationPhases separately retains where submission, first wire and presentation completed, including crossings of the formal boundary.',
    'The original formal/first-last-frame-filtered cohort is retained. continuousWindow adds a separate half-open first-180 to first-limit clock interval; its end observation remains an elapsed/state-gap boundary witness but is excluded from its callback distributions. A later rollback below limit cannot move tail observations back into this continuous formal window.',
    'firstWire is the JavaScript send observer after native capture/serialization, before transport send. It establishes neither remote ACK nor remote capture.',
    'Presentation is a newer simulation frontier with increased renderer submissions, observed after play.frame. It establishes neither pixel movement nor physical scanout.',
    'A superseded uncaptured intent is a missed synthetic edge. pendingAgeAtEnd is its historical age, not a continuously held input or a freeze.',
    'Relay selects kind-1 packets in both directions at scheduling. packetSummary includes steady delay and any burst; sendApi is the relay WebSocket API boundary, not peer delivery or native acceptance.',
    'Relay senderFrameBands and burst frame windows use sender frame annotations, not exact browser/relay wall-clock synchronization. No remote receipt or native input acceptance latency is inferred.',
    'Pacing distributions are sampled callback observations, not wall-time fractions. missingRemoteInputs comes from the live export, not from configured delay. historicalReplayRemainingTicks is max(0, predictionFrontier-currentFrame), not rollback depth.',
    'Rollback totals sum adjacent post-callback cumulative counter differences. The first sample has no predecessor. An interval beginning inside a window can have its prior sample outside; the frozen smoothness summary separately retains its original first/last subtraction. No per-correction depth can be recovered from these aggregates.',
    'observedTimeWindows includes every sample between the first and last formal observations, including historical replay below frame 180. The frozen smoothness summary uses its original per-sample frame criterion.',
    'The original frame-filtered summary can splice nonconsecutive callbacks if frame reaches limit and later rewinds below it. Its RAF/presentation gap values are preserved as historical outputs, not accepted as actual scheduling gaps; legacyFrameFilter lists the skipped and re-entered rows.',
    'New-state intervals use the frozen harness rAF timestamp convention; observation gaps and C++ work are separately retained. Full-session and boundary-crossing intervals prevent a formal frame filter from hiding warmup/tail gaps. over50ms retains observations for inspection and is not a new gate. No physical-presentation or CPU-time claim follows.'
  ],sides,
  relay:{valid:relay.valid,coverage:relay.coverage,injection:relay.injection,clock:relay.clock,counts:relay.counts,overflow:relay.overflow,errors:relay.errors,
    packetSummary,burstSummary,burstRows,pendingAtStop:relay.pending.map(x=>({scheduledId:x.scheduledId,plannedDelayMs:x.plannedDelayMs,burst:x.burst,dueAtMsFromRelayAnchor:relative(x.dueAt)})),
    timerTickLatenessMs:ms(relay.timing.map(x=>x[2])),timerTickGapMs:ms(relay.timing.map(x=>x[3]))},
  headed:{identityVerified:raw.headedControl.identityVerified,windows:raw.headedControl.windows},
  host:raw.hostLoad};
writeFileSync(resolve(output),JSON.stringify(evidence,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({output,bytes:readFileSync(output).length,sha256:sha(readFileSync(output)),sides:sides.map(x=>({side:x.side,
  cohorts:{beforeFormal:x.beforeFormal.planned,formal:x.formal.planned,afterFormal:x.afterFormal.planned},
  formalCaptured:x.formal.captured,formalPresented:x.formal.presented,
  scheduleToCapture:x.formal.conditionalLatencyMs.scheduleToFirstWireMs,
  scheduleToPresentation:x.formal.conditionalLatencyMs.scheduleToPresentationObservedMs,
  unresolved:x.unresolved.map(y=>({id:y.intent.id,location:y.location,holdMs:y.submittedHoldBeforeSupersededMs})),
  newState:x.newStateIntervals.fullSession,networkScale:x.pacing.formal.networkScaleMilli})),relayPackets:packetSummary.map(x=>({sender:x.sender,scheduled:x.all.scheduled,sent:x.all.sent,sendApiDelayMs:x.all.sendApiDelayMs}))}));
