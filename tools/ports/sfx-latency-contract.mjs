import assert from 'node:assert/strict';

function incomplete(reason,details={}){return {status:'incomplete',reason,...details};}
function failed(reason,details={}){return {status:'failed',reason,...details};}
function measuredQueueTailInterval(event,contextSampleRate){
  const queuedDuration=event.queueFrames/event.queueRate;
  const queueTailLowerTime=event.queueClockBeforeTime+queuedDuration;
  const queueTailUpperTime=event.queueClockAfterTime+queuedDuration;
  const queueTailLowerFrame=queueTailLowerTime*contextSampleRate;
  const queueTailUpperFrame=queueTailUpperTime*contextSampleRate;
  return {queueTailLowerTime,queueTailUpperTime,queueTailLowerFrame,queueTailUpperFrame,
    queueClockSampleSpanTime:event.queueClockAfterTime-event.queueClockBeforeTime,
    queueClockSampleSpanFrames:event.queueClockAfterFrame-event.queueClockBeforeFrame};
}
function queueObservationSummary(event){
  if(!event||typeof event!=='object')return {};
  const hasTimes=Number.isFinite(event.queueClockBeforeTime)&&Number.isFinite(event.queueClockAfterTime);
  const hasFrames=Number.isFinite(event.queueClockBeforeFrame)&&Number.isFinite(event.queueClockAfterFrame);
  const hasQueue=Number.isFinite(event.queueFrames)&&event.queueFrames>=0&&Number.isFinite(event.queueRate)&&event.queueRate>0&&
    Number.isFinite(event.contextSampleRate)&&event.contextSampleRate>0;
  const queueDuration=hasQueue?event.queueFrames/event.queueRate:null;
  const queueTailLowerTime=hasTimes&&hasQueue?event.queueClockBeforeTime+queueDuration:null;
  const queueTailUpperTime=hasTimes&&hasQueue?event.queueClockAfterTime+queueDuration:null;
  const queueTailLowerFrame=Number.isFinite(queueTailLowerTime)?queueTailLowerTime*event.contextSampleRate:null;
  const queueTailUpperFrame=Number.isFinite(queueTailUpperTime)?queueTailUpperTime*event.contextSampleRate:null;
  return {queueClockBeforeTime:event.queueClockBeforeTime??null,queueClockAfterTime:event.queueClockAfterTime??null,
    queueClockBeforeFrame:event.queueClockBeforeFrame??null,queueClockAfterFrame:event.queueClockAfterFrame??null,
    queueClockSampleSpanTime:hasTimes?event.queueClockAfterTime-event.queueClockBeforeTime:null,
    queueClockSampleSpanFrames:hasFrames?event.queueClockAfterFrame-event.queueClockBeforeFrame:null,
    queueTailLowerTime,queueTailUpperTime,queueTailLowerFrame,queueTailUpperFrame,queueTailFrame:queueTailLowerFrame,
    queueSampleOrder:event.queueSampleOrder??null,
    queueMeasurementBoundary:'SDL queue length is bracketed by the before/after AudioContext clock samples; pass compares against the lower tail bound, fail compares against the upper bound.'};
}

/**
 * Classify actual PCM captured from one production voice against the end of
 * the already queued PCM. All time comparisons use AudioContext frames.
 */
