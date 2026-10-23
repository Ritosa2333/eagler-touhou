import {DRIVER_API_VERSION,OBSERVATION_API_VERSION,TICK_STATUS} from '../../../../common/testkit/presentation-lab/contracts.mjs';
import {decodeRecords,analyzeWindow,compactReport,mergeIssueGroups,keyOf,STATE_GROUPS} from './analyzer.mjs';
import {LIFECYCLE_TRACKED_OWNER_IDS,STATE_AUTHORITY,STATE_FIELD_MAP} from './owners.mjs';
import {TH08_PRESENTATION_LAB_ABI,TH08_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';
import {decodeTimingRing,summarizeDisplayTiming} from './timing.mjs';

function validateCore(core){
  if(!core?.memory?.buffer)throw Error('Wrong runtime: missing WASM memory');
  for(const key of TH08_PRESENTATION_LAB_EXPORTS)if(typeof core[key]!=='function')throw Error('Wrong runtime: missing '+key);
}

export class Th08RuntimeDriver{
  constructor(runtime,identity){this.runtime=runtime;this.core=runtime.core;this.identity=identity;this.generation=0;this.token=null;validateCore(this.core);}
  describe(){return {driverApiVersion:DRIVER_API_VERSION,game:'th08',adapterVersion:'th08-presentation-lab/5',nativeAbi:TH08_PRESENTATION_LAB_ABI,features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,lifecycleEvidence:true,timing:true,capture:true,replay:true,productionFrameCallbackCadence:true}};}
  freeze(){
    if(this.token)return this.token;
    this.core.sdl_loop_stop();
    this.token=Object.freeze({sessionEpoch:this.identity.runtimeEpoch??this.identity.wasm??'th08',generation:++this.generation});
    return this.token;
  }
  resume(token){if(token!==this.token)throw Error('Stale Presentation Lab freeze token');this.core.sdl_loop_start();this.token=null;}
  advanceOneTick(){
    const result=this.core.sdl_loop_tick(this.runtime.app,1/60,17);
    return result?{status:TICK_STATUS.FINISHED,advancedTicks:0,detail:String(result)}:{status:TICK_STATUS.ADVANCED,advancedTicks:1};
  }
  frameAt(nowMs){
    if(!this.token)throw Error('Production frame callback requires a frozen Runtime');
    if(!Number.isFinite(nowMs)||nowMs<0)throw Error('Production frame timestamp must be finite and nonnegative');
    return this.core.sdl_presentation_audit_frame_at_v1(nowMs)===1;
  }
  drawOnly({alpha,world}){const result=this.core.audit_draw(alpha,world?1:0);return result?{status:'error',detail:String(result)}:{status:'drawn'};}
  setInput({code,down}){
    const bytes=new TextEncoder().encode(code+'\0'),pointer=this.core.allocate(bytes.length);
    try{new Uint8Array(this.core.memory.buffer,pointer,bytes.length).set(bytes);this.core.sdl_key(pointer,down?1:0);}
    finally{this.core.deallocate(pointer);}
  }
  clearInput(){this.core.sdl_keys_clear();}
  close(){this.token=null;return this.runtime.stop();}
}

