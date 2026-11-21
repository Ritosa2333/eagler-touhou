/**
 * BRO Web 联机传输层（阶段 3C 骨架，锁步方案第一步）
 *
 * 职责边界：只做「把每个玩家每帧的 1 字节输入可靠、有序地送到对端」。
 * 不碰游戏逻辑、不做状态快照/回滚（那是 PROBE_3B 的硬骨头，后续阶段）。
 *
 * 设计来源：
 *  - eagler-touhou 的 ConfirmedInputClock / inputTiming 契约（帧号驱动的输入）
 *  - BRO replay 的 KEY_BIT 编码（game/THlib/plus/Replay.lua: KeyState2Byte）
 *  - WebRTC perfect negotiation pattern（MDN，双方都可 offer，避免 glare）
 *
 * 传输：WebRTC RTCDataChannel（可靠 + 有序，ordered:true）。选它而非 WebSocket
 * 直连的原因：点对点、NAT 穿透后延迟低；WebSocket 只做信令（SDP/ICE 交换）。
 *
 * 帧协议（二进制，DataChannel send(ArrayBuffer)）：
 *   [0] type   : 1=INPUT 2=HELLO 3=SEED 4=PING 5=LOADOUT 6=READY
 *                7=PONG 8=ACK 9=TIMING 10=POS 11=DIGEST 12=GATE（均复用 7 字节头）
 *   POS（阶段 3G，共 15 字节）：7B 头 + [7..10] x:float32 LE + [11..14] y:float32 LE
 *   DIGEST（阶段 3I，共 15 字节）：7B 头 + hash:u32 LE + objCount:u32 LE
 *   GATE（阶段 3J，7B 头）：frame = 共同起跑墙钟帧号；keys bit0=host 权威
 *   [2..5] frame 仍是逻辑帧号，x/y 是本端 player 世界坐标，供对端实体位置同步。
 *   [1] player : 本端玩家号（0/1）
 *   [2..5] frame: uint32 little-endian —— INPUT/ACK 是逻辑帧号；
 *                 PING/PONG 是本端 performance.now() 的低 32 位毫秒时间戳；
 *                 TIMING 是建议输入延迟帧数
 *   [6] keys   : INPUT 的 KEY_BIT 字节；PING/PONG 装序号；ACK 装确认的玩家号；
 *                 TIMING 装标志位（bit0 = host 权威）
 *
 * 时序辅助（阶段 3E）：PING→PONG 测 RTT（无需两端时钟同步，回传发送端时间戳），
 * ACK 告诉发送端「对端已收到第 N 帧」，TIMING 由 host 下发统一的输入延迟帧数。
 * 这些只提供数据，真正的缓冲/预测在 input_buffer.mjs。
 *
 * 用法：
 *   const net = new NetPeer({ role:'host', room:'bro-001', player:0,
 *     signal: { send(msg), onMessage(cb) } });
 *   net.onInput((frame, player, keys) => { ... });   // 收到对端每帧输入
 *   await net.start();
 *   net.sendInput(frame, keys);                       // 每逻辑帧调一次
 */

export const MSG = {
  INPUT: 1, HELLO: 2, SEED: 3, PING: 4, LOADOUT: 5, READY: 6,
  PONG: 7, ACK: 8, TIMING: 9, POS: 10, DIGEST: 11, GATE: 12,
  ROUTE: 13, // 3Q：换路交接 switch/accept/commit（走 packText 的 JSON）
};
import { RouteMonitor } from './route_monitor.mjs';
import { RouteHandoff } from './route_handoff.mjs';

export const KEY_BIT = { up: 128, down: 64, left: 32, right: 16, slow: 8, shoot: 4, spell: 2, special: 1 };
const KEY_ORDER = ['up', 'down', 'left', 'right', 'slow', 'shoot', 'spell', 'special'];

/** 单调毫秒时钟（Node / 浏览器通用）；RTT 只在同一端做差，不需要两端时钟同步。 */
export function nowMs() {
  return (typeof performance !== 'undefined' && performance && performance.now)
    ? performance.now() : Date.now();
}

/** 把 KeyState 表编成 1 字节（与 Replay.lua KeyState2Byte 完全一致） */
export function keyStateToByte(state) {
  let b = 0;
  for (let i = 0; i < KEY_ORDER.length; i++) {
    if (state && state[KEY_ORDER[i]]) b |= KEY_BIT[KEY_ORDER[i]];
  }
  return b & 0xff;
}

/** 把 1 字节解成 KeyState 表（与 Replay.lua Byte2KeyState 完全一致） */
export function byteToKeyState(b) {
  const out = {};
  for (let i = 0; i < KEY_ORDER.length; i++) {
    out[KEY_ORDER[i]] = (b & KEY_BIT[KEY_ORDER[i]]) !== 0;
  }
  return out;
}

/** 打包一帧输入为 7 字节 ArrayBuffer */
export function packInput(type, player, frame, keys) {
  const buf = new ArrayBuffer(7);
  const v = new DataView(buf);
  v.setUint8(0, type & 0xff);
  v.setUint8(1, player & 0xff);
  v.setUint32(2, frame >>> 0, true);
  v.setUint8(6, keys & 0xff);
  return buf;
}

/** 解包一帧输入；返回 {type, player, frame, keys} 或 null（长度非法） */
export function unpackInput(buf) {
  const bytes = buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf;
  if (!bytes || bytes.length < 7) return null;
  const v = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return {
    type: v.getUint8(0),
    player: v.getUint8(1),
    frame: v.getUint32(2, true),
    keys: v.getUint8(6),
  };
}

/**
 * 变长文本消息（LOADOUT / READY）：复用 7 字节头，frame 字段装 payload 长度。
 * 选择/房间信息用 JSON，浏览器与 Node18+ 都有 TextEncoder/TextDecoder。
 */
export function packText(type, player, text) {
  const payload = new TextEncoder().encode(String(text == null ? '' : text).slice(0, 4096));
  const buf = new ArrayBuffer(7 + payload.length);
  const v = new DataView(buf);
  v.setUint8(0, type & 0xff);
  v.setUint8(1, player & 0xff);
  v.setUint32(2, payload.length, true);
  v.setUint8(6, 0);
  new Uint8Array(buf, 7).set(payload);
  return buf;
}

