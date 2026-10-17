import {DRIVER_API_VERSION,OBSERVATION_API_VERSION,TICK_STATUS} from '../../third_party/eagler-common/testkit/presentation-lab/contracts.mjs';
import {analyzeNormalizedWindow,compactReport,mergeIssueGroups} from '../../third_party/eagler-common/testkit/presentation-lab/analyzer.mjs';
import {ownerLabel} from './owners.mjs';
import {TH10_PRESENTATION_LAB_ABI,TH10_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';

const STRIDE=456,MAX_RECORDS=8192,TAU=Math.PI*2;
const floats=(view,offset,count)=>Array.from({length:count},(_,index)=>view.getFloat32(offset+index*4,true));
const channels=value=>[value&255,(value>>>8)&255,(value>>>16)&255,value>>>24];

function decodeRecords(buffer,pointer,count,stride){
  if(!buffer||typeof buffer.byteLength!=='number'||stride!==STRIDE||!Number.isInteger(pointer)||pointer<0||pointer%4||!Number.isInteger(count)||count<0||count>MAX_RECORDS||pointer+count*stride>buffer.byteLength)throw Error('Invalid TH10 presentation audit buffer');
  const view=new DataView(buffer),records=[];
  for(let index=0;index<count;index++){
    const at=pointer+index*stride,meta=Array.from({length:12},(_,i)=>view.getUint32(at+i*4,true)),values=floats(view,at+48,16),color=view.getUint32(at+112,true),secondary=view.getUint32(at+116,true),bounds=floats(view,at+120,4),vertices=floats(view,at+136,Math.min(meta[8],16)*5);
    const center=[(bounds[0]+bounds[2])/2,(bounds[1]+bounds[3])/2],shape=[],submittedUv=[];for(let i=0;i<vertices.length;i+=5){shape.push(vertices[i]-center[0],vertices[i+1]-center[1]);submittedUv.push(vertices[i+3],vertices[i+4]);}
    const continuous=meta[9],part=meta[3],coordinateSpace=part===5?'model-local':'submitted-screen',record={key:[meta[0],meta[1],meta[2],part].join(':'),ownerId:meta[0],ownerLabel:ownerLabel(meta[0]),objectId:meta[1],generation:meta[2],identityConfidence:meta[2]?'proven':'uncertain',partId:part,drawId:meta[4],
      lifecycle:{continuous:true,scriptIndex:meta[5]|0,spriteIndex:meta[6]|0,fileIndex:meta[10],visible:meta[7]&3},coordinateSpace,bounds,geometryQuality:meta[8]>16?'sampled':part===5?'model-local':'complete',
      fields:[
        {id:'position',label:'位置',value:values.slice(0,3),tolerance:.01,interpolationPolicy:'continuous'},
        {id:'script-position',label:'脚本位置',value:values.slice(3,6),tolerance:.01,interpolationPolicy:'continuous'},
        {id:'child-position',label:'子对象位置',value:values.slice(6,9),tolerance:.01,interpolationPolicy:'continuous'},
        {id:'rotation',label:'旋转',value:values.slice(9,12),tolerance:.0001,period:TAU,interpolationPolicy:continuous&1?'continuous':'unknown'},
        {id:'scale',label:'缩放',value:values.slice(12,14),tolerance:.0001,interpolationPolicy:continuous&2?'continuous':'unknown'},
        {id:'uv-x',label:'UV X',value:values[14],tolerance:.00001,period:1,interpolationPolicy:continuous&64?'continuous':'unknown'},
        {id:'uv-y',label:'UV Y',value:values[15],tolerance:.00001,period:1,interpolationPolicy:continuous&128?'continuous':'unknown'},
        {id:'rgb',label:'主颜色',value:channels(color).slice(0,3),tolerance:1.01,interpolationPolicy:continuous&4?'continuous':'unknown'},
        {id:'opacity',label:'透明度',value:channels(color)[3],tolerance:1.01,interpolationPolicy:continuous&8?'continuous':'unknown'},
        {id:'rgb2',label:'第二颜色',value:channels(secondary).slice(0,3),tolerance:1.01,interpolationPolicy:continuous&16?'continuous':'unknown'},
        {id:'opacity2',label:'第二透明度',value:channels(secondary)[3],tolerance:1.01,interpolationPolicy:continuous&32?'continuous':'unknown'},
        {id:'submitted-position',label:part===5?'模型局部中心':'提交几何中心',value:center,tolerance:.025,interpolationPolicy:part===5?'unknown':'continuous'},
        {id:'submitted-shape',label:part===5?'模型局部形状':'提交几何形状',value:shape,tolerance:.025,interpolationPolicy:'unknown'},
      ]};
    record.fields.push({id:'submitted-uv',label:'提交 UV',value:submittedUv,tolerance:.00001,period:1,interpolationPolicy:'unknown'});
    records.push(record);
  }
  return records;
}

function markLifecycle(previous,current){
  const before=new Map(previous.records.map(record=>[record.key,record]));
  const field=(record,id)=>record.fields.find(value=>value.id===id)?.value||[];
  const jump=(a,b)=>a.length!==b.length||a.reduce((sum,value,index)=>sum+(b[index]-value)**2,0)>=16384;
  for(const record of current.records){const prior=before.get(record.key);if(!prior)continue;const a=prior.lifecycle,b=record.lifecycle;if(a.scriptIndex!==b.scriptIndex||a.fileIndex!==b.fileIndex||a.visible!==b.visible||jump(field(prior,'position'),field(record,'position'))||jump(field(prior,'script-position'),field(record,'script-position'))||jump(field(prior,'child-position'),field(record,'child-position')))b.continuous=false;}
}

export class Th10RuntimeDriver{
  constructor(runtime,identity){this.runtime=runtime;this.core=runtime.core;this.identity=identity;this.generation=0;this.token=null;if(!this.core?.memory?.buffer)throw Error('Wrong TH10 runtime');for(const name of TH10_PRESENTATION_LAB_EXPORTS)if(typeof this.core[name]!=='function')throw Error('Wrong TH10 diagnostic runtime: missing '+name);}
  describe(){return {driverApiVersion:DRIVER_API_VERSION,game:'th10',adapterVersion:'th10-presentation-lab/1',nativeAbi:TH10_PRESENTATION_LAB_ABI,features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true,replay:true}};}
  freeze(){if(this.token)return this.token;const result=this.core.presentation_lab_freeze(this.runtime.app);if(result)throw Error('TH10 diagnostic freeze failed: '+result);this.token=Object.freeze({sessionEpoch:this.identity.runtimeEpoch??this.identity.wasm??'th10',generation:++this.generation});return this.token;}
  resume(token){if(token!==this.token)throw Error('Stale Presentation Lab freeze token');const result=this.core.presentation_lab_resume(this.runtime.app);if(result)throw Error('TH10 diagnostic resume failed: '+result);this.token=null;}
  advanceOneTick(){const result=this.core.presentation_lab_tick(this.runtime.app);return result?{status:TICK_STATUS.FINISHED,advancedTicks:0,detail:String(result)}:{status:TICK_STATUS.ADVANCED,advancedTicks:1};}
  drawOnly({alpha,world}){const result=this.core.presentation_lab_draw(this.runtime.app,alpha,world?1:0);return result?{status:'error',detail:String(result)}:{status:'drawn'};}
  setInput({code,down}){const bytes=new TextEncoder().encode(code+'\0'),pointer=this.core.graphics_allocate(bytes.length);try{new Uint8Array(this.core.memory.buffer,pointer,bytes.length).set(bytes);this.core.sdl_key(pointer,down?1:0);}finally{this.core.graphics_free(pointer);}}
  clearInput(){this.core.sdl_keys_clear();}
  close(){this.token=null;return this.runtime.stop();}
}

