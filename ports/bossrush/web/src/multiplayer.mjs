/**
 * BRO 联机高层接口（阶段 3D）——给网页 UI 用的「输出 / 输入」门面。
 *
 * 分层：
 *   netcode.mjs      纯传输（WebRTC DataChannel + 7B 输入帧 + LOADOUT/READY 文本帧）
 *   multiplayer.lua  纯 Lua 状态（自机/系统/P1P2/种子/输入的真值源）
 *   ↓ 本文件把两者接起来，并给 DOM UI 一组稳定 API：
 *
 * 读（WASM → 网页）：
 *   getCatalog()   可选自机/系统、解锁状态、按键名
 *   getState()     seed/stage/localSlot/双端选择/每端输入字节
 *   on(event, cb)  open / roster / input / ready / error / state
 *
 * 写（网页 → WASM → 对端）：
 *   setSlot(p) / setCharacter(id) / setSystems([1,2]) / setSeed(n)
 *   setLoadout({...}) / startMatch(stage, seed)
 *   tick(frame)    每逻辑帧：采集本地按键 → 发包 → 远端输入写回 Lua 缓冲
 *   setActiveInput(p|null)  调试/观战：让全局 KeyState 吃指定玩家的输入
 *
 * 用法（浏览器）：
 *   const multi = new BROMultiplayer({ ccall, input, room, player, signal });
 *   const catalog = multi.getCatalog();          // 渲染自机/系统选择 UI
 *   multi.setCharacter('reimu_player');
 *   multi.setSystems([1, 10]);
 *   await multi.connect();                       // 交换 LOADOUT + seed
 *   multi.startMatch('TH06@BossRush', 12345);
 *   // 主循环里：multi.tick(frame);
 */

import { NetPeer, MSG, KEY_BIT, keyStateToByte, byteToKeyState } from './netcode.mjs';
import { NetMesh } from './netmesh.mjs';
import { InputBuffer, InputClock, makeAdaptivePredict } from './input_buffer.mjs';
import { ReplayArchive } from './replay_archive.mjs';
import { RollbackJournal } from './rollback.mjs';
import { OutputQueue } from './output_queue.mjs';

const KEY_NAMES = Object.keys(KEY_BIT);