/** 解包文本消息；长度非法或 UTF-8 解码失败返回 null */
export function unpackText(buf) {
  const p = unpackInput(buf);
  if (!p) return null;
  // ArrayBuffer 没有 byteOffset，不能直接用 buf.byteOffset（会得到 NaN）。
  const bytes = buf instanceof DataView
    ? new Uint8Array(buf.buffer, buf.byteOffset, buf.byteLength)
    : (buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf);
  const off = 7;
  if (bytes.byteLength < off + p.frame) return null;
  try {
    return { type: p.type, player: p.player,
      text: new TextDecoder().decode(bytes.subarray(off, off + p.frame)) };
  } catch { return null; }
}

/**
 * 位置帧（阶段 3G）：15 字节 = 7B 头 + x:float32 + y:float32。
 * head 的 keys[6] 复用为标志位（bit0 = 有效）。x/y 用 float32 足够覆盖
 * 游戏世界坐标（±384），且比 JSON 文本帧省一个数量级的带宽。
 */
export function packPos(player, frame, x, y) {
  const buf = new ArrayBuffer(15);
  const v = new DataView(buf);
  v.setUint8(0, MSG.POS);
  v.setUint8(1, player & 0xff);
  v.setUint32(2, frame >>> 0, true);
  v.setUint8(6, 1); // flags: valid
  v.setFloat32(7, Number(x) || 0, true);
  v.setFloat32(11, Number(y) || 0, true);
  return buf;
}

/** 解包位置帧；长度非法返回 null。 */
export function unpackPos(buf) {
  const bytes = buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf;
  if (!bytes || bytes.byteLength < 15) return null;
  const v = bytes instanceof DataView
    ? bytes : new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (v.getUint8(0) !== MSG.POS) return null;
  return {
    type: MSG.POS,
    player: v.getUint8(1),
    frame: v.getUint32(2, true),
    flags: v.getUint8(6),
    x: v.getFloat32(7, true),
    y: v.getFloat32(11, true),
  };
}

/**
 * 状态摘要帧（阶段 3I）：15 字节，与 POS 同构。
 * 头 frame = 逻辑帧号；[6] flags；[7..10] hash:uint32 LE；[11..14] 对象数/保留。
 * hash 由 WASM 侧对「对象坐标/速度 + lstg.var 关键字段」做 FNV-1a 得到，
 * 两端在同一逻辑帧若状态一致则 hash 必须相等——这是分叉检测/回滚基线的前提。
 */
export function packDigest(player, frame, hash, objCount) {
  const buf = new ArrayBuffer(15);
  const v = new DataView(buf);
  v.setUint8(0, MSG.DIGEST);
  v.setUint8(1, player & 0xff);
  v.setUint32(2, frame >>> 0, true);
  v.setUint8(6, 1); // flags: valid
  v.setUint32(7, hash >>> 0, true);
  v.setUint32(11, objCount >>> 0, true);
  return buf;
}

/** 解包状态摘要帧；长度/类型非法返回 null。 */
export function unpackDigest(buf) {
  const bytes = buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf;
  if (!bytes || bytes.byteLength < 15) return null;
  const v = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (v.getUint8(0) !== MSG.DIGEST) return null;
  return {
    type: MSG.DIGEST,
    player: v.getUint8(1),
    frame: v.getUint32(2, true),
    flags: v.getUint8(6),
    hash: v.getUint32(7, true),
    objCount: v.getUint32(11, true),
  };
}

/**
 * NetPeer —— 一个 WebRTC 点对点端点。
 * signal 接口：{ send(msgObj), onMessage(cb) }，由调用方接到 WebSocket/信令通道。
 */
export class NetPeer {
  constructor(opts) {
    this.role = opts.role || 'host';           // host | client（仅影响游戏权威：种子/TIMING/GATE）
    this.room = opts.room || 'bro-default';
    this.player = opts.player | 0;
    // 3T：对端席位 + offer 主导权。1v1 未传 remotePlayer 时保持旧语义
    // （to = 1-player、host 发 offer）；mesh 模式显式传 remotePlayer，
    // 每对连接由 player 号小的一方发 offer —— 确定性，避免双方 glare。
    this.remotePlayer = Number.isFinite(opts.remotePlayer)
      ? (opts.remotePlayer | 0) : (1 - this.player);
    this.initiator = opts.initiator !== undefined
      ? !!opts.initiator
      : (opts.remotePlayer !== undefined
        ? this.player < this.remotePlayer
        : this.role === 'host');
    this.signal = opts.signal;
    this.iceServers = opts.iceServers || [
      { urls: 'stun:stun.l.google.com:19302' },
    ];
    // 3O：eagler 契约——优先 WebRTC 直连，连不上才自动走 TURN 中继。
    // preferTurn=true 时直接使用 iceTransportPolicy:'relay'（大厅「优先使用 TURN」）。
    this.preferTurn = opts.preferTurn === true;
    this.turnServers = (this.iceServers || []).filter((s) =>
      String(s && s.urls || '').toLowerCase().includes('turn:'));
    this.connectTimeoutMs = Number.isFinite(opts.connectTimeoutMs)
      ? Math.max(500, opts.connectTimeoutMs | 0) : 8000;
    this.route = {
      path: 'unknown', candidateType: null, remoteCandidateType: null,
      rtt: null, mode: this.preferTurn ? 'turn' : 'auto',
      fallback: null, reason: null, at: 0,
    };
    this._fallbackTried = false;
    this._connectTimer = null;
    this._routeTimer = null;
    this._routeCbs = [];
    this._signalBound = false;
    this._signalDown = false;
    this.routeLocked = false;        // 3P：开闸进入对局后锁定路径，不再因测量重建
    this.pc = null;
    this.dc = null;
    this.ready = false;
    this.polite = this.role === 'client';      // perfect negotiation：client 礼貌侧
    this._makingOffer = false;
    this._ignoreOffer = false;
    this._setRemotePending = false;
    this._inputCbs = [];
    this._openCbs = [];
    this._loadoutCbs = [];
    this._readyCbs = [];
    this._ackCbs = [];
    this._timingCbs = [];
    this._posCbs = [];
    this._digestCbs = [];
    this._gateCbs = [];
    this.lastPos = null;             // 最近收到的对端位置 {frame, player, x, y}
    this.lastGate = null;            // 阶段 3J：host 下发的共同起跑帧号
    this.remoteSeed = null;
    this.stats = { sent: 0, recv: 0, dropped: 0, lastFrame: -1 };

    // ---- 时序状态（阶段 3E） ----
    this.rtt = null;                 // 中位数 RTT（ms），null = 尚未测到
    this._rttSamples = [];           // 最近 16 个样本
    this._pingSeq = 0;
    this._pendingPing = new Map();   // seq -> 发送时刻
    this.remoteAck = -1;             // 对端确认收到的最大 INPUT 帧号
    this.delayFrames = 2;            // host 下发的统一输入延迟；开局后应锁定
    this._heartbeat = null;

    // ---- 开局选路测量（阶段 3P） ----
    // 密集 PING 收集 RTT/丢包，交给 RouteSelector 评分。与心跳/常规 RTT 采样
    // 隔离：survey 用自己的序号集合与定时器，不污染 this.rtt 的中位数。
    this._survey = null;             // 活动中的 survey 状态（见 runRouteSurvey）

    // ---- 持续质量监控 / 换路交接（阶段 3Q） ----
    this.routeMonitor = null;        // RouteMonitor 实例（armRouteMonitor 后启用）
    this.handoff = null;             // RouteHandoff 实例（换路时建立）
    this._evalTimer = null;          // 周期评估定时器
    this._routeCbs2 = [];            // onquality / onhandoff 之外的路径事件（保留扩展）
    this._qualityCbs = [];
    this._handoffCbs = [];
    this._controlFrameSeq = 0;       // 交接期控制帧缓冲序号
    this._lastControlSnapshot = {};  // 最近一次关键控制帧（换路后重放）
  }

