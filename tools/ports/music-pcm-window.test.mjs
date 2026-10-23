import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {MUSIC_PCM_PROBE_SOURCE,MUSIC_PCM_HISTORY_PROCESSOR_SOURCE,readNativeAudioState,collectRuntimePcmWindow,collectRuntimeOutputPcm,collectExpectedTrackRuntimePcm,
  summarizeRelatedRuntimePcmWindow,TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE,TH08_MIDI_VOICE_PCM_ROLE}
  from './music-browser-helpers.mjs';
import {th08OggRestartOutcome} from './music-browser-result.mjs';

function createAudioRealm({parent=null,top=null,topLevel=false}={}){
  class AudioNode{
    constructor(context,kind='node'){this.context=context;this.kind=kind;this.connections=[];}
    connect(destination,...args){
      if(this.failAnalyser&&destination.kind==='analyser')throw Error('Audio observation branch reconnect failed');
      if(!this.connections.some(edge=>edge.destination===destination&&JSON.stringify(edge.args)===JSON.stringify(args)))
        this.connections.push({destination,args});
      if(destination.kind==='analyser')destination.pcmSource=this;return destination;
    }
    disconnect(...args){if(args.length===0)this.connections=[];else if(Number.isInteger(args[0]))this.connections=this.connections.filter(edge=>(edge.args[0]??0)!==args[0]);
      else this.connections=this.connections.filter(edge=>edge.destination!==args[0]||(Number.isInteger(args[1])&&(edge.args[0]??0)!==args[1]));}
  }
  class AudioContext{
    constructor({sampleRate=48000,level=0}={}){
      this.sampleRate=sampleRate;this.currentTime=0;this.state='running';this.level=level;this.destination={};this.listeners=new Map();
    }
    addEventListener(type,listener){const list=this.listeners.get(type)??[];list.push(listener);this.listeners.set(type,list);}
    transition(state){this.state=state;for(const listener of this.listeners.get('statechange')??[])listener({type:'statechange'});}
    advance(milliseconds){if(this.state==='running')this.currentTime+=milliseconds/1000;}
    createAnalyser(){
      const analyser=new AudioNode(this,'analyser');analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
      analyser.getFloatTimeDomainData=samples=>samples.fill(analyser.pcmSource?.connections.some(edge=>edge.destination===analyser)
        ?analyser.pcmSource.pcmLevel??this.level:0);return analyser;
    }
    createGain(){const gain=new AudioNode(this,'gain');gain.gain={value:1};return gain;}
  }
  const window={AudioContext,AudioNode};
  window.top=top??(topLevel?window:{});window.parent=parent??window;
  if(topLevel)window.__eaglerMusicPcmProbeTop=true;
  const sandbox={window,AudioContext,AudioNode};
  vm.runInNewContext(MUSIC_PCM_PROBE_SOURCE,sandbox,{filename:'MUSIC_PCM_PROBE_SOURCE'});
  return {window,AudioContext,AudioNode,sandbox};
}

function installOutput(realm,context){
  const source=new realm.AudioNode(context);
  source.connect(context.destination);
  return source;
}

function installNativeReader(realm){
  return vm.runInNewContext(`(${readNativeAudioState.toString()})`,{window:realm.window},{filename:'readNativeAudioState'});
}

function installTh08Module(realm,context){
  const module={SDL3:{audioContext:context}};
  realm.window.Module=module;
  realm.window.__th08Runtime={Module:module};
}

