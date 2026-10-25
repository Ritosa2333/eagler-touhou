const SCHEMA='eagler-touhou/replay-recording-scenes/1';
const INPUT_ROUTES=['keyboard','virtual-gamepad','touch-controls'];
const CONSUMED_INPUT_VALID=1<<1;
const REQUIRED_STATUS_FIELDS=['schema','nextSequence','firstAvailable','count','capacity','samplesStart'];
const TRACE_FIELDS={
  base:['sequence','boundary','stage'],
  recording:['pollSequence','liveInput','recordingActive','recordingStage','recordingFrame','recordingInputCursor','recordingPhysicalInput',
    'consumerPhase','consumerValidMask','consumerSequence','consumerControlFrame','consumerStage','consumerCurrent','consumerPrevious'],
  playback:['pollSequence','liveInput','playbackLoaded','playbackStage','playbackFrame','playbackInputCursor',
    'playbackCurrent','playbackPrevious','consumerPhase','consumerValidMask','consumerSequence','consumerControlFrame',
    'consumerStage','consumerCurrent','consumerPrevious'],
  pointer:['motionPlaying','motionStage','touchCursor','touchCount','pointCount','pointId','pointXFloat32Bits','pointYFloat32Bits','pointReleased'],
};

const blocked=(id,reason)=>({id,status:'blocked',reason});
const mask=value=>Number.isSafeInteger(value)&&value>0&&value<=0xffff;

function traceSequence(samples,expectedBoundary){
  if(!Array.isArray(samples)||samples.length===0)return{status:'incomplete',reason:'No native Replay trace samples were captured.'};
  if(!Number.isSafeInteger(expectedBoundary))return{status:'incomplete',reason:'The title has no declared post-update trace boundary.'};
  for(let i=0;i<samples.length;i++){
    const sample=samples[i];
    if(!sample||!Number.isSafeInteger(sample.sequence)||!Number.isSafeInteger(sample.boundary)||!Number.isSafeInteger(sample.stage))
      return{status:'incomplete',reason:`Replay trace sample ${i} is missing its native sequence, boundary, or stage fields.`};
    if(sample.boundary!==expectedBoundary)
      return{status:'incomplete',reason:`Replay trace sample ${sample.sequence} was not captured at the declared post-update boundary.`};
    if(i&&sample.sequence!==samples[i-1].sequence+1)
      return{status:'incomplete',reason:`Replay trace has a sequence gap between ${samples[i-1].sequence} and ${sample.sequence}.`};
    if(i&&Number.isSafeInteger(sample.pollSequence)&&Number.isSafeInteger(samples[i-1].pollSequence)&&
      sample.pollSequence<samples[i-1].pollSequence)
      return{status:'failed',reason:`Native input poll sequence moved backwards at trace sample ${sample.sequence}.`};
  }
  return{status:'passed'};
}

function validNativeTrace(trace,requiredFields=[]){
  if(!trace||!Number.isSafeInteger(trace.schema)||trace.schema<1||
    !Number.isSafeInteger(trace.wordCount)||trace.wordCount<1||
    typeof trace.controlExport!=='string'||!trace.controlExport.trim()||
    typeof trace.readExport!=='string'||!trace.readExport.trim()||
    !trace.statusFields||!trace.sample||!trace.values)return false;
  const statusIndexes=Object.values(trace.statusFields),sampleIndexes=Object.values(trace.sample);
  return REQUIRED_STATUS_FIELDS.every(key=>Number.isSafeInteger(trace.statusFields[key])&&trace.statusFields[key]>=0)&&
    requiredFields.every(key=>Number.isSafeInteger(trace.sample[key])&&trace.sample[key]>=0)&&
    statusIndexes.every(value=>Number.isSafeInteger(value)&&value>=0)&&
    new Set(statusIndexes).size===statusIndexes.length&&
    sampleIndexes.every(value=>Number.isSafeInteger(value)&&value>=0)&&
    sampleIndexes.every(value=>value<trace.wordCount)&&
    new Set(sampleIndexes).size===sampleIndexes.length&&
    ['postUpdateBoundary','consumerRunPhase','consumedInputValidBit'].every(key=>Number.isSafeInteger(trace.values[key]));
}

function validIngressStrategy(ingress){
  if(!ingress||typeof ingress!=='object'||ingress.strategy!=='native-touch-control-suppressed'||
    typeof ingress.receipt!=='string'||!ingress.receipt.trim()||typeof ingress.contextField!=='string'||
    !ingress.contextField.trim()||typeof ingress.activeField!=='string'||!ingress.activeField.trim()||
    !Number.isSafeInteger(ingress.contextValue)||ingress.releasedValue!==0||ingress.activeValue!==1||
    typeof ingress.source!=='string'||!ingress.source.trim())return false;
  return true;
}

