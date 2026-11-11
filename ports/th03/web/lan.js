import {readNetworkMode,networkParams,describeNetwork} from './netplay/network-mode.js';
import {readRoomPreferences,saveRoomPreferences} from './room-preferences.js';
import {supportsNetworkWorker} from './netplay/simulation-client.js';
import {loadNetworkAssets,loadVerifiedBytes} from './startup-assets.js';
import {preloadOpeningMusic} from './local-bgm.js';
import {bgmVersion,onBgmVersion} from './bgm-version.js';
import {publishNetworkStatus} from './network-status.js';
const roomIds={start:'room-start',language:'room-language',difficulty:'room-difficulty',clock:'room-clock','connection-path':'room-connection-path'};
for(const [source,target]of Object.entries(roomIds)){const node=document.getElementById(source);if(node)node.id=target;}
const $ = id => document.getElementById(roomIds[id]||id), protocol = 'th03-lan/1';
let session = null, timer = null, busy = false, state = null, runtime = null;
let runtimeFrame=null,returningTask=null,runStopped=false,leaving=false;
let preloadKey='',preloadPromise=null,preloadGeneration=0,preloadRetryAt=0;
onBgmVersion(()=>{if(session&&state&&!state.started)prepareRoomAssets(state).catch(()=>{});});
const query = new URLSearchParams(location.search);
const networkRuntime=$('network-runtime');
try{networkRuntime.value=query.has('netWorker')?(query.get('netWorker')==='0'?'main':'worker'):(localStorage.getItem('th03-network-runtime')||'worker');}catch{networkRuntime.value='worker';}
if(networkRuntime.value!=='worker')networkRuntime.value='main';
if(!supportsNetworkWorker()){networkRuntime.value='main';networkRuntime.querySelector('[value="worker"]').disabled=true;}
networkRuntime.onchange=()=>{try{localStorage.setItem('th03-network-runtime',networkRuntime.value);}catch{}};
let connection=readNetworkMode();
const preferredConnection=readRoomPreferences().connections[connection.network];
if(preferredConnection){
  if(!query.has('transport')&&!['public-ws','direct-ws'].includes(query.get('network')))connection.mode=preferredConnection.mode;
  if(!query.has('ice')&&query.get('network')!=='public-turn')connection.ice=preferredConnection.ice;
}
$('mode').value=connection.mode;$('ice').value=connection.ice;
function renderConnection(){
  const isPublic=connection.network==='public';
  document.title=`东方梦时空 · ${isPublic?'公网':'局域网'}联机`;
  $('page-title').textContent=`${isPublic?'公网':'局域网'}联机`;
  document.querySelectorAll('[data-network]').forEach(link=>{
    if(link.dataset.network===connection.network)link.setAttribute('aria-current','page');else link.removeAttribute('aria-current');
  });
  $('mode-description').textContent=isPublic?'双方打开同一个公网网址，再创建或加入房间。':'双方连接同一个局域网，打开房主电脑提供的网址。';
  $('ice-option').hidden=!isPublic||connection.mode!=='rtc';
  $('connection-help').textContent=connection.mode==='relay'?'通过网页所在服务器转发输入；支持 HTTPS / WSS 和公网隧道。':
    !isPublic?'局域网内直接连接，不需要 STUN 或 TURN。':
    connection.ice==='relay'?'仅使用 TURN，用于打洞失败的网络；需要服务器已配置 TURN。':
    '使用 STUN 打洞，配置 TURN 后可自动中继；未配置 TURN 时仅尝试直连。失败可结束房间后切换 TCP。';
}
renderConnection();
$('mode').onchange=$('ice').onchange=()=>{
  if(session)return;
  connection={...connection,mode:$('mode').value,ice:$('ice').value};
  saveRoomPreferences({connection});
  const params=networkParams(connection);if($('room').value)params.set('room',$('room').value);
  history.replaceState(null,'',`lan.html?${params}`);renderConnection();
};
if (/^\d{4}$/.test(query.get('room') || '')) $('room').value = query.get('room');
function status(value, error = false) { $('lan-status').textContent = value; $('lan-status').dataset.error = String(error); }
const seatFor = role => (state.settings.hostSeat === 1 ? 1 : 0) ^ (role === 'guest' ? 1 : 0);
const seatLabel = role => seatFor(role) === 0 ? 'P1 · 左侧' : 'P2 · 右侧';
async function api(path, data = {}, method = 'POST') {
  const payload = {protocol, ...(session || {}), runId:state?.runId??0, ...data};
  const url = method === 'GET' ? `/api/${path}?${new URLSearchParams(payload)}` : `/api/${path}`;
  const response = await fetch(url, {method, cache: 'no-store',
    headers: method === 'POST' ? {'Content-Type': 'application/json'} : undefined,
    body: method === 'POST' ? JSON.stringify(payload) : undefined});
  const value = await response.json();
  if (!response.ok) throw Error(value.error || `请求失败 (${response.status})`);
  return value;
}
function render(value) {
  if (!session || leaving) return;
  if(returningTask)return;
  if(!value.started&&state?.started&&(runtime||runStopped)){void returnToRoom('本局已结束，可重新准备。',value);return;}
  state = value;
  if(!state.started)publishNetworkStatus({phase:'idle'});
  $('lobby').dataset.roomPhase=state.started?'loading':'lobby';
  $('lobby').dataset.roomLanguage=state.settings.language;
  if(state.started)$('lobby').dispatchEvent(new Event('room-preview-stop'));
  if(session.role==='host')saveRoomPreferences({settings:state.settings,connection:{network:state.network||'lan',mode:state.mode,ice:state.ice||'all'}});
  document.body.dataset.view=state.started?'playing':'room';
  $('entry').hidden = true; $('lobby').hidden = false; $('mode').disabled = true; $('mode').value = state.mode;
  connection={network:state.network||'lan',mode:state.mode,ice:state.ice||'all'};
  $('ice').value=connection.ice;$('ice').disabled=true;renderConnection();
  $('room-info').textContent = `房间 ${session.room}`;
  $('seat-help').textContent=`${describeNetwork(connection)} · 你是 ${seatLabel(session.role)}。开局后在游戏内选择角色。`;
  for (const role of ['host', 'guest']) {
    const card=$(role+'-seat');card.dataset.local=String(session.role===role);
    card.querySelector('.section-kicker').textContent=`PLAYER 0${seatFor(role)+1}`;
    card.querySelector('strong').textContent=`${seatLabel(role)} · ${role==='host'?'房主':'客机'}`;
    card.dataset.present=String(!!state.present[role]);card.dataset.ready=String(!!state.ready[role]);
    $(role + '-state').textContent = !state.present[role] ? '等待加入' : state.started ? '已开始' : state.ready[role] ? '已准备' : '未准备';
    $(role + '-progress').textContent = `P${seatFor(role)+1} ${role === 'host' ? '房主' : '客机'}：${state.startup[role]?.message || '等待加载'}`;
    $(role + '-progress').dataset.error = String(!!state.startup[role]?.error);
  }
  const leftCard=$(seatFor('host')===0?'host-seat':'guest-seat');
  if(leftCard.parentElement.firstElementChild!==leftCard)leftCard.parentElement.prepend(leftCard);
  if(!busy){
    $('host-seat-choice').value = String(state.settings.hostSeat ?? 0);
    for (const id of ['language', 'difficulty', 'clock']) $(id).value = String(state.settings[id]);
    $('rollback').checked = state.settings.rollback === true;
    $('focus-enabled').checked = state.settings.focusEnabled !== false;
    $('touch-unlimited-allowed').checked = state.settings.touchUnlimitedAllowed === true;
  }
  $('room-settings').disabled = busy || session.role !== 'host' || state.started;
  $('ready').disabled = busy || state.started;
  $('ready').textContent = state.ready[session.role] ? '取消准备' : '准备';
  returnButton.hidden=!state.started;returnButton.disabled=!!returningTask;
  $('leave').textContent=session.role==='host'?'解散房间':'离开房间';
  $('start').disabled = busy || session.role !== 'host' || state.started || !state.present.guest || !state.ready.host || !state.ready.guest;
  $('startup-progress').hidden = !state.started;
  if(session&&!state.started)prepareRoomAssets(state).catch(()=>{});
  networkRuntime.disabled=state.started;
  if (state.started && !runtime && !runStopped) launch().catch(error=>void returnToRoom(error.message));
  if (runtime && (!state.present.host || !state.present.guest)) {
    void returnToRoom('另一位玩家已离开，本局已结束');
  }
}
async function launch() {
  const params=new URLSearchParams({net:state.mode,room:session.room,token:session.token,role:session.role,
    runId:state.runId,embedded:'1',language:state.settings.language,difficulty:state.settings.difficulty,
    clock:state.settings.clock,rollback:state.settings.rollback?'1':'0',netWorker:networkRuntime.value==='worker'?'1':'0'});
  const frame=document.createElement('iframe');runtimeFrame=frame;
  frame.title='东方梦时空联机';frame.allow='autoplay; gamepad; fullscreen';frame.allowFullscreen=true;
  frame.style.cssText='width:100%;height:100%;min-height:480px;border:0';
  runtime={stop(){frame.contentWindow?.th03EmbeddedRuntime?.stop();frame.remove();if(runtimeFrame===frame)runtimeFrame=null;}};
  frame.src='local.html?'+params;const root=$('game');root.replaceChildren(frame);root.hidden=false;
}
async function returnToRoom(reason='本局已结束，可重新准备。',snapshot=null){
  if(leaving)return;
  if(returningTask)return returningTask;
  const old=runtime,frame=runtimeFrame,runId=state.runId;
  busy=true;runStopped=true;clearTimeout(timer);
  frame?.contentWindow?.th03EmbeddedRuntime?.stop();
  returningTask=(async()=>{
    try{
      const recorder=frame?.contentWindow?.th03Replay;
      recorder?.finish(reason);await recorder?.seal(2000);old?.stop();runtime=null;
      const next=snapshot||await api('end',{runId});
      state=next;runStopped=false;status(reason);$('game').hidden=true;
    }catch(error){status('返回房间失败：'+error.message,true);}
    finally{returningTask=null;busy=false;render(state);clearTimeout(timer);timer=setTimeout(poll,0);}
  })();return returningTask;
}
const returnButton=document.createElement('button');returnButton.type='button';
returnButton.id='return-room';returnButton.textContent='返回房间';returnButton.hidden=true;
returnButton.onclick=()=>void returnToRoom();$('leave').after(returnButton);
window.addEventListener('message',event=>{
  if(event.origin===location.origin&&event.source===runtimeFrame?.contentWindow)void handleRuntimeMessage(event.data);
});
async function prepareRoomAssets(room){
  const key=`${room.settings.language}:${room.settings.rollback?'rollback':'lockstep'}:${bgmVersion()}`;
  if(performance.now()<preloadRetryAt&&key===preloadKey)return preloadPromise;
  if(key===preloadKey&&preloadPromise)return preloadPromise;preloadKey=key;
  const generation=++preloadGeneration;
  const report=message=>{if(generation===preloadGeneration)$('resource-status').textContent=message;};
  preloadPromise=(async()=>{
    report('正在预载游戏磁盘、DOSBox-X 与音效…');
    const response=await fetch('disks/manifest.json',{cache:'no-store'});
    if(!response.ok)throw Error('磁盘清单读取失败');const disks=await response.json();
    const disk=disks.games[`3-${room.settings.language}`];if(!disk)throw Error('找不到所选语言的游戏磁盘');
    const runtimeResponse=await fetch('lockstep-runtime.json',{cache:'no-store'});
    if(!runtimeResponse.ok)throw Error('同步引擎清单缺失');const runtime=await runtimeResponse.json();
    const soundAssets=new Map();
    const [diskBytes,networkAssets]=await Promise.all([
      loadVerifiedBytes(new URL(disk.url,import.meta.url),disk.sha256,{gzip:true}),
      loadNetworkAssets(runtime,report,{rollbackEnabled:room.settings.rollback,onAsset:(path,bytes,sha256)=>{if(path.startsWith('sfx/'))soundAssets.set(path,{bytes,sha256});}})
    ]);
    let musicWarning='';
    await preloadOpeningMusic().catch(error=>{musicWarning=`；音乐预载失败：${error.message}`;});
    report(`游戏与音效已校验并预载（${(diskBytes.byteLength/1048576).toFixed(0)} MiB 游戏盘）${musicWarning}`);
    return {runtime,wasmBinary:networkAssets,soundAssets,diskBytes,language:room.settings.language};
  })();
  preloadPromise.catch(error=>{if(generation===preloadGeneration){report(`资源预载失败：${error.message}，稍后自动重试`);preloadRetryAt=performance.now()+5000;preloadPromise=null;}});
  globalThis.th03RoomHost??={};globalThis.th03RoomHost.preloadPromise=preloadPromise;
  return preloadPromise;
}
async function enter(kind) {
  if (busy || session) return;
  busy = true; $('create').disabled = $('join').disabled = true;
  try {
    const value = await api(kind, kind === 'create' ? {...connection,mode: $('mode').value,ice:$('ice').value,settings:readRoomPreferences().settings} : {room: $('room').value.trim()});
    session = {room: value.room, token: value.token, role: value.role};
    busy = false; render(value.state);$('connection-settings').open=false;
    status(`你是${session.role === 'host' ? '房主，可选择左右席位' : '客机，席位随房主选择自动分配'}，确认设置后点击准备。`);
    poll();
  } catch (error) { status(error.message, true); }
  finally { busy = false; $('create').disabled = $('join').disabled = false; }
}
async function poll() {
  if (!session || leaving) return;
  try { if (!busy) {const next=await api('state', {}, 'GET');if(!busy&&next.revision>=state.revision)render(next);} }
  catch (error) {
    status(error.message, true);
    if (/房间不存在|房间身份无效/.test(error.message)) {runtime?.stop();return;}
  }
  timer = setTimeout(poll, 1000);
}
async function mutate(path, data = {}) {
  if (busy || !session) return;
  busy = true; render(state);
  try { state = await api(path, data); }
  catch (error) { status(error.message, true); }
  finally { busy = false; render(state); }
}
$('create').onclick = () => enter('create'); $('join').onclick = () => enter('join');
$('configure-controls').onclick=()=>{
  if(runtime){runtimeFrame?.contentWindow?.th03EmbeddedRuntime?.openControls();$('game').scrollIntoView({block:'start'});return;}
  $('controls-dialog').querySelector('iframe').src='local.html?controlsOnly=1';
  $('controls-dialog').showModal();
};
$('controls-dialog').addEventListener('close',()=>$('controls-dialog').querySelector('iframe').removeAttribute('src'));
$('room').addEventListener('keydown', event => { if (event.key === 'Enter') enter('join'); });
$('ready').onclick = () => mutate('ready'); $('start').onclick = () => mutate('start');
for (const id of ['language', 'difficulty', 'clock', 'rollback', 'focus-enabled', 'touch-unlimited-allowed', 'host-seat-choice']) $(id).onchange = () => mutate('settings', {settings: {
  hostSeat: Number($('host-seat-choice').value), language: $('language').value, difficulty: Number($('difficulty').value), clock: Number($('clock').value), rollback: $('rollback').checked, focusEnabled: $('focus-enabled').checked, touchUnlimitedAllowed: $('touch-unlimited-allowed').checked}});
