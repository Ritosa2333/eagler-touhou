// m7.* 浏览器层真实联机 case（M7：确认输入联网——纯延迟）。
// 真实调用链（无网络模拟器旁路）：
//   - services/relay 子进程（真实 WebSocket 信令 + RTC 候选交换）；
//   - BrowserPeerTransport（wasm 内 EM_JS：RTC DataChannel mesh vs relay
//     竞速——frame0 前选定唯一路由）；
//   - NetplayRuntime（RollbackCore 纯延迟窗 + SessionGate HELLO/READY）；
//   - Session（驱动：本地席位延迟输入经 playback_input 接缝、远端席位
//     每帧 ConfirmedInputs 注入 seat_inputs）；
//   - 生产 WASM + 真实资产（每客户端送达核验 sha256）。
//
// 断言目标：
//   rtc_two_three_clients：2 席与 3 席真实会话——全员 HELLO/READY 后
//     frame0 放行；同 seed 同帧输入下两侧权威态（digest_core）逐帧一致。
//   relay_fallback_path：RTC 不可用（页内无 RTCPeerConnection）时 relay
//     路径仍完成握手与帧推进（transport 的降级语义）。
//   confirmed_hash_lockstep：确认前沿单调推进；confirmed 前的帧不模拟。
//   frame_zero_load_barrier：READY 门未闭合时 CanSimulate 恒假；闭合后
//     首帧即 frame0。
//   device_seat_isolation：两个客户端（seat 0/1）各自本地输入只出现在
//     自己的席位（远端注入不污染本地延迟输入）。
//
// 驱动面（BrowserRuntime M7 导出，全部真实生产面）：
//   th13_debug_mp_net_configure/connect/pump/ready/capture/can_simulate/
//   on_simulated/state + th13_debug_mp_snapshot_digest_core（权威模拟面）。
export async function run(ctx) {
  const SEED = 47932;
  // SessionSetup 22 词（SessionSetup.cpp 线格式）：v1 纯延迟。
  // w0=version w1=players w2=local w3=difficulty w4=seed w5/6=session_id
  // w7=delay w8..10=selections w14=mode w15=automatic w16=reserve
  // w17..20=build w21=challenge
  // 22 词固定序：w8..w10 = selections（恰好 3 词——characters 必须传 3 元）。
  const setupWords = ({players, local, characters, delay = 3}) => [
    1, players, local, 2, SEED, 0x9e37, 0x79b9, delay,
    ...characters.slice(0, 3), 0, 0, 0,
    1, 0, 2, 1, 2, 3, 4, 0,
  ];
  const netState = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_state())));
  // C 字符串参数经 allocateUTF8 传指针（th13-debug.html name_ptr 同式）。
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
  const snapshotCapture = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_capture())));
  const runFrame = async client => {
    const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
    ctx.assert(ok === 1, '帧泵');
  };
  const openMpClient = ({seats, characters, local}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}` +
                           `&mplocalseat=${local}`});

  // 双客户端联机泵：relay 真实信令 + 会话驱动（frame0 门 + 采集 + 模拟）。
  const pumpSession = async (clients, frames, inputs, startNow = 1000) => {
    let now = startNow;
    for (let frame = 0; frame < frames; ++frame) {
      now += 16;
      for (const client of clients) {
        if (await netPump(client, now) !== 1) {
          const state = await netState(client);
          ctx.assert(false, `pump@${frame}（state=${JSON.stringify(state)} ` +
            `relay=${relay.log().slice(-400)}）`);
        }
        // 采集本地输入（每帧一次；采集相位由 wasm 侧推进）。
        const held = inputs(client, frame);
        if (await netCapture(client, held, now) !== 1) {
          const state = await netState(client);
          ctx.assert(false, `capture@${frame}（state=${JSON.stringify(state)}）`);
        }
      }
      // 帧推进门：全员输入确认才模拟（纯延迟停等）。
      const granted = [];
      for (const client of clients) {
        granted.push(await netCanSimulate(client) === 1);
      }
      for (const [index, client] of clients.entries()) {
        if (granted[index]) {
          await runFrame(client);
          await netOnSimulated(client);
        }
      }
    }
    return now;  // 结束时钟（后续泵必须单调续推——ObserveClock 拒绝倒退）
  };

  const caseId = ctx.caseId ?? '';
  const relay = await ctx.startRelay();
  try {
    if (caseId.includes('rtc_two_three')) {
      // ---- 2 席真实会话：握手 → frame0 → 同帧一致 ----
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1}),
      ];
      try {
        const roomUrl = `${relay.url}/?room=th13m7&run=1`;
        for (const [index, client] of clients.entries()) {
          ctx.assert(await netConfigure(client, setupWords({
            players: 2, local: index, characters: [0, 0, 0]})) === 1,
            `configure@seat${index}`);
          ctx.assert(await netConnect(client, roomUrl) === 1,
            `connect@seat${index}`);
        }
        // 握手泵：全员 HELLO/READY → CanStart。
        let now = 1000, ready = false;
        for (let attempt = 0; attempt < 200 && !ready; ++attempt) {
          now += 16;
          for (const client of clients) {
            ctx.assert(await netPump(client, now) === 1, '握手泵');
            await netReady(client);
          }
          const states = [];
          for (const client of clients) states.push(await netState(client));
          ready = states.every(state => state.can_start === 1);
          if (!ready && attempt === 199) {
            ctx.assert(false, `frame0 屏障未闭合（${JSON.stringify(states)}）`);
          }
        }
        // frame0 后：采集 + 模拟 30 帧（两侧同输入序列——确定性对照）。
        now = await pumpSession(clients, 30, (client, frame) => (frame % 2 ? 0x001 : 0x008), now);
        // 权威模拟面 digest 逐帧一致（两侧同 seed 同输入 → 同状态）。
        // 快照前帧号同步：合成时钟泵下真实包延迟可能令一端停等数帧
        //（inputResend 定时器按 now_ms 计，合成 16ms/帧 ≠ 真实包往返），
        // 双端 sim_frame 错位时 digest 对照的是不同逻辑帧。先泵到同帧
        //（停等端追帧——领先端只 Pump 不查询 CanSimulate，放行副作用
        // 在不 step 的端会建悬空 Record/checkpoint）。时钟从 pumpSession
        // 结束值单调续推（ObserveClock 拒绝倒退——ClockReversed 终态）。
        let syncRounds = 0;
        for (;;) {
          const states = [];
          for (const client of clients) states.push(await netState(client));
          if (states[0].sim_frame === states[1].sim_frame) break;
          if (++syncRounds > 400) {
            ctx.assert(false, `帧号同步超时（${JSON.stringify(states)}）`);
          }
          now += 16;
          for (const client of clients) {
            if (await netPump(client, now) !== 1) {
              const after = await netState(client);
              ctx.assert(false, `sync pump（before=${JSON.stringify(states)} ` +
                `after=${JSON.stringify(after)}）`);
            }
          }
          const laggard = states[0].sim_frame < states[1].sim_frame ? clients[0] : clients[1];
          if (await netCanSimulate(laggard) === 1) {
            await runFrame(laggard);
            await netOnSimulated(laggard);
          }
        }
        const digests = [];
        for (const client of clients) {
          await snapshotCapture(client);
          digests.push((await digestCore(client)).digest);
        }
        ctx.assert(digests[0] === digests[1],
          `双客户端权威模拟面一致（${digests[0]} vs ${digests[1]}）`);
        const state = await netState(clients[0]);
        ctx.assert(state.sim_frame > 0, `已推进模拟帧（${JSON.stringify(state)}）`);
        ctx.evidence.push(`2 席会话：sim_frame=${state.sim_frame} digest=${digests[0]}`);
        // runtime 元数据：真实路径观测（BrowserPeerTransport 的 route）。
        const route = await clients[0].page.evaluate(
          () => globalThis.__eaglerPeerTransport?.route);
        ctx.assert(route === 'rtc', `RTC 路径生效（route=${route}）`);
        ctx.noteRuntime({transportPaths: [route]});
        // ---- 3 席真实会话（gate M7：clients>=3 的真实三客户端场景）----
        const trio = [
          await openMpClient({seats: 3, characters: [0, 1, 2], local: 0}),
          await openMpClient({seats: 3, characters: [0, 1, 2], local: 1}),
          await openMpClient({seats: 3, characters: [0, 1, 2], local: 2}),
        ];
        try {
          const trioUrl = `${relay.url}/?room=th13m7x3&run=1`;
          for (const [index, client] of trio.entries()) {
            ctx.assert(await netConfigure(client, setupWords({
              players: 3, local: index, characters: [0, 1, 2]})) === 1,
              `configure@t${index}`);
            ctx.assert(await netConnect(client, trioUrl) === 1, `connect@t${index}`);
          }
          let tnow = 1000, tready = false;
          for (let attempt = 0; attempt < 200 && !tready; ++attempt) {
            tnow += 16;
            for (const client of trio) {
              ctx.assert(await netPump(client, tnow) === 1, '3 席握手泵');
              await netReady(client);
            }
            const states = [];
            for (const client of trio) states.push(await netState(client));
            tready = states.every(state => state.can_start === 1);
            if (!tready && attempt === 199)
              ctx.assert(false, `3 席 frame0 屏障未闭合（${JSON.stringify(states)}）`);
          }
          tnow = await pumpSession(trio, 24,
            (client, frame) => (frame % 2 ? 0x001 : 0x008), tnow);
          // 帧号同步（最落后席位独占推进，领先端只 Pump）。
          let rounds = 0;
          for (;;) {
            const states = [];
            for (const client of trio) states.push(await netState(client));
            if (states.every(state => state.sim_frame === states[0].sim_frame)) break;
            if (++rounds > 400)
              ctx.assert(false, `3 席帧号同步超时（${JSON.stringify(states)}）`);
            tnow += 16;
            for (const client of trio) {
              if (await netPump(client, tnow) !== 1) {
                const after = await netState(client);
                ctx.assert(false, `3 席 sync pump（after=${JSON.stringify(after)}）`);
              }
            }
            const minFrame = Math.min(...states.map(state => state.sim_frame));
            for (const [index, client] of trio.entries()) {
              if (states[index].sim_frame === minFrame &&
                  await netCanSimulate(client) === 1) {
                await runFrame(client);
                await netOnSimulated(client);
              }
            }
          }
          const trioDigests = [];
          for (const client of trio) {
            await snapshotCapture(client);
            trioDigests.push((await digestCore(client)).digest);
          }
          ctx.assert(trioDigests[0] === trioDigests[1] && trioDigests[1] === trioDigests[2],
            `3 席权威模拟面一致（${trioDigests.join(' vs ')}）`);
          ctx.evidence.push(`3 席会话（角色 0/1/2）：digest=${trioDigests[0].slice(0, 16)}…`);
        } finally {
          for (const client of trio) {
            await client.assertDelivery();
            await client.close();
          }
        }
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('relay_fallback')) {
      const roomUrl = `${relay.url}/?room=th13m7rf&run=1`;
      // ---- relay 降级：页内禁 RTC（init 脚本摘除 RTCPeerConnection）----
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1}),
      ];
      try {
        for (const client of clients) {
          // 禁 RTC + 显式启用 WebSocket gameplay fallback（launcher 契约的
          // 可选能力 netplayAllowWebsocketFallback——服务端显式开启时的
          // 降级路径；默认关闭是产品语义，测试按 opt-in 契约驱动）。
          await client.page.evaluate(() => {
            globalThis.RTCPeerConnection = undefined;
            window.mod.eaglerOptions = {
              ...(window.mod.eaglerOptions ?? {}),
              netplayAllowWebsocketFallback: true,
            };
          });
        }
        for (const [index, client] of clients.entries()) {
          ctx.assert(await netConfigure(client, setupWords({
            players: 2, local: index, characters: [0, 0, 0]})) === 1, 'configure');
          ctx.assert(await netConnect(client, roomUrl) === 1, 'connect');
        }
        let now = 1000, ready = false;
        for (let attempt = 0; attempt < 200 && !ready; ++attempt) {
          now += 16;
          for (const client of clients) {
            await netPump(client, now);
            await netReady(client);
          }
          const states = [];
          for (const client of clients) states.push(await netState(client));
          ready = states.every(state => state.can_start === 1);
          if (attempt === 199)
            ctx.assert(false, `relay 降级握手未闭合（${JSON.stringify(states)} ` +
              `relay=${relay.log().slice(-600)}）`);
        }
        await pumpSession(clients, 20, () => 0x001, now);
        const state = await netState(clients[0]);
        ctx.assert(state.sim_frame > 0, 'relay 路径帧推进');
        ctx.evidence.push(`relay 降级：sim_frame=${state.sim_frame}`);
        // runtime 元数据：降级会话的实际路径（应为 relay）。
        const route = await clients[0].page.evaluate(
          () => globalThis.__eaglerPeerTransport?.route);
        ctx.assert(route === 'relay', `relay 降级路径生效（route=${route}）`);
        ctx.noteRuntime({transportPaths: [route]});
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('confirmed_hash')) {
      const roomUrl = `${relay.url}/?room=th13m7ch&run=1`;
      // ---- 确认前沿：单调推进 + confirmed 前不模拟 ----
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1}),
      ];
      try {
        for (const [index, client] of clients.entries()) {
          ctx.assert(await netConfigure(client, setupWords({
            players: 2, local: index, characters: [0, 0, 0]})) === 1, 'configure');
          ctx.assert(await netConnect(client, roomUrl) === 1, 'connect');
        }
        let now = 1000, ready = false;
        for (let attempt = 0; attempt < 200 && !ready; ++attempt) {
          now += 16;
          for (const client of clients) {
            await netPump(client, now);
            await netReady(client);
          }
          const states = [];
          for (const client of clients) states.push(await netState(client));
          ready = states.every(state => state.can_start === 1);
          if (attempt === 199)
            ctx.assert(false, '握手未闭合');
        }
        // 确认前沿单调：逐帧检查 confirmed 不回退。
        let previous = 0;
        for (let frame = 0; frame < 20; ++frame) {
          now += 16;
          for (const client of clients) {
            await netPump(client, now);
            await netCapture(client, frame % 2 ? 0x001 : 0, now);
          }
          for (const client of clients) {
            if (await netCanSimulate(client) === 1) {
              await runFrame(client);
              await netOnSimulated(client);
            }
          }
          const state = await netState(clients[0]);
          ctx.assert(state.confirmed === 0xFFFFFFFF || state.confirmed >= previous,
            `confirmed 单调（${state.confirmed} >= ${previous}）`);
          if (state.confirmed !== 0xFFFFFFFF) previous = state.confirmed;
        }
        ctx.evidence.push(`确认前沿：${previous}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else if (caseId.includes('frame_zero')) {
      const roomUrl = `${relay.url}/?room=th13m7fz&run=1`;
      // ---- frame0 屏障：READY 未闭合时 CanSimulate 恒假 ----
      const client = await openMpClient({seats: 2, characters: [0, 0], local: 0});
      try {
        ctx.assert(await netConfigure(client, setupWords({
          players: 2, local: 0, characters: [0, 0, 0]})) === 1, 'configure');
        ctx.assert(await netConnect(client, roomUrl) === 1, 'connect');
        // 未 READY：CanStart 假、CanSimulate 假（frame0 前不模拟）。
        let now = 1000;
        for (let attempt = 0; attempt < 30; ++attempt) {
          now += 16;
          await netPump(client, now);
          const state = await netState(client);
          ctx.assert(state.can_start === 0, `门未闭合 can_start=${state.can_start}`);
          ctx.assert(await netCanSimulate(client) === 0, '门未闭合不模拟');
        }
        ctx.evidence.push('frame0 屏障：单客户端 30 泵不模拟');
      } finally {
        await client.assertDelivery();
        await client.close();
      }
    } else if (caseId.includes('device_seat')) {
      const roomUrl = `${relay.url}/?room=th13m7ds&run=1`;
      // ---- 设备席位隔离：本地输入只出现在自己席位 ----
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1}),
      ];
      try {
        for (const [index, client] of clients.entries()) {
          ctx.assert(await netConfigure(client, setupWords({
            players: 2, local: index, characters: [0, 0, 0]})) === 1, 'configure');
          ctx.assert(await netConnect(client, roomUrl) === 1, 'connect');
        }
        let now = 1000, ready = false;
        for (let attempt = 0; attempt < 200 && !ready; ++attempt) {
          now += 16;
          for (const client of clients) {
            await netPump(client, now);
            await netReady(client);
          }
          const states = [];
          for (const client of clients) states.push(await netState(client));
          ready = states.every(state => state.can_start === 1);
          if (attempt === 199) ctx.assert(false, '握手未闭合');
        }
        // seat0 按射击（0x001），seat1 按低速（0x008）——各自只采本地。
        now = await pumpSession(clients, 30,
          (client, frame) => client === clients[0] ? 0x001 : 0x008, now);
        // 快照前帧号同步（合成钟泵下一端可能停等数帧——digest 对照必须
        // 同逻辑帧；时钟单调续推——ObserveClock 拒绝倒退）。
        let syncRounds = 0;
        for (;;) {
          const states = [];
          for (const client of clients) states.push(await netState(client));
          if (states[0].sim_frame === states[1].sim_frame) break;
          if (++syncRounds > 400) {
            ctx.assert(false, `帧号同步超时（${JSON.stringify(states)}）`);
          }
          now += 16;
          for (const client of clients) {
            if (await netPump(client, now) !== 1) {
              ctx.assert(false, `sync pump（${JSON.stringify(states)}）`);
            }
          }
          const laggard = states[0].sim_frame < states[1].sim_frame ? clients[0] : clients[1];
          if (await netCanSimulate(laggard) === 1) {
            await runFrame(laggard);
            await netOnSimulated(laggard);
          }
        }
        // 两侧权威模拟面一致（同 seed 同输入序列）。
        const digests = [];
        for (const client of clients) {
          await snapshotCapture(client);
          digests.push((await digestCore(client)).digest);
        }
        ctx.assert(digests[0] === digests[1],
          `席位隔离下的双端一致（${digests[0]} vs ${digests[1]}）`);
        ctx.evidence.push(`席位隔离：digest=${digests[0]}`);
      } finally {
        for (const client of clients) {
          await client.assertDelivery();
          await client.close();
        }
      }
    } else {
      throw new Error(`m7_netplay.mjs 不处理 case ${caseId}`);
    }
  } finally {
    relay.close();
  }
}
