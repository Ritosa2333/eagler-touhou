// TH13 多人 Runtime 桥（P5 轨道 B）。
// 语义与样板 ports/th15-mp/web/sdl-runtime/multiplayer.mjs 逐段对应；TH13
// 差异：无 Adonis 校准、SessionSetup 22 词线格式（SessionSetup.cpp）、
// 房间前缀 th13mp-、模式映射 adonisMode===1 → 1（纯延迟）其余 → 2（回滚）。
const BUILD_WORDS = 4;
const SETUP_WORDS = 22;
const SETUP_VERSION = 2;  // SessionSetup::Version（v2 预测/回滚线格式）
const ADONIS_MODE_DELAY = 1;
const WIRE_MODE_DELAY = 1;
const WIRE_MODE_ROLLBACK = 2;
const ROOM_PREFIX = 'th13mp-';

export function multiplayerBuildWords(manifest) {
  const digest = manifest?.execution?.sha256;
  if (manifest?.game !== 'th13' || manifest?.product !== 'th13mp' ||
      manifest?.variant !== 'multiplayer' || manifest?.features?.multiplayer !== true ||
      !/^[a-f0-9]{64}$/i.test(digest || '')) throw Error('TH13 多人 Runtime 身份无效');
  return Array.from({length: BUILD_WORDS}, (_, i) => parseInt(digest.slice(i * 8, i * 8 + 8), 16) >>> 0);
}

export function validateMultiplayerOptions(options = {}) {
  if (options.thpracEnabled) throw Error('TH13 多人模式不支持 THPrac');
  if (options.replayViewer === true) {
    if (options.netplayMode === 'lan' || options.netplaySpectator) throw Error('录像与联机不能同时启动');
    return {replay: true};
  }
  if (options.netplayMode !== 'lan') throw Error('请从多人房间启动');
  let url;
  try { url = new URL(options.netplayUrl); }
  catch { throw Error('TH13 多人连接地址无效'); }
  const room = url.searchParams.get('room'), run = url.searchParams.get('run');
  const count = options.netplayPlayerCount;
  const spectator = options.netplaySpectator === true;
  const seat = spectator ? 0 : options.netplayPlayer;
  // TH13 没有 Adonis：adonisMode 1 = 纯延迟（wire mode 1），其余 = 回滚（wire mode 2）。
  const mode = options.netplayAdonisMode === ADONIS_MODE_DELAY ? WIRE_MODE_DELAY : WIRE_MODE_ROLLBACK;
  const delay = options.netplayInputDelay ?? 2;
  const automatic = options.netplayInputDelayAuto ?? false;
  const reserve = options.netplayPredictionReserve ?? 2;
  if (!['ws:', 'wss:'].includes(url.protocol) || url.username || url.password || url.hash ||
      !room?.startsWith(ROOM_PREFIX) || !run || ![2, 3].includes(count) ||
      !Number.isInteger(seat) || seat < 0 || seat >= count ||
      !Number.isInteger(options.netplayDifficulty) || options.netplayDifficulty < 0 || options.netplayDifficulty > 5 ||
      !Number.isInteger(options.netplaySeed) || options.netplaySeed < 0 || options.netplaySeed > 65535 ||
      !Number.isInteger(delay) || delay < 0 || delay > 9 || typeof automatic !== 'boolean' || (automatic && delay) ||
      !Number.isInteger(reserve) || reserve < 1 || reserve > 2 ||
      (options.netplayPredictionLimit !== undefined &&
       options.netplayPredictionLimit !== (mode === WIRE_MODE_ROLLBACK ? 12 : 0)))
    throw Error('TH13 多人连接或输入时序配置无效');
  const loadouts = options.netplayLoadouts;
  // 席位选人：0..3（四角色，允许重复）；shot 不在 SessionSetup 线格式内，
  // 仅接受目录当前声明的 0（th15 的 shot 语义在此裁剪）。
  if (!Array.isArray(loadouts) || loadouts.length !== count ||
      loadouts.some(v => !v || !Number.isInteger(v.character) || v.character < 0 || v.character > 3 ||
        (v.shot !== undefined && v.shot !== 0)))
    throw Error('TH13 多人机体配置无效');
  if (spectator && (typeof options.netplaySpectatorId !== 'string' || !options.netplaySpectatorId))
    throw Error('观战身份缺失');
  if (options.netplayChallengeMode !== undefined && typeof options.netplayChallengeMode !== 'boolean')
    throw Error('挑战模式配置无效');
  return {url, room, run, count, seat, spectator, mode, delay, automatic, reserve, loadouts,
    challenge: options.netplayChallengeMode === true, replay: false};
}

