// m1.side_effect_boundary 浏览器层真实 case（M1/T8 测试作者；M1-plan §2/T8、§3 矩阵）。
//
// 断言目标（M1「副作用提交拍/Draw 不携带副作用」的生产网页面）：
//   1. th13_frame() 每逻辑帧恰 +1 且等于 probe_state[7]——两个手动泵客户端
//      （th13-debug.html 逐帧 th13_debug_frame，页内 rAF 不启动）全程逐帧核验，
//      且两泵 probe_state 流全等（提交拍与模拟轨迹不随泵会话漂移）。
//   2. SE 提交拍经仓根 tools/browser/audio-output-probe.mjs 的
//      installSfxStartProbe 两泵同窗一致：先 th13_debug_audio_resume 解挂输出
//      （BrowserRuntime.cpp:423-426，只解挂不推进帧），两客户端在同帧窗
//      （91..330；全回放实测 sfxStart 全部分布在帧 20..551）分别采样：
//        a. AudioBufferSourceNode.start 事件序列（WebSfx 即时输出 op2
//           play→node.start(0,offset)：when 恒 0、peak>1e-4、channels=2，
//           peak/channels 由确定性 SE 缓冲决定）——提交拍到达真实 Web Audio 输出；
//        b. 生产音频请求 tap（th13_debug_audio_observe→take，
//           BrowserRuntime.cpp:1419-1453，观察先于排队合并）逐字节一致
//           （帧/序/种类/id/值/pan 位）。
//   3. 音频设备流副作用边界（生产观测面 Module.th13Audio：AudioDevice::pump
//      每帧发布，AudioDevice.cpp:22-31 / BrowserRuntime.cpp:664）：
//        a. resume 后逐帧泵：suspended===false、device===true、track>=0、
//           submittedFrames/mixedFrames 增长（音乐流真实混入并提交设备）；
//        b. 负控 th13_loop_pause(1)（BrowserRuntime.cpp:428-434 suspend(true)→
//           SDL_PauseAudioStreamDevice）：60 泵内 submittedFrames 冻结（pump
//           在挂起期不回填，AudioDevice.cpp:670-673），而 th13_frame 仍每泵 +1
//           （游戏帧推进与音频设备状态解耦）；
//        c. 再 resume：设备恢复消费后 submittedFrames 越过冻结值且
//           nonzeroSamples>0（音乐混音非静音——resume 真实解挂）。
//      两泵同拍数的 requests/started/track 计数一致（游戏侧确定性）。
//   4. PCM 设备回调活性（installPcmProbe/samplePcm，泵与窗口重叠）：frames>0。
//      注：TH13 SDL3 的可听输出不经该 ScriptProcessor（实测 peak 恒 0 而生产
//      nonzeroSamples 增长），故非静音断言走 3c 的生产观测面，不以 PCM peak
//      为判据。
//   5. 每个客户端送达的 wasm 字节 == build.json 产物（仓根 verified-runtime.mjs）。
//
// 全部走生产 th13-debug.html 会话与生产帧体（defer→sample_and_tick→commit→
// present→audio pump，BrowserRuntime.cpp:650-667）；无测试专用游戏分支。
export async function run(ctx) {
  const SEED = 47932;      // 与 draw_purity 同一固定 boot 种子
  const PRELUDE = 90;      // 窗前预泵（对局开场；观察窗不含建立期）
  const SE_WINDOW = 240;   // SE 提交拍观察窗：游戏帧 91..330（sfxStart 富集段）
  const PAUSE_PUMPS = 60;  // 挂起期泵数（负控冻结窗）
  const RESUME_PUMPS = 1;  // 恢复后补泵（触发 pump 回填）
  const RESUME_SETTLE_MS = 400; // 恢复后等待设备真实消费（44100Hz×400ms≫3072 帧缓冲）
  const PCM_PUMP_MS = 900; // PCM 活性窗的泵覆盖时长（settle 250ms+窗口 300ms+余量）

  // ---- 客户端 A：手动泵 + 音频面观察 ----
  const clientA = await ctx.openClient({kind: 'manual', seed: SEED});
  ctx.assert(clientA.bootSeed === SEED, `泵客户端A th13_boot_seed==${SEED}`);
  await clientA.assertDelivery();
  ctx.evidence.push(`客户端A manual：replay 总帧=${clientA.total}`);
  await clientA.resumeAudio();  // 仅解挂输出面；不推进任何帧
  const streamA = await clientA.pump({count: PRELUDE});
  ctx.assert(streamA.length === PRELUDE, `客户端A 预泵 ${streamA.length}/${PRELUDE} 帧`);

  // SE 提交拍观察窗：sfxStart 切片起点 → 开生产音频请求 tap → 泵窗内帧。
  const sfxFromA = await clientA.sfxMark();
  await clientA.audioObserveEnable();
  streamA.push(...await clientA.pump({count: SE_WINDOW}));
  const sfxA = await clientA.sfxSlice(sfxFromA);
  const audioStreamA = await clientA.audioTake();

  // ---- 音频设备流副作用边界（固定拍数；跨客户端可比快照在后）----
  // (a) resume 态：设备真、未挂起、曲目装载、提交/混音计数在增长。
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
  // (b) 负控：loop_pause(1) 挂起设备——60 泵内提交计数冻结、帧计数照常 +1。
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
  // (c) 再 resume：设备恢复消费后越过冻结值；混音非静音。
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

  // ---- 客户端 B：全新会话，同 seed 同录像，同拍数协议 ----
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

  // ---- 游戏侧音频副作用两泵一致（同拍数；设备消费相关计数不比）----
  for (const field of ['requests', 'started', 'track', 'paused', 'failed', 'droppedRequests']) {
    ctx.assert(snapshotA[field] === snapshotB[field],
      `两泵音频游戏侧计数一致 ${field}（A=${snapshotA[field]} B=${snapshotB[field]}）`);
  }

  // ---- 全程帧账目（每逻辑帧恰 +1 且等于 probe_state[7]，两泵一致）----
  for (const [label, stream] of [['客户端A', streamA], ['客户端B', streamB]]) {
    for (let i = 0; i < stream.length; ++i) {
      ctx.assert(stream[i].frame === i + 1, `${label} 帧 ${i + 1} 计数恰 +1（观测 ${stream[i].frame}）`);
      ctx.assert(stream[i].probe[7] === stream[i].frame,
        `${label} 帧 ${i + 1} probe_state[7]==th13_frame`);
    }
  }
  // 两泵 probe_state 流全等（16 槽逐帧，固定拍数段；含挂起期帧——模拟轨迹与
  // 音频设备状态解耦的直接证据）。
  const compared = Math.min(streamA.length, streamB.length);
  ctx.assert(streamA.length === streamB.length,
    `两泵泵帧数一致（A=${streamA.length} B=${streamB.length}）`);
  for (let i = 0; i < compared; ++i) {
    const same = streamA[i].probe.every((value, slot) => value === streamB[i].probe[slot]);
    ctx.assert(same, `两泵 probe_state 全等@帧 ${streamA[i].frame}：` +
      `slot差异=[${streamA[i].probe.map((v, s) => v === streamB[i].probe[s] ? null : s).filter(v => v !== null)}]`);
  }

  // ---- SE 提交拍两泵同窗一致 ----
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
  // 生产音频请求 tap 流逐字节一致（帧/序/种类/id/值/pan 位）。
  ctx.assert(audioStreamA === audioStreamB,
    `生产音频请求流逐字节一致（A=${audioStreamA.length}B B=${audioStreamB.length}B）`);
  ctx.assert(audioStreamA.trim().length > 0, '生产音频请求流非空');

  // ---- PCM 设备回调活性（最后执行；泵数可变不入跨客户端比对）----
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
