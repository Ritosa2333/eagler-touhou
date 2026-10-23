import {analyzeWindow,compactReport,decodeRecords,keyOf} from './analyzer.mjs';
import {STATE_GROUPS,STATE_AUTHORITY,STATE_FIELD_MAP,WORLD_OWNER_IDS,LIFECYCLE_TRACKED_OWNER_IDS,OWNERS} from './owners.mjs';
import {TH08_PRESENTATION_LAB_ABI} from './native-abi.mjs';

export const TH08_VISUAL_ALPHAS=Object.freeze([0,.25,.5,.75,1,.5,.5,1]);
const integer=(value,min=0,max=0xffffffff)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
const requireValue=(condition,message)=>{if(!condition)throw Error(message);};
const object=value=>value!==null&&typeof value==='object'&&!Array.isArray(value);

// Native uint32 words retain NaN payloads and signed zero. JSON mirrors use
// JSON's value semantics; they never replace those original words.
export function sameJSON(left,right){
  const scalar=value=>typeof value==='number'&&!Number.isFinite(value)?null:value;
  left=scalar(left);right=scalar(right);
  if(left===right)return true;
  if(Array.isArray(left)||Array.isArray(right))return Array.isArray(left)&&Array.isArray(right)&&
    left.length===right.length&&left.every((value,index)=>sameJSON(value===undefined?null:value,right[index]===undefined?null:right[index]));
  if(!object(left)||!object(right))return false;
  const a=Object.keys(left).filter(key=>left[key]!==undefined).sort(),b=Object.keys(right).filter(key=>right[key]!==undefined).sort();
  return a.length===b.length&&a.every((key,index)=>key===b[index]&&sameJSON(left[key],right[key]));
}

export function decodeNativeVisualRecord(words){
  requireValue(Array.isArray(words)&&words.length===190&&words.every(value=>integer(value)),
    'TH08 native visual record must contain exactly 190 uint32 words');
  const buffer=Uint32Array.from(words).buffer;
  return decodeRecords(buffer,0,1)[0];
}

function lifecyclePrefix(value,label){
  requireValue(object(value)&&value.available===true&&value.capacity===16384&&integer(value.count,0,16384)&&
    value.next===value.count%value.capacity&&value.dropped===0&&integer(value.epoch,1)&&integer(value.runtimeEpoch,1),`${label}: incomplete native lifecycle cursor`);
  requireValue(value.native?.schema==='th08/presentation-lifecycle-buffer/1'&&value.native.stride===20&&
    Array.isArray(value.native.words)&&value.native.words.length===value.count*5&&value.native.words.every(word=>integer(word)),
  `${label}: missing native lifecycle words`);
  const events=[];
  for(let index=0;index<value.count;index++){
    const [owner,object,generation,tick,operation]=value.native.words.slice(index*5,index*5+5);
    events.push({owner,object,generation,tick,operation});
  }
  requireValue(sameJSON(events,value.events),`${label}: lifecycle events differ from native words`);
  const active=new Map(),generations=new Map();
  for(const event of events){
    const key=`${event.owner}:${event.object}`;
    requireValue(LIFECYCLE_TRACKED_OWNER_IDS.has(event.owner)&&event.object!==0&&event.generation>0,
      `${label}: unregistered lifecycle owner or generation`);
    if(event.operation===1){
      requireValue(!active.has(key)&&event.generation===(generations.get(key)||0)+1,`${label}: duplicate or skipped lifecycle birth`);
      active.set(key,event.generation);generations.set(key,event.generation);
    }else{
      requireValue(event.operation===2&&active.get(key)===event.generation,`${label}: unmatched lifecycle retirement`);
      active.delete(key);
    }
  }
  return {...value,events};
}

export function readNativeLifecycle(value,label='TH08 lifecycle'){return lifecyclePrefix(value,label);}

function cursor(value,prefix,label){
  requireValue(object(value)&&integer(value.count,0,prefix.count)&&value.next===value.count%prefix.capacity&&
    value.dropped===0&&value.epoch===prefix.epoch,`${label}: reference lifecycle prefix is unavailable or belongs to another epoch`);
  return value;
}

