export async function run(ctx) {
  const SEED = 47932;

  const seatState = (client, seat) =>
    client.page.evaluate(seat => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_seat_state(seat))), seat);
  const itemReport = client =>
    client.page.evaluate(() => window.mod.UTF8ToString(window.mod._th13_debug_item_report()));

  const openMpClient = async ({seats, characters}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}`});

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('target_all_consumers')) {
    const dual = await openMpClient({seats: 2, characters: [0, 0]});
    const single = await ctx.openClient({kind: 'manual', seed: SEED});
    try {
      const state0 = await seatState(dual, 0);
      ctx.assert(state0.seat_count === 2, `2 席会话 seat_count=${state0.seat_count}`);
      for (let frame = 0; frame < 90; ++frame) {
        const stageBefore = await dual.page.evaluate(() => window.mod._th13_debug_stage_frame());
        const okDual = await dual.page.evaluate(() => window.mod._th13_debug_frame());
        const okSingle = await single.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(okDual === 1 && okSingle === 1, `帧泵 ok@${frame}`);
        const stage = await dual.page.evaluate(() => window.mod._th13_debug_stage_frame());
        ctx.assert(stage === stageBefore + 1, `世界帧账 +1（${stageBefore}→${stage}）`);
        const m0 = await seatState(dual, 0), s0 = await seatState(single, 0);
        ctx.assert(m0.x === s0.x && m0.y === s0.y,
          `seat 0 轨迹一致@帧${frame}（mp=(${m0.x},${m0.y}) sp=(${s0.x},${s0.y})）`);
        ctx.assert(m0.phase === s0.phase && m0.invulnerable === s0.invulnerable,
          `seat 0 相位/无敌一致@帧${frame}`);
      }
      ctx.evidence.push('多席目标面：seat 0 与单席对照 90 帧逐帧一致');
    } finally {
      await dual.assertDelivery();
      await single.assertDelivery();
      await dual.close();
      await single.close();
    }
  } else if (caseId.includes('shared_pickup')) {
    const dual = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      const state0 = await seatState(dual, 0);
      ctx.assert(state0.seat_count === 2, `2 席会话 seat_count=${state0.seat_count}`);
      const start = {s0: await seatState(dual, 0), s1: await seatState(dual, 1)};
      let power0 = start.s0.power, power1 = start.s1.power;
      let bombs0 = start.s0.bombs, bombs1 = start.s1.bombs;
      let bothChanged = false, independent = true;
      for (let frame = 0; frame < 240; ++frame) {
        await dual.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x80);
        const ok = await dual.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `帧泵 ok@${frame}`);
        const s0 = await seatState(dual, 0), s1 = await seatState(dual, 1);
        if (s0.power !== power0 && s1.power !== power1) bothChanged = true;
        if (s0.power !== power0 && s1.power !== power1 && s0.power === s1.power)
          independent = false;
        power0 = s0.power; power1 = s1.power;
        bombs0 = s0.bombs; bombs1 = s1.bombs;
        if (frame === 0) {
          const report = await itemReport(dual);
          ctx.assert(report.startsWith('rate='), `道具池报告可读（${report.length}B）`);
        }
      }
      ctx.assert(independent, '两席 HUD 资源变化独立（无同时同值广播）');
      ctx.evidence.push(`240 帧道具流：seat0 power=${power0} bombs=${bombs0} | ` +
                        `seat1 power=${power1} bombs=${bombs1} bothSeen=${bothChanged}`);
    } finally {
      await dual.assertDelivery();
      await dual.close();
    }
  } else {
    throw new Error(`m3_world_resources.mjs 不处理 case ${caseId}`);
  }
}
