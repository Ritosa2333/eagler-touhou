import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import vm from 'node:vm';
import {PRESENTATION_AUDIO_SCHEMA,presentationAudioFrameClockPlan,decodePresentationAudioClockBlock,
  presentationAudioPcmProcessorSource,installPresentationAudioProbe,leasePresentationAudioSourceSlot,
  releasePresentationAudioSourceSlot} from './presentation-audio-browser.mjs';
import {assessPresentationAudioOutput,assertPresentationAudioEqual,assertPresentationGainPcm} from './presentation-audio-evidence.mjs';

const clone=value=>JSON.parse(JSON.stringify(value));
const seal=raw=>({schema:PRESENTATION_AUDIO_SCHEMA,raw,rawSha256:createHash('sha256').update(JSON.stringify(raw)).digest('hex')});
const gainMutation=call=>({sequence:call.sequence,method:'setValueAtTime',voiceKey:call.voiceKey,side:call.side,
  args:[call.value,call.time],beforeTime:call.beforeTime,afterTime:call.afterTime});
function clockBlock(plan,frame,size=128,{segmentLength=1,currentFrame=frame,sequence=1}={}){
  const local=[],segment=[];
  for(let i=0;i<size;i++){const f=frame+i-plan.anchorFrame;local.push(plan.localBase+f%plan.segmentFrames);segment.push(Math.floor(f/plan.segmentFrames));}
  return {processSequence:sequence,currentFrame,quantumFrames:size,validOffset:0,frame,frames:size,local,
    segment:segmentLength===1?[segment[0]]:segment};
}
function pcmBlock(plan,frame,onset,{sequence=1}={}){
  const block=clockBlock(plan,frame,128,{sequence});const left=new Float32Array(128),right=new Float32Array(128);
  for(let i=0;i<128;i++)if(frame+i>=onset){left[i]=.25;right[i]=.125;}
  return {...block,inputChannels:2,leftWords:Array.from(new Uint32Array(left.buffer)),rightWords:Array.from(new Uint32Array(right.buffer))};
}
function fixture(){
  const plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame+1024,time=frame/plan.sampleRate;
  const uploads=Array.from({length:46},(_,index)=>({serial:index+1,handle:index+1,voiceKey:String(100+index),sourceRate:44100,sourceFrames:4410,channels:1,uploadSuccess:true,
    contextSampleRate:48000,contextTime:.5,contextFrame:24000}));
  const taps=[],bindings=uploads.map((upload,index)=>{
    const ids=[];for(let n=0;n<3;n++){const id=taps.length+1;ids.push(id);const block=clockBlock(plan,plan.anchorFrame);
      taps.push({id,voiceKey:upload.voiceKey,kind:n?'source':'output',slot:n?n-1:null,
        configured:{startedAt:1,finishedAt:1.1,automationRate:{localFrame:'a-rate',segment:'a-rate'}},
        configuredAck:{kind:'configured',clockSchema:plan.schema,anchorFrame:plan.anchorFrame},
        final:{kind:'flushed',tag:1,processSequence:100,previousEnd:plan.anchorFrame+4096,failures:0,pending:0,currentFrame:plan.anchorFrame+4096},
        ready:{kind:'ready',frame:block.frame,processSequence:block.processSequence,currentFrame:block.currentFrame,block}});}
    return {logicalIndex:index,handle:upload.handle,voiceKey:upload.voiceKey,upload,
      baseline:{contextTime:2.001,leftGain:.5,rightGain:.5,volume:.5,pan:0,speed:1,playing:false,nodePresent:false},outputTap:ids[0],sourceTaps:ids.slice(1)};
  });
  const event={serial:47,logicalIndex:0,handle:1,voiceKey:'100',queueFrames:2048,queueRate:44100,sourceRate:44100,
    contextSampleRate:48000,contextTime:time,contextFrame:time*48000,queueClockBeforeTime:time,queueClockAfterTime:time,
    queueClockBeforeFrame:time*48000,queueClockAfterFrame:time*48000,queueSampleOrder:'cpp-sdl-queue-clock-bracket',routeSerial:48};
  const route={...event,serial:48,eventSerial:47,direct:true,fallback:false,fallbackStarted:false,pending:false};delete route.routeSerial;delete route.sourceRate;
  const hook={...event};delete hook.routeSerial;
  const sourcePcm={kind:'pcm',eventSerial:47,purpose:'event-audio',mode:'onset',tapId:2,voiceKey:'100',sourceId:1,slotGeneration:1,
    slot:0,slotReceiptSequence:1,startFrame:frame,onsetFrame:frame+16,endFrame:frame+144,
    blocks:[pcmBlock(plan,frame,frame+16),pcmBlock(plan,frame+128,frame+16,{sequence:2})]};
  const pcm=clone(sourcePcm);Object.assign(pcm,{mode:'range',tapId:1,startFrame:frame+16,onsetFrame:null});
  for(const block of pcm.blocks)for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);for(let i=0;i<samples.length;i++)samples[i]*=.5;block[side]=Array.from(words);}
  const dropped={events:0,routes:0,uploads:0,errors:0};
  const interval={available:true,enabled:true,sequence:48,dropped:{...dropped},events:[event],routes:[route],uploads:[],errors:[]};
  const raw={contract:'th08/presentation-audio/1',epoch:1,rateHz:60,clock:plan,
    window:{startNativeSequence:46,endNativeSequence:48,startedAtTime:2.01,finishedAtTime:2.1,sampleRate:48000,historySeconds:2,prefixFrames:128},
    bindings,taps,hookEvents:[hook],hookRoutes:[route],hookUploads:[],starts:[{id:1,sourceId:1,eventSerial:47,logicalIndex:0,handle:1,voiceKey:'100',sourceTap:2,
      sourceSlot:0,sourceSlotGeneration:1,previousSlotReceiptSequence:0,previousSlotEnded:null,outputTap:1,
      voiceSplitConnectionCount:1,priorControlStop:null,
      when:0,offset:0,duration:null,beforeTime:time,afterTime:time,gainSequence:8,bufferSame:true,sourceRate:44100,sourceFrames:4410,channels:2,playbackRate:1,loop:false}],
    markReceipts:[{kind:'armed',eventSerial:47,tapId:2,purpose:'event-audio',mode:'onset',sourceId:1,slotGeneration:1,
      startFrame:frame,processSequence:1,currentFrame:plan.anchorFrame},
      {kind:'armed',eventSerial:47,tapId:1,purpose:'event-audio',mode:'range',sourceId:1,slotGeneration:1,
        startFrame:frame+16,endFrame:frame+144,processSequence:1,currentFrame:plan.anchorFrame}],
    gainCalls:['left','right','left','right','left','right','left','right'].map((side,index)=>({sequence:index+1,voiceKey:'100',side,value:.5,time,beforeTime:time,afterTime:time,
      hadActiveSource:false,controlStoppedActive:false,volume:.5,pan:0})),intervals:[interval],sourcePcm:[sourcePcm],pcm:[pcm],
    native:{sequence:48,recordLimit:512,dropped,uploads,events:[event],routes:[route],errors:[]},errors:[],gaps:[],unassociatedStarts:[],dropped:0};
  raw.bindings=raw.bindings.map(binding=>({...binding,graph:{sourceBuffer:'AudioBufferSourceNode',sourceRoute:'voice.split',leftGain:'voice.left.gain',
    rightGain:'voice.right.gain',outputRoute:'voice.merge',outputTapInput:'voice.merge',outputTapConnected:true}}));
  raw.gainMutations=raw.gainCalls.map(gainMutation);raw.voiceStops=[];
  const expected={epoch:1,rateHz:60,startNativeSequence:46,expectedEvents:1,logicalIntervals:[{events:[0],routes:[{logicalIndex:0,direct:true,fallback:false,fallbackStarted:false,pending:false}],uploads:0,errors:0}]};
  return {raw,expected};
}
function reusedSlotFixture(){
  const {raw,expected}=fixture(),plan=raw.clock,nominalFrame=raw.sourcePcm[0].startFrame+256,time=nominalFrame/plan.sampleRate,
    frame=Math.ceil(time*plan.sampleRate),
    firstStart=raw.starts[0],endedAt=time-.001,endedRow={sequence:1,kind:'source-node-ended',voiceKey:'100',nodePresent:true,
      sourceId:firstStart.sourceId,eventSerial:firstStart.eventSerial,sourceSlot:firstStart.sourceSlot,
      slotGeneration:firstStart.sourceSlotGeneration,endedAt,beforeTime:endedAt,afterTime:endedAt,endedFrame:Math.ceil(endedAt*plan.sampleRate)};
  const event={...clone(raw.native.events[0]),serial:49,routeSerial:50,contextTime:time,contextFrame:time*plan.sampleRate,
    queueClockBeforeTime:time,queueClockAfterTime:time,queueClockBeforeFrame:time*plan.sampleRate,queueClockAfterFrame:time*plan.sampleRate};
  const route={...event,serial:50,eventSerial:49,direct:true,fallback:false,fallbackStarted:false,pending:false};delete route.routeSerial;delete route.sourceRate;
  const hook=clone(event);delete hook.routeSerial;
  const sourcePcm={kind:'pcm',eventSerial:49,purpose:'event-audio',mode:'onset',tapId:2,voiceKey:'100',sourceId:2,slotGeneration:2,
    slot:0,slotReceiptSequence:2,startFrame:frame,onsetFrame:frame+16,endFrame:frame+144,
    blocks:[pcmBlock(plan,frame,frame+16,{sequence:3}),pcmBlock(plan,frame+128,frame+16,{sequence:4})]};
  const outputPcm=clone(sourcePcm);Object.assign(outputPcm,{mode:'range',tapId:1,startFrame:frame+16,onsetFrame:null});
  for(const block of outputPcm.blocks)for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);
    for(let index=0;index<samples.length;index++)samples[index]*=.5;block[side]=Array.from(words);}
  raw.starts.push({...firstStart,id:2,sourceId:2,eventSerial:49,sourceSlotGeneration:2,previousSlotReceiptSequence:1,
    previousSlotEnded:endedRow,beforeTime:time,afterTime:time,gainSequence:16});
  raw.sourcePcm.push(sourcePcm);raw.pcm.push(outputPcm);raw.voiceStops.push(endedRow);
  raw.markReceipts.push({kind:'armed',eventSerial:49,tapId:2,purpose:'event-audio',mode:'onset',sourceId:2,slotGeneration:2,
    startFrame:frame,processSequence:3,currentFrame:frame},{kind:'armed',eventSerial:49,tapId:1,purpose:'event-audio',mode:'range',sourceId:2,
    slotGeneration:2,startFrame:frame+16,endFrame:frame+144,processSequence:3,currentFrame:frame});
  raw.gainCalls.push(...raw.gainCalls.slice(0,8).map((call,index)=>({...call,sequence:index+9,time,beforeTime:time,afterTime:time})));
  raw.gainMutations=raw.gainCalls.map(gainMutation);
  raw.native.events.push(event);raw.native.routes.push(route);raw.native.sequence=50;raw.hookEvents.push(hook);raw.hookRoutes.push(route);
  raw.window.endNativeSequence=50;raw.intervals[0].sequence=50;raw.intervals[0].events.push(event);raw.intervals[0].routes.push(route);
  expected.expectedEvents=2;expected.logicalIntervals[0].events.push(0);expected.logicalIntervals[0].routes.push({logicalIndex:0,direct:true,
    fallback:false,fallbackStarted:false,pending:false});
  return {raw,expected};
}
function noOpControlStopFixture(){
  const {raw,expected}=fixture(),start=raw.starts[0],time=start.beforeTime-.001,
    stop={sequence:1,kind:'voice-pool-stop',voiceKey:start.voiceKey,nodePresent:false,sourceId:null,eventSerial:null,
      sourceSlot:null,slotGeneration:null,beforeTime:time,afterTime:time};
  raw.voiceStops=[stop];start.priorControlStop=stop;
  return {raw,expected};
}
function activeThenNoOpControlStopFixture(){
  const {raw,expected}=reusedSlotFixture(),prior=raw.starts[0],next=raw.starts[1],ended=raw.voiceStops[0],rate=raw.clock.sampleRate,
    requestedTime=ended.endedAt-.001,activeCallTime=requestedTime-.001,noOpTime=next.beforeTime-.001;
  const active={sequence:1,kind:'voice-pool-stop',voiceKey:prior.voiceKey,nodePresent:true,sourceId:prior.sourceId,
      eventSerial:prior.eventSerial,sourceSlot:prior.sourceSlot,slotGeneration:prior.sourceSlotGeneration,
      beforeTime:activeCallTime,afterTime:activeCallTime},nodeStop={sequence:2,kind:'source-node-stop',voiceKey:prior.voiceKey,
      nodePresent:true,sourceId:prior.sourceId,eventSerial:prior.eventSerial,sourceSlot:prior.sourceSlot,
      slotGeneration:prior.sourceSlotGeneration,requestedTime,requestedFrame:Math.ceil(Math.max(requestedTime,requestedTime)*rate),
      stopUpperFrame:Math.ceil(Math.max(requestedTime,requestedTime)*rate),beforeTime:requestedTime,afterTime:requestedTime},
    noOp={sequence:4,kind:'voice-pool-stop',voiceKey:next.voiceKey,nodePresent:false,sourceId:null,eventSerial:null,
      sourceSlot:null,slotGeneration:null,beforeTime:noOpTime,afterTime:noOpTime};
  ended.sequence=3;raw.voiceStops=[active,nodeStop,ended,noOp];next.priorControlStop=noOp;
  return {raw,expected};
}
function overlappingVoiceFixture(){
  const {raw,expected}=reusedSlotFixture(),start=raw.starts[1],source=raw.sourcePcm[1],endFrame=raw.pcm[1].endFrame,
    overlapEnd=(endFrame+256)/raw.clock.sampleRate;
  start.sourceTap=3;start.sourceSlot=1;start.sourceSlotGeneration=1;start.previousSlotReceiptSequence=0;start.previousSlotEnded=null;
  source.tapId=3;source.slot=1;
  const sourceAck=raw.markReceipts.find(row=>row.eventSerial===start.eventSerial&&row.mode==='onset');
  sourceAck.tapId=3;sourceAck.slotGeneration=1;
  const outputAck=raw.markReceipts.find(row=>row.eventSerial===start.eventSerial&&row.mode==='range');outputAck.slotGeneration=1;
  source.slotGeneration=1;raw.pcm[1].slotGeneration=1;
  const priorEnd=raw.voiceStops[0];priorEnd.endedAt=overlapEnd;priorEnd.beforeTime=overlapEnd;priorEnd.afterTime=overlapEnd;
  priorEnd.endedFrame=Math.ceil(overlapEnd*raw.clock.sampleRate);
  return {raw,expected};
}
function sourceInsideMergeWindowFixture(){
  const {raw,expected}=overlappingVoiceFixture(),start=raw.starts[1],source=raw.sourcePcm[1],rate=raw.clock.sampleRate,
    window=raw.pcm[0],frame=window.startFrame+16,onsetFrame=frame+16,endFrame=onsetFrame+128,time=frame/rate;
  Object.assign(start,{beforeTime:time,afterTime:time});
  Object.assign(source,{startFrame:frame,onsetFrame,endFrame,
    blocks:[pcmBlock(raw.clock,frame,onsetFrame,{sequence:3}),pcmBlock(raw.clock,frame+128,onsetFrame,{sequence:4})]});
  Object.assign(raw.markReceipts.find(row=>row.eventSerial===start.eventSerial&&row.mode==='onset'),{startFrame:frame});
  Object.assign(raw.markReceipts.find(row=>row.eventSerial===start.eventSerial&&row.mode==='range'),{startFrame:onsetFrame,endFrame});
  const output=clone(source);Object.assign(output,{mode:'range',tapId:start.outputTap,startFrame:onsetFrame,onsetFrame:null});
  for(const block of output.blocks)for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);
    for(let index=0;index<samples.length;index++)samples[index]*=.5;block[side]=Array.from(words);}
  raw.pcm[1]=output;
  const event=raw.native.events[1],route=raw.native.routes[1];
  for(const row of [event,route,raw.hookEvents[1],raw.hookRoutes[1]])Object.assign(row,{contextTime:time,contextFrame:time*rate,
    queueClockBeforeTime:time,queueClockAfterTime:time,queueClockBeforeFrame:time*rate,queueClockAfterFrame:time*rate});
  for(const call of raw.gainCalls.filter(row=>row.sequence>8))Object.assign(call,{time,beforeTime:time,afterTime:time});
  raw.gainMutations=raw.gainCalls.map(gainMutation);
  const requestedTime=(window.startFrame+96)/rate,beforeTime=requestedTime-.25/rate,afterTime=requestedTime+.25/rate;
  raw.voiceStops.push({sequence:raw.voiceStops.length+1,kind:'source-node-stop',voiceKey:start.voiceKey,nodePresent:true,
    sourceId:start.sourceId,eventSerial:start.eventSerial,sourceSlot:start.sourceSlot,slotGeneration:start.sourceSlotGeneration,
    requestedTime,requestedFrame:Math.ceil(Math.max(requestedTime,beforeTime)*rate),
    stopUpperFrame:Math.ceil(Math.max(requestedTime,afterTime)*rate),beforeTime,afterTime});
  return {raw,expected};
}
function verdict(mutate=()=>{}){const {raw,expected}=fixture();mutate(raw,expected);return assessPresentationAudioOutput(seal(raw),expected);}

