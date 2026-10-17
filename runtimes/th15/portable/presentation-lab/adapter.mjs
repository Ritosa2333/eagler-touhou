import {PresentationLabControllerCore} from '../../third_party/eagler-common/testkit/presentation-lab/controller-core.mjs';
import {analyzeNormalizedWindow,compactReport,mergeIssueGroups} from '../../third_party/eagler-common/testkit/presentation-lab/analyzer.mjs';
export const requiredExports=['th15_lab_freeze','th15_lab_resume','th15_lab_tick','th15_lab_draw','th15_lab_reference_clock','th15_lab_world_frozen'];
const words=(core,name,n)=>Array.from(new Uint32Array(core.HEAPU8.buffer,core[name](),n));
const missing=['all non-player visual owners','complete Replay/input and host-clock state','complete ANM pools and rollback state','audio device and queues'];
export class Driver{
 constructor(runtime,identity){this.runtime=runtime;this.core=runtime.core;this.epoch=identity.runtimeEpoch;this.generation=0;this.token=null;for(const name of requiredExports)if(typeof this.core['_'+name]!=='function')throw Error('Missing diagnostic ABI '+name);}
 describe(){return {driverApiVersion:1,game:'th15',adapterVersion:'th15-presentation-lab/1',nativeAbi:'th15/player-submission/1',features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true,replay:false}};}
 freeze(){if(!this.token){if(!this.core._th15_lab_freeze())throw Error('Freeze failed');this.token=Object.freeze({sessionEpoch:this.epoch,generation:++this.generation});}return this.token;}
 resume(token){if(!this.token||token!==this.token)throw Error('Stale freeze token');if(!this.core._th15_lab_resume())throw Error('Resume failed');this.token=null;}
 advanceOneTick(){const status=['error','advanced','blocked-loading','finished'][this.core._th15_lab_tick()]||'error';return {status,advancedTicks:status==='advanced'?1:0};}
 drawOnly({alpha}){return {status:this.core._th15_lab_draw(alpha)?'drawn':'error'};}
 setInput({code,down}){const scan=this.runtime.scanCodes[code];if(scan)this.core._th15_key(scan,+down);}
 clearInput(){this.core._th15_keys_clear();}
 close(){this.token=null;this.runtime.stop();}
}
export class Observer{
 constructor(runtime,identity){this.runtime=runtime;this.core=runtime.core;this.epoch=identity.runtimeEpoch;}
 observationApiVersion=1;scanSchema='presentation-lab/scan/1';sessionSchema='presentation-lab/session/1';
 enable(on){if(on)this.core._th15_probe_presentation_enable(1);}
 frame(which=null){const c=this.core,ref=Array.from(new Float32Array(c.HEAPU8.buffer,c._th15_probe_presentation_reference(),5)),clock=words(c,'_th15_lab_reference_clock',5),sample=Array.from(new Float32Array(c.HEAPU8.buffer,c._th15_probe_presentation_sample(),3));return {sessionEpoch:this.epoch,simulationTick:clock[which===0?0:1],referenceDrawSerial:clock[which===0?2:3],...(which===null?{alpha:sample[0]}:{}),completeness:'incomplete',dropped:0,limitations:missing,records:ref[4]?[{ownerId:'player',objectId:'root',generation:ref[3],identityConfidence:'proven',partId:'first-vertex',drawId:'root',coordinateSpace:'submitted-screen',geometryQuality:'sampled',lifecycle:{continuous:Math.abs(ref[1]-ref[0])<128},fields:[{id:'vertex.x',value:ref[which===null?2:which],tolerance:.001,interpolationPolicy:'continuous',evidence:'Original Draw packet first vertex; generation from ANM identity, only X observed'}]}]:[]};}
 readReferences(){return {previous:this.frame(0),current:this.frame(1)};}
 readObservation(){return this.frame();}
 readStateEvidence(){return {coverageVersion:'th15/checkpoint-world/1',groups:[{id:'checkpoint-nine-modules',digest:JSON.stringify(words(this.core,'_th15_probe_checkpoint_digest',9))},{id:'world-probe',digest:JSON.stringify(words(this.core,'_th15_probe_world_state',16))}],missingGroups:missing};}
 readTiming(){return {schema:'presentation-lab/timing/1',completeness:'bounded',capacity:512,count:this.runtime.timing.length,rows:this.runtime.timing.slice(),limitations:['RAF callbacks with no presentation are not logged']};}
 readTrace(){return [];}
 readScene(){return {phase:this.core._th15_phase(),loadingMs:this.core._th15_probe_loading_remaining()};}
 worldFrozen(){return !!this.core._th15_lab_world_frozen();}
 readGate(){return !!this.core._th15_probe_loading_remaining();}
 setNegativeControl(on){this.core._th15_probe_presentation_fault(+on);}
 captureImage(frame,alpha){return {alpha,png:this.runtime.canvas.toDataURL(),boxes:[]};}
 analyze(input){return analyzeNormalizedWindow(input);}
 compact(report,options){return compactReport(report,options);}
 mergeIssues(reports){return mergeIssueGroups(reports);}
}
export class LabController extends PresentationLabControllerCore{
 constructor(runtime,identity){super({driver:new Driver(runtime,identity),observer:new Observer(runtime,identity),identity});this.runtime=runtime;}
}
