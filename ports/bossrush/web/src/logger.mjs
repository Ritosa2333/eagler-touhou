// 1d-28：统一运行日志。
//
// 之前有三套互不相通的日志：shell 的 `log()` 只留最后 40 行到 DOM、
// input 的 `logPush` 走 POST /__rgl_log（CF 上没这个端点，全堆 localStorage）、
// 看门狗/升级弹窗/资源失败则完全没有留痕。真机报「卡住 / 自机停顿」时
// 拿不到任何可复现的证据，只能猜。这里把它们收成一个环形缓冲。
//
// 硬约束（1d-27 的教训）：**带外诊断代码不许有掐死主循环的能力**。
// 本文件每一个对外方法都自带 try/catch，绝不向调用方抛异常；也不在
// 每帧热路径上写日志（只有里程碑、异常、限速心跳才进来）。
//
// 发布态同样加载（shell.mjs 直接 import，不经过 console.mjs 的可裁入口），
// 这样 CF 玩家端点一下「导出日志」就能把现场交出来。

const RING = 4000;                 // 环形缓冲：约 4000 条 / 通常够覆盖十几分钟
const LEVEL_RANK = { debug: 0, info: 1, event: 2, warn: 3, error: 4 };

function safeValue(v, depth = 0) {
  try {
    if (v === undefined || v === null) return v;
    if (typeof v === 'number' || typeof v === 'boolean' || typeof v === 'string') return v;
    if (typeof v === 'function') return '[function]';
    if (v instanceof Error) return { name: v.name, message: v.message, stack: String(v.stack || '').slice(0, 1200) };
    if (depth > 3) return '[deep]';
    // 走一遍 JSON 序列化把循环引用 / DOM 节点 / 大对象挡在外面。
    const s = JSON.stringify(v, (_k, x) => {
      if (typeof x === 'bigint') return String(x);
      if (typeof x === 'function') return undefined;
      if (x && typeof x === 'object' && typeof x.nodeType === 'number') return '[dom]';
      return x;
    });
    if (s === undefined) return String(v);
    return JSON.parse(s);
  } catch { return String(v).slice(0, 400); }
}

function captureEnv(release, buildId) {
  const e = {
    born: new Date().toISOString(),
    href: location.href,
    ua: navigator.userAgent,
    lang: navigator.language,
    dpr: window.devicePixelRatio || 1,
    viewport: `${window.innerWidth}x${window.innerHeight}`,
    screen: `${screen.width}x${screen.height}`,
    cores: navigator.hardwareConcurrency || 0,
    deviceMemory: navigator.deviceMemory || 0,
    release: !!release,
    build: buildId || '(unknown)',
    wasm: typeof WebAssembly !== 'undefined',
    audio: typeof (window.AudioContext || window.webkitAudioContext) !== 'undefined',
    idb: typeof indexedDB !== 'undefined',
  };
  try { e.connection = navigator.connection?.effectiveType || ''; } catch { /* ignore */ }
  return e;
}

/* 1d-28：GPU 探测**只在导出日志时**做。启动期 new 一个 WebGL2 上下文看着
   无害，但会跟 renderer 抢一个 context 名额（浏览器上限通常 8~16 个），
   SwiftShader / 老显卡上还可能拖慢首屏。它也不是每帧信息，没必要在
   启动链上付这个代价。只跑一次，结果缓存。 */
let gpuCache;
function gpuInfo() {
  if (gpuCache !== undefined) return gpuCache;
  gpuCache = { webgl2: false };
  try {
    const c = document.createElement('canvas');
    const gl = c.getContext('webgl2');
    gpuCache.webgl2 = !!gl;
    if (gl) {
      const dbg = gl.getExtension('WEBGL_debug_renderer_info');
      gpuCache.gpu = dbg ? gl.getParameter(dbg.UNMASKED_RENDERER_WEBGL) : gl.getParameter(gl.RENDERER);
      try { gl.getExtension('WEBGL_lose_context')?.loseContext(); } catch { /* ignore */ }
    }
  } catch { /* 拿不到就只留 webgl2:false */ }
  return gpuCache;
}

/* 导出时统一读一遍 Lua 侧状态。每一项都 pcall，chaos 分支没装的全局
   不会让整份快照失败。返回单行字符串，便于直接贴进日志文本。 */
