import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readdir,readFile,stat} from 'node:fs/promises';
import {isAbsolute,resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

export const MUSIC_BROWSER_GAMES=Object.freeze(['th08','th10','th11','th15','th20']);
export const TH08_OGG_PCM_ROLE='th08-sdl3-mixer';
export const TH08_MIDI_PCM_ROLE='launcher-tinysynth';
export const TH08_MIDI_VOICE_PCM_ROLE='launcher-tinysynth-voice';
export const RUNTIME_PCM_SAMPLE_INTERVAL_MS=80;
export const MUSIC_PCM_HISTORY_GAP_RANGE_LIMIT=64;
export function selectedMusicMappingGames(games=[]) {
  return games.filter(game=>game==='th11'||game==='th15');
}
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');

/** Bounded sample history fed by an existing production-output tap. */
export const MUSIC_PCM_HISTORY_PROCESSOR_SOURCE=`class EaglerMusicPcmHistoryProcessor extends AudioWorkletProcessor {
 constructor(options){super();const capacity=options?.processorOptions?.capacityFrames;
  this.capacityFrames=Number.isSafeInteger(capacity)&&capacity>0?capacity:0;this.peaks=new Float32Array(this.capacityFrames);
  this.firstFrame=null;this.nextFrame=null;this.gaps=0;this.gapRanges=[];this.gapDrops=0;this.gapRangeLimit=${MUSIC_PCM_HISTORY_GAP_RANGE_LIMIT};
  this.overwrittenFrames=0;this.requestCount=0;this.requestLimit=64;this.processCount=0;this.lastProcessStartFrame=null;this.lastProcessEndFrame=null;
  this.port.onmessage=event=>{const message=event?.data;if(!message||!['snapshot','snapshotBatch','status'].includes(message.type))return;
   if(++this.requestCount>this.requestLimit){this.port.postMessage({type:message.type,requestId:message.requestId,available:false,reason:'history-request-limit'});return;}
   if(message.type==='status'){
    this.port.postMessage({type:'status',requestId:message.requestId,available:true,firstFrame:this.firstFrame,renderedThroughFrame:this.nextFrame,
     processCount:this.processCount,lastProcessStartFrame:this.lastProcessStartFrame,lastProcessEndFrame:this.lastProcessEndFrame,
     currentFrame:Number.isSafeInteger(currentFrame)?currentFrame:null,gaps:this.gaps,gapDrops:this.gapDrops,overwrittenFrames:this.overwrittenFrames,
     gapRangeCount:this.gapRanges.length,gapRangeLimit:this.gapRangeLimit});return;
   }
   const measure=range=>{const fromFrame=range?.fromFrame,toFrame=range?.toFrame;
    if(!Number.isSafeInteger(fromFrame)||!Number.isSafeInteger(toFrame)||fromFrame<0||toFrame<=fromFrame||!this.capacityFrames)
     return{available:false,reason:'malformed-history-range',fromFrame,toFrame};
   const retainedStartFrame=this.nextFrame===null?null:Math.max(this.firstFrame,this.nextFrame-this.capacityFrames);
   const overlappingGapRanges=this.gapRanges.filter(gap=>gap.fromFrame<toFrame&&gap.toFrame>fromFrame);
   const retained=this.nextFrame!==null&&fromFrame>=retainedStartFrame&&toFrame<=this.nextFrame;
   const covered=retained&&this.gapDrops===0&&overlappingGapRanges.length===0;
   const coverageReason=this.gapDrops>0?'gap-index-overflow':overlappingGapRanges.length?'render-frame-gap-overlaps-range':
    !retained?'range-is-outside-retained-render-history':null;
   let peakAbs=0,nonZeroFrames=0,firstNonZeroFrame=null;
   if(covered)for(let frame=fromFrame;frame<toFrame;frame++){
    const absolute=Math.abs(this.peaks[frame%this.capacityFrames]);if(absolute>peakAbs)peakAbs=absolute;
    if(absolute>0){nonZeroFrames++;if(firstNonZeroFrame===null)firstNonZeroFrame=frame;}
   }
    return{available:true,fromFrame,toFrame,frames:toFrame-fromFrame,
    covered,coverageReason,retainedStartFrame,renderedThroughFrame:this.nextFrame,firstFrame:this.firstFrame,gaps:this.gaps,
    gapRangeLimit:this.gapRangeLimit,gapRanges:this.gapRanges.map(gap=>({fromFrame:gap.fromFrame,toFrame:gap.toFrame})),
    overlappingGapRanges:overlappingGapRanges.map(gap=>({fromFrame:gap.fromFrame,toFrame:gap.toFrame})),gapDrops:this.gapDrops,
    overwrittenFrames:this.overwrittenFrames,capacityFrames:this.capacityFrames,nonZeroFrames,firstNonZeroFrame,peakAbs};};
   if(message.type==='snapshotBatch'){
    const ranges=message.ranges;if(!Array.isArray(ranges)||ranges.length>64){this.port.postMessage({type:message.type,requestId:message.requestId,available:false,reason:'history-batch-size-invalid'});return;}
    this.port.postMessage({type:message.type,requestId:message.requestId,available:true,results:ranges.map(measure)});return;
   }
   this.port.postMessage({type:message.type,requestId:message.requestId,...measure(message)});
  };
 }
 process(inputs,outputs){const input=inputs?.[0]??[],output=outputs?.[0]??[],frames=output[0]?.length??input[0]?.length??128;
  const blockStart=currentFrame;if(this.nextFrame!==null&&blockStart!==this.nextFrame){this.gaps++;
   const gap={fromFrame:Math.min(blockStart,this.nextFrame),toFrame:Math.max(blockStart,this.nextFrame)};
   if(this.gapRanges.length<this.gapRangeLimit)this.gapRanges.push(gap);else this.gapDrops++;}
  if(this.firstFrame===null)this.firstFrame=blockStart;
  for(let offset=0;offset<frames;offset++){
   let peak=0;for(const channel of input){const absolute=Math.abs(channel?.[offset]??0);if(absolute>peak)peak=absolute;}
   this.peaks[(blockStart+offset)%this.capacityFrames]=peak;
  }
  this.nextFrame=blockStart+frames;this.processCount++;this.lastProcessStartFrame=blockStart;this.lastProcessEndFrame=this.nextFrame;
  if(this.firstFrame!==null&&this.nextFrame-this.firstFrame>this.capacityFrames)
   this.overwrittenFrames=Math.max(this.overwrittenFrames,this.nextFrame-this.firstFrame-this.capacityFrames);
  for(const channel of output)channel.fill(0);
  return true;
 }
}
registerProcessor('eagler-music-pcm-history',EaglerMusicPcmHistoryProcessor);`;

export const MUSIC_PCM_PROBE_SOURCE=`(()=>{
 if((window===window.top&&window.__eaglerMusicPcmProbeTop!==true)||!window.AudioContext||!window.AudioNode)return;
  const stateChangeLimit=64,nodeIds=new WeakMap(),contexts=[],contextStateRecords=[],routeStates=new WeakMap(),historyModuleLoads=new WeakMap();let nextNodeId=0,nextHistoryId=1;
  const identifyNode=(node,role=null)=>{let nodeId=nodeIds.get(node);if(nodeId===undefined){nodeId=nextNodeId++;nodeIds.set(node,nodeId);}
   return{nodeId,nodeType:node?.constructor?.name??'AudioNode',role};};
  const recordContext=context=>{if(contexts.includes(context))return;const contextIndex=contexts.push(context)-1,record={changes:[],count:0,drops:0};
   contextStateRecords[contextIndex]=record;context.addEventListener('statechange',()=>{record.count++;const change={sequence:record.count,state:context.state,currentTime:context.currentTime};
    if(record.changes.length<stateChangeLimit)record.changes.push(change);else record.drops++;});};
  const noteOutputRoute=(source,connected)=>{let record=routeStates.get(source);if(!record){record={known:false,connected:false,events:[],drops:0};routeStates.set(source,record);}
   if(record.known&&record.connected===connected)return;
   record.known=true;record.connected=connected;const context=source?.context,contextTime=context?.currentTime,contextFrame=context&&Number.isFinite(contextTime)&&Number.isFinite(context.sampleRate)
    ?Math.round(contextTime*context.sampleRate):null,event={sequence:record.events.length+record.drops+1,connected,contextTime:contextTime??null,contextFrame};
   if(record.events.length<stateChangeLimit)record.events.push(event);else record.drops++;};
  const describe=(context,role,tapIds,sourceIdentity=null,snapshot=null,reason=null)=>{
   if(!context)return{role,available:false,reason:reason??'target-audio-context-missing',contextIndex:null,tapIds:[],state:null,sampleRate:null,currentTime:null,
    stateChanges:[],stateChangeCount:0,stateChangeDrops:0,...(sourceIdentity?{sourceIdentity}: {})};
   recordContext(context);const contextIndex=contexts.indexOf(context),record=contextStateRecords[contextIndex];
   const clock=snapshot?.context===context?snapshot:{state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime};
   const available=contextIndex>=0&&tapIds.length>0&&probe.errors.length===0;
   return{role,available,reason:available?null:probe.errors[0]??reason??'target-context-has-no-output-tap',contextIndex,tapIds,state:clock.state,sampleRate:clock.sampleRate,
    currentTime:clock.currentTime,stateChanges:record?.changes.slice()??[],stateChangeCount:record?.count??0,stateChangeDrops:record?.drops??0,
    ...(sourceIdentity?{sourceIdentity}: {})};
  };
  const probe={contexts,taps:[],contextStateRecords,errors:[],identifyNode:identifyNode,
  capture(snapshot=null,{retainSamples=false}={}){const clocks=new Map(this.contexts.map(context=>
   [context,{context,state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime}]));
   if(snapshot?.context)clocks.set(snapshot.context,snapshot);const out=[];for(const analyser of this.taps){
  const samples=new Float32Array(analyser.fftSize);analyser.getFloatTimeDomainData(samples);
  let peakAbs=0,squares=0,nonZero=0;for(const value of samples){const absolute=Math.abs(value);if(absolute>peakAbs)peakAbs=absolute;if(value!==0)nonZero++;squares+=value*value;}
  const clock=clocks.get(analyser.context);
  out.push({tapId:analyser.__eaglerMusicPcmTapId,contextIndex:this.contexts.indexOf(analyser.context),contextState:clock?.state??null,sampleRate:clock?.sampleRate??null,currentTime:clock?.currentTime??null,
   role:analyser.__eaglerMusicPcmRole??null,sourceIdentity:analyser.__eaglerMusicPcmSourceIdentity??null,samples:samples.length,peakAbs,rms:Math.sqrt(squares/samples.length),nonZero,
   ...(retainSamples?{pcmSamples:Array.from(samples)}:{})});
 }return out;},describeTarget(context,role,snapshot=null){
  if(!context)return describe(null,role,[],null,snapshot);
  recordContext(context);
  for(const tap of this.taps)if(tap.context===context&&!tap.__eaglerMusicPcmRole)tap.__eaglerMusicPcmRole=role;
  const tapIds=this.taps.filter(tap=>tap.context===context&&tap.__eaglerMusicPcmRole===role).map(tap=>tap.__eaglerMusicPcmTapId);
  return describe(context,role,tapIds,null,snapshot);
 },describeSource(source,role,sourceRole,snapshot=null){
  if(!source?.context)return describe(null,role,[],null,snapshot,'target-source-node-missing');
  const matches=this.taps.filter(tap=>tap.context===source.context&&tap.__eaglerMusicPcmSourceNode===source);
  if(matches.length!==1)return describe(source.context,role,[],null,snapshot,'target-source-has-'+matches.length+'-output-taps');
  const tap=matches[0],identity=identifyNode(source,sourceRole);tap.__eaglerMusicPcmRole=role;tap.__eaglerMusicPcmSourceIdentity=identity;
  return describe(source.context,role,[tap.__eaglerMusicPcmTapId],identity,snapshot);
 },describeTap(tap,role,sourceRole=role,snapshot=null){
  if(!tap)return describe(null,role,[],null,snapshot,'target-source-tap-missing');
  const source=tap.__eaglerMusicPcmSourceNode,identity=source?identifyNode(source,sourceRole):null;
  tap.__eaglerMusicPcmRole=role;tap.__eaglerMusicPcmSourceIdentity=identity;
  return describe(tap.context,role,Number.isInteger(tap.__eaglerMusicPcmTapId)?[tap.__eaglerMusicPcmTapId]:[],identity,snapshot,
   identity?null:'target-source-node-missing');
 },attachTap(source,role,sourceRole=role){
  if(!source?.context||typeof role!=='string'||!role)return null;
  const context=source.context;
  recordContext(context);
  try{
   const analyser=context.createAnalyser();analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
   const mute=context.createGain();mute.gain.value=0;
   nativeConnect.call(source,analyser);nativeConnect.call(analyser,mute);nativeConnect.call(mute,context.destination);
   analyser.__eaglerMusicPcmTapId=this.taps.length;analyser.__eaglerMusicPcmRole=role;analyser.__eaglerMusicPcmMute=mute;
   analyser.__eaglerMusicPcmSourceNode=source;analyser.__eaglerMusicPcmSourceIdentity=identifyNode(source,sourceRole);
   this.taps.push(analyser);return analyser;
  }catch(error){this.errors.push(String(error?.stack||error));return null;}
 },
 async prepareTapHistory(tap,{capacityFrames}={}){
  if(!tap?.context||!this.taps.includes(tap))return{available:false,reason:'history-tap-is-not-owned-by-shared-probe'};
  if(!Number.isSafeInteger(capacityFrames)||capacityFrames<=0)return{available:false,reason:'history-capacity-is-invalid'};
  if(tap.__eaglerMusicPcmHistory)return{available:true,handle:tap.__eaglerMusicPcmHistory};
  const context=tap.context;
  if(!context.audioWorklet||typeof window.AudioWorkletNode!=='function'||typeof window.Blob!=='function'||!window.URL?.createObjectURL)
   return{available:false,reason:'AudioWorklet history capture is unavailable'};
  try{
   let load=historyModuleLoads.get(context);if(!load){const blob=new window.Blob([${JSON.stringify(MUSIC_PCM_HISTORY_PROCESSOR_SOURCE)}],{type:'text/javascript'}),url=window.URL.createObjectURL(blob);
    load=Promise.resolve(context.audioWorklet.addModule(url)).finally(()=>window.URL.revokeObjectURL?.(url));historyModuleLoads.set(context,load);}
   await load;
   const worklet=new window.AudioWorkletNode(context,'eagler-music-pcm-history',{numberOfInputs:1,numberOfOutputs:1,outputChannelCount:[1],
    processorOptions:{capacityFrames}}),mute=context.createGain();mute.gain.value=0;
   const pending=new Map(),history={handleId:nextHistoryId++,tapId:tap.__eaglerMusicPcmTapId,context,worklet,mute,capacityFrames,nextRequestId:1,pending,closed:false};
   worklet.port.onmessage=event=>{const message=event?.data;if(!['snapshot','snapshotBatch','status'].includes(message?.type)||!Number.isSafeInteger(message.requestId))return;
    const request=pending.get(message.requestId);if(request){pending.delete(message.requestId);window.clearTimeout(request.timer);request.resolve(message);}};
   nativeConnect.call(tap,worklet);nativeConnect.call(worklet,mute);nativeConnect.call(mute,context.destination);
   const request=message=>new Promise(resolve=>{
    if(history.closed||pending.size!==0){resolve({available:false,reason:history.closed?'history-tap-closed':'history-port-already-has-a-pending-request'});return;}
    const requestId=history.nextRequestId++,timer=window.setTimeout(()=>{pending.delete(requestId);resolve({type:message.type,requestId,available:false,reason:'history-response-timeout'});},1000);
    pending.set(requestId,{resolve,timer});worklet.port.postMessage({...message,requestId});
   });
   history.snapshot=({fromFrame,toFrame})=>request({type:'snapshot',fromFrame,toFrame});
   history.snapshotBatch=ranges=>request({type:'snapshotBatch',ranges});
   history.status=()=>request({type:'status'});
   history.close=()=>{if(history.closed)return;history.closed=true;
    for(const {resolve,timer} of pending.values()){window.clearTimeout(timer);resolve({available:false,reason:'history-tap-closed-before-response'});}pending.clear();
    try{tap.disconnect(worklet);}catch(_){}try{worklet.disconnect();}catch(_){}try{mute.disconnect();}catch(_){}try{worklet.port.close();}catch(_){}
    if(tap.__eaglerMusicPcmHistory===history)delete tap.__eaglerMusicPcmHistory;};
   tap.__eaglerMusicPcmHistory=history;return{available:true,handle:history};
  }catch(error){this.errors.push(String(error?.stack||error));return{available:false,reason:String(error?.message||error)};}
 },
 outputRouteSnapshot(source){const record=source&&routeStates.get(source);return record?{known:record.known,connected:record.connected,events:record.events.slice(),drops:record.drops}:
  {known:false,connected:false,events:[],drops:0};},
 outputRouteRange(source,fromFrame,toFrame){const snapshot=this.outputRouteSnapshot(source);
  if(!snapshot.known||snapshot.drops!==0||!Number.isSafeInteger(fromFrame)||!Number.isSafeInteger(toFrame)||toFrame<fromFrame)
   return{available:false,connected:null,reason:'production-output-route-history-is-incomplete',events:snapshot.events,drops:snapshot.drops};
  let state=null;const statesByFrame=new Map();for(const event of snapshot.events){if(!Number.isSafeInteger(event.contextFrame))return{available:false,connected:null,reason:'production-output-route-clock-is-unavailable',events:snapshot.events,drops:snapshot.drops};
   const states=statesByFrame.get(event.contextFrame)??new Set();states.add(event.connected);statesByFrame.set(event.contextFrame,states);
   if(states.size>1&&event.contextFrame>=fromFrame&&event.contextFrame<toFrame)return{available:false,connected:null,
    reason:'production-output-route-transition-is-ambiguous-within-one-render-frame',events:snapshot.events,drops:snapshot.drops};
   if(event.contextFrame<=fromFrame)state=event.connected;}
  if(state===null)return{available:false,connected:null,reason:'production-output-route-start-state-is-unobserved',events:snapshot.events,drops:snapshot.drops};
  const disconnected=snapshot.events.some(event=>event.contextFrame>=fromFrame&&event.contextFrame<toFrame&&!event.connected);
  return{available:true,connected:state&&!disconnected,reason:null,fromFrame,toFrame,events:snapshot.events,drops:snapshot.drops};
 },
 summary(){return{contexts:this.contexts.map(context=>({state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime})),tapCount:this.taps.length,errors:this.errors.slice()};}};
 const nativeConnect=AudioNode.prototype.connect,wrapped=new WeakMap();
 AudioNode.prototype.connect=function(destination,...args){
  const result=nativeConnect.call(this,destination,...args);
  if(destination===this.context.destination){
    noteOutputRoute(this,true);
   const existing=wrapped.get(this);
   if(existing){
    // Production disconnect() also removes this observation branch. Restore it
    // only when production reconnects its output; identical edges are idempotent.
    try{nativeConnect.call(this,existing.analyser,existing.output,0);}
    catch(error){probe.errors.push(String(error?.stack||error));}
    return result;
   }
   const context=this.context;
   try{
    recordContext(context);
    const analyser=context.createAnalyser();analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
    const mute=context.createGain();mute.gain.value=0;
    const output=Number.isInteger(args[0])?args[0]:0;
    nativeConnect.call(this,analyser,output,0);nativeConnect.call(analyser,mute);nativeConnect.call(mute,destination);
    analyser.__eaglerMusicPcmTapId=probe.taps.length;analyser.__eaglerMusicPcmMute=mute;
    analyser.__eaglerMusicPcmSourceNode=this;analyser.__eaglerMusicPcmSourceIdentity=identifyNode(this,null);
    probe.taps.push(analyser);wrapped.set(this,{analyser,output});
   }catch(error){probe.errors.push(String(error?.stack||error));}
  }
  return result;
 };
  const nativeDisconnect=AudioNode.prototype.disconnect;
  AudioNode.prototype.disconnect=function(...args){
   const result=Reflect.apply(nativeDisconnect,this,args);
   const outputRoute=wrapped.get(this),destination=this.context?.destination;
   const disconnectsObservedRoute=args.length===0||
    (args[0]===destination&&(args.length<2||args[1]===outputRoute?.output))||
    (Number.isInteger(args[0])&&outputRoute&&args[0]===outputRoute.output);
   if(disconnectsObservedRoute)noteOutputRoute(this,false);
   return result;
  };
 window.__eaglerMusicPcmProbe=probe;
})();`;

/** Read the Runtime's own exported audio buffers; no track playback is synthesized by the harness. */
export function readNativeAudioState(request) {
  const {game,includeMidiHost=false,includeRuntimePcm=false}=typeof request==='string'?{game:request}:request;
  const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module;
  const core=runtime?.core??window.core??module;
  const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer??window.HEAPU8?.buffer;
  const exported=name=>{for(const owner of [core,module]){const fn=owner?.[name]??owner?.[`_${name}`];if(typeof fn==='function')return fn.bind(owner);}return null;};
  const words=(name,count)=>{
    const fn=exported(name);if(!fn||!buffer)return null;const pointer=fn();
    if(!Number.isInteger(pointer)||pointer<=0||pointer+count*4>buffer.byteLength)return null;
    return Array.from(new Uint32Array(buffer,pointer,count));
  };
  const string=(name,limit)=>{
    const fn=exported(name);if(!fn||!buffer)return null;const pointer=fn();
    if(!Number.isInteger(pointer)||pointer<=0||pointer+limit>buffer.byteLength)return null;
    const bytes=new Uint8Array(buffer,pointer,limit),end=bytes.indexOf(0);
    if(end<0)return null;
    try{return new TextDecoder('utf-8',{fatal:true}).decode(bytes.subarray(0,end));}catch{return null;}
  };
  const decodeMusic=state=>{
    if(!Array.isArray(state)||state.length!==16||state[0]!==1)throw Error('TH08 music observation ABI is missing or unsupported');
    for(const index of [1,2,3,4,5,6,7])if(state[index]!==0&&state[index]!==1)throw Error(`TH08 music observation boolean ${index} is invalid`);
    return {abiVersion:1,exists:state[1]===1,outputPaused:state[2]===1,soundReady:state[3]===1,pending:state[4]===1,
      requested:state[5]===1,loop:state[6]===1,playing:state[7]===1,cursorFrames:Number((BigInt(state[9]>>>0)<<32n)|BigInt(state[8]>>>0)),
      totalFrames:state[10],introFrames:state[11],sampleRate:state[12],queuedFrames:state[13],mixedFrames:state[14],error:state[15]};
  };
  const decodeMidi=state=>{
    if(!Array.isArray(state)||state.length!==22||state[0]!==1)throw Error('TH08 MIDI observation ABI is missing or unsupported');
    for(const index of [1,2])if(state[index]!==0&&state[index]!==1)throw Error(`TH08 MIDI observation boolean ${index} is invalid`);
    return {abiVersion:1,active:state[1]===1,invalid:state[2]===1,fileIndex:state[3]===0xffffffff?-1:state[3],tempo:state[4],
      divisions:state[5],trackCount:state[6],activeTrackCount:state[7],elapsedTicks:Number((BigInt(state[9]>>>0)<<32n)|BigInt(state[8]>>>0)),
      fading:state[10],fadeElapsed:state[11],fadeInterval:state[12],messageCount:state[13],noteOnCount:state[14],noteOffCount:state[15],
      controllerCount:state[16],sysexCount:state[17],openCount:state[18],closeCount:state[19],clockMs:state[20],
      lastMusicSong:state[21]===0xffffffff?-1:state[21]};
  };
  if(game==='th11')return {game,native:{source:'th11_audio_statistics',words:words('th11_audio_statistics',9)},decoder:null};
  if(game==='th15')return {game,native:{source:'th15_audio_statistics',words:words('th15_audio_statistics',11)},decoder:null};
  const audioWords=words('sdl_audio_stats',12),musicWords=words('sdl_music_stats',6);
  let rms=null;
  const audioPointer=exported('sdl_audio_stats')?.();
  if(Number.isInteger(audioPointer)&&audioPointer>0&&audioPointer+44<=buffer?.byteLength)
    rms=new DataView(buffer).getFloat32(audioPointer+40,true);
  let decoder=null;
  if(musicWords&&game==='th08')decoder={enabled:musicWords[0]!==0,decodeMode:musicWords[1],soundReady:musicWords[2]!==0,
    pending:musicWords[3],bufferCount:musicWords[4],resourceChanges:musicWords[5]};
  else if(musicWords)decoder={enabled:musicWords[0]!==0,decodeMode:musicWords[1],streamCount:musicWords[2],
    waiting:musicWords[3],valid:musicWords[4],fullReady:musicWords[5]};
  let musicObservation=null,midiObservation=null;
  if(game==='th08'){
    const musicState=words('sdl_music_observation',16),musicName=string('sdl_music_name',17);
    const midiState=words('sdl_midi_observation',22),midiPath=string('sdl_midi_path',256);
    if(musicState)musicObservation={...decodeMusic(musicState),name:musicName};
    if(midiState)midiObservation={...decodeMidi(midiState),path:midiPath};
  }
  let runtimePcm,midiHost;
  if(includeRuntimePcm){
    const probe=window.__eaglerMusicPcmProbe,context=module?.SDL3?.audioContext??null;
    runtimePcm={target:probe?.describeTarget?.(context,'th08-sdl3-mixer')??{role:'th08-sdl3-mixer',available:false,
      reason:probe?'target-context-observer-is-unavailable':'runtime-audio-probe-is-unavailable',contextIndex:null,tapIds:[],state:context?.state??null,
      sampleRate:context?.sampleRate??null,currentTime:context?.currentTime??null,stateChanges:[],stateChangeCount:0,stateChangeDrops:0},
      captures:probe?.capture?.()??[]};
  }
  if(includeMidiHost){
    // Copy sequencer counters and bridge events in one browser task. Separate
    // frame/page round trips can straddle a native MIDI tick.
    const host=window.parent,synth=host.__eaglerTinySynth,probe=host.__eaglerMusicPcmProbe;
    const context=synth?.getAudioContext?.()??null,reverbBuffer=synth?.conv?.buffer??null;
    const audioClock=context?{context,state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime}:null;
    midiHost={contextState:context?.state??null,contextIndex:context&&probe?probe.contexts.indexOf(context):-1,
      pcmTarget:probe?.describeSource?.(synth?.comp,'launcher-tinysynth','tinysynth-compressor-output',audioClock)??{role:'launcher-tinysynth',available:false,
        reason:probe?'target-context-observer-is-unavailable':'launcher-audio-probe-is-unavailable',contextIndex:null,tapIds:[],state:context?.state??null,
        sampleRate:context?.sampleRate??null,currentTime:context?.currentTime??null,sourceIdentity:null,stateChanges:[],stateChangeCount:0,stateChangeDrops:0},
      voicePcmTarget:probe?.describeTap?.(synth?.__eaglerMusicVoiceTap,'launcher-tinysynth-voice','tinysynth-voice-output',audioClock)??{role:'launcher-tinysynth-voice',available:false,
        reason:probe?'target-context-has-no-output-tap':'launcher-audio-probe-is-unavailable',contextIndex:null,tapIds:[],state:context?.state??null,
        sampleRate:context?.sampleRate??null,currentTime:context?.currentTime??null,sourceIdentity:null,stateChanges:[],stateChangeCount:0,stateChangeDrops:0},
      captures:probe?.capture?.(audioClock)??[],activeVoices:Array.isArray(synth?.notetab)?synth.notetab.length:null,
      reverb:{enabled:!!(synth?.conv&&synth?.rev),bufferFrames:Number.isSafeInteger(reverbBuffer?.length)?reverbBuffer.length:null,
        sampleRate:Number.isFinite(reverbBuffer?.sampleRate)?reverbBuffer.sampleRate:null,
        duration:Number.isFinite(reverbBuffer?.duration)?reverbBuffer.duration:null},
      sentMessages:host.__eaglerTinySynthSends??[],nativeEvents:host.__eaglerMidiNativeEvents??[]};
  }
  return {game,native:{source:'sdl_audio_stats',words:audioWords,rms},decoder:decoder?{source:'sdl_music_stats',words:musicWords,...decoder}:null,
    ...(includeRuntimePcm?{runtimePcm}:{}),...(includeMidiHost?{midiHost}:{}),
    ...(game==='th08'?{musicObservation,midiObservation}:{})};
}

const u64=(low,high)=>Number((BigInt(high>>>0)<<32n)|BigInt(low>>>0));
function verifyObservationWords(words,count,name){
  assert.ok(Array.isArray(words)&&words.length===count,`${name} observation ABI contains exactly ${count} words`);
  for(const word of words)assert.ok(Number.isInteger(word)&&word>=0&&word<=0xffffffff,`${name} observation contains an invalid word`);
  assert.equal(words[0],1,`${name} observation ABI version is unsupported`);
}
export function decodeTh08MusicObservation(words,name=null){
  verifyObservationWords(words,16,'TH08 music');
  for(const index of [1,2,3,4,5,6,7])assert.ok(words[index]===0||words[index]===1,`TH08 music observation boolean ${index} is invalid`);
  return {abiVersion:1,exists:words[1]===1,outputPaused:words[2]===1,soundReady:words[3]===1,pending:words[4]===1,
    requested:words[5]===1,loop:words[6]===1,playing:words[7]===1,cursorFrames:u64(words[8],words[9]),
    totalFrames:words[10],introFrames:words[11],sampleRate:words[12],queuedFrames:words[13],mixedFrames:words[14],error:words[15],name};
}
export function decodeTh08MidiObservation(words,path=null){
  verifyObservationWords(words,22,'TH08 MIDI');
  for(const index of [1,2])assert.ok(words[index]===0||words[index]===1,`TH08 MIDI observation boolean ${index} is invalid`);
  return {abiVersion:1,active:words[1]===1,invalid:words[2]===1,fileIndex:words[3]===0xffffffff?-1:words[3],
    tempo:words[4],divisions:words[5],trackCount:words[6],activeTrackCount:words[7],elapsedTicks:u64(words[8],words[9]),
    fading:words[10],fadeElapsed:words[11],fadeInterval:words[12],messageCount:words[13],noteOnCount:words[14],
    noteOffCount:words[15],controllerCount:words[16],sysexCount:words[17],openCount:words[18],closeCount:words[19],clockMs:words[20],
    lastMusicSong:words[21]===0xffffffff?-1:words[21],path};
}

/** Validate an explicit, catalog-bound OGG inventory without opening a browser. */
export function validateMusicManifest(manifest,{repoRoot,games,catalog=PRODUCT_GAMES}={}) {
  assert.equal(manifest?.schema,'eagler-touhou/music-inputs/1','Unsupported music input manifest');
  assert.equal(manifest?.resourcePolicy,'private-local-paths','Music inputs must remain local private resources');
  assert.ok(Array.isArray(games)&&games.length>0,'Explicit nonempty OGG game selection required');
  assert.equal(new Set(games).size,games.length,'Music game selection contains duplicates');
  const selected=Object.create(null);
  for(const game of games){
    assert.ok(MUSIC_BROWSER_GAMES.includes(game),`Unsupported music acceptance game ${game}`);
    const contract=catalog[game]?.package;
    const music=manifest.games?.[game];
    assert.ok(contract?.musicSourceDirectories?.ogg&&contract?.musicMounts?.ogg,`${game}: catalog has no OGG source/mount contract`);
    assert.ok(music&&Array.isArray(music.files)&&music.files.length>0,`${game}: OGG resources are unavailable in the manifest`);
    assert.ok(isAbsolute(music.sourceDirectory),`${game}: source directory must be an explicit absolute local path`);
    const expectedSuffix=['games','web-content',game,...contract.musicSourceDirectories.ogg.split(/[\\/]+/).filter(Boolean)];
    const sourceDirectory=resolve(music.sourceDirectory),sourceParts=sourceDirectory.split(/[\\/]+/).filter(Boolean);
    assert.ok(sourceParts.length>=expectedSuffix.length&&sourceParts.slice(-expectedSuffix.length).join('/').toLowerCase()===expectedSuffix.join('/').toLowerCase(),
      `${game}: source directory differs from product catalog`);
    assert.equal(music.mount,contract.musicMounts.ogg,`${game}: mount differs from product catalog`);
    const names=new Set();
    for(const file of music.files){
      assert.ok(file&&typeof file==='object'&&!Array.isArray(file),`${game}: malformed OGG file entry`);
      assert.match(file.name,/^[A-Za-z0-9_.-]+\.ogg$/i,`${game}: OGG resource must use a safe basename`);
      assert.ok(isAbsolute(file.path),`${game}/${file.name}: OGG path must be an explicit absolute local path`);
      assert.ok(!names.has(file.name),`${game}: duplicate OGG resource ${file.name}`);names.add(file.name);
      assert.equal(resolve(file.path),resolve(sourceDirectory,file.name),`${game}: OGG resource path differs from its catalog source`);
      assert.ok(Number.isSafeInteger(file.bytes)&&file.bytes>0,`${game}/${file.name}: invalid byte count`);
      assert.match(file.sha256,/^[a-f0-9]{64}$/,`${game}/${file.name}: invalid SHA-256`);
    }
    assert.deepEqual(music.files.map(file=>file.name),[...names].sort((a,b)=>a.localeCompare(b)),`${game}: OGG inventory must be sorted by filename`);
    selected[game]={sourceDirectory,mount:music.mount,files:music.files};
  }
  return selected;
}

/** Bind every manifest entry to the current complete source-directory inventory. */
export async function verifyMusicInputs(manifest,{repoRoot,games,catalog=PRODUCT_GAMES}={}) {
  const selected=validateMusicManifest(manifest,{repoRoot,games,catalog}),identities={};
  for(const [game,input] of Object.entries(selected)){
    const actual=(await readdir(input.sourceDirectory,{withFileTypes:true}))
      .filter(entry=>entry.isFile()&&/\.ogg$/i.test(entry.name)).map(entry=>entry.name).sort((a,b)=>a.localeCompare(b));
    assert.deepEqual(input.files.map(file=>file.name),actual,`${game}: OGG source inventory changed or is incomplete`);
    identities[game]={sourceDirectory:input.sourceDirectory,mount:input.mount,files:[]};
    for(const file of input.files){
      const path=resolve(file.path),info=await stat(path),bytes=await readFile(path);
      assert.ok(info.isFile(),`${game}/${file.name}: OGG resource must be a regular file`);
      assert.equal(bytes.length,file.bytes,`${game}/${file.name}: OGG byte count changed`);
      assert.equal(digest(bytes),file.sha256,`${game}/${file.name}: OGG SHA-256 changed`);
      identities[game].files.push({name:file.name,path,bytes:file.bytes,sha256:file.sha256});
    }
  }
  return identities;
}

export function runtimeMusicResources(game,identity,{base}) {
  assert.ok(base,'Runtime fixture base URL is required');
  const input=identity?.[game];assert.ok(input?.mount&&Array.isArray(input.files)&&input.files.length>0,`${game}: verified OGG inputs are required`);
  return input.files.map(file=>({path:`${input.mount}/${file.name}`,url:`${base}/music/${game}/${file.name}`,bytes:file.bytes}));
}

/** Interpret only native fields whose exact ABI width and index are source-verified. */
export function describeNativeMusic(game,raw,{th11Tracks={},th15Tracks=[]}={}) {
  if(game==='th11'){
    assert.equal(raw?.native?.source,'th11_audio_statistics');
    assert.equal(raw.native.words?.length,9,'TH11 audio statistics ABI contains exactly nine words');
    const cue=raw.native.words[5]===0xffffffff?-1:raw.native.words[5];
    const stem=cue<0?null:th11Tracks[String(cue)]??null;
    return {nativeSource:raw.native.source,wordCount:9,trackIndex:cue,trackFile:stem?`${stem}.ogg`:null,
      cursor:null,paused:raw.native.words[8]!==0,nativePcmRms:null,decoder:raw.decoder??null};
  }
  if(game==='th15'){
    assert.equal(raw?.native?.source,'th15_audio_statistics');
    assert.equal(raw.native.words?.length,11,'TH15 audio statistics ABI contains exactly eleven words');
    const track=raw.native.words[5]===0xffffffff?-1:raw.native.words[5];
    return {nativeSource:raw.native.source,wordCount:11,trackIndex:track,
      trackFile:track<0?null:th15Tracks[track]??null,cursor:raw.native.words[9],paused:raw.native.words[8]!==0,
      nativePcmRms:null,decoder:raw.decoder??null};
  }
  assert.ok(['th08','th10','th20'].includes(game),`Unsupported native music ABI ${game}`);
  return {nativeSource:raw?.native?.source??null,wordCount:raw?.native?.words?.length??0,trackIndex:null,trackFile:null,
    cursor:null,paused:null,nativePcmRms:raw?.native?.rms??null,decoder:raw?.decoder??null,
    identityStatus:'partial-no-proven-track-id'};
}

export function summarizeMusicPcm(captures=[],{contextIndex}={}) {
  const usable=captures.filter(item=>item&&typeof item==='object'&&(contextIndex===undefined||item.contextIndex===contextIndex));
  return {captureCount:usable.length,contexts:[...new Set(usable.map(item=>item.contextState).filter(Boolean))],
    contextIndices:[...new Set(usable.map(item=>item.contextIndex).filter(Number.isInteger))],
    sampleRate:usable.find(item=>Number.isFinite(item.sampleRate))?.sampleRate??null,
    peakAbs:usable.reduce((max,item)=>Math.max(max,Number(item.peakAbs)||0),0),
    maxRms:usable.reduce((max,item)=>Math.max(max,Number(item.rms)||0),0),
    nonZeroSamples:usable.reduce((sum,item)=>sum+(Number(item.nonZero)||0),0),
    outputObserved:usable.some(item=>Number(item.nonZero)>0)};
}

export function runtimePcmTargetIdentity(target){
  if(!target||target.available!==true||!Number.isInteger(target.contextIndex)||target.contextIndex<0||
     !Array.isArray(target.tapIds)||!target.tapIds.length||new Set(target.tapIds).size!==target.tapIds.length||
     target.tapIds.some(id=>!Number.isInteger(id)||id<0)||
     !Number.isFinite(target.sampleRate)||target.sampleRate<=0)
    return {ok:false,reason:target?.reason??'target-context-or-output-tap-is-unavailable'};
  const source=target.sourceIdentity;
  if(source!==undefined&&(!source||!Number.isSafeInteger(source.nodeId)||source.nodeId<0||
     typeof source.nodeType!=='string'||!source.nodeType||typeof source.role!=='string'||!source.role))
    return {ok:false,reason:'target-source-node-identity-is-invalid'};
  return {ok:true,identity:{role:target.role,contextIndex:target.contextIndex,tapIds:[...target.tapIds].sort((a,b)=>a-b),sampleRate:target.sampleRate,
    ...(source?{sourceIdentity:{nodeId:source.nodeId,nodeType:source.nodeType,role:source.role}}:{})}};
}
const targetPcmIdentity=runtimePcmTargetIdentity;
function targetStateLogIsComplete(target){
  return Number.isSafeInteger(target?.stateChangeCount)&&target.stateChangeCount>=0&&
    Number.isSafeInteger(target?.stateChangeDrops)&&target.stateChangeDrops>=0&&Array.isArray(target?.stateChanges)&&
    target.stateChangeCount===target.stateChanges.length+target.stateChangeDrops&&
    target.stateChanges.every((change,index)=>change?.sequence===index+1&&typeof change.state==='string'&&Number.isFinite(change.currentTime));
}

/** Verify sampled PCM against one identified audio context and its output taps. */
export function summarizeRuntimePcmWindow(windows,{role,expectedTarget,minimumWindows=1}={}){
  const issues=[];
  if(typeof role!=='string'||!role)issues.push('audio window requires an explicit target role');
  if(!Array.isArray(windows))issues.push('audio window samples are missing');
  const samples=Array.isArray(windows)?windows:[];
  const first=samples[0]?.target;
  const expected=expectedTarget??first;
  const expectedResult=targetPcmIdentity(expected);
  if(!expectedResult.ok)issues.push(expectedResult.reason);
  const identity=expectedResult.ok?expectedResult.identity:null;
  if(identity&&identity.role!==role)issues.push('target audio context role does not match the requested window');
  if(!Number.isInteger(minimumWindows)||minimumWindows<1)issues.push('minimum audio window count is invalid');
  if(samples.length<(Number.isInteger(minimumWindows)&&minimumWindows>0?minimumWindows:1))
    issues.push(`audio window has ${samples.length} of ${minimumWindows} required samples`);
  const selected=[];let previous=null;
  for(let index=0;index<samples.length;index++){
    const sample=samples[index],target=sample?.target;
    const actual=targetPcmIdentity(target);
    if(!actual.ok){issues.push(`sample ${index}: ${actual.reason}`);continue;}
    if(!identity||JSON.stringify(actual.identity)!==JSON.stringify(identity))issues.push(`sample ${index}: target context or tap identity changed`);
    if(target.role!==role)issues.push(`sample ${index}: target role changed`);
    if(target.state!=='running')issues.push(`sample ${index}: target context state is ${target.state??'missing'}`);
    if(!Number.isFinite(target.currentTime))issues.push(`sample ${index}: target AudioContext time is missing`);
    if(Number(target.stateChangeDrops)>0)issues.push(`sample ${index}: target state-change log overflowed`);
    if(!targetStateLogIsComplete(target))
      issues.push(`sample ${index}: target state-change sequence is incomplete`);
    if(previous){
      if(!Number.isFinite(target.currentTime)||!Number.isFinite(previous.currentTime)||target.currentTime<=previous.currentTime)
        issues.push(`sample ${index}: target AudioContext clock did not advance`);
      if(Number(target.stateChangeCount)<Number(previous.stateChangeCount)||Number(target.stateChangeDrops)!==Number(previous.stateChangeDrops))
        issues.push(`sample ${index}: target state-change sequence is not continuous`);
      const changes=(target.stateChanges??[]).filter(change=>change.sequence>previous.stateChangeCount);
      if(changes.some(change=>change.state!=='running'))issues.push(`sample ${index}: target context left running during the sampled window`);
    }
    const tapIds=actual.identity.tapIds;
    const tapCaptures=(Array.isArray(sample?.captures)?sample.captures:[])
      .filter(capture=>capture?.contextIndex===actual.identity.contextIndex&&tapIds.includes(capture?.tapId));
    for(const tapId of tapIds){
      const rows=tapCaptures.filter(capture=>capture.tapId===tapId);
      if(rows.length!==1){issues.push(`sample ${index}: target output tap ${tapId} has ${rows.length} captures`);continue;}
      const capture=rows[0];
      if(capture.contextState!=='running')issues.push(`sample ${index}: target output tap ${tapId} state is ${capture.contextState??'missing'}`);
      if(capture.sampleRate!==identity.sampleRate)issues.push(`sample ${index}: target output tap ${tapId} sample rate changed`);
      if(identity.sourceIdentity&&JSON.stringify(capture.sourceIdentity)!==JSON.stringify(identity.sourceIdentity))
        issues.push(`sample ${index}: target output tap ${tapId} source node identity changed`);
      if(!Number.isFinite(capture.currentTime)||!Number.isSafeInteger(capture.nonZero)||capture.nonZero<0||
         !Number.isFinite(capture.peakAbs)||!Number.isFinite(capture.rms))
        issues.push(`sample ${index}: target output tap ${tapId} capture is incomplete`);
      selected.push(capture);
    }
    if(previous&&Number.isSafeInteger(target.stateChangeCount)&&Array.isArray(target.stateChanges)&&
       target.stateChangeDrops===0&&target.stateChangeCount===previous.stateChangeCount&&
       Number.isFinite(target.currentTime)&&target.currentTime>previous.currentTime)previous=target;
    else if(!previous&&Number.isFinite(target.currentTime)&&Number.isSafeInteger(target.stateChangeCount))previous=target;
  }
  const nonZeroSamples=selected.reduce((sum,capture)=>sum+capture.nonZero,0);
  const analysis={status:issues.length?'incomplete':'complete',role,identity,windowCount:samples.length,captureCount:selected.length,
    sampleRate:identity?.sampleRate??null,contextIndices:identity?[identity.contextIndex]:[],tapIds:identity?.tapIds??[],
    contextStates:samples.map(sample=>sample?.target?.state??null),firstCurrentTime:samples[0]?.target?.currentTime??null,
    lastCurrentTime:samples.at(-1)?.target?.currentTime??null,
    elapsedFrames:identity&&samples.length>1&&Number.isFinite(samples.at(-1)?.target?.currentTime)&&Number.isFinite(samples[0]?.target?.currentTime)?
      (samples.at(-1).target.currentTime-samples[0].target.currentTime)*identity.sampleRate:null,
    nonZeroSamples,peakAbs:selected.reduce((max,capture)=>Math.max(max,capture.peakAbs),0),
    maxRms:selected.reduce((max,capture)=>Math.max(max,capture.rms),0),outputObserved:nonZeroSamples>0,
    windows:samples.map((sample,index)=>({index,target:{role:sample?.target?.role??null,available:sample?.target?.available??false,
      contextIndex:sample?.target?.contextIndex??null,state:sample?.target?.state??null,sampleRate:sample?.target?.sampleRate??null,
      sourceIdentity:sample?.target?.sourceIdentity??null,
      currentTime:sample?.target?.currentTime??null,stateChangeCount:sample?.target?.stateChangeCount??null,
      stateChangeDrops:sample?.target?.stateChangeDrops??null,stateChanges:sample?.target?.stateChanges??[]},taps:(Array.isArray(sample?.captures)?sample.captures:[])
        .filter(capture=>identity&&capture?.contextIndex===identity.contextIndex&&identity.tapIds.includes(capture?.tapId))
        .map(capture=>({tapId:capture.tapId,contextState:capture.contextState,sampleRate:capture.sampleRate,currentTime:capture.currentTime,
          sourceIdentity:capture.sourceIdentity??null,samples:capture.samples,nonZero:capture.nonZero,peakAbs:capture.peakAbs,rms:capture.rms}))})),
    issues:[...new Set(issues)]};
  analysis.reason=analysis.issues[0]??null;
  return analysis;
}

/** A closed-context receipt is lifecycle evidence, not a PCM sample window. */
export function verifyRuntimeAudioClosedReceipt(before,after,{role}={}){
  const a=targetPcmIdentity(before),b=targetPcmIdentity(after),issues=[];
  if(!a.ok)issues.push(`pre-close target: ${a.reason}`);
  if(!b.ok)issues.push(`post-close target: ${b.reason}`);
  if(before?.role!==role||after?.role!==role)issues.push('closed-context receipt has the wrong target role');
  if(before?.state!=='running')issues.push('closed-context receipt lacks a running pre-close target');
  if(after?.state!=='closed')issues.push('target audio context did not reach closed state');
  if(a.ok&&b.ok&&JSON.stringify(a.identity)!==JSON.stringify(b.identity))issues.push('closed-context target identity changed');
  if(Number(before?.stateChangeDrops)>0||Number(after?.stateChangeDrops)>0)issues.push('closed-context state-change log overflowed');
  if(!targetStateLogIsComplete(before)||!targetStateLogIsComplete(after))issues.push('closed-context state-change sequence is incomplete');
  if(Number.isSafeInteger(before?.stateChangeCount)&&Number.isSafeInteger(after?.stateChangeCount)&&after.stateChangeCount<before.stateChangeCount)
    issues.push('closed-context state-change sequence moved backwards');
  if(Array.isArray(before?.stateChanges)&&Array.isArray(after?.stateChanges)&&
     before.stateChanges.some((change,index)=>JSON.stringify(change)!==JSON.stringify(after.stateChanges[index])))
    issues.push('closed-context state-change history does not preserve the pre-close prefix');
  const closingEvent=(after?.stateChanges??[]).some(change=>change.state==='closed'&&change.sequence>Number(before?.stateChangeCount));
  if(!closingEvent)issues.push('closed-context receipt lacks the target context closed transition');
  return {status:issues.length?'incomplete':'passed',kind:'runtime-audio-context-closed',role,
    identity:b.ok?b.identity:null,beforeState:before?.state??null,afterState:after?.state??null,
    beforeCurrentTime:before?.currentTime??null,afterCurrentTime:after?.currentTime??null,
    beforeStateChangeCount:before?.stateChangeCount??null,afterStateChangeCount:after?.stateChangeCount??null,
    issues,reason:issues[0]??null};
}

/** Capture PCM only from the expected, active OGG source until a real output sample appears. */
export async function collectExpectedTrackRuntimePcm({readSample,wait,role,expectedFile=null,maxSamples=32,
  pollIntervalMs=RUNTIME_PCM_SAMPLE_INTERVAL_MS}={}){
  const incomplete=(reason,details={})=>({status:'incomplete',reason,role,expectedFile,attempts:details.attempts??0,maxSamples,
    pollIntervalMs,pcmWindow:details.pcmWindow??null,...details});
  if(typeof readSample!=='function'||typeof wait!=='function'||typeof role!=='string'||!role||
     !Number.isSafeInteger(maxSamples)||maxSamples<1||!Number.isFinite(pollIntervalMs)||pollIntervalMs<0)
    return incomplete('active-track PCM collection parameters are invalid');
  const samples=[];let last=null,pcmObservedAtAttempt=null,pcmSourceFile=null;
  let expectedTarget=null;
  for(let index=0;index<maxSamples;index++){
    const attempt=index+1;
    try{last=await readSample();}catch(error){return incomplete(`active-track PCM source read failed: ${String(error?.stack||error)}`,
      {attempts:attempt,lastSample:last,pcmWindow:samples.length?summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows:1}):null});}
    const native=last?.native,trackFile=last?.trackFile??null;
    const nativeReady=native?.exists===true&&native.soundReady===true&&native.pending===false&&native.playing===true&&native.outputPaused===false;
    const expectedSource=nativeReady&&typeof trackFile==='string'&&(!expectedFile||trackFile===expectedFile);
    if(expectedFile&&samples.length>0&&!expectedSource){
      const pcmWindow=summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows:1});
      return incomplete(`expected track ${expectedFile} stopped or changed before non-zero PCM was observed`,
        {attempts:attempt,lastSample:last,observedFile:trackFile,pcmWindow});
    }
    if(expectedSource&&last?.target?.available===true){
      if(!expectedTarget)expectedTarget=last.target;
      samples.push({target:last.target,captures:last.captures});
      const current=last.captures?.find(capture=>capture?.contextIndex===last.target.contextIndex&&
        last.target.tapIds?.includes(capture.tapId)&&capture.nonZero>0);
      if(current&&pcmObservedAtAttempt===null){pcmObservedAtAttempt=attempt;pcmSourceFile=trackFile;}
      const pcmWindow=summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows:1});
      if(pcmObservedAtAttempt!==null)return {status:'observed',role,expectedFile,observedFile:trackFile,nativeReady,
        attempts:attempt,maxSamples,pollIntervalMs,pcmObservedAtAttempt,pcmSourceFile,windowCount:pcmWindow.windowCount,
        captureCount:pcmWindow.captureCount,firstCurrentTime:pcmWindow.firstCurrentTime,lastCurrentTime:pcmWindow.lastCurrentTime,
        elapsedFrames:pcmWindow.elapsedFrames,sampleRate:pcmWindow.sampleRate,outputObserved:pcmWindow.outputObserved,
        nonZeroSamples:pcmWindow.nonZeroSamples,target:expectedTarget,pcmWindow,lastSample:last};
    }
    if(index+1<maxSamples){try{await wait(pollIntervalMs);}catch(error){return incomplete(`active-track PCM wait failed: ${String(error?.stack||error)}`,
      {attempts:attempt,lastSample:last,pcmWindow:samples.length?summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows:1}):null});}}
  }
  const pcmWindow=samples.length?summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows:1}):null;
  if(pcmWindow?.status==='complete')return {status:'bounded-no-output',role,expectedFile,observedFile:last?.trackFile??null,
    nativeReady:last?.native?.exists===true&&last.native.soundReady===true&&last.native.pending===false&&
      last.native.playing===true&&last.native.outputPaused===false,attempts:maxSamples,maxSamples,pollIntervalMs,
    pcmObservedAtAttempt:null,pcmSourceFile:null,windowCount:pcmWindow.windowCount,captureCount:pcmWindow.captureCount,
    firstCurrentTime:pcmWindow.firstCurrentTime,lastCurrentTime:pcmWindow.lastCurrentTime,elapsedFrames:pcmWindow.elapsedFrames,
    sampleRate:pcmWindow.sampleRate,outputObserved:false,nonZeroSamples:pcmWindow.nonZeroSamples,target:expectedTarget,pcmWindow,
    reason:`No non-zero PCM was captured from the expected playing track in ${maxSamples} bounded samples`,lastSample:last};
  return incomplete(pcmWindow?.reason??'Expected playing track did not provide an identified Runtime PCM tap',
    {attempts:maxSamples,lastSample:last,target:expectedTarget,pcmWindow});
}

