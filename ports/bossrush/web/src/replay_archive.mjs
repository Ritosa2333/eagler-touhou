/**
 * replay_archive.mjs — 阶段 3W/3X：关卡级重放的输入流长归档 + 周期锚点
 *
 * 要解决的问题（PROBE_3M §5 边界）：
 *   3M 的 fullReplayRollback(target) 从帧 1 重放到 target，输入靠 InputBuffer.get()，
 *   但 InputBuffer.keep=180 会淘汰更早的帧。长局跑到帧 1000 时要回滚到帧 950，
 *   帧 1..820 的输入已被淘汰 → 无法重放。这就是「180 帧窗口」。
 *
 * 本模块不试图从中间帧恢复对象图（Lua 5.1 task 协程无法克隆，3M 已论证），
 * 而是把「真值输入流」单独存一份更长的归档：
 *   - 每帧各槽的真值输入字节（来自本地 push 与远端 onInput 的真值，不含预测）；
 *   - 周期性锚点：每 anchorEvery 帧记一个 {frame, digest}，标记「到该帧为止的
 *     输入流已完整可达」，供回滚选起点与完整性诊断；
 *   - 有界淘汰 keep 帧（默认 3600 = 60 秒 @60fps，远大于 180），内存可控。
 *
 * fullReplayRollback 改为优先从本归档取 1..target 的输入，从而解掉 180 帧窗口。
 * 注意：归档只保证「输入流可取」，不缩短重放时间——重放 N 帧仍真跑 N 帧（~3.2ms/帧）。
 */

export class ReplayArchive {
  constructor(opts = {}) {
    this.players = Number(opts.players) > 0 ? (Number(opts.players) | 0) : 2;
    this.keep = Number(opts.keep) > 0 ? (Number(opts.keep) | 0) : 3600;         // 60 秒
    this.anchorEvery = Number(opts.anchorEvery) > 0 ? (Number(opts.anchorEvery) | 0) : 300; // 5 秒
    this.onAnchor = typeof opts.onAnchor === 'function' ? opts.onAnchor : null;
    this.reset();
  }

  reset() {
    // frames[f] = Int 数组（每槽 1 字节，索引=slot）。用 Map 保持稀疏帧友好。
    this.frames = new Map();
    this.anchors = [];
    this.minFrame = Infinity;
    this.maxFrame = -1;
    this.stats = { recorded: 0, anchorCount: 0, evicted: 0, gaps: 0 };
  }

  /** 记录一帧的真值输入。inputs[p] = keysByte（缺省槽记 0）。 */
  record(frame, inputs) {
    const f = Number(frame) | 0;
    if (f < 0) return false;
    const row = new Array(this.players);
    for (let p = 0; p < this.players; p++) {
      const v = inputs && (Number(inputs[p]) & 0xff);
      row[p] = Number.isFinite(v) ? v : 0;
    }
    this.frames.set(f, row);
    this.stats.recorded++;
    if (f < this.minFrame) this.minFrame = f;
    if (f > this.maxFrame) this.maxFrame = f;
    // 周期锚点：按帧号整数倍打点，避免受到达顺序影响。
    if (this.anchorEvery > 0 && f > 0 && f % this.anchorEvery === 0) {
      if (!this.anchors.some(a => a.frame === f)) {
        const a = { frame: f, digest: null, at: Date.now() };
        this.anchors.push(a);
        this.anchors.sort((x, y) => x.frame - y.frame);
        this.stats.anchorCount++;
        if (this.onAnchor) {
          try { a.digest = this.onAnchor(f); } catch (_) { /* 摘要源缺失不致命 */ }
        }
      }
    }
    this._evict();
    return true;
  }

  /** 淘汰超出 keep 窗口的旧帧与旧锚点，内存有界。 */
  _evict() {
    if (this.maxFrame - this.keep <= this.minFrame) return;
    const cutoff = this.maxFrame - this.keep;
    for (const f of this.frames.keys()) {
      if (f < cutoff) { this.frames.delete(f); this.stats.evicted++; }
    }
    while (this.anchors.length && this.anchors[0].frame < cutoff) {
      this.anchors.shift();
    }
    // 重算 minFrame（淘汰后下界抬升）。
    let lo = Infinity;
    for (const f of this.frames.keys()) if (f < lo) lo = f;
    this.minFrame = lo === Infinity ? -1 : lo;
  }

  /** 取某帧输入数组；缺帧返回 null（区别于「全松手」的 0 数组）。 */
  at(frame) {
    const row = this.frames.get(Number(frame) | 0);
    return row ? row.slice() : null;
  }

  /** 连续可用性：从 from 到 to 每槽是否都有真值。用于重放前判完整。 */
  completeRange(from, to) {
    const a = Number(from) | 0, b = Number(to) | 0;
    for (let f = a; f <= b; f++) if (!this.frames.has(f)) return false;
    return true;
  }

  /** 区间内缺帧数（诊断用）。 */
  gaps(from, to) {
    const a = Number(from) | 0, b = Number(to) | 0;
    let n = 0;
    for (let f = a; f <= b; f++) if (!this.frames.has(f)) n++;
    this.stats.gaps = n;
    return n;
  }

  /** ≤ target 的最近锚点（回滚起点参考）；没有返回 null。 */
  bestAnchor(target) {
    const t = Number(target) | 0;
    let best = null;
    for (const a of this.anchors) if (a.frame <= t) best = a;
    return best;
  }

  /** 填充给定摘要源到锚点（如 WASM frame_digest），用于完整性校验。 */
  fillDigests(digestFn) {
    if (typeof digestFn !== 'function') return this.anchors.length;
    for (const a of this.anchors) {
      if (a.digest === null) {
        try { a.digest = Number(digestFn(a.frame)) >>> 0; } catch (_) { /* 保留 null */ }
      }
    }
    return this.anchors.length;
  }

  snapshot() {
    return {
      minFrame: this.minFrame,
      maxFrame: this.maxFrame,
      frames: this.frames.size,
      anchors: this.anchors.map(a => ({ ...a })),
      keep: this.keep,
      anchorEvery: this.anchorEvery,
      ...this.stats,
    };
  }
}

export default ReplayArchive;