const LUA_SNAPSHOT = `
  local function q(f) local ok, r = pcall(f) return ok and tostring(r) or ('err:' .. tostring(r)) end
  local v = (type(lstg) == 'table' and lstg.var) or {}
  return table.concat({
    -- 1d-58：__rgl_frozen 是函数、返回 boolean；写成 f() or 'n/a' 会把
    -- false 变成 'n/a'（曾经日志里 frozen 永远显示 n/a）。__rgl_speed_now
    -- 则是 note_speed() 写的**数值**，判 type=='function' 恒假。两个都显式
    -- tostring/取字段。
    'frozen=' .. q(function()
      if type(__rgl_frozen) ~= 'function' then return 'n/a' end
      return tostring(__rgl_frozen()) end),
    'speed=' .. q(function()
      local m = rawget(_G, '__rgl_speed_now')
      return (type(m) == 'number') and tostring(m) or 'n/a' end),
    'cyfps=' .. q(function() return v.CYFPS end),
    'level=' .. q(function() return v.level end),
    'chaos=' .. q(function() return v.chaos end),
    -- 1d-58：难度 + 难度档位。玩家报「Ultra 初始 chaos 为 0」时，只看 chaos
    -- 分不清是「配置没生效」还是「diff 根本没到 2」：var.difficulty 由
    -- DiffSelection[scene_id] - 1 得来（player.lua OtherInit），导入原版存档
    -- 后这张表被原版数据覆盖，缺键时 init.lua 会补 1 → Tutorial →
    -- init_fuse(0)==0。三个值一起记，一眼能分清账本问题还是难度档位问题。
    'diff=' .. q(function() return v.difficulty end),
    'diffSel=' .. q(function()
      local sd = rawget(_G, 'stagedata')
      if type(sd) ~= 'table' then return 'n/a' end
      local ds = sd.DiffSelection
      if type(ds) ~= 'table' then return 'n/a' end
      return tostring(ds[v.scene_id]) end),
    'fuseInit=' .. q(function()
      if type(__RGL) ~= 'table' or type(__RGL.entropy) ~= 'table' then return 'n/a' end
      local d = __RGL.entropy.detail
      return d and d.fuse_init or 'n/a' end),
    'entropy=' .. q(function()
      if type(__rgl_entropy_state) ~= 'function' then return 'n/a' end
      local t, f, r = __rgl_entropy_state()
      return tostring(t) .. '/' .. tostring(f) .. '/' .. tostring(r) end),
    'lvlupPending=' .. q(function()
      return type(__rgl_lvlup_pending) == 'function' and __rgl_lvlup_pending() or 'n/a' end),
    'scene=' .. q(function()
      local cs = type(stage) == 'table' and stage.current_stage or nil
      return cs and (cs.stage_name or '?') or '?' end),
    'movement=' .. q(function()
      return type(__rgl_movement_state) == 'function' and __rgl_movement_state() or 'n/a' end),
    -- 1d-57：弹雾现场。玩家报「wave 开头弹雾又被放大」时，看这三个数字就知道
    -- 是「真的一直在长」（tmax 远大于 fogtime）还是只是雾化瞬间那 4 倍贴图。
    'fog=' .. q(function()
      return type(__rgl_fog_state) == 'function' and __rgl_fog_state() or 'n/a' end),
  }, '|')
`;

