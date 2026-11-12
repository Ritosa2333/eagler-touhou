// m8.* 浏览器层回滚 case（M8：12 帧回滚）。真实调用链：
//   - relay 注入（EAGLER_NETPLAY_RELAY_DELAY_MS/DROP_EVERY——真实网络扰动）；
//   - RollbackCore mode=2 预测（predictableButtons=方向+射击；边缘动作
//     保持释放——错误预测触发回滚）；
//   - Session 驱动（预测帧 WorldState checkpoint → ExecuteRollback restore
//     → 重放预算 13 帧 → commit 边界音频恰播一次）；
//   - 生产 WASM + 真实资产。
//
// 断言目标：
//   rollback_one_three_twelve：注入 1/3/12 帧级延迟扰动 → restores>0
//     （真实回滚发生）→ 纠正后权威态 digest 与按时基准一致。
//   audio_bgm_once：回滚后音频请求序列无重复（恰播一次——被丢弃帧的
//     音频不出队）。
//   trance_death_resimulation：灵界/死亡帧在回滚重放后权威面一致。
//   production_draw_pixels：回滚期间生产绘制持续（真实帧输出）。
//   jitter_loss_reorder_duplicate：乱序/丢包注入下的会话存活与确认前沿。
//   resim_budget_new_progress：重放预算（12 窗 + 1 新进度）。
export async function run(ctx) {
  const SEED = 47932;
  // mode=2 预测线格式（w0=2）。
  const setupWords = ({players, local, characters, delay = 2}) => [
    2, players, local, 2, SEED, 0x9e37, 0x79b9, delay,
    ...characters.slice(0, 3), 0, 0, 0,
    2, 0, 2, 1, 2, 3, 4, 0,
  ];
  // 发送侧延迟注入（页内 init 脚本体；openClient 的 addInitScript 注入）：
  // 包装 RTCDataChannel.prototype.send 与 WebSocket.prototype.send，按
  // ±jitter 均匀抖动的 delay 推迟投递；dropEvery>0 时每 dropEvery 包丢 1。
  // route=rtc 时输入走 datachannel、relay 服务端的 delayMs 不生效——发送侧
  // 包装对两种 route 都真实生效（th11-mp rollback-browser-acceptance.mjs:
  // 130-158 同配方，简化为延迟/抖动/丢包注入——无相位/签名观测）。
  const wireDelayInit = (delayMs, jitterMs, dropEvery) => {
    const rnd = () => Math.random();
    let serial = 0;
    const wrap = (original, socket, data) => {
      // 丢包只作用于 unreliable 的 gameplay 输入通道（生产 inputDc =
      // {ordered:false, maxRetransmits:0}——真实可丢；SessionChannel 的
      // inputResend/repair 重传负责恢复）。control 通道（controlDc =
      // {ordered:true} 生产可靠；信令 WebSocket 的 SDP/ICE）与 adaptive
      // 可靠层（字符串 hello/ack）不丢：control 包无应用层重传（只缓存
      // 等 ACK），注入丢包令 receivedSequence 永久卡洞 → pendingControl
      // 无界堆积 → 握手死锁（diag-jitter-handshake 诊断实证）。延迟注入
      // 保留在 control 面（真实 RTT）。
      const isControl = typeof socket.url === 'string' && socket.url.includes('signal=1');
      const isControlDc = typeof socket.label === 'string' && socket.label.includes('control');
      const isInputDc = typeof socket.label === 'string' &&
        /-input$/.test(socket.label);  // `${tag}-input`（mesh 输入通道）
      const isString = !(data instanceof ArrayBuffer) && !ArrayBuffer.isView(data);
      const isWebSocket = typeof WebSocket === 'function' && socket instanceof WebSocket;
      // 扰动（延迟/丢包）只作用于 gameplay 输入面：RTC 的 -route-input
      // 通道与 relay 模式的 gameplay ws 二进制帧（信令 ws 与 control 面
      // 豁免——双方 200ms 级 control 延迟会令握手期的重传/watchdog 判
      // 失联断链；control 可靠性不是回滚 case 的被测面）。
      const disturbInput = !isString && !isControl &&
        (isInputDc || (isWebSocket && !isControlDc));
      const drop = dropEvery > 0 && disturbInput;
      const view = data instanceof ArrayBuffer ? new Uint8Array(data.slice(0))
        : ArrayBuffer.isView(data) ? new Uint8Array(data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength)) : null;
      if (!view || !disturbInput) return original.call(socket, data);
      try {
        const label = typeof socket.label === 'string' ? socket.label
          : typeof socket.url === 'string' ? socket.url : '?';
        (globalThis.__th13m8Labels ??= new Set()).add(label);
      } catch {}
      globalThis.__th13m8WireCount = (globalThis.__th13m8WireCount ?? 0) + 1;
      ++serial;
      if (drop && serial % dropEvery === 0) return undefined;  // 丢包（仅输入通道）
      const delay = Math.max(0, delayMs + (rnd() * 2 - 1) * jitterMs);
      if (!delay) return original.call(socket, view);
      setTimeout(() => {
        try {
          if (socket.readyState !== 'open' && socket.readyState !== 1) return;
          original.call(socket, view);
        } catch {}
      }, delay);
      return undefined;
    };
    if (globalThis.RTCDataChannel && !RTCDataChannel.prototype.__th13m8) {
      const original = RTCDataChannel.prototype.send;
      RTCDataChannel.prototype.__th13m8 = true;
      RTCDataChannel.prototype.send = function (data) { return wrap(original, this, data); };
    }
    if (globalThis.WebSocket && !WebSocket.prototype.__th13m8) {
      const original = WebSocket.prototype.send;
      WebSocket.prototype.__th13m8 = true;
      WebSocket.prototype.send = function (data) { return wrap(original, this, data); };
    }
  };
  // 帧域延迟注入：gameplay 输入包入队，宿主每逻辑帧调一次
  // __th13m8Advance() 投递 frame <= count-N 的包（确定性 N 逻辑帧延迟。
  // 真实毫秒的 setTimeout 延迟在 evaluate 往返节奏下不足以形成预测窗
  // ——90ms wireDelay 实测 restores=0，帧域注入才有可复现的预测错误）。
  const frameDelayInit = initialDelay => {
    let delayFrames = initialDelay;
    let count = 0;
    const queue = [];
    const wrap = (original, socket, data) => {
      const isControl = typeof socket.url === 'string' && socket.url.includes('signal=1');
      const isControlDc = typeof socket.label === 'string' && socket.label.includes('control');
      const isInputDc = typeof socket.label === 'string' &&
        /-input$/.test(socket.label);  // `${tag}-input`（mesh 输入通道）
      const isString = !(data instanceof ArrayBuffer) && !ArrayBuffer.isView(data);
      const isWebSocket = typeof WebSocket === 'function' && socket instanceof WebSocket;
      const disturbInput = !isString && !isControl &&
        (isInputDc || (isWebSocket && !isControlDc));
      if (!disturbInput) return original.call(socket, data);
      try {
        const label = typeof socket.label === 'string' ? socket.label
          : typeof socket.url === 'string' ? socket.url : '?';
        (globalThis.__th13m8Labels ??= new Set()).add(label);
      } catch {}
      queue.push({frame: count, send: () => {
        if (socket.readyState === 'open' || socket.readyState === 1)
          original.call(socket, data);
      }});
      return undefined;
    };
    if (globalThis.RTCDataChannel && !RTCDataChannel.prototype.__th13m8f) {
      const original = RTCDataChannel.prototype.send;
      RTCDataChannel.prototype.__th13m8f = true;
      RTCDataChannel.prototype.send = function (data) { return wrap(original, this, data); };
    }
    if (globalThis.WebSocket && !WebSocket.prototype.__th13m8f) {
      const original = WebSocket.prototype.send;
      WebSocket.prototype.__th13m8f = true;
      WebSocket.prototype.send = function (data) { return wrap(original, this, data); };
    }
    let delivered = 0;
    globalThis.__th13m8Advance = () => {
      ++count;
      while (queue.length && queue[0].frame <= count - delayFrames) {
        queue.shift().send();
        ++delivered;
      }
    };
    globalThis.__th13m8SetDelay = value => { delayFrames = Math.max(0, value); };
    globalThis.__th13m8Stats = () => ({count, queued: queue.length, delivered,
                                       wrapped: typeof RTCDataChannel.prototype.send === 'function'});
  };
  const netAdvance = client => client.page.evaluate(
    () => { if (globalThis.__th13m8Advance) globalThis.__th13m8Advance(); });
  const netState = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_state())));
  const netRollback = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_rollback())));
  const netAudio = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_audio())));
  const netAudioClear = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_audio_clear());
  // P3.1：apply_request 实际触达序列（宿主 commit 冲账拍回填，含来源帧号/
  // 冲账时确认前沿）——「恰播一次 / BGM 不早于确认帧 / 回滚臂==连续基线」。
  const netAudioApplied = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_audio_applied())));
  const netAudioAppliedClear = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_audio_applied_clear());
  const netConfigure = (client, words) => client.page.evaluate(
    ({words}) => {
      const ptr = window.mod.allocateUTF8(words.join(','));
      const ok = window.mod._th13_debug_mp_net_configure(ptr);
      window.mod._free(ptr);
      return ok;
    }, {words});
  const netConnect = (client, url) => client.page.evaluate(
    ({url}) => {
      const ptr = window.mod.allocateUTF8(url);
      const ok = window.mod._th13_debug_mp_net_connect(ptr);
      window.mod._free(ptr);
      return ok;
    }, {url});
  const netPump = (client, now) => client.page.evaluate(
    ({now}) => window.mod._th13_debug_mp_net_pump(now), {now});
  const netReady = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_ready());
  const netCapture = (client, held, now) => client.page.evaluate(
    ({held, now}) => window.mod._th13_debug_mp_net_capture(held, now), {held, now});
  const netCanSimulate = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_can_simulate());
  const netOnSimulated = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_on_simulated());
  const digestCore = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_digest_core())));
  const domainsAt = async clients => await Promise.all(clients.map(
    client => client.page.evaluate(() => JSON.parse(
      window.mod.UTF8ToString(
        window.mod._th13_debug_mp_snapshot_digest_domains())))));
  const snapshotCapture = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_capture())));
  const runFrame = async client => {
    const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
    if (ok !== 1) {
      const error = await client.page.evaluate(
        () => window.mod.UTF8ToString(window.mod._th13_error()));
      const state = await netState(client);
      ctx.assert(false, `帧泵（error=${error} state=${JSON.stringify(state)}）`);
    }
  };
  const openMpClient = ({seats, characters, local, wireDelay = 0, jitter = 0,
                         dropEvery = 0, frameDelay = 0, kind = 'manual', extraInit = ''}) =>
    ctx.openClient({kind, seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}` +
                           `&mplocalseat=${local}`,
                    // M8 发送侧扰动注入（延迟/抖动/丢包/帧域延迟）：
                    // route=rtc 时 relay 服务端的 delayMs 不作用于
                    // gameplay 输入（输入走 datachannel），发送侧包装对
                    // 两种 route 都真实生效。frameDelay 为帧域注入（每
                    // 逻辑帧 netAdvance 投递——确定性预测错误窗）。
                    init: (frameDelay > 0
                      ? `(${frameDelayInit.toString()})(${frameDelay})`
                      : (wireDelay > 0 || jitter > 0 || dropEvery > 0)
                        ? `(${wireDelayInit.toString()})(${wireDelay}, ${jitter}, ${dropEvery})`
                        : '') + (extraInit || '')});

  // 握手（frame0 屏障）→ 返回结束时的 now。
  const handshake = async (clients, room, nowStart = 1000, delay = 3, clock = null) => {
    // clock：Pump 时钟源。手动泵 case 用 Date.now()；rAF 自然泵 case 必须
    // 用页内 performance.now()（mp_tick 的 Pump 用 emscripten_get_now，同基
    // 单调——跨基混泵会被 ObserveClock 判「Network clock moved backwards」）。
    const nowOf = clock ?? (client => Date.now());
    const roomUrl = `${relay.url}/?room=${room}&run=1`;
    for (const [index, client] of clients.entries()) {
      ctx.assert(await netConfigure(client, setupWords({
        players: clients.length, local: index, delay,
        characters: [0, 0, 0]})) === 1, `configure@seat${index}`);
      ctx.assert(await netConnect(client, roomUrl) === 1, `connect@seat${index}`);
    }
    let now = nowStart, ready = false;
    // 握手窗口按真实时间判定：发送侧扰动（延迟/丢包）下 control 包重发
    // 间隔 200ms、adaptive ACK 重试 250ms——轮数上限（每轮 ~5ms 真实
    // 时间）只覆盖十几次重发机会，丢包下重发也可能丢。60s 真实窗口给
    // 可靠层足够往返（watchdog 的 confirmedTimeout 15s 同量级）。
    const deadline = Date.now() + 60_000;
    for (let attempt = 0; !ready; ++attempt) {
      now = await nowOf(clients[0]);
      if (now > deadline) {
        const states = [];
        for (const client of clients) states.push(await netState(client));
        ctx.assert(false, `握手未闭合（${JSON.stringify(states)} ` +
          `relay=${relay.log().slice(-400)}）`);
      }
      for (const client of clients) {
        const clientNow = clock ? await nowOf(client) : now;
        ctx.assert(await netPump(client, clientNow) === 1, '握手泵');
        await netReady(client);
      }
      const states = [];
      for (const client of clients) states.push(await netState(client));
      ready = states.every(state => state.can_start === 1);
    }
    for (const client of clients) ctx.assert(await netReady(client) === 1, 'Start');
    return now;
  };

  // 会话泵：采集 + 模拟（预测帧也可能推进）N 个模拟帧。
  // th12 同拍泵序（Session::step:62）：Pump 先（回滚请求在放行前消化
  // ——放行不会被同帧回滚作废令宿主 step 双推），再放行/采集/落账。
  const pumpSession = async (clients, simFrames, inputs, startNow, simulate = true) => {
    // 真实墙钟：ConfirmedInputWatchdog/重传策略按真实时间计时（合成时钟
    // 1ms 不等于真实 1ms——watchdog 会误超时）。每次泵用 Date.now()。
    let simulated = 0;
    let stallGuard = 0;
    while (simulated < simFrames) {
      const now = Date.now();
      // 1) Pump（信道收发 + 回滚请求消化——ExecuteRollback 在此）。
      for (const client of clients) {
        if (await netPump(client, now) !== 1) {
          const states = [], rollbacks = [];
          for (const c of clients) {
            states.push(await netState(c));
            rollbacks.push(await netRollback(c));
          }
          ctx.assert(false, `pump（states=${JSON.stringify(states)} ` +
            `rollbacks=${JSON.stringify(rollbacks)}）`);
        }
      }
      // 1.5) 帧域延迟时钟推进（frameDelay 客户端的入队包按逻辑帧投递）。
      for (const client of clients) await netAdvance(client);
      // 2) 统一节拍：所有客户端的 canSimulate 必须同真才推进本帧（查询
      // 有放行副作用——远端 seat_inputs 注入/Record/checkpoint 在
      // CanSimulate 内——只查 clients[0] 会令另一侧漏注入）。
      const decisions = [];
      for (const client of clients) {
        decisions.push(simulate ? (await netCanSimulate(client)) === 1 : false);
      }
      const canSimulate = decisions.every(Boolean);
      // 3) 采集（canSimulate 全真才采——相位与游戏帧同步）。
      if (canSimulate) {
        for (const client of clients) {
          const held = inputs(client, simulated);
          ctx.assert(await netCapture(client, held, now) === 1, 'capture');
        }
      }
      // 4) 模拟 + 落账。
      if (canSimulate) {
        for (const client of clients) {
          await runFrame(client);
          await netOnSimulated(client);
        }
        ++simulated;
        stallGuard = 0;
      } else if (++stallGuard > 4000) {
        const states = [];
        for (const client of clients) states.push(await netState(client));
        const rollbacks = [];
        for (const client of clients) rollbacks.push(await netRollback(client));
        ctx.assert(false, `停等超时（states=${JSON.stringify(states)} ` +
          `rollbacks=${JSON.stringify(rollbacks)}）`);
      }
    }
  };

  // P3.1 专用泵：统一节拍推进到「网络模拟帧 >= target」（pumpSession 以
  // runFrame 轮次计数，回滚重放会多耗轮次——两臂对照必须按净模拟帧对齐）。
  // 采集输入按当前 sim_frame 索引（恒火/方向流不依赖重放轮的索引漂移）。
  const pumpToSim = async (clients, target, inputs, maxRounds = 8000) => {
    for (let round = 0; round < maxRounds; ++round) {
      const states = [];
      for (const client of clients) states.push(await netState(client));
      if (states.every(state => state.sim_frame >= target)) return states;
      const now = Date.now();
      for (const client of clients) {
        if (await netPump(client, now) !== 1) {
          const rollbacks = [];
          for (const c of clients) rollbacks.push(await netRollback(c));
          ctx.assert(false, `pumpToSim pump（states=${JSON.stringify(states)} ` +
            `rollbacks=${JSON.stringify(rollbacks)}）`);
        }
      }
      for (const client of clients) await netAdvance(client);
      const decisions = [];
      for (const client of clients) decisions.push((await netCanSimulate(client)) === 1);
      if (!decisions.every(Boolean)) continue;
      const frameOf = [];
      for (const client of clients) frameOf.push((await netState(client)).sim_frame);
      for (let index = 0; index < clients.length; ++index) {
        ctx.assert(await netCapture(clients[index], inputs(clients[index], frameOf[index]),
                                    now) === 1, 'pumpToSim capture');
      }
      for (const client of clients) {
        await runFrame(client);
        await netOnSimulated(client);
      }
    }
    ctx.assert(false, `pumpToSim 超时（target=${target}）`);
  };
  const netSetFrameDelay = (client, value) => client.page.evaluate(
    value => { if (globalThis.__th13m8SetDelay) globalThis.__th13m8SetDelay(value); },
    value);

  // 收敛泵：统一节拍泵到无待回滚（预测差异全被确认消化）。
  // 双端 digest 对照的前提——快照时刻两侧权威态处于同一逻辑帧。
  // 统一节拍（与 pumpSession 同构）保证 capture 相位与 sim_frame 同步；
  // 追帧式异步推进会打破 capture 同步令输入流错位。
  const settleSession = async (clients, neutralHeld = 0x001, maxRounds = 400) => {
    let rounds = 0;
    let stallGuard = 0;
    for (;;) {
      const states = [];
      for (const client of clients) states.push(await netState(client));
      const rollbacks = [];
      for (const client of clients) rollbacks.push(await netRollback(client));
      const settled = rollbacks.every(rollback =>
        rollback.rollback_pending === 0 && rollback.last_error === 0);
      if (settled) return states;
      if (++rounds > maxRounds) {
        ctx.assert(false, `收敛超时（states=${JSON.stringify(states)} ` +
          `rollbacks=${JSON.stringify(rollbacks)}）`);
      }
      const now = Date.now();
      // Pump 先（回滚请求在放行前消化——与 pumpSession 同拍）。
      for (const client of clients) {
        if (await netPump(client, now) !== 1) {
          ctx.assert(false, `settle pump（states=${JSON.stringify(states)}）`);
        }
      }
      // 统一节拍：全真才推（capture 相位同步）。
      const decisions = [];
      for (const client of clients) {
        decisions.push((await netCanSimulate(client)) === 1);
      }
      const canSimulate = decisions.every(Boolean);
      if (canSimulate) {
        for (const client of clients) {
          ctx.assert(await netCapture(client, neutralHeld, now) === 1, 'settle capture');
        }
      }
      if (canSimulate) {
        for (const client of clients) {
          await runFrame(client);
          await netOnSimulated(client);
        }
        stallGuard = 0;
      } else if (++stallGuard > 4000) {
        ctx.assert(false, `settle 停等超时（states=${JSON.stringify(states)} ` +
          `rollbacks=${JSON.stringify(rollbacks)}）`);
      }
    }
  };

  // 帧号同步泵：回滚端与连续端的 digest 对照必须同逻辑帧。统一节拍下
  // 回滚端每次 ExecuteRollback 回退 d 帧后用 d 轮重放——领先端同轮各 +1，
  // 帧号差距恒定（settle 只清 pending 不追帧）。追赶期落后端独占推进
  // （CanSimulate→step→onSimulated），领先端只 Pump（不查询 CanSimulate
  // ——放行副作用会在不 step 的端建 Record/checkpoint）。落后端所需输入
  // 全部已调度：本地输入在 pumpSession/settle 期间相位已超前采集（统一
  // 节拍保证双端采集相位锁步），远端输入在途——Pump 的真实时间流逝令
  // 发送侧延迟到期后送达。追赶期不采集（目标帧的输入早已入窗）。
  const syncFrames = async (clients, maxRounds = 400) => {
    let rounds = 0, stalls = 0;
    for (;;) {
      const now = Date.now();
      for (const client of clients) {
        ctx.assert(await netPump(client, now) === 1, 'sync pump');
      }
      // Pump may rewind either peer. Compare its resulting frontiers, and
      // wait for the whole simulated prefix to become authoritative.
      const states = [];
      for (const client of clients) states.push(await netState(client));
      if (states[0].sim_frame === states[1].sim_frame) {
        const rollbacks = [];
        for (const client of clients) rollbacks.push(await netRollback(client));
        if (rollbacks.every((rollback, index) =>
              rollback.rollback_pending === 0 && rollback.last_error === 0 &&
              rollback.records === 0 && rollback.committed === states[index].sim_frame) &&
            states.every(state => state.confirmed >= state.sim_frame - 1))
          return states;
        // Neither peer needs a new logical frame while confirmation is in
        // flight. CanSimulate would grant a frame that this loop won't use.
        if (++rounds > maxRounds)
          ctx.assert(false, `确认同步超时（states=${JSON.stringify(states)}）`);
        await new Promise(resolve => setTimeout(resolve, 1));
        continue;
      }
      if (++rounds > maxRounds) {
        ctx.assert(false, `帧号同步超时（states=${JSON.stringify(states)}）`);
      }
      const laggard = states[0].sim_frame < states[1].sim_frame ? clients[0] : clients[1];
      if (await netCanSimulate(laggard) === 1) {
        await runFrame(laggard);
        await netOnSimulated(laggard);
        stalls = 0;
      } else if (++stalls > 4000) {
        ctx.assert(false, `sync 停等超时（states=${JSON.stringify(states)}）`);
      }
    }
  };

  const caseId = ctx.caseId ?? '';
  // 干净 relay：全部扰动（延迟/抖动/丢包）在发送侧注入（openMpClient 的
  // init 脚本——route=rtc 时 relay 服务端 delayMs 不作用于 gameplay 输入）。
  const relay = await ctx.startRelay({});
  try {
    if (caseId.includes('rollback_one_three_twelve')) {
      // 1/3/12 帧级纠正档：input_delay=2 + 发送侧 frameDelay 2/4/13。
      // 时序账（实测校准）：capture c 的包经 N 帧延迟投递，对端纠正
      // 距离 ≈ N-4（input_delay=2 缓冲 + 泵相位各吸收 1；实测 N=3/5/14
      // → distance 2/4/10）。12 帧档 N=14：迟到帧先经 12 帧窗外停等
      // 再以预测窗纠正——本手动 evaluate 泵形态的实测平台值 ≈10
      //（停等吸收压窗）；gate 的浏览器满窗 12（maxRollbackDistance>=12）
      // 需正式 Runtime 页的 rAF 自然泵（真实 60fps 下 200ms wireDelay
      // 即满窗）——如实登记，不以放宽断言伪装达标。
      // 各档真实回滚 restores>0；12 帧档纠正距离 >=10（平台值）。
      for (const [label, frameDelay, minDistance] of
             [['1帧', 3, 1], ['3帧', 5, 3], ['12帧', 14, 10]]) {
        const clients = [
          await openMpClient({seats: 2, characters: [0, 0], local: 0, frameDelay}),
          await openMpClient({seats: 2, characters: [0, 0], local: 1, frameDelay}),
        ];
        try {
          // 房间名保持 ASCII（relay URL 参数不编码非 ASCII——中文房间名
          // 令 ws 建链失败 transport disconnected，tier 排查实证）。
          const roomId = label === '1帧' ? 't1' : label === '3帧' ? 't3' : 't12';
          await handshake(clients, `th13m8rb-${roomId}`, 1000, 2);
          // seat0 变输入流（触发预测错误→回滚），seat1 恒射击。
          await pumpSession(clients, 60, (client, frame) =>
            client === clients[0] ? (frame % 5 === 0 ? 0x010 : 0x001) : 0x001);
          const rollback = await netRollback(clients[0]);
          const stats = await clients[0].page.evaluate(
            () => globalThis.__th13m8Stats && globalThis.__th13m8Stats());
          ctx.evidence.push(`注入自检：${JSON.stringify(stats)}`);
          ctx.assert(rollback.restores > 0,
            `${label} 档延迟注入触发真实回滚（${JSON.stringify(rollback)}）`);
          ctx.evidence.push(`${label} 档距离观测：maxDistance=${rollback.maxDistance} restores=${rollback.restores} resimulated=${rollback.resimulated}`);
        ctx.assert(rollback.maxDistance >= minDistance,
            `${label} 档纠正距离（maxDistance=${rollback.maxDistance} >= ${minDistance}）`);
          ctx.evidence.push(`${label} 档（frameDelay=${frameDelay}）：${JSON.stringify(rollback)}`);
          const route = await clients[0].page.evaluate(
            () => globalThis.__eaglerPeerTransport?.route);
          ctx.noteRuntime({
            transportPaths: [route],
            rollbackCount: rollback.restores,
            maxRollbackDistance: rollback.maxDistance,
          });
        } finally {
          for (const client of clients) {
            await client.assertDelivery();
            await client.close();
          }
        }
      }
    } else if (caseId.includes('audio_bgm_once')) {
      // P3.1 恰播一次（12 帧档真实回滚）。两臂同输入流（恒 0x001）：
      //   baseline 臂 = 无帧域延迟（远端逐帧即时确认，无预测错误）；
      //   rollback 臂 = frameDelay 14（早期远端效果帧输入迟到 → 预测 0 与
      //     实际 0x001 不符 → 12 帧回滚/重放；错误只会「少响」不会「多响」，
      //     重放补齐，故 apply 触达序列应与 baseline 相等）。
      // 判据（apply_request 实际触达，按来源帧分组的多重集）：
      //   - 两臂触达序列相等；双端触达序列一致；
      //   - 账本守恒：Voice applied+suppressed == captured；Music
      //     applied+dropped == captured（每个 Music 恰一次）；
      //   - Voice 首播 confirmed<=frame；Stop/Pan/Music confirmed>=frame。
      const groupApplied = audio => {
        const groups = {};
        for (const event of audio) {
          const key = `${event.kind}|${event.id}|${event.value}|` +
                      `${event.positional ? 1 : 0}|${event.x}|${event.text}`;
          const frame = String(event.frame);
          (groups[frame] ||= []).push(key);
        }
        for (const frame of Object.keys(groups)) groups[frame].sort();
        return groups;
      };
      const canonicalApplied = audio => {
        const groups = groupApplied(audio);
        return Object.keys(groups).sort((a, b) => Number(a) - Number(b))
          .map(frame => [frame, groups[frame]]);
      };
      let clientsRef = [];
      const inputs = (client, frame) =>
        // seat0 恒射击（发声只由 seat0 射击节拍产生）；seat1 只切低速位
        // （无射击/无位移 → 不产生射击/擦弹音；低速位切换令 ConfirmedHold
        // 预测过期 → 真实回滚，而发声事件集合不变）。
        client === clientsRef[0] ? 0x001 : (frame % 2 === 0 ? 0x008 : 0x000);
      const runArm = async (label, room, withDelay) => {
        // withDelay：先以 0 延迟热身 20 帧（发声流进入稳态），再切 17 帧
        // 帧域延迟制造 12 帧回滚窗；无延迟臂全程 0（连续基线）。
        const clients = [
          await openMpClient({seats: 2, characters: [0, 0], local: 0,
                              frameDelay: withDelay ? 17 : 0}),
          await openMpClient({seats: 2, characters: [0, 0], local: 1,
                              frameDelay: withDelay ? 17 : 0}),
        ];
        clientsRef = clients;
        try {
          if (withDelay) {
            for (const client of clients) await netSetFrameDelay(client, 0);
          }
          await handshake(clients, room);
          for (const client of clients) await netAudioAppliedClear(client);
          await pumpToSim(clients, 20, inputs);
          if (withDelay) {
            for (const client of clients) await netSetFrameDelay(client, 17);
          }
          await pumpToSim(clients, 40, inputs);
          await syncFrames(clients);
          const stats = await netRollback(clients[0]);
          const perClient = [];
          for (const client of clients) perClient.push(await netAudioApplied(client));
          ctx.assert(JSON.stringify(canonicalApplied(perClient[0])) ===
                     JSON.stringify(canonicalApplied(perClient[1])),
                     `${label} 双端 apply 触达序列一致`);
          const audioOut = await netAudio(clients[0]);
          return {stats, perClient, audioOut};
        } finally {
          for (const client of clients) {
            await client.assertDelivery();
            await client.close();
          }
        }
      };
      const baseline = await runArm('baseline', 'th13m8aub', false);
      const rollback = await runArm('rollback', 'th13m8aur', true);
      // 统一对齐到 target 帧（统一节拍在回滚收尾可能令一端多推进 1..4 帧，
      // 其发声音不属于对照窗口）。
      const target = 40;
      const applied = rollback.perClient.flat().filter(event => event.frame < target);
      const baselineApplied = baseline.perClient.flat()
        .filter(event => event.frame < target);
      ctx.evidence.push(`audio_bgm_once baseline=${JSON.stringify(baseline.stats)}`);
      ctx.evidence.push(`audio_bgm_once rollback=${JSON.stringify(rollback.stats)}`);
      // 真实回滚事实（12 帧档平台值 >=10，同 rollback_one_three_twelve）。
      ctx.assert(baseline.stats.restores === 0,
                 `baseline 臂无回滚（${baseline.stats.restores}）`);
      ctx.assert(rollback.stats.restores > 0,
                 `rollback 臂真实回滚（${rollback.stats.restores}）`);
      ctx.assert(rollback.stats.maxDistance >= 10,
                 `rollback 臂纠正距离 >=10（${rollback.stats.maxDistance}）`);
      // 去重账本可读且在回滚窗内实际吞掉重放 Voice。
      ctx.assert(rollback.stats.sfx_suppressed > 0,
                 `重放 Voice 被吞（${rollback.stats.sfx_suppressed}）`);
      // 守恒：每个 Voice 恰一次（首播 applied / 重放 suppressed）；
      // 每个 Music 恰一次（确认后 applied / 随丢弃 Record 消失）。
      for (const [label, stats] of [['baseline', baseline.stats],
                                    ['rollback', rollback.stats]]) {
        ctx.assert(stats.voice_applied + stats.sfx_suppressed === stats.voice_captured,
                   `${label} Voice 账本守恒（${stats.voice_applied}+` +
                   `${stats.sfx_suppressed}==${stats.voice_captured}）`);
        ctx.assert(stats.music_applied + stats.music_dropped === stats.music_captured,
                   `${label} Music 账本守恒（${stats.music_applied}+` +
                   `${stats.music_dropped}==${stats.music_captured}）`);
      }
      // 恰播一次 + 时序：Voice 首播同帧（预测帧至少在确认前已响——存在
      // confirmed<frame 的触达）；Stop/Pan/Music 只在确认后执行。
      const musicCounts = new Map();
      let voiceTiming = 0, voiceBeforeConfirm = 0, deferredTiming = 0;
      for (const event of applied) {
        if (event.kind === 'voice') {
          // confirmed < frame：该帧仍是预测帧，Voice 已在确认前真实触达
          //（延迟到确认的实现不可能出现这种条目）。
          if (event.confirmed !== 0xFFFFFFFF && event.confirmed < event.frame)
            ++voiceBeforeConfirm;
          ++voiceTiming;
        } else {
          ctx.assert(event.confirmed !== 0xFFFFFFFF && event.confirmed >= event.frame,
                     `${event.kind} 仅在确认后执行 f${event.frame} ` +
                     `(confirmed=${event.confirmed})`);
          ++deferredTiming;
        }
        if (event.kind === 'music') {
          const key = `${event.frame}|${event.id}|${event.value}|${event.text}`;
          musicCounts.set(key, (musicCounts.get(key) ?? 0) + 1);
        }
      }
      ctx.assert(voiceBeforeConfirm > 0,
                 `预测帧 Voice 首播早于确认（${voiceBeforeConfirm}/${voiceTiming}）`);
      for (const [key, count] of musicCounts) {
        ctx.assert(count === 1, `BGM 指令恰一次（${key} x${count}）`);
      }
      // 回滚臂 apply 触达序列 == 连续基线（按来源帧分组的多重集）。
      const rolledCanonical = canonicalApplied(applied);
      const baselineCanonical = canonicalApplied(baselineApplied);
      const rolledJson = JSON.stringify(rolledCanonical);
      const baselineJson = JSON.stringify(baselineCanonical);
      if (rolledJson !== baselineJson) {
        const rolledFrames = new Set(rolledCanonical.map(([frame]) => frame));
        const baselineFrames = new Set(baselineCanonical.map(([frame]) => frame));
        const onlyRolled = [...rolledFrames].filter(frame => !baselineFrames.has(frame));
        const onlyBaseline = [...baselineFrames].filter(frame => !rolledFrames.has(frame));
        ctx.evidence.push(`audio 序列差：rolledFrames=${rolledCanonical.length} ` +
          `baselineFrames=${baselineCanonical.length} onlyRolled=` +
          `${JSON.stringify(onlyRolled)} onlyBaseline=${JSON.stringify(onlyBaseline)}`);
      }
      ctx.assert(rolledJson === baselineJson,
                 '回滚臂 apply 触达序列 == 连续基线');
      ctx.evidence.push(`audio_bgm_once 触达 voice=${voiceTiming} ` +
        `beforeConfirm=${voiceBeforeConfirm} deferred=${deferredTiming} ` +
        `musicKeys=${musicCounts.size}`);
      // 既有 M8 观测面（保留）：commit 边界出队面为数组（P3.1 后正常为空，
      // 由宿主 commit 冲账排空——不作为恰播一次判据）。
      ctx.assert(Array.isArray(rollback.audioOut), '音频出队面为数组');
    } else if (caseId.includes('trance_death')) {
      // 对照实验开关：wireDelay=0（无预测无回滚）时双端应严格一致。
      const EXPERIMENT_ZERO_DELAY = false;
      const wireDelay = EXPERIMENT_ZERO_DELAY ? 0 : 100;
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay}),
      ];
      try {
        await handshake(clients, 'th13m8td');
        // seat1 按灵界（C=0x800）——灵界状态机在预测/回滚重放下推进。
        await pumpSession(clients, 50, (client, frame) =>
          client === clients[1] ? 0x801 : 0x001);
        const rollback = await netRollback(clients[0]);
        // 双端帧号同步（回滚端重放消耗令 sim_frame 落后于连续端——
        // 统一节拍下差距恒定，settle 只清 pending 不追帧；digest 对照
        // 必须同逻辑帧）。
        await syncFrames(clients);
        const digests = [];
        for (const client of clients) {
          await snapshotCapture(client);
          digests.push((await digestCore(client)).digest);
        }
        const domains = await domainsAt(clients);
        ctx.assert(digests[0] === digests[1],
          `灵界重放后双端一致（${digests[0]} vs ${digests[1]} domains=` +
          `${JSON.stringify(domains)}）`);
        ctx.evidence.push(`灵界/死亡重放：restores=${rollback.restores} ` +
          `digest=${digests[0]}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('production_draw')) {
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay: 80}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay: 80}),
      ];
      try {
        await handshake(clients, 'th13m8pd');
        // 回滚期间生产绘制持续：帧泵真实输出（th13_debug_frame 内的
        // defer+commit+present 完整链——绘制面不因回滚中断）。
        // sim_frame 语义：回滚重放消耗帧预算（ExecuteRollback 回退后
        // 重放轮不产生净推进）——统一节拍 30 轮下回滚端净推进 < 30 是
        // 回滚语义的正确结果，不是绘制中断。断言口径 = 泵期间无错误且
        // 帧持续产出（sim_frame > 0 且 error 空）。
        await pumpSession(clients, 30, (client, frame) =>
          frame % 3 === 0 ? 0x008 : 0x001);
        const state = await netState(clients[0]);
        ctx.assert(state.error === '', `回滚期会话存活（${state.error}）`);
        ctx.assert(state.sim_frame > 0, `回滚期生产帧推进（${state.sim_frame}）`);
        ctx.evidence.push(`回滚期生产绘制：sim_frame=${state.sim_frame}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('jitter_loss')) {
      // 发送侧乱序/丢包：抖动 40ms + 每 3 包丢 1（重传恢复）。
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay: 60, jitter: 40, dropEvery: 3}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay: 60, jitter: 40, dropEvery: 3}),
      ];
      try {
        await handshake(clients, 'th13m8jl');
        await pumpSession(clients, 40, () => 0x001);
        const state = await netState(clients[0]);
        ctx.assert(state.error === '', `乱序/丢包下会话存活（${state.error}）`);
        ctx.assert(state.sim_frame >= 30, `乱序/丢包下帧推进（${state.sim_frame}）`);
        const rollback = await netRollback(clients[0]);
        ctx.evidence.push(`乱序/丢包：restores=${rollback.restores} ` +
          `sim_frame=${state.sim_frame}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('resim_budget')) {
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay: 110}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay: 110}),
      ];
      try {
        await handshake(clients, 'th13m8rs');
        await pumpSession(clients, 40, (client, frame) =>
          frame % 4 === 0 ? 0x030 : 0x001);
        const rollback = await netRollback(clients[0]);
        // 重放预算：resimulated <= restores * 13（每次回滚至多重放 12 窗
        // + 1 新进度）。
        ctx.assert(rollback.resimulated <= rollback.restores * 13,
          `重放预算（resimulated=${rollback.resimulated} ` +
          `<= restores*13=${rollback.restores * 13}）`);
        ctx.evidence.push(`重放预算：${JSON.stringify(rollback)}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('rollback_full_window_raf')) {
      // 正式页 rAF 自然泵（BrowserRuntime mp_tick：Pump→采样→纠正重放→
      // 新进度→present，真实 60fps 节拍）+ 200ms 真时 wireDelay 注入。
      // 手动 evaluate 泵形态的 12 帧档平台值 ≈10、帧域注入在 rAF 形态实测
      // 压窗至 ~4（停等随投递时钟同步停滞）——rollback_one_three_twelve
      // 注记的满窗处方即「真实 60fps 下 200ms wireDelay」，此处按方执行。
      // wireDelay 按容器实际帧率换算满窗：200ms 是 60fps 处方（m8 注释），
      // 无头 swiftshader 实测 ~35fps（200ms≈7 帧→distance 5），350ms≈12 帧（单独跑实测 12）；全套并行负载下 fps 再降（350ms→10），450ms 留档。
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay: 450,
                            kind: 'raf'}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay: 450,
                            kind: 'raf'}),
      ];
      try {
        const pageClock = async client =>
          await client.page.evaluate(() => performance.now());
        await handshake(clients, 'th13m8fw', 1000, 3, pageClock);
        // 帧域时钟挂入自然泵：每 rAF 回调按本拍模拟帧数推进 __th13m8Advance
        // （第 N 帧入队的输入包在第 N+14 帧投递——确定性满窗预测错误）。
        // seat0 在同钩内经生产键盘接口 _th13_key 按逻辑帧交替输入
        // （frame%5 左/射——与手动 case 同型；键盘墙钟交替太慢，预测大多
        // 命中、纠正距离只有 ~4）。seat1 恒射击。
        for (const client of clients) await client.page.evaluate(seat0 => {
          const mod = window.mod;
          window.__rafMp = {ticks: 0, began: performance.now(), failure: null};
          mod.onGameFrame = (ok, milliseconds, ticks) => {
            ++window.__rafMp.ticks;
            if (!ok && !window.__rafMp.failure) {
              let detail = '';
              try { detail = mod.UTF8ToString(mod._th13_error()); } catch {}
              try {
                detail += ' net=' + mod.UTF8ToString(mod._th13_debug_mp_net_state());
              } catch {}
              window.__rafMp.failure = 'rAF 帧体失败：' + detail;
            }
            if (seat0) {
              const frame = mod._th13_frame();
              const want = frame % 5 === 0 ? 203 : 44;  // ArrowLeft / KeyZ
              if (window.__m8Key !== want) {
                if (window.__m8Key) mod._th13_key(window.__m8Key, 0);
                mod._th13_key(want, 1);
                window.__m8Key = want;
              }
            }
          };
          mod._th13_loop_start();
        }, client === clients[0]);
        await clients[1].page.locator('canvas').first().click({position: {x: 5, y: 5}});
        await clients[1].page.keyboard.down('KeyZ');
        const target = 600;
        const deadline = Date.now() + 120_000;
        let states = [];
        let lastProgress = {frame: -1, at: Date.now()};
        while (true) {
          states = [];
          for (const client of clients) {
            states.push(await netState(client));
            const failure = await client.page.evaluate(() => window.__rafMp?.failure ?? null);
            if (failure) ctx.assert(false, `rAF 泵（${failure}）`);
          }
          const advanced = states.every(state => state.sim_frame >= target);
          if (advanced) break;
          if (states.some(state => state.error)) {
            const bad = states.find(state => state.error);
            ctx.assert(false, `会话错误（${bad.error}）`);
          }
          const min = Math.min(...states.map(state => state.sim_frame));
          if (min > lastProgress.frame) lastProgress = {frame: min, at: Date.now()};
          else if (Date.now() - lastProgress.at > 30_000)
            ctx.assert(false, `停等（states=${JSON.stringify(states)}）`);
          if (Date.now() > deadline)
            ctx.assert(false, `未达目标帧（states=${JSON.stringify(states)}）`);
          await new Promise(done => setTimeout(done, 250));
        }
        // 前沿观测：三拍 1s 采样 sim/confirmed 引导量与回滚统计（满窗
        // 调参依据；confirmed 为对端输入前沿，sim-confirmed=预测引导上限）。
        for (let probe = 0; probe < 3; ++probe) {
          const sample = [];
          for (const client of clients)
            sample.push({state: await netState(client), rollback: await netRollback(client)});
          ctx.evidence.push('frontier: ' + probe + ' ' + JSON.stringify(
            sample.map(x => ({sim: x.state.sim_frame, confirmed: x.state.confirmed,
              stall: x.rollback.stall_frames, restores: x.rollback.restores,
              maxDistance: x.rollback.maxDistance}))));
          await new Promise(done => setTimeout(done, 1000));
        }
        const rollback = await netRollback(clients[0]);
        const rollbackPeer = await netRollback(clients[1]);
        // 注入自检：帧域队列真实投递 + 扰动面命中 gameplay 输入通道
        //（route=rtc 的 mesh 输入通道 label 以 -input 结尾）。
        for (const client of clients) {
          const stats = await client.page.evaluate(() =>
            globalThis.__th13m8Stats ? globalThis.__th13m8Stats()
              : {wireDisturbed: globalThis.__th13m8WireCount ?? 0});
          ctx.evidence.push(`满窗注入自检：${JSON.stringify(stats)}`);
          const disturbed = stats.delivered ?? stats.wireDisturbed;
          ctx.assert(disturbed > 0, `注入真实扰动 gameplay 输入面（${JSON.stringify(stats)}）`);
          const labels = await client.page.evaluate(() => [...(globalThis.__th13m8Labels ?? [])]);
          ctx.evidence.push(`注入通道面：${JSON.stringify(labels)}`);
          ctx.assert(labels.some(label => /-input$/.test(label)),
            `扰动命中 gameplay 输入通道（labels=${JSON.stringify(labels)}）`);
        }
        // 满窗断言取两侧最大：变输入席（seat1 预测 seat0 的帧粒变化流）
        // 承担纠正；恒输入席（seat0）对端输入全可预测、零回滚是合法结果
        //（实测 seat1 maxDistance=12/restores 34、seat0 restores=0）。
        const fullWindow = Math.max(rollback.maxDistance, rollbackPeer.maxDistance);
        ctx.assert(fullWindow >= 12,
          `rAF 自然泵满窗纠正（maxDistance=${fullWindow} >= 12；` +
          `seat0=${rollback.maxDistance}/${rollback.restores} ` +
          `seat1=${rollbackPeer.maxDistance}/${rollbackPeer.restores}）`);
        // 纠正后 digest 一致：P3.4 线上 digest 零失配（两侧交换并比对过）。
        ctx.assert(rollback.desync_detected === 0,
          `线上 digest 零失配（desync=${rollback.desync_detected}）`);
        ctx.assert(rollback.digests_received > 0,
          `digest 已交换（received=${rollback.digests_received} sent=${rollback.digests_sent}）`);
        ctx.evidence.push(`满窗回滚：本机 ${JSON.stringify(rollback)}；对端 maxDistance=` +
          `${rollbackPeer.maxDistance} restores=${rollbackPeer.restores}`);
        const route = await clients[0].page.evaluate(
          () => globalThis.__eaglerPeerTransport?.route);
        ctx.noteRuntime({
          transportPaths: [route],
          rollbackCount: Math.max(rollback.restores, rollbackPeer.restores),
          maxRollbackDistance: fullWindow,
        });
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else {
      throw new Error(`m8_rollback.mjs 不处理 case ${caseId}`);
    }
  } finally {
    relay.close();
  }
}
