/**
 * BRO 联机输入缓冲 + 确定性延迟时钟（阶段 3E）
 *
 * 为什么需要它：WebRTC DataChannel 虽可靠有序，但「到达时刻」和「逻辑帧号」
 * 不是一回事。网络抖动下，第 N 帧的输入可能晚于第 N+2 帧的本地逻辑帧到达。
 * 若渲染循环直接拿「最新收到的按键」当本帧输入，两端会算出不同结果。
 *
 * 本模块把输入按「帧号」而非「到达顺序」索引，并提供：
 *   - confirmed(player)  连续可用到哪一帧（空洞不会误判为已确认）
 *   - lookup(player,f)   缺帧时用上一帧输入预测（hold-last-input）
 *   - InputClock         把「当前墙钟帧」映射到「应执行的逻辑帧」= frame - delay
 *   - digest(f)          双端同一帧输入的指纹，给 e2e 验证帧同步用
 *
 * 边界（诚实）：预测错了这里只统计 corrections，**不回滚**。回滚需要
 * task 协程 + 状态快照，是后续阶段的事。当前 delay 足够大时 corrections 应为 0。
 */

/**
 * 3W：STG 感知的自适应输入预测器工厂。
 *
 * 返回签名 `(buffer, player, frame) => keysByte` 的纯函数，供 InputBuffer 注入。
 * 核心洞察：hold-last 对「持续键」（方向/慢速/射击）是最优的——切换帧能立即跟上；
 * 但对「脉冲键」（符卡 spell=2 / 特技 special=1）每次都多猜错一帧：
 * 玩家短按 1 帧就松开，hold-last 在松开帧仍预测「按住」，真值到达后被记 correction。
 * 自适应策略：脉冲键若检测到「上一帧为 1 且连续按住 ≤ pulseHold 帧」，预测松开（清 bit）。
 *
 * 确定性：只依赖已到达的历史真值，不含时间/随机；两端用同一函数 → 纠错行为一致。
 * 预测值永不改变最终状态（真值到达会覆盖并重放），只影响纠错/回滚次数。
 */
export function makeAdaptivePredict(opts = {}) {
  const pulseMask = Number.isFinite(opts.pulseMask) ? (opts.pulseMask & 0xff) : 0x03; // spell|special
  const pulseHold = Number.isFinite(opts.pulseHold) ? Math.max(1, opts.pulseHold | 0) : 1;
  return function adaptivePredict(buffer, player, frame) {
    const p = player | 0, f = frame | 0;
    const floor = Math.max(0, buffer.lowest[p] || 0);
    // 基线：hold-last，从 f-1 往前找最近真值。
    let keys = 0;
    for (let n = f - 1; n >= floor; n--) {
      const v = buffer.get(p, n);
      if (v !== null) { keys = v; break; }
    }
    if (!pulseMask || keys === 0) return keys & 0xff;
    // 脉冲键：数从 f-1 往前的连续按住长度（遇空洞即停，保守不猜）。
    for (let bit = 1; bit <= pulseMask; bit <<= 1) {
      if (!(bit & pulseMask) || !(keys & bit)) continue;
      let run = 0;
      for (let n = f - 1; n >= floor; n--) {
        const v = buffer.get(p, n);
        if (v === null || !(v & bit)) break;
        run++;
      }
      if (run > 0 && run <= pulseHold) keys &= ~bit; // 短按 → 预测松开
    }
    return keys & 0xff;
  };
}

export class InputBuffer {
  constructor(opts = {}) {
    this.keep = Number(opts.keep) || 180;          // 保留帧数（3 秒 @60fps）
    this.players = Number(opts.players) || 2;
    this.startFrame = Number.isFinite(opts.startFrame) ? (opts.startFrame | 0) : 0; // 双方首帧编号（BRO 从 1 起）
    // 3W：可插拔预测策略 (buffer, player, frame)=>keys；缺省保持原 hold-last 行为。
    this.predict = typeof opts.predict === 'function' ? opts.predict : null;
    this.history = [];                              // history[p] = Map<frame, keys>
    this.predictions = [];                          // predictions[p] = Map<frame, keys>
    this.confirmedAt = [];                          // 连续确认指针
    this.lowest = [];                               // 已清理的最小帧号
    this.stats = { pushed: 0, corrections: 0, predicted: 0, evicted: 0, conflicts: 0 };
    this.correctionEvents = [];              // 3K：待消费的纠错帧（预测被真值推翻）
    this.reset();
  }

