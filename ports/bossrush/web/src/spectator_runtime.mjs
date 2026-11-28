/**
 * BRO 观战阶段 A（实时观战）—— 发布端 / 观战端运行时
 *
 * 背景（见 eagler-touhou: ports/bossrush/proposals/SPECTATOR.md、b47~b52 取证）：
 *  - BRO 对局走 WebRTC DataChannel，relay 看不到输入；要让 relay 上的观众看到
 *    实时战况，必须另开一条「上传通道」，由玩家 0 把每帧输入推给 relay。
 *  - relay 的 0xe8 车道只认 player-0 的 data socket（`?player=0&players=n`，不带
 *    signal=1）；同槽第二条 player socket 会被 close(1008)。signal socket 走的是
 *    run.signalClients，与 data 的 run.clients 互不冲突，所以「复用 netplayUrl、
 *    另开一条不带 signal=1 的 WS」即可。
 *  - 观战端用 `?spectator=<clientId>&players=n` 连同一 relay；relay 先回放历史
 *    再转 live，观众只读（发消息会被 close(1008)）。
 *  - 观战端起同一场确定性对局（同 seed/systems/stage/loadouts），把每帧输入喂给
 *    BRO_net_apply_keys_at + BRO_net_feed_player_key，自己调 bro_frame 推进，
 *    不接本地键盘、不发包。
 *
 * 本模块刻意保持「纯 JS + 依赖注入」：不直接 import WebSocket / bro.mjs，
 * 由调用方（adapter / runtime_entry）注入 ccall、multi、配置与 WebSocket 工厂，
 * 因此可在 Node 单测里用假 ws / 假 ccall 完整验证。
 */

import {
  packSpecInput, packSpecBegin, unpackSpec, SpectatorFrameBuffer,
  SPEC_FLAG_PRESENT, SPEC_FLAG_DEAD, SPEC_FLAG_LOCAL,
} from './spectator_wire.mjs';

const SPECTATOR_ID_RE = /^[A-Za-z0-9_-]{8,64}$/;

/** relay 0xe8 上传车道前缀。 */
const SPEC_LANE = 0xe8;
/** 停止上传包（relay 认这个精确字节序列）。 */
const SPEC_STOP = Uint8Array.from([SPEC_LANE, 0x53, 0x54, 0x4f, 0x50, 1]); // e8 'S','T','O','P',1

function clampInt(v, lo, hi, fallback) {
  const n = Number(v);
  return Number.isFinite(n) ? Math.min(hi, Math.max(lo, Math.trunc(n))) : fallback;
}

/**
 * 把 netplayUrl（带 player=N）改成「观战端 socket URL」：
 * 删 player / signal，set spectator=clientId、players=playerCount。
 * 与 BrowserPeerTransport.cpp:eagler_spectator_connect 完全一致。
 */
export function spectatorRelayUrl(netplayUrl, clientId, playerCount) {
  const url = new URL(String(netplayUrl));
  url.searchParams.delete('player');
  url.searchParams.delete('signal');
  if (!SPECTATOR_ID_RE.test(String(clientId))) throw new Error('invalid spectator clientId');
  url.searchParams.set('spectator', String(clientId));
  url.searchParams.set('players', String(clampInt(playerCount, 1, 4, 2)));
  return url.href;
}

/**
 * 把 netplayUrl 变成「发布端 data socket URL」：删 signal，保留 player=N。
 * BRO 平时只用 signal socket（netplayUrl + ?signal=1），data socket 这条是新增的。
 */
export function publisherRelayUrl(netplayUrl) {
  const url = new URL(String(netplayUrl));
  url.searchParams.delete('signal');
  return url.href;
}

/**
 * 覆盖 0xe8 上传通道。只负责 socket 与背压；帧内容由 publishFrame 拼。
 */