function classifySfxLatencyEvidenceCore({contract,trace,observation,sourceLead,session}){
  if(!contract||typeof contract.game!=='string'||!contract.game||typeof contract.variant!=='string'||!contract.variant||
     !Number.isInteger(contract.logicalIndex)||contract.logicalIndex<0||!Number.isInteger(contract.handle)||contract.handle<=0||
     !Number.isInteger(contract.queueRate)||contract.queueRate<=0)
    return incomplete('a verified game-and-variant SFX latency contract is required');
  if(!trace||!Array.isArray(trace.uploads)||!Array.isArray(trace.events)||!Array.isArray(trace.routes)||!Array.isArray(trace.inputs)||!trace.dropped)
    return incomplete('native observation trace is missing bounded record and overflow metadata');
  if(!session||!Number.isInteger(session.id)||trace.sessionId!==session.id)
    return incomplete('native rows and PCM observation do not share one explicit probe session');
  if(trace.inputAmbiguous===true)
    return incomplete('the probe session captured an untrusted or repeated matching input before event association');
  if(Object.values(trace.dropped).some(value=>Number(value)>0))
    return incomplete('native trace dropped records; event-to-voice correlation is not complete',{dropped:trace.dropped});
  if(Array.isArray(trace.errors)&&trace.errors.length)
    return incomplete('native observation callback reported an error',{probeErrors:trace.errors});
  if(!Number.isFinite(session.contextSampleRate)||session.contextSampleRate<=0)
    return incomplete('AudioWorklet session clock or AudioContext sample rate is missing');

  const uploads=trace.uploads.filter(row=>row.handle===contract.handle);
  if(uploads.length!==1)return incomplete('expected exactly one recorded upload for the contract target handle',{targetUploadCount:uploads.length});
  const upload=uploads[0];
  if(!upload.uploadSuccess)return failed('target buffer upload to WebSfx failed',{upload});
  if(!Number.isInteger(upload.sourceRate)||upload.sourceRate<=0||!Number.isInteger(upload.sourceFrames)||upload.sourceFrames<=0)
    return incomplete('target upload lacks source sample rate or frame count',{upload});

  const handleEvents=trace.events.filter(row=>row.handle===contract.handle);
  if(handleEvents.length!==1)return incomplete('the target WebSfx voice had more than one native logical event in this session',
    {targetVoiceEventCount:handleEvents.length});
  const logicalEvents=trace.events.filter(row=>row.logicalIndex===contract.logicalIndex);
  if(logicalEvents.length!==1)return incomplete('expected exactly one native logical event for the contract sound',{targetEventCount:logicalEvents.length});
  const event=logicalEvents[0];
  if(event.handle!==contract.handle)return failed('native logical event maps to a different production buffer handle',{event,expectedHandle:contract.handle});
  if(event.sessionId!==session.id)return incomplete('logical sound event belongs to a different probe session',{eventSessionId:event.sessionId,sessionId:session.id});
  if(!Number.isInteger(event.inputSerial))return incomplete('the target logical sound event is not linked to a captured real input',{event});
  const inputs=trace.inputs.filter(row=>row.serial===event.inputSerial);
  if(inputs.length!==1)return incomplete('expected exactly one captured input associated with the target logical event',{inputCount:inputs.length,event});
  const input=inputs[0];
  const capturedInputs=trace.inputs.filter(row=>row.sessionId===session.id);
  if(capturedInputs.length!==1)return incomplete('the session must contain exactly one captured matching product input',{capturedInputCount:capturedInputs.length});
  if(input.sessionId!==session.id)return incomplete('captured input belongs to a different probe session',{inputSessionId:input.sessionId,sessionId:session.id});
  if(input.isTrusted!==true)return incomplete('the captured product input was not a trusted browser event',{input});
  if(!Number.isFinite(input.contextFrame)||!Number.isFinite(input.contextTime)||input.contextFrame>event.contextFrame)
    return incomplete('input and native sound event do not have ordered AudioContext frame timestamps',{input,event});
  if(input.contextSampleRate!==session.contextSampleRate||input.contextFrame!==input.contextTime*session.contextSampleRate)
    return incomplete('captured input frame is not derived from its single AudioContext time snapshot',{input,contextSampleRate:session.contextSampleRate});
  if(input.contextTime>event.queueClockBeforeTime)
    return incomplete('captured input occurred after the lower clock bound of the native queue observation',{input,event});
  if(contract.inputFilter&&Object.entries(contract.inputFilter).some(([key,value])=>input[key]!==value))
    return failed('captured browser input does not match the product scene contract',{input,expected:contract.inputFilter});
  if(event.voiceKey!==upload.voiceKey)return failed('logical event voice key does not match the uploaded target buffer',{event,upload});
  if(!Number.isInteger(event.queueFrames)||event.queueFrames<=0)
    return incomplete('logical sound event did not observe a nonempty mixed PCM queue',{queueFrames:event.queueFrames});
  if(!Number.isInteger(event.queueRate)||event.queueRate<=0||!Number.isFinite(event.contextSampleRate)||event.contextSampleRate<=0||
     !Number.isFinite(event.contextTime)||!Number.isFinite(event.contextFrame))
    return incomplete('queue frame rate or AudioContext frame mapping is missing',{event});
  if(event.contextFrame!==event.contextTime*event.contextSampleRate)
    return incomplete('native event frame is not derived from its single AudioContext time snapshot',{event});
  if(event.queueSampleOrder!=='cpp-sdl-queue-clock-bracket')
    return incomplete('native queue depth lacks its explicit AudioContext clock bracket',{event});
  if(!Number.isFinite(event.queueClockBeforeTime)||!Number.isFinite(event.queueClockAfterTime)||
     event.queueClockBeforeTime>event.queueClockAfterTime||
     event.queueClockBeforeFrame!==event.queueClockBeforeTime*event.contextSampleRate||
     event.queueClockAfterFrame!==event.queueClockAfterTime*event.contextSampleRate||
     event.contextTime!==event.queueClockBeforeTime||event.contextFrame!==event.queueClockBeforeFrame)
    return incomplete('native SDL queue query is not bracketed by ordered same-context clock samples',{event});
  if(event.queueRate!==contract.queueRate)
    return incomplete('native queue sample rate differs from the verified product contract',{queueRate:event.queueRate,expectedQueueRate:contract.queueRate});
  if(event.contextSampleRate!==session.contextSampleRate)
    return incomplete('native and AudioWorklet AudioContext sample rates disagree',{nativeSampleRate:event.contextSampleRate,workletSampleRate:session.contextSampleRate});

  const routes=trace.routes.filter(row=>row.eventSerial===event.serial);
  if(routes.length!==1)return incomplete('expected exactly one output-route record for the target logical event',{targetRouteCount:routes.length});
  const handleRoutes=trace.routes.filter(row=>row.handle===contract.handle);
  if(handleRoutes.length!==1)return incomplete('the target WebSfx voice had more than one output route in this session',
    {targetVoiceRouteCount:handleRoutes.length});
  const route=routes[0];
  if(route.sessionId!==session.id)return incomplete('output route belongs to a different probe session',{routeSessionId:route.sessionId,sessionId:session.id});
  if(route.handle!==event.handle||route.voiceKey!==event.voiceKey)
    return failed('target route does not match the logical event handle and voice key',{event,route});
  if(route.contextSampleRate!==event.contextSampleRate||!Number.isFinite(route.contextTime)||
     route.contextFrame!==route.contextTime*route.contextSampleRate)
    return incomplete('native route frame is not derived from its single AudioContext time snapshot',{route});
  if(route.queueSampleOrder!=='cpp-sdl-queue-clock-bracket'||!Number.isFinite(route.queueClockBeforeTime)||
     !Number.isFinite(route.queueClockAfterTime)||route.queueClockBeforeTime>route.queueClockAfterTime||
     route.queueClockBeforeFrame!==route.queueClockBeforeTime*route.contextSampleRate||
     route.queueClockAfterFrame!==route.queueClockAfterTime*route.contextSampleRate||
     route.contextTime!==route.queueClockBeforeTime||route.contextFrame!==route.queueClockBeforeFrame)
    return incomplete('native route queue query is not bracketed by ordered same-context clock samples',{route});
  if(route.pending||!route.direct||route.fallback||route.fallbackStarted)
    return failed('target SFX did not start through the direct WebSfx output route',{route});
  if(!Number.isFinite(route.contextFrame)||route.contextFrame<event.contextFrame)
    return incomplete('target direct route lacks an ordered AudioContext frame timestamp',{eventFrame:event.contextFrame,route});

  if(!observation||observation.eventSerial!==event.serial||observation.sessionId!==session.id)
    return incomplete('AudioWorklet did not capture actual PCM for the target native event',{observation});
  if(observation.handle!==event.handle||observation.voiceKey!==event.voiceKey)
    return failed('AudioWorklet PCM tap identity does not match the target native event',{event,observation});
  if(!Number.isFinite(observation.startPlanFrame)||!Number.isFinite(observation.markerReceivedAtFrame)||
     !Number.isFinite(observation.historyStartFrame)||!Number.isFinite(observation.searchedFromFrame)||
     !Number.isFinite(observation.searchCompleteFromFrame)||!Number.isFinite(observation.searchCompleteThroughFrame)||
     !Number.isInteger(observation.firstNonzeroFrame)||!Number.isInteger(observation.blockFrames)||observation.blockFrames<=0)
    return incomplete('AudioWorklet PCM onset or event-marker clock is incomplete',{observation});
  if(observation.incompleteReason)return incomplete('AudioWorklet could not recover the complete target onset from its bounded PCM history',
    {reason:observation.incompleteReason,observation});
  const searchedFrom=Math.ceil(observation.startPlanFrame);
  if(observation.searchedFromFrame!==searchedFrom||observation.searchCompleteFromFrame!==searchedFrom||
     observation.searchCompleteThroughFrame<observation.firstNonzeroFrame||observation.firstNonzeroFrame<searchedFrom)
    return incomplete('AudioWorklet did not verify PCM continuously from the scheduled target start',{searchedFrom,observation});
  if(!sourceLead||sourceLead.voiceKey!==event.voiceKey||sourceLead.sourceRate!==upload.sourceRate||
     sourceLead.sourceFrames!==upload.sourceFrames||sourceLead.renderedFromTargetBuffer!==true||
     sourceLead.eventSerial!==event.serial||sourceLead.sessionId!==session.id||
     sourceLead.contextSampleRate!==event.contextSampleRate||sourceLead.startPlanFrame!==observation.startPlanFrame||
     !Number.isInteger(sourceLead.frames)||sourceLead.frames<0||!Number.isFinite(sourceLead.offsetSeconds)||
     !Number.isFinite(sourceLead.playbackRate)||sourceLead.playbackRate<=0)
    return incomplete('source pre-roll was not rendered from the exact target buffer, offset, speed and sample rate',{sourceLead,upload,observation});
  if(observation.startPlanFrame<event.contextFrame)
    return incomplete('target source was scheduled before its correlated native logical sound event',
      {eventFrame:event.contextFrame,startPlanFrame:observation.startPlanFrame,markerReceivedAtFrame:observation.markerReceivedAtFrame});
  if(observation.firstNonzeroFrame<observation.historyStartFrame)
    return incomplete('the captured onset is no longer present in the retained PCM history',
      {firstNonzeroFrame:observation.firstNonzeroFrame,historyStartFrame:observation.historyStartFrame});
  const effectiveStartFrame=observation.firstNonzeroFrame-sourceLead.frames;
  const queueTail=measuredQueueTailInterval(event,event.contextSampleRate);
  if(!Number.isFinite(queueTail.queueTailLowerFrame)||!Number.isFinite(queueTail.queueTailUpperFrame)||
     queueTail.queueTailLowerFrame>queueTail.queueTailUpperFrame)
    return incomplete('mixed-queue tail interval could not be derived from the bracketed clock samples',{...queueTail,event});
  const boundaryEvidence={...queueTail,queueTailFrame:queueTail.queueTailLowerFrame,queueSampleOrder:event.queueSampleOrder};
  if(effectiveStartFrame>=queueTail.queueTailUpperFrame)
    return failed('target SFX PCM begins at or beyond the upper bound of the measured mixed-queue tail',
      {effectiveStartFrame,...boundaryEvidence,event,observation,sourceLead});
  if(effectiveStartFrame>=queueTail.queueTailLowerFrame)
    return incomplete('target SFX onset falls inside the measured queue-tail uncertainty interval',
      {effectiveStartFrame,...boundaryEvidence,event,observation,sourceLead});
  return {status:'passed',game:contract.game,variant:contract.variant,meaning:'actual target-voice PCM begins before the pre-existing SDL mixed PCM queue ends',
    logicalIndex:event.logicalIndex,handle:event.handle,voiceKey:event.voiceKey,queueFrames:event.queueFrames,
    queueRate:event.queueRate,contextSampleRate:event.contextSampleRate,eventContextFrame:event.contextFrame,
    firstNonzeroFrame:observation.firstNonzeroFrame,sourceLeadFrames:sourceLead.frames,effectiveStartFrame,
    ...boundaryEvidence,
    queueMeasurementBoundary:'SDL_GetAudioStreamQueued is bracketed by AudioContext currentTime snapshots; pass requires onset before the lower queue-tail bound, failure requires onset at or beyond the upper bound, and an onset within the interval is incomplete.',
    inputContextFrame:input.contextFrame,inputToEffectiveStartFrames:effectiveStartFrame-input.contextFrame,
    eventToEffectiveStartFrames:effectiveStartFrame-event.contextFrame,markerBlockFrames:observation.blockFrames};
}

