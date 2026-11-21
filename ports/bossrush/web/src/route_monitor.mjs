/**
 * BRO 对局期持续质量监控（阶段 3Q，对齐 eagler-touhou netplay-routing.md）
 *
 * 3P 做的是「开局一次性选路 + 开闸后锁定」。本模块补上真正的"持续"：
 * 对局中周期性把心跳 PONG 的 RTT / 超时丢包喂进来，滚动评分；质量持续劣化
 * 且过了保护期，就建议换路（direct↔turn）。避免偶发抖动误切：
 *   - 滑动时间窗（默认 8s）内的样本算 p50/p95/loss/jitter/score；
 *   - 保护期（默认 60s）：换路/开局后这段时间内不触发，避免刚连上就切；
 *   - 连续劣化计数（默认连续 3 个评估周期都差）才建议切换；
 *   - 迟滞（score 阈值 1±hysteresis）复用 3P 的思路。
 *
 * 输出是"建议"，真正的 switch→accept→commit 交接在 NetPeer/门面做。
 * 纯逻辑、零依赖，Node 单测与浏览器同源。
 */
import { RouteSelector } from './route_selector.mjs';

export class RouteMonitor {
  constructor(opts = {}) {
    this.windowMs = Math.max(1000, Number(opts.windowMs) || 8000);
    this.evaluateEveryMs = Math.max(200, Number(opts.evaluateEveryMs) || 2000);
    this.protectionMs = Math.max(0, Number(opts.protectionMs) != null ? Number(opts.protectionMs) : 60000);
    this.degradeStreakNeed = Math.max(1, Number(opts.degradeStreak) || 3);
    this.hysteresis = Number.isFinite(opts.hysteresis) ? opts.hysteresis : 0.35;
    this.rttP95Threshold = Number.isFinite(opts.rttP95Threshold) ? opts.rttP95Threshold : 120;
    this.lossThreshold = Number.isFinite(opts.lossThreshold) ? opts.lossThreshold : 0.15;
    this.jitterThreshold = Number.isFinite(opts.jitterThreshold) ? opts.jitterThreshold : 40;
    this.minSamples = Math.max(2, Number(opts.minSamples) || 6);
    this.path = opts.path === 'turn' ? 'turn' : 'direct';  // 当前路径（外部可更新）
    this.enabled = false;
    this.reset();
  }

  reset(path) {
    if (path) this.path = path === 'turn' ? 'turn' : 'direct';
    this.samples = [];        // { rtt, at }
    this.sent = 0;
    this.lost = 0;
    this.enabled = false;
    this.startedAt = null;    // 监控启用 / 上次换路时刻（保护期起点）
    this.lastEvalAt = null;
    this.degradeStreak = 0;   // 连续劣化评估次数
    this.recoverStreak = 0;   // 连续优良评估次数（切回直连用）
    this.suggestion = null;   // 最近一次建议
    this.evaluations = 0;
    this.history = [];        // 评估历史（给 HUD 曲线），有界
  }

  /** 启用监控（可在开跑时调用）。重置保护期起点。 */
  arm(path, now) {
    this.enabled = true;
    this.startedAt = now == null ? Date.now() : now;
    if (path) this.path = path === 'turn' ? 'turn' : 'direct';
    this.degradeStreak = 0;
    this.recoverStreak = 0;
    return this.snapshot();
  }

  disarm() { this.enabled = false; return this.snapshot(); }

  /** 一次成功往返。 */
  noteRtt(rtt, now) {
    const ms = Number(rtt);
    if (!Number.isFinite(ms) || ms < 0) return false;
    const at = now == null ? Date.now() : now;
    this.samples.push({ rtt: ms, at });
    return true;
  }

  /** 记一次发出 / 一次超时丢包。 */
  noteSent() { this.sent++; }
  noteLost() { this.lost++; }

  /** 淘汰时间窗外的样本，返回窗口内快照。 */
  _window(now) {
    const cutoff = now - this.windowMs;
    while (this.samples.length && this.samples[0].at < cutoff) this.samples.shift();
    return this.samples;
  }

