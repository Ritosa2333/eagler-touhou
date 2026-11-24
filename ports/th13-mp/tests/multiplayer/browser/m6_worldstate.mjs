export async function run(ctx) {
  const SEED = 47932;

  const openMpClient = ({seats, characters}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}`});
  const snapshotCapture = client =>
    client.page.evaluate(() => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_capture())));
  const snapshotRestore = client =>
    client.page.evaluate(() => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_restore())));
  const snapshotDigest = client =>
    client.page.evaluate(() => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_digest())));
  const seatState = (client, seat) =>
    client.page.evaluate(s => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_seat_state(s))), seat);
  const stageReport = client =>
    client.page.evaluate(
      () => window.mod.UTF8ToString(window.mod._th13_debug_stage_report()));
  const runFrame = async client => {
    const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
    ctx.assert(ok === 1, '帧泵');
    return ok;
  };

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('full_state_restore')) {
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      for (let frame = 0; frame < 1180; ++frame) await runFrame(client);
      const before = (await stageReport(client)).split('\n')[0];
      const captured = await snapshotCapture(client);
      ctx.assert(captured.ok === 1, `capture ok（${JSON.stringify(captured)}）`);
      ctx.assert(captured.digest.length === 16, 'digest 64bit hex');
      ctx.assert(captured.retained > 0, '快照非空');
      const captureDigest = captured.digest;
      for (let frame = 0; frame < 60; ++frame) await runFrame(client);
      const diverged = (await stageReport(client)).split('\n')[0];
      const restored = await snapshotRestore(client);
      ctx.assert(restored.ok === 1, `restore ok（${JSON.stringify(restored)}）`);
      ctx.assert(restored.digest === captureDigest,
        `restore digest == capture digest（${restored.digest} vs ${captureDigest}）`);
      const verify = await snapshotCapture(client);
      ctx.assert(verify.digest === captureDigest, '重 capture 同 digest');
      await runFrame(client);
      const afterRestore = (await stageReport(client)).split('\n')[0];
      const parse = line => {
        const enemies = /enemies=(\d+)/.exec(line);
        const bullets = /bullets=(\d+)/.exec(line);
        return `${enemies?.[1] ?? '?'}:${bullets?.[1] ?? '?'}`;
      };
      ctx.assert(parse(afterRestore) === parse(before),
        `restore 一帧后敌机/弹幕权威计数与 capture 时同态` +
        `（"${parse(afterRestore)}" vs "${parse(before)}"；全行 restore="${afterRestore}" capture="${before}"）`);
      const snapshotDigestCore = client2 =>
        client2.page.evaluate(() => JSON.parse(
          window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_digest_core())));
      const baseline = await openMpClient({seats: 2, characters: [0, 0]});
      try {
        for (let frame = 0; frame < 1180; ++frame) await runFrame(baseline);
        await runFrame(baseline);
        for (let frame = 0; frame < 12; ++frame) {
          await runFrame(client);
          await runFrame(baseline);
          const ca = await snapshotCapture(client);
          const cb = await snapshotCapture(baseline);
          ctx.assert(ca.ok === 1 && cb.ok === 1, `续推 capture ok@${frame}`);
          const da = await snapshotDigestCore(client);
          const db = await snapshotDigestCore(baseline);
          ctx.assert(da.ok === 1 && db.ok === 1, `续推 digest_core ok@${frame}`);
          if (da.digest !== db.digest) {
            ctx.assert(false, `续推第 ${frame} 帧权威模拟面 digest 分歧` +
              `（${da.digest} vs ${db.digest}）`);
            return;
          }
        }
        const s0a = await seatState(client, 0), s0b = await seatState(baseline, 0);
        ctx.assert(s0a.x === s0b.x && s0a.y === s0b.y && s0a.shots === s0b.shots,
          `seat 0 状态一致（${JSON.stringify(s0a)} vs ${JSON.stringify(s0b)}）`);
      } finally {
        await baseline.assertDelivery();
        await baseline.close();
      }
      ctx.evidence.push(`快照往返：digest=${captureDigest}（60 帧发散后 restore 全等）` +
        `；续推 12 帧权威模拟面 digest 逐帧一致`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('anm_borrowed')) {
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      for (let frame = 0; frame < 1180; ++frame) await runFrame(client);
      const captured = await snapshotCapture(client);
      ctx.assert(captured.ok === 1, 'capture ok');
      const captureDigest = captured.digest;
      for (let frame = 0; frame < 240; ++frame) await runFrame(client);
      const restored = await snapshotRestore(client);
      ctx.assert(restored.ok === 1, 'restore ok');
      ctx.assert(restored.digest === captureDigest, 'restore digest 全等');
      for (let frame = 0; frame < 120; ++frame) {
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `续推@${frame}`);
      }
      const errorText = await client.page.evaluate(() => window.mod.UTF8ToString(
        window.mod._th13_error()));
      ctx.assert(errorText === '', `restore 后续推无错误（"${errorText}"）`);
      const head = (await stageReport(client)).split('\n')[0];
      ctx.assert(head.includes('player=1'), `自机 VM 在场（"${head}"）`);
      ctx.evidence.push(`borrowed/父子链：240 帧销毁性推进后 restore 全等 + 120 帧健康续推`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else {
    throw new Error(`m6_worldstate.mjs 不处理 case ${caseId}`);
  }
}