test('actual Runtime browser probe and window collector isolate the SDL3 mixer context',async()=>{
  const frame=createAudioRealm({top:{}}),reader=installNativeReader(frame);
  const mixer=new frame.AudioContext({level:0}),unrelated=new frame.AudioContext({level:.25});
  installNativeReader(frame);installOutput(frame,mixer);installOutput(frame,unrelated);installTh08Module(frame,mixer);
  const initial=reader({game:'th08',includeRuntimePcm:true}).runtimePcm;
  assert.equal(initial.target.role,TH08_OGG_PCM_ROLE);
  assert.equal(initial.target.available,true);
  assert.equal(initial.target.contextIndex,0);
  assert.deepEqual(Array.from(initial.target.tapIds),[0]);
  assert.equal(initial.captures.length,2,'the raw probe sees another context so the collector must select by identity');

  const result=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:initial.target,windowCount:8,settleFrames:4096,
    readWindow:async()=>reader({game:'th08',includeRuntimePcm:true}).runtimePcm,
    wait:async milliseconds=>{mixer.advance(milliseconds);unrelated.advance(milliseconds);}});
  assert.equal(result.status,'complete',result.reason);
  assert.equal(result.proof,'runtime-output-pcm-window');
  assert.equal(result.pcmWindow.windowCount,8);
  assert.equal(result.pcmWindow.contextIndices.length,1);
  assert.deepEqual(Array.from(result.pcmWindow.tapIds),[0]);
  assert.equal(result.pcmWindow.nonZeroSamples,0,'unrelated nonzero PCM must not contaminate the SDL3 mixer observation');
  assert.equal(result.pcmWindow.outputObserved,false);
  assert.ok(result.pcmWindow.elapsedFrames>=4096);
  assert.ok(result.pcmWindow.windows.every(window=>window.target.state==='running'&&window.target.sampleRate===48000));
  const direct=installOutput(frame,mixer);
  assert.equal(direct.connections[0].destination,mixer.destination,'the original destination route remains direct');
  const analyser=direct.connections.find(connection=>connection.destination.kind==='analyser')?.destination;
  assert.ok(analyser,'the PCM tap branches from the original source');
  assert.equal(analyser.connections[0].destination.kind,'gain');
  assert.equal(analyser.connections[0].destination.gain.value,0,'the read-only tap branch is muted');
  assert.equal(analyser.connections[0].destination.connections[0].destination,mixer.destination);
});

test('the production history processor measures rendered post-output input frames and reports bounded gaps by interval',()=>{
  const replies=[];let currentFrame=100,Processor=null;
  const workerPort={onmessage:null,postMessage:message=>replies.push(message)};
  const realm={currentFrame,Float32Array,Number,Math,registerProcessor:(name,ctor)=>{if(name==='eagler-music-pcm-history')Processor=ctor;},
    AudioWorkletProcessor:class{constructor(){this.port=workerPort;}}};
  vm.runInNewContext(MUSIC_PCM_HISTORY_PROCESSOR_SOURCE,realm,{filename:'MUSIC_PCM_HISTORY_PROCESSOR_SOURCE'});
  const processor=new Processor({processorOptions:{capacityFrames:512}});
  const feed=(startFrame,channels)=>{realm.currentFrame=startFrame;const output=[new Float32Array(channels[0].length)];
    assert.equal(processor.process([channels],[output]),true);assert.ok(output[0].every(value=>value===0),'the history observer emits silence');return output[0];};
  const block0=new Float32Array(128),block1=new Float32Array(128);block1[17]=.125;block1[99]=-.5;
  feed(100,[block0]);feed(228,[block1]);
  processor.port.onmessage({data:{type:'snapshot',requestId:1,fromFrame:100,toFrame:356}});
  const measured=replies.at(-1);assert.equal(measured.covered,true);assert.equal(measured.frames,256);
  assert.equal(measured.nonZeroFrames,2);assert.equal(measured.firstNonZeroFrame,245);assert.equal(measured.peakAbs,.5);
  for(let frame=356;frame<740;frame+=128)feed(frame,[new Float32Array(128)]);
  processor.port.onmessage({data:{type:'snapshot',requestId:2,fromFrame:100,toFrame:356}});
  assert.equal(replies.at(-1).covered,false,'old samples that left the bounded ring cannot be declared silent');
  feed(900,[new Float32Array(128)]);
  processor.port.onmessage({data:{type:'snapshot',requestId:3,fromFrame:612,toFrame:740}});
  assert.equal(replies.at(-1).covered,true,'a recorded gap after the source interval does not invalidate earlier continuous PCM');
  assert.deepEqual(JSON.parse(JSON.stringify(replies.at(-1).gapRanges)),[{fromFrame:740,toFrame:900}]);
  processor.port.onmessage({data:{type:'snapshot',requestId:4,fromFrame:820,toFrame:948}});
  assert.equal(replies.at(-1).covered,false,'a render-frame gap intersecting the requested interval invalidates coverage');
  assert.equal(replies.at(-1).coverageReason,'render-frame-gap-overlaps-range');
  assert.deepEqual(JSON.parse(JSON.stringify(replies.at(-1).overlappingGapRanges)),[{fromFrame:740,toFrame:900}]);
  assert.equal(replies.at(-1).gaps,1);
  processor.port.onmessage({data:{type:'snapshot',requestId:5,fromFrame:100,toFrame:100}});
  assert.equal(replies.at(-1).available,false,'a zero-frame query is unavailable, never a silent PCM interval');
  assert.equal(replies.at(-1).reason,'malformed-history-range');
});

