import {createHash} from 'node:crypto';
import {isDeepStrictEqual} from 'node:util';
import {compareReplayCadenceRuns} from '../../common/testkit/presentation-lab/replay-pair.mjs';
import {PRESENTATION_AUDIO_SCHEMA,PRESENTATION_AUDIO_LIMITS,TH08_PRESENTATION_AUDIO_CONTRACT,
  presentationAudioFrameClockPlan,decodePresentationAudioClockBlock} from './presentation-audio-browser.mjs';

const integer=Number.isSafeInteger;
const finite=Number.isFinite;
function requireValue(test,message){if(!test)throw Error(message);}
// Assertion failures must not format multi-megabyte PCM or Replay structures.
export function assertPresentationAudioEqual(actual,expected,label){
  requireValue(isDeepStrictEqual(actual,expected),`${label}: retained values differ`);
}
const same=assertPresentationAudioEqual;
const sha=value=>createHash('sha256').update(JSON.stringify(value)).digest('hex');
const stripRoute=row=>{const {routeSerial,...rest}=row;return rest;};
function only(rows,predicate,label){const selected=rows.filter(predicate);requireValue(selected.length===1,`${label}: expected one actual record, found ${selected.length}`);return selected[0];}
function array(value,label){requireValue(Array.isArray(value),`${label} is missing`);return value;}
function zeroDrops(value,label){requireValue(value&&Object.keys(value).sort().join(',')==='errors,events,routes,uploads',`${label}: incomplete drop counters`);
  requireValue(Object.values(value).every(count=>count===0),`${label}: actual native trace overflow`);}
function timestamp(value,label){requireValue(finite(value)&&value>=0,`${label}: invalid AudioContext time`);}
function nativeClock(row,rate,label){
  requireValue(row.contextSampleRate===rate,`${label}: another AudioContext sample rate`);
  timestamp(row.contextTime,label);requireValue(row.contextFrame===row.contextTime*rate,`${label}: context frame mismatch`);
  timestamp(row.queueClockBeforeTime,label);timestamp(row.queueClockAfterTime,label);
  requireValue(row.queueClockBeforeTime<=row.queueClockAfterTime&&row.contextTime===row.queueClockBeforeTime&&
    row.queueClockBeforeFrame===row.queueClockBeforeTime*rate&&row.queueClockAfterFrame===row.queueClockAfterTime*rate&&
    row.queueSampleOrder==='cpp-sdl-queue-clock-bracket',`${label}: native queue clock bracket missing`);
  requireValue(integer(row.queueFrames)&&row.queueFrames>=0&&integer(row.queueRate)&&row.queueRate>0,`${label}: invalid actual SDL queue observation`);
}
function firstSampleAtOrAfter(time,rate){
  let frame=Math.floor(time*rate);
  while(frame/rate<time)frame++;
  while(frame>0&&(frame-1)/rate>=time)frame--;
  return frame;
}
function pcmWords(words,size,label){
  requireValue(Array.isArray(words)&&words.length===size&&words.every(word=>integer(word)&&word>=0&&word<=0xffffffff),`${label}: missing actual float32 PCM words`);
  const bits=Uint32Array.from(words),samples=new Float32Array(bits.buffer);
  requireValue(samples.every(finite),`${label}: non-finite actual PCM`);return samples;
}

/** Decode every retained sample; neither an onset summary nor a later match is trusted. */
export function readPresentationPcmWindow(row,plan,label){
  requireValue(row&&integer(row.startFrame)&&integer(row.endFrame)&&row.endFrame>row.startFrame&&
    row.startFrame>=plan.anchorFrame&&row.endFrame<=plan.endFrame,`${label}: invalid retained PCM interval`);
  const length=row.endFrame-row.startFrame;
  requireValue(length<=plan.sampleRate+PRESENTATION_AUDIO_LIMITS.prefixFrames,`${label}: unbounded PCM interval`);
  const left=new Float32Array(length),right=new Float32Array(length),blocks=array(row.blocks,label);
  requireValue(blocks.length>0,`${label}: no actual PCM blocks`);
  let cursor=row.startFrame,previous=null;
  for(const block of blocks){const decoded=decodePresentationAudioClockBlock(block,plan);
    requireValue(integer(block.processSequence)&&block.processSequence>0&&integer(block.currentFrame)&&block.currentFrame>=0,`${label}: missing actual worklet callback identity`);
    if(previous)requireValue(previous.endFrame===decoded.firstFrame&&previous.sequence+1===block.processSequence,`${label}: missing or reordered actual render quantum`);
    requireValue(integer(block.inputChannels)&&block.inputChannels>=0&&block.inputChannels<=2,`${label}: invalid actual input channels`);
    const l=pcmWords(block.leftWords,block.quantumFrames,label),r=pcmWords(block.rightWords,block.quantumFrames,label);
    const from=Math.max(row.startFrame,decoded.firstFrame),to=Math.min(row.endFrame,decoded.endFrame);
    requireValue(from===cursor&&to>from,`${label}: PCM block does not extend the exact interval`);
    for(let frame=from;frame<to;frame++){const source=frame-decoded.firstFrame+block.validOffset;left[frame-row.startFrame]=l[source];right[frame-row.startFrame]=r[source];}
    cursor=to;previous={endFrame:decoded.endFrame,sequence:block.processSequence};
  }
  requireValue(cursor===row.endFrame,`${label}: PCM interval ends early`);
  return {left,right,firstBlockFrame:blocks[0].frame};
}

