import assert from 'node:assert/strict';
import test from 'node:test';
import vm from 'node:vm';
import {MUSIC_PCM_PROBE_SOURCE} from './music-browser-helpers.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {collectDesktopSfxWindow,DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE,mapDesktopSfxStartJournal,validateDesktopSfxHistoryReadiness,validateDesktopSfxInputWindow} from './runtime-desktop-audio.mjs';
import {correlateInputSfxWindow} from './runtime-desktop-readiness.mjs';
import {assertGameplaySfxEvidence} from '../../tests/adaptation/adapters/audio-none-evidence.mjs';

const consumerSnapshot=(sequence,fire,activePlayerShots)=>({available:true,complete:true,phase:'consumer-run',sequence,stage:2,
  consumed:{fire},activePlayerShots});
const consumerReceipt=()=>({baseline:consumerSnapshot(10,false,4),held:consumerSnapshot(11,true,5),released:consumerSnapshot(12,false,5)});
const readinessEvidence=(sessionId,contextIndex,sampleRate,tapIds,readyTime)=>{
  const readyFrame=Math.floor(readyTime*sampleRate),armTime=readyTime-.05,armFrame=Math.floor(armTime*sampleRate);
  const barrierBasis=tapIds.map(tapId=>({tapId,firstFrame:armFrame,lastProcessStartFrame:armFrame,
    lastProcessEndFrame:armFrame+128,contextTime:(armFrame+128)/sampleRate,contextFrame:armFrame+128})),
    barrierFrame=Math.max(...barrierBasis.map(row=>row.lastProcessEndFrame));
  return{available:true,status:'ready',reason:null,source:'shared-audioworklet-history-status',sessionId,contextIndex,sampleRate,
    arm:{state:'running',currentTime:armTime,contextFrame:armFrame},barrierFrame,barrierBasis,ready:{state:'running',currentTime:readyTime,contextFrame:readyFrame},
    elapsedMs:2,pollCount:1,checks:[{poll:1,elapsedMs:2,armFrame,readyFrame,barrierFrame,renderedTapCount:tapIds.length,
      barrierCoveredTapCount:tapIds.length,minRenderedThroughFrame:readyFrame,reason:null}],
    taps:tapIds.map((tapId,index)=>({tapId,historyHandleId:index+1,available:true,reason:null,firstFrame:readyFrame-128,
      renderedThroughFrame:readyFrame,processCount:1,lastProcessStartFrame:readyFrame-128,lastProcessEndFrame:readyFrame,
      currentFrame:readyFrame-128,gaps:0,gapDrops:0,overwrittenFrames:0,contextState:'running',contextIndex,sampleRate,
      contextTime:readyTime,contextFrame:readyFrame}))};
};

