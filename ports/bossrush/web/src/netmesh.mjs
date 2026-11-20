/**
 * NetMesh —— 3T：跨机 3~4 人共斗的全互联（Full Mesh）网络层。
 *
 * 拓扑：房间内 N 个玩家两两各建一条 WebRTC DataChannel（4 人 = 6 条）。
 * 为什么不用 host 星型：弹幕游戏对延迟敏感，星型下 P2→host→P3 多一跳；
 * mesh 下每个玩家的 1 字节输入直达其余 N-1 人，也天然对齐 eagler 的
 * confirmed-input lockstep（每端对每帧都持有全部席位的真实输入）。
 *
 * 设计约束：
 *  - **复用 NetPeer 不重写**：每条链路仍是成熟的 3C~3Q 单 peer（选路/换路/
 *    PING/ACK/DIGEST/GATE 全保留）。NetPeer 3T 泛化了 remotePlayer/initiator，
 *    故这里只做「一对多」的编排。
 *  - **对外接口与 NetPeer 同形**：multiplayer.mjs 的门面代码 `this.net.*` 不改
 *    一行即可切 mesh；回调签名也保持 NetPeer 的多参数风格（frame, who, keys）。
 *  - **共享一条信令 WebSocket**：靠 NetPeer._onSignal 的 to+from 双过滤隔离 SDP。
 *  - **offer 由小号 player 发起**：NetPeer.initiator 默认 player<remotePlayer，
 *    确定性，不需要 host 中央调度，也不会两端同时 offer（glare）。
 *  - 1v1 仍走单 NetPeer（门面 maxPlayers<=2），本模块不介入。
 */

import { NetPeer } from './netcode.mjs';

const CB_NAMES = [
  'open', 'input', 'loadout', 'ready', 'ack', 'timing', 'pos', 'digest', 'gate',
  'quality', 'handoff', 'route', 'state', 'error', 'close', 'ice',
];

export class NetMesh {
  constructor(opts = {}) {
    this.selfPlayer = Number(opts.player) | 0;
    this.room = opts.room || 'bro-default';
    this.signal = opts.signal || null;
    this.iceServers = opts.iceServers || [];
    this.preferTurn = opts.preferTurn === true;
    this.connectTimeoutMs = opts.connectTimeoutMs;
    this.netFactory = opts.netFactory || null;     // 单测注入假 NetPeer
    this.maxPlayers = Number(opts.maxPlayers) > 0 ? (Number(opts.maxPlayers) | 0) : 4;

    /** @type {Map<number, NetPeer>} remotePlayer -> peer */
    this.peers = new Map();
    this.started = false;
    this.routeLocked = false;                      // 与 NetPeer 同名字段，门面会写
    this.stats = { sent: 0, recv: 0, dropped: 0, lastFrame: -1 };
    this._lastSurvey = null;                       // 3U：最近一次多链路选路报告

    this._cbs = {};
    for (const n of CB_NAMES) this._cbs[n] = [];
    this._onstate = null;
    this._onice = null;
  }

  // ---- 事件（签名与 NetPeer 的 onXxx 完全一致） -------------------------
  on(name, cb) { (this._cbs[name] ||= []).push(cb); return this; }
  _emit(name, ...args) {
    for (const cb of (this._cbs[name] || [])) {
      try { cb(...args); } catch (e) { console.error('[mesh] callback ' + name, e); }
    }
  }
  onOpen(cb) { return this.on('open', cb); }
  onInput(cb) { return this.on('input', cb); }
  onLoadout(cb) { return this.on('loadout', cb); }
  onReady(cb) { return this.on('ready', cb); }
  onAck(cb) { return this.on('ack', cb); }
  onTiming(cb) { return this.on('timing', cb); }
  onPos(cb) { return this.on('pos', cb); }
  onDigest(cb) { return this.on('digest', cb); }
  onGate(cb) { return this.on('gate', cb); }
  onQuality(cb) { return this.on('quality', cb); }
  onHandoff(cb) { return this.on('handoff', cb); }
  onRoute(cb) { return this.on('route', cb); }
  onError(cb) { return this.on('error', cb); }
  _safe(fn) { try { fn(); } catch (e) { console.error('[mesh] hook', e); } }

