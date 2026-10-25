import {createHash} from 'node:crypto';
import {practiceSnapshotRequest,readPracticeSnapshotRaw} from '../practice.mjs';
import {deriveTh08ReplayInputExpectation} from './th08-replay-format.mjs';
import {decodePracticeUiSnapshot,decodePracticeApplicationSnapshot} from '../practice-ui.mjs';
import {th08PracticeParameterCases,assertTh08PracticeParameters} from './th08-practice-parameters.mjs';
import {TH08_PRACTICE_TRAINER} from './th08-practice-effects.mjs';
import {TH08_PRACTICE_OPTIONS} from './th08-practice-options.mjs';
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const REPLAY_HEADER_BYTES=0x68;
const SNAPSHOT_WORDS=128,INITIAL_SNAPSHOT_WORDS=80;
const CONFIG_FIELDS=Object.freeze([
  'mode','stage','warp','section','phase','frame','dlg','score','life','bomb','power','gauge','graze',
  'point','point_total','point_stage','time','value','night','familiar','rank','rankLock','sentinel',
  'sp1_pts','bsX','bsY',
]);

// Fresh shell epochs start createPractice() with this source-owned default.
// It is captured here as an independent acceptance expectation.
export const TH08_PRACTICE_FRESH_CONFIG=Object.freeze({
  mode:1,stage:0,warp:0,section:0,phase:0,frame:0,dlg:false,score:0,life:2,bomb:8,power:128,
  gauge:0,graze:0,point:0,point_total:0,point_stage:0,time:0,value:60000,night:0,familiar:0,
  rank:12,rankLock:false,sentinel:1,sp1_pts:false,bsX:48,bsY:96,
});

const failure=message=>{throw new Error(`TH08 practice Replay metadata: ${message}`);};
function bytesOf(value){
  if(value instanceof Uint8Array)return value;
  if(ArrayBuffer.isView(value))return new Uint8Array(value.buffer,value.byteOffset,value.byteLength);
  if(value instanceof ArrayBuffer)return new Uint8Array(value);
  if(Array.isArray(value)&&value.every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255))return Uint8Array.from(value);
  failure('expected raw Replay bytes');
}
function sha256Of(value){return createHash('sha256').update(bytesOf(value)).digest('hex');}
function u32(bytes,offset,label){
  if(offset<0||offset+4>bytes.length)failure(`${label} is truncated`);
  return(bytes[offset]|(bytes[offset+1]<<8)|(bytes[offset+2]<<16)|(bytes[offset+3]<<24))>>>0;
}
function requiredInteger(value,key){
  if(!Number.isSafeInteger(value))failure(`PRAC ${key} must be a safe integer`);
  return value;
}
function optionalInteger(object,key,fallback=0){
  if(!Object.hasOwn(object,key))return fallback;
  return requiredInteger(object[key],key);
}
function optionalBoolean(object,key,fallback=false){
  if(!Object.hasOwn(object,key))return fallback;
  if(typeof object[key]!=='boolean')failure(`PRAC ${key} must be boolean`);
  return object[key];
}

/** Parse the plain USER/PRAC tail independently of the native PracticeConfig reader. */
export function parseTh08PracticeReplayMetadata(input){
  const bytes=bytesOf(input);
  if(bytes.length<REPLAY_HEADER_BYTES)failure('Replay is shorter than the 0x68-byte header');
  if(u32(bytes,0,'magic')!==0x50523854||bytes[4]!==6||bytes[5]!==0)failure('Replay must use th08 v6 format');
  const fileSize=u32(bytes,12,'encoded file size');
  if(fileSize<REPLAY_HEADER_BYTES||fileSize>bytes.length)failure('encoded file boundary is invalid');
  let position=fileSize;
  while(position+12<=bytes.length){
    if(String.fromCharCode(...bytes.subarray(position,position+4))!=='USER')break;
    const length=u32(bytes,position+4,'USER block length');
    if(length<12||length>bytes.length-position)failure('USER block length is invalid');
    if(u32(bytes,position+8,'USER block number')===0x43415250){
      const body=bytes.subarray(position+12,position+length);
      const end=body.indexOf(0);
      const jsonBytes=end<0?body:body.subarray(0,end);
      if(!jsonBytes.length)failure('PRAC payload is empty');
      let payload;
      try{payload=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(jsonBytes));}
      catch(error){failure(`PRAC JSON is invalid (${String(error)})`);}
      if(!payload||typeof payload!=='object'||Array.isArray(payload)||payload.game!=='th08'||
        typeof payload.version!=='string'||!payload.version.trim())failure('PRAC identity is not a th08 versioned object');
      const required=['mode','stage','life','bomb','power','gauge','score','graze','point_total','point_stage',
        'time','value','night','familiar','rank'];
      for(const key of required)if(!Object.hasOwn(payload,key))failure(`PRAC is missing required field ${key}`);
      const config={
        mode:requiredInteger(payload.mode,'mode'),stage:requiredInteger(payload.stage,'stage'),warp:0,
        section:optionalInteger(payload,'section'),phase:optionalInteger(payload,'phase'),frame:optionalInteger(payload,'frame'),
        dlg:optionalBoolean(payload,'dlg'),score:requiredInteger(payload.score,'score'),
        life:requiredInteger(payload.life,'life'),bomb:requiredInteger(payload.bomb,'bomb'),
        power:requiredInteger(payload.power,'power'),gauge:requiredInteger(payload.gauge,'gauge'),
        graze:requiredInteger(payload.graze,'graze'),point:0,point_total:requiredInteger(payload.point_total,'point_total'),
        point_stage:requiredInteger(payload.point_stage,'point_stage'),time:requiredInteger(payload.time,'time'),
        value:requiredInteger(payload.value,'value'),night:requiredInteger(payload.night,'night'),
        familiar:requiredInteger(payload.familiar,'familiar'),rank:requiredInteger(payload.rank,'rank'),
        rankLock:optionalBoolean(payload,'rankLock'),sentinel:1,sp1_pts:optionalBoolean(payload,'sp1_pts'),
        bsX:optionalInteger(payload,'bsX'),bsY:optionalInteger(payload,'bsY'),
      };
      if(![0,1].includes(config.mode)||config.stage<0||config.stage>9||config.life<0||config.life>8||
        config.bomb<0||config.bomb>8||config.power<0||config.power>128||config.rank<8||config.rank>99||
        (config.sp1_pts&&(config.bsX< -160||config.bsX>160||config.bsY<48||config.bsY>128)))
        failure('PRAC values are outside th08 PracticeConfig bounds');
      for(const key of Object.keys(payload))if(!['game','version',...required,'section','phase','frame','dlg','sp1_pts','bsX','bsY','rankLock'].includes(key))
        failure(`PRAC contains unsupported field ${key}`);
      return{schema:'eagler-touhou/th08-practice-replay-metadata/1',payload,config,
        block:{offset:position,bytes:length},source:'independent UTF-8 USER/PRAC tail parse after ReplayHeader.file_size'};
    }
    position+=length;
  }
  failure('Replay has no USER/PRAC block');
}

function configMatches(actual,expected){
  return Boolean(actual)&&CONFIG_FIELDS.every(key=>actual[key]===expected[key]);
}
function appliedMismatches(applied,config,selectionCase={}){
  if(!applied)return{expected:null,actual:null,fieldDiff:[{field:'applied',expected:'present',actual:null}]};
  const expected={score:Math.trunc(config.score/10),displayScore:Math.trunc(config.score/10),lives:config.life,
    bombs:config.bomb,power:config.power,gauge:config.gauge,gaugeCopy:config.gauge,graze:config.graze,
    grazeStage:config.graze,points:config.point||config.point_total,pointsStage:config.point||config.point_stage,
    timeOrbs:config.time,totalTimeOrbs:config.time,pointValue:Math.trunc(config.value/10)*10,
    clockTime:config.night,rank:config.rank,rankMinimum:config.rank,rankMaximum:config.rank,
    familiarCount:config.familiar,familiarPending:config.familiar!==0,stage:th08PracticeRuntimeStage(config),
    difficulty:selectionCase.difficulty,character:selectionCase.character};
  return{expected,actual:Object.fromEntries(Object.keys(expected).map(key=>[key,applied[key]])),
    fieldDiff:Object.entries(expected).filter(([key,value])=>applied[key]!==value)
      .map(([field,expected])=>({field,expected,actual:applied[field]}))};
}

