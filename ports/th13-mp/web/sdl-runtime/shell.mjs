import {installStartupBranding, finishStartupAnimation} from './startup-branding.mjs';
import createModule from './th13-sdl.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {scanCodes} from './keyboard.mjs';
import {runtimeAssets} from './runtime-assets.mjs';

const runtimeBuild = /*TH13_BUILD_INFO*/{version: 'development-incomplete', completeGame: false};
const game = 'th13', protocol = 'eagler-touhou/1', query = new URLSearchParams(location.search), canvas = document.querySelector('canvas'), $ = s => document.querySelector(s);
const epoch = Number(query.get('runtimeEpoch'));
const validEpoch = Number.isSafeInteger(epoch) && epoch > 0;
let core, launched = false, first = false, stopping = false, options = {}, music = true, queue = Promise.resolve();
let multiplayerBridge = null;
let spectatorPlayback = null;
// 房间预览（launcher prepare-room）：正式多人页在房间内先呈现标题面，
// launch 时停掉预览泵再交给桥进局（th15 shell 同型）。
let roomPreview = false, roomOwned = false, launchSerial = 0;
const emit = (event, fields = {}) => parent.postMessage({protocol, game, epoch, event,
  ...((event === 'first-frame' || event === 'room-menu') ? {launchSerial} : {}), ...fields}, location.origin);