  async start() {
    if (!this.signal || typeof this.signal.send !== 'function')
      throw new Error('NetPeer: signal.send required');
    this._openPc({ forceTurn: this.preferTurn });
    this._bindSignal();

    // initiator 主动创建 DataChannel 并发 offer（mesh 下每对由小号 player 发起）
    if (this.initiator) {
      // 可靠 + 有序：不设 maxRetransmits / maxPacketLifeTime（两者互斥，缺省即可靠）
      const ch = this.pc.createDataChannel('bro-input', { ordered: true });
      this._attachChannel(ch);
      await this._negotiate();
    }
    this._watchConnect();
    this.startRouteProbe();
    return this;
  }

  /** 建连超时 / ICE failed 时，改用强制 relay 重建一条候选连接。 */
  _openPc({ forceTurn = false } = {}) {
    if (this.pc) { try { this.pc.close(); } catch (_) { /* noop */ } }
    const useRelay = forceTurn || this.preferTurn;
    const config = { iceServers: this.iceServers };
    if (useRelay) config.iceTransportPolicy = 'relay';
    this.pc = new RTCPeerConnection(config);
    this.route.mode = useRelay ? 'turn' : 'auto';
    this.pc.onicecandidate = ({ candidate }) => {
      if (candidate) this._sig({ kind: 'ice', room: this.room, from: this.player, candidate });
    };
    this.pc.onconnectionstatechange = () => {
      const st = this.pc && this.pc.connectionState;
      if (typeof this.onstate === 'function') this.onstate(st);
      // eagler：直连失败先试 TURN，没有 TURN 才退回 ICE restart。
      if (st === 'failed' && !this._fallbackTurn('ice-failed')) {
        try { this.pc.restartIce(); } catch (_) { /* noop */ }
      }
    };
    this.pc.oniceconnectionstatechange = () => {
      if (typeof this.onice === 'function') this.onice(this.pc.iceConnectionState);
    };
    // 被动接受对端创建的 DataChannel
    this.pc.ondatachannel = ({ channel }) => this._attachChannel(channel);
  }

  /** 信令回调只绑一次；重建 PC 不会重复注册导致 SDP/ICE 被处理两次。 */
  _bindSignal() {
    if (this._signalBound) return;
    this._signalBound = true;
    this.signal.onMessage((msg) => this._onSignal(msg).catch((e) => {
      console.error('[net] signal error', e);
    }));
    // eagler：信令 WebSocket 断开不结束已经建立的 RTC 对局，只标记状态。
    if (typeof this.signal.onClose === 'function') {
      this.signal.onClose(() => {
        this._signalDown = true;
        if (typeof this.onstate === 'function') this.onstate('signaling-down');
      });
    }
    if (typeof this.signal.onOpen === 'function') {
      this.signal.onOpen(() => {
        this._signalDown = false;
        if (typeof this.onstate === 'function') this.onstate('signaling-up');
      });
    }
  }

  /** 直连失败 → TURN 回退（只试一次，避免反复重建）。返回是否真的回退。 */
  _fallbackTurn(reason) {
    if (this.ready || this._fallbackTried || this.preferTurn || !this.turnServers.length) return false;
    this._fallbackTried = true;
    this.ready = false;
    this.dc = null;
    this.route.fallback = reason;
    this._openPc({ forceTurn: true });
    if (this.initiator) {
      const ch = this.pc.createDataChannel('bro-input', { ordered: true });
      this._attachChannel(ch);
      this._negotiate().catch((e) => console.error('[net] turn renegotiate', e));
    } else {
      // client 请 host 重新 offer（礼貌侧不主动 glare）。
      this._sig({ kind: 'renegotiate', room: this.room, from: this.player, reason });
    }
    if (typeof this.onroute === 'function') this.onroute(this.getRouteSnapshot());
    return true;
  }

