// Read-only taps for the actual WebSfx graph. No production source is created,
// uploaded, started, stopped, rerouted, or assigned a test input by this module.
export const PRESENTATION_AUDIO_SCHEMA='eagler-presentation/audio-output/4';
export const PRESENTATION_AUDIO_CLOCK_SCHEMA='eagler-presentation/audio-frame-clock/2';
export const PRESENTATION_AUDIO_LIMITS=Object.freeze({records:512,historySeconds:2,prefixFrames:128,
  silentTailFrames:512,segmentSeconds:32,segments:225,localBase:1048576,sourceSlotsPerVoice:2});
export const TH08_PRESENTATION_AUDIO_CONTRACT=Object.freeze({id:'th08/presentation-audio/1',game:'th08',variant:'normal',
  enabledGlobal:'__th08SfxLatencyProbeEnabled',traceGlobal:'__th08SfxLatencyTrace',probeGlobal:'__th08SfxLatencyProbe',
  logicalSounds:46,handleOffset:1,gainCallsPerChannel:4,
  nativeEventBoundary:'SoundEffects::process after deduplication and controls, before output.play',
  gainBoundary:'SoundEffects pan and volume each call AudioHost::controls, which calls WebSfx volume and pan'});

const finite=Number.isFinite;
const integer=Number.isSafeInteger;
const copy=value=>JSON.parse(JSON.stringify(value));
const sleep=ms=>new Promise(resolve=>setTimeout(resolve,ms));
function requireValue(test,message){if(!test)throw Error(message);}

/** Lease source taps by generation; a previous generation must finish its raw receipts before reuse. */
export function leasePresentationAudioSourceSlot(slot,{sourceId,eventSerial}={}){
  if(!slot||slot.lease!==null)return null;
  requireValue(integer(slot.generation)&&slot.generation>=0&&integer(slot.lastReceiptSequence)&&slot.lastReceiptSequence>=0,
    'Invalid source-tap lease state');
  const lease={sourceId,eventSerial,slotGeneration:++slot.generation,previousSlotReceiptSequence:slot.lastReceiptSequence,
    previousSlotEnded:slot.lastEnded??null};slot.lease=lease;return lease;
}

/** A source tap becomes reusable only after the node ended and both initial PCM paths returned. */
export function releasePresentationAudioSourceSlot(slot,lease,{ended,sourceReceiptDone,outputReceiptDone,continuationPending=false,
  sourceReceiptSequence,endedRow}={}){
  if(!slot||slot.lease!==lease||!ended||!sourceReceiptDone||!outputReceiptDone||continuationPending)return false;
  requireValue(integer(sourceReceiptSequence)&&sourceReceiptSequence>0&&endedRow?.kind==='source-node-ended',
    'Source-tap release is missing its raw receipt or actual ended boundary');
  slot.lastReceiptSequence=sourceReceiptSequence;slot.lastEnded=endedRow;slot.lease=null;return true;
}

/** The command order at a shared timestamp is part of the clock contract. */
export function presentationAudioFrameClockPlan({sampleRate,anchorTime,segments=PRESENTATION_AUDIO_LIMITS.segments}={}){
  requireValue(integer(sampleRate)&&sampleRate>=8000&&sampleRate<=192000,'Invalid AudioContext sample rate');
  requireValue(integer(anchorTime)&&anchorTime>=1,'Clock anchor must be an integer AudioContext second');
  requireValue(integer(segments)&&segments>=1&&segments<=PRESENTATION_AUDIO_LIMITS.segments,'Invalid bounded clock horizon');
  const segmentFrames=sampleRate*PRESENTATION_AUDIO_LIMITS.segmentSeconds,base=PRESENTATION_AUDIO_LIMITS.localBase;
  requireValue(base+segmentFrames<2**24,'Local AudioParam frames exceed exact float32 integers');
  const local=[['setValueAtTime',base,anchorTime]],segment=[['setValueAtTime',0,anchorTime]];
  for(let index=0;index<segments;index++){
    const time=anchorTime+(index+1)*PRESENTATION_AUDIO_LIMITS.segmentSeconds;
    local.push(['linearRampToValueAtTime',base+segmentFrames,time]);
    if(index+1<segments){local.push(['setValueAtTime',base,time]);segment.push(['setValueAtTime',index+1,time]);}
  }
  return {schema:PRESENTATION_AUDIO_CLOCK_SCHEMA,sampleRate,anchorTime,anchorFrame:anchorTime*sampleRate,
    segmentSeconds:PRESENTATION_AUDIO_LIMITS.segmentSeconds,segmentFrames,segments,localBase:base,
    endTime:anchorTime+segments*PRESENTATION_AUDIO_LIMITS.segmentSeconds,
    endFrame:(anchorTime+segments*PRESENTATION_AUDIO_LIMITS.segmentSeconds)*sampleRate,
    automationRate:'a-rate',parameters:{localFrame:local,segment}};
}

export function configurePresentationAudioClock(node,plan,context){
  const startedAt=context.currentTime;
  for(const [name,commands]of Object.entries(plan.parameters)){
    const parameter=node.parameters.get(name);
    requireValue(parameter&&parameter.automationRate==='a-rate',`Missing actual a-rate ${name}`);
    for(const [method,value,time]of commands)parameter[method](value,time);
  }
  const finishedAt=context.currentTime;
  requireValue(startedAt<=finishedAt&&finishedAt<plan.anchorTime,'Audio observation clock was not configured before its anchor');
  return {startedAt,finishedAt,automationRate:{localFrame:node.parameters.get('localFrame').automationRate,
    segment:node.parameters.get('segment').automationRate}};
}