test('the production history processor makes gap-index overflow explicit even for a recent range',()=>{
  const replies=[];let currentFrame=0,Processor=null;
  const port={onmessage:null,postMessage:message=>replies.push(message)};
  const realm={Float32Array,Number,Math,registerProcessor:(name,ctor)=>{if(name==='eagler-music-pcm-history')Processor=ctor;},
    AudioWorkletProcessor:class{constructor(){this.port=port;}}};
  vm.runInNewContext(MUSIC_PCM_HISTORY_PROCESSOR_SOURCE,realm,{filename:'MUSIC_PCM_HISTORY_PROCESSOR_SOURCE'});
  const processor=new Processor({processorOptions:{capacityFrames:32768}});
  const feed=startFrame=>{currentFrame=startFrame;realm.currentFrame=startFrame;
    processor.process([[new Float32Array(128)]],[[new Float32Array(128)]]);};
  feed(0);
  for(let index=1;index<=65;index++)feed(index*256);
  processor.port.onmessage({data:{type:'snapshot',requestId:1,fromFrame:65*256,toFrame:65*256+128}});
  const result=replies.at(-1);
  assert.equal(result.available,true);
  assert.equal(result.covered,false,'a query cannot claim complete history after the bounded gap index overflowed');
  assert.equal(result.coverageReason,'gap-index-overflow');
  assert.equal(result.gapRangeLimit,64);
  assert.equal(result.gapRanges.length,64);
  assert.equal(result.gapDrops,1);
  assert.equal(result.gaps,65);
});

test('the actual PCM probe restores its existing tap after production disconnect and reconnect',()=>{
  const realm=createAudioRealm({top:{}}),reader=installNativeReader(realm),context=new realm.AudioContext({level:.25});
  const output=installOutput(realm,context);installTh08Module(realm,context);
  const read=()=>reader({game:'th08',includeRuntimePcm:true}).runtimePcm;
  const before=read();assert.ok(before.captures[0].nonZero>0);
  output.disconnect();
  assert.equal(output.connections.length,0,'the probe must preserve the production disconnect');
  assert.deepEqual(realm.window.__eaglerMusicPcmProbe.outputRouteSnapshot(output).connected,false);
  assert.equal(read().captures[0].nonZero,0,'the fixture models an analyser whose source edge was removed');
  output.connect(context.destination);
  const after=read();assert.ok(after.captures[0].nonZero>0,'PCM must recover with the actual production output');
  assert.deepEqual(Array.from(after.target.tapIds),Array.from(before.target.tapIds),'recovery retains the same tap identity');
  output.connect(context.destination);
  assert.equal(realm.window.__eaglerMusicPcmProbe.taps.length,1,'reconnection must not allocate duplicate taps');
  assert.equal(output.connections.filter(edge=>edge.destination===context.destination).length,1);
  assert.equal(output.connections.filter(edge=>edge.destination.kind==='analyser').length,1);
  const destinationOnly=createAudioRealm({top:{}}),destinationOnlyContext=new destinationOnly.AudioContext({level:.25});
  const merge=installOutput(destinationOnly,destinationOnlyContext),analyserTap=merge.connections.find(edge=>edge.destination.kind==='analyser').destination;
  destinationOnlyContext.currentTime=10/destinationOnlyContext.sampleRate;
  merge.disconnect(destinationOnlyContext.destination);
  assert.equal(merge.connections.some(edge=>edge.destination===destinationOnlyContext.destination),false,
    'the fixture disconnects only the production destination edge');
  assert.equal(merge.connections.some(edge=>edge.destination===analyserTap),true,
    'the existing observation branch remains downstream of the output source');
  const disconnectedRoute=destinationOnly.window.__eaglerMusicPcmProbe.outputRouteRange(merge,0,128);
  assert.equal(disconnectedRoute.available,true);assert.equal(disconnectedRoute.connected,false,
    'analyser activity cannot stand in for the production destination edge');
  output.disconnect();output.failAnalyser=true;
  output.connect(context.destination);
  const broken=read();assert.equal(broken.target.available,false,'a failed observation branch cannot prove output silence');
  assert.match(broken.target.reason,/observation branch reconnect failed/);
});