  /**
   * 3P：开局质量选路专用入口。与 3O 的「建连失败回退」不同——测量时 DC
   * 已经 OPEN（ready=true），但游戏尚未开跑，此刻允许按测量结果重建为 relay。
   * 只在未锁定对局（routeLocked=false）时生效：一旦 host 开闸进入对局就不再
   * 重建，避免游戏中途断线（那是 3O/持续选路的另一个话题）。
   * 返回是否真的切换。
   */
  applySurveyRoute(path, opts = {}) {
    const wantTurn = path === 'turn';
    const isTurn = this.route.path === 'turn' ||
      (this.pc && this.pc.config && this.pc.config.iceTransportPolicy === 'relay');
    // 3Q：换路交接显式 override 解锁（对局中持续选路要能切）；3P 手动路径仍受锁。
    if (this.routeLocked && opts.override !== true) return false;
    if (wantTurn === isTurn) return false;
    // 复用回退逻辑但临时解除 ready 门禁（测量期/交接期允许重建）。
    if (wantTurn && !this.preferTurn && this.turnServers.length) {
      const wasReady = this.ready, wasTried = this._fallbackTried;
      this.ready = false;
      this._fallbackTried = false;   // 3Q：持续选路每次交接允许重新尝试 relay
      const ok = this._fallbackTurn(opts.override ? 'handoff-quality' : 'survey-quality');
      if (!ok) { this.ready = wasReady; this._fallbackTried = wasTried; }
      return ok;
    }
    // 想回直连：3O 一次性回退不自动回切，测量期可主动重建为非 relay PC。
    if (!wantTurn && isTurn && !this.preferTurn) {
      this._fallbackTried = false;
      this.ready = false;
      this.dc = null;
      this._openPc({ forceTurn: false });
      if (this.initiator) {
        const ch = this.pc.createDataChannel('bro-input', { ordered: true });
        this._attachChannel(ch);
        this._negotiate().catch((e) => console.error('[net] survey renegotiate', e));
      } else {
        this._sig({ kind: 'renegotiate-direct', room: this.room, from: this.player });
      }
      this.route.fallback = null;
      if (typeof this.onroute === 'function') this.onroute(this.getRouteSnapshot());
      return true;
    }
    return false;
  }

  /** 建连看门狗：超时未 OPEN 自动转 TURN；再超时则明确报 connect-timeout。 */
  _watchConnect() {
    if (this._connectTimer) clearInterval(this._connectTimer);
    const t0 = nowMs();
    this._connectTimer = setInterval(() => {
      if (this.ready) {
        clearInterval(this._connectTimer); this._connectTimer = null; return;
      }
      const elapsed = nowMs() - t0;
      if (elapsed >= this.connectTimeoutMs && !this._fallbackTried) {
        this._fallbackTurn('connect-timeout');
      }
      if (elapsed >= this.connectTimeoutMs * 2) {
        clearInterval(this._connectTimer); this._connectTimer = null;
        this.route.reason = 'connect-timeout';
        if (typeof this.onstate === 'function') this.onstate('connect-timeout');
      }
    }, 250);
  }

  async _negotiate() {
    this._makingOffer = true;
    try { await this.pc.setLocalDescription(); }
    finally { this._makingOffer = false; }
    this._sig({ kind: 'description', room: this.room, from: this.player, description: this.pc.localDescription });
  }

  async _onSignal(msg) {
    if (!msg || msg.room !== this.room) return;
    if (msg.from === this.player) return;
    // 3T：多条 NetPeer 共享一条信令 WebSocket，必须按 to+from 双过滤，
    // 否则 p0↔p1 的 SDP 会被 p0↔p2 的 NetPeer 误吃（offer 串线）。
    if (msg.from !== this.remotePlayer) return;
    if (msg.to !== undefined && msg.to !== this.player) return;
    const description = msg.description, candidate = msg.candidate;
    // 3O：对端已切到 TURN，要求本端（host）重新发 offer。
    if (msg.kind === 'renegotiate' || msg.kind === 'renegotiate-direct') {
      // 两端都切到目标路径（relay 或直连）；host 另建 DC 并重发 offer，client 等新 offer。
      const forceTurn = msg.kind === 'renegotiate';
      this._openPc({ forceTurn });
      if (this.initiator) {
        const ch = this.pc.createDataChannel('bro-input', { ordered: true });
        this._attachChannel(ch);
        await this._negotiate();
      }
      return;
    }
    if (description) {
      // perfect negotiation 的 glare 处理
      const offerCollision = description.type === 'offer' &&
        (this._makingOffer || this.pc.signalingState !== 'stable');
      this._ignoreOffer = !this.polite && offerCollision;
      if (this._ignoreOffer) return;
      if (offerCollision) await Promise.all([
        this.pc.setLocalDescription({ type: 'rollback' }),
        this.pc.setRemoteDescription(description),
      ]);
      else await this.pc.setRemoteDescription(description);
      if (description.type === 'offer') await this._negotiate();
    } else if (candidate) {
      try { await this.pc.addIceCandidate(candidate); }
      catch (e) { if (!this._ignoreOffer) throw e; }
    }
  }

  /** 3T：发往指定对端（不再是写死的 1-player）；mesh 共享信令靠 to/from 精确投递。 */
  _sig(msg) { this.signal.send({ ...msg, to: this.remotePlayer }); }