export class SpectatorUploader {
  constructor(opts = {}) {
    this.url = opts.url || '';
    this.WebSocketCtor = opts.WebSocketCtor || (typeof WebSocket !== 'undefined' ? WebSocket : null);
    this.maxBuffered = clampInt(opts.maxBuffered, 1024, 16 * 1024 * 1024, 65536);
    this.ws = null;
    this.state = 'idle'; // idle | connecting | open | closed | error
    this.sent = 0;
    this.dropped = 0;
    this.onState = typeof opts.onState === 'function' ? opts.onState : null;
    this._log = typeof opts.log === 'function' ? opts.log : null;
  }

  _setState(s) {
    this.state = s;
    if (this.onState) { try { this.onState(s); } catch { /* 订阅者异常不影响 */ } }
  }

  connect() {
    if (!this.url || !this.WebSocketCtor) { this._setState('error'); return false; }
    if (this.ws && (this.state === 'open' || this.state === 'connecting')) return true;
    let ws;
    try {
      // 浏览器 WebSocket 第二参可以是子协议；传 undefined 表示默认。
      ws = new this.WebSocketCtor(this.url);
    } catch {
      this._setState('error');
      return false;
    }
    this.ws = ws;
    this._setState('connecting');
    if (ws.binaryType !== undefined) ws.binaryType = 'arraybuffer';
    ws.onopen = () => this._setState('open');
    ws.onclose = () => { if (this.state !== 'error') this._setState('closed'); this.ws = null; };
    ws.onerror = () => this._setState('error');
    return true;
  }

  /** 发一个 0xe8 封装的观战帧；返回是否成功入队。 */
  sendBytes(payload) {
    if (!this.ws || this.state !== 'open') { this.dropped++; return false; }
    // relay 与浏览器都用 bufferedAmount 做背压；堆积超过阈值就丢这一帧，
    // 观战是尽力而为，不能让上传拖慢主机对局。
    if (Number(this.ws.bufferedAmount || 0) > this.maxBuffered) { this.dropped++; return false; }
    const envelope = new Uint8Array(payload.length + 1);
    envelope[0] = SPEC_LANE;
    envelope.set(payload, 1);
    try {
      this.ws.send(envelope);
      this.sent++;
      return true;
    } catch {
      this.dropped++;
      return false;
    }
  }

  close() {
    if (!this.ws) return;
    try { if (this.state === 'open') this.ws.send(SPEC_STOP); } catch { /* 忽略 */ }
    try { this.ws.close(); } catch { /* 忽略 */ }
    this.ws = null;
    this._setState('closed');
  }
}

/**
 * 观战播放状态：生成 launcher 要求的合法 `spectatorPlayback`。
 * 约束（launcher/src/launcher/spectator-playback.mts parseSpectatorPlayback）：
 *   version=1、run 合法、revision/min/position/received/edge/target 均 uint、
 *   min<=position<=received、min<=edge<=received、min<=target<=received。
 *
 * 阶段 B 语义（SPECTATOR.md）：
 *   min      最早还能定位到的帧（环形缓冲丢帧后前移）
 *   position 已经重演到第几帧（=已应用帧数）
 *   edge     收到的最新连续帧（live 前沿）
 *   target   本次定位目标帧；不在定位时等于 position
 *   paused   用户按了暂停
 *   seeking  正在向 target 重演（可能同时 paused：暂停态下的背景定位）
 *   following position 追上 edge、且没暂停没定位
 */
export class SpectatorPlayback {
  constructor(opts = {}) {
    this.run = String(opts.run || 'bro-spectator');
    if (!/^[A-Za-z0-9_-]{1,64}$/.test(this.run)) this.run = 'bro-spectator';
    this.revision = 0;
    this.min = 0;
    this.position = 0;
    this.received = 0;
    this.edge = 0;
    this.target = 0;
    this.paused = false;
    this.seeking = false;
    this.following = opts.following !== false; // 阶段 A 只有「实时跟随」
    this.ended = false;
    this.nodes = Array.isArray(opts.nodes) ? opts.nodes.slice(0, 512) : [];
  }