  // ---- 席位编排 ----------------------------------------------------------
  /**
   * 依据信令下发的 roster 配置链路。增量：只新建新增席位、关闭离开席位。
   * 返回当前链路数。注意只建实例不 start —— start() 由门面在时机合适时调。
   */
  configure(roster, opts = {}) {
    const wanted = (Array.isArray(roster) ? roster : [])
      .map((p) => Number(p) | 0)
      .filter((p) => p >= 0 && p < this.maxPlayers && p !== this.selfPlayer);
    if (opts.expect && wanted.length !== (Number(opts.expect) | 0)) return this.peers.size;

    for (const p of wanted) if (!this.peers.has(p)) this._createPeer(p);
    for (const [p, peer] of [...this.peers]) {
      if (wanted.includes(p)) continue;
      peer.close();
      this.peers.delete(p);
      this._emit('close', { player: p });
    }
    return this.peers.size;
  }

  _createPeer(remotePlayer) {
    const opts = {
      role: this.selfPlayer === 0 ? 'host' : 'client',  // 游戏权威：种子/TIMING/GATE
      room: this.room,
      player: this.selfPlayer,
      remotePlayer,                                     // 3T：链路对端（offer 主导权由此推导）
      signal: this.signal,
      iceServers: this.iceServers,
      preferTurn: this.preferTurn,
      connectTimeoutMs: this.connectTimeoutMs,
    };
    const peer = this.netFactory ? this.netFactory(opts) : new NetPeer(opts);
    this._wirePeer(peer, remotePlayer);
    this.peers.set(remotePlayer, peer);
    return peer;
  }

  /** 把每条链路的事件汇聚成「一条 net」暴露给门面。 */
  _wirePeer(peer, remote) {
    peer.onOpen(() => this._emit('open', { player: remote }));
    peer.onInput((frame, who, keys) => {
      this.stats.recv++; this.stats.lastFrame = frame;
      this._emit('input', frame, who, keys);
    });
    peer.onLoadout((who, text) => this._emit('loadout', who, text));
    peer.onReady((who, text) => this._emit('ready', who, text));
    // ACK 聚合：本地输入要被**所有**远端确认才算确认（eagler acknowledged-through-all）
    if (typeof peer.onAck === 'function') {
      peer.onAck((frame, remoteAck) => this._emit('ack', frame, this.remoteAck));
    }
    if (typeof peer.onTiming === 'function') {
      peer.onTiming((delay, keys) => this._emit('timing', delay, keys));
    }
    if (typeof peer.onPos === 'function') {
      peer.onPos((frame, who, x, y) => this._emit('pos', frame, who, x, y));
    }
    if (typeof peer.onGate === 'function') {
      peer.onGate((startFrame, who, keys) => this._emit('gate', startFrame, who, keys));
    }
    if (typeof peer.onDigest === 'function') {
      peer.onDigest((frame, who, hash, objCount) => this._emit('digest', frame, who, hash, objCount));
    }
    if (typeof peer.onQuality === 'function') peer.onQuality((ev) => this._emit('quality', ev));
    if (typeof peer.onHandoff === 'function') peer.onHandoff((snap) => this._emit('handoff', snap));
    if (typeof peer.onRoute === 'function') peer.onRoute((snap) => this._emit('route', snap));
    peer.onstate = (s) => { if (this._onstate) this._safe(() => this._onstate(s, remote)); };
    peer.onice = (s) => { if (this._onice) this._safe(() => this._onice(s, remote)); };
  }

  set onstate(cb) {
    this._onstate = cb;
    for (const p of this.peers.values()) p.onstate = (s) => this._safe(() => cb(s, p.remotePlayer));
  }
  get onstate() { return this._onstate; }
  set onice(cb) {
    this._onice = cb;
    for (const p of this.peers.values()) p.onice = (s) => this._safe(() => cb(s, p.remotePlayer));
  }
  get onice() { return this._onice; }

  /** 所有链路都 OPEN 才算 mesh 就绪（空 mesh 不算）。 */
  get ready() {
    if (!this.peers.size) return false;
    for (const p of this.peers.values()) if (!p.ready) return false;
    return true;
  }
  get remoteAck() {
    let min = Infinity;
    for (const p of this.peers.values()) min = Math.min(min, Number(p.remoteAck));
    return min === Infinity ? -1 : min;
  }
  get pc() { return this.peers.values().next().value?.pc || null; }
  get dc() { return this.peers.values().next().value?.dc || null; }
  get remoteSeed() { return this.peers.values().next().value?.remoteSeed ?? null; }

