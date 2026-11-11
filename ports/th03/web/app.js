import {DOSBoxPC98} from './dosbox-adapter.js';
import {createRecorder} from './replay/recorder.js';
import {battleProof} from './replay/battle-proof.js';
import {WorkerNP21, supportsSoloWorker} from './solo-worker-client.js';
import {workerLocalPause} from './local-worker-client.js';
import {bootToSelection} from './dosbox-boot.js';
import {loadVerifiedBytes, loadNetworkAssets, loadLocalCore} from './startup-assets.js';
import {mountControls} from './controls.js';
import {Fat12, installNativePause, nativeAssets, sha256} from './disk.js';
import {unlockDiskScores} from './scores.js';
import {NativePause} from './native-pause.js';
import {Netplay} from './netplay.js';
import {mountPlayer} from './player-ui.js';
import {Lockstep} from './lockstep.js';
import {AsyncLockstep} from './netplay/async-lockstep.js';
import {WorkerNetworkSimulation,supportsNetworkWorker,workerNetworkPause} from './netplay/simulation-client.js';
import {createFrameGate} from './frame-limit.js';
import {LocalBgm} from './local-bgm.js';
import {LocalSfx} from './local-sfx.js';
import {installNetworkAudio} from './network-audio.js';
import {publishNetworkStatus,mountNetworkStatus} from './network-status.js';
import {NativeMusic, ConfirmedMusic} from './native-music.js';
import {createNetworkHistory,mountNetworkPerformance,readNativeCoreDiagnostics} from './network-performance.js';

const roomHost=globalThis.th03RoomHost;
const $ = id => roomHost?.root?.querySelector(`[data-runtime-id="${id}"]`)??document.getElementById(id);
const netQuery = new URLSearchParams(roomHost?.params??location.search);
const controlsOnly=netQuery.get('controlsOnly')==='1';
let rollbackEnabled = false, roomFocusEnabled = true, roomTouchUnlimitedAllowed = false, roomHostSeat = 0;
const online = ['rtc', 'relay'].includes(netQuery.get('net')) && ['host', 'guest'].includes(netQuery.get('role'))
  && /^[0-9]{4}$/.test(netQuery.get('room') || '') && !!netQuery.get('token');