/** Assess one real record/save/persist/fresh-epoch/import/native-menu playback chain. */
export function assessTh08PracticeReplayRoundTrip(evidence={}, {selectionCase=TH08_PRACTICE_SELECTION_CASE}={}){
  const missing=[];
  const requireValue=(condition,name)=>{if(!condition)missing.push(name);};
  requireValue(Number.isSafeInteger(evidence.recording?.frameBefore)&&Number.isSafeInteger(evidence.recording?.frameAfter)&&
    evidence.recording.frameAfter>evidence.recording.frameBefore,'recording gameplay tick');
  requireValue(evidence.recording?.saveSucceeded===true,'production save_replay finalizer');
  requireValue(evidence.recording?.idbfsSyncSucceeded===true,'completed Runtime IDBFS sync');
  requireValue(evidence.recording?.persistence?.finalizer?.afterFinalizer&&evidence.recording.persistence.observer&&evidence.recording.persistence.restored,'automatic IDBFS persistence receipts');
  requireValue(evidence.recording?.rawBytes!==undefined,'saved Replay bytes');
  requireValue(/^[a-f0-9]{64}$/.test(evidence.recording?.sha256??''),'saved Replay SHA-256');
  requireValue(evidence.persisted?.rawBytes!==undefined&&evidence.persisted?.freshEpochSha256!==undefined,
    'fresh epoch read of persisted slot');
  requireValue(evidence.import?.beforeLaunch===true&&evidence.import?.writeSucceeded===true&&
    evidence.import?.listed===true&&evidence.import?.rawBytes!==undefined,'official prelaunch Replay import and readback');
  requireValue(Number.isSafeInteger(evidence.recording?.inputCapacityFrames),'independently parsed Replay input/timing capacity');
  requireValue(evidence.fresh?.epoch!==undefined&&evidence.fresh?.epoch!==evidence.recording?.epoch,'new Runtime epoch');
  requireValue(evidence.fresh?.configured!==undefined,'new epoch initial configured parameters');
  requireValue(evidence.menu?.candidate!==undefined,'native Replay-menu candidate');
  requireValue(evidence.playback?.initialSnapshot?.schema==='eagler-touhou/th08-practice-initial-snapshot/1',
    'native immutable practice-application snapshot');
  requireValue(evidence.playback?.initialSnapshot?.state&&evidence.playback.initialSnapshot.cursor&&
    evidence.playback.initialSnapshot.run&&evidence.playback.initialSnapshot.applied,
    'complete immutable initial application state, run config and Replay cursor');
  requireValue(evidence.playback?.liveSnapshot!==undefined,'live native Replay gameplay snapshot');
  requireValue(evidence.playback?.liveSnapshot?.state&&evidence.playback.liveSnapshot.applied,
    'live native Replay state and applied values');
  requireValue(evidence.playback?.stream!==undefined,'native Replay stream cursor');
  if(missing.length)return{schema:'eagler-touhou/th08-practice-replay-roundtrip/1',status:'incomplete',complete:false,
    missing,scope:'recorded th08 practice parameters restored by the native Replay menu in a fresh Runtime epoch'};

  const raw=bytesOf(evidence.recording.rawBytes),problems=[];
  const {finalizer,observer,restored}=evidence.recording.persistence,boundary=finalizer.afterFinalizer;
  const validBoundary=Number.isSafeInteger(boundary.startSequence)&&boundary.startSequence>=0&&Number.isFinite(boundary.at);
  if(finalizer.available!==true||finalizer.finalized!==true||!validBoundary)problems.push('invalid native finalizer persistence boundary');
  if(observer.available!==true||observer.populate!==false||observer.completed!==true||observer.succeeded!==true||
    !Number.isSafeInteger(observer.startSequence)||observer.startSequence<=boundary.startSequence||
    !Number.isFinite(observer.startedAt)||observer.startedAt<boundary.at||!Number.isFinite(observer.completedAt)||observer.completedAt<observer.startedAt||
    JSON.stringify(observer.afterFinalizer)!==JSON.stringify(boundary))problems.push('automatic IDBFS sync did not start and complete after the native finalizer');
  if(restored.available!==true||restored.restored!==true||!Array.isArray(restored.calls)||!restored.calls.some(call=>
    call.startSequence===observer.startSequence&&call.startedAt===observer.startedAt&&call.completedAt===observer.completedAt&&call.populate===false&&call.completed===true&&call.succeeded===true))
    problems.push('automatic IDBFS completion is absent from the restored observer calls');

  if(sha256Of(raw)!==evidence.recording.sha256)problems.push('saved Replay SHA-256 does not match its byte sequence');
  if(sha256Of(evidence.persisted.rawBytes)!==evidence.persisted.freshEpochSha256)
    problems.push('fresh-epoch Replay SHA-256 does not match its byte sequence');
  if(sha256Of(evidence.import.rawBytes)!==evidence.import.readSha256)
    problems.push('import readback SHA-256 does not match its byte sequence');
  if(evidence.recording.sha256!==evidence.persisted.freshEpochSha256)problems.push('persisted Replay differs from the saved Replay');
  if(evidence.import.sha256!==evidence.recording.sha256||evidence.import.readSha256!==evidence.recording.sha256)
    problems.push('imported Replay bytes differ from the saved Replay');
  if(evidence.import.beforeLaunch!==true||evidence.import.listed!==true||evidence.import.writeSucceeded!==true)
    problems.push('official Shell import was not complete before launch');
  if(evidence.fresh.epoch===evidence.recording.epoch)problems.push('Replay menu ran in the recording Runtime epoch');
  if(evidence.recording.inputCapacityFrames<1)problems.push('saved Replay has no independently parsed input/timing capacity');

  let parsed;
  try{parsed=parseTh08PracticeReplayMetadata(raw);}
  catch(error){problems.push(String(error));}
  let parameterComparison=null,liveScoreDelta=null;
  if(parsed){
    if(!configMatches(evidence.menu.candidate,parsed.config))problems.push('native Replay candidate differs from independent PRAC parse');
    const initial=evidence.playback.initialSnapshot;
    if(!configMatches(initial.run,parsed.config))problems.push('initial run parameters differ from independent PRAC parse');
    if(!configMatches(evidence.fresh.configured,TH08_PRACTICE_FRESH_CONFIG))problems.push('fresh Runtime did not start from the independent default parameter block');
    const active=evidence.playback.liveSnapshot.state;
    if(active?.active!==true||active?.replay!==true||active?.inGame!==true)problems.push('native replay gameplay did not activate the practice run');
    const initialState=initial.state,cursor=initial.cursor;
    if(!(initial.validMask&1)||!(initial.validMask&2)||initialState?.replay!==true||!(initialState.gameFlags&8))
      problems.push('immutable practice snapshot is not bound to an active native Replay run');
    const expectedRuntimeStage=th08PracticeRuntimeStage(parsed.config);
    if(expectedRuntimeStage<0||initialState?.stage!==expectedRuntimeStage||!(cursor?.stageMask&(1<<expectedRuntimeStage)))
      problems.push('initial Replay cursor belongs to another stage');
    if(cursor?.frame!==0||cursor?.inputCursor!==0||cursor?.timingCursor!==0)
      problems.push('initial Replay cursor was not captured before the first logical playback tick');
    if(!Number.isSafeInteger(cursor?.inputBytes)||cursor.inputBytes<2||cursor.inputCursor>cursor.inputBytes||
      !Number.isSafeInteger(cursor?.timingBytes)||cursor.timingBytes<2||cursor.timingCursor>cursor.timingBytes/2)
      problems.push('initial Replay stream capacity is invalid');
    if(!Number.isSafeInteger(selectionCase.difficulty)||!Number.isSafeInteger(selectionCase.character))
      problems.push('independent TH08 title selection case is unavailable');
    if(evidence.playback.selectedGame?.difficulty!==selectionCase.difficulty||
      evidence.playback.selectedGame?.character!==selectionCase.character)
      problems.push('recorded practice title selection differs from its independent declaration');
    if(initial.state.difficulty!==selectionCase.difficulty||initial.state.character!==selectionCase.character)
      problems.push('native Replay run loaded another title selection');
    parameterComparison=appliedMismatches(initial.applied,parsed.config,selectionCase);
    if(parameterComparison.fieldDiff.length)problems.push(...parameterComparison.fieldDiff.map(item=>`initial.applied.${item.field}`));
    const stream=evidence.playback.stream;
    if(stream.stage!==undefined&&stream.stage!==expectedRuntimeStage)problems.push('live Replay cursor changed its selected stage');
    if(stream.inputBytes!==cursor.inputBytes)problems.push('live Replay input byte range differs from the initial cursor boundary');
    if(stream.frame<=cursor.frame||stream.inputCursor<=cursor.inputCursor||stream.timingCursor<cursor.timingCursor)
      problems.push('native Replay cursor did not advance from its immutable initial boundary');
    if(stream.frame>evidence.recording.inputCapacityFrames||stream.inputCursor>cursor.inputBytes||
      stream.timingCursor>cursor.timingBytes/2)
      problems.push('native Replay cursor exceeded the independently parsed stream capacity');
    liveScoreDelta=Number.isFinite(evidence.playback.liveSnapshot.applied?.score)&&
      Number.isFinite(initial.applied?.score)?evidence.playback.liveSnapshot.applied.score-initial.applied.score:null;
  }
  const stream=evidence.playback.stream;
  if(stream.available!==true||stream.loaded!==true||stream.gameFlags==null||!(stream.gameFlags&8)||
    !Number.isSafeInteger(stream.frame)||stream.frame<1||!Number.isSafeInteger(stream.inputCursor)||stream.inputCursor<1)
    problems.push('native Replay input cursor did not advance');
  if(problems.length)return{schema:'eagler-touhou/th08-practice-replay-roundtrip/1',status:'failed',complete:false,
    missing:[],problems,parsed:parsed?{config:parsed.config,block:parsed.block}:null,
    initialPracticeApplication:evidence.playback.initialSnapshot?{source:evidence.playback.initialSnapshot.source,
      stage:evidence.playback.initialSnapshot.state.stage,gameFlags:evidence.playback.initialSnapshot.state.gameFlags,
      cursor:evidence.playback.initialSnapshot.cursor,run:evidence.playback.initialSnapshot.run,
      applied:evidence.playback.initialSnapshot.applied,parameterComparison}:null,
    livePracticeState:evidence.playback.liveSnapshot?{applied:evidence.playback.liveSnapshot.applied,scoreDelta:liveScoreDelta}:null,
    scope:'recorded th08 practice parameters restored by the native Replay menu in a fresh Runtime epoch'};
  return{schema:'eagler-touhou/th08-practice-replay-roundtrip/1',status:'observed',complete:true,missing:[],problems:[],
    parsed:{config:parsed.config,block:parsed.block},
    evidence:{recordedFrameDelta:evidence.recording.frameAfter-evidence.recording.frameBefore,
      savePath:evidence.recording.path,bytes:raw.byteLength,sha256:evidence.recording.sha256,
      freshRuntimeEpoch:evidence.fresh.epoch,importPath:evidence.import.path,
      initialPracticeApplication:{source:evidence.playback.initialSnapshot.source,
        stage:evidence.playback.initialSnapshot.state.stage,expectedRuntimeStage:th08PracticeRuntimeStage(parsed.config),
        gameFlags:evidence.playback.initialSnapshot.state.gameFlags,
        cursor:evidence.playback.initialSnapshot.cursor,run:evidence.playback.initialSnapshot.run,
        applied:evidence.playback.initialSnapshot.applied,parameterComparison},
      livePracticeState:{applied:evidence.playback.liveSnapshot.applied,scoreDelta:liveScoreDelta},
      playback:{frame:stream.frame,inputCursor:stream.inputCursor,timingCursor:stream.timingCursor,
        inputBytes:stream.inputBytes,timingBytes:stream.timingBytes,gameFlags:stream.gameFlags}},
    scope:'recorded th08 practice parameters restored by the native Replay menu in a fresh Runtime epoch'};
}

