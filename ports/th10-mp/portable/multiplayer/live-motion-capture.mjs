// Test-only observation of real default framebuffer blits. It never schedules a
// game tick, changes alpha, draws an object, or waits synchronously for the GPU.
import {analyzeLiveMotion,liveMotionAnalysisPolicy} from './live-motion-analysis.mjs';
import {createLiveMotionTextureGuards} from './live-motion-texture-guards.mjs';

const FRAME_WORDS=352,RECORD_WORDS=80,HEADER_WORDS=32;
const MAX_ROI=96,MARGIN=12,OBJECTS=3,QUEUE_SLOTS=6,MAX_SAMPLES=4096;
const SLOT_BYTES=MAX_ROI*MAX_ROI*4*OBJECTS;
// Keep eight attempts in each original fixed section: four general and four
// reserved for visible nonzero remote-player endpoints. Early local/bullet
// movement cannot spend the remote reserve. Re-entry never resets either cap.
// Qualification uses pending metadata only, never a pixel/correlation result.
const WINDOWS=[[180,260],[700,780],[1100,1180]],WINDOW_SAMPLES=32;
const SECTION_FRAMES=20,SECTIONS=4,SECTION_SAMPLES=8,INVALID=0xffffffff;
const GENERAL_SAMPLES=4,REMOTE_SAMPLES=4;
const REJECT_FRAME=64|128|256|512|1024;
const TEXTURE_HEADER_WORDS=32,TEXTURE_MAX_SIDE=128;
const TEXTURE_SLOT_BYTES=TEXTURE_HEADER_WORDS*4+TEXTURE_MAX_SIDE*TEXTURE_MAX_SIDE*4;
const TEXTURE_CAPACITY=WINDOWS.length*WINDOW_SAMPLES*2;

export const liveMotionPixelSelectionPolicy=Object.freeze({
 schema:2,windows:Object.freeze(WINDOWS.map(pair=>Object.freeze(pair.slice()))),
 sectionsPerWindow:SECTIONS,sectionFrames:SECTION_FRAMES,attemptsPerSection:SECTION_SAMPLES,
 attemptsPerWindow:WINDOW_SAMPLES,maximumAttempts:WINDOWS.length*WINDOW_SAMPLES,
 generalAttemptsPerSection:GENERAL_SAMPLES,remoteAttemptsPerSection:REMOTE_SAMPLES,
 remoteReserveMinimumForegroundAlpha:liveMotionAnalysisPolicy.minimumForegroundAlpha,
 remoteReserveSource:'eligible nonlocal player slots 0/1, exact nonzero display XY, all four submitted vertex alpha values at least the existing foreground minimum',
 remoteReservePriority:'eligible remote reserve first; otherwise general; unused reserve is never borrowed',
 exactNonzeroDisplayMotion:true,failedAttemptsReplenished:false,rewindResetsBudget:false,
 completionRequirement:'unused quotas are allowed; every charged attempt must complete a PBO readback, including no-supported-roi and other failures in the attempted/completed check',
 selectionSource:'current pending metadata only; all real commits retained for final analysis',
});

