/**
 * BRO 输出提交队列（阶段 3N，对齐 eagler rollback.md 的 confirmed-output 契约）
 *
 * 回滚会重复执行逻辑帧，但音频播放、存档写盘、Replay/观战发布是**不可逆**的。
 * 因此这些副作用按帧暂存，只有该帧在所有远端确认且没有待纠正事务后才提交；
 * 回滚时先丢弃尚未提交的目标帧及之后的输出，避免重复/错序。
 *
 * BRO 现状：音频由 WASM 内部直接触发、无法逐帧撤销，所以本队列先接住
 * 「可由 JS 侧控制」的输出（网页音效、UI 事件、Replay/观战帧标记），
 * 并在门面 getFrontiers() 中暴露提交水位。WASM 内部音频是已知边界。
 */
export class OutputQueue {
  constructor(opts = {}) {
    this.keep = Number(opts.keep) || 240;
    this.events = [];                    // 有序 { frame, kind, payload }
    this.nextSeq = 0;
    this.committedThrough = -1;
    this.stats = { queued: 0, committed: 0, discarded: 0, dropped: 0 };
  }

  reset() {
    this.events.length = 0;
    this.nextSeq = 0;
    this.committedThrough = -1;
    this.stats = { queued: 0, committed: 0, discarded: 0, dropped: 0 };
  }

  /** 按逻辑帧暂存一个不可逆副作用。 */
  push(frame, kind, payload) {
    const f = Number(frame) | 0;
    if (f < 0 || f < this.committedThrough) { this.stats.dropped++; return false; }
    this.events.push({ frame: f, kind: String(kind || 'event'), payload, seq: this.nextSeq++ });
    this.stats.queued++;
    return true;
  }

  /**
   * 只提交 confirmed 与 simulated 两者较小值之前的完整帧事件。
   * 对齐 eagler `AudioEvents::CommitThrough(confirmed, simulated)`。
   */
  commitThrough(confirmed, simulated) {
    const limit = Math.min(Number(confirmed) | 0, Number(simulated) | 0);
    if (limit <= this.committedThrough) return [];
    const out = [];
    const keep = [];
    for (const e of this.events) {
      if (e.frame <= limit) { out.push(e); this.committedThrough = Math.max(this.committedThrough, e.frame); }
      else if (e.frame > this.committedThrough) keep.push(e);
      else this.stats.dropped++;
    }
    this.events = keep;
    this.stats.committed += out.length;
    return out;
  }

  /** 回滚：丢弃目标帧及之后的未提交输出。返回丢弃条数。 */
  discardFrom(frame) {
    const f = Number(frame) | 0;
    const before = this.events.length;
    this.events = this.events.filter(e => e.frame < f);
    const n = before - this.events.length;
    this.stats.discarded += n;
    return n;
  }

  /** 退休过老已确认帧，保持内存有界。 */
  trim(nowFrame) {
    const cutoff = (Number(nowFrame) | 0) - this.keep;
    const before = this.events.length;
    this.events = this.events.filter(e => e.frame > cutoff);
    this.stats.dropped += before - this.events.length;
  }

  snapshot() {
    return {
      pending: this.events.length,
      committedThrough: this.committedThrough,
      firstPending: this.events.length ? this.events[0].frame : null,
      lastPending: this.events.length ? this.events[this.events.length - 1].frame : null,
      ...this.stats,
    };
  }
}
