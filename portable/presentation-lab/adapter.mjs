import {DRIVER_API_VERSION,OBSERVATION_API_VERSION,TICK_STATUS} from '../../third_party/eagler-common/testkit/presentation-lab/contracts.mjs';
import {decodeRecords,analyzeWindow,compactReport,mergeIssueGroups,keyOf} from './analyzer.mjs';
import {decodeTimingRing} from './timing.mjs';

const requiredExports=['sdl_loop_start','sdl_loop_stop','sdl_loop_tick','sdl_key','sdl_keys_clear','allocate','deallocate','audit_enable','audit_draw','audit_fault','audit_gate','audit_world_frozen','audit_state','audit_records','audit_count','audit_tick_id','audit_dropped','audit_reference','audit_reference_count','audit_reference_tick','audit_reference_dropped','audit_stride','audit_timing_records','audit_timing_capacity','audit_timing_count','audit_timing_next','audit_timing_stride','trace','diagnostics'];

function validateCore(core){
  if(!core?.memory?.buffer)throw Error('Wrong runtime: missing WASM memory');
  for(const key of requiredExports)if(typeof core[key]!=='function')throw Error('Wrong runtime: missing '+key);
}

export class Th08RuntimeDriver{
  constructor(runtime,identity){this.runtime=runtime;this.core=runtime.core;this.identity=identity;this.generation=0;this.token=null;validateCore(this.core);}
  describe(){return {driverApiVersion:DRIVER_API_VERSION,game:'th08',adapterVersion:'th08-presentation-lab/1',nativeAbi:'th08/presentation-audit-native/1',features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true,replay:true}};}
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
  observationApiVersion=OBSERVATION_API_VERSION;
  scanSchema='th08/presentation-scan/1';
  sessionSchema='th08/presentation-session/1';
  enable(on){this.core.audit_enable(on?1:0);}
  readRecordSet(reference=null){const pointer=reference===null?this.core.audit_records():this.core.audit_reference(reference),count=reference===null?this.core.audit_count():this.core.audit_reference_count(reference);return {tick:reference===null?this.core.audit_tick_id():this.core.audit_reference_tick(reference),dropped:reference===null?this.core.audit_dropped():this.core.audit_reference_dropped(reference),records:decodeRecords(this.core.memory.buffer,pointer,count,this.core.audit_stride())};}
  readReferences(){return {previous:this.readRecordSet(0),current:this.readRecordSet(1)};}
  readObservation(){return this.readRecordSet();}
  readStateEvidence(){return Array.from(new Uint32Array(this.core.memory.buffer,this.core.audit_state(),9));}
  readTiming(){return decodeTimingRing(this.core.memory.buffer,{base:this.core.audit_timing_records(),capacity:this.core.audit_timing_capacity(),count:this.core.audit_timing_count(),next:this.core.audit_timing_next(),stride:this.core.audit_timing_stride()});}
  readTrace(){return Array.from(new Uint32Array(this.core.memory.buffer,this.core.trace(this.runtime.app),68));}
  readScene(){return {status:this.runtime.status(),diagnostics:Array.from(new Int32Array(this.core.memory.buffer,this.core.diagnostics(this.runtime.app),16))};}
  worldFrozen(){return !!this.core.audit_world_frozen();}
  readGate(){return !!this.core.audit_gate();}
  setNegativeControl(on){this.core.audit_fault(on?1:0);}
  captureImage(frame,alpha){return {alpha,png:this.runtime.Module.canvas?.toDataURL?.()||null,boxes:frame.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}))};}
  analyze(input){return analyzeWindow(input);}
  compact(report,options){return compactReport(report,options);}
  mergeIssues(reports){return mergeIssueGroups(reports);}
}