function decodeFrame(frame,prefix,label){
  requireValue(object(frame)&&integer(frame.tick,1)&&frame.dropped===0,`${label}: invalid native frame or dropped records`);
  const native=frame.native;
  requireValue(native?.schema==='th08/presentation-record-buffer/1'&&native.stride===760&&integer(native.count,0,8192)&&
    native.runtimeEpoch===prefix.runtimeEpoch&&Array.isArray(native.words)&&native.words.length===native.count*190&&native.words.every(word=>integer(word)),
  `${label}: missing or malformed native 190-word buffer`);
  cursor(native.lifecycle,prefix,label);
  const records=[];
  for(let index=0;index<native.count;index++)records.push(decodeNativeVisualRecord(native.words.slice(index*190,index*190+190)));
  requireValue(sameJSON(records,frame.records),`${label}: decoded records differ from native words`);
  const keys=new Set();
  const active=new Map();
  for(const event of prefix.events.slice(0,native.lifecycle.count)){
    const key=`${event.owner}:${event.object}`;
    if(event.operation===1)active.set(key,event.generation);else active.delete(key);
  }
  for(const record of records){
    requireValue(integer(record.meta[0],1,OWNERS.length-1)&&record.meta[1]!==0,`${label}: unknown native owner or source`);
    const key=keyOf(record);requireValue(!keys.has(key),`${label}: duplicate native object key ${key}`);keys.add(key);
    if(LIFECYCLE_TRACKED_OWNER_IDS.has(record.meta[0]))requireValue(record.meta[13]===3&&
      active.get(`${record.meta[0]}:${record.meta[1]}`)===record.meta[12],`${label}: submitted generation is not alive in this reference prefix`);
  }
  return {...frame,records};
}

function state(value,label){
  requireValue(object(value)&&value.coverageVersion==='th08/state/3'&&Array.isArray(value.groups)&&
    value.groups.length===STATE_GROUPS.length&&Array.isArray(value.missingGroups)&&value.missingGroups.length===0&&
    sameJSON(value.authority,STATE_AUTHORITY),`${label}: incomplete or unregistered authority coverage`);
  for(let index=0;index<STATE_GROUPS.length;index++){
    const group=value.groups[index],id=STATE_GROUPS[index];
    requireValue(group?.id===id&&integer(group.digest)&&sameJSON(group.includedFields,STATE_FIELD_MAP[id]),
      `${label}: authority group ${id} has missing identity, fields or digest`);
  }
  return value;
}