/** Serialize a native call to the production finalizer without crossing the Shell's unsafe score-save RPC. */
export function finalizeTh08PracticeReplayRaw({game,variant='normal',slot=1,name='P05TEST'}={}){
  if(game!=='th08'||variant!=='normal')return{available:false,reason:'th08 normal native finalizer required'};
  const root=typeof window==='object'?window:globalThis,runtime=root.__th08Runtime;
  const module=runtime?.Module,core=runtime?.core??module,app=runtime?.app;
  const finalize=core?.save_replay??core?._save_replay,allocate=core?.allocate??core?.malloc??module?._malloc;
  const release=core?.deallocate??core?.free??module?._free,buffer=core?.memory?.buffer??module?.HEAPU8?.buffer;
  if(!runtime||!Number.isInteger(app)||typeof finalize!=='function'||typeof allocate!=='function'||
    typeof release!=='function'||!buffer)return{available:false,reason:'native save_replay export or wasm allocator unavailable'};
  if(!Number.isSafeInteger(slot)||slot<1||slot>15||typeof name!=='string'||!name.trim()||name.length>8||/[\x00-\x1f]/.test(name))
    return{available:false,reason:'invalid native Replay save slot or name'};
  const encoded=new TextEncoder().encode(`${name}\0`),pointer=allocate(encoded.length);
  if(!Number.isInteger(pointer)||pointer<=0||pointer+encoded.length>buffer.byteLength){if(pointer)release(pointer);return{available:false,reason:'native Replay name allocation is outside wasm memory'};}
  try{
    new Uint8Array(buffer,pointer,encoded.length).set(encoded);
    const finalized=finalize(app,slot,pointer)===1;
    const observer=root.__th08PracticeIdbfsObserver;
    const afterFinalizer=observer?.active?{startSequence:observer.started,at:performance.now()}:null;
    return{available:true,finalized,slot,name,afterFinalizer};
  }finally{release(pointer);}
}

/** Instrument the Shell's existing IDBFS callback while preserving its implementation and result. */
export function armTh08PracticeIdbfsObserverRaw(){
  const root=typeof window==='object'?window:globalThis,module=root.__th08Runtime?.Module,fs=module?.FS;
  if(!fs||typeof fs.syncfs!=='function')return{available:false,reason:'Runtime IDBFS syncfs is unavailable'};
  const key='__th08PracticeIdbfsObserver';if(root[key]?.active)return{available:false,reason:'IDBFS observer is already active'};
  const original=fs.syncfs,observer={active:true,started:0,calls:[],waiters:[]};
  fs.syncfs=function(populate,callback){
    const receipt={startSequence:++observer.started,startedAt:performance.now(),populate,completed:false};observer.calls.push(receipt);
    return original.call(this,populate,error=>{
      Object.assign(receipt,{completed:true,completedAt:performance.now(),succeeded:!error});
      for(const waiter of [...observer.waiters])waiter(receipt);
      callback?.(error);
    });
  };
  observer.restore=()=>{if(fs.syncfs!==observer.wrapper)return false;fs.syncfs=original;observer.active=false;return true;};
  observer.wrapper=fs.syncfs;root[key]=observer;
  return{available:true,armed:true,calls:observer.calls.length};
}

export function waitTh08PracticeIdbfsObserverRaw({timeoutMs=30000,afterFinalizer}={}){
  const root=typeof window==='object'?window:globalThis,observer=root.__th08PracticeIdbfsObserver;
  if(!observer?.active)return Promise.resolve({available:false,reason:'IDBFS observer is not armed'});
  if(!Number.isSafeInteger(timeoutMs)||timeoutMs<1||!Number.isSafeInteger(afterFinalizer?.startSequence)||afterFinalizer.startSequence<0||!Number.isFinite(afterFinalizer.at))
    return Promise.resolve({available:false,reason:'invalid IDBFS finalizer observation boundary'});
  const qualifies=receipt=>receipt.startSequence>afterFinalizer.startSequence&&receipt.startedAt>=afterFinalizer.at&&receipt.populate===false&&receipt.completed===true&&receipt.succeeded===true;
  const existing=observer.calls.find(qualifies);
  if(existing)return Promise.resolve({available:true,...existing,calls:observer.calls.length,afterFinalizer});
  return new Promise(resolve=>{
    let done=false;const receive=receipt=>{if(qualifies(receipt))finish({available:true,...receipt,calls:observer.calls.length,afterFinalizer});};
    const finish=value=>{if(done)return;done=true;clearTimeout(timer);observer.waiters=observer.waiters.filter(waiter=>waiter!==receive);resolve(value);};
    const timer=setTimeout(()=>finish({available:true,succeeded:false,reason:'IDBFS sync timed out',calls:observer.calls.length,afterFinalizer}),timeoutMs);
    observer.waiters.push(receive);
  });
}

export function restoreTh08PracticeIdbfsObserverRaw(){
  const root=typeof window==='object'?window:globalThis,observer=root.__th08PracticeIdbfsObserver;
  if(!observer?.active)return{available:false,reason:'IDBFS observer is not active'};
  const restored=observer.restore();
  return{available:true,restored,calls:observer.calls.slice()};
}


// th08's practice menu uses stage 9 for Last Word; practice_runtime_stage()
// resolves its section to the actual 0-8 GameplayScene stage. This table is
// independently used to validate that boundary, not to derive applied values.
const TH08_LAST_WORD_RUNTIME_STAGES=Object.freeze([0,1,2,5,6,7,8,5,8,3,4,3,3,3,3,3,3]);

export function th08PracticeRuntimeStage(config){
  if(!config||!Number.isInteger(config.stage)||!Number.isInteger(config.section))return -1;
  if(config.stage!==9)return config.stage>=0&&config.stage<=8?config.stage:-1;
  const index=config.section-105; // TH08_LW_1 in PracticeSections.hpp
  return index>=0&&index<TH08_LAST_WORD_RUNTIME_STAGES.length?TH08_LAST_WORD_RUNTIME_STAGES[index]:-1;
}

function decodeConfig(words,offset){
  const values=words.slice(offset,offset+26),config={};
  for(const index of [6,21,23])if(values[index]!==0&&values[index]!==1)
    throw new Error(`PracticeSnapshot config boolean at ${offset+index} is invalid`);
  CONFIG_FIELDS.forEach((name,index)=>{config[name]=values[index];});
  for(const name of ['dlg','rankLock','sp1_pts'])config[name]=config[name]!==0;
  return config;
}

