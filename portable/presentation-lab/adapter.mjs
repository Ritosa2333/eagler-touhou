import {DRIVER_API_VERSION,OBSERVATION_API_VERSION,TICK_STATUS} from '../../third_party/eagler-common/testkit/presentation-lab/contracts.mjs';
import {analyzeNormalizedWindow,compactReport,mergeIssueGroups} from '../../third_party/eagler-common/testkit/presentation-lab/analyzer.mjs';
import {ownerLabel,STATE_GROUPS} from './owners.mjs';
import {TH11_PRESENTATION_LAB_ABI,TH11_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';

const STRIDE=456,MAX_RECORDS=8192,TAU=Math.PI*2;
const floats=(view,offset,count)=>Array.from({length:count},(_,index)=>view.getFloat32(offset+index*4,true));
const channels=value=>[value&255,(value>>>8)&255,(value>>>16)&255,value>>>24];

export function decodeRecords(buffer,pointer,count,stride){
  if(!buffer||typeof buffer.byteLength!=='number'||stride!==STRIDE||!Number.isInteger(pointer)||pointer<0||pointer%4||!Number.isInteger(count)||count<0||count>MAX_RECORDS||pointer+count*stride>buffer.byteLength)throw Error('Invalid TH11 presentation audit buffer');
  const view=new DataView(buffer),records=[];
  for(let index=0;index<count;index++){
    const at=pointer+index*stride,meta=Array.from({length:12},(_,i)=>view.getUint32(at+i*4,true)),values=floats(view,at+48,16),color=view.getUint32(at+112,true),secondary=view.getUint32(at+116,true),bounds=floats(view,at+120,4),vertices=floats(view,at+136,Math.min(meta[8],16)*5);
    const center=[(bounds[0]+bounds[2])/2,(bounds[1]+bounds[3])/2],shape=[],submittedUv=[];
    for(let i=0;i<vertices.length;i+=5){shape.push(vertices[i]-center[0],vertices[i+1]-center[1]);submittedUv.push(vertices[i+3],vertices[i+4]);}
    const continuous=meta[9],part=meta[3],layer=meta[11];
    records.push({key:[meta[0],meta[1],meta[2],part].join(':'),ownerId:meta[0],ownerLabel:ownerLabel(meta[0]),objectId:meta[1],generation:meta[2],identityConfidence:meta[2]?'proven':'uncertain',partId:part,drawId:meta[4],layer,
      lifecycle:{continuous:true,scriptIndex:meta[5]|0,spriteIndex:meta[6]|0,fileIndex:meta[10],visible:meta[7]&3,
        ...(continuous&256?{discontinuity:{kind:'authored-rebase',source:'std-camera-set'}}:{})},
      coordinateSpace:'submitted-screen',bounds,geometryQuality:meta[8]>16?'sampled':'complete',
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
        // Final submission may include the retail half-pixel convention and
        // per-pass pixel alignment, so it is not a continuous authored field.
        {id:'submitted-position',label:'提交几何中心',value:center,tolerance:.025,interpolationPolicy:'unknown'},
        {id:'submitted-shape',label:'提交几何形状',value:shape,tolerance:.025,interpolationPolicy:'unknown'},
        {id:'submitted-uv',label:'提交 UV',value:submittedUv,tolerance:.00001,period:1,interpolationPolicy:'unknown'},
      ]});
  }
  return records;
}

function markLifecycle(previous,current){
  const before=new Map(previous.records.map(record=>[record.key,record]));
  const field=(record,id)=>record.fields.find(value=>value.id===id)?.value||[];
  const jump=(a,b)=>a.length!==b.length||a.reduce((sum,value,index)=>sum+(b[index]-value)**2,0)>=16384;
  for(const record of current.records){
    const prior=before.get(record.key);if(!prior)continue;
    const a=prior.lifecycle,b=record.lifecycle;
    if(a.scriptIndex!==b.scriptIndex||a.fileIndex!==b.fileIndex||a.visible!==b.visible||jump(field(prior,'position'),field(record,'position'))||jump(field(prior,'script-position'),field(record,'script-position'))||jump(field(prior,'child-position'),field(record,'child-position')))b.continuous=false;
  }
}

export function decodeTiming(){return {schema:'th11/presentation-timing/1',rows:[],note:'timing ring not instrumented in this milestone'};}

const KEYCODES={ArrowUp:82,ArrowDown:81,ArrowLeft:80,ArrowRight:79,KeyZ:29,KeyX:27,ShiftLeft:225,ShiftRight:229,Enter:40,Escape:41,KeyR:21,KeyP:19};

