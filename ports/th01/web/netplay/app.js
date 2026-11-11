import {publishNetworkStatus,mountNetworkStatus} from '../network-status.js';
import {mountHorizontalTouch,mountTouchPreferences} from '../th01-touch.js';
import {bgmVersion,onBgmVersion} from '../bgm-version.js';
import {populateStages,stageLabel} from '../stage-select.js';
import {NetSession} from './session.js';
import {readNetworkMode,describeNetwork,networkParams} from './network-mode.js';
import {assetBytes,freshJSON,parallelAssets} from '../preload-cache.js';
import {preloadMusicTrack} from '../bgm-track.js';
import {musicRoomTracks} from '../music-room.js';
import {stageSelection} from '../stage-select.js';
const $=id=>document.getElementById(id),roles=['host','guest','guest2'];
populateStages($('startStage'));
let identity,room,session,polling=false,busy=false,editing=false,leaving=false;
let returningTask=null;
let roomPreloadKey='',roomPreloadPromise=null,roomPreloadGeneration=0,roomPreloadRetryAt=0;
const keys=new Set(),touches=new Map();let taps=0,commands=0;
const keyBits={ArrowLeft:1,ArrowRight:2,KeyZ:4,KeyX:8,KeyC:4096,ArrowUp:16,ArrowDown:32,Enter:64,Escape:128};
const status=text=>{$('status').textContent=text;};
const initial=readNetworkMode();for(const id of ['network','transport','ice'])$(id).value=initial[id];
function config(){
  const network=$('network').value,transport=$('transport').value;
  return {startStage:$('startStage').value,players:Number($('players').value),rank:Number($('rank').value),count:Number($('count').value),
    lifeCompensation:$('life-compensation').checked,network,transport,ice:network==='public'&&transport==='rtc'?$('ice').value:'all',delay:Number($('delay').value)};
}
function help(){
  const c=config();$('ice').disabled=c.network!=='public'||c.transport!=='rtc';
  $('network-title').textContent=c.network==='public'?'公网联机':'局域网联机';
  document.title=`东方灵异传 · ${$('network-title').textContent}`;
  for(const a of document.querySelectorAll('.mode-nav [data-mode]')){
    if(a.dataset.mode===(c.network==='public'?'public':'lan'))a.setAttribute('aria-current','page');else a.removeAttribute('aria-current');
  }
  $('network-help').textContent=c.network==='public'?'全员使用同一 HTTPS 地址。自动模式使用 STUN 打洞；强制 TURN 需先配置服务。':'全员使用同一个局域网网址。UDP 默认直连，无需 STUN；公网请选择公网模式。';
  $('balance').textContent=`${c.players} 人 · ${c.count} 玉 · ${c.lifeCompensation?'生命补偿 +'+Math.floor(c.count/2):'生命补偿关闭'}；Boss 耐久沿用当前移植版的人数／玉数配置。每步约 17.7 ms，自动缓冲按连接实测选择。`;
}
for(const id of ['startStage','players','rank','count','life-compensation','network','transport','ice','delay'])$(id).onchange=help;help();
const wake=()=>session?.pump();
const horizontalTouch=mountHorizontalTouch($('canvas'),{
  bombBit:4096,
  allowed:()=>!!session&&!session.closed&&!document.hidden,
  gameplay:()=>{const s=session?.game.state();return !!s&&!s.menu&&s.phase<=1&&!s.entrance;},
  menu:()=>!!session?.game.state()?.menu&&identity?.role==='host',
  gesture:()=>{void session?.game.unlockAudio().catch(()=>{});},
  change:wake,pulse:bit=>tap(bit===16?8:bit===256?64:bit),
});
mountTouchPreferences($('music-volume').closest('label'),horizontalTouch);
function sample(){let v=taps|commands;for(const key of keys)v|=keyBits[key];for(const bit of touches.values())v|=bit;taps=commands=0;v=horizontalTouch.pack(v);horizontalTouch.consume();return v;}
function clear(){horizontalTouch.reset();keys.clear();touches.clear();taps=commands=0;for(const b of document.querySelectorAll('[data-bit]'))b.classList.remove('active');wake();}
function tap(bit){taps|=bit;wake();}
addEventListener('keydown',event=>{
  if(!session||session.closed||event.target.closest('input,select,textarea')||!keyBits[event.code])return;
  event.preventDefault();if(!keys.has(event.code)){keys.add(event.code);taps|=keyBits[event.code];wake();}
});
addEventListener('keyup',event=>{if(!keyBits[event.code])return;const changed=keys.delete(event.code);if(session){event.preventDefault();if(changed)wake();}});
addEventListener('blur',clear);document.addEventListener('visibilitychange',()=>{if(document.hidden)clear();});
document.addEventListener('focusin',event=>{if(event.target.closest('input,select,textarea'))clear();});
for(const button of document.querySelectorAll('[data-bit]')){
  button.onpointerdown=event=>{event.preventDefault();const bit=Number(button.dataset.bit);button.setPointerCapture(event.pointerId);touches.set(event.pointerId,bit);taps|=bit;button.classList.add('active');wake();void session?.game.unlockAudio().catch(()=>{});};
  const release=event=>{touches.delete(event.pointerId);button.classList.remove('active');wake();};
  button.onpointerup=button.onpointercancel=button.onlostpointercapture=release;
}
async function post(action,data={}){
  const response=await fetch(`/api/${action}`,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({...identity,runId:room?.runId??0,...data})});
  const value=await response.json();if(!response.ok)throw Error(value.error||'房间请求失败');return value;
}
function showRoom(value){
  if(value.phase==='lobby')publishNetworkStatus({phase:'idle'});
  room=value;const host=identity.role==='host',lobby=room.phase==='lobby';
  $('lobby').dataset.roomPhase=room.phase;
  $('lobby').dataset.roomLanguage='jp';
  if(!lobby)$('lobby').dispatchEvent(new Event('room-preview-stop'));
  if(lobby)prepareRoomAssets(room).catch(()=>{});
  editing=editing&&lobby;$('entry').hidden=!editing;$('lobby').hidden=false;
  $('room-title').textContent=`合作房间 · P${room.seats[identity.role]+1}${host?'（房主）':''}`;$('room-code').textContent=room.room;
  $('room-settings').textContent=`${stageLabel(room.settings.startStage)}${room.settings.startStage!=='normal'?'（练习，不保存正式成绩）':''} · ${room.settings.players} 人 · ${room.settings.count} 玉 · ${['Easy','Normal','Hard','Lunatic'][room.settings.rank]} · ${describeNetwork(room.settings)} · ${room.settings.delay?room.settings.delay+' 步缓冲':'自动缓冲'}`;
  $('members').replaceChildren(...roles.slice(0,room.settings.players).map(role=>{
    const el=document.createElement('span');el.textContent=`P${room.seats[role]+1}${role===identity.role?' · 你':''}：${!room.present[role]?'等待加入':room.ready[role]?'已准备':'未准备'}`;return el;
  }));
  $('swap').disabled=$('settings').disabled=!host||!lobby;$('ready').disabled=!lobby;
  $('ready').textContent=room.ready[identity.role]?'取消准备':'准备';
  returnButton.hidden=lobby;returnButton.disabled=!!returningTask;
  $('leave').textContent=host?'解散房间':'离开房间';
  $('start').disabled=!host||!lobby||!roles.slice(0,room.settings.players).every(r=>room.present[r]&&room.ready[r]);
  $('seat-note').textContent=`你控制 P${room.seats[identity.role]+1}；${host?'你负责菜单、路线选择和续关。':'菜单由房主操作，你可以请求暂停。'}`;
}
async function prepareRoomAssets(value){
  const key=String(value.settings.startStage)+':'+bgmVersion();
  if(key===roomPreloadKey&&roomPreloadPromise)return roomPreloadPromise;
  if(performance.now()<roomPreloadRetryAt)return;
  const generation=++roomPreloadGeneration;roomPreloadKey=key;
  const report=text=>{if(generation===roomPreloadGeneration&&room?.phase==='lobby')$('resource-status').textContent=text;};
  roomPreloadPromise=(async()=>{
    report('正在预载原生核心、绘图线程、音效与开场音乐…');
    const version=await freshJSON('netplay/runtime.json');
    const assets=Object.entries(version.files);
    await parallelAssets(assets,async([name,hash])=>assetBytes(name,{hash}));
    const bytes=await assetBytes('game.wasm',{hash:version.files['game.wasm']});
    const module=await WebAssembly.compile(bytes);
    if(WebAssembly.Module.imports(module).length)throw Error('游戏核心含外部导入');
    const sounds=await freshJSON('sfx/manifest.json');
    await parallelAssets(sounds.sounds,async sound=>assetBytes(`sfx/${String(sound.id).padStart(2,'0')}.wav`,{hash:sound.sha256}));
    const stage=stageSelection(value.settings.startStage).stage;
    const track=musicRoomTracks[stage-1]?.key||'st0';
    await preloadMusicTrack(track);
    globalThis.th01RoomPreload={version,module};
    report('游戏核心、音效与开场音乐已校验并预载');
  })();
  roomPreloadPromise.catch(error=>{if(generation===roomPreloadGeneration){report(`预载失败：${error.message}，稍后重试`);roomPreloadKey='';roomPreloadPromise=null;roomPreloadRetryAt=performance.now()+5000;}});
  return roomPreloadPromise;
}
function render(current){
  mountNetworkStatus($('player').querySelector('.toolbar'));
  publishNetworkStatus(current.networkStatus());
  const state=current.game.state();$('connection').textContent=current.connectionText();
  $('runtime').textContent=`确认步 ${current.queue.frame} · 最近一致摘要 ${current.lastHash}\n${state?`第 ${state.stage} 关 · ${state.entrance?'过场':state.menu?'菜单 '+state.menu:['等待开球','游戏中','关卡完成','全员阵亡'][state.phase]} · 倒计时 ${state.timer}`:'加载中'}`;
  $('menu-touch').hidden=!state?.menu||identity.role!=='host';
  $('continue-game').hidden=identity.role!=='host'||state?.menu!==9;
  $('continue-game').textContent='选择续关（再按 Z 确认）';
  $('menu-note').textContent=state?.menu===12?'姓名登记：房主方向键选字符，Z输入，X删除，Enter保存。联机成绩与本地单机成绩分开保存。':state?.phase===2&&!state.menu?'房主按 Z／Enter 查看结算或进入下一关。':state?.menu?'菜单由房主操作。':state?.phase===0&&!state.entrance?'任意玩家按 Z 后开球和倒计时。':'';
  if(current.game.scoreSaveError)$('menu-note').textContent+=' 本机成绩保存失败，本局可继续。';
  $('coop-hud').replaceChildren(...(state?.players||[]).map((p,i)=>{const el=document.createElement('span');el.textContent=`P${i+1}${i===current.slot?'（你）':''} · ${p.ghost?'幽灵':`残机 ${p.lives} / Bomb ${p.bombs}`} · 获救 ${p.rescues}`;return el;}));
}
function failure(reason,current){if(current!==session||returningTask)return;publishNetworkStatus(current.networkStatus());clear();status(reason);$('connection').textContent=current.connectionText();void returnToRoom(reason);}
async function accept(value){
  if(returningTask)return;
  if(identity&&value.phase==='lobby'&&room?.phase==='loading'){await returnToRoom('本局已结束，可重新准备。',value);return;}
  if(!identity)identity={room:value.room,role:value.role,token:value.token};showRoom(value);
  if(room.ended&&session&&!session.closed){session.fail(Error(room.ended),false);return;}
  if(room.phase==='loading'&&!session){
    await prepareRoomAssets(room).catch(()=>{});
    clear();$('cover').hidden=true;
    session=new NetSession(identity,room,{canvas:$('canvas'),api:post,status,buttons:sample,render,failure});
    session.game.onMusicStatus=text=>{$('music-status').textContent=text;};session.game.setMusicVolume(Number($('music-volume').value)/100);
    window.th01Net={get session(){return session;},get room(){return room;},
      ...(room.settings.qa&&identity.role==='host'?{qa(bit){commands|=bit;wake();}}:{})};
    const current=session;
    try{await current.prepare();}catch(error){current.fail(error);return;}
    if(session!==current||current.closed)return;
  }
  for(const message of value.signals||[])await session?.receiveSignal(message);
}
async function poll(){
  if(polling||!identity||leaving)return;polling=true;
  try{const response=await fetch('/api/events?'+new URLSearchParams({room:identity.room,token:identity.token}),{cache:'no-store'});
    const value=await response.json();if(!response.ok)throw Error(value.error||'房间服务不可用');await accept(value);
    if(session&&!session.closed){if(session.running)status(session.waitingText());else $('connection').textContent=session.connectionText();}
  }catch(error){if(session&&!session.closed)session.fail(error);else status(error.message);}finally{polling=false;}
}
async function action(fn){if(busy)return;busy=true;try{await fn();}catch(error){status(error.message);}finally{busy=false;}}
$('create').onclick=()=>action(async()=>{await accept(await post('create',{settings:config()}));status('已创建房间，让队友输入四位房间号');});
$('join').onclick=()=>action(async()=>{await accept(await post('join',{room:$('code').value.trim()}));status('已加入，请准备');});
$('ready').onclick=()=>action(async()=>accept(await post('ready',{ready:!room.ready[identity.role]})));
$('start').onclick=()=>action(async()=>accept(await post('start')));
$('swap').onclick=()=>action(async()=>accept(await post('seat')));
$('settings').onclick=()=>{editing=!editing;$('entry').hidden=!editing;$('entry-actions').hidden=true;$('save-settings').hidden=false;
  for(const id of ['startStage','players','rank','count','network','transport','ice','delay'])$(id).value=String(room.settings[id]);$('life-compensation').checked=room.settings.lifeCompensation;help();};