/** Strict diagnostic comparison; a main-thread clock bracket does not establish a rendered apply frame. */
export function assertPresentationGainPcm({source,output,startFrame,sampleRate,baseline,calls,firstPullFrame}){
  requireValue(source.length===output.length&&source.length>0,'Gain comparison has no exact sample interval');
  requireValue(finite(baseline)&&baseline>=0,'Missing actual gain baseline');
  const transitions=calls.map(call=>{
    timestamp(call.time,'gain request');timestamp(call.beforeTime,'gain before');timestamp(call.afterTime,'gain after');
    requireValue(call.beforeTime<=call.afterTime&&finite(call.value)&&call.value>=0&&
      typeof call.hadActiveSource==='boolean'&&typeof call.controlStoppedActive==='boolean','Invalid actual gain call');
    const lower=firstSampleAtOrAfter(Math.max(call.time,call.beforeTime),sampleRate);
    const upper=firstSampleAtOrAfter(Math.max(call.time,call.afterTime),sampleRate);
    // A silent voice may be first pulled after the main-thread clock bracket.
    // A previously active voice may not move automation to an arbitrary new onset.
    const clamp=!call.hadActiveSource&&!call.controlStoppedActive&&firstPullFrame>=lower?firstPullFrame:null;
    return {lower,upper,clamp,value:Math.fround(call.value),last:Math.max(upper,clamp??upper)};
  });
  const allowed=(transition,frame)=>(frame>=transition.lower&&frame<=transition.upper)||frame===transition.clamp;
    // AudioParam stores its numeric input as float32 before rendering.
  const values=[baseline,...transitions.map(transition=>transition.value)];
  const prefixBefore=(count,frame)=>{let at=-1;
    for(let i=0;i<count;i++){const transition=transitions[i];let next=Math.max(at,transition.lower);
      if(next>transition.upper){if(transition.clamp!==null&&transition.clamp>=at)next=transition.clamp;else return false;}
      if(next>=frame)return false;at=next;}return true;};
  let states=new Set();
  for(let count=0;count<=transitions.length;count++)if(prefixBefore(count,startFrame)&&
    (count===transitions.length||transitions[count].last>=startFrame))states.add(count);
  for(let sample=0;sample<source.length;sample++){
    const frame=startFrame+sample,next=new Set();
    for(const count of states){
      for(let applied=count;applied<=transitions.length;applied++){
        if(applied>count&&!allowed(transitions[applied-1],frame))break;
        if(applied<transitions.length&&transitions[applied].last<=frame)continue;
        const expected=Math.fround(source[sample]*values[applied]);
        if(expected===output[sample])next.add(applied);
      }
    }
    requireValue(next.size>0,`Actual output PCM does not match the ordered production gain timeline at frame ${frame}`);states=next;
  }
  return {samples:source.length,indistinguishableFinalStates:states.size};
}

function continuationPcmProof(raw,start,sourceRow,outputRow,plan){
  const tail=only(raw.silentContinuations??[],row=>row.eventSerial===start.eventSerial,'Actual output PCM is silent in its prefix; bounded continuation');
  const from=outputRow.endFrame,to=from+PRESENTATION_AUDIO_LIMITS.silentTailFrames;
  requireValue(tail.startFrame===from&&tail.endFrame===to&&tail.sourceTap===start.sourceTap&&tail.outputTap===start.outputTap&&
    tail.sourceId===start.sourceId&&tail.slotGeneration===start.sourceSlotGeneration&&tail.sourceArmed===true&&tail.outputArmed===true,
  `Event ${start.eventSerial}: continuation does not retain its bounded source generation`);
  for(const [kind,tapId] of [['source',start.sourceTap],['output',start.outputTap]]){
    const row=tail[kind];
    requireValue(row?.kind==='pcm'&&row.purpose==='silent-tail'&&row.mode==='range'&&row.onsetFrame===null&&
      row.eventSerial===start.eventSerial&&row.voiceKey===start.voiceKey&&row.tapId===tapId&&row.sourceId===start.sourceId&&
      row.slotGeneration===start.sourceSlotGeneration&&row.startFrame===from&&row.endFrame===to,
    `Event ${start.eventSerial}: ${kind} continuation belongs to another interval or source`);
  }
  const source=readPresentationPcmWindow(tail.source,plan,'Continuation source PCM'),output=readPresentationPcmWindow(tail.output,plan,'Continuation output PCM');
  requireValue(source.left.some((v,i)=>v!==0||source.right[i]!==0),'Continuation source PCM is silent');
  const onset=output.left.findIndex((v,i)=>v!==0||output.right[i]!==0);
  requireValue(onset>=0,`Event ${start.eventSerial}: Actual output PCM is silent across the bounded continuation`);
  requireValue(source.left[onset]!==0||source.right[onset]!==0,`Event ${start.eventSerial}: output onset has no owned source PCM`);
  const ownEnds=(raw.voiceStops??[]).filter(row=>row.sourceId===start.sourceId).flatMap(row=>
    row.kind==='source-node-stop'?[row.stopUpperFrame]:row.kind==='source-node-ended'?[row.endedFrame]:[]).filter(integer);
  requireValue(!ownEnds.length||from+onset<Math.min(...ownEnds),`Event ${start.eventSerial}: continuation output starts after its source ended`);
  return{eventSerial:start.eventSerial,sourceId:start.sourceId,slotGeneration:start.sourceSlotGeneration,
    startFrame:sourceRow.onsetFrame,endFrame:to,onsetFrame:from+onset,outputDelayFrames:from+onset-sourceRow.onsetFrame,
    source,output};
}

function createGainTimelineDiagnostic(){
  return {windows:[],matchedChannels:0,differingChannels:0,context:[]};
}

function observeGainTimeline({diagnostic,source,output,startFrame,sampleRate,baseline,calls,firstPullFrame,event,side,outputRow}){
  try{
    assertPresentationGainPcm({source,output,startFrame,sampleRate,baseline,calls,firstPullFrame});
    diagnostic.matchedChannels++;
  }catch(error){
    const reason=String(error?.message??error),frameMatch=/Actual output PCM does not match the ordered production gain timeline at frame (\d+)/u.exec(reason);
    if(!frameMatch)throw error;
    diagnostic.differingChannels++;
    diagnostic.context.push({eventSerial:event.serial,logicalIndex:event.logicalIndex,side,firstDivergentFrame:Number(frameMatch[1]),
      window:{startFrame:outputRow.startFrame,endFrame:outputRow.endFrame},reason});
  }
}

