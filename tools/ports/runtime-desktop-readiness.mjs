import {validateDesktopSfxInputWindow} from './runtime-desktop-audio.mjs';

export function evaluateDesktopGameplayReadiness(state,game){
  const reasons=[];
  if(state?.capabilities?.input!==true||!Number.isFinite(state?.input?.value))reasons.push('native-input-unavailable');
  if(state?.capabilities?.position!==true||!Number.isFinite(state?.position?.x)||!Number.isFinite(state?.position?.y))
    reasons.push('native-player-position-unavailable');
  if(state?.capabilities?.pause===true&&state.pause?.active!==false)reasons.push('gameplay-not-unpaused');
  if(state?.nativeObservation){
    if(state.localSeat!==0)reasons.push('local-seat-unavailable');
    if(!Number.isFinite(state.frames?.simulation))reasons.push('native-simulation-frame-unavailable');
  }
  const phaseReady=state?.capabilities?.phase===true
    ?['stage','match'].includes(state.phase?.name)
    :game==='th08'&&state?.status?.supervisorActive===2&&state.status.active===1&&!state.status.loading;
  if(!phaseReady)reasons.push('native-gameplay-phase-unavailable');
  if(game==='th09')reasons.push(...th09TouchFireEligibility(state).reasons);
  return{ready:reasons.length===0,reasons};
}

export function incompleteGameplayPrecondition(state,game){
  const readiness=evaluateDesktopGameplayReadiness(state,game);
  if(readiness.ready)return null;
  return{checkStatus:'incomplete',reason:`Native gameplay eligibility was not established: ${readiness.reasons.join(', ')}.`,
    eligibility:readiness,precondition:state};
}

export function th09TouchFireEligibility(state){
  const touch=state?.touchInput;
  const ready=touch?.context==='gameplay'&&touch.ready===true&&touch.inTitle===false;
  return{ready,reasons:ready?[]:[
    ...(touch?.context!=='gameplay'?['touch-context-not-gameplay']:[]),
    ...(touch?.ready!==true?['native-touch-player-not-ready']:[]),
    ...(touch?.inTitle!==false?['title-context-active']:[]),
  ]};
}

export function incompleteTh09TouchPrecondition(state){
  const eligibility=th09TouchFireEligibility(state);
  if(eligibility.ready)return null;
  return{checkStatus:'incomplete',reason:`TH09 touch fire eligibility was not established: ${eligibility.reasons.join(', ')}.`,
    eligibility,precondition:state};
}

function validateSfxOutputRoute(route,fromFrame,toFrame){
  if(route?.available!==true||route.fromFrame!==fromFrame||route.toFrame!==toFrame||route.drops!==0||!Array.isArray(route.events)||
    route.events.length===0||!Number.isSafeInteger(fromFrame)||!Number.isSafeInteger(toFrame)||toFrame<=fromFrame)return null;
  let state=null,disconnected=false,previousFrame=-1;const statesByFrame=new Map();
  for(const event of route.events){
    if(!Number.isSafeInteger(event?.contextFrame)||event.contextFrame<previousFrame||typeof event.connected!=='boolean')return null;
    previousFrame=event.contextFrame;
    const states=statesByFrame.get(event.contextFrame)??new Set();states.add(event.connected);statesByFrame.set(event.contextFrame,states);
    if(states.size>1&&event.contextFrame>=fromFrame&&event.contextFrame<toFrame)return null;
    if(event.contextFrame<=fromFrame)state=event.connected;
    if(event.contextFrame>=fromFrame&&event.contextFrame<toFrame&&!event.connected)disconnected=true;
  }
  if(state===null)return null;
  const connected=state&&!disconnected;
  return route.connected===connected?connected:null;
}