/** Decode the th08 BrowserExports v1 snapshot without deriving expected values from it. */
export function decodePracticeSnapshot(raw){
  if(raw?.available!==true||!Array.isArray(raw.words)||raw.words.length!==SNAPSHOT_WORDS)
    throw new Error(`PracticeSnapshot unavailable: ${raw?.reason??'invalid word count'}`);
  const words=raw.words;
  if(words.some(value=>typeof value!=='number'||!Number.isFinite(value)))throw new Error('PracticeSnapshot contains a non-finite word');
  if(words[0]!==1)throw new Error(`Unsupported PracticeSnapshot version ${words[0]}`);
  if(!Number.isSafeInteger(words[18])||words[18]<0)throw new Error('PracticeSnapshot title timer is invalid');
  if(![0,2,3].includes(words[19]))throw new Error('PracticeSnapshot title input-ready extension is invalid');
  const validMask=words[1];
  if(!Number.isInteger(validMask)||(validMask&~0x0f)!==0)throw new Error('PracticeSnapshot has an unknown validity bit');
  const flag=index=>{
    if(words[index]!==0&&words[index]!==1)throw new Error(`PracticeSnapshot flag ${index} is invalid`);
    return words[index]===1;
  };
  const config=(bit,offset)=>{
    if(!(validMask&bit))return null;
    const values=words.slice(offset,offset+26);
    if(values.some(value=>!Number.isSafeInteger(value))||values[22]!==1)
      throw new Error(`PracticeSnapshot config block at ${offset} is malformed`);
    return decodeConfig(words,offset);
  };
  const applied=validMask&8?{
    score:words[100],displayScore:words[101],lives:words[102],bombs:words[103],power:words[104],
    gauge:words[105],gaugeCopy:words[106],graze:words[107],grazeStage:words[108],points:words[109],
    pointsStage:words[110],timeOrbs:words[111],totalTimeOrbs:words[112],pointValue:words[113],clockTime:words[114],
    rank:words[115],rankMinimum:words[116],rankMaximum:words[117],familiarCount:words[118],
    enemyFrame:words[119],controlFrame:words[120],lifeState:words[121],familiarPending:flag(122),
    cheats:words[123],everlastingBgm:flag(124),stage:words[125],difficulty:words[126],character:words[127],
  }:null;
  return{
    schema:'eagler-touhou/th08-practice-snapshot/1',validMask,
    state:{enabled:flag(2),menu:flag(3),accepted:flag(4),active:flag(5),replay:flag(6),assisted:flag(7),
      replayCandidateValid:flag(8),inGame:flag(9),titleScreen:words[10],titlePhase:words[11],titleCursor:words[12],
      stage:words[13],difficulty:words[14],character:words[15],shotType:words[16],gameFlags:words[17],titleTimer:words[18],
      ...(words[19]?{titleInputReady:Boolean(words[19]&1)}:{})},
    configured:config(1,20),run:config(2,46),replayCandidate:config(4,72),applied,
    source:{configured:'PracticeState::configured via normal Runtime options',run:'PracticeState::run after native practice-menu acceptance',
      replayCandidate:'PracticeState::replay_candidate from Replay menu inspection',
      applied:'PracticeRuntime::apply_practice | GameplaySession::numbers/ rank | GameplayScene stage and familiar population'},
    units:{score:'configured tenths of in-game points; applied whole in-game points',lives:'native float count',bombs:'native float count',
      power:'native float power',gauge:'native integer gauge units',graze:'native count',points:'native point count',
      timeOrbs:'native count',pointValue:'native point value units',clockTime:'native half-hour index',rank:'native rank units',
      familiarCount:'native familiar count',enemyFrame:'native gameplay frame',controlFrame:'native control frame'},
  };
}

/** Decode the immutable th08 snapshot captured after practice application and before gameplay jobs. */
export function decodePracticeInitialSnapshot(raw){
  if(raw?.available!==true||!Array.isArray(raw.words)||raw.words.length!==INITIAL_SNAPSHOT_WORDS)
    throw new Error(`PracticeInitialSnapshot unavailable: ${raw?.reason??'invalid word count'}`);
  const words=raw.words;
  if(words.some(value=>typeof value!=='number'||!Number.isFinite(value)))throw new Error('PracticeInitialSnapshot contains a non-finite word');
  if(words[0]!==1)throw new Error(`Unsupported PracticeInitialSnapshot version ${words[0]}`);
  const validMask=words[1];
  if(!Number.isInteger(validMask)||(validMask&~3)!==0)throw new Error('PracticeInitialSnapshot has an unknown validity bit');
  const flag=index=>{
    if(words[index]!==0&&words[index]!==1)throw new Error(`PracticeInitialSnapshot flag ${index} is invalid`);
    return words[index]===1;
  };
  const integerFields=[2,3,4,5,6,7,8,9,10,11,12,13];
  for(const index of integerFields)if(!Number.isSafeInteger(words[index])||words[index]<0)
    throw new Error(`PracticeInitialSnapshot boundary field ${index} is invalid`);
  const run=validMask&1?decodeConfig(words,16):null;
  for(let index=16;index<42;index++)if(!Number.isSafeInteger(words[index]))
    throw new Error(`PracticeInitialSnapshot run config field ${index} is invalid`);
  const applied=validMask&1?{
    score:words[42],displayScore:words[43],lives:words[44],bombs:words[45],power:words[46],
    gauge:words[47],gaugeCopy:words[48],graze:words[49],grazeStage:words[50],points:words[51],
    pointsStage:words[52],timeOrbs:words[53],totalTimeOrbs:words[54],pointValue:words[55],clockTime:words[56],
    rank:words[57],rankMinimum:words[58],rankMaximum:words[59],familiarCount:words[60],enemyFrame:words[61],
    controlFrame:words[62],lifeState:words[63],familiarPending:flag(64),cheats:words[65],
    everlastingBgm:flag(66),stage:words[67],difficulty:words[68],character:words[69],
  }:null;
  if((validMask&2)&&!(validMask&1))throw new Error('PracticeInitialSnapshot replay bit requires an applied practice run');
  if(validMask&1){
    if(words[2]>8||words[4]>4||words[5]>11||run.mode!==1||th08PracticeRuntimeStage(run)!==words[2]||
      words[67]!==words[2]||words[68]!==words[4]||words[69]!==words[5])
      throw new Error('PracticeInitialSnapshot run identity does not match its applied stage');
  }
  if(validMask&2){
    if(!(words[3]&8)||!(words[6]&(1<<words[2])))throw new Error('PracticeInitialSnapshot is not bound to the selected Replay stage');
    if(words[7]!==0||words[8]!==0||words[9]!==0)
      throw new Error('PracticeInitialSnapshot Replay cursor was not captured before the first logical playback tick');
  }
  return{
    schema:'eagler-touhou/th08-practice-initial-snapshot/1',validMask,
    state:{stage:words[2],gameFlags:words[3],difficulty:words[4],character:words[5],replay:Boolean(validMask&2)},
    cursor:{stageMask:words[6],frame:words[7],inputCursor:words[8],timingCursor:words[9],
      inputBytes:words[10],timingBytes:words[11],inputEnd:words[12],timingEnd:words[13]},
    run,applied,
    source:'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs',
    units:{score:'whole in-game points',lives:'native float count',bombs:'native float count',power:'native float power',
      gauge:'native integer gauge units',graze:'native count',points:'native point count',timeOrbs:'native count',
      pointValue:'native point value units',clockTime:'native half-hour index',rank:'native rank units',
      familiarCount:'native familiar count',enemyFrame:'native gameplay frame',controlFrame:'native control frame',
      replayCursor:'ReplayStream successful-play frame and input/timing byte cursors'},
  };
}

