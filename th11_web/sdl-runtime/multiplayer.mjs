import {createAdonisCalibration} from './adonis-calibration.mjs';
import {inspectMultiplayerReplay,MULTIPLAYER_REPLAY_MAX_FRAMES} from './multiplayer-replay.mjs';

// This file is a title seam. Session/packet/calibration/RTC/relay behavior stays
// in the pinned common native and browser authorities.
export function multiplayerBuildWords(manifest) {
 const digest=manifest?.execution?.sha256;
 if(manifest?.game!=='th11'||manifest?.product!=='th11mp'||manifest?.variant!=='multiplayer'||
    manifest?.profile!=='multiplayer'||manifest?.features?.multiplayer!==true||!/^[a-f0-9]{64}$/i.test(digest||''))
  throw Error('TH11 多人 Runtime 身份校验失败');
 return Array.from({length:4},(_,i)=>parseInt(digest.slice(i*8,i*8+8),16)>>>0);
}
export function validateMultiplayerOptions(options={}) {
 if(options.thpracEnabled)throw Error('此 TH11 多人 Runtime 暂不支持 THPrac');
 if(options.netplayChallengeMode!==undefined&&typeof options.netplayChallengeMode!=='boolean')throw Error('TH11 多人挑战模式配置无效');
 if(options.replayViewer===true){
  if(options.netplayMode==='lan'||options.netplaySpectator)throw Error('录像和联机不能同时启动');
  return {replay:true};
 }
 if(options.netplayMode!=='lan')throw Error('请从多人房间启动 TH11 多人游戏');
 const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 const count=options.netplayPlayerCount,spectator=options.netplaySpectator===true,seat=spectator?0:options.netplayPlayer;
 const delay=options.netplayInputDelay??0,automatic=options.netplayInputDelayAuto??false,reserve=options.netplayPredictionReserve??2;
 if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||url.hash||!room||!run||
    ![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count||
    !Number.isInteger(options.netplaySeed)||options.netplaySeed<0||options.netplaySeed>65535||
    !Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>4||
    !Number.isInteger(delay)||delay<0||delay>9||typeof automatic!=='boolean'||(automatic&&delay)||
    !Number.isInteger(reserve)||reserve<1||reserve>2||options.netplayAdonisMode!==1||
    (options.netplayPredictionLimit!==undefined&&options.netplayPredictionLimit!==0))
  throw Error('TH11 多人仅支持经过实际通道测量的纯延迟模式，预测必须为 0');
 const loadouts=options.netplayLoadouts;
 if(!Array.isArray(loadouts)||loadouts.length!==count||loadouts.some(v=>!v||!Number.isInteger(v.character)||v.character<0||v.character>1||
    !Number.isInteger(v.shot)||v.shot<0||v.shot>2))throw Error('TH11 多人机体配置无效');
 if(spectator&&(typeof options.netplaySpectatorId!=='string'||!options.netplaySpectatorId))
  throw Error('TH11 观战身份缺失');
 return {replay:false,url,room,run,count,seat,spectator,delay,automatic,reserve,loadouts,challenge:options.netplayChallengeMode===true};
}
export async function multiplayerSetupWords(options,buildWords,cryptoProvider=globalThis.crypto) {
 const config=validateMultiplayerOptions(options);
 if(config.replay)throw Error('录像不建立玩家连接');
 if(!Array.isArray(buildWords)||buildWords.length!==4||!buildWords.some(Boolean))throw Error('TH11 Runtime 构建身份缺失');
 const identity=new TextEncoder().encode('th11mp:'+config.url.origin+config.url.pathname+':'+config.room+':'+config.run);
 const digest=new DataView(await cryptoProvider.subtle.digest('SHA-256',identity));
 const words=[5,config.count,config.seat,options.netplayDifficulty,options.netplaySeed,digest.getUint32(0,true),digest.getUint32(4,true)||1,config.delay];
 for(let i=0;i<3;i++){const value=config.loadouts[i]||{character:0,shot:0};words.push(value.character,value.shot);}
 words.push(1,+config.automatic,config.reserve,...buildWords,+config.challenge);
 return {config,words};
}
export function createMultiplayerRuntime({Module,core,getOptions,getLanguage,manifest,emit,onError,onExit,target=globalThis}) {
 const buildWords=multiplayerBuildWords(manifest),build=manifest.execution.sha256;
 const doc=target.document;
 const stylesheet=doc.createElement('link');stylesheet.rel='stylesheet';stylesheet.href=new URL('./multiplayer.css',import.meta.url).href;doc.head.append(stylesheet);
 let active=false,mode='idle',replayViewer=false,notice=null,failed=false;
 let lastDiagnostic=0,lastGeneration=-1,finished=false;
 const readWords=(pointer,count)=>Array.from(new Uint32Array(core.memory.buffer,pointer,count));
 const textAt=pointer=>{if(!pointer)return '';const heap=new Uint8Array(core.memory.buffer);let end=pointer;while(end<heap.length&&heap[end])++end;return new TextDecoder().decode(heap.subarray(pointer,end));};
 const nativeError=()=>textAt(core.th11_mp_error())||textAt(core.th11_error())||'TH11 多人操作失败';
 const withBytes=(bytes,call)=>{const pointer=core.malloc(bytes.length);if(!pointer)throw Error('TH11 多人内存不足');try{Module.HEAPU8.set(bytes,pointer);return call(pointer,bytes.length);}finally{core.free(pointer);}};
 const withString=(value,call)=>withBytes(new TextEncoder().encode(value+'\0'),call);
 const validate=bytes=>withBytes(bytes,(pointer,size)=>core.th11_mp_replay_validate(pointer,size)===1);
 const inspect=bytes=>inspectMultiplayerReplay(bytes,{validate,buildWords});
 const label=(zh,en)=>getLanguage()==='chs'?zh:en;
 const clearSeek=()=>{notice?.remove();notice=null;};
 const showSeek=text=>{clearSeek();notice=doc.createElement('output');notice.className='th11-mp-replay-seek';notice.setAttribute('aria-live','polite');notice.textContent=text;doc.body.append(notice);};
 // Connection, calibration, failures and room returns are Launcher surfaces.
 // Reuse its standard events and shared transport diagnostics, as TH08/TH10 do.
 const endSession=(message,returnToMenu=false)=>{if(message)emit('notice',{message});void onExit({returnToMenu}).catch(fail);};
 function fail(reason){
  if(failed)return;
  const message=reason?.message||String(reason);failed=true;target.__eaglerNetplayFailed=true;
  target.__eaglerNetplayError=message;calibration.stop();core.th11_loop_stop();clearSeek();onError(reason);
 }
 const calibration=createAdonisCalibration({core:{memory:core.memory,multiplayer_calibration_status:()=>core.th11_mp_calibration_status(),
  multiplayer_network_poll:()=>core.th11_mp_pump()},getApp:()=>active?1:0,getOptions,emit,game:'th11',build,onError:fail,pause:()=>{
   // A peer may close its input lane just after the shared Return frame.
   // That confirmed native terminal state is a normal exit, not a live-game
   // disconnect. Persist it even if the peer's final retirement ACK is late.
   if(active&&mode==='player'){
    const s=status();
    if(gameStatus()[3]===4&&s[3]>0&&s[4]!==0xffffffff&&s[4]>=s[3]-1){finished=true;calibration.stop();endSession(undefined,true);return;}
   }
   if(active&&target.__eaglerPeerTransport?.disconnected&&!core.th11_mp_pump())fail(Error(nativeError()));
   core.th11_loop_stop();
  },target});
 function status(){return readWords(core.th11_mp_status(),24);}
 function gameStatus(){return readWords(core.th11_mp_game_status(),56);}
 function replayStatus(){return readWords(core.th11_mp_replay_ui_status(),12);}
 function flush(){
  if(!active||mode!=='player')return;
  if(!withString('PLAYER',pointer=>core.th11_mp_replay_save(0,pointer)))throw Error(nativeError());
 }
 function applyOptions(){core.th11_mp_always_hitbox(+(getOptions().alwaysHitbox===true||getOptions().touchAlwaysHitbox===true));}
 async function launch(){
  clearSeek();failed=false;finished=false;target.__eaglerNetplayFailed=false;target.__eaglerNetplayError='';
  target.__eaglerNetplayLanActive=false;target.__eaglerNetplaySpectator=false;
  const options=getOptions(),checked=validateMultiplayerOptions(options);
  replayViewer=checked.replay;
  if(replayViewer){
   const words=new Uint32Array(buildWords);
   if(!withBytes(new Uint8Array(words.buffer),pointer=>core.th11_mp_replay_menu(pointer,words.length)))throw Error(nativeError());
   active=true;mode='replay-menu';applyOptions();
   return {runLoop:true};
  }
  const {config,words}=await multiplayerSetupWords(options,buildWords);
  const buffer=new Uint32Array(words);
  if(!withBytes(new Uint8Array(buffer.buffer),pointer=>core.th11_mp_configure(pointer,words.length)))throw Error(nativeError());
  const connected=config.spectator
   ?withString(options.netplayUrl,url=>withString(options.netplaySpectatorId,id=>core.th11_mp_spectator_connect(url,id)))
   :withString(options.netplayUrl,url=>core.th11_mp_connect(url));
  if(!connected)throw Error(nativeError());
  if(!core.th11_mp_start())throw Error(nativeError());
  active=true;mode=config.spectator?'spectator':'player';lastGeneration=-1;applyOptions();
  target.__eaglerNetplaySpectator=config.spectator;
  target.__eaglerNetplayInputDelayFrames=config.spectator?0:config.delay;target.__eaglerNetplayAdonisMode=1;
  if(!config.spectator)calibration.start();
  return {runLoop:true};
 }
 function frame(){
  if(!active||failed)return;
  const state=status(),game=gameStatus(),now=performance.now();
  let viewer=null;
  if(replayViewer){
   viewer=replayStatus();mode=viewer[1]===2?'replay':'replay-menu';
   if(viewer[1]===4){if(!finished){finished=true;void onExit().catch(fail);}return;}
   const seeking=viewer[1]===2&&viewer[2]<viewer[4];
   if(seeking){
    const text=label('正在定位至第 '+viewer[10]+' 关 · ','Seeking to Stage '+viewer[10]+' · ')+Math.min(100,Math.floor(viewer[2]*100/Math.max(1,viewer[4])))+'%';
    if(!notice)showSeek(text);else notice.textContent=text;
   }else clearSeek();
  }else{
   if(state[12]!==lastGeneration){if(lastGeneration>=0&&mode==='player'){finished=false;calibration.start();}lastGeneration=state[12];}
   if(state[13]&&!finished){
    finished=true;
    const capacity=state[3]>=MULTIPLAYER_REPLAY_MAX_FRAMES;
    if(mode==='player'){
     flush();
     if(capacity)endSession(label('已达到本版单局 250,000 确认帧上限，已保存未完成录像。','This run reached the 250,000 confirmed-frame limit. A partial Replay was saved.'));
     else if(game[3]===4)endSession(undefined,true);
    }else endSession(capacity?label('本次观战达到单局 250,000 确认帧上限。','This spectator session reached the 250,000 confirmed-frame limit.'):label('本次观战结束。','Spectator session complete.'));
   }
  }
  target.__eaglerNetplayLanActive=state[9]===1;target.__eaglerNetplayLanFrame=state[3];target.__eaglerNetplayLanConfirmed=state[4]===0xffffffff?undefined:state[4];
  target.__eaglerNetplayTransport=state[20]===1?'rtc':state[20]===2?'relay':state[20]===3?'spectator':'';
  target.__eaglerNetplayPath=target.__eaglerPeerTransport?.route||target.__eaglerNetplayTransport||'connecting';
  target.__eaglerNetplayLanRollback=0;target.__eaglerNetplayLanResimulated=0;
  target.__eaglerNetplayDiagnostics={frame:state[3],confirmed:state[4],inputDelay:state[7],prediction:0,undoBytes:0,generation:state[12],retired:!!state[13],frameZeroReady:!!state[21],
   sent:state[15],received:state[16],repairs:state[17],ignored:state[18],spectatorBacklog:state[14],spectatorPublisherFailed:!!state[22],nativeHash:game[1],mode,replay:viewer};
  if(now-lastDiagnostic>=1000){lastDiagnostic=now;emit('runtime-info',{netplayInputDelayFrames:state[7],netplayPredictionFrames:0});}
 }
 return {
  launch,frame,flush,inspect,validateOptions:validateMultiplayerOptions,applyOptions,status,gameStatus,replayStatus,
  get active(){return active;},get readOnly(){return mode!=='player';},get mode(){return mode;},
  // The native viewer owns menu navigation and physical Esc. Recorded inputs
  // remain the only source of gameplay while a Replay is playing.
  acceptInput(){return active&&!failed&&!finished&&(mode==='player'||replayViewer);},
  keyboard(event,down){
   if(!active)return false;
   const code=event.code||event.key;
   if(mode==='spectator'&&code==='Escape'){if(down&&!event.repeat)void onExit();return true;}
   return false;
  },
  afterAudioResume(){},
  fail,
  stop(){calibration.stop();clearSeek();active=false;mode='idle';replayViewer=false;target.__eaglerNetplayLanActive=false;core.th11_mp_stop();},
 };
}