function finishGainTimelineDiagnostic(diagnostic){
  const channelComparisons=diagnostic.matchedChannels+diagnostic.differingChannels;
  if(channelComparisons===0)return {schema:'eagler-presentation/gain-timeline-diagnostic/1',status:'not-assessed',normative:false,
    method:'assertPresentationGainPcm',source:'AudioParam gain calls and their main-thread currentTime brackets',
    reason:'No structurally valid event window reached the diagnostic comparison',counts:{eventWindows:0,channelComparisons:0,
      matchedChannels:0,differingChannels:0,observedPcmFrames:0},extent:null,context:[]};
  const startFrame=Math.min(...diagnostic.windows.map(row=>row.startFrame)),endFrame=Math.max(...diagnostic.windows.map(row=>row.endFrame)),
    observedPcmFrames=diagnostic.windows.reduce((sum,row)=>sum+row.frames,0),differs=diagnostic.differingChannels>0;
  return {schema:'eagler-presentation/gain-timeline-diagnostic/1',status:differs?'differs':'matched',normative:false,
    method:'assertPresentationGainPcm',source:'AudioParam gain calls and their main-thread currentTime brackets',
    reason:differs?'The diagnostic gain timeline differs from sampled output; this is not an AUDIO-06 decision because the main-thread currentTime bracket does not establish the first rendered sample affected.':
      'The sampled output matches this diagnostic gain timeline; the result does not independently establish the rendered apply sample.',
    counts:{eventWindows:diagnostic.windows.length,channelComparisons,matchedChannels:diagnostic.matchedChannels,
      differingChannels:diagnostic.differingChannels,observedPcmFrames},
    extent:{firstFrame:startFrame,endFrame,observedPcmFrames,eventWindows:diagnostic.windows.map(({eventSerial,logicalIndex,startFrame,endFrame})=>
      ({eventSerial,logicalIndex,startFrame,endFrame}))},context:diagnostic.context};
}

function logicalAudio(rows){return {events:rows.events.map(row=>row.logicalIndex),
  routes:rows.routes.map(row=>({logicalIndex:row.logicalIndex,direct:row.direct,fallback:row.fallback,
    fallbackStarted:row.fallbackStarted,pending:row.pending})),uploads:rows.uploads.length,errors:rows.errors.length};}

function audioCaptureLedgerSummary(raw){
  const starts=Array.isArray(raw?.starts)?raw.starts:[],stops=Array.isArray(raw?.voiceStops)?raw.voiceStops:[],
    mutations=Array.isArray(raw?.gainMutations)?raw.gainMutations:[],continuations=Array.isArray(raw?.silentContinuations)?raw.silentContinuations:[];
  const counts=rows=>Object.fromEntries([...new Set(rows.map(row=>String(row)))].sort().map(key=>[key,rows.filter(row=>String(row)===key).length]));
  const pcmNonzero=row=>!!row?.blocks?.some(block=>[...(block.leftWords??[]),...(block.rightWords??[])].some(word=>word!==0));
  return {sourceSlotStarts:starts.length,sourceSlotGenerations:starts.filter(row=>integer(row.sourceSlotGeneration)).length,
    reusedSourceSlots:starts.filter(row=>integer(row.sourceSlotGeneration)&&row.sourceSlotGeneration>1).length,
    sourceSlotCapacityGaps:Array.isArray(raw?.gaps)?raw.gaps.filter(row=>row.reason==='source-slot-inflight-reuse').length:0,
    gainMutations:mutations.length,gainMutationMethods:counts(mutations.map(row=>row.method)),voiceStopRecords:stops.length,
    voiceStopKinds:counts(stops.map(row=>row.kind)),silentContinuations:continuations.length,
    silentContinuationReceipts:continuations.map(row=>({eventSerial:row.eventSerial,source:!!row.source,output:!!row.output,
      sourceNonzero:pcmNonzero(row.source),outputNonzero:pcmNonzero(row.output)}))};
}