test('disconnect output selectors only change the recorded production route and same-frame transitions are incomplete',()=>{
  const realm=createAudioRealm({top:{}}),context=new realm.AudioContext(),output=installOutput(realm,context),
    probe=realm.window.__eaglerMusicPcmProbe;
  const tap=output.connections.find(edge=>edge.destination.kind==='analyser').destination;
  output.disconnect(1);
  assert.equal(output.connections.some(edge=>edge.destination===context.destination),true,
    'a selector for a different output leaves the recorded output edge connected');
  assert.equal(output.connections.some(edge=>edge.destination===tap),true);
  assert.equal(probe.outputRouteSnapshot(output).connected,true);
  output.disconnect(context.destination,1);
  assert.equal(probe.outputRouteSnapshot(output).connected,true,
    'a destination disconnect for a different output index does not mark this output disconnected');
  output.disconnect(context.destination,0);
  assert.equal(probe.outputRouteSnapshot(output).connected,false);
  output.connect(context.destination);
  const interval=probe.outputRouteRange(output,0,128);
  assert.equal(interval.available,false,'disconnect and reconnect at the same render frame have no proven ordering');
  assert.match(interval.reason,/ambiguous/);
  const selectedRealm=createAudioRealm({top:{}}),selectedContext=new selectedRealm.AudioContext(),selectedOutput=new selectedRealm.AudioNode(selectedContext);
  selectedOutput.connect(selectedContext.destination,1);
  selectedOutput.disconnect(0);
  assert.equal(selectedOutput.connections.some(edge=>edge.destination===selectedContext.destination),true,
    'the recorded connection uses output 1, so disconnect(0) cannot remove or mark it disconnected');
  assert.equal(selectedRealm.window.__eaglerMusicPcmProbe.outputRouteSnapshot(selectedOutput).connected,true);
  selectedOutput.disconnect(1);
  assert.equal(selectedRealm.window.__eaglerMusicPcmProbe.outputRouteSnapshot(selectedOutput).connected,false);
});

test('rejected OGG observes eight real output windows without waiting for a playing source',async()=>{
  for(const level of [0,.25]){
    const realm=createAudioRealm({top:{}}),reader=installNativeReader(realm);
    const mixer=new realm.AudioContext({level}),unrelated=new realm.AudioContext({level:.75});
    installOutput(realm,mixer);installOutput(realm,unrelated);installTh08Module(realm,mixer);
    const native={exists:true,pending:true,soundReady:false,playing:false,name:'th08_01.wav'};
    const result=await collectRuntimeOutputPcm({role:TH08_OGG_PCM_ROLE,
      readSample:async()=>({native,...reader({game:'th08',includeRuntimePcm:true}).runtimePcm}),
      wait:async milliseconds=>{mixer.advance(milliseconds);unrelated.advance(milliseconds);}});
    assert.equal(result.collection.status,'complete',result.collection.reason);
    assert.equal(result.collection.pcmWindow.windowCount,8);
    assert.deepEqual(Array.from(result.collection.pcmWindow.tapIds),[0]);
    assert.equal(result.collection.quiet,level===0,'a rejected decoder cannot hide audible Runtime output');
    assert.deepEqual(result.lastSample.native,native,'observing output must preserve the rejected native source');
  }
  const absent=await collectRuntimeOutputPcm({role:TH08_OGG_PCM_ROLE,
    readSample:async()=>({native:{pending:true},target:null,captures:[]}),wait:async()=>{}});
  assert.equal(absent.collection.status,'incomplete','missing output evidence is not silence');
});