/** Decode the immutable th08 snapshot captured after practice application and before gameplay jobs. */
export function decodeGameplayInitialSnapshot(raw){
  if(raw?.available!==true||!Array.isArray(raw.words)||raw.words.length!==INITIAL_SNAPSHOT_WORDS)
    throw new Error(`GameplayInitialSnapshot unavailable: ${raw?.reason??'invalid word count'}`);
  const words=raw.words;
  if(words.some(value=>typeof value!=='number'||!Number.isFinite(value)))throw new Error('GameplayInitialSnapshot contains a non-finite word');
  if(words[0]!==1)throw new Error(`Unsupported GameplayInitialSnapshot version ${words[0]}`);
  const validMask=words[1];
  if(!Number.isInteger(validMask)||(validMask&~7)!==0)throw new Error('GameplayInitialSnapshot has an unknown validity bit');
  const flag=index=>{
    if(words[index]!==0&&words[index]!==1)throw new Error(`GameplayInitialSnapshot flag ${index} is invalid`);
    return words[index]===1;
  };
  const integerFields=[2,3,4,5,6,7,8,9,10,11,12,13];
  for(const index of integerFields)if(!Number.isSafeInteger(words[index])||words[index]<0)
    throw new Error(`GameplayInitialSnapshot boundary field ${index} is invalid`);
  const run=validMask&4?decodeConfig(words,16):null;
  for(let index=16;index<42;index++)if(!Number.isSafeInteger(words[index]))
    throw new Error(`GameplayInitialSnapshot run config field ${index} is invalid`);
  const applied=validMask&1?{
    score:words[42],displayScore:words[43],lives:words[44],bombs:words[45],power:words[46],
    gauge:words[47],gaugeCopy:words[48],graze:words[49],grazeStage:words[50],points:words[51],
    pointsStage:words[52],timeOrbs:words[53],totalTimeOrbs:words[54],pointValue:words[55],clockTime:words[56],
    rank:words[57],rankMinimum:words[58],rankMaximum:words[59],familiarCount:words[60],enemyFrame:words[61],
    controlFrame:words[62],lifeState:words[63],familiarPending:flag(64),cheats:words[65],
    everlastingBgm:flag(66),stage:words[67],difficulty:words[68],character:words[69],
  }:null;
  if((validMask&6)&&!(validMask&1))throw new Error('GameplayInitialSnapshot replay bit requires an applied practice run');
  if(validMask&1){
    if(words[2]>8||words[4]>4||words[5]>11||words[67]!==words[2]||words[68]!==words[4]||words[69]!==words[5])throw new Error('GameplayInitialSnapshot applied identity differs from boundary');
    if(!(validMask&4)&&words.slice(16,42).some(value=>value!==0))throw new Error('GameplayInitialSnapshot ordinary run payload is not empty');
  }
  if(validMask&4){
    if(words[2]>8||words[4]>4||words[5]>11||run.mode!==1||th08PracticeRuntimeStage(run)!==words[2]||
      words[67]!==words[2]||words[68]!==words[4]||words[69]!==words[5])
      throw new Error('GameplayInitialSnapshot run identity does not match its applied stage');
  }
  if(validMask&2){
    if(!(words[3]&8)||!(words[6]&(1<<words[2])))throw new Error('GameplayInitialSnapshot is not bound to the selected Replay stage');
    if(words[7]!==0||words[8]!==0||words[9]!==0)
      throw new Error('GameplayInitialSnapshot Replay cursor was not captured before the first logical playback tick');
  }
  return{
    schema:'eagler-touhou/th08-gameplay-initial-snapshot/1',validMask,
    state:{stage:words[2],gameFlags:words[3],difficulty:words[4],character:words[5],replay:Boolean(validMask&2),customPractice:Boolean(validMask&4)},
    cursor:{stageMask:words[6],frame:words[7],inputCursor:words[8],timingCursor:words[9],
      inputBytes:words[10],timingBytes:words[11],inputEnd:words[12],timingEnd:words[13]},
    run,applied,
    source:'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs',
    units:{score:'whole in-game points',lives:'native float count',bombs:'native float count',power:'native float power',
      gauge:'native integer gauge units',graze:'native count',points:'native point count',timeOrbs:'native count',
      pointValue:'native point value units',clockTime:'native half-hour index',rank:'native rank units',
      familiarCount:'native familiar count',enemyFrame:'native gameplay frame',controlFrame:'native control frame',
      replayCursor:'ReplayStream successful-play frame and input/timing byte cursors'},
  };
}

export const TH08_PRACTICE_PARAMETER_CASE=Object.freeze({
  mode:1,stage:0,warp:0,section:0,phase:0,frame:0,dlg:false,score:1234560,life:4,bomb:3,power:96,
  gauge:10000,graze:23,point:0,point_total:234,point_stage:345,time:321,value:43210,night:4,familiar:17,
  rank:10,rankLock:true,sp1_pts:false,bsX:48,bsY:96,
});

export const TH08_PRACTICE_SELECTION_CASE=Object.freeze({difficulty:1,character:2,
  source:'GameConfiguration.hpp default difficulty=1 | TitleMenus.cpp PracticeStart copies config.difficulty | CharacterSelect stores the selected cursor'});

export function th08PracticeFeatureInventory(snapshot,{pointerTested=false,exercisedKeys=[]}={}){
  if(snapshot?.schema!=='eagler-touhou/th08-practice-snapshot/1')
    return{status:'incomplete',items:[],reason:'Native PracticeSnapshot is unavailable'};
  const exercised=new Set();
  if(snapshot.state.menu||snapshot.state.active)exercised.add('title-practice-route');
  if(snapshot.state.active&&(snapshot.validMask&8))exercised.add('custom-parameter-consumption');
  return{status:'observed',complete:false,items:[
    {id:'title-practice-route',label:'Title practice selection and native option menu',available:snapshot.state.enabled,
      exercised:exercised.has('title-practice-route'),source:'TitleScene::attach | ThpracUi::draw_practice'},
    {id:'custom-parameter-consumption',label:'Stage, warp, resources, score, gauge, clock, rank and familiar settings',available:snapshot.state.enabled,
      exercised:exercised.has('custom-parameter-consumption'),source:'PracticeRuntime::apply_practice'},
    {id:'replay-candidate',label:'Practice parameters inspected and restored by Replay menu',available:snapshot.state.enabled,
      exercised:snapshot.state.replayCandidateValid,source:'practice_replay_menu_check | practice_replay_menu_activate'},
    ...[
      ['F1','Invincibility'],['F2','Infinite lives'],['F3','Infinite bombs'],['F4','Infinite power'],
      ['F5','Time lock'],['F6','Auto bomb'],['F7','Everlasting BGM'],['U','Enemy invincibility'],
    ].map(([key,label])=>({id:`trainer-${key.toLowerCase()}`,label:`${key} ${label}`,available:snapshot.state.enabled,
      exercised:exercisedKeys.includes(key),source:'ThpracUi::update_input | ThpracUi::draw_overlay'})),
    {id:'tracker',label:'Tab gameplay tracker',available:snapshot.state.enabled,exercised:false,
      source:'ThpracUi::update_input | ThpracUi::draw_overlay'},
    {id:'advanced-options',label:'F12 advanced options and gameplay controls',available:snapshot.state.enabled,exercised:false,
      source:'ThpracUi::update_input | draw_advanced'},
    {id:'native-pointer',label:'Native pointer selection in the practice overlay',available:snapshot.state.enabled,
      exercised:pointerTested,source:'SDL mouse event | ThpracUi::mouse'},
  ]};
}

export function assessTh08PracticeConsumption(snapshot,requested=TH08_PRACTICE_PARAMETER_CASE,
  selectionCase=TH08_PRACTICE_SELECTION_CASE,observed={}){
  const missing=[],mismatches=[];
  if(snapshot?.schema!=='eagler-touhou/th08-practice-snapshot/1'||!snapshot.configured||!snapshot.run||!snapshot.applied)missing.push('live configured/run/owner observation');
  if(observed.initialRaw?.available!==true)missing.push('immutable gameplay initialization raw');
  const identityValid=id=>id&&Number.isSafeInteger(id.epoch)&&Number.isSafeInteger(id.app)&&Number.isFinite(id.documentTimeOrigin)&&typeof id.url==='string'&&id.url.length>0;
  if(!identityValid(observed.identityBefore)||!identityValid(observed.identityAfter))missing.push('document/epoch/app identities');
  if(missing.length)return{status:'incomplete',complete:false,missing,mismatches,initialRaw:observed.initialRaw??null};
  let initial;try{initial=decodeGameplayInitialSnapshot(observed.initialRaw);}catch(error){return{status:'failed',complete:false,missing:[],mismatches:[{field:'initialRaw',reason:error.message}],initialRaw:observed.initialRaw};}
  if(!(initial.validMask&1))return{status:'incomplete',complete:false,missing:['available immutable initialization boundary'],mismatches:[]};
  const check=(ok,field,expected,actual)=>{if(!ok)mismatches.push({field,expected,actual});};
  check(JSON.stringify(observed.identityBefore)===JSON.stringify(observed.identityAfter),'identity','same document/epoch/app',observed.identityAfter);
  for(const field of CONFIG_FIELDS.filter(name=>name!=='sentinel'))for(const owner of ['configured','run']){
    const expected=owner==='run'&&field==='warp'?0:requested[field];
    check(snapshot[owner][field]===expected,`${owner}.${field}`,expected,snapshot[owner][field]);
  }
  check(snapshot.state.active&&snapshot.state.inGame&&!snapshot.state.replay&&(snapshot.state.gameFlags&9)===1,'state.owner','active live Practice',snapshot.state);
  check(initial.state.customPractice&&!initial.state.replay&&(initial.state.gameFlags&9)===1,'initial.owner','custom Practice initialization',initial.state);
  check(JSON.stringify(initial.run)===JSON.stringify(snapshot.run),'initial.run',snapshot.run,initial.run);
  for(const field of ['stage','difficulty','character'])check(initial.state[field]===snapshot.applied[field],`initial/live.${field}`,initial.state[field],snapshot.applied[field]);
  check(snapshot.applied.enemyFrame>initial.applied.enemyFrame,'live.enemyFrame','advance after initialization',snapshot.applied.enemyFrame);
  mismatches.push(...appliedMismatches(initial.applied,requested,selectionCase).fieldDiff.map(item=>({...item,field:`applied.${item.field}`})));
  return{status:mismatches.length?'failed':'observed',complete:false,missing,mismatches,
    configMatches:mismatches.every(item=>!item.field.startsWith('configured.')&&!item.field.startsWith('run.')),
    requested,configured:snapshot.configured,run:snapshot.run,applied:initial.applied,liveApplied:snapshot.applied,
    initialRaw:observed.initialRaw,initialSnapshot:initial,identityBefore:observed.identityBefore,identityAfter:observed.identityAfter};
}

