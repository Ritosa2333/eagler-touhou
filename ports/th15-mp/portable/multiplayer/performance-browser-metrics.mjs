// Offline analysis of a fixed real-time window. No timers or game mutations.
export function distribution(values) {
  const sorted=values.filter(Number.isFinite).toSorted((a,b)=>a-b);
  const percentile=p=>sorted.length?sorted[Math.floor((sorted.length-1)*p)]:null;
  return {count:sorted.length,p50:percentile(.5),p95:percentile(.95),p99:percentile(.99),
    max:sorted.at(-1)??null,over33_4:sorted.filter(x=>x>33.4).length,
    over50:sorted.filter(x=>x>50).length,over100:sorted.filter(x=>x>100).length};
}

export function holds(times,start,end) {
  const boundaries=[start,...times.filter(t=>t>=start&&t<=end),end];
  return boundaries.slice(1).map((at,i)=>({begin:boundaries[i],end:at,ms:at-boundaries[i],
    startBoundary:i===0,endBoundary:i===boundaries.length-2}));
}

export function summarize(data) {
  const admitted=data.rows.filter(r=>r.native&&r.entry>=data.start&&r.entry<=data.end);
  const rows=data.rows.filter(r=>r.native&&r.entry>=data.start&&r.exit<=data.end);
  const crossing=data.rows.filter(r=>r.native&&r.entry<data.end&&r.exit>data.end);
  const base=data.startSnapshot,first=base.rollback,last=rows.at(-1)?.rollback??first;
  const baselineHigh=base.highFrame??first[2];let high=baselineHigh;const frontiers=[];
  for(const row of rows)if(row.rollback[2]>high){high=row.rollback[2];frontiers.push(row.exit);}
  const presents=data.presentations.filter(t=>t>=data.start&&t<=data.end);
  const presentHolds=holds(presents,data.start,data.end),frontierHolds=holds(frontiers,data.start,data.end);
  const notified=admitted.filter(r=>Number.isFinite(r.nativeMs));
  const noNotice=admitted.filter(r=>!Number.isFinite(r.nativeMs));
  const span=data.end-data.start;
  const peak=index=>Math.max(first[index],...rows.map(r=>r.rollback[index]));
  return {windowMs:span,measuredNativeCallbacks:admitted.length,completeNativeCallbacks:rows.length,crossingEndCallbacks:crossing.length,
    callbackWall:distribution(admitted.map(r=>r.exit-r.entry)),observerWall:distribution(admitted.map(r=>r.observerMs)),
    crossingCallbackWall:distribution(crossing.map(r=>r.exit-r.entry)),
    shellHookWall:distribution(notified.map(r=>r.shellMs)),
    nativeReportedWall:distribution(notified.map(r=>r.nativeMs)),nativeNotificationCoverage:{
      notified:notified.length,withoutNotification:noNotice.length,
      unnotifiedCallbackWall:distribution(noNotice.map(r=>r.exit-r.entry)),
      meaning:'Native-reported timing excludes callbacks without onGameFrame; complete callbackWall includes them.'},
    logicHz:(high-baselineHigh)*1000/span,captureHz:(last[3]-first[3])*1000/span,
    forwardFrontierFrames:high-baselineHigh,capturedFrames:last[3]-first[3],
    presentationHz:presents.length*1000/span,presentationSubmissions:presents.length,
    presentWindowHolds:distribution(presentHolds.map(x=>x.ms)),
    frontierWindowHolds:distribution(frontierHolds.map(x=>x.ms)),
    longestPresentationHolds:presentHolds.toSorted((a,b)=>b.ms-a.ms).slice(0,5),
    longestFrontierHolds:frontierHolds.toSorted((a,b)=>b.ms-a.ms).slice(0,5),
    rollbackDelta:{corrections:last[8]-first[8],historyTicks:last[9]-first[9],restores:last[11]-first[11],
      restoredFrameRecords:last[12]-first[12],barriers:last[10]-first[10]},
    correctingCallbacks:rows.filter(r=>r.rollback[1]!==0).length,
    retainedFramesMax:peak(5),undoKiBMax:peak(13),undoCapacityKiB:peak(14),
    simulationHeapUsedKiBMax:peak(15),simulationHeapCapacityKiB:peak(16),
    wasmCapacityBytesStart:base.wasmBytes,wasmCapacityBytesMax:Math.max(base.wasmBytes,...rows.map(r=>r.wasmBytes)),
    runtimePhases:[...new Set([base.nativePhase,...rows.map(r=>r.nativePhase)])],
    observedInputDelayFrames:[...new Set([base.net[7],...rows.map(r=>r.net[7])])],
    finalCompleteWindowStatus:{net:rows.at(-1)?.net??base.net,rollback:last},
    boundaryMeaning:'Start is sampled immediately before the window. End is fixed, not timer delivery time. Callback timing includes the full duration of every callback that starts inside the window, including end-crossing callbacks. Progress uses only callbacks completed before end. Frontier uses the maximum observed NextFrame, never counts rewinds as progress. Final gaps are censored at window end.',
    presentationMeaning:'Completed WebGL color blit into the game canvas default framebuffer, not GPU completion or monitor scanout.',
    memoryMeaning:'Simulation heap and Wasm linear-memory capacities, not total browser memory.',
    observations:{nativeCallbackTokens:data.nativeTokens,unknownCallbacks:data.rows.filter(r=>!r.native&&r.entry>=data.start&&r.entry<=data.end).length,
      presentationOutsideNativeCallback:data.presentationsOutsideNativeCallback},
  };
}

// Replay was already validated by the production native reader. This independently
// describes the confirmed physical input workload and common all-seat input prefix.
export function replayInputs(bytes,fromFrame=0,toFrame=Infinity) {
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  if(new TextDecoder().decode(bytes.subarray(0,8))!=='EAGLRPY1'||view.getUint32(8,true)!==1)throw Error('Unexpected Replay format');
  const players=bytes[20],frames=view.getUint32(32,true),prefix=40+view.getUint32(24,true)+8*view.getUint32(28,true);
  if(![2,3].includes(players)||prefix+frames*players*12!==bytes.length)throw Error('Unexpected Replay bounds');
  const begin=Math.max(0,fromFrame),end=Math.min(frames,toFrame);
  const seats=Array.from({length:players},(_,seat)=>({seat,frames:Math.max(0,end-begin),fire:0,left:0,right:0,distinctButtons:new Set()}));
  for(let frame=begin;frame<end;++frame)for(let seat=0;seat<players;++seat){
    const buttons=view.getUint16(prefix+(frame*players+seat)*12,true),row=seats[seat];
    row.fire+=!!(buttons&1);row.left+=!!(buttons&64);row.right+=!!(buttons&128);row.distinctButtons.add(buttons);
  }
  return {players,frames,prefix,begin,end,seats:seats.map(s=>({...s,distinctButtons:[...s.distinctButtons].sort((a,b)=>a-b)}))};
}
