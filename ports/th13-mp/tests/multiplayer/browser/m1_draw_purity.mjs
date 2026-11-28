export async function run(ctx) {
  const SEED = 47932;
  const WINDOW = 256;
  const SNAP = [30, 120, 240];
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

  const manual1 = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(manual1.bootSeed === SEED, `手动泵客户端1 th13_boot_seed==${SEED}`);
  await manual1.assertDelivery();
  ctx.evidence.push(`客户端1 manual：replay 总帧=${manual1.total} bootSeed=${manual1.bootSeed}`);
  const records1 = await manual1.pump({count: WINDOW, snap: SNAP});
  ctx.assert(records1.length === WINDOW, `客户端1 泵出 ${records1.length}/${WINDOW} 帧`);
  checkSequential(records1, '客户端1');
  checkProbeFrameIdentity(records1, '客户端1');

  const manual2 = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(manual2.bootSeed === SEED, `手动泵客户端2 th13_boot_seed==${SEED}`);
  await manual2.assertDelivery();
  const records2 = await manual2.pump({count: WINDOW, snap: SNAP});
  ctx.assert(records2.length === WINDOW, `客户端2 泵出 ${records2.length}/${WINDOW} 帧`);
  checkSequential(records2, '客户端2');
  checkProbeFrameIdentity(records2, '客户端2');
  checkProbeStreamsEqual(records1, records2, '客户端1', '客户端2');

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

  const raf = await ctx.openClient({kind: 'raf', seed: SEED});
  ctx.assert(raf.bootSeed === SEED, `rAF 客户端 th13_boot_seed==${SEED}`);
  await raf.assertDelivery();
  await raf.startRaf(RAF_TARGETS);
  const rafState = await raf.waitRafTargets(RAF_TARGETS);
  ctx.assert(!rafState.failure, `rAF 客户端帧体无失败（${rafState.records.length} 回调观测）`);
  ctx.assert(rafState.paused, 'rAF 客户端抵达全部目标后经 th13_loop_pause 冻结');
  ctx.assert(rafState.done.length === RAF_TARGETS.length,
    `rAF 客户端目标帧批次到达数=${rafState.done.length}/${RAF_TARGETS.length}`);
  let previous = 0;
  for (const record of rafState.records) {
    ctx.assert(record.frame === previous + record.ticks,
      `rAF 帧账目：frame ${previous}+ticks${record.ticks}==${record.frame}`);
    ctx.assert(record.probe[7] === record.frame,
      `rAF 帧账目 probe_state[7]==th13_frame@帧 ${record.frame}`);
    previous = record.frame;
  }
  const ticksHistogram = {};
  for (const record of rafState.records)
    ticksHistogram[record.ticks] = (ticksHistogram[record.ticks] ?? 0) + 1;
  ctx.evidence.push(`rAF 客户端：${rafState.records.length} 次回调观测，ticks 分布=` +
    `${JSON.stringify(ticksHistogram)}，目标到达=${JSON.stringify(rafState.done)}`);
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

  await manual1.close();
  await manual2.close();
  await raf.close();
  await repump.close();

  ctx.evidence.push(`共 4 个真实浏览器客户端会话（2 手动泵 + 1 rAF + 1 重泵复核），` +
    `同 seed=${SEED} 同录像 th13_01.rpy stage0，probe 窗 1..${WINDOW}，dataURL 帧=${SNAP.join(',')}`);
}
