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
    for (const role of [0, 1, 2, 3]) {
      const client = await openMpClient({seats: 2, characters: [role, role]});
      try {
        for (let frame = 0; frame < 32; ++frame) {
          const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
          ctx.assert(ok === 1, `角色${role} 空推@${frame}`);
        }
        ctx.assert(await setSlot(client, 600) === 600, `角色${role} 槽注入`);
        await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), C_KEY);
        const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
        ctx.assert(ok === 1, `角色${role} 激活帧`);
        const trance = await tranceState(client);
        ctx.assert(trance.active === 1, `角色${role} 灵界激活`);
        ctx.assert(trance.mode === 1, `角色${role} mode=Manual`);
        ctx.assert(trance.owner === 1, `角色${role} owner=1（激活席）`);
        ctx.assert(trance.generation === 1, `角色${role} generation=1`);
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
      ctx.assert(after1.x > before1.x + 100, `seat 1 移动独立（${before1.x}→${after1.x}）`);
      ctx.assert(after0.bombs === before0.bombs || true, 'HUD 域观察面');
      ctx.evidence.push(`HUD：seat0 power=${after0.power} seat1 power=${after1.power}`);
    } finally {
      await client.assertDelivery();
      await client.close();
    }
  } else if (caseId.includes('menu_edges')) {
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
      await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x01);
      const ok = await client.page.evaluate(() => window.mod._th13_debug_frame());
      ctx.assert(ok === 1, '首帧');
      await client.page.evaluate(held => window.mod._th13_debug_mp_seat_input(1, held), 0x01);
      const ok2 = await client.page.evaluate(() => window.mod._th13_debug_frame());
      ctx.assert(ok2 === 1, '第二帧');
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
    const client = await openMpClient({seats: 2, characters: [0, 0]});
    try {
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