/** Native title-menu route and configured-value consumption, expressed through shared driver operations. */
export async function enterTh08PracticeMenu({read,waitUntil,press,waitForGameplay,waitForPracticeInput,readInitial,readIdentity,parameterCase=TH08_PRACTICE_PARAMETER_CASE,
  selectionCase=TH08_PRACTICE_SELECTION_CASE}={}){
  if(typeof read!=='function'||typeof waitUntil!=='function'||typeof press!=='function'||typeof waitForGameplay!=='function'||typeof waitForPracticeInput!=='function')
    throw new TypeError('TH08 practice menu adapter needs native read, synchronous-predicate wait, input-ready wait, key and gameplay operations');
  const identityBefore=typeof readIdentity==='function'?await readIdentity():null;
  let practice=await waitUntil('TH08 start menu ready',state=>state.state.titleScreen===0&&state.state.titlePhase===1&&state.state.titleInputReady===true);
  for(let count=0;count<9&&!(practice.state.titleScreen===0&&practice.state.titlePhase===1&&practice.state.titleCursor===3);count++){
    await press('ArrowDown');practice=await read();
  }
  if(practice.state.titleScreen!==0||practice.state.titlePhase!==1||practice.state.titleCursor!==3)
    throw new Error('Native title menu did not select Practice Start');
  await press('KeyZ');
  practice=await waitUntil('TH08 practice difficulty ready',state=>state.state.titleScreen===8&&state.state.titlePhase===1);
  if(practice.state.titleCursor!==selectionCase.difficulty)
    throw new Error(`Native difficulty selection ${practice.state.titleCursor} differs from declared ${selectionCase.difficulty}`);
  await press('KeyZ');
  await waitUntil('TH08 practice character ready',state=>state.state.titleScreen===9&&state.state.titlePhase===1);
  practice=await read();
  for(let count=0;count<12&&practice.state.titleCursor!==selectionCase.character;count++){
    await press('ArrowRight');practice=await read();
  }
  if(practice.state.titleCursor!==selectionCase.character)throw new Error('Native character menu did not select the declared profile');
  await press('KeyZ');
  practice=await waitUntil('TH08 native practice options ready',state=>state.state.menu&&state.state.titleScreen===11,15000);
  if(!practice.state.menu||practice.state.titleScreen!==11)throw new Error('Native thprac stage-select menu did not open');
  if(!practice.state.enabled)throw new Error('Practice feature was not enabled in the native owner');
  const menuInput=await waitForPracticeInput();await press('KeyZ');
  practice=await waitUntil('TH08 custom practice consumption',state=>state.state.active&&state.applied,20000);
  if(!practice.state.active||!practice.applied)throw new Error('Native custom practice was not consumed by GameplayScene');
  const snapshot=await waitForGameplay();practice=await read();
  const initialRaw=typeof readInitial==='function'?await readInitial():null;
  const identityAfter=typeof readIdentity==='function'?await readIdentity():null;
  return{initialRaw,identityBefore,identityAfter,menuInput,titleSelection:{screen:practice.state.titleScreen,cursor:practice.state.titleCursor},snapshot,practice,
    featureInventory:th08PracticeFeatureInventory(practice)};
}

/** Exercise the title-owned F7 setting through keyboard and native canvas-pointer input. */
export async function exerciseTh08PracticeControls({read,press,clickPointer,waitForChange}={}){
  if(typeof read!=='function'||typeof press!=='function'||typeof clickPointer!=='function'||typeof waitForChange!=='function')
    throw new TypeError('TH08 practice controls need native read, key, pointer and wait operations');
  const before=await read();
  if(before.applied.everlastingBgm!==false)throw new Error('F7 started in an unexpected state');
  await press('F7');
  let afterKeyboard=await read();
  if(afterKeyboard.applied.everlastingBgm!==true)throw new Error('Native F7 did not toggle everlasting BGM on');
  const pointer=await clickPointer(50,141);
  const afterPointer=await waitForChange(()=>read(),native=>native.applied.everlastingBgm===false,3000);
  if(afterPointer.applied.everlastingBgm!==false)throw new Error('Native canvas pointer did not toggle the F7 row back off');
  return{result:{status:'observed',complete:false,keyboard:{key:'F7',before:false,after:true},
    pointer:{target:{logicalX:50,logicalY:141},mapping:pointer,before:true,after:false},menuOpen:true},
    snapshot:afterPointer,featureInventory:th08PracticeFeatureInventory(afterPointer,{pointerTested:true,exercisedKeys:['F7']})};
}

export function requireTh08PracticeLifecycleComplete(result){
  if(result?.status!=='observed'||result.complete!==true)throw Error(`Same Runtime Practice lifecycle evidence is not complete: ${JSON.stringify({
    status:result?.status,missing:result?.missing,problems:result?.problems})}`);
  return result;
}

/** Replay input publication and a completed world update are separate native boundaries. */
export function th08PracticeReplayPlaybackReady(live,stream,initial,{requireWorldProgress=true}={}){
  const inputConsumed=live?.state?.inGame===true&&live.state.active===true&&live.state.replay===true&&live.applied&&
    stream?.loaded===true&&stream.frame>0&&stream.inputCursor>0&&stream.ended!==true;
  if(!inputConsumed)return false;
  return !requireWorldProgress||Number.isSafeInteger(initial?.applied?.enemyFrame)&&
    Number.isSafeInteger(live.applied.enemyFrame)&&live.applied.enemyFrame>initial.applied.enemyFrame;
}