test('installer validates the actual Runtime realm Map without accepting a parent-realm replacement',async()=>{
  const host=vm.runInNewContext('({Map, voices:new Map()})');
  host.__th08SfxLatencyProbeEnabled=true;host.__th08SfxLatencyTrace={uploads:[]};host.AudioWorkletNode=function(){};
  const context={state:'running',audioWorklet:{}},pool={context,voices:host.voices};
  const runtime={Module:{canvas:{ownerDocument:{defaultView:host}},eaglerSfxOutput:pool,SDL3:{audioContext:context}}};
  assert.equal(pool.voices instanceof Map,false);
  await assert.rejects(installPresentationAudioProbe(runtime,{epoch:1,rateHz:60}),/retain all actual eager SFX uploads/);
  pool.voices=new Map();
  await assert.rejects(installPresentationAudioProbe(runtime,{epoch:1,rateHz:60}),/Actual WebSfx AudioContext and voice pool/);
});

test('synthetic classifier fixture binds native play, actual source, gain and both PCM paths',()=>{
  const result=verdict();assert.equal(result.status,'passed',result.reason);assert.equal(result.comparedPcmFrames,128);
  assert.equal(result.captureLedger.gainMutations,8);assert.equal(result.captureLedger.voiceStopRecords,0);
  const failed=verdict(raw=>{raw.gaps.push({eventSerial:47,reason:'test-gap'});});
  assert.equal(failed.status,'failed');assert.equal(failed.captureLedger.sourceSlotCapacityGaps,0);
});
test('a production no-op voice stop retains null source identity and a bounded pre-start bracket',()=>{
  const {raw,expected}=noOpControlStopFixture(),result=assessPresentationAudioOutput(seal(raw),expected);
  assert.equal(result.status,'passed',result.reason);assert.equal(result.captureLedger.voiceStopRecords,1);
  const forged=clone(raw);forged.voiceStops[0].sourceId=forged.starts[0].sourceId;forged.starts[0].priorControlStop=forged.voiceStops[0];
  assert.match(assessPresentationAudioOutput(seal(forged),expected).reason,/Idle production voice stop carries source identity/);
});
test('an active replacement followed by a no-op preserves both lifecycle rows and the latest start receipt',()=>{
  const {raw,expected}=activeThenNoOpControlStopFixture(),result=assessPresentationAudioOutput(seal(raw),expected);
  assert.equal(result.status,'passed',result.reason);assert.equal(result.captureLedger.voiceStopKinds['voice-pool-stop'],2);
  assert.equal(raw.voiceStops[0].nodePresent,true);assert.equal(raw.voiceStops.at(-1).nodePresent,false);
  assert.deepEqual(raw.starts[1].priorControlStop,raw.voiceStops.at(-1));
});
test('source taps remain leased through actual node end and both raw receipts, then advance generation',()=>{
  const slot={generation:0,lease:null,lastReceiptSequence:0,lastEnded:null},first=leasePresentationAudioSourceSlot(slot,{sourceId:1,eventSerial:47});
  assert.equal(first.slotGeneration,1);assert.equal(leasePresentationAudioSourceSlot(slot,{sourceId:2,eventSerial:48}),null);
  const endedRow={kind:'source-node-ended',sourceId:1,eventSerial:47,endedAt:2.2};
  for(const incomplete of [{ended:false,sourceReceiptDone:true,outputReceiptDone:true},
    {ended:true,sourceReceiptDone:false,outputReceiptDone:true},{ended:true,sourceReceiptDone:true,outputReceiptDone:false},
    {ended:true,sourceReceiptDone:true,outputReceiptDone:true,continuationPending:true}])
    assert.equal(releasePresentationAudioSourceSlot(slot,first,{...incomplete,sourceReceiptSequence:1,endedRow}),false);
  assert.equal(releasePresentationAudioSourceSlot(slot,first,{ended:true,sourceReceiptDone:true,outputReceiptDone:true,
    sourceReceiptSequence:7,endedRow}),true);
  const second=leasePresentationAudioSourceSlot(slot,{sourceId:2,eventSerial:48});
  assert.equal(second.slotGeneration,2);assert.equal(second.previousSlotReceiptSequence,7);assert.equal(second.previousSlotEnded,endedRow);
});
test('a future stop request cannot release a source slot before ended and both PCM receipts',()=>{
  const slot={generation:0,lease:null,lastReceiptSequence:0,lastEnded:null},lease=leasePresentationAudioSourceSlot(slot,{sourceId:1,eventSerial:47});
  const futureStop={stopRequested:true,requestedFrame:999,ended:false,sourceReceiptDone:true,outputReceiptDone:true,
    sourceReceiptSequence:1,endedRow:{kind:'source-node-ended',sourceId:1}};
  assert.equal(releasePresentationAudioSourceSlot(slot,lease,futureStop),false);
  assert.equal(slot.lease,lease);assert.equal(leasePresentationAudioSourceSlot(slot,{sourceId:2,eventSerial:48}),null);
});
test('schema 3 or mismatched slot-generation evidence cannot satisfy the lifecycle contract',()=>{
  const {raw,expected}=fixture();assert.equal(assessPresentationAudioOutput({...seal(raw),schema:'eagler-presentation/audio-output/3'},expected).status,'failed');
  assert.equal(verdict(raw=>{raw.starts[0].sourceSlotGeneration=2;}).status,'failed');
  assert.equal(verdict(raw=>{raw.sourcePcm[0].slotReceiptSequence=0;}).status,'failed');
  assert.equal(verdict(raw=>{raw.pcm[0].slotGeneration=2;}).status,'failed');
  assert.equal(verdict(raw=>{raw.markReceipts[1].sourceId=99;}).status,'failed');
});
test('slot reuse requires the prior generation ended boundary and completed source receipt',()=>{
  const {raw,expected}=reusedSlotFixture(),result=assessPresentationAudioOutput(seal(raw),expected);
  assert.equal(result.status,'passed',result.reason);assert.equal(result.captureLedger.reusedSourceSlots,1);
  const bracketed=clone(raw),ended=bracketed.voiceStops[0];
  ended.beforeTime=ended.endedAt+.00002;ended.afterTime=ended.endedAt+.00004;ended.endedFrame=Math.ceil(ended.afterTime*bracketed.clock.sampleRate);
  bracketed.starts[1].previousSlotEnded=clone(ended);
  const bracketedResult=assessPresentationAudioOutput(seal(bracketed),expected);
  assert.equal(bracketedResult.status,'passed',bracketedResult.reason||'ended callback reads may cross sample frames');
  const lateFrame=clone(bracketed),lateEnd=lateFrame.voiceStops[0];
  lateEnd.endedFrame=Math.ceil(lateFrame.starts[1].beforeTime*lateFrame.clock.sampleRate)+1;
  lateFrame.starts[1].previousSlotEnded=clone(lateEnd);
  assert.match(assessPresentationAudioOutput(seal(lateFrame),expected).reason,/previous raw PCM receipt and actual node end/);
  const earlyFrame=clone(bracketed),earlyEnd=earlyFrame.voiceStops[0];
  earlyEnd.endedFrame=Math.ceil(earlyEnd.endedAt*earlyFrame.clock.sampleRate);earlyFrame.starts[1].previousSlotEnded=clone(earlyEnd);
  assert.match(assessPresentationAudioOutput(seal(earlyFrame),expected).reason,/actual render boundary/);
  const beforeStart=clone(raw);beforeStart.voiceStops[0].endedAt=beforeStart.starts[0].afterTime-.001;
  beforeStart.starts[1].previousSlotEnded=clone(beforeStart.voiceStops[0]);
  assert.match(assessPresentationAudioOutput(seal(beforeStart),expected).reason,/actual render boundary/);
  const broken=clone(raw);broken.starts[1].previousSlotReceiptSequence=0;
  assert.match(assessPresentationAudioOutput(seal(broken),expected).reason,/previous raw PCM receipt/);
  const unended=clone(raw);unended.voiceStops=[];
  assert.match(assessPresentationAudioOutput(seal(unended),expected).reason,/ended boundary/);
});
test('shared voice merge windows require every other source to have a nonoverlapping stop/end boundary',()=>{
  const {raw,expected}=overlappingVoiceFixture(),result=assessPresentationAudioOutput(seal(raw),expected);
  assert.equal(result.status,'failed');assert.match(result.reason,/overlap this shared voice-merge PCM window/);
});
test('a source started and stopped inside a shared voice window is rejected, including a future stop and bracket advance',()=>{
  const {raw,expected}=sourceInsideMergeWindowFixture(),window=raw.pcm[0],sourceStart=Math.ceil(Math.max(raw.starts[1].when,raw.starts[1].beforeTime)*raw.clock.sampleRate),
    stop=raw.voiceStops.find(row=>row.kind==='source-node-stop');
  assert.ok(sourceStart>window.startFrame&&sourceStart<window.endFrame);
  assert.ok(stop.requestedFrame>window.startFrame&&stop.requestedFrame<window.endFrame);
  assert.equal(stop.stopUpperFrame,stop.requestedFrame+1,'The stop observation bracket advances across one sample frame');
  const inside=assessPresentationAudioOutput(seal(raw),expected);
  assert.equal(inside.status,'failed');assert.match(inside.reason,/overlap this shared voice-merge PCM window/);
  const future=clone(raw),futureStop=future.voiceStops.find(row=>row.kind==='source-node-stop');
  futureStop.requestedTime=(window.endFrame+64)/future.clock.sampleRate;
  futureStop.requestedFrame=Math.ceil(Math.max(futureStop.requestedTime,futureStop.beforeTime)*future.clock.sampleRate);
  futureStop.stopUpperFrame=Math.ceil(Math.max(futureStop.requestedTime,futureStop.afterTime)*future.clock.sampleRate);
  const futureResult=assessPresentationAudioOutput(seal(future),expected);
  assert.equal(futureResult.status,'failed');assert.match(futureResult.reason,/overlap this shared voice-merge PCM window/);
  const forged=clone(raw);forged.voiceStops.find(row=>row.kind==='source-node-stop').requestedFrame++;
  assert.equal(assessPresentationAudioOutput(seal(forged),expected).status,'failed');
});
test('normalized logical-tick numeric SFX indices allow distinct events and reject duplicates or invalid indices',()=>{
  const {raw,expected}=fixture(),assessTick=events=>assessPresentationAudioOutput(seal(raw),{...expected,
    logicalTickIntervals:[[...events.map(values=>({events:values}))]]});
  for(const tickEvents of [[ [0,7] ],[[0,20],[7]]]){
    const result=assessTick(tickEvents);assert.equal(result.status,'passed',result.reason);
  }
  const duplicate=assessTick([[7,7]]);assert.equal(duplicate.status,'failed');assert.match(duplicate.reason,/same SFX more than once/);
  for(const index of [-1,46,1.5,'7']){
    const invalid=assessTick([[index]]);assert.equal(invalid.status,'failed');assert.match(invalid.reason,/invalid normalized SFX index/);
  }
});
test('gain mutation assessment follows the raw producer args and timing bracket fields',()=>{
  const {raw,expected}=fixture(),mutation=raw.gainMutations[0];
  assert.deepEqual(Object.keys(mutation),['sequence','method','voiceKey','side','args','beforeTime','afterTime']);
  assert.equal(assessPresentationAudioOutput(seal(raw),expected).status,'passed');
  for(const alter of [row=>{delete row.args;},row=>{row.args[0]++;},row=>{row.args[1]++;},row=>{row.args.push(99);},
    row=>{row.beforeTime++;},row=>{row.afterTime++;}]){
    const broken=clone(raw);alter(broken.gainMutations[0]);
    assert.equal(assessPresentationAudioOutput(seal(broken),expected).status,'failed');
  }
  const fake=clone(raw);fake.gainMutations[0].value=fake.gainCalls[0].value;fake.gainMutations[0].time=fake.gainCalls[0].time;
  fake.gainMutations[0].args[0]++;
  assert.equal(assessPresentationAudioOutput(seal(fake),expected).status,'failed','fabricated value/time fields cannot replace producer args');
});
test('fixed AudioHost pan/volume controls retain four gain calls per channel, rejecting missing and extra calls',()=>{
  for(const count of [2,3,5]){
    const result=verdict(raw=>{const template=raw.gainCalls[0];
      raw.gainCalls=Array.from({length:count*2},(_,index)=>({...template,sequence:index+1,side:index%2?'right':'left'}));
      raw.gainMutations=raw.gainCalls.map(gainMutation);
      raw.starts[0].gainSequence=raw.gainCalls.length;
    });
    assert.equal(result.status,'failed');assert.match(result.reason,/four production AudioHost\/WebSfx gain calls/);
  }
});
test('scheduled source and native route without real PCM cannot pass',()=>{
  assert.equal(verdict(raw=>{raw.pcm=[];}).status,'failed');
  assert.equal(verdict(raw=>{raw.sourcePcm=[];}).status,'failed');
  assert.equal(verdict(raw=>{raw.starts=[];}).status,'failed');
});
test('actual silence, disconnected output and wrong gain fail while source remains nonzero',()=>{
  for(const mode of ['silence','disconnect','gain']){
    const result=verdict(raw=>{for(const block of raw.pcm[0].blocks){if(mode==='disconnect')block.inputChannels=0;
      for(const side of ['leftWords','rightWords']){const words=Uint32Array.from(block[side]),samples=new Float32Array(words.buffer);
        for(let i=0;i<samples.length;i++)samples[i]=mode==='gain'?samples[i]*.25:0;block[side]=Array.from(words);}}});
    assert.equal(result.status,'failed',mode);
    if(mode==='gain')assert.match(result.reason,/Native audio event 47 \(logicalIndex 0\), left channel: Actual output PCM.*frame/);
  }
});
test('AudioParam API converts its float argument before rendered multiplication',()=>{
  const gain=.1039920172913118,source=Float32Array.from([.7674566507339478,-.049142589]),
    output=Float32Array.from(source,sample=>Math.fround(sample*Math.fround(gain))),call={value:gain,time:0,beforeTime:0,afterTime:0,
      hadActiveSource:false,controlStoppedActive:false};
  assert.doesNotThrow(()=>assertPresentationGainPcm({source,output,startFrame:128,sampleRate:48000,baseline:gain,calls:[call],firstPullFrame:128}));
  assert.notEqual(output[0],Math.fround(source[0]*gain),"the browser's AudioParam float conversion is observable in PCM");
});
test('source, voice gain and output tap identities are cross-bound to one production voice',()=>{
  assert.equal(verdict().status,'passed');
  assert.equal(verdict(raw=>{raw.starts[0].voiceSplitConnectionCount=0;}).status,'failed');
  assert.equal(verdict(raw=>{raw.bindings[0].graph.outputTapConnected=false;}).status,'failed');
  assert.equal(verdict(raw=>{raw.gainMutations[0].method='linearRampToValueAtTime';}).status,'failed');
  assert.equal(verdict(raw=>{raw.gainMutations.push({method:'value',voiceKey:'100',side:'left',value:0});}).status,'failed');
});
test('raw SHA and semantic checks both reject tampered PCM',()=>{
  const {raw,expected}=fixture(),output=seal(raw);raw.pcm[0].blocks[0].leftWords[16]++;
  assert.match(assessPresentationAudioOutput(output,expected).reason,/hash/);
  assert.equal(assessPresentationAudioOutput(seal(raw),expected).status,'failed');
});
test('native events, route, uploaded key, actual source and interval order are independent obligations',()=>{
  for(const mutate of [raw=>{raw.hookEvents[0].logicalIndex=4;},raw=>{raw.native.events[0].handle=2;},
    raw=>{raw.native.routes[0].direct=false;},raw=>{raw.starts[0].bufferSame=false;},raw=>{raw.starts[0].sourceTap=5;},
    raw=>{raw.intervals[0].events=[];},raw=>{raw.gainCalls.splice(0,1);},raw=>{raw.hookRoutes=[];}])
    assert.equal(verdict(mutate).status,'failed');
});
test('actual dropped/error/gap metadata cannot be hidden by nonzero PCM',()=>{
  for(const mutate of [raw=>{raw.dropped=1;},raw=>{raw.native.dropped.events=1;},raw=>{raw.errors.push({message:'clock'});},
    raw=>{raw.gaps.push({eventSerial:47,reason:'missing'});},raw=>{raw.native.errors.push({message:'callback'});}])
    assert.equal(verdict(mutate).status,'failed');
});
test('real worklet marker and final error acknowledgements are mandatory',()=>{
  for(const mutate of [raw=>{raw.markReceipts.pop();},raw=>{raw.taps[0].configuredAck=null;},raw=>{raw.taps[0].final=null;},
    raw=>{raw.taps[1].final.failures=1;},raw=>{raw.taps[0].final.pending=1;}])assert.equal(verdict(mutate).status,'failed');
});
test('same PCM with another epoch, rate or finite clock horizon cannot pass',()=>{
  assert.equal(verdict((raw,expected)=>{expected.epoch++;}).status,'failed');
  assert.equal(verdict((raw,expected)=>{expected.rateHz=144;}).status,'failed');
  assert.equal(verdict(raw=>{raw.window.finishedAtTime=raw.clock.endTime;}).status,'failed');
});
test('retained onset cannot skip a nonzero prefix or invent leading silence',()=>{
  assert.equal(verdict(raw=>{raw.sourcePcm[0].onsetFrame++;}).status,'failed');
  assert.equal(verdict(raw=>{raw.sourcePcm[0].blocks[0].leftWords[0]=0x3e800000;}).status,'failed');
});
test('missing, duplicated and reordered raw PCM blocks remain failures',()=>{
  for(const mutate of [raw=>{raw.sourcePcm[0].blocks.pop();},raw=>{raw.pcm[0].blocks.reverse();},
    raw=>{raw.pcm[0].blocks[1].processSequence++;},raw=>{raw.sourcePcm[0].blocks[1].frame++;}])assert.equal(verdict(mutate).status,'failed');
});
test('segmented local frame clock stays exact after 804 seconds and keeps original segment array lengths',()=>{
  for(const rate of [44100,48000]){const plan=presentationAudioFrameClockPlan({sampleRate:rate,anchorTime:3});
    for(const elapsed of [0,255,257,804,837,7199]){const frame=plan.anchorFrame+elapsed*rate;
      const block=clockBlock(plan,frame,128,{segmentLength:128});assert.equal(decodePresentationAudioClockBlock(block,plan).firstFrame,frame);
      assert.ok(Math.max(...block.local)<2**24);}
    const frame=plan.anchorFrame+plan.segmentFrames-60,block=clockBlock(plan,frame,128,{segmentLength:128});
    assert.equal(decodePresentationAudioClockBlock(block,plan).endFrame,frame+128);
    assert.throws(()=>decodePresentationAudioClockBlock({...block,segment:[block.segment[0]]},plan),/Noncontinuous/);
  }
});
test('one-sample anchor drift, missing step, missing reset and float32 precision loss are rejected exactly',()=>{
  const plan=presentationAudioFrameClockPlan({sampleRate:44100,anchorTime:3}),frame=plan.anchorFrame+plan.segmentFrames-64;
  for(const mutation of [b=>{b.segment[64]=0;},b=>{b.segment[63]=1;},b=>{b.local[64]+=plan.segmentFrames;},
    b=>{b.local[65]=b.local[64];},b=>{b.local[80]=2**24;},b=>{b.local=[];}]){
    const block=clockBlock(plan,frame,128,{segmentLength:128});mutation(block);assert.throws(()=>decodePresentationAudioClockBlock(block,plan));
  }
});
test('same-time command order is ramp then local reset; segment remains a separate real parameter',()=>{
  const plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:3});
  assert.deepEqual(plan.parameters.localFrame.slice(0,4),[['setValueAtTime',1048576,3],['linearRampToValueAtTime',2584576,35],
    ['setValueAtTime',1048576,35],['linearRampToValueAtTime',2584576,67]]);
  assert.deepEqual(plan.parameters.segment.slice(0,2),[['setValueAtTime',0,3],['setValueAtTime',1,35]]);
  assert.equal(plan.endTime-plan.anchorTime,7200);
});
function processorFixture(){let Processor;const messages=[],context={currentFrame:0,Float32Array,Uint32Array,Number,Array,
  AudioWorkletProcessor:class{constructor(){this.port={postMessage:message=>messages.push(message)};}},registerProcessor:(_name,value)=>{Processor=value;}};
  vm.runInNewContext(presentationAudioPcmProcessorSource(),context);const processor=new Processor();return {processor,messages,context};}