function hasCompleteSfxPcmInterval(history,fromFrame,toFrame){
  if(!Number.isSafeInteger(fromFrame)||!Number.isSafeInteger(toFrame)||toFrame<=fromFrame||history?.available!==true||history.covered!==true||
    history.fromFrame!==fromFrame||history.toFrame!==toFrame||history.frames!==toFrame-fromFrame||
    !Number.isSafeInteger(history.renderedThroughFrame)||history.renderedThroughFrame<toFrame||
    !Number.isSafeInteger(history.retainedStartFrame)||history.retainedStartFrame>fromFrame||
    !Number.isSafeInteger(history.gaps)||history.gaps<0||!Number.isSafeInteger(history.gapDrops)||history.gapDrops<0||
    !Number.isSafeInteger(history.gapRangeLimit)||history.gapRangeLimit<1||!Array.isArray(history.gapRanges)||
    history.gapRanges.length>history.gapRangeLimit||history.gaps!==history.gapRanges.length+history.gapDrops||
    history.gapRanges.some(gap=>!Number.isSafeInteger(gap?.fromFrame)||gap.fromFrame<0||
      !Number.isSafeInteger(gap?.toFrame)||gap.toFrame<=gap.fromFrame)||
    !Array.isArray(history.overlappingGapRanges)||!Number.isSafeInteger(history.overwrittenFrames)||history.overwrittenFrames<0||
    !Number.isSafeInteger(history.capacityFrames)||history.capacityFrames<=0||
    !Number.isSafeInteger(history.nonZeroFrames)||history.nonZeroFrames<0||history.nonZeroFrames>toFrame-fromFrame||
    !Number.isFinite(history.peakAbs)||history.peakAbs<0||
    (history.nonZeroFrames===0)!==(history.peakAbs===0)||
    (history.nonZeroFrames===0)!==(history.firstNonZeroFrame===null))return false;
  const overlaps=history.gapRanges.filter(gap=>gap.fromFrame<toFrame&&gap.toFrame>fromFrame);
  if(history.gapDrops!==0||overlaps.length!==0||history.coverageReason!==null||
    overlaps.length!==history.overlappingGapRanges.length||
    overlaps.some((gap,index)=>gap.fromFrame!==history.overlappingGapRanges[index]?.fromFrame||
      gap.toFrame!==history.overlappingGapRanges[index]?.toFrame))return false;
  return history.firstNonZeroFrame===null||
    (Number.isSafeInteger(history.firstNonZeroFrame)&&history.firstNonZeroFrame>=fromFrame&&history.firstNonZeroFrame<toFrame);
}

function sfxPcmIntervalIssue(voice){
  const fromFrame=voice?.sourceStartFrame,toFrame=voice?.sourceEndFrame,history=voice?.pcmHistory;
  const identity={voiceId:voice?.id??null,sourceNodeId:voice?.sourceNodeIdentity?.nodeId??null,
    outputNodeId:voice?.outputSourceIdentity?.nodeId??null,tapId:voice?.outputTapIndex??null,fromFrame,toFrame};
  const issue=(code,detail,extra={})=>({code,...identity,detail,...extra});
  const describe=()=>`voice ${String(identity.voiceId)} sourceNode=${String(identity.sourceNodeId)} outputNode=${String(identity.outputNodeId)} `+
    `tap=${String(identity.tapId)} frames=[${String(fromFrame)},${String(toFrame)})`;
  if(!Number.isSafeInteger(fromFrame)||!Number.isSafeInteger(toFrame)||toFrame<fromFrame)
    return issue('invalid-source-interval',describe()+' has a missing or reversed rendered-frame interval.');
  if(toFrame===fromFrame)
    return issue('zero-render-interval',describe()+` has zero rendered frames; stopEventCount=${String(voice?.stopEventCount??'<unknown>')} ended=${String(voice?.ended===true)}.`);
  if(history?.available!==true)
    return issue('pcm-history-unavailable',describe()+` has no complete PCM history: ${String(history?.reason??'history unavailable')}.`);
  const overlapping=Array.isArray(history.gapRanges)?history.gapRanges.filter(gap=>Number.isSafeInteger(gap?.fromFrame)&&
    Number.isSafeInteger(gap?.toFrame)&&gap.fromFrame<toFrame&&gap.toFrame>fromFrame):[];
  if(Number.isSafeInteger(history.gapDrops)&&history.gapDrops>0)
    return issue('pcm-gap-index-overflow',describe()+` cannot be verified because the bounded PCM gap index dropped ${history.gapDrops} gap record(s).`,
      {gapDrops:history.gapDrops,gapRangeLimit:history.gapRangeLimit});
  if(overlapping.length)
    return issue('pcm-gap-intersects-source',describe()+' intersects a discontinuous rendered-history interval.',
      {overlappingGapRanges:overlapping});
  if(history.covered!==true)
    return issue('pcm-range-not-covered',describe()+` is not fully covered: ${String(history.coverageReason??'coverage unavailable')}.`,
      {coverageReason:history.coverageReason??null,retainedStartFrame:history.retainedStartFrame,
       renderedThroughFrame:history.renderedThroughFrame,gapCount:history.gaps??null});
  if(!hasCompleteSfxPcmInterval(history,fromFrame,toFrame))
    return issue('pcm-interval-malformed',describe()+' has inconsistent rendered-frame coverage metadata.');
  return null;
}