function assessOutputCore(output,{epoch,rateHz,logicalIntervals,logicalTickIntervals,startNativeSequence,expectedEvents}={},gainTimeline){
  requireValue(output?.schema===PRESENTATION_AUDIO_SCHEMA,'Actual source and output PCM evidence is missing or uses an unsupported schema');
  const raw=output.raw,contract=TH08_PRESENTATION_AUDIO_CONTRACT;
  requireValue(raw&&/^[a-f0-9]{64}$/.test(output.rawSha256??'')&&sha(raw)===output.rawSha256,'Actual audio raw hash differs');
  requireValue(raw.contract===contract.id&&raw.epoch===epoch&&raw.rateHz===rateHz,'PCM evidence belongs to another title, Runtime epoch or cadence');
  const plan=raw.clock;
  same(plan,presentationAudioFrameClockPlan({sampleRate:plan?.sampleRate,anchorTime:plan?.anchorTime}),'Independent segmented frame-clock plan');
  const window=raw.window,rate=plan.sampleRate;
  requireValue(window?.sampleRate===rate&&window.historySeconds===PRESENTATION_AUDIO_LIMITS.historySeconds&&
    window.prefixFrames===PRESENTATION_AUDIO_LIMITS.prefixFrames,'PCM capture contract differs');
  requireValue(integer(window.startNativeSequence)&&window.startNativeSequence>=0&&integer(window.endNativeSequence)&&
    window.endNativeSequence>=window.startNativeSequence,'Native audio serial window missing');
  if(startNativeSequence!==undefined)requireValue(window.startNativeSequence===startNativeSequence,'PCM window does not start at the actual paired checkpoint');
  requireValue(finite(window.startedAtTime)&&finite(window.finishedAtTime)&&window.startedAtTime>=plan.anchorTime&&
    window.finishedAtTime>=window.startedAtTime&&window.finishedAtTime<plan.endTime,'Actual capture exceeded the finite exact frame clock');
  requireValue(raw.dropped===0,'Audio observer dropped raw records');
  requireValue(array(raw.errors,'audio errors').length===0,'Actual audio observer reported errors');
  requireValue(array(raw.gaps,'audio gaps').length===0,'Actual audio PCM observation has gaps');
  requireValue(array(raw.unassociatedStarts,'unassociated starts').length===0,'Actual WebSfx start lacks a native played event');
  requireValue(array(raw.hookUploads,'window uploads').length===0,'TH08 replaced an eager voice in the captured window');
  const native=raw.native;requireValue(native&&native.sequence===window.endNativeSequence&&native.recordLimit===PRESENTATION_AUDIO_LIMITS.records,'Native trace serial or capacity changed');
  zeroDrops(native.dropped,'Native SFX');requireValue(array(native.errors,'native errors').length===0,'Native observation callback failed');
  const events=array(native.events,'native events'),routes=array(native.routes,'native routes'),uploads=array(native.uploads,'native uploads');
  requireValue(events.length>0&&events.length<=PRESENTATION_AUDIO_LIMITS.records,'Captured Replay has no bounded native played-event window');
  if(expectedEvents!==undefined)requireValue(events.length===expectedEvents,'PCM event count differs from paired logical ticks');
  same(raw.hookEvents.map(stripRoute),events.map(stripRoute),'Native played-event callback');same(raw.hookRoutes,routes,'Native actual-route callback');
  requireValue(raw.starts.length===events.length&&raw.sourcePcm.length===events.length&&raw.pcm.length===events.length&&routes.length===events.length,
    'Each native played event requires exactly one route, actual source start, source PCM and output PCM');
  const voiceStops=array(raw.voiceStops,'actual source lifecycle ledger'),startById=new Map(raw.starts.map(start=>[start.id,start]));
  requireValue(startById.size===raw.starts.length&&voiceStops.every((row,index)=>row.sequence===index+1),
    'Source lifecycle sequence is duplicated or incomplete');
  const stopBySource=new Map(),endedBySource=new Map(),poolStopsWithNode=[];
  for(const row of voiceStops){
    requireValue(finite(row.beforeTime)&&finite(row.afterTime)&&row.afterTime>=row.beforeTime,'Invalid actual source lifecycle time');
    if(row.kind==='source-node-stop'||row.kind==='source-node-ended'){
      const start=startById.get(row.sourceId);
      requireValue(start&&row.nodePresent===true&&row.eventSerial===start.eventSerial&&row.voiceKey===start.voiceKey&&
        row.sourceSlot===start.sourceSlot&&row.slotGeneration===start.sourceSlotGeneration,
      'Source stop/end receipt is detached from its leased node generation');
      if(row.kind==='source-node-stop'){
        const rows=stopBySource.get(row.sourceId)??[];rows.push(row);stopBySource.set(row.sourceId,rows);
        requireValue(integer(row.requestedFrame)&&row.requestedFrame>=0&&
          (row.requestedTime===null||finite(row.requestedTime))&&
          row.requestedFrame===Math.ceil(Math.max(row.requestedTime??0,row.beforeTime)*rate)&&
          integer(row.stopUpperFrame)&&row.stopUpperFrame>=row.requestedFrame&&
          row.stopUpperFrame===Math.ceil(Math.max(row.requestedTime??0,row.afterTime)*rate),
        'Source stop request lacks its actual scheduled boundary or has a mismatched sample frame');
      } else {
        requireValue(!endedBySource.has(row.sourceId),'Source node has duplicate ended receipts');endedBySource.set(row.sourceId,row);
        requireValue(finite(row.endedAt)&&integer(row.endedFrame)&&row.endedFrame>=Math.ceil(row.afterTime*rate)&&
          row.endedAt>=start.afterTime&&row.endedAt<=row.beforeTime&&row.beforeTime<=row.afterTime,
          'Source ended receipt lacks its actual render boundary');
      }
    } else if(row.kind==='voice-pool-stop'){
      if(row.nodePresent===true){
        const start=startById.get(row.sourceId);
        requireValue(start&&row.eventSerial===start.eventSerial&&row.voiceKey===start.voiceKey&&row.sourceSlot===start.sourceSlot&&
          row.slotGeneration===start.sourceSlotGeneration,'Production voice replacement is detached from its source generation');
        poolStopsWithNode.push(row);
      } else requireValue(row.nodePresent===false&&row.sourceId===null&&row.eventSerial===null&&row.sourceSlot===null&&
        row.slotGeneration===null&&uploads.some(upload=>upload.voiceKey===row.voiceKey),
      'Idle production voice stop carries source identity or an unknown voice');
    } else requireValue(false,'Unknown source lifecycle record');
  }
  requireValue(poolStopsWithNode.every(row=>stopBySource.has(row.sourceId)),'Production voice replacement is missing its matching source stop receipt');
  requireValue(raw.bindings.length===contract.logicalSounds&&uploads.length===contract.logicalSounds,'Complete actual eager upload mapping is missing');
  requireValue(raw.taps.length===contract.logicalSounds*(1+PRESENTATION_AUDIO_LIMITS.sourceSlotsPerVoice),'Actual source/output tap inventory is incomplete');
  const tapById=new Map();
  for(const tap of raw.taps){requireValue(integer(tap.id)&&tap.id>0&&!tapById.has(tap.id),'Duplicate actual tap identity');tapById.set(tap.id,tap);
    requireValue(tap.configured&&finite(tap.configured.startedAt)&&tap.configured.startedAt<=tap.configured.finishedAt&&
      tap.configured.finishedAt<plan.anchorTime,'Tap automation was not fully configured before the common anchor');
    same(tap.configured.automationRate,{localFrame:'a-rate',segment:'a-rate'},'Actual clock automation rates');
    same(tap.configuredAck,{kind:'configured',clockSchema:plan.schema,anchorFrame:plan.anchorFrame},'Actual clock configuration acknowledgement');
    requireValue(tap.ready?.kind==='ready'&&tap.ready.frame===tap.ready.block?.frame&&tap.ready.processSequence===tap.ready.block?.processSequence,'Tap has no actual readiness quantum');
    decodePresentationAudioClockBlock(tap.ready.block,plan);
    requireValue(tap.final?.kind==='flushed'&&tap.final.tag===epoch&&tap.final.failures===0&&tap.final.pending===0&&
      integer(tap.final.processSequence)&&tap.final.processSequence>=tap.ready.processSequence&&integer(tap.final.previousEnd)&&
      tap.final.previousEnd>=tap.ready.frame,'Tap final callback/error ledger is missing or incomplete');
  }
  const bindingByKey=new Map();
  for(let index=0;index<contract.logicalSounds;index++){
    const binding=only(raw.bindings,row=>row.logicalIndex===index,'Logical sound binding');
    requireValue(binding.handle===index+contract.handleOffset&&!bindingByKey.has(binding.voiceKey),'Invalid or reused native voice key');bindingByKey.set(binding.voiceKey,binding);
    same(binding.upload,only(uploads,row=>row.handle===binding.handle,'Native upload'),'Actual uploaded voice');
    requireValue(binding.upload.uploadSuccess===true&&binding.upload.voiceKey===binding.voiceKey&&integer(binding.upload.sourceRate)&&binding.upload.sourceRate>0&&
      integer(binding.upload.sourceFrames)&&binding.upload.sourceFrames>0,'Invalid actual uploaded PCM');
    same(binding.graph,{sourceBuffer:'AudioBufferSourceNode',sourceRoute:'voice.split',leftGain:'voice.left.gain',rightGain:'voice.right.gain',
      outputRoute:'voice.merge',outputTapInput:'voice.merge',outputTapConnected:true},'Production source/gain/output tap graph');
    requireValue(binding.baseline?.nodePresent===false&&binding.baseline.playing===false&&binding.baseline.speed===1&&
      finite(binding.baseline.leftGain)&&finite(binding.baseline.rightGain)&&binding.baseline.contextTime>=plan.anchorTime&&binding.baseline.contextTime<=window.startedAtTime,
      'Voice baseline is not the observed idle production boundary');
    const outputTap=tapById.get(binding.outputTap);requireValue(outputTap?.kind==='output'&&outputTap.voiceKey===binding.voiceKey,'Voice output tap does not match its native key');
    requireValue(binding.sourceTaps?.length===PRESENTATION_AUDIO_LIMITS.sourceSlotsPerVoice&&new Set(binding.sourceTaps).size===binding.sourceTaps.length,'Source tap slots missing');
    binding.sourceTaps.forEach((id,slot)=>{const tap=tapById.get(id);requireValue(tap?.kind==='source'&&tap.slot===slot&&tap.voiceKey===binding.voiceKey,'Source tap slot identity differs');});
  }
  const intervals=array(raw.intervals,'Actual logical audio intervals');
  if(logicalIntervals){requireValue(intervals.length===logicalIntervals.length,'Observed audio interval count differs from logical tick and draw boundaries');
    let cursor=window.startNativeSequence;
    intervals.forEach((interval,index)=>{requireValue(interval.available===true&&interval.enabled===true&&integer(interval.sequence)&&interval.sequence>=cursor,'Invalid native interval cursor');
      zeroDrops(interval.dropped,'Audio interval');
      for(const kind of ['events','routes','uploads','errors']){array(interval[kind],`interval ${kind}`);
        const expected=native[kind].filter(row=>row.serial>cursor&&row.serial<=interval.sequence);
        same(interval[kind],expected,`Actual interval ${index} ${kind}`);}
      same(logicalAudio(interval),logicalIntervals[index],`Logical interval ${index}`);cursor=interval.sequence;});
    requireValue(cursor===window.endNativeSequence,'Actual audio trace extends past retained logical boundaries');
  }
  if(logicalTickIntervals){
    for(const tick of logicalTickIntervals){
      requireValue(Array.isArray(tick),'Missing logical-tick audio intervals');
      const tickEvents=tick.flatMap(interval=>array(interval?.events,'Logical-tick native events'));
      const indices=tickEvents;
      requireValue(indices.every(index=>integer(index)&&index>=0&&index<contract.logicalSounds),
        'Logical-tick audio contains an invalid normalized SFX index');
      requireValue(new Set(indices).size===indices.length,'A logical tick played the same SFX more than once; its shared voice merge cannot be separated');
    }
  }
  let previousGainSequence=0;
  for(const call of array(raw.gainCalls,'Actual gain calls')){
    requireValue(call.sequence===++previousGainSequence&&bindingByKey.has(call.voiceKey)&&['left','right'].includes(call.side),'Missing or reordered actual gain call');
    requireValue(finite(call.volume)&&call.volume>=0&&finite(call.pan)&&call.pan>=-1&&call.pan<=1,'Invalid actual WebSfx controls');
    const expected=call.volume*(call.side==='left'?(call.pan>0?1-call.pan:1):(call.pan<0?1+call.pan:1));
    requireValue(call.value===expected,'Actual AudioParam request does not match production WebSfx gain controls');
  }
  const gainMutations=array(raw.gainMutations,'complete AudioParam mutation ledger');
  requireValue(gainMutations.length===raw.gainCalls.length&&gainMutations.every((mutation,index)=>
    mutation.method==='setValueAtTime'&&mutation.sequence===raw.gainCalls[index].sequence&&
    mutation.voiceKey===raw.gainCalls[index].voiceKey&&mutation.side===raw.gainCalls[index].side&&
    Array.isArray(mutation.args)&&mutation.args.length===2&&mutation.args[0]===raw.gainCalls[index].value&&
    mutation.args[1]===raw.gainCalls[index].time&&finite(mutation.beforeTime)&&finite(mutation.afterTime)&&
    mutation.afterTime>=mutation.beforeTime&&mutation.beforeTime===raw.gainCalls[index].beforeTime&&
    mutation.afterTime===raw.gainCalls[index].afterTime),
  'A production gain was changed by an unmodeled AudioParam method or direct value assignment');
  const priorStartByKey=new Map(),continuationProofs=[];let previousEvent=window.startNativeSequence,observedPcmFrames=0;
  requireValue(array(raw.markReceipts,'Actual mark acknowledgements').length===events.length*2,'Source/output markers did not all reach their actual Worklets');
  const latestGenerationByTap=new Map(),receiptSequenceSet=new Set();
  for(let index=0;index<events.length;index++){
    const event=events[index],binding=bindingByKey.get(event.voiceKey);
    requireValue(integer(event.serial)&&event.serial>previousEvent&&event.serial<=window.endNativeSequence,'Native played events are missing or reordered');previousEvent=event.serial;
    requireValue(binding&&event.logicalIndex===binding.logicalIndex&&event.handle===binding.handle,'Native played event does not match the true uploaded sound');
    nativeClock(event,rate,'Native event');
    const route=only(routes,row=>row.eventSerial===event.serial,'Native played-event route');
    requireValue(route.serial>event.serial&&route.serial===event.routeSerial&&route.handle===event.handle&&route.voiceKey===event.voiceKey&&route.logicalIndex===event.logicalIndex,
      'Actual native route is not bound to its played event');nativeClock(route,rate,'Native route');
    requireValue(route.direct===true&&route.fallback===false&&route.fallbackStarted===false&&route.pending===false,'SFX did not use the observed direct production voice');
    const start=only(raw.starts,row=>row.eventSerial===event.serial,'Actual source.start');
    requireValue(start.id===index+1&&start.voiceKey===event.voiceKey&&start.logicalIndex===event.logicalIndex&&start.handle===event.handle&&
      start.sourceId===start.id&&binding.sourceTaps.includes(start.sourceTap)&&start.outputTap===binding.outputTap&&
      integer(start.sourceSlot)&&start.sourceSlot>=0&&start.sourceSlot<PRESENTATION_AUDIO_LIMITS.sourceSlotsPerVoice&&
      integer(start.sourceSlotGeneration)&&start.sourceSlotGeneration>0,'Actual source identity does not match the native event');
    const sourceTap=tapById.get(start.sourceTap);
    requireValue(sourceTap?.slot===start.sourceSlot&&sourceTap.voiceKey===start.voiceKey,'Source slot tap identity differs from its bound voice');
    const priorGeneration=latestGenerationByTap.get(start.sourceTap)??0;
    requireValue(start.sourceSlotGeneration===priorGeneration+1,'Source tap generation is missing, duplicated or reordered');
    if(priorGeneration===0){
      requireValue(start.previousSlotReceiptSequence===0&&start.previousSlotEnded===null,'Initial source slot generation claims stale prior activity');
    } else {
      const priorStart=only(raw.starts,row=>row.sourceTap===start.sourceTap&&row.sourceSlotGeneration===priorGeneration,'Prior source-slot generation');
      const priorSource=only(raw.sourcePcm,row=>row.eventSerial===priorStart.eventSerial,'Prior source-slot PCM receipt');
      const priorEnded=only(raw.voiceStops,row=>row.kind==='source-node-ended'&&row.sourceId===priorStart.sourceId&&
        row.slotGeneration===priorGeneration,'Prior source-slot ended boundary');
      same(start.previousSlotEnded,priorEnded,'Source slot reuse ended boundary');
      requireValue(priorSource.slotReceiptSequence===start.previousSlotReceiptSequence&&integer(start.previousSlotReceiptSequence)&&
        start.previousSlotReceiptSequence>0&&finite(priorEnded.endedAt)&&priorEnded.endedAt<=start.beforeTime&&
        priorEnded.endedFrame<=Math.ceil(start.beforeTime*rate),
      'Source slot was reused before its previous raw PCM receipt and actual node end');
    }
    latestGenerationByTap.set(start.sourceTap,start.sourceSlotGeneration);
    requireValue(start.voiceSplitConnectionCount===1,'The sampled source did not connect once to its bound production voice splitter');
    if(start.priorControlStop){
      const stop=only(raw.voiceStops,row=>row.kind==='voice-pool-stop'&&row.sequence===start.priorControlStop.sequence,'Prior production voice stop');
      same(stop,start.priorControlStop,'Source reuse stop boundary');
      requireValue(stop.voiceKey===start.voiceKey&&finite(stop.beforeTime)&&finite(stop.afterTime)&&
        stop.afterTime>=stop.beforeTime&&stop.afterTime<=start.beforeTime,'Production voice stop is not bounded before the new start');
      if(stop.nodePresent===false){
        requireValue(stop.sourceId===null&&stop.eventSerial===null&&stop.sourceSlot===null&&stop.slotGeneration===null,
          'Idle production voice stop is attached to a source generation');
      } else {
        requireValue(stop.nodePresent===true&&stop.eventSerial!==start.eventSerial,
          'Production voice replacement does not retain a distinct prior source');
        const replaced=only(raw.starts,row=>row.id===stop.sourceId,'Replaced production source');
        requireValue(replaced.eventSerial===stop.eventSerial&&replaced.sourceSlot===stop.sourceSlot&&
          replaced.sourceSlotGeneration===stop.slotGeneration&&stopBySource.has(stop.sourceId),
        'Production stop does not retain its replaced source generation and node stop request');
      }
    }
    requireValue(start.bufferSame===true&&start.sourceRate===binding.upload.sourceRate&&start.sourceFrames===binding.upload.sourceFrames&&
      start.channels===2&&start.playbackRate===1&&start.loop===false&&start.when===0&&start.offset===0&&start.duration===null,
      'Actual source.start did not use the native uploaded SFX buffer and production playback parameters');
    requireValue(finite(start.beforeTime)&&start.beforeTime>=event.queueClockAfterTime&&start.afterTime>=start.beforeTime&&
      start.afterTime<=route.queueClockBeforeTime,'Actual source.start is not bracketed by its native event and route');
    requireValue(integer(start.gainSequence)&&start.gainSequence>=0&&start.gainSequence<=raw.gainCalls.length,'Actual source gain cursor is invalid');
    const sourceRow=only(raw.sourcePcm,row=>row.eventSerial===event.serial,'Actual source PCM'),outputRow=only(raw.pcm,row=>row.eventSerial===event.serial,'Actual voice output PCM');
    const sourceArmed=only(raw.markReceipts,row=>row.eventSerial===event.serial&&row.tapId===start.sourceTap,'Actual source PCM marker acknowledgement');
    const outputArmed=only(raw.markReceipts,row=>row.eventSerial===event.serial&&row.tapId===start.outputTap,'Actual output PCM marker acknowledgement');
    requireValue(sourceArmed.kind==='armed'&&sourceArmed.purpose==='event-audio'&&sourceArmed.mode==='onset'&&
      sourceArmed.sourceId===start.sourceId&&sourceArmed.slotGeneration===start.sourceSlotGeneration&&integer(sourceArmed.processSequence)&&
      outputArmed.kind==='armed'&&outputArmed.purpose==='event-audio'&&outputArmed.mode==='range'&&outputArmed.sourceId===start.sourceId&&
      outputArmed.slotGeneration===start.sourceSlotGeneration&&integer(outputArmed.processSequence),
    'Source/output marker acknowledgements do not share the leased generation');
    requireValue(sourceRow.kind==='pcm'&&sourceRow.purpose==='event-audio'&&sourceRow.mode==='onset'&&sourceRow.tapId===start.sourceTap&&
      sourceRow.sourceId===start.sourceId&&sourceRow.slotGeneration===start.sourceSlotGeneration&&sourceRow.slot===start.sourceSlot&&
      integer(sourceRow.slotReceiptSequence)&&sourceRow.slotReceiptSequence>0&&!receiptSequenceSet.has(sourceRow.slotReceiptSequence)&&sourceRow.voiceKey===event.voiceKey&&
      sourceRow.startFrame===Math.ceil(start.beforeTime*rate)&&integer(sourceRow.onsetFrame)&&sourceRow.onsetFrame>=sourceRow.startFrame&&
      sourceRow.endFrame===sourceRow.onsetFrame+PRESENTATION_AUDIO_LIMITS.prefixFrames,'Actual source onset evidence differs from source.start');
    receiptSequenceSet.add(sourceRow.slotReceiptSequence);
    const source=readPresentationPcmWindow(sourceRow,plan,'Source PCM');
    const firstNonzero=source.left.findIndex((value,i)=>value!==0||source.right[i]!==0);
    requireValue(firstNonzero>=0&&sourceRow.startFrame+firstNonzero===sourceRow.onsetFrame,'Actual source onset summary skips or invents PCM');
    requireValue(outputRow.kind==='pcm'&&outputRow.purpose==='event-audio'&&outputRow.mode==='range'&&outputRow.tapId===start.outputTap&&
      outputRow.sourceId===start.sourceId&&outputRow.slotGeneration===start.sourceSlotGeneration&&outputRow.voiceKey===event.voiceKey&&
      outputRow.startFrame===sourceRow.onsetFrame&&outputRow.endFrame===sourceRow.endFrame&&outputRow.onsetFrame===null,'Output PCM does not cover the same absolute source sample interval');
    const outputPcm=readPresentationPcmWindow(outputRow,plan,'Output PCM');
    const audible=outputPcm.left.some((value,index)=>value!==0||outputPcm.right[index]!==0);
    requireValue(!audible||!(raw.silentContinuations??[]).some(row=>row.eventSerial===start.eventSerial),
      `Event ${start.eventSerial}: an audible prefix has an unrequested continuation`);
    let continuation=null,continuationError=null;
    if(!audible)try{continuation=continuationPcmProof(raw,start,sourceRow,outputRow,plan);}catch(error){continuationError=error;}
    const proofEnd=continuation?.endFrame??outputRow.endFrame;
    gainTimeline.windows.push({eventSerial:event.serial,logicalIndex:event.logicalIndex,startFrame:outputRow.startFrame,
      endFrame:outputRow.endFrame,frames:outputPcm.left.length});
    for(const other of raw.starts){
      if(other.id===start.id||other.voiceKey!==start.voiceKey)continue;
      const otherStartFrame=Math.ceil(Math.max(other.when??0,other.beforeTime)*rate);
      requireValue(finite(other.beforeTime)&&integer(otherStartFrame),'Another production source lacks a bounded actual start frame');
      if(otherStartFrame>=proofEnd)continue;
      const endRows=stopBySource.get(other.id)??[],scheduledEnd=endRows.at(-1)?.stopUpperFrame,
        naturalEnd=endedBySource.get(other.id)?.endedFrame,
        endFrame=[scheduledEnd,naturalEnd].filter(integer).sort((left,right)=>left-right)[0]??null;
      requireValue(endFrame!==null&&(endFrame<=otherStartFrame||endFrame<=outputRow.startFrame),
        'Another production source may overlap this shared voice-merge PCM window');
    }
    requireValue(proofEnd<=tapById.get(start.sourceTap).final.previousEnd&&proofEnd<=tapById.get(start.outputTap).final.previousEnd,
      'PCM packet exceeds its actual final worklet checkpoint');
    const prior=priorStartByKey.get(start.voiceKey),floor=prior?.gainSequence??0;
    for(const side of ['left','right']){
      const previousCalls=raw.gainCalls.filter(call=>call.voiceKey===start.voiceKey&&call.side===side&&call.sequence<=floor);
      const baseline=previousCalls.length?previousCalls.at(-1).value:binding.baseline[`${side}Gain`];
      const calls=raw.gainCalls.filter(call=>call.voiceKey===start.voiceKey&&call.side===side&&call.sequence>floor&&call.sequence<=start.gainSequence);
      requireValue(calls.length===contract.gainCallsPerChannel,
        'TH08 played event does not retain the four production AudioHost/WebSfx gain calls per channel');
      observeGainTimeline({diagnostic:gainTimeline,source:source[side].subarray(firstNonzero),output:outputPcm[side],startFrame:sourceRow.onsetFrame,
        sampleRate:rate,baseline,calls,firstPullFrame:outputPcm.firstBlockFrame,event,side,outputRow});
    }
    if(continuationError)throw continuationError;
    if(continuation){const {source,output,...receipt}=continuation;continuationProofs.push(receipt);observedPcmFrames+=output.left.length;}
    priorStartByKey.set(start.voiceKey,start);observedPcmFrames+=outputPcm.left.length;
  }
  requireValue(receiptSequenceSet.size===events.length&&Array.from({length:events.length},(_,index)=>index+1).every(sequence=>receiptSequenceSet.has(sequence)),
    'Source-slot receipt sequence is incomplete');
  return {status:'passed',epoch,rateHz,nativeEvents:events.length,actualStarts:raw.starts.length,actualSourceWindows:raw.sourcePcm.length,
    actualOutputWindows:raw.pcm.length,observedPcmFrames,gainTimelineDiagnostic:finishGainTimelineDiagnostic(gainTimeline),rawSha256:output.rawSha256,
    ...(continuationProofs.length?{continuationProofs}:{}),
    captureLedger:audioCaptureLedgerSummary(raw),clockSchema:plan.schema,clockEndTime:plan.endTime,clockHorizonSeconds:plan.segments*plan.segmentSeconds};
}

