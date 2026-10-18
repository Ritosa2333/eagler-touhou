# TH10 r7 音频热点只读审计

## 结论与范围

当前没有证据支持根据旧样本的一次音频长尾，修改 TH10 的补音频预算、队列水位、确认提交顺序或曲目策略。紧邻的 r7 ON / OFF 重复组表现接近；旧组的极端音频尾部未在这组对照中复现，不能解释成 VBO 优化消除了固定的音频成本，也不能未经定位就归咎环境。

本审计记录源码能够确认的结构和重复工作，供后续实际热点分析使用。两个候选均未在本次审计中实现。审计阶段仅通过 nig 读取源码和证据；保存本报告时只新增本文，没有执行命令、构建、测试或浏览器操作，也没有改动任何音频源码。

- 工作树：`D:/workspace/eagler/worktrees/th10-rollback-smoothness`。
- 审计基准：HEAD `2e5d5464b2b28ef3fb936234660b773b604c91ce` 加 r7 未提交修改。
- r7 正式 WASM：`82232288a0c4f3045c885053c4a34da5faecba87b930e1fccc5392f1016af12f`。
- r7 sourceDigest：`802a500599ae44adf85b080a738dd94c8d0c37e4dec9b1b4cc3b7ba79d215c66`。
- 冻结构建依据：[runtime/frozen.json](../artifacts/rollback-smoothness/candidate-r7/runtime/frozen.json) 和 [runtime/build.json](../artifacts/rollback-smoothness/candidate-r7/runtime/build.json)。
- 本文时间均为 2026-10-12 UTC；源码行锚点对应审计时的 r7 工作树，后续编辑应以函数名复核。

已完整阅读共享 Audio Playbook，来源为 `D:/workspace/eagler/worktrees/multiplayer-rollback-architecture/docs/playbooks/audio.md`。原则是保留游戏对曲目、循环、fade、暂停和 SFX 时序的所有权，底层输出优化不得替换这些语义。

## 1. 紧邻对照已经改变了旧长尾的解释边界

根代理串行执行的两组使用同一份 r7 正式 WASM、两端 CDP 4× CPU 限速、480 帧、50 ± 5 ms 单程延迟注入、桌面合成 120 Hz 触控移动、硬件渲染和原生高刷展示。两组均启用相同的回调分段计时，分别设置 Model quad residency ON / OFF。

| 指标 | ON，14:23:31–14:24:00 | OFF 重复组，14:25:20–14:25:59 |
|---|---:|---:|
| P0 / P1 逻辑推进 Hz | 50.34 / 51.10 | 52.30 / 52.48 |
| P0 / P1 原生回调工作量 / 新推进帧，ms | 16.74 / 13.67 | 16.07 / 13.42 |
| P0 / P1 测量窗 AudioPump 总时间，ms | 142.1 / 72.5 | 112.3 / 67.3 |
| 两端完整 44 字 canonical、确认音频、RTC 路由 | PASS | PASS |

数值来自各端 `summary.logicHz`、`summary.callbackWorkPerNewFrameMs` 和 `summary.callbackPhases.phases.audioPumpMs.totalMs`。报告保留首次 high-water 越界间的全部回调；各次实际输入时机、游戏过程和窗口边界可能略有差异。这是一组紧邻的真实运行对照，不是相同世界状态逐指令微基准。

证据：

- [ON 性能报告](../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-on-phases.json)。
- [ON 执行与退出回执](../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-on-phases.json.runner.json)。
- [OFF 重复组性能报告](../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases-repeat1.json)。
- [OFF 重复组执行与退出回执](../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases-repeat1.json.runner.json)。

因此，当前没有 Model VBO 改动带来整作性能收益的证据。真实上传减少与像素正确性是另一层证据，不能替代整作表现。约 50–52 Hz 的逻辑推进也不能宣称已实现目标 60 Hz 或整体验收。

[较早 OFF 样本](../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases.json)曾出现极端音频尾部，包括约 703 ms 的单次 AudioPump。它是保留的异常证据，尚未在紧邻 ON / OFF 重复组中复现；不能拿它作为修改音频策略的唯一依据。

上述 RTC 为本地独立浏览器和隔离 relay 下的真实连接验证，不是公网/WAN 或手机实机证据。`passed` 的范围是运行、路由、状态和确认音频门槛；最终像素运动连续性、输入到光子延迟和可听输出另有验证边界。