// 触摸坐标换算的 canvas 包围盒缓存：resize/视口变化时失效，避免每次
// pointermove 消息都强制布局（getBoundingClientRect）。
let canvasBox = null;
const invalidateCanvasBox = () => { canvasBox = null; };
addEventListener('resize', invalidateCanvasBox);
visualViewport?.addEventListener?.('resize', invalidateCanvasBox);
const canvasRect = () => canvasBox ??= canvas.getBoundingClientRect();
const keyboard = createBrowserKeyboard({
  hostContext: {
    protocol,
    game,
    epoch,
    origin: location.origin,
    ready: () => launched && !document.hidden && !stopping,
    reset: clearKeyboard,
  },
  accept: code => !!scanCodes[code],
  send: (code, down) => core?._th13_key(scanCodes[code], +down),
});
function clearKeyboard() {
  keyboard.clear();
  core?._th13_keys_clear();
}
// BEGIN GENERATED SPECTATOR PLAYBACK
// Generated from eagler-common/browser/spectator-playback.mjs.
// Native playback owns every cursor and restore. The shell only transports
// authenticated Launcher commands and publishes native reports.
function createSpectatorPlaybackBridge({active,memory,state,control,allocate,deallocate,emit,
  replay=()=>globalThis.__eaglerPeerTransport?.spectatorReplay,
  complete,
  schedule=globalThis.setInterval,cancel=globalThis.clearInterval}) {
  let timer=null,last=null,menuSession=null;
  const clear=()=>{if(last){last=null;emit('runtime-info',{spectatorPlayback:null});}};
  const closeMenu=()=>{if(menuSession){menuSession=null;globalThis.__eaglerSpectatorMenu=false;emit('runtime-info',{spectatorMenu:false});}};
  function poll() {
    if(!active()){closeMenu();clear();return null;}
    const file=replay();
    if(menuSession){clear();return null;}
    try {
      const pointer=state();
      if(!pointer){clear();return null;}
      const heap=memory();
      if(!(heap instanceof Uint8Array)||!Number.isInteger(pointer)||pointer<1||pointer>=heap.length)
        throw Error('Invalid native spectator report address');
      const limit=Math.min(heap.length,pointer+131072);let end=pointer;
      while(end<limit&&heap[end])++end;
      if(end===limit)throw Error('Native spectator report exceeds its bound');
      const report=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(pointer,end)));
      if(report?.version!==1||typeof report.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(report.run)||
         !Number.isInteger(report.revision)||report.revision<1||report.revision>0xfffffffe||
         !Array.isArray(report.nodes)||report.nodes.length>512)throw Error('Invalid native spectator report');
      if(complete&&!menuSession&&report.ended&&!report.paused&&!report.seeking&&report.position>=report.received&&
         report.run.startsWith('s'+file?.session+'_')&&file?.bytes instanceof Uint8Array){
        const pointer=allocate(file.bytes.length);
        if(!Number.isInteger(pointer)||pointer<1)throw Error('Spectator result allocation failed');
        try{const heap=memory();if(pointer>heap.length-file.bytes.length)throw Error('Invalid spectator result allocation');heap.set(file.bytes,pointer);
          if(complete(pointer,file.bytes.length)){menuSession=file.session;globalThis.__eaglerSpectatorMenu=true;emit('runtime-info',{spectatorMenu:true});}
        }finally{deallocate(pointer);}
        if(menuSession){clear();return null;}
      }
      if(last?.run===report.run&&(report.revision<=last.revision||report.received<last.received))return last;
      last=report;emit('runtime-info',{spectatorPlayback:report});return report;
    } catch(error) {
      clear();return null;
    }
  }
  function command(message) {
    if(!active())throw Error('Spectator playback is not active');
    if(typeof message?.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(message.run)||
       !['pause','play','seek','live'].includes(message.action)||
       (message.action==='seek'&&(!Number.isInteger(message.frame)||message.frame<0||message.frame>0xfffffffe)))
      throw Error('Invalid spectator playback command');
    const report=poll();
    if(!report||message.run!==report.run)throw Error('Spectator playback run has changed');
    if(message.action==='seek'&&(message.frame<report.min||message.frame>report.edge))
      throw Error('Spectator seek is outside received history');
    // Keep terminators as zero bytes. The classic inline HTML optimizer can
    // turn a JS NUL escape into a literal NUL, which HTML replaces by U+FFFD.
    const run=new TextEncoder().encode(message.run),action=new TextEncoder().encode(message.action);
    const actionOffset=run.length+1,bytes=new Uint8Array(actionOffset+action.length+1);
    bytes.set(run);bytes.set(action,actionOffset);
    const pointer=allocate(bytes.length);
    if(!Number.isInteger(pointer)||pointer<1)throw Error('Spectator command allocation failed');
    try {
      const heap=memory();
      if(pointer>heap.length-bytes.length)throw Error('Invalid spectator command allocation');
      heap.set(bytes,pointer);
      if(!control(pointer,pointer+actionOffset,message.action==='seek'?message.frame:0))
        throw Error('Spectator playback command could not be applied');
    } finally {deallocate(pointer);}
    poll();return {};
  }
  return Object.freeze({poll,command,
    start(){if(timer===null)timer=schedule(poll,250);poll();},
    stop(){if(timer!==null)cancel(timer);timer=null;closeMenu();clear();},
  });
}
// END GENERATED SPECTATOR PLAYBACK
const err = () => {
  let p = runtimeBuild.multiplayer && typeof core._th13_mp_error === 'function'
    ? core._th13_mp_error() : core._th13_error();
  if (runtimeBuild.multiplayer && !core.HEAPU8[p]) p = core._th13_error();
  const end = core.HEAPU8.indexOf(0, p);
  return new TextDecoder().decode(core.HEAPU8.subarray(p, end < 0 ? p + 256 : end));
};
function offerMultiplayerReport(message, stack) {
  if (!runtimeBuild.multiplayer || options.netplayMode !== 'lan') return;
  const errorBox = $('#error');
  let native = null;
  let playerAnimation = null;
  try {
    const pointer = core?._th13_mp_diagnostic_report?.();
    if (pointer) native = JSON.parse(core.UTF8ToString(pointer));
    const presentation = core?._th13_debug_stage_report?.();
    if (presentation) {
      playerAnimation = core.UTF8ToString(presentation).split('\n')
        .map(line => line.trim()).filter(line => line.startsWith('player script='));
    }
  } catch (error) {
    console.warn('[th13] Native diagnostics unavailable', error);
  }
  const digest = /multiplayer\/([0-9a-f]{64})\//.exec(location.pathname)?.[1] ?? null;
  const route = window.__eaglerPeerTransport?.route;
  const report = {
    schema: 'eagler-th13-multiplayer-error/1',
    capturedAt: new Date().toISOString(),
    error: message.replace(/(?:https?|wss?):\/\/[^\s]+/gi, '[URL]').slice(0, 1000),
    wasmFrames: Array.from(String(stack ?? '').matchAll(/wasm-function\[(\d+)\]:(0x[0-9a-f]+)/gi),
      ([, fn, offset]) => ({function: Number(fn), offset})).slice(0, 24),
    runtime: {version: runtimeBuild.version, generation: digest},
    browser: navigator.userAgent,
    presentation: {canvas: [canvas.width, canvas.height], devicePixelRatio,
      visibility: document.visibilityState, fps: window.__eaglerNetplayPresentationFps ?? null,
      playerAnimation},
    network: {route: ['rtc', 'relay'].includes(route) ? route : null,
      transport: (() => {
        const t = globalThis.__eaglerPeerTransport;
        if (!t) return null;
        return {closed: !!t.closed, failed: !!t.failed, disconnected: !!t.disconnected,
          error: String(t.error ?? '').replace(/(?:https?|wss?):\/\/[^\s]+/gi, '[URL]').slice(0, 1000),
          peers: [...(t.peers?.entries?.() ?? [])].map(([seat, p]) => ({seat,
            inputOpen: !!p.inputOpen, controlOpen: !!p.controlOpen,
            connection: p.pc?.connectionState ?? null, ice: p.pc?.iceConnectionState ?? null,
            recovering: !!p.recoveryPending, restartAttempts: p.restartAttempts ?? 0}))};
      })(),
      diagnostics: window.__eaglerNetplayDiagnostics ?? null},
    native,
  };
  const content = JSON.stringify(report, null, 2) + '\n';
  const download = document.createElement('button');
  download.type = 'button';
  download.style.display = 'block';
  download.style.margin = '0.75rem auto 0';
  download.textContent = '下载联机错误报告 / Download report';
  download.addEventListener('click', () => {
    const url = URL.createObjectURL(new Blob([content], {type: 'application/json'}));
    const link = document.createElement('a');
    link.href = url;
    link.download = 'eagler-th13-mp-error-' + report.capturedAt.replace(/[:.]/g, '-') + '.json';
    link.click();
    setTimeout(() => URL.revokeObjectURL(url), 30000);
  });
  errorBox.append(download);
}
const fatal = e => {
  const message = e?.message || String(e);
  $('#error').textContent = message;
  try { core?._th13_loop_pause(1); } catch { /* The WASM instance may have trapped. */ }
  offerMultiplayerReport(message, e?.stack);
  emit('error', {error: message});
};

// 存档读：优先 'th13.scoredat'（原版格式存档键，launcher 存档线写入）；
// 新键缺席时回退读旧 'th13.save'——过渡兼容：已放出去的旧存档在壳升级后
// 不至于「消失」。回退只读不写：写路径一律落新键（旧键并存由 remove 统一
// 清理），一旦写过新键即恒优先。
function readSaveBytes() {
  try {
    const text = localStorage.getItem('th13.scoredat') ?? localStorage.getItem('th13.save');
    if (!text) return null;
    const binary = atob(text);
    const bytes = new Uint8Array(binary.length);
    for (let i = 0; i < binary.length; ++i) bytes[i] = binary.charCodeAt(i);
    return bytes;
  } catch {
    return null;
  }
}

// 存档写：一律落 'th13.scoredat'。旧 'th13.save' 保留不动，供旧版本壳
// 过渡回退读取。
function writeSaveBytes(bytes) {
  try {
    let binary = '';
    for (let i = 0; i < bytes.length; ++i) binary += String.fromCharCode(bytes[i]);
    localStorage.setItem('th13.scoredat', btoa(binary));
    return true;
  } catch {
    return false;
  }
}