export function correlateSfxWindow(baselineWindow,actionWindow){
  const baseline=baselineWindow?.nativeSfx,action=actionWindow?.nativeSfx;
  const unavailable=(reason)=>({status:'incomplete',reason,baselineAvailable:baseline?.available===true,
    actionAvailable:action?.available===true,newVoiceActivations:[],correlatedVoice:null,correlatedPcm:null,correlated:false});
  if(baseline?.available!==true||action?.available!==true)
    return unavailable('Native SFX voice observation is unavailable in the baseline or action window.');
  if(!Array.isArray(baseline.snapshots?.end)||!Array.isArray(action.activationEvents)||!Array.isArray(action.activeObservations))
    return unavailable('Native SFX voice activation samples are incomplete.');
  const previous=new Map(baseline.snapshots.end.map(voice=>[voice.id,voice]));
  const activations=action.activationEvents.filter(voice=>{
    const prior=previous.get(voice.id);
    return !prior||(!(prior.playing||prior.node)||prior.started!==voice.started);
  });
  const inputWindow=actionWindow?.actionWindow;
  if(inputWindow&&(!Number.isFinite(inputWindow.startElapsedMs)||!Number.isFinite(inputWindow.endElapsedMs)||
    inputWindow.endElapsedMs<inputWindow.startElapsedMs||activations.some(voice=>!Number.isFinite(voice.elapsedMs))))
    return unavailable('Native SFX activation timing cannot be aligned with the touch action window.');
  const selected=inputWindow?activations.filter(voice=>voice.elapsedMs>=inputWindow.startElapsedMs&&
    voice.elapsedMs<=inputWindow.endElapsedMs):activations;
  if(!selected.length)return{status:'failed',reason:'No new native SFX voice activation was observed during the touch action window.',
    baselineAvailable:true,actionAvailable:true,newVoiceActivations:[],correlatedVoice:null,correlatedPcm:null,correlated:false};
  const evidence=selected.map(voice=>{
    const observations=action.activeObservations.filter(item=>item.id===voice.id&&item.started===voice.started&&
      item.outputTapIndex===voice.outputTapIndex),samples=[voice,...observations].filter(item=>item.pcm).map(item=>({
      elapsedMs:Number.isFinite(item.elapsedMs)?item.elapsedMs:null,nonZero:item.pcm.nonZero??0,
      peakAbs:item.pcm.peakAbs??0,rms:item.pcm.rms??0}));
    const pcm={tapIndex:voice.outputTapIndex,captures:samples.length,nonZeroSamples:samples.reduce((sum,row)=>sum+row.nonZero,0),
      peakAbs:samples.reduce((peak,row)=>Math.max(peak,row.peakAbs),0),rmsMax:samples.reduce((peak,row)=>Math.max(peak,row.rms),0),samples};
    return{voice,pcm,mapped:Number.isInteger(voice.outputTapIndex)&&voice.outputTapIndex>=0};
  });
  if(selected.length>1)return{status:'incomplete',reason:'Multiple new native SFX voices were observed in the touch action window.',
    baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,correlatedVoice:null,correlatedPcm:null,correlated:false};
  const mapped=evidence.filter(item=>item.mapped);
  if(!mapped.length)return unavailable('New native SFX voice activation was observed, but no matching WebAudio output tap was available.');
  const voice=evidence[0].voice,from=voice.sourceStartFrame,to=voice.sourceEndFrame,history=voice.pcmHistory,route=voice.outputRoute,
    routeConnected=validateSfxOutputRoute(route,from,to);
  const intervalComplete=Number.isSafeInteger(voice.sourceNodeIdentity?.nodeId)&&Number.isSafeInteger(voice.outputSourceIdentity?.nodeId)&&
    voice.historyTapIdentity?.tapId===voice.outputTapIndex&&voice.historyTapIdentity?.sourceNodeId===voice.outputSourceIdentity?.nodeId&&
    voice.historyTapIdentity?.historyHandleId===voice.tapHistoryId&&Number.isSafeInteger(voice.tapHistoryId)&&
    hasCompleteSfxPcmInterval(history,from,to)&&routeConnected!==null;
  if(!intervalComplete)return{status:'incomplete',reason:'Sparse SFX samples do not cover the activated source interval; no continuous output verdict is available.',
    baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,observedSparsePcm:evidence.map(item=>({voiceId:item.voice.id,
      tapIndex:item.pcm.tapIndex,samples:item.pcm.samples})),correlatedVoice:null,correlatedPcm:null,correlated:false};
  if(routeConnected!==true)return{status:'failed',reason:'A newly active native SFX voice had no production destination route during its source interval.',
    baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,correlatedVoice:null,
    correlatedPcm:{tapIndex:voice.outputTapIndex,renderedFrames:history.frames,nonZeroFrames:history.nonZeroFrames,peakAbs:history.peakAbs},correlated:false};
  if(history.nonZeroFrames===0||history.peakAbs===0)return{status:'failed',reason:'A newly active native SFX voice produced only zero PCM across its complete rendered source interval.',
    baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,correlatedVoice:null,
    correlatedPcm:{tapIndex:voice.outputTapIndex,renderedFrames:history.frames,nonZeroFrames:history.nonZeroFrames,peakAbs:history.peakAbs},correlated:false};
  return{status:'passed',reason:null,baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,
    mappedActiveVoices:mapped.map(item=>item.voice),correlatedVoice:voice,correlatedPcm:{tapIndex:voice.outputTapIndex,
      captures:evidence[0].pcm.captures,nonZeroSamples:evidence[0].pcm.nonZeroSamples,peakAbs:evidence[0].pcm.peakAbs,
      renderedFrames:history.frames,nonZeroFrames:history.nonZeroFrames,renderedPeakAbs:history.peakAbs},correlated:true};
}