export class Th08ObservationAdapter{
  constructor(runtime){this.runtime=runtime;this.core=runtime.core;}
  bindSessionIdentity(identity){this.runtimeEpoch=identity.runtimeEpoch;}
  observationApiVersion=OBSERVATION_API_VERSION;
  scanSchema='th08/presentation-scan/1';
  sessionSchema='th08/presentation-session/1';
  enable(on){this.core.audit_enable(on?1:0);}
  readRecordSet(reference=null){
    const pointer=reference===null?this.core.audit_records():this.core.audit_reference(reference);
    const count=reference===null?this.core.audit_count():this.core.audit_reference_count(reference);
    const stride=this.core.audit_stride(),buffer=this.core.memory.buffer;
    const records=decodeRecords(buffer,pointer,count,stride);
    const cursor=reference===null?{
      count:this.core.audit_lifecycle_count(),next:this.core.audit_lifecycle_next(),
      dropped:this.core.audit_lifecycle_dropped(),epoch:this.core.audit_lifecycle_epoch(),
    }:{
      count:this.core.audit_reference_lifecycle_count(reference),next:this.core.audit_reference_lifecycle_next(reference),
      dropped:this.core.audit_reference_lifecycle_dropped(reference),epoch:this.core.audit_reference_lifecycle_epoch(reference),
    };
    return {tick:reference===null?this.core.audit_tick_id():this.core.audit_reference_tick(reference),
      dropped:reference===null?this.core.audit_dropped():this.core.audit_reference_dropped(reference),records,
      native:{schema:'th08/presentation-record-buffer/1',stride,count,runtimeEpoch:this.runtimeEpoch??null,
        words:Array.from(new Uint32Array(buffer,pointer,count*stride/4)),lifecycle:cursor}};
  }
  readReferences(){return {previous:this.readRecordSet(0),current:this.readRecordSet(1)};}
  readObservation(){return this.readRecordSet();}
  readStateEvidence(){
    const buffer=this.core.memory.buffer,pointer=this.core.audit_state(),count=this.core.audit_state_words();
    if(count!==STATE_GROUPS.length||!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+count*Uint32Array.BYTES_PER_ELEMENT>buffer.byteLength)
      return {coverageVersion:'th08/state/3',groups:[],missingGroups:['native state fingerprint table unavailable or ABI-mismatched'],authority:STATE_AUTHORITY};
    const words=new Uint32Array(buffer,pointer,count);
    return {coverageVersion:'th08/state/3',groups:Array.from(words,(digest,index)=>({id:STATE_GROUPS[index],digest,includedFields:STATE_FIELD_MAP[STATE_GROUPS[index]]||[]})),missingGroups:[],authority:STATE_AUTHORITY};
  }
  readLifecycleEvidence(){
    const buffer=this.core.memory.buffer,pointer=this.core.audit_lifecycle_records(),capacity=this.core.audit_lifecycle_capacity();
    const count=this.core.audit_lifecycle_count(),next=this.core.audit_lifecycle_next(),dropped=this.core.audit_lifecycle_dropped();
    const epoch=typeof this.core.audit_lifecycle_epoch==='function'?this.core.audit_lifecycle_epoch():null;
    if(!Number.isInteger(pointer)||pointer<=0||pointer%4||!Number.isInteger(capacity)||capacity<1||
       !Number.isInteger(count)||count<0||count>capacity||!Number.isInteger(next)||next<0||next>=capacity||
       !Number.isInteger(dropped)||dropped<0||pointer+capacity*5*Uint32Array.BYTES_PER_ELEMENT>buffer.byteLength)
      return {available:false,capacity:0,count:0,next:0,dropped:0,events:[]};
    const ring=new Uint32Array(buffer,pointer,capacity*5),start=(next-count+capacity)%capacity,events=[],words=[];
    for(let i=0;i<count;i++){
      const at=((start+i)%capacity)*5;
      words.push(...ring.subarray(at,at+5));
      events.push({owner:ring[at],object:ring[at+1],generation:ring[at+2],tick:ring[at+3],operation:ring[at+4]});
    }
    return {available:true,capacity,count,next,dropped,epoch,runtimeEpoch:this.runtimeEpoch??null,events,
      native:{schema:'th08/presentation-lifecycle-buffer/1',stride:20,words}};
  }
  readTiming(){return decodeTimingRing(this.core.memory.buffer,{base:this.core.audit_timing_records(),capacity:this.core.audit_timing_capacity(),count:this.core.audit_timing_count(),next:this.core.audit_timing_next(),stride:this.core.audit_timing_stride()});}
  readTrace(){return Array.from(new Uint32Array(this.core.memory.buffer,this.core.trace(this.runtime.app),68));}
  readReplayConsumerTrace(){
    const read=field=>this.core.sdl_replay_trace_status_v1(field)>>>0;
    const schema=read(29),nextSequence=read(30),firstAvailable=read(31),count=read(32),capacity=read(33);
    if(schema!==2||capacity!==256||count>capacity||firstAvailable>nextSequence||nextSequence-firstAvailable!==count)
      return {available:false,schema,nextSequence,firstAvailable,count,capacity,latest:[]};
    const latest=[];
    if(count){const start=34+(count-1)*36;for(let i=0;i<36;i++)latest.push(read(start+i));}
    if(count&&latest[0]!==((nextSequence-1)>>>0))return {available:false,schema,nextSequence,firstAvailable,count,capacity,latest};
    return {available:true,schema,nextSequence,firstAvailable,count,capacity,latest};
  }
  readSfxTrace(){
    const host=this.runtime.Module?.canvas?.ownerDocument?.defaultView;
    const trace=host?.__th08SfxLatencyTrace;
    if(!host||!trace)return {available:false,enabled:host?.__th08SfxLatencyProbeEnabled===true,
      sequence:0,counts:{events:0,routes:0,uploads:0,errors:0},dropped:{events:0,routes:0,uploads:0,errors:0}};
    const kinds=['events','routes','uploads','errors'],counts={},dropped={};
    for(const kind of kinds){counts[kind]=Array.isArray(trace[kind])?trace[kind].length:-1;
      dropped[kind]=Number.isSafeInteger(trace.dropped?.[kind])?trace.dropped[kind]:-1;}
    return {available:Number.isSafeInteger(trace.sequence)&&kinds.every(kind=>counts[kind]>=0),
      enabled:host.__th08SfxLatencyProbeEnabled===true,sequence:trace.sequence,counts,dropped};
  }
  readSfxRowsSince(sequence){
    const host=this.runtime.Module?.canvas?.ownerDocument?.defaultView,trace=host?.__th08SfxLatencyTrace;
    const kinds=['events','routes','uploads','errors'],rows={};
    for(const kind of kinds)rows[kind]=Array.isArray(trace?.[kind])?trace[kind].filter(row=>Number.isSafeInteger(row.serial)&&row.serial>sequence).map(row=>({...row})):[];
    const cursor=this.readSfxTrace();
    return {...cursor,...rows};
  }
  readPairedCheckpoint(){
    const state=this.readStateEvidence(),status=Array.from({length:29},(_,field)=>this.core.status(this.runtime.app,field));
    const replayStatus=status.slice(12,22),audioPointer=this.core.sdl_audit_audio(),audioWords=this.core.sdl_audit_audio_words();
    const buffer=this.core.memory.buffer;
    const auditWords=Number.isSafeInteger(audioWords)&&audioWords===25&&Number.isInteger(audioPointer)&&audioPointer>0&&audioPointer%4===0&&
      audioPointer+audioWords*Uint32Array.BYTES_PER_ELEMENT<=buffer.byteLength
      ?Array.from(new Uint32Array(buffer,audioPointer,audioWords)):[];
    return {schema:'eagler-presentation/checkpoint/1',state:{complete:state.groups.length===STATE_GROUPS.length&&state.missingGroups.length===0,
      groups:state.groups.map(({id,digest})=>({id,digest})),missingGroups:state.missingGroups},
      runtime:{status,trace:this.readTrace()},
      replay:{available:replayStatus[0]===1&&replayStatus.length===10,status:replayStatus,trace:this.readReplayConsumerTrace()},
      audio:{auditWords,sfx:this.readSfxTrace()}};
  }
  readScene(){return {status:this.runtime.status(),diagnostics:Array.from(new Int32Array(this.core.memory.buffer,this.core.diagnostics(this.runtime.app),16)),
    nativeScene:Array.from(new Int32Array(this.core.memory.buffer,this.core.audit_scene(),16))};}
  worldFrozen(){return !!this.core.audit_world_frozen();}
  readGate(){return !!this.core.audit_gate();}
  setNegativeControl(on){this.core.audit_fault(on?3:0);}
  captureImage(frame,alpha){return {alpha,png:this.runtime.Module.canvas?.toDataURL?.()||null,boxes:frame.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}))};}
  analyze(input){const lifecycle=this.readLifecycleEvidence();return {...analyzeWindow({...input,lifecycle}),
    nativeFrames:{schema:'th08/presentation-native-frames/1',previous:input.previous,current:input.current,samples:input.samples},
    nativeLifecycle:lifecycle,interpolationReady:input.gate,
    displayTiming:summarizeDisplayTiming(this.readTiming())};}
  compact(report,options){return compactReport(report,options);}
  mergeIssues(reports){return mergeIssueGroups(reports);}
}