// These are the pending-checkable structural rules from the existing analyzer.
// Presented binding is deliberately NOT fabricated here: afterCallback and the
// unchanged analyzer still validate the actual committed 352-word record.
// Keep the bounded parity tests when either ABI/eligibility implementation moves.
export function inspectLiveMotionPendingPixelEligibility(sample){
 const bad=reason=>({eligible:false,reason,frameReason:reason,objects:[]});
 const w=sample?.words;
 if(!Number.isInteger(sample?.index)||sample.index<0||sample.index>INVALID||!Number.isFinite(sample.atMs))
  return bad('invalid-sample-envelope');
 if(!(w instanceof Uint32Array)||w.length!==FRAME_WORDS||w[0]!==1||w[1]!==FRAME_WORDS||
    w[2]!==RECORD_WORDS||w[3]!==4)return bad('invalid-observer-abi');
 const f=new Float32Array(w.buffer,w.byteOffset,w.length),aux=HEADER_WORDS+3*RECORD_WORDS;
 const bases=[HEADER_WORDS,HEADER_WORDS+RECORD_WORDS,HEADER_WORDS+2*RECORD_WORDS];
 const frontiers=[null,null];
 const mask=((w[bases[0]+7]&256)?1:0)|((w[bases[1]+7]&256)?2:0);
 let invalidFrontier=!!(w[bases[2]+7]&256);
 if(!mask)invalidFrontier ||= w[aux]===0x52465031;
 else{
  invalidFrontier ||= w[aux]!==0x52465031||w[aux+1]!==1||w[aux+2]!==mask||
   w[aux+3]!==16||w[aux+4]!==2||w[aux+5]!==0||w[aux+6]!==0||w[aux+7]!==0;
  for(let i=40;i<RECORD_WORDS;++i)invalidFrontier ||= w[aux+i]!==0;
  for(let slot=0;slot<2;++slot){
   const at=aux+8+slot*16,b=bases[slot];
   if(!(mask&(1<<slot))){for(let i=0;i<16;++i)invalidFrontier ||= w[at+i]!==0;continue;}
   invalidFrontier ||= w[b]!==1||w[b+5]!==slot||slot===w[15]||!w[at]||
    w[at+1]===INVALID||w[at+2]!==w[17]||w[at+1]>w[at+2]||w[at+3]>w[at+4]||
    (w[at+5]|0)!==(w[b+6]|0)||![1,3,7].includes(w[at+7])||w[at+14]!==0||w[at+15]!==0;
   for(let i=0;i<3;++i)invalidFrontier ||= !Number.isFinite(f[at+8+i])||
    !Number.isFinite(f[at+11+i])||w[at+11+i]!==w[b+31+i]||
    ((w[at+7]&2)!==0&&w[at+8+i]!==w[at+11+i]);
   frontiers[slot]=at;
  }
 }
 if(invalidFrontier)return bad('invalid-completed-frontier-extension');
 if(w[9]!==2||w[8]!==0||(w[10]&32))return bad('unmatched-commit-header');
 if((w[10]&31)!==31||(w[10]&REJECT_FRAME))return bad('unmapped-or-unsafe-draw');
 if(!w[4]||!w[5]||!w[6]||!w[7]||!w[24]||w[17]===INVALID||
    ![2,3].includes(w[16])||w[15]>=w[16]||!(w[12]||w[13]))return bad('invalid-frame-identity');
 if(!Number.isFinite(f[28])||!Number.isFinite(f[29])||f[28]<0||f[28]>1||f[29]<0||f[29]>1)
  return bad('invalid-alpha');
 const blit=[0,0,w[25],w[26],0,480,640,0,0x4000,0x2600];
 if(sample.bufferSize?.length!==2||sample.bufferSize[0]!==640||sample.bufferSize[1]!==480||
    sample.blit?.length!==blit.length||!blit.every((value,i)=>sample.blit[i]===value)||!w[25]||!w[26])
  return bad('unsupported-blit-or-buffer');
 let owners=0,submitted=0,rejected=0;
 for(let slot=0;slot<4;++slot){const b=HEADER_WORDS+slot*RECORD_WORDS;
  if(w[b+7]&1){++owners;if(w[b+7]&2)++submitted;if(w[b+8])++rejected;}}
 if(owners!==w[27]||submitted!==w[30]||rejected!==w[31]||(w[aux+7]&1))
  return bad('invalid-object-accounting');
 if((w[10]&(2048|4096|8192))||(w[20]&0x74))return bad('frozen-replay-or-open-frame');
 if(w[21]!==INVALID)return bad('recovery-pending');
 if(w[22]>=liveMotionAnalysisPolicy.predictionLimit)return bad('network-prediction-limit');
 const objects=bases.map((b,slot)=>{
  const no=reason=>({slot,status:'not-covered',reason});
  if(!(w[b+7]&1))return no('owner-not-seen');
  if(w[b+8])return no('native-rejection');
  if(w[b+1]!==slot||(slot<2?w[b]!==1||w[b+5]!==slot||w[b+3]!==slot||w[b+2]!==slot:
     w[b]!==2||w[b+2]>=2000||(w[b+6]|0)!==1)||!w[b+4]||w[b+79]!==0)
   return no('unsupported-object-identity');
  if(w[b+7]&128)return no('new-incarnation');
  if(w[11]!==2||(w[b+7]&63)!==63||w[b+9]!==1)return no('not-interpolated-screen-quad');
  const frontier=slot<2?frontiers[slot]:null;
  const previous=frontier===null?b+28:frontier+8,current=frontier===null?b+31:frontier+11;
  if((w[b+6]|0)!==(w[frontier===null?b+78:frontier+5]|0)||w[b+26]!==w[17]||w[b+26]===INVALID)
   return no('discontinuous-endpoints');
  for(let i=0;i<3;++i)if(!Number.isFinite(f[previous+i])||!Number.isFinite(f[current+i]))
   return no('discontinuous-endpoints');
  if(!w[b+19]||w[b+21]!==w[24])return no('resource-target-unmatched');
  const x=w[b+22],y=w[b+23],width=w[b+24],height=w[b+25];
  if(!width||!height||x+width>w[25]||y+height>w[26])return no('unsupported-viewport');
  const points=[];let visible=false;
  for(let vertex=0;vertex<4;++vertex){const q=b+50+vertex*7;
   if(![f[q],f[q+1],f[q+2],f[q+3],f[q+5],f[q+6]].every(Number.isFinite)||f[q+3]<=0)
    return no('invalid-submitted-quad');
   if(f[q]+.5<x||f[q]+.5>x+width||f[q+1]+.5<y||f[q+1]+.5>y+height)
    return no('quad-clipped-by-viewport');
   visible ||= (w[q+4]>>>24)!==0;
   points.push([(f[q]+.5)*640/w[25],480-(f[q+1]+.5)*480/w[26]]);
  }
  if(!visible)return no('transparent-quad');
  const center=points.reduce((v,p)=>[v[0]+p[0]/4,v[1]+p[1]/4],[0,0]);
  points.sort((a,b)=>Math.atan2(a[1]-center[1],a[0]-center[0])-Math.atan2(b[1]-center[1],b[0]-center[0]));
  let area=0;for(let i=0;i<4;++i){const a=points[i],b=points[(i+1)%4];area+=a[0]*b[1]-a[1]*b[0];}
  if(!(area/2>=4))return no('degenerate-or-tiny-quad');
  return {slot,status:'eligible',reason:'eligible-object',
   moving:f[previous]!==f[current]||f[previous+1]!==f[current+1],
   endpointSource:frontier===null?'logical-tick':'completed-display-frontier',
   bounds:[Math.min(...points.map(p=>p[0])),Math.min(...points.map(p=>p[1])),
    Math.max(...points.map(p=>p[0])),Math.max(...points.map(p=>p[1]))]};
 });
 const eligible=objects.some(o=>o.status==='eligible'&&o.moving);
 return {eligible,frameReason:'eligible-frame',objects,reason:eligible?'eligible-moving-quad':
  objects.some(o=>o.status==='eligible')?'stationary-display-endpoints':'no-eligible-object'};
}