export async function multiplayerSetupWords(options, buildWords, cryptoProvider = globalThis.crypto) {
  const c = validateMultiplayerOptions(options);
  if (c.replay) throw Error('录像不建立玩家连接');
  if (!Array.isArray(buildWords) || buildWords.length !== BUILD_WORDS || !buildWords.some(Boolean))
    throw Error('TH13 构建身份或连接配置无效');
  const digest = new DataView(await cryptoProvider.subtle.digest(
    'SHA-256', new TextEncoder().encode('th13mp:' + c.room + ':' + c.run)));
  // SessionSetup.cpp DecodeSessionSetup / EncodeSessionSetup 的 22 词布局：
  // 0 version, 1 player_count, 2 local_player, 3 difficulty, 4 seed,
  // 5/6 session_id lo/hi, 7 requested_delay(=input_delay), 8..10 selections,
  // 11..13 padding, 14 mode, 15 automatic, 16 prediction_reserve,
  // 17..20 build, 21 challenge。prediction_limit/measured_prediction 不在线上
  // 词表内（由 C++ 侧常量 MaxRollbackFrames 持有）。
  const words = new Array(SETUP_WORDS).fill(0);
  words[0] = SETUP_VERSION;
  words[1] = c.count;
  words[2] = c.seat;
  words[3] = options.netplayDifficulty;
  words[4] = options.netplaySeed;
  words[5] = digest.getUint32(0, true);
  words[6] = digest.getUint32(4, true) || 1;
  words[7] = c.delay;
  for (let i = 0; i < 3; i++) words[8 + i] = c.loadouts[i]?.character ?? 0;
  words[14] = c.mode;
  words[15] = +c.automatic;
  words[16] = c.reserve;
  for (let i = 0; i < BUILD_WORDS; i++) words[17 + i] = buildWords[i] >>> 0;
  words[21] = +c.challenge;
  return {config: c, words};
}

