import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {assertQueueTailMutationIsRejected,classifySfxLatencyEvidence,sfxLatencyPcmProcessorSource,sfxLatencyProbeInitSource}
  from './sfx-latency-contract.mjs';
import {deriveTh08NormalSfxContract,TH08_NORMAL_SFX_PROBE}
  from '../../tests/adaptation/adapters/audio.mjs';
import {resolveSfxLatencyContract,SFX_LATENCY_PRODUCTS}
  from '../../tests/adaptation/adapters/audio.mjs';

const root=resolve(import.meta.dirname,'../..');
const soundPath='ports/th08/web/cpp/game/SoundEffects.cpp';
const managerPath='ports/th08/web/cpp/platform/GameAudioManager.cpp';
const audioHostPath='ports/th08/web/cpp/sdl/AudioHost.cpp';

async function productionSources(){return {soundEffectsSource:await readFile(resolve(root,soundPath),'utf8'),
  audioManagerSource:await readFile(resolve(root,managerPath),'utf8')};}

function evidence(queueFrames=3072,effectiveDelayFrames=200){
  const eventTime=10,eventFrame=eventTime*44100,queueClockAfterTime=eventTime+2/44100;
  const event={serial:4,logicalIndex:10,handle:11,voiceKey:'2150000000',queueFrames,queueRate:44100,
    sourceRate:22050,contextSampleRate:44100,contextTime:eventTime,contextFrame:eventFrame,sessionId:7,inputSerial:15,
    queueClockBeforeTime:eventTime,queueClockAfterTime,queueClockBeforeFrame:eventFrame,
    queueClockAfterFrame:queueClockAfterTime*44100,queueSampleOrder:'cpp-sdl-queue-clock-bracket'};
  const inputTime=event.contextTime-.001;
  const input={serial:15,sessionId:7,type:'keydown',key:'z',code:'KeyZ',isTrusted:true,
    contextTime:inputTime,contextSampleRate:44100,contextFrame:inputTime*44100};
  const startPlanFrame=event.contextFrame+128,lead=13;
  const trace={sequence:6,recordLimit:512,sessionId:7,dropped:{uploads:0,events:0,routes:0,errors:0,probe:0,probeCallbacks:0},errors:[],
    uploads:[{serial:1,handle:11,voiceKey:event.voiceKey,sourceRate:event.sourceRate,sourceFrames:2048,channels:1,uploadSuccess:true}],
    events:[event],inputs:[input],routes:[{serial:6,eventSerial:event.serial,logicalIndex:10,handle:11,voiceKey:event.voiceKey,
      direct:true,fallback:false,fallbackStarted:false,pending:false,queueFrames,queueRate:44100,contextSampleRate:44100,
      contextTime:event.contextTime+256/44100,contextFrame:(event.contextTime+256/44100)*44100,
      queueClockBeforeTime:event.contextTime+256/44100,queueClockAfterTime:event.contextTime+258/44100,
      queueClockBeforeFrame:(event.contextTime+256/44100)*44100,queueClockAfterFrame:(event.contextTime+258/44100)*44100,
      queueSampleOrder:'cpp-sdl-queue-clock-bracket',sessionId:7}]};
  const observation={sessionId:7,eventSerial:event.serial,handle:11,voiceKey:event.voiceKey,startPlanFrame,
    firstNonzeroFrame:startPlanFrame+effectiveDelayFrames+lead,markerReceivedAtFrame:event.contextFrame+128,
    historyStartFrame:event.contextFrame,searchedFromFrame:startPlanFrame,searchCompleteFromFrame:startPlanFrame,
    searchCompleteThroughFrame:startPlanFrame+effectiveDelayFrames+lead,blockFrames:128,sampleLeft:.2,sampleRight:.2};
  const sourceLead={eventSerial:event.serial,sessionId:7,voiceKey:event.voiceKey,sourceRate:event.sourceRate,sourceFrames:2048,
    renderedFromTargetBuffer:true,contextSampleRate:44100,offsetSeconds:0,playbackRate:1,frames:lead,startPlanFrame};
  return {contract:{...TH08_NORMAL_SFX_PROBE},event,input,trace,observation,sourceLead,session:{id:7,markerBlockFrames:128,contextSampleRate:44100}};
}

