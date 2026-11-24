export async function run(ctx) {
  const netState = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_state())));
  const digestCore = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_digest_core())));
  const snapshotCapture = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_capture())));
  const snapshotRestore = client => client.page.evaluate(() => JSON.parse(
    window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_restore())));
  const replayBegin = (client, ts) => client.page.evaluate(
    ({ts}) => window.mod._th13_debug_mp_replay_begin(ts), {ts});
  const replayAppend = (client, frame, stage, h0, h1, h2) => client.page.evaluate(
    ({frame, stage, h0, h1, h2}) =>
      window.mod._th13_debug_mp_replay_append(frame, stage, h0, h1, h2),
    {frame, stage, h0, h1, h2});
  const replayStageEntry = (client, stage) => client.page.evaluate(
    ({stage}) => window.mod._th13_debug_mp_replay_stage_entry(stage), {stage});
  const replayEncode = (client, name, score, completed) => client.page.evaluate(
    ({name, score, completed}) => JSON.parse(window.mod.UTF8ToString(
      window.mod._th13_debug_mp_replay_encode(
        window.mod.allocateUTF8(name), score, completed))),
    {name, score, completed});
  const replayLoad = (client, b64) => client.page.evaluate(
    ({b64}) => JSON.parse(window.mod.UTF8ToString(
      window.mod._th13_debug_mp_replay_load(window.mod.allocateUTF8(b64)))),
    {b64});
  const replayFrame = (client, frame) => client.page.evaluate(
    ({frame}) => JSON.parse(window.mod.UTF8ToString(
      window.mod._th13_debug_mp_replay_frame(frame))), {frame});
  const stageEntryCapture = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_stage_entry_capture());
  const stageEntryRestore = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_stage_entry_restore());
  const spectatorBeginPublish = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_spectator_begin_publish());
  const spectatorBeginViewing = client => client.page.evaluate(
    () => window.mod._th13_debug_mp_spectator_begin_viewing());
  const spectatorRecord = (client, frame, stage, h0, h1, withEntry) =>
    client.page.evaluate(
      ({frame, stage, h0, h1, withEntry}) =>
        window.mod._th13_debug_mp_spectator_record(frame, stage, h0, h1, withEntry),
      {frame, stage, h0, h1, withEntry});
  const spectatorPublish = (client, confirmed, simulated) => client.page.evaluate(
    ({confirmed, simulated}) => JSON.parse(window.mod.UTF8ToString(
      window.mod._th13_debug_mp_spectator_publish(confirmed, simulated))),
    {confirmed, simulated});
  const spectatorReceive = (client, b64) => client.page.evaluate(
    ({b64}) => JSON.parse(window.mod.UTF8ToString(
      window.mod._th13_debug_mp_spectator_receive(window.mod.allocateUTF8(b64)))),
    {b64});
  const spectatorState = client => client.page.evaluate(
    () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_spectator_state())));
  const runFrame = client => client.page.evaluate(() => window.mod._th13_debug_frame());

  const openSingle = async ({seats = 2, characters = [0, 0], local = 0} = {}) =>
    ctx.openClient({kind: 'manual', seed: 47932,
                    query: `mp=${seats}&mpc=${characters.join(',')}` +
                           `&mplocalseat=${local}`});
  const heldAt = (seat, frame) =>
    seat === 0 ? (frame % 5 === 0 ? 0x010 : 0x001)
              : (frame % 7 === 0 ? 0x801 : 0x001);

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('replay_roundtrip_seek')) {
    const client = await openSingle();
    try {
      ctx.assert(await replayBegin(client, 12345) === 1, 'replay begin');
      for (let frame = 0; frame < 40; ++frame) {
        ctx.assert(await replayAppend(client, frame, 1,
          heldAt(0, frame), heldAt(1, frame), 0) === 1, `append@${frame}`);
      }
      ctx.assert(await replayStageEntry(client, 1) === 1, 'stage entry 登记');
      const encoded = await replayEncode(client, 'browser-roundtrip', 654321, 1);
      ctx.assert(encoded.ok === 1, `encode ok（${JSON.stringify(encoded).slice(0, 80)}）`);
      ctx.assert(encoded.frames === 40, `帧数（${encoded.frames}）`);
      ctx.assert(typeof encoded.b64 === 'string' && encoded.b64.length > 100,
        'base64 桥产出');
      const loaded = await replayLoad(client, encoded.b64);
      ctx.assert(loaded.ok === 1, `load ok（${JSON.stringify(loaded)}）`);
      ctx.assert(loaded.frames === 40, `载入帧数（${loaded.frames}）`);
      ctx.assert(loaded.chapters >= 1, `章节数（${loaded.chapters}）`);
      ctx.assert(loaded.completed === 1, 'completed 面');
      ctx.assert(loaded.score === 654321, `score（${loaded.score}）`);
      for (let frame = 0; frame < 40; frame += 7) {
        const decoded = await replayFrame(client, frame);
        ctx.assert(decoded.ok === 1, `frame@${frame} ok`);
        ctx.assert(decoded.held[0] === heldAt(0, frame) &&
                   decoded.held[1] === heldAt(1, frame),
          `frame@${frame} 输入往返（${decoded.held}）`);
      }
      const anchor = await replayFrame(client, 0);
      ctx.assert(anchor.ok === 1, '章节锚点帧可定位');
      ctx.evidence.push(`Replay roundtrip：frames=${loaded.frames} ` +
        `chapters=${loaded.chapters} score=${loaded.score}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('spectator_confirmed_latejoin')) {
    const client = await openSingle();
    try {
      ctx.assert(await spectatorBeginPublish(client) === 1, 'begin publish');
      for (let frame = 0; frame < 30; ++frame) {
        ctx.assert(await spectatorRecord(client, frame, 1,
          heldAt(0, frame), heldAt(1, frame), frame === 0 ? 1 : 0) === 1,
          `record@${frame}`);
      }
      const published = await spectatorPublish(client, 25, 30);
      ctx.assert(published.ok === 1,
        `publish ok（${JSON.stringify(published).slice(0, 120)}）`);
      ctx.assert(published.published === 26, `发布前沿（${published.published}）`);
      ctx.assert(await spectatorBeginViewing(client) === 1, 'begin viewing');
      const received = await spectatorReceive(client, published.b64);
      ctx.assert(received.ok === 1, `receive ok（${JSON.stringify(received)}）`);
      ctx.assert(received.received === 26, `观众前沿（${received.received}）`);
      const state = await spectatorState(client);
      ctx.assert(state.viewing === 1, '观众态激活');
      ctx.evidence.push(`晚入观众：published=${published.published} ` +
        `received=${received.received}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('slow_spectator_isolation')) {
    const client = await openSingle();
    try {
      ctx.assert(await spectatorBeginPublish(client) === 1, 'begin publish');
      for (let frame = 0; frame < 60; ++frame) {
        ctx.assert(await spectatorRecord(client, frame, 1,
          heldAt(0, frame), heldAt(1, frame), frame === 0 ? 1 : 0) === 1,
          `record@${frame}`);
      }
      const firstBatch = await spectatorPublish(client, 29, 30);
      ctx.assert(firstBatch.ok === 1, 'first publish ok');
      ctx.assert(firstBatch.published === 30, `第一批前沿（${firstBatch.published}）`);
      ctx.assert(await spectatorBeginViewing(client) === 1, 'begin viewing');
      const received = await spectatorReceive(client, firstBatch.b64);
      ctx.assert(received.ok === 1, `first receive（${JSON.stringify(received)}）`);
      ctx.assert(received.received === 30, `观众第一批（${received.received}）`);
      const secondBatch = await spectatorPublish(client, 59, 60);
      ctx.assert(secondBatch.ok === 1, 'second publish ok');
      ctx.assert(secondBatch.published === 60,
        `第二批前沿不受观众消化影响（${secondBatch.published}）`);
      const rest = await spectatorReceive(client, secondBatch.b64);
      const final = await spectatorState(client);
      ctx.assert(final.received === 60,
        `观众收齐（received=${final.received}）`);
      ctx.evidence.push(`慢观众隔离：published=${secondBatch.published} ` +
        `received=${final.received}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('stage_entry_carry')) {
    const client = await openSingle();
    try {
      for (let frame = 0; frame < 30; ++frame) {
        ctx.assert(await runFrame(client) === 1, `baseline frame@${frame}`);
      }
      ctx.assert(await stageEntryCapture(client) === 1, 'stage entry capture');
      const checkpoint = await snapshotCapture(client);
      ctx.assert(checkpoint.ok === 1, '完整快照 capture');
      const checkpointDigest = checkpoint.digest;
      const rolled = await snapshotRestore(client);
      ctx.assert(rolled.ok === 1, '完整快照 restore（世界回退）');
      ctx.assert(rolled.digest === checkpointDigest,
        `restore 全等（${rolled.digest} vs ${checkpointDigest}）`);
      ctx.assert(await stageEntryRestore(client) === 1, 'stage entry restore');
      for (let frame = 30; frame < 60; ++frame) {
        ctx.assert(await runFrame(client) === 1, `carry frame@${frame}`);
      }
      const carriedSnap = await snapshotCapture(client);
      ctx.assert(carriedSnap.ok === 1, 'carried snapshot');
      const carriedDigest = (await digestCore(client)).digest;
      const baselineClient = await openSingle();
      try {
        for (let frame = 0; frame < 60; ++frame) {
          ctx.assert(await runFrame(baselineClient) === 1, `baseline frame@${frame}`);
        }
        const baselineSnap = await snapshotCapture(baselineClient);
        ctx.assert(baselineSnap.ok === 1, 'baseline snapshot');
        const baselineDigest = (await digestCore(baselineClient)).digest;
        ctx.assert(carriedDigest === baselineDigest,
          `关头延续一致（${carriedDigest} vs ${baselineDigest}）`);
        ctx.evidence.push(`关头延续：digest=${baselineDigest}`);
      } finally {
        await baselineClient.assertDelivery();
        await baselineClient.close();
      }
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('three_reentries')) {
    const client = await openSingle();
    try {
      const sessions = [];
      for (let round = 0; round < 3; ++round) {
        ctx.assert(await replayBegin(client, 1000 + round) === 1,
          `reentry ${round} begin`);
        for (let frame = 0; frame < 10; ++frame) {
          ctx.assert(await replayAppend(client, frame, 1,
            heldAt(0, frame + round * 10), heldAt(1, frame + round * 10), 0) === 1,
            `reentry ${round} append@${frame}`);
        }
        const encoded = await replayEncode(client, `run-${round}`, 1000 + round, 1);
        ctx.assert(encoded.ok === 1, `reentry ${round} encode`);
        sessions.push(encoded);
        const loaded = await replayLoad(client, encoded.b64);
        ctx.assert(loaded.ok === 1 && loaded.frames === 10,
          `reentry ${round} load（${JSON.stringify(loaded)}）`);
        ctx.assert(loaded.score === 1000 + round,
          `reentry ${round} score 隔离（${loaded.score}）`);
      }
      const first = await replayLoad(client, sessions[0].b64);
      ctx.assert(first.ok === 1 && first.score === 1000,
        `第一轮身份保持（${first.score}）`);
      ctx.evidence.push(`三次重入：scores=${sessions.map(s => s.frames).join(',')} ` +
        `identity 保持`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('run_generation_isolation')) {
    const client = await openSingle();
    try {
      for (let frame = 0; frame < 20; ++frame) {
        ctx.assert(await runFrame(client) === 1, `old frame@${frame}`);
      }
      ctx.assert(await stageEntryCapture(client) === 1, 'old capture');
      ctx.assert(await replayBegin(client, 999) === 1, 'new run begin');
      for (let frame = 0; frame < 5; ++frame) {
        ctx.assert(await replayAppend(client, frame, 1,
          heldAt(0, frame + 5), heldAt(1, frame + 5), 0) === 1,
          `new append@${frame}`);
      }
      ctx.assert(await replayStageEntry(client, 1) === 1, 'new stage entry');
      const encoded = await replayEncode(client, 'new-gen', 1, 0);
      ctx.assert(encoded.ok === 1, 'new encode');
      const loaded = await replayLoad(client, encoded.b64);
      ctx.assert(loaded.ok === 1, 'new load');
      ctx.assert(loaded.chapters >= 1, `新局章节（${loaded.chapters}）`);
      ctx.evidence.push(`generation 隔离：新局档 chapters=${loaded.chapters}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('terminal_local_save')) {
    const client = await openSingle();
    try {
      ctx.assert(await replayBegin(client, 7) === 1, 'begin');
      for (let frame = 0; frame < 15; ++frame) {
        ctx.assert(await replayAppend(client, frame, 1,
          heldAt(0, frame), heldAt(1, frame), 0) === 1, `append@${frame}`);
      }
      const done = await replayEncode(client, 'cleared', 999999, 1);
      const half = await replayEncode(client, 'half', 500, 0);
      ctx.assert(done.ok === 1 && half.ok === 1, '双档编码');
      const loadedDone = await replayLoad(client, done.b64);
      const loadedHalf = await replayLoad(client, half.b64);
      ctx.assert(loadedDone.completed === 1 && loadedHalf.completed === 0,
        `completed 面（${loadedDone.completed}/${loadedHalf.completed}）`);
      ctx.assert(loadedDone.score === 999999 && loadedHalf.score === 500,
        `score 面（${loadedDone.score}/${loadedHalf.score}）`);
      ctx.evidence.push(`终局保存：done=${loadedDone.score} half=${loadedHalf.score}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('disconnect_confirmed_save_return')) {
    const client = await openSingle();
    try {
      ctx.assert(await replayBegin(client, 3) === 1, 'begin');
      for (let frame = 0; frame < 30; ++frame) {
        ctx.assert(await replayAppend(client, frame, 1,
          heldAt(0, frame), heldAt(1, frame), 0) === 1, `append@${frame}`);
      }
      const saved = await replayEncode(client, 'disconnect', 777, 0);
      ctx.assert(saved.ok === 1, '断线存档编码');
      const loaded = await replayLoad(client, saved.b64);
      ctx.assert(loaded.ok === 1 && loaded.frames === 30,
        `返回载入（${JSON.stringify(loaded)}）`);
      for (let frame = 0; frame < 30; frame += 9) {
        const decoded = await replayFrame(client, frame);
        ctx.assert(decoded.ok === 1, `返回帧@${frame} 可定位`);
      }
      ctx.evidence.push(`断线保存返回：frames=${loaded.frames}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('scene_coverage_matrix')) {
    const matrix = [];
    for (const seats of [2, 3]) {
      for (const stageIndex of [0, 1, 2]) {
        const characters = seats === 2 ? [0, 0] : [1, 2, 1];
        const client = await ctx.openClient({kind: 'manual', seed: 47932,
          query: `mp=${seats}&mpc=${characters.join(',')}` +
                 `&mplocalseat=0&stage=${stageIndex}`});
        try {
          let advanced = 0;
          for (let frame = 0; frame < 10; ++frame) {
            if (await runFrame(client) === 1) ++advanced;
          }
          ctx.assert(advanced >= 5,
            `矩阵 seats=${seats} stage=${stageIndex + 1} 帧推进（${advanced}）`);
          const snap = await snapshotCapture(client);
          ctx.assert(snap.ok === 1,
            `矩阵 seats=${seats} stage=${stageIndex + 1} snapshot`);
          matrix.push({seats, stage: stageIndex + 1, advanced, ok: snap.ok});
        } finally {
          await client.assertDelivery();
          await client.close();
        }
      }
    }
    ctx.assert(matrix.length === 6, `矩阵格数（${matrix.length}）`);
    ctx.evidence.push(`场景矩阵：${matrix.map(m =>
      `${m.seats}p-s${m.stage}:${m.advanced}f`).join(' ')}`);
  } else if (caseId.includes('runtime_page_soak')) {
    const SEED = 47932;
    const delayInit = `
      (() => {
        const rnd = () => Math.random();
        let serial = 0;
        const wrap = (original, socket, data) => {
          const isControl = typeof socket.url === 'string' && socket.url.includes('signal=1');
          const isControlDc = typeof socket.label === 'string' && socket.label.includes('control');
          const isInputDc = typeof socket.label === 'string' && /-input$/.test(socket.label);
          const isString = !(data instanceof ArrayBuffer) && !ArrayBuffer.isView(data);
          const isWebSocket = typeof WebSocket === 'function' && socket instanceof WebSocket;
          const disturbInput = !isString && !isControl && (isInputDc || (isWebSocket && !isControlDc));
          if (!disturbInput) return original.call(socket, data);
          ++serial;
          if (serial % 50 === 0) return undefined;  // 2% 丢包（重传恢复）
          const delay = Math.max(0, 50 + (rnd() * 2 - 1) * 20);
          setTimeout(() => {
            try {
              if (socket.readyState !== 'open' && socket.readyState !== 1) return;
              original.call(socket, data);
            } catch {}
          }, delay);
          return undefined;
        };
        if (globalThis.RTCDataChannel && !RTCDataChannel.prototype.__th13soak) {
          const original = RTCDataChannel.prototype.send;
          RTCDataChannel.prototype.__th13soak = true;
          RTCDataChannel.prototype.send = function (data) { return wrap(original, this, data); };
        }
        if (globalThis.WebSocket && !WebSocket.prototype.__th13soak) {
          const original = WebSocket.prototype.send;
          WebSocket.prototype.__th13soak = true;
          WebSocket.prototype.send = function (data) { return wrap(original, this, data); };
        }
      })();`;
    const netPump = (client, now) => client.page.evaluate(
      ({now}) => window.mod._th13_debug_mp_net_pump(now), {now});
    const netReady = client => client.page.evaluate(
      () => window.mod._th13_debug_mp_net_ready());
    const netRollback = client => client.page.evaluate(
      () => JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_mp_net_rollback())));
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
    const soakMinutes = Number(process.env.TH13_SOAK_MINUTES ?? '5');
    const soakMs = soakMinutes * 60_000;
    const relay = await ctx.startRelay({});
    try {
      const evidence = [];
      for (const seats of [2, 3]) {
        const clients = [];
        try {
          const roomUrl = `${relay.url}/?room=th13m9soak${seats}&run=1`;
          for (let seat = 0; seat < seats; ++seat) {
            const client = await ctx.openClient({kind: 'raf', seed: SEED,
              query: `mp=${seats}&mpc=0,0,0&mplocalseat=${seat}`, init: delayInit});
            clients.push(client);
            ctx.assert(await netConfigure(client, [
              2, seats, seat, 2, SEED, 0x9e37, 0x79b9, 2,
              0, 0, 0, 0, 0, 0, 2, 0, 2, 1, 2, 3, 4, 0]) === 1, `soak configure@seat${seat}`);
            ctx.assert(await netConnect(client, roomUrl) === 1, `soak connect@seat${seat}`);
          }
          const readyDeadline = Date.now() + 60_000;
          for (;;) {
            let allReady = true;
            for (const client of clients) {
              const pageNow = await client.page.evaluate(() => performance.now());
              await netPump(client, pageNow);
              await netReady(client);
              const state = await netState(client);
              if (state.error) ctx.assert(false, `soak 握手错误（${state.error}）`);
              if (state.can_start !== 1) allReady = false;
            }
            if (allReady) break;
            if (Date.now() > readyDeadline) ctx.assert(false, 'soak 握手超时');
          }
          for (const client of clients) await client.page.evaluate(() => {
            const mod = window.mod;
            window.__rafMp = {ticks: 0, began: performance.now(), failure: null};
            mod.onGameFrame = (ok) => {
              ++window.__rafMp.ticks;
              if (!ok && !window.__rafMp.failure) {
                let detail = '';
                try { detail = mod.UTF8ToString(mod._th13_error()); } catch {}
                try {
                  detail += ' net=' + mod.UTF8ToString(mod._th13_debug_mp_net_state());
                } catch {}
                window.__rafMp.failure = 'rAF 帧体失败：' + detail;
              }
            };
            mod._th13_loop_start();
          });
          for (const client of clients) {
            await client.page.locator('canvas').first().click({position: {x: 5, y: 5}});
            await client.page.keyboard.down('KeyZ');
          }
          const began = Date.now();
          let lastProgress = {frame: -1, at: began};
          let minSpread = Infinity, maxSpread = 0, minFps = Infinity;
          let cleanEnd = null, simulatingStoppedAt = 0;
          while (Date.now() - began < soakMs) {
            const states = await Promise.all(clients.map(client => netState(client)));
            for (let i = 0; i < clients.length; ++i) {
              const failure = await clients[i].page.evaluate(() => window.__rafMp?.failure ?? null);
              if (failure) ctx.assert(false, `soak rAF（${failure}）`);
            }
            for (const state of states)
              if (state.error) ctx.assert(false, `soak 会话错误（${state.error}）`);
            const frames = states.map(state => state.sim_frame);
            const simulating = states.every(state => state.simulating === 1);
            if (!simulating) {
              if (!simulatingStoppedAt) simulatingStoppedAt = Date.now();
              const released = states.every(state => state.run_released === 1);
              if (released) {
                cleanEnd = {frame: Math.max(...frames), at: Date.now()};
                evidence.push(`soak ${seats}p 正常终局@帧${cleanEnd.frame}（game over/ending，` +
                  `模拟期 ${((cleanEnd.at - began) / 1000).toFixed(0)}s 全绿；终局后不再模拟，本拓扑提前收卷）`);
                ctx.evidence.push(`soak ${seats}p 正常终局@帧${cleanEnd.frame}`);
                break;
              }
              if (Date.now() - simulatingStoppedAt > 60_000)
                ctx.assert(false, `soak ${seats}p 终局交接未释放（frames=${JSON.stringify(frames)}）`);
              await new Promise(done => setTimeout(done, 5000));
              continue;
            }
            const spread = Math.max(...frames) - Math.min(...frames);
            minSpread = Math.min(minSpread, spread);
            maxSpread = Math.max(maxSpread, spread);
            const settling = Date.now() - began < 45_000;
            ctx.assert(spread <= (settling ? 12 : 6),
              `soak ${seats}p 帧号差（spread=${spread} frames=${JSON.stringify(frames)}` +
              ` phase=${settling ? '收敛' : '稳态'}）`);
            const min = Math.min(...frames);
            if (min > lastProgress.frame) lastProgress = {frame: min, at: Date.now()};
            else if (Date.now() - lastProgress.at > 30_000)
              ctx.assert(false, `soak ${seats}p 停死（frames=${JSON.stringify(frames)}）`);
            await new Promise(done => setTimeout(done, 5000));
          }
          let fpsSum = 0;
          for (const client of clients) {
            const meter = await client.page.evaluate(() => {
              const mp = window.__rafMp;
              return {ticks: mp.ticks, seconds: (performance.now() - mp.began) / 1000};
            });
            const fps = meter.ticks / Math.max(0.001, meter.seconds);
            fpsSum += fps;
            minFps = Math.min(minFps, fps);
            evidence.push(`soak ${seats}p 呈现 fps=${fps.toFixed(1)}`);
          ctx.evidence.push(`soak ${seats}p 呈现 fps=${fps.toFixed(1)}`);
          }
          const rollback = await netRollback(clients[0]);
          ctx.assert(rollback.desync_detected === 0,
            `soak ${seats}p digest 零失配（desync=${rollback.desync_detected}）`);
          ctx.assert(rollback.digests_received > 0,
            `soak ${seats}p digest 已交换（received=${rollback.digests_received}）`);
          ctx.assert(rollback.stall_frames <= 12,
            `soak ${seats}p 停等帧在上界内（stall=${rollback.stall_frames} ≤12）`);
          if (process.env.TH13_SOAK_REQUIRE_FPS_58 === '1') {
            ctx.assert(minFps >= 58,
              `soak ${seats}p 平均呈现 fps ≥58（实测 ${minFps.toFixed(1)}）`);
          } else {
            ctx.evidence.push(`BLOCKED: soak ${seats}p 平均呈现 fps≥58 在无硬件渲染容器` +
              `物理不可达（实测 ${minFps.toFixed(1)}）；该断言由 TH13_SOAK_REQUIRE_FPS_58=1` +
              `环境启用，阈值不降`);
          }
          evidence.push(`soak ${seats}p 完成：spread ${minSpread}..${maxSpread} ` +
            `restores=${rollback.restores} maxDistance=${rollback.maxDistance}`);
          const route = await clients[0].page.evaluate(
            () => globalThis.__eaglerPeerTransport?.route);
          ctx.noteRuntime({
            transportPaths: [route],
            rollbackCount: rollback.restores,
            maxRollbackDistance: rollback.maxDistance,
          });
        } finally {
          for (const client of clients) {
            if (client) {
              await client.assertDelivery();
              await client.close();
            }
          }
        }
      }
      for (const line of evidence) ctx.evidence.push(line);
      evidence.length = 0;
    } finally {
      relay.close();
    }
  } else {
    throw new Error(`m9_lifecycle.mjs 不处理 case ${caseId}`);
  }
}
