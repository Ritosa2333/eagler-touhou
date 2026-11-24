export async function run(ctx) {
  const SEED = 47932;
  const setupWords = ({players, local, characters, delay = 2}) => [
    2, players, local, 2, SEED, 0x9e37, 0x79b9, delay,
    ...characters.slice(0, 3), 0, 0, 0,
    2, 0, 2, 1, 2, 3, 4, 0,
  ];
  const wireDelayInit = (delayMs, jitterMs, dropEvery) => {
    const rnd = () => Math.random();
    let serial = 0;
    const wrap = (original, socket, data) => {
      const isControl = typeof socket.url === 'string' && socket.url.includes('signal=1');
      const isControlDc = typeof socket.label === 'string' && socket.label.includes('control');
      const isInputDc = typeof socket.label === 'string' &&
        /-input$/.test(socket.label);
      const isString = !(data instanceof ArrayBuffer) && !ArrayBuffer.isView(data);
      const isWebSocket = typeof WebSocket === 'function' && socket instanceof WebSocket;
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
      if (drop && serial % dropEvery === 0) return undefined;
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
  const frameDelayInit = initialDelay => {
    let delayFrames = initialDelay;
    let count = 0;
    let armed = false;
    const queue = [];
    const wrap = (original, socket, data) => {
      const isControl = typeof socket.url === 'string' && socket.url.includes('signal=1');
      const isControlDc = typeof socket.label === 'string' && socket.label.includes('control');
      const isInputDc = typeof socket.label === 'string' &&
        /-input$/.test(socket.label);
      const isString = !(data instanceof ArrayBuffer) && !ArrayBuffer.isView(data);
      const isWebSocket = typeof WebSocket === 'function' && socket instanceof WebSocket;
      const disturbInput = !isString && !isControl &&
        (isInputDc || (isWebSocket && !isControlDc));
      if (!armed || !disturbInput) return original.call(socket, data);
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
    globalThis.__th13m8Arm = () => { armed = true; };
    globalThis.__th13m8Stats = () => ({count, queued: queue.length, delivered, armed,
                                       wrapped: typeof RTCDataChannel.prototype.send === 'function'});
  };
  const netAdvance = client => client.page.evaluate(
    () => { if (globalThis.__th13m8Advance) globalThis.__th13m8Advance(); });
  const netArmDelay = client => client.page.evaluate(
    () => { if (globalThis.__th13m8Arm) globalThis.__th13m8Arm(); });
  const netState = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_state())));
  const netRollback = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_rollback())));
  const netAudio = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_audio())));
  const netAudioClear = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_net_audio_clear());
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
                    init: (frameDelay > 0
                      ? `(${frameDelayInit.toString()})(${frameDelay})`
                      : (wireDelay > 0 || jitter > 0 || dropEvery > 0)
                        ? `(${wireDelayInit.toString()})(${wireDelay}, ${jitter}, ${dropEvery})`
                        : '') + (extraInit || '')});

  const handshake = async (clients, room, nowStart = 1000, delay = 3, clock = null) => {
    const nowOf = clock ?? (client => Date.now());
    const roomUrl = `${relay.url}/?room=${room}&run=1`;
    for (const [index, client] of clients.entries()) {
      ctx.assert(await netConfigure(client, setupWords({
        players: clients.length, local: index, delay,
        characters: [0, 0, 0]})) === 1, `configure@seat${index}`);
      ctx.assert(await netConnect(client, roomUrl) === 1, `connect@seat${index}`);
    }
    let now = nowStart, ready = false;
    const deadline = Date.now() + 60_000;
    for (let attempt = 0; !ready; ++attempt) {
      now = await nowOf(clients[0]);
      if (now > deadline) {
        const states = [];
        for (const client of clients) states.push(await netState(client));
        ctx.assert(false, `握手未闭合（${JSON.stringify(states)} ` +
          `relay=${relay.log().slice(-400)}）`);
      }
      for (const [index, client] of clients.entries()) {
        const clientNow = clock ? await nowOf(client) : now;
        if (await netPump(client, clientNow) !== 1) {
          const states = [];
          for (const other of clients) states.push(await netState(other));
          ctx.assert(false, `握手泵@seat${index} attempt=${attempt} ` +
            `（states=${JSON.stringify(states)} relay=${relay.log().slice(-400)}）`);
        }
        await netReady(client);
      }
      const states = [];
      for (const client of clients) states.push(await netState(client));
      ready = states.every(state => state.can_start === 1);
    }
    for (const client of clients) ctx.assert(await netReady(client) === 1, 'Start');
    return now;
  };

  const pumpSession = async (clients, simFrames, inputs, startNow, simulate = true) => {
    let simulated = 0;
    let stallGuard = 0;
    while (simulated < simFrames) {
      const now = Date.now();
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
      for (const client of clients) await netAdvance(client);
      const decisions = [];
      for (const client of clients) {
        decisions.push(simulate ? (await netCanSimulate(client)) === 1 : false);
      }
      const canSimulate = decisions.every(Boolean);
      if (canSimulate) {
        for (const client of clients) {
          const held = inputs(client, simulated);
          ctx.assert(await netCapture(client, held, now) === 1, 'capture');
        }
      }
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

  const netSetFrameDelay = (client, value) => client.page.evaluate(
    value => { if (globalThis.__th13m8SetDelay) globalThis.__th13m8SetDelay(value); },
    value);
  const forceRelayRoute = client => client.page.evaluate(() => {
    globalThis.RTCPeerConnection = undefined;
    window.mod.eaglerOptions = {
      ...(window.mod.eaglerOptions ?? {}),
      netplayAllowWebsocketFallback: true,
    };
  });
  const transportRoute = client => client.page.evaluate(
    () => globalThis.__eaglerPeerTransport?.route ?? null);
  const sessionClock = () => {
    let now = Date.now();
    return () => (now = Math.max(now + 1, Date.now()));
  };
  const settleInputs = async (client, now, frontier, label) => {
    for (let attempt = 0; ; ++attempt) {
      const pumped = await netPump(client, now);
      const state = await netState(client);
      if (pumped === 1 && state.confirmed !== 0xFFFFFFFF && state.confirmed >= frontier) return;
      if (pumped !== 1 || attempt >= 4000) {
        const shim = await client.page.evaluate(
          () => globalThis.__th13m8Stats ? globalThis.__th13m8Stats() : null);
        ctx.assert(false, `${label} 送达屏障（pumped=${pumped} frontier=${frontier} ` +
          `state=${JSON.stringify(state)} rollback=${JSON.stringify(await netRollback(client))} ` +
          `shim=${JSON.stringify(shim)}）`);
      }
    }
  };

  const syncFrames = async (clients, maxRounds = 400) => {
    let rounds = 0, stalls = 0;
    for (;;) {
      const now = Date.now();
      for (const client of clients) {
        ctx.assert(await netPump(client, now) === 1, 'sync pump');
      }
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
  const relay = await ctx.startRelay({});
  try {
    if (caseId.includes('rollback_one_three_twelve')) {
      const frames = 60, inputDelay = 2;
      for (const [label, frameDelay, distance] of
             [['1帧', 3, 1], ['3帧', 5, 3], ['12帧', 14, 12]]) {
        const clients = [
          await openMpClient({seats: 2, characters: [0, 0], local: 0, frameDelay}),
          await openMpClient({seats: 2, characters: [0, 0], local: 1, frameDelay}),
        ];
        try {
          for (const client of clients) await forceRelayRoute(client);
          const clock = sessionClock();
          await handshake(clients, `th13m8rb-${distance}`, 0, inputDelay, clock);
          for (const client of clients) await netArmDelay(client);
          const inputs = (client, frame) =>
            (frame % 2 === 0) === (client === clients[0]) ? 0x001 : 0x000;
          let stalls = 0;
          const captures = clients.map(() => []);
          for (let round = 1; round <= frames; ++round) {
            for (const client of clients) await netAdvance(client);
            const now = clock();
            for (const [seat, client] of clients.entries()) {
              const released = captures[1 - seat][round - frameDelay - 1];
              if (released >= 1) {
                await settleInputs(client, now, released - 1 + inputDelay, `${label} 第${round}轮`);
              } else {
                ctx.assert(await netPump(client, now) === 1, `${label} 第${round}轮泵`);
              }
            }
            for (const client of clients) {
              for (let step = 0; ; ++step) {
                if (step) ctx.assert(await netPump(client, now) === 1, `${label} 第${round}轮补帧泵`);
                const state = await netState(client);
                if (state.sim_frame >= round) break;
                if (state.capture_frame <= Math.max(state.sim_frame, Math.min(round, frameDelay) - 1)) {
                  ctx.assert(await netCapture(client, inputs(client, state.capture_frame + inputDelay),
                                              now) === 1, `${label} 第${round}轮捕获`);
                }
                if ((await netCanSimulate(client)) !== 1) {
                  ++stalls;
                  break;
                }
                await runFrame(client);
                await netOnSimulated(client);
              }
            }
            for (const [seat, client] of clients.entries())
              captures[seat].push((await netState(client)).capture_frame);
          }
          const route = await transportRoute(clients[0]);
          ctx.assert(route === 'relay', `${label} 档延迟注入覆盖全部输入通道（route=${route}）`);
          let maxRestores = 0, maxDistance = 0;
          for (const [seat, client] of clients.entries()) {
            const state = await netState(client);
            const rollback = await netRollback(client);
            const stats = await client.page.evaluate(
              () => globalThis.__th13m8Stats && globalThis.__th13m8Stats());
            ctx.evidence.push(`${label} 档席位${seat} 注入自检：${JSON.stringify(stats)}`);
            ctx.assert(state.sim_frame === frames && state.error === '',
              `${label} 档席位${seat} 追平（${JSON.stringify(state)}）`);
            ctx.assert(rollback.restores >= frames - frameDelay - 1,
              `${label} 档席位${seat} 持续回滚（restores=${rollback.restores} ` +
              `>= ${frames - frameDelay - 1}）`);
            ctx.assert(rollback.maxDistance === distance,
              `${label} 档席位${seat} 纠正距离（maxDistance=${rollback.maxDistance} === ${distance}）`);
            ctx.assert(rollback.last_error === 0 && rollback.rollback_pending === 0 &&
                       rollback.desync_detected === 0,
              `${label} 档席位${seat} 纠正收敛（${JSON.stringify(rollback)}）`);
            ctx.evidence.push(`${label} 档（frameDelay=${frameDelay}）席位${seat}：` +
              `restores=${rollback.restores} resimulated=${rollback.resimulated} ` +
              `checkpoints=${rollback.checkpoints} maxDistance=${rollback.maxDistance} ` +
              `distanceSum=${rollback.rollback_distance_sum}`);
            maxRestores = Math.max(maxRestores, rollback.restores);
            maxDistance = Math.max(maxDistance, rollback.maxDistance);
          }
          ctx.evidence.push(`${label} 档停等轮次：${stalls}`);
          ctx.noteRuntime({
            transportPaths: [route],
            rollbackCount: maxRestores,
            maxRollbackDistance: maxDistance,
          });
        } finally {
          for (const client of clients) {
            await client.assertDelivery();
            await client.close();
          }
        }
      }
    } else if (caseId.includes('audio_bgm_once')) {
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
        client === clientsRef[0] ? 0x001 : (frame % 2 === 0 ? 0x008 : 0x000);
      const runArm = async (label, room, withDelay) => {
        const clients = [
          await openMpClient({seats: 2, characters: [0, 0], local: 0,
                              frameDelay: withDelay ? 17 : 0}),
          await openMpClient({seats: 2, characters: [0, 0], local: 1,
                              frameDelay: withDelay ? 17 : 0}),
        ];
        clientsRef = clients;
        try {
          for (const client of clients) await forceRelayRoute(client);
          if (withDelay) {
            for (const client of clients) await netSetFrameDelay(client, 0);
          }
          const clock = sessionClock();
          await handshake(clients, room, 0, 3, clock);
          for (const client of clients) await netArmDelay(client);
          for (const client of clients) await netAudioAppliedClear(client);
          const inputDelay = 3;
          const captures = clients.map(() => []);
          let round = 0, delayFrames = 0;
          const deliver = async () => {
            for (const client of clients) await netAdvance(client);
            const now = clock();
            for (const [seat, client] of clients.entries()) {
              const released = captures[1 - seat][round - delayFrames - 1];
              if (released >= 1) {
                await settleInputs(client, now, released - 1 + inputDelay, `${label} 第${round}轮`);
              } else {
                ctx.assert(await netPump(client, now) === 1, `${label} 第${round}轮泵`);
              }
            }
            return now;
          };
          const record = async () => {
            for (const [seat, client] of clients.entries())
              captures[seat].push((await netState(client)).capture_frame);
          };
          const pumpTo = async target => {
            for (;;) {
              const states = [];
              for (const client of clients) states.push(await netState(client));
              if (states.every(state => state.sim_frame >= target)) return;
              ctx.assert(round < target + delayFrames + 400,
                `${label} 泵超时（states=${JSON.stringify(states)}）`);
              ++round;
              const now = await deliver();
              const decisions = [];
              for (const client of clients) decisions.push((await netCanSimulate(client)) === 1);
              for (const client of clients) {
                const capture = await netState(client);
                if (capture.capture_frame > capture.sim_frame) continue;
                ctx.assert(await netCapture(client, inputs(client, capture.capture_frame + inputDelay),
                                            now) === 1, `${label} 捕获`);
              }
              if (decisions.every(Boolean)) {
                for (const client of clients) {
                  await runFrame(client);
                  await netOnSimulated(client);
                }
              }
              await record();
            }
          };
          await pumpTo(20);
          delayFrames = withDelay ? 17 : 0;
          if (withDelay) {
            for (const client of clients) await netSetFrameDelay(client, delayFrames);
          }
          await pumpTo(40);
          for (;;) {
            const states = [];
            for (const client of clients) states.push(await netState(client));
            const rollbacks = [];
            for (const client of clients) rollbacks.push(await netRollback(client));
            const lagging = Math.min(...states.map(state => state.sim_frame));
            if (states.every((state, seat) => state.sim_frame === lagging &&
                             rollbacks[seat].rollback_pending === 0 &&
                             rollbacks[seat].last_error === 0 && rollbacks[seat].records === 0 &&
                             rollbacks[seat].committed === state.sim_frame &&
                             state.confirmed >= state.sim_frame - 1)) break;
            ctx.assert(round < 900, `${label} 追平排空超时（states=${JSON.stringify(states)} ` +
              `rollbacks=${JSON.stringify(rollbacks)}）`);
            ++round;
            const now = await deliver();
            for (const [seat, client] of clients.entries()) {
              const capture = await netState(client);
              if (capture.capture_frame <= capture.sim_frame) {
                ctx.assert(await netCapture(client, inputs(client, capture.capture_frame + inputDelay),
                                            now) === 1, `${label} 追平捕获`);
              }
              if (states[seat].sim_frame !== lagging || (await netCanSimulate(client)) !== 1) continue;
              await runFrame(client);
              await netOnSimulated(client);
            }
            await record();
          }
          ctx.assert(await transportRoute(clients[0]) === 'relay', `${label} 臂走 relay 路由`);
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
      const target = 40;
      const applied = rollback.perClient.flat().filter(event => event.frame < target);
      const baselineApplied = baseline.perClient.flat()
        .filter(event => event.frame < target);
      ctx.evidence.push(`audio_bgm_once baseline=${JSON.stringify(baseline.stats)}`);
      ctx.evidence.push(`audio_bgm_once rollback=${JSON.stringify(rollback.stats)}`);
      ctx.assert(baseline.stats.restores === 0,
                 `baseline 臂无回滚（${baseline.stats.restores}）`);
      ctx.assert(rollback.stats.restores > 0,
                 `rollback 臂真实回滚（${rollback.stats.restores}）`);
      ctx.assert(rollback.stats.maxDistance >= 10,
                 `rollback 臂纠正距离 >=10（${rollback.stats.maxDistance}）`);
      ctx.assert(rollback.stats.sfx_suppressed > 0,
                 `重放 Voice 被吞（${rollback.stats.sfx_suppressed}）`);
      for (const [label, stats] of [['baseline', baseline.stats],
                                    ['rollback', rollback.stats]]) {
        ctx.assert(stats.voice_applied + stats.sfx_suppressed === stats.voice_captured,
                   `${label} Voice 账本守恒（${stats.voice_applied}+` +
                   `${stats.sfx_suppressed}==${stats.voice_captured}）`);
        ctx.assert(stats.music_applied + stats.music_dropped === stats.music_captured,
                   `${label} Music 账本守恒（${stats.music_applied}+` +
                   `${stats.music_dropped}==${stats.music_captured}）`);
      }
      const musicCounts = new Map();
      let voiceTiming = 0, voiceBeforeConfirm = 0, deferredTiming = 0;
      for (const event of applied) {
        if (event.kind === 'voice') {
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
      ctx.assert(Array.isArray(rollback.audioOut), '音频出队面为数组');
    } else if (caseId.includes('trance_death')) {
      const EXPERIMENT_ZERO_DELAY = false;
      const wireDelay = EXPERIMENT_ZERO_DELAY ? 0 : 100;
      const clients = [
        await openMpClient({seats: 2, characters: [0, 0], local: 0, wireDelay}),
        await openMpClient({seats: 2, characters: [0, 0], local: 1, wireDelay}),
      ];
      try {
        await handshake(clients, 'th13m8td');
        await pumpSession(clients, 50, (client, frame) =>
          client === clients[1] ? 0x801 : 0x001);
        const rollback = await netRollback(clients[0]);
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
              const want = frame % 5 === 0 ? 203 : 44;
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
        const fullWindow = Math.max(rollback.maxDistance, rollbackPeer.maxDistance);
        ctx.assert(fullWindow >= 12,
          `rAF 自然泵满窗纠正（maxDistance=${fullWindow} >= 12；` +
          `seat0=${rollback.maxDistance}/${rollback.restores} ` +
          `seat1=${rollbackPeer.maxDistance}/${rollbackPeer.restores}）`);
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
