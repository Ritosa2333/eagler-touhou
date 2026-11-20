/**
 * BRO 联机回滚日志（阶段 3K，opt-in）
 *
 * 设计边界（诚实）：这是**轻量回滚**，不是 GGPO 全状态回滚。
 * Lua 侧快照只覆盖 lstg.var 标量 + 两个玩家实体 + 按键字节；
 * boss/bullet/item/laser 的对象图不重建。因此它能把「玩家/计分状态」
 * 消费掉 InputBuffer 的 correction 事件并回到同帧，再用快照 hash 自校验；
 * 完整对象图回滚仍未实现。
 *
 * 用法（门面内部已接好）：
 *   journal.arm()                       // 默认关闭，打开后才保存/回滚
 *   journal.save(frame)                 // 每逻辑帧在 tick 内、引擎推进前
 *   journal.beforeFrame(target, events) // stepGame 开头消费纠错
 */

export class RollbackJournal {
  constructor(opts = {}) {
    this.enabled = false;
    this.keep = Number(opts.keep) || 240;
    this.call = opts.call || (() => undefined);
    this.callBool = opts.callBool || (() => false);
    this.jsonCall = opts.jsonCall || (() => ({ count: 0 }));
    this.emit = opts.emit || (() => {});
    this.frames = new Map();                 // playFrame -> 保存时的 hash
    this.stats = {
      enabled: false, saved: 0, restarts: 0, replayed: 0,
      verified: 0, failed: 0, missed: 0, lastFrame: -1,
    };
  }

  arm(opts = {}) {
    this.enabled = true;
    if (Number.isFinite(opts.keep)) this.keep = Math.max(30, opts.keep | 0);
    this.stats.enabled = true;
    this.emit('rollback', this.getState());
    return this.getState();
  }

  disarm() {
    this.enabled = false;
    this.stats.enabled = false;
    this.emit('rollback', this.getState());
    return this.getState();
  }

  reset() {
    this.frames.clear();
    this.stats = {
      enabled: this.enabled, saved: 0, restarts: 0, replayed: 0,
      verified: 0, failed: 0, missed: 0, lastFrame: -1,
    };
    if (typeof this.call === 'function') this.call('BRO_net_clear_snapshots');
  }

  /** 保存当前轻量状态。返回 hash（0 = 被关闭或失败）。 */
  save(frame) {
    if (!this.enabled) return 0;
    const f = Number(frame) | 0;
    if (f < 0) return 0;
    const h = Number(this.call('BRO_net_save_snapshot', f)) >>> 0;
    this.frames.set(f, h);
    this.stats.saved++;
    this.stats.lastFrame = f;
    if (this.frames.size > this.keep) {
      const oldest = this.frames.keys().next().value;
      if (oldest !== undefined) this.frames.delete(oldest);
    }
    return h;
  }

  /**
   * 在本帧输入写回之前调用。events 来自 InputBuffer.takeCorrections()。
   * replayFn(rf) 可选：调用方重放 rf 帧的输入（以及可选的引擎帧）。
   */
  beforeFrame(targetFrame, events, replayFn) {
    if (!this.enabled || !Array.isArray(events) || events.length === 0) return null;
    const target = Number(targetFrame) | 0;
    const candidates = events
      .map(e => Number(e && e.frame) | 0)
      .filter(f => f >= 0 && f < target && f >= target - this.keep);
    if (candidates.length === 0) return null;
    const from = Math.min(...candidates);
    if (!this.frames.has(from)) {
      this.stats.missed++;
      return null;
    }

    if (!this.callBool('BRO_net_restore_snapshot', from)) {
      this.stats.failed++;
      return null;
    }
    const verified = this.callBool('BRO_net_snapshot_check', from);
    if (verified) this.stats.verified++;
    else this.stats.failed++;

    let replayed = 0;
    if (typeof replayFn === 'function') {
      for (let f = from; f < target; f++) { replayFn(f); replayed++; }
    }
    this.stats.restarts++;
    this.stats.replayed += replayed;
    const info = {
      from, to: target, verified, replayed,
      events: events.filter(e => Number(e && e.frame) === from).length,
      at: Date.now(),
    };
    this._last = info;
    this.emit('rollback', info);
    return info;
  }

  getState() {
    let lua = { count: 0 };
    try { lua = this.jsonCall('BRO_net_snapshot_stats') || { count: 0 }; } catch (_) { /* noop */ }
    return {
      ...this.stats,
      frames: this.frames.size,
      luaFrames: lua.count || 0,
      first: lua.first ?? null,
      last: lua.last ?? null,
      lastEvent: this._last || null,
    };
  }
}