/** Add the clock-bounded queue-tail interval to every verdict that has a target native event. */
export function classifySfxLatencyEvidence(evidence){
  const verdict=classifySfxLatencyEvidenceCore(evidence);
  const event=evidence?.trace?.events?.find(row=>row.logicalIndex===evidence?.contract?.logicalIndex);
  return event?{...verdict,...queueObservationSummary(event)}:verdict;
}

/** The processor source is also executed directly in Node tests. */
export function sfxLatencyPcmProcessorSource(){
  return `class SfxLatencyPcmTap extends AudioWorkletProcessor{
    constructor(){super();this.sessionId=0;this.marks=[];this.history=[];this.historyFrames=0;this.historyLimit=16384;
      this.port.onmessage=e=>{const m=e.data||{};if(m.kind==='clear'){this.sessionId=m.sessionId;this.marks=[];this.history=[];this.historyFrames=0;}
        else if(m.kind==='mark'&&m.sessionId===this.sessionId)this.marks.push({...m,markerReceivedAtFrame:null,scannedThroughFrame:null});};}
    process(inputs,outputs){const input=inputs[0]||[],output=outputs[0]||[],frames=(input[0]||output[0]||[]).length;
      for(const channel of output)channel.fill(0);
      const left=input[0]?input[0].slice():new Float32Array(frames),right=input[1]?input[1].slice():new Float32Array(frames);
      this.history.push({frame:currentFrame,frames,left,right});this.historyFrames+=frames;
      while(this.historyFrames>this.historyLimit&&this.history.length>1){const old=this.history.shift();this.historyFrames-=old.frames;}
      const historyStartFrame=this.history.length?this.history[0].frame:currentFrame;
      for(let markIndex=0;markIndex<this.marks.length;){const mark=this.marks[markIndex];
        if(mark.sessionId!==this.sessionId){this.marks.splice(markIndex,1);continue;}
        if(mark.markerReceivedAtFrame===null)mark.markerReceivedAtFrame=currentFrame;
        const searchedFromFrame=Math.ceil(mark.startPlanFrame);
        if(searchedFromFrame<historyStartFrame&&(mark.scannedThroughFrame===null||mark.scannedThroughFrame===undefined)){this.port.postMessage({kind:'incomplete',sessionId:this.sessionId,eventSerial:mark.eventSerial,
            markerReceivedAtFrame:mark.markerReceivedAtFrame,historyStartFrame,searchedFromFrame,reason:'marker-outside-history'});
          this.marks.splice(markIndex,1);continue;}
        let found=null;const scanFrom=Math.max(searchedFromFrame,mark.scannedThroughFrame??searchedFromFrame);
        if(scanFrom<historyStartFrame){this.port.postMessage({kind:'incomplete',sessionId:this.sessionId,eventSerial:mark.eventSerial,
            markerReceivedAtFrame:mark.markerReceivedAtFrame,historyStartFrame,searchedFromFrame,reason:'unscanned-history-gap'});
          this.marks.splice(markIndex,1);continue;}
        for(const block of this.history){const from=Math.max(scanFrom,block.frame),to=block.frame+block.frames;
          for(let frame=from;frame<to;frame++){const offset=frame-block.frame,l=block.left[offset]||0,r=block.right[offset]||0;
            if(l!==0||r!==0){found={frame,left:l,right:r};break;}}
          if(found)break;}
        if(found){this.port.postMessage({kind:'pcm',sessionId:this.sessionId,eventSerial:mark.eventSerial,
            firstNonzeroFrame:found.frame,markerReceivedAtFrame:mark.markerReceivedAtFrame,historyStartFrame,searchedFromFrame,
            searchCompleteFromFrame:searchedFromFrame,searchCompleteThroughFrame:found.frame,
            blockFrames:frames,sampleLeft:found.left,sampleRight:found.right,startPlanFrame:mark.startPlanFrame});
          this.marks.splice(markIndex,1);continue;}
        const scannedThrough=currentFrame+frames;mark.scannedThroughFrame=scannedThrough;
        if(Number.isFinite(mark.sourceEndFrame)&&scannedThrough>mark.sourceEndFrame){this.port.postMessage({kind:'incomplete',sessionId:this.sessionId,
            eventSerial:mark.eventSerial,markerReceivedAtFrame:mark.markerReceivedAtFrame,historyStartFrame,searchedFromFrame,reason:'source-rendered-silence'});
          this.marks.splice(markIndex,1);continue;}
        markIndex++;}
      return true;}
  }
  registerProcessor('sfx-latency-pcm-tap',SfxLatencyPcmTap);`;
}

