import {PresentationLabControllerCore} from '/common/controller-core.mjs';
import {decodeRecords,analyzeWindow,compactReport,mergeIssueGroups,keyOf} from './analyzer.mjs';

export function decodeTimingRing(buffer,{base,capacity,count,next,stride}){
  // The WASM memory belongs to the runtime iframe realm, so instanceof against
  // the parent window's ArrayBuffer constructor is intentionally invalid.
  if(!buffer||typeof buffer.byteLength!=='number'||capacity!==512||stride!==128||!Number.isInteger(base)||base<0||base%8||count>capacity||count<0||next>=capacity||next<0||base+capacity*stride>buffer.byteLength)throw Error('Invalid presentation timing ring');
  const view=new DataView(buffer),rows=[];for(let n=0;n<count;n++){const index=(next+capacity-count+n)%capacity,offset=base+index*stride,flags=view.getUint32(offset+16,true),camera=[];
    for(let i=0;i<26;i++)camera.push(view.getFloat32(offset+24+i*4,true));
    rows.push({timestampMs:view.getFloat64(offset,true),deltaMs:view.getFloat32(offset+8,true),alpha:view.getFloat32(offset+12,true),flags:{ready:!!(flags&1),fast:!!(flags&2),highRefresh:!!(flags&4),tickDue:!!(flags&8),interpolate:!!(flags&16),presented:!!(flags&32),cameraValid:!!(flags&64),cameraRebased:!!(flags&128)},simulationFrame:view.getUint32(offset+20,true),camera:flags&64?{previous:camera.slice(0,13),current:camera.slice(13)}:null});
  }return {schema:'th08/presentation-timing-ring/1',capacity,count,durationMs:rows.length>1?rows.at(-1).timestampMs-rows[0].timestampMs:0,rows};
}

const th08Adapter={
  scanSchema:'th08/presentation-scan/1',sessionSchema:'th08/presentation-session/1',
  validate(core){for(const key of ['audit_enable','audit_draw','audit_fault','audit_gate','audit_world_frozen','audit_state','audit_records','audit_count','audit_tick_id','audit_dropped','audit_reference','audit_reference_count','audit_reference_tick','audit_reference_dropped','audit_stride','audit_timing_records','audit_timing_capacity','audit_timing_count','audit_timing_next','audit_timing_stride','trace','diagnostics'])if(typeof core[key]!=='function')throw Error('Wrong runtime: missing '+key);},
  enable(core,on){core.audit_enable(on?1:0);},
  records(core,reference=null){const pointer=reference===null?core.audit_records():core.audit_reference(reference),count=reference===null?core.audit_count():core.audit_reference_count(reference);return {tick:reference===null?core.audit_tick_id():core.audit_reference_tick(reference),dropped:reference===null?core.audit_dropped():core.audit_reference_dropped(reference),records:decodeRecords(core.memory.buffer,pointer,count,core.audit_stride())};},
  state(core){return Array.from(new Uint32Array(core.memory.buffer,core.audit_state(),9));},
  timing(core){return decodeTimingRing(core.memory.buffer,{base:core.audit_timing_records(),capacity:core.audit_timing_capacity(),count:core.audit_timing_count(),next:core.audit_timing_next(),stride:core.audit_timing_stride()});},
  trace(core,runtime){return Array.from(new Uint32Array(core.memory.buffer,core.trace(runtime.app),68));},
  worldFrozen(core){return !!core.audit_world_frozen();},setFault(core,on){core.audit_fault(on?1:0);},draw(core,alpha,world){return core.audit_draw(alpha,world?1:0);},gate(core){return !!core.audit_gate();},
  picture(runtime,frame,alpha){return {alpha,png:runtime.Module.canvas?.toDataURL?.()||null,boxes:frame.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}))};},
  analyze:analyzeWindow,compact:compactReport,mergeIssues:mergeIssueGroups,
  scene(core,runtime){return {status:runtime.status(),diagnostics:Array.from(new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16))};},
};

export class LabController extends PresentationLabControllerCore{
  constructor(runtime,identity){super(runtime,identity,th08Adapter);}
}