function validInputReceipt(trace,ingress){
  if(!validIngressStrategy(ingress))return false;
  const receipt=trace?.inputReceipts?.[ingress.receipt];
  if(!receipt||typeof receipt.export!=='string'||!receipt.export.trim()||
    !Number.isSafeInteger(receipt.wordCount)||receipt.wordCount<1||!receipt.fields||
    typeof receipt.source!=='string'||!receipt.source.trim())return false;
  const entries=Object.entries(receipt.fields);
  if(entries.length===0||new Set(entries.map(([,field])=>field?.index)).size!==entries.length)return false;
  if(entries.some(([,field])=>!Number.isSafeInteger(field?.index)||field.index<0||field.index>=receipt.wordCount||
    !['i32','bool01'].includes(field.type)))return false;
  return receipt.fields[ingress.contextField]?.type==='i32'&&receipt.fields[ingress.activeField]?.type==='bool01';
}

function validFinalizer(finalizer){
  return Boolean(finalizer&&typeof finalizer.export==='string'&&finalizer.export.trim()&&finalizer.app===true&&
    typeof finalizer.allocateExport==='string'&&finalizer.allocateExport.trim()&&
    typeof finalizer.freeExport==='string'&&finalizer.freeExport.trim()&&
    Number.isSafeInteger(finalizer.slot)&&finalizer.slot>=0&&typeof finalizer.name==='string'&&finalizer.name.trim()&&
    typeof finalizer.path==='string'&&finalizer.path.trim()&&!/^([\\/]|[A-Za-z]:)/.test(finalizer.path)&&finalizer.source?.trim()&&
    !finalizer.path.split(/[\\/]/).includes('..'));
}

function validOpener(opener){
  return Boolean(opener&&typeof opener.export==='string'&&opener.export.trim()&&opener.app===false&&
    typeof opener.allocateExport==='string'&&opener.allocateExport.trim()&&
    typeof opener.freeExport==='string'&&opener.freeExport.trim()&&
    typeof opener.saveRoot==='string'&&opener.saveRoot.startsWith('/')&&opener.saveRoot.trim()&&
    !opener.saveRoot.split(/[\\/]/).includes('..')&&opener.source?.trim());
}