/** 把 JS 值安全写成 Lua 字面量（只支持简单类型，避免把任意 JSON 拼进源码） */
function luaValue(v) {
  if (v === null || v === undefined) return 'nil';
  if (typeof v === 'number') return Number.isFinite(v) ? String(v) : 'nil';
  if (typeof v === 'boolean') return v ? 'true' : 'false';
  if (typeof v === 'string') {
    return '"' + v.replace(/\\/g, '\\\\').replace(/"/g, '\\"')
      .replace(/\n/g, '\\n').replace(/\r/g, '\\r') + '"';
  }
  if (Array.isArray(v)) return '{' + v.map(luaValue).join(',') + '}';
  return '"' + String(v).replace(/\\/g, '\\\\').replace(/"/g, '\\"') + '"';
}

/**
 * 浏览器信令门面：WebSocket 握手 + join + 把 SDP/ICE 转给 NetPeer。
 * 单独导出，方便网页 UI 自己管连接/重连。
 */
export function createWebSocketSignal({
  url, urls, room, player, onMessage,
  connectTimeoutMs = 2000, retryDelayMs = 1000,
}) {
  const source = (Array.isArray(urls) && urls.length) ? urls : [url];
  const candidates = source.filter(Boolean);
  if (!candidates.length) throw new Error('createWebSocketSignal: url or urls required');
  const msgListeners = [], closeListeners = [], openListeners = [];
  let ws = null, idx = 0, timer = null, retryTimer = null;
  let manualClose = false, chosen = null;

  function scheduleRetry() {
    // 还有未尝试的候选时，由 connectOne 顺序继续；只有全试完才整轮重试。
    if (manualClose || retryTimer || idx < candidates.length) return;
    // 已成功过的入口优先复用（eagler：同一页面生命周期内锁定入口）。
    if (chosen) candidates.unshift(candidates.splice(candidates.indexOf(chosen), 1)[0]);
    idx = 0;
    retryTimer = setTimeout(() => { retryTimer = null; connectOne(); }, retryDelayMs);
  }

  function connectOne() {
    if (idx >= candidates.length) { scheduleRetry(); return; }
    const u = candidates[idx++];
    chosen = u;
    let settled = false;
    try { ws = new WebSocket(u); } catch (_) { connectOne(); return; }
    const current = ws;
    timer = setTimeout(() => {
      if (settled || current.readyState === 1) return;
      settled = true;
      try { current.close(); } catch (_) { /* noop */ }
      connectOne();                       // 2s 预算：无响应入口直接跳过
    }, Math.max(200, connectTimeoutMs | 0));

    current.addEventListener('open', () => {
      if (settled) return;
      settled = true;
      clearTimeout(timer);
      clearTimeout(retryTimer); retryTimer = null;  // 旧入口关闭排的重连要取消
      idx = candidates.length;                      // 成功后锁定该入口，后续断线重连复用它
      signal.send({ kind: 'join', room, player });
      openListeners.forEach((cb) => cb({ url: u }));
    });
    current.addEventListener('message', (e) => {
      let m;
      try { m = JSON.parse(e.data); } catch { return; }
      if (typeof onMessage === 'function') onMessage(m);
      for (const cb of msgListeners) cb(m);
    });
    current.addEventListener('close', () => {
      clearTimeout(timer);
      closeListeners.forEach((cb) => cb({ url: u, manual: manualClose }));
      if (!manualClose && settled) scheduleRetry();
    });
    current.addEventListener('error', () => {
      if (settled) return;
      settled = true;
      clearTimeout(timer);
      try { current.close(); } catch (_) { /* noop */ }
      connectOne();
    });
  }

  const signal = {
    get ws() { return ws; },
    get url() { return chosen; },
    get ready() { return !!ws && ws.readyState === 1; },
    send(msg) {
      if (!ws || ws.readyState !== 1) return false;
      ws.send(JSON.stringify(msg));
      return true;
    },
    onMessage(cb) { msgListeners.push(cb); return this; },
    onOpen(cb) { openListeners.push(cb); return this; },
    onClose(cb) { closeListeners.push(cb); return this; },
    close() {
      manualClose = true;
      clearTimeout(timer); clearTimeout(retryTimer);
      if (ws) { try { ws.close(); } catch (_) { /* noop */ } }
      return true;
    },
  };
  connectOne();
  return signal;
}

export class BROMultiplayer {
  constructor(opts = {}) {
    this.ccall = opts.ccall;
    if (typeof this.ccall !== 'function') throw new Error('BROMultiplayer: ccall required');
    this.input = opts.input || null;               // attachInput() 的返回值，可选
    this.getKeys = opts.getKeys || null;           // Node/自定义输入源：() => KeyState 表
    this.room = opts.room || 'bro-default';
    this.player = Number(opts.player) | 0;
    this.role = opts.role || (this.player === 0 ? 'host' : 'client');
    this.signal = opts.signal || null;
    this.iceServers = opts.iceServers || [];       // 同机测试用空表；公网部署填 STUN/TURN
    // 3O：默认直连优先；大厅可勾选「优先使用 TURN」（强制 relay）。
    this.preferTurn = opts.preferTurn === true;
    this.turnServers = opts.turnServers || [];
    this.netFactory = opts.netFactory || null;     // 单测注入假 NetPeer
    this.optsAutoStart = opts.autoStart !== false; // 保留给接入方决定是否自动协商
    // 3R：多人共斗上限（默认 2 保持 1v1 全部既有语义；lobby/4 人模式显式传 maxPlayers:4）。
    // InputBuffer/InputClock 按此容量分配；Lua 侧 _G.BRO_MAX_PLAYERS 恒为 4（本体上限）。
    this.maxPlayers = Number(opts.maxPlayers) > 0 ? (Number(opts.maxPlayers) | 0) : 2;
    this.net = null;
    // 3T：>2 人时 this.net 指向 NetMesh（接口与 NetPeer 同形），1v1 不动。
    this.mesh = null;
    this.meshFactory = opts.meshFactory || null;     // 单测注入假 mesh
    this._meshStarted = false;
    this._meshStartRequested = false;
    this._rosterWired = false;
    this.catalogCache = null;
    this._listeners = new Map();
    this._settingKeys = null;
    this.roster = {};                              // slot -> loadout（P1~P4 动态）
    for (let p = 0; p < this.maxPlayers; p++) this.roster[p] = null;
    this.remoteReady = {};
    for (let p = 0; p < this.maxPlayers; p++) this.remoteReady[p] = false;
    this.stats = { frames: 0, sent: 0, recv: 0, lastRemoteFrame: -1 };

    // ---- 时序层（阶段 3E） ----
    // 3W：自适应预测默认开启（符卡/特技短按松开帧少猜错一次；不改确定性）。
    // 注入自定义 buffer 的测试/接入方自行决定 predict；opts.adaptivePredict=false 可关。
    this.adaptivePredict = opts.adaptivePredict !== false;
    this.buffer = opts.buffer || new InputBuffer({
      keep: 180, startFrame: 1, players: this.maxPlayers,
      predict: this.adaptivePredict ? makeAdaptivePredict() : null,
    });
    // 3X：关卡级重放的输入流长归档（与短窗口 InputBuffer 分离）。
    // 真值输入每帧记入，fullReplayRollback 从这里取 1..target，解掉 180 帧窗口。
    this.archive = opts.archive || new ReplayArchive({
      players: this.maxPlayers,
      keep: Number.isFinite(opts.archiveKeep) ? Math.max(180, opts.archiveKeep | 0) : 3600,
      anchorEvery: Number.isFinite(opts.archiveAnchorEvery) ? Math.max(1, opts.archiveAnchorEvery | 0) : 300,
      onAnchor: (f) => {
        const v = Number(this.call('BRO_net_frame_digest', f)) >>> 0;
        return v;
      },
    });
    this.clock = opts.clock || new InputClock({
      buffer: this.buffer, localPlayer: this.player, players: this.maxPlayers,
      delay: Number(opts.delay) || 2,
    });
    this.lastCollect = null;
    this._timingDirty = false;

    // ---- 位置帧同步（阶段 3G） ----
    // 每帧把本地 player 坐标经 POS 帧发给对端；收到对端 POS 写入远端代理并插值。
    // 未进关（local_pos.ready=false）时不发，避免用 (0,0) 覆盖对端位置。
    this.posSync = opts.posSync !== false;
    this.posAlpha = Number.isFinite(opts.posAlpha) ? opts.posAlpha : 0.25;
    this.localPos = null;
    this._remotePosCount = 0;

    // ---- 帧级状态摘要 / 分叉检测（阶段 3I） ----
    // 每个逻辑帧（playFrame 编号）采样一次本地状态 hash，经 DIGEST 帧发对端；
    // 对端 hash 写入 Lua 后与本地同帧比对，分歧即 emit('divergence')。
    // 这是回滚/快照的前置：先能发现分叉，才谈得上修正。
    this.digestSync = opts.digestSync !== false;
    this._lastDigest = { frame: -1, hash: 0 };
    this._divergenceSeen = false;

    // ---- 开局栅栏 / 统一起跑帧号（阶段 3J） ----
    // 3I 摘要靠「两端都从墙钟帧 1 开始 ++」的脆弱假设对齐：一端先加载好先跑，
    // 另一端后到，游戏世界从不同帧起演化、摘要必然分叉。栅栏让两端都 armed 后，
    // 由 host 选一个共同起跑墙钟帧号（max(双方已到帧)+margin）广播；开闸前只喂
    // 输入缓冲（让对端能推进/收到输入），不推进游戏帧/实体/摘要。
    // **默认 enabled=false**：不开栅栏时 tick/stepGame 行为与 3I 完全一致（兼容
    // 既有单测/e2e）；只有网页显式 armGate() 才启用。opt-in，逐步接入。
    this.gate = {
      enabled: false,
      opened: false,
      gated: false,
      startFrame: null,        // 共同起跑墙钟帧号（host 下发 / client 收到）
      lastWallFrame: 0,        // 本端到达过的最大墙钟帧（openGate 计算用）
      margin: Number.isFinite(opts.gateMargin) ? (opts.gateMargin | 0) : 30,
      blockedFrames: 0,        // 开闸前被拦下的帧数（UI 显示/诊断）
    };

    // ---- 轻量快照 / 回滚（阶段 3K，opt-in） ----
    // 默认关闭：老 e2e/单测与只跑 UI 的 lobby 不会触发任何新的 Lua 调用。
    this.rollback = new RollbackJournal({
      keep: opts.rollbackKeep,
      call: (...a) => this.call(...a),
      callBool: (...a) => this.callBool(...a),
      jsonCall: (...a) => this.jsonCall(...a),
      emit: (ev, data) => this.emit(ev, data),
    });
    this.rollbackReplayFrames = opts.rollbackReplayFrames === true;
    // 3L：回滚是否响应真实 DIGEST 分叉。回滚本身 opt-in；一旦 arm 则默认响应。
    this.reactToDivergence = opts.reactToDivergence !== false;
    this.rollbackMinGap = Number.isFinite(opts.rollbackMinGap) ? Math.max(1, opts.rollbackMinGap | 0) : 8;
    this._remoteDigests = new Map();       // frame -> 对端 hash（JS 侧缓存，避免 Lua 全表扫描）
    this._divergenceEvents = [];           // 待回滚消费的分叉帧
    this._divergenceAttempted = new Set(); // 已尝试过的帧，防止「追不平 → 每帧回滚」死循环
    this._lastRollbackWall = -Infinity;
    this._divergenceRecovery = { seen: 0, converged: 0, unresolved: 0, lastFrame: -1 };
    // 3N：不可逆输出按帧暂存，只在确认水位后提交（回滚时丢弃未提交部分）。
    this.output = new OutputQueue();
    this._autoFullReplay = opts.autoFullReplay === true;  // 分级选路：轻量失败后是否自动全量恢复
    this._fullReplayMaxFrames = Number.isFinite(opts.fullReplayMaxFrames)
      ? Math.max(1, opts.fullReplayMaxFrames | 0) : 60;
    this._fullReplayMinGap = Number.isFinite(opts.fullReplayMinGap)
      ? Math.max(1, opts.fullReplayMinGap | 0) : 90;
    this._lastAutoFullReplayWall = -Infinity;
    this._autoRoutes = { attempted: 0, converged: 0, unresolved: 0, skipped: 0, lastFrame: -1, lastReason: null };
    // 3M：关卡级重放回滚（默认不在 stepGame 中触发）
    this._fullReplay = { runs: 0, frames: 0, lastFrame: -1, lastConverged: null, lastMs: 0 };
    this._startVars = null;
    // 3P：开局选路测量状态（门面缓存，避免 UI 直接摸 net）。
    this.routeSurvey = { active: false, startedAt: null, finishedAt: null, progress: null, lastReport: null };
    this._routeMonitorArmed = false;
    if (opts.rollback === true) this.rollback.arm();
  }

  // ---- 事件 -------------------------------------------------------------
  on(event, cb) {
    if (!this._listeners.has(event)) this._listeners.set(event, []);
    this._listeners.get(event).push(cb);
    return () => {
      const a = this._listeners.get(event) || [];
      const i = a.indexOf(cb);
      if (i >= 0) a.splice(i, 1);
    };
  }

  emit(event, data) {
    for (const cb of (this._listeners.get(event) || [])) {
      try { cb(data); } catch (e) { console.error('[multi] listener ' + event, e); }
    }
  }

  // ---- Lua 桥 ------------------------------------------------------------
  evalLua(code) {
    const out = this.ccall('bro_eval', 'string', ['string'], [code]);
    if (typeof out === 'string' && out.slice(0, 4) === 'ERR ') {
      this.emit('error', out);
      throw new Error(out);
    }
    return out;
  }

  call(name, ...args) {
    // bro_eval 用 lua_tostring 取返回值，布尔/nil 会变成空串；统一 tostring 包一层。
    return this.evalLua('return tostring(' + name + '(' + args.map(luaValue).join(',') + '))');
  }

  callBool(name, ...args) {
    const s = this.call(name, ...args);
    return s === 'true' || s === '1';
  }

  jsonCall(name, ...args) {
    const s = this.call(name, ...args);
    return JSON.parse(s);
  }

  // ---- 读：WASM → 网页 -----------------------------------------------------
  getCatalog(force = false) {
    if (this.catalogCache && !force) return this.catalogCache;
    this.catalogCache = this.jsonCall('BRO_net_catalog');
    return this.catalogCache;
  }

  getState() { return this.jsonCall('BRO_net_state'); }

  /** 3R：按槽的个人战绩 { '0':{score,graze,dead,getsc}, '1':... }（团队分数仍在 lstg.var）。 */
  getStats() { return this.jsonCall('BRO_net_stats'); }

  /**
   * 3S：多人 HUD 数据。团队共享区（擦弹/分数/蓄力=lstg.var，全体可见）+ 个人区
   * （players[]：label=P1..P4、miss、hp/hpRatio 血条、graze、score，来自 3R 个人账）。
   * 画面内 P1~P4 面板由 Web 补丁层 hook drawScore 绘制；这里给网页 DOM 也留一份。
   */
  getHud() { return this.jsonCall('BRO_hud_data'); }
  setHudEnabled(on) { return this.callBool('BRO_hud_set_enabled', !!on); }
  setHudForceSingle(on) { return this.callBool('BRO_hud_set_force_single', !!on); }

  /** 3R：本 Web 构建支持的最大同屏人数（Lua 与 JS 门面一致）。 */
  getMaxPlayers() {
    const cat = this.getCatalog();
    return (cat && Number(cat.maxPlayers) > 0) ? Number(cat.maxPlayers) : this.maxPlayers;
  }

  /**
   * 阶段 3G：本地 player 世界坐标 {x,y,frame,ready}。
   * tick 用它发 POS 帧；网页 HUD 也可读来显示自身/对手小地图。
   */
  getLocalPos() {
    const p = this.jsonCall('BRO_net_local_pos');
    this.localPos = p;
    return p;
  }

  /**
   * 阶段 3F/3G：多 player 实体表（槽位 → {x,y,frame,samples,kind}）。
   * 网页可用它显示实体位置；Dist/Angle 的就近解析就基于这张表。
   * kind: local=本端真实 player / remote=POS 驱动的远端代理。
   */
  getPlayers() { return this.jsonCall('BRO_net_players'); }

  /** 阶段 3F：直接设置某槽位实体坐标（无实体时自动建代理；网页手动驱动用）。 */
  setPlayerPos(slot, x, y, frame) {
    this.call('BRO_net_set_player_pos', Number(slot) | 0, Number(x) || 0, Number(y) || 0,
      frame === undefined ? undefined : Number(frame) | 0);
    this.emit('players', this.getPlayers());
    return this.getPlayers();
  }

  /** 阶段 3G：写远端代理的目标坐标（POS 帧回调用；x/y 由 step 插值逼近）。 */
  setPlayerTarget(slot, x, y, frame) {
    this.call('BRO_net_set_player_target', Number(slot) | 0, Number(x) || 0, Number(y) || 0,
      Number(frame) | 0);
    return true;
  }

  // ---- 阶段 3H：真实第二玩家实体 -------------------------------------------
  /**
   * 在本 WASM 实例里生成一个真实的 player_class 实体（远端玩家，slot = 1-player）。
   * charId 是角色类名（reimu_player/marisa_player/...），必须已进关且有 init_player_data。
   * 返回 {ok, slot, char, x, y, group, globalPlayerKept}；失败含 err。
   */
  spawnRemoteEntity(charId, slot) {
    const s = (slot === undefined || slot === null) ? (1 - this.player) : (Number(slot) | 0);
    let r;
    try { r = this.jsonCall('BRO_net_spawn_player', s, charId); }
    catch (e) { r = { ok: false, err: String(e) }; }
    if (r && r.ok) this.emit('players', this.getPlayers());
    return r;
  }

  /** 把某槽位输入字节写进真实实体的 p.key（每帧由 tick 自动调用；也可手动调试）。 */
  feedEntityKey(slot, byte) {
    return Number(this.call('BRO_net_feed_player_key', Number(slot) | 0, Number(byte) & 0xff)) & 0xff;
  }

  // ---- 3AC：多人 Replay 旁路多流（方案 A）------------------------------------
  // 原版 STGR 只录本地一条输入流；旁路文件每帧记所有玩家，回放时喂回远端实体。
  // 落盘走 MEMFS /replay（storage.mjs 自动持久化），宿主用 eagler read/write 分享。

  /** 开始一局多人录像会话（在 stage.Set("save") 之后调用）。
   *  JS 门面不传参，Lua 侧按已注册实体自动推断 playerCount/localSlot；
   *  需要覆盖时用 bro_eval 直接调 BRO_mre_begin{playerCount=4,...}。 */
  mreBegin() { return this.jsonCall('BRO_mre_begin'); }
  /** 采集当前帧（每帧 GetInput 之后调用）。 */
  mreCapture() { return this.callBool('BRO_mre_capture'); }
  /** 保存旁路到 replay/slotN.multi。 */
  mreSave(slot) { return this.jsonCall('BRO_mre_save', Number(slot) | 0); }
  /** 加载旁路（stage.Set("load") 之后调用）。 */
  mreLoad(slot) { return this.jsonCall('BRO_mre_load', Number(slot) | 0); }
  /** 回放喂帧（每帧 GetInput 之后调用）。 */
  mreFeed() { return this.callBool('BRO_mre_feed'); }
  /** 回放时按需 spawn 缺失的远端实体。 */
  mreEnsureEntities() { return Number(this.call('BRO_mre_ensure_entities')) | 0; }
  /** 回放状态 / 当前录像帧（调试与测试）。 */
  mreStatus() { return this.jsonCall('BRO_mre_status'); }
  mreFrames() { return this.jsonCall('BRO_mre_frames'); }
  mreReadFile(slot) { return this.call('BRO_mre_read_file', Number(slot) | 0); }
  mreReset() { return this.callBool('BRO_mre_reset'); }

  /** 查询槽位实体：{real, valid, x, y, ...}。real=true 表示 New 出来的真实实体。 */
  getPlayerInfo(slot) { return this.jsonCall('BRO_net_player_info', Number(slot) | 0); }

  /** 销毁真实实体（Del + 清注册表）。 */
  destroyRemoteEntity(slot) {
    const r = this.callBool('BRO_net_destroy_player', Number(slot) | 0);
    if (r) this.emit('players', this.getPlayers());
    return r;
  }

  getLocalLoadout() {
    const s = this.getState();
    return s.loadouts[String(this.player)] || null;
  }

  /** 默认键位表来自 Lua setting.keys，改键后 UI 不用同步映射 */
  getSettingKeys(force = false) {
    if (this._settingKeys && !force) return this._settingKeys;
    try {
      this._settingKeys = JSON.parse(this.evalLua('return Serialize(setting.keys)'));
    } catch { this._settingKeys = {}; }
    return this._settingKeys;
  }

  // ---- 写：网页 → WASM ----------------------------------------------------
  setSlot(p) {
    const r = this.callBool('BRO_net_set_slot', Number(p) | 0);
    this.player = Number(p) | 0;
    this.clock.localPlayer = this.player;
    this.emit('state', this.getState());
    return r;
  }

  setCharacter(id) {
    const r = this.callBool('BRO_net_set_character', id);
    if (r) this._broadcastLoadout();
    this.emit('state', this.getState());
    return r;
  }

  setSystems(ids) {
    const arr = Array.isArray(ids) ? ids : String(ids || '').split(/[,\s]+/).filter(Boolean);
    this.call('BRO_net_set_systems', arr.map((n) => Number(n) | 0));
    this._broadcastLoadout();
    this.emit('state', this.getState());
    return this.getLocalLoadout();
  }

  setLoadout(obj = {}) {
    if (obj.slot !== undefined) this.setSlot(obj.slot);
    if (obj.character !== undefined) this.setCharacter(obj.character);
    if (obj.systems !== undefined) this.setSystems(obj.systems);
    return this.getState();
  }

  setSeed(seed) {
    const s = Number(this.call('BRO_net_set_seed', Number(seed) | 0)) | 0;
    if (this.ccall) this.ccall('bro_set_seed', null, ['number'], [s]);
    this.emit('state', this.getState());
    return s;
  }

  setActiveInput(p) {
    if (p === null || p === undefined) this.call('BRO_net_set_active_input');
    else this.call('BRO_net_set_active_input', Number(p) | 0);
    const v = this.getState().activeInput;
    return v === undefined ? null : v;
  }

  startMatch(stage, seed) {
    if (seed !== undefined && seed !== null) this.setSeed(seed);
    const ok = this.callBool('BRO_net_start_match', stage || this.getState().stage);
    this.emit('start', { stage, seed, ok });
    return ok;
  }

  reset() {
    this.call('BRO_net_reset');
    this.roster = {};
    this.remoteReady = {};
    for (let p = 0; p < this.maxPlayers; p++) { this.roster[p] = null; this.remoteReady[p] = false; }
    this.catalogCache = null;
    this.buffer.reset();
    if (this.archive) this.archive.reset();
    this.lastCollect = null;
    this.stats.frames = 0;
    this.localPos = null;
    this._remotePosCount = 0;
    this._lastDigest = { frame: -1, hash: 0 };
    this._divergenceSeen = false;
    // 3L：分叉回滚状态跟随 reset 一起清空。
    this._remoteDigests.clear();
    this._divergenceEvents.length = 0;
    this._divergenceAttempted.clear();
    this._lastRollbackWall = -Infinity;
    this._divergenceRecovery = { seen: 0, converged: 0, unresolved: 0, lastFrame: -1 };
    this._fullReplay = { runs: 0, frames: 0, lastFrame: -1, lastConverged: null, lastMs: 0 };
    this._startVars = null;
    this.routeSurvey = { active: false, startedAt: null, finishedAt: null, progress: null, lastReport: null };
    this._routeMonitorArmed = false;
    if (this.net && typeof this.net.cancelRouteSurvey === 'function') this.net.cancelRouteSurvey();
    if (this.net && typeof this.net.stopRouteMonitor === 'function') this.net.stopRouteMonitor();
    this.output.reset();
    this._autoFullReplay = false;
    this._lastAutoFullReplayWall = -Infinity;
    this._autoRoutes = { attempted: 0, converged: 0, unresolved: 0, skipped: 0, lastFrame: -1, lastReason: null };
    // 阶段 3J：reset 关闭栅栏回到默认态（与新建实例一致）。
    this.gate.enabled = false;
    this.gate.opened = false;
    this.gate.gated = false;
    this.gate.startFrame = null;
    this.gate.blockedFrames = 0;
    this._gateDirty = false;
    this.rollback.reset();
    return this.getState();
  }

  // ---- 输入：网页 → WASM → 对端 -------------------------------------------
  /** 采集本地 8 个 replay 键，得到 KEY_BIT 字节（与 Replay.lua 同构） */
  getLocalKeysByte() {
    if (typeof this.getKeys === 'function') return keyStateToByte(this.getKeys());
    if (!this.input || !this.input.diag) return 0;
    const held = new Set(this.input.diag.held || []);
    const map = this.getSettingKeys();
    const st = {};
    for (const name of KEY_NAMES) st[name] = held.has(map[name]);
    return keyStateToByte(st);
  }

  /**
   * 阶段 3I：统一执行时钟——一个逻辑帧的完整三步。
   * 联机对局主循环应调用本方法（而非手动分调 tick + bro_frame）：
   *   ① tick(f)   按墙钟帧 f 采集本地输入、对齐远端输入、喂实体；
   *   ② bro_frame() 推进引擎一帧（若提供 advance 回调则调它）；
   *   ③ 摘要采样已在 tick 内完成（本帧状态 hash → 对端）。
   * 两端都以同一 f 序列驱动，playFrame=f-delay 对齐，摘要才可比。
   * advance 缺省时不推进引擎（兼容 lobby 这种只跑 UI 的页面）。
   */
  /**
   * 3Z：惰性安装系统资源语义层。未进关时 lstg.system 是 stub（DoFrame[i] 为 table），
   * install 返回 false；进关后变真身才成功。成功后置位，不再重复 eval。
   */
  _ensureSysShare() {
    if (this._sysShareDone || !this.ccall) return;
    try {
      const r = this.ccall('bro_eval', 'string', ['string'],
        ['return tostring((function() if type(BRO_SYSTEM_SHARE)==\'table\' then return BRO_SYSTEM_SHARE.install() end return false end)())']);
      if (String(r) === 'true') this._sysShareDone = true;
    } catch { /* 未就绪，下帧再试 */ }
  }

  stepGame(frame, advance) {
    const f = Number(frame) | 0;
    // 3Z：进关后 lstg.system 才从 stub 变真身，此时惰性安装系统资源共享/独立层
    // （幂等，成功后置位不再尝试）。开机第一次真正推进引擎前调用。
    this._ensureSysShare();
    // 3K：若上一帧之后有迟到的真实输入推翻了预测，先回到同帧再写本帧输入。
    this._rollbackBefore(f, advance);
    this.tick(f);
    // 阶段 3J：开闸前只喂输入，不推进引擎（两端从共同帧号起跑）。
    if (this.gate.gated) return null;
    if (typeof advance === 'function') advance(f);
    else if (typeof this.ccall === 'function') this.ccall('bro_frame', 'number', [], []);
    return this.lastCollect && this.lastCollect.frame;
  }

  // ---- 开局栅栏（阶段 3J） ---------------------------------------------
  /** 启用栅栏并进入「等待开闸」：开闸前 tick/stepGame 只喂输入、不推进游戏。 */
  armGate(opts = {}) {
    this.gate.enabled = true;
    this.gate.opened = false;
    this.gate.gated = true;
    this.gate.startFrame = null;
    this.gate.blockedFrames = 0;
    this.gate.lastWallFrame = 0;
    if (Number.isFinite(opts.margin)) this.gate.margin = Math.max(1, opts.margin | 0);
    this.emit('gate', this.getGate());
    return this.getGate();
  }

  /**
   * host 开闸：选一个两端都没到达、留足余量的共同起跑墙钟帧号并广播。
   * startFrame = max(本端最大墙钟帧, 对端最大输入帧) + margin。
   * 显式传 startFrame 时用调用方的值（便于单测/自定义同步源）。
   * client 不应自行开闸——client 通过 net.onGate 采纳 host 下发的帧号。
   */
  openGate(startFrame) {
    let sf = Number(startFrame);
    if (!Number.isFinite(sf)) {
      const remote = this.stats.lastRemoteFrame >= 0 ? this.stats.lastRemoteFrame : 0;
      sf = Math.max(this.gate.lastWallFrame, remote) + this.gate.margin;
    }
    sf = Math.max(0, sf | 0);
    this.gate.enabled = true;
    this.gate.startFrame = sf;
    // 3P：开闸进入对局 → 锁定当前路径，选路测量不再重建连接。
    if (this.net) this.net.routeLocked = true;
    if (this.role === 'host' && this.net && typeof this.net.sendGate === 'function') {
      this.net.sendGate(sf);
    }
    this.emit('gate', this.getGate());
    return sf;
  }

  /** 栅栏状态快照（给网页 HUD / 单测） */
  getGate() {
    return {
      enabled: this.gate.enabled,
      opened: this.gate.opened,
      gated: this.gate.gated,
      startFrame: this.gate.startFrame,
      lastWallFrame: this.gate.lastWallFrame,
      margin: this.gate.margin,
      blockedFrames: this.gate.blockedFrames,
    };
  }

  // ---- 轻量快照 / 回滚（阶段 3K） ---------------------------------------
  /** 打开快照/回滚。默认关闭，打开后每逻辑帧保存一次轻量状态。 */
  armRollback(opts = {}) {
    const state = this.rollback.arm(opts);
    if (opts.replayFrames === true) this.rollbackReplayFrames = true;
    // 3L：回滚一旦打开，默认就响应 DIGEST 分叉；可显式关掉只做输入纠错回滚。
    this.reactToDivergence = opts.reactToDivergence !== false;
    // 3N：分级选路默认关闭。显式打开后，轻量回滚追不平才自动做关卡级重放恢复。
    this._autoFullReplay = opts.autoFullReplay === true;
    if (Number.isFinite(opts.fullReplayMaxFrames)) {
      this._fullReplayMaxFrames = Math.max(1, opts.fullReplayMaxFrames | 0);
    }
    if (Number.isFinite(opts.minGap)) this.rollbackMinGap = Math.max(1, opts.minGap | 0);
    return {
      ...state, reactToDivergence: this.reactToDivergence,
      autoFullReplay: this._autoFullReplay,
    };
  }

  disarmRollback() {
    const state = this.rollback.disarm();
    this._divergenceEvents.length = 0;
    return state;
  }

  getRollback() {
    return {
      ...this.rollback.getState(),
      reactToDivergence: this.reactToDivergence,
      autoFullReplay: this._autoFullReplay,
      pendingDivergence: this._divergenceEvents.length,
      recovery: { ...this._divergenceRecovery },
      fullReplay: { ...this._fullReplay },
      autoRoutes: { ...this._autoRoutes },
      output: this.output.snapshot(),
    };
  }

  /** 3X：输入流长归档状态（给 UI/诊断：跨 180 帧窗口的重放能力）。 */
  getReplayArchive() {
    return this.archive ? this.archive.snapshot() : null;
  }

  /** 3M：在开局处抓一份 lstg.var 标量快照，供关卡级重放回滚使用。 */
  captureStartVars() {
    const s = this.call('BRO_net_replay_capture_start');
    this._startVars = (s && s !== 'nil') ? s : null;
    return this._startVars;
  }

  /**
   * 3M：关卡级全状态回滚原型（opt-in，默认不接 stepGame）。
   * 回到关卡起始 + 按帧重放输入流，重建 boss/bullet/item/laser 对象图——
   * 绕过「Lua task 协程无法克隆」的另一条路，代价是回滚 N 帧要真跑 N 帧。
   * expectHash 传对端同帧摘要，可直接判断是否追平。
   */
  fullReplayRollback(targetFrame, opts = {}) {
    const target = Number(targetFrame) | 0;
    if (target < 1) return { frames: 0, converged: false, reason: 'nothing-to-replay' };
    const st = this.getState();
    const stage = opts.stage || st.stage || 'TH06@BossRush';
    const seed = Number.isFinite(opts.seed) ? (opts.seed | 0) : (Number(st.seed) || 0);
    const vars = typeof opts.startVars === 'string' ? opts.startVars
      : (this._startVars || this.captureStartVars());
    const before = Number(this.call('BRO_net_frame_digest', target)) >>> 0;
    const t0 = Date.now();
    let consumed = 0;
    // 3X：优先用长归档的输入流（解 180 帧窗口）；归档缺帧时回退到 InputBuffer。
    const rowAt = (f) => {
      const fromArchive = this.archive && this.archive.at(f);
      if (fromArchive) return fromArchive;
      const row = new Array(this.maxPlayers);
      let any = false;
      for (let p = 0; p < this.maxPlayers; p++) {
        const v = this.buffer.get(p, f);
        row[p] = v === null ? 0 : v;
        if (v !== null) any = true;
      }
      return any ? row : null;
    };
    // 归档/缓冲共同覆盖 1..target 才重放，否则如实拒绝（避免用 0 冒充真值造成假分叉）。
    let haveAll = true;
    for (let f = 1; f <= target; f++) if (!rowAt(f)) { haveAll = false; break; }
    if (!haveAll) {
      return { frames: 0, converged: false, reason: 'input-gap',
        archive: this.archive ? this.archive.snapshot() : null };
    }
    this.call('BRO_net_replay_begin', stage, seed, vars);
    try {
      for (let f = 1; f <= target; f++) {
        // 本地玩家输入走 replayReader → 全局 KeyState；其余玩家走 per-instance p.key。
        const row = rowAt(f);
        for (let p = 0; p < this.maxPlayers; p++) {
          const v = row[p];
          this.call('BRO_net_feed_player_key', p, v);
          if (p === this.player) this.call('BRO_net_replay_push', v);
        }
        if (this.ccall('bro_frame', 'number', [], []) !== 0) {
          return { frames: f, converged: false, reason: 'bro_frame-failed' };
        }
      }
    } finally {
      consumed = Number(this.call('BRO_net_replay_end')) | 0;
    }
    const after = Number(this.call('BRO_net_frame_digest', target)) >>> 0;
    const want = opts.expectHash === undefined ? null : (opts.expectHash >>> 0);
    const converged = want === null ? null : after === want;
    // 3N：可同时验证重放窗口内的多个分叉帧（分级选路要知道「最早错误帧」是否追平）。
    const verify = [];
    if (Array.isArray(opts.verifyFrames)) {
      for (const c of opts.verifyFrames) {
        const cf = Number(c && c.frame) | 0;
        if (cf < 1 || cf > target) continue;
        const ch = Number(this.call('BRO_net_frame_digest', cf)) >>> 0;
        verify.push({
          frame: cf, after: ch, remote: Number(c.hash) >>> 0,
          converged: ch === (Number(c.hash) >>> 0),
        });
      }
    }
    const info = {
      from: 0, to: target, frames: consumed, before, after, expect: want,
      converged, verify,
      changed: before !== after, ms: Date.now() - t0, at: Date.now(),
      archive: this.archive ? { minFrame: this.archive.minFrame, maxFrame: this.archive.maxFrame,
        anchor: this.archive.bestAnchor(target) } : null,
    };
    this._fullReplay.runs++;
    this._fullReplay.frames += consumed;
    this._fullReplay.lastConverged = converged;
    this._fullReplay.lastFrame = target;
    this._fullReplay.lastMs = info.ms;
    this.emit('full-replay-rollback', info);
    return info;
  }

  /** 3L：取走待回滚消费的分叉帧（UI 诊断也可调用）。 */
  takeDivergence() {
    const out = this._divergenceEvents.slice();
    this._divergenceEvents.length = 0;
    return out;
  }

  /**
   * 在写回本帧输入之前消费两类事件：
   *   ① 输入预测纠错（3K）；② 真实 DIGEST 分叉（3L）。
   * replay 默认只重放输入写回；打开 replayFrames 后才逐帧再调 advance（对象图未回滚，慎用）。
   */
  _rollbackBefore(wallFrame, advance) {
    const target = this.clock.playFrame(wallFrame);
    const inputEvents = this.buffer.takeCorrections();
    const rawDiv = this.takeDivergence();
    // 对端跑得更快时可能先报到未来帧：留到本端推进到该帧再消费。
    const future = [], divEvents = [];
    for (const e of rawDiv) ((Number(e.frame) | 0) >= target ? future : divEvents).push(e);
    if (future.length) this._divergenceEvents.push(...future);

    const requeue = () => {
      if (inputEvents.length) this.buffer.correctionEvents.unshift(...inputEvents);
      if (divEvents.length) this._divergenceEvents.unshift(...divEvents);
    };
    if (!this.rollback.enabled) { requeue(); return null; }
    const events = this.reactToDivergence ? inputEvents.concat(divEvents) : inputEvents;
    if (events.length === 0) return null;
    // 冷却：一帧内可能同时收到多个分叉报告，避免连续回滚造成画面抖动。
    if ((wallFrame | 0) - this._lastRollbackWall < this.rollbackMinGap) { requeue(); return null; }

    const replayFn = (rf) => {
      // 3BF：回滚重放必须覆盖房间里所有席位。原写死 p<2 会在 3 人房漏掉 P2 的
      // 真值输入 → 重放后摘要仍分叉（这是「回滚看着跑了但追不平」的隐形 bug）。
      for (let p = 0; p < this.maxPlayers; p++) {
        const v = this.buffer.get(p, rf);
        if (v === null) continue;
        this.call('BRO_net_apply_keys_at', p, rf, v, false);
        this.call('BRO_net_feed_player_key', p, v);
      }
      if (this.rollbackReplayFrames && typeof advance === 'function') advance(rf);
    };
    const rb = this.rollback.beforeFrame(target, events, replayFn);
    if (!rb) {
      // 快照缺失/恢复失败：允许这些分叉帧后续重试。
      for (const e of divEvents) this._divergenceAttempted.delete(Number(e.frame) | 0);
      return null;
    }
    // 3N：目标帧及之后的未提交输出全部作废，避免回滚重演时重复播放/写盘。
    this.output.discardFrom(rb.from);
    this._lastRollbackWall = wallFrame | 0;

    // 3L：对被重放过的分叉帧重算摘要，判断是否追平对端。
    const results = [];
    for (const e of divEvents) {
      const f = Number(e.frame) | 0;
      if (f < rb.from || f >= target) continue;
      const after = Number(this.call('BRO_net_frame_digest', f)) >>> 0;
      const want = Number(e.remote) >>> 0;
      const hit = after === want;
      if (hit) this._divergenceAttempted.delete(f); // 追平后允许以后再次分叉
      results.push({
        frame: f, before: Number(e.local) >>> 0, after, remote: want, converged: hit,
      });
      this._divergenceRecovery.lastFrame = f;
    }
    this._divergenceRecovery.seen += results.length;
    this._divergenceRecovery.converged += results.filter(r => r.converged).length;
    this._divergenceRecovery.unresolved += results.filter(r => !r.converged).length;
    rb.divergence = results;
    rb.converged = results.filter(r => r.converged).length;
    rb.unresolved = results.filter(r => !r.converged).length;

    // 3N：轻量回滚的玩家状态已恢复，但对象图没重建 → unresolved。
    // 打开 autoFullReplay 时按窗口自动升级到关卡级重放恢复（重放后引擎帧号
    // 正好停在 target-1，随后 stepGame 正常推进到 target，不会错位）。
    if (rb.unresolved > 0 && this._autoFullReplay && typeof this.ccall === 'function') {
      rb.fullRecover = this._autoFullRecover(wallFrame | 0, target, results);
    }

    if (results.length) this.emit('rollback-divergence', rb);
    return rb;
  }

  /**
   * 3N：分级选路——轻量回滚追不平，才做关卡级重放恢复。
   * 回放终点必须停在 target-1：stepGame 还要正常推进本帧到 target。
   */
  _autoFullRecover(wallFrame, target, results) {
    const replayTo = (target | 0) - 1;
    const bad = results.filter(r => !r.converged && r.frame >= 1 && r.frame <= replayTo);
    const skip = (reason) => {
      this._autoRoutes.skipped++;
      this._autoRoutes.lastReason = reason;
      return { attempted: false, reason };
    };
    if (replayTo < 1) return skip('no-history-before-target');
    if (bad.length === 0) return skip('no-unresolved-frame-in-window');
    if (wallFrame - this._lastAutoFullReplayWall < this._fullReplayMinGap) return skip('cooldown');
    // 输入窗口必须覆盖帧 1..replayTo；否则重放会漏帧，必然再分叉。
    // 3BF：同样按 maxPlayers 检查全部席位（3 人房不能只看 P0/P1）。
    for (let p = 0; p < this.maxPlayers; p++) {
      if (this.buffer.get(p, 1) === null || this.buffer.get(p, replayTo) === null) {
        return skip('input-window-incomplete');
      }
    }
    const expect = this._remoteDigests.get(replayTo);
    const info = this.fullReplayRollback(replayTo, {
      expectHash: expect === undefined ? undefined : expect,
      verifyFrames: bad.map(r => ({ frame: r.frame, hash: r.remote })),
    });
    this._lastAutoFullReplayWall = wallFrame;
    this._autoRoutes.attempted++;
    this._autoRoutes.lastFrame = replayTo;
    const verified = Array.isArray(info.verify) ? info.verify.filter(v => v.converged) : [];
    const converged = verified.length === bad.length &&
      (info.converged === null || info.converged === true);
    if (converged) {
      this._autoRoutes.converged++;
      for (const v of verified) this._divergenceAttempted.delete(v.frame);
    } else {
      this._autoRoutes.unresolved++;
    }
    this._autoRoutes.lastReason = converged ? 'converged' : 'unresolved';
    this.emit('rollback-auto-full', { target, replayTo, converged, info });
    return { attempted: true, converged, replayTo, verified: verified.length, total: bad.length };
  }

  /**
   * 3N：各类帧前沿快照，对齐 eagler rollback.md 的分工：
   * 本地采样 / 模拟 / 远端实际 / 预测 / 输出提交，各自独立，不互相冒充。
   */
  getFrontiers() {
    const s = this.clock.snapshot(this.stats.frames);
    const playFrame = this.lastCollect
      ? this.lastCollect.frame : this.clock.playFrame(this.stats.frames);
    const netTiming = (this.net && typeof this.net.getTiming === 'function')
      ? this.net.getTiming() : {};
    const confirmedAll = this.buffer.confirmedThroughAllRemotes();
    return {
      localSample: this.stats.frames | 0,
      simulated: playFrame,
      remoteActual: confirmedAll + 1,
      predictedFrom: this.buffer.predictedFrontier(playFrame),
      confirmedByPlayer: s.confirmed ? s.confirmed.slice() : [],
      confirmedThroughAllRemotes: confirmedAll,
      acknowledgedLocal: this.buffer.acknowledgedLocalThroughAllRemotes(netTiming.remoteAck),
      outputCommit: this.output.committedThrough,
      outputPending: this.output.events.length,
    };
  }

  /** 3N：把一项不可逆副作用按当前逻辑帧暂存，确认后才提交。 */
  pushOutput(kind, payload) {
    const f = this.lastCollect ? this.lastCollect.frame : this.clock.playFrame(this.stats.frames);
    return this.output.push(f, kind, payload);
  }

  /**
   * 每逻辑帧调一次（阶段 3E 时序版）：
   *   1. 本地按键按【墙钟帧号】入缓冲并发包；
   *   2. 远端 INPUT 到达时按【它自己的帧号】入缓冲（见 onInput）；
   *   3. 用协商好的 delay 取 playFrame = frame - delay，双端输入写回 Lua；
   *   4. 缺帧用上一帧输入预测，游戏不卡顿；真值到达时记 corrections。
   *
   * 这样两端执行的是「同一个帧号 + 同一份输入字节」，与到达时刻无关。
   */
  tick(frame) {
    const f = Number(frame) | 0;
    this.gate.lastWallFrame = Math.max(this.gate.lastWallFrame, f);
    this.call('BRO_net_tick', f);
    const keys = this.getLocalKeysByte();

    // 本地输入入缓冲（即使还没连上网也能单机跑，预测逻辑一致）
    this.buffer.push(this.player, f, keys);
    // 3X：真值输入同时记入长归档（供关卡级重放回滚跨 180 帧窗口）。
    if (this.archive) {
      const row = new Array(this.maxPlayers);
      for (let p = 0; p < this.maxPlayers; p++) row[p] = p === this.player ? keys : (this.buffer.get(p, f) ?? 0);
      this.archive.record(f, row);
    }
    if (this.net && this.net.ready) {
      this.net.sendInput(f, keys);
      this.stats.sent++;
    }

    // 阶段 3J：开闸前只喂输入（上面已 push/send，对端能推进 lastRemoteFrame），
    // 不 collect 写回、不喂实体、不发 POS/DIGEST、不步进——两端从同一帧起跑。
    // 默认 enabled=false，既有单测/e2e 不受影响。
    if (this.gate.enabled) {
      const waiting = this.gate.startFrame === null || f < this.gate.startFrame;
      if (waiting) {
        this.gate.gated = true;
        this.gate.blockedFrames++;
        this.stats.frames++;
        if (this._gateDirty) { this._gateDirty = false; this.emit('gate', this.getGate()); }
        return keys;
      }
      if (this.gate.gated || !this.gate.opened) {
        this.gate.gated = false;
        this.gate.opened = true;
        this.emit('gate-open', this.getGate());
      }
    }

    const col = this.clock.collect(f);
    this.lastCollect = col;
    this.call('BRO_net_set_timing', this.clock.delay, col.frame, col.predicted.includes(true));
    // 3AC：从 2 人遗留改为 maxPlayers —— 4 人 Full Mesh 下 P2/P3/P4 的
    // 真值输入此前从未写进实体 p.key（col.inputs 本就覆盖 maxPlayers 槽），
    // 导致多人录像的远端槽全是 0。collect() 按 this.players=maxPlayers 生成 inputs。
    for (let p = 0; p < this.maxPlayers; p++) {
      this.call('BRO_net_apply_keys_at', p, col.frame, col.inputs[p], col.predicted[p]);
      // 阶段 3H：把该帧输入写进真实实体的 p.key（无实体时无副作用）；
      // 这样远端玩家用自己的 per-instance 输入表行动，本地全局 KeyState 不受污染。
      this.call('BRO_net_feed_player_key', p, col.inputs[p]);
    }

    // 阶段 3G：读本地 player 坐标 → POS 帧发对端；步进远端代理插值。
    if (this.posSync) {
      const lp = this.getLocalPos();
      if (lp.ready && this.net && this.net.ready && typeof this.net.sendPos === 'function') {
        this.net.sendPos(col.frame, lp.x, lp.y);
      }
      this.call('BRO_net_step_entities', this.posAlpha);
    }

    // 阶段 3I：采样本逻辑帧状态摘要 → 发对端（写入 Lua 本地表）。
    // 采样在输入写回之后，反映「本帧将执行的状态」；两端同帧号同输入时应相等。
    if (this.digestSync) this.sampleDigest(col.frame);
    // 3K：保存点与摘要一致（输入已写回、引擎尚未推进），便于同帧校验。
    this.rollback.save(col.frame);
    // 3N：不可逆输出只提交到「所有席位确认 ∧ 已模拟」水位，其余留待确认。
    this.output.commitThrough(this.buffer.confirmedThroughAllRemotes(), col.frame);
    this.output.trim(col.frame);

    this.stats.frames++;
    if (this._timingDirty) { this._timingDirty = false; this.emit('timing', this.getTiming()); }
    return keys;
  }

  /**
   * 阶段 3I：计算并登记某逻辑帧的本地状态摘要，发给对端。
   * hash 由 WASM 侧按对象/var 加权累加生成（稳定、跨实例一致、顺序无关）。
   * 返回 hash 数字。
   * #38：摘要已改成纯整数，这里也只剩一次数值 call；帧数走数字 getter，
   * 不再每个逻辑帧做一次 Lua→JSON→JSON.parse（旧实现是「越玩越卡」的一半）。
   */
  sampleDigest(frame) {
    const f = Number(frame) | 0;
    if (f < 0) return 0;
    const hash = Number(this.call('BRO_net_frame_digest', f)) >>> 0;
    this._lastDigest = { frame: f, hash };
    if (this.net && this.net.ready && typeof this.net.sendDigest === 'function') {
      const frames = Number(this.call('BRO_net_digest_local_frames')) || 0;
      this.net.sendDigest(f, hash, frames);
    }
    return hash;
  }

  /** 阶段 3I：读对账结果 {checked,mismatched,firstFrame,recent}。 */
  getDivergence() {
    return this.jsonCall('BRO_net_divergence');
  }

  /** 3O：强制刷新当前 ICE 路径（direct / turn）并回写缓存。 */
  async getRoute() {
    if (this.net && typeof this.net.getRoute === 'function') return this.net.getRoute();
    return { path: 'unknown', mode: 'offline', rtt: null };
  }

  // ---- 开局选路测量（阶段 3P） -------------------------------------------
  /**
   * 「联机开始后卡在加载界面一会做延迟判定选路」的正式入口。
   * 在 count 个间隔里密集 PING 测 RTT/丢包，交给 RouteSelector 评分，
   * 得到 direct/turn 决策。测量期网页不应推进游戏（保持在加载界面）；
   * 测完 emit('route-survey', report)，由调用方据此选路/设延迟/开跑。
   *
   * opts 透传给 NetPeer.runRouteSurvey；额外：
   *   applyDelay 测完是否按 p50 自动设输入延迟（默认 true，仅 host 生效）。
   * 返回 survey 报告（或 null：连接未就绪）。
   */
  async beginRouteSurvey(opts = {}) {
    this.routeSurvey = {
      active: true, startedAt: Date.now(), progress: null,
      lastReport: this.routeSurvey ? this.routeSurvey.lastReport : null,
    };
    this.emit('route-survey', { status: 'start', at: this.routeSurvey.startedAt });
    if (!this.net || typeof this.net.runRouteSurvey !== 'function') {
      this.routeSurvey.active = false;
      const empty = { path: 'unknown', reason: 'no-transport', samples: 0 };
      this.routeSurvey.lastReport = empty;
      this.emit('route-survey', { status: 'done', report: empty });
      return empty;
    }
    const report = await this.net.runRouteSurvey({
      count: opts.count, intervalMs: opts.intervalMs, timeoutMs: opts.timeoutMs,
      selectorOpts: opts.selectorOpts,
      onProgress: (done, total, metrics, rtt, player) => {
        this.routeSurvey.progress = { done, total, metrics, rtt, player };
        this.emit('route-survey', { status: 'progress', done, total, metrics, rtt, player });
      },
    });
    this.routeSurvey.active = false;
    this.routeSurvey.finishedAt = Date.now();
    this.routeSurvey.lastReport = report;
    if (report && opts.applyDelay !== false && this.role === 'host' && report.rtt50 != null) {
      // 用 p50 的一半折算帧数（对齐 autoDelay 的 suggestedDelay 口径）。
      const frames = Math.max(2, Math.ceil((report.rtt50 / 2) / (1000 / 60)) + 1);
      this.setDelay(frames);
    }
    this.emit('route-survey', { status: 'done', report });
    return report;
  }

  /** 读取最近一次选路测量状态（给网页 HUD）。 */
  getRouteSurvey() {
    const netSurvey = this.net && this.net.route ? this.net.route.lastSurvey : null;
    return {
      active: !!(this.routeSurvey && this.routeSurvey.active),
      startedAt: this.routeSurvey ? this.routeSurvey.startedAt : null,
      finishedAt: this.routeSurvey ? this.routeSurvey.finishedAt : null,
      progress: this.routeSurvey ? this.routeSurvey.progress : null,
      report: (this.routeSurvey && this.routeSurvey.lastReport) || netSurvey || null,
    };
  }

  /**
   * 把测量决策落到实际路径：decision=turn 时若当前不是 TURN 则触发回退；
   * 结果写回 net.route。返回 { applied, from, to }。
   */
  applyRouteDecision(report) {
    const r = report || (this.routeSurvey && this.routeSurvey.lastReport);
    const net = this.net;
    if (!r || !r.path || !net) return { applied: false, reason: 'no-report' };
    const cur = (net.getRouteSnapshot ? net.getRouteSnapshot().path : net.route && net.route.path) || 'unknown';
    if (typeof net.applySurveyRoute === 'function') {
      // 3U：mesh 多链路报告带 perPeer —— 每条链路按自己的测量决策落路，
      // 而不是把聚合 path 广播给所有人（P0-P1 直连 / P0-P2 TURN 可并存）。
      // 1v1 报告无 perPeer，仍传单 path，NetPeer 行为零改动。
      let want = r.path;
      if (r.perPeer && typeof r.perPeer === 'object') {
        const byPlayer = {};
        for (const [player, sub] of Object.entries(r.perPeer)) {
          if (sub && sub.path) byPlayer[player] = sub.path;
        }
        if (Object.keys(byPlayer).length) want = byPlayer;
      }
      const ok = net.applySurveyRoute(want);
      return { applied: ok, from: cur, to: want,
        perPeer: r.perPeer ? Object.fromEntries(
          Object.entries(r.perPeer).map(([p, sub]) => [p, sub && sub.path])) : undefined,
        reason: ok ? 'survey-switch' : (net.routeLocked ? 'route-locked' : 'already-or-unsupported') };
    }
    // 兼容老 NetPeer（无 applySurveyRoute）：仅记录建议，不重建。
    return { applied: false, from: cur, to: r.path, reason: 'no-apply-api' };
  }

  // ---- 持续质量监控 / 换路交接（阶段 3Q） --------------------------------
  /**
   * 启用对局期持续选路监控。默认 opt-in 关闭。
   *   autoSwitch=true 时 host 在质量持续劣化过保护期后自动发起换路交接；
   *   否则只 emit('route-quality') 建议，由网页决定要不要 armRouteSwitch()。
   */
  armRouteMonitor(opts = {}) {
    if (!this.net || typeof this.net.armRouteMonitor !== 'function') return null;
    const snap = this.net.armRouteMonitor(opts);
    this._routeMonitorArmed = true;
    this.emit('route-quality', { status: 'armed', monitor: snap });
    return snap;
  }

  stopRouteMonitor() {
    if (this.net && typeof this.net.stopRouteMonitor === 'function') this.net.stopRouteMonitor();
    this._routeMonitorArmed = false;
    this.emit('route-quality', { status: 'stopped' });
  }

  /** 读监控 + 交接状态（给 HUD/单测）。 */
  getRouteMonitor() {
    if (!this.net || typeof this.net.getRouteMonitor !== 'function') return null;
    return this.net.getRouteMonitor();
  }

  /** 手动发起一次换路交接（host）。switchFrame 省略时自动取留白帧。 */
  armRouteSwitch(targetPath, switchFrame) {
    if (!this.net || typeof this.net.proposeRouteSwitch !== 'function') return null;
    const msg = this.net.proposeRouteSwitch(targetPath, switchFrame);
    this.emit('route-handoff', { status: 'proposed', msg });
    return msg;
  }

  /** 网页 UI 读当前时序：RTT、延迟、确认帧、预测/修正次数、逐帧指纹。 */
  getTiming() {
    const t = (this.net && typeof this.net.getTiming === 'function')
      ? this.net.getTiming() : { rtt: null, samples: 0, remoteAck: -1, suggestedDelay: 2 };
    const s = this.clock.snapshot(this.stats.frames);
    const playFrame = this.lastCollect ? this.lastCollect.frame : this.clock.playFrame(this.stats.frames);
    return {
      ...t,
      delay: this.clock.delay,
      playFrame,
      nextPlayFrame: this.clock.playFrame(this.clock.lastWallFrame + 1),
      confirmed: s.confirmed,
      pending: s.pending,
      predicted: s.predicted,
      corrections: s.corrections,
      conflicts: s.conflicts,
      pushed: s.pushed,
      inputDigest: this.buffer.digest(playFrame),
      lastPredicted: this.lastCollect ? this.lastCollect.predicted : [false, false],
      posSync: this.posSync,
      remotePosCount: this._remotePosCount,
      localPos: this.localPos,
      realEntities: this._realEntityFlags(),
      digest: this.getDivergence(),
      lastDigest: this._lastDigest,
      gate: this.getGate(),
      frontiers: this.getFrontiers(),
      rollback: this.rollback ? this.getRollback() : null,
      routeMonitor: this.getRouteMonitor(),
    };
  }

  /** 阶段 3H：两槽位是否有真实实体（供 UI/HUD 显示 co-op 状态）。 */
  _realEntityFlags() {
    const out = {};
    for (let p = 0; p < 2; p++) {
      try { out[p] = !!this.jsonCall('BRO_net_player_info', p).real; }
      catch { out[p] = false; }
    }
    return out;
  }

  /**
   * 设置输入延迟帧数。host 调用会广播给 client，确保两端一致；
   * client 只接受 host 下发的 TIMING，避免两端各自算导致帧号错位。
   */
  setDelay(n) {
    const v = this.clock.setDelay(n);
    this.call('BRO_net_set_timing', this.clock.delay,
      this.lastCollect ? this.lastCollect.frame : 0, false);
    if (v && this.role === 'host' && this.net && typeof this.net.sendTiming === 'function') {
      this.net.sendTiming(this.clock.delay);
    }
    this.emit('timing', this.getTiming());
    return this.clock.delay;
  }

  /** 用测得的 RTT 给出建议延迟（host 调用后广播）。 */
  autoDelay() {
    const t = (this.net && typeof this.net.getTiming === 'function') ? this.net.getTiming() : null;
    if (!t || t.suggestedDelay === undefined) return this.clock.delay;
    return this.setDelay(t.suggestedDelay);
  }

  // ---- 连接 --------------------------------------------------------------
  _broadcastLoadout() {
    if (!this.net || !this.net.ready) return;
    const s = this.getState();
    this.net.sendLoadout({
      player: this.player,
      slot: this.player,
      character: s.loadouts[String(this.player)].char,
      systems: s.loadouts[String(this.player)].systems,
      index: s.loadouts[String(this.player)].index,
      stage: s.stage,
      seed: s.seed,
    });
  }

  async connect() {
    if (this.net) return this.net;
    const opts = { role: this.role, room: this.room, player: this.player,
      signal: this.signal, iceServers: [...this.iceServers, ...this.turnServers],
      preferTurn: this.preferTurn };
    if (this.maxPlayers > 2) {
      // 3T：跨机 3~4 人 → Full Mesh（每对一条 DC）。链路按信令 roster 增量配置，
      // 实际协商延后到 startNet()（网页在 joined/peer 时机调用）。
      this.mesh = this.meshFactory
        ? this.meshFactory({ ...opts, maxPlayers: this.maxPlayers })
        : new NetMesh({ ...opts, maxPlayers: this.maxPlayers });
      this.net = this.mesh;
      this._wireRosterSignal();
    } else {
      this.net = this.netFactory ? this.netFactory(opts) : new NetPeer(opts);
    }
    this._wireNet();
    return this.net;
  }

  /** 3T：信令系统消息（peer/joined/leave）驱动 mesh 席位编排。 */
  _wireRosterSignal() {
    if (!this.signal || typeof this.signal.onMessage !== 'function' || this._rosterWired) return;
    this._rosterWired = true;
    this.signal.onMessage((m) => {
      if (!m || !this.mesh || (m.room && m.room !== this.room)) return;
      if (m.kind !== 'peer' && m.kind !== 'joined' && m.kind !== 'leave') return;
      const roster = Array.isArray(m.roster) ? m.roster : [];
      this.mesh.configure(roster);
      this.emit('roster-signal', { kind: m.kind, from: m.from, roster: this.mesh.getPlayers() });
      if (this._meshStartRequested && !this._meshStarted && this.mesh.peerCount > 0) {
        this._startMesh().catch((e) => this.emit('error', 'mesh start: ' + e.message));
      }
    });
  }

  /** 3T：roster 到达且 startNet() 已请求后，真正启动全部链路。 */
  async _startMesh() {
    if (!this.mesh || this._meshStarted) return this.net;
    this._meshStarted = true;
    const r = await this.mesh.start();
    this.mesh.startHeartbeat(1000);
    this.mesh.onQuality((ev) => this.emit('route-quality', { status: 'evaluate', ...ev }));
    this.mesh.onHandoff((snap) => this.emit('route-handoff', { status: 'update', snapshot: snap }));
    if (this.role === 'host') this.mesh.sendTiming(this.clock.delay);
    this.emit('open', { player: this.player, room: this.room, mesh: true, peers: this.mesh.getPlayers() });
    this._broadcastLoadout();
    if (this.role === 'host') this.mesh.sendSeed(this.getState().seed);
    return r;
  }

  /** 传输层回调接线：1v1 与 mesh 共用（NetMesh 对外接口与 NetPeer 同形）。 */
  _wireNet() {
    this.net.onOpen(() => {
      this.emit('open', { player: this.player, room: this.room });
      this._broadcastLoadout();
      if (this.role === 'host') this.net.sendSeed(this.getState().seed);
    });

    this.net.onLoadout((who, text) => {
      let obj;
      try { obj = JSON.parse(text); } catch { this.emit('error', 'bad loadout json'); return; }
      if (obj && obj.character) {
        // 3BC：Lua 侧空表经 Serialize 变成 {} —— 不能直接 .map，非数组按空处理。
        const systems = Array.isArray(obj.systems) ? obj.systems.map(Number) : [];
        this.call('BRO_net_set_loadout', who, obj.character, systems, obj.index || 1);
      }
      this.roster[who] = obj;
      if (obj && obj.seed !== undefined) this.call('BRO_net_set_seed', Number(obj.seed) | 0);
      if (obj && obj.stage) this.call('BRO_net_set_slot', this.player); // 刷新本地应用
      this.emit('roster', { player: who, loadout: obj, roster: this.roster });
      this.emit('state', this.getState());
    });

    this.net.onReady((who, text) => {
      let obj = {};
      try { obj = JSON.parse(text); } catch { /* ignore */ }
      this.remoteReady[who] = !!obj.ready;
      this.emit('ready', { player: who, ready: this.remoteReady[who] });
    });

    this.net.onInput((frame, who, keys) => {
      // 按帧号入缓冲，不按到达顺序直接写 KeyState —— 这是帧同步的核心。
      this.buffer.push(who, frame, keys);
      // 3X：远端真值回填长归档对应帧的槽（tick 时该帧远端值可能尚未到达）。
      if (this.archive && who >= 0 && who < this.maxPlayers) {
        const row = this.archive.at(frame) || new Array(this.maxPlayers).fill(0);
        row[who] = keys;
        this.archive.record(frame, row);
      }
      this.stats.recv++;
      this.stats.lastRemoteFrame = frame;
      this.emit('input', { frame, player: who, keys,
        decoded: byteToKeyState(keys) });
    });

    if (typeof this.net.onAck === 'function') {
      this.net.onAck((frame) => {
        this._timingDirty = true;
        this.emit('ack', { frame, remoteAck: this.net.remoteAck });
      });
    }
    if (typeof this.net.onTiming === 'function') {
      this.net.onTiming((delay) => {
        // client 采纳 host 下发的统一延迟，不自己改。
        if (this.role !== 'host') this.clock.setDelay(delay);
        this.emit('timing', this.getTiming());
      });
    }

    if (typeof this.net.onPos === 'function') {
      this.net.onPos((frame, who, x, y) => {
        // 对端 player 世界坐标 → 写远端代理目标；x/y 由 tick 里的 step 插值逼近。
        this.setPlayerTarget(who, x, y, frame);
        this._remotePosCount++;
        this.emit('players', { frame, player: who, x, y, players: this.getPlayers() });
      });
    }

    if (typeof this.net.onGate === 'function') {
      this.net.onGate((startFrame, who) => {
        // 阶段 3J：client 采纳 host 下发的共同起跑帧号。
        // 只有已 armGate 的实例才生效，避免影响未启用栅栏的既有页面。
        if (this.role === 'host' || !this.gate.enabled) return;
        if (this.gate.startFrame !== startFrame) {
          this.gate.startFrame = startFrame >>> 0;
          this._gateDirty = true;
          this.emit('gate', this.getGate());
        }
      });
    }

    if (typeof this.net.onDigest === 'function') {
      this.net.onDigest((frame, who, hash, objCount) => {
        // 对端同帧状态摘要 → 写 Lua；随后对账（只有双方都有的帧才计入）。
        const f = Number(frame) | 0;
        const h = hash >>> 0;
        this.call('BRO_net_peer_digest', f, h);
        this._remoteDigests.set(f, h);
        this.emit('digest', { frame: f, player: who, hash: h, objCount });
        // 3L：分叉帧入回滚队列。用本端该帧已登记的 hash 直接比，避免 recent 只留 8 帧的盲区。
        const localRaw = Number(this.call('BRO_net_local_digest', f));
        if (localRaw >= 0 && h !== (localRaw >>> 0) && !this._divergenceAttempted.has(f)) {
          this._divergenceAttempted.add(f);
          this._divergenceEvents.push({ kind: 'digest', frame: f, local: localRaw >>> 0, remote: h });
        }
        const d = this.getDivergence();
        if (d.mismatched > 0 && !this._divergenceSeen) {
          this._divergenceSeen = true;
          this.emit('divergence', d);
        } else if (d.mismatched === 0) {
          this._divergenceSeen = false;
        }
      });
    }

  }

  /** 真正启动 WebRTC 协商；网页在信令 joined/peer 时机调用 */
  async startNet() {
    if (!this.net) await this.connect();
    if (this.mesh) {
      // mesh 必须等 roster 才知道连谁；这里只记请求，
      // 由 _wireRosterSignal 在 roster 到达时调 _startMesh()。
      this._meshStartRequested = true;
      if (this.mesh.peerCount > 0) return this._startMesh();
      return null;
    }
    const r = await this.net.start();
    // 心跳只用于 RTT 采样；开局前由 host 下发统一延迟，保证两端 playFrame 一致。
    if (typeof this.net.startHeartbeat === 'function') this.net.startHeartbeat(1000);
    // 3Q：持续质量监控 / 换路交接事件转发给网页（默认 net 未 arm，不会触发）。
    if (typeof this.net.onQuality === 'function') {
      this.net.onQuality((ev) => this.emit('route-quality', { status: 'evaluate', ...ev }));
    }
    if (typeof this.net.onHandoff === 'function') {
      this.net.onHandoff((snap) => this.emit('route-handoff', { status: 'update', snapshot: snap }));
    }
    if (this.role === 'host' && typeof this.net.sendTiming === 'function') {
      this.net.sendTiming(this.clock.delay);
    }
    return r;
  }

  sendReady(ready = true) { return this.net ? this.net.sendReady(ready) : false; }

  disconnect() {
    if (!this.net) return false;
    if (typeof this.net.stopHeartbeat === 'function') this.net.stopHeartbeat();
    this.net.close();
    this.net = null;
    this.mesh = null;
    this._meshStarted = false;
    this._meshStartRequested = false;
    this.emit('close', {});
    return true;
  }
}

export { KEY_BIT, keyStateToByte, byteToKeyState, MSG };