function createInjectedRuntime({stopMethod=true,renderAfterStatusRequests=1,advanceClockOnRender=true,initialClockGapFrames=0,
  advanceOnEveryStatusRequest=false,delayedStatusTapIndices=[],statusResponseDelayMs=0}={}){
  const blobs=new Map();let nextBlob=1;
  class Blob{constructor(parts){this.source=parts.join('');}}
  class AudioNode{
    constructor(context,kind='node'){this.context=context;this.kind=kind;this.connections=[];this.pcmLevel=.25;}
    connect(destination,...args){this.connections.push({destination,args});if(destination.kind==='analyser'||destination.kind==='worklet')destination.source=this;return destination;}
    disconnect(...args){if(args.length===0)this.connections=[];else if(Number.isInteger(args[0]))this.connections=this.connections.filter(edge=>(edge.args[0]??0)!==args[0]);
      else this.connections=this.connections.filter(edge=>edge.destination!==args[0]||(Number.isInteger(args[1])&&(edge.args[0]??0)!==args[1]));}
  }
  class AudioParam{
    constructor(value=1){this._value=value;}
    setValueAtTime(value){this._value=value;return this;}
    linearRampToValueAtTime(value){this._value=value;return this;}
    exponentialRampToValueAtTime(value){this._value=value;return this;}
    setTargetAtTime(value){this._value=value;return this;}
    setValueCurveAtTime(values){this._value=values.at(-1);return this;}
    cancelScheduledValues(){return this;}
    cancelAndHoldAtTime(){return this;}
  }
  Object.defineProperty(AudioParam.prototype,'value',{configurable:true,get(){return this._value;},set(value){this._value=value;}});
  class AudioScheduledSourceNode extends AudioNode{}
  if(stopMethod)AudioScheduledSourceNode.prototype.stop=function(...args){this.stopCall=args;if(this.stopError)throw this.stopError;return this.stopReturn;};
  class AudioBufferSourceNode extends AudioScheduledSourceNode{
    constructor(context){super(context,'buffer-source');this.buffer={sampleRate:48000,duration:.2,length:9600};this.playbackRate=new AudioParam(1);this.detune=new AudioParam(0);
      this.loop=false;this.startReturn={started:true};this.startError=null;this.lastCall=null;this.listeners=new Map();}
    start(...args){this.lastCall={receiver:this,args};if(this.startError)throw this.startError;return this.startReturn;}
    addEventListener(type,listener){const list=this.listeners.get(type)??[];list.push(listener);this.listeners.set(type,list);}
    emitEnded(){this.onended?.({type:'ended',target:this});for(const listener of this.listeners.get('ended')??[])listener({type:'ended',target:this});}
  }
  class AudioWorkletNode extends AudioNode{
    constructor(context,name,options){super(context,'worklet');this.name=name;this.options=options;this.tapIndex=context.workletNodes.length;context.workletNodes.push(this);
      const mainPort={onmessage:null,postMessage:data=>{if(data?.type==='status')context.onHistoryStatus();this.processor.port.onmessage?.({data});},close(){}};
      const workerPort={onmessage:null,postMessage:data=>{const deliver=()=>mainPort.onmessage?.({data});
        if(data?.type==='status'&&delayedStatusTapIndices.includes(this.tapIndex)&&statusResponseDelayMs>0){
          const ticker=setInterval(()=>context.advanceQuantum(),1);setTimeout(()=>{clearInterval(ticker);deliver();},statusResponseDelayMs);
        }else deliver();}};
      context.workletModuleState.nextPort={worker:workerPort};this.processor=new context.workletProcessor({processorOptions:options.processorOptions});context.workletModuleState.nextPort=null;
      this.port=mainPort;}
    render(startFrame,channels){this.context.workletModuleState.realm.currentFrame=startFrame;
      const frames=channels[0]?.length??128,output=[new Float32Array(frames)];this.processor.process([channels],[output]);return output[0];}
  }
  class AudioContext{
    constructor(){this.state='running';this.sampleRate=48000;this._currentTime=4.25;this.currentFrame=Math.floor(this._currentTime*this.sampleRate);
      this.destination={kind:'destination'};this.listeners=[];this.workletNodes=[];this.statusRequests=0;this.statusRenderPending=false;
      this.initialClockGapFrames=initialClockGapFrames;this.initialClockGapPending=initialClockGapFrames>0;
      Object.defineProperty(this,'currentTime',{configurable:true,get:()=>this._currentTime,set:value=>{this._currentTime=value;
        if(Number.isFinite(value))this.currentFrame=Math.floor(value*this.sampleRate);}});
      this.advanceQuantum=()=>{const frame=this.currentFrame,channels=[new Float32Array(128)];
        for(const node of this.workletNodes)node.render(frame,channels);
        if(advanceClockOnRender){this.currentFrame=frame+128;this._currentTime=this.currentFrame/this.sampleRate;}};
      this.onHistoryStatus=()=>{this.statusRequests++;if(this.statusRequests<renderAfterStatusRequests)return;
        if(this.initialClockGapPending){this.currentFrame+=this.initialClockGapFrames;this._currentTime=this.currentFrame/this.sampleRate;this.initialClockGapPending=false;return;}
        if(!advanceOnEveryStatusRequest){if(this.statusRenderPending)return;this.statusRenderPending=true;setTimeout(()=>{this.statusRenderPending=false;},0);}
        this.advanceQuantum();};
      this.audioWorklet={addModule:async url=>{const registry=new Map(),state={nextPort:null};
        const realm={Float32Array,Number,Math,currentFrame:null,registerProcessor:(name,ctor)=>registry.set(name,ctor),
          AudioWorkletProcessor:class{constructor(){this.port=state.nextPort.worker;}}};
        vm.runInNewContext(blobs.get(url),realm,{filename:'EaglerMusicPcmHistoryProcessor'});state.realm=realm;this.workletModuleState=state;
        this.workletProcessor=registry.get('eagler-music-pcm-history');}};}
    addEventListener(type,listener){if(type==='statechange')this.listeners.push(listener);}
    createAnalyser(){const analyser=new AudioNode(this,'analyser');analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
      analyser.getFloatTimeDomainData=samples=>samples.fill(analyser.source?.pcmLevel??0);return analyser;}
    createGain(){const gain=new AudioNode(this,'gain');gain.gain={value:1};return gain;}
    renderSource(source,startFrame,channels){for(const node of this.workletNodes)if(node.source?.source===source)node.render(startFrame,channels);}
  }
  const listeners=new Map(),window={AudioContext,AudioNode,AudioParam,AudioScheduledSourceNode,AudioBufferSourceNode,AudioWorkletNode,Blob,setTimeout,clearTimeout,
    URL:{createObjectURL:blob=>{const url='blob:'+nextBlob++;blobs.set(url,blob.source);return url;},revokeObjectURL:url=>blobs.delete(url)},top:{},parent:null};window.window=window;window.parent=window;
  window.addEventListener=(type,listener,capture=false)=>{const list=listeners.get(type)??[];list.push({listener,capture});listeners.set(type,list);};
  window.dispatch=(type,event)=>{let stopped=false;event.type=type;event.stopImmediatePropagation=()=>{stopped=true;};
    for(const row of listeners.get(type)??[]){row.listener(event);if(stopped)break;}};
  const sandbox={window,AudioContext,AudioNode,AudioParam,AudioScheduledSourceNode,AudioBufferSourceNode,AudioWorkletNode,Map,WeakMap,WeakSet,Float32Array,Array,Number,Math,String,Error,Reflect,Object};
  vm.runInNewContext(MUSIC_PCM_PROBE_SOURCE,sandbox,{filename:'MUSIC_PCM_PROBE_SOURCE'});
  vm.runInNewContext(DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE,sandbox,{filename:'DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE'});
  const context=new AudioContext(),merge=new AudioNode(context,'gain');merge.connect(context.destination);
  const source=new AudioBufferSourceNode(context),voice={playing:true,node:source,started:4.25,merge,
    buffer:{duration:.2,sampleRate:48000,length:9600,numberOfChannels:1},speed:1};source.buffer=voice.buffer;
  const heap=new ArrayBuffer(4096),heapU8=new Uint8Array(heap),tracePointer=4,diagnosticsPointer=512;
  const trace=new Uint32Array(heap,tracePointer,68),position=new Float32Array(heap,tracePointer+7*4,2),diagnostics=new Int32Array(heap,diagnosticsPointer,16);
  trace[3]=100;trace[2]=20;position[0]=200;position[1]=300;diagnostics[11]=0;
  const status=Array(32).fill(0);status[0]=2;status[1]=1;status[3]=1;
  const core={memory:{buffer:heap},status:()=>0,trace:()=>tracePointer,diagnostics:()=>diagnosticsPointer};
  core.status=(_app,index)=>status[index]??0;
  const module={HEAPU8:heapU8,SDL3:{audioContext:context},eaglerSfxOutput:{voices:new Map([['voice-7',voice]])}};
  const wrongRealmModule={SDL3:{audioContext:new AudioContext()},eaglerSfxOutput:{voices:new Map()}};
  window.Module=wrongRealmModule;
  window.__th08Runtime={Module:module,core,app:{}};
  window.__desktopAudioRuntime={Audio:module};
  window.__eaglerDesktopAudioRuntimeMount={game:'th08',variant:'normal',runtime:{global:'__desktopAudioRuntime',module:'Audio',core:'Core',app:'App'}};
  const reader=vm.runInNewContext(`(${readRuntimeObservation.toString()})`,{window},{filename:'readRuntimeObservation'});
  const setNativeInput=value=>{diagnostics[11]=value;};
  return{window,context,merge,source,voice,module,reader,setNativeInput};
}

function addRuntimeVoice(runtime,id,duration,speed=1){
  const merge=new runtime.window.AudioNode(runtime.context,'gain');merge.connect(runtime.context.destination);
  const buffer={duration,sampleRate:runtime.context.sampleRate,length:Math.ceil(duration*runtime.context.sampleRate),numberOfChannels:1};
  const source=new runtime.window.AudioBufferSourceNode(runtime.context);source.buffer=buffer;
  runtime.module.eaglerSfxOutput.voices.set(id,{playing:true,node:source,started:runtime.context.currentTime,merge,buffer,speed,volume:1});
  return{merge,source,buffer};
}

async function runRenderedSfxAction(runtime,{disconnectProductionOutput=false,volume=1}={}){
  const probe=runtime.window.__eaglerDesktopAudioProbe,inputStart=await probe.beginInputWindow('th08','KeyZ');
  assert.equal(inputStart.historyReady,true,inputStart.historyError);
  runtime.context.currentTime=4.3;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:10});
  runtime.voice.volume=volume;runtime.voice.started=4.45;runtime.context.currentTime=4.45;
  runtime.source.start(0,0);
  const outputFrames=new Float32Array(9600);if(volume!==0)outputFrames.fill(.4);
  runtime.context.renderSource(runtime.merge,Math.round(4.45*runtime.context.sampleRate),[outputFrames]);
  if(disconnectProductionOutput){runtime.context.currentTime=4.5;runtime.merge.disconnect(runtime.context.destination);}
  runtime.context.currentTime=4.7;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:20});
  const capture=await probe.endInputWindow(),end={...probe.clockFor('th08'),source:'SDL3.audioContext.currentTime'};
  const actionWindow={source:'SDL3.audioContext.currentTime',code:'KeyZ',game:'th08',sessionId:inputStart.sessionId,
    startContextTime:inputStart.clock.currentTime,endContextTime:end.currentTime,contextIndex:inputStart.clock.contextIndex,
    sampleRate:inputStart.clock.sampleRate,inputCapture:capture,historyReadiness:inputStart.historyReadiness,clockEvidence:{start:inputStart.clock,end},
    releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,nativeValue:0,
      contextIndex:end.contextIndex,currentTime:end.currentTime,consumer:consumerReceipt()}};
  const mapped=mapDesktopSfxStartJournal(capture,{activeObservations:[]});
  return{capture,actionWindow,mapped,result:correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {actionWindow,nativeSfx:{available:true,...mapped}})};
}

