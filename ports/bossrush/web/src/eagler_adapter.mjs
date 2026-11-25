/**
 * eagler_adapter.mjs — eagler-touhou Launcher ↔ BRO Runtime 协议适配层
 *
 * 阶段 3V。清酒：实际联机测试要等接入 eagler 网站，所以游戏内接口必须先对齐网站契约。
 *
 * 权威契约来源（2026-11-20 拉取 main 分支原文）：
 *  - launcher/src/contracts/runtime-protocol.mts  （消息信封/命令/事件/选项）
 *  - common/contracts/product-catalog.mjs        （HOST_PROTOCOL / multiplayer 声明）
 *  - common/contracts/netplay-timing.mjs         （parseMeasuredNetplayTiming 严格规则）
 *
 * 设计原则：**纯 JS 适配层，游戏本体零改动**。把 eagler 的 postMessage 信封翻译成
 * BROMultiplayer / input / storage 的既有方法调用，把 BRO 状态翻译成 eagler 事件。
 * 保留 BRO 原有直调门面（lobby.html 等），适配层是叠加的一层，不替换。
 *
 * 对齐点（已逐字段核对权威源码）：
 *  - HOST_PROTOCOL = "eagler-touhou/1"
 *  - epoch 由 URL 查询参数 runtimeEpoch 提供，必须是 safe integer 且 > 0
 *  - 出站事件: {protocol, game, epoch, event, ...payload}
 *  - 命令响应: {protocol, game, epoch, request, ok, ...}
 *  - 入站命令: {protocol, game, epoch, command, request?, ...payload}
 *  - 13 必需命令 + 8 必需事件（见下方常量，逐字抄自 RUNTIME_PROTOCOL_*）
 *  - netplay 时序对象必须过 parseMeasuredNetplayTiming（本文件 isValidNetplayTiming 复刻其规则）
 */

// ---------------------------------------------------------------------------
// 权威常量（逐字对齐 runtime-protocol.mts / product-catalog.mjs）
// ---------------------------------------------------------------------------

export const HOST_PROTOCOL = 'eagler-touhou/1';
export const RUNTIME_EPOCH_QUERY_PARAMETER = 'runtimeEpoch';