## 2. 当前 pump 有工作量上限，没有墙钟欠账无限补混音

主文件：[AudioHost.cpp](../th10_web/cpp/sdl/AudioHost.cpp)。

`Host::pump()`，L143–161，使用固定水位：

| 参数 | 帧数 | 44100 Hz 下的时长 |
|---|---:|---:|
| chunk | 1024 | 约 23.2 ms |
| low | 4096 | 约 92.9 ms |
| high | 6144 | 约 139.3 ms |

循环最多处理 6 块，且本地 queued 达到 high 就退出。它没有把 `host.millis` 的迟到时间展开为无限个补混音任务。因此不能把 rAF 迟到直接等同于“补齐全部过去墙钟音频”。

`Host::render()`，L128–141，在每个至多 1024 帧的混音切片后检查音乐通知，然后运行原有 `Audio::pump()` 补流任务。带通知的音乐缓冲通过 `Host::position()`（L88–100）的 `ma_sound_get_cursor_in_pcm_frames()` 取得真实源消费位置；普通 SFX 保留原控制时钟。

这一区分是已有正确性修复。历史文档 [AUDIO-FIX-3.5.1.md](../th10_web/docs/AUDIO-FIX-3.5.1.md)记录了旧版墙钟领先导致覆盖未消费音乐、多个布尔通知折叠导致漏补的问题。修复后的源消费位置和逐切片通知服务应当保留。

### 仍然存在的结构风险

6 块是生产帧数上限，不是 CPU 时间上限。rAF 尾部在 [ApplicationHost.cpp](../th10_web/cpp/sdl/ApplicationHost.cpp) L388 同步调用 `sdl_audio_pump()`，此时已经通过逻辑准入和展示时间判断。昂贵的单个混音块仍可能拉长回调；但是否发生、耗在哪里，应由可重复的分段或采样证据决定。

不能直接改成“每个 rAF 只补一块”。单块只够约 23.2 ms；补充机会持续低于约 43 次/秒时，单块配额不足以维持 44100 Hz 的连续输出，可能把画面问题变成音频中断。也不能删掉块间通知服务或任意清空队列，这会触碰已经修复的环形缓冲和输出连续性。

## 3. 当前 smoke 关闭了 BGM PCM 源，OGG 性能不在本组覆盖范围

明确入口是 [smoke.mjs](../portable/multiplayer/smoke.mjs) L30：

```js
string('#screen',core.sdl_canvas);
core.sdl_music_enabled(0);
```

对应 `AudioHost.cpp` L277 起的 `musicRead()`：`music_enabled == false` 时只按请求长度填零、推进源游标并返回，不进入 `MusicFile::select()`、OGG 定位或解码。

因此，旧组 AudioPump 长尾不能归因于 Vorbis 解码、整曲 full decode 或流式 OGG seek。BGM 所有权和缓冲结构仍然存在，SFX 仍开；“关闭 BGM 源”也不等于没有混音负载。

本组音频 PASS 能覆盖确认命令的一致性及运行状态，不能推广为以下结论：

- OGG stream 和 full 模式的实际性能通过；
- 混音 PCM 与旧版逐样本一致；
- 浏览器设备输出没有可听空洞；
- 手机上音频、触控或后台恢复已经通过。

这些边界必须和“完整 canonical / 确认音频摘要 PASS”一并保留。

## 4. SDL 还有不计入 AudioPump 的独立主线程回调

本次读到的 SDK 源码根目录为：

`D:/workspace/eagler/toolchains/emsdk/upstream/emscripten/cache/ports/sdl3/SDL-release-3.4.2/src/audio/`

| 源码及锚点 | 可确认的行为 |
|---|---|
| `emscripten/SDL_emscriptenaudio.c`，`EMSCRIPTENAUDIO_OpenDevice()`，L191、L263 起 | 设备采样率来自 `AudioContext.sampleRate`；以 ScriptProcessorNode 的 `onaudioprocess` 触发输出 |
| `SDL_audio.c`，`SDL_PlaybackAudioThreadIterate()`，L1172 起 | 从 SDL stream 消费数据，完成设备侧转换/混合，再调用 PlayDevice |
| `emscripten/SDL_emscriptenaudio.c`，`EMSCRIPTENAUDIO_PlayDevice()`，L40 起 | JS 按声道逐样本复制到浏览器输出缓冲 |
| `SDL_audiocvt.c`，`SDL_PutAudioStreamData()`，L858 起 | 当前无额外 put 回调的 TH10 路径把原始输入写入队列，不在这里完成设备输出重采样 |