export class Th10ObservationAdapter{
  constructor(runtime){this.runtime=runtime;this.core=runtime.core;}
  observationApiVersion=OBSERVATION_API_VERSION;scanSchema='presentation-lab/scan/1';sessionSchema='presentation-lab/session/1';
  enable(on){if(on)this.core.audit_timing_reset();this.core.audit_enable(on?1:0);}
  readFrame(reference=null){const core=this.core,pointer=reference===null?core.audit_records():core.audit_reference(reference),count=reference===null?core.audit_count():core.audit_reference_count(reference),dropped=reference===null?core.audit_dropped():core.audit_reference_dropped(reference),simulationTick=reference===null?core.audit_tick():core.audit_reference_tick(reference),referenceDrawSerial=reference===null?core.audit_draw_serial():core.audit_reference_draw_serial(reference);return {sessionEpoch:core.audit_session_epoch(),simulationTick,referenceDrawSerial,completeness:dropped?'incomplete':'complete',dropped,records:decodeRecords(core.memory.buffer,pointer,count,core.audit_stride()),limitations:['semantic owner registry is incomplete','screen-effect rectangles and inactive untextured mesh paths bypass the ANM observer','model draws expose local geometry, not projected pixels','draw-only state purity is not yet proven']};}
  readReferences(){const previous=this.readFrame(0),current=this.readFrame(1);markLifecycle(previous,current);return {previous,current};}
  readObservation(){return this.readFrame();}
  readStateEvidence(){const pointer=this.core.audit_state(this.runtime.app),buffer=this.core.memory.buffer;if(!pointer||pointer%4||pointer+28>buffer.byteLength)return {coverageVersion:'th10/state/2',groups:[],missingGroups:['all']};const words=new Uint32Array(buffer,pointer,7);if(words[0]!==1)return {coverageVersion:'th10/state/2',groups:[],missingGroups:['all']};const definitions=[['lifecycle',['screen transition','loading gate','application lifetime']],['simulation-core',['economy','input snapshots','motion input','script and visual RNG']],['anm-authored',['pooled VM timers','script cursors','authored transforms','interpolators','colors']],['presentation-sidecars',['player','bullet','item','laser','popup presentation endpoints']],['replay-and-host-clock',['Replay cursors','elapsed','audio remainder','frame cadence','presentation cadence','freeze state']]],groups=[];for(let i=0;i<definitions.length;i++)if(words[1]&(1<<i))groups.push({id:definitions[i][0],digest:words[i+2],includedFields:definitions[i][1]});return {coverageVersion:'th10/state/2',groups,missingGroups:['gameplay actor pools outside GameEconomy','heap-allocated and embedded ANM VMs outside the manager pool','audio device and queued sample internals','non-ANM screen-effect owners']};}
  readTiming(){const core=this.core,base=core.audit_timing_records(),capacity=core.audit_timing_capacity(),count=core.audit_timing_count(),next=core.audit_timing_next(),stride=core.audit_timing_stride(),buffer=core.memory.buffer;if(capacity!==512||stride!==32||base<0||base%8||count<0||count>capacity||next<0||next>=capacity||base+capacity*stride>buffer.byteLength)throw Error('Invalid TH10 timing ring');const view=new DataView(buffer),rows=[];for(let n=0;n<count;n++){const index=(next+capacity-count+n)%capacity,at=base+index*stride,flags=view.getUint32(at+16,true);rows.push({timestampMs:view.getFloat64(at,true),deltaMs:view.getFloat32(at+8,true),alpha:view.getFloat32(at+12,true),flags:{ready:!!(flags&1),fast:!!(flags&2),highRefresh:!!(flags&4),tickDue:!!(flags&8),interpolate:!!(flags&16),presented:!!(flags&32)},simulationTick:view.getUint32(at+20,true),referenceDrawSerial:view.getUint32(at+24,true)});}return {schema:'presentation-lab/timing/1',completeness:count===capacity?'bounded':'complete',capacity,count,durationMs:rows.length>1?rows.at(-1).timestampMs-rows[0].timestampMs:0,rows};}
  readTrace(){return [];}
  readScene(){return {status:this.runtime.status()};}
  worldFrozen(){return !!this.core.presentation_lab_world_frozen(this.runtime.app);}
  readGate(){return !!this.core.presentation_lab_gate();}
  setNegativeControl(on){this.core.presentation_lab_fault(on?1:0);}
  captureImage(frame,alpha){return {alpha,png:this.runtime.Module.canvas?.toDataURL?.()||null,boxes:frame.records.map(record=>({key:record.key,bounds:record.bounds}))};}
  analyze(input){return analyzeNormalizedWindow(input);}
  compact(report,options){return compactReport(report,options);}
  mergeIssues(reports){return mergeIssueGroups(reports);}
}