  _attachChannel(ch) {
    if (this.dc === ch) return;
    this.dc = ch;
    ch.binaryType = 'arraybuffer';
    ch.onopen = () => {
      this.ready = true;
      this._openCbs.forEach((cb) => cb());
      // 握手：交换玩家号
      ch.send(packInput(MSG.HELLO, this.player, 0, 0));
    };
    ch.onmessage = (ev) => {
      const p = unpackInput(ev.data);
      if (!p) { this.stats.dropped++; return; }
      if (p.type === MSG.INPUT) {
        this.stats.recv++;
        this.stats.lastFrame = p.frame;
        // 可靠有序通道：收到第 N 帧即可确认到 N（缓冲层的空洞由应用侧判断）。
        this.sendAck(p.frame);
        this._inputCbs.forEach((cb) => cb(p.frame, p.player, p.keys));
      } else if (p.type === MSG.ACK) {
        if (p.frame > this.remoteAck) this.remoteAck = p.frame;
        this._ackCbs.forEach((cb) => cb(p.frame, p.keys));
      } else if (p.type === MSG.PING) {
        // 回传对方的时间戳（p.frame），不改写成自己的时钟。
        this.dc.send(packInput(MSG.PONG, this.player, p.frame, p.keys));
      } else if (p.type === MSG.PONG) {
        const sent = this._pendingPing.get(p.keys);
        if (sent !== undefined) {
          this._pendingPing.delete(p.keys);
          const rtt = nowMs() - sent;
          this._addRttSample(rtt);
          // 3Q：持续监控吃心跳往返（survey 期间的 PONG 不算，避免双重计样本）。
          if (this.routeMonitor && this.routeMonitor.enabled && !(this._survey && this._survey.seqs.size)) {
            this.routeMonitor.noteRtt(rtt);
            if (this._monitorPending) this._monitorPending.delete(p.keys);
          }
          // 3P：survey 窗口内的 PONG 单独记账（含序号，便于丢包判定）。
          if (this._survey && this._survey.seqs.has(p.keys)) {
            const e = this._survey.seqs.get(p.keys);
            if (e && e.timer) clearTimeout(e.timer);
            this._survey.seqs.delete(p.keys);
            this._survey.selector.addSample(rtt, p.keys, nowMs());
            this._survey.done++;
            if (typeof this._survey.onSample === 'function') {
              try { this._survey.onSample(this._survey.done, this._survey.total, this._survey.selector.metrics(), rtt); } catch (_) { /* noop */ }
            }
            if (typeof this._survey.onReturn === 'function') this._survey.onReturn();
          }
        }
      } else if (p.type === MSG.TIMING) {
        this.delayFrames = p.frame | 0;
        this._rememberControl('timing', { delay: this.delayFrames, authoritative: !!(p.keys & 1) });
        this._timingCbs.forEach((cb) => cb(this.delayFrames, p.keys));
      } else if (p.type === MSG.GATE) {
        // 阶段 3J：host 下发共同起跑墙钟帧号（frame 字段），keys bit0=权威。
        this.lastGate = p.frame >>> 0;
        this._rememberControl('gate', { startFrame: this.lastGate, authoritative: !!(p.keys & 1) });
        this._gateCbs.forEach((cb) => cb(this.lastGate, p.player, p.keys));
      } else if (p.type === MSG.POS) {
        const pos = unpackPos(ev.data);
        if (!pos) { this.stats.dropped++; return; }
        this.lastPos = pos;
        this._rememberControl('pos', { frame: pos.frame, x: pos.x, y: pos.y });
        this._posCbs.forEach((cb) => cb(pos.frame, pos.player, pos.x, pos.y));
      } else if (p.type === MSG.DIGEST) {
        const dg = unpackDigest(ev.data);
        if (!dg) { this.stats.dropped++; return; }
        this._rememberControl('digest', { frame: dg.frame, hash: dg.hash, objCount: dg.objCount });
        this._digestCbs.forEach((cb) => cb(dg.frame, dg.player, dg.hash, dg.objCount));
      } else if (p.type === MSG.SEED && this.role === 'client') {
        const v = new DataView(ev.data);
        this.remoteSeed = v.getUint32(6, true);
      } else if (p.type === MSG.LOADOUT || p.type === MSG.READY) {
        const t = unpackText(ev.data);
        if (!t) { this.stats.dropped++; return; }
        if (p.type === MSG.LOADOUT) this._rememberControl('loadout', { player: t.player, text: t.text });
        const cbs = p.type === MSG.LOADOUT ? this._loadoutCbs : this._readyCbs;
        cbs.forEach((cb) => cb(t.player, t.text));
      } else if (p.type === MSG.ROUTE) {
        // 3Q：换路交接控制帧（switch/accept/commit）。
        const t = unpackText(ev.data);
        if (!t) { this.stats.dropped++; return; }
        this._onRouteControl(t.text).catch((e) => console.error('[net] route control', e));
      }
    };
    ch.onclose = () => { this.ready = false; };
  }

  onOpen(cb) { this._openCbs.push(cb); return this; }
  onInput(cb) { this._inputCbs.push(cb); return this; }
  onLoadout(cb) { this._loadoutCbs.push(cb); return this; }
  onReady(cb) { this._readyCbs.push(cb); return this; }
  onAck(cb) { this._ackCbs.push(cb); return this; }
  onTiming(cb) { this._timingCbs.push(cb); return this; }
  onPos(cb) { this._posCbs.push(cb); return this; }
  onDigest(cb) { this._digestCbs.push(cb); return this; }
  onGate(cb) { this._gateCbs.push(cb); return this; }

  // ---- 时序（阶段 3E） --------------------------------------------------
  _addRttSample(ms) {
    if (!Number.isFinite(ms) || ms < 0 || ms > 60000) return;
    this._rttSamples.push(ms);
    if (this._rttSamples.length > 16) this._rttSamples.shift();
    const sorted = this._rttSamples.slice().sort((a, b) => a - b);
    this.rtt = sorted[Math.floor(sorted.length / 2)]; // 中位数抗偶发抖动
  }

  /** 发一次 PING；序号放 keys 字段，回包时凭它取回发送时刻。 */
  ping() {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    const seq = (this._pingSeq = (this._pingSeq + 1) & 0xff) || 1; // 0 保留
    const t = nowMs();
    this._pendingPing.set(seq, t);
    if (this.routeMonitor && this.routeMonitor.enabled && this._monitorPending) {
      this._monitorPending.set(seq, t);
      this.routeMonitor.noteSent();
    }
    this.dc.send(packInput(MSG.PING, this.player, t >>> 0, seq));
    return true;
  }

