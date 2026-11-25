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
    this.nodes = [];
  }

  /** 收到一帧（已解包）后更新计数。 */
  onFrame(frame) {
    const f = clampInt(frame && frame.frame, 0, 0xffffffff, 0);
    this.received = Math.max(this.received, f + 1);
    this.edge = this.received;
    this.target = this.received;
    this._clamp();
    return this.snapshot();
  }

  /** 每消费一帧推进播放位置。 */
  onConsume(frame) {
    const f = clampInt(frame, 0, 0xffffffff, 0);
    this.position = Math.max(this.position, f + 1);
    this.min = Math.min(this.min, this.position);
    this._clamp();
    this.revision++;
    return this.snapshot();
  }

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
  }

  _emit() {
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
    this._emit();
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
   * 每逻辑帧调一次：消费下一帧观战输入、喂引擎、推进 bro_frame。
   * 返回 {advanced:boolean, frame:number}。缺帧时不推进（等待）。
   */
  pump() {
    if (!this.started) return { advanced: false, frame: -1, reason: 'not-started' };
    const msg = this.buffer.next();
    if (!msg) return { advanced: false, frame: this.lastFrame, reason: 'waiting' };

    // init_player_data 之后补 spawn 远端真实实体（只需一次）。
    if (!this.ready && this._checkReady()) this._spawnEntities();

    const frame = msg.frame;
    for (let p = 0; p < this.playerCount; p++) {
      const row = msg.rows[p] || { keys: 0, flags: 0, x: 0, y: 0 };
      const keys = row.keys & 0xff;
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
    this.playback.onConsume(frame);
    this._emit();
    return { advanced: true, frame };
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