/** Observe output even when a rejected source never becomes playable. */
export async function collectRuntimeOutputPcm({readSample,wait,role,windowCount=8,settleFrames=4096}={}){
  let lastSample=null;
  const collection=await collectRuntimePcmWindow({role,windowCount,settleFrames,wait,
    readWindow:async()=>{
      lastSample=await readSample();
      return {target:lastSample?.target,captures:lastSample?.captures??[]};
    }});
  return {lastSample,collection};
}

/** Collect actual browser PCM windows using the same target and timing rules as the Node regressions. */
export async function collectRuntimePcmWindow({readWindow,wait,role,expectedTarget=null,windowCount=8,settleFrames=0,
  settleTimeoutMs=3000,interWindowWaitMs=RUNTIME_PCM_SAMPLE_INTERVAL_MS,pollIntervalMs=25,retainCaptureRows=false}={}){
  const incomplete=(reason,details={})=>({status:'incomplete',proof:'none',role,reason,...details});
  if(typeof readWindow!=='function'||typeof wait!=='function')return incomplete('audio window reader and wait callback are required');
  if(typeof role!=='string'||!role||!Number.isInteger(windowCount)||windowCount<1||!Number.isFinite(settleFrames)||settleFrames<0)
    return incomplete('audio window collection parameters are invalid');
  const read=async()=>{try{return await readWindow();}catch(error){throw Error(`Runtime audio target read failed: ${String(error?.stack||error)}`);}};
  let first;
  try{first=await read();}catch(error){return incomplete(String(error));}
  const sourceTarget=expectedTarget??first?.target;
  const sourceIdentity=targetPcmIdentity(sourceTarget);
  if(!sourceIdentity.ok)return incomplete(sourceIdentity.reason,{target:sourceTarget??null,settle:{status:'unavailable'}});
  if(sourceTarget.role!==role)return incomplete('pre-close target role does not match the requested PCM window',{target:sourceTarget});
  const partialPcmWindow=samples=>samples.length?summarizeRuntimePcmWindow(samples,{role,expectedTarget:sourceTarget,minimumWindows:windowCount}):null;
  const sameIdentity=target=>{
    const value=targetPcmIdentity(target);
    return value.ok&&JSON.stringify(value.identity)===JSON.stringify(sourceIdentity.identity)&&target.role===role;
  };
  const settle={status:settleFrames===0?'not-requested':'running',requiredFrames:settleFrames,
    startedAtTime:first?.target?.currentTime??null,startedState:first?.target?.state??null,
    completedAtTime:null,elapsedFrames:null,polls:0};
  const closed=(target,windowsBeforeClose=[])=>{
    const receipt=verifyRuntimeAudioClosedReceipt(sourceTarget,target,{role});
    return receipt.status==='passed'?{status:'closed',proof:'closed-context-lifecycle',role,quiet:true,
      pcmWindow:{status:'incomplete',reason:'closed audio context does not produce a post-close PCM sample window',windowCount:0,captureCount:0,
        outputObserved:null,issues:['closed audio context does not produce a post-close PCM sample window']},
      preClosePcmWindow:partialPcmWindow(windowsBeforeClose),
      lifecycleReceipt:receipt,settle,target}:incomplete('target context closed without a valid identity-matched lifecycle receipt',
        {target,lifecycleReceipt:receipt,settle,pcmWindow:partialPcmWindow(windowsBeforeClose)});
  };
  const unusable=(sample,index,phase,windowsBeforeSample=null)=>{
    const target=sample?.target;
    const observed=windowsBeforeSample===null?null:[...windowsBeforeSample,sample];
    const partial=observed?{pcmWindow:partialPcmWindow(observed)}:{};
    if(!sameIdentity(target))return incomplete(`${phase} sample ${index}: target context or tap identity changed`,{target,settle,...partial});
    if(target.state==='closed')return closed(target,windowsBeforeSample??[]);
    if(target.state!=='running')return incomplete(`${phase} sample ${index}: target context state is ${target.state??'missing'}`,{target,settle,...partial});
    if(Number(target.stateChangeDrops)>0)return incomplete(`${phase} sample ${index}: target state-change log overflowed`,{target,settle,...partial});
    if(!targetStateLogIsComplete(target))return incomplete(`${phase} sample ${index}: target state-change sequence is incomplete`,{target,settle,...partial});
    return null;
  };
  const firstIssue=unusable(first,0,'initial',[]);if(firstIssue)return firstIssue;
  let settlingSample=first;
  if(settleFrames>0){
    if(!Number.isFinite(first.target.currentTime)||!Number.isFinite(first.target.sampleRate)||first.target.sampleRate<=0)
      return incomplete('audio clock or sample rate is unavailable for settling',{target:first.target,settle});
    const began=Date.now(),startTime=first.target.currentTime,requiredTime=settleFrames/first.target.sampleRate;
    while(true){
      const elapsed=(settlingSample.target.currentTime-startTime)*first.target.sampleRate;
      if(elapsed>=settleFrames)break;
      if(Date.now()-began>=settleTimeoutMs)return incomplete('target AudioContext clock did not advance through the required settle frames',
        {settle:{...settle,status:'incomplete',completedAtTime:settlingSample.target.currentTime,elapsedFrames:elapsed},target:settlingSample.target});
      try{await wait(pollIntervalMs);settlingSample=await read();}catch(error){return incomplete(String(error),{settle,target:settlingSample?.target??null});}
      settle.polls++;
      const issue=unusable(settlingSample,settle.polls,'settle');if(issue)return issue;
      const changes=(settlingSample.target.stateChanges??[]).filter(change=>change.sequence>Number(first.target.stateChangeCount));
      if(changes.some(change=>change.state!=='running'))return incomplete('target context left running during audio-window settling',
        {settle:{...settle,status:'incomplete',completedAtTime:settlingSample.target.currentTime,
          elapsedFrames:(settlingSample.target.currentTime-startTime)*first.target.sampleRate},target:settlingSample.target});
    }
    settle.status='complete';settle.completedAtTime=settlingSample.target.currentTime;
    settle.elapsedFrames=(settlingSample.target.currentTime-startTime)*first.target.sampleRate;
    settle.requiredTimeSeconds=requiredTime;
  }
  const samples=[];
  let current=settleFrames>0?settlingSample:first;
  for(let index=0;index<windowCount;index++){
    if(index>0){
      try{await wait(interWindowWaitMs);current=await read();}catch(error){return incomplete(String(error),{settle,pcmWindow:partialPcmWindow(samples)});}
      const issue=unusable(current,index,'capture',samples);if(issue)return issue;
    }
    samples.push(current);
  }
  const pcmWindow=summarizeRuntimePcmWindow(samples,{role,expectedTarget:sourceTarget,minimumWindows:windowCount});
  if(pcmWindow.status!=='complete')return incomplete(pcmWindow.reason??'runtime PCM window is incomplete',
    {proof:'none',target:sourceTarget,settle,pcmWindow});
  return {status:'complete',proof:'runtime-output-pcm-window',role,quiet:pcmWindow.outputObserved===false,
    target:sourceIdentity.identity,settle,pcmWindow,...(retainCaptureRows?{captureRows:samples.map(sample=>({target:sample.target,
      captures:sample.captures??[],relatedTargets:sample.relatedTargets??null}))}:{})};
}