$('save-settings').onclick=()=>action(async()=>{const value=await post('settings',{settings:config()});editing=false;await accept(value);});
async function returnToRoom(reason='本局已结束，可重新准备。',snapshot=null){
  if(returningTask)return returningTask;
  const old=session,runId=room?.runId;
  old?.close();clear();
  returningTask=(async()=>{
    try{
      await old?.recorder.seal(2000);
      const next=snapshot||await post('end',{runId});
      old?.recorder.close(true);session=null;
      $('cover').hidden=false;$('fallback').hidden=true;
      showRoom(next);status(reason);
    }catch(error){status('返回房间失败：'+error.message+'，可点击返回房间重试。');}
    finally{returningTask=null;returnButton.disabled=false;}
  })();return returningTask;
}
const returnButton=document.createElement('button');returnButton.type='button';
returnButton.id='return-room';returnButton.textContent='返回房间';returnButton.hidden=true;
returnButton.onclick=()=>void returnToRoom();$('leave').after(returnButton);
async function leave(mode=room?.settings||config()){
  leaving=true;session?.close();try{await session?.recorder.seal();if(identity)await post('leave');}finally{identity=null;location.href='lan.html?'+networkParams(mode);}
}
$('leave').onclick=$('retry').onclick=()=>action(()=>leave());
$('retry-ws').onclick=()=>action(()=>leave({...room.settings,transport:'ws',ice:'all'}));
$('pause-game').onclick=()=>tap(128);$('continue-game').onclick=()=>{commands|=256;wake();};
$('sound').onclick=()=>action(async()=>{if(!session?.game.state())throw Error('游戏尚未加载');await session.game.unlockAudio();$('canvas').focus();});
$('music-volume').oninput=()=>session?.game.setMusicVolume(Number($('music-volume').value)/100);
$('canvas').onpointerdown=()=>{$('canvas').focus();void session?.game.unlockAudio().catch(()=>{});};
$('fullscreen').onclick=()=>action(async()=>{if(document.fullscreenElement)await document.exitFullscreen();else await $('screen').requestFullscreen();});
addEventListener('pagehide',()=>{session?.close();if(identity)navigator.sendBeacon('/api/leave',new Blob([JSON.stringify(identity)],{type:'application/json'}));});
$('code').value=new URLSearchParams(location.search).get('room')?.match(/^\d{4}$/)?.[0]||'';
fetch('/api/network',{cache:'no-store'}).then(r=>{if(!r.ok)throw Error();return r.json();}).then(v=>{$('network-help').textContent+=` TURN：${v.turnConfigured?'已配置':'未配置'}。`;}).catch(()=>status('请使用 start-lan.bat 启动联机服务'));
setInterval(poll,250);
onBgmVersion(()=>{if(identity&&room?.phase==='lobby')prepareRoomAssets(room).catch(()=>{});});
