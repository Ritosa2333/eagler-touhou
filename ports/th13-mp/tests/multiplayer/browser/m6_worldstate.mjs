// m6.full_state_restore_replay / m6.anm_borrowed_parent_chains 浏览器层
// 真实 case（M6 结构化快照的生产网页面）。
//
// 被测生产面（当前构建生产 wasm；无测试专用游戏分支）：
//   - th13-debug.html ?mp=<2>&mpc=<c0>,<c1> 多席回放会话；
//   - th13_debug_mp_snapshot_capture/restore/digest/drop（M6 调试面：
//     生产 WorldState 的 capture/restore/digest 驱动——被测的是
//     web/cpp/multiplayer/WorldState.* 本体，接口只做透传）；
//   - th13_debug_mp_seat_state/th13_debug_stage_report（权威态观测）。
//
// 断言目标：
//   full_state_restore_replay：真实回放会话 S→前进→restore(S)→digest 全等
//   （生产 ANM registry 含敌机槽位/自机弹/特效 VM 与父子链），再喂同帧
//   泵推进后权威态（敌机/弹/自机弹 digest 面与席位状态）与 restore 前一致。
//   anm_borrowed_parent_chains：生产 registry 的 borrowed 根（暂停 overlay
//   面板等外部 VM）与父子链——capture→销毁性推进（敌机死亡/弹消/VM 自灭
//   走真实链回收）→restore 后 registry 计数/句柄面恢复，且后续帧泵不因
//   链断裂报错（错误面恒空）。
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
    // ---- S→前进→restore(S)→digest 全等 + 同输入续推一致 ----
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      // 推进到有敌机/弹幕/自机弹的真实战斗窗（th13_01 回放 f1100+ 有敌弹流）。
      for (let frame = 0; frame < 1180; ++frame) await runFrame(client);
      const before = (await stageReport(client)).split('\n')[0];
      // capture（生产 ANM registry 面全量）。
      const captured = await snapshotCapture(client);
      ctx.assert(captured.ok === 1, `capture ok（${JSON.stringify(captured)}）`);
      ctx.assert(captured.digest.length === 16, 'digest 64bit hex');
      ctx.assert(captured.retained > 0, '快照非空');
      const captureDigest = captured.digest;
      // 前进 60 帧（发散：弹幕推进/敌机移动/VM 消长）。
      for (let frame = 0; frame < 60; ++frame) await runFrame(client);
      const diverged = (await stageReport(client)).split('\n')[0];
      // restore。
      const restored = await snapshotRestore(client);
      ctx.assert(restored.ok === 1, `restore ok（${JSON.stringify(restored)}）`);
      ctx.assert(restored.digest === captureDigest,
        `restore digest == capture digest（${restored.digest} vs ${captureDigest}）`);
      // 再 capture 一份交叉验证（restore 后未推帧——同状态同 digest）。
      const verify = await snapshotCapture(client);
      ctx.assert(verify.digest === captureDigest, '重 capture 同 digest');
      // 权威态观测面：restore 后先跑一帧（表现层 sync 按权威态对齐——
      // stageReport 头行的 shots/items 计数是表现层手账，restore 不直接
      // 恢复表现缓存，下一帧重建），观测敌机/弹幕权威计数健康。
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
      // 同输入续推：restore 分支与另一客户端基准的**权威模拟面 digest**
      // 逐帧一致（digest_core：席位/敌机/弹/道具/神霊/RNG/会话——不含
      // anm registry 表现层 VM 池）。全量 digest 不作续推对照：特效粒子流
      // （每帧出生/死亡的视觉 VM）在 restore 分支被回滚丢弃 60 帧视觉
      // 历史，与从未回滚的连续基线天然不同批——回滚语义而非缺陷；粒子
      // 流的快照/恢复正确性由 restore 全等 + 重 capture 交叉验证 +
      // anm_borrowed_parent_chains case 覆盖。
      const snapshotDigestCore = client2 =>
        client2.page.evaluate(() => JSON.parse(
          window.mod.UTF8ToString(window.mod._th13_debug_mp_snapshot_digest_core())));
      const baseline = await openMpClient({seats: 2, characters: [0, 0]});
      try {
        for (let frame = 0; frame < 1180; ++frame) await runFrame(baseline);
        // 相位对齐：client 在续推前多跑了 1 帧观测（上面的 restore 一帧
        // 观测），baseline 补齐同帧——两侧从同一起点进入续推循环。
        await runFrame(baseline);
        for (let frame = 0; frame < 12; ++frame) {
          await runFrame(client);
          await runFrame(baseline);
          // digest 接口读快照数据：先 capture 当前权威态再取 core digest。
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
        // 席位面一致。
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
    // ---- 生产 registry 父子链/borrowed 根：销毁性推进后恢复 ----
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      for (let frame = 0; frame < 1180; ++frame) await runFrame(client);
      const captured = await snapshotCapture(client);
      ctx.assert(captured.ok === 1, 'capture ok');
      const captureDigest = captured.digest;
      // 销毁性推进 240 帧：敌机死亡回收/弹幕消长/VM 自灭走真实链回收
      //（th13_01 回放窗内敌机批次更替，registry 成员大幅换血）。
      for (let frame = 0; frame < 240; ++frame) await runFrame(client);
      // restore。
      const restored = await snapshotRestore(client);
      ctx.assert(restored.ok === 1, 'restore ok');
      ctx.assert(restored.digest === captureDigest, 'restore digest 全等');
      // 恢复后帧泵不因链断裂报错（错误面恒空）——续推 120 帧。
      for (let frame = 0; frame < 120; ++frame) {
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `续推@${frame}`);
      }
      const errorText = await client.page.evaluate(() => window.mod.UTF8ToString(
        window.mod._th13_error()));
      ctx.assert(errorText === '', `restore 后续推无错误（"${errorText}"）`);
      // 权威观测面健康（敌机/弹计数非负、自机在场）。
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