当 44100 Hz 引擎输出与设备采样率不一致时，SDL 的设备侧重采样发生在独立音频回调中。rAF 尾部的 `audioPumpMs` 包含 miniaudio 混音、原始补流、限幅/RMS 和 SDL 入队，不能代表所有主线程音频 CPU 开销。

因此，`outsideNativeCallbackMs` 也不能直接称为“rAF 空闲等待”。这段经过时间可能包含 SDL 音频输出回调、其他浏览器工作和调度等待；应先分离实际工作，再作归因。这里识别的是具体可检查路径，没有把旧长尾直接归因于 SDL。

### 设备块大小需要运行时核实

TH10 `AudioHost.cpp` L69 设置 sample-frames hint 为 2048。当前 SDK 的 `SDL_audio.c` L151 直接返回该 hint，而 `SDL_emscriptenaudio.c` L193 再乘 2 后创建 ScriptProcessorNode。

按这份源码推导，请求的设备块应为 4096 帧：44100 Hz 时约 92.9 ms，48000 Hz 时约 85.3 ms。它会把设备侧转换集中成较大的任务。

这仍是 SDK 源码推导，未在本审计中读到运行时 `bufferSize` 和实际 `sampleRate` 记录，不能冒充实际设备测量。缩小块大小可能降低单次回调成本，也可能提高回调频率并降低抗抖动能力；没有输出连续性证据前，不应直接修改。

## 5. 回滚不会把预测声音反复送进真实混音器

相关源码：

- [World.cpp](../th10_web/cpp/platform/World.cpp)，`begin_netplay_frame()`、`end_netplay_frame()`、`commit_audio()`，L67–107。
- [Application.cpp](../th10_web/cpp/platform/Application.cpp)，回滚音频丢弃 L166，tick 前/后提交 L223、L341。
- [AudioEvents.cpp](../th10_web/cpp/multiplayer/AudioEvents.cpp)，`CommitThrough()`，L110–145。
- [AudioEvents.hpp](../th10_web/cpp/multiplayer/AudioEvents.hpp)，16 个帧槽和逐帧字节容量，L25–30。

现有流程在每个预测/重演帧打开事件记录，将 `AudioManager::command_sink` 指向该帧。SFX、stop 和音乐命令先被捕获；回滚通过 `DiscardFrom(replayFrom)` 删除未提交记录；只有仍然存活的确认帧按原顺序交给输出 AudioManager。

`World::commit_audio()` 在 correction 或待处理 rollback request 存在时直接暂缓。提交游标 `next_commit_` 单调增加，不允许丢弃已发出的确认历史，也不会因重复调用提交函数而再次发声。每个确认逻辑帧继续执行原有 `advance_fades()` 和 `audio.update()`。

没有发现“每次重演都立即向真实混音器重复发声”的错误路径。

### 确认提交和设备 pump 的计时不能混淆

确认范围突然前移时，`CommitThrough()` 可在一次应用逻辑调用内释放多个确认帧的命令。这部分成本进入 `nativeInputAndStepMs`，不进入尾部 `audioPumpMs`。

若以后确认推进时出现可重复的单步长尾，适合单独测量：本次提交帧数、命令数及输出 AudioManager 更新时间。但不能把确认帧合成一次 update、只保留最后一个 BGM 命令或删除中间 stop/play；这些都会改变原生音乐状态机、fade 步数和命令顺序。

## 6. 两个源码可证明的优化候选

### 候选 A：按属性更新 Host 音频控制，削掉重复计算

`AudioHost.cpp` 的 `Host::controls()`，L101–106，每次一起计算/设置 volume、pan 模式、pan 和 pitch。Pan 与 Volume 操作分支（L177–179）都调用完整的 `controls()`。

而 [AudioControl.cpp](../th10_web/cpp/game/AudioControl.cpp) 的 `update_effects()`，L61–70，发出音效时先设置 pan，再设置 volume。由此，一条实际音效命令会重复执行无关的 `pow()` 和 setter。