export function readNativeWindow(report,{label=report?.label||'TH08 visual window',retainedLifecycle=null}={}){
  requireValue(report?.schema==='th08/presentation-audit/1'&&report.build?.game==='th08'&&
    report.build.nativeAbi===TH08_PRESENTATION_LAB_ABI&&integer(report.build.runtimeEpoch,1),`${label}: wrong title, ABI or Runtime epoch`);
  requireValue(report.detailsTruncated!==true,`${label}: truncated visual report`);
  const prefix=lifecyclePrefix(report.nativeLifecycle,label);
  requireValue(prefix.runtimeEpoch===report.build.runtimeEpoch,`${label}: native lifecycle belongs to another Runtime session`);
  if(retainedLifecycle!==null){
    const retained=lifecyclePrefix(retainedLifecycle,`${label} retained lifecycle`);
    requireValue(retained.epoch===prefix.epoch&&retained.runtimeEpoch===prefix.runtimeEpoch&&retained.count>=prefix.count&&
      sameJSON(retained.native.words.slice(0,prefix.count*5),prefix.native.words),`${label}: lifecycle prefix differs from the retained session`);
  }
  const raw=report.nativeFrames;
  requireValue(raw?.schema==='th08/presentation-native-frames/1'&&Array.isArray(raw.samples)&&raw.samples.length===8,
    `${label}: missing complete native frame window`);
  const previous=decodeFrame(raw.previous,prefix,`${label} previous`),current=decodeFrame(raw.current,prefix,`${label} current`);
  requireValue(previous.tick+1===current.tick&&previous.native.lifecycle.count<=current.native.lifecycle.count&&
    current.native.lifecycle.count===prefix.count,`${label}: references are not adjacent or lifecycle cursor is stale`);
  const samples=raw.samples.map((frame,index)=>{
    const sample=decodeFrame(frame,prefix,`${label} sample ${index}`);
    requireValue(sample.alpha===TH08_VISUAL_ALPHAS[index]&&sample.tick===current.tick&&
      sameJSON(sample.native.lifecycle,current.native.lifecycle),`${label}: alpha sample advanced time or lifecycle state`);
    return sample;
  });
  const receipt=report.readVerification;
  requireValue(receipt?.schema==='eagler-presentation/sweep-reads/1'&&Array.isArray(receipt.receipts)&&receipt.receipts.length===9,
    `${label}: 18 actual authority reads are required`);
  const reads=receipt.receipts.map((item,index)=>{
    requireValue(item?.phase===(index?'after-draw':'before')&&item.alpha===(index?TH08_VISUAL_ALPHAS[index-1]:null)&&
      Array.isArray(item.reads)&&item.reads.length===2,`${label}: missing repeated authority boundary ${index}`);
    const pair=item.reads.map((read,at)=>state(read,`${label} read ${index}:${at}`));
    requireValue(sameJSON(pair[0],pair[1]),`${label}: repeated observation changed authority state at boundary ${index}`);
    return pair;
  });
  const input={previous,current,samples,stateBefore:reads[0][0],statesAfter:[reads[0][1],...reads.slice(1).flat()],
    worldFrozen:report.worldFrozen,gate:report.gate,build:report.build,label:report.label,
    negativeControl:report.negativeControl,lifecycle:prefix};
  requireValue(typeof input.worldFrozen==='boolean'&&typeof input.gate==='boolean'&&typeof input.negativeControl==='boolean',
    `${label}: missing actual window flags`);
  const recomputed=analyzeWindow(input),expected=Object.hasOwn(report,'totalObjects')?compactReport(recomputed):recomputed;
  if(Object.hasOwn(report,'totalObjects'))requireValue(report.totalObjects===recomputed.objects.length,`${label}: visual object count differs`);
  for(const key of Object.keys(recomputed))requireValue(sameJSON(report[key],expected[key]),`${label}: derived ${key} differs from the actual native window`);
  return {...input,recomputed};
}

const wrappedDifference=(left,right,period)=>period?Math.abs(((right-left+period/2)%period+period)%period-period/2):Math.abs(right-left);
export function newbornSubmissionMatches(current,sample){
  if(!current||!sample||!sameJSON(current.meta,sample.meta)||current.spriteIdentity!==sample.spriteIdentity||
    current.values.length!==32||sample.values.length!==32||current.vertices.length!==sample.vertices.length)return false;
  for(let index=0;index<32;index++){
    if(index===29&&(current.meta[11]&(1<<20)))continue; // Opaque sprite pointer is checked as uint32 above.
    const a=current.values[index],b=sample.values[index];
    if(!Number.isFinite(a)||!Number.isFinite(b))return false;
    const period=index>=8&&index<=10?Math.PI*2:index===11||index===12?1:0;
    const tolerance=index>=13&&index<=20||index===30?1.01/255:index>=21&&index<=28?.025:.00001;
    if(wrappedDifference(a,b,period)>tolerance)return false;
  }
  for(let index=0;index<current.vertices.length;index++){
    const a=current.vertices[index],b=sample.vertices[index],slot=index%9;
    if(!Number.isFinite(a)||!Number.isFinite(b)||wrappedDifference(a,b,slot===3||slot===4?1:0)>(slot<3?.025:slot>=5?1.01/255:.00001))return false;
  }
  return true;
}