  /** 收到一帧（已解包）后更新计数；跟随态下 target 跟着 edge 走。 */
  onFrame(frame) {
    const f = clampInt(frame && frame.frame, 0, 0xffffffff, 0);
    this.received = Math.max(this.received, f + 1);
    this.edge = Math.max(this.edge, this.received);
    if (!this.seeking && !this.paused) this.target = this.edge;
    this._clamp();
    return this.snapshot();
  }

  /** 每消费一帧推进播放位置。 */
  onConsume(frame) {
    const f = clampInt(frame, 0, 0xffffffff, 0);
    this.position = Math.max(this.position, f + 1);
    if (!this.seeking) {
      this.target = this.position;
      if (!this.paused && this.position >= this.edge) this.following = true;
    }
    this._clamp();
    this.revision++;
    return this.snapshot();
  }

  /** 环形缓冲丢帧后，定位下界跟着前移（min<=position 恒成立）。 */
  setMin(v) {
    this.min = clampInt(v, 0, this.position, this.min);
    this._clamp();
    this.revision++;
    return this.snapshot();
  }

  markPaused(v) {
    this.paused = !!v;
    if (this.paused) this.following = false;
    this.revision++;
    return this.snapshot();
  }

  markFollowing(v) { this.following = !!v; this.revision++; return this.snapshot(); }

  /** 直接改完字段后统一发一次新快照。 */
  bump() { this.revision++; this._clamp(); return this.snapshot(); }

  markEnded() { this.ended = true; this.revision++; return this.snapshot(); }

  _clamp() {
    this.received = clampInt(this.received, 0, 0xffffffff, 0);
    this.position = clampInt(this.position, this.min, this.received, this.min);
    this.edge = clampInt(this.edge, this.min, this.received, this.min);
    this.target = clampInt(this.target, this.min, this.received, this.min);
  }

  snapshot() {
    this._clamp();
    return {
      version: 1,
      run: this.run,
      revision: this.revision >>> 0,
      min: this.min >>> 0,
      position: this.position >>> 0,
      received: this.received >>> 0,
      edge: this.edge >>> 0,
      target: this.target >>> 0,
      paused: !!this.paused,
      seeking: !!this.seeking,
      following: !!this.following,
      ended: !!this.ended,
      nodes: this.nodes.slice(0, 512),
    };
  }
}

/**
 * 观战端：收帧 → 按帧缓冲 → 逐帧驱动确定性对局。
 *
 * 依赖注入：
 *   ccall(name,...args)   —— WASM 调用（bro_eval / bro_frame）
 *   call(name,...args)    —— Lua 全局调用（内部用 bro_eval(return tostring(...))）
 *   jsonCall(name,...)    —— Lua 全局返回 JSON
 *   config                —— {playerCount, seed, stage, loadouts[], systems[], localSlot}
 *   WebSocketCtor, log
 *   onPlayback(snapshot)  —— 每次状态变化回调（adapter 据此 _emit runtime-info）
 */