function remoteReserveEligibility(sample,eligibility){
 if(eligibility.frameReason!=='eligible-frame')
  return {eligible:false,reason:eligibility.frameReason,slots:[]};
 const remote=eligibility.objects.filter(o=>o.slot<2&&o.slot!==sample.words[15]);
 const ready=remote.filter(o=>o.status==='eligible');
 if(!ready.length)return {eligible:false,reason:'remote-object-not-eligible',slots:[]};
 const moving=ready.filter(o=>o.moving);
 if(!moving.length)return {eligible:false,reason:'remote-stationary-display-endpoints',slots:[]};
 const slots=moving.filter(o=>{
  const b=HEADER_WORDS+o.slot*RECORD_WORDS;
  for(let vertex=0;vertex<4;++vertex)
   if((sample.words[b+54+vertex*7]>>>24)/255<liveMotionAnalysisPolicy.minimumForegroundAlpha)return false;
  return true;
 }).map(o=>o.slot);
 return {eligible:slots.length>0,slots,reason:slots.length?'eligible-remote-reserve':
  'remote-foreground-opacity-insufficient'};
}

// Charge before any ROI/getter/queue/GL operation. An unsuccessful attempt
// still spends its chosen class. No success callback, quota borrowing, reset,
// endpoint magnitude, pixel result, or later sample affects the decision.
export function createLiveMotionPixelSchedule(){
 const counters=()=>({attempted:0,scheduled:0,generalAttempted:0,remoteAttempted:0,
  generalScheduled:0,remoteScheduled:0});
 const windows=WINDOWS.map(([first,last])=>({first,last,...counters(),
  sections:Array.from({length:SECTIONS},(_,index)=>({first:first+index*SECTION_FRAMES,
   last:index===SECTIONS-1?last:first+(index+1)*SECTION_FRAMES-1,...counters(),
   remoteReserveSkipReasons:{}}))}));
 function select(sample){
  const frame=sample?.words?.[17],windowIndex=windows.findIndex(w=>frame>=w.first&&frame<=w.last);
  if(windowIndex<0)return {attempted:false,reason:'outside-fixed-window'};
  const window=windows[windowIndex],sectionIndex=Math.min(SECTIONS-1,Math.floor((frame-window.first)/SECTION_FRAMES));
  const section=window.sections[sectionIndex],where={window:windowIndex,section:sectionIndex};
  if(section.attempted>=SECTION_SAMPLES)return {...where,attempted:false,budgetClass:null,
   reason:'fixed-section-budget-exhausted',remoteReserveReason:'reserved-budget-exhausted'};
  const eligibility=inspectLiveMotionPendingPixelEligibility(sample);
  const reserve=remoteReserveEligibility(sample,eligibility),remoteRemaining=section.remoteAttempted<REMOTE_SAMPLES;
  const remoteReserveReason=remoteRemaining?reserve.reason:'reserved-budget-exhausted';
  const detail={...where,eligibility,remoteReserveReason,qualifiedRemoteSlots:reserve.slots};
  if(remoteRemaining&&!reserve.eligible)
   section.remoteReserveSkipReasons[reserve.reason]=(section.remoteReserveSkipReasons[reserve.reason]||0)+1;
  if(!eligibility.eligible)return {...detail,attempted:false,budgetClass:null,reason:eligibility.reason};
  const budgetClass=remoteRemaining&&reserve.eligible?'remote':
   section.generalAttempted<GENERAL_SAMPLES?'general':null;
  if(!budgetClass)return {...detail,attempted:false,budgetClass:null,reason:'general-budget-exhausted-remote-reserved'};
  const key=budgetClass==='remote'?'remoteAttempted':'generalAttempted';
  ++section[key];++window[key];
  ++section.attempted;++window.attempted;
  return {...detail,attempted:true,budgetClass,reason:'fixed-section-attempt',
   attemptInBudgetClass:section[key],attemptInSection:section.attempted,attemptInWindow:window.attempted};
 }
 return {windows,select};
}