/** Same GameApplication lifecycle. Configured values persist; only documented consumers reset. */
export function assessTh08PracticeLifecycle(evidence={}){
  const names=['before','retried','practiceTitle','replayMenu','replay','replayTitle','ordinary','fresh'],missing=names.filter(name=>!evidence[name]);
  if(missing.length)return{status:'incomplete',complete:false,missing,problems:[]};
  const problems=[],check=(value,message)=>{if(!value)problems.push(message);},same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
  const {before,retried,practiceTitle,replayMenu,replay,replayTitle,ordinary,fresh}=evidence;
  for(const name of names)check(same(evidence[name].configured,before.configured),`${name}: configured parameters changed`);
  check(before.state.active&&!before.state.replay,'source is not live custom Practice');
  check(evidence.retryBeforeFrame===before.applied?.enemyFrame&&evidence.retryBeforeFrame>=360&&retried.applied?.enemyFrame<evidence.retryBeforeFrame,'retry did not rebuild the actual world frame');
  check(retried.state.active&&!retried.state.replay&&same(retried.run,before.run),'retry lost the accepted Practice owner');

  for(const [name,snapshot] of [['practiceTitle',practiceTitle],['replayTitle',replayTitle]])check(snapshot.state.menu===false&&snapshot.state.accepted===false,`${name}: title retained a menu acceptance`);
  check(replayMenu.run.mode===0&&replayMenu.state.replayCandidateValid===false,'Replay menu did not reset live run and candidate validity');
  check(replay.state.active&&replay.state.replay&&same(replay.run,evidence.replayConfig),'Replay did not restore its actual saved PRAC owner');

  check(evidence.replayStream?.loaded===true&&evidence.replayStream.frame>0&&evidence.replayStream.inputCursor>0,'saved Replay input did not advance');
  check(evidence.activeReplayExit===true,'Replay returned naturally instead of a witnessed active pause/quit');
  check(ordinary.state.inGame&&!ordinary.state.active&&!ordinary.state.replay&&(ordinary.state.gameFlags&9)===0,'ordinary Start retained Practice or Replay ownership');
  const expected={score:0,displayScore:0,power:0,gauge:0,graze:0,grazeStage:0,points:0,pointsStage:0,timeOrbs:0,totalTimeOrbs:0,clockTime:0,cheats:0};
  check(ordinary.state.stage===0,'ordinary Start selected another native stage');
  const initialNames=['before','retried','replay','ordinary','fresh'];
  for(const name of initialNames){
    const raw=evidence.initials?.[name];
    if(raw?.available!==true){missing.push(`${name}: immutable gameplay initial snapshot`);continue;}
    let initial;try{initial=decodeGameplayInitialSnapshot(raw);}catch(error){problems.push(`${name}: ${error.message}`);continue;}
    if(!(initial.validMask&1)){missing.push(`${name}: available initialization boundary`);continue;}
    const live=evidence[name];
    check(initial.state.stage===live.state.stage&&initial.state.replay===live.state.replay,`${name}: immutable/live stage or Replay identity differs`);
    if(name==='ordinary')check((initial.state.gameFlags&9)===0&&initial.state.stage===0,'ordinary initial retained Practice/Replay flags or another stage');
    else if(name!=='replay')check((initial.state.gameFlags&9)===1&&(live.state.gameFlags&9)===1,`${name}: initial/live native Practice flag missing or Replay flag retained`);
    check(initial.state.customPractice===(name!=='ordinary'),`${name}: initial custom Practice owner differs`);
    if(live.applied)for(const field of ['difficulty','character'])check(initial.state[field]===live.applied[field],`${name}: immutable/live ${field} differs`);
    const frame=name==='ordinary'?evidence.ordinaryProgress?.counters?.enemySceneFrame:live.applied?.enemyFrame;
    if(!Number.isSafeInteger(frame))missing.push(`${name}: live native world frame`);
    else check(frame>initial.applied.enemyFrame,`${name}: live world did not advance after initialization`);
    if(name==='ordinary')for(const [field,value] of Object.entries(expected))check(initial.applied[field]===value,`ordinary Start initial ${field} did not follow GameplayStart initialization`);
    else{
      const config=name==='replay'?evidence.replayConfig:before.configured;
      check(same(initial.run,config),`${name}: immutable accepted config differs`);
      check(appliedMismatches(initial.applied,config,TH08_PRACTICE_SELECTION_CASE).fieldDiff.length===0,`${name}: immutable resource initialization differs`);
    }
  }
  const identities=evidence.identities;
  if(!identities||names.some(name=>{const id=identities[name];return !id||!Number.isSafeInteger(id.epoch)||!Number.isSafeInteger(id.app)||!Number.isFinite(id.documentTimeOrigin)||typeof id.url!=='string'||!id.url;}))missing.push('same document/epoch/app boundary identities');
  else for(const name of names)check(same(identities[name],identities.before),`${name}: document/epoch/app identity changed`);
  check(fresh.state.active&&!fresh.state.replay&&same(fresh.run,before.configured),'fresh Practice did not accept the preserved configuration');

  return{status:problems.length?'failed':missing.length?'incomplete':'observed',complete:problems.length===0&&missing.length===0,missing,problems};
}

export async function runTh08PracticeLifecycle({read,waitUntil,readInitial,readIdentity,readRuntime,waitRuntime,press,waitForPracticeInput,restartFromPause,enterOrdinary,enterReplay,
  replayConfig,record={},stage,save}={}){
  const evidence=record.evidence={replayConfig,initials:{},identities:{}};record.status='running';
  const capture=async(name)=>{evidence[name]=await read();evidence.identities[name]=await readIdentity();if(['before','retried','ordinary','fresh'].includes(name))evidence.initials[name]=await readInitial();await save();return evidence[name];};
  const titleReady=s=>!s.state.inGame&&s.state.titleScreen===0&&s.state.titlePhase===1;
  const toTitle=async({replay=false}={})=>{
    const before=await read();
    if(replay&&(!before.state.replay||!before.state.inGame))throw Error('Saved Replay ended before the active exit action');
    await press('Escape');await waitRuntime('native pause before title exit',s=>s.pause?.active===true,5000);
    if(replay){const held=await read();if(!held.state.replay||!held.state.inGame)throw Error('Saved Replay ended before pause was observed');evidence.activeReplayExit=true;}
    // UiMenus state 1 resume -> state 2 quit -> state 6 no -> state 5 yes.
    await press('ArrowDown');await press('KeyZ');await press('ArrowUp');await press('KeyZ');
    let current=await waitUntil('native title after quit',s=>!s.state.inGame,15000);
    for(let count=0;count<8&&!titleReady(current);count++){await press('KeyX');current=await read();}
    if(!titleReady(current))throw Error('Native quit did not return to the title menu');
    return current;
  };
  try{
    await capture('before');evidence.retryBeforeFrame=evidence.before.applied.enemyFrame;
    await stage('practice-same-runtime-pause-retry',{},async()=>{
      await press('Escape');await waitRuntime('native Practice pause',s=>s.pause?.active===true,5000);
      await restartFromPause();
      await waitUntil('Practice world rebuilt after retry',s=>s.state.inGame&&s.state.active&&!s.state.replay&&s.applied?.enemyFrame<evidence.retryBeforeFrame,15000);
      await waitRuntime('native Practice ready after retry',s=>s.pause?.active===false&&s.worldProgress?.comparable===true,15000);
      await capture('retried');
    });
    await stage('practice-same-runtime-exit-title',{},async()=>{await toTitle();await capture('practiceTitle');});
    const replay=await stage('practice-same-runtime-saved-replay',{},()=>enterReplay());
    evidence.replayMenu=replay.initial;evidence.replay=replay.liveSnapshot;evidence.replayStream=replay.stream;evidence.initials.replay=await readInitial();evidence.identities.replayMenu=await readIdentity();evidence.identities.replay=await readIdentity();await save();
    await stage('practice-same-runtime-replay-exit',{},async()=>{await toTitle({replay:true});await capture('replayTitle');});
    await stage('practice-same-runtime-ordinary-start',{},async()=>{
      let current=await read();for(let count=0;count<12&&current.state.titleCursor!==0;count++){await press('ArrowDown');current=await read();}
      if(current.state.titleCursor!==0)throw Error('Native title did not select ordinary Start');
      await enterOrdinary();await waitRuntime('ordinary Start native world ready',s=>s.pause?.active===false&&s.worldProgress?.comparable===true,15000);
      await capture('ordinary');evidence.ordinaryProgress=(await readRuntime()).worldProgress;await save();
    });
    await stage('practice-same-runtime-fresh-practice',{},async()=>{
      await toTitle();await enterTh08PracticeMenu({read,waitUntil,press,waitForPracticeInput,readInitial,readIdentity,waitForGameplay:()=>waitRuntime('fresh Practice gameplay',s=>s.pause?.active===false&&s.worldProgress?.comparable===true,15000)});
      await capture('fresh');
    });
    Object.assign(record,assessTh08PracticeLifecycle(evidence));await save();
    requireTh08PracticeLifecycleComplete(record);
    return record;
  }catch(error){record.status='failed';record.complete=false;record.error=String(error.stack??error);await save();throw error;}
}