export class SpectatorClient {
  constructor(opts = {}) {
    this.ccall = opts.ccall;
    this.call = opts.call;
    this.jsonCall = opts.jsonCall;
    this.WebSocketCtor = opts.WebSocketCtor || (typeof WebSocket !== 'undefined' ? WebSocket : null);
    this.config = opts.config || {};
    this.playerCount = clampInt(this.config.playerCount, 1, 4, 2);
    this.localSlot = clampInt(this.config.localSlot, 0, this.playerCount - 1, 0);
    this.run = String(opts.run || 'bro-spectator');
    this.buffer = new SpectatorFrameBuffer();
    this.playback = new SpectatorPlayback({ run: this.run });
    this.onPlayback = typeof opts.onPlayback === 'function' ? opts.onPlayback : null;
    this.log = typeof opts.log === 'function' ? opts.log : null;
    this.ws = null;
    this.state = 'idle';
    this.started = false;
    this.ready = false;      // init_player_data 完成、远端实体已 spawn
    this.pumped = 0;
    this.errors = [];
    this.lastFrame = -1;
    this._begin = null;
    // ---- 阶段 B：定位用的历史帧环形缓冲 ----
    // 只存每帧每玩家的 keys/flags（x/y 仅供展示，定位不需要），108000 帧 × 60B 量级，
    // 对齐 relay 的 16MB 观战历史上限（SPECTATOR.md）。
    this.history = new Map();          // frame -> {keys:Uint8Array, flags:Uint8Array}
    this.historyOrder = [];            // 插入顺序，用来做环形淘汰
    this.historyTrim = 0;              // historyOrder 里已被淘汰到的下标
    this.historyFrames = clampInt(opts.historyFrames ?? this.config.historyFrames, 256, 108000, 108000);
    this.maxFrame = -1;                // 收到过的最大帧号
    this.historyMin = 0;               // 环形缓冲里还留得住的最早帧
    this.seekBudgetMs = clampInt(opts.seekBudgetMs, 1, 50, 5);   // 每次 pump 的重演时间预算
    this.seekStepCap = clampInt(opts.seekStepCap, 1, 600, 30);   // 每次 pump 最多重演帧数
    this._lastEmitAt = -1e9;           // 第一次变化必须立刻上报
    this._replayed = 0;                // 本次 seek 已重演的帧数（诊断）
  }

  _now() {
    return (typeof performance !== 'undefined' && typeof performance.now === 'function')
      ? performance.now() : Date.now();
  }

  /**
   * 上报播放状态。观战端一帧可能推进多次（定位时一次 pump 重演几十帧），
   * 所以默认按 120ms 节流，命令回执/结束等关键变化用 _emit(true) 强制发。
   */
  _emit(force = false) {
    const now = this._now();
    if (!force && now - this._lastEmitAt < 120) return;
    this._lastEmitAt = now;
    if (this.onPlayback) { try { this.onPlayback(this.playback.snapshot()); } catch { /* 忽略 */ } }
  }

  connect(url) {
    if (!url || !this.WebSocketCtor) { this.state = 'error'; return false; }
    let ws;
    try { ws = new this.WebSocketCtor(url); }
    catch (e) { this.state = 'error'; this.errors.push(String(e)); return false; }
    this.ws = ws;
    this.state = 'connecting';
    if (ws.binaryType !== undefined) ws.binaryType = 'arraybuffer';
    ws.onopen = () => { this.state = 'open'; this._log('spectator ws open'); };
    ws.onclose = () => { this.state = 'closed'; };
    ws.onerror = () => { this.state = 'error'; };
    ws.onmessage = (ev) => this._receive(ev.data);
    return true;
  }

  _receive(data) {
    let bytes;
    if (data instanceof ArrayBuffer) bytes = new Uint8Array(data);
    else if (ArrayBuffer.isView && ArrayBuffer.isView(data)) bytes = new Uint8Array(data.buffer, data.byteOffset, data.byteLength);
    else if (typeof data === 'string') {
      try { bytes = Uint8Array.from(atob(data), (c) => c.charCodeAt(0)); }
      catch { return; }
    } else return;
    const msg = this.buffer.push(bytes);
    if (!msg) return;
    if (msg.kind === 1) { // BEGIN
      this._begin = msg;
      return;
    }
    this.playback.onFrame(msg);
    this._recordHistory(msg);
    this._emit();
  }

  /** 记一帧输入到历史缓冲（定位时从这里重演，relay 的 live 缓冲不再是唯一来源）。 */
  _recordHistory(msg) {
    const frame = clampInt(msg && msg.frame, 0, 0xffffffff, 0);
    if (frame < this.historyMin || this.history.has(frame)) return;
    const keys = new Uint8Array(this.playerCount);
    const flags = new Uint8Array(this.playerCount);
    const rows = Array.isArray(msg.rows) ? msg.rows : [];
    for (let p = 0; p < this.playerCount; p++) {
      const row = rows[p] || null;
      keys[p] = row ? row.keys & 0xff : 0;
      flags[p] = row ? row.flags & 0xff : 0;
    }
    this.history.set(frame, { keys, flags });
    this.historyOrder.push(frame);
    if (frame > this.maxFrame) this.maxFrame = frame;
    this._trimHistory();
  }