test('the actual MIDI reader binds PCM to TinySynth rather than an iframe or unrelated host context',async()=>{
  const host=createAudioRealm({topLevel:true}),frame=createAudioRealm({parent:host.window,top:host.window});
  const frameReader=installNativeReader(frame),synthContext=new host.AudioContext({level:0}),otherHostContext=new host.AudioContext({level:0});
  const unrelatedSource=installOutput(host,synthContext);unrelatedSource.pcmLevel=.75;
  installOutput(host,otherHostContext);
  const synthComp=installOutput(host,synthContext);synthComp.kind='dynamics-compressor';synthComp.pcmLevel=.2;
  const synthOut=new host.AudioNode(synthContext,'synth-out');synthOut.pcmLevel=0;
  const voiceTap=host.window.__eaglerMusicPcmProbe.attachTap(synthOut,TH08_MIDI_VOICE_PCM_ROLE,'tinysynth-voice-output');
  host.window.__eaglerTinySynth={getAudioContext:()=>synthContext,comp:synthComp,out:synthOut,notetab:[],
    conv:{buffer:{length:24000,sampleRate:48000,duration:.5}},rev:{},__eaglerMusicVoiceTap:voiceTap};
  const iframeMixer=new frame.AudioContext({level:0});installOutput(frame,iframeMixer);installTh08Module(frame,iframeMixer);
  const state=frameReader({game:'th08',includeMidiHost:true});
  assert.equal(state.midiHost.pcmTarget.role,TH08_MIDI_PCM_ROLE);
  assert.equal(state.midiHost.pcmTarget.contextIndex,0);
  assert.deepEqual(Array.from(state.midiHost.pcmTarget.tapIds),[2],'the final-mix target selects the exact TinySynth compressor source');
  assert.deepEqual(JSON.parse(JSON.stringify(state.midiHost.pcmTarget.sourceIdentity)),
    {nodeId:2,nodeType:'AudioNode',role:'tinysynth-compressor-output'});
  assert.equal(state.midiHost.voicePcmTarget.role,TH08_MIDI_VOICE_PCM_ROLE);
  assert.deepEqual(Array.from(state.midiHost.voicePcmTarget.tapIds),[3]);
  assert.deepEqual(JSON.parse(JSON.stringify(state.midiHost.voicePcmTarget.sourceIdentity)),
    {nodeId:3,nodeType:'AudioNode',role:'tinysynth-voice-output'});
  assert.deepEqual(JSON.parse(JSON.stringify({activeVoices:state.midiHost.activeVoices,reverb:state.midiHost.reverb})),
    {activeVoices:0,reverb:{enabled:true,bufferFrames:24000,sampleRate:48000,duration:.5}});

  const result=await collectRuntimePcmWindow({role:TH08_MIDI_PCM_ROLE,expectedTarget:state.midiHost.pcmTarget,windowCount:3,retainCaptureRows:true,
    readWindow:async()=>{
      const latest=frameReader({game:'th08',includeMidiHost:true}).midiHost;
      return {target:latest.pcmTarget,captures:latest.captures,relatedTargets:{voicePcmTarget:latest.voicePcmTarget}};
    },wait:async milliseconds=>{synthContext.advance(milliseconds);otherHostContext.advance(milliseconds);}});
  assert.equal(result.status,'complete',result.reason);
  assert.equal(result.pcmWindow.outputObserved,true);
  assert.equal(result.pcmWindow.contextIndices.length,1);
  assert.equal(result.pcmWindow.nonZeroSamples,3*2048);
  const voiceResult=summarizeRelatedRuntimePcmWindow(result,{role:TH08_MIDI_VOICE_PCM_ROLE,relatedTargetKey:'voicePcmTarget',
    expectedTarget:state.midiHost.voicePcmTarget,minimumWindows:3});
  assert.equal(voiceResult.status,'complete',voiceResult.reason);
  assert.equal(voiceResult.pcmWindow.role,TH08_MIDI_VOICE_PCM_ROLE);
  assert.equal(voiceResult.pcmWindow.outputObserved,false,'voice and compressor readings come from the same clocked capture');
  assert.deepEqual(Array.from(voiceResult.pcmWindow.tapIds),[3]);
  assert.deepEqual(voiceResult.pcmWindow.windows.map(window=>window.target.currentTime),
    result.pcmWindow.windows.map(window=>window.target.currentTime));
  const wrongSource={...result,captureRows:result.captureRows.map((row,index)=>index===1?{...row,captures:row.captures.map(capture=>
    capture.tapId===3?{...capture,sourceIdentity:{...capture.sourceIdentity,nodeId:99}}:capture)}:row)};
  const wrongSourceResult=summarizeRelatedRuntimePcmWindow(wrongSource,{role:TH08_MIDI_VOICE_PCM_ROLE,relatedTargetKey:'voicePcmTarget',
    expectedTarget:state.midiHost.voicePcmTarget,minimumWindows:3});
  assert.equal(wrongSourceResult.status,'incomplete','same-window voice evidence must retain its exact source identity');
});