export function createLiveMotionCapture({core,app,gl},{enabled=false,startedAtPerformanceMs=0}={}){
 if(!enabled)return {enabled:false,afterBlit(){},afterCallback(){},finish(){return {enabled:false};}};
 for(const key of ['multiplayer_motion_observation_set_enabled','multiplayer_motion_observation',
                  'multiplayer_motion_observation_status','multiplayer_motion_texture_set_enabled',
                  'multiplayer_motion_texture_capture'])
  if(typeof core[key]!=='function')throw Error('Missing live motion export: '+key);
 if(!gl||typeof gl.fenceSync!=='function'||typeof gl.getBufferSubData!=='function')
  throw Error('Live motion requires the actual WebGL2 context');
 const wordsArena=new Uint32Array(MAX_SAMPLES*FRAME_WORDS*2);
 const pixelArena=new Uint8Array(WINDOWS.length*WINDOW_SAMPLES*SLOT_BYTES);
 const textureArena=new Uint8Array(TEXTURE_CAPACITY*TEXTURE_SLOT_BYTES);
 const textureGuards=createLiveMotionTextureGuards({core,app});
 const pixelSchedule=createLiveMotionPixelSchedule(),windows=pixelSchedule.windows;
 const samples=[],errors=[];
 const slots=[],stats={defaultBlits:0,metadata:0,pixelAttempts:0,pixelCompleted:0,
  metadataOverflow:0,queueOverflow:0,waitFailed:0,bindingMismatches:0,unsupportedRoi:0,
  pendingAtStop:0,readbackErrors:0,polls:0,maximumInFlight:0,
  textureAttempts:0,textureAccepted:0,textureRejected:0,textureErrors:0,textureStatuses:{},
  pixelSelectionReasons:{}};
 let stopped=false,enabledNative=false,awaiting=[],pixelIndex=0,textureIndex=0,dropped=0;
 const fail=(where,error)=>{if(errors.length<32)errors.push({where,message:String(error)});};
 const readWords=(phase,destination)=>{
  const pointer=core.multiplayer_motion_observation(app,phase);
  if(!pointer||pointer%4||pointer+FRAME_WORDS*4>core.memory.buffer.byteLength)
   throw Error('Invalid live motion frame pointer');
  destination.set(new Uint32Array(core.memory.buffer,pointer,FRAME_WORDS));
  if(destination[0]!==1||destination[1]!==FRAME_WORDS||destination[2]!==RECORD_WORDS||destination[3]!==4)
   throw Error('Unexpected live motion ABI');
  return destination;
 };
 const readStatus=()=>{
  const pointer=core.multiplayer_motion_observation_status();
  if(!pointer||pointer%4||pointer+64>core.memory.buffer.byteLength)throw Error('Invalid motion status pointer');
  return Array.from(new Uint32Array(core.memory.buffer,pointer,16));
 };
 const oldPack=gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING);
 try{
  textureGuards.beforeEnable();
  for(let i=0;i<QUEUE_SLOTS;++i){
   const buffer=gl.createBuffer();if(!buffer)throw Error('PBO allocation failed');
   slots.push({buffer,fence:null,sample:null,bytes:0,destination:0});
   gl.bindBuffer(gl.PIXEL_PACK_BUFFER,buffer);gl.bufferData(gl.PIXEL_PACK_BUFFER,SLOT_BYTES,gl.STREAM_READ);
  }
  if(core.multiplayer_motion_observation_set_enabled(app,1)!==1)throw Error('Motion observer rejected enable');
  enabledNative=true;
  if(core.multiplayer_motion_texture_set_enabled(app,1)!==1)throw Error('Texture observer rejected enable');
 }catch(error){
  if(enabledNative){core.multiplayer_motion_observation_set_enabled(app,0);enabledNative=false;}
  for(const slot of slots)gl.deleteBuffer(slot.buffer);
  throw error;
 }finally{gl.bindBuffer(gl.PIXEL_PACK_BUFFER,oldPack);}

 function disposeSlot(slot){
  if(slot.fence)gl.deleteSync(slot.fence);
  slot.fence=null;slot.sample=null;slot.bytes=0;
 }
 function poll(){
  ++stats.polls;
  for(const slot of slots){
   if(!slot.fence)continue;
   let ready;
   try{ready=gl.clientWaitSync(slot.fence,0,0);}catch(error){
    ++stats.readbackErrors;slot.sample.readbackStatus='failed';fail('clientWaitSync',error);disposeSlot(slot);continue;
   }
   if(ready===gl.TIMEOUT_EXPIRED)continue;
   if(ready===gl.WAIT_FAILED){++stats.waitFailed;slot.sample.readbackStatus='failed';
    fail('clientWaitSync','WAIT_FAILED');disposeSlot(slot);continue;}
   if(ready!==gl.ALREADY_SIGNALED&&ready!==gl.CONDITION_SATISFIED){
    ++stats.readbackErrors;slot.sample.readbackStatus='failed';fail('clientWaitSync','Unexpected result '+ready);
    disposeSlot(slot);continue;
   }
   const pack=gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING);
   try{
    gl.bindBuffer(gl.PIXEL_PACK_BUFFER,slot.buffer);
    const destination=pixelArena.subarray(slot.destination,slot.destination+slot.bytes);
    gl.getBufferSubData(gl.PIXEL_PACK_BUFFER,0,destination);
    for(const roi of slot.sample.pixels)
     roi.rgba=pixelArena.subarray(slot.destination+roi.byteOffset,slot.destination+roi.byteOffset+roi.width*roi.height*4);
    slot.sample.readbackStatus='complete';++stats.pixelCompleted;
   }catch(error){++stats.readbackErrors;slot.sample.readbackStatus='failed';fail('getBufferSubData',error);}
   finally{gl.bindBuffer(gl.PIXEL_PACK_BUFFER,pack);disposeSlot(slot);}
  }
 }
 function rectangles(eligibility){
  const out=[];
  for(const object of eligibility.objects){
   if(object.status!=='eligible')continue;
   const [minX,minY,maxX,maxY]=object.bounds,slot=object.slot;
   const left=Math.floor(minX)-MARGIN,right=Math.ceil(maxX)+MARGIN;
   const bottom=Math.floor(minY)-MARGIN,top=Math.ceil(maxY)+MARGIN;
   const width=right-left,height=top-bottom;
   // Never crop an oversized/edge object and silently call the crop covered.
   if(left<0||bottom<0||right>640||top>480||width<=0||height<=0||width>MAX_ROI||height>MAX_ROI){
    ++stats.unsupportedRoi;continue;
   }
   out.push({slot,x:left,y:bottom,width,height});
  }
  return out;
 }
 function submitPixels(sample,selection){
  // select() already charged this attempt. No failure path replenishes it.
  const window=windows[selection.window],section=window.sections[selection.section];
  const rois=rectangles(selection.eligibility);
  if(!rois.length){sample.readbackStatus='no-supported-roi';return;}
  const slot=slots.find(item=>!item.fence);
  if(!slot){sample.readbackStatus='dropped';++dropped;++stats.queueOverflow;return;}
  // Retain already copied packets even if a later getter or PBO submit fails.
  sample.pixels=rois;
  // Read only the CPU copy of each actually submitted player texture. Native
  // validates the pending draw/owner/version; it never reads or uploads the GPU.
  // Copy immediately: the native fixed packet is reused by the next getter.
  for(const roi of rois)if(roi.slot<2){
   try{readTexture(sample,roi);}
   catch(error){++stats.textureErrors;roi.textureError=String(error);throw error;}
  }
  textureGuards.afterSubmit(sample,rois);
  const read=gl.getParameter(gl.READ_FRAMEBUFFER_BINDING),pack=gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING);
  const stores=[gl.PACK_ALIGNMENT,gl.PACK_ROW_LENGTH,gl.PACK_SKIP_PIXELS,gl.PACK_SKIP_ROWS];
  const values=stores.map(key=>gl.getParameter(key));
  let defaultReadBuffer=null,offset=0;
  try{
   gl.bindFramebuffer(gl.READ_FRAMEBUFFER,null);defaultReadBuffer=gl.getParameter(gl.READ_BUFFER);gl.readBuffer(gl.BACK);
   gl.bindBuffer(gl.PIXEL_PACK_BUFFER,slot.buffer);
   gl.pixelStorei(gl.PACK_ALIGNMENT,1);for(const key of stores.slice(1))gl.pixelStorei(key,0);
   for(const roi of rois){
    roi.byteOffset=offset;gl.readPixels(roi.x,roi.y,roi.width,roi.height,gl.RGBA,gl.UNSIGNED_BYTE,offset);
    offset+=roi.width*roi.height*4;
   }
   const fence=gl.fenceSync(gl.SYNC_GPU_COMMANDS_COMPLETE,0);
   if(!fence)throw Error('GPU fence allocation failed');
   slot.fence=fence;slot.sample=sample;slot.bytes=offset;slot.destination=pixelIndex++*SLOT_BYTES;
   sample.pixels=rois;sample.readbackStatus='pending';++window.scheduled;++section.scheduled;
   const scheduledKey=selection.budgetClass==='remote'?'remoteScheduled':'generalScheduled';
   ++window[scheduledKey];++section[scheduledKey];
   stats.maximumInFlight=Math.max(stats.maximumInFlight,slots.filter(item=>item.fence).length);
   gl.flush();
  }catch(error){
   sample.readbackStatus='failed';++stats.readbackErrors;fail('readPixels PBO submit',error);disposeSlot(slot);
  }finally{
   // READ_BUFFER belongs to the bound framebuffer; restore default's setting
   // before rebinding the caller's read framebuffer.
   if(defaultReadBuffer!==null)gl.readBuffer(defaultReadBuffer);
   gl.bindFramebuffer(gl.READ_FRAMEBUFFER,read);gl.bindBuffer(gl.PIXEL_PACK_BUFFER,pack);
   stores.forEach((key,i)=>gl.pixelStorei(key,values[i]));
  }
 }
 function readTexture(sample,roi){
  if(textureIndex>=TEXTURE_CAPACITY)throw Error('Texture evidence arena exhausted');
  ++stats.textureAttempts;
  const at=textureIndex++*TEXTURE_SLOT_BYTES,slot=roi.slot;
  const w=sample.words,b=HEADER_WORDS+slot*RECORD_WORDS;
  const pointer=core.multiplayer_motion_texture_capture(app,w[4],w[5],w[6],w[7],slot,w[b+19],w[b+20]);
  const memory=core.memory.buffer;
  if(!pointer||pointer%4||pointer+TEXTURE_HEADER_WORDS*4>memory.byteLength)
   throw Error('Invalid texture evidence pointer');
  const header=new Uint32Array(textureArena.buffer,at,TEXTURE_HEADER_WORDS);
  header.set(new Uint32Array(memory,pointer,TEXTURE_HEADER_WORDS));
  roi.texture={header,rgba:textureArena.subarray(at+TEXTURE_HEADER_WORDS*4,at+TEXTURE_HEADER_WORDS*4)};
  const bytes=header[23];
  if(header[0]!==0x4c4d5431||header[1]!==1||header[2]!==TEXTURE_HEADER_WORDS||
     header[24]!==TEXTURE_HEADER_WORDS*4||header[25]!==TEXTURE_MAX_SIDE||
     bytes>TEXTURE_MAX_SIDE*TEXTURE_MAX_SIDE*4||pointer+TEXTURE_HEADER_WORDS*4+bytes>memory.byteLength||
     (header[3]!==1&&bytes!==0))throw Error('Invalid texture evidence ABI or length');
  textureArena.set(new Uint8Array(memory,pointer+TEXTURE_HEADER_WORDS*4,bytes),at+TEXTURE_HEADER_WORDS*4);
  roi.texture.rgba=textureArena.subarray(at+TEXTURE_HEADER_WORDS*4,at+TEXTURE_HEADER_WORDS*4+bytes);
  const status=header[3];
  stats.textureStatuses[status]=(stats.textureStatuses[status]||0)+1;
  if(status===1)++stats.textureAccepted;else ++stats.textureRejected;
  return roi.texture;
 }
 function afterBlit(blit){
  if(stopped)return;
  ++stats.defaultBlits;
  if(samples.length>=MAX_SAMPLES){++stats.metadataOverflow;++dropped;return;}
  const index=samples.length,offset=index*FRAME_WORDS*2;
  const sample={index,atMs:performance.now()-startedAtPerformanceMs,
   words:wordsArena.subarray(offset,offset+FRAME_WORDS),
   presentedWords:wordsArena.subarray(offset+FRAME_WORDS,offset+FRAME_WORDS*2),
   blit:Array.from(blit),bufferSize:[gl.drawingBufferWidth,gl.drawingBufferHeight],
   bindingMatched:false,readbackStatus:'metadata-only',pixels:[],pixelSelection:null};
  samples.push(sample);awaiting.push(sample);++stats.metadata;
  try{
   readWords(1,sample.words);
   const selection=pixelSchedule.select(sample),{eligibility,...detail}=selection;
   if(eligibility){detail.frameReason=eligibility.frameReason;
    detail.objectReasons=eligibility.objects.map(o=>o.reason);
    detail.movingSlots=eligibility.objects.filter(o=>o.status==='eligible'&&o.moving).map(o=>o.slot);}
   sample.pixelSelection=detail;
   stats.pixelSelectionReasons[detail.reason]=(stats.pixelSelectionReasons[detail.reason]||0)+1;
   if(selection.attempted){++stats.pixelAttempts;submitPixels(sample,selection);}
  }catch(error){sample.readbackStatus='failed';fail('after real default blit',error);}
 }
 function bindPresented(sample,actual){
  sample.presentedWords.set(actual);
  const pending=sample.words;
  if(actual[9]!==3||!actual[8]||actual[10]!==((pending[10]|32)>>>0))return false;
  for(let i=0;i<FRAME_WORDS;++i)
   if(i!==8&&i!==9&&i!==10&&pending[i]!==actual[i])return false;
  return true;
 }
 function afterCallback(){
  if(stopped)return;
  const items=awaiting;awaiting=[];
  // Native RAII closes the callback after runtimeFinish returns. Observe its
  // final committed record in the subsequent microtask, never inside C++.
  queueMicrotask(()=>{
   if(stopped)return;
   if(items.length){
    try{
     const actual=new Uint32Array(FRAME_WORDS);readWords(0,actual);
     for(const sample of items)if(!(sample.bindingMatched=bindPresented(sample,actual)))++stats.bindingMismatches;
    }catch(error){fail('afterCallback binding',error);}
   }
   try{textureGuards.afterCallback();}catch(error){fail('texture callback boundary',error);}
   poll();
  });
 }
 function base64(bytes){
  let value='';for(let i=0;i<bytes.length;i+=16384)value+=String.fromCharCode(...bytes.subarray(i,i+16384));
  return btoa(value);
 }
 function finish(){
  if(stopped)return {enabled:true,error:'Live motion capture already stopped'};
  poll();stopped=true;
  for(const slot of slots){
   if(slot.fence){++stats.pendingAtStop;++dropped;slot.sample.readbackStatus='dropped';disposeSlot(slot);}
   gl.deleteBuffer(slot.buffer);
  }
  // Unlike an unused reserve, an already charged attempt is not optional.
  // This also rejects no-supported-roi, which the unchanged analyzer permits
  // as a metadata-only boundary. No failure receives a replacement attempt.
  if(stats.pixelAttempts!==stats.pixelCompleted)
   fail('pixel attempt completion','Charged attempts '+stats.pixelAttempts+' != completed readbacks '+stats.pixelCompleted+
    ' (includes no-supported-roi and any other charged attempt without a completed PBO readback)');
  let observerStatus=null;
  try{observerStatus=readStatus();}catch(error){fail('final observer status',error);}
  if(enabledNative){core.multiplayer_motion_texture_set_enabled(app,0);
   core.multiplayer_motion_observation_set_enabled(app,0);enabledNative=false;}
  let nativeTextureGuards;
  try{nativeTextureGuards=textureGuards.finish();
   if(!nativeTextureGuards.passed)fail('native texture guards',JSON.stringify(nativeTextureGuards.errors));}
  catch(error){fail('native texture guards finish',error);nativeTextureGuards={passed:false,error:String(error)};}
  let analysis;
  try{analysis=analyzeLiveMotion({samples,observerStatus,errors,dropped,textureEvidenceEnabled:true});}
  catch(error){fail('offline motion analysis',error);analysis={passed:false,error:String(error)};}
  return {schema:'th10-live-motion-capture/1',enabled:true,
   scope:'real submitted screen quads and asynchronous default-buffer ROIs; diagnostic, not clean performance or physical scanout',
   nativeStoppedBeforeAnalysis:true,defaultOff:true,nonblockingGpuWait:true,
   textureEvidenceEnabled:true,textureEvidenceSchema:'LMT1/1',
   textureEvidenceScope:'Submit-stamped CPU texture crops for independent foreground support; not final compositing or occlusion truth',
   sampleEncoding:'wordsBase64/presentedWordsBase64: 352 little-endian uint32. pixels[].rgbaBase64: default framebuffer, GL bottom-left RGBA8. pixels[].texture.headerWordsBase64: 32 little-endian uint32 LMT1. pixels[].texture.rgbaBase64: straight RGBA8 CPU source crop, row 0 is source cropY and source row 0 is texture v=0; top-to-bottom, no framebuffer Y flip.',
   maximumSamples:MAX_SAMPLES,maximumRoi:[MAX_ROI,MAX_ROI],roiMargin:MARGIN,
   pixelSelectionPolicy:liveMotionPixelSelectionPolicy,
   windows,stats,dropped,errors,observerStatus,nativeTextureGuards,analysis,
   samples:samples.map(sample=>({
    index:sample.index,atMs:sample.atMs,blit:sample.blit,bufferSize:sample.bufferSize,
    bindingMatched:sample.bindingMatched,readbackStatus:sample.readbackStatus,pixelSelection:sample.pixelSelection,
    wordsBase64:base64(new Uint8Array(sample.words.buffer,sample.words.byteOffset,sample.words.byteLength)),
    presentedWordsBase64:base64(new Uint8Array(sample.presentedWords.buffer,sample.presentedWords.byteOffset,sample.presentedWords.byteLength)),
    pixels:sample.pixels.map(roi=>({slot:roi.slot,x:roi.x,y:roi.y,width:roi.width,height:roi.height,
     rgbaBase64:roi.rgba?base64(roi.rgba):null,
     textureError:roi.textureError||null,
     texture:roi.texture?{
      headerWordsBase64:base64(new Uint8Array(roi.texture.header.buffer,roi.texture.header.byteOffset,roi.texture.header.byteLength)),
      rgbaBase64:base64(roi.texture.rgba)}:null}))
   }))};
 }
 return {enabled:true,afterBlit,afterCallback,finish};
}
