export async function run(ctx) {
  const SEED = 47932;
  const PRELUDE = 90;
  const SE_WINDOW = 240;
  const PAUSE_PUMPS = 60;
  const RESUME_PUMPS = 1;
  const RESUME_SETTLE_MS = 400;
  const PCM_PUMP_MS = 900;

  const clientA = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(clientA.bootSeed === SEED, `泵客户端A th13_boot_seed==${SEED}`);
  await clientA.assertDelivery();
  ctx.evidence.push(`客户端A manual：replay 总帧=${clientA.total}`);
  await clientA.resumeAudio();
  const streamA = await clientA.pump({count: PRELUDE});
  ctx.assert(streamA.length === PRELUDE, `客户端A 预泵 ${streamA.length}/${PRELUDE} 帧`);

  const sfxFromA = await clientA.sfxMark();
  await clientA.audioObserveEnable();
  streamA.push(...await clientA.pump({count: SE_WINDOW}));
  const sfxA = await clientA.sfxSlice(sfxFromA);
  const audioStreamA = await clientA.audioTake();

  streamA.push(...await clientA.pump({count: 1}));
  let statsA = await clientA.audioStats();
  ctx.assert(statsA.suspended === false, `客户端A resume 后 suspended===false`);
  ctx.assert(statsA.device === true, `客户端A 设备在线（device===true）`);
  ctx.assert(Number.isFinite(statsA.track) && statsA.track >= 0,
    `客户端A 曲目已装载（track=${statsA.track}）`);
  const submittedA0 = statsA.submittedFrames;
  const mixedA0 = statsA.mixedFrames;
  ctx.assert(submittedA0 > 0, `客户端A 音乐流已提交设备（submittedFrames=${submittedA0}）`);
  ctx.assert(mixedA0 > 0, `客户端A 混音计数非零（mixedFrames=${mixedA0}）`);
  await clientA.loopPause(true);
  streamA.push(...await clientA.pump({count: PAUSE_PUMPS}));
  statsA = await clientA.audioStats();
  ctx.assert(statsA.suspended === true, `客户端A loop_pause(1) 后 suspended===true`);
  const submittedA1 = statsA.submittedFrames;
  streamA.push(...await clientA.pump({count: PAUSE_PUMPS}));
  statsA = await clientA.audioStats();
  ctx.assert(statsA.submittedFrames === submittedA1,
    `客户端A 挂起期提交计数冻结（${submittedA1}→${statsA.submittedFrames}，` +
    `${PAUSE_PUMPS} 泵无回填）`);
  ctx.assert(statsA.suspended === true, `客户端A 挂起期持续 suspended===true`);
  await clientA.resumeAudio();
  await new Promise(done => setTimeout(done, RESUME_SETTLE_MS));
  streamA.push(...await clientA.pump({count: RESUME_PUMPS}));
  statsA = await clientA.audioStats();
  ctx.assert(statsA.suspended === false, `客户端A 再 resume 后 suspended===false`);
  ctx.assert(statsA.submittedFrames > submittedA1,
    `客户端A 恢复后提交计数增长（${submittedA1}→${statsA.submittedFrames}）`);
  ctx.assert(statsA.nonzeroSamples > 0,
    `客户端A 音乐混音非静音（nonzeroSamples=${statsA.nonzeroSamples}）`);
  const snapshotA = await clientA.audioStats();

  const clientB = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(clientB.bootSeed === SEED, `泵客户端B th13_boot_seed==${SEED}`);
  await clientB.assertDelivery();
  await clientB.resumeAudio();
  const streamB = await clientB.pump({count: PRELUDE});
  const sfxFromB = await clientB.sfxMark();
  await clientB.audioObserveEnable();
  streamB.push(...await clientB.pump({count: SE_WINDOW}));
  const sfxB = await clientB.sfxSlice(sfxFromB);
  const audioStreamB = await clientB.audioTake();
  streamB.push(...await clientB.pump({count: 1}));
  let statsB = await clientB.audioStats();
  ctx.assert(statsB.suspended === false && statsB.device === true, `客户端B resume 后设备在线未挂起`);
  await clientB.loopPause(true);
  streamB.push(...await clientB.pump({count: PAUSE_PUMPS}));
  statsB = await clientB.audioStats();
  const submittedB1 = statsB.submittedFrames;
  streamB.push(...await clientB.pump({count: PAUSE_PUMPS}));
  statsB = await clientB.audioStats();
  ctx.assert(statsB.submittedFrames === submittedB1, `客户端B 挂起期提交计数冻结`);
  await clientB.resumeAudio();
  await new Promise(done => setTimeout(done, RESUME_SETTLE_MS));
  streamB.push(...await clientB.pump({count: RESUME_PUMPS}));
  statsB = await clientB.audioStats();
  ctx.assert(statsB.submittedFrames > submittedB1 && statsB.nonzeroSamples > 0,
    `客户端B 恢复后提交增长且混音非静音（submitted ${submittedB1}→${statsB.submittedFrames}，` +
    `nonzero=${statsB.nonzeroSamples}）`);
  const snapshotB = await clientB.audioStats();

  for (const field of ['requests', 'started', 'track', 'paused', 'failed', 'droppedRequests']) {
    ctx.assert(snapshotA[field] === snapshotB[field],
      `两泵音频游戏侧计数一致 ${field}（A=${snapshotA[field]} B=${snapshotB[field]}）`);
  }

  for (const [label, stream] of [['客户端A', streamA], ['客户端B', streamB]]) {
    for (let i = 0; i < stream.length; ++i) {
      ctx.assert(stream[i].frame === i + 1, `${label} 帧 ${i + 1} 计数恰 +1（观测 ${stream[i].frame}）`);
      ctx.assert(stream[i].probe[7] === stream[i].frame,
        `${label} 帧 ${i + 1} probe_state[7]==th13_frame`);
    }
  }
  const compared = Math.min(streamA.length, streamB.length);
  ctx.assert(streamA.length === streamB.length,
    `两泵泵帧数一致（A=${streamA.length} B=${streamB.length}）`);
  for (let i = 0; i < compared; ++i) {
    const same = streamA[i].probe.every((value, slot) => value === streamB[i].probe[slot]);
    ctx.assert(same, `两泵 probe_state 全等@帧 ${streamA[i].frame}：` +
      `slot差异=[${streamA[i].probe.map((v, s) => v === streamB[i].probe[s] ? null : s).filter(v => v !== null)}]`);
  }

  ctx.assert(sfxA.length === sfxB.length,
    `SE 提交拍事件数同窗一致（A=${sfxA.length} B=${sfxB.length}）`);
  for (let i = 0; i < Math.min(sfxA.length, sfxB.length); ++i) {
    ctx.assert(sfxA[i].when === 0, `A 事件 ${i} 起播 when==0（WebSfx 即时输出语义）`);
    ctx.assert(sfxA[i].peak === sfxB[i].peak && sfxA[i].channels === sfxB[i].channels,
      `SE 提交拍事件 ${i} peak/channels 两泵一致（peak ${sfxA[i].peak} vs ${sfxB[i].peak}）`);
  }
  ctx.assert(sfxA.length >= 1, `观察窗内有 SE 提交拍事件（A 窗 ${sfxA.length} 起）`);
  for (const event of sfxA)
    ctx.assert(event.peak > 1e-4, `SE 提交拍缓冲非静音（peak=${event.peak}）`);
  ctx.assert(audioStreamA === audioStreamB,
    `生产音频请求流逐字节一致（A=${audioStreamA.length}B B=${audioStreamB.length}B）`);
  ctx.assert(audioStreamA.trim().length > 0, '生产音频请求流非空');

  const pcmA = await ctx.samplePcm(clientA.page)
    .catch(error => ({frames: -1, peak: -1, error: String(error)}));
  const deadlineA = Date.now() + PCM_PUMP_MS;
  while (Date.now() < deadlineA) await clientA.pump({count: 60});
  ctx.assert(pcmA.frames > 0, `客户端A PCM 设备回调活性（frames=${pcmA.frames}）`);
  const pcmB = await ctx.samplePcm(clientB.page)
    .catch(error => ({frames: -1, peak: -1, error: String(error)}));
  const deadlineB = Date.now() + PCM_PUMP_MS;
  while (Date.now() < deadlineB) await clientB.pump({count: 60});
  ctx.assert(pcmB.frames > 0, `客户端B PCM 设备回调活性（frames=${pcmB.frames}）`);

  ctx.evidence.push(`SE 提交拍窗=帧 ${PRELUDE + 1}..${PRELUDE + SE_WINDOW}：sfxStart 事件 ` +
    `${sfxA.length} 起（when=0、peak>1e-4、peak/channels 逐事件两泵一致），` +
    `生产请求流 ${audioStreamA.length}B 逐字节一致`);
  ctx.evidence.push(`设备流边界：A submitted ${submittedA0}→(pause 冻结 ${submittedA1})→` +
    `${snapshotA.submittedFrames}，nonzero=${snapshotA.nonzeroSamples}，track=${snapshotA.track}；` +
    `B 同协议 submitted 冻结 ${submittedB1}→${snapshotB.submittedFrames}，` +
    `nonzero=${snapshotB.nonzeroSamples}；游戏侧计数两泵一致 ` +
    `requests=${snapshotA.requests} started=${snapshotA.started}`);
  ctx.evidence.push(`两泵固定拍数段 probe_state 流全等（${compared} 帧 × 16 槽，含挂起期帧），` +
    `帧计数全程连续且 probe[7]==th13_frame；PCM 设备回调活性 A=${pcmA.frames} B=${pcmB.frames}`);

  await clientA.close();
  await clientB.close();
  ctx.evidence.push(`共 2 个真实浏览器客户端会话，同 seed=${SEED} 同录像 th13_01.rpy stage0，` +
    `固定拍数段总泵帧=${compared}`);
}