export function correlateInputSfxWindow(baselineWindow,actionWindow){
  const baseline=baselineWindow?.nativeSfx,action=actionWindow?.nativeSfx;
  const unavailable=(reason,firstIssue=null)=>({status:'incomplete',reason,baselineAvailable:baseline?.available===true,
    actionAvailable:action?.available===true,newVoiceActivations:[],correlatedVoice:null,correlatedPcm:null,correlated:false,
    ...(firstIssue?{firstIssue}:{})});
  if(baseline?.available!==true||action?.available!==true)
    return unavailable('Native SFX voice observation is unavailable in the baseline or action window.');
  if(!Array.isArray(baseline.snapshots?.end)||!Array.isArray(action.activationEvents)||!Array.isArray(action.activeObservations)||
    !Array.isArray(action.sourceIntervals))
    return unavailable('Native SFX voice activation samples are incomplete.');
  const inputActionWindow=actionWindow?.actionWindow,inputValidation=validateDesktopSfxInputWindow(inputActionWindow);
  if(!inputValidation.valid)return unavailable(inputValidation.reason);
  const inputEdgeWindow={startContextTime:inputValidation.down.contextTime,endContextTime:inputValidation.up.contextTime};
  const previous=new Map(baseline.snapshots.end.map(voice=>[voice.id,voice]));
  const newActivations=action.activationEvents.filter(voice=>{
    const prior=previous.get(voice.id);
    return !prior||(!(prior.playing||prior.node)||prior.started!==voice.started);
  });
  if(newActivations.some(voice=>!Number.isFinite(voice.started)||!Number.isFinite(voice.startCallContextTime)||
    !Number.isFinite(voice.observedAtContextTime)||voice.observedAtContextTime<voice.startCallContextTime||voice.contextIndex!==inputActionWindow.contextIndex||
    voice.contextSampleRate!==inputActionWindow.sampleRate||!Number.isInteger(voice.outputTapIndex)||voice.outputTapIndex<0||
    voice.tapMatchCount!==1))
    return unavailable('A new SFX activation lacks an unambiguous voice, tap, or matching SDL3 audio clock.');
  const activations=newActivations.filter(voice=>voice.started>=inputEdgeWindow.startContextTime&&
    voice.started<=inputEdgeWindow.endContextTime);
  if(!activations.length)return{status:'failed',reason:'No new native SFX voice activation was observed during the trusted input interval.',
    baselineAvailable:true,actionAvailable:true,inputEdgeWindow,newVoiceActivations:[],correlatedVoice:null,correlatedPcm:null,correlated:false};
  const intervalGroups=new Map();
  for(const voice of activations){
    const history=voice.pcmHistory,route=voice.outputRoute,from=voice.sourceStartFrame,to=voice.sourceEndFrame,
      routeConnected=validateSfxOutputRoute(route,from,to);
    const pcmIssue=sfxPcmIntervalIssue(voice);
    if(pcmIssue||
      voice.historyTapIdentity?.tapId!==voice.outputTapIndex||voice.historyTapIdentity?.sourceNodeId!==voice.outputSourceIdentity?.nodeId||
      voice.historyTapIdentity?.historyHandleId!==voice.tapHistoryId||!Number.isSafeInteger(voice.tapHistoryId))
      return unavailable(pcmIssue?.detail??`SFX source node ${String(voice.sourceNodeIdentity?.nodeId??'<unknown>')} on tap `+
        `${String(voice.outputTapIndex??'<unknown>')} has incomplete output/history identity.`,pcmIssue??{
          code:'output-history-identity-mismatch',voiceId:voice.id??null,sourceNodeId:voice.sourceNodeIdentity?.nodeId??null,
          outputNodeId:voice.outputSourceIdentity?.nodeId??null,tapId:voice.outputTapIndex??null,fromFrame:from,toFrame:to});
    if(routeConnected===null)
      return unavailable('The Runtime SFX production destination route is not completely observed over its PCM interval.');
    if(routeConnected!==true)return{status:'failed',reason:'A newly active Runtime SFX voice had no production destination route during its source interval.',
      baselineAvailable:true,actionAvailable:true,inputEdgeWindow,newVoiceActivations:[],correlatedVoice:null,correlatedPcm:null,correlated:false,
      disconnectedOutputVoice:voice};
  }
  for(const voice of activations){
    const key=`${voice.outputSourceIdentity.nodeId}\u0000${voice.outputTapIndex}`;
    const rows=intervalGroups.get(key)??[];rows.push(voice);intervalGroups.set(key,rows);
  }
  for(const [key,rows] of intervalGroups){
    const allSessionStarts=action.sourceIntervals.filter(interval=>
      `${interval?.outputSourceIdentity?.nodeId}\u0000${interval?.outputTapIndex}`===key);
    if(!allSessionStarts.length||rows.some(voice=>!allSessionStarts.some(interval=>
      interval.startJournalSequence===voice.startJournalSequence&&interval.sourceNodeIdentity?.nodeId===voice.sourceNodeIdentity?.nodeId)))
      return unavailable('The full source-start history for a firing output tap is missing.');
    if(allSessionStarts.some(interval=>interval.sourceIntervalAvailable!==true||!Number.isSafeInteger(interval.sourceStartFrame)||
      !Number.isSafeInteger(interval.sourceEndFrame)||interval.sourceEndFrame<interval.sourceStartFrame))
      return unavailable('A source interval on the firing output tap has an unknown or invalid playback boundary.');
    allSessionStarts.sort((left,right)=>left.sourceStartFrame-right.sourceStartFrame||left.startJournalSequence-right.startJournalSequence);
    for(let index=1;index<allSessionStarts.length;index++)if(allSessionStarts[index-1].sourceEndFrame>allSessionStarts[index].sourceStartFrame)
      return unavailable('Source intervals overlap on the same production voice output tap across the full input session.');
  }
  for(const rows of intervalGroups.values()){
    rows.sort((left,right)=>left.sourceStartFrame-right.sourceStartFrame);
    for(let index=1;index<rows.length;index++)if(rows[index-1].sourceEndFrame>rows[index].sourceStartFrame)
      return unavailable('Retriggered SFX source intervals overlap on the same production voice output tap.');
  }
  const groups=new Map();
  for(const voice of activations){
    const key=`${voice.id}\u0000${voice.outputTapIndex}`,group=groups.get(key)??{id:voice.id,tapIndex:voice.outputTapIndex,activations:[]};
    const observations=action.activeObservations.filter(item=>item.id===voice.id&&item.started===voice.started&&item.outputTapIndex===voice.outputTapIndex);
    if(observations.some(item=>item.contextIndex!==inputActionWindow.contextIndex||item.contextSampleRate!==inputActionWindow.sampleRate||
      !Number.isFinite(item.observedAtContextTime)||item.observedAtContextTime<voice.started))
      return unavailable('A target voice PCM observation is missing or inconsistent with the SDL3 audio clock.');
    const pcmRows=observations.map(item=>({captureSequence:item.captureSequence,contextTime:item.observedAtContextTime,
      nonZero:item.pcm?.nonZero??null,peakAbs:item.pcm?.peakAbs??null,rms:item.pcm?.rms??null}));
    if(voice.pcm&&Number.isSafeInteger(voice.captureSequence)&&!pcmRows.some(row=>row.captureSequence===voice.captureSequence))
      pcmRows.unshift({captureSequence:voice.captureSequence,contextTime:voice.observedAtContextTime,
        nonZero:voice.pcm.nonZero??null,peakAbs:voice.pcm.peakAbs??null,rms:voice.pcm.rms??null});
    group.activations.push({voice,pcm:{tapIndex:voice.outputTapIndex,captures:pcmRows.length,
      nonZeroSamples:pcmRows.reduce((sum,row)=>sum+(Number.isFinite(row.nonZero)?row.nonZero:0),0),
      peakAbs:pcmRows.reduce((peak,row)=>Math.max(peak,Number.isFinite(row.peakAbs)?row.peakAbs:0),0),
      rmsMax:pcmRows.reduce((peak,row)=>Math.max(peak,Number.isFinite(row.rms)?row.rms:0),0),samples:pcmRows,
      renderedFrames:voice.pcmHistory.frames,nonZeroFrames:voice.pcmHistory.nonZeroFrames,
      renderedPeakAbs:voice.pcmHistory.peakAbs,firstNonZeroFrame:voice.pcmHistory.firstNonZeroFrame}});
    groups.set(key,group);
  }
  const evidence=[...groups.values()].map(group=>({id:group.id,tapIndex:group.tapIndex,activationCount:group.activations.length,
    activations:group.activations}));
  const silent=evidence.flatMap(group=>group.activations).filter(item=>item.pcm.nonZeroFrames===0||item.pcm.renderedPeakAbs===0);
  if(silent.length)return{status:'failed',reason:'A newly active SFX voice reached its own WebAudio tap, but the captured PCM remained zero.',
    baselineAvailable:true,actionAvailable:true,inputEdgeWindow,newVoiceActivations:evidence,silentMappedVoices:silent.map(item=>({voice:item.voice,pcm:item.pcm})),
    correlatedVoice:null,correlatedPcm:null,correlated:false};
  if(evidence.length>1)return{status:'incomplete',reason:'Multiple new SFX voices were observed in the trusted input window, so the firing source cannot be isolated.',
    baselineAvailable:true,actionAvailable:true,inputEdgeWindow,newVoiceActivations:evidence,correlatedVoice:null,correlatedPcm:null,correlated:false};
  const group=evidence[0],activationRows=group.activations,pcmSamples=activationRows.flatMap(item=>item.pcm.samples);
  const correlatedPcm={tapIndex:group.tapIndex,captures:pcmSamples.length,
    nonZeroSamples:pcmSamples.reduce((sum,sample)=>sum+sample.nonZero,0),
    peakAbs:pcmSamples.reduce((peak,sample)=>Math.max(peak,sample.peakAbs),0),
    rmsMax:pcmSamples.reduce((peak,sample)=>Math.max(peak,sample.rms,0),0),samples:pcmSamples,
    renderedFrames:activationRows.reduce((sum,item)=>sum+item.pcm.renderedFrames,0),
    nonZeroFrames:activationRows.reduce((sum,item)=>sum+item.pcm.nonZeroFrames,0),
    renderedPeakAbs:activationRows.reduce((peak,item)=>Math.max(peak,item.pcm.renderedPeakAbs),0),
    sourceIntervals:activationRows.map(item=>({sequence:item.voice.startJournalSequence,fromFrame:item.voice.sourceStartFrame,
      toFrame:item.voice.sourceEndFrame,tapIdentity:item.voice.historyTapIdentity}))};
  return{status:'passed',reason:null,baselineAvailable:true,actionAvailable:true,newVoiceActivations:evidence,
    inputEdgeWindow,correlatedVoice:{id:group.id,tapIndex:group.tapIndex,activationCount:group.activations.length},correlatedPcm,correlated:true};
}