export function assessPresentationAudioOutput(output,expected){
  const gainTimeline=createGainTimelineDiagnostic();
  try{return assessOutputCore(output,expected,gainTimeline);}catch(error){return {status:output?'failed':'incomplete',epoch:expected?.epoch??null,
    rateHz:expected?.rateHz??null,reason:String(error?.message??error).slice(0,1000),rawSha256:output?.rawSha256??null,
    gainTimelineDiagnostic:finishGainTimelineDiagnostic(gainTimeline),captureLedger:audioCaptureLedgerSummary(output?.raw)};}
}

export function assessPresentationAudioRun(run,{requiredTicks=600}={}){
  if(!run||run.capture?.requestedTicks!==requiredTicks||run.capture?.completedTicks!==requiredTicks||run.samples?.length!==requiredTicks)
    return {status:'incomplete',reason:`The full declared ${requiredTicks}-tick Runtime window is missing`};
  const logicalTickIntervals=run.samples.map(sample=>[sample.audio,...(sample.drawReceipts??[]).map(receipt=>receipt.audio)]),
    intervals=logicalTickIntervals.flat();
  return assessPresentationAudioOutput(run.audioOutput,{epoch:run.epoch,rateHz:run.scheduler?.rateHz,
    startNativeSequence:run.start?.audio?.sfx?.sequence,expectedEvents:run.audioEvents,logicalIntervals:intervals,logicalTickIntervals});
}