/** Build explicit finite input scenes from a title's production-owned Replay semantics. */
export function createReplayRecordingScenes({semantics,stage=0,windowTicks=12}={}){
  if(!Number.isSafeInteger(stage)||stage<0)throw new Error('Replay scene stage must be a nonnegative integer');
  if(!Number.isSafeInteger(windowTicks)||windowTicks<4||windowTicks>120)
    throw new Error('Replay scene windowTicks must be an integer from 4 through 120');
  const title={game:semantics?.game??null,variant:semantics?.variant??null};
  const finalizer=semantics?.finalizer;
  const opener=semantics?.opener;
  const declared=Array.isArray(semantics?.inputs)?semantics.inputs:[];
  const byRoute=new Map(declared.map(item=>[item?.source,item]));
  const cases=[];
  for(const route of INPUT_ROUTES){
    const input=byRoute.get(route);
    if(!input){cases.push(blocked(`record-${route}`,`The title has no production mapping for ${route}.`));continue;}
    if(!input.operation||!input.release||!mask(input.logicalMask)||!input.anchor?.trim()||!validFinalizer(finalizer)){
      cases.push(blocked(`record-${route}`,`The ${route} declaration or production Replay finalizer is incomplete.`));continue;
    }
    const consumer=semantics.recording?.consumer;
    if(!consumer||![-1,0].includes(consumer.frameOffset)||!consumer.source?.trim()||!Number.isInteger(consumer.initialInput)){
      cases.push(blocked(`record-${route}`,'Declare recording-to-player consumer frame alignment and its production source.'));continue;
    }
    cases.push({id:`record-${route}`,status:'ready',kind:'recording-input',route,stage,
      press:structuredClone(input.operation),release:structuredClone(input.release),logicalMask:input.logicalMask,
      finalizer:structuredClone(finalizer),
      consumer:semantics.recording?.consumer?{...structuredClone(semantics.recording.consumer),
        phase:semantics.nativeTrace?.values?.consumerRunPhase,validBit:semantics.nativeTrace?.values?.consumedInputValidBit}:null,
      anchor:input.anchor,window:{warmupTicks:1,pressedTicks:2,neutralTicks:2,totalTicks:5},
      evidence:['native poll sequence','recorded physical input','Replay recording frame and cursor','independently parsed saved Replay input words']});
  }
  const pointer=semantics?.pointerExtension;
  const pointObservation=pointer?.appliedPoint;
  if(validFinalizer(finalizer)&&pointer?.id&&Number.isSafeInteger(pointer.requiredVersion)&&Array.isArray(pointer.trajectory)&&
    pointer.trajectory.length>=3&&pointer.consumedAt?.trim()&&pointer.source?.trim()&&
    pointObservation?.source?.trim()&&pointObservation?.coordinates?.trim()&&
    Number.isFinite(pointObservation.tolerance)&&pointObservation.tolerance>0&&
    pointObservation.traceWords&&['count','id','xFloat32Bits','yFloat32Bits','released'].every(key=>Number.isSafeInteger(pointObservation.traceWords[key]))){
    const actions=pointer.trajectory.map(item=>item?.action);
    const oneId=pointer.trajectory.every(item=>Number.isSafeInteger(item.id)&&item.id===pointer.trajectory[0].id);
    const coords=pointer.trajectory.every(item=>Number.isFinite(item.x)&&Number.isFinite(item.y)&&item.x>=0&&item.x<=1&&item.y>=0&&item.y<=1);
    if(oneId&&coords&&actions[0]==='down'&&actions.at(-1)==='up'&&actions.includes('move')){
      cases.push({id:'record-pointer-trajectory',status:'ready',kind:'recording-extension',extension:pointer.id,finalizer:structuredClone(finalizer),
        requiredVersion:pointer.requiredVersion,trajectory:structuredClone(pointer.trajectory),stage,
        evidence:['independent extension parser','native extension playback cursor','applied pointer point snapshot'],
        anchor:pointer.source,consumedAt:pointer.consumedAt,pointObservation:structuredClone(pointObservation)});
    }else cases.push(blocked('record-pointer-trajectory','The title pointer trajectory must contain one bounded down/move/up path for a single pointer id.'));
  }else cases.push(blocked('record-pointer-trajectory','The title has no source-anchored pointer extension and native consumption boundary.'));

  const playback=semantics?.playback;
  const isolation=semantics?.isolation;
  const replayPath=validOpener(opener)&&validFinalizer(finalizer)?
    `${opener.saveRoot.replace(/\/+$/,'')}/${finalizer.path.replace(/^\/+/, '')}`:null;
  if(playback?.consumerBoundary?.trim()&&Number.isSafeInteger(playback.rawIndexOffset)&&validOpener(opener)&&validFinalizer(finalizer)){
    cases.push({id:'playback-baseline',status:'ready',kind:'playback-baseline',stage,windowTicks,
      opener:structuredClone(opener),path:replayPath,
      rawIndexOffset:playback.rawIndexOffset,liveInput:{kind:'none'},
      evidence:['post-update consumer receipt','independent raw Replay input word','playback cursor']});
    const probes=Array.isArray(isolation?.probes)?isolation.probes:[];
    const covered=new Set();
    for(const probe of probes){
      if(typeof probe?.name!=='string'||!probe.name.trim()||!probe.operation&&probe.kind===undefined||
        !probe.release||!mask(probe.logicalMask)||!probe.anchor?.trim()||!isolation?.source?.trim())continue;
      if(probe.ingress&&!validInputReceipt(semantics?.nativeTrace,probe.ingress)){
        cases.push(blocked(`playback-live-${probe.name}`,'The title native input-receipt declaration is incomplete or incompatible with this probe.'));
        continue;
      }
      if(typeof probe.covers==='string')covered.add(probe.covers);
      cases.push({id:`playback-live-${probe.name}`,status:'ready',kind:'playback-live-perturbation',stage,windowTicks,
        opener:structuredClone(opener),path:replayPath,
        rawIndexOffset:playback.rawIndexOffset,probe:structuredClone(probe),
        ...(probe.ingress?{ingress:structuredClone(probe.ingress)}:{}),
        exemptionMask:Number.isSafeInteger(isolation.playbackControlMask)?isolation.playbackControlMask:0,
        evidence:['native poll confirms perturbation arrived','Replay stream word and player consumer remain file-derived',
          'baseline and perturbation use the same Replay SHA-256']});
    }
    for(const input of isolation?.liveInputs??[])if(!covered.has(input))
      cases.push(blocked(`playback-live-${input}`,`The title has no source-anchored live perturbation for ${input}.`));
  }else{
    cases.push(blocked('playback-baseline','The title has no source-anchored Replay consumer mapping, production Replay finalizer and opener.'));
    for(const input of isolation?.liveInputs??['direction','shoot'])
      cases.push(blocked(`playback-live-${input}`,`The title has no explicit live-input isolation mapping for ${input}.`));
  }
  const nativeTrace=semantics?.nativeTrace;
  for(let index=0;index<cases.length;index++){
    const item=cases[index];if(item.status!=='ready')continue;
    const fields=item.kind==='recording-input'?TRACE_FIELDS.recording:
      item.kind==='recording-extension'?TRACE_FIELDS.pointer:TRACE_FIELDS.playback;
    const requiredFields=[...TRACE_FIELDS.base,...fields];
    if(!validNativeTrace(nativeTrace,requiredFields)){
      cases[index]=blocked(item.id,'The title has no complete native trace field map for this scene and its read-only control API.');
    }else item.boundary=nativeTrace.values.postUpdateBoundary;
  }
  const traceReady=cases.some(item=>item.status==='ready')&&validNativeTrace(nativeTrace,[...TRACE_FIELDS.base]);
  const required=cases.map(item=>item.id),ready=cases.filter(item=>item.status==='ready').length;
  return{schema:SCHEMA,title,stage,windowTicks,finalizer:validFinalizer(finalizer)?structuredClone(finalizer):null,
    opener:validOpener(opener)?structuredClone(opener):null,replayPath,
    nativeTrace:traceReady?structuredClone(nativeTrace):null,
    requiredCases:required,status:ready===required.length?'ready':'blocked',cases};
}