/** Reclassify another tap captured in the same Runtime read and PCM window. */
export function summarizeRelatedRuntimePcmWindow(primaryWindow,{role,relatedTargetKey,expectedTarget,minimumWindows=8}={}){
  const incomplete=(reason,pcmWindow=null)=>({status:'incomplete',proof:'none',role,reason,pcmWindow});
  if(primaryWindow?.status!=='complete'||primaryWindow?.proof!=='runtime-output-pcm-window'||
     !Array.isArray(primaryWindow.captureRows)||typeof relatedTargetKey!=='string'||!relatedTargetKey)
    return incomplete('primary PCM window does not retain same-read tap captures');
  const rows=primaryWindow.captureRows;
  const samples=rows.map((row,index)=>{
    const target=row?.relatedTargets?.[relatedTargetKey];
    return target?{target,captures:row.captures??[]}:{target:{available:false,reason:`same-read target ${relatedTargetKey} is missing at sample ${index}`},captures:row?.captures??[]};
  });
  const pcmWindow=summarizeRuntimePcmWindow(samples,{role,expectedTarget,minimumWindows});
  if(pcmWindow.status!=='complete')return incomplete(pcmWindow.reason??'same-read Runtime PCM window is incomplete',pcmWindow);
  return {status:'complete',proof:'runtime-output-pcm-window',role,quiet:pcmWindow.outputObserved===false,
    target:pcmWindow.identity,pcmWindow};
}