// ---- Replay 文件通道（replay/th13_(\d{2}|ud[a-z0-9]{4})\.rpyx? 命名空间）----
// launcher Replay 工作台经 list/read/write/remove postMessage 命令读写本
// 存储区；游戏内保存经 Module.onReplaySaved 落同一索引（C++ 已先行 import
// 进 catalog），launch 时 restoreReplaysToCatalog 重导入。键定死（与
// th13.html 同款内联实现——打包名册固定，不新增模块文件）：索引
// th13.replays = JSON 数组 [{name,size,ts}]，blob th13.replay.<name> =
// base64；name 恒为小写（sanitizePath 已归一）。护栏：单条 >1MiB 拒收；
// 索引总量（base64 账面）超 5MiB 先逐出最旧条目，配额溢出再逐出重试——
// 正在写入的条目绝不逐出，全部失败返回 false 由调用方报错（不静默丢档）。
// 多人 Replay 使用独立命名空间（th13mp_XX.rpy）与独立 localStorage 键，
// 避免与单人 Replay/成绩互相覆盖（MP-DEVELOPMENT 的存储根约定）。
const replayStem = runtimeBuild.multiplayer ? 'th13mp' : 'th13';
const replayIndexKey = replayStem + '.replays', replayBlobPrefix = replayStem + '.replay.';
// launcher 的 replayFilePolicy 目前仍按 th13_ 命名（th15mp 才有 mp 前缀特例）；
// 正式页同时接受 th13mp_（原生约定）与 th13_（launcher 导入分配）两种名字。
const replayNamePattern = runtimeBuild.multiplayer
  ? /^th13(?:mp)?_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?$/
  : /^th13_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?$/;
const replaySingleMax = runtimeBuild.multiplayer ? 16 * 1024 * 1024 : 1024 * 1024;
const replayTotalMax = 5 * 1024 * 1024;
// All-seat recordings exceed localStorage's quota during a normal full run.
// Keep synchronous native reads in memory; sync/close await durable IDB writes.
const multiplayerReplays = new Map();
let multiplayerReplayDatabase, multiplayerReplayWrites = Promise.resolve();
let multiplayerReplayWriteError;
async function initializeMultiplayerReplays() {
  if (!runtimeBuild.multiplayer) return;
  multiplayerReplayDatabase = await new Promise((resolve, reject) => {
    const request = indexedDB.open('th13mp.replays.v1', 1);
    request.onupgradeneeded = () => request.result.createObjectStore('files', {keyPath: 'name'});
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error);
  });
  const files = await new Promise((resolve, reject) => {
    const request = multiplayerReplayDatabase.transaction('files').objectStore('files').getAll();
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error);
  });
  core.FS.mkdirTree('/save/replay');
  for (const file of files) multiplayerReplays.set(file.name, file);
  // Native slot selection must see the same files after a new iframe starts.
  for (const entry of replayIndex()) {
    const bytes = readReplayFile(entry.name);
    if (bytes) core.FS.writeFile('/save/replay/' + entry.name, bytes);
  }
}
function writeMultiplayerReplay(record, remove = false) {
  multiplayerReplayWrites = multiplayerReplayWrites.then(() => new Promise((resolve, reject) => {
    const transaction = multiplayerReplayDatabase.transaction('files', 'readwrite');
    const store = transaction.objectStore('files');
    if (remove) store.delete(record.name); else store.put(record);
    transaction.oncomplete = resolve;
    // Explicit abort() leaves IDBTransaction.error null.
    transaction.onabort = () => reject(transaction.error || new DOMException('Replay transaction aborted', 'AbortError'));
  })).catch(error => { multiplayerReplayWriteError = error; });
}
async function flushMultiplayerReplays() {
  await multiplayerReplayWrites;
  if (multiplayerReplayWriteError) throw multiplayerReplayWriteError;
}
function replayIndex() {
  const stored = [...multiplayerReplays.values()].map(({name, size, ts}) => ({name, size, ts}));
  try {
    const list = JSON.parse(localStorage.getItem(replayIndexKey) || '[]');
    return stored.concat(Array.isArray(list)
      ? list.filter(entry => entry && typeof entry.name === 'string' && replayNamePattern.test(entry.name) &&
          !multiplayerReplays.has(entry.name)) : []);
  } catch {
    return stored;
  }
}
function replayWriteIndex(list) {
  localStorage.setItem(replayIndexKey, JSON.stringify(list));
}
function replayToBase64(bytes) {
  let binary = '';
  for (let i = 0; i < bytes.length; ++i) binary += String.fromCharCode(bytes[i]);
  return btoa(binary);
}
function replayFromBase64(text) {
  const binary = atob(text);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; ++i) bytes[i] = binary.charCodeAt(i);
  return bytes;
}
function readReplayFile(name) {
  if (multiplayerReplays.has(name)) return multiplayerReplays.get(name).bytes;
  try {
    const text = localStorage.getItem(replayBlobPrefix + name);
    return text === null ? null : replayFromBase64(text);
  } catch {
    return null;
  }
}
function writeReplayFile(name, bytes) {
  try {
    if (!replayNamePattern.test(name) || !bytes || !bytes.length || bytes.length > replaySingleMax) return false;
    if (runtimeBuild.multiplayer) {
      const record = {name, bytes: bytes.slice(), size: bytes.length, ts: Date.now()};
      multiplayerReplays.set(name, record);
      writeMultiplayerReplay(record);
      return true;
    }
    const list = replayIndex().filter(entry => entry.name !== name);
    list.push({name, size: bytes.length, ts: Date.now()});
    const weight = entry => Math.ceil((entry.size || 0) * 4 / 3);  // base64 账面
    const dropOldest = () => {
      let oldest = 0;
      for (let i = 1; i < list.length; ++i) if ((list[i].ts ?? 0) < (list[oldest].ts ?? 0)) oldest = i;
      const [gone] = list.splice(oldest, 1);
      localStorage.removeItem(replayBlobPrefix + gone.name);
      return gone.name;
    };
    const save = () => {
      localStorage.setItem(replayBlobPrefix + name, replayToBase64(bytes));
      replayWriteIndex(list);
    };
    try {
      // list 已含本次条目：超总量时逐最旧，绝不逐出正写入的条目（最新 ts，
      // 且 list.length>1 兜底——单条 ≤1MiB 恒小于总量上限，理论到不了）。
      while (list.reduce((sum, entry) => sum + weight(entry), 0) > replayTotalMax && list.length > 1)
        dropOldest();
      save();
      return true;
    } catch (quota) {
      while (list.length > 1) {
        try {
          dropOldest();
          save();
          return true;
        } catch { /* 继续逐出最旧 */ }
      }
      return false;
    }
  } catch {
    return false;
  }
}
function removeReplayFile(name) {
  if (multiplayerReplays.has(name)) {
    multiplayerReplays.delete(name);
    writeMultiplayerReplay({name}, true);
    localStorage.removeItem(replayBlobPrefix + name);
    replayWriteIndex(replayIndex().filter(entry => entry.name !== name));
    return true;
  }
  if (localStorage.getItem(replayBlobPrefix + name) === null &&
      !replayIndex().some(entry => entry.name === name)) return false;
  localStorage.removeItem(replayBlobPrefix + name);
  replayWriteIndex(replayIndex().filter(entry => entry.name !== name));
  return true;
}
// launch 时按索引把持久化录像重导入 catalog（与手动导入同一
// _th13_replay_import 路径：th13_NN.rpy → 编号槽 NN-1 覆盖，其余 → 首个
// 空闲用户槽；原文件名 → 槽位语义不变）。失败只记 console——持久档损坏
// 不阻断启动，launcher 工作台仍可读/删原文件。
function restoreReplaysToCatalog() {
  for (const entry of replayIndex()) {
    const bytes = readReplayFile(entry.name);
    if (!bytes) continue;
    const dataPtr = core._malloc(bytes.length);
    const namePtr = core.allocateUTF8(entry.name);
    try {
      core.HEAPU8.set(bytes, dataPtr);
      if (!core._th13_replay_import(dataPtr, bytes.length, namePtr))
        console.warn('[th13] replay restore failed:', entry.name,
          core._th13_replay_import_error ? core.UTF8ToString(core._th13_replay_import_error()) : err());
    } finally {
      core._free(dataPtr);
      core._free(namePtr);
    }
  }
}