/** Decode retained parameter arrays, including the permitted constant len=1 form. */
export function decodePresentationAudioClockBlock(block,plan){
  requireValue(plan?.schema===PRESENTATION_AUDIO_CLOCK_SCHEMA,'Unknown PCM frame clock');
  const size=block?.quantumFrames,offset=block?.validOffset;
  requireValue(integer(size)&&size>0&&size<=2048&&integer(offset)&&offset>=0&&offset<size,'Invalid actual render quantum');
  requireValue(Array.isArray(block.local)&&block.local.length===size,'Missing actual per-sample local-frame array');
  requireValue(Array.isArray(block.segment)&&(block.segment.length===1||block.segment.length===size),'Missing actual segment array');
  let first=null;
  for(let i=offset;i<size;i++){
    const local=block.local[i],segment=block.segment.length===1?block.segment[0]:block.segment[i];
    requireValue(integer(local)&&local>=plan.localBase&&local<plan.localBase+plan.segmentFrames,'Invalid exact local sample frame');
    requireValue(integer(segment)&&segment>=0&&segment<plan.segments,'Invalid exact frame-clock segment');
    const frame=plan.anchorFrame+segment*plan.segmentFrames+local-plan.localBase;
    requireValue(integer(frame)&&frame>=plan.anchorFrame&&frame<plan.endFrame,'PCM sample lies outside clock horizon');
    if(first===null)first=frame;
    requireValue(frame===first+i-offset,'Noncontinuous a-rate sample clock');
  }
  requireValue(block.frame===first&&block.frames===size-offset,'PCM block frame does not match its real parameters');
  return {firstFrame:first,endFrame:first+size-offset};
}

