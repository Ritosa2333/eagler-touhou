/**
 * BRO 开局选路质量评分（阶段 3P，对齐 eagler-touhou netplay-routing.md 思路）
 *
 * 现有 3O 只做「直连失败才回退 TURN」的一次性回退，缺少开局主动的
 * 「卡在加载界面一会、密集测延迟再选路」阶段。本模块把 netcode 的
 * 密集 RTT 采样浓缩成一个可判定的评分：
 *   - p50 / p95 RTT（p95 抗偶发抖动，对齐 eagler 用高分位做选路）
 *   - 丢包率（survey 中 PING 超时未回 PONG 的比例）
 *   - jitter（相邻 RTT 平均绝对差，反映链路稳定性）
 *   - 综合 score（越低越好），与阈值比较给出 direct / turn 决策
 *
 * 设计约束：
 *   - 纯逻辑、零依赖，可在 Node 单测与浏览器里同源运行。
 *   - 样本不足时 decide() 返回 null（不决策），避免用 1~2 个样本误判。
 *   - 带迟滞（hysteresis）：一旦选定某路径，要明显更优才切换，
 *     防止 p95 在阈值附近抖动导致来回切路（eagler 的 60s 保护期思想在职）。
 */
export class RouteSelector {
  constructor(opts = {}) {
    // 判定所需最小样本数；少于它不决策。
    this.minSamples = Math.max(2, Number(opts.minSamples) || 6);
    // p95 RTT 阈值（ms）：超过则认为直连质量不可接受，倾向 TURN。
    this.rttP95Threshold = Number.isFinite(opts.rttP95Threshold)
      ? opts.rttP95Threshold : 120;
    // 丢包率阈值（0..1）：超过则倾向 TURN。
    this.lossThreshold = Number.isFinite(opts.lossThreshold)
      ? opts.lossThreshold : 0.15;
    // jitter 阈值（ms）：链路抖动过大也视为差链路。
    this.jitterThreshold = Number.isFinite(opts.jitterThreshold)
      ? opts.jitterThreshold : 40;
    // 迟滞量（score 单位，相对阈值 1.0 的缓冲带）：默认 0.35，即已选 direct
    // 时新分数须 >1.35 才切 turn（反之 <0.65 才切回）。避免阈值附近来回抖。
    this.hysteresis = Number.isFinite(opts.hysteresis) ? opts.hysteresis : 0.35;
    // 最近样本窗口（FIFO）：避免陈旧 RTT 污染 p95。跨多轮不 reset 时尤其重要。
    this.maxSamples = Math.max(2, Number(opts.maxSamples) || 12);
    this.reset();
  }

  reset() {
    this.samples = [];      // { seq, rtt, at }
    this.lost = 0;          // 超时未回的 PING 数
    this.sent = 0;          // 发出的 PING 总数
    this.decision = null;   // 'direct' | 'turn'
    this._lastScore = null;
  }

  /** 记录一次成功往返。rtt 单位 ms。 */
  addSample(rtt, seq, at) {
    const ms = Number(rtt);
    if (!Number.isFinite(ms) || ms < 0) return false;
    this.samples.push({ seq: seq == null ? this.samples.length : (seq | 0), rtt: ms, at: at == null ? 0 : at });
    while (this.samples.length > this.maxSamples) this.samples.shift();
    return true;
  }

  /** 记录一次发出的 PING（用于算丢包率）。 */
  noteSent() { this.sent++; }

  /** 记录一次 PING 超时未回。 */
  noteLost(seq) { this.lost++; }

  get sampleCount() { return this.samples.length; }