function sealedV3Evidence(){
  // Scalar identity/timing fields excerpted from the sealed behavior-v3 TH08 normal report.
  // The fixture deliberately contains no captured PCM payload or private audio asset.
  const contextSampleRate=48000,eventTime=1.1813333333333333,eventFrame=56704,sessionId=1;
  const voiceKey='99434744',eventSerial=1,handle=11,logicalIndex=10,queueFrames=3072,queueRate=44100;
  const event={serial:eventSerial,logicalIndex,handle,voiceKey,queueFrames,queueRate,sourceRate:22050,contextSampleRate,
    contextTime:eventTime,contextFrame:eventFrame,sessionId,inputSerial:1,queueClockBeforeTime:eventTime,queueClockAfterTime:eventTime,
    queueClockBeforeFrame:eventFrame,queueClockAfterFrame:eventFrame,queueSampleOrder:'cpp-sdl-queue-clock-bracket'};
  const input={serial:1,sessionId,type:'keydown',key:'z',code:'KeyZ',isTrusted:true,contextTime:eventTime,
    contextSampleRate,contextFrame:eventFrame};
  const trace={sessionId,recordLimit:512,dropped:{uploads:0,events:0,routes:0,errors:0,probe:0,probeCallbacks:0},errors:[],inputAmbiguous:false,
    uploads:[{serial:11,handle,voiceKey,sourceRate:22050,sourceFrames:10832,channels:1,uploadSuccess:true}],events:[event],inputs:[input],
    routes:[{serial:2,eventSerial,logicalIndex,handle,voiceKey,direct:true,fallback:false,fallbackStarted:false,pending:false,
      queueFrames,queueRate,contextSampleRate,contextTime:eventTime,contextFrame:eventFrame,queueClockBeforeTime:eventTime,
      queueClockAfterTime:eventTime,queueClockBeforeFrame:eventFrame,queueClockAfterFrame:eventFrame,
      queueSampleOrder:'cpp-sdl-queue-clock-bracket',sessionId}]};
  const observation={kind:'pcm',sessionId,eventSerial,handle,voiceKey,firstNonzeroFrame:56832,markerReceivedAtFrame:56192,
    historyStartFrame:56192,searchedFromFrame:eventFrame,searchCompleteFromFrame:eventFrame,searchCompleteThroughFrame:56832,
    blockFrames:128,startPlanFrame:eventFrame,sampleLeft:.03480437770485878,sampleRight:.03480437770485878};
  const sourceLead={eventSerial,sessionId,voiceKey,sourceRate:22050,sourceFrames:10832,renderedFromTargetBuffer:true,
    contextSampleRate,offsetSeconds:0,playbackRate:1,frames:0,startPlanFrame:eventFrame};
  return {contract:{...TH08_NORMAL_SFX_PROBE},event,input,trace,observation,sourceLead,
    session:{id:sessionId,markerBlockFrames:128,contextSampleRate}};
}

test('SFX latency catalog lists only the explicit TH08 normal scene',()=>{
  assert.deepEqual(SFX_LATENCY_PRODUCTS,[{game:'th08',variant:'normal'}]);
  assert.equal(resolveSfxLatencyContract('th08','normal'),TH08_NORMAL_SFX_PROBE);
  assert.equal(resolveSfxLatencyContract('th08','mp'),null);
  assert.equal(resolveSfxLatencyContract('th09','normal'),null);
});

test('TH08 normal production source maps logical sound 10 through sample 8 to buffer 11',async()=>{
  const actual=deriveTh08NormalSfxContract(await productionSources());
  assert.deepEqual(actual,{...TH08_NORMAL_SFX_PROBE,evidence:
    'sound_definitions → sound_samples → GameAudioManager::prepare_samples buffer i+1 → SoundEffects::process deduplicated play observer'});
});