test('closed, suspended, nonadvancing and state-transition windows remain explicit and cannot pass as PCM silence',async()=>{
  const makeTarget=()=>{
    const realm=createAudioRealm({top:{}}),reader=installNativeReader(realm),context=new realm.AudioContext({level:0});
    installOutput(realm,context);installTh08Module(realm,context);
    const readWindow=()=>reader({game:'th08',includeRuntimePcm:true}).runtimePcm;
    return {realm,reader,context,readWindow,target:readWindow().target};
  };

  const closed=makeTarget();closed.context.transition('closed');
  const closedResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:closed.target,settleFrames:4096,
    readWindow:async()=>closed.readWindow(),wait:async()=>{}});
  assert.equal(closedResult.status,'closed');
  assert.equal(closedResult.proof,'closed-context-lifecycle');
  assert.equal(closedResult.lifecycleReceipt.status,'passed');
  assert.equal(closedResult.pcmWindow.status,'incomplete');
  assert.equal(closedResult.pcmWindow.captureCount,0,'a closed lifecycle receipt must not masquerade as PCM captures');

  const suspended=makeTarget();suspended.context.transition('suspended');
  const suspendedResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:suspended.target,settleFrames:0,
    readWindow:async()=>suspended.readWindow(),wait:async()=>{}});
  assert.equal(suspendedResult.status,'incomplete');
  assert.match(suspendedResult.reason,/state is suspended/);

  const stalled=makeTarget();
  const stalledResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:stalled.target,windowCount:2,settleFrames:4096,
    settleTimeoutMs:20,pollIntervalMs:2,readWindow:async()=>stalled.readWindow(),wait:async()=>{}});
  assert.equal(stalledResult.status,'incomplete');
  assert.match(stalledResult.reason,/clock did not advance/);

  const flapping=makeTarget();let waits=0;
  const flappingResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:flapping.target,windowCount:3,settleFrames:0,
    readWindow:async()=>flapping.readWindow(),wait:async milliseconds=>{
      waits++;if(waits===1){flapping.context.transition('suspended');flapping.context.transition('running');}
      flapping.context.advance(milliseconds);
    }});
  assert.equal(flappingResult.status,'incomplete');
  assert.match(flappingResult.reason,/left running during the sampled window/);
  assert.equal(flappingResult.pcmWindow.windows.length,3,'failed windows retain their observed state, context clock and sample rate');

  const overflow=makeTarget();let overflowWaits=0;
  const overflowResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:overflow.target,windowCount:4,settleFrames:0,
    readWindow:async()=>overflow.readWindow(),wait:async milliseconds=>{
      overflowWaits++;if(overflowWaits===1){for(let index=0;index<33;index++){overflow.context.transition('suspended');overflow.context.transition('running');}}
      overflow.context.advance(milliseconds);
    }});
  assert.equal(overflowResult.status,'incomplete');
  assert.match(overflowResult.reason,/state-change log overflowed/);
  assert.equal(overflowResult.pcmWindow.windows.length,2);
  assert.ok(overflowResult.pcmWindow.windows[1].target.stateChangeDrops>0);

  const noTapRealm=createAudioRealm({top:{}}),noTapReader=installNativeReader(noTapRealm),noTapContext=new noTapRealm.AudioContext();
  installTh08Module(noTapRealm,noTapContext);
  const noTapResult=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,settleFrames:0,
    readWindow:async()=>noTapReader({game:'th08',includeRuntimePcm:true}).runtimePcm,wait:async()=>{}});
  assert.equal(noTapResult.status,'incomplete');
  assert.match(noTapResult.reason,/target-context-has-no-output-tap/);
});