export function describeNoneMusicControl(pcm,{settleMs=1800,captureWindowMs=600}={}) {
  const running=pcm?.captureCount>0&&pcm.contexts?.includes('running');
  const silent=running&&pcm.nonZeroSamples===0;
  return {status:silent?'silent-after-settling':pcm?.outputObserved?'output-observed-unclassified':'no-running-output-tap',
    scope:'Runtime output PCM across observed Runtime audio taps',settleMs,captureWindowMs,
    samples:Number(pcm?.captureCount)||0,contextStates:pcm?.contexts??[]};
}

export function nativeAudioStatsShape(game) {
  if(game==='th11')return {source:'th11_audio_statistics',wordCount:9,trackIndex:5,paused:8,cursor:null};
  if(game==='th15')return {source:'th15_audio_statistics',wordCount:11,trackIndex:5,paused:8,cursor:9};
  assert.ok(['th08','th10','th20'].includes(game));
  if(game==='th08')return {source:'sdl_audio_stats + sdl_music_stats + th08 music observations',audioWordCount:12,musicWordCount:6,
    musicObservationWordCount:16,midiObservationWordCount:22,trackIndex:null,cursor:'musicObservation.cursorFrames'};
  return {source:'sdl_audio_stats + sdl_music_stats',audioWordCount:12,musicWordCount:6,trackIndex:null,cursor:null};
}

export function mapTrackToInput(trackFile,files=[]) {
  if(!trackFile)return null;
  const input=files.find(file=>file.name===trackFile);
  return input?{name:input.name,path:input.path,bytes:input.bytes,sha256:input.sha256}:null;
}
