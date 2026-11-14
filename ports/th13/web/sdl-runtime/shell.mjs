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
const emit = (event, fields = {}) => parent.postMessage({protocol, game, epoch, event, ...fields}, location.origin);
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
const err = () => {
  const p = core._th13_error();
  const end = core.HEAPU8.indexOf(0, p);
  return new TextDecoder().decode(core.HEAPU8.subarray(p, end < 0 ? p + 256 : end));
};
const fatal = e => {
  const message = e?.message || String(e);
  $('#error').textContent = message;
  core?._th13_loop_pause(1);
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
const replayIndexKey = 'th13.replays', replayBlobPrefix = 'th13.replay.';
const replayNamePattern = /^th13_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?$/;
const replaySingleMax = 1024 * 1024, replayTotalMax = 5 * 1024 * 1024;
function replayIndex() {
  try {
    const list = JSON.parse(localStorage.getItem(replayIndexKey) || '[]');
    return Array.isArray(list)
      ? list.filter(entry => entry && typeof entry.name === 'string' && replayNamePattern.test(entry.name)) : [];
  } catch {
    return [];
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
async function stop() {
  if (stopping) return;
  stopping = true;
  try {
    core._th13_loop_stop();
    closeAudio();
    launched = false;
    emit('exit', {code: 0, status: 'success'});
  } finally {
    stopping = false;
  }
}

function sanitizePath(value) {
  const name = String(value).replaceAll('\\', '/').toLowerCase().replace(/^\/savesth13\//, '').replace(/^\//, '');
  if (!/^(?:scoreth13\.dat|th13\.cfg|replay\/th13_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name)) throw Error('存档路径无效');
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

async function command(m) {
  switch (m.command) {
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
    case 'launch':
      if (!launched) {
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
        launched = true;
        apply();
        first = false;
        $('#loading').textContent = '';
        core._th13_loop_start();
        // 持久化录像回灌 catalog（launcher 工作台经 write 写入的、以及上次
        // 会话游戏内保存的）：晚于 loop_start，与 th13.html 内置预装同一拍。
        restoreReplaysToCatalog();
        emit('runtime-info', {renderer: 'SDL3 / WebGL2 / C++', architecture: protocol, ...runtimeBuild});
      }
      return {};
    case 'sync':
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
      if (typeof m.request === 'string') parent.postMessage({protocol, game, epoch, request: m.request, ok: false, error: String(e), errno: e?.errno}, location.origin);
      else fatal(e);
    }
  }).catch(fatal);
});

document.addEventListener('visibilitychange', () => {
  if (launched) {
    clearKeyboard();
    core?._th13_touch_cancel();
    core?._th13_loop_pause(+document.hidden);
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
      if (core._th13_phase() === 4) void stop().catch(fatal);
    }
  });

  core.SDL3 = core.SDL3 || {};
  if (parent !== window && parent.__touhouAudioContext) core.SDL3.audioContext = parent.__touhouAudioContext;
  window.Module = core;
  window.FS = core.FS;
  core.resetBrowserKeyboard = () => keyboard.clear();
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

  emit('ready');
  if (query.get('standalone') === '1' || (!validEpoch && parent === window)) {
    await command({command: 'launch'});
  }
})().catch(e => {
  if (e?.name === 'AbortError' && e?.message === 'EAGLER_RUNTIME_SESSION_SUPERSEDED') return false;
  fatal(e);
  throw e;
});