  /**
   * 阶段 3P：开局「加载期选路测量」。在 count 个间隔里密集 PING，
   * 每个 PING 用序号记账：超时未回记丢包，回来的记 RTT。窗口结束后给出
   * { rtt50, rtt95, loss, jitter, samples, path, decision, score, reason, metrics }。
   *
   * 这是「联机开始后卡在加载界面一会做延迟判定」的实质：调用方（门面/网页）
   * 在测量期间不推进游戏，测完再按 decision 选路/开跑。与心跳采样隔离。
   *
   * opts:
   *   count        PING 次数（默认 12）
   *   intervalMs   发送间隔（默认 120ms）
   *   timeoutMs    单个 PING 超时（默认 900ms），超时记丢包
   *   selectorOpts 传给 RouteSelector 的阈值（minSamples/rttP95Threshold/lossThreshold…）
   *   onProgress   (done, total, metrics) => void 每次往返/超时回调（网页进度条）
   * 返回 Promise<report>；测量未开始（DC 未 open）时返回 null。
   */
  async runRouteSurvey(opts = {}) {
    // 动态 import 保持 netcode.mjs 独立可被引擎侧复用时无额外依赖（同为本地 ESM）。
    const { RouteSelector } = await import('./route_selector.mjs');
    const count = Math.max(2, Number(opts.count) || 12);
    const intervalMs = Math.max(20, Number(opts.intervalMs) || 120);
    const timeoutMs = Math.max(100, Number(opts.timeoutMs) || 900);
    if (!this.dc || this.dc.readyState !== 'open') return null;

    // 复用已有 selector（若门面预置），否则新建。
    const selector = opts.selector instanceof RouteSelector
      ? opts.selector : new RouteSelector(opts.selectorOpts || {});
    selector.reset();

    // 若上一次 survey 未收尾，先强制结束避免串扰。
    if (this._survey) this._finishSurvey('superseded');

    const state = {
      selector,
      seqs: new Map(),       // seq -> { timer, sentAt }
      total: count,
      done: 0,
      resolve: null,
      onSample: opts.onProgress,
      timeoutMs,
      finished: false,
      reason: null,
    };
    this._survey = state;

    const report = await new Promise((resolve) => {
      state.resolve = resolve;
      let fired = 0;
      const fire = () => {
        if (state.finished || fired >= count) return;
        const seq = (this._pingSeq = (this._pingSeq + 1) & 0xff) || 1; // 0 保留
        const t = nowMs();
        selector.noteSent();
        state.seqs.set(seq, { sentAt: t });
        this._pendingPing.set(seq, t);
        this.dc.send(packInput(MSG.PING, this.player, t >>> 0, seq));
        // 超时未回 → 记丢包并推进进度
        const timer = setTimeout(() => {
          const e = state.seqs.get(seq);
          if (!e) return;                    // 已收到 PONG，正常回收
          state.seqs.delete(seq);
          this._pendingPing.delete(seq);
          selector.noteLost(seq);
          state.done++;
          if (typeof state.onSample === 'function') {
            try { state.onSample(state.done, state.total, selector.metrics(), null); } catch (_) { /* noop */ }
          }
          maybeFinish();
        }, timeoutMs);
        state.seqs.get(seq).timer = timer;
        fired++;
      };
      const maybeFinish = () => {
        if (state.finished || state.done < count) return;
        this._finishSurvey(null);
      };
      state.onReturn = maybeFinish;

      // 定时发出 count 个 PING
      fire();
      const ticker = setInterval(() => {
        if (state.finished) { clearInterval(ticker); return; }
        fire();
        if (fired >= count) clearInterval(ticker);
      }, intervalMs);
      state.ticker = ticker;
    });

    return report;
  }

  /** 收尾 survey：清定时器、算报告、resolve。内部方法。 */
  _finishSurvey(reason) {
    const s = this._survey;
    if (!s || s.finished) return;
    s.finished = true;
    s.reason = reason;
    if (s.ticker) { clearInterval(s.ticker); s.ticker = null; }
    for (const [, e] of s.seqs) { if (e.timer) clearTimeout(e.timer); }
    s.seqs.clear();

    // 未完成的 PING 视为丢弃（被 supersede / 尚未超时）
    if (reason === 'superseded') {
      const resolve = s.resolve;
      this._survey = null;
      if (resolve) resolve(null);
      return;
    }

    const decision = s.selector.decide();
    const m = s.selector.metrics();
    const path = decision ? decision.path : (this.route.path || 'unknown');
    const report = {
      rtt50: m.p50, rtt95: m.p95, mean: m.mean, jitter: m.jitter,
      loss: m.loss, sent: m.sent, lost: m.lost, samples: m.n,
      path, decision: decision ? decision.path : null,
      score: decision ? decision.score : null,
      reason: decision ? decision.reason : 'insufficient-samples',
      metrics: m,
      selector: s.selector.snapshot(),
      at: Date.now(),
    };
    this.route.rtt = m.p50 != null ? Math.round(m.p50) : this.route.rtt;
    this.route.lastSurvey = report;
    const resolve = s.resolve;
    this._survey = null;
    if (typeof this.onroute === 'function') this.onroute(this.getRouteSnapshot());
    if (resolve) resolve(report);
  }

  /** 供外部在 close() 时安全中止 survey。 */
  cancelRouteSurvey() { this._finishSurvey('superseded'); }

  // ---- 持续质量监控 / 换路交接（阶段 3Q） --------------------------------
  /** 记住最近一次关键控制帧，供换路后重放（新路径上重建对端关键状态）。 */
  _rememberControl(kind, payload) {
    this._lastControlSnapshot[kind] = payload;
    // 交接进行中：额外缓冲带序号的帧，commit 后按序重放。
    if (this.handoff && this.handoff.active) {
      this.handoff.bufferFrame(this._controlFrameSeq++, kind, payload);
    }
  }

  onQuality(cb) { this._qualityCbs.push(cb); return this; }
  onHandoff(cb) { this._handoffCbs.push(cb); return this; }

  /**
   * 启用对局期持续质量监控。心跳 PONG 喂 RTT、超时喂丢包；每 evaluateEveryMs
   * 评估一次，质量持续劣化过保护期 → emit onquality({action:'switch-turn'|...})。
   * 默认不自动换路（调用方决定是否执行），避免测试页/未接好的 UI 被切。
   */
  armRouteMonitor(opts = {}) {
    this.stopRouteMonitor();
    this.routeMonitor = new RouteMonitor({
      windowMs: opts.windowMs, evaluateEveryMs: opts.evaluateEveryMs,
      protectionMs: opts.protectionMs, degradeStreak: opts.degradeStreak,
      rttP95Threshold: opts.rttP95Threshold, lossThreshold: opts.lossThreshold,
      jitterThreshold: opts.jitterThreshold, hysteresis: opts.hysteresis,
      path: this.route.path,
    });
    this.handoff = new RouteHandoff({ role: this.role });
    this._monitorPending = new Map();     // seq -> sentAt（监控心跳超时检测）
    this._monitorTimeoutMs = Math.max(300, Number(opts.pingTimeoutMs) || 1500);
    this._autoSwitch = opts.autoSwitch === true;
    this.routeMonitor.arm(this.route.path);
    const period = this.routeMonitor.evaluateEveryMs;
    this._evalTimer = setInterval(() => this._evaluateQuality(), period);
    return this.routeMonitor.snapshot();
  }

