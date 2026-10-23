import {gateReplayRecordOnFinalErrors} from '../browser/final-error-audit.mjs';
import {replayTerminalPolicy} from '../../tests/adaptation/adapters/replay.mjs';

export {gateReplayRecordOnFinalErrors};

export function replayScenarioPassedTargetIds(records){
  return records.filter(record=>record.status==='passed'&&record.playback?.scenarioStatus==='passed').map(record=>record.id);
}

export function hasHashBoundNaturalReplayEndpoint(replay){
  const consumption=replay?.fullInputConsumption,reference=consumption?.naturalTerminalReference;
  const expected=consumption?.expected,actual=consumption?.actual;
  return consumption?.status==='passed'&&reference?.status==='passed'&&
    typeof expected?.sha256==='string'&&reference.fixtureSha256===expected.sha256&&
    Number.isSafeInteger(expected.stage)&&reference.stage===expected.stage&&
    Number.isSafeInteger(actual?.frame)&&reference.frame===actual.frame&&
    Number.isSafeInteger(actual?.inputCursor)&&reference.inputCursor===actual.inputCursor&&
    Number.isSafeInteger(actual?.timingCursor)&&reference.timingCursor===actual.timingCursor&&
    typeof reference.source==='string'&&reference.source.length>0;
}

/** Bind REPLAY-01 to the imported fixture and the successful native playback operations. */
export function replayOriginalImportCheck(record,{game=record?.game,variant=record?.variant}={}){
  const step=id=>record?.checks?.find(check=>check?.id===id);
  const imported=step('import-current-replay-through-shell');
  const advancing=step('native-replay-plays-and-advances');
  const terminal=step('native-terminal-signal-observed');
  const importValue=imported?.value;
  const playbackValue=advancing?.value;
  const replay=record?.playback;
  const sameReplayBytes=typeof importValue?.sha256==='string'&&
    /^[a-f0-9]{64}$/.test(importValue.sha256)&&importValue.sha256===replay?.fixture?.sha256;
  const importValid=imported?.status==='passed'&&typeof importValue?.path==='string'&&
    Number.isSafeInteger(importValue.bytes)&&importValue.bytes>0&&sameReplayBytes;
  const playbackValid=advancing?.status==='passed'&&playbackValue?.naturalFrames>=120&&
    Number.isSafeInteger(playbackValue?.startFrame)&&Number.isSafeInteger(playbackValue?.frame)&&
    playbackValue.frame>=playbackValue.startFrame+120&&Array.isArray(playbackValue?.observed)&&playbackValue.observed.length>0;
  const endpointProven=hasHashBoundNaturalReplayEndpoint(replay);
  const terminalValid=terminal?.status==='passed'&&replay?.status==='observed'&&
    replay?.scenarioStatus==='passed'&&replay?.terminalSignalObserved?.status==='passed'&&
    replay?.terminalSignalObserved?.value===true&&
    (replayTerminalPolicy({game,variant})?endpointProven:replay?.fullInputConsumption?.status==='unproven');
  const finalAudit=record?.finalErrorAudit?.status;
  const failed=record?.status==='failed'||finalAudit==='failed'||
    [imported,advancing,terminal].some(check=>check?.status==='failed');
  const observed=importValid&&playbackValid&&terminalValid&&record?.status==='passed'&&finalAudit==='passed';
  const complete=observed&&Boolean(replayTerminalPolicy({game,variant}))&&endpointProven;
  const status=failed?'failed':complete?'passed':observed?'partial':'incomplete';
  const reason=!importValid?'The original Replay import has no successful hash-bound fixture observation.':
    !playbackValid?'The imported Replay has not demonstrated 120 naturally advancing native frames.':
    finalAudit!=='passed'?'The Replay scenario has no successful final browser error audit.':
    terminal?.status!=='passed'||replay?.terminalSignalObserved?.value!==true?
      'The imported Replay has no successful native terminal signal observation.':
    !endpointProven?replay?.fullInputConsumption?.reason??
      'The imported Replay has no independently sourced natural endpoint bound to its fixture SHA-256.':
    'The Replay scenario has not completed successfully.';
  return{id:'REPLAY-01',adaptationIds:['REPLAY-01'],status,complete,
    ...(!failed&&!complete?{reason}:{}),
    scope:complete?`${game}/${variant} imported the hash-bound original Replay, matched its independently sourced natural endpoint, and reached the native terminal state.`:
      'Replay import, native progress, and the terminal signal are reported separately from independently sourced natural endpoint evidence.',
    observation:{game,variant,adaptationIds:['REPLAY-01'],observation:{
      imported:imported?{status:imported.status,path:importValue?.path,bytes:importValue?.bytes,sha256:importValue?.sha256}:null,
      nativeAdvance:advancing?{status:advancing.status,startFrame:playbackValue?.startFrame,
        frame:playbackValue?.frame,naturalFrames:playbackValue?.naturalFrames,observed:playbackValue?.observed}:null,
      terminalSignal:replay?.terminalSignalObserved??null,
      fullInputConsumption:replay?.fullInputConsumption??{status:'unobserved'},
      replayFixtureSha256:replay?.fixture?.sha256??null,sameReplayBytes,
      finalErrorAudit:finalAudit??'unobserved',scenarioStatus:replay?.scenarioStatus??'unobserved',
    }}};
}

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
export function assessReplayTerminalSignal({game,variant=null,state,candidateCounterReference=null,inspectionStages=[],maximumFrame=null,
  nativeInputConsumption=null}){
  const observation=playbackObservation(state);
  const candidate=candidateCounterObservation(candidateCounterReference,observation);
  const terminalSignalObserved={status:observation.ended?'passed':'failed',value:observation.ended,field:'ended'};
  const fullInputConsumption={status:'unproven',reason:game==='th11'
    ?'th11 ended does not distinguish game_over, StageExit, or input end_marker, and does not prove that every Replay input block was consumed.'
    :'The native ended signal does not prove that every Replay input block was consumed.'};
  const requiresParsedConsumption=Boolean(replayTerminalPolicy({game,variant}));
  let th08FullConsumption=requiresParsedConsumption?
    nativeInputConsumption??{status:'incomplete',reason:'The parsed th08 Replay capacity was not assessed.'}:null;
  if(requiresParsedConsumption&&th08FullConsumption?.status==='passed'&&!hasHashBoundNaturalReplayEndpoint({
    fullInputConsumption:th08FullConsumption,
  })){
    th08FullConsumption={...th08FullConsumption,status:'incomplete',
      reason:'File-derived capacity and native terminal counters have no hash-bound natural endpoint reference for this Replay.'};
  }
  const consumption=requiresParsedConsumption?th08FullConsumption:fullInputConsumption;
  const evidence={terminalSignalObserved,fullInputConsumption:consumption};
  if(!observation.ended)return{...evidence,status:'failed',reason:'Native Replay state has not reported its terminal signal',observation,candidateCounterObservation:candidate};

  if(game==='th06'||game==='th07'){
    const consistent=observation.frame!==null&&observation.total!==null&&observation.total>0&&observation.frame===observation.total;
    return{...evidence,status:consistent?'observed':'failed',reason:consistent?null:`${game} native terminal cursor does not equal its native total`,
      observation,candidateCounterObservation:candidate,
      nativeFormatConsistency:{status:consistent?'passed':'failed',unit:'native replay cursor and native total',frame:observation.frame,total:observation.total}};
  }

  if(game==='th08'){
    if(th08FullConsumption?.status==='failed')return{...evidence,status:'failed',reason:th08FullConsumption.reason,observation,candidateCounterObservation:candidate};
    if(th08FullConsumption?.status==='incomplete')return{...evidence,status:'incomplete',reason:th08FullConsumption.reason,observation,candidateCounterObservation:candidate};
    return{...evidence,status:'observed',observation,candidateCounterObservation:candidate,
      terminalSignalScope:th08FullConsumption?'th08 Replay reached its hash-bound natural endpoint and then left gameplay.':
        'th08 native state reports its terminal signal after leaving gameplay; its Replay cursor has no declared total tick count.',
      nativeCounterObservation:{unit:'ReplayStream consumed-input cursor',frame:observation.frame,total:observation.total}};
  }

  if(game==='th11'){
    const validStages=inspectionStages.length>0&&inspectionStages.every(stage=>Number.isSafeInteger(stage.frames)&&stage.frames>0);
    const metadataTotal=validStages?inspectionStages.reduce((sum,stage)=>sum+stage.frames,0):null;
    const totalMatches=metadataTotal!==null&&observation.total===metadataTotal;
    return{...evidence,status:totalMatches?'observed':'failed',reason:totalMatches?null:'th11 native total differs from the inspected stage metadata sum',
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