  /** 环形淘汰：超出 historyFrames 就丢最旧的帧，并把 playback.min 前移。 */
  _trimHistory() {
    const cap = this.historyFrames;
    let over = this.history.size - cap;
    if (over <= 0) return;
    let i = this.historyTrim;
    while (over > 0 && i < this.historyOrder.length) {
      const f = this.historyOrder[i];
      if (this.history.delete(f)) {
        over--;
        if (f + 1 > this.historyMin) {
          this.historyMin = f + 1;
          // 还没重演到这一帧时不能把 min 报过 position（launcher 要求 min<=position）
          this.playback.setMin(Math.min(this.historyMin, this.playback.position));
        }
      }
      i++;
    }
    this.historyTrim = i;
    // 顺序表本身也要回收，避免历史帧数不变而数组无限长
    if (this.historyTrim > 4096) {
      this.historyOrder = this.historyOrder.slice(this.historyTrim);
      this.historyTrim = 0;
    }
  }

  /** 起同一场确定性对局。调用顺序见 agent-2 报告第 5 节 A。 */
  start() {
    if (this.started || typeof this.call !== 'function') return false;
    const cfg = this.config;
    // 1. 角色 / 系统 / 种子（必须在进关前）
    try { this.call('BRO_net_set_slot', this.localSlot); } catch { /* 老版本无此 API */ }
    const loadouts = Array.isArray(cfg.loadouts) ? cfg.loadouts : [];
    for (let p = 0; p < this.playerCount; p++) {
      const lo = loadouts[p] || {};
      const char = lo.char || lo.character || 'reimu_player';
      const systems = lo.systems || cfg.systems || [];
      try {
        if (p === this.localSlot) this.call('BRO_net_set_character', char);
        else this.call('BRO_net_set_loadout', p, char, systems, lo.index || 0);
      } catch { /* 单测假 call 可能只实现部分 */ }
    }
    if (Array.isArray(cfg.systems)) {
      try { this.call('BRO_net_set_systems', cfg.systems); } catch { /* 忽略 */ }
    }
    if (Number.isFinite(cfg.seed)) {
      try { this.call('BRO_net_set_seed', cfg.seed); } catch { /* 忽略 */ }
      try { this.ccall('bro_set_seed', null, ['number'], [cfg.seed]); } catch { /* 忽略 */ }
    }
    // 2. 进关（started=true 触发成就/存档护栏）
    const stage = cfg.stage || 'TH06@BossRush';
    try { this.call('BRO_net_start_match', stage, cfg.seed); } catch { /* 忽略 */ }
    this.started = true;
    this._log(`spectator start stage=${stage} seed=${cfg.seed}`);
    return true;
  }

  /**
   * 每逻辑帧调一次：按当前播放状态推进。
   *
   * 三种模式（阶段 B）：
   *   paused   —— 不推进，返回 reason:'paused'
   *   seeking  —— 向 playback.target 重演，一次 pump 内跑多帧（seekBudgetMs/StepCap）
   *   live     —— 追下一帧；缺帧则等待
   *
   * 返回 {advanced:boolean, frame:number, reason?:string}。advanced=false 时调用方
   * 不要扣时间步长（runtime_entry 据此停表并复用上一帧画面）。
   */
  pump() {
    if (!this.started) return { advanced: false, frame: -1, reason: 'not-started' };
    if (this.playback.seeking) return this._pumpSeek();
    if (this.playback.paused) return { advanced: false, frame: this.lastFrame, reason: 'paused' };
    return this._pumpLive();
  }