export function createMultiplayerBridge({core, getOptions, manifest, emit, onError, onExit, target = globalThis}) {
  const buildWords = multiplayerBuildWords(manifest);
  let active = false, receiveTransport = null, receiveHook = null, previousReceive = null;
  const error = () => {
    const read = typeof core._th13_mp_error === 'function' ? core._th13_mp_error : core._th13_error;
    const p = read ? read() : 0;
    if (!p) return '';
    const end = core.HEAPU8.indexOf(0, p);
    return new TextDecoder().decode(core.HEAPU8.subarray(p, end < 0 ? p + 256 : end));
  };
  const bytes = (value, call) => {
    const p = core._malloc(value.length);
    if (!p) throw Error('多人内存不足');
    try {
      core.HEAPU8.set(value, p);
      return call(p, value.length);
    } finally {
      core._free(p);
    }
  };
  const string = (value, call) => bytes(new TextEncoder().encode(value + '\0'), call);
  function detachWake() {
    if (receiveTransport?.onReceive === receiveHook) receiveTransport.onReceive = previousReceive;
    receiveTransport = receiveHook = previousReceive = null;
  }
  // 传输层收到包后唤醒单帧泵（th15 桥同款）；旁观者只收不玩，不装钩子。
  function attachWake() {
    if (getOptions().netplaySpectator || typeof core._th13_mp_wake !== 'function') return;
    detachWake();
    receiveTransport = target.__eaglerPeerTransport;
    if (!receiveTransport) return;
    const transport = receiveTransport, previous = transport.onReceive;
    previousReceive = previous;
    receiveHook = () => {
      if (!active || receiveTransport !== transport) return;
      previous?.();
      if (active && receiveTransport === transport) core._th13_mp_wake();
    };
    transport.onReceive = receiveHook;
  }
  async function launch() {
    const options = getOptions(), checked = validateMultiplayerOptions(options);
    if (checked.replay) return false;
    const {config, words} = await multiplayerSetupWords(options, buildWords);
    const data = new Uint32Array(words);
    if (!bytes(new Uint8Array(data.buffer), p => core._th13_mp_configure(p, words.length)))
      throw Error('TH13 多人会话配置失败：' + error());
    const connected = config.spectator
      ? string(options.netplayUrl, url => string(options.netplaySpectatorId,
          id => core._th13_mp_spectator_connect(url, id)))
      : string(options.netplayUrl, url => core._th13_mp_connect(url));
    if (!connected) throw Error('TH13 多人连接失败：' + error());
    // start 返回 0 = frame0 READY 门未闭合（每次调用内部 Pump + MarkLocalReady）。
    // 桥轮询到门闭合；错误面非空即硬失败，不吞成“永远未就绪”。
    const deadline = Date.now() + 60_000;
    let canStart = 0;
    for (;;) {
      if (core._th13_mp_start()) break;
      const message = error();
      if (message) throw Error(message);
      const at = typeof core._th13_mp_status === 'function' ? core._th13_mp_status() >>> 2 : 0;
      if (at) canStart = core.HEAPU32[at + 2];
      if (Date.now() > deadline)
        throw Error('TH13 多人会话启动超时（canStart=' + canStart + '）');
      await new Promise(done => setTimeout(done, 10));
    }
    active = true;
    target.__eaglerNetplayLanActive = true;
    target.__eaglerNetplaySpectator = config.spectator;
    target.__eaglerNetplayFailed = false;
    target.__eaglerNetplayInputDelayFrames = config.delay;
    target.__eaglerNetplayAdonisMode = config.mode;
    attachWake();
    return true;
  }
  function frame() {
    if (!active) return;
    const at = core._th13_mp_status() >>> 2;
    if (!at) return;
    const s = Array.from(core.HEAPU32.subarray(at, at + 24));
    // th13 的 ConfirmedThroughAllRemotes 在 frame0 前是 INVALID_FRAME；诊断
    // 全局按 th11 口径给 undefined，避免 launcher 显示 4294967295。
    target.__eaglerNetplayLanConfirmed = s[4] === 0xffffffff ? undefined : s[4];
    target.__eaglerNetplayLanFrame = s[3];
    if (typeof core._th13_mp_rollback_status === 'function') {
      const p = core._th13_mp_rollback_status() >>> 2;
      if (!p) return;
      const r = Array.from(core.HEAPU32.subarray(p, p + 9));
      target.__eaglerNetplayRollback = {enabled: !!r[1], corrections: r[2], replayedFrames: r[3],
        lastDepth: r[4], peakDepth: r[5], pending: r[6]};
      target.__eaglerNetplayLanRollback = r[2];
      target.__eaglerNetplayLanResimulated = r[3];
      target.__eaglerNetplayPresentationFps = r[8] / 100;
      target.__eaglerNetplayDiagnostics = {frame: s[3], confirmed: s[4], inputDelay: s[7],
        prediction: s[8], generation: s[12],
        rollback: {restores: r[2], resimulated: r[3], peakDepth: r[5], pending: r[6], barrier: r[7]},
        sent: s[15], received: s[16], repairs: s[17], spectatorPublisherFailed: !!s[22]};
    }
  }
  function save() {
    if (active && getOptions().replayViewer !== true && !getOptions().netplaySpectator &&
        !core._th13_mp_replay_save()) throw Error(error());
  }
  function retire() {
    detachWake();
    active = false;
    target.__eaglerNetplayLanActive = false;
  }
  function close() {
    retire();
    core._th13_mp_close();
  }
  // 录像回放入口：th15 从 FS 读档；TH13 的 Replay 由宿主 localStorage 持有，
  // 因此第三参可直传字节。stage 仅在 C++ 提供可选 select_stage 导出时转发
  //（正式导出表未列入，缺省时由录像自身的首关开始）。
  function openReplay(path, stage, replayBytes) {
    close();
    core._th13_loop_stop();
    if (!core._th13_initialize()) throw Error(error());
    const data = replayBytes ?? core.FS.readFile(
      String(path).startsWith('/') ? String(path) : '/save/' + String(path));
    if (!data?.length) throw Error('录像文件为空');
    const loaded = bytes(data, (p, n) => bytes(
      new Uint8Array(new Uint32Array(buildWords).buffer),
      b => core._th13_mp_replay_load(p, n, b)));
    if (!loaded || !core._th13_mp_start()) throw Error(error());
    if (typeof core._th13_mp_replay_seek_stage === 'function' && Number(stage) > 0 &&
        !core._th13_mp_replay_seek_stage(Number(stage))) throw Error(error());
    active = true;
    target.__eaglerNetplayFailed = false;
    target.__eaglerNetplaySpectator = true;
  }
  return {launch, frame, save, retire, close, openReplay, get active() { return active; }};
}