/** 观战 Lua 值序列化（与 multiplayer.mjs 的 luaValue 同构，独立定义避免循环依赖）。 */
function spectatorLuaValue(v) {
  if (v === null || v === undefined) return 'nil';
  if (typeof v === 'number') return Number.isFinite(v) ? String(v) : 'nil';
  if (typeof v === 'boolean') return v ? 'true' : 'false';
  if (typeof v === 'string') {
    return '"' + v.replace(/\\/g, '\\\\').replace(/"/g, '\\"')
      .replace(/\n/g, '\\n').replace(/\r/g, '\\r') + '"';
  }
  if (Array.isArray(v)) return '{' + v.map(spectatorLuaValue).join(',') + '}';
  return '"' + String(v).replace(/\\/g, '\\\\').replace(/"/g, '\\"') + '"';
}

/** 关卡号 → 关卡名（对齐 game/stage_queue.lua 的 STAGE_SUFFIX）。 */
function stageQueueName(level) {
  const SUFFIX = { 1: '06', 2: '07', 3: '08', 4: '09', 5: '095', 6: '10', 7: '11', 8: '12',
    9: '125', 10: '128', 11: '13', 12: '14', 13: '143', 14: '15', 15: '16', 16: '165',
    17: '17', 18: '18', 19: '185' };
  const s = SUFFIX[Number(level) | 0];
  return s ? 'TH' + s + '@BossRush' : 'TH06@BossRush';
}

/** 13 个必需命令（response=required 的都在此；顺序与官方 Object.freeze 数组一致）。 */
export const RUNTIME_PROTOCOL_COMMANDS = Object.freeze([
  'configure', 'resources', 'keyboard', 'keyboard-clear', 'touch-controls',
  'touch-cancel', 'direct-touch', 'launch', 'list', 'read', 'write', 'remove', 'sync',
]);

import {
  SpectatorUploader, SpectatorClient,
  spectatorRelayUrl, publisherRelayUrl,
} from './spectator_runtime.mjs';
import { packSpecInput } from './spectator_wire.mjs';

export const RUNTIME_PROTOCOL_OPTIONAL_COMMANDS = Object.freeze([
  'prepare-room', 'return-room', 'playback', 'retry-music', 'thprac-mouse',
  'network-cancel', 'spectator-room-state', 'spectator-playback',
]);

/** 8 个必需事件（requirement=required）。 */
export const RUNTIME_PROTOCOL_EVENTS = Object.freeze([
  'ready', 'transfer', 'first-frame', 'runtime-info', 'frame-health',
  'audio-health', 'exit', 'error',
]);

export const RUNTIME_PROTOCOL_OPTIONAL_EVENTS = Object.freeze([
  'midi-fallback', 'music-complete', 'music-error', 'music-incomplete', 'notice',
  'player-debug', 'network-request', 'room-menu',
]);

/** 命令阶段（官方 RuntimeCommandPhase），用于门禁：configure/list/read/write 在 live 前。 */
export const COMMAND_PHASE = Object.freeze({
  'configure': 'prelaunch', 'resources': 'ready', 'keyboard': 'live',
  'keyboard-clear': 'live', 'touch-controls': 'live', 'touch-cancel': 'live',
  'direct-touch': 'live', 'launch': 'prelaunch', 'list': 'ready', 'read': 'ready',
  'write': 'ready', 'remove': 'ready', 'sync': 'ready',
});

// ---------------------------------------------------------------------------
// 信封工具：严格复刻 parseRuntimeInboundMessage 的校验语义
// ---------------------------------------------------------------------------

/** epoch 必须是 safe integer 且 > 0（官方 isRuntimeProtocolEpoch）。 */
export function isRuntimeProtocolEpoch(v) {
  return Number.isSafeInteger(v) && Number(v) > 0;
}

/** 从 URL 查询串读 runtimeEpoch；无则返回 null（由调用方决定默认策略）。 */
export function readRuntimeEpoch(search = (typeof location !== 'undefined' ? location.search : '')) {
  try {
    const q = new URLSearchParams(search);
    const raw = q.get(RUNTIME_EPOCH_QUERY_PARAMETER);
    if (raw === null) return null;
    const n = Number(raw);
    return isRuntimeProtocolEpoch(n) ? n : null;
  } catch { return null; }
}

const isRecord = (v) => v !== null && typeof v === 'object' && !Array.isArray(v);
const COMMAND_SET = new Set([...RUNTIME_PROTOCOL_COMMANDS, ...RUNTIME_PROTOCOL_OPTIONAL_COMMANDS]);

/**
 * 校验入站消息信封。返回 { command, request, payload } 或 null（丢弃）。
 * 严格按官方 parseRuntimeInboundMessage：protocol/game/epoch 三元组 + epoch 匹配。
 */
export function parseRuntimeCommand(value, expectedGame, expectedEpoch) {
  if (!isRecord(value)) return null;
  if (value.protocol !== HOST_PROTOCOL) return null;
  if (typeof value.game !== 'string' || value.game !== expectedGame) return null;
  if (!isRuntimeProtocolEpoch(value.epoch)) return null;
  if (expectedEpoch !== undefined && expectedEpoch !== null && value.epoch !== expectedEpoch) return null;
  if (typeof value.command !== 'string' || !COMMAND_SET.has(value.command)) return null;
  return { command: value.command, request: typeof value.request === 'string' ? value.request : undefined };
}

/** 构造出站事件信封（Runtime → Host）。 */
export function makeRuntimeEvent(game, epoch, event, payload = {}) {
  return { protocol: HOST_PROTOCOL, game, epoch, event, ...(payload || {}) };
}

/** 构造命令响应信封（Runtime → Host，带 request + ok）。 */
export function makeRuntimeResponse(game, epoch, request, ok, extra = {}) {
  return { protocol: HOST_PROTOCOL, game, epoch, request, ok: !!ok, ...(extra || {}) };
}

/** 校验出站响应能被官方 isRuntimeResponseMessage 识别（自测用）。 */
export function isRuntimeResponseMessage(v) {
  return isRecord(v) && typeof v.request === 'string' && typeof v.ok === 'boolean';
}

// ---------------------------------------------------------------------------
// netplay 时序对象：逐字复刻 parseMeasuredNetplayTiming 的严格校验
// ---------------------------------------------------------------------------

/**
 * 官方规则（netplay-timing.mjs）：
 *   phase==="ready", automatic bool, adonisMode 1|2,
 *   inputDelay 0..9, fullDelay 1..31, predictionReserve 0..2,
 *   rttP95Us 1..1e6(整数,微秒), samples 96..120, lost 0..72,
 *   route ∈ {rtc,relay,spectator},
 *   fullDelay === max(1, ceil(floor(rttP95Us/2)*60/1e6)),
 *   predictionReserve <= fullDelay - (automatic?1:0),
 *   adonisMode===1 → predictionReserve===0;
 *     否则 reserveBudget===0 ? predictionReserve!==0 : predictionReserve>=1,
 *   automatic → inputDelay === max(0, fullDelay - predictionReserve).
 */
export function isValidNetplayTiming(v) {
  if (!v || typeof v !== 'object' || Array.isArray(v)) return false;
  const int = (k, min, max) => Number.isInteger(v[k]) && v[k] >= min && v[k] <= max;
  if (v.phase !== 'ready' || typeof v.automatic !== 'boolean') return false;
  if (v.adonisMode !== 1 && v.adonisMode !== 2) return false;
  if (!int('inputDelay', 0, 9) || !int('fullDelay', 1, 31) || !int('predictionReserve', 0, 2)) return false;
  if (!int('rttP95Us', 1, 1_000_000) || !int('samples', 96, 120) || !int('lost', 0, 72)) return false;
  if (!['rtc', 'relay', 'spectator'].includes(String(v.route))) return false;
  const fd = v.fullDelay, pr = v.predictionReserve;
  const budget = fd - (v.automatic ? 1 : 0);
  if (fd !== Math.max(1, Math.ceil(Math.floor(v.rttP95Us / 2) * 60 / 1_000_000))) return false;
  if (pr > budget) return false;
  if (v.adonisMode === 1) { if (pr !== 0) return false; }
  else if (budget === 0) { if (pr === 0) return false; }
  else if (pr < 1) return false;
  if (v.automatic && v.inputDelay !== Math.max(0, fd - pr)) return false;
  return true;
}

const clampInt = (n, lo, hi, fallback) => {
  if (!Number.isFinite(Number(n))) return fallback;
  return Math.max(lo, Math.min(hi, Math.round(Number(n))));
};

/**
 * 把网页 configure 的 BRO 专属设置映射成 Lua 表构造表达式（纯函数，便于单测）。
 * 只输出实际下发的字段；缺省项交给游戏侧 web_settings.lua 的 normalize 回落。
 * 这些字段由 launcher 在 game==='bro' 时填入 configure.options，其他作品不会带上。
 */
export function broWebSettingsExpression(options) {
  const o = options && typeof options === 'object' ? options : {};
  const numeric = (v, min, max) => typeof v === 'number' && Number.isFinite(v)
    ? String(Math.max(min, Math.min(max, Math.round(v)))) : null;
  const flag = (v) => typeof v === 'boolean' ? (v ? 'true' : 'false') : null;
  // 3AO：弹型贴图风格，白名单枚举，输出带引号的 Lua 字符串。
  const texture = (v) => (v === 'legacy' || v === 'original') ? "'legacy'"
    : (v === 'new' || v === 'beauty') ? "'new'" : null;
  // 网页键盘改键：launcher 的 code 别名 → VK 数字，按 BRO 游戏的字段名下发。
  // 只输出白名单字段；游戏侧只改 setting.keys[name]，不替换整表。
  const keyGroups = {
    keys: ['up', 'down', 'left', 'right', 'slow', 'shoot', 'spell', 'special'],
    keysys: ['repfast', 'repslow', 'menu', 'snapshot', 'retry'],
  };
  const keys = (v) => {
    if (!v || typeof v !== 'object') return null;
    const groups = [];
    for (const group of Object.keys(keyGroups)) {
      const sub = v[group];
      if (!sub || typeof sub !== 'object') continue;
      const items = [];
      for (const name of keyGroups[group]) {
        if (!Object.prototype.hasOwnProperty.call(sub, name)) continue;
        const vk = numeric(sub[name], 1, 255);
        if (vk != null) items.push(`${name}=${vk}`);
      }
      if (items.length) groups.push(`${group}={${items.join(',')}}`);
    }
    return groups.length ? '{' + groups.join(',') + '}' : null;
  };
  const map = [
    ['broResolution', 'resolution', (v) => numeric(v, 360, 1440)],
    ['broBgmVolume', 'bgmVolume', (v) => numeric(v, 0, 100)],
    ['broSfxVolume', 'sfxVolume', (v) => numeric(v, 0, 100)],
    ['broLock60', 'lock60', flag],
    ['broDisplayBG', 'displayBG', flag],
    ['broAuraDistortion', 'auraDistortion', flag],
    ['broEasyRender', 'easyRender', flag],
    ['broDisplayKey', 'displayKey', flag],
    ['broFrameskip', 'frameskip', flag],
    ['broBulletTexture', 'bulletTexture', texture],
    ['broKeys', 'keys', keys],
  ];
  const parts = [];
  for (const [src, dst, conv] of map) {
    if (!Object.prototype.hasOwnProperty.call(o, src)) continue;
    const val = conv(o[src]);
    if (val != null) parts.push(`${dst}=${val}`);
  }
  return '{' + parts.join(',') + '}';
}

/** 从 netplay URL 里取房间号（?room= / ?run=），取不到返回空串。 */
function netplayRoomFromUrl(url) {
  if (typeof url !== 'string' || !url) return '';
  try {
    const u = new URL(url, 'http://localhost');
    return u.searchParams.get('room') || u.searchParams.get('run') || '';
  } catch { return ''; }
}

/**
 * netplay URL → 信令 WebSocket 地址。
 * 房间流程下发的 URL 可能带 launcher 自己的查询参数（run/runtimeGeneration），
 * BRO 信令只认 ws(s)://host:port，所以只保留协议+主机+端口。
 */
function netplaySignalUrl(url) {
  if (typeof url !== 'string' || !url) return '';
  if (!/^wss?:\/\//i.test(url)) return '';
  try {
    const u = new URL(url);
    return `${u.protocol}//${u.host}`;
  } catch { return ''; }
}

/** Lua 字符串字面量（只用于自机类名/系统 csv，内容受限）。 */
function luaStr(v) {
  if (v === null || v === undefined) return 'nil';
  return '"' + String(v).replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/[\r\n]/g, ' ') + '"';
}

/**
 * 规整 launcher 下发的 netplayLoadouts。
 * 支持 ['reimu_player', ...] 与 [{char|character, systems|broSystems, index}]，
 * 也容忍按槽位索引的对象。缺项留空（Lua 侧保持默认自机）。
 */
function normalizeNetplayLoadouts(loadouts, count) {
  const out = [];
  const push = (slot, raw) => {
    const p = Number(slot);
    if (!Number.isInteger(p) || p < 0 || p >= count) return;
    if (out.some((x) => x.slot === p)) return;
    let char = null, systems = '', index = 1;
    if (typeof raw === 'string') char = raw;
    else if (raw && typeof raw === 'object') {
      char = typeof raw.char === 'string' ? raw.char : (typeof raw.character === 'string' ? raw.character : null);
      const sys = raw.systems || raw.broSystems;
      if (Array.isArray(sys)) systems = sys.filter((n) => Number.isInteger(n) && n >= 1 && n <= 255).join(',');
      else if (typeof sys === 'string') systems = sys;
      const idx = Number(raw.index);
      if (Number.isInteger(idx) && idx >= 1 && idx <= 4) index = idx;
    }
    if (!char && !systems) return;
    out.push({ slot: p, char: char || 'reimu_player', systems, index });
  };
  if (Array.isArray(loadouts)) {
    loadouts.forEach((raw, i) => push(i, raw));
  } else if (loadouts && typeof loadouts === 'object') {
    for (const [k, raw] of Object.entries(loadouts)) push(k, raw);
  }
  return out.sort((a, b) => a.slot - b.slot);
}

/**
 * 把房间选项（房主选定的全队系统 + 章节游玩队列）映射成 Lua 表构造表达式。
 * 纯函数便于单测；只输出合法整数组，缺省交给 Lua 侧回落。
 * 这些字段由 launcher 经 configure.options 下发（netplayBro*），其他作品不会带上。
 */
export function broRoomOptionsExpression(options) {
  const o = options && typeof options === 'object' ? options : {};
  const arr = (v, max, dedupe) => {
    if (!Array.isArray(v)) return null;
    const seen = new Set(), out = [];
    for (const n of v) {
      if (!Number.isInteger(n) || n < 1 || n > max) continue;
      if (dedupe && seen.has(n)) continue;
      seen.add(n);
      out.push(String(n));
    }
    return '{' + out.join(',') + '}';
  };
  const systems = arr(o.netplayBroSystems, 11, true);
  const queue = arr(o.netplayBroStageQueue, 19, false);
  const parts = [];
  if (systems !== null) parts.push('systems=' + systems);
  if (queue !== null) parts.push('queue=' + queue);
  // 3AR：房间难度 0=Vanilla(当前弹幕) / 1=Legacy(legacy 4.0 弹幕)。
  // 只接受 0/1；缺省（单人/未指定）不写该字段 → Lua 侧保持默认 Hard。
  if (o.netplayDifficulty === 0 || o.netplayDifficulty === 1) {
    parts.push('legacy=' + (o.netplayDifficulty === 1 ? 'true' : 'false'));
  }
  // 3AV：难度 2/3 是独立章节入口（非 19 关队列）。legacy 保持 Hard(缺省)，
  // 额外下发 chapter 让 Lua 把首关指向独立 stage group；静态 label 映射，
  // 不信任外部字符串，零新协议字段（仍走 configure.options.netplayDifficulty）。
  if (o.netplayDifficulty === 2) parts.push("chapter='aex'");
  if (o.netplayDifficulty === 3) parts.push("chapter='badapple'");
  return '{' + parts.join(',') + '}';
}

/**
 * 把 BRO 的 survey 报告（rtt50 毫秒、route direct/turn）转成 eagler 严格时序对象。
 *
 * 对齐要点：
 *  - BRO 用毫秒浮点、direct|turn；eagler 要微秒整数、rtc|relay|spectator。
 *  - rttP95Us → fullDelay 由官方公式反推（fullDelay 是派生量，不能自由填）。
 *  - samples 官方要求 96..120；BRO 单次 survey 只有 count 发（默认 8），
 *    **这里用声明窗口 96 并保留 measuredLost 的实测比例**（诚实标注：这是声明采样窗，
 *    不是伪造 96 个实测样本）。接入 eagler 时按其 sampling 窗口重载即可。
 *
 * @param {object} m  BRO survey 报告或单链路报告：{rtt50|rttMs|rttP95Us, route|path, lost?, sent?, samples?}
 * @param {object} opts {automatic?, adonisMode?, samples?, spectator?}
 * @returns {object|null} 过 isValidNetplayTiming 的对象；无法构造时返回 null
 */
export function toNetplayTiming(m, opts = {}) {
  if (!m || typeof m !== 'object') return null;
  const spectator = opts.spectator === true || m.route === 'spectator' || m.path === 'spectator';
  if (spectator) {
    // spectator 路线仍需满足数值约束；给最小合法 RTT，由公式定 fullDelay。
    m = { ...m, rttP95Us: 1000, route: 'spectator' };
  }
  let rttP95Us;
  if (Number.isFinite(m.rttP95Us)) rttP95Us = clampInt(m.rttP95Us, 1, 1_000_000, 1000);
  else {
    // BRO 现有报告是 rtt50（毫秒，浮点）。P95 用 ms * 1000 * 1.5 作保守估计。
    const ms = Number.isFinite(m.rttP95Ms) ? m.rttP95Ms
      : Number.isFinite(m.rtt50) ? m.rtt50
      : Number.isFinite(m.rttMs) ? m.rttMs : 0.7;
    rttP95Us = clampInt(ms * 1000 * 1.5, 1, 1_000_000, 1000);
  }
  // fullDelay 由官方公式派生，不可自由指定。
  const fullDelay = Math.max(1, Math.ceil(Math.floor(rttP95Us / 2) * 60 / 1_000_000));
  // automatic 模式下 inputDelay = fullDelay - predictionReserve，且必须 <= 9。
  // 高 RTT（fullDelay 太大）会让 automatic 落入契约无解区（fullDelay>11），
  // 此时降级为 manual（automatic=false），inputDelay 可独立取 0..9。
  let automatic = opts.automatic !== false;
  const manualInputDelay = clampInt(opts.inputDelay, 0, 9,
    Math.min(9, Math.max(0, fullDelay - 2)));
  if (automatic) {
    const probeReserve = Math.min(2, Math.max(0, fullDelay - 1));
    if (Math.max(0, fullDelay - probeReserve) > 9) automatic = false;
  }
  let adonisMode = opts.adonisMode === 1 ? 1 : 2;
  const reserveBudget = fullDelay - (automatic ? 1 : 0);
  // adonisMode 2 要求预算 0 时 reserve!=0（不可能，因为 reserve<=budget=0）→ 该组合无解；
  // 此时退回 adonisMode 1（reserve=0 合法）。这是契约本身的边界，不是我们的自由裁量。
  if (adonisMode === 2 && reserveBudget === 0) adonisMode = 1;
  const mode = adonisMode;
  const predictionReserve = mode === 1 ? 0 : Math.min(2, reserveBudget);
  const inputDelay = automatic ? Math.max(0, fullDelay - predictionReserve) : manualInputDelay;
  const sent = Number.isFinite(m.sent) ? m.sent : 0;
  const lost = Number.isFinite(m.lost) ? m.lost : 0;
  const samples = clampInt(opts.samples ?? m.samples ?? 96, 96, 120, 96);
  const rawRoute = String(m.route || m.path || 'direct').toLowerCase();
  const route = spectator ? 'spectator' : (rawRoute === 'turn' || rawRoute === 'relay' ? 'relay' : 'rtc');
  const out = {
    phase: 'ready', automatic, adonisMode: mode,
    inputDelay, fullDelay, predictionReserve,
    rttP95Us, samples,
    lost: clampInt(lost / Math.max(1, sent) * samples, 0, 72, 0),
    route,
  };
  return isValidNetplayTiming(out) ? out : null;
}

// ---------------------------------------------------------------------------
// KeyboardEvent.code → VK（BRO input.mjs 注入用数值 vk）
// ---------------------------------------------------------------------------

const CODE_VK = Object.freeze({
  KeyZ: 90, KeyX: 88, KeyC: 67, KeyV: 86,
  ArrowUp: 38, ArrowDown: 40, ArrowLeft: 37, ArrowRight: 39,
  ShiftLeft: 16, ShiftRight: 16, ControlLeft: 17, ControlRight: 17,
  Space: 32, Enter: 13, Escape: 27, KeyA: 65, KeyB: 66, KeyD: 68,
  KeyS: 83, KeyW: 87, KeyQ: 81, KeyE: 69, KeyR: 82, KeyF: 70,
  Digit1: 49, Digit2: 50, Digit3: 51, Digit4: 52, Digit5: 53,
});
export function codeToVk(code) { return CODE_VK[String(code)] ?? null; }

// ---------------------------------------------------------------------------
// 主适配器：EaglerRuntimeAdapter
// ---------------------------------------------------------------------------

/**
 * @typedef {object} AdapterOptions
 * @property {string}   game          eagler GameId（BRO 尚未注册进 product-catalog，默认 'bro'；
 *                                    接入时由网站决定正式 id，构造参数可覆盖）
 * @property {number}   epoch         runtimeEpoch；缺省时从 URL 读，再缺省用 Date.now()
 * @property {function} post          出站消息函数 (msg) => void；默认 parent.postMessage(msg,'*')
 * @property {object}   multi         BROMultiplayer 实例（可选；configure 时也可注入工厂）
 * @property {object}   input         attachInput() 返回值（可选）
 * @property {object}   storage       createStorage() 返回值（可选，供 list/read/write/sync）
 * @property {object}   resourceCache createResourceCache() 返回值（可选，resources 命令按 hash 缓存导入包）
 * @property {function} launchHook    launch 前 await 的启动函数（可选，runtime 入口用）
 * @property {object}   messageTarget 仅接受来自该 window 的消息（可选，协议壳用）
 * @property {string}   allowedOrigin 仅接受该 origin 的消息（可选，协议壳用）
 * @property {function} getFrameHealth   () => {fps,maxGapMs,frameMs}（可选）
 * @property {function} getAudioHealth   () => {queuedMs,backend,underruns}（可选）
 * @property {object}   transport     peerTransportGlobal 桥接对象（可选，见下）
 */

export class EaglerRuntimeAdapter {
  constructor(opts = {}) {
    this.game = String(opts.game || 'bro');
    // eagler 的 ready.saveRoot 必须与 product-catalog 的 storage.saveRoot 对齐。
    // BRO 目录声明 '/bro'（见 product-catalog.mjs）；用构造参数可覆盖。
    this.saveRoot = String(opts.saveRoot || '/bro');
    this.epoch = isRuntimeProtocolEpoch(opts.epoch) ? opts.epoch
      : (readRuntimeEpoch() ?? (typeof Date !== 'undefined' ? Date.now() : 1));
    // Date.now() 是毫秒正整数，天然 > 0 且 safe integer；极端fallback保底 1。
    if (!isRuntimeProtocolEpoch(this.epoch)) this.epoch = 1;
    this.post = typeof opts.post === 'function' ? opts.post
      : ((m) => { if (typeof parent !== 'undefined' && parent.postMessage) parent.postMessage(m, '*'); });
    this.multi = opts.multi || null;
    this.input = opts.input || null;
    this.storage = opts.storage || null;
    this.resourceCache = opts.resourceCache || null;
    this.launchHook = typeof opts.launchHook === 'function' ? opts.launchHook : null;
    this.messageTarget = opts.messageTarget || (typeof parent !== 'undefined' ? parent : null);
    this.allowedOrigin = String(opts.allowedOrigin || (typeof location !== 'undefined' ? location.origin : '*'));
    this.addEventListenerImpl = typeof opts.addEventListener === 'function' ? opts.addEventListener
      : ((type, fn) => { if (typeof addEventListener === 'function') addEventListener(type, fn); });
    this.removeEventListenerImpl = typeof opts.removeEventListener === 'function' ? opts.removeEventListener
      : ((type, fn) => { if (typeof removeEventListener === 'function') removeEventListener(type, fn); });
    this.getFrameHealth = typeof opts.getFrameHealth === 'function' ? opts.getFrameHealth : null;
    this.getAudioHealth = typeof opts.getAudioHealth === 'function' ? opts.getAudioHealth : null;
    this.ccall = typeof opts.ccall === 'function' ? opts.ccall
      : (this.multi && this.multi.ccall) || null;
    this.multiFactory = typeof opts.multiFactory === 'function' ? opts.multiFactory : null;
    // 阶段 3AZ：联机主循环接线状态（诊断用，随 runtime-info 上报）。
    this.multiStarted = false;
    this.multiModule = null;
    this.netplayWiring = null;
    this.netplayConnectError = null;
    this.netplayFrameError = null;
    this.netplayLoadoutsApplied = 0;
    // fs 后端（可选注入）：{list(), read(path), write(path,bytes), remove(path)}。
    // 未注入时退回 storage.inspect + ccall BRO_fs_*（接入 eagler 时按其 FS 桥接）。
    this.fs = opts.fs || null;

    this.phase = 'ready';                 // ready → prelaunch → live
    this.config = { music: 'ogg', options: {} };
    this.launchSerial = 0;
    this.firstFrameSent = false;
    this.frameTimer = null;
    this.eventBus = new Set();
    this._onMessage = null;
    this._periodicMs = Number.isFinite(opts.periodicMs) ? Math.max(100, opts.periodicMs) : 1000;

    /** peerTransportGlobal：BRO 侧传输桥（命名按 eagler __<game>PeerTransport 约定）。
     *  eagler 官方公开文档未给出其逐方法签名（见 PROBE_3V 边界），因此这里做成
     *  「注入优先 + 默认桥」：宿主注入同名对象则透明转发；否则用 BRO 自带 NetMesh
     *  的最小适配面。接入网站时按实际端口核对方法名即可，改这里一处。 */
    this.transportName = opts.transportName || `__${this.game}PeerTransport`;
    this.transport = opts.transport || null;

    /** 观战阶段 A：发布端（player-0 上传 0xe8 帧）/ 观战端（收帧确定性重演）。
     *  两者都只在 netplay* 配置带 spectator 标志时按需构造，普通对局零影响。 */
    this.spectatorClient = null;
    this.spectatorUploader = null;
    this.spectatorPlayback = null;
    this.spectatorFrame = 0; // 发布端对局内帧号（0 起，观战端从 0 连续取）
    this.WebSocketCtor = opts.WebSocketCtor || (typeof WebSocket !== 'undefined' ? WebSocket : null);
    this.netFactory = opts.netFactory || null; // 注入 Lua 门面：{call,jsonCall,ccall}
  }

  // ---- 生命周期 -----------------------------------------------------------

  /** 开始收 postMessage；立即上报 ready 事件。 */
  attach() {
    if (!this._onMessage) {
      this._onMessage = (event) => {
        if (event && this.messageTarget && event.source && event.source !== this.messageTarget) return;
        if (event && this.allowedOrigin !== '*' && event.origin && event.origin !== this.allowedOrigin) return;
        return this._handleMessage(event && event.data);
      };
      this.addEventListenerImpl('message', this._onMessage);
    }
    this._emit('ready', { saveRoot: this.saveRoot });
    if (typeof setInterval === 'function' && !this.frameTimer) {
      this.frameTimer = setInterval(() => this._emitPeriodic(), this._periodicMs);
    }
    this._installTransportGlobal();
    return this;
  }

  detach() {
    if (this._onMessage) this.removeEventListenerImpl('message', this._onMessage);
    this._onMessage = null;
    if (this.frameTimer && typeof clearInterval === 'function') clearInterval(this.frameTimer);
    this.frameTimer = null;
    return this;
  }

  /** 订阅本地事件（便于宿主转成 DOM/日志；不影响 postMessage 上报）。 */
  on(fn) { this.eventBus.add(fn); return () => this.eventBus.delete(fn); }

  _emit(event, payload) {
    const msg = makeRuntimeEvent(this.game, this.epoch, event, payload);
    try { this.post(msg); } catch { /* 宿主可能已关闭 */ }
    for (const fn of this.eventBus) { try { fn(event, payload, msg); } catch { /* 订阅者异常不影响协议 */ } }
    if (event === 'first-frame') this.firstFrameSent = true;
    return msg;
  }

  _emitPeriodic() {
    if (this.phase !== 'live') return;
    if (this.getFrameHealth) {
      const h = this.getFrameHealth() || {};
      this._emit('frame-health', {
        fps: Number.isFinite(h.fps) ? h.fps : 60,
        maxGapMs: Number.isFinite(h.maxGapMs) ? h.maxGapMs : 0,
        frameMs: Number.isFinite(h.frameMs) ? h.frameMs : 16.7,
      });
    }
    if (this.getAudioHealth) {
      const a = this.getAudioHealth() || {};
      this._emit('audio-health', {
        queuedMs: Number.isFinite(a.queuedMs) ? a.queuedMs : 0,
        minQueuedMs: Number.isFinite(a.minQueuedMs) ? a.minQueuedMs : 0,
        backend: String(a.backend || 'webaudio'),
        underruns: Number.isFinite(a.underruns) ? a.underruns : 0,
        robust: a.robust !== false,
      });
    }
  }

  /** 由渲染循环在首帧呈现后调用一次（launch 之后）。幂等。 */
  notifyFirstFrame(launchSerial) {
    if (this.firstFrameSent) return false;
    this.phase = 'live';
    this._emit('first-frame', { launchSerial: launchSerial ?? this.launchSerial });
    return true;
  }

  /** 上报 netplay 时序；自动转成 eagler 严格形状。 */
  reportNetplayTiming(report, opts = {}) {
    const timing = toNetplayTiming(report, opts);
    if (!timing) {
      const routeLabel = report ? String(report.path || report.route || '') : '';
      this._emit('error', { message: 'invalid-netplay-timing', error: routeLabel });
      return null;
    }
    // eagler 的时序上报走 network-request 事件（可选事件白名单），宿主据此读取。
    this._emit('network-request', { type: 'timing', timing });
    return timing;
  }

  // ---- 命令分发 -----------------------------------------------------------

  _handleMessage(data) {
    const parsed = parseRuntimeCommand(data, this.game, this.epoch);
    if (!parsed) return null;             // 信封不符：静默丢弃（与官方一致）
    const { command, request } = parsed;
    const payload = data || {};
    try {
      const result = this._dispatch(command, payload);
      // 异步命令（当前仅 sync）：flush 落盘完成后才回包，使 launcher 的
      // `await sync()` 真正等到数据落 IndexedDB，而不是拿到 ok 就走。
      if (result && result.deferred && typeof result.deferred.then === 'function') {
        result.deferred.then(
          (value) => {
            if (request === undefined) return;
            const extra = value && typeof value === 'object' && !Array.isArray(value) ? value : {};
            this._reply(request, true, { ...(result.response || {}), ...extra });
          },
          (e) => { if (request !== undefined) this._reply(request, false, { error: String(e && e.message || e) }); },
        );
        return result;
      }
      if (request !== undefined) {
        this._reply(request, true, result && result.response);
      }
      return result;
    } catch (e) {
      if (request !== undefined) this._reply(request, false, { error: String(e && e.message || e) });
      this._emit('error', { message: `command:${command}`, error: String(e && e.message || e) });
      return null;
    }
  }

  _reply(request, ok, extra) {
    this.post(makeRuntimeResponse(this.game, this.epoch, request, ok, extra || {}));
  }

  /** 把 configure.options 里的 BRO 专属设置下发到游戏本体。
   *  configure（prelaunch）与 launch 各调一次：前者覆盖「先配置后进关」，
   *  后者兜底「setting 表在 boot 之后才就绪」。expr 为 '{}' 时无专属项，跳过。
   *  bro_eval 失败（WASM 尚未 boot）不打断协议，complete_load 会再 flush。 */
  applyWebSettings() {
    if (!this.ccall || typeof this.ccall !== 'function') return false;
    const options = (this.config && this.config.options) || {};
    const expr = broWebSettingsExpression(options);
    if (expr === '{}') return false;
    try {
      this.ccall('bro_eval', 'string', ['string'],
        [`return tostring(BRO_web_apply_settings(${expr}))`]);
      return true;
    } catch { return false; }
  }

  /** 把房主选定的系统/章节队列下发到游戏本体（configure 与 launch 各调一次，
   *  与 applyWebSettings 同样的双调用兜底）。房间选项只在联机 configure 里出现；
   *  WASM 尚未 boot 时 bro_eval 失败不致命，launch 时会重放。 */
  applyRoomOptions() {
    if (!this.ccall || typeof this.ccall !== 'function') return false;
    const options = (this.config && this.config.options) || {};
    const expr = broRoomOptionsExpression(options);
    if (expr === '{}') return false;
    try {
      this.ccall('bro_eval', 'string', ['string'],
        [`return tostring(BRO_room_setup(${expr}))`]);
      return true;
    } catch { return false; }
  }

  /** Lua VM 建立后的唯一真正下发点。
   *  configure/launch 里的两次 apply 都早于 bro_boot：那时 ccall 尚未注入、
   *  Lua 状态 L 也不存在，调用只会静默返回 false。runtime_entry 在 bro_boot
   *  成功之后、进关之前调用本方法，网页设置与房间选项才真正落到游戏里。 */
  applyAfterBoot() {
    const webSettingsApplied = this.applyWebSettings();
    const roomOptionsApplied = this.applyRoomOptions();
    this._emit('runtime-info', {
      renderer: 'webgl2', architecture: 'wasm32-lua515',
      version: 'bro-5.1.2+3v', epoch: this.epoch, phase: 'boot',
      webSettingsApplied, roomOptionsApplied,
    });
    return { webSettingsApplied, roomOptionsApplied };
  }

  _dispatch(command, p) {
    switch (command) {
      case 'configure':    return this._cmdConfigure(p);
      case 'resources':    return this._cmdResources(p);
      case 'keyboard':     return this._cmdKeyboard(p);
      case 'keyboard-clear': return this._cmdKeyboardClear();
      case 'touch-controls': return { ok: true };
      case 'touch-cancel': return { ok: true };
      case 'direct-touch': return { ok: true };
      case 'launch':       return this._cmdLaunch(p);
      case 'list':         return this._cmdList();
      case 'read':         return this._cmdRead(p);
      case 'write':        return this._cmdWrite(p);
      case 'remove':       return this._cmdRemove(p);
      case 'sync':         return this._cmdSync();
      case 'playback':     return this._cmdPlayback(p);
      case 'spectator-playback': return this._cmdSpectatorPlayback(p);
      default:             return { ok: true, ignored: true };  // 可选命令未实现：ok，不破坏宿主流程
    }
  }

  /** 阶段 3AZ：房间参数 → BROMultiplayer 构造参数（纯函数式，便于单测断言）。 */
  _multiOptions(np) {
    const n = np || {};
    return {
      ccall: this.ccall,
      input: this.input,
      room: n.room || 'bro-default',
      player: Number.isInteger(n.player) ? n.player : 0,
      maxPlayers: clampInt(n.playerCount, 1, 4, 2),
      iceServers: Array.isArray(n.iceServers) ? n.iceServers : [],
      preferTurn: n.preferTurn === true,
      // 3BF：正式房间默认 arm 轻量回滚；关卡级全量重放保持 opt-in。
      rollback: n.rollback !== false,
      autoFullReplay: n.autoFullReplay === true,
      ...(Number.isFinite(n.rollbackKeep) ? { rollbackKeep: n.rollbackKeep | 0 } : {}),
    };
  }

  /**
   * 阶段 3AZ：把正式房间流程接进联机主循环。
   *
   * 为什么以前是断的：`_cmdConfigure` 只在宿主注入 multiFactory 时才有门面，
   * 而 launcher 从来只发 netplay* 选项、不注入工厂 → this.multi 恒为 null，
   * runtime 主循环也只有单机分支（syncHeld + bro_frame）。结果是房间里既没有
   * 输入交换、也没有远端真身，远端只剩 POS 代理。
   *
   * 现在：netplayMode === 'lan' 且带信令地址时，本函数在进关之后（Lua 表就绪）
   * 动态加载 multiplayer.mjs、建链、下发房间人数/各槽自机、发起 match。
   * 返回 null 表示「本局不是联机」，调用方按单机路径推进。
   */
  async startNetplayIfConfigured() {
    const np = this.netplay || {};
    if (this.multiStarted) return this._netplaySummary();
    // 3BG 观战+联机同开：launcher 给纯观战端同时下发 netplayMode:'lan' 与
    // netplaySpectator:true。观战端的状态由 SpectatorClient.pump() 按 relay 帧
    // 确定性驱动，绝不能再建联机门面、调 BRO_net_start_match/sendReady 去重置
    // 关卡与 RNG —— 那会直接把重演打乱。runtime_entry 的帧循环已用 !spectator
    // 做门控，但本函数在那之前就跑，必须自己挡住。
    if (np.spectator === true) {
      this.netplayWiring = { attempted: true, ok: false, reason: 'spectator-client' };
      return null;
    }
    if (np.mode !== 'lan' || !np.url) {
      this.netplayWiring = { attempted: true, ok: false, reason: 'no-netplay' };
      return null;
    }
    if (typeof this.ccall !== 'function') {
      this.netplayWiring = { attempted: true, ok: false, reason: 'no-ccall' };
      return null;
    }
    try {
      if (!this.multi) {
        const mod = await this._loadMultiplayerModule();
        if (!mod) throw new Error('multiplayer module unavailable');
        this.multi = this._createMulti(mod, np);
      }
      const multi = this.multi;
      // 1) 先把「我是哪个槽 / 房间几个人 / 每人用谁」下发到 Lua，
      //    远端真身生成时要用到 loadouts（BRO_net_autospawn_remotes）。
      this.applyNetplaySlots();
      // 2) 建链（信令 + WebRTC）。失败不阻塞开局：单人也能先跑，链路起来后补齐。
      if (typeof multi.connect === 'function') {
        try { await multi.connect(); } catch (error) {
          // 单机也能先跑：链路失败只记录，链路起来后 startNet 会补齐。
          this.netplayConnectError = String(error && error.message || error);
        }
      }
      if (typeof multi.startNet === 'function') {
        const net = multi.startNet();
        if (net && typeof net.catch === 'function') void net.catch(() => {});
      }
      // 3) 发起 match：BRO_net_start_match 会设种子/关卡并 BRO_enter_stage。
      if (typeof multi.startMatch === 'function') multi.startMatch(undefined, np.seed ?? undefined);
      if (typeof multi.sendReady === 'function') multi.sendReady(true);
      this.multiStarted = true;
      this.netplayWiring = { attempted: true, ok: true, reason: 'started', connectError: this.netplayConnectError || null };
      this._emit('runtime-info', { netplay: this._netplaySummary() });
      return this._netplaySummary();
    } catch (error) {
      this.netplayWiring = { attempted: true, ok: false, reason: 'error', error: String(error && error.message || error) };
      this._emit('runtime-info', { netplay: this._netplaySummary() });
      throw error;
    }
  }

  /** 动态加载门面模块（源码树 ../src 与产物目录同层都兼容）。 */
  async _loadMultiplayerModule() {
    if (this.multiModule) return this.multiModule;
    const candidates = ['./multiplayer.mjs', '../src/multiplayer.mjs'];
    for (const spec of candidates) {
      try {
        const mod = await import(/* @vite-ignore */ spec);
        if (mod && (mod.BROMultiplayer || mod.default)) { this.multiModule = mod; return mod; }
      } catch { /* 试下一个候选路径 */ }
    }
    return null;
  }

  /** 用真实门面建链（宿主注入工厂时优先工厂，便于测试/自定义传输）。 */
  _createMulti(mod, np) {
    if (this.multiFactory) return this.multiFactory(this._multiOptions(np));
    const opts = this._multiOptions(np);
    const Ctor = mod && (mod.BROMultiplayer || mod.default);
    if (typeof Ctor !== 'function') throw new Error('BROMultiplayer class missing');
    let signal = null;
    const url = np.signalUrl || netplaySignalUrl(np.url);
    if (url && typeof mod.createWebSocketSignal === 'function') {
      // 信令语义与 web/test/browser/lobby.html 相同：client 收到 joined、
      // host 看到房间里第二个人时，才真正开始 WebRTC 协商。
      signal = mod.createWebSocketSignal({
        url,
        room: opts.room,
        player: opts.player,
        onMessage: (m) => this._onSignalMessage(m),
      });
    }
    const multi = new Ctor({
      ...opts,
      signal,
      role: opts.player === 0 ? 'host' : 'client',
      delay: Number.isFinite(np.inputDelay) ? np.inputDelay : undefined,
    });
    return multi;
  }

  _onSignalMessage(message) {
    const multi = this.multi;
    if (!multi || !message || typeof multi.startNet !== 'function') return;
    const kind = message.kind;
    if (kind === 'joined' && Number(multi.player) !== 0) void multi.startNet().catch(() => {});
    if (kind === 'peer' && Number(message.count) >= 2 && Number(multi.player) === 0) {
      void multi.startNet().catch(() => {});
    }
  }

  /** 把房间人数 / 本地槽 / 各槽自机下发给 Lua（远端真身生成依赖它）。 */
  applyNetplaySlots() {
    const np = this.netplay || {};
    if (typeof this.ccall !== 'function') return false;
    const count = clampInt(np.playerCount, 1, 4, 1);
    const slot = Number.isInteger(np.player) ? np.player : 0;
    try {
      this.ccall('bro_eval', 'string', ['string'],
        [`return tostring(BRO_net_set_player_count(${count}))`]);
      this.ccall('bro_eval', 'string', ['string'],
        [`return tostring(BRO_net_set_slot(${slot}))`]);
      const list = normalizeNetplayLoadouts(np.loadouts, count);
      for (const item of list) {
        const expr = `return tostring(BRO_net_set_loadout(${item.slot}, ${luaStr(item.char)}, ${luaStr(item.systems)}, ${item.index}))`;
        this.ccall('bro_eval', 'string', ['string'], [expr]);
      }
      this.netplayLoadoutsApplied = list.length;
      return true;
    } catch (error) {
      this.netplayWiring = { attempted: true, ok: false, reason: 'slots', error: String(error && error.message || error) };
      return false;
    }
  }

  /** 单帧联机推进：由 runtime 主循环调用（锁步语义全在门面里）。 */
  stepNetplayFrame(frame) {
    const multi = this.multi;
    if (!multi || typeof multi.stepGame !== 'function') return { advanced: false, reason: 'no-multi' };
    let advanced = false;
    const advance = () => {
      advanced = true;
      this.ccall('bro_frame', 'number', [], []);
    };
    try {
      const out = multi.stepGame(frame, advance);
      // out === null 表示开局栅栏未开（只喂输入不推进），此时不能扣时间步长。
      return { advanced, gated: out === null, frame };
    } catch (error) {
      this.netplayFrameError = String(error && error.message || error);
      return { advanced, frame, error: this.netplayFrameError };
    }
  }

  _netplaySummary() {
    const multi = this.multi;
    const np = this.netplay || {};
    return {
      wired: !!this.multiStarted,
      room: np.room || null,
      player: Number.isInteger(np.player) ? np.player : 0,
      playerCount: clampInt(np.playerCount, 1, 4, 1),
      slots: this.netplayLoadoutsApplied || 0,
      frames: multi && multi.stats ? multi.stats.frames : 0,
      netReady: !!(multi && multi.net && multi.net.ready),
      wiring: this.netplayWiring || null,
      connectError: this.netplayConnectError || null,
      error: this.netplayFrameError || null,
    };
  }

  _cmdConfigure(p) {
    this.config = {
      music: String(p.music || 'ogg'),
      language: p.language,
      options: (p.options && typeof p.options === 'object') ? { ...p.options } : {},
    };
    this.phase = 'prelaunch';
    // 线上增量资源：宿主可经 configure.options.resourceManifestUrl 下发清单地址。
    // 只登记地址（同步、零网络）；真正的 refresh+prefetch 在 launch 前由 shell 执行。
    const manifestUrl = this.config.options.resourceManifestUrl || p.resourceManifestUrl;
    if (typeof manifestUrl === 'string' && manifestUrl && this.resourceCache?.setManifestUrl) {
      try { this.resourceCache.setManifestUrl(manifestUrl); } catch { /* 非致命 */ }
    }
    // netplay* → BRO 门面参数：只记录，建链由 startNet 时按此配置。
    const o = this.config.options;
    this.netplay = {
      mode: o.netplayMode === 'lan' ? 'lan' : null,
      url: typeof o.netplayUrl === 'string' ? o.netplayUrl : null,
      player: Number.isInteger(o.netplayPlayer) ? o.netplayPlayer : null,
      playerCount: clampInt(o.netplayPlayerCount, 1, 4, 2),
      seed: Number.isFinite(o.netplaySeed) ? o.netplaySeed : null,
      difficulty: o.netplayDifficulty,
      loadouts: Array.isArray(o.netplayLoadouts) ? o.netplayLoadouts : [],
      systems: Array.isArray(o.netplayBroSystems) ? o.netplayBroSystems : [],
      stageQueue: Array.isArray(o.netplayBroStageQueue) ? o.netplayBroStageQueue : [],
      iceServers: Array.isArray(o.netplayIceServers) ? o.netplayIceServers : [],
      preferTurn: o.netplayPreferTurn === true,
      spectator: o.netplaySpectator === true,
      spectatorId: typeof o.netplaySpectatorId === 'string' ? o.netplaySpectatorId : '',
      inputDelay: Number.isFinite(o.netplayInputDelay) ? o.netplayInputDelay : null,
      inputDelayAuto: o.netplayInputDelayAuto === true,
      predictionReserve: Number.isFinite(o.netplayPredictionReserve) ? o.netplayPredictionReserve : null,
      adonisMode: o.netplayAdonisMode === 1 ? 1 : 2,
      predictionLimit: Number.isFinite(o.netplayPredictionLimit) ? o.netplayPredictionLimit : null,
      // 3BF：把回滚开关接进正式房间。此前门面从不 arm，预测纠错与 DIGEST 分叉
      // 事件只入队不消费 —— 多人一旦出现预测推翻就永久分叉、无人修正。
      // 轻量回滚默认开启（只回滚玩家/计分状态，边界见 PROBE_3K），可用
      // netplayRollback:false 显式关掉；关卡级全量重放昂贵，仍为显式 opt-in。
      rollback: o.netplayRollback !== false,
      autoFullReplay: o.netplayAutoFullReplay === true,
      rollbackKeep: Number.isFinite(o.netplayRollbackKeep) ? (o.netplayRollbackKeep | 0) : undefined,
    };
    // 阶段 3AZ：房间号/信令地址解析（netplay URL 形如 ws://host:port?room=xxx）。
    this.netplay.room = (typeof o.netplayRoom === 'string' && o.netplayRoom)
      || netplayRoomFromUrl(this.netplay.url) || 'bro-default';
    this.netplay.signalUrl = netplaySignalUrl(this.netplay.url);
    // 若宿主提供工厂，按 eagler netplay 参数构造门面（对齐 URL/player/人数）。
    if (this.multiFactory && typeof this.ccall === 'function') {
      this.multi = this.multiFactory(this._multiOptions(this.netplay));
    }
    // 网站设置对齐：把网页设置为权威下发到游戏本体（音量/分辨率/3D背景/锁帧等）。
    const webSettingsApplied = this.applyWebSettings();
    const roomOptionsApplied = this.applyRoomOptions();
    this._emit('runtime-info', {
      renderer: 'webgl2', architecture: 'wasm32-lua515',
      version: 'bro-5.1.2+3v', epoch: this.epoch,
      webSettingsApplied, roomOptionsApplied,
      resourceManifestUrl: this.resourceCache?.status?.().manifestUrl || null,
    });
    return { response: { ok: true, netplay: this.netplay, webSettingsApplied, roomOptionsApplied } };
  }

  _cmdResources(p) {
    const items = Array.isArray(p.resources) ? p.resources : [];
    if (!this.resourceCache || !items.length) {
      // 老版本地页没有资源缓存：保留原 hosted 语义，不阻塞启动。
      return { response: { ok: true, mode: 'hosted', loaded: 0, total: items.length } };
    }
    const total = items.length;
    const deferred = Promise.resolve(this.resourceCache.installResources(items)).then((stats) => {
      this._emit('transfer', { phase: 'resources', ...stats, total });
      return { ok: !stats.errors.length, mode: 'cache', ...stats, total };
    });
    // 宿主 installResource() 会对每条命令等回复；必须等 blob 真的落缓存，
    // 否则 launcher revoke objectURL 后资源就再也取不到。
    return { deferred, response: { ok: true, mode: 'cache', loaded: 0, total } };
  }

  _cmdKeyboard(p) {
    const down = p.down === true || p.down === 1;
    const vk = codeToVk(p.code);
    if (vk === null) return { response: { ok: false, reason: 'unknown-code' } };
    if (this.input && typeof this.input.injectKey === 'function') this.input.injectKey(vk, down);
    else if (this.ccall) this.ccall('rgl_key', null, ['number', 'number'], [vk, down ? 1 : 0]);
    return { response: { ok: true } };
  }

  _cmdKeyboardClear() {
    if (this.input && typeof this.input.clearAll === 'function') this.input.clearAll();
    else if (this.ccall) this.ccall('rgl_input_reset', null, [], []);
    return { response: { ok: true } };
  }

  /**
   * 观战阶段 A：根据 netplay 配置装配发布端 / 观战端。
   *  - 发布端（host，player=0，非 spectator）：开一条 player-0 data socket，
   *    每帧由 runtime_entry 调 publishSpectatorFrame() 推 0xe8+packSpecInput。
   *  - 观战端（spectator=true）：开 ?spectator=<id> socket，收帧后由
   *    runtime_entry 调 pumpSpectatorFrame() 驱动确定性对局。
   * 两者互斥；普通单机 / 普通联机（无 spectator 标志）完全不触发。
   */
  setupSpectator() {
    const np = this.netplay || {};
    // 关闭上一局残留
    this.closeSpectator();
    if (!np.url || !np.spectator) return null; // 阶段 A 只给观战端开；发布端由 hostNet 标志触发
    if (!this.WebSocketCtor) return null;
    const cfg = {
      playerCount: np.playerCount || 2,
      localSlot: np.player ?? 0,
      seed: np.seed,
      stage: this._spectatorStage(),
      systems: np.systems || [],
      loadouts: np.loadouts || [],
    };
    const client = new SpectatorClient({
      ccall: this.ccall,
      call: (name, ...args) => this._luaCall(name, ...args),
      jsonCall: (name, ...args) => this._luaJsonCall(name, ...args),
      WebSocketCtor: this.WebSocketCtor,
      config: cfg,
      run: String(np.spectatorId || 'bro-spect'),
      onPlayback: (snap) => { this.spectatorPlayback = snap; this._emitSpectatorPlayback(snap); },
      log: (m) => this._emit('error', { message: 'spectator', error: String(m) }),
    });
    let url;
    try { url = spectatorRelayUrl(np.url, np.spectatorId || 'bro-spectator', cfg.playerCount); }
    catch { return null; }
    this.spectatorClient = client;
    client.connect(url);
    return client;
  }

  /** 发布端：用 player-0 的 data socket 把每帧输入推给 relay。 */
  setupSpectatorPublisher() {
    const np = this.netplay || {};
    // 不在此处 closeSpectator()：runtime_entry 先调 setupSpectator() 再调本方法，
    // 观战端已建好 client，若在这里关会把它误关掉。两者靠 np.spectator 互斥。
    if (!np.url || np.spectator || (np.player ?? 0) !== 0 || !this.WebSocketCtor) return null;
    let url;
    try { url = publisherRelayUrl(np.url); } catch { return null; }
    const up = new SpectatorUploader({
      url, WebSocketCtor: this.WebSocketCtor,
      onState: (s) => this._emit('error', { message: 'spectator-upload', error: String(s) }),
    });
    up.connect();
    this.spectatorUploader = up;
    return up;
  }

  /** 每帧由 runtime_entry 调；观战端消费下一帧并推进 bro_frame。 */
  pumpSpectatorFrame() {
    if (!this.spectatorClient) return null;
    if (!this.spectatorClient.started) this.spectatorClient.start();
    return this.spectatorClient.pump();
  }

  /**
   * 阶段 B：launcher 时间轴命令（pause | play | seek | live）。
   * 契约见 launcher/src/contracts/runtime-protocol.mts：
   *   {command:'spectator-playback', run, action, frame?}
   * run 必须与当前观战对局一致，避免旧面板的命令打到新一局。
   */
  _cmdSpectatorPlayback(p) {
    const client = this.spectatorClient;
    if (!client) return { response: { ok: false, reason: 'no-spectator' } };
    const payload = (p && typeof p === 'object') ? p : {};
    const run = String(payload.run || '');
    if (run && run !== client.run) return { response: { ok: false, reason: 'run-mismatch' } };
    const result = client.control(payload.action, payload.frame);
    return { response: { ...result, run: client.run, playback: client.playback.snapshot() } };
  }

  /** 每帧由 runtime_entry 调；发布端把本机/全玩家输入打包上传。
   *  frameNo 必须是对局内从 0 开始的逻辑帧号（观战端 SpectatorFrameBuffer
   *  从 0 连续取帧），不能用 runtime.frames（页面帧计数，开局已很大）。
   *  这里在 launch 时归零，每次成功上传后自增；调用方传的 frame 仅用于诊断。
   */
  publishSpectatorFrame(frame, epoch, rows) {
    if (!this.spectatorUploader) return false;
    const frameNo = this.spectatorFrame || 0;
    const ok = this.spectatorUploader.sendBytes(packSpecInput(frameNo, epoch, this.netplay?.playerCount || 2, rows));
    if (ok) this.spectatorFrame = frameNo + 1;
    return ok;
  }

  closeSpectator() {
    if (this.spectatorClient) { try { this.spectatorClient.close(); } catch { /* 忽略 */ } this.spectatorClient = null; }
    if (this.spectatorUploader) { try { this.spectatorUploader.close(); } catch { /* 忽略 */ } this.spectatorUploader = null; }
  }

  /** Lua 门面调用（与 multiplayer.mjs 的 call/jsonCall 同构）。 */
  _luaCall(name, ...args) {
    const f = this.netFactory;
    if (f && typeof f.call === 'function') return f.call(name, ...args);
    if (!this.ccall) return '';
    const code = 'return tostring(' + name + '(' + args.map(spectatorLuaValue).join(',') + '))';
    return this.ccall('bro_eval', 'string', ['string'], [code]);
  }

  _luaJsonCall(name, ...args) {
    const f = this.netFactory;
    if (f && typeof f.jsonCall === 'function') return f.jsonCall(name, ...args);
    const s = this._luaCall(name, ...args);
    try { return JSON.parse(s); } catch { return {}; }
  }

  _spectatorStage() {
    const q = this.netplay?.stageQueue;
    if (Array.isArray(q) && q.length) return stageQueueName(q[0]);
    return 'TH06@BossRush';
  }

  /** 上报 runtime-info.spectatorPlayback（launcher 据此驱动实时观战条）。 */
  _emitSpectatorPlayback(snap) {
    this._emit('runtime-info', {
      renderer: 'webgl2', architecture: 'wasm32-lua515',
      version: 'bro-5.1.2+3v', epoch: this.epoch,
      spectatorPlayback: snap,
    });
  }

  _cmdLaunch(p) {
    this.launchSerial = Number.isFinite(p.launchSerial) ? p.launchSerial | 0 : (this.launchSerial + 1);
    this.firstFrameSent = false;
    this.spectatorFrame = 0; // 观战帧号从本局起点 0 重新计
    this.phase = 'prelaunch';
    /* 新一局开始：整表清空按键（noAdopt）。宿主重开时上一局残留的 held
       会跨场景存活（X 按一次后卡死的现象），这里是最干净的释放边界。 */
    if (this.input && typeof this.input.clearAll === 'function') {
      try { this.input.clearAll('launch', true); } catch { /* 忽略 */ }
    }
    // 兜底：若 configure 时 WASM 尚未 boot，这里 setting 表应已就绪，重放网页设置与房间选项。
    this.applyWebSettings();
    this.applyRoomOptions();
    // 阶段 3AZ：联机 match 不再在这里发起——此时 WASM 还没 boot，
    // BRO_net_start_match 无处可落。真正的建链+开局在 runtime_entry 进关之后
    // 调 startNetplayIfConfigured()（Lua 表与 init_player_data 都已就绪）。
    const serial = this.launchSerial;
    if (this.launchHook) {
      // runtime 入口在这里才真正 boot/接主循环；必须等启动完再回 launch 响应。
      const deferred = Promise.resolve()
        .then(() => this.launchHook())
        .then(() => ({ ok: true, launchSerial: serial }));
      return { deferred, response: { ok: true, launchSerial: serial } };
    }
    return { response: { ok: true, launchSerial: serial } };
  }

  /** eagler 的 listing()（files.ts）要求 files 为 {path,size} 对象数组；
   *  fs.list()/storage.inspect() 可能给字符串或带 size/sig 的对象，统一归一化。 */
  _cmdList() {
    const norm = (entries) => (Array.isArray(entries) ? entries : []).map((e) => {
      if (typeof e === 'string') return { path: e, size: 0 };
      const path = e && typeof e.path === 'string' ? e.path : null;
      if (!path) return null;
      const size = e && Number.isFinite(Number(e.size)) ? Math.max(0, Math.round(Number(e.size))) : 0;
      return { path, size };
    }).filter(Boolean);
    if (this.fs && typeof this.fs.list === 'function') {
      const r = this.fs.list();
      if (r && typeof r.then === 'function') {
        return { deferred: r.then((files) => ({ ok: true, files: norm(files) })), response: { ok: true, files: [] } };
      }
      return { response: { ok: true, files: norm(r) } };
    }
    if (this.storage && typeof this.storage.inspect === 'function') {
      return { response: { ok: true, files: norm(this.storage.inspect()) } };
    }
    return { response: { ok: false, reason: 'no-storage' } };
  }

  _cmdRead(p) {
    const path = String(p.path || '');
    try {
      if (this.fs && typeof this.fs.read === 'function') {
        const r = this.fs.read(path);
        if (r && typeof r.then === 'function') {
          return { deferred: r.then((bytes) => ({ ok: true, bytes })), response: { ok: true, bytes: [] } };
        }
        return { response: { ok: true, bytes: r } };
      }
      if (this.ccall) {
        const s = this.ccall('bro_eval', 'string', ['string'],
          [`return tostring(BRO_fs_read(${JSON.stringify(path)}))`]);
        if (typeof s === 'string' && s.startsWith('ERR ')) throw new Error(s);
        const bytes = JSON.parse(s);
        return { response: { ok: true, bytes: Array.isArray(bytes) ? bytes : [] } };
      }
      return { response: { ok: false, reason: 'no-fs' } };
    } catch (e) { return { response: { ok: false, error: String(e && e.message || e) } }; }
  }

  _cmdWrite(p) {
    const path = String(p.path || '');
    const bytes = Array.isArray(p.bytes) ? p.bytes : [];
    try {
      if (this.fs && typeof this.fs.write === 'function') {
        const r = this.fs.write(path, bytes);
        if (r && typeof r.then === 'function') {
          return { deferred: r.then(() => ({ ok: true })), response: { ok: true } };
        }
        return { response: { ok: true } };
      }
      if (this.ccall) {
        const s = this.ccall('bro_eval', 'string', ['string'],
          [`return tostring(BRO_fs_write(${JSON.stringify(path)},${JSON.stringify(bytes)}))`]);
        return { response: { ok: !String(s).startsWith('ERR ') } };
      }
      return { response: { ok: false, reason: 'no-fs' } };
    } catch (e) { return { response: { ok: false, error: String(e && e.message || e) } }; }
  }

  _cmdRemove(p) {
    const path = String(p.path || '');
    try {
      if (this.fs && typeof this.fs.remove === 'function') {
        const r = this.fs.remove(path);
        if (r && typeof r.then === 'function') {
          return { deferred: r.then(() => ({ ok: true })), response: { ok: true } };
        }
        return { response: { ok: true } };
      }
      if (this.ccall) {
        this.ccall('bro_eval', 'string', ['string'],
          [`return tostring(BRO_fs_remove(${JSON.stringify(path)}))`]);
        return { response: { ok: true } };
      }
      return { response: { ok: false, reason: 'no-fs' } };
    } catch (e) { return { response: { ok: false, error: String(e && e.message || e) } }; }
  }

  _cmdSync() {
    if (this.storage && typeof this.storage.flush === 'function') {
      const r = this.storage.flush();
      // 用 deferred 让 _handleMessage 等落盘完成再回包；不再把不可序列化的
      // Promise 塞进 response 字段（launcher 的 listing/bytes 都不认它）。
      if (r && typeof r.then === 'function') return { deferred: r, response: { ok: true } };
    }
    return { response: { ok: true } };
  }

  /** 调用 Lua 侧回放桥，返回已解析的对象（Lua Serialize JSON）。 */
  _replayCall(fn, args = []) {
    if (!this.ccall || typeof this.ccall !== 'function') {
      throw new Error('replay bridge unavailable before boot');
    }
    const expr = `return tostring(${fn}(${args.map((a) => JSON.stringify(a)).join(',')}))`;
    const raw = this.ccall('bro_eval', 'string', ['string'], [expr]);
    if (typeof raw !== 'string') return null;
    try { return JSON.parse(raw); } catch { return raw; }
  }

  /** 上报 runtime-info.playback；Eagler ReplayWorkspace 据此驱动控制条。 */
  _emitPlaybackInfo(extra = {}) {
    let playback = {};
    try { playback = this._replayCall('BRO_replay_status') || {}; } catch { /* 未 boot 时留空 */ }
    if (!playback || typeof playback !== 'object' || Array.isArray(playback)) playback = {};
    // 错误对象（Lua Serialize({error=...})）不上报成播放状态。
    if (typeof playback.error === 'string') playback = {};
    this._emit('runtime-info', {
      renderer: 'webgl2', architecture: 'wasm32-lua515',
      version: 'bro-5.1.2+3v', epoch: this.epoch,
      playback: { ...playback, ...extra },
    });
  }

  /** eagler playback 条件命令：inspect / open / pause / play / seek / rate / relocate。
   *  inspect 需要 Lua 已 boot（ReadReplayInfo 真身）；file session 发生在 launch
   *  前，Module 此时可能尚未建。用 fs 后端触发 createEnvironment 兜底。 */
  _cmdPlayback(p) {
    const action = String(p.action || '');
    const path = String(p.path || '');
    const boot = async () => {
      if (!this.ccall && this.fs && typeof this.fs.environment === 'function') {
        // saveFS.environment() 返回 {Module,ccall,ev,storage}；launch 前已就绪但
        // adapter.ccall 要等 startBRO 才挂上，这里主动取一次。
        try {
          const env = await this.fs.environment();
          if (env && env.ccall) {
            this.ccall = env.ccall;
            this.Module = env.Module;
            if (!this.storage && env.storage) this.storage = env.storage;
          }
        } catch { /* 交由下面兜底 */ }
      }
      return !!this.ccall;
    };
    const run = async () => {
      const ready = await boot();
      if (!ready) return { ok: false, error: 'replay bridge unavailable' };
      if (action === 'inspect') {
        const desc = this._replayCall('BRO_replay_inspect', [path]);
        if (!desc || typeof desc !== 'object' || Array.isArray(desc) || typeof desc.error === 'string') {
          return { ok: false, error: (desc && desc.error) || 'invalid replay' };
        }
        return { ok: true, description: desc };
      }
      if (action === 'open') {
        const stageName = p.stage === undefined ? null : String(p.stage);
        const result = this._replayCall('BRO_replay_open', [path, stageName]);
        // Lua 回 JSON：{ok:false,error} / {ok:true,path,stage}。
        if (result && typeof result === 'object' && result.ok === false) {
          return { ok: false, error: result.error || 'replay open failed' };
        }
        if (result === false) return { ok: false, error: 'replay open failed' };
        this.playbackPath = path;
        this._emitPlaybackInfo({ path, stage: stageName || '', playing: true });
        return { ok: true };
      }
      if (action === 'relocate') {
        // 宿主在文件会话里已 write 目标路径，这里只切换当前播放引用。
        const target = String(p.target || p.path || '');
        this.playbackPath = target;
        this._emitPlaybackInfo({ path: target });
        return { ok: true };
      }
      if (action === 'pause' || action === 'play' || action === 'seek' || action === 'rate') {
        const value = Number.isFinite(Number(p.value)) ? Number(p.value) : 0;
        const result = this._replayCall('BRO_replay_control', [action, value]);
        const failed = result === false || (result && typeof result === 'object' && (result.ok === false || result.error));
        if (failed) {
          // seek 在 STGR 上明确不支持：ok:false 让 UI 显示 unsupported，不静默成功。
          return { ok: false, reason: 'unsupported', action };
        }
        this._emitPlaybackInfo({ playing: action !== 'pause' });
        return { ok: true, action };
      }
      return { ok: false, reason: 'unknown-action', action };
    };
    const deferred = Promise.resolve().then(run);
    return { deferred, response: { ok: true } };
  }

  // ---- peerTransportGlobal 桥 ---------------------------------------------

  _installTransportGlobal() {
    if (typeof globalThis === 'undefined') return;
    if (globalThis[this.transportName]) {
      // 宿主（eagler launcher）已注入同名对象：优先用宿主传输。
      this.transport = globalThis[this.transportName];
      return;
    }
    if (this.transport) { globalThis[this.transportName] = this.transport; return; }
    // 默认桥：把 BRO 门面的输入收发暴露成最小 transport 面（方法名接入时核对）。
    const self = this;
    const bridge = {
      get ready() { return !!(self.multi && self.multi.net); },
      get player() { return self.netplay ? self.netplay.player : (self.multi ? self.multi.player : 0); },
      get playerCount() { return self.netplay ? self.netplay.playerCount : (self.multi ? self.multi.maxPlayers : 2); },
      send(frame, keys) {
        if (self.multi && typeof self.multi.sendInput === 'function') {
          self.multi.sendInput(frame, keys);
          return true;
        }
        return false;
      },
      onInput(fn) {
        if (self.multi && typeof self.multi.on === 'function') self.multi.on('input', fn);
        return () => {
          if (self.multi && typeof self.multi.off === 'function') self.multi.off('input', fn);
        };
      },
      close() { if (self.multi && typeof self.multi.disconnect === 'function') self.multi.disconnect(); },
      /** 上报测量：宿主读它生成 netplay timing 对象。 */
      measuredTiming() {
        const rep = self.multi && typeof self.multi.getRouteSurvey === 'function'
          ? self.multi.getRouteSurvey() : null;
        return rep ? toNetplayTiming(rep.lastReport || rep, { spectator: self.netplay?.spectator }) : null;
      },
    };
    this.transport = bridge;
    try { globalThis[this.transportName] = bridge; } catch { /* 只读全局环境 */ }
  }
}

/**
 * 便捷入口：创建并 attach。
 * @param {AdapterOptions} opts
 */
export function createEaglerRuntimeAdapter(opts = {}) {
  return new EaglerRuntimeAdapter(opts).attach();
}