test('central no-BGM firing proof recomputes the actual start journal, rendered PCM and native shot creation',async()=>{
  const runtime=createInjectedRuntime(),r=await runRenderedSfxAction(runtime);
  const e={mode:'none',idle:{nativeSfx:{available:true,snapshots:{end:[]}}},
    firing:{actionWindow:r.actionWindow,nativeSfx:{available:true,...r.mapped,sampledActiveObservations:[]}}};
  assert.doesNotThrow(()=>assertGameplaySfxEvidence(e,{game:'th08'}));
  for(const mutate of [
    value=>value.firing.nativeSfx.activationEvents[0].sourceNodeIdentity.nodeId++,
    value=>value.firing.actionWindow.releaseEvidence.consumer.held.activePlayerShots=4,
    value=>value.firing.actionWindow.inputCapture.sfxStartJournal.events=[],
    value=>value.mode='ogg',
  ]){const invalid=JSON.parse(JSON.stringify(e));mutate(invalid);assert.throws(()=>assertGameplaySfxEvidence(invalid,{game:'th08'}));}
});

test('Desktop SFX adapter samples the shared parallel PCM tap and leaves the production direct edge intact',()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const sample=probe.capture('th08');
  assert.equal(sample.clock.source,'SDL3.audioContext');
  assert.equal(sample.clock.contextIndex,0);
  assert.equal(sample.captures.length,1);
  assert.equal(sample.captures[0].contextIndex,0);
  assert.equal(runtime.window.__eaglerDesktopAudioRuntimeMount.runtime.module,'Audio',
    'The fixture exercises an adapter-declared module property rather than a Module-global mount');
  assert.notEqual(runtime.window.Module,runtime.module,'A wrong-realm Module decoy cannot replace the declared Runtime mount');
  assert.equal(sample.captures[0].samples.length,2048);
  assert.equal(sample.captures[0].nonZero,2048);
  assert.equal(sample.captures[0].samples[0],.25,'the tap returns actual sampled PCM values');
  assert.equal(sample.voices.voices[0].outputTapIndex,0,'voice merge maps to the shared tap by source node identity');
  assert.equal(runtime.merge.connections.filter(edge=>edge.destination===runtime.context.destination).length,1,
    'the original production merge-to-destination edge stays connected');
  const analyser=runtime.merge.connections.find(edge=>edge.destination.kind==='analyser')?.destination;
  assert.ok(analyser);
  assert.equal(analyser.connections[0].destination.gain.value,0,'the observation branch remains muted');
  const observation=runtime.reader({game:'th08',includeAudioClock:true});
  assert.deepEqual(JSON.parse(JSON.stringify(observation.audioClock)),{
    available:true,source:'SDL3.audioContext.currentTime',contextIndex:0,state:'running',sampleRate:48000,currentTime:4.25});
});

test('SFX input arming waits for a real shared-Worklet render quantum covered by the SDL3 clock',async()=>{
  const runtime=createInjectedRuntime({renderAfterStatusRequests:2}),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ'),readiness=start.historyReadiness;
  assert.equal(start.historyReady,true,start.historyError);
  assert.equal(readiness.available,true);
  assert.equal(readiness.status,'ready');
  assert.equal(readiness.pollCount,2,'the first status has no processed block, so input arming polls again');
  assert.ok(readiness.ready.contextFrame>readiness.arm.contextFrame,'the SDL3 context clock advanced after history arming');
  assert.equal(readiness.taps.length,start.historyTaps.length);
  for(const tap of readiness.taps){
    assert.ok(tap.processCount>=1,'the actual injected processor processed at least one quantum');
    assert.ok(tap.lastProcessEndFrame>tap.firstFrame);
    assert.ok(tap.renderedThroughFrame>=readiness.ready.contextFrame,
      'the target tap rendered through the same SDL3 frame used as the ready boundary');
  }
  assert.deepEqual(validateDesktopSfxHistoryReadiness(readiness,{sessionId:start.sessionId,contextIndex:start.clock.contextIndex,
    sampleRate:start.clock.sampleRate,historyTapIds:start.historyTaps}),{valid:true,reason:null});
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  assert.ok(probe.startJournalSnapshot().inputEvents[0].contextTime>=readiness.ready.currentTime,
    'the trusted input edge follows the rendered-history boundary');
  const capture=await probe.endInputWindow();
  assert.deepEqual(capture.sfxStartJournal.historyReadiness,readiness,'the readiness receipt remains attached to the closed input session');
});

test('SFX readiness fixes one post-arm barrier across delayed tap replies and an initial render gap',async()=>{
  const runtime=createInjectedRuntime({initialClockGapFrames:512,advanceOnEveryStatusRequest:true,delayedStatusTapIndices:[1],statusResponseDelayMs:35});
  addRuntimeVoice(runtime,'voice-8',.2);
  const probe=runtime.window.__eaglerDesktopAudioProbe,start=await probe.beginInputWindow('th08','KeyZ'),readiness=start.historyReadiness;
  assert.equal(start.historyReady,true,start.historyError);
  assert.equal(readiness.taps.length,2);
  assert.ok(readiness.pollCount>=3,'the delayed tap leaves earlier per-tap receipts behind until a later poll');
  assert.ok(readiness.taps.some(tap=>tap.firstFrame>readiness.arm.contextFrame),
    'the first complete history quantum follows a real clock interval with no rendered history');
  assert.ok(readiness.barrierFrame>=Math.max(...readiness.barrierBasis.map(row=>row.firstFrame)),
    'the barrier is no earlier than the first available rendered frame');
  assert.ok(readiness.barrierFrame>readiness.arm.contextFrame,'the barrier is derived from a post-arm processed quantum');
  assert.ok(readiness.taps.every(tap=>tap.renderedThroughFrame>=readiness.barrierFrame));
  assert.ok(readiness.ready.contextFrame>=readiness.barrierFrame);
  assert.ok(readiness.checks.some(check=>check.readyFrame>check.minRenderedThroughFrame),
    'the SDL3 clock advances beyond a delayed tap status; readiness uses the fixed barrier instead of chasing that clock');
  assert.deepEqual(validateDesktopSfxHistoryReadiness(readiness,{sessionId:start.sessionId,contextIndex:0,sampleRate:48000,
    historyTapIds:start.historyTaps}),{valid:true,reason:null});
  const capture=await probe.endInputWindow();
  assert.deepEqual(capture.sfxStartJournal.historyReadiness,readiness);
});