function closeAudio() {
  const s = core.SDL3, context = s?.audioContext, borrowed = parent !== window && context === parent.__touhouAudioContext;
  if (borrowed) s.audioContext = undefined;
  try {
    core._th13_loop_stop();
  } finally {
    if (borrowed) s.audioContext = context;
  }
}
// M9/P6：多人录像 inspect（native 描述面——eagler_replay_inspect）。
function inspectReplay(bytes) {
  const pointer = core._malloc(bytes.length);
  try {
    core.HEAPU8.set(bytes, pointer);
    const report = core._eagler_replay_inspect(pointer, bytes.length);
    if (!report) throw Error('录像解析失败');
    const end = core.HEAPU8.indexOf(0, report);
    return {description: JSON.parse(new TextDecoder().decode(
      core.HEAPU8.subarray(report, end < 0 ? report + 65536 : end)))};
  } finally {
    core._free(pointer);
  }
}
// M9/P6：回放进度报告（ReplayTransport 消费；replayViewer 打开后周期发布）。
let playbackPath = null, playbackPaused = false, playbackRate = 1;
let playbackReportTimer = null;
function publishPlaybackState() {
  if (!runtimeBuild.multiplayer || options.replayViewer !== true || !launched) {
    if (playbackReportTimer !== null) { clearInterval(playbackReportTimer); playbackReportTimer = null; }
    return;
  }
  const pointer = core._eagler_replay_state?.() ? core._eagler_replay_state() >>> 2 : 0;
  if (!pointer) return;
  const state = Array.from(core.HEAPU32.subarray(pointer, pointer + 8));
  emit('runtime-info', {playback: {
    kind: 'replay', path: playbackPath, frame: state[0], total: state[1],
    paused: playbackPaused, seeking: !!state[7], target: state[7] ? state[6] : null, rate: playbackRate,
    ended: !!state[2], stage: state[3] || 1,
    difficulty: -1, left: 0, right: 0, startFrame: state[5],
  }});
}
function startPlaybackReports(path) {
  playbackPath = path;
  playbackPaused = false;
  playbackRate = 1;
  if (playbackReportTimer !== null) clearInterval(playbackReportTimer);
  playbackReportTimer = setInterval(publishPlaybackState, 250);
  publishPlaybackState();
}
const save = async () => {
  multiplayerBridge?.save();
  await flushMultiplayerReplays();
};
async function stop() {
  if (stopping) return;
  if (roomOwned && !options.replayViewer) {
    stopping = true;
    try {
      await save();
      await command({command: 'return-room'});
      emit('room-menu');
    } finally { stopping = false; }
    return;
  }
  stopping = true;
  try {
    core._th13_loop_stop();
    try { await save(); } finally { multiplayerBridge?.close(); }
    closeAudio();
    spectatorPlayback?.stop();
    if (playbackReportTimer !== null) { clearInterval(playbackReportTimer); playbackReportTimer = null; }
    launched = false;
    emit('exit', {code: 0, status: 'success'});
  } finally {
    stopping = false;
  }
}

