import {publishNetworkStatus,mountNetworkStatus} from '../network-status.js';
import {startFrameLoop} from '../frame-limit.js';
import {createInputChannel,acceptInputChannel} from './input-transport.js';
import {readNetworkMode,networkUrl} from './network-mode.js';
import {PROTOCOL,RollbackQueue,TICK_MS,MAX_ROLLBACK,BOOT_DELAY,MAX_INPUT_DELAY,MAX_ROLLBACK_INPUT_DELAY,lockstepDelayForRtt,rollbackPolicyForRtt,validRollbackPolicy,rollbackRttForSamples} from './rollback-queue.js';
import {mountPlayer} from '../player-ui.js';
import {isBound,keyboardBits,gamepadBits,gamepadStatus,normalize} from './controls.js';
import {mountFocusSettings} from './focus-display.js';
import {mountControlSettings,controlSnapshot,isFormTarget} from '../control-settings.js';
import {InputLatch} from './input-latch.js';
import {createRecorder} from '../replay/recorder.js';
const downloadFaultReport=(value,name)=>{const url=URL.createObjectURL(new Blob([JSON.stringify(value,null,2)],{type:'application/json'})),a=document.createElement('a');a.href=url;a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};
import {createRelay} from './relay.js';
import {mergeDeparture} from './membership.js';
import {loadRtcConfiguration,refreshRtcPath,describeRtcConfiguration,countIceCandidate,describeIceCandidates,recordIceServerError} from './connection.js';
import {assetBytes,freshJSON,parallelAssets} from '../preload-cache.js';
import {bgmVersion,onBgmVersion,resolveBgmTrack,loadMidiTracks} from '../bgm-version.js';
const $=id=>document.getElementById(id),roles=['host','guest','guest2'];
const label=role=>({host:'房主',guest:'客机 1',guest2:'客机 2'})[role];
const status=text=>{$('status').textContent=text;player.note(text);};
const settingNames=['players','difficulty','lives','bombs'];
const assistFlags=['focusEnabled','teamTargeting'];
let session,state,iframe,runtime,queue,localReady;
let preloadKey='',preloadPromise=null,preloadGeneration=0,preloadRetryAt=0;
const mode=readNetworkMode();
let useRelay=mode.transport==='ws';
// Legacy URL preference only seeds a newly created room. Joining clients
// always use the host's server-authoritative setting, frozen at game start.
const defaultRollback=mode.rollback!=='off';
const inputLatch=new InputLatch();
let inputPolicy;
let timingSent=false,runDelay=null,runInputDelay=null,runPolicy=null;
let relay;
let preparing=false,running=false,stopped=false,busy=false,runRequested=false;
let returningTask=null,runEpoch=0;
let nativeBusy=false;
const boundaryActions=[];
function atNativeBoundary(action){if(nativeBusy)boundaryActions.push(action);else action();}
function completeNativeBoundary(){nativeBusy=false;for(const action of boundaryActions.splice(0)){if(stopped)break;action();}}
let pollTimer,heartbeatTimer,loadTimer,raf,wakeTimer=null,wakeAt=0;
let wakePending=false,wakeToken=0;
const wakeChannel=new MessageChannel();
wakeChannel.port1.onmessage=event=>{
  if(event.data!==wakeToken||!wakePending)return;
  wakePending=false;wakeTimer=null;pump(performance.now());
};
let waitingSince=0,replayUntil=0,rollbackCount=0,resimulated=0,maxRewind=0;
let replayTouchContext=null;
let waitedMs=0,measuredWaitMs=0,waitPercent=0,measuredGame=null,gameHz=null;
const performanceHistory=[];
function endWait(){if(waitingSince){waitedMs+=performance.now()-waitingSince;waitingSince=0;}}
const pendingHashes=new Map();
let keys=new Set(),childBits=0,pendingAction=0,bits=0,inputChangedAt=0;
let peers=new Map(),hashes=new Map(),captureTimes=new Map();
let pumping=false,lastNow=0,accumulator=0,lastUi=0,lastMeasure=0,measureFrame=0;
let stepCost=0,stepCount=0,maxCost=0,hz=0,averageCost=0,peakCost=0,inputAge=0,lastApplied=0,receivedInputs=0;
const newCosts=()=>({capture:0,captures:0,simulate:0,steps:0,present:0,presentations:0,hash:0,restore:0,restores:0,confirm:0,replay:0,replays:0,lead:0});
const REPLAY_MAX_STEPS=4;
let costs=newCosts(),measuredCosts=newCosts(),waitDetail='',earlyInputs=0;
const activeRoles=()=>roles.slice(0,state.settings.players);
const onlinePeers=()=>[...peers.values()].filter(peer=>!peer.offline);
const GRACE_MS=5000;
let membership=null,membershipId=0;
let pollFailedAt=0,roomServiceError='';
let bootStage='等待加载游戏页面';
let bootStep=0,bootStartedAt=0;
let connectionConfigPending=false,pendingConnectionSignals=[];
let connectionAttempt=0,connectionReadySent=false,fallbackPending=false;
let connectionConfigSummary='尚未读取';
let stopReason='';
const player=mountPlayer($('game'),{onChange:()=>{if(running){observeInput();wake();}},onGesture:async()=>{
  if(!runtime)return;
  if(await runtime.unlockAudio())player.note('声音已启用。靠近队友并停火，低速键连按 5 次；每两次最多间隔 24 个游戏帧。');
  else player.note('声音尚未就绪，进入游戏后点击“启用声音”。');
}});
const rawView=document.createElement('button');rawView.type='button';let rawEnabled=false;
rawView.textContent='显示：本机判定点';rawView.setAttribute('aria-pressed','false');
rawView.onclick=()=>{rawEnabled=!rawEnabled;runtime?.setRawPresentation(rawEnabled);rawView.textContent=rawEnabled?'显示：原始画面':'显示：本机判定点';rawView.setAttribute('aria-pressed',String(rawEnabled));};
$('game').querySelector('.player-toolbar').append(rawView);
const musicControl=document.createElement('label');musicControl.textContent='音乐 ';
const musicVolume=document.createElement('input');musicVolume.type='range';musicVolume.min=0;musicVolume.max=100;musicVolume.value=70;musicVolume.setAttribute('aria-label','浏览器 BGM 音量');musicVolume.style.width='100px';
try{const saved=localStorage.getItem('th02.network.bgmVolume');if(saved!==null)musicVolume.value=Math.max(0,Math.min(100,Number(saved)||0));}catch{}
musicControl.append(musicVolume);$('game').querySelector('.player-toolbar').append(musicControl);
musicVolume.oninput=()=>{runtime?.setMusicVolume(Number(musicVolume.value)/100);try{localStorage.setItem('th02.network.bgmVolume',musicVolume.value);}catch{}};
const faultDownload=document.createElement('button');faultDownload.type='button';faultDownload.dataset.faultDownload='';faultDownload.textContent='下载故障信息';
faultDownload.onclick=()=>{
  if(!runtime){player.note('请先启动游戏。');return;}
  try{downloadFaultReport({schema:'touhou-network-fault/1',recordedAt:new Date().toISOString(),
    environment:{userAgent:navigator.userAgent,visibility:document.visibilityState},
    settings:state?.settings,slot:queue?.slot,frame:queue?.frame,confirmed:queue?.confirmed,
    running,stopped,stopReason,inputPolicy,lockstepDelay:queue?.lockstepDelay,
    predictionLeadLimit:queue?.active?queue.predictionLead:0,
    rollbackSafetyWindow:MAX_ROLLBACK,render:runtime.renderInfo(),performanceHistory,
    inputTransport:Object.fromEntries([...peers].map(([role,peer])=>[role,peer.channel?.transportStats??null])),
    diagnostics:$('diagnostics').textContent,runtime:runtime.faultReport()},'th02-network-fault.json');
    player.note('故障信息已下载，只保存在本机。');
  }catch(error){player.note('读取故障信息失败：'+error.message);}
};
$('game').querySelector('.player-toolbar').append(faultDownload);
const replayRecorder=createRecorder($('game').querySelector('.player-toolbar'));
globalThis.th02Replay=replayRecorder;