  /** 实况：逐帧消费；历史缺口时等待 relay 补齐。 */
  _pumpLive() {
    const frame = this.lastFrame + 1;
    const entry = this.history.get(frame);
    if (!entry) {
      // 中途加入（历史已被环形缓冲裁掉）时直接跳到最早可用帧，别永远卡在 0。
      if (frame < this.historyMin) {
        this.playback.setMin(this.historyMin);
        this.lastFrame = this.historyMin - 1;
        this.playback.position = this.historyMin;
      }
      this.playback.markFollowing(this.playback.position >= this.playback.edge);
      return { advanced: false, frame: this.lastFrame, reason: 'waiting' };
    }
    this._applyFrame(frame, entry);
    this.playback.onConsume(frame);
    this._syncMin();
    this._emit();
    return { advanced: true, frame };
  }

  /** 历史被裁掉后，min 只能在位置追上时前移（保持 min<=position）。 */
  _syncMin() {
    const next = Math.min(this.historyMin, this.playback.position);
    if (next > this.playback.min) this.playback.setMin(next);
  }

  /**
   * 定位：向 target 连续重演，直到到达、缺帧或超预算。
   * 慢路径来自 SPECTATOR.md 阶段 B：没有世界快照，就只能从起点重演。
   */
  _pumpSeek() {
    const pb = this.playback;
    const target = Math.max(this.historyMin, pb.min,
      Math.min(pb.target, Math.max(this.maxFrame + 1, pb.edge)));
    const deadline = this._now() + this.seekBudgetMs;
    let steps = 0;
    while (steps < this.seekStepCap && this.lastFrame + 1 < target) {
      const frame = this.lastFrame + 1;
      const entry = this.history.get(frame);
      if (!entry) break; // 缺口：等 relay 补齐这批帧，暂停定位进度但不改变状态
      this._applyFrame(frame, entry);
      steps++;
      this._replayed++;
      if (this._now() >= deadline) break;
    }
    if (steps > 0) {
      this.playback.position = this.lastFrame + 1;
      this._syncMin();
      this._emit();
    }
    const reached = this.lastFrame + 1 >= target;
    if (reached) {
      pb.seeking = false;
      pb.position = this.lastFrame + 1;
      pb.target = pb.position;
      pb.following = !pb.paused && pb.position >= pb.edge;
      pb.bump();
      this._emit(true);
      return { advanced: steps > 0, frame: this.lastFrame, reason: pb.paused ? 'seeked-paused' : 'seeked' };
    }
    if (steps === 0) {
      // 没有可重演的帧（缺帧/边界）：不是错误，回到等待
      return { advanced: false, frame: this.lastFrame, reason: 'seek-waiting' };
    }
    return { advanced: true, frame: this.lastFrame, reason: 'seeking' };
  }

  /** 把一帧历史输入喂给引擎并推进一个逻辑帧。 */
  _applyFrame(frame, entry) {
    // init_player_data 之后补 spawn 远端真实实体（只需一次；重演重置后要重来）。
    if (!this.ready && this._checkReady()) this._spawnEntities();
    for (let p = 0; p < this.playerCount; p++) {
      const keys = entry.keys[p] & 0xff;
      try {
        this.call('BRO_net_apply_keys_at', p, frame, keys, false);
        this.call('BRO_net_feed_player_key', p, keys);
      } catch (e) { this.errors.push(String(e)); }
    }
    try { this.call('BRO_net_tick', frame); } catch { /* 忽略 */ }
    try { this.call('BRO_net_set_active_input', this.localSlot); } catch { /* 忽略 */ }
    const rc = this.ccall('bro_frame', 'number', [], []);
    if (rc !== 0) this.errors.push(`bro_frame=${rc}`);
    this.lastFrame = frame;
    this.pumped++;
  }

  /**
   * 回到 run 起点重演（BRO 没有世界快照序列化，这是 SPECTATOR.md 阶段 B 允许的慢路径）。
   * BRO_net_reset 会把 localSlot 重置成 0，所以必须再走一遍 start() 的配置下发。
   */
  _beginReplay() {
    try { this.call('BRO_net_reset'); } catch (e) { this.errors.push(String(e)); }
    this.started = false;
    this.ready = false;
    this.lastFrame = this.playback.min - 1;
    this.playback.position = this.playback.min;
    this.started = this.start() !== false;
    this._log(`spectator replay from frame ${this.playback.min}`);
  }