function validateProbeConfig(config){
  if(!config||typeof config.game!=='string'||!config.game||typeof config.variant!=='string'||!config.variant)
    throw new TypeError('SFX probe setup requires explicit game and variant');
  for(const name of ['probeGlobal','enabledGlobal','traceGlobal'])
    if(typeof config[name]!=='string'||!/^[$A-Z_a-z][$\w]*$/.test(config[name]))throw new TypeError(`invalid ${name}`);
  if(!Array.isArray(config.targetHandles)||!config.targetHandles.length||config.targetHandles.some(v=>!Number.isInteger(v)||v<=0))
    throw new TypeError('SFX probe setup requires one or more positive target handles');
  if(!Number.isInteger(config.logicalIndex)||config.logicalIndex<0)throw new TypeError('SFX probe setup requires the target logical sound index');
  if(!config.inputFilter||typeof config.inputFilter.type!=='string'||!config.inputFilter.type)
    throw new TypeError('SFX probe setup requires an explicit browser input filter');
  return {game:config.game,variant:config.variant,logicalIndex:config.logicalIndex,inputFilter:{...config.inputFilter},
    probeGlobal:config.probeGlobal,enabledGlobal:config.enabledGlobal,traceGlobal:config.traceGlobal,
    targetHandles:[...new Set(config.targetHandles)]};
}