  reset() {
    this.history = [];
    this.predictions = [];
    this.confirmedAt = [];
    this.lowest = [];
    for (let p = 0; p < this.players; p++) {
      this.history[p] = new Map();
      this.predictions[p] = new Map();
      this.confirmedAt[p] = this.startFrame - 1;
      this.lowest[p] = 0;
    }
    this.stats = { pushed: 0, corrections: 0, predicted: 0, evicted: 0, conflicts: 0 };
    this.correctionEvents = [];
  }

  /** 写入一帧真实输入。重复写同帧不同值记为冲突（输入流本应不可变）。 */
  push(player, frame, keys) {
    const p = player | 0, f = Number(frame) | 0, k = Number(keys) & 0xff;
    if (p < 0 || p >= this.players || f < 0) return false;
    const h = this.history[p];
    if (h.has(f) && h.get(f) !== k) this.stats.conflicts++;

    // 若曾对该帧做过预测，记录是否猜错（回滚阶段要用这个指标）。
    const pred = this.predictions[p].get(f);
    if (pred !== undefined && pred !== k) {
      this.stats.corrections++;
      this.correctionEvents.push({ player: p, frame: f, from: pred, to: k });
    }
    if (pred !== undefined) this.predictions[p].delete(f);

    h.set(f, k);
    this.stats.pushed++;

    // 推进连续确认指针：从当前值往后走，遇到空洞即停。
    let c = this.confirmedAt[p];
    while (h.has(c + 1)) c++;
    this.confirmedAt[p] = c;

    // 淘汰过老帧，保持内存有界。
    const cutoff = f - this.keep;
    if (cutoff > this.lowest[p]) {
      for (const key of h.keys()) if (key < cutoff) { h.delete(key); this.evictPrediction(p, key); this.stats.evicted++; }
      this.lowest[p] = cutoff;
    }
    return true;
  }

  /** 3K：取出并清空待消费的纠错事件（回滚消费者用）。 */
  takeCorrections() {
    const out = this.correctionEvents;
    this.correctionEvents = [];
    return out;
  }

  evictPrediction(p, frame) {
    const pr = this.predictions[p];
    if (pr) pr.delete(frame);
  }

  /** 真实输入；没有返回 null（不要用 0 代替，0 是「全松手」的合法值）。 */
  get(player, frame) {
    const h = this.history[player | 0];
    if (!h) return null;
    const v = h.get(frame | 0);
    return v === undefined ? null : v;
  }

  has(player, frame) { return this.get(player, frame) !== null; }

  /** 连续确认到的最大帧号（空洞之前的最后一帧）。 */
  confirmed(player) { return this.confirmedAt[player | 0] ?? -1; }

  /**
   * 3N：所有席位连续确认前沿的最小值（对齐 eagler
   * `RollbackCore::ConfirmedThroughAllRemotes()`）。输出提交、日志退休
   * 都用这个水位，而不是「本地模拟到哪」——本地跑得快不能当确认。
   */
  confirmedThroughAllRemotes() {
    let min = Infinity;
    for (let p = 0; p < this.players; p++) min = Math.min(min, this.confirmed(p));
    return min === Infinity ? -1 : min;
  }

  /**
   * 3N：本端输入被所有远端 ACK 的前沿（对齐 eagler
   * `AcknowledgedLocalThroughAllRemotes()` 的本端视角）。ACK 由 NetPeer 提供。
   */
  acknowledgedLocalThroughAllRemotes(remoteAck = -1) {
    const v = Number(remoteAck);
    return Number.isFinite(v) ? Math.max(-1, v | 0) : -1;
  }

  /** 3N：预测前沿——从 playFrame 往前，最早仍被预测（未拿到真值）的帧。 */
  predictedFrontier(playFrame = -1) {
    const f = Number(playFrame) | 0;
    if (f < 0) return -1;
    let start = -1;
    // 3T：原先只看 player 0/1，3~4 人时下界/完整性都会算错。
    let floor = 0;
    for (let p = 0; p < this.players; p++) floor = Math.max(floor, this.lowest[p] || 0);
    for (let n = f; n >= floor; n--) {
      let guessed = false, complete = true;
      for (let p = 0; p < this.players; p++) {
        if (this.predictions[p]?.has(n)) guessed = true;
        if (!this.has(p, n)) complete = false;
      }
      if (guessed) start = n;
      else if (complete) break;   // 所有席位都有真实输入 → 预测区间到此为止
    }
    return start;
  }

