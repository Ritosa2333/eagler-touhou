export function playbackObservation(state){
  const phase=state?.phase??state?.screen??null;
  return {
    frame:Number.isSafeInteger(state?.frame)?state.frame:null,
    total:Number.isSafeInteger(state?.total)?state.total:null,
    ended:state?.ended===true,
    stage:Number.isSafeInteger(state?.stage)?state.stage:null,
    startStage:Number.isSafeInteger(state?.startStage)?state.startStage:null,
    phase,
    playbackPhase:phase??(state?.ended===true?'ended':state?.kind==='replay'?'replaying':null),
    phaseSource:phase===null?'inferred-from-replay-state':'native-state',
    preparing:state?.preparing===true,
    paused:state?.paused===true,
    seeking:state?.seeking===true,
    target:Number.isSafeInteger(state?.target)?state.target:null,
    rate:Number.isFinite(state?.rate)?state.rate:null,
  };
}

export function replayProtocolFailures(events){
  return(Array.isArray(events)?events:[]).filter(event=>event&&
    (event.event==='error'||event.event==='abort'||event.ok===false));
}

export function createReplayProgressTracker({startedAt,initialState=null,initialMaximumFrame=null}){
  let previous=playbackObservation(initialState),lastAdvanceAt=startedAt;
  const initialFrames=[previous.frame,initialMaximumFrame].filter(Number.isSafeInteger);
  let maximumFrame=initialFrames.length?Math.max(...initialFrames):null;
  return {
    get maximumFrame(){return maximumFrame;},
    sample(state,observedAt){
      const current=playbackObservation(state);
      const advanced=(current.frame!==null&&previous.frame!==null&&current.frame!==previous.frame)||
        (current.stage!==null&&previous.stage!==null&&current.stage!==previous.stage);
      if(advanced)lastAdvanceAt=observedAt;
      if(current.frame!==null)maximumFrame=Math.max(maximumFrame??current.frame,current.frame);
      previous=current;
      return {...current,observedAt,elapsedMs:observedAt-startedAt,lastAdvanceAt,
        lastAdvanceAtIso:new Date(lastAdvanceAt).toISOString(),lastAdvanceAgeMs:observedAt-lastAdvanceAt};
    },
  };
}

function candidateCounterObservation(reference,observation){
  if(!reference)return null;
  const referenceValue=reference.counter?.value;
  const nativeFrame=observation.frame;
  return{
    kind:reference.kind,
    name:reference.counter?.name??null,
    referenceValue:Number.isSafeInteger(referenceValue)?referenceValue:null,
    nativeFrame,
    numericDelta:Number.isSafeInteger(referenceValue)&&Number.isSafeInteger(nativeFrame)?nativeFrame-referenceValue:null,
    referenceReplaySha256:reference.replaySha256??null,
    persistedReplaySha256:reference.persistedReplaySha256??null,
    selectedReplaySha256:reference.selectedReplaySha256??null,
    sameReplayBytes:reference.replaySha256!==undefined&&reference.replaySha256!==null&&
      reference.replaySha256===reference.selectedReplaySha256,
    counterBoundToSelectedReplay:reference.counterBoundToSelectedReplay===true,
    comparableToNativeCursor:reference.comparableToNativeCursor===true,
    semanticComparison:reference.comparableToNativeCursor===true?'same-counter-semantics':'unproven',
    countDefinition:reference.countDefinition??null,
  };
}

/**
 * Assess only the native terminal signal and documented native
 * self-consistency. Original-format compatibility remains a separate result.
 */
export function assessReplayTerminalSignal({game,state,candidateCounterReference=null,inspectionStages=[],maximumFrame=null}){
  const observation=playbackObservation(state);
  const candidate=candidateCounterObservation(candidateCounterReference,observation);
  const terminalSignalObserved={status:observation.ended?'passed':'failed',value:observation.ended,field:'ended'};
  const fullInputConsumption={status:'unproven',reason:game==='th11'
    ?'TH11 ended does not distinguish game_over, StageExit, or input end_marker, and does not prove that every Replay input block was consumed.'
    :'The native ended signal does not prove that every Replay input block was consumed.'};
  const evidence={terminalSignalObserved,fullInputConsumption};
  if(!observation.ended)return{...evidence,status:'failed',reason:'Native Replay state has not reported its terminal signal',observation,candidateCounterObservation:candidate};

  if(game==='th06'||game==='th07'){
    const consistent=observation.frame!==null&&observation.total!==null&&observation.total>0&&observation.frame===observation.total;
    return{...evidence,status:consistent?'observed':'failed',reason:consistent?null:`${game} native terminal cursor does not equal its native total`,
      observation,candidateCounterObservation:candidate,
      nativeFormatConsistency:{status:consistent?'passed':'failed',unit:'native replay cursor and native total',frame:observation.frame,total:observation.total}};
  }

  if(game==='th08'){
    return{...evidence,status:'observed',observation,candidateCounterObservation:candidate,
      terminalSignalScope:'TH08 native state reports its terminal signal after leaving gameplay; its Replay cursor has no declared total tick count.',
      nativeCounterObservation:{unit:'ReplayStream consumed-input cursor',frame:observation.frame,total:observation.total}};
  }

  if(game==='th11'){
    const validStages=inspectionStages.length>0&&inspectionStages.every(stage=>Number.isSafeInteger(stage.frames)&&stage.frames>0);
    const metadataTotal=validStages?inspectionStages.reduce((sum,stage)=>sum+stage.frames,0):null;
    const totalMatches=metadataTotal!==null&&observation.total===metadataTotal;
    return{...evidence,status:totalMatches?'observed':'failed',reason:totalMatches?null:'TH11 native total differs from the inspected stage metadata sum',
      observation,candidateCounterObservation:candidate,
      nativeFormatConsistency:{status:totalMatches?'passed':'failed',unit:'sum of inspected stage metadata frames',metadataTotal,observedTotal:observation.total,
        stageFrames:validStages?inspectionStages.map(stage=>stage.frames):null}};
  }

  if(game==='th10'||game==='th15'){
    const observedProgress=maximumFrame!==null&&maximumFrame>=120;
    return{...evidence,status:observedProgress?'observed':'failed',reason:observedProgress?null:`${game} did not record 120 native playback frames before its terminal signal`,
      observation,candidateCounterObservation:candidate,
      terminalSignalScope:'Native terminal signal after observed playback progress; this observation does not prove that the full Replay input was consumed.',
      comparison:{observedFrame:observation.frame,observedTotal:observation.total,maximumFrame,ended:observation.ended,observedProgress}};
  }

  return{...evidence,status:'observed',reason:'Native Runtime reported its terminal signal; the attached original counter is reported separately without a proven same-counter mapping.',
    observation,candidateCounterObservation:candidate,
    terminalSignalScope:'Native Runtime terminal signal only; full Replay input consumption and original-format terminal compatibility remain unproven.',
    comparison:{observedFrame:observation.frame,observedTotal:observation.total,maximumFrame,ended:observation.ended}};
}