/** Verify one finite recorded-input window against native polling and the independently decoded file. */
export function assessReplayRecordingInputWindow({samples,scene,rawFrames,expectedSha256}={}){
  if(scene?.kind!=='recording-input'||scene.status!=='ready')
    return{status:'incomplete',reason:'The requested recording input scene is not ready.'};
  const sequence=traceSequence(samples,scene.boundary);if(sequence.status!=='passed')return sequence;
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??'')||!rawFrames||rawFrames.sha256!==expectedSha256||
    !Array.isArray(rawFrames?.frames)||!Number.isSafeInteger(rawFrames.stride)||rawFrames.stride<1)
    return{status:'incomplete',reason:'Independent raw Replay input words are missing or do not cover the recorded window.'};
  const contract=scene.consumer;
  if(!contract||![-1,0].includes(contract.frameOffset)||!contract.source?.trim()||!Number.isInteger(contract.phase)||
    !Number.isInteger(contract.validBit)||contract.validBit<=0||!Number.isInteger(contract.initialInput))
    return{status:'incomplete',reason:'Recording has no source-owned player-consumer alignment contract.'};
  const mismatches=[];let previousFrame=null,previousPoll=null,previousConsumer=null,consumedPressed=0,releasedAfterPress=false;
  for(let index=0;index<samples.length;index++){
    const sample=samples[index];
    const recording=sample.recording;
    if(!recording||recording.active!==true||![recording.stage,recording.frame,recording.cursor,recording.physical].every(Number.isSafeInteger))
      return{status:'incomplete',reason:`Recording trace sample ${sample.sequence} lacks the active stream and physical-input receipt.`};
    if(!Number.isSafeInteger(sample.pollSequence)||!Number.isSafeInteger(sample.liveInput)||sample.liveInput<0||sample.liveInput>0xffff)
      return{status:'incomplete',reason:`Recording trace sample ${sample.sequence} lacks the native polled input receipt.`};
    const raw=rawFrames.frames.find(item=>item.frame===recording.frame);
    if(!Number.isSafeInteger(raw?.buttons))
      return{status:'incomplete',reason:`Independent Replay frame ${recording.frame} is not available.`};
    const consumer=sample.consumer,consumerFrame=recording.frame+contract.frameOffset;
    const consumed=rawFrames.frames.find(item=>item.frame===consumerFrame);
    const preceding=consumerFrame===0?contract.initialInput:rawFrames.frames.find(item=>item.frame===consumerFrame-1)?.buttons;
    if(!consumer||consumer.phase!==contract.phase||!(consumer.validMask&contract.validBit)||
      ![consumer.sequence,consumer.controlFrame,consumer.stage,consumer.current,consumer.previous,consumed?.buttons,preceding].every(Number.isSafeInteger))
      return{status:'incomplete',reason:`Recording trace ${sample.sequence} has no aligned native player-consumer and independent input bytes.`};
    if(consumer.stage!==scene.stage||consumer.controlFrame!==sample.controlFrame||
      consumer.current!==consumed.buttons||consumer.previous!==preceding||
      previousConsumer!==null&&consumer.sequence!==previousConsumer+1)
      mismatches.push({sequence:sample.sequence,consumer,consumerFrame,expectedCurrent:consumed.buttons,expectedPrevious:preceding});
    if(consumer.current&scene.logicalMask)consumedPressed++;
    else if(consumedPressed)releasedAfterPress=true;
    previousConsumer=consumer.sequence;
    const held=(sample.liveInput&scene.logicalMask)!==0;
    const recordedHeld=(recording.physical&scene.logicalMask)!==0;
    if(sample.stage!==scene.stage||recording.stage!==scene.stage||held!==recordedHeld||recording.physical!==raw.buttons||recording.frame<=0||
      (previousFrame!==null&&recording.frame!==previousFrame+1)||
      (previousPoll!==null&&sample.pollSequence!==previousPoll+1)||recording.cursor!==recording.frame*rawFrames.stride){
      mismatches.push({sequence:sample.sequence,held,recordedHeld,physical:recording.physical,raw:raw.buttons,
        stage:sample.stage,expectedStage:scene.stage,frame:recording.frame,previousFrame,previousPoll,
        pollSequence:sample.pollSequence,cursor:recording.cursor,expectedCursor:recording.frame*rawFrames.stride});
    }
    previousFrame=recording.frame;previousPoll=sample.pollSequence;
  }
  if(mismatches.length)return{status:'failed',reason:'Native polling, Replay recording, and independent raw input words disagree.',mismatches};
  const pressed=samples.filter(sample=>sample.liveInput&scene.logicalMask).length;
  const released=samples.filter(sample=>(sample.liveInput&scene.logicalMask)===0).length;
  if(pressed<scene.window.pressedTicks||released<scene.window.neutralTicks||consumedPressed<scene.window.pressedTicks||!releasedAfterPress)
    return{status:'incomplete',reason:'The finite scene did not capture both polled and player-consumed press/release windows.',pressed,released,consumedPressed,releasedAfterPress};
  return{status:'passed',scope:'finite-recording-input-window',completeReplay:false,
    source:scene.route,logicalMask:scene.logicalMask,samples:samples.length,pressed,released,consumedPressed,releasedAfterPress,
    replaySha256:rawFrames.sha256};
}