test('actual Runtime reader states reach the restart outcome as incomplete, while captured post-close PCM remains failed',async()=>{
  const sourceRealm=createAudioRealm({top:{}}),sourceReader=installNativeReader(sourceRealm),sourceContext=new sourceRealm.AudioContext({level:.25});
  installOutput(sourceRealm,sourceContext);installTh08Module(sourceRealm,sourceContext);
  const sourcePcm=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,windowCount:8,settleFrames:0,
    readWindow:async()=>sourceReader({game:'th08',includeRuntimePcm:true}).runtimePcm,
    wait:async milliseconds=>sourceContext.advance(milliseconds)});
  assert.equal(sourcePcm.status,'complete');
  assert.equal(sourcePcm.pcmWindow.outputObserved,true);
  const expectedTarget=sourcePcm.target;
  const before={native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_00.wav'},trackFile:'th08_00.ogg',
    input:{name:'th08_00.ogg',sha256:'a'.repeat(64)},pcm:sourcePcm.pcmWindow};
  const reopened={native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_01.wav'},trackFile:'th08_01.ogg',
    input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:sourcePcm.pcmWindow};
  const outcome=quietWindow=>th08OggRestartOutcome({before,quietWindow,reopened,sameRuntimeFrame:true,exitObserved:true});

  const suspendedRealm=createAudioRealm({top:{}}),suspendedReader=installNativeReader(suspendedRealm),suspendedContext=new suspendedRealm.AudioContext();
  installOutput(suspendedRealm,suspendedContext);installTh08Module(suspendedRealm,suspendedContext);
  const suspendedTarget=suspendedReader({game:'th08',includeRuntimePcm:true}).runtimePcm.target;suspendedContext.transition('suspended');
  const suspended=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:suspendedTarget,settleFrames:0,
    readWindow:async()=>suspendedReader({game:'th08',includeRuntimePcm:true}).runtimePcm,wait:async()=>{}});
  const suspendedOutcome=outcome(suspended);
  assert.equal(suspendedOutcome.status,'incomplete');
  assert.match(suspendedOutcome.quietReason,/state is suspended/);

  const stalledRealm=createAudioRealm({top:{}}),stalledReader=installNativeReader(stalledRealm),stalledContext=new stalledRealm.AudioContext();
  installOutput(stalledRealm,stalledContext);installTh08Module(stalledRealm,stalledContext);
  const stalledTarget=stalledReader({game:'th08',includeRuntimePcm:true}).runtimePcm.target;
  const stalled=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,expectedTarget:stalledTarget,windowCount:2,settleFrames:4096,
    settleTimeoutMs:20,pollIntervalMs:2,readWindow:async()=>stalledReader({game:'th08',includeRuntimePcm:true}).runtimePcm,wait:async()=>{}});
  const stalledOutcome=outcome(stalled);
  assert.equal(stalledOutcome.status,'incomplete');
  assert.match(stalledOutcome.quietReason,/clock did not advance/);

  const noTapRealm=createAudioRealm({top:{}}),noTapReader=installNativeReader(noTapRealm),noTapContext=new noTapRealm.AudioContext();
  installTh08Module(noTapRealm,noTapContext);
  const noTap=await collectRuntimePcmWindow({role:TH08_OGG_PCM_ROLE,settleFrames:0,
    readWindow:async()=>noTapReader({game:'th08',includeRuntimePcm:true}).runtimePcm,wait:async()=>{}});
  const noTapOutcome=outcome(noTap);
  assert.equal(noTapOutcome.status,'incomplete');
  assert.match(noTapOutcome.quietReason,/target-context-has-no-output-tap/);

  const pcmViolationOutcome=outcome(sourcePcm);
  assert.equal(pcmViolationOutcome.status,'failed');
  assert.match(pcmViolationOutcome.quietReason,/remained nonzero after close/);
  assert.equal(expectedTarget.role,TH08_OGG_PCM_ROLE);
});