test('native queue records use one clock read and expose the SDL-before-clock boundary',async()=>{
  const source=await readFile(resolve(root,audioHostPath),'utf8');
  assert.equal((source.match(/context\.currentTime/g)||[]).length,2,
    'the upload row and shared clock sampler each read currentTime once; event/route rows reuse bracket arguments');
  assert.equal((source.match(/const contextTime=queueClockBeforeTime;/g)||[]).length,2,
    'event and route records use the captured lower clock bound without rereading currentTime');
  assert.equal((source.match(/const contextFrame=contextTime!==null/g)||[]).length,3,
    'all recorded clock frames derive from their captured times');
  assert.equal((source.match(/queueSampleOrder:'cpp-sdl-queue-clock-bracket'/g)||[]).length,2,
    'event and route rows identify their before/after clock bracket');
  assert.match(source,/const double clock_before=th08_sfx_latency_context_time\(\);[\s\S]*?SDL_GetAudioStreamQueued[\s\S]*?const double clock_after=th08_sfx_latency_context_time\(\);/);
});

test('production mapping mutations cannot be hidden by a duplicated test-side expectation',async()=>{
  const sources=await productionSources();
  const renamed={...sources,soundEffectsSource:sources.soundEffectsSource.replace('"se_ok00.wav"','"se_cancel00.wav"')};
  assert.throws(()=>deriveTh08NormalSfxContract(renamed),/now maps to sample/);
  const wrongHandle={...sources,audioManagerSource:sources.audioManagerSource.replace('effects.buffers[i]=i+1;','effects.buffers[i]=i+2;')};
  assert.throws(()=>deriveTh08NormalSfxContract(wrongHandle),/buffer handle i\+1/);
  const missingObservation={...sources,soundEffectsSource:sources.soundEffectsSource.replace(
    'th08_sfx_latency_event(static_cast<u32>(index),buffer);','')};
  assert.throws(()=>deriveTh08NormalSfxContract(missingObservation),/logical sound event must observe/);
  const premature={...sources,audioManagerSource:sources.audioManagerSource+'\nvoid invalid_request_observer(){th08_sfx_latency_event(10,11);}\n'};
  assert.throws(()=>deriveTh08NormalSfxContract(premature),/deduplicated production/);
});

test('actual target PCM before the queued tail passes with both queue depths',async()=>{
  for(const frames of [3072,6144]){
    const sample=evidence(frames,200),verdict=classifySfxLatencyEvidence(sample);
    assert.equal(verdict.status,'passed');
    assert.equal(verdict.effectiveStartFrame,sample.event.contextFrame+128+200);
    assert.equal(verdict.queueTailLowerFrame,(sample.event.queueClockBeforeTime+frames/sample.event.queueRate)*sample.event.contextSampleRate);
    assert.equal(verdict.queueTailUpperFrame,(sample.event.queueClockAfterTime+frames/sample.event.queueRate)*sample.event.contextSampleRate);
    assert.equal(verdict.queueTailFrame,verdict.queueTailLowerFrame);
    assert.ok(verdict.queueTailUpperFrame>=verdict.queueTailLowerFrame);
    assert.equal(verdict.queueSampleOrder,'cpp-sdl-queue-clock-bracket');
    assert.equal(verdict.inputToEffectiveStartFrames,verdict.effectiveStartFrame-sample.input.contextFrame);
    const incrementallyScanned={...sample,observation:{...sample.observation,
      historyStartFrame:sample.observation.startPlanFrame+64}};
    assert.equal(classifySfxLatencyEvidence(incrementallyScanned).status,'passed',
      'a marker seen before onset can prove long leading silence incrementally after old zero blocks roll out');
  }
});