可以审查的实现方向是：sound 首次建立时保留完整初始化；Pan 只更新 pan，Volume 只更新 volume，Frequency 只更新 pitch；Duplicate、Format 和首次 Play 继续完整建立输出状态。

这不要求改变调度、水位、确认事件或曲目所有权，是较小的候选。但本审计没有测出它占整作成本多少，不能承诺明显提升逻辑帧率或消除卡顿。应在当前串行对照结束、热点证据支持后再做，避免干扰已有实验。

### 候选 B：在重采样器内部优化同速路径，完整保留状态

相关源码 [miniaudio.h](../portable/sdl/third_party/miniaudio.h)：

| 函数 | 行锚点 | 相关事实 |
|---|---:|---|
| `ma_engine_node_is_pitching_enabled()` | L76618 起 | 不以当前 pitch == 1 自动禁用重采样 |
| `ma_engine_node_process_pcm_frames__general()` | L76671 起 | pitching 开启时仍执行重采样 |
| `ma_linear_resampler_process_pcm_frames_f32_upsample()` | L53590 起 | 按帧维护 x0/x1、整数/小数相位和输入消费 |
| `ma_linear_resampler_interpolate_frame_f32()` | L53332 起 | 使用原来的浮点插值顺序 |
| `ma_mix_f32_fast()` | L13978 起 | 按减、乘、加的既定顺序计算结果 |

TH10 创建 sound 时只启用 `MA_SOUND_FLAG_NO_SPATIALIZATION`，所以同速音源仍可能承担通用线性重采样路径成本。

不能直接追加 `MA_SOUND_FLAG_NO_PITCH`，也不能看到 pitch == 1 就 memcpy。库明确警告动态跳过会在之后改变 pitch 时造成 glitch；当前路径还保留样本缓存、相位及输出延迟，完全绕过会改变 PCM。

若后续 profile 和实际音源采样率分布证明它值得优化，应在重采样器内部设置严格的同速适用条件，保留样本延迟、输入消费、x0/x1、相位、块边界与后续非同速转换状态。条件不满足时继续使用原实现，不能改浮点运算顺序或通过 fast-math 放宽一致性。

它比候选 A 有更大的潜在收益，也有更大的验证和维护成本。当前没有足够的 actual active-voice / sample-rate 分布和热点占比，暂不实施。

## 7. 必要的验证边界与后续归因方式

现有 [multiplayer-audio-native-equivalence-test.cpp](../tests/multiplayer-audio-native-equivalence-test.cpp)验证原始 AudioManager 字节、确认命令顺序、摘要、密集事件、丢弃/重提交与不重复发声。它不是 PCM 波形测试。

现有 [check-th10-music-continuity.mjs](../portable/check-th10-music-continuity.mjs)把同一混音器中的环形音乐流与完整不可变 PCM 源比较，适合发现环形缓冲同步错误。若修改的算法同时作用于两侧，两侧可能一起变化而仍通过；它不能单独证明修改前后的 mixer 输出相同。

若真正修改 controls 或 resampler，需要冻结旧/新 WASM，并对同一 PCM 输入及命令序列比较输出。应按实际改动范围覆盖 pan/volume/frequency 交错、首次和重复 Play、Duplicate、Format、Stop/Position/Play、循环边界、fade、BGM stream/full、SFX，以及 [replay-audio-seek.mjs](../portable/multiplayer/replay-audio-seek.mjs) 所代表的原有 seek 语义。确认音频摘要与完整 canonical 门槛继续保留。

如果新的相邻样本再现音频长尾，优先以按需启用、固定容量的观测拆分 `Host::pump()`：

1. 本次块数、起止 queued、实际活跃音源数量；
2. miniaudio 混音、通知及原补流、限幅/RMS、SDL 入队的分段时间；
3. 独立 SDL 音频回调的次数、单次耗时、实际 sampleRate / bufferSize；
4. 发生长尾时是否恰逢确认音频多帧提交。

这些是针对可重复风险的后续观测建议，不是要求先加入常驻重型日志或逐回调世界扫描。采样与计时开关本身也要在最终候选中关闭复核。

当前决策是继续主线 profile 和实际热点优化，保留本审计作为架构与归因边界。不因为不可重复的旧单次长尾，先改变补音频策略、音频所有权或正确的确认/回滚隔离。
