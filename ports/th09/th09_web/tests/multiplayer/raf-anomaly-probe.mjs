// Serialized into the owned test page. No imports, timers, game calls, or frame scheduling.
export function installRafAnomalyProbe(options = {}) {
  if (options.enabled !== true) return {enabled:false,installed:false,reason:'disabled'};
  const key='rafAnomalyProbe';
  if (Object.hasOwn(globalThis,key)) return {enabled:true,installed:false,reason:'install-conflict'};
  const limits=Object.freeze({thresholdMs:250,prePolls:32,postPolls:32,newRequests:64,pending:4,
    controls:128,lifecycle:128,peers:4,errors:8,retainedBytes:256*1024,reservedBytes:32*1024});
  const finite=n=>typeof n==='number'&&Number.isFinite(n);
  const number=n=>finite(n)?n:null;
  const integer=n=>Number.isSafeInteger(n)&&n>=0;
  const value=(n,allowed)=>allowed.includes(n)?n:null;
  const copy=n=>JSON.parse(JSON.stringify(n));
  const errors=[],pre=[],post=[];
  const counts={polls:0,requestsScanned:0,requestsSkipped:0,pendingTruncated:0,controlsTruncated:0,
    lifecycleTruncated:0,peersTruncated:0,preEvicted:0,postOmitted:0,storageOmitted:0,maximumPollGapMs:0};
  let owner=null,cursor=0,last=null,episode=null,historyBytes=0,stopped=false,finalSnapshot=null;
  const error=code=>{if(!errors.includes(code)&&errors.length<limits.errors)errors.push(code);};
  const controlNames=['_th09_loop_start','_th09_loop_pause','_th09_loop_stop'];
  const terminalReasons=['cancelled','paused','stopped','restarted','hidden','owner-changed','probe-unavailable','request-unavailable'];

  function timeline(rows) {
    if(!Array.isArray(rows)) return null;
    if(rows.length>limits.controls){++counts.controlsTruncated;return null;}
    const result=[];let epoch=null,running=false,pause=null,interruptedAt=null,interruption=null;
    for(let index=0;index<rows.length;++index){
      const row=rows[index];
      if(!row||!finite(row.enteredAt)||!controlNames.includes(row.name))continue;
      if(row.name==='_th09_loop_start'){epoch=index+1;running=true;interruptedAt=row.enteredAt;interruption='restarted';}
      if(row.name==='_th09_loop_stop'){running=false;interruptedAt=row.enteredAt;interruption='stopped';}
      if(row.name==='_th09_loop_pause'){
        pause=number(row.argument);
        if(pause!==0){interruptedAt=row.enteredAt;interruption='paused';}
      }
      result.push({at:row.enteredAt,index,epoch,running,pause,interruptedAt,interruption});
    }
    return result;
  }
  function controlAt(rows,at){
    if(!rows)return null;
    let lo=0,hi=rows.length;
    while(lo<hi){const mid=(lo+hi)>>>1;if(rows[mid].at<=at)lo=mid+1;else hi=mid;}
    return lo?rows[lo-1]:null;
  }
  function hiddenBetween(rows,start,end){
    if(!Array.isArray(rows))return null;
    if(rows.length>limits.lifecycle){++counts.lifecycleTruncated;return null;}
    let hidden=false;
    for(const row of rows){
      if(!Array.isArray(row)||!finite(row[0])||row[0]>end)continue;
      if(row[0]<=start)hidden=row[2]!=='visible'||row[1]==='freeze'||row[1]==='pagehide';
      else if(row[2]!=='visible'||row[1]==='freeze'||row[1]==='pagehide')return true;
    }
    return hidden;
  }
  function compactRequest(row,diag,controls,at){
    if(!row||!integer(row.index)||!finite(row.requestedAt))return null;
    const parent=integer(row.parentIndex)?diag.requests[row.parentIndex]:null;
    const parentFrame=integer(parent?.lastFrameIndex)?diag.frames[parent.lastFrameIndex]:null;
    const callback=integer(row.callbackId)?diag.callbacks[row.callbackId]:null;
    const until=finite(row.enteredAt)?row.enteredAt:at;
    const initial=controlAt(controls,row.requestedAt),current=controlAt(controls,until);
    const eligibleAt=finite(parent?.exitedAt)?Math.max(row.requestedAt,parent.exitedAt):null;
    const hidden=hiddenBetween(globalThis.gameLifecycle,row.requestedAt,until);
    const reasons=[];
    if(!diag.valid)reasons.push('diagnostic-invalid');
    if(callback?.game!==true)reasons.push('not-game-callback');
    if(!parentFrame||parentFrame.activeAfter!==true||parentFrame.endedAfter!==false||!(parentFrame.sampleAfter>0))reasons.push('no-active-parent-sample');
    if(eligibleAt===null||until<eligibleAt)reasons.push('parent-not-exited');
    if(!initial||!current||initial.epoch===null)reasons.push('loop-epoch-unknown');
    else if(initial.epoch!==current.epoch||current.interruptedAt>row.requestedAt)reasons.push('control-interrupted');
    if(initial?.pause!==0||current?.pause!==0||!initial?.running||!current?.running)reasons.push('loop-not-running');
    if(hidden!==false)reasons.push(hidden===true?'hidden-interval':'lifecycle-unknown');
    if(row.cancelledAt!==null&&row.cancelledAt!==undefined)reasons.push('cancelled');
    if(row.requestThrew)reasons.push('request-threw');
    const active=globalThis.play?.active===true&&!globalThis.play?.finished&&!globalThis.play?.disconnected&&
      globalThis.measure?.started>0&&!globalThis.measure?.ended&&!globalThis.measure?.finished&&!globalThis.measure?.closed?.length;
    if(!finite(row.enteredAt)&&!active)reasons.push('harness-inactive');
    return {index:row.index,nativeId:number(row.nativeId),callbackId:number(row.callbackId),parentIndex:number(row.parentIndex),
      observedLoopEpoch:initial?.epoch??null,requestedAt:row.requestedAt,enteredAt:number(row.enteredAt),exitedAt:number(row.exitedAt),
      cancelledAt:number(row.cancelledAt),parentExitedAt:number(parent?.exitedAt),eligibleAt,
      requestAgeMs:Math.max(0,until-row.requestedAt),eligibleWaitMs:eligibleAt===null?null:Math.max(0,until-eligibleAt),
      frameCalls:number(row.frameCalls),pauseAtEntry:number(row.pauseAtEntry),pauseAtExit:number(row.pauseAtExit),
      parentWrapperMs:finite(parent?.enteredAt)&&finite(parent?.exitedAt)?parent.exitedAt-parent.enteredAt:null,
      parentSampleAfter:number(parentFrame?.sampleAfter),eligible:reasons.length===0,reasons};
  }
  function transportSnapshot(){
    const t=globalThis.__eaglerPeerTransport;
    if(!t||typeof t!=='object')return null;
    const length=Array.isArray(t.received)?t.received.length:null,head=integer(t.receivedHead)?t.receivedHead:0;
    const peers=[];
    if(t.peers instanceof Map){
      for(const [id,p] of t.peers){
        if(peers.length>=limits.peers){++counts.peersTruncated;break;}
        peers.push({id:integer(id)?id:null,
          connection:value(p?.pc?.connectionState,['new','connecting','connected','disconnected','failed','closed']),
          ice:value(p?.pc?.iceConnectionState,['new','checking','connected','completed','disconnected','failed','closed']),
          inputBufferedBytes:number(p?.inputDc?.bufferedAmount),controlBufferedBytes:number(p?.controlDc?.bufferedAmount),
          lastReceivedDateNow:number(p?.lastReceivedAt),lastInputSentDateNow:number(p?.lastInputSentAt)});
      }
    }
    return {route:value(t.route,['rtc','relay']),closed:!!t.closed,disconnected:!!t.disconnected,
      receivedArrayLength:length,receivedHead:head,queuedPackets:length===null?null:Math.max(0,length-head),
      relayBufferedBytes:number(t.relay?.bufferedAmount),pendingSignalCount:Array.isArray(t.pendingSignals)?t.pendingSignals.length:null,peers};
  }
  function store(observation,kind){
    const bytes=JSON.stringify(observation).length;
    if(kind==='pre'){
      while(pre.length&&(pre.length>=limits.prePolls||historyBytes+bytes>limits.retainedBytes-limits.reservedBytes)){
        const old=pre.shift();historyBytes-=old.bytes;++counts.preEvicted;
      }
    }else if(post.length>=limits.postPolls){++counts.postOmitted;return;}
    if(historyBytes+bytes>limits.retainedBytes-limits.reservedBytes){++counts.storageOmitted;return;}
    (kind==='pre'?pre:post).push({bytes,row:observation});historyBytes+=bytes;
  }
  function settle(reason,at,row=null){
    if(!episode||episode.outcome!=='pending')return;
    episode.outcome=reason;episode.outcomeObservedAt=at;
    episode.lastRequest=row??episode.lastRequest;
    if(reason==='entered'){
      episode.enteredAt=row.enteredAt;episode.waitMs=row.enteredAt-row.requestedAt;
      episode.eligibleWaitMs=row.eligibleWaitMs;
    }else if(terminalReasons.includes(reason))episode.censored=true;
  }
  function observe(input={}){
    if(stopped)return {enabled:true,status:'stopped',polls:counts.polls,episode:episode?{outcome:episode.outcome,requestIndex:episode.requestIndex}:null};
    const before=performance.now(),origin=number(performance.timeOrigin),wall=Date.now();
    const probe=globalThis.rafDiagnosticProbe,play=globalThis.play,measure=globalThis.measure;
    if(!probe||typeof probe.peek!=='function'||!play||!measure){error('required-owner-unavailable');settle('probe-unavailable',before);return {enabled:true,status:'unavailable'};}
    if(!owner)owner={probe,play,measure,core:globalThis.core};
    if(owner.probe!==probe||owner.play!==play||owner.measure!==measure||owner.core!==globalThis.core){
      error('owner-changed');settle('owner-changed',before);stopped=true;return {enabled:true,status:'owner-changed'};
    }
    let d;
    try{d=probe.peek();}catch{error('peek-failed');settle('probe-unavailable',before);return {enabled:true,status:'unavailable'};}
    if(!d||!Array.isArray(d.requests)||!Array.isArray(d.frames)||!Array.isArray(d.callbacks)||!Array.isArray(d.pendingIndices)){
      error('peek-schema-invalid');return {enabled:true,status:'unavailable'};
    }
    const controls=timeline(d.controls);
    const at=number(d.snapshotAt)??before,current=controlAt(controls,at);
    const pending=[];
    for(let i=0;i<Math.min(d.pendingIndices.length,limits.pending);++i){
      const row=compactRequest(d.requests[d.pendingIndices[i]],d,controls,at);if(row)pending.push(row);
    }
    if(d.pendingTruncated||d.pendingIndices.length>limits.pending)++counts.pendingTruncated;
    if(!d.valid)error('raf-diagnostic-invalid');
    if(cursor>d.requests.length){error('request-array-rewound');cursor=d.requests.length;}
    const begin=Math.max(cursor,d.requests.length-limits.newRequests);counts.requestsSkipped+=begin-cursor;
    const completed=[];
    for(let i=begin;i<d.requests.length;++i){
      ++counts.requestsScanned;
      const raw=d.requests[i];
      if(finite(raw?.enteredAt)&&raw.enteredAt-raw.requestedAt>=limits.thresholdMs){
        const row=compactRequest(raw,d,controls,at);if(row)completed.push(row);
      }
    }
    cursor=d.requests.length;
    // The last request may still be pending in one poll and enter before the next.
    // Keep its array identity independently of the new-request cursor.
    if(last)for(const prior of last.pending){
      const raw=d.requests[prior.index];
      if(finite(raw?.enteredAt)&&raw.index===prior.index&&raw.requestedAt===prior.requestedAt&&
          raw.enteredAt-raw.requestedAt>=limits.thresholdMs&&!completed.some(r=>r.index===raw.index)){
        const row=compactRequest(raw,d,controls,at);if(row)completed.push(row);
      }
    }
    const sample=Array.isArray(measure.samples)&&measure.samples.length?measure.samples[measure.samples.length-1]:null;
    const stats=Array.isArray(input.stats)?input.stats.slice(0,8).map(number):null;
    const observation={poll:counts.polls++,clock:{name:'Page performance.now()',timeOrigin:origin,before,observedAt:at,
        after:null,dateNow:wall,dateNowScope:'RTC wall-clock stamps only; not interchangeable with performance.now'},
      lifecycle:{visible:globalThis.document?.visibilityState==='visible',active:play.active===true,connected:play.connected===true,
        recovering:!!play.recovering,finished:!!play.finished,measureStarted:number(measure.started),measureEnded:number(measure.ended),
        closedCount:Array.isArray(measure.closed)?measure.closed.length:null,pauseRequested:number(d.lastPauseRequested),observedLoopEpoch:current?.epoch??null},
      game:{sampleCount:Array.isArray(measure.samples)?measure.samples.length:null,lastSampleAt:sample?number(measure.started+sample[0]):null,
        next:sample?number(sample[3]):null,confirmed:sample?number(sample[4]):null,corrections:sample?number(sample[5]):null,
        resimulated:sample?number(sample[6]):null,captures:sample?number(sample[7]):null,present:sample?number(sample[11]):null,
        phase:sample?number(sample[12]):null,polledStats:stats},
      raf:{requested:number(d.counts?.requested),entered:number(d.counts?.entered),exited:number(d.counts?.exited),
        cancelled:number(d.counts?.cancelled),gameFrames:number(d.counts?.gameFrames),pendingCount:number(d.pendingCount),
        pendingTruncated:!!d.pendingTruncated,valid:d.valid===true},pending,transport:transportSnapshot()};
    if(last)counts.maximumPollGapMs=Math.max(counts.maximumPollGapMs,Math.max(0,at-last.clock.observedAt));
    if(!episode){
      const candidate=[...pending,...completed].filter(r=>r.eligible&&r.eligibleWaitMs>=limits.thresholdMs)
        .sort((a,b)=>a.requestedAt-b.requestedAt||a.index-b.index)[0];
      if(candidate){
        episode={kind:candidate.enteredAt===null?'observed-pending':'completed-before-poll',requestIndex:candidate.index,
          observedLoopEpoch:candidate.observedLoopEpoch,requestedAt:candidate.requestedAt,detectedAt:at,
          outcome:'pending',outcomeObservedAt:null,censored:false,enteredAt:null,waitMs:null,eligibleWaitMs:null,
          trigger:copy(candidate),lastRequest:copy(candidate),lastPendingObservedAt:candidate.enteredAt===null?at:null};
        if(candidate.enteredAt!==null)settle('entered',at,candidate);
      }
    }
    if(episode&&episode.outcome==='pending'){
      const raw=d.requests[episode.requestIndex];
      const row=compactRequest(raw,d,controls,at);
      if(!row||row.index!==episode.requestIndex||row.requestedAt!==episode.requestedAt)settle('request-unavailable',at);
      else{
        episode.lastRequest=copy(row);
        // A stop/pause/restart before entry censors the natural pending interval.
        const observedEnd=controlAt(controls,row.enteredAt??at);
        if(observedEnd?.epoch!==episode.observedLoopEpoch)settle('restarted',at,row);
        else if(observedEnd?.interruptedAt>episode.requestedAt)settle(observedEnd.interruption??'stopped',at,row);
        else if(hiddenBetween(globalThis.gameLifecycle,episode.requestedAt,row.enteredAt??at)===true)settle('hidden',at,row);
        else if(row.cancelledAt!==null)settle('cancelled',at,row);
        else if(row.enteredAt!==null)settle('entered',at,row);
        else episode.lastPendingObservedAt=at;
      }
    }
    observation.clock.after=performance.now();
    store(observation,episode?'post':'pre');last=observation;
    return {enabled:true,status:episode?'latched':'armed',poll:observation.poll,clock:observation.clock,
      episode:episode?{kind:episode.kind,requestIndex:episode.requestIndex,observedLoopEpoch:episode.observedLoopEpoch,
        detectedAt:episode.detectedAt,outcome:episode.outcome,lastPendingObservedAt:episode.lastPendingObservedAt}:null,
      pendingCount:observation.raf.pendingCount,largestEligiblePendingMs:pending.reduce((m,r)=>r.eligible?Math.max(m,r.eligibleWaitMs):m,0)};
  }
  function snapshot(input={}){
    if(finalSnapshot)return finalSnapshot;
    if(input.final===true&&!stopped)observe(input);
    if(input.final===true){stopped=true;if(episode?.outcome==='pending')episode.censored=true;}
    const result={version:1,enabled:true,side:[0,1].includes(options.side)?options.side:null,stopped,limits,
      scope:'Poll observations of an existing game RAF chain. No new frames, timers, transport processing, trace capture, or production fallback. The 250 ms trigger is diagnostic, not a smoothness acceptance threshold.',
      clocks:{page:'Page performance.now() with performance.timeOrigin',rtc:'Date.now() wall clock',
        pollGaps:'Gaps between observations are not proof that RAF or JavaScript was blocked; only an identified request supplies wait evidence.'},
      valid:errors.length===0,coverageComplete:errors.length===0&&!counts.requestsSkipped&&!counts.pendingTruncated&&
        !counts.controlsTruncated&&!counts.lifecycleTruncated&&!counts.peersTruncated&&!counts.storageOmitted,
      counts:{...counts},errors:[...errors],episode,history:{pre:pre.map(r=>r.row),post:post.map(r=>r.row),retainedJsonBytes:historyBytes},
      finalObservation:last,limitations:['Received head and array length can reset during compaction; neither is a cumulative receive/poll counter.',
        'Queue and bufferedAmount observations do not provide packet arrival, delivery, native acceptance, or physical scanout timestamps.',
        'A pending request observed after pause/stop remains in finalObservation; its active interval is censored, not automatically a game freeze.',
        'Pre/post history is at most 32 actual polls each, not a guaranteed duration; omitted polls and final pending are retained explicitly.']};
    const detached=copy(result);if(input.final===true)finalSnapshot=detached;return detached;
  }
  Object.defineProperty(globalThis,key,{value:Object.freeze({observe,snapshot}),configurable:true,writable:false});
  return {enabled:true,installed:true,limits};
}