export function assertPauseRecoveryEvidence(pause,resumed){
  const paused=readNativeWindow(pause,{retainedLifecycle:resumed?.nativeLifecycle}),active=readNativeWindow(resumed);
  requireValue(pause.build.runtimeEpoch===resumed.build.runtimeEpoch&&pause.worldFrozen===true&&resumed.worldFrozen===false,
    'Pause/recovery must belong to one actual Runtime session');
  const progress=pause.pauseProgress,recovery=resumed.pauseRecovery;
  requireValue(progress?.schema==='th08/pause-progress/1'&&recovery?.schema==='th08/pause-recovery/1'&&
    progress.runtimeEpoch===pause.build.runtimeEpoch&&recovery.runtimeEpoch===resumed.build.runtimeEpoch,
  'Missing native pause progress/recovery evidence');
  const snapshot=(value,prefix,label)=>{
    requireValue(object(value)&&typeof value.worldFrozen==='boolean'&&Array.isArray(value.scene?.nativeScene)&&
      value.scene.nativeScene.length===16&&value.scene.nativeScene.every(word=>Number.isSafeInteger(word)&&word>=-0x80000000&&word<=0x7fffffff),
    `${label}: missing native scene words`);
    const reference=decodeFrame(value.reference,prefix,label);
    requireValue(value.tick===reference.tick,`${label}: snapshot tick differs from actual reference`);
    return {...value,reference};
  };
  const before=snapshot(progress.before,paused.lifecycle,'pause before'),after=snapshot(progress.after,paused.lifecycle,'pause after');
  requireValue(integer(progress.steps,2,120)&&after.tick===before.tick+progress.steps&&before.worldFrozen&&after.worldFrozen&&
    after.scene.nativeScene[5]===before.scene.nativeScene[5]&&after.scene.nativeScene[6]>=before.scene.nativeScene[6]+progress.steps&&
    sameJSON(after.reference.native,paused.current.native)&&progress.complete===true,
  'Pause did not retain a frozen world across multiple actual control ticks');
  requireValue(Array.isArray(progress.samples)&&progress.samples.length===progress.steps&&
    progress.samples.every((sample,index)=>sample.step===index+1&&sample.tick===before.tick+index+1&&
      sample.worldFrozen===true&&Array.isArray(sample.nativeScene)&&sample.nativeScene.length===16&&
      sample.nativeScene.every(word=>Number.isSafeInteger(word)&&word>=-0x80000000&&word<=0x7fffffff)&&
      sample.nativeScene[5]===before.scene.nativeScene[5]&&sample.nativeScene[6]>=before.scene.nativeScene[6]+index+1)&&
    sameJSON(progress.samples.at(-1).nativeScene,after.scene.nativeScene),
  'Pause lacks every real control step or a frozen world advanced between the endpoints');
  const start=snapshot(recovery.before,active.lifecycle,'resume before'),movement=snapshot(recovery.movementBefore,active.lifecycle,'movement before'),
    end=snapshot(recovery.after,active.lifecycle,'resume after');
  requireValue(sameJSON(start.reference.native,after.reference.native)&&sameJSON(start.scene.nativeScene,after.scene.nativeScene)&&
    start.worldFrozen&&!end.worldFrozen&&
    end.scene.nativeScene[5]>start.scene.nativeScene[5]&&sameJSON(end.reference.native,active.current.native),
  'Recovery did not resume the native world from this pause window');
  requireValue(Array.isArray(recovery.samples)&&recovery.samples.length>=1&&recovery.samples.length<=180&&
    recovery.samples.every((sample,index)=>sample.step===index+1&&sample.tick===start.tick+4+index&&
      typeof sample.worldFrozen==='boolean'&&Array.isArray(sample.nativeScene)&&sample.nativeScene.length===16)&&
    recovery.samples.at(-1).worldFrozen===false&&recovery.samples.at(-1).nativeScene[5]>start.scene.nativeScene[5],
  'Recovery lacks bounded actual native step observations');
  requireValue(recovery.samples.every(sample=>sample.nativeScene.every(word=>Number.isSafeInteger(word)&&word>=-0x80000000&&word<=0x7fffffff))&&
    movement.tick===recovery.samples.at(-1).tick&&sameJSON(movement.scene.nativeScene,recovery.samples.at(-1).nativeScene)&&
    !movement.worldFrozen&&end.tick===movement.tick+5,
  'Recovery input movement is not adjacent to the recorded native step observations');
  requireValue(recovery.samples.every(sample=>!sample.worldFrozen||sample.nativeScene[5]===start.scene.nativeScene[5]),
    'Recovery advanced the native world while its observed pause flag was still set');
  const moved=movement.reference.records.some(old=>old.meta[0]===13&&end.reference.records.some(now=>
    keyOf(old)===keyOf(now)&&(Math.abs(old.values[0]-now.values[0])>.01||Math.abs(old.values[1]-now.values[1])>.01)));
  requireValue(moved&&recovery.playerMoved===true&&recovery.keysReleased===true&&recovery.complete===true&&
    sameJSON(recovery.input,[{code:'Escape',down:true,ticks:3},{code:'Escape',down:false},{code:'ArrowRight',down:true,ticks:4},{code:'ArrowRight',down:false}]),
  'Recovery lacks actual released input and restored player motion');
  return {status:'passed',machineComplete:true,complete:false,pausedSteps:progress.steps,recoverySteps:recovery.samples.length,
    worldFramesBefore:start.scene.nativeScene[5],worldFramesAfter:end.scene.nativeScene[5],playerMoved:true,humanReview:'pending'};
}