test('stalled SFX history or SDL3 clock returns incomplete readiness instead of allowing input',async()=>{
  const stalledWorklet=createInjectedRuntime({renderAfterStatusRequests:Number.MAX_SAFE_INTEGER}),workletProbe=stalledWorklet.window.__eaglerDesktopAudioProbe;
  const noQuantum=await workletProbe.beginInputWindow('th08','KeyZ');
  assert.equal(noQuantum.historyReady,false);
  assert.match(noQuantum.historyReadiness.reason,/No AudioWorklet render quantum/);
  assert.equal(noQuantum.historyReadiness.taps[0].processCount,0);
  const noQuantumCapture=await workletProbe.endInputWindow();
  assert.equal(noQuantumCapture.sfxStartJournal.historyReady,false);
  assert.equal(noQuantumCapture.events.length,0,'the incomplete readiness branch sends no keyboard input');

  const stalledClock=createInjectedRuntime({advanceClockOnRender:false}),clockProbe=stalledClock.window.__eaglerDesktopAudioProbe;
  const noClock=await clockProbe.beginInputWindow('th08','KeyZ');
  assert.equal(noClock.historyReady,false);
  assert.match(noClock.historyReadiness.reason,/SDL3 audio clock has not advanced/);
  assert.ok(noClock.historyReadiness.taps[0].processCount>0,'Worklet processing alone does not qualify while the SDL3 clock is stalled');
  assert.equal(noClock.historyReadiness.ready,undefined);
  await clockProbe.endInputWindow();
});

test('the input capture listener is installed before Shell stopImmediatePropagation and records trusted edges',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const nativeSeen=[];
  runtime.window.addEventListener('keydown',event=>{nativeSeen.push(event.code);event.stopImmediatePropagation();},true);
  runtime.window.addEventListener('keyup',event=>{nativeSeen.push(event.code);event.stopImmediatePropagation();},true);
  const start=await probe.beginInputWindow('th08','KeyZ');
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:10});
  runtime.context.currentTime=4.3;
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:20});
  const capture=await probe.endInputWindow(),end={...runtime.reader({game:'th08',includeAudioClock:true}).audioClock,source:'SDL3.audioContext.currentTime'};
  assert.deepEqual(nativeSeen,['KeyZ','KeyZ'],'Shell listeners still receive and consume the real key events');
  const window={source:'SDL3.audioContext.currentTime',code:'KeyZ',startContextTime:start.clock.currentTime,endContextTime:end.currentTime,
    game:'th08',sessionId:start.sessionId,contextIndex:start.clock.contextIndex,sampleRate:start.clock.sampleRate,inputCapture:capture,
    historyReadiness:start.historyReadiness,clockEvidence:{start:start.clock,end},
    releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,nativeValue:0,contextIndex:end.contextIndex,currentTime:end.currentTime,
      consumer:consumerReceipt()}};
  assert.equal(validateDesktopSfxInputWindow(window).valid,true);
  const uncovered={...start.historyReadiness,taps:start.historyReadiness.taps.map((tap,index)=>index===0?{...tap,
    renderedThroughFrame:start.historyReadiness.ready.contextFrame-1,lastProcessEndFrame:start.historyReadiness.ready.contextFrame-1}:tap)};
  assert.equal(validateDesktopSfxHistoryReadiness(uncovered,{sessionId:start.sessionId,contextIndex:0,sampleRate:48000,
    historyTapIds:capture.historyTaps}).valid,false,'one tap ending before the sampled SDL3 frame is not ready');
  const earlyInput=structuredClone(window);earlyInput.inputCapture.events[0].contextTime=start.historyReadiness.ready.currentTime-1/48000;
  assert.equal(validateDesktopSfxInputWindow(earlyInput).valid,false,'a trusted key edge before the ready clock cannot use later rendered history');
  assert.deepEqual(JSON.parse(JSON.stringify(capture.events.map(event=>[event.type,event.isTrusted,event.contextTime]))),[
    ['keydown',true,start.historyReadiness.ready.currentTime],['keyup',true,4.3]]);
});

test('the injected tap, SDL3 reader, and input-clock classifier share one observed firing source',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const request=runtimeObservationRequest({game:'th08',variant:'normal'}),read=()=>runtime.reader({...request,includeAudioClock:true});
  const preKey=read();assert.equal(preKey.input.value,0);
  const inputStart=await probe.beginInputWindow('th08','KeyZ');
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:10});
  runtime.setNativeInput(1);const held=read();assert.equal(held.input.value,1);
  const voice=runtime.module.eaglerSfxOutput.voices.get('voice-7');
  voice.started=4.45;runtime.context.currentTime=4.45;
  const sourceStart=runtime.source.start(0,0);
  assert.equal(sourceStart,runtime.source.startReturn,'The native start return value is preserved');
  assert.equal(runtime.source.lastCall.receiver,runtime.source,'The native start receiver is preserved');
  const rendered=new Float32Array(9600);rendered.fill(.4);
  runtime.context.renderSource(runtime.merge,Math.round(4.45*runtime.context.sampleRate),[rendered]);
  runtime.context.currentTime=4.46;
  const sample=probe.capture('th08'),voiceRow=sample.voices.voices[0],tap=sample.captures.find(row=>row.tapIndex===voiceRow.outputTapIndex);
  const sampledNativeSfx={activeObservations:[{...voiceRow,captureSequence:0,observedAtContextTime:sample.clock.currentTime,
    contextSampleRate:sample.clock.sampleRate,pcm:{nonZero:tap.nonZero,peakAbs:tap.peakAbs,rms:tap.rms}}]};
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:20});runtime.setNativeInput(0);runtime.context.currentTime=4.7;
  const postKey=read(),inputCapture=await probe.endInputWindow(),endClock={...postKey.audioClock,source:'SDL3.audioContext.currentTime'};assert.equal(postKey.input.value,preKey.input.value);
  const actionWindow={source:'SDL3.audioContext.currentTime',code:'KeyZ',game:'th08',sessionId:inputStart.sessionId,startContextTime:inputStart.clock.currentTime,
    endContextTime:endClock.currentTime,contextIndex:inputStart.clock.contextIndex,sampleRate:inputStart.clock.sampleRate,
    inputCapture,historyReadiness:inputStart.historyReadiness,clockEvidence:{start:inputStart.clock,end:endClock},
    releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:preKey.input.value,consumedValue:held.input.value,
      nativeValue:postKey.input.value,contextIndex:endClock.contextIndex,currentTime:endClock.currentTime,consumer:consumerReceipt()}};
  const journalEvidence=mapDesktopSfxStartJournal(inputCapture,sampledNativeSfx);
  assert.equal(journalEvidence.available,true);
  assert.equal(journalEvidence.activationEvents[0].started,4.45,'The start call is timestamped from its SDL3 AudioContext');
  assert.equal(journalEvidence.activationEvents[0].sourceNodeIdentity.nodeId,voiceRow.sourceNodeIdentity.nodeId);
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {actionWindow,nativeSfx:{available:true,...journalEvidence}});
  assert.equal(result.status,'passed');
  assert.equal(result.correlatedPcm.nonZeroSamples,2048);
  assert.equal(result.newVoiceActivations[0].activations[0].voice.voiceStarted,4.45,
    'The immutable start journal preserves the WebSfx owner timestamp beside the actual start call clock');
  const staleCapture={...inputCapture,sfxStartJournal:{...inputCapture.sfxStartJournal,sessionId:inputCapture.sessionId+1}};
  assert.equal(mapDesktopSfxStartJournal(staleCapture,sampledNativeSfx).available,false,
    'Start rows from another input session cannot be reused for this action');
});