/** Used verbatim by the Runtime collector and by the real OfflineAudioContext probe. */
export function presentationAudioPcmProcessorSource(){
  return `class PresentationAudioPcmTap extends AudioWorkletProcessor {
    static get parameterDescriptors(){return [
      {name:'localFrame',defaultValue:0,minValue:0,maxValue:16777215,automationRate:'a-rate'},
      {name:'segment',defaultValue:-1,minValue:-1,maxValue:1024,automationRate:'a-rate'}];}
    constructor(){super();this.plan=null;this.history=[];this.retained=0;this.pending=[];
      this.sequence=0;this.previousEnd=null;this.ready=false;this.failures=0;this.enabled=true;
      this.port.onmessage=e=>{const m=e.data||{};
        if(m.kind==='configure'){if(this.plan){this.error('duplicate-clock-configuration');return;}this.plan=m.plan;this.historyLimit=m.historyFrames;
          this.port.postMessage({kind:'configured',clockSchema:this.plan.schema,anchorFrame:this.plan.anchorFrame});}
        else if(m.kind==='mark'){if(this.pending.length>=512)this.error('pending-marker-overflow');else{this.pending.push({...m});
          this.port.postMessage({kind:'armed',eventSerial:m.eventSerial,purpose:m.purpose??'event-audio',mode:m.mode,
            sourceId:m.sourceId??null,slotGeneration:m.slotGeneration??null,startFrame:m.startFrame??null,endFrame:m.endFrame??null,
            processSequence:this.sequence,currentFrame});}}
        else if(m.kind==='flush')this.port.postMessage({kind:'flushed',tag:m.tag,processSequence:this.sequence,previousEnd:this.previousEnd,failures:this.failures,pending:this.pending.length,currentFrame});
        else if(m.kind==='dispose')this.enabled=false;};}
    error(reason,detail={}){this.failures++;if(this.failures<=64)this.port.postMessage({kind:'error',reason,detail,processSequence:this.sequence,currentFrame});
      else if(this.failures===65)this.port.postMessage({kind:'overflow',count:1});}
    completeMarks(){
      for(let index=0;index<this.pending.length;){const mark=this.pending[index];
        if(!this.history.length){index++;continue;}
        const first=this.history[0].frame,last=this.history[this.history.length-1],end=last.frame+last.frames;
        if(mark.startFrame<first){this.port.postMessage({kind:'gap',eventSerial:mark.eventSerial,purpose:mark.purpose??'event-audio',mode:mark.mode,sourceId:mark.sourceId??null,
          slotGeneration:mark.slotGeneration??null,reason:'marker-outside-continuous-history',startFrame:mark.startFrame,historyStartFrame:first});this.pending.splice(index,1);continue;}
        if(mark.startFrame>=end){index++;continue;}
        if(mark.mode==='onset'&&mark.onsetFrame===undefined){
          let through=mark.startFrame,found=null;
          for(const block of this.history){const from=Math.max(mark.startFrame,block.frame),to=block.frame+block.frames;
            if(from>=to)continue;if(from!==through){this.error('source-onset-history-gap',{eventSerial:mark.eventSerial,through,from});break;}
            for(let frame=from;frame<to;frame++){const j=frame-block.frame+block.validOffset;
              if(block.left[j]!==0||block.right[j]!==0){found=frame;break;}}
            through=to;if(found!==null)break;}
          if(found!==null){mark.onsetFrame=found;mark.endFrame=found+mark.prefixFrames;}
            else if(end>=mark.startFrame+mark.budgetFrames){this.port.postMessage({kind:'gap',eventSerial:mark.eventSerial,purpose:mark.purpose??'event-audio',mode:mark.mode,sourceId:mark.sourceId??null,
              slotGeneration:mark.slotGeneration??null,reason:'actual-source-rendered-no-nonzero-pcm',startFrame:mark.startFrame,throughFrame:end});this.pending.splice(index,1);continue;}
        }
        if(mark.endFrame===undefined||end<mark.endFrame){index++;continue;}
        const blocks=this.history.filter(b=>b.frame+b.frames>mark.startFrame&&b.frame<mark.endFrame).map(b=>({
          processSequence:b.processSequence,currentFrame:b.currentFrame,quantumFrames:b.quantumFrames,validOffset:b.validOffset,
          frame:b.frame,frames:b.frames,local:b.local,segment:b.segment,inputChannels:b.inputChannels,
          leftWords:Array.from(new Uint32Array(b.left.buffer)),rightWords:Array.from(new Uint32Array(b.right.buffer))}));
        this.port.postMessage({kind:'pcm',eventSerial:mark.eventSerial,purpose:mark.purpose??'event-audio',mode:mark.mode,startFrame:mark.startFrame,
          endFrame:mark.endFrame,onsetFrame:mark.onsetFrame??null,sourceId:mark.sourceId??null,
          slotGeneration:mark.slotGeneration??null,blocks});this.pending.splice(index,1);
      }
    }
    process(inputs,outputs,parameters){const out=outputs[0]||[];for(const channel of out)channel.fill(0);
      if(!this.enabled)return false;this.sequence++;
      const size=out[0]?.length||inputs[0]?.[0]?.length||128;
      if(!this.plan)return true;
      const local=parameters.localFrame,segment=parameters.segment,plan=this.plan;
      if(!this.ready&&local.length===1&&segment.length===1&&local[0]<plan.localBase&&segment[0]<0)return true;
      if(local.length!==size||(segment.length!==1&&segment.length!==size)){this.error('invalid-actual-parameter-array-length',{size,local:local.length,segment:segment.length});return true;}
      let offset=0;
      while(offset<size&&((segment.length===1?segment[0]:segment[offset])<0||local[offset]<plan.localBase))offset++;
      if(offset===size){if(this.ready)this.error('clock-returned-to-unarmed-state');return true;}
      let first=null;
      for(let i=offset;i<size;i++){const l=local[i],s=segment.length===1?segment[0]:segment[i];
        if(!Number.isSafeInteger(l)||l<plan.localBase||l>=plan.localBase+plan.segmentFrames||!Number.isSafeInteger(s)||s<0||s>=plan.segments){this.error('clock-outside-exact-segment',{sample:i,local:l,segment:s});return true;}
        const f=plan.anchorFrame+s*plan.segmentFrames+l-plan.localBase;
        if(first===null)first=f;if(!Number.isSafeInteger(f)||f!==first+i-offset){this.error('noncontinuous-independent-clock',{sample:i,frame:f,expected:first+i-offset});return true;}}
      if(this.previousEnd!==null&&first!==this.previousEnd){this.error('noncontinuous-clock-blocks',{previousEnd:this.previousEnd,first});return true;}
      this.previousEnd=first+size-offset;
      const input=inputs[0]||[],left=input[0]?input[0].slice():new Float32Array(size),right=input[1]?input[1].slice():new Float32Array(size);
      if(left.length!==size||right.length!==size){this.error('invalid-pcm-block-length');return true;}
      const block={processSequence:this.sequence,currentFrame,quantumFrames:size,validOffset:offset,frame:first,frames:size-offset,
        local:Array.from(local),segment:Array.from(segment),inputChannels:input.length,left,right};
      this.history.push(block);this.retained+=block.frames;
      while(this.retained>this.historyLimit&&this.history.length>1){const previous=this.history.shift();this.retained-=previous.frames;}
      if(!this.ready){this.ready=true;this.port.postMessage({kind:'ready',frame:first,processSequence:this.sequence,currentFrame,block:{
        processSequence:block.processSequence,currentFrame:block.currentFrame,quantumFrames:size,validOffset:offset,
        frame:first,frames:size-offset,local:block.local,segment:block.segment}});}
      this.completeMarks();return true;
    }
  }
  registerProcessor('presentation-audio-pcm-tap',PresentationAudioPcmTap);`;
}

async function hashJson(value){const bytes=new TextEncoder().encode(JSON.stringify(value));
  return Array.from(new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes)),v=>v.toString(16).padStart(2,'0')).join('');}
function propertyWrapper(object,key,wrapper){const descriptor=Object.getOwnPropertyDescriptor(object,key),original=object[key];
  object[key]=wrapper(original);return ()=>{if(descriptor)Object.defineProperty(object,key,descriptor);else delete object[key];};}
