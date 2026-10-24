// Post-run analysis only. Never add clocks, GPU synchronization or work to the
// measured callback. All times below belong to ONE document performance clock.
const diagnosticFlags=[
  ['nativeProfile','diagnostic-native-profile-run-not-acceptance'],
  ['audioProfile','diagnostic-audio-profile-run-not-acceptance'],
  ['cpuSamples','diagnostic-cpu-profile-run-not-acceptance'],
  ['stationary','diagnostic-stationary-run-not-movement-acceptance'],
];

export function diagnosticModes(settings){return diagnosticFlags.filter(([flag])=>settings[flag]).map(([flag])=>flag);}

export function assessPerformance(results,settings){
  const diagnostics=diagnosticModes(settings),maximumMs=settings.maxGapMs;
  const peers=results.map(({seat,metrics})=>({seat,checks:[
    ['nativeReportedWall.max',metrics?.nativeReportedWall?.max],
    ['presentWindowHolds.max',metrics?.presentWindowHolds?.max],
    ['frontierWindowHolds.max',metrics?.frontierWindowHolds?.max],
  ].map(([metric,value])=>({metric,value:Number.isFinite(value)?value:null,maximumMs,
    passed:Number.isFinite(value)&&value>=0&&value<=maximumMs}))}));
  const failures=diagnosticFlags.filter(([flag])=>settings[flag]).map(([,kind])=>({kind}));
  if(peers.length!==2)failures.push({kind:'performance-peer-coverage',expected:2,observed:peers.length});
  for(const peer of peers)if(peer.checks.some(check=>!check.passed))failures.push({
    seat:peer.seat,kind:'performance-threshold',nativeMax:peer.checks[0].value,
    presentMax:peer.checks[1].value,frontierMax:peer.checks[2].value,
    invalidMetrics:peer.checks.filter(check=>check.value===null||check.value<0).map(check=>check.metric),
  });
  const thresholdsMet=peers.length===2&&peers.every(peer=>peer.checks.every(check=>check.passed));
  return {diagnosticOnly:diagnostics.length>0,diagnostics,peers,thresholdsMet,
    performancePassed:!diagnostics.length&&thresholdsMet,failures};
}

// Keep the established counter-change rule: the first row supplies a baseline,
// and rewound simulation frames do not become new forward-frontier events.
export function progressEvents(rows){
  let lastPresent=null,highFrame=null;const presents=[],frontiers=[];
  rows.forEach((row,index)=>{
    if(row.rollback?.length===24){
      const value=row.rollback[21];
      if(lastPresent!==null&&value>lastPresent)presents.push({at:row.exit,index,id:row.id,before:lastPresent,after:value});
      lastPresent=value;
    }
    if(row.net?.length===24){
      const value=row.net[3];
      if(highFrame!==null&&value>highFrame)frontiers.push({at:row.exit,index,id:row.id,before:highFrame,after:value});
      highFrame=Math.max(highFrame??0,value);
    }
  });
  return {presents,frontiers};
}

export function windowHolds(events,start,end){
  const inside=events.filter(event=>event.at>=start&&event.at<=end);
  const boundaries=[{at:start},...inside,{at:end}];
  return boundaries.slice(1).map((to,index)=>({begin:boundaries[index].at,end:to.at,
    durationMs:to.at-boundaries[index].at,fromEvent:index?boundaries[index]:null,
    toEvent:index<inside.length?to:null,
    startBoundary:index===0,endBoundary:index===inside.length}));
}

function callbackDetails(row,index,rows){
  const previous=rows[index-1],wallMs=row.exit-row.entry;
  return {id:row.id,kind:row.kind??'raf',requested:row.requested,rafTimestamp:row.stamp,
    entry:row.entry,exit:row.exit,wallMs,nativeReportedMs:row.nativeMs,
    shellHookMs:row.shellHookMs??0,observerMs:row.observerMs,
    otherCallbackMs:wallMs-row.nativeMs-(row.shellHookMs??0)-row.observerMs,
    requestToEntryMs:row.entry-row.requested,rafTimestampToEntryMs:row.entry-row.stamp,
    previousCallback:previous?{id:previous.id,exit:previous.exit}:null,
    previousExitToEntryMs:previous?row.entry-previous.exit:null,
    forwardTicks:row.nativeTicks,frame:row.net?.[3],presentations:row.rollback?.[21],
    correcting:row.rollback?.[3],networkWait:row.rollback?.[15],cpuYield:row.rollback?.[16],
    historyTicksDelta:previous?.rollback&&row.rollback?row.rollback[11]-previous.rollback[11]:null,
    ...(row.profile?{nativeProfile:row.profile}:{})};
}