// A material is another real sweep at the same frozen reference interval.
// PNG bytes are sealed by the producer's file/PNG hashes; this gate checks the
// image-to-native-record relationship, never infers pixels from metadata.
export function assertVisualMaterialEvidence(material,{sourceReport=null,retainedLifecycle=null}={}){
  const window=readNativeWindow(material,{retainedLifecycle});
  requireValue(window.recomputed.valid&&window.recomputed.purityStatus==='pass'&&material.negativeControl===false,
    'Visual material must retain a valid, pure native window');
  if(sourceReport!==null){
    const source=readNativeWindow(sourceReport,{retainedLifecycle});
    requireValue(material.label===`${sourceReport.label}/visual-window`&&sameJSON(material.build,sourceReport.build)&&
      material.tick===sourceReport.tick&&sameJSON(material.nativeFrames,sourceReport.nativeFrames)&&
      sameJSON(material.nativeLifecycle,sourceReport.nativeLifecycle)&&
      sameJSON(material.readVerification,sourceReport.readVerification)&&
      source.current.tick===window.current.tick,
    'Visual material differs from its recorded source window');
  }
  requireValue(Array.isArray(material.images)&&material.images.length===5,'Visual material requires exactly five actual PNG samples');
  for(let index=0;index<5;index++){
    const image=material.images[index],sample=window.samples[index];
    requireValue(image?.alpha===TH08_VISUAL_ALPHAS[index]&&typeof image.png==='string'&&
      /^data:image\/png;base64,iVBORw0KGgo[A-Za-z0-9+/]*={0,2}$/.test(image.png),
    `Visual material PNG ${index} is absent or belongs to another alpha`);
    const boxes=sample.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}));
    requireValue(sameJSON(image.boxes,boxes),`Visual material boxes ${index} differ from the native submissions`);
  }
  return {schema:'eagler-presentation/visual-material-validation/1',status:'passed',game:'th08',variant:'normal',
    runtimeEpoch:material.build.runtimeEpoch,tick:material.tick,label:material.label,images:5,
    humanReview:'pending',physicalRefreshVerified:false};
}

export const TH08_VISUAL_CONTRACT=Object.freeze({game:'th08',variant:'normal',nativeAbi:TH08_PRESENTATION_LAB_ABI,
  alphas:TH08_VISUAL_ALPHAS,recordWords:190,lifecycleWords:5,stateGroups:STATE_GROUPS,
  worldOwnerIds:WORLD_OWNER_IDS,lifecycleOwnerIds:LIFECYCLE_TRACKED_OWNER_IDS,
  keyOf,decodeNativeVisualRecord,readNativeWindow,readNativeLifecycle,newbornSubmissionMatches,assertPauseRecoveryEvidence,assertVisualMaterialEvidence});
