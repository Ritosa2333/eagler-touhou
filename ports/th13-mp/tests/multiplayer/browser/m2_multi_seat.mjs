export async function run(ctx) {
  const SEED = 47932;
  const RIGHT = 0x80, UP = 0x10;

  const seatState = (client, seat) =>
    client.page.evaluate(seat => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_seat_state(seat))), seat);

  const openMpClient = async ({seats, characters}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}`});

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('independent_inputs')) {
    const dual = await openMpClient({seats: 2, characters: [0, 0]});
    const single = await ctx.openClient({kind: 'manual', seed: SEED});
    try {
      const state0 = await seatState(dual, 0);
      ctx.assert(state0.seat_count === 2, `2 席会话 seat_count=${state0.seat_count}`);
      const start1 = (await seatState(dual, 1)).x;
      for (let frame = 0; frame < 90; ++frame) {
        await dual.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), RIGHT);
        const stageBefore = await dual.page.evaluate(() => window.mod._th13_debug_stage_frame());
        const okDual = await dual.page.evaluate(() => window.mod._th13_debug_frame());
        const okSingle = await single.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(okDual === 1 && okSingle === 1, `帧泵 ok@${frame}（dual=${okDual} single=${okSingle}）`);
        const stage = await dual.page.evaluate(() => window.mod._th13_debug_stage_frame());
        ctx.assert(stage === stageBefore + 1, `世界帧账 +1（${stageBefore}→${stage}）`);
        const m0 = await seatState(dual, 0), s0 = await seatState(single, 0);
        ctx.assert(m0.x === s0.x && m0.y === s0.y,
          `seat 0 轨迹一致@帧${frame}（mp=(${m0.x},${m0.y}) sp=(${s0.x},${s0.y})）`);
        ctx.assert(m0.phase === s0.phase && m0.invulnerable === s0.invulnerable,
          `seat 0 相位/无敌一致@帧${frame}`);
      }
      const e1 = await seatState(dual, 1);
      ctx.assert(e1.x > start1 + 100, `seat 1 独立右移（${start1}→${e1.x}，录像流不影响注入席）`);
      ctx.evidence.push(`2 席：seat 0 与单席逐帧一致；seat 1 独立移动 x=${e1.x}`);
    } finally {
      await dual.assertDelivery();
      await single.assertDelivery();
      await dual.close();
      await single.close();
    }
    const triple = await openMpClient({seats: 3, characters: [0, 0, 0]});
    try {
      const start2 = (await seatState(triple, 2)).y;
      for (let frame = 0; frame < 90; ++frame) {
        await triple.page.evaluate(held => window.mod._th13_debug_mp_seat_input(2, held), UP);
        const ok = await triple.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `3 席帧泵 ok@${frame}`);
      }
      const s2 = await seatState(triple, 2);
      ctx.assert(s2.y < start2 - 100, `seat 2 上移（${start2}→${s2.y}）`);
      ctx.evidence.push(`3 席 seat 2 垂直：y=${start2}→${s2.y}`);
    } finally {
      await triple.assertDelivery();
      await triple.close();
    }
  } else if (caseId.includes('four_characters')) {
    for (let a = 0; a < 4; ++a) {
      for (let b = 0; b < 4; ++b) {
        const client = await openMpClient({seats: 2, characters: [a, b]});
        try {
          const s0 = await seatState(client, 0), s1 = await seatState(client, 1);
          ctx.assert(s0.character === a, `seat 0 角色记录 ${s0.character}==${a}`);
          ctx.assert(s1.character === b, `seat 1 角色记录 ${s1.character}==${b}`);
          ctx.assert(s0.seat_count === 2, `席位数 2（${s0.seat_count}）`);
          const start = {x0: s0.x, x1: s1.x};
          for (let frame = 0; frame < 30; ++frame) {
            await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), RIGHT);
            const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
            ctx.assert(ok === 1, `组合(${a},${b}) 帧泵 ok@${frame}`);
          }
          const e0 = await seatState(client, 0), e1 = await seatState(client, 1);
          ctx.assert(Math.abs(e0.x - start.x0) < 1e-3, `seat 0 零输入不动（${start.x0}→${e0.x}）`);
          const scripts = await client.page.evaluate(() => {
            const report = window.mod.UTF8ToString(window.mod._th13_debug_stage_report());
            return [...report.matchAll(/^\s+player script=(\d+)/gm)].map(match => Number(match[1]));
          });
          ctx.assert(scripts.length === 2 && scripts[0] === 0 && scripts[1] === 3,
            `组合(${a},${b}) 行走动画独立（${scripts.join(',')}，预期 0,3）`);
          const moved = e1.x - start.x1;
          ctx.assert(moved > 100, `seat 1 右移（${start.x1}→${e1.x}）`);
          const expectedPerFrame = b === 1 ? 640 / 128 : 576 / 128;
          ctx.assert(Math.abs(moved - 30 * expectedPerFrame) < 2.0,
            `角色 ${b} 定标位移 ${moved.toFixed(2)} ≈ 30×${expectedPerFrame}`);
          ctx.evidence.push(`组合(${a},${b})：seat1 位移 ${moved.toFixed(2)}`);
        } finally {
          await client.assertDelivery();
          await client.close();
        }
      }
    }
  } else {
    throw new Error(`m2_multi_seat.mjs 不处理 case ${caseId}`);
  }
}