  /**
   * 到评估周期则算一次分并更新建议。返回本次评估对象，或 null（未到期/样本不足/未启用）。
   * now 由调用方注入，便于单测精确控制保护期与周期。
   */
  evaluate(now) {
    if (!this.enabled) return null;
    const t = now == null ? Date.now() : now;
    if (this.lastEvalAt != null && t - this.lastEvalAt < this.evaluateEveryMs) return null;
    this.lastEvalAt = t;

    const samples = this._window(t);
    const sel = new RouteSelector({
      minSamples: this.minSamples,
      rttP95Threshold: this.rttP95Threshold,
      lossThreshold: this.lossThreshold,
      jitterThreshold: this.jitterThreshold,
      hysteresis: this.hysteresis,
    });
    // 用窗口样本重建评分（RouteSelector 是单次窗口语义，这里每周期重算）。
    for (const s of samples) { sel.noteSent(); sel.addSample(s.rtt, null, s.at); }
    // 把累计的丢包折算进当前窗口（sent 至少覆盖窗口样本数 + 丢包）。
    const missingSent = Math.max(0, this.sent - sel.sent);
    for (let i = 0; i < missingSent; i++) sel.noteSent();
    for (let i = 0; i < this.lost; i++) sel.noteLost();

    const metrics = sel.metrics();
    const score = sel.score();
    const inProtection = this.startedAt != null && (t - this.startedAt) < this.protectionMs;
    const enough = score != null;
    const poor = enough && score > 1 + this.hysteresis;
    const good = enough && score < 1 - this.hysteresis;

    const prevSuggestion = this.suggestion;
    let action = 'hold';
    let reason = enough ? (poor ? 'quality-poor' : good ? 'quality-good' : 'band') : 'insufficient-samples';

    if (poor) { this.degradeStreak++; this.recoverStreak = 0; }
    else if (good) { this.recoverStreak++; this.degradeStreak = 0; }
    else { this.degradeStreak = 0; this.recoverStreak = 0; }

    if (!inProtection && this.path === 'direct' && this.degradeStreak >= this.degradeStreakNeed) {
      action = 'switch-turn';
      reason = 'degraded-streak';
    } else if (!inProtection && this.path === 'turn' && this.recoverStreak >= this.degradeStreakNeed) {
      // 切回直连要求连续优良更久（默认同 streak，可用更大 degradeStreak 收紧），带迟滞已防抖。
      action = 'switch-direct';
      reason = 'recovered-streak';
    } else if (inProtection && (poor || good)) {
      reason = 'protection';
    }

    const ev = {
      at: t, path: this.path, score, metrics, inProtection,
      degradeStreak: this.degradeStreak, recoverStreak: this.recoverStreak,
      action, reason,
      changed: action !== prevSuggestion,
    };
    if (action !== 'hold') this.suggestion = action;
    else if (!poor && !good) this.suggestion = null;
    this.evaluations++;
    this.history.push({ at: t, score, p95: metrics.p95, loss: metrics.loss, path: this.path, action });
    if (this.history.length > 40) this.history.shift();
    return ev;
  }

  /** 换路完成后调用：切路径、清窗口、重置保护期与计数。 */
  onSwitched(path, now) {
    this.path = path === 'turn' ? 'turn' : 'direct';
    this.samples.length = 0;
    this.sent = 0;
    this.lost = 0;
    this.startedAt = now == null ? Date.now() : now;
    this.lastEvalAt = null;
    this.degradeStreak = 0;
    this.recoverStreak = 0;
    this.suggestion = null;
  }

  snapshot(now) {
    const t = now == null ? Date.now() : now;
    const samples = this._window(t);
    const sel = new RouteSelector({ minSamples: this.minSamples });
    for (const s of samples) { sel.noteSent(); sel.addSample(s.rtt, null, s.at); }
    const missingSent = Math.max(0, this.sent - sel.sent);
    for (let i = 0; i < missingSent; i++) sel.noteSent();
    for (let i = 0; i < this.lost; i++) sel.noteLost();
    return {
      enabled: this.enabled,
      path: this.path,
      windowMs: this.windowMs,
      protectionMs: this.protectionMs,
      protectionLeftMs: this.startedAt != null
        ? Math.max(0, this.protectionMs - (t - this.startedAt)) : 0,
      inProtection: this.startedAt != null && (t - this.startedAt) < this.protectionMs,
      windowSamples: samples.length,
      sent: this.sent,
      lost: this.lost,
      metrics: sel.metrics(),
      score: sel.score(),
      degradeStreak: this.degradeStreak,
      recoverStreak: this.recoverStreak,
      suggestion: this.suggestion,
      evaluations: this.evaluations,
      history: this.history.slice(),
    };
  }
}

export default RouteMonitor;