function render(fixture,plan,frame,{segmentLength=1,samples=.25}={}){const block=clockBlock(plan,frame,128,{segmentLength});fixture.context.currentFrame=frame;
  return fixture.processor.process([[new Float32Array(128).fill(samples),new Float32Array(128).fill(samples)]],
    [[new Float32Array(128),new Float32Array(128)]],{localFrame:Float32Array.from(block.local),segment:Float32Array.from(block.segment)});}
test('the real processor source preserves arrays and silent branch while capturing actual input',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});
  f.processor.port.onmessage({data:{kind:'mark',mode:'onset',purpose:'event-audio',eventSerial:47,sourceId:9,slotGeneration:3,
    startFrame:frame,prefixFrames:128,budgetFrames:48000}});
  render(f,plan,frame);assert.equal(f.messages.filter(row=>row.kind==='pcm').length,1);
  const pcm=f.messages.find(row=>row.kind==='pcm');assert.equal(pcm.blocks[0].segment.length,1);assert.equal(pcm.blocks[0].local.length,128);
  assert.equal(pcm.blocks[0].leftWords[0],0x3e800000);assert.equal(pcm.purpose,'event-audio');assert.equal(pcm.sourceId,9);assert.equal(pcm.slotGeneration,3);
  const armed=f.messages.find(row=>row.kind==='armed');assert.equal(armed.sourceId,9);assert.equal(armed.slotGeneration,3);assert.equal(armed.purpose,'event-audio');
});
test('the real processor fails a frame gap and does not reset continuity for a new marker',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});render(f,plan,frame);
  f.processor.port.onmessage({data:{kind:'mark',mode:'onset',eventSerial:48,startFrame:frame+256,prefixFrames:128,budgetFrames:48000}});
  render(f,plan,frame+256);assert.ok(f.messages.some(row=>row.reason==='noncontinuous-clock-blocks'));assert.ok(!f.messages.some(row=>row.kind==='pcm'));
});
test('worklet currentFrame is retained without replacing the independent per-sample clock',()=>{
  const f=processorFixture(),plan=presentationAudioFrameClockPlan({sampleRate:48000,anchorTime:2}),frame=plan.anchorFrame;
  f.processor.port.onmessage({data:{kind:'configure',plan,historyFrames:96000}});render(f,plan,frame);
  const block=clockBlock(plan,frame+128);f.context.currentFrame=frame;
  f.processor.process([[new Float32Array(128),new Float32Array(128)]],[[new Float32Array(128),new Float32Array(128)]],
    {localFrame:Float32Array.from(block.local),segment:Float32Array.from(block.segment)});
  assert.ok(!f.messages.some(row=>row.kind==='error'));assert.equal(f.processor.history.at(-1).currentFrame,frame);
  assert.equal(f.processor.history.at(-1).frame,frame+128);
});
test('bounded equality keeps strict deep semantics without constructing a giant diagnostic diff',()=>{
  const left=Array.from({length:20000},(_,i)=>({i,value:'pcm'})),right=clone(left);right.at(-1).value='changed';
  assert.throws(()=>assertPresentationAudioEqual(left,right,'native rows'),error=>error.message==='native rows: retained values differ'&&error.message.length<100);
  assertPresentationAudioEqual(left,clone(left),'same');
});
test('gain transitions must follow the exact observed timing bracket and cannot reverse or drift',()=>{
  const source=Float32Array.from([1,1,1,1]),call={value:.5,time:2/48000,beforeTime:2/48000,afterTime:2/48000,hadActiveSource:true,controlStoppedActive:true};
  assertPresentationGainPcm({source,output:Float32Array.from([1,1,.5,.5]),startFrame:0,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:0});
  for(const output of [[1,1,1,1],[.5,.5,1,1],[1,.5,.5,.5],[1,1,.5,.49]])assert.throws(()=>assertPresentationGainPcm({source,
    output:Float32Array.from(output),startFrame:0,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:0}));
});
test('an idle first-pull clamp is finite and is forbidden for a previously active production voice',()=>{
  const source=new Float32Array(8).fill(1),output=new Float32Array(8).fill(.5),call={value:.5,time:0,beforeTime:0,afterTime:0,hadActiveSource:false,controlStoppedActive:false};
  assertPresentationGainPcm({source,output,startFrame:128,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:128});
  const delayed=Float32Array.from([1,1,.5,.5,.5,.5,.5,.5]);
  assert.throws(()=>assertPresentationGainPcm({source,output:delayed,startFrame:128,sampleRate:48000,baseline:1,calls:[call],firstPullFrame:128}));
});
test('capture beyond clock horizon is a real failure even if old errors were removed',()=>{
  assert.equal(verdict(raw=>{raw.window.finishedAtTime=raw.clock.endTime+1;raw.errors=[];raw.gaps=[];}).status,'failed');
});