const embedded = !roomHost && online && netQuery.get('embedded') === '1' && window.parent !== window;
function notifyRoom(message, loaded = false, error = false) {
  if(roomHost)roomHost.notify({protocol:'th03-lan/1',event:'progress',message,loaded,error});
  if (embedded) window.parent.postMessage({protocol: 'th03-lan/1', event: 'progress', message, loaded, error}, location.origin);
}
function returnRoom() {
  const event={protocol:'th03-lan/1',event:'room-menu'};
  if(roomHost)roomHost.notify(event);
  if(embedded)window.parent.postMessage(event,location.origin);
}
if (embedded) {
  document.body.classList.add('embedded');
  new ResizeObserver(() => window.parent.postMessage({protocol: 'th03-lan/1', event: 'resize',
    height: document.querySelector('main').getBoundingClientRect().height}, location.origin)).observe(document.querySelector('main'));
}
let emulator, manifest, database, activeLang, starting = false, dirty = 0, saved = 0, saveTask, saveTimer, netplay;
let visibilityPaused = false, storageWarning = '';
let nativePause, nativeState, pauseSeat = null, pauseSelection = 0, localBgm, nativeMusic, confirmedMusic;
let localSfx, networkAudio;
let replayRecorder;
let closed = false, closing = false, escapeAt = -Infinity, escapeTimer;
let sync, syncIdentity, syncFrame=0, syncReplaying=false;
const nativeCanvas = document.createElement('canvas');
nativeCanvas.width = 640; nativeCanvas.height = 400;
nativeCanvas.hidden = true; nativeCanvas.style.display = 'none';
document.body.append(nativeCanvas);
const visibleCanvas = $('canvas');
const useNetworkWorker=online&&netQuery.get('netWorker')!=='0'&&supportsNetworkWorker();
let bitmapContext=null;
if(useNetworkWorker)try{bitmapContext=visibleCanvas.getContext('bitmaprenderer',{alpha:false});}catch{}
const visibleContext=online?(bitmapContext||visibleCanvas.getContext('2d',{alpha:false})):null;
if(online&&!visibleContext)throw Error('浏览器无法创建游戏画面');
const presentationDue = createFrameGate();
let presentationCount = 0, lastUpload = -1;
let presentationMs=0;
const networkHistory=createNetworkHistory();
const networkEvents=createNetworkHistory(100);
const logError=(kind,error)=>{if(online)networkEvents.push({at:performance.now(),kind,message:String(error?.message??error).replace(/https?:\/\/[^\s)]+/g,'[URL]').slice(0,2000)});};
window.addEventListener('error',event=>logError('error',event.error??event.message));
window.addEventListener('unhandledrejection',event=>logError('unhandledrejection',event.reason));
function presentFrame() {
  if (!online || !emulator || closed || closing || emulator.state === 'exited') return;
  const uploaded=emulator.module.frameUploads||0;
  if(uploaded===lastUpload)return;
  const began=performance.now();
  if(!emulator.module.netPresent())return;
  lastUpload=uploaded;
  visibleContext.imageSmoothingEnabled = false;
  visibleContext.drawImage(nativeCanvas, 0, 0, 640, 400);
  presentationCount++;
  presentationMs+=performance.now()-began;
}
const syncSnapshots=new Map();
const hiddenSeats=new Set();
let previousPerformance=null;
function performanceTotals(){
  const module=emulator?.module,costs=sync?.costs;
  return {at:performance.now(),steps:sync?.queue.frame??module?.localFrameCount??0,
    executed:costs?.steps??module?.localFrameCount??0,simulate:costs?.simulate??module?.localStepMs??0,
    uploads:module?.frameUploads||0,presentations:online?presentationCount:module?.frameUploads||0,
    capture:costs?.capture||0,captures:costs?.captures||0,hash:costs?.hash||0,
    restore:costs?.restore||0,restores:costs?.restores||0,replays:costs?.replays||0,presentationMs,
    confirm:costs?.confirm||0,confirmCallbacks:costs?.confirmCallbacks||0,skippedConfirmations:costs?.skippedConfirmations||0,
    native:module?.stepMetrics?.totalMs||0,nativeSteps:module?.stepMetrics?.count||0};
}
setInterval(()=>{
  if(!emulator||closed)return;
  const next=performanceTotals(),old=previousPerformance;previousPerformance=next;
  if(!old)return;
  const dt=Math.max(1,next.at-old.at),rate=k=>(next[k]-old[k])*1000/dt;
  const delta=k=>next[k]-old[k];
  const stats={stepsHz:rate('steps'),uploadsHz:rate('uploads'),presentationsHz:rate('presentations'),
    simulateMs:delta('simulate')/Math.max(1,delta('executed')),
    snapshotMs:delta('capture')/Math.max(1,delta('captures')),hashMs:delta('hash'),
    replaySteps:delta('replays'),rollbackCount:delta('restores'),restoreMs:delta('restore'),snapshotBytes:emulator.module.netSnapshotInfo?.().snapshotBytes||0,
    confirmMs:delta('confirm'),confirmCallbacks:delta('confirmCallbacks'),skippedConfirmations:delta('skippedConfirmations'),
    nativeMs:delta('native')/Math.max(1,delta('nativeSteps')),presentationMs:delta('presentationMs')/Math.max(1,delta('presentations'))};
  window.th03Performance=stats;
  if(online)networkHistory.push({at:next.at,intervalMs:dt,...stats,frame:sync?.queue.frame??null,confirmed:sync?.queue.confirmed??null,
    tick:emulator.tick,virtualNow:emulator.module.virtualNow,waitReason:sync?.queue.waitReason??'',replayUntil:sync?.replayUntil??null,
    predictionLead:Math.max(0,(sync?.queue.frame??0)-(sync?.queue.confirmed??0)),directionPrediction:sync?.queue.directionPrediction??null,
    inputDelay:sync?.queue.inputDelay??null,bootInputDelay:sync?.queue.bootDelay??null,inputPolicy:netplay?.inputPolicy??null,
    startupRttSamples:netplay?.rttSamples??null,
    predictionLeadLimit:sync?.queue.active?sync.queue.predictionLead:0,
    phase:nativeState?.phase??0,selecting:nativePause?.selecting??false,pauseSeat,hiddenSeats:[...hiddenSeats],documentHidden:document.hidden,
    nativeMaxMs:emulator.module.stepMetrics?.maxMs??0,program:emulator.module.programReads?.at(-1)?.name??null,
    snapshotStorage:emulator.module.netSnapshotInfo?.().storage??'disabled',writeJournal:emulator.module.mobileRuntime.writeJournal,heapBytes:emulator.module.HEAPU8?.length??emulator.module.heapBytes,
    executionThread:emulator.module.executionThread??'page',workerRpcCount:emulator.rpcCount??0,workerExecutionMs:emulator.executionMs??0,workerPending:emulator.pending?.size??0,
    nativeCore:emulator.isNetworkWorker?emulator.snapshot.nativeCore:readNativeCoreDiagnostics(emulator.module),pauseSearch:emulator.isNetworkWorker?emulator.snapshot.pauseSearch:nativePause?.search?'wasm-simd':'javascript',
    audioState:emulator.audio.context.state,soundEffects:localSfx?.info()??null,status:$('status').textContent,connection:$('connection-path').textContent});
  $('performance-stats').textContent=`模拟 ${stats.stepsHz.toFixed(1)}/60 步/秒 · 画面上传 ${stats.uploadsHz.toFixed(1)} 次/秒 · 显示提交 ${stats.presentationsHz.toFixed(1)} 次/秒\n模拟 ${stats.simulateMs.toFixed(2)} ms/步 · 快照 ${stats.snapshotMs.toFixed(2)} ms/次 · 校验 ${stats.hashMs.toFixed(2)} ms\n补算 ${stats.replaySteps} 步 · 恢复 ${stats.restoreMs.toFixed(2)} ms · 快照 ${(stats.snapshotBytes/1048576).toFixed(1)} MiB\n${localBgm?.status||'音乐准备中'}`;
},1000);
const enqueue=value=>{if(netplay?.isLockstep&&!sync?.applying){netplay.command(value);return true;}return false;};
const nativeNow=()=>online?(emulator?.module.virtualNow??0):performance.now();
const inGameplay = () => !!nativeState?.phase;
const screenChildren = [...$('screen').children];
const player = mountPlayer($('screen'), {solo: true,network:online, onGesture: () => { clearEscape(); resumeAudio(); },
  onFullscreenExit: () => { if(pauseSeat===null)openPause(0); }});
if(online)mountNetworkStatus($('screen').querySelector('.player-toolbar'));
if(embedded){
  const back=document.createElement('button');back.type='button';back.className='room-return-current-run';back.textContent='返回房间';
  back.onclick=returnRoom;$('screen').append(back);
}
$('screen').addEventListener('player-viewchange',event=>{
  if(roomHost)roomHost.notify({protocol:'th03-lan/1',event:'view',immersive:event.detail.immersive});
  else if(embedded)window.parent.postMessage({protocol:'th03-lan/1',event:'view',immersive:event.detail.immersive},location.origin);
});
$('screen').classList.remove('screen');
const viewport = document.createElement('div'); viewport.className = 'screen';
viewport.append(...screenChildren); player.stage.prepend(viewport);
// Overlays must be outside the translated viewport's stacking context.
for (const id of ['cover', 'pause-shade', 'pause-menu']) player.stage.append($(id));
player.setLabel(online ? '等待分配席位' : '1P');
const controls = mountControls({canvas: $('canvas'), getEmulator: () => emulator, getNetwork: () => netplay,
  networkMode:online||controlsOnly,
  dialog: $('settings'), onGesture: () => { clearEscape(); resumeAudio(); }, onOtherInput: clearEscape,
  onPause: openPause, onMenu: navigatePause, onEscape: handleEscape,
  onFocus: (mask, points) => nativePause?.setFocus(mask, points),
  getTouch: () => player, getRoomFocus: () => roomFocusEnabled, getRoomUnlimited: () => !online||roomTouchUnlimitedAllowed, onTouch: inputs => nativePause?.setTouch(inputs),
  isPaused: () => pauseSeat !== null, inGameplay});
