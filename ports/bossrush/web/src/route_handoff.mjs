/**
 * BRO 换路交接状态机（阶段 3Q，对齐 eagler netplay-routing.md 的 switch→accept→commit）
 *
 * 换路不是"说切就切"：两端丢帧/错序会让新路径上的摘要从哪帧续都成问题。
 * eagler 的做法是三段交接 + 可靠控制帧重放：
 *   switch  : host 提议换到 targetPath，指定共同切换帧 switchFrame；
 *   accept  : 两端确认接受，并从此刻起把关键控制帧（TIMING/GATE/DIGEST/POS/LOADOUT）
 *             与输入帧按帧缓存，不直接丢弃；
 *   commit  : 到 switchFrame 两端重建到新路径，把缓存的帧按序重放，再正式恢复。
 *
 * 本模块只管"状态机 + 待重放帧账本"的纯逻辑；真正的 PC 重建在 NetPeer，
 * 由调用方在 onCommit 回调里执行 applySurveyRoute + 重放返回的控制帧。
 * 纯逻辑零依赖、可单测。
 */
export const HANDOFF_PHASE = { IDLE: 'idle', SWITCH: 'switch', ACCEPT: 'accept', COMMIT: 'commit' };

export class RouteHandoff {
  constructor(opts = {}) {
    this.role = opts.role === 'client' ? 'client' : 'host';
    this.replayKeep = Math.max(4, Number(opts.replayKeep) || 60);
    this.reset();
  }

  reset() {
    this.phase = HANDOFF_PHASE.IDLE;
    this.token = 0;
    this.targetPath = null;
    this.switchFrame = null;
    this.buffer = [];        // { frame, seq, kind, payload } 待重放的可靠帧
    this._seq = 0;
    this.accepted = false;
    this.committed = false;
    this.history = [];
  }

  get active() { return this.phase !== HANDOFF_PHASE.IDLE; }

  /**
   * host 发起换路提议。返回要广播的 switch 帧数据。
   * switchFrame 必须给两端留出缓冲时间（调用方保证 > 当前帧）。
   */
  propose(targetPath, switchFrame) {
    if (this.active) return null;
    this.phase = HANDOFF_PHASE.SWITCH;
    this.token = (this.token + 1) & 0xffff;
    this.targetPath = targetPath === 'turn' ? 'turn' : 'direct';
    this.switchFrame = Number(switchFrame) | 0;
    this.buffer.length = 0;
    this.accepted = false;
    this.committed = false;
    this.history.push({ phase: 'switch', token: this.token, at: Date.now() });
    return this.switchMessage();
  }

  switchMessage() {
    return { phase: 'switch', token: this.token, targetPath: this.targetPath, switchFrame: this.switchFrame };
  }

  /**
   * 收到对端的 switch 提议（client 侧）或 accept（host 侧）。
   * 返回应发送的应答帧，或 null。
   */
  onMessage(msg) {
    if (!msg || typeof msg !== 'object') return null;
    if (msg.phase === 'switch') {
      // 只接受 host 的新提议；重复/过期 token 忽略。
      if (this.role !== 'client' || (this.active && this.token === msg.token)) return null;
      this.phase = HANDOFF_PHASE.ACCEPT;
      this.token = msg.token | 0;
      this.targetPath = msg.targetPath === 'turn' ? 'turn' : 'direct';
      this.switchFrame = Number(msg.switchFrame) | 0;
      this.buffer.length = 0;
      this.accepted = true;
      this.committed = false;
      this.history.push({ phase: 'switch-recv', token: this.token, at: Date.now() });
      return { phase: 'accept', token: this.token };
    }
    if (msg.phase === 'accept') {
      if (this.role !== 'host' || this.phase !== HANDOFF_PHASE.SWITCH || msg.token !== this.token) return null;
      this.phase = HANDOFF_PHASE.ACCEPT;
      this.accepted = true;
      this.history.push({ phase: 'accept-recv', token: this.token, at: Date.now() });
      return { phase: 'commit', token: this.token, targetPath: this.targetPath, switchFrame: this.switchFrame };
    }
    if (msg.phase === 'commit') {
      if (msg.token !== this.token || this.phase !== HANDOFF_PHASE.ACCEPT) return null;
      return this._commit();
    }
    return null;
  }

  /**
   * host 在收到 accept 后主动提交（client 则等 commit 帧）。
   * 返回 { targetPath, switchFrame, replay }（replay 为按 seq 排序的待重放帧）。
   */
  commit() {
    if (this.role !== 'host' || this.phase !== HANDOFF_PHASE.ACCEPT) return null;
    return this._commit();
  }

  _commit() {
    this.phase = HANDOFF_PHASE.COMMIT;
    this.committed = true;
    const replay = this.replay();
    this.history.push({ phase: 'commit', token: this.token, replay: replay.length, at: Date.now() });
    return { targetPath: this.targetPath, switchFrame: this.switchFrame, replay, token: this.token };
  }

  /** 交接进行中（switch/accept）缓存一帧；commit 后不再缓存。返回是否缓存。 */
  bufferFrame(frame, kind, payload) {
    if (this.phase !== HANDOFF_PHASE.SWITCH && this.phase !== HANDOFF_PHASE.ACCEPT) return false;
    this.buffer.push({ frame: Number(frame) | 0, seq: this._seq++, kind: String(kind), payload });
    while (this.buffer.length > this.replayKeep) this.buffer.shift();
    return true;
  }

  /** 取待重放帧（按 seq 稳定排序）。 */
  replay() {
    return this.buffer.slice().sort((a, b) => a.seq - b.seq);
  }

  /** commit 完成、新路径就绪后收尾。 */
  finish() {
    this.phase = HANDOFF_PHASE.IDLE;
    this.buffer.length = 0;
    this.history.push({ phase: 'finish', token: this.token, at: Date.now() });
  }

  snapshot() {
    return {
      phase: this.phase, active: this.active, role: this.role,
      token: this.token, targetPath: this.targetPath, switchFrame: this.switchFrame,
      accepted: this.accepted, committed: this.committed,
      buffered: this.buffer.length, replayKeep: this.replayKeep,
      history: this.history.slice(-12),
    };
  }
}

export default RouteHandoff;