function startupFailure(reason){
  if(!preparing)return;
  if(!running)showStartup();
  const panel=$('startup-progress');panel.hidden=false;
  const message=document.createElement('p');message.setAttribute('role','alert');
  message.textContent=`本局已停止：${reason}。下面是停止前的最后状态，不再更新。`;
  panel.prepend(message);
  $('diagnostics').textContent=`本局已停止：${reason}\n${$('diagnostics').textContent}`;
  player.diagnostics($('diagnostics').textContent,{show:true});
  // Publish even when WebRTC never opened. This is a diagnostic, not an
  // authoritative gameplay departure or a reason to alter other peers' state.
  if(session){
    const q=new URLSearchParams({room:session.room,token:session.token,boot_stage:`启动已停止：${reason}`.slice(0,300),boot_step:String(bootStep)});
    fetch(`/api/events?${q}`,{signal:AbortSignal.timeout(5000)}).catch(()=>{});
  }
}
function publishConnectionStatus(){
  const now=performance.now();
  mountNetworkStatus($('game')?.querySelector('.player-toolbar'));
  publishNetworkStatus({phase:!preparing&&!running?'idle':stopped?'stopped':running?'connected':'connecting',
    bufferFrames:running?(queue.active?queue.inputDelay:queue.lockstepDelay):null,
    bufferMs:running?(queue.active?queue.inputDelay:queue.lockstepDelay)*TICK_MS:null,serviceRetry:!!roomServiceError,
    peers:[...peers.values()].map(p=>({name:`P${(state?.slots?.[p.role]??'?')==='?'?'?':state.slots[p.role]+1}`,path:p.path||(useRelay?'WebSocket 中继':''),
      state:p.offline?'offline':p.suspectAt!=null?'recovering':p.channel?.readyState==='open'?'connected':'connecting',
      rttMs:p.rtt,rttAgeMs:p.rttAt==null?null:now-p.rttAt,packetAgeMs:now-p.lastPacket,hidden:p.hidden,
      remainingMs:p.suspectAt==null?null:Math.max(0,GRACE_MS-(now-p.suspectAt)),attempt:p.attempt,
      recovery:useRelay?'等待中继恢复':'恢复设备连接',reason:p.suspectReason}))});
}
function showStartup(){
  publishConnectionStatus();
  const panel=$('startup-progress');
  if(running){panel.hidden=true;return;}
  if(!preparing||stopped)return;
  panel.hidden=false;panel.replaceChildren();
  const rows=[{name:'本机',stage:bootStage,step:bootStep},...onlinePeers().map(p=>({
    name:label(p.role),stage:p.ready?'资源已就绪':p.bootStage||'尚未收到加载状态（不代表正在下载）',
    step:p.ready?7:p.bootStep??0,connection:useRelay?`数据通道 ${p.channel?.readyState||'未建立'} · WebSocket 中继${p.iceWarning?' · '+p.iceWarning:''}`:`数据通道 ${p.channel?.readyState||'未建立'} / ICE ${p.pc.iceConnectionState} / 协商 ${p.pc.signalingState} · ${p.path||'路径待确认'} · ${p.signalStage||'等待协商'} · 候选 本机 ${p.localCandidates||0} / 收到 ${p.remoteCandidates||0} / 已提交 ${p.appliedCandidates||0}${p.iceWarning?' · '+p.iceWarning:''}`,
    iceDetails:useRelay?'WebSocket 中继：不使用 ICE/STUN/TURN，等待全员连入同一中继房间。':`候选收集 ${p.pc.iceGatheringState}；本机：${describeIceCandidates(p.localCandidateTypes)}；收到：${describeIceCandidates(p.remoteCandidateTypes)}。服务错误：${p.iceServerErrors.join('；')||'未报告'}。`,
    age:p.bootReportedAt==null?null:(performance.now()-p.bootReportedAt)/1000
  }))];
  for(const row of rows){
    const box=document.createElement('div'),caption=document.createElement('p'),bar=document.createElement('progress');
    caption.textContent=`${row.name}：${row.stage}${row.connection?' · '+row.connection:''}${row.age>5?` · 状态已 ${Math.floor(row.age)} 秒未更新`:''}`;
    bar.max=7;bar.value=row.step;bar.style.width='100%';bar.setAttribute('aria-label',`${row.name} 已完成 ${row.step}/7 个加载阶段`);
    const note=document.createElement('small');note.textContent=`已完成 ${row.step}/7 个阶段（不是下载百分比）`;
    box.append(caption,bar,note);panel.append(box);
    if(row.iceDetails){const details=document.createElement('p');details.textContent=row.iceDetails;box.append(details);}
  }
  $('diagnostics').textContent=`启动已用 ${Math.floor((performance.now()-bootStartedAt)/1000)} 秒。资源加载与连接建立是两个独立过程。\n`
    +`本机实际连接配置：${connectionConfigSummary}\n`
    +`开局链路测量：${onlinePeers().map(peer=>`${label(peer.role)} ${peer.timingSamples?.length||0}/3`).join(' · ')}；全员测量完成后统一输入缓冲。\n`
    +'资源就绪但通道仍 connecting：正在建立网络连接，不是继续下载资源。\n'
    +(useRelay?'本局通过房间服务转发可靠有序消息；不进行 NAT 穿透。':'候选类型：host 本地地址，srflx 服务器反射地址，relay 中继地址，prflx 对端反射地址。计数不代表连接成功；701 表示某次 ICE 服务访问失败，不代表所有候选都失败。');
  if(roomServiceError)$('diagnostics').textContent+='\n房间服务正在重试：'+roomServiceError;
  player.diagnostics(`${rows.map(row=>`${row.name}：${row.step}/7 · ${row.stage}${row.connection?'\n'+row.connection:''}`).join('\n\n')}\n\n${$('diagnostics').textContent}`);
  player.note(`启动 ${Math.floor((performance.now()-bootStartedAt)/1000)} 秒 · 本机 ${bootStep}/7：${bootStage}。点击“启动／运行详情”查看各端状态。`);
  if(localReady&&onlinePeers().some(peer=>peer.channel?.readyState!=='open')){
    player.note(useRelay?'游戏已加载，等待所有玩家连接服务器转发。':'游戏已加载，正在尝试设备直连；8 秒内未连通将自动改用服务器转发。');
  }
}
function closeDisconnected(reason){
  replayRecorder.finish(reason);
  if(stopped)return;
  stopReason=String(reason);startupFailure(stopReason);
  stopped=true;running=false;clearInput();runtime?.stop();iframe?.remove();runtime=null;
  clearTimers();disconnect();render();status(`${reason}。本局游戏已关闭。`);
}
function suspect(peer){
  if(stopped||peer.offline||peers.get(peer.role)!==peer)return;
  if(!running){
    // No gameplay membership change during boot. A broken direct channel
    // still requests the shared fallback, including after it briefly opened.
    peer.iceWarning=`开局连接等待：${peer.pc.connectionState}（由建连/加载期限处理）`;
    if(!useRelay)requestRelayFallback(`${label(peer.role)}设备直连中断`);
    showStartup();
    return;
  }
  peer.suspectAt??=performance.now();
  peer.recovering=true;
  status(`${label(peer.role)}连接中断，正在恢复（最多 ${GRACE_MS/1000} 秒），保留本局进度…`);
}
function reportDeparture(role){
  if(session.role==='host')beginDeparture([role]);
  else send(peers.get('host'),{type:'suspect',role});
}
function beginDeparture(dropped){
  if(nativeBusy){boundaryActions.push(()=>beginDeparture(dropped));return;}
  if(stopped||!running||session.role!=='host')return;
  const targets=[...new Set([...(membership?.roles||[]),...dropped])].filter(r=>!peers.get(r)?.offline);
  if(!targets.length||targets.some(r=>r==='host'||!peers.has(r)))throw Error('无效离线目标');
  membership={id:++membershipId,roles:targets,reports:new Map(),started:performance.now()};
  // Freeze before collecting reports. The original guest input lane may have
  // arrived at different frontiers; surviving peers contribute their history.
  membership.reports.set(session.role,queue.membershipReport());
  broadcast({type:'departure-prepare',id:membership.id,roles:targets});
  finishDeparture();
}
function finishDeparture(){
  if(nativeBusy){boundaryActions.push(finishDeparture);return;}
  if(session.role!=='host'||!membership)return;
  const remaining=onlinePeers().filter(p=>!membership.roles.includes(p.role));
  if(!remaining.every(p=>membership.reports.has(p.role)))return;
  const closed=remaining.filter(p=>p.channel?.readyState!=='open');
  if(closed.length){beginDeparture(closed.map(p=>p.role));return;}
  const change=mergeDeparture([...membership.reports.values()],membership.roles.map(r=>state.slots[r]),state.settings.players);
  const message={type:'departure-commit',id:membership.id,roles:membership.roles,...change};
  for(const peer of remaining)send(peer,message);
  const dropped=membership.roles.slice();applyDeparture(message);
  request('drop',{roles:dropped}).then(r=>update(r.state)).catch(()=>{});
}
function applyDeparture(message){
  if(nativeBusy){boundaryActions.push(()=>applyDeparture(message));return;}
  if(!membership||message.id!==membership.id||JSON.stringify(message.roles)!==JSON.stringify(membership.roles)||
     JSON.stringify(message.slots)!==JSON.stringify(message.roles.map(r=>state.slots[r])))throw Error('离线确认不匹配');
  queue.applyDeparture(message);
  for(const role of message.roles){
    const peer=peers.get(role);peer.offline=true;peer.ended=true;
    clearTimeout(peer.connectTimer);peer.channel?.close();peer.pc.close();
  }
  membership=null;lastNow=performance.now();accumulator=0;
  render();
  status(`${message.roles.map(label).join('、')}已离线，角色转为离线幽灵，其余玩家继续。`);
  wake();
}
async function request(action,data={}){
  const response=await fetch(`/api/${action}`,{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({protocol:PROTOCOL,transport:useRelay?'ws':'rtc',room:session?.room,token:session?.token,runId:state?.runId??0,...data}),signal:AbortSignal.timeout(10000)});
  const result=await response.json();if(!response.ok)throw Object.assign(Error(result.error||`HTTP ${response.status}`),{status:response.status});return result;
}
function signal(peer,message){if(stopped||peers.get(peer.role)!==peer)return Promise.resolve();return request('signal',{to:peer.role,message:{...message,attempt:peer.attempt||0}});}
function send(peer,message){
  if(!peer||peer.offline)return;
  if(peer.channel?.readyState!=='open'){suspect(peer);return;}
  if(peer.channel.bufferedAmount>262144){suspect(peer);return;}
  try{peer.channel.send(JSON.stringify({...message,protocol:PROTOCOL,generation:state.generation,runId:state.runId}));}
  catch{suspect(peer);}
}
function broadcast(message){for(const peer of onlinePeers())send(peer,message);}
function pingPeer(peer){
  const id=++peer.pingId;peer.pings.set(id,{at:performance.now(),startup:!!peer.timingStarted&&!running});trim(peer.pings);
  send(peer,{type:'ping',id});
}
function recordPong(peer,id){
  if(!peer.pings.has(id))return;
  const ping=peer.pings.get(id);peer.rtt=performance.now()-ping.at;peer.rttAt=performance.now();peer.pings.delete(id);
  if(ping.startup&&(peer.timingSamples?.length||0)<3){
    (peer.timingSamples??=[]).push(peer.rtt);
    if(peer.timingSamples.length<3)pingPeer(peer);
    maybeRun();
  }
}
function clearInput(){keys.clear();childBits=0;pendingAction=0;bits=0;inputLatch.clear();player.reset();}
function clearTimers(){
  player.setActive(false);player.exit();
  clearTimeout(pollTimer);clearTimeout(loadTimer);clearInterval(heartbeatTimer);
  cancelWake();raf?.();raf=null;
  for(const peer of peers.values())clearTimeout(peer.connectTimer);
}
function disconnect(){
  if(relay)relay.close();relay=null;
  for(const peer of peers.values()){
    if(peer.channel)peer.channel.onopen=peer.channel.onclose=peer.channel.onerror=peer.channel.onmessage=null;
    if(peer.pc)peer.pc.onconnectionstatechange=peer.pc.ondatachannel=peer.pc.onicecandidate=null;
    peer.channel?.close();peer.pc?.close();
  }
}
function fail(error){
  if(stopped)return;
  if(session&&state?.phase==='loading'){void returnToRoom('本局已停止：'+(error.message||error));return;}
  stopReason=String(error.message||error);startupFailure(stopReason);replayRecorder.finish(stopReason);
  stopped=true;running=false;clearInput();
  for(const peer of peers.values())if(peer.channel?.readyState==='open'){
    try{send(peer,{type:'fatal',reason:String(error.message||error).slice(0,600)});}catch{}
  }
  runtime?.stop();clearTimers();disconnect();render();
  status(`本局已停止：${error.message||error}。点击“返回房间”后可重新准备。`);
}
function closeFinishedRoom(){} // End acknowledgements never release room membership.
function finishGame(){void returnToRoom('本局已结束，返回房间后可重新准备。');}
async function returnToRoom(reason='本局已结束，可重新准备。',snapshot=null){
  if(returningTask)return returningTask;
  const ownedRun=state?.runId,ownedSession=session;
  if(!ownedSession)return;
  stopped=true;running=false;busy=true;runEpoch++;clearInput();clearTimers();
  replayRecorder.finish(reason);runtime?.stop();disconnect();
  iframe?.remove();iframe=null;runtime=null;
  returningTask=(async()=>{
    try{
      await replayRecorder.seal(2000);
      const result=snapshot?{state:snapshot}:await request('end',{runId:ownedRun});
      if(session!==ownedSession)return;
      peers.clear();hashes.clear();pendingHashes.clear();captureTimes.clear();
      boundaryActions.length=0;nativeBusy=false;pumping=false;
      queue=null;localReady=null;preparing=runRequested=false;
      connectionAttempt++;connectionReadySent=fallbackPending=false;
      waitedMs=measuredWaitMs=waitPercent=0;measuredGame=gameHz=null;performanceHistory.length=0;
      connectionConfigPending=false;pendingConnectionSignals=[];
      membership=null;membershipId=0;waitingSince=replayUntil=0;replayTouchContext=null;
      rollbackCount=resimulated=maxRewind=0;receivedInputs=earlyInputs=0;
      lastNow=accumulator=lastUi=lastMeasure=measureFrame=0;
      stepCost=stepCount=maxCost=hz=averageCost=peakCost=inputAge=lastApplied=0;
      costs=newCosts();measuredCosts=newCosts();waitDetail='';stopReason='';
      bootStep=0;bootStage='等待全员准备';pollFailedAt=0;roomServiceError='';
      stopped=false;busy=false;state=result.state;
      render();status(reason);clearTimeout(pollTimer);pollTimer=setTimeout(poll,0);
    }catch(error){
      status('返回房间失败：'+error.message+'。可点击“返回房间”重试。');
    }finally{returningTask=null;busy=false;render();}
  })();
  return returningTask;
}
const returnButton=document.createElement('button');returnButton.type='button';
returnButton.id='return-room';returnButton.className='room-ui-return';returnButton.textContent='返回房间';
returnButton.onclick=()=>{if(state?.phase!=='lobby')void returnToRoom();};
$('game').append(returnButton);
function render(){
  publishConnectionStatus();
  if(!session||!state)return;
  $('lobby').dataset.roomPhase=state.phase;
  returnButton.hidden=state.phase==='lobby';returnButton.disabled=!!returningTask;
  $('leave').textContent=session.role==='host'?'解散房间':'离开房间';
  $('lobby').dataset.roomLanguage=state.settings.language;
  if(state.phase!=='lobby')$('lobby').dispatchEvent(new Event('room-preview-stop'));
  if(state.phase==='lobby')prepareRoomResources(state).catch(()=>{});
  const slot=state.slots[session.role],lobby=state.phase==='lobby'&&!stopped,active=activeRoles();
  $('entry').hidden=true;$('lobby').hidden=false;$('transport').disabled=true;
  $('room-info').textContent=`房间 ${session.room} · ${state.settings.players} 人模式 · ${state.settings.language==='jp'?'日文版':'汉化版'} · 你是${label(session.role)} · ${slot===null?'尚未选座':`P${slot+1}`}`;
  $('seats').textContent=Array.from({length:state.settings.players},(_,n)=>{
    const role=active.find(r=>state.slots[r]===n);return `P${n+1}：${role?`${label(role)}${peers.get(role)?.offline?'（已离线）':state.ready[role]?'（已准备）':''}`:'空位'}`;
  }).join(' / ');
  for(let n=0;n<3;n++){
    $(`seat${n}`).hidden=n>=state.settings.players;
    $(`seat${n}`).disabled=!lobby||busy||active.some(r=>r!==session.role&&state.slots[r]===n);
    $(`seat${n}`).textContent=slot===n?`已选择 P${n+1}`:`选择 P${n+1}`;
    $(`seat${n}`).setAttribute('aria-pressed',String(slot===n));
  }
  $('settings').disabled=!lobby||busy||session.role!=='host';
  for(const key of [...settingNames,'language'])$(key).value=state.settings[key];
  if(!busy)for(const key of assistFlags)$(key).checked=state.settings[key];
  $('launch-summary').textContent=`${state.settings.stage===5?'Extra（固定 Extra 难度）':`第 ${state.settings.stage+1} 关 · ${['Easy','Normal','Hard','Lunatic'][state.settings.difficulty]}`} · 每人 ${state.settings.lives} 残机 / ${state.settings.bombs} Bomb · 机体 ${Array.from({length:state.settings.players},(_,i)=>`P${i+1} ${['高速','平衡','低速'][state.settings[`p${i+1}`]]}`).join(' / ')} · 低速${state.settings.focusEnabled?` ${state.settings.focusPercent}%`:'关闭'} · ${state.settings.teamTargeting?'敌人瞄准全队':'敌人仅瞄准 P1'}`;
  if(!busy)$('rollback').checked=state.settings.rollback;
  const loadoutSlot=slot===null?null:`p${slot+1}`;
  $('loadout-panel').hidden=slot===null||!lobby;
  if(loadoutSlot){
    const selected=state.settings[loadoutSlot];
    for(const card of document.querySelectorAll('[data-loadout]')){
      const active=Number(card.dataset.loadout)===selected;
      card.setAttribute('aria-pressed',String(active));card.disabled=!lobby||busy;
    }
    $('loadout-status').textContent=`当前座位 P${slot+1} · ${['高速机体','平衡机体','低速机体'][selected]}`;
  }
  $('ready').disabled=!lobby||busy||slot===null;
  $('ready').textContent=state.ready[session.role]?'取消准备':'准备';
  $('start').hidden=session.role!=='host';
  $('start').disabled=!lobby||busy||!active.every(role=>state.present[role]&&state.ready[role]);
}
async function prepareRoomResources(value){
 const key=[value.settings.language,value.settings.stage,value.settings.rollback,bgmVersion()].join(':');
 if(key===preloadKey&&preloadPromise)return preloadPromise;
  if(performance.now()<preloadRetryAt)return;
 const generation=++preloadGeneration;preloadKey=key;
 const report=text=>{if(generation===preloadGeneration&&state?.phase==='lobby'&&!stopped)$('resource-status').textContent=text;};
 preloadPromise=(async()=>{
  report('正在预载游戏磁盘、DOSBox-X、音效和开场音乐…');
  const [patch,manifest,sfxManifest,bgmManifest,alternate,midi]=await Promise.all([
   freshJSON('local-patch.json'),freshJSON('netplay/runtime.json'),freshJSON('sfx/manifest.json'),
   freshJSON('bgm/manifest.json'),freshJSON('bgm/akyu/manifest.json'),loadMidiTracks()]);
  const disk=patch.disks[value.settings.language];if(!disk)throw Error('找不到所选语言的游戏磁盘');
  const supports=WebAssembly.validate(new Uint8Array([0,97,115,109,1,0,0,0,5,5,2,0,1,0,1]));
  const core=!!value.settings.rollback&&supports?'tracked':'plain',coreEntry=manifest.cores[core];
  const track=resolveBgmTrack(`stage${value.settings.stage}-m86`,bgmManifest.tracks,alternate.tracks,midi);
  await Promise.all([
   parallelAssets(Object.entries(manifest.assets),async([path,hash])=>assetBytes(path,{hash})),
   assetBytes(disk.file,{hash:disk.sha256,size:disk.size,gzip:true}),
   assetBytes(coreEntry.file,{hash:coreEntry.sha256}),
   parallelAssets(Object.entries(sfxManifest.effects),async([,item])=>assetBytes(`sfx/${item.file}`,{hash:item.sha256})),
   track&&assetBytes(`bgm/${track.url}`,{hash:track.audioSha256||track.sha256,size:track.bytes})
  ]);
  if(generation===preloadGeneration)report('游戏磁盘、核心、音效和当前关卡音乐已校验并预载');
 })();
 preloadPromise.catch(error=>{if(generation===preloadGeneration){report(`预载失败：${error.message}，稍后重试`);preloadKey='';preloadPromise=null;preloadRetryAt=performance.now()+5000;}});
 return preloadPromise;
}
function update(value){
  if(returningTask)return;
  if(state&&value?.revision<=state.revision)return;
  if(value?.phase==='lobby'&&state?.phase==='loading'){void returnToRoom('本局已结束，可重新准备。',value);return;}
  if(stopped)return;
  if(!value||value.protocol!==PROTOCOL||typeof value.settings?.rollback!=='boolean'||assistFlags.some(key=>typeof value.settings?.[key]!=='boolean')||value.settings?.focusPercent!==50||'cpuMultiplier' in value.settings||!['cn','jp'].includes(value.settings?.language)||!['ws','rtc'].includes(value.transport)||typeof value.connectionCommitted!=='boolean')throw Error('请关闭旧服务窗口，重新运行新版 start-lan.bat（公网使用 start-public.bat），并刷新所有玩家的页面');
  if(state&&value.revision<=state.revision)return;
  const changed=state&&value.transport!==state.transport;
  if(changed&&running)throw Error('游戏运行期间连接方式发生变化，请重新建房');
  state=value;useRelay=state.transport==='ws';mode.transport=state.transport;
  $('transport').value=state.transport;history.replaceState(null,'',networkUrl(mode));render();
  if(changed&&preparing){
    status('设备直连未连通，正在自动改用服务器转发；保留房间、座位和已加载的游戏…');
    setupConnections().catch(fail);
  }
  if(state.phase==='ended'){closeDisconnected('房主已离开');return;}
  if(running)for(const peer of onlinePeers())if(!state.present[peer.role])suspect(peer);
  if(state.phase==='loading'&&!preparing){
    preparing=true;
    // The same poll may include offers before asynchronous preloading resumes.
    connectionConfigPending=true;
    // Every browser enters the same TH06-style immersive layout. Only the
    // host can usually satisfy the browser's user-activation requirement for
    // native fullscreen; guests still lose the old top/bottom chrome.
    player.enter({native:true}).catch(()=>{});
    const ownedEpoch=runEpoch;beginGame().catch(error=>{if(ownedEpoch===runEpoch)fail(error);});
  }
  if(preparing)maybeRun();
}
async function action(name,data={}){
  if(busy)return;busy=true;render();
  try{const result=await request(name,data);if(result.state)update(result.state);}
  catch(error){status(error.message);}finally{busy=false;render();}
}
async function enter(role){
  if(busy||session)return;busy=true;$('create').disabled=true;$('join').disabled=true;
  try{
    if(role!=='host'&&!/^[0-9]{4}$/.test($('room').value.trim()))throw Error('请输入四位数字房间号');
    const result=await request(role==='host'?'create':'join',{room:$('room').value.trim().toUpperCase(),...(role==='host'?{rollback:defaultRollback}:{})});
    if(result.protocol!==PROTOCOL)throw Error('请重新启动新版 start-lan.bat');
    session={room:result.room,token:result.token,role:result.role};update(result.state);
    status('房主选择双人或三人模式；所有玩家分别选座并准备后开始。');poll();
  }catch(error){status(error.message);}finally{busy=false;$('create').disabled=false;$('join').disabled=false;render();}
}
async function poll(){
  if(stopped)return;
  let result;
  try{
    const q=new URLSearchParams({room:session.room,token:session.token,runId:state?.runId??0});
    if(preparing&&!running){q.set('boot_stage',bootStage);q.set('boot_step',String(bootStep));}
    const response=await fetch(`/api/events?${q}`,{signal:AbortSignal.timeout(10000)});
    result=await response.json();if(!response.ok)throw Error(result.error||`HTTP ${response.status}`);
    pollFailedAt=0;roomServiceError='';
  }catch(error){
    if(stopped)return;
    pollFailedAt||=performance.now();roomServiceError=String(error.message||error).slice(0,300);
    // Once started, gameplay membership is governed by authenticated player
    // channels and their watchdog. HTTP signaling loss does not close the game.
    // Loading uses its resource deadline; a slow CPU must not pay a 5 s grace.
    if(!running&&!preparing&&performance.now()-pollFailedAt>=30000){closeDisconnected('房间服务连接失败：'+roomServiceError);return;}
    if(!running)status('房间服务暂时无响应，正在重试：'+roomServiceError);
    pollTimer=setTimeout(poll,1000);return;
  }
  try{
    if(returningTask)return;
    if(result.state?.revision<state?.revision){if(!stopped)pollTimer=setTimeout(poll,400);return;}
    update(result.state);
    for(const [role,progress]of Object.entries(result.startup||{})){
      const peer=peers.get(role);if(!peer)continue;
      peer.bootStage=progress.stage;peer.bootStep=progress.step;
      peer.bootReportedAt=performance.now()-progress.age*1000;
    }
    showStartup();
    // HTTP polling must not wait for browser ICE/mDNS operations. Preserve
    // description ordering per peer, while candidate resolution is separate.
    for(const event of result.events)if(!stopped)dispatchSignal(event);
    if(!stopped)pollTimer=setTimeout(poll,400);
  }catch(error){
    if(stopped)return;
    fail(error);
  }
}
function trim(map){while(map.size>60)map.delete(map.keys().next().value);}
function compare(peer,tick){
  if(peer.offline||membership)return;
  const a=hashes.get(tick),b=peer.hashes.get(tick);if(!a||!b)return;
  if(a.hash!==b.hash||a.gameFrame!==b.gameFrame)throw Error(`${label(peer.role)}在同步步 ${tick} 不一致：本机 ${a.hash}/${a.gameFrame}，对方 ${b.hash}/${b.gameFrame}`);
  peer.confirmed=tick;peer.hashes.delete(tick);
}
function attachChannel(peer,channel){
  if(channel.label!=='th02-inputs')throw Error('未知连接通道');
  if(peer.channel)throw Error('重复操作通道');peer.channel=channel;
  channel.onopen=()=>{
    if(stopped||peers.get(peer.role)!==peer){channel.close();return;}
    clearTimeout(peer.connectTimer);peer.lastPacket=performance.now();
    try{
      send(peer,{type:'hello',settings:state.settings,slots:state.slots,inputPolicy});pingPeer(peer);
      if(localReady)send(peer,{type:'ready',build:localReady});
      if(running)send(peer,{type:'input-tail',slot:queue.slot,inputs:[...queue.inputs[queue.slot]].filter(([at])=>at>=queue.frame-32)});
    }
    catch(error){fail(error);}
  };
  channel.onmessage=event=>{
    if(peers.get(peer.role)!==peer)return;
    try{
      if(peer.offline||peers.get(peer.role)!==peer)return;
      if(typeof event.data!=='string'||event.data.length>65536)throw Error('无效同步消息');
      const m=JSON.parse(event.data);
      if(m.protocol!==PROTOCOL||m.generation!==state.generation||m.runId!==state.runId)throw Error('同步消息不属于本局');
      const recovering=peer.recovering;
      peer.lastPacket=performance.now();peer.suspectAt=null;peer.recovering=false;
      if(recovering&&running&&onlinePeers().every(p=>!p.recovering)){
        lastNow=performance.now();accumulator=0;status(`连接已恢复，继续本局，你操作 P${queue.slot+1}。`);
      }
      if(stopped&&m.type!=='end'&&m.type!=='end-ack')return;
      switch(m.type){
        case 'hello':
          if(m.inputPolicy?.policySchema!==2)throw Error('输入策略版本不同，请所有玩家刷新后重新建房');
          if(JSON.stringify(m.settings)!==JSON.stringify(state.settings)||JSON.stringify(m.slots)!==JSON.stringify(state.slots))throw Error('开局设置不一致');
          if(m.inputPolicy?.inputDelay!==inputPolicy.inputDelay||m.inputPolicy?.directionPrediction!==inputPolicy.directionPrediction||m.inputPolicy?.rollbackEnabled!==inputPolicy.rollbackEnabled)throw Error('联机性能策略不一致，请全员刷新并使用相同网络模式');
          peer.hello=true;maybeRun();break;
        case 'ready':peer.ready=m.build;maybeRun();break;
        case 'timing':
          if(!Number.isFinite(m.rtt)||m.rtt<0||m.rtt>30000)throw Error('无效网络测量');
          if(!Number.isFinite(m.rollbackRtt)||m.rollbackRtt<0||m.rollbackRtt>30000)throw Error('无效回滚延迟测量');
          peer.startRtt=m.rtt;peer.startRollbackRtt=m.rollbackRtt;maybeRun();break;
        case 'run':
          if(peer.role!=='host'||session.role==='host')throw Error('无效开局通知');
          if(runRequested)throw Error('重复开局通知');
          if(!Number.isInteger(m.delay)||m.delay<BOOT_DELAY||m.delay>MAX_INPUT_DELAY)throw Error('无效开局输入缓冲');
          if(!Number.isInteger(m.inputDelay)||m.inputDelay<0||m.inputDelay>MAX_ROLLBACK_INPUT_DELAY||
             m.inputDelay>m.delay||(!inputPolicy.rollbackEnabled&&m.inputDelay!==0))throw Error('无效回滚输入策略');
          if(!validRollbackPolicy(m.policy)||m.policy.rollbackEnabled!==inputPolicy.rollbackEnabled||
             m.policy.inputDelay!==m.inputDelay||m.policy.rttMs<Math.ceil(Math.max(...[...peers.values()].map(p=>rollbackRttForSamples(p.timingSamples)))))throw Error('无效混合输入策略');
          runDelay=m.delay;runInputDelay=m.inputDelay;runPolicy=m.policy;
          runRequested=true;maybeRun();break;
        case 'input':
          if(membership?.roles.includes(peer.role))break;
          if(m.slot!==state.slots[peer.role])throw Error('玩家输入身份不匹配');
          queue.receive(m.frame,m.buttons,m.slot);receivedInputs++;wake();break;
        case 'input-tail':
          if(membership?.roles.includes(peer.role))break;
          if(m.slot!==state.slots[peer.role]||!Array.isArray(m.inputs)||m.inputs.length>160)throw Error('无效输入补包');
          for(const [frame,buttons]of m.inputs)queue.receive(frame,buttons,m.slot);
          wake();break;
        case 'rollback':
          if(peer.role!=='host'||session.role==='host')throw Error('无效回滚启用通知');
          queue.arm(m.frame);break;
        case 'hash':
          if(!Number.isSafeInteger(m.tick)||m.tick<0||m.tick>queue.frame+120||typeof m.hash!=='string'||m.hash.length>16||!Number.isInteger(m.gameFrame))throw Error('无效状态摘要');
          peer.hashes.set(m.tick,m);compare(peer,m.tick);trim(peer.hashes);
          if(m.boot===true)peer.rollbackReady=true;armRollback();break;
        case 'heartbeat':
          peer.hidden=!!m.hidden;
          if(typeof m.bootStage==='string')peer.bootStage=m.bootStage.slice(0,300);
          showStartup();break;
        case 'ping':if(Number.isSafeInteger(m.id))send(peer,{type:'pong',id:m.id});break;
        case 'pong':recordPong(peer,m.id);break;
        case 'suspect':
          if(session.role!=='host'||!peers.has(m.role)||m.role==='host'||m.role===peer.role)throw Error('无效断链报告');
          if(!peers.get(m.role).offline&&!membership?.roles.includes(m.role))beginDeparture([m.role]);break;
        case 'departure-prepare':
          if(peer.role!=='host'||!Number.isSafeInteger(m.id)||m.id<=membershipId||!Array.isArray(m.roles)||
             !m.roles.length||new Set(m.roles).size!==m.roles.length||m.roles.some(r=>r==='host'||!activeRoles().includes(r)))throw Error('无效离线协商');
          membershipId=m.id;
          if(m.roles.includes(session.role)){closeDisconnected('你已与本局断开连接');return;}
          membership={id:m.id,roles:m.roles,started:performance.now()};
          atNativeBoundary(()=>send(peer,{type:'departure-report',id:m.id,report:queue.membershipReport()}));break;
        case 'departure-report':
          if(session.role!=='host'||!membership||m.id!==membership.id)break;
          if(!membership.roles.includes(peer.role)){membership.reports.set(peer.role,m.report);finishDeparture();}break;
        case 'departure-commit':
          if(peer.role!=='host')throw Error('只有房主能确认离线');applyDeparture(m);break;
        case 'end':
          if(peer.role!=='host'||session.role==='host')throw Error('只有房主能退出本局');
          if(stopped)send(peer,{type:'end-ack'});else finishGame();break;
        case 'end-ack':if(session.role!=='host')throw Error('无效退出确认');peer.ended=true;closeFinishedRoom();break;
        case 'fatal':
          if(!running){fail(Error(`${label(peer.role)}启动失败：${String(m.reason).slice(0,600)}`));return;}
          if(peer.role==='host'){closeDisconnected('房主已停止游戏');return;}
          suspect(peer);break;
        default:throw Error('未知同步消息');
      }
    }catch(error){fail(error);}
  };
  channel.onclose=()=>{if(peers.get(peer.role)!==peer)return;if(stopped||peer.offline)peer.pc.close();else suspect(peer);};
  channel.onerror=()=>{if(peers.get(peer.role)===peer)suspect(peer);};
  channel.onreconnecting=()=>{if(peers.get(peer.role)===peer)suspect(peer);};
}
function makePeer(role,rtcConfiguration,previous=null,attempt=0){
  const pc=new RTCPeerConnection(rtcConfiguration);
  const peer={...previous,role,pc,channel:null,attempt,rtcConfiguration,retryAt:performance.now()+8000,
    candidates:previous?.futureCandidates?.get(attempt)||[],futureCandidates:new Map(),localCandidateTypes:{},remoteCandidateTypes:{},iceServerErrors:[],
    hashes:previous?.hashes||new Map(),pings:new Map(),confirmed:previous?.confirmed||0,lastPacket:previous?.lastPacket??performance.now(),pingId:0};
  peers.set(role,peer);
  if(previous){clearTimeout(previous.connectTimer);previous.channel?.close();previous.pc.close();}
  peer.signalChain=Promise.resolve();
  pc.onicecandidate=event=>{if(event.candidate&&currentRtcPeer(peer)){peer.localCandidates=(peer.localCandidates||0)+1;countIceCandidate(peer.localCandidateTypes,event.candidate);signal(peer,{type:'ice',candidate:event.candidate.toJSON()}).catch(error=>rtcFailure(peer,error));showStartup();}};
  pc.onicecandidateerror=event=>{if(stopped)return;recordIceServerError(peer.iceServerErrors,event);showStartup();};
  pc.onicegatheringstatechange=showStartup;
  pc.onconnectionstatechange=()=>{
    if(!currentRtcPeer(peer))return;
    showStartup();
    if(pc.connectionState==='failed'&&!running){rtcFailure(peer,Error(`${label(role)}直连失败`));return;}
    if(['failed','disconnected'].includes(pc.connectionState))suspect(peer);
  };
  pc.oniceconnectionstatechange=showStartup;
  pc.onsignalingstatechange=showStartup;
  pc.ondatachannel=event=>{if(!currentRtcPeer(peer)){event.channel.close();return;}try{acceptInputChannel(pc,event.channel,'th02-inputs',channel=>attachChannel(peer,channel));}catch(error){rtcFailure(peer,error);}};
  peer.connectTimer=setTimeout(()=>rtcFailure(peer,Error(`与${label(role)}直连等待超过 8 秒（ICE ${pc.iceConnectionState}）`)),8000);return peer;
}
function makeRelayPeer(role){
  const channel=relay.channelFor(role);
  // Minimal lifecycle view for existing per-peer departure handling; not RTC.
  const pc={get connectionState(){return channel.readyState==='open'?'connected':channel.readyState==='closed'?'closed':'connecting';},
    iceConnectionState:'不适用',signalingState:'中继',close(){channel.close();}};
  const peer={role,pc,hashes:new Map(),pings:new Map(),confirmed:0,lastPacket:performance.now(),pingId:0,path:'WebSocket 中继',signalStage:'等待全员中继就绪'};
  peers.set(role,peer);attachChannel(peer,channel);
  peer.connectTimer=setTimeout(()=>fail(Error('等待全员服务器转发连接超过 30 秒，请确认所有玩家的页面仍打开，且房间服务／公网隧道窗口仍在运行')),30000);
}
function currentRtcPeer(peer){return !stopped&&!useRelay&&peers.get(peer.role)===peer;}
function rtcFailure(peer,error){
  if(!currentRtcPeer(peer))return;
  if(running){suspect(peer);peer.retryAt=performance.now()+1000;return;}
  requestRelayFallback(String(error.message||error));
}
function reconnectRtc(peer){
  if(!currentRtcPeer(peer)||!running||performance.now()<peer.retryAt)return;
  peer.retryAt=performance.now()+8000;
  // The lower role always offers. Both endpoints can request recovery without
  // colliding offers, resetting the game queue, or accepting stale ICE.
  if(roles.indexOf(session.role)>roles.indexOf(peer.role)){
    signal(peer,{type:'reconnect'}).catch(()=>{});return;
  }
  const replacement=makePeer(peer.role,peer.rtcConfiguration,peer,peer.attempt+1);
  attachChannel(replacement,createInputChannel(replacement.pc,'th02-inputs'));
  replacement.signalChain=signalingStep(replacement,'恢复设备直连',async()=>replacement.pc.setLocalDescription(await replacement.pc.createOffer()))
    .then(()=>{if(currentRtcPeer(replacement))return signal(replacement,{type:'offer',description:replacement.pc.localDescription.toJSON()});})
    .catch(error=>rtcFailure(replacement,error));
}
async function requestRelayFallback(reason){
  if(stopped||useRelay||fallbackPending)return;
  if(running){fail(Error(reason));return;}
  fallbackPending=true;
  status(`${reason}，正在为全房间切换服务器转发…`);
  try{
    const result=await request('fallback');if(stopped)return;
    if(returningTask||result.state?.revision<state?.revision)return;
    update(result.state);
    if(!useRelay)fail(Error(`${reason}；本局已完成连接确认，请重新建房`));
  }catch(error){if(!stopped&&!useRelay)fail(Error(`${reason}；请求服务器转发失败：${error.message||error}`));}
  finally{fallbackPending=false;}
}
async function setupConnections(){
  if(connectionAttempt)pendingConnectionSignals=[];
  const attempt=++connectionAttempt,relayMode=useRelay;
  connectionReadySent=false;runRequested=false;timingSent=false;runDelay=null;runInputDelay=null;connectionConfigPending=true;
  const oldPeers=[...peers.values()];peers.clear();
  for(const peer of oldPeers){clearTimeout(peer.connectTimer);peer.channel?.close();peer.pc.close();}
  relay?.close();relay=null;
  runPolicy=null;
  inputPolicy=Object.freeze({inputDelay:0,directionPrediction:6,policySchema:2,rollbackEnabled:state.settings.rollback});
  queue=new RollbackQueue(state.slots[session.role],state.settings.players,inputPolicy);
  let rtcConfiguration=null;
  if(!relayMode){
    try{
      if(!window.RTCPeerConnection)throw Error('浏览器不支持设备直连');
      rtcConfiguration=await loadRtcConfiguration(session);
    }catch(error){
      if(attempt===connectionAttempt&&!stopped)await requestRelayFallback(String(error.message||error));
      return;
    }
  }
  if(stopped||attempt!==connectionAttempt)return;
  connectionConfigSummary=relayMode?'服务器转发（不需要 STUN/TURN 账号）':describeRtcConfiguration(rtcConfiguration);
  if(relayMode){
    relay=createRelay({...session,runId:state.runId},state.generation,roles,fail);
    for(const role of activeRoles())if(role!==session.role)makeRelayPeer(role);
    relay.connect();
  }else{
    try{
      for(const role of activeRoles())if(role!==session.role)makePeer(role,rtcConfiguration);
      for(const peer of peers.values())if(roles.indexOf(session.role)<roles.indexOf(peer.role)){
        attachChannel(peer,createInputChannel(peer.pc,'th02-inputs'));
        peer.signalChain=signalingStep(peer,'生成连接邀请',async()=>peer.pc.setLocalDescription(await peer.pc.createOffer()))
          .then(async()=>{if(currentRtcPeer(peer)){await signal(peer,{type:'offer',description:peer.pc.localDescription.toJSON()});if(currentRtcPeer(peer)){peer.signalStage='已发送邀请，等待应答';showStartup();}}}).catch(error=>rtcFailure(peer,error));
      }
    }catch(error){await requestRelayFallback(String(error.message||error));return;}
  }
  connectionConfigPending=false;
  for(const event of pendingConnectionSignals.splice(0))dispatchSignal(event);
  showStartup();
}
async function signalingStep(peer,name,operation){
  peer.signalStage=name;showStartup();
  let timer;
  try{
    return await Promise.race([operation(),new Promise((_,reject)=>{
      timer=setTimeout(()=>reject(Error(`${label(peer.role)}：${name}超过 15 秒（ICE ${peer.pc.iceConnectionState}）`)),15000);
    })]);
  }finally{clearTimeout(timer);}
}
function submitCandidate(peer,candidate){
  // A single unresolved/unsupported candidate must not hold up the SDP answer
  // or other candidates. Overall connection timeout still bounds the attempt.
  let settled=false;
  const timer=setTimeout(()=>{if(!settled&&!stopped&&!peer.offline){peer.iceWarning='有候选解析超过 8 秒，继续尝试其他候选';showStartup();}},8000);
  Promise.resolve().then(()=>peer.pc.addIceCandidate(candidate)).then(()=>{
    peer.appliedCandidates=(peer.appliedCandidates||0)+1;
  }).catch(error=>{peer.iceWarning=`候选处理失败：${String(error.message||error).slice(0,120)}`;})
    .finally(()=>{settled=true;clearTimeout(timer);showStartup();});
}
function dispatchSignal(event){
  if(useRelay)return;
  // Other browsers may finish fetching TURN credentials before this browser.
  // Keep polling, but do not dispatch their offers until our peers exist.
  if(connectionConfigPending){
    if(pendingConnectionSignals.length>=256)throw Error('等待连接配置期间收到过多信令');
    pendingConnectionSignals.push(event);return;
  }
  const peer=peers.get(event?.from);
  if(!peer||!event.message)throw Error('未知信令来源');
  if(peer.offline)return;
  const attempt=event.message.attempt??0;
  if(!Number.isSafeInteger(attempt)||attempt<0||attempt>1000000)throw Error('无效连接恢复编号');
  if(event.message.type==='reconnect'){
    if(!running||roles.indexOf(session.role)>roles.indexOf(peer.role)||attempt<peer.attempt)return;
    peer.retryAt=Math.min(peer.retryAt,performance.now());suspect(peer);reconnectRtc(peer);return;
  }
  if(attempt<peer.attempt)return;
  if(event.message.type==='ice'){
    if(attempt>peer.attempt){
      if(!running||attempt>peer.attempt+1)throw Error('无效恢复候选编号');
      const candidates=peer.futureCandidates.get(attempt)||[];
      if(candidates.length>=256)throw Error('恢复候选过多');
      candidates.push(event.message.candidate);peer.futureCandidates.set(attempt,candidates);return;
    }
    peer.remoteCandidates=(peer.remoteCandidates||0)+1;
    countIceCandidate(peer.remoteCandidateTypes,event.message.candidate);
    if(peer.pc.remoteDescription)submitCandidate(peer,event.message.candidate);
    else peer.candidates.push(event.message.candidate);
    showStartup();return;
  }
  if(event.message.type==='offer'&&attempt>peer.attempt){
    if(!running||roles.indexOf(event.from)>=roles.indexOf(session.role))throw Error('无效连接恢复邀请');
    const replacement=makePeer(peer.role,peer.rtcConfiguration,peer,attempt);
    replacement.signalChain=receivedSignal(event).catch(error=>rtcFailure(replacement,error));return;
  }
  if(attempt!==peer.attempt)return;
  peer.signalChain=peer.signalChain.then(()=>{if(currentRtcPeer(peer)&&!peer.offline)return receivedSignal(event);}).catch(error=>rtcFailure(peer,error));
}
async function remoteDescription(peer,description){
  await signalingStep(peer,'应用远端连接描述',()=>peer.pc.setRemoteDescription(description));
  for(const candidate of peer.candidates.splice(0))submitCandidate(peer,candidate);
}
async function receivedSignal({from,message}){
  const peer=peers.get(from);if(!peer||!message)throw Error('未知信令来源');
  if(peer.offline)return;
  if(message.type==='offer'){
    if(roles.indexOf(from)>=roles.indexOf(session.role)||peer.pc.remoteDescription)throw Error('重复或无效连接请求');
    await remoteDescription(peer,message.description);
    await signalingStep(peer,'生成连接应答',async()=>peer.pc.setLocalDescription(await peer.pc.createAnswer()));
    if(!currentRtcPeer(peer))return;
    await signal(peer,{type:'answer',description:peer.pc.localDescription.toJSON()});
    peer.signalStage='已发送应答，等待连接';showStartup();
  }else if(message.type==='answer'){
    if(roles.indexOf(from)<=roles.indexOf(session.role)||peer.pc.remoteDescription)throw Error('无效连接响应');
    await remoteDescription(peer,message.description);
    peer.signalStage='已接收应答，等待连接';showStartup();
  }else throw Error('未知信令');
}
async function beginGame(){
  const ownedEpoch=runEpoch;
  await prepareRoomResources(state).catch(()=>{});if(stopped||ownedEpoch!==runEpoch)return;
  bootStartedAt=performance.now();
  bootStage='获取网络连接配置';showStartup();
  await setupConnections();
  if(stopped||ownedEpoch!==runEpoch)return;
  heartbeatTimer=setInterval(()=>{
    try{
      const now=performance.now();
      showStartup();
      for(const peer of onlinePeers()){
        if(!useRelay)void refreshRtcPath(peer);
        // Connecting/loading have their own 30 s / 10 min deadlines. A mobile
        // browser compiling WASM must not be evicted by the in-game watchdog.
        if(running&&now-peer.lastPacket>3000)suspect(peer);
        if(running&&!useRelay&&peer.recovering)reconnectRtc(peer);
        if(running&&peer.suspectAt!=null&&now-peer.suspectAt>=GRACE_MS){
          if(peer.role==='host'){closeDisconnected('主机连接已断开');return;}
          if(!membership?.roles.includes(peer.role))reportDeparture(peer.role);
        }
        if(peer.channel?.readyState==='open'){
          pingPeer(peer);send(peer,{type:'heartbeat',hidden:document.hidden,bootStage:running?undefined:bootStage});
          send(peer,{type:'input-tail',slot:queue.slot,inputs:[...queue.inputs[queue.slot]].filter(([at])=>at>=queue.frame-32)});
        }
      }
      if(running&&membership&&now-membership.started>=GRACE_MS){
        if(session.role==='host')beginDeparture(onlinePeers().filter(p=>!membership.roles.includes(p.role)&&!membership.reports.has(p.role)).map(p=>p.role));
        else if(now-membership.started>=15000)closeDisconnected('房主未完成离线同步');
      }
    }catch(error){fail(error);}
  },1000);
  status('所有玩家正在各自加载游戏并建立网络连接…');
  loadTimer=setTimeout(()=>fail(Error(`等待全员资源就绪超过 10 分钟；本机：${bootStage}。请查看下方各端加载状态`)),600000);
  bootStage='加载游戏页面与 JavaScript 模块';showStartup();
  iframe=document.createElement('iframe');iframe.title='本机游戏模拟器';iframe.allow='autoplay; gamepad; fullscreen';iframe.allowFullscreen=true;
  const loaded=new Promise((resolve,reject)=>{iframe.onload=resolve;iframe.onerror=()=>reject(Error('游戏页面加载失败'));});
  iframe.src='netplay/runtime.html';player.stage.append(iframe);await loaded;if(stopped||ownedEpoch!==runEpoch)return;
  if(!iframe.contentWindow.th02RuntimeReady)throw Error('游戏页面版本不匹配，请刷新所有页面');
  if(!await iframe.contentWindow.th02RuntimeReady)throw Error('游戏模块加载失败，详情见游戏页面');
  if(stopped||ownedEpoch!==runEpoch)return;
  runtime=iframe.contentWindow.th02Sync;if(!runtime)throw Error('游戏模块没有就绪');
  runtime.setMusicVolume(Number(musicVolume.value)/100);
  runtime.configure(state.slots.host,state.slots[session.role],state.settings.players);
  runtime.setRawPresentation(rawEnabled);
  // Pointer controls live in the parent; desktop keyboards still focus the
  // runtime canvas. Reset when crossing either document's lifecycle boundary.
  iframe.contentWindow.addEventListener('blur',()=>player.reset());
  player.setLabel(`你操作 P${state.slots[session.role]+1}`);
  runtime.updateControls(controlSnapshot(),controlEditor.isEditing());
  runtime.onInput(value=>{childBits=value;observeInput();wake();});runtime.onMenuAction(value=>{pendingAction|=value;observeInput();wake();});
  // Extra uses its native rank selected by stage; the launcher rank byte is 0–3.
  localReady=await runtime.load({...state.settings,difficulty:state.settings.difficulty===4?1:state.settings.difficulty});if(stopped||ownedEpoch!==runEpoch)return;
  await replayRecorder.boot({settings:{...state.settings,difficulty:state.settings.difficulty===4?1:state.settings.difficulty},players:state.settings.players,hostSlot:state.slots.host,opening:localReady});
  if(stopped||ownedEpoch!==runEpoch)return;
  if(session.role==='host')replayRecorder.connectNetwork({room:session.room,token:session.token,generation:state.generation,runId:state.runId});
  for(const peer of peers.values())if(peer.channel?.readyState==='open')send(peer,{type:'ready',build:localReady});maybeRun();
}
function maybeRun(){
  showStartup();
  if(running||stopped||!localReady||![...peers.values()].every(peer=>peer.channel?.readyState==='open'&&peer.hello&&peer.ready))return;
  if([...peers.values()].some(peer=>JSON.stringify(peer.ready)!==JSON.stringify(localReady)))throw Error('游戏资源版本不一致，请所有玩家刷新页面');
  // Boot can block the browser for far longer than the actual network RTT.
  // Start the three policy samples only after everyone's native stage is ready;
  // loading pings remain diagnostic and never set the match's fixed delay.
  for(const peer of peers.values())if(!peer.timingStarted){peer.timingStarted=true;peer.pings.clear();peer.timingSamples=[];pingPeer(peer);}
  if(![...peers.values()].every(peer=>peer.timingSamples?.length===3))return;
  if(!timingSent){
    timingSent=true;
    broadcast({type:'timing',rtt:Math.max(...[...peers.values()].flatMap(peer=>peer.timingSamples)),rollbackRtt:Math.max(...[...peers.values()].map(peer=>rollbackRttForSamples(peer.timingSamples)))});
  }
  // All browsers acknowledge the same route before the host can send run.
  // An atomic server decision prevents a 3P partial mesh from splitting into
  // running RTC peers and fallback WS peers during a timeout race.
  if(!state.connectionCommitted){
    if(!connectionReadySent){
      connectionReadySent=true;const attempt=connectionAttempt;
      request('connected').then(result=>{if(!stopped&&attempt===connectionAttempt)update(result.state);})
        .catch(error=>{if(!stopped&&attempt===connectionAttempt){if(useRelay)fail(error);else requestRelayFallback(String(error.message||error));}});
    }
    return;
  }
  if(session.role==='host'){
    if(![...peers.values()].every(peer=>Number.isFinite(peer.startRtt)))return;
    const rtt=Math.max(...[...peers.values()].flatMap(peer=>[peer.startRtt,...peer.timingSamples]));
    const rollbackRtt=Math.max(...[...peers.values()].flatMap(peer=>[peer.startRollbackRtt,rollbackRttForSamples(peer.timingSamples)]));
    runPolicy=rollbackPolicyForRtt(rollbackRtt,inputPolicy.rollbackEnabled);
    runInputDelay=runPolicy.inputDelay;
    runDelay=inputPolicy.rollbackEnabled?Math.max(BOOT_DELAY,runInputDelay):lockstepDelayForRtt(rtt);
    broadcast({type:'run',delay:runDelay,inputDelay:runInputDelay,policy:runPolicy});runRequested=true;
  }
  if(!runRequested)return;
  queue.setLockstepDelay(runDelay);queue.setInputDelay(runInputDelay);
  queue.setPredictionLead(runPolicy.predictionLead);
  inputPolicy=Object.freeze({...inputPolicy,...runPolicy});
  if(inputPolicy.rollbackEnabled)queue.arm(0);
  clearTimeout(loadTimer);runtime.start();running=true;player.setActive(true);
  for(const peer of onlinePeers()){peer.lastPacket=performance.now();peer.suspectAt=null;}
  lastNow=lastMeasure=performance.now();measureFrame=queue.frame;accumulator=0;
  status(`${state.settings.players} 人已同步入关，你操作 P${queue.slot+1}。方向键移动，Z 射击，X Bomb，Shift 低速。`);raf=startFrameLoop(tick);
  showStartup();
}
function cancelWake(){
  clearTimeout(wakeTimer);wakeTimer=null;wakePending=false;wakeToken++;
}
function wake(delay=0){
  if(!running||stopped)return;
  delay=Math.max(0,delay);
  const deadline=performance.now()+delay;if(wakePending&&wakeAt<=deadline)return;
  cancelWake();wakeAt=deadline;wakePending=true;
  const token=wakeToken;
  // Replay still yields after its bounded work slice, but does not pay the
  // nested setTimeout minimum on every expensive emulator step.
  if(delay===0)wakeChannel.port2.postMessage(token);
  else wakeTimer=setTimeout(()=>{
    if(token!==wakeToken||!wakePending)return;
    wakePending=false;wakeTimer=null;pump(performance.now());
  },delay);
}
function tick(now){if(running&&!stopped){observeInput();pump(now);}}
function armRollback(){
  if(!inputPolicy.rollbackEnabled||session.role!=='host'||!running||membership||queue.activation!==null||!runtime.readyForRollback()||
     !onlinePeers().every(peer=>peer.rollbackReady))return;
  // DOS boot remains confirmed-only. All devices/audio are initialized before
  // taking snapshots; every peer switches at the same future simulation tick.
  const frame=queue.frame+120;queue.arm(frame);broadcast({type:'rollback',frame});
}
async function confirmFrames(){
  const ownedEpoch=runEpoch;
  const before=performance.now();
  const prefix=Math.min(queue.frame,queue.confirmed);
  const outcome=runtime.confirmQueued?undefined:await runtime.confirm(prefix);
  if(stopped||ownedEpoch!==runEpoch)return;
  runtime.confirmQueued?.(prefix,()=>{if(!stopped&&ownedEpoch===runEpoch)finishGame();});
  replayRecorder.confirm(prefix,queue.dirty!==null);
  if(outcome==='exit'){finishGame();return;}
  if(stopped)return;
  for(const [tick,sample]of pendingHashes)if(tick<=prefix){
    hashes.set(tick,sample);broadcast({type:'hash',...sample});
    for(const peer of onlinePeers())compare(peer,tick);
    pendingHashes.delete(tick);trim(hashes);
  }
  // Receipt may advance confirmed while the Worker acknowledges an older
  // prefix. Preserve the newer history until its corrections are simulated.
  queue.prune(prefix);armRollback();
  costs.confirm+=performance.now()-before;
}
async function simulate(pair,replay){
  const ownedEpoch=runEpoch;
  const before=performance.now();
  const frame=queue.frame;
  const capture=queue.active&&frame>=queue.confirmed;
  if(capture&&!runtime.asyncExecution){
    const at=performance.now();runtime.capture(frame);costs.capture+=performance.now()-at;costs.captures++;
  }
  const stepAt=performance.now();
  nativeBusy=true;
  if(runtime.asyncExecution)await runtime.execute(pair,{frame,replay,offlineMask:queue.offlineMask(frame),capture,hash:(frame+1)%120===0});
  else runtime.step(pair,{frame,replay,offlineMask:queue.offlineMask(frame)});
  if(stopped||ownedEpoch!==runEpoch)return;
  replayRecorder.stage(frame,{inputs:pair,offlineMask:queue.offlineMask(frame),proof:runtime.replayProof(),hash:(frame+1)%120===0?runtime.checksum():null});
  queue.commit(pair);
  // Inputs can arrive while a Worker owns this step. Include its actual
  // completed inputs in the history before checking every changed prediction.
  if(runtime.asyncExecution&&queue.active)queue.refreshCorrection(true);
  completeNativeBoundary();
  costs.simulate+=performance.now()-stepAt;costs.steps++;
  costs.lead=Math.max(costs.lead,queue.frame-queue.confirmed);
  if(queue.frame%120===0){
    const at=performance.now();pendingHashes.set(queue.frame,{...runtime.checksum(),boot:runtime.readyForRollback()});costs.hash+=performance.now()-at;
  }
  const cost=performance.now()-before;stepCost+=cost;stepCount++;maxCost=Math.max(maxCost,cost);
}
function observeInput(){
  if(!running||document.hidden)return;
  inputLatch.observe(controlEditor.isEditing()?0:normalize(player.sample(keyboardBits(keys)|childBits|gamepadBits())|pendingAction));
}
function captureLocalInput(frontier=queue.frame,context=null){
  // Continue authoritative synchronization while hidden, with neutral input.
  // Do not poll held gamepads, touch auto-fire or menu pulses in the background.
  let packed=0;
  if(!document.hidden){
    player.setContext(context??runtime.touchContext());
    runtime.setPointPreferences({focus:!!focusSettings(),always:player.alwaysPoint()});
    observeInput();const value=normalize(inputLatch.sample());
    if(value!==bits){bits=value;inputChangedAt=performance.now();}
    packed=player.pack(bits|pendingAction,{includePointPreference:false});
  }else if(bits){bits=0;inputChangedAt=performance.now();}
  const captured=queue.capture(packed,frontier);
  if(!captured)return false;
  player.consume();inputLatch.consume();
  pendingAction=0;captureTimes.set(captured.frame,performance.now());broadcast(captured);return true;
}
function nextInputs(source=queue,report=source===queue){
  // Bound speculative work using the negotiated lead budget, independently
  // from direction persistence and the twelve-step restore safety window.
  // confirmed is a contiguous authoritative prefix; late packets wake pump.
  const leadLimit=Math.max(1,source.predictionLead);
  if(source.active&&source.frame>=source.confirmed+leadLimit){
    if(report)waitDetail=`等待对端输入 · 预测领先 ${source.frame-source.confirmed}/${leadLimit} 步`;
    return null;
  }
  const allowed=runtime.canPredict(),pair=source.peek(allowed);
  if(pair){if(report)waitDetail='';return pair;}
  if(!report)return null;
  const missing=source.inputs.flatMap((lane,slot)=>source.isOffline(slot,source.frame)||lane.has(source.frame)?[]:[`P${slot+1}`]);
  const reasons={window:`预测窗口已满（${MAX_ROLLBACK} 步）`,boot:'启动阶段等待真实输入',
    'prediction-disabled':`禁止预测：${runtime.predictionBlockReason()}`,'local-input':'本步本机输入尚未采样'};
  waitDetail=`${reasons[source.waitReason]||'等待真实输入'} · 当前步缺 ${missing.join('、')||'无'} · 连续输入确认到 ${source.confirmed}`;
  return null;
}
function planReplay(target){
  // Do not advance the live queue until the Worker reports exactly which
  // steps completed. Authoritative packets may arrive during the await.
  const cursor=Object.create(queue),plans=[];
  for(;cursor.frame<target&&plans.length<REPLAY_MAX_STEPS;cursor.frame++){
    const pair=nextInputs(cursor,false);if(!pair)break;
    const frame=cursor.frame;
    plans.push({frame,inputs:pair,predicted:cursor.active&&frame>=cursor.confirmed,
      authoritative:cursor.inputs.every((lane,slot)=>cursor.isOffline(slot,frame)||lane.has(frame)),
      offlineMask:cursor.offlineMask(frame),capture:cursor.active&&frame>=cursor.confirmed,
      hash:(frame+1)%120===0,end:frame+1===target});
  }
  return plans;
}
function updateMetrics(now,waiting){
  const span=now-lastMeasure;
  costs.lead=Math.max(costs.lead,queue.frame-queue.confirmed);
  if(span>=1000){
    hz=(queue.frame-measureFrame)*1000/span;averageCost=stepCount?stepCost/stepCount:0;peakCost=maxCost;measuredCosts=costs;
    const totalWait=waitedMs+(waitingSince?now-waitingSince:0),game=runtime.snapshot();
    waitPercent=Math.min(100,Math.max(0,(totalWait-measuredWaitMs)*100/span));measuredWaitMs=totalWait;
    gameHz=game&&measuredGame&&game.generation===measuredGame.generation&&game.stage===measuredGame.stage&&game.gameFrame>=measuredGame.gameFrame?(game.gameFrame-measuredGame.gameFrame)*1000/span:null;
    measuredGame=game;
    performanceHistory.push({timeMs:Math.round(now),frame:queue.frame,hz,gameHz,waitPercent,stepMs:averageCost,lockstepDelay:queue.lockstepDelay,inputPolicy,predictionLeadLimit:queue.active?queue.predictionLead:0,rollbackCount,resimulated,maxRewind,costs:{...costs},workerPerformance:runtime.performanceInfo?.(),rtt:onlinePeers().map(peer=>({role:peer.role,ms:peer.rtt}))});
    if(performanceHistory.length>120)performanceHistory.shift();
    costs=newCosts();lastMeasure=now;measureFrame=queue.frame;stepCost=stepCount=maxCost=0;
  }
  if(document.hidden||now-lastUi<250)return;lastUi=now;
  publishConnectionStatus();
  const links=[...peers.values()].map(p=>p.offline?`${label(p.role)}：已离线`:`${label(p.role)}：往返 ${p.rtt==null?'—':p.rtt.toFixed(1)} ms · ${p.path||'路径待确认'} · 相同状态步 ${p.confirmed||'等待'}${p.hidden?' · 页面隐藏':''}`).join('\n');
  const c=measuredCosts,game=runtime.snapshot();
  const own=game?.players?.[queue.slot];player.setLabel(`你操作 P${queue.slot+1}${own?' · 团队分数 '+own.score.toLocaleString('zh-CN'):''}`);
  $('diagnostics').textContent=`${label(session.role)} · P${queue.slot+1} · ${state.settings.players} 人 · 同步步 ${queue.frame}\n`
    +`本机 ${hz.toFixed(1)}/60 步每秒 · 执行平均 ${averageCost.toFixed(1)} / 最慢 ${peakCost.toFixed(1)} ms\n`
    +`游戏逻辑 ${gameHz===null?'待统计':gameHz.toFixed(1)} 帧/秒 · 等输入时间 ${waitPercent.toFixed(0)}%（最近周期）\n`
    +`性能诊断 v2 · 最近统计周期：模拟 ${(c.simulate/Math.max(1,c.steps)).toFixed(1)} ms/步 · 呈现 ${(c.present/Math.max(1,c.presentations)).toFixed(1)} ms/批 · 快照 ${(c.capture/Math.max(1,c.captures)).toFixed(1)} ms/次（${c.captures} 次）\n`
    +`恢复 ${c.restore.toFixed(1)} ms（${c.restores} 次） · 重算 ${c.replay.toFixed(1)} ms（${c.replays} 步，含模拟和快照） · 确认 ${c.confirm.toFixed(1)} ms · 校验 ${c.hash.toFixed(1)} ms\n`
    +`${queue.active?`混合回滚 · 本地缓冲 D=${queue.inputDelay} · 预测预算 P=${inputPolicy.predictionSteps} · 领先上限 ${queue.predictionLead} 步 · 方向保持 ${queue.directionPrediction} 步`:`确认锁步 · 自适应缓冲 ${queue.lockstepDelay} 步（${Math.round(queue.lockstepDelay*TICK_MS)} ms） · ${inputPolicy.rollbackEnabled?'等待回滚启用':'回滚已关闭'}`} · 最近输入至应用 ${inputAge.toFixed(1)} ms · 已收输入 ${receivedInputs}\n${links}\n`
    +`已确认 ${Math.min(queue.frame,queue.confirmed)} · 预测领先 ${Math.max(0,queue.frame-queue.confirmed)}/${queue.active?queue.predictionLead:0} 步 · 回滚安全窗口 ${MAX_ROLLBACK} 步\n`
    +`最近周期最大领先 ${c.lead} 步 · 游戏帧 ${game?.gameFrame??'启动中'} · 预测条件：${runtime.predictionBlockReason()||'允许'} · 补算前提前发输入 ${earlyInputs} 次\n`
    +`回滚 ${rollbackCount} 次 · 重算 ${resimulated} 步 · 最长 ${maxRewind} 步 · 快照 ${((runtime.rollbackInfo()?.snapshotBytes||0)/1048576).toFixed(0)} MiB\n`
    +`检查点每 ${runtime.rollbackInfo()?.checkpointFrames||1} 步 · 当前 ${runtime.rollbackInfo()?.snapshots||0} 个 · 边界重建 ${runtime.rollbackInfo()?.checkpointReplays||0} 步\n`
    +`当前等待 ${waitingSince?(now-waitingSince).toFixed(0):0} ms\n`
    +(waiting?waitDetail:queue.frame<replayUntil?`正在补算，还剩 ${replayUntil-queue.frame} 步`:'正常推进／等待下一步时刻')+`\n${gamepadStatus}`
    +(roomServiceError?'\n房间服务正在重试（游戏按玩家数据通道继续）：'+roomServiceError:'');
  player.diagnostics(`${runtime.diagnosticText()}\n\n${$('diagnostics').textContent}`);
  if(!game)player.note(`正在启动 DOS／游戏 · 同步步 ${queue.frame} · ${waiting?waitDetail:'正在推进'}。详情可查看当前状态。`);
}
async function pump(now){
  if(!running||stopped||pumping)return;
  const ownedEpoch=runEpoch;
  // Message/timer wakes can run before an rAF carrying an older timestamp.
  // Never move the elapsed-time baseline backward and count that time twice.
  now=Math.max(now,lastNow);
  if(membership){
    lastNow=now;accumulator=0;waitDetail='正在同步玩家离线边界';
    if(!waitingSince)waitingSince=now;
    updateMetrics(now,true);return;
  }
  pumping=true;
  try{
    accumulator=Math.min(accumulator+Math.max(0,now-lastNow),TICK_MS*4);lastNow=now;
    // Retry one already-due live frame. Network waiting must not accumulate
    // several fresh samples at once; historical repair keeps its own frontier.
    if(queue.retryFrame===queue.frame&&queue.frame>=replayUntil&&queue.dirty===null)accumulator=Math.min(accumulator,TICK_MS);
    let count=0,waiting=false;const budgetStart=performance.now();
    // A new live tick can become due while old history is still replaying.
    // Send it before more repair work, without sampling the historical cursor.
    if(queue.frame<replayUntil&&accumulator>=TICK_MS&&captureLocalInput(replayUntil,replayTouchContext))earlyInputs++;
    if(queue.dirty!==null)queue.refreshCorrection();
    if(queue.dirty!==null){
      // Publish the due frontier input before costly replay. Never sample a
      // historical replay frame, and never resample an already captured frame.
      if(queue.frame>=replayUntil&&accumulator>=TICK_MS&&captureLocalInput())earlyInputs++;
      // Keep touch/menu generation at the live boundary throughout repair.
      if(queue.frame>=replayUntil)replayTouchContext=runtime.touchContext();
      const from=queue.dirty;replayUntil=Math.max(replayUntil,queue.frame);
      maxRewind=Math.max(maxRewind,replayUntil-from);rollbackCount++;
      const restoreAt=performance.now();
      nativeBusy=true;await runtime.restore(from);if(stopped||ownedEpoch!==runEpoch)return;replayRecorder.rewind(from);queue.rewind(from);
      // A still earlier input can arrive during restore, before rewind clears
      // the old correction marker. Reconcile the retained completed history.
      if(runtime.asyncExecution&&queue.active)queue.refreshCorrection(true);
      completeNativeBoundary();
      costs.restore+=performance.now()-restoreAt;costs.restores++;
      for(const at of pendingHashes.keys())if(at>from)pendingHashes.delete(at);
    }
    if(queue.dirty!==null||membership){wake();return;}
    while(queue.frame<replayUntil&&count<4&&(count===0||performance.now()-budgetStart<6)){
      const plans=runtime.asyncExecution?planReplay(replayUntil):null;
      if(runtime.asyncExecution){
        if(!plans.length){waiting=true;nextInputs();break;}
        const replayAt=performance.now();nativeBusy=true;
        const outcome=await runtime.replaySlice(queue.frame,plans);if(stopped||ownedEpoch!==runEpoch)return;
        if(!Number.isInteger(outcome?.applied)||outcome.applied<0||outcome.applied>plans.length||!Array.isArray(outcome.records)||outcome.records.length!==outcome.applied)throw Error('Worker 回滚补算返回边界异常');
        for(let index=0;index<outcome.applied;index++){
          const plan=plans[index],record=outcome.records[index];
          if(record.frame!==plan.frame)throw Error('Worker 回滚补算记录帧异常');
          replayRecorder.stage(plan.frame,{inputs:plan.inputs,offlineMask:plan.offlineMask,proof:record.proof,hash:record.hash});
          queue.commit(plan.inputs);resimulated++;count++;
          if(record.hash)pendingHashes.set(plan.frame+1,{...record.hash,boot:record.ready});
          maxCost=Math.max(maxCost,record.stepMs);
        }
        if(outcome.applied)endWait();
        if(queue.active)queue.refreshCorrection(true);
        completeNativeBoundary();
        const elapsed=performance.now()-replayAt;costs.simulate+=elapsed;costs.steps+=outcome.applied;costs.replay+=elapsed;costs.replays+=outcome.applied;
        stepCost+=elapsed;stepCount+=outcome.applied;
        costs.lead=Math.max(costs.lead,queue.frame-queue.confirmed);
        if(outcome.applied<plans.length){waiting=outcome.blocked;break;}
      }else{
        const pair=nextInputs();if(!pair){waiting=true;break;}
        endWait();const replayAt=performance.now();
        await simulate(pair,queue.frame+1<replayUntil);if(stopped||ownedEpoch!==runEpoch)return;resimulated++;count++;
        costs.replay+=performance.now()-replayAt;costs.replays++;
      }
      if(queue.dirty!==null||membership)break;
    }
    // Never confirm or present a branch that changed while the worker ran.
    if(queue.dirty!==null||membership){wake();return;}
    await confirmFrames();if(stopped||ownedEpoch!==runEpoch)return;
    if(queue.dirty!==null||membership){wake();return;}
    if(queue.frame<replayUntil){
      if(waiting&&!waitingSince)waitingSince=performance.now();
      updateMetrics(performance.now(),waiting);wake(waiting?16:0);return;
    }
    while(accumulator>=TICK_MS&&count<4&&(count===0||performance.now()-budgetStart<6)){
      captureLocalInput();
      const pair=nextInputs();if(!pair){queue.retryFrame=queue.frame;accumulator=Math.min(accumulator,TICK_MS);waiting=true;break;}
      endWait();
      await simulate(pair,false);if(stopped||ownedEpoch!==runEpoch)return;
      const after=performance.now();
      const at=captureTimes.get(queue.frame-1);if(pair[queue.slot]!==lastApplied&&at)inputAge=after-at;
      lastApplied=pair[queue.slot];captureTimes.delete(queue.frame-1);accumulator-=TICK_MS;count++;
      queue.retryFrame=null;
      if(queue.dirty!==null||membership)break;
      await confirmFrames();if(stopped||ownedEpoch!==runEpoch)return;
      if(queue.dirty!==null||membership)break;
    }
    if(queue.dirty!==null||membership){wake();return;}
    const presentAt=performance.now();
    if(!document.hidden&&runtime.present()){costs.present+=performance.now()-presentAt;costs.presentations++;}
    const after=performance.now();
    if(waiting&&!waitingSince)waitingSince=after;
    updateMetrics(after,waiting);
    // Native execution time counts toward this tick's budget. The old timer
    // waited another full interval after a 15 ms step, reducing throughput.
    wake(waiting?16:Math.max(0,TICK_MS-accumulator-(after-lastNow)));
  }catch(error){if(ownedEpoch===runEpoch)fail(error);}finally{if(ownedEpoch===runEpoch){nativeBusy=false;pumping=false;}}
}
const focusSettings=mountFocusSettings($('control-settings'),{alwaysPointControl:player.alwaysPointControl});
const controlEditor=mountControlSettings($('control-settings'),{
  onChange:value=>{clearInput();runtime?.updateControls(value,controlEditor.isEditing());},
  onEditing:editing=>{clearInput();runtime?.updateControls(controlSnapshot(),editing);}
});
window.addEventListener('keydown',event=>{if(!running||controlEditor.isEditing()||isFormTarget(event.target)||event.metaKey||event.ctrlKey||event.altKey||!isBound(event.code))return;event.preventDefault();keys.add(event.code);observeInput();wake();});
document.addEventListener('focusin',event=>{if(isFormTarget(event.target))clearInput();});
window.addEventListener('keyup',event=>{keys.delete(event.code);observeInput();wake();});window.addEventListener('blur',clearInput);
document.addEventListener('visibilitychange',()=>{
  if(document.hidden)clearInput();
  if(running&&!stopped){broadcast({type:'heartbeat',hidden:document.hidden});wake();}
});
window.addEventListener('message',event=>{if(event.origin===location.origin&&event.source===iframe?.contentWindow&&event.data?.protocol===PROTOCOL&&event.data?.event==='runtime-exit')fail(Error('游戏模拟器已经退出'));});
window.addEventListener('message',event=>{
  if(event.origin!==location.origin||event.source!==iframe?.contentWindow||event.data?.protocol!==PROTOCOL||stopped)return;
  if(event.data.event==='boot-progress'){
    bootStage=String(event.data.message).slice(0,300);
    player.note(bootStage);
    if(Number.isInteger(event.data.step)&&event.data.step>=0&&event.data.step<=7)bootStep=event.data.step;
    showStartup();
    for(const peer of onlinePeers())if(peer.channel?.readyState==='open')send(peer,{type:'heartbeat',hidden:document.hidden,bootStage});
  }else if(event.data.event==='boot-error')fail(Error(`游戏页面：${String(event.data.message).slice(0,600)}`));
  else if(event.data.event==='page-warning'){
    let warning=$('page-warning');
    if(!warning){warning=document.createElement('pre');warning.id='page-warning';$('diagnostics').after(warning);}
    warning.textContent=`页面异常记录（未自动结束游戏）：\n${String(event.data.message).slice(0,1500)}`;
    if(event.data.externalScript)warning.textContent+='\n堆栈包含站外脚本。请暂停此站点上的相关用户脚本／扩展并刷新复测；尚不能据此认定它导致了网络连接失败。';
  }
});
window.addEventListener('pagehide',()=>{replayRecorder.finish('录像来源页面关闭');replayRecorder.close();if(session)navigator.sendBeacon('/api/leave',new Blob([JSON.stringify(session)],{type:'application/json'}));});
$('create').onclick=()=>enter('host');$('join').onclick=()=>enter('guest');
for(let n=0;n<3;n++)$(`seat${n}`).onclick=()=>action('seat',{slot:n});
for(const card of document.querySelectorAll('[data-loadout]'))card.onclick=()=>action('loadout',{loadout:Number(card.dataset.loadout)});
for(const name of [...settingNames,'language','rollback',...assistFlags])$(name).onchange=()=>{
  if(session?.role!=='host'||state?.phase!=='lobby')return;
  action('settings',{settings:{...Object.fromEntries(settingNames.map(key=>[key,Number($(key).value)])),...Object.fromEntries(assistFlags.map(key=>[key,$(key).checked])),focusPercent:50,language:$('language').value,rollback:$('rollback').checked}});
};
$('ready').onclick=()=>{
  if(!state.ready[session.role]&&session.role!=='host')player.note('已准备，等待房主开始。房主开始后进入游戏画面。');
  action('ready');
};
$('start').onclick=()=>{action('start');};
$('leave').onclick=async()=>{
  replayRecorder.finish('玩家离开本局');stopped=true;running=false;clearInput();runtime?.stop();clearTimers();disconnect();
  try{await replayRecorder.seal();await request('leave');}
  catch(error){
    // The host may remove this disconnected seat while its recording seals.
    if(![403,404].includes(error.status))throw error;
  }finally{location.href=networkUrl(mode);}
};
onBgmVersion(()=>{if(session&&state?.phase==='lobby'&&!stopped)prepareRoomResources(state).catch(()=>{});});