test('the firing PCM proof comes from the post-volume merge output and never the raw source buffer',async()=>{
  const muted=createInjectedRuntime();muted.source.pcmLevel=.8;
  const observed=await runRenderedSfxAction(muted,{volume:0});
  assert.equal(observed.mapped.available,true);
  assert.equal(observed.mapped.activationEvents[0].pcmHistory.nonZeroFrames,0,
    'the worklet captured the rendered voice.merge output after volume gain');
  assert.equal(observed.mapped.activationEvents[0].pcmHistory.peakAbs,0);
  assert.equal(observed.mapped.activationEvents[0].outputRoute.connected,true);
  assert.equal(observed.result.status,'failed','nonzero raw source data cannot compensate for zero post-gain output');
});

test('a live observation branch cannot pass after the production destination edge is disconnected',async()=>{
  const runtime=createInjectedRuntime();runtime.source.pcmLevel=.8;
  const observed=await runRenderedSfxAction(runtime,{disconnectProductionOutput:true});
  const activation=observed.mapped.activationEvents[0];
  assert.equal(activation.pcmHistory.covered,true);
  assert.ok(activation.pcmHistory.nonZeroFrames>0,'the existing analyser branch still sees its merge input');
  assert.equal(activation.outputRoute.connected,false,'route history tracks the production destination edge separately');
  assert.equal(observed.result.status,'failed','activity on the observer side branch is not production output');
  assert.match(observed.result.reason,/destination route/);
});

test('short-duration firing PCM cannot borrow a later same-tap source started after keyup',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ');
  runtime.context.currentTime=4.3;runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  runtime.voice.started=4.35;runtime.context.currentTime=4.35;runtime.source.start(0,0,.02);
  runtime.context.currentTime=4.38;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const laterSource=new runtime.window.AudioBufferSourceNode(runtime.context);laterSource.buffer=runtime.voice.buffer;
  runtime.voice.node=laterSource;runtime.voice.started=4.4;runtime.context.currentTime=4.4;laterSource.start(0,0);
  const rendered=new Float32Array(Math.round((4.7-4.25)*runtime.context.sampleRate));
  const laterStart=Math.round((4.4-4.25)*runtime.context.sampleRate),laterEnd=Math.round((4.6-4.25)*runtime.context.sampleRate);
  rendered.fill(.4,laterStart,laterEnd);
  runtime.context.renderSource(runtime.merge,Math.round(4.25*runtime.context.sampleRate),[rendered]);
  runtime.context.currentTime=4.7;
  const capture=await probe.endInputWindow(),mapped=mapDesktopSfxStartJournal(capture,{activeObservations:[]}),
    first=capture.sfxStartJournal.events[0],later=capture.sfxStartJournal.events[1],
    actionWindow={source:'SDL3.audioContext.currentTime',code:'KeyZ',game:'th08',sessionId:start.sessionId,
      startContextTime:start.clock.currentTime,endContextTime:4.7,contextIndex:start.clock.contextIndex,sampleRate:start.clock.sampleRate,
      inputCapture:capture,historyReadiness:start.historyReadiness,clockEvidence:{start:start.clock,end:{available:true,source:'SDL3.audioContext.currentTime',contextIndex:0,
        sampleRate:48000,currentTime:4.7,state:'running'}},releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,
        nativeValue:0,contextIndex:0,currentTime:4.7,consumer:consumerReceipt()}};
  assert.equal(first.sourceIntervalAvailable,true,first.sourceIntervalReason);
  assert.equal(first.endFrame-first.startFrame,Math.ceil(.02*runtime.context.sampleRate),
    'the third start argument bounds playback to its requested buffer-content duration');
  assert.ok(later.sessionSequence>capture.events.find(event=>event.type==='keyup').sessionSequence,
    'the nonzero source begins after the trusted input release');
  assert.equal(later.outputTapIndex,first.outputTapIndex,'both starts use the same production output tap');
  assert.ok(later.pcmHistory.nonZeroFrames>0,'the later source is audibly rendered on that tap');
  assert.equal(first.pcmHistory.nonZeroFrames,0,'the first source interval contains only its own zero output frames');
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {actionWindow,nativeSfx:{available:true,...mapped}});
  assert.equal(result.status,'failed','a short silent shot cannot pass by borrowing post-keyup samples from the same tap');
  assert.match(result.reason,/PCM remained zero/);
});

test('repeated successful stop calls use the latest scheduled source end and keep its complete PCM interval',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ');
  runtime.context.currentTime=4.3;runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  runtime.source.loop=true;runtime.voice.started=4.3;runtime.source.start(0,0);
  runtime.context.currentTime=4.4;runtime.source.stop(4.6);
  runtime.context.currentTime=4.5;runtime.source.stop(4.7);
  const rendered=new Float32Array(21600);rendered.fill(.3);
  runtime.context.renderSource(runtime.merge,Math.round(4.3*runtime.context.sampleRate),[rendered]);
  runtime.context.currentTime=4.72;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const capture=await probe.endInputWindow(),event=capture.sfxStartJournal.events[0];
  assert.equal(event.stopEventCount,2);
  assert.equal(event.stopDrops,0);
  assert.equal(event.endFrame,Math.round(4.7*runtime.context.sampleRate),
    'the later successful stop replaces the earlier scheduled end for the same source');
  assert.equal(event.pcmHistory.covered,true);
  assert.equal(event.pcmHistory.toFrame,event.endFrame);
  assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,true);
  assert.equal(start.sessionId,capture.sessionId);
});

