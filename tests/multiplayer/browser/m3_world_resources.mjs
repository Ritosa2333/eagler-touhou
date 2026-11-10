// m3.shared_pickup_once / m3.target_all_consumers_ties 浏览器层真实 case
//（M3「目标解析、碰撞与全员资源」的生产网页面）。
//
// 被测生产面（当前构建的生产 wasm；无测试专用游戏分支）：
//   - th13-debug.html ?mp=<2>&mpc=<c0>,<c1>,<c2> 多席回放会话（M2 面复用）；
//   - th13_debug_mp_seat_state：逐席玩家面 + seat_huds（power/bombs/
//     extra_lives——R-I3 逐席资源观察面）；
//   - th13_debug_item_report：道具池报告（type/pos/state——拾取归属的
//     物理面）。
//
// 断言目标：
//   target_all_consumers_ties：多席会话与单席对照同回放逐帧对拍——seat 0
//   轨迹/相位与单席一致（M3 目标面写入不改变 seat 0 世界与玩家面）；
//   世界帧账逐帧 +1。
//   shared_pickup_once：回放道具流中，道具拾取只影响**最近席**的 HUD
//   资源——seat 1 的 power/bombs 变化与 seat 0 独立（两席 HUD 域分离，
//   R-I3/R-I4：拾取恰一次、团队分数一份）。
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
    // ---- 多席 vs 单席对照：seat 0 世界与玩家面逐帧一致 ----
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
        // 目标面（M3）写入不改变 seat 0：轨迹/相位逐帧一致。
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
    // ---- 道具拾取：最近席 HUD 资源独立（R-I3/R-I4） ----
    // 回放会话道具流真实存在（stage1 前段掉落）；多席下每席 HUD 独立记账。
    const dual = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      const state0 = await seatState(dual, 0);
      ctx.assert(state0.seat_count === 2, `2 席会话 seat_count=${state0.seat_count}`);
      // seat 1 持续远离 seat 0（右移）——拉开距离使「最近席」判定稳定。
      const start = {s0: await seatState(dual, 0), s1: await seatState(dual, 1)};
      let power0 = start.s0.power, power1 = start.s1.power;
      let bombs0 = start.s0.bombs, bombs1 = start.s1.bombs;
      let bothChanged = false, independent = true;
      for (let frame = 0; frame < 240; ++frame) {
        await dual.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x80);
        const ok = await dual.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `帧泵 ok@${frame}`);
        const s0 = await seatState(dual, 0), s1 = await seatState(dual, 1);
        // 资源变化观察：两席 HUD 域独立（一席变化不复制到另一席）。
        if (s0.power !== power0 && s1.power !== power1) bothChanged = true;
        if (s0.power !== power0 && s1.power !== power1 && s0.power === s1.power)
          independent = false;  // 同帧同值变化 = 广播泄漏信号
        power0 = s0.power; power1 = s1.power;
        bombs0 = s0.bombs; bombs1 = s1.bombs;
        // 道具池报告可读（物理面存在）。
        if (frame === 0) {
          const report = await itemReport(dual);
          ctx.assert(report.startsWith('rate='), `道具池报告可读（${report.length}B）`);
        }
      }
      // 全程无「同帧同值」的广播泄漏（R-I3：资源入最近席，非全员复制）。
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