/** Compare a finite playback window to independently decoded input and its actual player consumer. */
export function assessReplayPlaybackWindow({samples,rawFrames,stage,rawIndexOffset=-1,boundary,expectedSha256}={}){
  const sequence=traceSequence(samples,boundary);if(sequence.status!=='passed')return sequence;
  if(!rawFrames||!Array.isArray(rawFrames.frames)||rawFrames.sha256!==expectedSha256||
    !Number.isSafeInteger(rawFrames.stride)||rawFrames.stride<1)
    return{status:'incomplete',reason:'The independently decoded Replay input window is not bound to the selected Replay identity.'};
  const mismatches=[];let previousFrame=null,previousConsumerSequence=null;
  for(const sample of samples){
    const playback=sample.playback,consumer=sample.consumer;
    if(!playback||playback.loaded!==true||!consumer||!Number.isSafeInteger(playback.stage)||!Number.isSafeInteger(playback.frame)||
      !Number.isSafeInteger(playback.cursor)||!Number.isSafeInteger(playback.current)||
      !Number.isSafeInteger(consumer.sequence)||!Number.isSafeInteger(consumer.controlFrame)||
      !Number.isSafeInteger(consumer.stage)||!Number.isSafeInteger(consumer.current)||!Number.isSafeInteger(consumer.previous)||
      !Number.isSafeInteger(consumer.validMask)||!Number.isSafeInteger(consumer.phase)||!Number.isSafeInteger(sample.controlFrame))
      return{status:'incomplete',reason:`Replay or consumer receipt is missing at trace sample ${sample.sequence}.`};
    if(consumer.phase!==1||(consumer.validMask&CONSUMED_INPUT_VALID)===0)
      return{status:'incomplete',reason:`Player input was not consumed in a complete consumer phase at sample ${sample.sequence}.`};
    if(previousFrame!==null&&playback.frame!==previousFrame+1)
      return{status:'incomplete',reason:`Replay playback frame did not advance by one between samples ${sample.sequence-1} and ${sample.sequence}.`};
    if(previousConsumerSequence!==null&&consumer.sequence!==previousConsumerSequence+1)
      return{status:'incomplete',reason:`Player consumer sequence has a gap before trace sample ${sample.sequence}.`};
    const rawIndex=playback.frame+rawIndexOffset,raw=rawFrames.frames.find(item=>item.frame===rawIndex);
    if(!raw)return{status:'incomplete',reason:`Raw Replay frame ${rawIndex} for native playback frame ${playback.frame} is unavailable.`};
    const mismatched={};
    if(sample.stage!==stage)mismatched.stage={actual:sample.stage,expected:stage};
    if(playback.stage!==stage)mismatched.playbackStage={actual:playback.stage,expected:stage};
    if(consumer.stage!==stage)mismatched.consumerStage={actual:consumer.stage,expected:stage};
    if(sample.controlFrame!==consumer.controlFrame)mismatched.controlFrame={actual:sample.controlFrame,consumer:consumer.controlFrame};
    if(playback.cursor!==playback.frame*rawFrames.stride)mismatched.cursor={actual:playback.cursor,expected:playback.frame*rawFrames.stride};
    if(playback.current!==raw.buttons)mismatched.streamInput={actual:playback.current,expected:raw.buttons};
    if(consumer.current!==raw.buttons)mismatched.consumedInput={actual:consumer.current,expected:raw.buttons};
    if(consumer.previous!==playback.previous)mismatched.previousInput={consumer:consumer.previous,stream:playback.previous};
    if(Object.keys(mismatched).length)mismatches.push({sequence:sample.sequence,rawIndex,mismatched});
    previousFrame=playback.frame;previousConsumerSequence=consumer.sequence;
  }
  if(mismatches.length)return{status:'failed',reason:'Native Replay cursor/input or player-consumer receipt disagrees with independently decoded raw frames.',mismatches};
  return{status:'passed',scope:'finite-playback-consumption-window',completeReplay:false,
    samples:samples.length,stage,sha256:rawFrames.sha256,rawIndexOffset};
}

