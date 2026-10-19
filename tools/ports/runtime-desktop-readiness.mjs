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

export function correlateTh09SfxWindow(baselineWindow,actionWindow){
  const baseline=baselineWindow?.nativeSfx,action=actionWindow?.nativeSfx;
  const previous=new Map((baseline?.snapshots?.end??[]).map(voice=>[voice.id,voice]));
  const activations=(action?.activationEvents??[]).filter(voice=>{
    if(!Number.isInteger(voice.outputTapIndex)||voice.outputTapIndex<0)return false;
    const prior=previous.get(voice.id);
    return !prior||(!(prior.playing||prior.node)||prior.started!==voice.started);
  });
  const correlated=activations.map(voice=>({voice,pcm:action?.associatedOutputPcm?.find(tap=>tap.tapIndex===voice.outputTapIndex)??null}))
    .find(item=>item.pcm?.nonZeroSamples>0)??null;
  return{baselineAvailable:baseline?.available===true,actionAvailable:action?.available===true,
    newVoiceActivations:activations,correlatedVoice:correlated?.voice??null,correlatedPcm:correlated?.pcm??null,
    correlated:Boolean(correlated)};
}