function valuePropertyWrapper(object,key,onSet){
  const own=Object.getOwnPropertyDescriptor(object,key);let owner=Object.getPrototypeOf(object),inherited=null;
  while(owner&&!inherited){inherited=Object.getOwnPropertyDescriptor(owner,key)??null;owner=Object.getPrototypeOf(owner);}
  let value=object[key];const getter=own?.get??inherited?.get,setter=own?.set??inherited?.set;
  Object.defineProperty(object,key,{configurable:true,enumerable:own?.enumerable??inherited?.enumerable??true,
    get(){return getter?getter.call(this):value;},set(next){onSet(next);if(setter)setter.call(this,next);else value=next;}});
  return ()=>{if(own)Object.defineProperty(object,key,own);else delete object[key];};
}

/** Install only after freezing native ticks; real wall-clock audio remains running. */
export async function installPresentationAudioProbe(runtime,{epoch,rateHz}={}){
  requireValue(integer(epoch)&&epoch>0&&[60,120,144].includes(rateHz),'Actual Runtime epoch and cadence are required');
  const contract=TH08_PRESENTATION_AUDIO_CONTRACT,Module=runtime?.Module;
  const host=Module?.canvas?.ownerDocument?.defaultView??globalThis,pool=Module?.eaglerSfxOutput,context=pool?.context;
  requireValue(host[contract.enabledGlobal]===true&&host[contract.traceGlobal],'Native SFX observation must be enabled before boot');
  requireValue(typeof host.Map==='function'&&pool?.voices instanceof host.Map&&context&&context===Module.SDL3?.audioContext,
    'Actual WebSfx AudioContext and voice pool are required');
  requireValue(context.state==='running'&&context.audioWorklet&&typeof host.AudioWorkletNode==='function','A running AudioContext with AudioWorklet is required');
  const trace=()=>host[contract.traceGlobal];
  const uploads=copy(trace().uploads);
  requireValue(uploads.length===contract.logicalSounds,'TH08 must retain all actual eager SFX uploads');
  const bindings=[],voiceByObject=new Map();
  for(let index=0;index<contract.logicalSounds;index++){
    const rows=uploads.filter(row=>row.handle===index+contract.handleOffset);
    requireValue(rows.length===1&&rows[0].uploadSuccess===true,'Missing unique successful native SFX upload');
    const upload=rows[0],voice=pool.voices.get(Number(upload.voiceKey));
    requireValue(voice&&voice.buffer&&voice.split&&voice.merge&&voice.left?.gain&&voice.right?.gain,'Native upload is not bound to its actual WebSfx voice');
    requireValue(voice.buffer.sampleRate===upload.sourceRate&&voice.buffer.length===upload.sourceFrames,'Actual AudioBuffer differs from native upload');
    const binding={logicalIndex:index,handle:upload.handle,voiceKey:upload.voiceKey,upload,voice,sourceSlots:[],controlStoppedActive:false};
    bindings.push(binding);voiceByObject.set(voice,binding);
  }
  const idleDeadline=Date.now()+20000;
  while(bindings.some(({voice})=>voice.node!==null||voice.playing)){
    requireValue(Date.now()<idleDeadline,'Existing production SFX did not naturally become idle');await sleep(10);
  }
  const raw={contract:contract.id,epoch,rateHz,clock:null,window:null,bindings:[],taps:[],hookEvents:[],hookRoutes:[],hookUploads:[],
    starts:[],gainCalls:[],gainMutations:[],voiceStops:[],intervals:[],markReceipts:[],sourcePcm:[],pcm:[],silentContinuations:[],native:null,errors:[],gaps:[],unassociatedStarts:[],dropped:0};
  const add=(list,row)=>{const limit=(list===raw.gainCalls||list===raw.gainMutations||list===raw.voiceStops||list===raw.intervals||list===raw.markReceipts)?PRESENTATION_AUDIO_LIMITS.records*8:PRESENTATION_AUDIO_LIMITS.records;
    if(list.length<limit)list.push(row);else raw.dropped++;};
  const note=(where,error)=>add(raw.errors,{where,message:String(error?.message??error)});
  const restores=[],nodes=[],activeSources=new Set(),pending=new Map(),eventsByKey=new Map(),continuationWaiters=new Map(),sourceOwners=new WeakMap(),sourceRecordsByEvent=new Map();
  let active=true,starting=null,sourceSequence=0,sourceReceiptSequence=0,gainSequence=0,finished=null,moduleURL=null;
  const failEvent=(serial,reason,detail={})=>{add(raw.gaps,{eventSerial:serial,reason,...detail});const waiter=pending.get(serial);if(waiter){waiter.resolve();pending.delete(serial);}};
  const maybeReleaseSlot=record=>{
    const slot=record?.slot;
    if(slot)releasePresentationAudioSourceSlot(slot,record.lease,{ended:record.ended,sourceReceiptDone:record.sourceReceiptDone,
      outputReceiptDone:record.outputReceiptDone,continuationPending:record.continuationPending,
      sourceReceiptSequence:record.sourceReceiptSequence,endedRow:record.endedRow});
  };
  const scheduleSilentContinuation=(binding,row)=>{
    if(continuationWaiters.has(row.eventSerial))return;
    const start=raw.starts.find(candidate=>candidate.eventSerial===row.eventSerial);
    const source=binding.sourceSlots.find(tap=>tap.entry.id===start?.sourceTap);
    if(!start||!source){note('silent-continuation',`Missing source tap for event ${row.eventSerial}`);return;}
    const startFrame=row.endFrame,endFrame=startFrame+PRESENTATION_AUDIO_LIMITS.silentTailFrames;
    const continuation={eventSerial:row.eventSerial,startFrame,endFrame,sourceTap:start.sourceTap,sourceId:start.sourceId,
      slotGeneration:start.sourceSlotGeneration,
      outputTap:binding.output.entry.id,source:null,output:null,sourceArmed:false,outputArmed:false};
    let resolve;
    continuationWaiters.set(row.eventSerial,{continuation,promise:new Promise(done=>{resolve=done;}),resolve});
    raw.silentContinuations.push(continuation);
    const record=sourceRecordsByEvent.get(row.eventSerial);if(record)record.continuationPending=true;
    source.node.port.postMessage({kind:'mark',mode:'range',purpose:'silent-tail',eventSerial:row.eventSerial,startFrame,endFrame,
      sourceId:start.sourceId,slotGeneration:start.sourceSlotGeneration});
    binding.output.node.port.postMessage({kind:'mark',mode:'range',purpose:'silent-tail',eventSerial:row.eventSerial,startFrame,endFrame,
      sourceId:start.sourceId,slotGeneration:start.sourceSlotGeneration});
  };
  const captureHasPcm=row=>[...(row.blocks??[])].some(block=>['leftWords','rightWords'].some(side=>{
    const words=block[side];if(!Array.isArray(words))return false;
    return Array.from(new Float32Array(Uint32Array.from(words).buffer)).some(sample=>sample!==0);
  }));
  const settleContinuation=(continuation,row,kind)=>{
    const waiter=continuationWaiters.get(row.eventSerial);
    if(!waiter||row.startFrame!==continuation.startFrame||row.endFrame!==continuation.endFrame||row.sourceId!==continuation.sourceId||
      row.slotGeneration!==continuation.slotGeneration)return false;
    continuation[kind]=row;
    if(continuation.source&&continuation.output){waiter.resolve();const record=sourceRecordsByEvent.get(row.eventSerial);
      if(record){record.continuationPending=false;record.continuationReceiptDone=true;maybeReleaseSlot(record);
        const eventWaiter=pending.get(row.eventSerial);if(eventWaiter){eventWaiter.outputDone=true;
          if(eventWaiter.sourceDone){eventWaiter.resolve();pending.delete(row.eventSerial);}}}}
    return true;
  };
  const createTap=(binding,kind,slot=null)=>{
    const node=new host.AudioWorkletNode(context,'presentation-audio-pcm-tap',{numberOfInputs:1,numberOfOutputs:1,
      outputChannelCount:[2],channelCount:2,channelCountMode:'explicit',channelInterpretation:'discrete'});
    const sink=context.createGain();sink.gain.value=0;node.connect(sink);sink.connect(context.destination);nodes.push({node,sink});
    const entry={id:raw.taps.length+1,voiceKey:binding.voiceKey,kind,slot,configured:null,configuredAck:null,ready:null,final:null};raw.taps.push(entry);
    const target={node,entry,binding,generation:0,lease:null,lastReceiptSequence:0,lastEnded:null};
    node.port.onmessage=event=>{const message=event.data||{};if(!active)return;
      if(message.kind==='ready'){entry.ready=copy(message);return;}
      if(message.kind==='configured'){entry.configuredAck=copy(message);return;}
      if(message.kind==='armed'){
        if(message.purpose==='silent-tail'){
          const continuation=continuationWaiters.get(message.eventSerial)?.continuation;
          if(continuation){if(kind==='source')continuation.sourceArmed=true;else continuation.outputArmed=true;}
        } else add(raw.markReceipts,{tapId:entry.id,...copy(message)});
        return;}
      if(message.kind==='flushed'){entry.final=copy(message);return;}
      if(message.kind==='error'){note(`worklet:${entry.id}`,JSON.stringify(message));return;}
      if(message.kind==='overflow'){raw.dropped+=message.count;return;}
      if(message.kind==='gap'){
        if(kind==='source'){
          const record=message.sourceId?sourceRecordsByEvent.get(message.eventSerial):null;
          if(record&&record.id===message.sourceId&&record.slotGeneration===message.slotGeneration){record.sourceReceiptDone=true;record.sourceGap=copy(message);maybeReleaseSlot(record);}
        }
        failEvent(message.eventSerial,message.reason,{tapId:entry.id,...message});return;}
      if(message.kind==='pcm'){
        const row={tapId:entry.id,voiceKey:binding.voiceKey,...copy(message)};
        const continuation=continuationWaiters.get(message.eventSerial)?.continuation;
        if(continuation&&settleContinuation(continuation,row,kind))return;
        if(kind==='source'){
          const record=sourceRecordsByEvent.get(message.eventSerial);
          if(record&&record.id===message.sourceId&&record.slotGeneration===message.slotGeneration){
            record.sourceReceiptDone=true;record.sourceReceiptSequence=++sourceReceiptSequence;
            row.slotReceiptSequence=record.sourceReceiptSequence;row.slot=record.slot?.entry.slot??null;
            add(raw.sourcePcm,row);
            record.binding.output.node.port.postMessage({kind:'mark',mode:'range',purpose:'event-audio',eventSerial:message.eventSerial,
              startFrame:message.onsetFrame,endFrame:message.endFrame,sourceId:record.id,slotGeneration:record.slotGeneration});
            const waiter=pending.get(message.eventSerial);if(waiter){waiter.sourceDone=true;
              if(waiter.outputDone){waiter.resolve();pending.delete(message.eventSerial);}}
          } else {
            add(raw.gaps,{eventSerial:message.eventSerial,reason:'source-slot-generation-mismatch',tapId:entry.id,
              sourceId:message.sourceId??null,slotGeneration:message.slotGeneration??null});
          }
          maybeReleaseSlot(record);
        } else {
          add(raw.pcm,row);const record=sourceRecordsByEvent.get(message.eventSerial);
          if(record&&record.id===message.sourceId&&record.slotGeneration===message.slotGeneration)record.outputReceiptDone=true;
          else add(raw.gaps,{eventSerial:message.eventSerial,reason:'output-source-generation-mismatch',tapId:entry.id,
            sourceId:message.sourceId??null,slotGeneration:message.slotGeneration??null});
          const needsTail=!captureHasPcm(row);
          if(needsTail)scheduleSilentContinuation(binding,row);
          maybeReleaseSlot(record);
          const waiter=pending.get(message.eventSerial);if(waiter){waiter.outputDone=!needsTail||!continuationWaiters.has(message.eventSerial);
            if(waiter.sourceDone&&waiter.outputDone){waiter.resolve();pending.delete(message.eventSerial);}}
        }
      }};
    return target;
  };
  const dispose=()=>{
    if(!active)return;active=false;
    for(const restore of restores.reverse())try{restore();}catch(error){note('restore',error);}
    for(const binding of bindings)if(binding.output)try{binding.voice.merge.disconnect(binding.output.node);}catch{}
    for(const {node,tap}of activeSources)try{node.disconnect(tap.node);}catch{}
    for(const {node,sink}of nodes){try{node.port.postMessage({kind:'dispose'});node.disconnect();sink.disconnect();node.port.close();}catch{}}
    if(moduleURL)URL.revokeObjectURL(moduleURL);
  };
  try{
    moduleURL=URL.createObjectURL(new Blob([presentationAudioPcmProcessorSource()],{type:'text/javascript'}));
    await context.audioWorklet.addModule(moduleURL);
    for(const binding of bindings){binding.output=createTap(binding,'output');binding.voice.merge.connect(binding.output.node);
      for(let slot=0;slot<PRESENTATION_AUDIO_LIMITS.sourceSlotsPerVoice;slot++)binding.sourceSlots.push(createTap(binding,'source',slot));}
    const plan=presentationAudioFrameClockPlan({sampleRate:context.sampleRate,anchorTime:Math.ceil(context.currentTime)+2});raw.clock=plan;
    for(const binding of bindings)for(const tap of [binding.output,...binding.sourceSlots]){
      tap.entry.configured=configurePresentationAudioClock(tap.node,plan,context);
      tap.node.port.postMessage({kind:'configure',plan,historyFrames:context.sampleRate*PRESENTATION_AUDIO_LIMITS.historySeconds});}
    const readinessDeadline=Date.now()+10000;
    while(raw.taps.some(tap=>!tap.ready||!tap.configuredAck)){requireValue(Date.now()<readinessDeadline&&raw.errors.length===0,'Actual PCM taps did not acknowledge the independent clock');await sleep(5);}
    for(const binding of bindings){const voice=binding.voice;
      requireValue(!voice.node&&!voice.playing,'Production voice restarted during observation setup');
      binding.outputConnected=true;
      raw.bindings.push({logicalIndex:binding.logicalIndex,handle:binding.handle,voiceKey:binding.voiceKey,upload:binding.upload,
        baseline:{contextTime:context.currentTime,leftGain:voice.left.gain.value,rightGain:voice.right.gain.value,
          volume:voice.volume,pan:voice.pan,speed:voice.speed,playing:voice.playing,nodePresent:voice.node!==null},
        graph:{sourceBuffer:'AudioBufferSourceNode',sourceRoute:'voice.split',leftGain:'voice.left.gain',rightGain:'voice.right.gain',
          outputRoute:'voice.merge',outputTapInput:'voice.merge',outputTapConnected:binding.outputConnected},
        outputTap:binding.output.entry.id,sourceTaps:binding.sourceSlots.map(tap=>tap.entry.id)});
      for(const side of ['left','right']){
        const parameter=voice[side].gain;
        restores.push(valuePropertyWrapper(parameter,'value',value=>{
          if(active)add(raw.gainMutations,{sequence:raw.gainMutations.length+1,method:'value',voiceKey:binding.voiceKey,side,value,
            beforeTime:context.currentTime,afterTime:context.currentTime});}));
        for(const method of ['setValueAtTime','linearRampToValueAtTime','exponentialRampToValueAtTime','setTargetAtTime',
          'setValueCurveAtTime','cancelScheduledValues','cancelAndHoldAtTime']){
          if(typeof parameter[method]!=='function')continue;
          restores.push(propertyWrapper(parameter,method,original=>function(...args){
            const beforeTime=context.currentTime,hadActiveSource=!!voice.node;
            const result=original.apply(this,args),afterTime=context.currentTime;
            if(active){const mutation={sequence:raw.gainMutations.length+1,method,voiceKey:binding.voiceKey,side,args:copy(args),
                beforeTime,afterTime};add(raw.gainMutations,mutation);
              if(method==='setValueAtTime')add(raw.gainCalls,{sequence:++gainSequence,voiceKey:binding.voiceKey,side,value:args[0],time:args[1],beforeTime,afterTime,
                hadActiveSource,controlStoppedActive:binding.controlStoppedActive,volume:voice.volume,pan:voice.pan});}
            return result;}));
        }
      }
    }
    const startSequence=trace().sequence;
    raw.window={startNativeSequence:startSequence,endNativeSequence:null,startedAtTime:context.currentTime,finishedAtTime:null,
      sampleRate:context.sampleRate,historySeconds:PRESENTATION_AUDIO_LIMITS.historySeconds,prefixFrames:PRESENTATION_AUDIO_LIMITS.prefixFrames};
    const previousProbe=host[contract.probeGlobal];
    const callbacks={onNativeEvent(row){add(raw.hookEvents,copy(row));const key=String(row.voiceKey),list=eventsByKey.get(key)||[];list.push(copy(row));eventsByKey.set(key,list);},
      onRoute(row){add(raw.hookRoutes,copy(row));},onUpload(row){add(raw.hookUploads,copy(row));note('upload','TH08 eager voice was replaced during the capture window');},
      onAudioShutdown(){note('native','Audio shutdown during capture');}};
    host[contract.probeGlobal]=Object.fromEntries(Object.entries(callbacks).map(([name,callback])=>[name,function(row){
      if(active)try{callback(row);}catch(error){note(name,error);}if(typeof previousProbe?.[name]==='function')previousProbe[name](row);} ]));
    restores.push(()=>{if(previousProbe===undefined)delete host[contract.probeGlobal];else host[contract.probeGlobal]=previousProbe;});
    restores.push(propertyWrapper(pool,'stop',original=>function(voice){const binding=voiceByObject.get(voice),node=voice?.node,beforeTime=context.currentTime;
      const result=original.apply(this,arguments),afterTime=context.currentTime;
      if(binding){const owner=node?sourceOwners.get(node):null,stopped={sequence:raw.voiceStops.length+1,kind:'voice-pool-stop',voiceKey:binding.voiceKey,nodePresent:!!node,
          sourceId:owner?.id??null,eventSerial:owner?.eventSerial??null,sourceSlot:owner?.slot?.entry.slot??null,
          slotGeneration:owner?.slotGeneration??null,beforeTime,afterTime};
        add(raw.voiceStops,stopped);binding.lastControlStop=stopped;if(node)binding.controlStoppedActive=true;}
      return result;}));
    restores.push(propertyWrapper(pool,'start',original=>function(voice){
      const previous=starting,binding=voiceByObject.get(voice),event=binding?eventsByKey.get(binding.voiceKey)?.shift():null;
      starting={binding,event};try{return original.apply(this,arguments);}finally{starting=previous;}}));
    restores.push(propertyWrapper(context,'createBufferSource',original=>function(){const node=original.apply(this,arguments),owner=starting;
      if(!owner?.binding)return node;
      const {binding,event}=owner,slot=binding.sourceSlots.find(candidate=>candidate.lease===null)??null;
      const id=++sourceSequence;
      const lease=leasePresentationAudioSourceSlot(slot,{sourceId:id,eventSerial:event?.serial??null});
      const sourceRecord={id,eventSerial:event?.serial??null,voiceKey:binding.voiceKey,binding,slot,lease,startedAt:null,splitConnects:0,
        ended:false,sourceReceiptDone:false,outputReceiptDone:false,continuationPending:false};
      sourceRecord.slotGeneration=lease?.slotGeneration??null;
      sourceRecord.previousSlotReceiptSequence=lease?.previousSlotReceiptSequence??null;
      sourceRecord.previousSlotEnded=lease?.previousSlotEnded??null;
      sourceOwners.set(node,sourceRecord);
      const connect=node.connect;node.connect=function(destination,...args){if(destination===binding.voice.split)sourceRecord.splitConnects++;
        return connect.call(this,destination,...args);};
      let connection=null;
      if(slot){node.connect(slot.node);connection={node,tap:slot};activeSources.add(connection);}
      node.addEventListener('ended',()=>{
        sourceRecord.ended=true;
        sourceRecord.endedRow={sequence:raw.voiceStops.length+1,kind:'source-node-ended',voiceKey:binding.voiceKey,nodePresent:true,
          sourceId:id,eventSerial:event?.serial??null,sourceSlot:slot?.entry.slot??null,slotGeneration:sourceRecord.slotGeneration??null,
          endedAt:context.currentTime,beforeTime:context.currentTime,afterTime:context.currentTime,endedFrame:Math.ceil(context.currentTime*context.sampleRate)};
        add(raw.voiceStops,sourceRecord.endedRow);if(connection)activeSources.delete(connection);maybeReleaseSlot(sourceRecord);
      },{once:true});
      const originalStop=node.stop;node.stop=function(...args){const beforeTime=context.currentTime,result=originalStop.apply(this,args),afterTime=context.currentTime;
        sourceRecord.stopRequested=true;
        sourceRecord.stopRow={sequence:raw.voiceStops.length+1,kind:'source-node-stop',voiceKey:binding.voiceKey,nodePresent:true,sourceId:id,
          eventSerial:event?.serial??null,sourceSlot:slot?.entry.slot??null,slotGeneration:sourceRecord.slotGeneration??null,
          requestedTime:args[0]??null,requestedFrame:Math.ceil(Math.max(args[0]??0,beforeTime)*context.sampleRate),
          stopUpperFrame:Math.ceil(Math.max(args[0]??0,afterTime)*context.sampleRate),beforeTime,afterTime};
        add(raw.voiceStops,sourceRecord.stopRow);return result;};
      const originalStart=node.start;node.start=function(when=0,offset=0,duration){
        const beforeTime=context.currentTime,result=originalStart.apply(this,arguments),afterTime=context.currentTime;sourceRecord.startedAt=beforeTime;
        const row={id,eventSerial:event?.serial??null,logicalIndex:event?.logicalIndex??null,handle:binding.handle,voiceKey:binding.voiceKey,
          sourceId:id,sourceTap:slot?.entry.id??null,sourceSlot:slot?.entry.slot??null,sourceSlotGeneration:sourceRecord.slotGeneration??null,
          previousSlotReceiptSequence:sourceRecord.previousSlotReceiptSequence??null,previousSlotEnded:sourceRecord.previousSlotEnded??null,
          outputTap:binding.output.entry.id,when,offset,duration:duration??null,beforeTime,afterTime,
          voiceSplitConnectionCount:sourceRecord.splitConnects,priorControlStop:binding.lastControlStop??null,
          gainSequence,bufferSame:node.buffer===binding.voice.buffer,sourceRate:node.buffer?.sampleRate??null,
          sourceFrames:node.buffer?.length??null,channels:node.buffer?.numberOfChannels??null,playbackRate:node.playbackRate.value,loop:node.loop};
        add(raw.starts,row);
        binding.lastControlStop=null;
        binding.controlStoppedActive=false;
        if(!event){add(raw.unassociatedStarts,row);return result;}
        if(!slot){add(raw.gaps,{eventSerial:event.serial,reason:'source-slot-inflight-reuse',sourceId:id,voiceKey:binding.voiceKey,
          sourceSlotCount:binding.sourceSlots.length});return result;}
        sourceRecordsByEvent.set(event.serial,sourceRecord);
        let resolve;const promise=new Promise(done=>{resolve=done;});pending.set(event.serial,{promise,resolve,sourceDone:false,outputDone:false});
        slot.node.port.postMessage({kind:'mark',mode:'onset',purpose:'event-audio',eventSerial:event.serial,sourceId:id,
          slotGeneration:sourceRecord.slotGeneration,startFrame:Math.ceil(Math.max(when,beforeTime)*context.sampleRate),
          prefixFrames:PRESENTATION_AUDIO_LIMITS.prefixFrames,budgetFrames:context.sampleRate});
        return result;};return node;}));
    const awaitEvents=async rows=>{
      const serials=(rows?.events??[]).map(row=>row.serial),deadline=Date.now()+5000;
      for(const serial of serials){const entry=pending.get(serial);
        if(!entry){if(!raw.pcm.some(row=>row.eventSerial===serial)&&!raw.gaps.some(row=>row.eventSerial===serial))failEvent(serial,'native-event-without-actual-source-start');continue;}
        const left=Math.max(0,deadline-Date.now());
        let timer;await Promise.race([entry.promise,new Promise(resolve=>{timer=setTimeout(resolve,left);})]);clearTimeout(timer);
        if(pending.has(serial))failEvent(serial,'actual-pcm-observation-timeout');
      }
    };
    const observe=async rows=>{add(raw.intervals,copy(rows));await awaitEvents(rows);};
    const finish=async()=>{
      if(finished)return finished;
      await awaitEvents({events:raw.hookEvents});
      const continuationDeadline=Date.now()+5000;
      for(const waiter of continuationWaiters.values()){
        const left=Math.max(0,continuationDeadline-Date.now());let timer;
        await Promise.race([waiter.promise,new Promise(resolve=>{timer=setTimeout(resolve,left);})]);clearTimeout(timer);
        if(!waiter.continuation.source||!waiter.continuation.output)
          note('silent-continuation',`Audio tail PCM timed out for event ${waiter.continuation.eventSerial}`);
      }
      for(const {node}of nodes)node.port.postMessage({kind:'flush',tag:epoch});
      const flushDeadline=Date.now()+5000;
      while(raw.taps.some(tap=>!tap.final)){if(Date.now()>=flushDeadline){note('flush','Actual worklet error/marker ledger did not finish');break;}await sleep(5);}
      const actual=trace();raw.native={sequence:actual?.sequence??null,recordLimit:actual?.recordLimit??null,dropped:copy(actual?.dropped??{}),
        uploads:copy(actual?.uploads??[]),events:copy((actual?.events??[]).filter(row=>row.serial>startSequence)),
        routes:copy((actual?.routes??[]).filter(row=>row.serial>startSequence)),errors:copy(actual?.errors??[])};
      raw.window.endNativeSequence=actual?.sequence??null;raw.window.finishedAtTime=context.currentTime;
      const serialized=copy(raw);finished={schema:PRESENTATION_AUDIO_SCHEMA,rawSha256:await hashJson(serialized),raw:serialized};return finished;
    };
    return {observe,finish,dispose};
  }catch(error){dispose();throw error;}
}