const markerLayer=document.createElement('div');markerLayer.className='hit-points';
const markerNodes=[0,1].map(()=>{const node=document.createElement('i');node.hidden=true;markerLayer.append(node);return node;});
viewport.append(markerLayer);
function drawLocalPoints(){
  for(const node of markerNodes)node.hidden=true;
  if(!closed&&!closing)for(const point of nativePause?.markers()||[]){
    const settingSeat=online?0:point.seat;
    const ownSeat=netplay?.localSeat??0;
    if((online&&point.seat!==ownSeat)||!(point.focused&&controls.pointEnabled(settingSeat)||point.seat===ownSeat&&player.alwaysPoint()))continue;
    const node=markerNodes[point.seat];node.hidden=false;node.style.left=`${point.x/640*100}%`;node.style.top=`${point.y/400*100}%`;
  }
  requestAnimationFrame(drawLocalPoints);
}
requestAnimationFrame(drawLocalPoints);
if(online)mountNetworkPerformance({host:$('screen'),toolbar:document.querySelector('nav.toolbar .commands'),
  getReport:()=>({schema:2,game:'TH03',seat:netplay?.localSeat??null,language:activeLang,
    rollbackEnabled,transport:netQuery.get('net'),userAgent:navigator.userAgent,
    inputPolicy:netplay?.inputPolicy??null,startupRttSamples:netplay?.rttSamples??null,
    startupInputReports:{local:netplay?.localInputReport??null,remote:netplay?.peerInputReport??null},
    identity:syncIdentity?{protocol:syncIdentity.protocol,files:syncIdentity.files}:null,
    samples:networkHistory.read(),errors:networkEvents.read(),
    programReads:emulator?.module.programReads??[],slowSteps:emulator?.module.stepMetrics?.slowSteps??[]})});
if(online||controlsOnly){
  $('settings').querySelector('h2').textContent='本机按键与手柄';
  $('pad-0').parentElement.firstChild.textContent='本机手柄';
  $('pad-1').parentElement.hidden=true;
  $('defaults').textContent='恢复本机默认操作';
  const button=document.createElement('button');button.type='button';button.textContent='自定义键盘 / 手柄';button.dataset.networkControls='';
  button.onclick=()=>$('controls').click();$('screen').querySelector('.player-toolbar').append(button);
  if(online){const direct=button.cloneNode(true);direct.textContent='键盘 / 手柄';direct.className='network-controls-open';direct.removeAttribute('data-network-controls');direct.onclick=()=>$('controls').click();$('screen').append(direct);}
  for(let seat=0;seat<2;seat++){
    const checkbox=$(`focus-enabled-${seat}`);checkbox.disabled=true;
    checkbox.closest('label').title='低速模式由房主在房间中统一设置';
  }
  $('focus-points-1').closest('fieldset').hidden=true;
  $('focus-points-0').closest('fieldset').querySelector('legend').textContent='本机玩家';
  document.querySelector('.control-hint').textContent='设置仅保存到本机，P1 / P2 均使用这套操作。默认空格、手柄 RB / R1 低速；左摇杆始终用于移动。手柄请先按任意键激活，浏览器手柄功能需 HTTPS 或 localhost。低速开关由房主统一设置。';
}
function localizePause(){
  const cn=(activeLang||$('language').value)==='cn';
  $('pause-menu').lang=cn?'zh-CN':'ja';
  $('pause-title').textContent=`${(pauseSeat??0)+1}P ${cn?'暂停':'一時停止'}`;
  pauseButtons().forEach((button,i)=>{
    const label=document.createElement('span');label.className='pause-label';
    label.textContent=(cn?['继续','本局认输','背水阵']:['再開','ラウンド降参','背水の陣'])[i];
    button.replaceChildren(label);
  });
}
$('language').addEventListener('change',localizePause);localizePause();
try {
  const difficulty = localStorage.getItem('th03-local-difficulty-v2');
  if (/^[0-3]$/.test(difficulty)) $('difficulty').value = difficulty;
} catch {}
let restartOnLoad = false;
try {
  const launch = JSON.parse(sessionStorage.getItem('th03-local-restart'));
  sessionStorage.removeItem('th03-local-restart');
  if (launch && ['jp', 'cn'].includes(launch.lang) && /^[0-3]$/.test(launch.rank)
    && ['8', '16', '24', '32'].includes(launch.clock)) {
    $('language').value = launch.lang; $('difficulty').value = launch.rank; $('clock').value = '32';
    restartOnLoad = true;
  }
} catch {}
$('difficulty').addEventListener('change', () => {
  try { localStorage.setItem('th03-local-difficulty-v2', $('difficulty').value); } catch {}
});