$('leave').onclick = async () => {
  if(leaving)return;
  // Disposal rejects pending simulation work. Ignore its late progress/error
  // callbacks so they cannot start a competing return-to-room transaction.
  leaving=true;
  busy=true;clearTimeout(timer);runtimeFrame?.contentWindow?.th03EmbeddedRuntime?.stop();
  try { const recorder=runtimeFrame?.contentWindow?.th03Replay;recorder?.finish('玩家离开本局');await recorder?.seal(2000);await api('leave'); } catch {}
  runtime?.stop();
  location.href = `lan.html?${networkParams(connection)}`;
};
async function handleRuntimeMessage(value){
  if(value?.protocol!==protocol||returningTask||leaving)return;
  if(value.event==='network-status'){publishNetworkStatus(value.snapshot);return;}
  if(value.event==='room-menu'){void returnToRoom();return;}
  if(value.event==='view'){runtimeFrame?.classList.toggle('game-immersive',value.immersive===true);return;}
  if(value.event==='connection'){
    $('connection-details').hidden=false;$('connection-path').hidden=false;
    $('connection-path').textContent='本机连接：'+String(value.message).slice(0,240);return;
  }
  if(value.event!=='progress')return;
  const ownedFrame=runtimeFrame,ownedRun=state.runId;
  try{
    const next=await api('progress',{message:value.message,loaded:value.loaded===true,error:value.error===true});
    if(returningTask||leaving||runtimeFrame!==ownedFrame||state.runId!==ownedRun||next.revision<state.revision)return;
    render(next);
  }
  catch(error){if(!returningTask&&!leaving&&runtimeFrame===ownedFrame&&state.runId===ownedRun)status(error.message,true);}
  if(returningTask||leaving||runtimeFrame!==ownedFrame||state.runId!==ownedRun)return;
  if(value.error){status(value.message,true);void returnToRoom(value.message);}else if(value.loaded)status('本机游戏已启动');
}
window.addEventListener('pagehide', () => {
  globalThis.th03Replay?.finish('观战来源页面关闭');globalThis.th03Replay?.close();
  if (session) navigator.sendBeacon('/api/leave', new Blob([JSON.stringify({protocol, ...session})], {type: 'application/json'}));
});