  stopRouteMonitor() {
    if (this._evalTimer) { clearInterval(this._evalTimer); this._evalTimer = null; }
    if (this._monitorSweep) { clearInterval(this._monitorSweep); this._monitorSweep = null; }
    if (this.routeMonitor) this.routeMonitor.disarm();
  }

  /** 评估一次质量；必要时发建议。autoSwitch 时 host 自动发起换路交接。 */
  _evaluateQuality() {
    const m = this.routeMonitor;
    if (!m || !m.enabled) return null;
    // 先扫超时心跳：超过 pingTimeoutMs 未回 PONG → 记一次丢包。
    if (this._monitorPending) {
      const cutoff = nowMs() - this._monitorTimeoutMs;
      for (const [seq, sentAt] of this._monitorPending) {
        if (sentAt < cutoff) {
          this._monitorPending.delete(seq);
          this._pendingPing.delete(seq);
          m.noteLost();
        }
      }
    }
    const ev = m.evaluate();
    if (!ev) return null;
    this.emitQuality(ev);
    if (ev.action === 'switch-turn' || ev.action === 'switch-direct') {
      if (this.handoff && this.handoff.active) return ev; // 已有交接在进行，不并发
      if (this._autoSwitch && this.role === 'host') {
        const targetPath = ev.action === 'switch-turn' ? 'turn' : 'direct';
        this.proposeRouteSwitch(targetPath);
      }
    }
    return ev;
  }

  emitQuality(ev) {
    for (const cb of this._qualityCbs) { try { cb(ev); } catch (_) { /* noop */ } }
  }

  emitHandoff(snap) {
    for (const cb of this._handoffCbs) { try { cb(snap); } catch (_) { /* noop */ } }
  }

  /**
   * host 发起换路交接：提议 targetPath，switchFrame 给两端留出缓冲时间。
   * 发 ROUTE switch 帧；两端缓冲控制帧；client 回 accept → host commit →
   * 到 switchFrame 两端重建路径 + 重放缓冲帧。
   */
  proposeRouteSwitch(targetPath, switchFrame) {
    if (!this.handoff || this.handoff.active) return null;
    // switchFrame 默认给当前 rtt 折算 + 一拍缓冲；测试可显式传。
    const lead = this.rtt == null ? 24 : Math.max(12, Math.ceil(this.rtt / (1000 / 60)) + 12);
    const sf = Number.isFinite(switchFrame) ? (switchFrame | 0) : lead;
    const msg = this.handoff.propose(targetPath, sf);
    if (!msg) return null;
    this._sendRouteControl(msg);
    this.emitHandoff(this.handoff.snapshot());
    return msg;
  }

  /** 处理 ROUTE 文本帧：交给 handoff 状态机，必要时回帧 / 执行换路。 */
  async _onRouteControl(text) {
    if (!this.handoff) return;
    let msg;
    try { msg = JSON.parse(text); } catch { return; }
    if (msg.phase === 'switch') {
      // client 收到提议 → 回 accept。
      const reply = this.handoff.onMessage(msg);
      if (reply) { this._sendRouteControl(reply); this.emitHandoff(this.handoff.snapshot()); }
      return;
    }
    if (msg.phase === 'accept') {
      // host 收到 accept → 先在旧 DC 上发 commit 帧，再自己 commit + 重建。
      const reply = this.handoff.onMessage(msg);
      if (!reply) return;
      this._sendRouteControl(reply);
      const hostResult = this.handoff.commit();
      await this._performSwitch(hostResult);
      this.emitHandoff(this.handoff.snapshot());
      return;
    }
    if (msg.phase === 'commit') {
      // client 收到 commit → 执行换路（onMessage 内部完成 _commit 并返回重放帧）。
      const result = this.handoff.onMessage(msg);
      if (result) await this._performSwitch(result);
      this.emitHandoff(this.handoff.snapshot());
      return;
    }
  }

  _sendRouteControl(obj) { this.sendText(MSG.ROUTE, JSON.stringify(obj)); }

  /** 真正切路径（复用 3P 的重建），随后把交接期缓冲的控制帧按序重放。 */
  async _performSwitch(result) {
    if (!result) return;
    const { targetPath, replay } = result;
    const switched = this.applySurveyRoute(targetPath, { override: true });
    // 换路后 route 状态随之更新，监控切到新路径并重置保护期。
    if (this.routeMonitor) this.routeMonitor.onSwitched(targetPath);
    // DC 可能因重建而变化：等新 DC open 后再重放（短延迟轮询，最多 ~2s）。
    const waitOpen = async () => {
      for (let i = 0; i < 40; i++) {
        if (this.dc && this.dc.readyState === 'open') return true;
        await new Promise((r) => setTimeout(r, 50));
      }
      return false;
    };
    const opened = await waitOpen();
    if (opened && Array.isArray(replay)) this._replayControlFrames(replay);
    this._lastSwitch = { targetPath, switched, replayed: opened ? replay.length : 0, at: Date.now() };
    this.emitHandoff(this.handoff ? this.handoff.snapshot() : null);
    if (this.handoff) this.handoff.finish();
  }

  /** 在新 DC 上按序重放关键控制帧（恢复对端延迟/起跑帧/位置/选择）。 */
  _replayControlFrames(frames) {
    for (const f of frames) {
      if (!this.dc || this.dc.readyState !== 'open') break;
      if (f.kind === 'timing') this.sendTiming(f.payload.delay);
      else if (f.kind === 'gate') this.sendGate(f.payload.startFrame);
      else if (f.kind === 'pos') this.sendPos(f.payload.frame, f.payload.x, f.payload.y);
      else if (f.kind === 'loadout') this.sendText(MSG.LOADOUT, f.payload.text);
      else if (f.kind === 'digest' && f.payload) this.sendDigest(f.payload.frame, f.payload.hash, f.payload.objCount);
    }
  }

  getRouteMonitor() {
    return {
      monitor: this.routeMonitor ? this.routeMonitor.snapshot() : null,
      handoff: this.handoff ? this.handoff.snapshot() : null,
      lastSwitch: this._lastSwitch || null,
      autoSwitch: this._autoSwitch === true,
    };
  }