  /** 聚合路由：任一链路走 TURN 就报 turn，RTT 取最差链路。 */
  get route() {
    const list = [...this.peers.values()].map((p) => p.route || {});
    const paths = list.map((r) => r.path).filter((x) => x && x !== 'unknown');
    const path = paths.includes('turn') ? 'turn' : (paths[0] || 'unknown');
    const rtts = list.map((r) => Number(r.rtt)).filter((x) => Number.isFinite(x));
    return {
      path,
      mode: list.some((r) => r.mode === 'turn') ? 'turn' : 'auto',
      rtt: rtts.length ? Math.max(...rtts) : null,
      lastSurvey: this._lastSurvey,
      peers: Object.fromEntries([...this.peers].map(([p, peer]) => [p, peer.getRouteSnapshot?.() || peer.route])),
    };
  }
  getRouteSnapshot() { return this.route; }

  // ---- 启动 / 关闭 -------------------------------------------------------
  async start() {
    if (this.started) return this;
    this.started = true;
    const all = [...this.peers.entries()];
    const results = await Promise.all(all.map(async ([p, peer]) => {
      try { await peer.start(); return { player: p, ok: true }; }
      catch (e) { this._emit('error', { player: p, error: e }); return { player: p, ok: false, error: String(e) }; }
    }));
    this._emit('state', { kind: 'started', results, ready: this.ready });
    return this;
  }

  close() {
    let n = 0;
    for (const peer of this.peers.values()) {
      try { peer.close(); n++; } catch (_) { /* noop */ }
    }
    this.peers.clear();
    this.started = false;
    this._emit('close', { player: null, closed: n });
    return n > 0;
  }
  getPeer(player) { return this.peers.get(Number(player) | 0) || null; }
  getPlayers() { return [...this.peers.keys()]; }
  get peerCount() { return this.peers.size; }

  // ---- 广播（对应 NetPeer 的每个 sendXxx） -------------------------------
  _broadcast(fn, ...args) {
    let ok = 0;
    for (const peer of this.peers.values()) {
      try { if (peer[fn](...args) !== false) ok++; }
      catch (e) { this._emit('error', { player: peer.remotePlayer, error: e }); }
    }
    this.stats.sent += ok;
    return ok;
  }
  sendInput(frame, keys) { return this._broadcast('sendInput', frame, keys); }
  sendPos(frame, x, y) { return this._broadcast('sendPos', frame, x, y); }
  sendDigest(frame, hash, objCount) { return this._broadcast('sendDigest', frame, hash, objCount); }
  sendText(type, text) { return this._broadcast('sendText', type, text); }
  sendLoadout(obj) { return this._broadcast('sendLoadout', obj); }
  sendReady(ready) { return this._broadcast('sendReady', ready); }
  sendSeed(seed) { return this._broadcast('sendSeed', seed); }
  sendTiming(delay) { return this._broadcast('sendTiming', delay); }
  sendGate(startFrame) { return this._broadcast('sendGate', startFrame); }
  startHeartbeat(ms) { return this._broadcast('startHeartbeat', ms); }
  stopHeartbeat() { return this._broadcast('stopHeartbeat'); }
  armRouteMonitor(opts) { return this._broadcast('armRouteMonitor', opts); }
  stopRouteMonitor() { return this._broadcast('stopRouteMonitor'); }
  proposeRouteSwitch(path, frame) { return this._broadcast('proposeRouteSwitch', path, frame); }

  /**
   * 3U：聚合全部链路的持续监控。顶层字段取首条链路（HUD 旧读法不破坏），
   * perPeer 给出每条链路自己的监控快照。
   */
  getRouteMonitor() {
    const perPeer = {};
    let first = null;
    for (const [player, peer] of this.peers) {
      const snap = typeof peer.getRouteMonitor === 'function' ? peer.getRouteMonitor() : null;
      if (!snap) continue;
      if (!first) first = snap;
      perPeer[player] = snap;
    }
    if (!first) return null;
    return { ...first, perPeer, peers: Object.keys(perPeer).map(Number) };
  }
  /**
   * 必须与 NetPeer.getTiming() 同形。空 mesh（单机演示页 / roster 未到）时
   * 若返回 null，门面 getFrontiers() 读 .remoteAck 会抛错并中断整页初始化。
   */
  getTiming() {
    const peer = this.peers.values().next().value;
    const base = peer && typeof peer.getTiming === 'function' ? peer.getTiming() : null;
    if (base) return { ...base, remoteAck: this.remoteAck };
    return {
      rtt: null, samples: 0, remoteAck: this.remoteAck,
      delayFrames: 0, suggestedDelay: 2,
    };
  }
  async getRoute() { return this.route; }