export const TH08_PRACTICE_ADAPTER=Object.freeze({
  trainer:TH08_PRACTICE_TRAINER,
  options:TH08_PRACTICE_OPTIONS,
  bossEffects:{snapshot:TH08_PRACTICE_TRAINER.snapshot,stage:0,slot:0,protectedMask:81,vulnerableMask:17,unlockedMask:65,
    lockTimer:{snapshot:TH08_PRACTICE_OPTIONS.snapshot,toggle:17,menuKey:'F12',fields:{option:47,frame:17,consumer:60,generation:76,visible:77,frames:78,length:79,text:80,vertices:88}},
    keys:{playerGuard:'F1',enemyGuard:'KeyU',timeLock:'F5',fire:'KeyZ'},parameters:{stage:0,warp:3,section:3,frame:0},
    source:'PracticePatches::THStageWarp first Boss | PracticeRuntime::update_practice | EnemyDamage::damage_enemy | EnemyPopulation::check_timeout'},
  uiSnapshot:{export:'practice_ui_snapshot_v1',wordCount:128,wordType:'u32',app:true,decode:decodePracticeUiSnapshot,keyboard:'vk256/1'},
  applicationSnapshot:{decode:decodePracticeApplicationSnapshot,source:'GameplayScene::capture_gameplay_initial_snapshot | actual ECL timeline state before bind_jobs'},
  controlKeys:[...Array.from({length:6},(_,index)=>({key:'F'+(index+1),kind:'cheat-mask',mask:1<<index})),
    {key:'F7',kind:'everlasting-bgm'},{key:'KeyU',kind:'cheat-mask',mask:64}],
  localeReview:{locales:['ja-JP','en-US','zh-CN'],font:'/shared/unifont.otf',
    source:'ports/th08/web/cpp/sdl/ThpracUi.cpp | initialize | locale selection and io.FontDefault'},
  lifecycle:{minimumRecordingFrames:360,run:runTh08PracticeLifecycle,assess:assessTh08PracticeLifecycle},
  schema:'eagler-touhou/practice-title/1',
  source:'ports/th08/web/cpp/game/TitleMenus.cpp | PracticeStart | PracticeRuntime::apply_practice',
  parameterCase:TH08_PRACTICE_PARAMETER_CASE,
  parameterMatrix:th08PracticeParameterCases(TH08_PRACTICE_PARAMETER_CASE),
  programSnapshot:{export:'practice_program_snapshot_v1',source:'GameplayScene::capture_gameplay_initial_snapshot | frozen actual ECL bytes'},
  assessParameterMatrix:rows=>assertTh08PracticeParameters(rows,{cases:th08PracticeParameterCases(TH08_PRACTICE_PARAMETER_CASE),decodeSnapshot:decodePracticeSnapshot,
    assessConsumption:(native,params,observation)=>assessTh08PracticeConsumption(native,params,TH08_PRACTICE_SELECTION_CASE,observation)}),
  selectionCase:TH08_PRACTICE_SELECTION_CASE,
  snapshot:{export:'practice_snapshot_v1',wordCount:128,wordType:'f64',app:true,decode:decodePracticeSnapshot},
  gameplayInitialSnapshot:{export:'gameplay_initial_snapshot_v1',wordCount:80,wordType:'f64',app:true,decode:decodeGameplayInitialSnapshot},
  initialSnapshot:{export:'practice_initial_snapshot_v1',wordCount:80,wordType:'f64',app:true,decode:decodePracticeInitialSnapshot},
  enterMenu:enterTh08PracticeMenu,
  exerciseControls:exerciseTh08PracticeControls,
  assessConsumption:assessTh08PracticeConsumption,
  featureInventory:th08PracticeFeatureInventory,
  replayRoundTrip:{supported:true,format:'th08-prac-v1',savePath:'replay/th8_01.rpy',
    selectionCase:TH08_PRACTICE_SELECTION_CASE,
    finalizer:finalizeTh08PracticeReplayRaw,finalizerArgs:{slot:1,name:'P05TEST'},
    sync:{arm:armTh08PracticeIdbfsObserverRaw,wait:waitTh08PracticeIdbfsObserverRaw,restore:restoreTh08PracticeIdbfsObserverRaw},
    parse:parseTh08PracticeReplayMetadata,inputExpectation:deriveTh08ReplayInputExpectation,
    importPath:makeTh08PracticeReplayImportPath,isUserImportPath:path=>/^replay\/th8_ud[a-z0-9]{4}\.rpy$/.test(path),
    recordingFrame:snapshot=>snapshot?.applied?.enemyFrame,recordingStage:(snapshot,parsed)=>th08PracticeRuntimeStage(parsed?.config),
    inputCapacityFrames:expectation=>expectation?.capacity?.maxSuccessfulFrames,
    freshConfig:TH08_PRACTICE_FRESH_CONFIG,enterMenu:enterTh08PracticeReplayMenu,
    assess:assessTh08PracticeReplayRoundTrip},
});


export function makeTh08PracticeReplayImportPath(suffix){
  if(typeof suffix!=='string'||!/^[a-z0-9]{4}$/.test(suffix))throw new TypeError('TH08 user Replay suffix must contain four lowercase alphanumeric characters');
  return `replay/th8_ud${suffix}.rpy`;
}

/** Native menu navigation only; persistence, file transfer, epochs and evidence pairing stay in the shared driver. */
// TitleReplays.cpp case 1 discards confirmation edges until stateTimer reaches 10.
export function th08ReplayListConfirmReady(snapshot){
  return snapshot?.state?.titleScreen===7&&snapshot.state.titlePhase===1&&Number.isSafeInteger(snapshot.state.titleTimer)&&snapshot.state.titleTimer>=10;
}

export async function enterTh08PracticeReplayMenu({page,frame,target,runtime,practice,readRuntimeObservation,
  runtimeObservationRequest,stage,press,importPath,caseRecord,save,sourcePractice,epoch,systemSlot=false,captureInitial=true}={}){
  if(!page||!frame||typeof stage!=='function'||typeof save!=='function'||typeof readRuntimeObservation!=='function'||typeof press!=='function')
    throw new TypeError('TH08 Replay menu adapter needs the shared page, native frame and evidence operations');
  const read=async()=>decodePracticeSnapshot(await frame.evaluate(readPracticeSnapshotRaw,
    practiceSnapshotRequest({runtime,contract:practice})));
  const wait=async(label,predicate,timeoutMs=30000)=>{
    const started=Date.now();let state;
    do{state=await read();if(predicate(state))return state;await page.waitForTimeout(75);}
    while(Date.now()-started<timeoutMs);
    throw new Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);
  };
  const initial=await stage(systemSlot?'practice-same-runtime-title-before-replay':'practice-replay-default-state-before-menu',{epoch},()=>wait(
    'TH08 fresh Runtime title menu',state=>state.state.titleScreen===0&&state.state.titlePhase===1&&state.state.titleInputReady===true));
  if(!systemSlot)caseRecord.p05FreshEpoch={epoch,configured:initial.configured,
    title:{screen:initial.state.titleScreen,phase:initial.state.titlePhase,cursor:initial.state.titleCursor},
    importedBeforeLaunch:true,customParametersSupplied:false};
  let title=initial;
  for(let count=0;count<12&&title.state.titleCursor!==4;count++){await press('ArrowDown');title=await read();}
  if(title.state.titleScreen!==0||title.state.titlePhase!==1||title.state.titleCursor!==4)
    throw new Error('Native title menu did not select Start Replay');
  await stage('practice-replay-native-title-selection',{epoch,menuItem:'Start Replay'},async()=>{
    await press('KeyZ');return wait('TH08 native Replay menu',state=>state.state.titleScreen===7&&state.state.titlePhase===1);
  });
  let replayMenu=await read();const menuReset=replayMenu;
  if(replayMenu.state.titleCursor!==0)throw new Error('Native Replay list did not begin at saved system slot');
  if(!systemSlot)await press('ArrowDown');
  replayMenu=await wait('TH08 saved Replay selection',state=>state.state.titleScreen===7&&
    th08ReplayListConfirmReady(state)&&state.state.titleCursor===(systemSlot?0:1));
  caseRecord.p05ReplaySelection={systemSlot,importPath,snapshot:replayMenu,guardSource:'TitleReplays.cpp OnUpdateReplayMenu case 1 stateTimer >= 10'};await save();
  const candidate=await stage('practice-replay-native-menu-candidate',{epoch,importPath},async()=>{
    await press('KeyZ');
    const opened=await wait('TH08 native Replay metadata inspection',state=>state.state.titleScreen===7&&
      state.state.titlePhase===2&&state.state.replayCandidateValid);
    return{screen:opened.state.titleScreen,phase:opened.state.titlePhase,cursor:opened.state.titleCursor,
      valid:opened.state.replayCandidateValid,config:opened.replayCandidate};
  });
  await press('KeyZ');
  const stageSelected=await wait('TH08 native Replay stage selection',state=>state.state.titleScreen===7&&state.state.titlePhase===3);
  await press('KeyZ');
  let liveSnapshot=null,observation=null,initialRaw=null,initialSnapshot=null,playbackReady=false;
  const started=Date.now();
  do{
    liveSnapshot=await read();
    observation=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
    if(captureInitial&&!initialSnapshot&&th08PracticeReplayPlaybackReady(liveSnapshot,observation.replay?.stream,null,{requireWorldProgress:false})){
      initialRaw=await stage('practice-replay-immutable-initial-application-snapshot',
        {epoch,source:'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs'},()=>
          frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime,contract:practice,kind:'initial'})));
      initialSnapshot=decodePracticeInitialSnapshot(initialRaw);
    }
    playbackReady=th08PracticeReplayPlaybackReady(liveSnapshot,observation.replay?.stream,initialSnapshot,{requireWorldProgress:captureInitial});
    if(playbackReady)break;
    await page.waitForTimeout(75);
  }while(Date.now()-started<30000);
  caseRecord.p05Native={candidate,stage:{screen:stageSelected.state.titleScreen,phase:stageSelected.state.titlePhase,
    cursor:stageSelected.state.titleCursor},initialRaw,initial:initialSnapshot,live:liveSnapshot,playback:observation.replay?.stream??null,
    selectedGame:{difficulty:sourcePractice.applied.difficulty,character:sourcePractice.applied.character}};
  await save();
  if(!playbackReady)throw new Error(`Native saved Replay did not advance its input and world after initialization: ${JSON.stringify({
    frame:observation?.replay?.stream?.frame,inputCursor:observation?.replay?.stream?.inputCursor,
    initialWorldFrame:initialSnapshot?.applied?.enemyFrame,liveWorldFrame:liveSnapshot?.applied?.enemyFrame})}`);
  return{candidate:candidate.config,initial:systemSlot?menuReset:initial,initialSnapshot,liveSnapshot,stream:observation.replay?.stream,
    selectedGame:{difficulty:sourcePractice.applied.difficulty,character:sourcePractice.applied.character}};
}