  sendAck(frame) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    this.dc.send(packInput(MSG.ACK, this.player, frame, this.player));
    return true;
  }

  /** host 下发统一延迟；bit0 置 1 表示权威值，client 应当采纳。 */
  sendTiming(delayFrames) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    // host 自己也遵守下发值，避免 getTiming() 出现 net/clock 两个不同的 delay。
    this.delayFrames = Math.max(0, delayFrames | 0);
    this.dc.send(packInput(MSG.TIMING, this.player, this.delayFrames, 1));
    return true;
  }

  /**
   * 阶段 3J：host 下发共同起跑墙钟帧号。两端在 wallFrame >= startFrame 前
   * 只喂输入不推进游戏，从同一帧号起跑，摘要/回滚帧号才真正对齐。
   * keys bit0=权威（client 必须采纳）。
   */
  sendGate(startFrame) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    this.dc.send(packInput(MSG.GATE, this.player, startFrame, 1));
    return true;
  }

  /** 定时 ping（毫秒）；返回停止函数。心跳只是采样，不影响游戏帧。 */
  startHeartbeat(intervalMs = 1000) {
    this.stopHeartbeat();
    this.ping();
    this._heartbeat = setInterval(() => this.ping(), Math.max(100, intervalMs | 0));
    return () => this.stopHeartbeat();
  }
  stopHeartbeat() {
    if (this._heartbeat) { clearInterval(this._heartbeat); this._heartbeat = null; }
  }

  /** 3O：一次性的路径采样（async），结果同时缓存在 this.route 供 UI 同步读取。 */
  async getRoute() {
    const pc = this.pc;
    if (!pc || typeof pc.getStats !== 'function') return this.getRouteSnapshot();
    try {
      const stats = await pc.getStats();
      let pair = null;
      if (stats && typeof stats.forEach === 'function') {
        stats.forEach((r) => {
          if (!r || r.type !== 'candidate-pair') return;
          if (!(r.state === 'succeeded' || r.nominated)) return;
          if (!pair || (r.priority || 0) > (pair.priority || 0)) pair = r;
        });
      }
      if (pair) {
        const local = stats.get ? stats.get(pair.localCandidateId) : null;
        const remote = stats.get ? stats.get(pair.remoteCandidateId) : null;
        const a = String((local && local.candidateType) || '');
        const b = String((remote && remote.candidateType) || '');
        const path = (a === 'relay' || b === 'relay') ? 'turn'
          : (a || b ? 'direct' : 'unknown');
        const rtt = pair.currentRoundTripTime != null
          ? Math.round(pair.currentRoundTripTime * 1000) : this.rtt;
        this.route = {
          ...this.route, path, candidateType: a || null, remoteCandidateType: b || null,
          rtt: Number.isFinite(rtt) ? rtt : null, reason: null, at: Date.now(),
        };
      }
    } catch (_) {
      this.route.reason = 'stats-unavailable';
    }
    this._routeCbs.forEach((cb) => cb(this.getRouteSnapshot()));
    return this.getRouteSnapshot();
  }

  getRouteSnapshot() { return { ...this.route, signalDown: this._signalDown }; }

  onRoute(cb) { this._routeCbs.push(cb); return this; }

  /** 周期性刷新路径（eagler 每秒 getStats；这里 5s 足够 HUD 用）。 */
  startRouteProbe(intervalMs = 5000) {
    this.stopRouteProbe();
    void this.getRoute();
    this._routeTimer = setInterval(() => void this.getRoute(), Math.max(500, intervalMs | 0));
    return () => this.stopRouteProbe();
  }

  stopRouteProbe() {
    if (this._routeTimer) { clearInterval(this._routeTimer); this._routeTimer = null; }
  }

  /** 给网页 UI 的时序快照 */
  getTiming() {
    return {
      rtt: this.rtt,
      samples: this._rttSamples.length,
      remoteAck: this.remoteAck,
      delayFrames: this.delayFrames,
      suggestedDelay: this.rtt == null ? 2 : Math.max(2, Math.ceil((this.rtt / 2) / (1000 / 60)) + 1),
      route: this.getRouteSnapshot(),
    };
  }

  sendText(type, text) {
    if (!this.dc || this.dc.readyState !== 'open') { this.stats.dropped++; return false; }
    this.dc.send(packText(type, this.player, text));
    return true;
  }

  /** 发送本端选择（自机/系统/P1P2 的 JSON），对端 UI 可直接渲染 roster */
  sendLoadout(obj) { return this.sendText(MSG.LOADOUT, JSON.stringify(obj)); }
  sendReady(ready) { return this.sendText(MSG.READY, JSON.stringify({ ready: !!ready })); }

  sendInput(frame, keysByte) {
    if (!this.dc || this.dc.readyState !== 'open') { this.stats.dropped++; return false; }
    this.dc.send(packInput(MSG.INPUT, this.player, frame, keysByte));
    this.stats.sent++;
    return true;
  }

  /** 阶段 3G：每帧把本端 player 世界坐标发给对端，供其实体位置同步。 */
  sendPos(frame, x, y) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    this.dc.send(packPos(this.player, frame, x, y));
    return true;
  }

  /** 阶段 3I：每帧把本端状态摘要发给对端，供分叉检测/回滚基线比对。 */
  sendDigest(frame, hash, objCount) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    this.dc.send(packDigest(this.player, frame, hash, objCount));
    return true;
  }

  sendSeed(seed) {
    if (!this.dc || this.dc.readyState !== 'open') return false;
    const buf = new ArrayBuffer(10);
    const v = new DataView(buf);
    v.setUint8(0, MSG.SEED); v.setUint8(1, this.player);
    v.setUint32(6, seed >>> 0, true);
    this.dc.send(buf);
    return true;
  }

  close() {
    this.cancelRouteSurvey();
    this.stopRouteMonitor();
    this.stopHeartbeat();
    this.stopRouteProbe();
    if (this._connectTimer) { clearInterval(this._connectTimer); this._connectTimer = null; }
    if (this.dc && typeof this.dc.close === 'function') this.dc.close();
    this.dc = null;
    if (this.pc) this.pc.close();
    this.ready = false;
  }
}