test('stop observation wraps the inherited AudioScheduledSourceNode method and preserves return values and exceptions',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const owner=Object.getPrototypeOf(runtime.window.AudioBufferSourceNode.prototype);
  assert.equal(Object.hasOwn(runtime.window.AudioBufferSourceNode.prototype,'stop'),false,'the VM follows the browser API inheritance layout');
  assert.equal(typeof owner.stop,'function');
  await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.3;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});runtime.voice.started=4.3;runtime.source.start(0,0);
  runtime.context.currentTime=4.5;const stopReturn={nativeStop:true};runtime.source.stopReturn=stopReturn;
  assert.equal(runtime.source.stop(0),stopReturn,'the wrapper returns the native inherited-method result unchanged');
  const failure=new Error('native stop rejected');runtime.source.stopError=failure;
  assert.throws(()=>runtime.source.stop(0),error=>error===failure,'the original inherited-method exception is preserved');
  runtime.context.currentTime=4.6;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const capture=await probe.endInputWindow(),event=capture.sfxStartJournal.events[0];
  assert.equal(capture.sfxStartJournal.stopHookAvailable,true);
  assert.equal(event.stopEventCount,1,'only the successful native stop is journaled');
  assert.equal(event.endFrame,Math.round(4.5*runtime.context.sampleRate));
});

test('missing inherited stop observation is explicit incomplete evidence',async()=>{
  const runtime=createInjectedRuntime({stopMethod:false}),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ');
  assert.equal(start.historyReady,true,start.historyError);
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});runtime.source.start(0);
  runtime.context.currentTime=4.6;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const capture=await probe.endInputWindow();
  assert.equal(capture.sfxStartJournal.stopHookAvailable,false);
  assert.match(capture.sfxStartJournal.stopHookError,/AudioScheduledSourceNode\.stop/);
  assert.equal(capture.sfxStartJournal.available,false);
  assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,false);
});

test('input close frame rejects a source ending after close even when a later Worklet reply claims coverage',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.3;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});runtime.voice.started=4.3;runtime.source.start(0);
  runtime.context.currentTime=4.4;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const tap=probe.taps.find(item=>item.__eaglerMusicPcmSourceNode===runtime.merge),history=tap.__eaglerMusicPcmHistory;
  let lateQueries=0;const originalSnapshotBatch=history.snapshotBatch;
  history.snapshotBatch=async ranges=>{lateQueries++;return{available:true,results:ranges.map(range=>({available:true,...range,covered:true,
    renderedThroughFrame:Math.ceil(runtime.context.currentTime*runtime.context.sampleRate)+128,nonZeroFrames:range.toFrame-range.fromFrame,peakAbs:.25}))};};
  runtime.context.currentTime=4.41;
  const capture=await probe.endInputWindow(),event=capture.sfxStartJournal.events[0],close=capture.sfxStartJournal.closeBoundary;
  history.snapshotBatch=originalSnapshotBatch;
  assert.equal(close.available,true);
  assert.equal(event.intervalEndFrame,Math.round(4.5*runtime.context.sampleRate));
  assert.ok(event.intervalEndFrame>close.closeFrame);
  assert.equal(lateQueries,0,'an interval past the synchronous close frame is never submitted for async sampling');
  assert.equal(event.sourceIntervalAvailable,false);
  assert.match(event.sourceIntervalReason,/after the input-window close frame/);
  assert.equal(event.endFrame,null);
  assert.equal(event.pcmHistory.available,false);

  const normal=await runRenderedSfxAction(createInjectedRuntime());
  const baseCapture=normal.actionWindow.inputCapture,baseJournal=baseCapture.sfxStartJournal,baseEvent=baseJournal.events[0];
  const lateEvent={...baseEvent,endFrame:baseJournal.closeBoundary.closeFrame+128,intervalEndFrame:baseJournal.closeBoundary.closeFrame+128,
    sourceIntervalAvailable:true,sourceIntervalReason:null,pcmHistory:{available:true,covered:true,renderedThroughFrame:baseJournal.closeBoundary.closeFrame+128,
      fromFrame:baseEvent.startFrame,toFrame:baseJournal.closeBoundary.closeFrame+128,nonZeroFrames:1,peakAbs:.25}};
  const lateCapture={...baseCapture,sfxStartJournal:{...baseJournal,events:[lateEvent]}};
  const mapped=mapDesktopSfxStartJournal(lateCapture,{activeObservations:[]});
  assert.equal(mapped.available,false,'the mapper rejects a forged future-covered range');
  const lateAction={...normal.actionWindow,inputCapture:lateCapture};
  const verdict=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},{actionWindow:lateAction,nativeSfx:{available:true,...mapped}});
  assert.equal(verdict.status,'incomplete','the consumer classifier cannot pass a Worklet range beyond session close');
  assert.match(verdict.reason,/Native SFX voice observation is unavailable/);
  const missingBoundaryCapture={...baseCapture,sfxStartJournal:{...baseJournal,closeBoundary:null}};
  assert.equal(mapDesktopSfxStartJournal(missingBoundaryCapture,{activeObservations:[]}).available,false,
    'a missing close-frame identity cannot be mapped as complete PCM evidence');
  const missingBoundaryVerdict=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {actionWindow:{...normal.actionWindow,inputCapture:missingBoundaryCapture},nativeSfx:{available:false}});
  assert.equal(missingBoundaryVerdict.status,'incomplete');
});

test('dropped source-stop history makes the start journal incomplete',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  runtime.source.loop=true;runtime.source.start(0,0);
  for(let index=0;index<9;index++){
    runtime.context.currentTime=4.3+index/100;runtime.source.stop(4.4+index/100);
  }
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const capture=await probe.endInputWindow();
  assert.equal(capture.sfxStartJournal.events[0].stopDrops,1);
  assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,false,
    'a bounded stop-journal overflow cannot silently yield a partial PCM interval');
});

test('start journaling records only successful target-pool starts and preserves throws',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ'),input=runtime.source;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:10});
  assert.equal(input.start(0,.125),input.startReturn);
  const foreignContext=new runtime.window.AudioContext(),foreign=new runtime.window.AudioBufferSourceNode(foreignContext);
  assert.equal(foreign.start(0),foreign.startReturn,'Non-Runtime contexts keep their native behavior and are ignored');
  const failure=new Error('native start rejected');input.startError=failure;
  assert.throws(()=>input.start(0),error=>error===failure,'The original exception is rethrown unchanged');
  const capture=await probe.endInputWindow(),journal=capture.sfxStartJournal;
  assert.equal(journal.sessionId,start.sessionId);
  assert.equal(journal.events.length,1,'A failed source start is not recorded as an activation');
  assert.equal(journal.events[0].voiceId,'voice-7');
  assert.equal(journal.events[0].outputTapIndex,0);
  assert.equal(journal.events[0].tapMatchCount,1);
  assert.equal(journal.events[0].offset,.125);
  assert.equal(Object.isFrozen(journal),true,'The exported journal snapshot is immutable');
  assert.equal(Object.isFrozen(journal.events),true,'The exported start rows cannot be appended or replaced');
  assert.equal(Object.isFrozen(journal.events[0]),true,'Journal rows are immutable after capture');
  assert.equal(Object.isFrozen(journal.events[0].sourceNodeIdentity),true,'Nested source identity is immutable too');
  assert.equal(Object.isFrozen(journal.events[0].outputSourceIdentity),true,'Nested output identity is immutable too');
  assert.equal(journal.errors,0);
});