test('sealed v3 PCM passes with a marker processed 512 frames before the event, while scheduled-before-event and lost-onset-history do not',()=>{
  const sample=sealedV3Evidence();
  const positive=classifySfxLatencyEvidence(sample);
  assert.equal(sample.observation.markerReceivedAtFrame,sample.event.contextFrame-512);
  assert.equal(positive.status,'passed',positive.reason);
  assert.equal(positive.firstNonzeroFrame,56832);
  assert.equal(positive.effectiveStartFrame,56832-sample.sourceLead.frames);
  assert.ok(positive.effectiveStartFrame<positive.queueTailLowerFrame);

  const scheduledTooEarly={...sample,observation:{...sample.observation,startPlanFrame:sample.event.contextFrame-128,
    searchedFromFrame:sample.event.contextFrame-128,searchCompleteFromFrame:sample.event.contextFrame-128},
    sourceLead:{...sample.sourceLead,startPlanFrame:sample.event.contextFrame-128}};
  const earlyVerdict=classifySfxLatencyEvidence(scheduledTooEarly);
  assert.equal(earlyVerdict.status,'incomplete');
  assert.match(earlyVerdict.reason,/scheduled before its correlated native logical sound event/);

  const onsetNoLongerInHistory={...sample,observation:{...sample.observation,
    historyStartFrame:sample.observation.firstNonzeroFrame+1}};
  const historyVerdict=classifySfxLatencyEvidence(onsetNoLongerInHistory);
  assert.equal(historyVerdict.status,'incomplete');
  assert.match(historyVerdict.reason,/onset is no longer present in the retained PCM history/);
});

test('onset inside the bracketed queue-tail interval is incomplete, not product failure',()=>{
  const sample=evidence(3072,2945),verdict=classifySfxLatencyEvidence(sample);
  assert.equal(verdict.status,'incomplete');
  assert.equal(verdict.effectiveStartFrame,verdict.queueTailLowerFrame+1);
  assert.equal(verdict.queueTailUpperFrame,
    (sample.event.queueClockAfterTime+sample.event.queueFrames/sample.event.queueRate)*sample.event.contextSampleRate);
  assert.ok(verdict.queueClockSampleSpanFrames>0);
  assert.equal(verdict.queueClockSampleSpanFrames,sample.event.queueClockAfterFrame-sample.event.queueClockBeforeFrame);
  assert.equal(verdict.queueClockSampleSpanTime,sample.event.queueClockAfterTime-sample.event.queueClockBeforeTime);
});

test('the same classifier rejects an applied queue-tail mutation at its measured tail',async()=>{
  for(const queueFrames of [3072,6144]){
    const sample=evidence(queueFrames);
    const queueTailLowerTime=sample.event.queueClockBeforeTime+queueFrames/sample.event.queueRate;
    const queueTailUpperTime=sample.event.queueClockAfterTime+queueFrames/sample.event.queueRate;
    const queueTailLowerFrame=queueTailLowerTime*sample.session.contextSampleRate;
    const queueTailUpperFrame=queueTailUpperTime*sample.session.contextSampleRate;
    const actualStartFrame=queueTailUpperTime*sample.session.contextSampleRate;
    const queueClockSampleSpanTime=sample.event.queueClockAfterTime-sample.event.queueClockBeforeTime;
    const queueClockSampleSpanFrames=sample.event.queueClockAfterFrame-sample.event.queueClockBeforeFrame;
    const mutation={applied:true,status:'applied',sessionId:sample.session.id,eventSerial:sample.event.serial,
      queueFrames:sample.event.queueFrames,queueRate:sample.event.queueRate,queueTailTime:queueTailUpperTime,
      queueTailLowerTime,queueTailUpperTime,queueTailLowerFrame,queueTailUpperFrame,queueClockSampleSpanTime,queueClockSampleSpanFrames,
      actualStartTime:queueTailUpperTime,actualStartFrame};
    const firstNonzeroFrame=Math.ceil(mutation.actualStartFrame)+sample.sourceLead.frames;
    sample.observation={...sample.observation,startPlanFrame:mutation.actualStartFrame,
      searchedFromFrame:Math.ceil(mutation.actualStartFrame),searchCompleteFromFrame:Math.ceil(mutation.actualStartFrame),
      firstNonzeroFrame,searchCompleteThroughFrame:firstNonzeroFrame};
    sample.sourceLead={...sample.sourceLead,startPlanFrame:mutation.actualStartFrame};
    const rejected=assertQueueTailMutationIsRejected({...sample,mutation});
    assert.equal(rejected.effectiveStartFrame,Math.ceil(mutation.actualStartFrame));
    assert.ok(rejected.effectiveStartFrame>=rejected.queueTailLowerFrame,
      'the classifier must reject a start at the upper bound when it compares against the conservative lower bound');
  }
});

