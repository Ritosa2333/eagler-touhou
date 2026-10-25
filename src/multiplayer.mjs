import {inspectMultiplayerReplay} from './multiplayer-replay.mjs';
import {createAdonisCalibration} from '../../../common/browser/adonis-calibration.mjs';
import {createSpectatorPlaybackBridge} from '../../../common/browser/spectator-playback.mjs';
export function replayStageLoadout(meta){
 const loadout=meta?.loadouts?.[meta.recordedPlayer];
 return loadout?[loadout,meta.loadouts[0],meta.difficulty]:[undefined,undefined,undefined];
}

export function multiplayerBuildWords(manifest) {
 const digest=manifest?.execution?.sha256;
 if(manifest?.game!=='th12'||manifest?.product!=='th12mp'||manifest?.variant!=='multiplayer'||
    manifest?.profile!=='multiplayer'||manifest?.features?.multiplayer!==true||!/^[a-f0-9]{64}$/i.test(digest||''))
  throw Error('TH12 多人 Runtime 身份校验失败');
 return Array.from({length:4},(_,i)=>parseInt(digest.slice(i*8,i*8+8),16)>>>0);
}
export function validateMultiplayerOptions(options={}) {
 if(options.thpracEnabled)throw Error('此 TH12 多人 Runtime 暂不支持 THPrac');
 if(options.netplayChallengeMode!==undefined&&typeof options.netplayChallengeMode!=='boolean')throw Error('TH12 多人挑战模式配置无效');
 if(options.replayViewer===true){
  if(options.netplayMode==='lan'||options.netplaySpectator)throw Error('录像和联机不能同时启动');
  return {replay:true};
 }
 if(options.netplayMode!=='lan')throw Error('请从多人房间启动 TH12 多人游戏');
 const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 const count=options.netplayPlayerCount,spectator=options.netplaySpectator===true,seat=spectator?0:options.netplayPlayer;
 const delay=options.netplayInputDelay??0,automatic=options.netplayInputDelayAuto??false,reserve=options.netplayPredictionReserve??2;
 const timingMode=options.netplayAdonisMode,prediction=options.netplayPredictionLimit??(timingMode===1?0:12);
 if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||url.hash||!room||!run||
    ![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count||
    !Number.isInteger(options.netplaySeed)||options.netplaySeed<0||options.netplaySeed>65535||
    !Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>4||
    !Number.isInteger(delay)||delay<0||delay>9||typeof automatic!=='boolean'||(automatic&&delay)||
    !Number.isInteger(reserve)||reserve<1||reserve>2||![0,1,2].includes(timingMode)||
    prediction!==(timingMode===1?0:12)||(timingMode===0&&(delay!==0||automatic)))
  throw Error('TH12 多人输入配置不匹配：回滚需要固定 12 帧上限，直接模式需要 0 帧输入延迟');
 const loadouts=options.netplayLoadouts;
 if(!Array.isArray(loadouts)||loadouts.length!==count||loadouts.some(v=>!v||!Number.isInteger(v.character)||v.character<0||v.character>2||
    !Number.isInteger(v.shot)||v.shot<0||v.shot>1))throw Error('TH12 多人机体配置无效');
 if(spectator&&(typeof options.netplaySpectatorId!=='string'||!options.netplaySpectatorId))
  throw Error('TH12 观战身份缺失');
 return {replay:false,url,room,run,count,seat,spectator,delay,automatic,reserve,timingMode,prediction,loadouts,challenge:options.netplayChallengeMode===true};
}
export async function multiplayerSetupWords(options,buildWords,cryptoProvider=globalThis.crypto) {
 const config=validateMultiplayerOptions(options);
 if(config.replay)throw Error('录像不建立玩家连接');
 if(!Array.isArray(buildWords)||buildWords.length!==4||!buildWords.some(Boolean))throw Error('TH12 Runtime 构建身份缺失');
 const identity=new TextEncoder().encode('th12mp:'+config.url.origin+config.url.pathname+':'+config.room+':'+config.run);
 const digest=new DataView(await cryptoProvider.subtle.digest('SHA-256',identity));
 const words=[6,config.count,config.seat,options.netplayDifficulty,options.netplaySeed,digest.getUint32(0,true),digest.getUint32(4,true)||1,config.delay];
 for(let i=0;i<3;i++){const value=config.loadouts[i]||{character:0,shot:0};words.push(value.character,value.shot);}
 words.push(config.timingMode,+config.automatic,config.reserve,...buildWords,+config.challenge);
 return {config,words};
}
export function rollbackDiagnostics(core,status){
 const pointer=core._th12_mp_rollback_status?.();
 const raw=pointer===undefined?null:new Uint32Array(core.HEAPU8.buffer,pointer,8);
 const missing=status[4]===0xffffffff?status[3]:Math.max(0,status[3]-status[4]-1);
 return {restores:raw?.[1]??0,resimulated:raw?.[2]??0,committed:raw?.[3]??0,
  correcting:!!raw?.[4],request:raw?.[5]??0xffffffff,frontier:status[3],missingInputs:missing};
}
// The shared native runtime owns input scheduling, prediction and confirmation.
// This adapter translates only TH12 browser input, assets and presentation.
export function createMultiplayerRuntime({core,manifest,getOptions,emit,loadStage,onError,onExit,store}) {
 const buildWords=multiplayerBuildWords(manifest);
 let active=false,readOnly=false,lastReport=0,savedAt=0,exitPending=false,transitioning=false,rollbackCount=0;
 let replayPaused=false,replayRate=1,seekTarget=null,replayPath=null,halfFrame=false,replayMeta=null,savePending=Promise.resolve(),playbackReported=-Infinity;
 const cstring=pointer=>{const heap=core.HEAPU8,end=heap.indexOf(0,pointer);return pointer?new TextDecoder().decode(heap.subarray(pointer,end<0?heap.length:end)):'';};
 const error=()=>Error(cstring(core._th12_mp_error())||'TH12 多人运行失败');
 const withBytes=(bytes,fn)=>{const p=core._malloc(bytes.length);if(!p)throw Error('TH12 多人内存不足');try{core.HEAPU8.set(bytes,p);return fn(p,bytes.length);}finally{core._free(p);}};
 const withString=(value,fn)=>withBytes(new TextEncoder().encode(value+'\0'),fn);
 const words=()=>Array.from(new Uint32Array(core.HEAPU8.buffer,core._th12_mp_status(),24));
 const state=()=>JSON.parse(cstring(core._th12_mp_state()));
 const calibration=createAdonisCalibration({core:{get memory(){return {buffer:core.HEAPU8.buffer};},multiplayer_calibration_status:()=>core._th12_mp_calibration_status(),multiplayer_network_poll:()=>core._th12_mp_pump()},
  getApp:()=>active?1:0,getOptions,emit:(event,fields)=>emit({event,...fields}),game:'th12',build:manifest.execution.sha256,onError,pause:()=>{}});
 const spectator=createSpectatorPlaybackBridge({active:()=>active&&getOptions().netplaySpectator===true,memory:()=>core.HEAPU8,state:()=>core._th12_mp_spectator_state(),
  control:(...args)=>core._th12_mp_spectator_control(...args),allocate:n=>core._malloc(n),deallocate:p=>core._free(p),emit:(event,fields)=>emit({event,...fields})});
 function save(){
  if(!active||readOnly)return;
  const count=withString('PLAYER',p=>core._th12_mp_replay_encode(p));if(!count)return;
  const bytes=core.HEAPU8.slice(core._th12_mp_replay_data(),core._th12_mp_replay_data()+count);
  replayPath??='/savesth12/replay/th12_mp_'+Date.now()+'.rpy';const path=replayPath;
  savePending=savePending.catch(()=>{}).then(async()=>{await store().write(path,bytes);await store().sync();});return savePending;
 }
 async function saveAs(slot,name){
  if(!active||readOnly||!Number.isInteger(slot)||slot<1||slot>25)throw Error('多人录像保存槽位无效');
  const count=withString(String(name).slice(0,8),p=>core._th12_mp_replay_encode(p));if(!count)throw Error('多人录像尚无已确认输入');
  const bytes=core.HEAPU8.slice(core._th12_mp_replay_data(),core._th12_mp_replay_data()+count);
  const id='th12_mp_'+String(slot).padStart(2,'0')+'.rpy';await savePending;
  await store().write('/savesth12/replay/'+id,bytes);await store().sync();return {id,bytes,metadata:describe(inspect(bytes))};
 }
 async function launch(){
  exitPending=false;savedAt=performance.now();replayPath=null;replayMeta=null;core.eaglerOptions=getOptions();
  const {config,words:setup}=await multiplayerSetupWords(getOptions(),buildWords);
  await loadStage(getOptions().netplayDifficulty===4?7:1,config.loadouts[config.seat],config.loadouts[0]);
  if(!withBytes(new Uint8Array(new Uint32Array(setup).buffer),p=>core._th12_mp_configure(p,setup.length)))throw error();
  const connected=withString(getOptions().netplayUrl,url=>config.spectator?withString(getOptions().netplaySpectatorId,id=>core._th12_mp_spectator_connect(url,id)):core._th12_mp_connect(url));
  if(!connected||!core._th12_mp_start())throw error();
  active=true;readOnly=config.spectator;
  globalThis.__eaglerNetplaySpectator=config.spectator;globalThis.__eaglerNetplayAdonisMode=config.timingMode;
  if(config.spectator)spectator.start();else calibration.start();
 }
 async function transition(stage){
  if(transitioning)return;transitioning=true;
  try{
   if(!core._th12_mp_stage_begin())throw error();
   await loadStage(stage,...replayStageLoadout(replayMeta));
   if(!core._th12_mp_stage_loaded())throw error();
  }
  finally{transitioning=false;}
 }
 function sounds(){
  const count=core._th12_mp_sounds_count(),p=core._th12_mp_sounds(),view=new DataView(core.HEAPU8.buffer),out=[];
  for(let n=0;n<count;++n){const at=p+n*16;out.push({id:view.getInt32(at,true),pan:view.getInt32(at+4,true),requests:view.getInt32(at+8,true),stop:!!view.getInt32(at+12,true)});}
  core._th12_mp_sounds_clear();return out;
 }
 function report(now){
  const status=words();globalThis.__eaglerNetplayLanActive=status[9]===1;globalThis.__eaglerNetplayLanFrame=status[3];
  const rollback=rollbackDiagnostics(core,status);rollbackCount=rollback.restores;
  globalThis.__eaglerNetplayLanRollback=rollback.restores;globalThis.__eaglerNetplayLanResimulated=rollback.resimulated;
  globalThis.__eaglerNetplayMissingInputs=rollback.missingInputs;
  globalThis.__eaglerNetplayLanFrameAdvantage=rollback.missingInputs;
  globalThis.__eaglerNetplayInputRepairs=status[17];
  globalThis.__eaglerNetplayInputDelayFrames=status[7];globalThis.__eaglerNetplayLanConfirmed=status[4]===0xffffffff?undefined:status[4];
  globalThis.__eaglerNetplayTransport=status[20]===1?'rtc':status[20]===2?'relay':readOnly?'spectator':'';
  globalThis.__eaglerNetplayPath=globalThis.__eaglerPeerTransport?.route||globalThis.__eaglerNetplayTransport||'connecting';
  if(now-lastReport>=1000){lastReport=now;globalThis.__eaglerNetplayDiagnostics={frame:status[3],confirmed:status[4],inputDelay:status[7],prediction:status[8],rollback};
   emit({event:'runtime-info',netplayInputDelayFrames:status[7],netplayPredictionFrames:status[8],multiplayer:state()});}
  if(!readOnly&&now-savedAt>=15000){savedAt=now;void save().catch(onError);}
  if(status[13]&&!exitPending){exitPending=true;void save().then(async()=>{if(state().restart&&!readOnly){await loadStage(getOptions().netplayDifficulty===4?7:1);if(!core._th12_mp_restart())throw error();exitPending=false;replayPath=null;calibration.start();}else await onExit();}).catch(onError);}
 }
 function tick(input={}){
  if(!active||transitioning)return [];
  const now=performance.now(),m=input.motion,mode=m?.mode??0;
  const held=input.held??0;
  if(replayMeta&&replayPaused&&seekTarget===null){playbackReport(now);return [];}
  if(replayPath&&readOnly&&replayRate===.5&&seekTarget===null){halfFrame=!halfFrame;if(halfFrame)return [];}
  const budget=seekTarget===null?Math.max(1,replayRate):32;
  for(let n=0;n<budget;++n){
   if(!core._th12_mp_step(held&0x32ff,+(!!input.pause),mode,(m?.x??0)/(mode===3?32767:128),(m?.y??0)/(mode===3?32767:128),now))throw error();
   if(replayMeta&&words()[3]>=replayMeta.frameCount){replayPaused=true;seekTarget=null;break;}
   const needed=core._th12_mp_stage_needed();if(needed){void transition(needed).catch(onError);break;}
   if(seekTarget!==null&&words()[3]>=seekTarget){seekTarget=null;break;}
  }
  report(now);playbackReport(now);return sounds();
 }
 function stop(){calibration.stop();spectator.stop();core._th12_mp_stop();active=false;rollbackCount=0;globalThis.__eaglerNetplayLanRollback=globalThis.__eaglerNetplayLanResimulated=0;replayMeta=null;seekTarget=null;exitPending=false;globalThis.__eaglerNetplayLanActive=false;globalThis.__eaglerNetplaySpectator=false;}
 function inspect(bytes){
  if(!withBytes(bytes,(p,n)=>core._th12_mp_replay_validate(p,n)))throw Error('不是有效的 TH12 多人录像');
  return inspectMultiplayerReplay(bytes,{validate:b=>!!withBytes(b,(p,n)=>core._th12_mp_replay_validate(p,n)),buildWords});
 }
 function describe(meta){
  return {name:meta.name,character:meta.loadouts[meta.recordedPlayer].character,shot:meta.loadouts[meta.recordedPlayer].shot,difficulty:meta.difficulty,
   timestamp:meta.timestamp,date:meta.timestamp?new Date(meta.timestamp*1000).toISOString():null,score:meta.score,lastStage:meta.chapters.at(-1).stage,mode:'multiplayer',
   stages:meta.chapters.map((chapter,i)=>({id:chapter.stage,label:chapter.stage===7?'Extra':`Stage ${chapter.stage}`,frames:(meta.chapters[i+1]?.frame??meta.frameCount)-chapter.frame}))};
 }
 function playbackReport(now=performance.now(),force=false){
  if(!replayMeta||!active||!force&&now-playbackReported<100)return;
  playbackReported=now;const frame=words()[3];
  emit({event:'runtime-info',playback:{kind:'replay',path:replayPath.replace(/^\/savesth12\//,''),frame,total:replayMeta.frameCount,stage:state().stage,startFrame:0,startStage:replayMeta.chapters[0].stage,
   difficulty:replayMeta.difficulty,paused:replayPaused,rate:replayRate,seeking:seekTarget!==null,target:seekTarget,preparing:transitioning,ended:frame>=replayMeta.frameCount,description:describe(replayMeta)}});
 }
 async function openReplay(path,stage){
  const bytes=await store().read(path);if(!bytes)throw Error('录像不存在');inspect(bytes);stop();
  const meta=inspect(bytes);await loadStage(meta.difficulty===4?7:1,...replayStageLoadout(meta));
  if(!withBytes(new Uint8Array(new Uint32Array(buildWords).buffer),build=>withBytes(bytes,(p,n)=>core._th12_mp_replay_load(p,n,build))))throw error();
  const chapter=stage===undefined?meta.chapters[0]:meta.chapters.find(c=>c.stage===stage);if(!chapter)throw Error('录像没有此关卡');
  active=readOnly=true;replayMeta=meta;replayPath=path;replayPaused=false;replayRate=1;seekTarget=chapter.frame||null;halfFrame=false;
  globalThis.__eaglerNetplaySpectator=false;playbackReport(performance.now(),true);return meta;
 }
 async function playback(message){
  const path=message.path;
  if(message.action==='inspect'){const bytes=await store().read(path);return {description:describe(inspect(bytes))};}
  if(message.action==='open'){await openReplay(path,message.stage);return {};}
  if(!active||!readOnly||!replayPath)throw Error('请先打开多人录像');
  if(message.action==='pause')replayPaused=true;
  if(message.action==='play')replayPaused=false;
  if(message.action==='rate'){replayRate=message.value;replayPaused=!replayRate;}
  if(message.action==='seek'){
   const target=message.value;if(target>core._th12_mp_replay_frames())throw Error('录像位置超出范围');
   if(target<words()[3]){const paused=replayPaused,rate=replayRate;await openReplay(replayPath);replayPaused=paused;replayRate=rate;}
   seekTarget=target===words()[3]?null:target;
  }
  playbackReport(performance.now(),true);
  return {};
 }
 return {launch,tick,save,saveAs,stop,state,playback,spectatorCommand:spectator.command,inspect,openReplay,
  get active(){return active;},get transitioning(){return transitioning;},get readOnly(){return readOnly;},
  get rollbackCount(){return rollbackCount;},
  pace:elapsed=>active&&!readOnly?core._th12_mp_paced_elapsed(elapsed):elapsed};
}
