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
export class LabController {
  constructor(runtime,identity){
    this.runtime=runtime;this.core=runtime.core;this.identity=identity;this.history=[];this.last=null;this.running=false;this.keys=new Set();
    for(const key of ['audit_draw','audit_state','audit_reference','audit_stride','audit_timing_records','audit_timing_capacity','audit_timing_count','audit_timing_next','audit_timing_stride'])if(typeof this.core[key]!=='function')throw Error('Wrong runtime: missing '+key);
    this.core.audit_enable(1);
  }
  records(reference=null){
    const c=this.core,which=reference;
    const pointer=which===null?c.audit_records():c.audit_reference(which),count=which===null?c.audit_count():c.audit_reference_count(which);
    return {tick:which===null?c.audit_tick_id():c.audit_reference_tick(which),dropped:which===null?c.audit_dropped():c.audit_reference_dropped(which),
      records:decodeRecords(c.memory.buffer,pointer,count,c.audit_stride())};
  }
  state(){const c=this.core;return Array.from(new Uint32Array(c.memory.buffer,c.audit_state(),9));}
  timing(){
    const c=this.core;return decodeTimingRing(c.memory.buffer,{base:c.audit_timing_records(),capacity:c.audit_timing_capacity(),count:c.audit_timing_count(),next:c.audit_timing_next(),stride:c.audit_timing_stride()});
  }
  trace(){const c=this.core;return Array.from(new Uint32Array(c.memory.buffer,c.trace(this.runtime.app),68));}
  freeze(){this.core.sdl_loop_stop();this.running=false;}
  play(){this.core.audit_fault(0);this.core.sdl_loop_start();this.running=true;}
  key(code,down){
    const b=new TextEncoder().encode(code+'\0'),c=this.core,p=c.allocate(b.length);
    try{new Uint8Array(c.memory.buffer,p,b.length).set(b);c.sdl_key(p,down?1:0);if(down)this.keys.add(code);else this.keys.delete(code);}finally{c.deallocate(p);}
  }
  clearKeys(){this.core.sdl_keys_clear();this.keys.clear();}
  step(count=1){
    this.freeze();
    if(!Number.isInteger(count)||count<1||count>600)throw Error('Step size must be 1..600');
    for(let i=0;i<count;i++){const result=this.core.sdl_loop_tick(this.runtime.app,1/60,17);if(result)throw Error('Game tick stopped: '+result);}
    return this.records(1);
  }
  press(code,wait=40){this.key(code,true);this.step(3);this.key(code,false);this.step(wait);}
  preview(alpha,{world=!this.core.audit_world_frozen(),negativeControl=false}={}){
    this.freeze();this.core.audit_fault(negativeControl?1:0);
    try{if(this.core.audit_draw(Number(alpha),world?1:0))throw Error('Presentation draw failed');return this.records();}finally{this.core.audit_fault(0);}
  }
  sweep({label='',negativeControl=false,images=false,world=!this.core.audit_world_frozen()}={}){
    this.freeze();
    const previous=this.records(0),current=this.records(1),stateBefore=this.state(),traceBefore=this.trace(),samples=[],statesAfter=[],pictures=[];
    const alphas=[0,.25,.5,.75,1,.5,.5,1];
    for(const alpha of alphas){
      const frame=this.preview(alpha,{world,negativeControl});samples.push({...frame,alpha});statesAfter.push(this.state());
      if(images&&pictures.length<5)pictures.push({alpha,png:this.runtime.Module.canvas?.toDataURL?.()||null,boxes:frame.records.map(r=>({key:keyOf(r),bounds:r.values.slice(21,25)}))});
    }
    const report=analyzeWindow({previous,current,samples,stateBefore,statesAfter,worldFrozen:!world,gate:!!this.core.audit_gate(),build:this.identity,label,negativeControl});
    report.traceBefore=traceBefore;report.traceAfter=this.trace();report.scene={status:this.runtime.status(),diagnostics:Array.from(new Int32Array(this.core.memory.buffer,this.core.diagnostics(this.runtime.app),16))};
    if(images)report.images=pictures;
    this.last=report;this.history.push(compactReport(report,{maxObjects:96,images:false}));if(this.history.length>16)this.history.shift();
    return report;
  }
  mark(){this.freeze();const timing=this.timing(),report=this.sweep({label:'f8-incident',images:true});report.timing=timing;return report;}
  async scan({ticks=180,stride=6,negativeControl=false,onWindow=()=>{},signal}={}){
    if(!Number.isInteger(ticks)||ticks<1||ticks>3600||!Number.isInteger(stride)||stride<1||stride>60||Math.ceil(ticks/stride)>120)throw Error('Bounded scan: 1..3600 ticks, stride 1..60, at most 120 windows');
    const reports=[];let completedTicks=0;
    for(let n=0;n<ticks;n+=stride){
      if(signal?.aborted)break;
      this.step(Math.min(stride,ticks-n));completedTicks=Math.min(n+stride,ticks);const report=this.sweep({label:`scan+${completedTicks}`,negativeControl});reports.push(compactReport(report,{maxObjects:96,images:false}));onWindow(report);
      // Do not continue mutating a suspect simulation after an unsafe redraw.
      if(!report.purity)break;
      await new Promise(resolve=>setTimeout(resolve,0));
    }
    return {schema:'th08/presentation-scan/1',build:this.identity,reports,completedTicks,windows:reports.length,groups:mergeIssueGroups(reports),detailsBound:96,stoppedForPurity:reports.some(r=>!r.purity)};
  }
  export(){return {schema:'th08/presentation-session/1',build:this.identity,recent:this.history,last:this.last?compactReport(this.last):null};}
}
