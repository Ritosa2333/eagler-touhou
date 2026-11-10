// m1.draw_purity_rng 浏览器层真实 case（M1/T8 测试作者；M1-plan §2/T8、§3 矩阵）。
//
// 断言目标（M1「Draw 不得推进模拟/RNG」的生产网页面）：
//   1. 3 个客户端同 seed 同录像（th13_01.rpy stage1）状态逐帧全等——
//      2 个手动泵客户端（th13-debug.html 逐帧 th13_debug_frame，页内 rAF 不启动）
//      + 1 个 rAF 自然泵客户端（th13_loop_start，经生产回调 Module.onGameFrame
//      观测，BrowserRuntime.cpp:32-34）。probe_state[16]（Application.cpp:1640-1674
//      单席 16 槽 projection，含 [7]=frames、[5]=stage_frame、[8]=score）逐帧全等
//      ⇒ 泵来源与呈现次数不改变模拟/RNG 轨迹。
//   2. 选定帧 canvas dataURL 逐字节一致（手动泵客户端之间 + 全新第 4 会话重泵复核）
//      ⇒ 绘制输出确定性（与 shot-frames.mjs:154-158 确定性自检同判据）。
//   3. rAF 客户端帧计数账目：每回调 frame 增量 == ticks（逻辑帧恰 +1/帧）。
//   4. 每个客户端送达的 th13-sdl.mjs/wasm 字节 == build.json 产物（仓根
//      verified-runtime.mjs；wasmSha256/servedWasmSha256 与 build.json 不一致即失败）。
//
// 无任何测试专用游戏分支：全部走生产 th13-debug.html 会话建立
// （th13_boot_seed → th13_draw_startup(90) → th13_debug_replay → 逐帧泵）与
// 生产 rAF 循环；种子经 window.__th13_boot_seed（Application.cpp:80-88）注入。
export async function run(ctx) {
  const SEED = 47932;          // 固定 boot 种子（0..65535 合法域）
  // 手动泵逐帧观测窗：游戏帧 1..256。rAF 帧钟追赶批次尾帧可越过最后目标
  // （FrameCadence.advance 每回调 ≤4 帧，BrowserRuntime.cpp:247），256 覆盖
  // 目标 240 的最大过冲（243）并留余量。
  const WINDOW = 256;
  const SNAP = [30, 120, 240]; // dataURL 逐字节比对帧
  const RAF_TARGETS = [60, 120, 240];

  const probeAt = (records, frame) => records[frame - 1].probe;
  const checkSequential = (records, label) => {
    for (let i = 0; i < records.length; ++i) {
      ctx.assert(records[i].frame === i + 1,
        `${label} 帧计数连续：records[${i}].frame=${records[i].frame} 期望 ${i + 1}`);
    }
  };
  const checkProbeFrameIdentity = (records, label) => {
    for (const record of records) {
      ctx.assert(record.probe[7] === record.frame,
        `${label} probe_state[7]==th13_frame：帧 ${record.frame} 观测值 ${record.probe[7]}`);
    }
  };
  const checkProbeStreamsEqual = (a, b, labelA, labelB) => {
    for (let i = 0; i < a.length; ++i) {
      const same = a[i].probe.every((value, slot) => value === b[i].probe[slot]);
      ctx.assert(same, `${labelA}/${labelB} probe_state 逐帧全等@帧 ${a[i].frame}：` +
        `slot差异=[${a[i].probe.map((v, s) => v === b[i].probe[s] ? null : s).filter(v => v !== null)}]`);
    }
  };

  // ---- 客户端 1：手动泵，全窗逐帧 probe + 选定帧 dataURL ----
  const manual1 = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(manual1.bootSeed === SEED, `手动泵客户端1 th13_boot_seed==${SEED}`);
  await manual1.assertDelivery();
  ctx.evidence.push(`客户端1 manual：replay 总帧=${manual1.total} bootSeed=${manual1.bootSeed}`);
  const records1 = await manual1.pump({count: WINDOW, snap: SNAP});
  ctx.assert(records1.length === WINDOW, `客户端1 泵出 ${records1.length}/${WINDOW} 帧`);
  checkSequential(records1, '客户端1');
  checkProbeFrameIdentity(records1, '客户端1');

  // ---- 客户端 2：全新手动泵会话，同 seed 同录像同窗 ----
  const manual2 = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(manual2.bootSeed === SEED, `手动泵客户端2 th13_boot_seed==${SEED}`);
  await manual2.assertDelivery();
  const records2 = await manual2.pump({count: WINDOW, snap: SNAP});
  ctx.assert(records2.length === WINDOW, `客户端2 泵出 ${records2.length}/${WINDOW} 帧`);
  checkSequential(records2, '客户端2');
  checkProbeFrameIdentity(records2, '客户端2');
  // 手动泵 ×2：probe_state 逐帧全等（模拟/RNG 轨迹与会话建立方式无关）。
  checkProbeStreamsEqual(records1, records2, '客户端1', '客户端2');

  // ---- 选定帧 canvas dataURL 逐字节一致（同 kind 同帧号跨会话）----
  for (const frame of SNAP) {
    const url1 = records1[frame - 1].url;
    const url2 = records2[frame - 1].url;
    ctx.assert(typeof url1 === 'string' && url1.startsWith('data:image/png;base64,'),
      `客户端1 帧 ${frame} dataURL 形态`);
    ctx.assert(typeof url2 === 'string' && url2.startsWith('data:image/png;base64,'),
      `客户端2 帧 ${frame} dataURL 形态`);
    ctx.assert(url1 === url2,
      `选定帧 ${frame} canvas dataURL 逐字节一致（跨手动泵会话，` +
      `${url1.length}B vs ${url2.length}B）`);
    ctx.evidence.push(`帧 ${frame} dataURL 逐字节一致：${url1.length} 字节 ` +
      `png sha 前 12=${url1.slice(22, 34)}`);
  }

  // ---- 客户端 3：rAF 自然泵（th13_loop_start），观测至 targets 后暂停 ----
  const raf = await ctx.openClient({kind: 'raf', seed: SEED});
  ctx.assert(raf.bootSeed === SEED, `rAF 客户端 th13_boot_seed==${SEED}`);
  await raf.assertDelivery();
  await raf.startRaf(RAF_TARGETS);
  const rafState = await raf.waitRafTargets(RAF_TARGETS);
  ctx.assert(!rafState.failure, `rAF 客户端帧体无失败（${rafState.records.length} 回调观测）`);
  ctx.assert(rafState.paused, 'rAF 客户端抵达全部目标后经 th13_loop_pause 冻结');
  ctx.assert(rafState.done.length === RAF_TARGETS.length,
    `rAF 客户端目标帧批次到达数=${rafState.done.length}/${RAF_TARGETS.length}`);
  // 帧计数账目：每回调 frame 增量恰 == ticks（首记录从 0 起步）且 probe[7]==frame。
  let previous = 0;
  for (const record of rafState.records) {
    ctx.assert(record.frame === previous + record.ticks,
      `rAF 帧账目：frame ${previous}+ticks${record.ticks}==${record.frame}`);
    ctx.assert(record.probe[7] === record.frame,
      `rAF 帧账目 probe_state[7]==th13_frame@帧 ${record.frame}`);
    previous = record.frame;
  }
  // ticks 分布与覆盖先落证据，后做全等比对（后续断言失败也可诊断）。
  const ticksHistogram = {};
  for (const record of rafState.records)
    ticksHistogram[record.ticks] = (ticksHistogram[record.ticks] ?? 0) + 1;
  ctx.evidence.push(`rAF 客户端：${rafState.records.length} 次回调观测，ticks 分布=` +
    `${JSON.stringify(ticksHistogram)}，目标到达=${JSON.stringify(rafState.done)}`);
  // rAF 观测帧（ticks 批次尾帧）与手动泵流全等；ticks===1 时即逐帧全等。
  let compared = 0;
  for (const record of rafState.records) {
    ctx.assert(record.frame >= 1 && record.frame <= WINDOW,
      `rAF 观测帧 ${record.frame} 落在手动泵窗 [1,${WINDOW}] 内`);
    const manual = probeAt(records1, record.frame);
    const same = record.probe.every((value, slot) => value === manual[slot]);
    ctx.assert(same, `rAF/手动泵 probe_state 全等@帧 ${record.frame}（ticks=${record.ticks}）：` +
      `slot差异=[${record.probe.map((v, s) => v === manual[s] ? null : s).filter(v => v !== null)}]`);
    compared += 1;
  }
  ctx.evidence.push(`rAF/手动泵流比对帧数=${compared}/${WINDOW}（观测点=每回调批次尾帧）`);
  ctx.assert(compared >= 1, 'rAF 客户端至少一个观测帧与手动泵流比对');

  // ---- 客户端 4：全新会话重泵复核（同 seed 同录像，泵至选定帧再比对 dataURL）----
  const repump = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(repump.bootSeed === SEED, `重泵复核客户端 th13_boot_seed==${SEED}`);
  await repump.assertDelivery();
  const records3 = await repump.pump({count: WINDOW, snap: SNAP});
  ctx.assert(records3.length === WINDOW, `重泵复核泵出 ${records3.length}/${WINDOW} 帧`);
  checkProbeStreamsEqual(records1, records3, '客户端1', '重泵复核');
  for (const frame of SNAP) {
    ctx.assert(records3[frame - 1].url === records1[frame - 1].url,
      `重泵复核帧 ${frame} dataURL 逐字节一致`);
  }

  // ---- 资源回收（摘要已在 node 侧留存）----
  await manual1.close();
  await manual2.close();
  await raf.close();
  await repump.close();

  ctx.evidence.push(`共 4 个真实浏览器客户端会话（2 手动泵 + 1 rAF + 1 重泵复核），` +
    `同 seed=${SEED} 同录像 th13_01.rpy stage0，probe 窗 1..${WINDOW}，dataURL 帧=${SNAP.join(',')}`);
}