export function createLogger(opts = {}) {
  const {
    qs = new URLSearchParams(location.search),
    release = false,
    buildId = '',
    getRGL = () => globalThis.rgl,
    getExtra = null,
    echoConsole = false,     // shell.log() 自己会打 console，默认别重复刷屏
  } = opts;
  const t0 = performance.now();
  const env = captureEnv(release, buildId);
  const ring = new Array(RING);
  const counters = { debug: 0, info: 0, event: 0, warn: 0, error: 0 };
  let seq = 0;
  /* 实际留存条数。不能拿 min(seq, RING) 当 size：clear() 之后 seq 还在涨，
     但环形缓冲里只剩一条 —— 导出的「保留 N 条」会骗人。 */
  let retained = 0;
  let dropped = 0;
  const enabled = !qs.has('nolog');
  const verbose = qs.has('log') || !release;
  const listeners = [];

  function push(level, tag, msg, data) {
    if (!enabled) return null;
    try {
      const s = ++seq;
      const now = performance.now();
      const rec = {
        i: s,
        t: +(now - t0).toFixed(1),       // 单调时钟：卡住时也能看出时间线断在哪
        ts: new Date().toISOString(),
        lvl: level,
        tag: String(tag),
        msg: String(msg),
      };
      if (data !== undefined) rec.data = safeValue(data);
      const slot = (s - 1) % RING;
      if (ring[slot] === undefined) retained++;   // 覆盖旧记录时总数不变
      ring[slot] = rec;
      counters[level] = (counters[level] || 0) + 1;
      // 控制台：错误/警告永远打；其它只在开发态且显式要求时才回显。
      const prefix = `[rgl/${rec.tag}]`;
      if (level === 'error') console.error(prefix, rec.msg, data ?? '');
      else if (level === 'warn') console.warn(prefix, rec.msg, data ?? '');
      else if (echoConsole && verbose) console.log(prefix, rec.msg, data ?? '');
      return rec;
    } catch { dropped++; return null; }
  }

  const api = {
    get size() { return retained; },
    get total() { return seq; },
    get counters() { return { ...counters }; },
    get env() { return env; },
    get dropped() { return dropped; },
    debug: (tag, msg, data) => push('debug', tag, msg, data),
    info:  (tag, msg, data) => push('info', tag, msg, data),
    event: (tag, msg, data) => push('event', tag, msg, data),
    warn:  (tag, msg, data) => push('warn', tag, msg, data),
    error: (tag, msg, data) => push('error', tag, msg, data),
    /** 给外部模块（input.mjs 等）转发它自己的事件流，避免重复造轮子。 */
    ingest(tag, obj) {
      if (!enabled) return null;
      const k = obj && obj.k;
      if (!k) return null;
      // 只收排查价值高的事件；每帧心跳（hb）不进统一缓冲，input 自己有 JSONL。
      if (k === 'hb' || k === 'rep' || k === 'd' || k === 'u') return null;
      const lvl = (k === 'clr' || k === 'ghost' || k === 'stall' || k === 'altMeta')
        ? 'warn' : 'event';
      return push(lvl, tag || 'input', k, obj);
    },
    /** 导出前的现场快照。所有 getter 都单独 try，任何一个坏了不影响其余。 */
    snapshot() {
      const out = {
        at: new Date().toISOString(),
        uptimeMs: +(performance.now() - t0).toFixed(0),
        total: seq,
        retained,
        counters: { ...counters },
        dropped,
        env,
      };
      let rgl = null;
      try { rgl = getRGL(); } catch { /* ignore */ }
      const take = (name, fn) => { try { const v = fn(); if (v !== undefined) out[name] = safeValue(v); } catch (e) { out[name] = 'err: ' + String(e); } };
      if (rgl) {
        take('watchdog', () => rgl.watchdog?.());
        take('perf', () => rgl.perf);
        take('schedule', () => rgl.schedule);
        take('frame', () => rgl.frame);
        take('renders', () => rgl.renders);
        take('earlyFrame', () => rgl.earlyFrame?.());
        take('inputDiag', () => rgl.input?.diag);
        take('rendererStats', () => rgl.renderer?.stats?.());
        take('assets', () => rgl.assets?.stats?.());
        take('scripts', () => rgl.scripts);
        take('lua', () => rgl.ev?.(LUA_SNAPSHOT));
      }
      if (getExtra) take('extra', getExtra);
      take('page', () => ({
        state: document.body?.dataset?.rglState,
        frameError: document.body?.dataset?.rglFrameError,
        visibility: document.visibilityState,
        focus: typeof document.hasFocus === 'function' ? document.hasFocus() : '?',
      }));
      take('memory', () => performance.memory ? {
        usedMB: +(performance.memory.usedJSHeapSize / 1048576).toFixed(1),
        limitMB: +(performance.memory.jsHeapSizeLimit / 1048576).toFixed(1),
      } : undefined);
      take('gpu', gpuInfo);
      return out;
    },
    /** 按序号排序后的全部留存记录。 */
    records() {
      const out = [];
      try {
        const n = retained;
        const start = seq - n;
        for (let j = 0; j < n; j++) {
          const r = ring[(start + j) % RING];
          if (r) out.push(r);
        }
      } catch { /* ignore */ }
      return out;
    },
    exportJSON() {
      try { return JSON.stringify({ format: 'rgl-log/1', snapshot: api.snapshot(), records: api.records() }, null, 2); }
      catch (e) { return JSON.stringify({ format: 'rgl-log/1', error: String(e) }); }
    },
    exportText(note) {
      const L = [];
      try {
        L.push('===== Touhou Roguelike 运行日志 =====');
        L.push('导出时间: ' + new Date().toISOString());
        if (note && String(note).trim()) {
          L.push('玩家描述: ' + String(note).trim());
        }
        L.push('说明: t 为页面启动后的毫秒数（单调时钟）。把本文件连同「问题发生时在做什么」一起发给开发者。');
        L.push('');
        L.push('----- 现场快照 -----');
        L.push(JSON.stringify(api.snapshot(), null, 2));
        L.push('');
        L.push(`----- 事件流（共 ${seq} 条，保留 ${retained} 条）-----`);
        for (const r of api.records()) {
          const pad = String(r.t).padStart(10);
          const d = r.data === undefined ? '' : '  ' + (typeof r.data === 'string' ? r.data : JSON.stringify(r.data));
          L.push(`${pad}ms  ${r.lvl.toUpperCase().padEnd(5)} ${r.tag.padEnd(9)} ${r.msg}${d}`);
        }
      } catch (e) { L.push('[导出失败] ' + String(e)); }
      return L.join('\n') + '\n';
    },
    /** 触发浏览器下载。返回文件名；失败返回 null（绝不抛）。 */
    download(asJSON = false, note) {
      try {
        const text = asJSON ? api.exportJSON() : api.exportText(note);
        const stamp = new Date().toISOString().replace(/[:.]/g, '-');
        const name = `rgl-log-${stamp}.${asJSON ? 'json' : 'txt'}`;
        const blob = new Blob([text], { type: asJSON ? 'application/json' : 'text/plain;charset=utf-8' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url; a.download = name;
        document.body.appendChild(a); a.click(); a.remove();
        setTimeout(() => { try { URL.revokeObjectURL(url); } catch { /* ignore */ } }, 5000);
        api.event('log', '导出运行日志 ' + name, { bytes: text.length });
        return name;
      } catch { return null; }
    },
    /** 复制到剪贴板；不支持时降级到隐藏 textarea + execCommand。 */
    async copy(note) {
      const text = api.exportText(note);
      try {
        if (navigator.clipboard?.writeText) { await navigator.clipboard.writeText(text); return true; }
      } catch { /* 权限拒绝，走降级 */ }
      try {
        const ta = document.createElement('textarea');
        ta.value = text; ta.style.position = 'fixed'; ta.style.opacity = '0';
        document.body.appendChild(ta); ta.select();
        const ok = document.execCommand('copy'); ta.remove();
        return !!ok;
      } catch { return false; }
    },
    /** 清掉缓冲（保留 env 与计数），用于「问题刚发生，重新开始记」。 */
    clear() {
      try {
        for (let i = 0; i < RING; i++) ring[i] = undefined;
        retained = 0;
        api.event('log', '日志缓冲已清空');
      }
      catch { /* ignore */ }
    },
    detach() {
      for (const [t, ty, fn, o] of listeners) {
        try { t.removeEventListener(ty, fn, o); } catch { /* ignore */ }
      }
      listeners.length = 0;
    },
  };

  /* 全局错误与页面状态：发布态也要留痕。 */
  const on = (target, type, fn, o) => {
    try { target.addEventListener(type, fn, o); listeners.push([target, type, fn, o]); } catch { /* ignore */ }
  };
  on(window, 'error', (e) => {
    try {
      api.error('uncaught', String(e.message || 'error'), {
        src: e.filename, line: e.lineno, col: e.colno,
        stack: String(e.error?.stack || '').slice(0, 1200),
      });
    } catch { /* ignore */ }
  });
  on(window, 'unhandledrejection', (e) => {
    try { api.error('promise', String(e.reason?.message || e.reason), { stack: String(e.reason?.stack || '').slice(0, 800) }); }
    catch { /* ignore */ }
  });
  on(document, 'visibilitychange', () => {
    try { api.event('page', 'visibility=' + document.visibilityState); } catch { /* ignore */ }
  });
  on(window, 'blur', () => { try { api.event('page', 'blur'); } catch { /* ignore */ } });
  on(window, 'focus', () => { try { api.event('page', 'focus'); } catch { /* ignore */ } });
  on(window, 'pagehide', () => { try { api.event('page', 'pagehide'); } catch { /* ignore */ } });
  on(document, 'securitypolicyviolation', (e) => {
    try { api.warn('csp', String(e.violatedDirective || 'csp'), { blocked: e.blockedURI }); } catch { /* ignore */ }
  });
  api.info('boot', 'logger ready', { ring: RING, release: !!release });
  return api;
}

/** 让主 canvas 的 WebGL context lost 也进日志（由 shell 调）。 */
export function attachContextLoss(logger, canvas) {
  if (!logger || !canvas) return;
  try {
    canvas.addEventListener('webglcontextlost', (e) => {
      logger.error('webgl', 'context lost', { status: e.statusMessage });
    }, false);
    canvas.addEventListener('webglcontextrestored', () => {
      logger.warn('webgl', 'context restored');
    }, false);
  } catch { /* ignore */ }
}