test('source intervals use buffer-content duration and the combined playbackRate/detune rate',async()=>{
  for(const {playbackRate,detune,expectedRate} of [
    {playbackRate:.5,detune:0,expectedRate:.5},
    {playbackRate:.5,detune:1200,expectedRate:1},
  ]){
    const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe,source=runtime.source;
    source.buffer={sampleRate:44100,duration:.5,length:22050};runtime.voice.buffer=source.buffer;
    source.playbackRate.value=playbackRate;source.detune.value=detune;
    await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.3;
    runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
    source.start(0,.1,.25);
    runtime.context.currentTime=4.3+.25/expectedRate;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
    const event=(await probe.endInputWindow()).sfxStartJournal.events[0];
    const expectedFrames=Math.ceil(.25/expectedRate*runtime.context.sampleRate);
    assert.equal(event.sourceIntervalAvailable,true,event.sourceIntervalReason);
    assert.equal(event.startContextTime,4.3);
    assert.equal(event.offset,.1);
    assert.equal(event.requestedDuration,.25,'duration is retained as buffer-content seconds');
    assert.equal(event.contentDuration,.25);
    assert.equal(event.computedPlaybackRate,expectedRate);
    assert.equal(event.endFrame,event.startFrame+expectedFrames,
      'the interval uses duration content seconds divided by playbackRate times detune ratio');
  }
});

test('past positive start times use the current AudioContext frame',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.5;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  runtime.source.start(4.4,0);
  runtime.context.currentTime=4.7;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const event=(await probe.endInputWindow()).sfxStartJournal.events[0];
  assert.equal(event.startContextTime,4.5,'a past positive when is treated as immediate playback');
  assert.equal(event.startFrame,Math.round(4.5*runtime.context.sampleRate));
});

test('future start times retain the call clock separately from the scheduled source boundary',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.5;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  runtime.source.start(4.6,0,.02);
  runtime.context.currentTime=4.7;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const capture=await probe.endInputWindow(),event=capture.sfxStartJournal.events[0];
  assert.equal(event.startCallContextTime,4.5);
  assert.equal(event.startContextTime,4.6);
  assert.equal(event.observedAtContextTime,4.5);
  assert.equal(event.sourceIntervalAvailable,true,event.sourceIntervalReason);
  assert.equal(event.endFrame,Math.round(4.6*runtime.context.sampleRate)+Math.ceil(.02*runtime.context.sampleRate));
  assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,true,
    'the source schedule can be later than its successful API call without corrupting clock ordering');
});

test('dynamic AudioParam automation or direct writes make a source interval incomplete',async()=>{
  for(const mutate of [parameter=>parameter.setValueAtTime(2,4.4),parameter=>{parameter.value=2;}]){
    const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
    await probe.beginInputWindow('th08','KeyZ');runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
    runtime.source.start(0,0,.02);mutate(runtime.source.playbackRate);
    runtime.context.currentTime=4.5;runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
    const capture=await probe.endInputWindow(),event=capture.sfxStartJournal.events[0];
    assert.equal(event.sourceIntervalAvailable,false,'a changing source speed cannot produce an inferred end frame');
    assert.equal(event.endFrame,null);
    assert.match(event.sourceIntervalReason,/changed after start/);
    assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,false);
  }
});

test('ended observation preserves the game handler and later stop cannot extend the completed interval',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe,source=runtime.source;
  await probe.beginInputWindow('th08','KeyZ');runtime.context.currentTime=4.3;
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  source.onended=()=>{source.gameEndedCalls=(source.gameEndedCalls??0)+1;};
  const originalHandler=source.onended;source.start(0,0,.02);
  const predictedEnd=Math.round(4.3*runtime.context.sampleRate)+Math.ceil(.02*runtime.context.sampleRate);
  source.emitEnded();runtime.context.currentTime=4.8;source.stop(4.9);
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const event=(await probe.endInputWindow()).sfxStartJournal.events[0];
  assert.equal(source.onended,originalHandler,'the probe does not replace the game onended callback');
  assert.equal(source.gameEndedCalls,1);
  assert.equal(event.ended,true,'the added ended listener records the actual lifecycle edge');
  assert.equal(event.endFrame,predictedEnd,'a stop after ended cannot move the finished source boundary');
});

test('missing tap identity, ambiguous pool ownership, and journal overflow are incomplete evidence',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  const voice=runtime.module.eaglerSfxOutput.voices.get('voice-7'),source=runtime.source;
  runtime.module.eaglerSfxOutput.voices.set('duplicate-owner',{...voice});
  source.start(0);
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const ambiguousCapture=await probe.endInputWindow(),ambiguous=ambiguousCapture.sfxStartJournal;
  assert.equal(ambiguous.unattributed,1,'One actual source owned by two runtime voices is recorded as un-attributable');
  assert.equal(mapDesktopSfxStartJournal(ambiguousCapture,{activeObservations:[]}).available,false);

  const missingTap=createInjectedRuntime(),missingProbe=missingTap.window.__eaglerDesktopAudioProbe;
  await missingProbe.beginInputWindow('th08','KeyZ');missingTap.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  const missingVoice=missingTap.module.eaglerSfxOutput.voices.get('voice-7');
  missingVoice.merge=new missingTap.window.AudioNode(missingTap.context,'gain');missingVoice.node=missingTap.source;
  missingTap.source.start(0);
  missingTap.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const noTapCapture=await missingProbe.endInputWindow(),noTap=noTapCapture.sfxStartJournal;
  assert.equal(noTap.events[0].tapMatchCount,0);
  assert.equal(mapDesktopSfxStartJournal(noTapCapture,{activeObservations:[]}).available,false);

  const overflow=createInjectedRuntime(),overflowProbe=overflow.window.__eaglerDesktopAudioProbe;
  await overflowProbe.beginInputWindow('th08','KeyZ');overflow.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
  const overflowVoice=overflow.module.eaglerSfxOutput.voices.get('voice-7');
  for(let i=0;i<65;i++){const next=new overflow.window.AudioBufferSourceNode(overflow.context);overflowVoice.node=next;next.start(0);}
  overflow.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});
  const overflowCapture=await overflowProbe.endInputWindow(),overflowJournal=overflowCapture.sfxStartJournal;
  assert.equal(overflowJournal.events.length,64);
  assert.equal(overflowJournal.drops,1,'The bounded journal reports every dropped start');
  assert.equal(mapDesktopSfxStartJournal(overflowCapture,{activeObservations:[]}).available,false);
});

test('declared non-Module mount is strict and missing mount never falls back to a same-name realm object',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  runtime.window.__eaglerDesktopAudioRuntimeMount.runtime.global='__missingDesktopAudioRuntime';
  const start=await probe.beginInputWindow('th08','KeyZ');
  assert.equal(start.clock.available,false,'The declared mount is the sole owner lookup path when a mapping is injected');
  runtime.source.start(0);
  const capture=await probe.endInputWindow();
  assert.equal(capture.sfxStartJournal.available,false,'The unmapped context invalidates this session journal');
  assert.ok(capture.sfxStartJournal.errors>0);
  assert.equal(capture.sfxStartJournal.events.length,0,'Neither window.Module nor __th08Runtime is accepted as an undeclared fallback');
  assert.equal(mapDesktopSfxStartJournal(capture,{activeObservations:[]}).available,false);
});