test('event, route, voice, source lead and session identities are mandatory',()=>{
  const sample=evidence();
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    events:[sample.event,{...sample.event,serial:5,logicalIndex:45}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,session:{...sample.session,id:8}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,events:[{...sample.event,sessionId:8}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    events:[{...sample.event,contextTime:sample.event.contextTime+1/44100}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    events:[{...sample.event,queueSampleOrder:'unknown'}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    events:[{...sample.event,queueClockAfterTime:sample.event.queueClockBeforeTime-1/44100}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,routes:[{...sample.trace.routes[0],sessionId:8}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    routes:[{...sample.trace.routes[0],contextTime:sample.trace.routes[0].contextTime+1/44100}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,sourceLead:{...sample.sourceLead,eventSerial:99}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,sourceLead:{...sample.sourceLead,sessionId:8}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,observation:{...sample.observation,handle:12}}).status,'failed');
  assert.equal(classifySfxLatencyEvidence({...sample,observation:{...sample.observation,voiceKey:'other'}}).status,'failed');
  assert.equal(classifySfxLatencyEvidence({...sample,observation:{...sample.observation,sessionId:8}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,inputs:[]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,inputs:[{...sample.input,sessionId:8}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,inputs:[{...sample.input,code:'KeyX'}]}}).status,'failed');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    inputs:[{...sample.input,isTrusted:false}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,
    inputs:[{...sample.input,contextTime:sample.input.contextTime+1/44100}]}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,trace:{...sample.trace,inputAmbiguous:true}}).status,'incomplete');
});

test('missing frame history, fallback, overflow and empty queues cannot pass',()=>{
  const sample=evidence();
  assert.equal(classifySfxLatencyEvidence({...sample,observation:{...sample.observation,
    searchCompleteFromFrame:sample.observation.startPlanFrame+1}}).status,'incomplete');
  assert.equal(classifySfxLatencyEvidence({...sample,observation:{...sample.observation,incompleteReason:'marker-outside-history'}}).status,'incomplete');
  const fallback={...sample,trace:{...sample.trace,routes:[{...sample.trace.routes[0],direct:false,fallback:true,fallbackStarted:true}]}};
  assert.equal(classifySfxLatencyEvidence(fallback).status,'failed');
  const overflow={...sample,trace:{...sample.trace,dropped:{...sample.trace.dropped,events:1}}};
  assert.equal(classifySfxLatencyEvidence(overflow).status,'incomplete');
  const noQueue={...sample,trace:{...sample.trace,events:[{...sample.event,queueFrames:0}]}};
  assert.equal(classifySfxLatencyEvidence(noQueue).status,'incomplete');
});

test('the production AudioWorklet processor finds onset across leading-silence blocks and fixes marker receipt time',()=>{
  let Processor;
  class AudioWorkletProcessor{constructor(){this.port={onmessage:null,messages:[],postMessage(message){this.messages.push(message);},close(){}};}}
  const sandbox={AudioWorkletProcessor,currentFrame:0,registerProcessor(name,ctor){assert.equal(name,'sfx-latency-pcm-tap');Processor=ctor;}};
  vm.runInNewContext(sfxLatencyPcmProcessorSource(),sandbox);
  assert.equal(typeof Processor,'function');
  const processor=new Processor();
  processor.port.onmessage({data:{kind:'clear',sessionId:1}});
  const feed=(frame,impulseAt=-1)=>{sandbox.currentFrame=frame;const left=new Float32Array(128),right=new Float32Array(128);
    if(impulseAt>=frame&&impulseAt<frame+128)left[impulseAt-frame]=.25;
    const output=[new Float32Array(128),new Float32Array(128)];processor.process([[left,right]],[output]);
    assert.ok(output.every(channel=>channel.every(value=>value===0)),'the read-only tap must emit silence');};
  feed(0);processor.port.onmessage({data:{kind:'mark',sessionId:1,eventSerial:4,startPlanFrame:0,sourceEndFrame:2048}});
  feed(128);feed(256);feed(384);feed(512,600);
  const result=processor.port.messages.find(message=>message.kind==='pcm');
  assert.ok(result);assert.equal(result.firstNonzeroFrame,600);assert.equal(result.markerReceivedAtFrame,128);
  assert.equal(result.searchedFromFrame,0);assert.equal(result.historyStartFrame,0);
  assert.equal(result.searchCompleteFromFrame,0);assert.equal(result.searchCompleteThroughFrame,600);
});

test('the production AudioWorklet processor recovers a late marker from ring history and rejects expired history',()=>{
  let Processor;class AudioWorkletProcessor{constructor(){this.port={onmessage:null,messages:[],postMessage(message){this.messages.push(message);},close(){}};}}
  const sandbox={AudioWorkletProcessor,currentFrame:0,registerProcessor(_name,ctor){Processor=ctor;}};
  vm.runInNewContext(sfxLatencyPcmProcessorSource(),sandbox);
  const late=new Processor();late.port.onmessage({data:{kind:'clear',sessionId:2}});
  const feed=(processor,frame,impulseAt=-1)=>{sandbox.currentFrame=frame;const left=new Float32Array(128),right=new Float32Array(128);
    if(impulseAt>=frame&&impulseAt<frame+128)left[impulseAt-frame]=.5;
    processor.process([[left,right]],[[new Float32Array(128),new Float32Array(128)]]);};
  feed(late,0);feed(late,128);feed(late,256,300);feed(late,384);
  late.port.onmessage({data:{kind:'mark',sessionId:2,eventSerial:8,startPlanFrame:0,sourceEndFrame:1024}});feed(late,512);
  const recovered=late.port.messages.find(message=>message.kind==='pcm');
  assert.equal(recovered.firstNonzeroFrame,300);assert.equal(recovered.markerReceivedAtFrame,512);

  const expired=new Processor();expired.historyLimit=256;expired.port.onmessage({data:{kind:'clear',sessionId:3}});
  for(let frame=0;frame<384;frame+=128)feed(expired,frame);
  expired.port.onmessage({data:{kind:'mark',sessionId:3,eventSerial:9,startPlanFrame:0,sourceEndFrame:1024}});feed(expired,384);
  assert.equal(expired.port.messages.at(-1).kind,'incomplete');
  assert.equal(expired.port.messages.at(-1).reason,'marker-outside-history');

  const longLead=new Processor();longLead.port.onmessage({data:{kind:'clear',sessionId:4}});
  longLead.port.onmessage({data:{kind:'mark',sessionId:4,eventSerial:10,startPlanFrame:0,sourceEndFrame:30000}});
  for(let frame=0;frame<17024;frame+=128)feed(longLead,frame);
  feed(longLead,17024,17024);
  const longLeadResult=longLead.port.messages.find(message=>message.kind==='pcm');
  assert.equal(longLeadResult.firstNonzeroFrame,17024);
  assert.ok(longLeadResult.historyStartFrame>longLeadResult.searchedFromFrame,
    'incremental scanning proves long leading silence even after old zero blocks leave the ring');
  assert.equal(longLeadResult.searchCompleteFromFrame,0);
});

test('early input observer precedes shell propagation stop and mutates only the correlated production voice start',async()=>{
  const source=sfxLatencyProbeInitSource(TH08_NORMAL_SFX_PROBE),listeners=[];let sourceStartCalls=[];
  class AudioBufferSourceNode{start(...args){sourceStartCalls.push(args);return 'started';}}
  class AudioWorkletNode{constructor(){this.port={onmessage:null,messages:[],postMessage(message){this.messages.push(message);},close(){}};}
    connect(){}disconnect(){}}
  class Blob{constructor(parts){this.parts=parts;}}
  const audioContext={sampleRate:44100,currentTime:.1,state:'running',destination:{},audioWorklet:{async addModule(){}},
    createGain(){return {gain:{value:1},connect(){},disconnect(){}};}};
  const voice={buffer:{sampleRate:22050,length:2048,duration:2048/22050},merge:{connect(){},disconnect(){}},offset:0,speed:1,
    loop:false,playing:false,node:null};
  const pool={context:audioContext,voices:new Map([[2150000000,voice]]),start(target){
    const node=new AudioBufferSourceNode();node.buffer=target.buffer;target.node=node;target.playing=true;node.start(0,target.offset);}};
  const nativeTrace={sequence:1,recordLimit:512,uploads:[{serial:1,handle:11,voiceKey:'2150000000',sourceRate:22050,
    sourceFrames:2048,channels:1,uploadSuccess:true}],events:[],routes:[],errors:[],dropped:{uploads:0,events:0,routes:0,errors:0}};
  const sandbox={AudioBufferSourceNode,AudioWorkletNode,Blob,URL:{createObjectURL(){return 'blob:test';},revokeObjectURL(){}},
    addEventListener(type,listener,options){listeners.push({type,listener,options});},removeEventListener(type,listener){
      const index=listeners.findIndex(row=>row.type===type&&row.listener===listener);if(index>=0)listeners.splice(index,1);},
    __th08SfxLatencyTrace:nativeTrace};
  vm.runInNewContext(source,sandbox);
  assert.equal(listeners.length,1,'the probe init script must register its dormant capture listener before page scripts');
  const earlyListener=listeners[0].listener;
  const probe=sandbox.__th08SfxLatencyProbe;
  const nativeObservedCounts=[];
  sandbox.addEventListener('keydown',event=>{nativeObservedCounts.push(probe.snapshot().inputs.length);event.stopImmediatePropagation();},true);
  assert.equal(listeners[1].type,'keydown','the simulated shell installs its original native capture handler after the probe init script');
  const dispatchKeydown=event=>{
    let stopped=false;const dispatched={...event,stopImmediatePropagation(){stopped=true;}};
    for(const row of [...listeners]){if(row.type!==dispatched.type)continue;row.listener.call(sandbox,dispatched);if(stopped)break;}
  };
  await probe.install({SDL3:{audioContext},eaglerSfxOutput:pool});
  dispatchKeydown({type:'keydown',key:'z',code:'KeyZ',timeStamp:98,isTrusted:true});
  assert.equal(probe.snapshot().inputs.length,0,'the early listener stays dormant until a session arms its input filter');
  assert.deepEqual(nativeObservedCounts,[0],'the shell native listener still handles input while the observer is dormant');
  nativeObservedCounts.length=0;
  probe.begin();const listenerCountBeforeArm=listeners.length;probe.armInputCapture(TH08_NORMAL_SFX_PROBE.inputFilter);probe.armQueueTailMutation(11);
  assert.equal(listeners.length,listenerCountBeforeArm,'arming input capture must not add or reorder the already-registered listener');
  assert.equal(listeners[0].listener,earlyListener);
  dispatchKeydown({type:'keydown',key:'z',code:'KeyZ',timeStamp:99,isTrusted:false});
  dispatchKeydown({type:'keydown',key:'z',code:'KeyZ',timeStamp:100,isTrusted:true});
  dispatchKeydown({type:'keydown',key:'z',code:'KeyZ',timeStamp:101,isTrusted:true});
  assert.deepEqual(nativeObservedCounts,[1,2,3],
    'the later shell listener must still run and stop propagation after the probe has observed each matching input');
  const event={serial:2,logicalIndex:10,handle:11,voiceKey:'2150000000',queueFrames:3072,queueRate:44100,
    sourceRate:22050,contextSampleRate:44100,contextTime:.1,contextFrame:4410,
    queueClockBeforeTime:.1,queueClockAfterTime:.1+2/44100,queueClockBeforeFrame:4410,
    queueClockAfterFrame:(.1+2/44100)*44100,queueSampleOrder:'cpp-sdl-queue-clock-bracket'};
  nativeTrace.events.push(event);probe.onNativeEvent(event);pool.start(voice);
  const result=probe.snapshot();
  assert.equal(sourceStartCalls.length,1,'diagnostic code must call the production source start exactly once');
  assert.equal(sourceStartCalls[0][0],event.queueClockAfterTime+event.queueFrames/event.queueRate);
  assert.equal(result.starts[0].eventSerial,event.serial);
  assert.equal(result.starts[0].mutated,true);
  assert.equal(result.mutation.applied,true);
  assert.equal(result.mutation.queueTailLowerTime,event.queueClockBeforeTime+event.queueFrames/event.queueRate);
  assert.equal(result.mutation.queueTailUpperTime,event.queueClockAfterTime+event.queueFrames/event.queueRate);
  assert.equal(result.mutation.actualStartFrame,result.mutation.queueTailUpperFrame);
  assert.equal(result.inputAmbiguous,true);
  assert.equal(result.inputs.length,3,'untrusted and repeated matching inputs must be retained as evidence');
  assert.equal(result.inputs[0].reason,'untrusted-matching-input');
  assert.equal(result.inputs[2].duplicateOf,result.inputs[1].serial);
  assert.equal(result.events[0].inputSerial,result.inputs[1].serial,'the native event must associate with the first trusted match');
  probe.restoreQueueTailMutation();
  probe.detach();dispatchKeydown({type:'keydown',key:'z',code:'KeyZ',timeStamp:102,isTrusted:true});
  assert.equal(listeners.length,2,'detach must leave the early listener in its original position rather than re-registering it');
  assert.equal(nativeObservedCounts.length,4,'the shell native listener remains active after probe detach');
  assert.equal(probe.snapshot().inputs.length,3,'the persistent observer must stop recording when detached');
});

test('browser setup requires explicit product scope and keeps the production graph direct',()=>{
  assert.throws(()=>sfxLatencyProbeInitSource(),/explicit game and variant/);
  const source=sfxLatencyProbeInitSource(TH08_NORMAL_SFX_PROBE);
  assert.doesNotThrow(()=>new Function(source));
  assert.match(source,/voice\.merge\.connect\(node\);node\.connect\(zero\);zero\.connect\(context\.destination\)/);
  assert.match(source,/voice\.merge\.connect\(node\);node\.connect\(zero\);zero\.connect\(context\.destination\)/);
  assert.match(source,/AudioBufferSourceNode\.prototype\.start/);
  assert.match(source,/scheduling-at-measured-queue-tail/);
  assert.match(source,/nativeSnapshot\.dropped/);
  const captureStart=source.indexOf('function captureInput(event)');
  const captureEnd=source.indexOf('root.addEventListener(config.inputFilter.type,captureInput,true)',captureStart);
  const inputCapture=source.slice(captureStart,captureEnd);
  assert.equal((inputCapture.match(/state\.context\.currentTime/g)||[]).length,1,
    'the input frame must derive from one captured AudioContext time');
  assert.match(inputCapture,/isTrusted:event\.isTrusted===true/);
  assert.match(inputCapture,/repeated-matching-input-before-event-association/);
  assert.match(inputCapture,/state\.inputCaptureArmed/);
  assert.doesNotMatch(source,/root\.addEventListener\(filter\.type,state\.inputListener,true\)|removeEventListener\(state\.inputFilter\.type/,
    'begin/arm/detach must not re-register the listener after the shell native-key handler');
  assert.ok(source.indexOf('root.addEventListener(config.inputFilter.type,captureInput,true)')<source.indexOf('root[config.probeGlobal]=api'),
    'the capture listener must be installed synchronously during probe init before shell page scripts');
  assert.doesNotMatch(source,/merge\.disconnect\(\)|voice\.merge\.disconnect\(\)/,
    'the production merge→destination connection must remain intact');
  const other=sfxLatencyProbeInitSource({...TH08_NORMAL_SFX_PROBE,game:'other',variant:'normal',probeGlobal:'__otherProbe',
    enabledGlobal:'__otherEnabled',traceGlobal:'__otherTrace'});
  assert.doesNotMatch(other,/__th08SfxLatency/,'shared observer source must not hardcode a TH08 probe name');
});