/** Returns a self-contained source string for Page.addInitScript(). */
export function sfxLatencyProbeInitSource(input){
  const config=validateProbeConfig(input);
  return `(()=>{
  const root=globalThis,config=${JSON.stringify(config)},PROCESSOR=${JSON.stringify(sfxLatencyPcmProcessorSource())};
  if(root[config.probeGlobal])return;
  // Session-local rows stay bounded; every overflow makes the shared classifier incomplete.
  const MAX=512;
  const state={module:null,context:null,pool:null,voices:new Map(),tapModuleUrl:null,sessionId:0,active:false,
    events:[],routes:[],inputs:[],observations:[],starts:[],errors:[],dropped:0,pending:new Map(),uploadFacts:[],
    originalPoolStart:null,originalSourceStart:null,wrappedSourceStart:null,blockFrames:null,contextSampleRate:null,
    starting:null,lastNodeStart:null,mutation:null,inputFilter:null,inputCaptureArmed:false,inputMatch:null,inputConsumed:false,inputAmbiguous:false,nextInputSerial:1};
  const add=(rows,row)=>{if(rows.length<MAX)rows.push(row);else ++state.dropped;};
  const copy=row=>row&&({...row});
  const trace=()=>root[config.traceGlobal]||null;
  const noteError=(where,error)=>add(state.errors,{where,message:String(error&&error.message||error)});
  function onUpload(row){add(state.uploadFacts,copy(row));}
  function captureInput(event){try{if(!state.active||!state.inputCaptureArmed||!state.inputFilter)return;
      const filter=state.inputFilter;
      if(event.type!==filter.type||Object.entries(filter).some(([key,value])=>key!=='type'&&event[key]!==value))return;
      const contextTime=state.context.currentTime,contextSampleRate=state.context.sampleRate,contextFrame=contextTime*contextSampleRate;
      const row={serial:state.nextInputSerial++,sessionId:state.sessionId,type:event.type,key:event.key??null,code:event.code??null,
        isTrusted:event.isTrusted===true,
        button:Number.isInteger(event.button)?event.button:null,pointerType:event.pointerType??null,timeStamp:Number(event.timeStamp),
        contextTime,contextSampleRate,contextFrame};
      if(!row.isTrusted){state.inputAmbiguous=true;row.reason='untrusted-matching-input';add(state.inputs,row);return;}
      if(state.inputMatch&&!state.inputConsumed){state.inputAmbiguous=true;row.duplicateOf=state.inputMatch.serial;
        row.reason='repeated-matching-input-before-event-association';add(state.inputs,row);return;}
      if(state.inputConsumed)return;
      state.inputMatch=row;add(state.inputs,row);
    }catch(error){noteError('input-capture',error);}}
  // The shell installs a native-key capture handler after its init scripts and
  // stops propagation there. Register this dormant observer first; sessions
  // only arm data capture and never reorder the DOM listener.
  root.addEventListener(config.inputFilter.type,captureInput,true);
  function onNativeEvent(row){if(!state.active)return;const tagged={...copy(row),sessionId:state.sessionId},key=String(row.voiceKey);
    const target=state.voices.get(key),isTargetEvent=!!target&&target.handle===row.handle&&row.logicalIndex===config.logicalIndex;
    if(isTargetEvent&&state.inputMatch&&!state.inputConsumed){tagged.inputSerial=state.inputMatch.serial;state.inputConsumed=true;}
    add(state.events,tagged);
    if(isTargetEvent&&Number.isInteger(tagged.inputSerial)){const queue=state.pending.get(key)||[];queue.push(tagged);state.pending.set(key,queue);}
    if(isTargetEvent&&state.mutation?.armed&&row.handle===state.mutation.handle&&!state.mutation.eventSerial){
      if(!Number.isFinite(row.queueClockBeforeTime)||!Number.isFinite(row.queueClockAfterTime)||
         row.queueClockBeforeTime>row.queueClockAfterTime){state.mutation.status='invalid-clock-bracket';
        state.mutation.reason='native queue query lacks ordered AudioContext clock bounds';return;}
      const queuedDuration=row.queueFrames/row.queueRate;
      state.mutation.eventSerial=row.serial;state.mutation.queueTailLowerTime=row.queueClockBeforeTime+queuedDuration;
      state.mutation.queueTailUpperTime=row.queueClockAfterTime+queuedDuration;state.mutation.queueTailTime=state.mutation.queueTailUpperTime;
      state.mutation.queueTailLowerFrame=state.mutation.queueTailLowerTime*row.contextSampleRate;
      state.mutation.queueTailUpperFrame=state.mutation.queueTailUpperTime*row.contextSampleRate;
      state.mutation.queueClockSampleSpanTime=row.queueClockAfterTime-row.queueClockBeforeTime;
      state.mutation.queueClockSampleSpanFrames=row.queueClockAfterFrame-row.queueClockBeforeFrame;
      state.mutation.queueFrames=row.queueFrames;state.mutation.queueRate=row.queueRate;state.mutation.status='waiting-for-target-source';}}
  function onRoute(row){if(state.active)add(state.routes,{...copy(row),sessionId:state.sessionId});}
  function onAudioShutdown(){detach();state.active=false;state.events=[];state.routes=[];state.inputs=[];state.observations=[];state.starts=[];
    state.pending.clear();state.uploadFacts=[];state.mutation=null;}
  function detach(){
    state.active=false;state.inputCaptureArmed=false;state.inputFilter=null;state.inputMatch=null;state.inputConsumed=false;
    for(const tap of state.voices.values()){try{tap.merge.disconnect(tap.node);tap.node.disconnect();tap.zero.disconnect();tap.node.port.close();}catch(error){noteError('detach',error);}}
    state.voices.clear();
    if(state.pool&&state.originalPoolStart)state.pool.start=state.originalPoolStart;
    if(state.originalSourceStart&&AudioBufferSourceNode.prototype.start===state.wrappedSourceStart)
      AudioBufferSourceNode.prototype.start=state.originalSourceStart;
    if(state.tapModuleUrl){URL.revokeObjectURL(state.tapModuleUrl);state.tapModuleUrl=null;}
    state.pool=null;state.originalPoolStart=null;state.originalSourceStart=null;state.wrappedSourceStart=null;
    state.context=null;state.module=null;state.starting=null;state.lastNodeStart=null;state.mutation=null;
    state.inputFilter=null;state.inputCaptureArmed=false;state.inputMatch=null;state.inputConsumed=false;state.inputAmbiguous=false;
  }
  async function install(module){
    if(!module||!module.SDL3?.audioContext)throw new Error(config.game+' '+config.variant+' SDL3 AudioContext is unavailable');
    const context=module.SDL3.audioContext,pool=module.eaglerSfxOutput,native=trace();
    if(!pool||pool.context!==context||!pool.voices)throw new Error('production WebSfx voice pool is unavailable');
    if(state.pool===pool&&state.context===context)return snapshot();
    detach();state.module=module;state.context=context;state.pool=pool;state.contextSampleRate=context.sampleRate;
    if(!native||!Array.isArray(native.uploads))throw new Error('bounded native SFX upload trace is unavailable');
    state.uploadFacts=native.uploads.map(copy);
    const facts=[];
    for(const handle of config.targetHandles){const rows=state.uploadFacts.filter(row=>row.handle===handle&&row.uploadSuccess);
      if(rows.length!==1)throw new Error('expected exactly one successful native upload for target buffer '+handle);
      const voiceKey=String(rows[0].voiceKey),voice=pool.voices.get(Number(voiceKey));
      if(!voice||!voice.merge||!voice.buffer)throw new Error('production WebSfx graph missing target voice '+voiceKey);
      if(voice.buffer.sampleRate!==rows[0].sourceRate||voice.buffer.length!==rows[0].sourceFrames)
        throw new Error('uploaded source metadata does not match the production AudioBuffer');
      facts.push({handle,voiceKey,sourceRate:rows[0].sourceRate,sourceFrames:rows[0].sourceFrames});}
    if(!context.audioWorklet)throw new Error('AudioWorklet is unavailable on the production AudioContext');
    const blob=new Blob([PROCESSOR],{type:'text/javascript'});state.tapModuleUrl=URL.createObjectURL(blob);
    await context.audioWorklet.addModule(state.tapModuleUrl);
    for(const fact of facts){const voice=pool.voices.get(Number(fact.voiceKey));
      const node=new AudioWorkletNode(context,'sfx-latency-pcm-tap',{numberOfInputs:1,numberOfOutputs:1,
        outputChannelCount:[2],channelCount:2,channelCountMode:'explicit',channelInterpretation:'speakers'});
      const zero=context.createGain();zero.gain.value=0;
      node.port.onmessage=e=>{const m=e.data||{};if(m.sessionId!==state.sessionId)return;
        if(m.kind==='pcm'||m.kind==='incomplete'){state.blockFrames=m.blockFrames||state.blockFrames;
          add(state.observations,{...m,handle:fact.handle,voiceKey:fact.voiceKey});}};
      // This is a parallel read-only branch. The original merge→destination edge stays connected.
      voice.merge.connect(node);node.connect(zero);zero.connect(context.destination);
      state.voices.set(fact.voiceKey,{handle:fact.handle,voiceKey:fact.voiceKey,voice,merge:voice.merge,node,zero,buffer:voice.buffer,
        sourceRate:fact.sourceRate,sourceFrames:fact.sourceFrames});}
    const originalPoolStart=pool.start;state.originalPoolStart=originalPoolStart;
    pool.start=function(voice){let event=null,voiceKey=null,target=null;
      try{
        try{for(const [key,value]of pool.voices)if(value===voice){voiceKey=String(key);break;}
          target=voiceKey&&state.voices.get(voiceKey);const queue=voiceKey?state.pending.get(voiceKey):null;
          event=queue?.shift()||null;if(queue&&!queue.length)state.pending.delete(voiceKey);
          state.lastNodeStart=null;state.starting=target&&event?{target,event,voiceKey}:null;
        }catch(error){noteError('pool-start-setup',error);state.starting=null;}
        const result=originalPoolStart.call(this,voice);
        try{
          if(target&&event&&state.lastNodeStart?.eventSerial===event.serial){
            const contextTime=context.currentTime,contextSampleRate=context.sampleRate;
            const start={eventSerial:event.serial,handle:event.handle,voiceKey,contextTime,contextSampleRate,contextFrame:contextTime*contextSampleRate,
              startPlanFrame:state.lastNodeStart.startPlanFrame,startPlanTime:state.lastNodeStart.startPlanTime,
              offsetSeconds:voice.offset,playbackRate:voice.speed,
              loop:!!voice.loop,sourceRate:voice.buffer.sampleRate,sourceFrames:voice.buffer.length,
              sourceDuration:voice.buffer.duration,sessionId:state.sessionId,mutated:state.lastNodeStart.mutated};add(state.starts,start);
            target.node.port.postMessage({kind:'mark',sessionId:state.sessionId,eventSerial:event.serial,
              startPlanFrame:start.startPlanFrame,sourceEndFrame:start.startPlanFrame+
                (start.sourceDuration-start.offsetSeconds)*context.sampleRate/start.playbackRate});
            if(state.mutation?.eventSerial===event.serial){state.mutation.applied=!!start.mutated;
              state.mutation.status=start.mutated?'applied':'not-applied';state.mutation.actualStartFrame=start.startPlanFrame;
              state.mutation.actualStartTime=start.startPlanTime;}}
        }catch(error){noteError('pool-start-observer',error);}
        return result;
      }finally{state.starting=null;}};
    const proto=AudioBufferSourceNode.prototype,originalSourceStart=proto.start;state.originalSourceStart=originalSourceStart;
    const wrappedSourceStart=function(...args){
      const active=state.starting,target=active?.target;let callArgs=args,mutated=false,startPlanFrame=null;
      try{if(target&&this.buffer===target.buffer){let when=args.length?Number(args[0]):0;
          if(!Number.isFinite(when))throw new Error('target BufferSource.start received a non-finite time');
          const now=context.currentTime,normalTime=Math.max(now,when),mutation=state.mutation;
          if(mutation?.armed&&mutation.eventSerial===active.event.serial){
            const tail=mutation.queueTailTime;
            if(Number.isFinite(tail)&&tail>now){when=tail;callArgs=args.length?[when,...args.slice(1)]:[when];mutated=true;mutation.status='scheduling-at-measured-queue-tail';}
            else{mutation.status='not-applied-queue-tail-not-future';mutation.reason='measured queue tail was not ahead of AudioContext.currentTime';}}
          const startPlanTime=Math.max(now,when);startPlanFrame=startPlanTime*context.sampleRate;
          state.lastNodeStart={eventSerial:active.event.serial,startPlanFrame,startPlanTime,mutated,normalStartFrame:normalTime*context.sampleRate};}}
      catch(error){noteError('buffer-source-start-observer',error);callArgs=args;mutated=false;}
      const result=originalSourceStart.apply(this,callArgs);
      if(target&&this.buffer===target.buffer&&state.lastNodeStart?.eventSerial===active?.event.serial)
        state.lastNodeStart.mutated=mutated;
      return result;};
    state.wrappedSourceStart=wrappedSourceStart;proto.start=wrappedSourceStart;
    state.active=false;state.blockFrames=null;return snapshot();}
  function begin(){if(!state.context||!state.voices.size)throw new Error('install() must complete before begin()');
    if([...state.voices.values()].some(row=>row.voice.playing||row.voice.node))
      throw new Error('target WebSfx voices must be idle before opening a measurement session');
    state.inputCaptureArmed=false;state.inputFilter=null;state.inputMatch=null;state.inputConsumed=false;state.inputAmbiguous=false;
    state.sessionId++;state.active=true;state.events=[];state.routes=[];state.inputs=[];state.observations=[];state.starts=[];state.errors=[];state.dropped=0;
    state.pending.clear();state.blockFrames=null;state.lastNodeStart=null;state.mutation=null;
    const native=trace();if(native){native.sequence=0;native.uploads=[];native.events=[];native.routes=[];native.errors=[];
      native.dropped={uploads:0,events:0,routes:0,errors:0};}
    for(const tap of state.voices.values())tap.node.port.postMessage({kind:'clear',sessionId:state.sessionId});
    return {game:config.game,variant:config.variant,session:{id:state.sessionId,contextSampleRate:state.context.sampleRate},
      targets:[...state.voices.values()].map(v=>({handle:v.handle,voiceKey:v.voiceKey}))};}
  function armInputCapture(filter){if(!state.active)throw new Error('begin() must complete before arming an input capture');
    if(state.inputCaptureArmed)throw new Error('an input capture is already armed');
    if(!filter||filter.type!==config.inputFilter.type||Object.entries(config.inputFilter).some(([key,value])=>filter[key]!==value))
      throw new Error('input filter must match the product scene contract');
    state.inputFilter={...filter};state.inputMatch=null;state.inputConsumed=false;state.inputCaptureArmed=true;
    return copy(state.inputFilter);}
  async function measureSourceLead(eventSerial){
    const started=state.starts.filter(row=>row.sessionId===state.sessionId&&row.eventSerial===eventSerial);
    if(started.length!==1)throw new Error('expected exactly one production WebSfx start record for event '+eventSerial);
    const start=started[0],target=state.voices.get(start.voiceKey);if(!target)throw new Error('target WebSfx tap is not installed');
    if(start.sourceRate!==target.sourceRate||start.sourceRate!==target.buffer.sampleRate||start.sourceFrames!==target.buffer.length)
      throw new Error('target source sample metadata changed during playback');
    if(!(start.playbackRate>0)||!Number.isFinite(start.offsetSeconds)||start.offsetSeconds<0||start.offsetSeconds>=target.buffer.duration)
      throw new Error('target source offset or playback rate is not renderable');
    const frames=Math.ceil((target.buffer.duration-start.offsetSeconds)*state.context.sampleRate/start.playbackRate);
    if(!Number.isSafeInteger(frames)||frames<=0||frames>0xffffffff)throw new Error('target source render length is outside OfflineAudioContext frame range');
    const offline=new OfflineAudioContext(2,frames,state.context.sampleRate),source=offline.createBufferSource();
    source.buffer=target.buffer;source.playbackRate.value=start.playbackRate;source.connect(offline.destination);source.start(0,start.offsetSeconds);
    const rendered=await offline.startRendering();let first=-1;
    for(let i=0;i<rendered.length&&first<0;i++)for(let channel=0;channel<rendered.numberOfChannels;channel++)
      if(rendered.getChannelData(channel)[i]!==0){first=i;break;}
    if(first<0)throw new Error('actual target AudioBuffer contains no nonzero rendered PCM after its playback offset');
    return {eventSerial,sessionId:start.sessionId,voiceKey:start.voiceKey,sourceRate:start.sourceRate,sourceFrames:start.sourceFrames,
      renderedFromTargetBuffer:true,contextSampleRate:state.context.sampleRate,offsetSeconds:start.offsetSeconds,playbackRate:start.playbackRate,
      frames:first,startPlanFrame:start.startPlanFrame,
      meaning:'same-browser OfflineAudioContext render of the actual source buffer at the captured offset and playback rate'};}
  function armQueueTailMutation(handle){const target=[...state.voices.values()].find(row=>row.handle===handle);
    if(!state.active||!target)throw new Error('queue-tail mutation target is not an installed active voice');
    if(state.mutation)throw new Error('a queue-tail mutation is already armed');
    state.mutation={handle,sessionId:state.sessionId,armed:true,eventSerial:null,queueTailTime:null,queueTailLowerTime:null,
      queueTailUpperTime:null,queueTailLowerFrame:null,queueTailUpperFrame:null,queueClockSampleSpanTime:null,
      queueClockSampleSpanFrames:null,applied:false,status:'waiting-for-native-event'};
    return copy(state.mutation);}
  function restoreQueueTailMutation(){const previous=copy(state.mutation);state.mutation=null;return previous;}
  function mutationStatus(){return copy(state.mutation);}
  async function renderQueueTailMutation({event,sourceLead}){
    if(!state.context||!event||!sourceLead)throw new Error('event and rendered source lead are required');
    const target=state.voices.get(String(event.voiceKey));if(!target)throw new Error('event target voice buffer is unavailable');
    if(event.queueRate<=0||event.queueFrames<=0)throw new Error('queue-tail control needs a nonempty measured SDL queue');
    const rate=state.context.sampleRate,tailSeconds=event.queueFrames/event.queueRate;
    const sourceDuration=(target.buffer.duration-sourceLead.offsetSeconds)/sourceLead.playbackRate;
    const totalFrames=Math.ceil((tailSeconds+sourceDuration)*rate)+1;
    if(!Number.isSafeInteger(totalFrames)||totalFrames<=0||totalFrames>0xffffffff)throw new Error('queue-tail render length is outside OfflineAudioContext frame range');
    const render=async(withEffect)=>{const context=new OfflineAudioContext(2,totalFrames,rate);
      const queue=context.createBuffer(2,totalFrames,rate);queue.getChannelData(0).fill(.125);queue.getChannelData(1).fill(-.125);
      const queued=context.createBufferSource();queued.buffer=queue;queued.connect(context.destination);queued.start(0);queued.stop(tailSeconds);
      if(withEffect){const source=context.createBufferSource();source.buffer=target.buffer;source.playbackRate.value=sourceLead.playbackRate;
        source.connect(context.destination);source.start(tailSeconds,sourceLead.offsetSeconds);}
      return context.startRendering();};
    const [baseline,combined]=await Promise.all([render(false),render(true)]);let first=-1;
    for(let i=0;i<combined.length&&first<0;i++)for(let channel=0;channel<combined.numberOfChannels;channel++)
      if(combined.getChannelData(channel)[i]-baseline.getChannelData(channel)[i]!==0){first=i;break;}
    if(first<0)throw new Error('queue-tail control rendered no target-source PCM contribution');
    const queueTail=measuredQueueTailInterval(event,rate);
    return {firstNonzeroFrame:event.contextFrame+first,offlineFirstNonzeroFrame:first,
      ...queueTail,sourceLeadFrames:sourceLead.frames,
      renderedFrames:combined.length,queueFrames:event.queueFrames,queueRate:event.queueRate,contextSampleRate:rate,
      meaning:'supplemental target AudioBuffer PCM delta in a queue-tail OfflineAudioContext render'};}
  function snapshot(){const native=trace(),nativeSnapshot=native?JSON.parse(JSON.stringify(native)):null;
    if(nativeSnapshot){const nativeEvents=nativeSnapshot.events||[],nativeRoutes=nativeSnapshot.routes||[];
      const eventBySerial=new Map(state.events.map(row=>[row.serial,row]));const routeBySerial=new Map(state.routes.map(row=>[row.serial,row]));
      let gaps=0;for(const row of nativeEvents){const callback=eventBySerial.get(row.serial);if(!callback)++gaps;else{
        row.sessionId=callback.sessionId;if(Number.isInteger(callback.inputSerial))row.inputSerial=callback.inputSerial;}}
      for(const row of nativeRoutes){const callback=routeBySerial.get(row.serial);if(!callback)++gaps;else row.sessionId=callback.sessionId;}
      nativeSnapshot.uploads=state.uploadFacts.map(copy);nativeSnapshot.inputs=state.inputs.map(copy);
      nativeSnapshot.inputAmbiguous=state.inputAmbiguous;
      nativeSnapshot.dropped={...(nativeSnapshot.dropped||{}),probe:state.dropped,probeCallbacks:gaps};
      nativeSnapshot.errors=[...(nativeSnapshot.errors||[]),...state.errors.map(copy)];nativeSnapshot.sessionId=state.sessionId;}
    return {game:config.game,variant:config.variant,installed:!!state.context&&state.voices.size>0,
      targets:[...state.voices.values()].map(v=>({handle:v.handle,voiceKey:v.voiceKey})),
      sessionId:state.sessionId,active:state.active,inputAmbiguous:state.inputAmbiguous,contextSampleRate:state.contextSampleRate,
      markerBlockFrames:state.blockFrames,session:{id:state.sessionId,markerBlockFrames:state.blockFrames,contextSampleRate:state.contextSampleRate},
      uploads:state.uploadFacts.map(copy),nativeTrace:nativeSnapshot,events:state.events.map(copy),routes:state.routes.map(copy),inputs:state.inputs.map(copy),
      starts:state.starts.map(copy),observations:state.observations.map(copy),errors:state.errors.map(copy),dropped:state.dropped,
      mutation:copy(state.mutation)};}
  const api={install,begin,armInputCapture,snapshot,measureSourceLead,renderQueueTailMutation,armQueueTailMutation,restoreQueueTailMutation,mutationStatus,
    onUpload,onNativeEvent,onRoute,onAudioShutdown,detach,get session(){return {id:state.sessionId,markerBlockFrames:state.blockFrames,
      contextSampleRate:state.contextSampleRate};}};
  root[config.probeGlobal]=api;root[config.enabledGlobal]=true;
})();`;
}