function status(text, error = false) {
  $('status').textContent = text; $('status').dataset.error = String(error);
  if (starting) notifyRoom(text, false, error);
}
function report(error) {
  logError('reported',error);
  replayRecorder?.finish(String(error.message||error));sync?.stop();
  console.error(error); status(`错误：${error.message || error}`, true);
  if (online) { emulator?.pause(); controls.release(); notifyRoom(`错误：${error.message || error}`, false, true); }
}
async function resumeAudio() {
  try { await emulator?.module.SDL2?.audioContext?.resume(); } catch {}
}
async function metadata(lang) {
  if (!manifest) {
    const response = await fetch('disks/manifest.json');
    if (!response.ok) throw Error(`镜像清单加载失败 (${response.status})`);
    manifest = await response.json();
  }
  const meta = manifest.games[`3-${lang}`];
  if (!meta) throw Error('找不到梦时空镜像');
  return meta;
}
function validate(data, meta) {
  const decode = (at, length) => new TextDecoder().decode(data.subarray(at, at + length));
  if (data.length !== meta.size || decode(meta.autoexecSizeOffset - 28, 11) !== 'AUTOEXECBAT') throw Error('存档镜像结构不匹配');
  if (decode(meta.languageMarkerOffset, 8) !== meta.languageMarker) throw Error('存档与所选语言版本不匹配');
}
async function prepareDisk(data, meta, lang = activeLang || $('language').value) {
  validate(data, meta);
  unlockDiskScores(new Fat12(data));
  await installNativePause(data, lang);
  if(online)networkAudio=await installNetworkAudio(data);
  const script = new TextEncoder().encode('@ECHO OFF\r\nPATH A:\\DOS;A:\\\r\nSET TEMP=A:\\DOS\r\nSET DOSDIR=A:\\DOS\r\nA:\r\nCD \\YUMEZIKU\r\nCALL GAME.BAT\r\n');
  if (script.length > meta.autoexecCapacity) throw Error('启动脚本空间不足');
  data.fill(0, meta.autoexecOffset, meta.autoexecOffset + meta.autoexecCapacity);
  data.set(script, meta.autoexecOffset);
  new DataView(data.buffer, data.byteOffset, data.byteLength).setUint32(meta.autoexecSizeOffset, script.length, true);
  // TH03 shares snd_active between BGM commands and sound-driver routing.
  // Keep FM command routing enabled; WEBMUSIC masks PMD music parts 0..14
  // to zero output and leaves part 15 (SE) enabled, as TH04's PMD bridge does.
  const cfg = meta.keyboardConfig;
  if (cfg.size !== 8 || cfg.offset < 0 || cfg.offset + cfg.size > data.length) throw Error('梦时空键盘配置位置无效');
  data[cfg.offset + 0] = 1; // PMD commands enabled, music output masked by TSR
  data[cfg.offset + 1] = 0; // KM_KEY_KEY
  const difficulty = Number($('difficulty').value);
  if (!Number.isInteger(difficulty) || difficulty < 0 || difficulty > 3) throw Error('难度设置无效');
  data[cfg.offset + 2] = difficulty;
  return data;
}
function openDatabase() {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open('th03-fast-local-lab', 1);
    request.onupgradeneeded = () => request.result.createObjectStore('disks');
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error);
    request.onblocked = () => reject(Error('存档数据库被占用'));
  });
}
async function stored(lang, mode, value) {
  database ||= await openDatabase();
  return new Promise((resolve, reject) => {
    const transaction = database.transaction('disks', mode), store = transaction.objectStore('disks');
    const request = mode === 'readonly' ? store.get(lang) : store.put(value, lang);
    transaction.oncomplete = () => resolve(request.result);
    transaction.onerror = () => reject(transaction.error || request.error);
    transaction.onabort = () => reject(transaction.error || Error('存档事务已取消'));
  });
}
async function loadDisk(lang) {
  const meta = await metadata(lang);
  if (!online) {
    try {
      const disk = await stored(lang, 'readonly');
      if (disk) { status('正在读取本地存档'); return prepareDisk(new Uint8Array(disk), meta); }
    } catch (error) {
      storageWarning = `存档读取受限：${error.message}`;
      $('save-status').textContent = storageWarning;
    }
  }
  status('正在加载梦时空镜像');
  const prepared=online&&roomHost?.preloadPromise?await roomHost.preloadPromise:null;
  const data = prepared?.language===lang?prepared.diskBytes.slice():await loadVerifiedBytes(meta.url, meta.sha256, {gzip: true});
  return prepareDisk(data, meta);
}
function diskChanged(name) {
  if (online) return;
  if (!activeLang || !name.endsWith(`3-${activeLang}.hdi`)) return;
  dirty++; $('save-status').textContent = '等待保存';
  clearTimeout(saveTimer);
  saveTimer = setTimeout(() => { saveTimer = null; save(); }, 1500);
}
async function save() {
  if (saveTask) return saveTask;
  if (!emulator || saved === dirty) return;
  saveTask = (async () => {
    try {
      while (saved !== dirty) {
        const version = dirty, disk = await emulator.getDiskImage(`3-${activeLang}.hdi`);
        await stored(activeLang, 'readwrite', disk); saved = version;
      }
      storageWarning = ''; $('save-status').textContent = '已保存';
    } catch (error) { $('save-status').textContent = `保存失败：${error.message}`; }
    finally { saveTask = null; }
  })();
  return saveTask;
}
function reflectState() {
  if (closed || closing) return;
  const running = emulator?.state === 'running';
  $('reset').disabled = online || !!nativePause?.pending;
  if (emulator) status(emulator.state === 'exited' ? '游戏已退出' : running ? '运行中' : '已暂停');
}
function pauseButtons() { return Array.from($('pause-menu').querySelectorAll('button')); }
function refreshPauseMenu() {
  const buttons = pauseButtons(), allowed = nativeState?.phase === 1 && !nativePause?.pending;
  buttons[1].disabled = buttons[2].disabled = !allowed;
  if (buttons[pauseSelection].disabled) pauseSelection = 0;
  buttons.forEach((button, i) => { button.dataset.selected = String(i === pauseSelection); });
  const cn=(activeLang||$('language').value)==='cn';
  $('pause-help').textContent=(cn?['返回对局','放弃当前回合','进入背水阵']:['ゲームに戻る','このラウンドを降参','背水の陣に入る'])[pauseSelection];
}
function openPause(seat) {
  if(enqueue('pause'))return;
  if(netplay&&!sync?.applying)seat=netplay.localSeat;
  if (closed || closing || !emulator || emulator.state === 'exited' || nativePause?.pending || pauseSeat !== null || (!online && $('settings').open)) return;
  nativeState = nativePause?.update(nativeNow());
  if (!nativeState?.phase) return;
  clearEscape();
  controls.release(); visibilityPaused = false; emulator.pause(); save();
  pauseSeat = seat; pauseSelection = 0;
  $('pause-menu').dataset.seat = String(seat);
  localizePause();
  $('pause-message').textContent = '';
  $('pause-menu').hidden = $('pause-shade').hidden = false;
  refreshPauseMenu(); reflectState(); pauseButtons()[0].focus();
}
function closePause() {
  if(enqueue('resume'))return;
  controls.release(); pauseSeat = null;
  $('pause-menu').hidden = $('pause-shade').hidden = true;
  visibilityPaused = document.hidden;
  if ((online || !document.hidden) && emulator?.state !== 'exited') emulator.run();
  reflectState(); $('canvas').focus(); resumeAudio();
}
function choosePause(choice) {
  if(enqueue(choice))return;
  if (pauseSeat === null) return;
  if (choice === 'resume') { closePause(); return; }
  if (!nativePause?.request(choice === 'round' ? 1 : 2, pauseSeat,nativeNow())) {
    $('pause-message').textContent = activeLang==='cn'?'当前回合无法认输。':'このラウンドでは降参できません。';
    nativeState = nativePause?.update(nativeNow()); refreshPauseMenu(); return;
  }
  closePause();
}
function navigatePause(seat, action) {
  if(enqueue(action))return;
  if (netplay && !sync?.applying && seat !== null) seat = seat === 0 ? netplay.localSeat : -1;
  if (pauseSeat === null || (seat !== null && seat !== pauseSeat)) return;
  const buttons = pauseButtons();
  if (action === 'resume') { closePause(); return; }
  if (action === 'confirm') { buttons[pauseSelection].click(); return; }
  if (!['up', 'down'].includes(action)) return;
  const step = action === 'up' ? -1 : 1;
  do { pauseSelection = (pauseSelection + step + buttons.length) % buttons.length; } while (buttons[pauseSelection].disabled);
  refreshPauseMenu(); buttons[pauseSelection].focus();
}
pauseButtons().forEach((button, i) => {
  button.addEventListener('click', () => choosePause(button.dataset.choice));
  button.addEventListener('focus', () => { if (!online || sync?.applying) { pauseSelection = i; refreshPauseMenu(); } });
});
// Clicking away from a pause button must not strand keyboard input on the
// canvas, touch overlay or iframe body. Restore focus without changing selection.
player.stage.addEventListener('pointerdown',event=>{
  if(pauseSeat===null||event.target.closest('#pause-menu'))return;
  event.preventDefault();event.stopPropagation();pauseButtons()[pauseSelection].focus();
},true);
window.addEventListener('focus',()=>{
  if(pauseSeat!==null&&!$('settings').open)pauseButtons()[pauseSelection].focus();
});
function clearEscape() { escapeAt = -Infinity; clearTimeout(escapeTimer); }
function handleEscape() {
  if(enqueue('escape'))return;
  if (closed || closing || !emulator || emulator.state !== 'running') return;
  nativeState = nativePause?.update(nativeNow());
  if (nativeState?.phase) { openPause(0); return; }
  if (!nativePause?.selecting) return;
  const now = performance.now();
  if (now - escapeAt <= 1000) { clearEscape(); closeGame(); return; }
  escapeAt = now;
  status('再按一次 Esc 关闭游戏');
  escapeTimer = setTimeout(() => { clearEscape(); reflectState(); }, 1000);
}
async function closeGame() {
  if(online){returnRoom();return;}
  closing = true; controls.release(); visibilityPaused = false;
  emulator.pause(); clearTimeout(saveTimer); saveTimer = null;
  localBgm?.dispose(); localBgm = null;
  localSfx?.dispose(); localSfx = null;
  try { await emulator.module.SDL2?.audioContext?.suspend(); } catch {}
  await save();
  closed = true; closing = false; nativePause.reset(); nativeState = null; pauseSeat = null;
  $('pause-menu').hidden = $('pause-shade').hidden = true;
  $('cover').hidden = false; $('start').disabled = false; $('start').textContent = '启动游戏';
  for (const id of ['language', 'difficulty', 'clock', 'import']) $(id).disabled = false;
  $('reset').disabled = true;
  status(saved === dirty ? '游戏已关闭' : '游戏已关闭；存档保存失败，可导出备份', saved !== dirty); $('start').focus();
}
function trackNativePause() {
  if (!online && !closed && !closing && nativePause && emulator?.state !== 'exited') {
    nativeState = nativePause.update();
    if (!emulator.isWorker && nativeMusic && localBgm) {
      for (const [ax,hash] of nativeMusic.read(emulator.module.HEAPU8)) localBgm.command(ax,hash);
    }
    if (!nativePause.selecting && escapeAt !== -Infinity) { clearEscape(); reflectState(); }
    const result = nativePause.result();
    if (result === 'accepted') reflectState();
    if (result === 'rejected') {
      reflectState(); status('降参指令未被游戏接受，请重新暂停后再试。', true);
    }
    if (result === 'unknown') {
      reflectState(); status('无法确认降参指令的结果，请查看当前游戏画面。', true);
    }
    if (pauseSeat !== null) refreshPauseMenu();
  }
  player.setActive(!closed && !closing && emulator?.state === 'running' && pauseSeat === null && !$('settings').open);
  player.setContext({key: nativeState?.phase ? `round:${nativeState.generation}:${nativeState.phase}` : 'select:menu',
    play: nativeState?.phase === 1 && pauseSeat === null});
  setTimeout(trackNativePause, 100);
}
trackNativePause();
function stepSynchronized(inputs,frame,replay=false){
  emulator.module.netBeginFrame(frame,replay);
  try{
    syncReplaying=replay;
    syncFrame=frame;
    nativeState=nativePause.update(nativeNow(),nativeMusic.dosRange(emulator.module.HEAPU8));
    for(let seat=0;seat<2;seat++)for(const command of inputs[seat].commands){
      if(command==='hidden'){hiddenSeats.add(seat);continue;}
      if(command==='visible'){hiddenSeats.delete(seat);continue;}
      if(command==='pause'||command==='escape'){
        if(pauseSeat===seat)closePause();else if(pauseSeat===null)openPause(seat);
      }else if(pauseSeat===seat){
        if(['round','match','resume'].includes(command))choosePause(command);
        else navigatePause(seat,command);
      }
    }
    const paused=pauseSeat!==null||hiddenSeats.size>0;
    const applied=paused?inputs.map(input=>({...input,actions:[],touch:0})):inputs;
    controls.applyFrame(applied);
    if(!paused){
      emulator.run();emulator.step();
      confirmedMusic?.stage(frame,nativeMusic.read(emulator.module.HEAPU8));
      nativeState=nativePause.update(nativeNow(),nativeMusic.dosRange(emulator.module.HEAPU8));
      nativePause.result(nativeNow());
    }else emulator.pause();
    if(pauseSeat!==null&&!replay)refreshPauseMenu();
    window.th03SyncState={frame:frame+1,tick:emulator.module.netInfo().tick,phase:nativeState?.phase??0,
      selecting:nativePause.selecting,pauseSeat,hidden:[...hiddenSeats]};
  }finally{
    emulator.module.netEndFrame();
    syncReplaying=false;
  }
}
function captureSynchronized(frame){
  if(syncSnapshots.has(frame))return;
  emulator.module.netCapture(frame);
  syncSnapshots.set(frame,{app:{pauseSeat,pauseSelection,hidden:[...hiddenSeats],visibilityPaused,nativePause:nativePause.capture(),music:nativeMusic.capture(),controls:controls.capture()}});
}
function restoreSynchronized(frame){
  const saved=syncSnapshots.get(frame);if(!saved)throw Error(`同步快照已过期：${frame}`);
  emulator.module.netRestore(frame);lastUpload=-1;const state=saved.app;
  pauseSeat=state.pauseSeat;pauseSelection=state.pauseSelection;hiddenSeats.clear();for(const seat of state.hidden)hiddenSeats.add(seat);
  visibilityPaused=state.visibilityPaused;nativePause.restore(state.nativePause);nativeMusic.restore(state.music);confirmedMusic.rewind(frame);controls.restore(state.controls);
  localizePause();
  $('pause-menu').hidden=$('pause-shade').hidden=pauseSeat===null;
  if(pauseSeat===null)emulator.run();else emulator.pause();
  for(const key of [...syncSnapshots.keys()])if(key>=frame)syncSnapshots.delete(key);
  return frame;
}
function confirmSynchronized(prefix,retainFrom=prefix){
  confirmedMusic?.confirm(prefix);
  emulator.module.netConfirm?.(prefix,retainFrom);
  for(const key of [...syncSnapshots.keys()])if(key<retainFrom)syncSnapshots.delete(key);
}
function syncHash(){
  const heap=emulator.module.HEAPU8;let h=2166136261;
  // Match TH04: verify all 640 KiB of DOS conventional memory instead of
  // synchronously hashing the complete DOSBox-X heap every two seconds.
  // Keep full-heap checking available for emulator regression tests.
  const range=nativeMusic?.dosRange(heap);
  const full=netQuery.get('fullHash')==='1';
  if(!full&&!range)throw Error('DOS 常规内存校验范围尚未绑定');
  const start=full?0:range.start,end=full?heap.length:range.end;
  if(emulator.module.netHash)h=emulator.module.netHash(start,end);
  else for(let i=start;i<end;i++)h=Math.imul(h^heap[i],16777619);
  return (h>>>0).toString(16).padStart(8,'0')+':'+emulator.module.netInfo().tick+':'+pauseSeat+':'+[...hiddenSeats].sort().join(',');
}
function reflectNetworkSnapshot(snapshot){
  const oldPause=pauseSeat;nativeState=snapshot.nativeState;
  pauseSeat=snapshot.pauseSeat;pauseSelection=snapshot.selection;hiddenSeats.clear();for(const seat of snapshot.hidden||[])hiddenSeats.add(seat);
  $('pause-menu').hidden=$('pause-shade').hidden=pauseSeat===null;
  if(pauseSeat!==null){$('pause-menu').dataset.seat=String(pauseSeat);localizePause();refreshPauseMenu();}
  if(oldPause!==pauseSeat){controls.release();if(pauseSeat!==null)pauseButtons()[pauseSelection].focus();reflectState();}
  window.th03SyncState={frame:sync?.queue.frame??0,tick:snapshot.tick,phase:nativeState?.phase??0,selecting:snapshot.selecting,pauseSeat,hidden:[...hiddenSeats]};
}
$('start').addEventListener('click', async () => {
  if (starting || (emulator && !closed) || closing) return;
  if (closed) {
    // A fresh page releases SDL callbacks, WASM and audio before a new boot.
    try { sessionStorage.setItem('th03-local-restart', JSON.stringify({lang: $('language').value,
      rank: $('difficulty').value, clock: $('clock').value})); } catch {}
    location.reload(); return;
  }
  starting = true;
  for (const id of ['start', 'language', 'difficulty', 'clock', 'import']) $(id).disabled = true;
  $('start').textContent = '正在启动';
  try {
    if (online) {
      status('正在读取房间开局设置');
      const query = new URLSearchParams({protocol: 'th03-lan/1', room: netQuery.get('room'), token: netQuery.get('token')});
      const response = await fetch(`/api/state?${query}`, {cache: 'no-store'});
      const room = await response.json();
      if (!response.ok || !room.started || room.runId!==Number(netQuery.get('runId')) || room.mode !== netQuery.get('net')) throw Error(room.error || '房间尚未开始或本局编号已失效');
      for (const id of ['language', 'difficulty', 'clock']) $(id).value = String(room.settings[id]);
      rollbackEnabled=room.settings.rollback===true;
      roomFocusEnabled=room.settings.focusEnabled!==false;
      roomTouchUnlimitedAllowed=room.settings.touchUnlimitedAllowed===true;
      roomHostSeat=room.settings.hostSeat===1?1:0;
      player.setLabel(`${(roomHostSeat^(netQuery.get('role')==='guest'?1:0))+1}P`);
      player.setUnlimitedAllowed(roomTouchUnlimitedAllowed);
      for(let seat=0;seat<2;seat++)$(`focus-enabled-${seat}`).checked=roomFocusEnabled;
    }
    activeLang = $('language').value;
    localizePause();
    const networkTask = online?(async()=>{
      if(roomHost?.preloadPromise){const prepared=await roomHost.preloadPromise;return {...prepared,soundAssets:new Map(prepared.soundAssets)};}
      const response=await fetch('lockstep-runtime.json',{cache:'no-store'});
      if(!response.ok)throw Error('同步引擎清单缺失');
      const runtime=await response.json();
      const soundAssets=new Map();
      const wasmBinary=await loadNetworkAssets(runtime,message=>status(message),{rollbackEnabled,
        onAsset:(path,bytes,sha256)=>{if(path.startsWith('sfx/'))soundAssets.set(path,{bytes,sha256});}});
      return {runtime,wasmBinary,soundAssets};
    })():loadLocalCore().then(wasmBinary=>({wasmBinary}));
    const [disk,networkAssets]=await Promise.all([loadDisk(activeLang),networkTask]);
    const wasmBinary=networkAssets?.wasmBinary;
    if(online){
      const {runtime}=networkAssets;
      syncIdentity={protocol:runtime.protocol,files:runtime.files,disk:await sha256(disk),hostSeat:roomHostSeat,
        language:activeLang,difficulty:$('difficulty').value,clock:$('clock').value,rollback:rollbackEnabled,focusEnabled:roomFocusEnabled,touchUnlimitedAllowed:roomTouchUnlimitedAllowed};
    }
    nativePause = new NativePause(() => emulator, (await nativeAssets()).meta);
    nativeState = null; clearEscape();
    status('正在启动 DOSBox-X');
    const config={canvas:online?nativeCanvas:visibleCanvas,lockstep:online,independentSfx:online,rollbackEnabled,wasmBinary,lockstepEpoch:946684800000,onError:report,
      DIPswtch: [0x3e, 0xf3, 0x7b], ExMemory: 7, Latencys: online?40:100, SampleHz: 44100, SNDboard: 4,
      no_mouse: true, use_menu: false, fontfile: activeLang === 'cn' ? 'font_cn.bmp' : 'font.bmp',
      onDiskChange: diskChanged, onExit: () => {
        if(online){returnRoom();return;}
        localBgm?.stop();
        localSfx?.stop();
        controls.release(); pauseSeat = null; nativeState = null;
        $('pause-menu').hidden = $('pause-shade').hidden = true;
        nativePause.reset(); reflectState(); save();
      }};
    const useWorker=!online&&netQuery.get('runtime')!=='main'&&supportsSoloWorker();
    // Keep simulation off the page by default, with an explicit main-thread
    // compatibility option using the same resources and input policy.
    emulator=useNetworkWorker?await WorkerNetworkSimulation.create(config,{patch:nativePause.meta,music:networkAudio,
      focusEnabled:roomFocusEnabled,touchUnlimitedAllowed:roomTouchUnlimitedAllowed,fullHash:netQuery.get('fullHash')==='1'})
      :useWorker?await WorkerNP21.create(config,{game:'03local',patch:nativePause.meta,music:(await nativeAssets()).meta.startup.music,onError:report}):await DOSBoxPC98.create(config);
    if(emulator.isWorker)nativePause=emulator.isNetworkWorker?workerNetworkPause(emulator):workerLocalPause(emulator);
    if(emulator.isNetworkWorker){
      emulator.onSnapshot=reflectNetworkSnapshot;emulator.onExit=()=>{replayRecorder?.finish('游戏已退出');sync?.stop();returnRoom();};
      emulator.onDraw=bitmap=>{const began=performance.now();if(bitmapContext)bitmapContext.transferFromImageBitmap(bitmap);else{visibleContext.imageSmoothingEnabled=false;visibleContext.drawImage(bitmap,0,0,640,400);}presentationCount++;presentationMs+=performance.now()-began;};
    }
    localBgm = new LocalBgm(emulator.module.SDL2.audioContext, () => {});
    nativeMusic = new NativeMusic(online?networkAudio:(await nativeAssets()).meta.startup.music);
    if(online){
      localSfx=new LocalSfx(emulator.module.SDL2.audioContext);
      // Load every small effect before simulation: no downloads/decoding on a
      // shot, and no muted-PCM game if any local effect failed to load.
      await localSfx.load(networkAssets.soundAssets);
      networkAssets.soundAssets.clear();
    }
    confirmedMusic = new ConfirmedMusic({command:(ax,hash)=>{
      if((ax>>>8)===0x0c)localSfx?.command(ax);else localBgm.command(ax,hash);
    }});
    if(emulator.isNetworkWorker)emulator.onAudio=(ax,hash)=>{if((ax>>>8)===0x0c)localSfx?.command(ax);else localBgm?.command(ax,hash);};
    else if(emulator.isWorker)emulator.onMusic=(ax,hash)=>localBgm?.command(ax,hash);
    // The device decodes music asynchronously; downloads cannot stall game start.
    const loadingBgm=localBgm;
    loadingBgm.load({warmOpening:true}).catch(error => {
      if(loadingBgm.closed)return;
      loadingBgm.status = `本地音乐错误：${error.message}`;
      console.error(error);
    });
    await emulator.addDiskImage(`3-${activeLang}.hdi`, disk);
    await emulator.setHdd(0, `3-${activeLang}.hdi`);
    let bootMusic=[];
    if(online){
      const progress=message=>{status(message);notifyRoom(message);};
      if(emulator.isNetworkWorker){const boot=await emulator.boot(progress);bootMusic=boot.events;syncIdentity.boot=boot.hash;controls.release();}
      else{bootMusic=await bootToSelection({emulator,pause:nativePause,music:nativeMusic,clearInput:()=>controls.release(),progress});syncIdentity.boot=syncHash();}
    }
    if(roomHost?.stopped)throw Error('房间已结束');
    if (online) {
      netplay = new Netplay({room: netQuery.get('room'), token: netQuery.get('token'), role: netQuery.get('role'), mode: netQuery.get('net'),runId:Number(netQuery.get('runId')),sampleLocal:controls.sampleLocal,
        onMetrics: snapshot => {
          if(sync?.queue){snapshot.bufferFrames=sync.queue.active?sync.queue.inputDelay:sync.queue.bootDelay;snapshot.bufferMs=snapshot.bufferFrames*1000/60;}
          publishNetworkStatus(snapshot);
          const message={protocol:'th03-lan/1',event:'network-status',snapshot};
          if(roomHost)roomHost.notify(message);
          else if(embedded)window.parent.postMessage(message,location.origin);
        },
        onState: message => {
          $('connection-path').hidden=false;$('connection-path').textContent=`本机连接：${message}`;
          if(roomHost)roomHost.notify({protocol:'th03-lan/1',event:'connection',message});
          else if(embedded)window.parent.postMessage({protocol:'th03-lan/1',event:'connection',message},location.origin);
        }, onError: error => report(error)});
      emulator.pause();
      status('本机资源已就绪，等待另一位玩家完成加载');
      await netplay.start();
      status('双方已连接，正在校验开局状态');
      await netplay.synchronize(syncIdentity);
      status('正在确认双方输入缓冲');
      const inputPolicy=await netplay.negotiateInputPolicy({rollbackEnabled});
      replayRecorder=createRecorder($('screen').querySelector('.player-toolbar'));globalThis.th03Replay=replayRecorder;
      await replayRecorder.boot({settings:{language:activeLang,difficulty:$('difficulty').value,clock:$('clock').value,rollback:rollbackEnabled,focusEnabled:roomFocusEnabled,touchUnlimitedAllowed:roomTouchUnlimitedAllowed,hostSeat:roomHostSeat},players:2,hostSlot:roomHostSeat,opening:syncIdentity});
      if(netQuery.get('role')==='host')replayRecorder.connectNetwork({room:netQuery.get('room'),token:netQuery.get('token'),runId:Number(netQuery.get('runId'))});
      const options={recorder:replayRecorder,proof:()=>battleProof(emulator,nativePause,controls,nativeMusic,pauseSeat,pauseSelection,hiddenSeats),network:netplay,rollbackEnabled,inputDelay:inputPolicy.inputDelay,predictionLead:inputPolicy.predictionLead,snapshotStride:2,onError:report,onState:message=>status(message)};
      sync=emulator.isNetworkWorker?new AsyncLockstep({...options,execute:(inputs,frame)=>emulator.execute(inputs,frame),replaySlice:(frame,plans)=>emulator.replaySlice(frame,plans),restore:frame=>emulator.restore(frame),
        confirmQueued:(prefix,retain)=>emulator.confirmQueued(prefix,retain),present:()=>emulator.present(presentationDue)})
        :new Lockstep({...options,step:stepSynchronized,present:()=>{if(presentationDue(performance.now()))presentFrame();},hash:syncHash,capture:captureSynchronized,restore:restoreSynchronized,confirm:confirmSynchronized});
    }
    // Boot can contain menu SE while the other device is still loading.
    // Keep its BGM commands; do not play those old effects at room start.
    for(const [ax,hash]of bootMusic)if((ax>>>8)!==0x0c)localBgm.command(ax,hash);
    if(roomHost?.stopped)throw Error('房间已结束');
    closed = false; dirty = saved = 0;
    $('cover').hidden = true; emulator.run(); $('canvas').focus(); resumeAudio();
    if(online)sync.start();
    $('reset').disabled = online; $('export').disabled = false;
    reflectState();
    if (!storageWarning) $('save-status').textContent = online ? '联机镜像' : '存档已就绪';
    if (online) notifyRoom('游戏已启动', true);
  } catch (error) {
    if(emulator?.isNetworkWorker)emulator.dispose();else emulator?.pause();
    localBgm?.dispose();
    localSfx?.dispose();localSfx=null;
    netplay?.close(); netplay = null; report(error); $('start').textContent = '刷新重试'; $('start').disabled = false;
    $('start').onclick = () => location.reload();
  } finally { starting = false; }
});
const embeddedRuntime={openControls:()=>$('controls').click(),stop(){if(roomHost)roomHost.stopped=true;closed=true;controls.release();sync?.stop();netplay?.close();localBgm?.dispose();localSfx?.dispose();if(emulator?.isNetworkWorker)emulator.dispose();else emulator?.pause();}};
if(roomHost)roomHost.runtime=embeddedRuntime;
if(embedded)window.th03EmbeddedRuntime=embeddedRuntime;
if (!controlsOnly&&(restartOnLoad || embedded || roomHost)) $('start').click();
$('reset').addEventListener('click', async () => {
  if (online || closed || closing || nativePause?.pending) return;
  if (!confirm('重新启动梦时空？当前对局会结束。')) return;
  controls.release(); emulator.pause(); await save();
  netplay?.close(); netplay = null;
  if (saved !== dirty && !confirm('存档未成功保存，仍要重新启动？')) { reflectState(); return; }
  clearEscape(); pauseSeat = null; nativeState = null; nativePause.reset();
  $('pause-menu').hidden = $('pause-shade').hidden = true;
  localBgm?.stop(); nativeMusic?.reset(); confirmedMusic?.frames.clear();
  visibilityPaused = false; await emulator.reset(); emulator.run(); reflectState(); $('canvas').focus(); resumeAudio();
});
$('controls').addEventListener('click', () => { controls.release(); if(online)$('screen').append($('settings')); $('settings').showModal(); });
if(controlsOnly){
  document.body.classList.add('controls-only');
  $('settings').querySelector('form').hidden=true;
  $('settings').show();
}
window.addEventListener('message',event=>{
  if(embedded&&event.source===window.parent&&event.origin===location.origin&&event.data?.protocol==='th03-lan/1'&&event.data.event==='open-controls')$('controls').click();
});
$('fullscreen').addEventListener('click', async () => {
  if ($('screen').classList.contains('immersive')) await player.exit(); else await player.enter();
});
$('canvas').addEventListener('pointerdown', () => { clearEscape(); $('canvas').focus(); resumeAudio(); });
$('canvas').addEventListener('contextmenu', event => event.preventDefault());
$('export').addEventListener('click', async () => {
  try {
    const url = URL.createObjectURL(new Blob([await emulator.getDiskImage(`3-${activeLang}.hdi`)], {type: 'application/octet-stream'}));
    const link = document.createElement('a'); link.href = url;
    link.download = `th03-${activeLang}-${new Date().toISOString().slice(0, 10)}.hdi`; link.click();
    setTimeout(() => URL.revokeObjectURL(url), 10000);
  } catch (error) { report(error); }
});
$('import').addEventListener('change', async event => {
  const file = event.target.files[0]; if (!file) return;
  try {
    if ((emulator && !closed) || starting || closing) throw Error('请在启动前导入存档');
    const lang = $('language').value, meta = await metadata(lang);
    if (file.size !== meta.size) throw Error('存档镜像大小不匹配');
    const disk = await prepareDisk(new Uint8Array(await file.arrayBuffer()), meta, lang);
    if (!confirm('覆盖当前语言版本的本地存档？')) return;
    await stored(lang, 'readwrite', disk); status('存档已导入');
  } catch (error) { report(error); }
  finally { event.target.value = ''; }
});
document.addEventListener('visibilitychange', () => {
  clearEscape();
  if (closed || closing) return;
  if(online&&netplay){controls.release();netplay.command(document.hidden?'hidden':'visible');return;}
  if (document.hidden) {
    controls.release(); visibilityPaused = emulator?.state === 'running';
    if (visibilityPaused) emulator.pause();
    save();
  } else if (visibilityPaused) { visibilityPaused = false; if (pauseSeat === null) emulator?.run(); }
  reflectState();
});
window.addEventListener('blur', clearEscape);
window.addEventListener('beforeunload', event => {
  if (emulator && dirty !== saved) { event.preventDefault(); event.returnValue = ''; }
});
window.addEventListener('pagehide', () => { sync?.stop();netplay?.close(); localBgm?.dispose();localSfx?.dispose();if(emulator?.isNetworkWorker)emulator.dispose(); });