/** Compare two fresh playback windows and require the declared live action to reach native polling. */
export function assessReplayInputIsolation({baseline,perturbed,scene,rawFrames,expectedSha256,baselineSha256,perturbedSha256}={}){
  if(scene?.kind!=='playback-live-perturbation'||scene.status!=='ready')
    return{status:'incomplete',reason:'The requested Replay live-perturbation scene is not ready.'};
  const baseSequence=traceSequence(baseline,scene.boundary),perturbedSequence=traceSequence(perturbed,scene.boundary);
  if(baseSequence.status!=='passed')return baseSequence;
  if(perturbedSequence.status!=='passed')return perturbedSequence;
  if(!/^[a-f0-9]{64}$/.test(expectedSha256??'')||baselineSha256!==expectedSha256||perturbedSha256!==expectedSha256)
    return{status:'incomplete',reason:'Both playback windows must be bound to the selected Replay SHA-256.'};
  const baselinePlayback=assessReplayPlaybackWindow({samples:baseline,rawFrames,stage:scene.stage,
    rawIndexOffset:scene.rawIndexOffset,boundary:scene.boundary,expectedSha256});
  if(baselinePlayback.status!=='passed')return{...baselinePlayback,reason:`Baseline playback: ${baselinePlayback.reason}`};
  const perturbedPlayback=assessReplayPlaybackWindow({samples:perturbed,rawFrames,stage:scene.stage,
    rawIndexOffset:scene.rawIndexOffset,boundary:scene.boundary,expectedSha256});
  if(perturbedPlayback.status!=='passed')return{...perturbedPlayback,reason:`Perturbed playback: ${perturbedPlayback.reason}`};
  if(baseline.length!==perturbed.length||baseline.length<scene.windowTicks)
    return{status:'incomplete',reason:'Baseline and perturbed playback windows do not have the required matching sample count.'};
  const maskValue=scene.probe.logicalMask,exemptionMask=scene.exemptionMask??0;
  const ingress=scene.ingress??scene.probe.ingress??null;
  if(!mask(maskValue)||!Number.isSafeInteger(exemptionMask)||exemptionMask<0||exemptionMask>0xffff||
    (maskValue&exemptionMask)!==0)
    return{status:'incomplete',reason:'The declared perturbation and playback-control masks are not disjoint valid input masks.'};
  if([...baseline,...perturbed].some(sample=>!Number.isSafeInteger(sample.liveInput)||sample.liveInput<0||sample.liveInput>0xffff||
    !Number.isSafeInteger(sample.pollSequence)))
    return{status:'incomplete',reason:'Paired playback is missing a native polled-input receipt.'};
  const ingressValues=[];
  if(ingress){
    if(!validIngressStrategy(ingress))
      return{status:'incomplete',reason:'The probe ingress-receipt strategy is malformed.'};
    for(const [label,window] of [['baseline',baseline],['perturbed',perturbed]])for(const [index,sample] of window.entries()){
      const receipt=sample.ingress;
      const context=receipt?.fields?.[ingress.contextField],active=receipt?.fields?.[ingress.activeField];
      if(receipt?.status!=='passed'||receipt.receiptId!==ingress.receipt||!Number.isSafeInteger(context)||
        (active!==0&&active!==1))
        return{status:'incomplete',reason:`The native ingress receipt is missing or invalid at ${label} sample ${index}.`,label,index,receipt:receipt??null};
      if(context!==ingress.contextValue)
        return{status:'incomplete',reason:`The native touch control was outside its declared gameplay context at ${label} sample ${index}.`,label,index,
          expectedContext:ingress.contextValue,actualContext:context};
      ingressValues.push({label,index,active});
    }
    const baselineIngress=ingressValues.filter(item=>item.label==='baseline');
    const perturbedIngress=ingressValues.filter(item=>item.label==='perturbed');
    if(baselineIngress.some(item=>item.active!==ingress.releasedValue))
      return{status:'failed',reason:'The baseline already held the native touch-fire control.'};
    if(perturbedIngress[0]?.active!==ingress.releasedValue||perturbedIngress.at(-1)?.active!==ingress.releasedValue)
      return{status:'incomplete',reason:'The native touch-fire receipt did not begin and end released.'};
    const firstPress=perturbedIngress.findIndex(item=>item.active===ingress.activeValue);
    const lastPress=perturbedIngress.findLastIndex(item=>item.active===ingress.activeValue);
    if(firstPress<1||lastPress<firstPress||lastPress>=perturbedIngress.length-1||
      perturbedIngress.slice(lastPress+1).some(item=>item.active!==ingress.releasedValue))
      return{status:'incomplete',reason:'The native touch-fire receipt did not capture a complete released→pressed→released interval.'};
    if([...baseline,...perturbed].some(sample=>(sample.liveInput&maskValue)!==0))
      return{status:'failed',reason:'Touch fire leaked into the native polled Replay input despite the declared ingress suppression.'};
  }
  const hasPerturb=ingress?ingressValues.some(item=>item.label==='perturbed'&&item.active===ingress.activeValue):
    perturbed.some(sample=>(sample.liveInput&maskValue)!==0);
  if(!hasPerturb)return{status:'incomplete',reason:'The declared live perturbation never appeared in its native ingress receipt.'};
  if(baseline.some(sample=>(sample.liveInput&maskValue)!==0))
    return{status:'failed',reason:'The baseline playback window already contains the declared perturbation.'};
  const mismatches=[];
  const baselineControlOrigin=baseline[0]?.controlFrame,perturbedControlOrigin=perturbed[0]?.controlFrame;
  const baselineConsumerOrigin=baseline[0]?.consumer?.sequence,perturbedConsumerOrigin=perturbed[0]?.consumer?.sequence;
  if(![baselineControlOrigin,perturbedControlOrigin,baselineConsumerOrigin,perturbedConsumerOrigin].every(Number.isSafeInteger))
    return{status:'incomplete',reason:'Paired playback has no initial control-frame or player-consumer sequence.'};
  for(let index=0;index<baseline.length;index++){
    const a=baseline[index],b=perturbed[index];
    if(!a.playback?.loaded||!b.playback?.loaded||!a.consumer||!b.consumer)
      return{status:'incomplete',reason:`Playback/consumer evidence is missing at paired window sample ${index}.`};
    if(![a.playback.frame,a.playback.cursor,a.playback.current,a.playback.previous,
      b.playback.frame,b.playback.cursor,b.playback.current,b.playback.previous,
      a.consumer.sequence,a.consumer.controlFrame,a.consumer.stage,a.consumer.current,a.consumer.previous,a.consumer.phase,a.consumer.validMask,
      b.consumer.sequence,b.consumer.controlFrame,b.consumer.stage,b.consumer.current,b.consumer.previous,b.consumer.phase,b.consumer.validMask,
      a.controlFrame,b.controlFrame].every(Number.isSafeInteger))
      return{status:'incomplete',reason:`Paired Replay or consumer fields are incomplete at window sample ${index}.`};
    if(a.consumer.phase!==1||b.consumer.phase!==1||
      (a.consumer.validMask&CONSUMED_INPUT_VALID)===0||(b.consumer.validMask&CONSUMED_INPUT_VALID)===0)
      return{status:'incomplete',reason:`A complete player-consumer receipt is missing at paired window sample ${index}.`};
    const allowedBaseline=exemptionMask;
    const allowedPerturbed=(ingress?0:maskValue)|exemptionMask;
    const unexpectedBaseline=a.liveInput&~allowedBaseline;
    const unexpectedPerturbed=b.liveInput&~allowedPerturbed;
    if(unexpectedBaseline||unexpectedPerturbed){
      mismatches.push({index,field:'unmappedLiveInput',baseline:unexpectedBaseline,perturbed:unexpectedPerturbed});
    }
    const fields=['sequence','stage','pollSequence'];
    for(const field of fields)if(a[field]!==b[field])mismatches.push({index,field,baseline:a[field],perturbed:b[field]});
    const baselineControlOffset=a.controlFrame-baselineControlOrigin,perturbedControlOffset=b.controlFrame-perturbedControlOrigin;
    if(baselineControlOffset!==perturbedControlOffset)
      mismatches.push({index,field:'controlFrameOffset',baseline:baselineControlOffset,perturbed:perturbedControlOffset});
    for(const field of ['frame','cursor','current','previous'])if(a.playback[field]!==b.playback[field])
      mismatches.push({index,field:`playback.${field}`,baseline:a.playback[field],perturbed:b.playback[field]});
    const baselineConsumerOffset=a.consumer.sequence-baselineConsumerOrigin;
    const perturbedConsumerOffset=b.consumer.sequence-perturbedConsumerOrigin;
    if(baselineConsumerOffset!==perturbedConsumerOffset)
      mismatches.push({index,field:'consumer.sequenceOffset',baseline:baselineConsumerOffset,perturbed:perturbedConsumerOffset});
    for(const field of ['stage','current','previous','phase','validMask'])if(a.consumer[field]!==b.consumer[field])
      mismatches.push({index,field:`consumer.${field}`,baseline:a.consumer[field],perturbed:b.consumer[field]});
  }
  const firstPress=ingress?-1:perturbed.findIndex(sample=>(sample.liveInput&maskValue)!==0);
  const releasedAfterPress=ingress?true:firstPress>=0&&perturbed.slice(firstPress+1).some(sample=>(sample.liveInput&maskValue)===0);
  if(!releasedAfterPress)return{status:'incomplete',reason:'The captured perturbation has no later native poll confirming release.'};
  if(mismatches.length)return{status:'failed',reason:'Live input changed the Replay stream or the player-consumed Replay state.',mismatches};
  return{status:'passed',scope:'paired-finite-live-input-isolation-window',completeReplay:false,
    perturbation:scene.probe.name,logicalMask:maskValue,observedPollSamples:perturbed.filter(sample=>sample.liveInput&maskValue).length,
    ...(ingress?{ingressStrategy:ingress.strategy,observedIngressSamples:ingressValues.filter(item=>item.label==='perturbed'&&item.active===ingress.activeValue).length}:{}),
    samples:baseline.length,replaySha256:expectedSha256,allowedControlMask:exemptionMask};
}

