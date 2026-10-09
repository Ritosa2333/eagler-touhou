import {createAdonisCalibration} from './adonis-calibration.mjs';
import {inspectMultiplayerReplay,multiplayerReplayPath,MULTIPLAYER_REPLAY_MAX_FRAMES} from './multiplayer-replay.mjs';

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
 if(options.thpracEnabled||options.netplayChallengeMode===true)
  throw Error('此 TH11 多人 Runtime 暂不支持 THPrac 或挑战模式');
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
 return {replay:false,url,room,run,count,seat,spectator,delay,automatic,reserve,loadouts};
}
export async function multiplayerSetupWords(options,buildWords,cryptoProvider=globalThis.crypto) {
 const config=validateMultiplayerOptions(options);
 if(config.replay)throw Error('录像不建立玩家连接');
 if(!Array.isArray(buildWords)||buildWords.length!==4||!buildWords.some(Boolean))throw Error('TH11 Runtime 构建身份缺失');
 const identity=new TextEncoder().encode('th11mp:'+config.url.origin+config.url.pathname+':'+config.room+':'+config.run);
 const digest=new DataView(await cryptoProvider.subtle.digest('SHA-256',identity));
 const words=[5,config.count,config.seat,options.netplayDifficulty,options.netplaySeed,digest.getUint32(0,true),digest.getUint32(4,true)||1,config.delay];
 for(let i=0;i<3;i++){const value=config.loadouts[i]||{character:0,shot:0};words.push(value.character,value.shot);}
 words.push(1,+config.automatic,config.reserve,...buildWords,0);
 return {config,words};
}
export function createMultiplayerRuntime({Module,core,getOptions,getLanguage,manifest,emit,onError,onExit,sync,canvas,target=globalThis}) {
 const buildWords=multiplayerBuildWords(manifest),build=manifest.execution.sha256;
 const doc=target.document;
 let active=false,mode='idle',panel=null,toolbar=null,notice=null,paused=false,metadata=null,seekTarget=null;
 let lastDiagnostic=0,lastGeneration=-1,finished=false,flushed=false,selectedPath='';
 const readWords=(pointer,count)=>Array.from(new Uint32Array(core.memory.buffer,pointer,count));
 const textAt=pointer=>{if(!pointer)return '';const heap=new Uint8Array(core.memory.buffer);let end=pointer;while(end<heap.length&&heap[end])++end;return new TextDecoder().decode(heap.subarray(pointer,end));};
 const nativeError=()=>textAt(core.th11_mp_error())||textAt(core.th11_error())||'TH11 多人操作失败';
 const withBytes=(bytes,call)=>{const pointer=core.malloc(bytes.length);if(!pointer)throw Error('TH11 多人内存不足');try{Module.HEAPU8.set(bytes,pointer);return call(pointer,bytes.length);}finally{core.free(pointer);}};
 const withString=(value,call)=>withBytes(new TextEncoder().encode(value+'\0'),call);
 const validate=bytes=>withBytes(bytes,(pointer,size)=>core.th11_mp_replay_validate(pointer,size)===1);
 const inspect=bytes=>inspectMultiplayerReplay(bytes,{validate,buildWords});
 const chinese=()=>getLanguage()==='chs';
 const label=(zh,en)=>chinese()?zh:en;
 const removeUi=()=>{panel?.remove();toolbar?.remove();notice?.remove();panel=toolbar=notice=null;};
 const button=(text,call)=>{const node=doc.createElement('button');node.type='button';node.textContent=text;node.addEventListener('click',()=>{try{const result=call();result?.catch?.(fail);}catch(error){fail(error);}});return node;};
 const heading=(owner,text)=>{const h=doc.createElement('h2');h.textContent=text;owner.append(h);};
 const makePanel=()=>{panel?.remove();panel=doc.createElement('section');panel.id='th11-multiplayer-menu';panel.setAttribute('aria-label',label('多人录像','Multiplayer Replay'));panel.style.cssText='position:fixed;inset:6%;z-index:30;overflow:auto;padding:24px;background:#131726f5;color:#f7f5ee;font:16px/1.5 sans-serif;border:1px solid #8e9fc2;border-radius:12px';doc.body.append(panel);return panel;};
 const makeNotice=text=>{notice?.remove();notice=doc.createElement('output');notice.setAttribute('aria-live','polite');notice.style.cssText='position:fixed;left:50%;top:10%;transform:translateX(-50%);z-index:24;max-width:80%;padding:12px 18px;background:#101426ed;color:white;font:16px/1.5 sans-serif;border-radius:8px;white-space:pre-line';notice.textContent=text;doc.body.append(notice);};
 const terminalControls=text=>{toolbar?.remove();toolbar=doc.createElement('div');toolbar.id='th11-multiplayer-session-controls';
  toolbar.style.cssText='position:fixed;right:8px;bottom:8px;z-index:24;max-width:calc(100vw - 16px);display:flex;align-items:center;justify-content:flex-end;gap:8px;flex-wrap:wrap;color:white;font:12px/1.4 sans-serif;text-shadow:0 1px 2px #000;pointer-events:none';
  const message=doc.createElement('output');message.setAttribute('aria-live','polite');message.textContent=text;
  const exit=button(label('返回房间','Back to room'),onExit);exit.style.pointerEvents='auto';toolbar.append(message,exit);doc.body.append(toolbar);
 };
 function fail(reason){
  target.__eaglerNetplayFailed=true;target.__eaglerNetplayError=reason?.message||String(reason);
  calibration.stop();core.th11_loop_stop();onError(reason);
  if(active){const owner=makePanel();heading(owner,label('多人运行已停止','Multiplayer stopped'));const text=doc.createElement('p');text.textContent=target.__eaglerNetplayError;owner.append(text,button(label('返回','Back'),onExit));}
 }
 const calibration=createAdonisCalibration({core:{memory:core.memory,multiplayer_calibration_status:()=>core.th11_mp_calibration_status(),
  multiplayer_network_poll:()=>core.th11_mp_pump()},getApp:()=>active?1:0,getOptions,emit,game:'th11',build,onError:fail,pause:()=>{
   // The shared disconnect callback can stop RAF before its next native pump.
   // Let the title turn that terminal transport state into a visible error.
   if(active&&target.__eaglerPeerTransport?.disconnected&&!core.th11_mp_pump())fail(Error(nativeError()));
   core.th11_loop_stop();
  },target});
 function status(){return readWords(core.th11_mp_status(),24);}
 function gameStatus(){return readWords(core.th11_mp_game_status(),56);}
 function flush(){
  if(!active||mode!=='player')return;
  if(!withString('PLAYER',pointer=>core.th11_mp_replay_save(0,pointer)))throw Error(nativeError());
 }
 function showReplayMenu(){
  core.th11_loop_stop();core.th11_mp_stop();mode='replay-menu';metadata=null;seekTarget=null;paused=false;finished=false;
  toolbar?.remove();toolbar=null;notice?.remove();notice=null;
  const owner=makePanel();heading(owner,label('多人录像','Multiplayer Replay'));
  const explanation=doc.createElement('p');explanation.textContent=label('选择录像后可按关卡定位。每份录像包含全部玩家的已确认输入。','Choose a Replay to watch or seek by stage. Each Replay includes all players.');owner.append(explanation);
  const table=doc.createElement('table');table.style.cssText='width:100%;border-collapse:collapse;text-align:left';
  const head=doc.createElement('tr');for(const text of [label('录像','Replay'),label('记录玩家','Recorded player'),label('模式 / 得分','Mode / Score'),label('时间','Date')]){const cell=doc.createElement('th');cell.textContent=text;cell.style.padding='8px';head.append(cell);}table.append(head);
  let files=[];try{files=Module.FS.readdir('/savesth11mp/replay').filter(name=>/^th11_(?:\d{2}|ud[a-z0-9]{4})\.rpy$/i.test(name)).sort();}catch{}
  for(const name of files){
   const row=doc.createElement('tr'),path=multiplayerReplayPath('replay/'+name);let info,reason='';
   try{info=inspect(Module.FS.readFile('/savesth11mp/'+path));}catch(error){reason=error.message;}
   const values=[null,info?'P'+(info.recordedPlayer+1)+' / '+info.playerCount+'P':'—',
    info?[['Easy','Normal','Hard','Lunatic','Extra'][info.difficulty],info.score.toLocaleString(),info.completed?label('完成','Complete'):label('途中','Partial')].join(' · '):reason,
    info?.timestamp?new Date(info.timestamp*1000).toLocaleString():'—'];
   values.forEach((value,i)=>{const cell=doc.createElement('td');cell.style.cssText='padding:10px 8px;border-top:1px solid #42506b';
    if(i===0){const select=button(name,()=>playReplay(path));select.disabled=!info;cell.append(select);}else cell.textContent=value;row.append(cell);});
   table.append(row);
  }
  if(files.length)owner.append(table);else{const empty=doc.createElement('p');empty.textContent=label('没有多人录像。可先在 Launcher 的多人录像页导入。','No multiplayer Replays. Import one through the Launcher.');owner.append(empty);}
  owner.append(button(label('返回','Back'),onExit));owner.querySelector('button:not(:disabled)')?.focus();
 }
 function replayToolbar(){
  toolbar?.remove();toolbar=doc.createElement('div');toolbar.id='th11-multiplayer-replay-controls';toolbar.style.cssText='position:fixed;inset:auto 12px 12px;z-index:25;display:flex;align-items:center;gap:10px;flex-wrap:wrap;padding:10px;background:#101426ed;color:white;font:14px sans-serif;border-radius:8px';
  const pause=button(label('暂停','Pause'),togglePause);pause.dataset.action='pause';
  const stages=doc.createElement('select');stages.setAttribute('aria-label',label('关卡','Stage'));
  for(const chapter of metadata.chapters){const choice=doc.createElement('option');choice.value=String(chapter.frame);choice.textContent=label('第 '+chapter.stage+' 关','Stage '+chapter.stage);stages.append(choice);}
  stages.addEventListener('change',()=>{try{seek(Number(stages.value));}catch(error){fail(error);}});
  const progress=doc.createElement('input');progress.type='range';progress.min='0';progress.max=String(metadata.frameCount-1);progress.value='0';progress.step='1';progress.style.flex='1';progress.setAttribute('aria-label',label('播放进度','Replay position'));
  progress.addEventListener('change',()=>{try{seek(Number(progress.value));}catch(error){fail(error);}});
  const position=doc.createElement('output');position.dataset.action='position';position.style.minWidth='9em';
  const identity=doc.createElement('span');identity.textContent='P'+(metadata.recordedPlayer+1)+' · '+metadata.playerCount+'P';
  toolbar.append(button(label('录像列表','Replay list'),showReplayMenu),pause,stages,progress,position,identity);doc.body.append(toolbar);
 }
 function playReplay(path){
  const bytes=Module.FS.readFile('/savesth11mp/'+multiplayerReplayPath(path));const info=inspect(bytes);
  core.th11_loop_stop();core.th11_mp_stop();
  if(!withBytes(bytes,(pointer,size)=>core.th11_mp_replay_load(pointer,size)))throw Error(nativeError());
  panel?.remove();panel=null;notice?.remove();notice=null;metadata=info;selectedPath=path;mode='replay';paused=false;seekTarget=null;finished=false;
  applyOptions();replayToolbar();core.th11_loop_start();canvas.focus({preventScroll:true});
 }
 function togglePause(){
  if(mode!=='replay')return;paused=!paused;core.sdl_loop_pause(paused?1:0);
  const control=toolbar?.querySelector('[data-action="pause"]');if(control)control.textContent=paused?label('继续','Resume'):label('暂停','Pause');
 }
 function seek(frame){
  if(mode!=='replay'||!Number.isInteger(frame)||frame<0||frame>=metadata.frameCount)throw Error('Invalid TH11 Replay position');
  core.th11_loop_stop();if(!core.th11_mp_replay_seek(frame))throw Error(nativeError());
  seekTarget=frame;finished=false;makeNotice(label('正在定位录像…','Seeking Replay…'));core.th11_loop_start();
 }
 function applyOptions(){core.th11_mp_always_hitbox(+(getOptions().alwaysHitbox===true||getOptions().touchAlwaysHitbox===true));}
 async function launch(){
  removeUi();target.__eaglerNetplayFailed=false;target.__eaglerNetplayError='';
  const options=getOptions(),checked=validateMultiplayerOptions(options);
  if(checked.replay){active=true;showReplayMenu();return {runLoop:false};}
  const {config,words}=await multiplayerSetupWords(options,buildWords);
  const buffer=new Uint32Array(words);
  if(!withBytes(new Uint8Array(buffer.buffer),pointer=>core.th11_mp_configure(pointer,words.length)))throw Error(nativeError());
  const connected=config.spectator
   ?withString(options.netplayUrl,url=>withString(options.netplaySpectatorId,id=>core.th11_mp_spectator_connect(url,id)))
   :withString(options.netplayUrl,url=>core.th11_mp_connect(url));
  if(!connected)throw Error(nativeError());
  if(!core.th11_mp_start())throw Error(nativeError());
  active=true;mode=config.spectator?'spectator':'player';finished=false;flushed=false;lastGeneration=-1;applyOptions();
  target.__eaglerNetplayInputDelayFrames=config.spectator?0:config.delay;target.__eaglerNetplayAdonisMode=1;
  makeNotice(config.spectator?label('正在等待已确认的观战数据…','Waiting for confirmed spectator data…'):label('正在测量实际游戏连接…','Measuring the game connection…'));
  if(!config.spectator)calibration.start();
  return {runLoop:true};
 }
 function frame(){
  if(!active||mode==='replay-menu')return;
  const state=status(),game=gameStatus(),now=performance.now();
  if(state[12]!==lastGeneration){if(lastGeneration>=0&&mode==='player'){flushed=false;finished=false;makeNotice(label('正在重新测量连接…','Measuring the new connection…'));calibration.start();}lastGeneration=state[12];}
  if(state[3]>0&&seekTarget===null){notice?.remove();notice=null;}
  if(mode==='replay'&&metadata){
   const cursor=Math.min(state[3],metadata.frameCount);
   if(seekTarget!==null){
    if(cursor>=seekTarget){seekTarget=null;notice?.remove();notice=null;core.sdl_loop_pause(paused?1:0);}
    else if(notice)notice.textContent=label('正在定位录像 · ','Seeking Replay · ')+Math.min(100,Math.floor(cursor*100/Math.max(1,seekTarget)))+'%';
   }
   const progress=toolbar?.querySelector('input[type="range"]');if(progress&&doc.activeElement!==progress)progress.value=String(Math.min(cursor,metadata.frameCount-1));
   const position=toolbar?.querySelector('[data-action="position"]');if(position)position.textContent=(cursor/60).toFixed(1)+' / '+(metadata.frameCount/60).toFixed(1)+' s';
  }
  if(state[13]&&!finished){
   finished=true;
   const capacity=state[3]>=MULTIPLAYER_REPLAY_MAX_FRAMES;
   if(mode==='player'){flush();flushed=true;void sync(false).catch(fail);terminalControls(capacity?label('已达到本版单局 250,000 确认帧上限，已保存未完成录像。','This run reached the 250,000 confirmed-frame limit. A partial Replay was saved.'):label('多人录像已保存。','Multiplayer Replay saved.'));}
   else if(mode==='replay'){paused=true;core.sdl_loop_pause(1);const control=toolbar?.querySelector('[data-action="pause"]');if(control)control.textContent=capacity?label('达到帧数上限','Frame limit reached'):label('播放完毕','Replay complete');}
   else terminalControls(capacity?label('本次观战达到单局 250,000 确认帧上限。','This spectator session reached the 250,000 confirmed-frame limit.'):label('本次观战结束。','Spectator session complete.'));
  }
  target.__eaglerNetplayLanActive=state[9]===1;target.__eaglerNetplayLanFrame=state[3];target.__eaglerNetplayLanConfirmed=state[4]===0xffffffff?undefined:state[4];
  target.__eaglerNetplayTransport=state[20]===1?'rtc':state[20]===2?'relay':state[20]===3?'spectator':'';
  target.__eaglerNetplayLanRollback=0;target.__eaglerNetplayLanResimulated=0;
  target.__eaglerNetplayDiagnostics={frame:state[3],confirmed:state[4],inputDelay:state[7],prediction:0,undoBytes:0,generation:state[12],retired:!!state[13],frameZeroReady:!!state[21],
   sent:state[15],received:state[16],repairs:state[17],ignored:state[18],spectatorBacklog:state[14],spectatorPublisherFailed:!!state[22],nativeHash:game[1],mode,recordedPlayer:metadata?.recordedPlayer};
  if(now-lastDiagnostic>=1000){lastDiagnostic=now;emit('runtime-info',{netplayInputDelayFrames:state[7],netplayPredictionFrames:0});}
 }
 return {
  launch,frame,flush,inspect,validateOptions:validateMultiplayerOptions,applyOptions,status,gameStatus,
  get active(){return active;},get readOnly(){return mode!=='player';},get mode(){return mode;},
  acceptInput(){return active&&mode==='player'&&!finished;},
  keyboard(event,down){if(!active||mode==='player')return false;
   if(down&&!event.repeat&&(event.code==='Escape'||event.key==='Escape')){if(mode==='replay')showReplayMenu();else void onExit();return true;}
   if(down&&!event.repeat&&mode==='replay'&&(event.code==='Space'||event.key===' ')){togglePause();return true;}
   return mode==='replay'||mode==='spectator';
  },
  afterAudioResume(){if(mode==='replay'&&paused&&seekTarget===null)core.sdl_loop_pause(1);},
  fail,
  stop(){calibration.stop();removeUi();active=false;mode='idle';core.th11_mp_stop();},
 };
}