/** Assert a real same-runtime queue-tail mutation through the ordinary classifier. */
export function assertQueueTailMutationIsRejected({contract,trace,observation,sourceLead,session,mutation}){
  assert.equal(mutation?.applied,true,'the target BufferSource.start must be mutated in the live runtime');
  const event=trace?.events?.find(row=>row.logicalIndex===contract?.logicalIndex&&row.handle===contract?.handle);
  assert.ok(event,'the live mutation must retain its native logical event');
  assert.equal(mutation.sessionId,session?.id,'the live mutation and PCM evidence must share the probe session');
  assert.equal(mutation.eventSerial,event.serial,'the live mutation must target the observed logical event');
  assert.equal(mutation.queueFrames,event.queueFrames,'the mutation must use the event-time SDL queue depth');
  assert.equal(mutation.queueRate,event.queueRate,'the mutation must use the event-time SDL queue rate');
  const queueTail=measuredQueueTailInterval(event,session.contextSampleRate);
  assert.equal(mutation.queueTailLowerTime,queueTail.queueTailLowerTime,'the mutation must retain the lower clock-bounded queue tail');
  assert.equal(mutation.queueTailUpperTime,queueTail.queueTailUpperTime,'the mutation must retain the upper clock-bounded queue tail');
  assert.equal(mutation.queueTailLowerFrame,queueTail.queueTailLowerFrame,'the mutation must retain the lower queue-tail frame');
  assert.equal(mutation.queueTailUpperFrame,queueTail.queueTailUpperFrame,'the mutation must retain the upper queue-tail frame');
  assert.equal(mutation.queueClockSampleSpanTime,queueTail.queueClockSampleSpanTime,'the mutation must retain the clock sample span in seconds');
  assert.equal(mutation.queueClockSampleSpanFrames,queueTail.queueClockSampleSpanFrames,'the mutation must retain the clock sample span in frames');
  assert.equal(mutation.queueTailTime,queueTail.queueTailUpperTime,'the live negative control must schedule at the upper measured queue-tail bound');
  assert.equal(mutation.actualStartTime,queueTail.queueTailUpperTime,'the real target BufferSource.start must receive the upper queue-tail bound');
  assert.equal(mutation.actualStartFrame,queueTail.queueTailUpperFrame,
    'the actual BufferSource start frame must equal the upper queue-tail frame using the classifier conversion order');
  const verdict=classifySfxLatencyEvidence({contract,trace,observation,sourceLead,session});
  assert.equal(verdict.status,'failed','the live queue-tail PCM mutation must fail the latency contract');
  assert.match(verdict.reason,/upper bound of the measured mixed-queue tail/);
  return verdict;
}