/** Require a declared pointer scene to match its file-bound extension and applied-point snapshots. */
export function assessReplayPointerConsumption({samples,extension,scene,expectedSha256}={}){
  const sequence=traceSequence(samples,scene?.boundary);if(sequence.status!=='passed')return sequence;
  if(scene?.kind!=='recording-extension'||scene.status!=='ready')
    return{status:'incomplete',reason:'The requested pointer scene is not ready.'};
  const {stage,requiredVersion,pointObservation}=scene;
  if(!extension||extension.version!==requiredVersion||!Array.isArray(extension.touch)||
    !Number.isSafeInteger(extension.stage)||extension.stage!==stage||
    !/^[a-f0-9]{64}$/.test(expectedSha256??'')||extension.sha256!==expectedSha256)
    return{status:'incomplete',reason:'A verified versioned pointer extension for the selected stage is unavailable.'};
  const expected=extension.touch.find(item=>item.stage===stage)?.entries;
  if(!Array.isArray(expected)||expected.length!==scene.trajectory.length)
    return{status:'incomplete',reason:'The verified extension does not contain exactly the declared finite pointer trajectory.'};
  if(!pointObservation?.source?.trim()||!pointObservation?.coordinates?.trim()||
    !Number.isFinite(pointObservation.tolerance)||pointObservation.tolerance<=0||
    !pointObservation.traceWords||!['count','id','xFloat32Bits','yFloat32Bits','released'].every(key=>Number.isSafeInteger(pointObservation.traceWords[key])))
    return{status:'incomplete',reason:'The title has no declared applied-point observation fields and coordinate units.'};
  const actionValues={down:0,move:1,up:2};
  for(let index=0;index<expected.length;index++){
    const actual=expected[index],declared=scene.trajectory[index];
    if(actual.id!==declared.id||actual.action!==actionValues[declared.action]||
      Math.abs(actual.x-declared.x)>pointObservation.tolerance||Math.abs(actual.y-declared.y)>pointObservation.tolerance)
      return{status:'failed',reason:'The hash-bound extension entries differ from the declared pointer trajectory.',index,declared,actual};
  }
  let previous=null,observedAdvance=false;
  for(const [index,sample] of samples.entries()){
    const motion=sample.motion;
    if(!motion||motion.playing!==true||motion.stage!==stage||![motion.touchCursor,motion.touchCount,motion.pointCount].every(Number.isSafeInteger))
      return{status:'incomplete',reason:`Native MotionTrack state is unavailable at sample ${sample.sequence}.`};
    if(motion.touchCount!==expected.length||motion.touchCursor<0||motion.touchCursor>expected.length||
      (previous!==null&&motion.touchCursor<previous))
      return{status:'failed',reason:'Native MotionTrack touch cursor/count disagrees with the independently parsed extension.',sample:sample.sequence,
        actual:{cursor:motion.touchCursor,count:motion.touchCount},expectedCount:expected.length};
    if(previous===null){
      if(motion.touchCursor!==0||motion.pointCount!==0)
        return{status:'incomplete',reason:'The trace began after pointer events had already been consumed.'};
    }else if(motion.touchCursor>previous){
      if(motion.touchCursor!==previous+1)
        return{status:'incomplete',reason:'Multiple pointer events were consumed between snapshots, so intermediate applied points are not observable.'};
      const event=expected[motion.touchCursor-1],point=motion.point;
      const tolerance=pointObservation.tolerance;
      if(event.action===0||event.action===1){
        if(motion.pointCount!==1||!point||point.id!==event.id||
          Math.abs(point.x-event.x)>tolerance||Math.abs(point.y-event.y)>tolerance||point.released!==false)
          return{status:'failed',reason:'A consumed pointer down/move entry is not reflected in the native applied-point snapshot.',
            sample:sample.sequence,cursor:motion.touchCursor,expected:event,actual:{count:motion.pointCount,point}};
      }else if(event.action===2){
        const released=motion.pointCount===0||(motion.pointCount===1&&point&&point.id===event.id&&
          Math.abs(point.x-event.x)<=tolerance&&Math.abs(point.y-event.y)<=tolerance&&point.released===true);
        if(!released)return{status:'failed',reason:'A consumed pointer-up entry did not release its applied pointer.',
          sample:sample.sequence,cursor:motion.touchCursor,expected:event,actual:{count:motion.pointCount,point}};
      }else return{status:'incomplete',reason:'The selected pointer trajectory contains an unsupported action.'};
      observedAdvance=true;
    }
    previous=motion.touchCursor;
  }
  if(!observedAdvance||previous!==expected.length)
    return{status:'incomplete',reason:'The native pointer extension cursor did not consume the entire finite trajectory window.',consumed:previous,expected:expected.length};
  return{status:'passed',scope:'finite-pointer-extension-consumption-window',completeReplay:false,
    stage,requiredVersion,consumed:previous,entries:expected.length,
    pointObservation:{source:pointObservation.source,coordinates:pointObservation.coordinates,tolerance:pointObservation.tolerance}};
}