  /**
   * 3U：开局选路测量——**每条链路独立并发 survey**。
   *
   * 为什么不能只测主链路：全互联下 P0-P1 可能直连最优、P0-P2 却因 NAT 类型
   * 必须走 TURN。一条链路测出来的 path 套用到全部链路是错的。每条 NetPeer 的
   * PING/PONG 走各自 DataChannel、各自 _pendingPing/_survey，天然隔离，可并发。
   *
   * 返回：
   *  - 单链路：直接返回该 NetPeer 报告（形状与 1v1 完全一致，门面零改动）。
   *  - 多链路：聚合报告（顶层字段供门面 applyDelay/applyRouteDecision 使用）
   *    带 perPeer={ [player]: report }，每个 player 拿自己的 path/rtt/loss。
   */
  async runRouteSurvey(opts = {}) {
    const all = [...this.peers.entries()].sort((a, b) => a[0] - b[0]);
    if (!all.length) { this._lastSurvey = null; return null; }
    if (all.length === 1) {
      const only = await all[0][1].runRouteSurvey(opts);
      this._lastSurvey = only;
      return only;
    }

    const onProgress = typeof opts.onProgress === 'function' ? opts.onProgress : null;
    const perOpts = { ...opts };
    delete perOpts.onProgress;
    const results = await Promise.all(all.map(async ([player, peer]) => {
      try {
        const report = await peer.runRouteSurvey({
          ...perOpts,
          // 第 5 个参数标出是哪条链路；旧 onProgress(done,total,metrics,rtt) 签名不破坏。
          onProgress: onProgress
            ? (done, total, metrics, rtt) => onProgress(done, total, metrics, rtt, player)
            : undefined,
        });
        return { player, report };
      } catch (e) {
        this._emit('error', { player, error: e });
        return { player, report: null };
      }
    }));

    const perPeer = {};
    for (const { player, report } of results) if (report) perPeer[player] = report;
    const valid = results.map((x) => x.report).filter(Boolean);
    if (!valid.length) { this._lastSurvey = null; return null; }

    const maxOf = (k) => {
      const xs = valid.map((r) => Number(r[k])).filter((x) => Number.isFinite(x));
      return xs.length ? Math.max(...xs) : null;
    };
    const sumOf = (k) => valid.reduce((a, r) => a + (Number(r[k]) || 0), 0);
    const anyTurn = valid.some((r) => r.path === 'turn');
    // 聚合 path 保守取「任一链路要 TURN 就整体 turn」；真正的逐链路决策在 perPeer。
    const aggregate = {
      path: anyTurn ? 'turn' : (valid[0].path || 'unknown'),
      mode: anyTurn ? 'turn' : 'auto',
      rtt50: maxOf('rtt50'), rtt95: maxOf('rtt95'),
      mean: maxOf('mean'), jitter: maxOf('jitter'), loss: maxOf('loss'),
      sent: sumOf('sent'), lost: sumOf('lost'), samples: sumOf('samples'),
      perPeer,
      peers: Object.keys(perPeer).map(Number),
      at: Date.now(),
      _mesh: true,
    };
    this._lastSurvey = aggregate;
    return aggregate;
  }
  cancelRouteSurvey() { return this._broadcast('cancelRouteSurvey'); }

  /**
   * 3U：按测量结果落路径。
   *  - path 为字符串：兼容旧语义，广播给所有链路。
   *  - path 为 { [player]: 'direct'|'turn' }：只给对应链路应用自己的决策。
   *    mesh.routeLocked 时整体拒绝（开闸进入对局后不再改路）。
   */
  applySurveyRoute(path, opts = {}) {
    if (this.routeLocked && opts.override !== true) return false;
    if (path && typeof path === 'object') {
      let switched = 0;
      for (const [player, peer] of this.peers) {
        const want = path[player];
        if (want == null) continue;
        try { if (peer.applySurveyRoute(want, opts) !== false) switched++; }
        catch (e) { this._emit('error', { player, error: e }); }
      }
      return switched > 0;
    }
    return this._broadcast('applySurveyRoute', path, opts);
  }
}

export default NetMesh;