  /** p50 / p95 / jitter / loss 一屏快照。 */
  metrics() {
    const n = this.samples.length;
    if (!n) {
      return { n: 0, p50: null, p95: null, mean: null, jitter: null,
        loss: this.sent ? this.lost / this.sent : 0, sent: this.sent, lost: this.lost };
    }
    const sorted = this.samples.map(s => s.rtt).sort((a, b) => a - b);
    const pick = (q) => {
      // 最近秩法：idx = ceil(q*n)-1，落在 [0, n-1]
      const idx = Math.max(0, Math.min(n - 1, Math.ceil(q * n) - 1));
      return sorted[idx];
    };
    const p50 = pick(0.5), p95 = pick(0.95);
    let sum = 0;
    for (const s of sorted) sum += s;
    const mean = sum / n;
    let jsum = 0;
    for (let i = 1; i < n; i++) jsum += Math.abs(sorted[i] - sorted[i - 1]);
    const jitter = n > 1 ? jsum / (n - 1) : 0;
    return {
      n, p50, p95, mean, jitter,
      loss: this.sent ? this.lost / this.sent : 0,
      sent: this.sent, lost: this.lost,
    };
  }

  /**
   * 综合分（越低越好）：p95 主导 + 丢包重罚 + jitter 小权重。
   *
   * 门禁用「发出 PING 数 >= minSamples 且至少 2 个成功往返」，而不是单纯
   * 成功率：高丢包时成功样本天然很少，恰恰是最该选路的场景，若按成功率
   * 门禁会永远无法决策。只要发出足够、拿到 ≥2 个 RTT 能算 p95，就允许评分，
   * 丢包率高会体现在 lossPart 把分数推过阈值。
   */
  score() {
    const m = this.metrics();
    if (m.n < 2 || m.sent < this.minSamples) return null;
    const rttPart = m.p95 / this.rttP95Threshold;
    const lossPart = (m.loss / this.lossThreshold) * 1.5;
    const jitterPart = (m.jitter / this.jitterThreshold) * 0.5;
    return rttPart + lossPart + jitterPart;
  }

  /**
   * 给出选路决策。score <= 1 视为直连可接受；否则建议 TURN。
   * 带迟滞：已有决策时，新分数必须明显越过阈值才切换。
   * 返回 { path, score, metrics, switched, reason } 或 null（样本不足）。
   */
  decide() {
    const score = this.score();
    if (score == null) return null;
    const m = this.metrics();
    const candidate = score <= 1 ? 'direct' : 'turn';

    let reason = candidate === 'direct' ? 'quality-good' : 'quality-poor';
    if (m.loss > this.lossThreshold && candidate === 'direct') {
      reason = 'loss-high';
    } else if (m.p95 > this.rttP95Threshold && candidate === 'direct') {
      reason = 'rtt-high';
    }

    // 首次决策直接采纳。
    if (this.decision == null) {
      this.decision = candidate;
      this._lastScore = score;
      return { path: candidate, score, metrics: m, switched: true, reason };
    }

    // 迟滞：切换要跨过 1±hysteresis 的带，避免阈值附近抖动。
    const willSwitch = candidate !== this.decision;
    let accept = !willSwitch;
    if (willSwitch) {
      if (candidate === 'turn') accept = score > 1 + this.hysteresis;
      else accept = score < 1 - this.hysteresis;
    }
    if (!accept) {
      return { path: this.decision, score, metrics: m, switched: false, reason: 'hysteresis' };
    }
    const prev = this.decision;
    this.decision = candidate;
    this._lastScore = score;
    return { path: candidate, score, metrics: m, switched: candidate !== prev, reason };
  }

  /** 给网页 HUD 的快照。 */
  snapshot() {
    return {
      minSamples: this.minSamples,
      threshold: { rttP95: this.rttP95Threshold, loss: this.lossThreshold, jitter: this.jitterThreshold },
      hysteresis: this.hysteresis,
      metrics: this.metrics(),
      score: this.score(),
      decision: this.decision,
    };
  }
}

/** 便捷函数：直接从 {rttSamples:[ms...], sent, lost} 生成一次决策（单测友好）。 */
export function decideFromSamples(rttSamples, opts = {}) {
  const sel = new RouteSelector(opts);
  for (const rtt of (rttSamples || [])) { sel.noteSent(); sel.addSample(rtt); }
  const extraSent = Math.max(0, (opts.sent || 0) - sel.sent);
  for (let i = 0; i < extraSent; i++) sel.noteSent();
  for (let i = 0; i < (opts.lost || 0); i++) sel.noteLost();
  return sel.decide();
}

export default RouteSelector;
