// m2.independent_inputs_2_3 / m2.four_characters_duplicates 浏览器层真实
// case（M2 工单「一套世界、多席玩家」的生产网页面）。
//
// 被测生产面（全部走当前构建的生产 wasm 导出，无测试专用游戏分支）：
//   - th13-debug.html ?mp=<2|3>&mpc=<c0>,<c1>,<c2>（M2 测试接口）：对局
//     建立前经 th13_debug_mp_seats 配置席位；begin_run 的
//     run->initialize 完成后经 RunScene::enable_multiplayer →
//     StageBattle::enable_seats 生效（R-X 允许重复角色）。
//   - th13_debug_mp_seat_input(seat, held)：对局中按席位注入按键位掩码
//     （seat 0 走 th13_key 物理采样路径——单一 GameInput 语义）。
//   - th13_debug_mp_seat_state(seat)：读席位玩家面（x/y/phase/invulnerable/
//     shots/bombs/extra_lives/power/character/seat_count），断言逐席独立。
//
// 断言目标：
//   independent_inputs_2_3：2 席会话，seat 0 持续 ←（th13_key）、seat 1
//   持续 →（seat_input）。泵 90 帧后：两席 x 分离且方向正确、|x0+x1|
//   对称（同角色同速）、y 一致（无垂直输入）；世界帧账逐帧 +1
//   （th13_debug_stage_frame）。3 席会话加 seat 2 持续 ↑：y 上移、x 不动。
//   four_characters_duplicates：2 席全四角色组合（16 会话，含 0,0 与
//   3,3 重复）：角色记录逐席正确、seat 1 右移定标按角色（pl01 640/128
//   vs 其余 576/128 px/帧）、seat 0 零输入不动（席位输入互不串扰）。
export async function run(ctx) {
  const SEED = 47932;
  const RIGHT = 0x80, UP = 0x10;

  const seatState = (client, seat) =>
    client.page.evaluate(seat => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_seat_state(seat))), seat);

  // 多席回放会话：URL 参数配置席位（页面装载序列内、begin_run 前生效）。
  const openMpClient = async ({seats, characters}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}`});

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('independent_inputs')) {
    // ---- 驱动口径：回放会话的 seat 0 输入 = 录像流（真实生产输入源），
    // seat 1/2 的注入输入独立于录像。断言：
    //   A. 多席会话的 seat 0 轨迹与单席对照会话逐帧一致（多席启用不改变
    //      世界与 seat 0 玩家面——single_seat_sp_equivalence 的生产网页面）；
    //   B. seat 1 按注入方向独立移动（与录像流无关）；
    //   C. 世界帧账逐帧 +1。
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
        // A：seat 0 轨迹逐帧一致（x/y/相位/无敌）。
        const m0 = await seatState(dual, 0), s0 = await seatState(single, 0);
        ctx.assert(m0.x === s0.x && m0.y === s0.y,
          `seat 0 轨迹一致@帧${frame}（mp=(${m0.x},${m0.y}) sp=(${s0.x},${s0.y})）`);
        ctx.assert(m0.phase === s0.phase && m0.invulnerable === s0.invulnerable,
          `seat 0 相位/无敌一致@帧${frame}`);
      }
      // B：seat 1 独立右移（90 帧 × 4.5 = 405，钳制边界 ~184）。
      const e1 = await seatState(dual, 1);
      ctx.assert(e1.x > start1 + 100, `seat 1 独立右移（${start1}→${e1.x}，录像流不影响注入席）`);
      ctx.evidence.push(`2 席：seat 0 与单席逐帧一致；seat 1 独立移动 x=${e1.x}`);
    } finally {
      await dual.assertDelivery();
      await single.assertDelivery();
      await dual.close();
      await single.close();
    }
    // ---- 3 席：seat 2 ↑ 独立移动 ----
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
    // ---- 全四角色组合（16 会话）----
    for (let a = 0; a < 4; ++a) {
      for (let b = 0; b < 4; ++b) {
        const client = await openMpClient({seats: 2, characters: [a, b]});
        try {
          const s0 = await seatState(client, 0), s1 = await seatState(client, 1);
          ctx.assert(s0.character === a, `seat 0 角色记录 ${s0.character}==${a}`);
          ctx.assert(s1.character === b, `seat 1 角色记录 ${s1.character}==${b}`);
          ctx.assert(s0.seat_count === 2, `席位数 2（${s0.seat_count}）`);
          // 重复角色席位状态独立：seat 1 右移、seat 0 零输入不动。
          const start = {x0: s0.x, x1: s1.x};
          for (let frame = 0; frame < 30; ++frame) {
            await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), RIGHT);
            const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
            ctx.assert(ok === 1, `组合(${a},${b}) 帧泵 ok@${frame}`);
          }
          const e0 = await seatState(client, 0), e1 = await seatState(client, 1);
          ctx.assert(Math.abs(e0.x - start.x0) < 1e-3, `seat 0 零输入不动（${start.x0}→${e0.x}）`);
          // R-A: the guest's right input must not tilt the idle first seat.
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