test('active OGG PCM collection waits for the expected native track and stops at its first real output sample',async()=>{
  const target={role:TH08_OGG_PCM_ROLE,available:true,contextIndex:0,tapIds:[0],sampleRate:48000,state:'running',
    stateChangeCount:0,stateChangeDrops:0,stateChanges:[]};
  const makeSample=(trackFile,nonZero,currentTime)=>({
    native:{exists:true,soundReady:true,pending:false,playing:true,outputPaused:false,name:trackFile.replace('.ogg','.wav')},trackFile,
    target:{...target,currentTime},captures:[{tapId:0,contextIndex:0,role:TH08_OGG_PCM_ROLE,contextState:'running',sampleRate:48000,
      currentTime,samples:2048,nonZero,peakAbs:nonZero?0.5:0,rms:nonZero?0.2:0}],
  });
  const samples=[makeSample('th08_01.ogg',2048,.1),makeSample('th08_00.ogg',0,.2),makeSample('th08_00.ogg',2048,.3)];
  let reads=0;const waits=[];
  const result=await collectExpectedTrackRuntimePcm({role:TH08_OGG_PCM_ROLE,expectedFile:'th08_00.ogg',maxSamples:8,
    readSample:async()=>samples[reads++],wait:async milliseconds=>waits.push(milliseconds)});
  assert.equal(result.status,'observed');
  assert.equal(result.attempts,3);
  assert.equal(result.pcmObservedAtAttempt,3);
  assert.equal(result.pcmSourceFile,'th08_00.ogg');
  assert.equal(result.observedFile,'th08_00.ogg');
  assert.deepEqual(waits,[80,80]);
  assert.deepEqual(result.pcmWindow.windows.map(window=>window.taps[0].currentTime),[.2,.3],
    'nonzero PCM from a different native source is excluded from the expected track evidence');
  assert.equal(result.pcmWindow.nonZeroSamples,2048);

  let boundedReads=0;
  const bounded=await collectExpectedTrackRuntimePcm({role:TH08_OGG_PCM_ROLE,expectedFile:'th08_00.ogg',maxSamples:2,
    readSample:async()=>makeSample('th08_00.ogg',0,++boundedReads/10),wait:async()=>{}});
  assert.equal(bounded.status,'bounded-no-output');
  assert.equal(bounded.attempts,2);
  assert.equal(bounded.pcmWindow.status,'complete');
  assert.equal(bounded.pcmWindow.outputObserved,false);
  assert.match(bounded.reason,/2 bounded samples/);

  let changedReads=0;
  const changed=await collectExpectedTrackRuntimePcm({role:TH08_OGG_PCM_ROLE,expectedFile:'th08_00.ogg',maxSamples:4,
    readSample:async()=>makeSample(changedReads++===0?'th08_00.ogg':'th08_01.ogg',0,changedReads/10),wait:async()=>{}});
  assert.equal(changed.status,'incomplete','a source change before audible PCM ends the attribution window');
  assert.match(changed.reason,/stopped or changed/);
});