export class Th11RuntimeDriver{
  constructor(runtime,identity){
    this.runtime=runtime;this.core=runtime.core;this.identity=identity;this.generation=0;this.token=null;
    if(!this.core?.memory?.buffer)throw Error('Wrong TH11 runtime');
    for(const name of TH11_PRESENTATION_LAB_EXPORTS)if(typeof this.core[name]!=='function')throw Error('Wrong TH11 diagnostic runtime: missing '+name);
  }
  describe(){return {driverApiVersion:DRIVER_API_VERSION,game:'th11',adapterVersion:'th11-presentation-lab/1',nativeAbi:TH11_PRESENTATION_LAB_ABI,features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true,replay:true}};}
  phase(){return this.core.th11_phase();}
  sessionFrame(){return this.core.th11_frame();}
  freeze(){if(this.token)return this.token;const result=this.core.presentation_lab_freeze();if(result)throw Error('TH11 diagnostic freeze failed: '+result);this.token=Object.freeze({sessionEpoch:this.identity.runtimeEpoch??this.identity.wasm??'th11',generation:++this.generation});return this.token;}
  resume(token){if(token!==this.token)throw Error('Stale Presentation Lab freeze token');const result=this.core.presentation_lab_resume();if(result)throw Error('TH11 diagnostic resume failed: '+result);this.token=null;}
  advanceOneTick(){const result=this.core.presentation_lab_tick();if(result<0)return {status:TICK_STATUS.ERROR,advancedTicks:0,detail:'loop still running'};if(result>0)return {status:TICK_STATUS.FINISHED,advancedTicks:0};return {status:TICK_STATUS.ADVANCED,advancedTicks:1};}
  drawOnly({alpha,world}){const result=this.core.presentation_lab_draw(alpha,world?1:0);return result?{status:'error',detail:String(result)}:{status:'drawn'};}
  replayInfo(bytes){
    const pointer=this.core.malloc(bytes.length);
    try{
      new Uint8Array(this.core.memory.buffer,pointer,bytes.length).set(bytes);
      const info=this.core.presentation_lab_replay_info(pointer,bytes.length);
      const words=Array.from(new Uint32Array(this.core.memory.buffer,info,20));
      return {count:words[0],character:words[1],subtype:words[2],difficulty:words[3],stages:words.slice(4,12),frames:words.slice(12,20)};
    }finally{this.core.free(pointer);}
  }
  loadDemo(index){const result=this.core.presentation_lab_demo(index);if(result)throw Error('TH11 demo start failed: '+result);}
  loadReplay(bytes,stage=0){
    const pointer=this.core.malloc(bytes.length);
    try{
      new Uint8Array(this.core.memory.buffer,pointer,bytes.length).set(bytes);
      const result=this.core.presentation_lab_replay(pointer,bytes.length,stage);
      if(result)throw Error('TH11 replay start failed: '+result);
    }finally{this.core.free(pointer);}
  }
  setInput({code,down}){const scan=KEYCODES[code];if(scan===undefined)return;this.core.th11_key(scan,down?1:0);}
  // Diagnostic scene seeding. Starts the real production stage lifecycle
  // (same restart path as the title menu) without consuming stored inputs.
  startStage({stage=1,character=0,subtype=0,difficulty=1}={}){
    if(this.core.presentation_lab_start_stage(stage,character,subtype,difficulty)!==0)return false;
    for(let i=0;i<8;++i){const result=this.core.presentation_lab_tick();if(result!==0)break;if(!this.core.presentation_lab_world_frozen())break;}
    return true;
  }
  clearInput(){this.core.th11_keys_clear();}
  close(){this.token=null;this.core.th11_loop_stop();}
}

export class Th11ObservationAdapter{
  constructor(runtime){this.runtime=runtime;this.core=runtime.core;}
  observationApiVersion=OBSERVATION_API_VERSION;
  scanSchema='th11/presentation-scan/1';
  sessionSchema='th11/presentation-session/1';
  enable(on){this.core.audit_enable(on?1:0);}
  readRecordSet(reference=null){
    const pointer=reference===null?this.core.audit_records():this.core.audit_reference(reference);
    const count=reference===null?this.core.audit_count():this.core.audit_reference_count(reference);
    const simulationTick=reference===null?this.core.audit_tick():this.core.audit_reference_tick(reference);
    const referenceDrawSerial=reference===null?this.core.audit_draw_serial():this.core.audit_reference_draw_serial(reference);
    const dropped=reference===null?this.core.audit_dropped():this.core.audit_reference_dropped(reference);
    return {sessionEpoch:this.core.audit_session_epoch(),simulationTick,referenceDrawSerial,
      completeness:dropped?'incomplete':'complete',dropped,
      records:decodeRecords(this.core.memory.buffer,pointer,count,this.core.audit_stride()),
      limitations:['non-ANM screen rectangles are not observed','HUD text lifetime is not observed','audio queues and storage writes are outside the draw transaction','title/ending embedded VMs are not observed']};
  }
  readReferences(){const value={previous:this.readRecordSet(0),current:this.readRecordSet(1)};markLifecycle(value.previous,value.current);return value;}
  readObservation(){return this.readRecordSet();}
  readStateEvidence(){
    const buffer=this.core.memory.buffer,pointer=this.core.audit_state(),bytes=STATE_GROUPS.length*Uint32Array.BYTES_PER_ELEMENT;
    if(!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+bytes>buffer.byteLength)return {coverageVersion:'th11/state/1',groups:[],missingGroups:['all']};
    const words=new Uint32Array(buffer,pointer,STATE_GROUPS.length);
    return {coverageVersion:'th11/state/1',groups:Array.from(words,(digest,index)=>({id:STATE_GROUPS[index],digest,includedFields:[STATE_GROUPS[index]]})),
      missingGroups:['非 ANM 屏幕矩形 (solid_rectangle)','音频设备与队列','HUD 文本生命周期','标题/结局菜单 VM','存档写入']};
  }
  readTiming(){return decodeTiming();}
  readTrace(){return [];}
  readScene(){return {status:0,diagnostics:[]};}
  worldFrozen(){return !!this.core.presentation_lab_world_frozen();}
  readGate(){return !!this.core.presentation_lab_gate();}
  setNegativeControl(on){this.core.presentation_lab_fault(on?1:0);}
  captureImage(frame,alpha){return {alpha,png:this.runtime.Module?.canvas?.toDataURL?.()||null,boxes:frame.records.map(record=>({key:record.key,bounds:record.bounds}))};}
  analyze(input){return analyzeNormalizedWindow(input);}
  compact(report,options){return compactReport(report,options);}
  mergeIssues(reports){return mergeIssueGroups(reports);}
}