export function assessPresentationAudioEvidence(runs,{requiredTicks=600,ratesHz=[60,120,144],scope={game:'th08',variant:'normal'},
  expectedAuthorityGroups=17,cadenceDifferenceGroups=['host-frame-cadence','synchronous-audio-operations']}={}){
  try{
    requireValue(scope.game==='th08'&&scope.variant==='normal','No trusted PCM contract is registered for the selected product');
    const comparison=compareReplayCadenceRuns({runs,requiredTicks,ratesHz,scope,expectedAuthorityGroups,cadenceDifferenceGroups});
    const results=(Array.isArray(runs)?runs:[]).map(run=>assessPresentationAudioRun(run,{requiredTicks}));
    const logicalPassed=['DISPLAY-01','REPLAY-07'].every(id=>comparison.checks.some(check=>check.id===id&&check.status==='passed'))&&
      comparison.checks.some(check=>check.id==='AUDIO-06'&&check.evidence?.sameLogicalEventRouteSequence==='passed');
    const status=results.some(result=>result.status==='failed')||comparison.status==='failed'?'failed':
      logicalPassed&&results.length===ratesHz.length&&results.every(result=>result.status==='passed')?'passed':'incomplete';
    return {schema:'eagler-presentation/audio-assessment/2',acceptance:'event-source-output-conservation',status,complete:status==='passed',machineComplete:status==='passed',scope,
      requiredTicks,ratesHz:[...ratesHz],runs:results,logicalComparisonStatus:comparison.status,
      reason:status==='passed'?null:'Complete native event/route, source.start, gain mutation ledger and actual source/output PCM are required for every cadence',
      extent:'Native played-event conservation, actual source ownership and attributed nonzero output at each declared cadence; gain timeline diagnostics are retained separately',
      rollbackAudioVerified:false,physicalAudioDeviceVerified:false};
  }catch(error){return {schema:'eagler-presentation/audio-assessment/2',acceptance:'event-source-output-conservation',status:'failed',complete:false,machineComplete:false,
    reason:String(error?.message??error).slice(0,1000),runs:[],scope,requiredTicks,ratesHz:[...ratesHz]};}
}

export function applyPresentationAudioEvidence(comparison,runs,options={}){
  const audio=assessPresentationAudioEvidence(runs,options);
  const checks=(comparison?.checks??[]).map(check=>check.id==='AUDIO-06'?{...check,status:audio.status,complete:audio.complete,
    machineComplete:audio.machineComplete,reason:audio.reason,evidence:{...check.evidence,actualPcm:audio}}:check);
  return {...comparison,checks,status:checks.some(check=>check.status==='failed')?'failed':checks.every(check=>check.status==='passed')?'passed':'incomplete'};
}