  /** 区间内缺失的帧号，供 UI 显示与决定是否继续等待。 */
  missing(player, from, to) {
    const out = [];
    for (let f = Math.max(0, from | 0); f <= (to | 0); f++) {
      if (!this.has(player, f)) out.push(f);
    }
    return out;
  }

  /**
   * 取该帧输入；缺帧时用「上一帧真实输入」预测并缓存。
   * 返回 { keys, predicted, have } —— have 表示是否真实到达。
   */
  lookup(player, frame) {
    const p = player | 0, f = frame | 0;
    const real = this.get(p, f);
    if (real !== null) return { keys: real, predicted: false, have: true };
    let keys;
    if (this.predict) {
      keys = this.predict(this, p, f) & 0xff;
    } else {
      let last = null;
      for (let n = f - 1; n >= Math.max(0, this.lowest[p]); n--) {
        const v = this.get(p, n);
        if (v !== null) { last = v; break; }
      }
      keys = last === null ? 0 : last;
    }
    if (!this.predictions[p].has(f)) {
      this.predictions[p].set(f, keys);
      this.stats.predicted++;
    }
    return { keys, predicted: true, have: false };
  }

  /** 某帧双端输入的确定性指纹（e2e 用来确认两端执行的是同一帧数据）。 */
  digest(frame) {
    const f = frame | 0;
    const parts = [];
    for (let p = 0; p < this.players; p++) {
      const v = this.get(p, f);
      parts.push(v === null ? '-' : v.toString(16).padStart(2, '0'));
    }
    return f + ':' + parts.join(',');
  }

  /** 给网页 UI 的状态快照（3T：pending/confirmed 覆盖全部席位） */
  snapshot(nowFrame = 0) {
    const confirmed = [];
    let pending = 0;
    const upto = Math.max(0, (nowFrame | 0) - 1);
    for (let p = 0; p < this.players; p++) {
      confirmed.push(this.confirmed(p));
      pending += this.missing(p, 0, upto).length;
    }
    return {
      confirmed,
      nowFrame: nowFrame | 0,
      pending,
      ...this.stats,
    };
  }
}

/**
 * 输入延迟时钟：把「墙钟帧」映射到「应执行的逻辑帧」。
 *
 * delay 由两端协商（host 下发，开局后锁定）；两端必须一致，否则帧号错位。
 * 执行帧 = wallFrame - delay；若该帧双端输入都到齐则执行，否则用预测补帧。
 */
export class InputClock {
  constructor(opts = {}) {
    this.buffer = opts.buffer || new InputBuffer(opts);
    this.localPlayer = Number(opts.localPlayer) | 0;
    this.delay = Math.max(0, Number(opts.delay) | 0);
    // 3R：参与人数（默认 2 保持 1v1 旧行为；多人时门面传 maxPlayers）。
    this.players = Number(opts.players) > 0 ? (Number(opts.players) | 0) : 2;
    this.minDelay = 0;
    this.maxDelay = 30;
    this.lastWallFrame = 0;
  }

  setDelay(n) {
    const v = Math.max(this.minDelay, Math.min(this.maxDelay, Number(n) | 0));
    const changed = v !== this.delay;
    this.delay = v;
    return changed;
  }

  playFrame(wallFrame) { return Math.max(0, (Number(wallFrame) | 0) - this.delay); }

  /** 所有参与端该帧是否都有真实输入（无预测）；1v1 时等价原来的双端判断 */
  ready(frame) {
    const f = Number(frame) | 0;
    for (let p = 0; p < this.players; p++) {
      if (!this.buffer.has(p, f)) return false;
    }
    return true;
  }

  /**
   * 采集一个墙钟帧应执行的输入。
   * 返回 { frame, inputs:[k0,k1], predicted:[bool,bool], ready, localKeys }
   */
  collect(wallFrame) {
    const f = this.playFrame(wallFrame);
    this.lastWallFrame = Number(wallFrame) | 0;
    const inputs = [], predicted = [];
    let localKeys = 0;
    for (let p = 0; p < this.players; p++) {
      const r = this.buffer.lookup(p, f);
      inputs[p] = r.keys;
      predicted[p] = r.predicted;
      if (p === this.localPlayer && this.buffer.has(p, f)) localKeys = r.keys;
    }
    return { frame: f, inputs, predicted, ready: !predicted.includes(true), localKeys };
  }

  snapshot(wallFrame = 0) {
    return { delay: this.delay, localPlayer: this.localPlayer,
      playFrame: this.playFrame(wallFrame), ...this.buffer.snapshot(wallFrame) };
  }
}