test('untrusted or duplicated key edges make the action clock evidence incomplete',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  const start=await probe.beginInputWindow('th08','KeyZ');
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:false,repeat:false,timeStamp:10});
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:20});
  const capture=await probe.endInputWindow(),end={...runtime.reader({game:'th08',includeAudioClock:true}).audioClock,source:'SDL3.audioContext.currentTime'};
  const window={source:'SDL3.audioContext.currentTime',code:'KeyZ',startContextTime:start.clock.currentTime,endContextTime:end.currentTime,
    game:'th08',sessionId:start.sessionId,contextIndex:0,sampleRate:48000,inputCapture:capture,historyReadiness:start.historyReadiness,clockEvidence:{start:start.clock,end},
    releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,nativeValue:0,contextIndex:end.contextIndex,currentTime:end.currentTime,
      consumer:consumerReceipt()}};
  assert.equal(validateDesktopSfxInputWindow(window).valid,false);

  const duplicate=await probe.beginInputWindow('th08','KeyZ');
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:30});
  runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:true,timeStamp:31});
  runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:40});
  const repeated=await probe.endInputWindow();
  const duplicateWindow={source:'SDL3.audioContext.currentTime',code:'KeyZ',startContextTime:duplicate.clock.currentTime,endContextTime:4.25,
    game:'th08',sessionId:duplicate.sessionId,contextIndex:0,sampleRate:48000,inputCapture:repeated,historyReadiness:duplicate.historyReadiness,clockEvidence:{start:duplicate.clock,end},
    releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,nativeValue:0,contextIndex:end.contextIndex,currentTime:4.25,
      consumer:consumerReceipt()}};
  assert.equal(validateDesktopSfxInputWindow(duplicateWindow).valid,false,'an unisolated repeat is explicit incomplete evidence');
});

test('SFX history preflights the real 46-voice shape and allocates duration plus two seconds within the fixed budget',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  runtime.voice.buffer={duration:.161,sampleRate:48000,length:7728,numberOfChannels:1};runtime.source.buffer=runtime.voice.buffer;
  const buffers=[runtime.voice.buffer];
  for(let index=1;index<46;index++)buffers.push(addRuntimeVoice(runtime,'synthetic-voice-'+index,1.033).buffer);
  const expectedBytes=buffers.reduce((total,buffer)=>total+Math.ceil((buffer.duration+2)*runtime.context.sampleRate)*4,0);
  const started=await probe.beginInputWindow('th08','KeyZ');
  assert.equal(started.historyReady,true,started.historyError);
  assert.equal(started.historyTaps.length,46);
  assert.equal(started.historyCapacityBytes,expectedBytes);
  assert.ok(expectedBytes<=32*1024*1024);
  assert.equal(runtime.context.workletNodes.length,46);
  await probe.endInputWindow();
});

test('SFX history aggregate and per-tap limits reject before allocating any Worklet history',async()=>{
  const aggregate=createInjectedRuntime(),aggregateProbe=aggregate.window.__eaglerDesktopAudioProbe;
  aggregate.voice.buffer={duration:2.5,sampleRate:48000,length:120000,numberOfChannels:1};aggregate.source.buffer=aggregate.voice.buffer;
  for(let index=1;index<40;index++)addRuntimeVoice(aggregate,'aggregate-voice-'+index,2.5);
  const aggregateStart=await aggregateProbe.beginInputWindow('th08','KeyZ');
  assert.equal(aggregateStart.historyReady,false);
  assert.match(aggregateStart.historyError,/voices=40 requiredBytes=34560000 aggregateLimitBytes=33554432 perTapLimitFrames=1048576/);
  assert.equal(aggregate.context.workletNodes.length,0,'the aggregate budget is checked before the first history allocation');
  assert.equal(aggregate.merge.__eaglerMusicPcmHistory,undefined);
  assert.equal(aggregateProbe.startJournalSnapshot().historyReady,false);
  await aggregateProbe.endInputWindow();

  const single=createInjectedRuntime(),singleProbe=single.window.__eaglerDesktopAudioProbe;
  single.voice.buffer={duration:20,sampleRate:48000,length:960000,numberOfChannels:1};single.source.buffer=single.voice.buffer;
  const singleStart=await singleProbe.beginInputWindow('th08','KeyZ');
  assert.equal(singleStart.historyReady,false);
  assert.match(singleStart.historyError,/voices=1 requiredBytes=4224000 aggregateLimitBytes=33554432 perTapLimitFrames=1048576 perTapLimitExceeded=true/);
  assert.equal(single.context.workletNodes.length,0,'the per-tap limit is checked before history allocation');
  await singleProbe.endInputWindow();
});

test('desktop SFX collection keeps the journal open until delayed PCM sampling completes and always closes on failure',async()=>{
  const runtime=createInjectedRuntime(),probe=runtime.window.__eaglerDesktopAudioProbe;
  await probe.beginInputWindow('th08','KeyZ');
  const order=[];
  const collected=await collectDesktopSfxWindow({
    capture:()=>{order.push('capture-start');return new Promise(resolve=>setTimeout(()=>{order.push('capture-end');resolve({sampled:true});},20));},
    operation:async()=>{order.push('operation-start');runtime.window.dispatch('keydown',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:1});
      await new Promise(resolve=>setTimeout(resolve,2));runtime.window.dispatch('keyup',{code:'KeyZ',isTrusted:true,repeat:false,timeStamp:2});runtime.source.start(0);},
    finish:()=>{order.push('finish');return probe.endInputWindow();},
  });
  assert.deepEqual(order,['capture-start','operation-start','capture-end','finish']);
  assert.deepEqual(collected.firing,{sampled:true});
  assert.equal(collected.inputCapture.events.length,2);
  assert.equal(collected.inputCapture.sfxStartJournal.events.length,1,'the start after key release is still journaled before session close');
  assert.equal(probe.startJournalSnapshot().sessionId,null,'finish closes the session');

  await probe.beginInputWindow('th08','KeyZ');
  await assert.rejects(collectDesktopSfxWindow({capture:()=>Promise.reject(new Error('sample failed')),operation:async()=>{},
    finish:()=>probe.endInputWindow()}),/sample failed/);
  assert.equal(probe.startJournalSnapshot().sessionId,null,'a failed sample still runs the close step');

  const operationError=new Error('operation failed'),finishError=new Error('finish failed');
  await assert.rejects(collectDesktopSfxWindow({capture:async()=>({}),operation:async()=>{throw operationError;},finish:async()=>{throw finishError;}}),error=>{
    assert.ok(error instanceof AggregateError);assert.deepEqual(error.errors,[operationError,finishError]);return true;
  });
});