  /**
   * 阶段 B 命令入口：pause | play | seek | live。
   * 返回 {ok, action, ...}；adapter 把它塞进命令回执。
   */
  control(action, frame) {
    const pb = this.playback;
    const act = String(action || '');
    const maxFrame = this.maxFrame < 0 ? 0 : this.maxFrame;
    if (act === 'pause') {
      pb.seeking = false;          // 暂停同时中止正在进行的定位，世界停在当前位置
      pb.target = pb.position;
      pb.markPaused(true);
      this._emit(true);
      return { ok: true, action: act, paused: true, position: pb.position };
    }
    if (act === 'play') {
      pb.markPaused(false);
      pb.target = Math.max(pb.edge, pb.position);
      pb.seeking = pb.position < pb.edge;
      pb.following = !pb.seeking;
      pb.bump();
      this._emit(true);
      return { ok: true, action: act, paused: false, position: pb.position, seeking: pb.seeking };
    }
    if (act === 'seek') {
      const lo = Math.max(pb.min, this.historyMin);
      const t = clampInt(frame, lo, Math.max(lo, maxFrame), pb.position);
      if (t < pb.position) this._beginReplay(); // 只能从起点重演回到过去
      pb.target = t;
      pb.seeking = this.lastFrame + 1 < t;
      pb.following = false;
      if (!pb.seeking) { pb.position = t; pb.bump(); }
      else pb.bump();
      this._emit(true);
      return { ok: true, action: act, target: t, seeking: pb.seeking, position: pb.position };
    }
    if (act === 'live') {
      pb.markPaused(false);
      const edge = Math.max(pb.edge, maxFrame);
      pb.target = edge;
      pb.seeking = pb.position < edge;
      pb.following = !pb.seeking;
      pb.bump();
      this._emit(true);
      return { ok: true, action: act, target: edge, seeking: pb.seeking, position: pb.position };
    }
    return { ok: false, reason: 'unknown-action', action: act };
  }

  /**
   * 判断 init_player_data 是否完成（远端实体可 spawn 的时机）。
   * 优先问 Lua 全局 `lstg.var.init_player_data`（agent-2 报告：进关约 130 帧后置位）；
   * BRO_net_state() 没有这个字段，所以只在有 jsonCall 且返回体显式带标记时才信它。
   */
  _checkReady() {
    if (typeof this.jsonCall !== 'function') return false;
    try {
      const v = this.jsonCall('BRO_spectator_init_player_data');
      if (v === true || v === 1 || v === 'true' || v === '1') return true;
    } catch { /* 老版本无此门面 */ }
    try {
      const st = this.jsonCall('BRO_net_state') || {};
      if (st.init_player_data || st.initPlayerData || st.ready) return true;
    } catch { /* 忽略：下一帧再试 */ }
    return false;
  }

  _spawnEntities() {
    const loadouts = Array.isArray(this.config.loadouts) ? this.config.loadouts : [];
    for (let p = 0; p < this.playerCount; p++) {
      if (p === this.localSlot) continue;
      const char = (loadouts[p] || {}).char || (loadouts[p] || {}).character || 'reimu_player';
      try {
        const r = this.jsonCall('BRO_net_spawn_player', p, char);
        if (r && r.error) this.errors.push(String(r.error));
      } catch (e) { this.errors.push(String(e)); }
    }
    this.ready = true;
    this._log('spectator remote entities spawned');
  }

  _log(msg) { if (this.log) try { this.log(msg); } catch { /* 忽略 */ } }

  close() {
    if (this.ws) { try { this.ws.close(); } catch { /* 忽略 */ } this.ws = null; }
    this.state = 'closed';
    this.playback.markEnded();
    this._emit();
  }
}

export const SPECTATOR_EXPORT_VERSION = 1;