function sanitizePath(value) {
  const name = String(value).replaceAll('\\', '/').toLowerCase().replace(/^\/savesth13(?:mp)?\//, '').replace(/^\//, '');
  const replay = 'replay\\/' + (runtimeBuild.multiplayer ? 'th13(?:mp)?' : 'th13') +
    '_(?:\\d{2}|ud[a-z0-9]{4})\\.rpyx?';
  if (!new RegExp('^(?:scoreth13\\.dat|th13\\.cfg|' + replay + ')$').test(name)) throw Error('存档路径无效');
  return name;
}

function apply() {
  core.eaglerOptions = options;
  core._th13_music_enabled(+music);
  core._th13_touch_options(+!!options.touchEnabled,
    Math.max(0, ['touch', 'touch-unlimited', 'joystick', 'joystick-free'].indexOf(options.touchMovementMode)),
    Math.max(100,Math.min(300,Number(options.touchSensitivity) || 100)) / 100,
    +(options.touchFocusMode === 'two-finger'), +!!options.doubleTapBombEnabled);
}

async function resource(r) {
  if (!r || typeof r.path !== 'string' ||
      (!['/msgothic.ttc', '/unifont.otf', '/fonts/cp932.bin'].includes(r.path) &&
       !/^\/assets\/[a-zA-Z0-9_-]+\.(?:anm|ecl|std|sht|msg|wav|fmt|txt|ver|ogg|png)$/.test(r.path))) {
    throw Error('资源路径无效');
  }
  const u = new URL(r.url, location.href);
  if (u.origin !== location.origin || !['http:', 'https:', 'blob:'].includes(u.protocol)) throw Error('资源来源无效');
  const response = await fetch(u);
  if (!response.ok) throw Error('资源读取失败');
  const bytes = new Uint8Array(await response.arrayBuffer());
  const expected = r.bytes ?? r.size;
  if (bytes.length > 64 * 1024 * 1024 || (expected != null && bytes.length !== expected)) throw Error('资源大小错误');
  core.FS.mkdirTree(r.path.slice(0, r.path.lastIndexOf('/')) || '/');
  core.FS.writeFile(r.path, bytes, {canOwn: true});
  emit('transfer', {mode: 'runtime', loaded: bytes.length, total: bytes.length, path: r.path});
}

async function initializeTitle() {
  let builtAt = new Date().toISOString();
  try {
    const resp = await fetch('./manifest.json');
    if (resp.ok) builtAt = (await resp.json()).builtAt || builtAt;
  } catch {}
  await installStartupBranding(core, {game, builtAt});
  let scaleOption = query.get('renderScale');
  if (!scaleOption) {
    try { scaleOption = new URLSearchParams(parent.location.search).get('renderScale'); } catch {}
  }
  const requested = Number(scaleOption);
  // 移动端默认 renderScale=1（= 原版原生 640×480 分辨率）：粗指针/小屏
  // 默认降档保 60fps；显式 options/URL 指定优先。桌面与无头捕获不受影响。
  const mobileDefault = matchMedia('(pointer: coarse)').matches ||
      Math.min(screen.width || 9999, screen.height || 9999) < 640 ? 1 : 2;
  core._th13_render_scale(options.renderScale === 1 ? 1 : options.renderScale === 2 ? 2
    : requested === 1 ? 1 : requested === 2 ? 2 : mobileDefault);
  if (!core._th13_prepare_loading()) throw Error(err());
  $('#loading').textContent = '';
  const startupImageAt = performance.now();
  performance.mark('eagler-startup-image');
  await new Promise(requestAnimationFrame);
  await new Promise(requestAnimationFrame);
  if (!core._th13_initialize()) throw Error(err());
  apply();
  await finishStartupAnimation(startupImageAt, frames => core._th13_draw_startup(frames));
  performance.mark('eagler-startup-menu-ready');
  first = false;
  $('#loading').textContent = '';
}

async function command(m) {
  switch (m.command) {
    case 'prepare-room':
      roomOwned = true;
      // 正式房间预载：呈现原生标题面后才允许 READY（launcher 以
      // response.presented===true 作为 ACK）。
      if (!runtimeBuild.multiplayer || launched || options.netplayMode === 'lan' ||
          options.replayViewer) throw Error('房间预览需要未启动的多人标题页');
      if (!roomPreview) {
        await initializeTitle();
        if (!core._th13_room_preview()) throw Error(err() || '房间标题呈现失败');
        roomPreview = true;
        core._th13_loop_start();
        await new Promise(requestAnimationFrame);
      }
      return {presented: true};
    case 'configure':
      if (launched) throw Error('不能配置正在运行的游戏');
      if (!['ogg', 'none'].includes(m.music)) throw Error('不支持的音乐模式');
      options = m.options || {};
      music = m.music === 'ogg';
      for (const r of [...(m.sharedResources || []), ...(m.runtimeResources || []), ...(m.resources || [])]) await resource(r);
      apply();
      return {};
    case 'resources':
      for (const r of m.resources || []) await resource(r);
      return {};
    case 'keyboard':
      if (launched && !document.hidden && !stopping) keyboard.event(m, !!m.down, 'hosted');
      return {};
    case 'keyboard-clear':
      clearKeyboard();
      return {};
    case 'touch-cancel':
      core._th13_touch_cancel();
      return {};
    case 'direct-touch': {
      if (m.type === 'cancel') { core._th13_touch_cancel(); return {}; }
      if (!launched || document.hidden || stopping || !options.touchEnabled ||
          !['down', 'move', 'up'].includes(m.type) || ![m.x, m.y, m.id].every(Number.isFinite)) return {};
      const b = canvas.getBoundingClientRect();
      if (b.width && b.height) core._th13_touch(({down: 0, move: 1, up: 2})[m.type], m.id,
        (m.x * innerWidth - b.left) / b.width, (m.y * innerHeight - b.top) / b.height);
      return {};
    }
    case 'touch-controls': {
      const t = m.controls || m;
      core.eaglerControls = t;
      if (Number.isFinite(t.touchSensitivity) && t.touchSensitivity>=100&&t.touchSensitivity<=300 &&
          t.touchSensitivity !== options.touchSensitivity) {
        options.touchSensitivity = t.touchSensitivity;
        apply();
      }
      core._th13_touch_controls(+!!options.touchEnabled, +!!t.fireEnabled, +!!t.focusEnabled,
        t.bombSerial >>> 0, t.escapeSerial >>> 0, Number(t.joystickX) || 0, Number(t.joystickY) || 0);
      return {};
    }
    case 'return-room':
      if (!runtimeBuild.multiplayer) throw Error('此 Runtime 不支持返回房间');
      roomOwned = true;
      spectatorPlayback?.stop();
      clearKeyboard();
      core._th13_loop_stop();
      // Retain the terminal input/ACK cache until the next launch. The other
      // client can still be completing its last frame when this room appears.
      multiplayerBridge.retire();
      if (!core._th13_room_return() || !core._th13_room_preview())
        throw Error(err() || '返回房间失败');
      roomPreview = true;
      launched = first = false;
      core._th13_loop_start();
      await new Promise(requestAnimationFrame);
      return {presented: true};
    case 'launch':
      launchSerial = Number(m.launchSerial) || 0;
      if (!launched) {
        if (roomPreview) core._th13_loop_stop();
        else await initializeTitle();
        launched = true;
        roomPreview = false;
        apply();
        first = false;
        $('#loading').textContent = '';
        // 多人：configure → connect → start 由桥完成，成功后才启动 rAF 泵；
        // 缺省（非多人）保持单席原有顺序。录像启动不建立玩家连接，桥返回
        // false 时交给 playback 命令/宿主回放器接管。观众启动同理：桥的
        // launch 走 spectator_connect → start（收流待命），泵启动后由
        // 观战桥发布 native 报告。
        let loopStarted = !runtimeBuild.multiplayer;
        if (runtimeBuild.multiplayer) {
          if (options.replayViewer === true && typeof options.replayPath === 'string' && options.replayPath) {
            await multiplayerBridge.openReplay(options.replayPath, options.replayStage);
            loopStarted = true;
            startPlaybackReports('replay/' + options.replayPath.split('/').pop());
          } else if (await multiplayerBridge.launch()) loopStarted = true;
        }
        if (loopStarted) core._th13_loop_start();
        if (runtimeBuild.multiplayer && options.replayViewer === true && !loopStarted) {
          first = true;
          emit('first-frame');
        }
        // 观众：桥启动后开始周期发布观战报告（暂停/定位控制面）。
        if (options.netplaySpectator) spectatorPlayback?.start();
        // 持久化录像回灌 catalog（launcher 工作台经 write 写入的、以及上次
        // 会话游戏内保存的）：晚于 loop_start，与 th13.html 内置预装同一拍。
        // 多人 Replay 使用独立命名空间，不回灌单席 catalog。
        if (!runtimeBuild.multiplayer) restoreReplaysToCatalog();
        emit('runtime-info', {renderer: 'SDL3 / WebGL2 / C++', architecture: protocol, ...runtimeBuild});
      }
      return {};
    case 'spectator-playback':
      // 观战控制（暂停/继续/定位/回直播）：native SpectatorStageView。
      if (!runtimeBuild.multiplayer) throw Error('此 Runtime 不支持观战控制');
      return spectatorPlayback.command(m);
    case 'playback': {
      // M9/P6：多人录像回放——open 建档启动；inspect/pause/play/seek/rate
      // 走 native 观测/控制面（eagler_replay_state / replay_pause 族导出）。
      if (!runtimeBuild.multiplayer) throw Error('此 Runtime 不支持录像回放控制');
      if (!launched || options.replayViewer !== true) throw Error('请先打开录像管理器');
      if (m.action === 'inspect') {
        const target = sanitizePath(m.path);
        if (!target.startsWith('replay/')) throw Error('录像路径无效');
        const bytes = readReplayFile(target.slice(7));
        if (!bytes) throw Error('文件不存在');
        return inspectReplay(bytes);
      }
      if (m.action === 'pause' || m.action === 'play') {
        if (!core._th13_mp_replay_pause(m.action === 'pause' ? 1 : 0))
          throw Error('回放暂停控制失败');
        playbackPaused = m.action === 'pause';
        publishPlaybackState();
        return {};
      }
      if (m.action === 'rate') {
        if (!core._th13_mp_replay_rate(Number(m.value) || 0))
          throw Error('回放速率无效');
        playbackRate = Number(m.value) || 1;
        publishPlaybackState();
        return {};
      }
      if (m.action === 'seek') {
        // Timeline values are absolute replay frames.
        if (!core._th13_mp_replay_seek(Number(m.value) || 0))
          throw Error('回放定位目标无效');
        publishPlaybackState();
        return {};
      }
      if (m.action !== 'open') throw Error('TH13 多人录像暂不支持此回放控制');
      const target = sanitizePath(m.path);
      if (!target.startsWith('replay/')) throw Error('录像路径无效');
      const name = target.slice(7), bytes = readReplayFile(name);
      if (!bytes) throw Error('文件不存在');
      await multiplayerBridge.openReplay(name, m.stage, bytes);
      core._th13_loop_start();
      startPlaybackReports(target);
      return {};
    }
    case 'sync':
      await save();
      return {};
    case 'list': {
      const files = [];
      const saveBytes = readSaveBytes();
      if (saveBytes && saveBytes.length > 0) {
        files.push({path: 'scoreth13.dat', size: saveBytes.length});
      }
      for (const entry of replayIndex()) files.push({path: 'replay/' + entry.name, size: entry.size || 0});
      return {files};
    }
    case 'read': {
      const target = sanitizePath(m.path);
      if (target === 'scoreth13.dat') {
        const bytes = readSaveBytes();
        if (!bytes) throw Error('存档不存在');
        return {bytes: Array.from(bytes)};
      }
      if (target.startsWith('replay/')) {
        const bytes = readReplayFile(target.slice(7));
        if (!bytes) throw Error('文件不存在');
        return {bytes: Array.from(bytes)};
      }
      throw Error('文件不存在');
    }
    case 'write': {
      if (launched) throw Error('请先退出游戏');
      const target = sanitizePath(m.path);
      const bytes = new Uint8Array(m.bytes || []);
      if (!bytes.length || bytes.length > 64 * 1024 * 1024) throw Error('文件大小无效');
      if (target === 'scoreth13.dat') {
        if (!writeSaveBytes(bytes)) throw Error('保存 localStorage 失败');
        return {};
      }
      if (target.startsWith('replay/')) {
        if (bytes.length > replaySingleMax) throw Error('录像文件过大');
        if (!writeReplayFile(target.slice(7), bytes)) throw Error('保存 localStorage 失败');
        await flushMultiplayerReplays();
        return {};
      }
      throw Error('不支持写入此文件');
    }
    case 'remove': {
      if (launched) throw Error('请先退出游戏');
      const target = sanitizePath(m.path);
      if (target === 'scoreth13.dat') {
        localStorage.removeItem('th13.scoredat');
        // 过渡键一并清除：readSaveBytes 的回退读会 resurrect 旧档。
        localStorage.removeItem('th13.save');
        return {};
      }
      if (target.startsWith('replay/')) {
        if (!removeReplayFile(target.slice(7))) throw Error('文件不存在');
        await flushMultiplayerReplays();
        return {};
      }
      throw Error('不支持删除此文件');
    }
    default:
      throw Error('不支持的操作');
  }
}

window.addEventListener('message', e => {
  const m = e.data;
  if (e.source !== parent || e.origin !== location.origin || m?.protocol !== protocol || m.game !== game || !validEpoch || m.epoch !== epoch || typeof m.command !== 'string') return;
  queue = queue.then(async () => {
    if (await initialized === false) return;
    try {
      const result = await command(m);
      if (typeof m.request === 'string') parent.postMessage({protocol, game, epoch, request: m.request, ok: true, ...result}, location.origin);
    } catch (e) {
      console.error('[th13-runtime] command failed:', m.command, e);
      if (typeof m.request === 'string') parent.postMessage({protocol, game, epoch, request: m.request, ok: false, error: String(e), errno: e?.errno}, location.origin);
      else fatal(e);
    }
  }).catch(fatal);
});

document.addEventListener('visibilitychange', () => {
  if (launched) {
    clearKeyboard();
    core?._th13_touch_cancel();
    // M1：多人失焦不得单端暂停；单席保持原有隐藏暂停语义。
    if (!runtimeBuild.multiplayer) core._th13_loop_pause(+document.hidden);
  }
});
window.addEventListener('blur', () => {
  clearKeyboard();
  core?._th13_touch_cancel();
});

for (const [event, type] of [['pointerdown', 0], ['pointermove', 1], ['pointerup', 2], ['pointercancel', 2]]) {
  canvas.addEventListener(event, e => {
    if (!launched || !options.touchEnabled || e.pointerType === 'mouse') return;
    e.preventDefault();
    if (type === 0) canvas.setPointerCapture(e.pointerId);
    const b = canvas.getBoundingClientRect();
    if (b.width && b.height) core._th13_touch(type, e.pointerId,
      (e.clientX - b.left) / b.width, (e.clientY - b.top) / b.height);
  }, {passive: false});
}

for (const event of ['keydown', 'keyup']) window.addEventListener(event, e => {
  if (!launched || e.altKey || e.metaKey) return;
  if (keyboard.mapNative(e, event === 'keydown')) {
    e.preventDefault();
    e.stopImmediatePropagation();
    return;
  }
  if (keyboard.event(e, event === 'keydown')) {
    e.preventDefault();
    e.stopPropagation();
  }
}, {capture: true});

for (const event of ['pointerdown', 'keydown']) window.addEventListener(event, () => core?.SDL3?.audioContext?.resume().catch(() => {}), {capture: true});
canvas.addEventListener('webglcontextlost', e => {
  e.preventDefault();
  fatal(Error('图形环境失效，请退出后重新开始。'));
});
window.addEventListener('error', event => {
  if (launched && runtimeBuild.multiplayer && options.netplayMode === 'lan' &&
      event.error instanceof WebAssembly.RuntimeError) fatal(event.error);
});
window.addEventListener('pagehide', () => {
  clearKeyboard();
  if (launched) {
    core._th13_loop_stop();
    closeAudio();
  }
});

const initialized = (async () => {
  let last = performance.now(), ticks = 0, presents = 0, maxFrameMs = 0;
  core = await createModule({
    canvas,
    printErr: console.error,
    onGameFrame(ok, ms, count) {
      if (!ok) { fatal(Error(err())); return; }
      ticks += count;
      presents++;
      maxFrameMs = Math.max(maxFrameMs, ms);
      multiplayerBridge?.frame();
      if (!first) {
        first = true;
        last = performance.now();
        ticks = 0;
        presents = 0;
        maxFrameMs = 0;
        emit('first-frame');
      }
      const now = performance.now();
      if (now - last >= 1000) {
        const audio = core.th13Audio;
        if (audio) emit('audio-health', {
          ready: audio.device && !audio.failed, renderedFrames: audio.mixedFrames,
          queuedFrames: audio.queuedFrames, queuedMs: audio.queuedFrames * 1000 / audio.sampleRate,
          errorCode: +audio.failed, backend: 'SDL3', track: audio.track,
          cursor: audio.musicCursor, musicEnabled: music, suspended: audio.suspended,
        });
        emit('frame-health', {
          fps: presents * 1000 / (now - last),
          simulationFps: ticks * 1000 / (now - last),
          maxGapMs: maxFrameMs,
        });
        last = now;
        ticks = 0;
        presents = 0;
        maxFrameMs = 0;
      }
      if (launched && (core._th13_phase() === 4 ||
          (roomOwned && !options.replayViewer && core._th13_room_return_ready())))
        void stop().catch(fatal);
    }
  });

  core.SDL3 = core.SDL3 || {};
  if (parent !== window && parent.__touhouAudioContext) core.SDL3.audioContext = parent.__touhouAudioContext;
  window.Module = core;
  window.FS = core.FS;
  core.resetBrowserKeyboard = () => keyboard.clear();
  await initializeMultiplayerReplays();
  // 游戏内保存录像（save_replay_slot → 下载 EM_JS 钩子）：catalog 已由
  // C++ 侧 import_replay_bytes 入槽，这里只补 localStorage 持久化。自吞
  // 异常——持久化失败不影响游戏内保存/下载流程。
  core.onReplaySaved = (name, bytes) => {
    try {
      return writeReplayFile(String(name).toLowerCase(), bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes));
    } catch (error) {
      console.warn('[th13] replay persist failed:', error);
      return false;
    }
  };

  if (query.get('managedData') === '1') {
    if (!validEpoch || parent === window || typeof parent.__eaglerPrepareManagedRuntimeDataV1 !== 'function') {
      throw Error('请从 eagler-touhou 启动此运行时');
    }
    const {buffer} = await parent.__eaglerPrepareManagedRuntimeDataV1({game, generation: query.get('gameGeneration'), epoch});
    // The Launcher owns this buffer in the parent realm. Check the native
    // ArrayBuffer brand instead of comparing its constructor with the iframe's.
    let byteLength = 0;
    try { byteLength = Object.getOwnPropertyDescriptor(ArrayBuffer.prototype, 'byteLength').get.call(buffer); }
    catch { /* Non-ArrayBuffer values are invalid package data. */ }
    if (byteLength < 16 || byteLength > 256 * 1024 * 1024)
      throw Error('游戏资源包无效');
    const view = new DataView(buffer);
    const magic = new TextDecoder().decode(new Uint8Array(buffer, 0, 8));
    if (magic !== 'TH13PK01') throw Error('非法神灵庙资源包 magic: ' + magic);
    const manifestLength = view.getUint32(8, true);
    if (!manifestLength || manifestLength > 1024 * 1024 || manifestLength > buffer.byteLength - 12)
      throw Error('资源清单长度无效');
    const manifest = JSON.parse(new TextDecoder('utf-8', {fatal: true}).decode(new Uint8Array(buffer, 12, manifestLength)));
    const baseOffset = 12 + manifestLength;
    if (manifest.schema !== 'th13-prepared-content/1' || manifest.game !== game || !Array.isArray(manifest.files))
      throw Error('资源清单格式无效');
    const names = new Set();
    let offset = 0;
    const verified = [];
    for (const f of manifest.files) {
      if (!f || typeof f.path !== 'string' || !/^assets\/[a-zA-Z0-9_-]+\.(?:png|anm|ecl|std|sht|msg|wav|rpy|fmt|txt|ver|ogg)$/.test(f.path) ||
          names.has(f.path) || !Number.isSafeInteger(f.offset) || f.offset !== offset ||
          !Number.isSafeInteger(f.bytes) || f.bytes <= 0 || f.bytes > buffer.byteLength - baseOffset - offset ||
          typeof f.sha256 !== 'string' || !/^[a-f0-9]{64}$/.test(f.sha256)) throw Error('资源条目无效');
      const fileBytes = new Uint8Array(buffer, baseOffset + offset, f.bytes);
      const digest = new Uint8Array(await crypto.subtle.digest('SHA-256', fileBytes));
      const sha256 = Array.from(digest, byte => byte.toString(16).padStart(2, '0')).join('');
      if (sha256 !== f.sha256) throw Error('资源校验失败：' + f.path);
      verified.push([f.path, fileBytes]);
      names.add(f.path);
      offset += f.bytes;
    }
    if (baseOffset + offset !== buffer.byteLength) throw Error('资源包尾部长度无效');
    for (const name of [...runtimeAssets, 'thbgm.fmt', 'thbgm.ogg'])
      if (!names.has('assets/' + name)) throw Error('资源包缺少：' + name);
    core.FS.mkdirTree('/assets');
    for (const [path, bytes] of verified) core.FS.writeFile('/' + path, bytes, {canOwn: true});

    const response = await fetch('./resources.json').catch(() => null);
    if (response && response.ok) {
      const resManifest = await response.json();
      if (resManifest.schema === 'eagler-sdl-resources/1' && resManifest.game === game && Array.isArray(resManifest.resources)) {
        for (const r of resManifest.resources) await resource(r);
      }
    }
  } else {
    core.FS.mkdirTree('/assets');
    for (const name of runtimeAssets) {
      const r = await fetch('assets/' + name).catch(() => null);
      if (r && r.ok) {
        core.FS.writeFile('/assets/' + name, new Uint8Array(await r.arrayBuffer()));
      }
    }
    const font = await fetch('assets/msgothic.ttc').catch(() => null);
    if (font && font.ok) core.FS.writeFile('/msgothic.ttc', new Uint8Array(await font.arrayBuffer()));
  }

  // 多人变体由打包器写入 runtimeBuild.multiplayer=true；桥只在正式多人页加载。
  if (runtimeBuild.multiplayer) {
    const {createMultiplayerBridge} = await import('./multiplayer.mjs');
    const manifest = await (await fetch('./manifest.json')).json();
    multiplayerBridge = createMultiplayerBridge({core, getOptions: () => options, manifest,
      emit, onError: fatal, onExit: stop});
    // 观战桥（M9/P6）：native 观战报告/控制转发；观众端完整录像落地
    //（complete = th13_mp_spectator_result，接管后可本地保存）。
    spectatorPlayback = createSpectatorPlaybackBridge({
      active: () => launched && options.netplaySpectator === true && !stopping,
      memory: () => core.HEAPU8,
      state: () => core._th13_mp_spectator_state?.() || 0,
      control: (run, action, frame) => core._th13_mp_spectator_control?.(run, action, frame) || 0,
      allocate: size => core._malloc(size),
      deallocate: pointer => core._free(pointer),
      emit,
      complete: (pointer, size) => core._th13_mp_spectator_result?.(pointer, size) || 0,
    });
  }

  emit('ready', {roomPreview: runtimeBuild.multiplayer === true, roomReturn: runtimeBuild.multiplayer === true});
  if (query.get('standalone') === '1' || (!validEpoch && parent === window)) {
    await command({command: 'launch'});
  }
})().catch(e => {
  if (e?.name === 'AbortError' && e?.message === 'EAGLER_RUNTIME_SESSION_SUPERSEDED') return false;
  fatal(e);
  throw e;
});