function transportDuring(transport,begin,end,limit){
  const within=value=>Number.isFinite(value)&&value>=begin&&value<=end;
  const sends=transport.filter(event=>event.phase==='measure'&&Number.isFinite(event.scheduled)&&
    event.scheduled<=end&&(!Number.isFinite(event.actual)||event.actual>=begin));
  const delivered=sends.filter(event=>Number.isFinite(event.actual)&&Number.isFinite(event.lateness));
  const receives=transport.filter(event=>event.phase==='measure'&&event.kind==='receive'&&within(event.at));
  return {inFlightDuringGap:sends.length,dueWithinGap:sends.filter(event=>within(event.due)).length,
    deliveredWithinGap:delivered.filter(event=>within(event.actual)).length,
    pendingAtGapEnd:sends.filter(event=>!Number.isFinite(event.actual)||event.actual>end).length,
    maxObservedLatenessMs:delivered.length?Math.max(...delivered.map(event=>event.lateness)):null,
    worstObservedTimers:delivered.toSorted((a,b)=>b.lateness-a.lateness).slice(0,limit).map(event=>({
      id:event.id,kind:event.kind,scheduled:event.scheduled,due:event.due,actual:event.actual,
      requestedDelay:event.requestedDelay,lateness:event.lateness,
      dueWithinGap:within(event.due),deliveredWithinGap:within(event.actual)})),
    receives:{count:receives.length,first:receives[0]?.at??null,last:receives.at(-1)?.at??null}};
}

function explainGap(span,rows,transport,limit){
  const callbacks=[];let coveredMs=0,coveredThrough=span.begin;
  const complete={count:0,wallMs:0,nativeReportedMs:0,shellHookMs:0,observerMs:0,otherCallbackMs:0};
  let clippedMs=0;
  rows.forEach((row,index)=>{
    const begin=Math.max(row.entry,span.begin),end=Math.min(row.exit,span.end);
    if(end<=begin)return;
    coveredMs+=Math.max(0,end-Math.max(begin,coveredThrough));coveredThrough=Math.max(coveredThrough,end);
    const clipped=begin!==row.entry||end!==row.exit,detail=callbackDetails(row,index,rows);
    callbacks.push({...detail,overlapMs:end-begin,clipped});
    if(clipped)clippedMs+=end-begin;
    else{++complete.count;for(const key of ['wallMs','nativeReportedMs','shellHookMs','observerMs','otherCallbackMs'])complete[key]+=detail[key];}
  });
  return {...span,insideObservedCallbacksMs:coveredMs,outsideObservedCallbacksMs:span.durationMs-coveredMs,
    completeCallbacks:complete,clippedCallbackOverlapMs:clippedMs,callbacks,
    transport:transportDuring(transport,span.begin,span.end,limit)};
}

export function timingTails(rows,{start,end,transport=[],thresholdMs,limit=5},progress=progressEvents(rows)){
  const summarize=events=>{
    const spans=windowHolds(events,start,end);
    return {count:spans.length,overThreshold:spans.filter(span=>span.durationMs>thresholdMs).length,
      worst:spans.toSorted((a,b)=>b.durationMs-a.durationMs).slice(0,limit).map(span=>explainGap(span,rows,transport,limit))};
  };
  return {schema:'th11-callback-gap-diagnostics/1',observationalOnly:true,thresholdMs,retainedPerKind:limit,
    meaning:{
      clock:'One document performance clock; no peer-clock subtraction.',
      native:'Wall time through native work, GL submission and audio; not CPU thread time or GPU completion.',
      presentation:'Counter changes observed at callback exit; not GPU completion or display scanout.',
      outsideCallbacks:'Time outside the observed native callbacks may contain other tasks, normal frame scheduling or process descheduling; no cause is assigned.',
      waits:'Request-to-entry, timestamp-to-entry and previous-exit-to-entry overlap; never add them to the gap partition.',
      edges:'The first row initializes counters, so the start segment may include an unrecognized first presentation/frontier event. End segments are censored at the configured end.',
      clipped:'Native/shell/observer totals cover complete callbacks only; a callback crossing the window edge contributes its overlap separately.',
      timers:'Same-page injected send timers overlapping the gap; a late delivery may occur after the gap. Timing overlap does not establish causality.',
      phases:'Native profile bins may nest (for example exactTick contains update/draw); they are context, not additive components.',
    },
    presentation:summarize(progress.presents),frontier:summarize(progress.frontiers),
    nativeCallbacks:{count:rows.length,overThreshold:rows.filter(row=>row.nativeMs>thresholdMs).length,
      worst:rows.map((row,index)=>({row,index})).toSorted((a,b)=>b.row.nativeMs-a.row.nativeMs).slice(0,limit)
        .map(({row,index})=>({...callbackDetails(row,index,rows),transport:transportDuring(transport,row.entry,row.exit,limit)}))}};
}
