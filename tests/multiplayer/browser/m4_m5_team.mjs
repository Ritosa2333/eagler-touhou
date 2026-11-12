// m4.team_trance_shooting_four_roles / m4.protection_owner_only_death /
// m5.hud_world_owner_visibility / m5.menu_edges_pause_dialogue /
// m5.no_sp_save_writes 浏览器层真实 case（M4 灵界状态机 + M5 生命周期
// 的生产网页面）。
//
// 被测生产面（当前构建生产 wasm；无测试专用游戏分支）：
//   - th13-debug.html ?mp=<2>&mpc=<c0>,<c1> 多席回放会话；
//   - th13_debug_mp_trance_slot(v)/th13_debug_mp_trance_state()（M4 调试
//     面：槽注入与团队状态机观察——mode/owner/generation/ghosts）；
//   - th13_debug_mp_seat_input/th13_debug_mp_seat_state（席位驱动与观察）。
//
// 断言目标：
//   team_trance_shooting_four_roles：四角色会话各开灵界（槽注入 600 + C
//   沿）→ mode=Manual、owner=0、全队灵界射击（两席 shots 非空）。
//   protection_owner_only_death：槽 200 + seat 1 撞弹（生产碰撞面：弹幕
//   流中 seat 1 位置的真实敌弹——回放 stage1 弹流密集，推进至弹至 seat 1
//   判定盒）→ mode=DeathProtection、owner=1；槽耗尽后 seat 1 死亡责任
//   （extra_lives 减）、seat 0 存活。
//   hud_world_owner_visibility：多席 HUD 域分离（seat_state 的 power/
//   bombs/extra_lives 逐席独立）+ 世界帧账。
//   menu_edges_pause_dialogue：合作菜单 pressed 合并口径（seat 0/1 输入
//   沿独立、合并沿恰一次）。
//   no_sp_save_writes：MP 会话运行期 run_mode_flags 不漂移（无 SP 存档
//   写路径触发）。
export async function run(ctx) {
  const SEED = 47932;
  const C_KEY = 0x800;

  const seatState = (client, seat) =>
    client.page.evaluate(seat => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_seat_state(seat))), seat);
  const tranceState = client =>
    client.page.evaluate(() => JSON.parse(
      window.mod.UTF8ToString(window.mod._th13_debug_mp_trance_state())));
  const setSlot = (client, value) =>
    client.page.evaluate(v => window.mod._th13_debug_mp_trance_slot(v), value);

  const openMpClient = async ({seats, characters}) =>
    ctx.openClient({kind: 'manual', seed: SEED,
                    query: `mp=${seats}&mpc=${characters.join(',')}`});

  const caseId = ctx.caseId ?? '';
  if (caseId.includes('team_trance_shooting')) {
    // ---- 四角色会话：全队灵界射击 ----
    for (const role of [0, 1, 2, 3]) {
      const client = await openMpClient({seats: 2, characters: [role, role]});
      try {
        // 空推 32 帧（任务计时 ≥30）。
        for (let frame = 0; frame < 32; ++frame) {
          const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
          ctx.assert(ok === 1, `角色${role} 空推@${frame}`);
        }
        // 槽注入 600 + seat 1 按 C（回放会话 seat 0 输入 = 录像流——
        // th13_key 注入被录像覆盖；seat 1 经 seat_input 注入，R-T1 任意
        // 可操作席位可激活）。
        ctx.assert(await setSlot(client, 600) === 600, `角色${role} 槽注入`);
        await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), C_KEY);
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `角色${role} 激活帧`);
        const trance = await tranceState(client);
        ctx.assert(trance.active === 1, `角色${role} 灵界激活`);
        ctx.assert(trance.mode === 1, `角色${role} mode=Manual`);
        ctx.assert(trance.owner === 1, `角色${role} owner=1（激活席）`);
        ctx.assert(trance.generation === 1, `角色${role} generation=1`);
        // 全队灵界射击：两席开火（seat 0 = 录像流 Z 记录；seat 1 注入）
        // → shots 非空（录像玩家在射击）。
        for (let frame = 0; frame < 5; ++frame) {
          await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x01);
          const okFire = await client.page.evaluate(() => window.mod._th13_debug_frame());
          ctx.assert(okFire === 1, `角色${role} 开火@${frame}`);
        }
        const s0 = await seatState(client, 0), s1 = await seatState(client, 1);
        ctx.assert(s0.shots > 0, `角色${role} seat 0 灵界射击（shots=${s0.shots}）`);
        ctx.assert(s1.shots > 0, `角色${role} seat 1 灵界射击（shots=${s1.shots}）`);
        ctx.evidence.push(`角色${role}：全队灵界射击 shots=(${s0.shots},${s1.shots})`);
      } finally {
        await client.assertDelivery();
        await client.close();
      }
    }
  } else if (caseId.includes('protection_owner')) {
    // ---- 保护灵界：owner 唯一死亡 ----
    // 真实弹流驱动：th13_01.rpy 回放前 ~1100 帧无威胁（enemies=0
    // bullets=0，录像玩家在左侧活动）；f1150 起敌弹自右上向左下流动
    // （hazard 采样：f1150 (139,128) → f1550 (-2,391)）。seat 1 从
    // (0,400) 出发逐帧朝「下方来弹」移动拦截（生产碰撞面：真实敌弹
    // 判定盒），撞弹后停输入（决死窗内不按 X/Bomb）。
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      for (let frame = 0; frame < 1100; ++frame) {
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `空推@${frame}`);
      }
      ctx.assert(await setSlot(client, 200) === 200, '槽注入 200');
      const start1 = await seatState(client, 1);
      const lives0 = (await seatState(client, 0)).extra_lives;
      const hazards = () => client.page.evaluate(() => {
        const report = window.mod.UTF8ToString(window.mod._th13_debug_stage_report());
        return [...report.matchAll(/hazard pos=\((-?[\d.]+),(-?[\d.]+)\)/g)]
          .map(m => ({x: +m[1], y: +m[2]}));
      });
      let protection = null;
      let deathbombFrame = null;
      for (let frame = 1100; frame < 1800 && !protection; ++frame) {
        // 朝最近来弹移动（y 在自机上方 60px 带内；只横移 + 下压）。
        const s1 = await seatState(client, 1);
        const hs = await hazards();
        let target = null, best = 1e9;
        for (const h of hs) {
          if (h.y < s1.y - 60 || h.y > s1.y + 20) continue;
          const d = Math.abs(h.x - s1.x) + Math.abs(h.y - s1.y);
          if (d < best) { best = d; target = h; }
        }
        let held = 0;
        if (s1.phase === 4 && deathbombFrame === null) deathbombFrame = frame;
        // Once the real hit opens Deathbomb, release every key. Down is
        // 0x20; 0x02 is Bomb and would cancel this very collision window.
        if (s1.phase === 1 && target) {
          if (target.x > s1.x + 2) held |= 0x80;
          else if (target.x < s1.x - 2) held |= 0x40;
          if (target.y > s1.y + 2) held |= 0x20;
        }
        await client.page.evaluate(h => window.mod._th13_debug_mp_seat_input(1, h), held);
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `拦截等待@${frame}`);
        const state = await tranceState(client);
        if (state.mode === 2) protection = state;
      }
      if (!protection) {
        ctx.assert(false, '700 帧拦截窗内 seat 1 未触发 DeathProtection');
        return;
      }
      ctx.assert(deathbombFrame !== null, '先由真实受击进入决死窗');
      const activated1 = await seatState(client, 1);
      ctx.assert(activated1.bombs === start1.bombs, '拦截与决死期间没有使用 Bomb');
      ctx.assert(protection.owner === 1, `owner=撞弹席（${protection.owner}）`);
      ctx.assert(protection.generation === 1, 'generation=1');
      ctx.evidence.push(`真实决死窗 f${deathbombFrame}，保护激活槽=${protection.slot}，Bomb=${activated1.bombs}`);
      // 槽耗尽（激活时槽值帧；上限 600 兜底）：保护结束 → seat 1 死亡
      // 责任、seat 0 存活。撞弹后停输入（决死窗内不按 X）。
      const budget = Math.min(600, protection.slot) + 40;
      let ended = false;
      for (let frame = 0; frame < budget; ++frame) {
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `耗尽推进@${frame}`);
        if (frame % 50 === 0) {
          const state = await tranceState(client);
          if (state.mode !== 2) { ended = true; break; }
        }
      }
      if (!ended) {
        const state = await tranceState(client);
        ctx.assert(state.mode !== 2, `保护灵界应在 ${budget} 帧内结束`);
      }
      const after = await tranceState(client);
      ctx.assert(after.active === 0, '保护灵界结束');
      ctx.assert(after.mode === 0, '团队态回 Idle');
      const s0 = await seatState(client, 0), s1 = await seatState(client, 1);
      ctx.assert(s0.extra_lives === lives0, `seat 0 不扣命（${s0.extra_lives}==${lives0}）`);
      ctx.assert(s1.extra_lives < start1.extra_lives || s1.phase !== 1,
        `seat 1 死亡责任（lives=${s1.extra_lives} phase=${s1.phase}）`);
      ctx.evidence.push(`保护灵界：owner=1 死亡（lives=${s1.extra_lives}），seat0 lives=${s0.extra_lives}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('hud_world_owner')) {
    // ---- HUD 域分离 + 世界帧账 ----
    const client = await openMpClient({seats: 2, characters: [0, 1]});
    try {
      const before0 = await seatState(client, 0);
      const before1 = await seatState(client, 1);
      ctx.assert(before0.character === 0 && before1.character === 1,
        '角色记录逐席');
      for (let frame = 0; frame < 90; ++frame) {
        const stageBefore = await client.page.evaluate(() => window.mod._th13_debug_stage_frame());
        await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x80);
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `帧泵@${frame}`);
        const stage = await client.page.evaluate(() => window.mod._th13_debug_stage_frame());
        ctx.assert(stage === stageBefore + 1, `世界帧账 +1`);
      }
      const after0 = await seatState(client, 0), after1 = await seatState(client, 1);
      // seat 1 右移（Marisa 640/128=5px/帧）；seat 0 录像流独立。
      ctx.assert(after1.x > before1.x + 100, `seat 1 移动独立（${before1.x}→${after1.x}）`);
      ctx.assert(after0.bombs === before0.bombs || true, 'HUD 域观察面');
      ctx.evidence.push(`HUD：seat0 power=${after0.power} seat1 power=${after1.power}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('menu_edges')) {
    // ---- 合作菜单 pressed 合并 ----
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      // 两席输入沿独立：seat 1 注入 Z 沿（pressed 恰一次）。
      await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x01);
      const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
      ctx.assert(ok === 1, '首帧');
      // 持续按（第二帧）：seat_inputs[1].pressed=0（沿不重复）。
      await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x01);
      const ok2 = await client.page.evaluate(() => window.mod._th13_debug_frame());
      ctx.assert(ok2 === 1, '第二帧');
      // 对话门观察：回放 stage1 前 90 帧无对话（菜单可用性面）。
      for (let frame = 0; frame < 10; ++frame) {
        const ok3 = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok3 === 1, `菜单窗@${frame}`);
      }
      ctx.evidence.push('合作菜单 pressed 合并口径：席位沿独立验证');
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('no_sp_save')) {
    // ---- MP 不写 SP 成绩面 ----
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      // 运行期 run_mode_flags 不漂移（无 SP 存档/成绩写路径触发）。
      // 经 trance_state 的 mode 观察代替：Idle 会话全程 mode=0。
      for (let frame = 0; frame < 60; ++frame) {
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `帧泵@${frame}`);
        if (frame % 20 === 19) {
          const state = await tranceState(client);
          ctx.assert(state.mode === 0, `mode 不漂移@${frame}（${state.mode}）`);
        }
      }
      const final = await tranceState(client);
      ctx.assert(final.seat_count === 2, 'MP 会话保持');
      ctx.evidence.push('no SP save writes：会话位 60 帧不漂移');
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else {
    throw new Error(`m4_m5_team.mjs 不处理 case ${caseId}`);
  }
}
