# TH11MP 回滚音频与密集 3P 场景只读审计

日期：2026-10-13。工作树：`D:\workspace\eagler\worktrees\th11-rollback-smoothness`。
初稿为只读源码审计和验收建议；后续 NO_PITCH 组件实测及正式 build09 接入证据追加于 §7。
审计代理没有执行命令、构建、测试或浏览器，也没有改动生产音频源、共同库或 TH10；
§7 的运行与生产 attachment 修改由根执行完成，不把静态审计回填成运行证据。

## 1. 结论及覆盖边界

当前没有查到能够由“3P 同时产生很多音效”直接触发的确定数组越界、每次播放新建声部泄漏，
或因 56 个平行声部导致线性增加的递归深度。现有队列和声部有明确上界，见 §2。

仍需真实验证用户指定的 **3P、Challenge、Lunatic 整场、BGM 与音效开启**。
已有协议/世界 oracle 不能证明真实混音器、SDL/浏览器音频设备和长时间零售资源路径稳定。
短测 ready 或混音计数增长也不能证明实际 SFX 已发声。不得以关闭 SFX、跳过密集阶段或短测替代整场。

已识别的是两类具体观察边界，而非已证明的崩溃根因：

- 整批确认输出的事件遍历及音频控制命令不受“恢复历史帧 8ms”预算保护；
  回滚记录持有整份 BattleEvent vector，相关动态内存不属于 32 MiB 原始 journal 配额。
- 现有 9-word 音频状态只能识别部分错误，不覆盖 PCM 非有限值或单个声部控制失败。
  本轮先使用现有低频状态和真实崩溃位置，避免尚无重现时给 mix 热路径增加测量成本。

## 2. 已核对的所有权和容量

路径均相对本工作树。

| 部分 | 源码依据 | 可确认的事实 |
| --- | --- | --- |
| SFX 资源与声部 | `th11_web/cpp/sdl/AudioDevice.cpp:22–45` | 46 份样本向量、56 个常驻 Voice；播放仅 stop/seek/pan/volume/start，不按玩家或每条事件分配 Voice。别名声部引用同一只读样本。 |
| 销毁顺序 | 同文件 `Impl::~Impl`、`Voice::~Voice` | 先停止/释放 sound，再 decoder；所有声部在样本向量销毁前释放；SDL stream 在 engine 前销毁。没有从本顺序发现悬空样本指针。 |
| 游戏音效队列 | `th11_web/cpp/game/SoundEffects.hpp` 与 `.cpp:31–61` | 每帧最多 12 个不同 ID，每个 ID 最多保留 128 个 pan；ID 限制 0–55，重复 ID 聚合，超额按已有游戏规则不再追加。stop 用负 count 标记，不继续向负下标写入。 |
| 队列消费 | `SoundEffects.cpp:53–61` | 一次逻辑音频 update 最多 12 组声部操作。禁音或未初始化时不消费，但队列仍受上述边界限制。 |
| 音频并发 | `AudioDevice.cpp:1–8,35–39,91–105` | 此处 MA_NO_THREADING，SDL stream 无应用回调，title 主线程 pump 主动混音。源码中未发现 SDL 音频回调同时修改该 miniaudio graph 的路径。 |
| 混音上界 | `AudioDevice.cpp:96–100` | 每个 pump 最多 6×1024 PCM frames，低水位 4096、高水位 6144；这是采样帧预算，不是毫秒预算。exact tick 和 rAF 尾部都可 pump，不能声称“每 rAF 永远只一次”。 |
| miniaudio 输入拓扑 | `portable/sdl/third_party/miniaudio.h:74234–74292` | endpoint 的声部兄弟节点顺序遍历，每份 premix 缓冲用完即释放；不是 56 声部层层嵌套。 |
| premix 内存 | 同文件 `73666–73671,74256–74275` | 默认每输出声道 512 KiB 的 premix stack 由 ma_stack_init 申请，是独立混音图内存，不能与 WASM C stack 混为一谈。 |

这里只证明静态结构具有这些边界；没有测得最坏 PCM 解码耗时，也没有证明所有资源文件必定有效。

## 3. 回滚确认链保持的音频语义

`th11_web/cpp/sdl/MultiplayerRollback.inc`：

1. `rollback_run_frame:167` 将当帧 battle.events 移入记录；历史 Update/Draw 不直接提交这些声音。
2. `rollback_commit:94–128` 先拒绝 Correcting 或待处理 rollback request，再退休 raw journal/typed leases，
   获得 SetCommitLimit，按帧顺序提交 Sound、StopSound、StageMusic、BossMusic、Resume、Fade。
3. 每个已确认逻辑帧都调用一次 audio.update，包括无声音的帧；fade 和 effects.process 不按重演次数加速。
4. 同帧成功 ConfirmedInputs 与 Replay.Append 后才前推已发布计数；被撤销的 future 记录随 checkpoint 尾部释放。
5. 菜单/暂停/资源转场由世界前置确认屏障走 exact 路径，不能为了少一次音效命令绕过屏障。

不可跨确认帧把 start/stop/fade 任意合并，也不可为了减少内存丢弃 BattleEvent。
当前记录字段虽名为 sounds，实际接收完整事件向量，包含 Popup、Notification、Lives 等。
其内容由 `GameBattle.cpp:517–526` 的正常事件 API 产生。

## 4. 3P 密集音效需定位的具体压力

### 4.1 事件向量先增长，后聚合

`MultiplayerBattle.cpp:213` 对每个新擦弹席位发出 sound(28)；
`GameBattle::sound:517` 每次直接 push_back。物品拾取、发弹、各席 Bomb 也产生事件。
SoundEffects 的 12×128 聚合发生在确认发布之后，因此不能把队列上界误当成 checkpoint 事件容量上界。

恢复窗口约束保留的帧数，不能限制每帧 vector 的容量。生产 journal 的 32 MiB 并不包含这些 vector
或 typed snapshot 的所有分配。真实 3P 整场若出现线性 memory.byteLength 增长或固定阶段崩溃，
应保存帧号/关卡/恢复深度/音频状态/栈，再针对该区间检查事件容量及 owner 退休。
没有实际证据前不能宣称已 OOM，也不要提前截断事件。

### 4.2 恢复预算之外的确认批次

12 帧历史一次确认可带来最多约 144 组 SFX 控制链，再加事件遍历和 BGM 切换/淡出推进。
`rollback_commit` 没有按墙钟中途让出；8ms 判断约束的是完整历史模拟事务之间。
这不等于当前存在大停顿，但真实卡顿测量需要区分 Restore/重演与 commit/audio，不应全部归因于快照。
先记录真实慢帧；没有重现时不改命令顺序、不改变音效数量或 pump 预算。

### 4.3 现有错误与统计的可见范围

`AudioDevice.cpp:91–106`：

- mix/帧数失败使 pump 写 stats[3]=1；SDL_PutAudioStreamData 失败写 2。
  pump 是 void，此错误不直接写 Application.error；验收必须实际检查错误槽。
- PCM 使用 std::clamp 限幅；NaN 不因此变成有限值。存在非有限 PCM 时可能仍统计为成功混音。
  目前未观察到这种 PCM，本项属于检测缺口。
- sound_stop/seek/pan/volume/start 没有汇总其返回状态。stats[1]/[2] 增长证明成功提交了块，
  不等于某个音效确实播放、PCM 合法或设备实际输出正常。
- 缺失 BGM 的暂时静音和稍后 resource_changed 重试是既有非致命合同，不应误标为游戏停显。
- music_enabled=false 只改变音量；若 BGM 文件仍已载入，不应从 TH10 的某条 smoke 经验推断 TH11
  一定跳过了 Vorbis 解码。实际 Host 资源策略和文件存在情况需随收据记录。

## 5. TH10 经验的正确使用范围

已只读重阅 `worktrees/multiplayer-rollback-architecture/docs/playbooks/audio.md`，
以及封存树 `worktrees/th10-rollback-smoothness/docs/th10-audio-hotpath-audit-r7.zh-CN.md`。

TH10 的音频长尾曾达到约 703ms，但该文明确未稳定重现、未证明由 BGM 解码导致；
它没有提供一个已经定位且能照搬的“多音效崩溃修复”。不得把推测写成 TH11 已修根因。
值得沿用的边界是：确认命令、设备 PCM、独立浏览器音频回调分开测量；
固定 6×1024 样本不是墙钟保证；BGM none 与关闭 SFX 含义不同；
foreground 恢复要等 AudioContext running 再重接 ScriptProcessor。

TH11 的 `eagler-host.mjs` 已有 disconnect-before-suspend/reconnect-after-running 路径。
本审计没有修改该路径，也不宣称所有移动/后台生命周期已通过。

## 6. 最小验证顺序

按 root 的当前决定，先运行真实浏览器入口寻找实际错误，不为无重现假设修改音频生产代码。

1. 正式 Release 精确构建身份下，保持现有短测音效开启。每 100ms 读真实 audio9 与
   memory.byteLength，同时保留页面异常、WASM trap、原始错误、最后逻辑帧/关卡和三端网络前沿。
   短测不能替代 BGM 或整场。
2. 完成用户指定 3P、Challenge、Lunatic、BGM+SFX 整场。记录选机、路由、延迟、触控生成方式，
   保留第一次故障位置；主流程正常推进，不能以跳关、跳过音效或缩短整场换 PASS。
3. 只有出现音频/内存或特定场景疑点时，再隔离其最小真实混音重现。现有
   `portable/multiplayer/audio-resource-test.mjs:24–31` 可参考真实 PCM finite/energy 验证，
   但它是旧 diagnostic build 的晚到音乐、pause/fade/cancel、单 ID SFX 测试，**尚无本轮重跑结果**。
4. 如需新增诊断 probe，限定独立构建：同帧 12 种 ID、同 ID 超 128 的既有丢弃语义、
   连续多帧启动所有 56 常驻声部、Stop/start 交错、确认批次与 BGM 更换。
   每块检查 PCM 有限值并保存必要能量摘要；不把 probe 加进正式 mix 热路径，
   不以合成 probe 替代整场。此项只是后备方案，尚未授权或执行新构建。

初稿阶段没有音频 stress PASS；其后的独立组件结果见 §7。真实整场仍需另附对应构建/命令/收据，
本审计的静态结论应保留原边界，不能回填成运行证据。

## 7. NO_PITCH 组件实测与正式构建接入（2026-10-13 追加）

### 7.1 证据身份与实际结果

根执行串行命令 `Qms_09BsKJSw1mqYDGC33vQf` 已完成，exit 0：

```text
node portable/multiplayer/audio-pitch-bypass-test.mjs --output artifacts/rollback-smoothness/audio-pitch-bypass-01
```

- 收据：`artifacts/rollback-smoothness/audio-pitch-bypass-01/receipt.json`。
- 原始结果：同目录 `execute.stdout.log`，以及每个 case 的 `original.f32`、`no-pitch.f32`、`differences.jsonl`。
- 实际通过 **62 cases / 1,881,506 checks**；`passed=true`、`sourceStable=true`。
- 实际依赖摘要：`1a654e0cf36c7302ae4001d93ee75725cd9382a78c565a7721cd27b0826574ba`。
- 真实 DAT SHA-256：`3cb521c5d420d8cbad6494d53bcdb048bcc396abf95c18fac68e38fd2d19f8e2`。
- 真实 BGM 为 `th11_01.ogg`，SHA-256：`23b0e0510ecd18f00f67d4e3ddeca84183f21f491c980ee618e834e3c4e8ed14`。
  其 fmt 实际为 44100 Hz、双声道，loop intro 103264、end 4370016 PCM frames。

该测试是 WASI32、O2 的独立真实解码/混音组件。使用生产 Archive/Lzss/ResourceCrypt 读取原始资源，
以及生产 SoundEffects 的 46 个 WAV 名称与 56 个声音定义，复用 pinned miniaudio/stb_vorbis。
它没有实例化生产 AudioDevice/SDL 设备，也没有执行 rollback 确认发布或正式游戏流程。

### 7.2 为什么可绕过 pitch，哪些转换保留

生产代码没有 sound pitch、sound group pitch 或 Doppler 控制调用，原先已关闭 spatialization。
`portable/sdl/third_party/miniaudio.h:76618–76623,76729–76733` 明确表明，默认路径即使 pitch=1
也始终经过 resampler；设置 NO_PITCH 后，`76771–76784` 才会跳过该阶段。

所有 SFX decoder 输出 44100 Hz，engine 同为 44100 Hz。不同采样率声音的转换没有取消：
库在 `77251–77256` 检测到 source rate 与 engine rate 不同时，会强制重新启用 resampler。
fixture 还检查实际节点标志，并以独立、明确标为合成输入的 48000 Hz WAV 验证该路径。

这不是移除抗混叠滤波：同 rate 分支在 `53641–53643` 原本就不应用滤波。
音量、声道转换和 pan 继续走原分支。

### 7.3 PCM 差异必须明确保留

旧线性路径保留一个 PCM frame 的历史，因此两路 PCM **不逐位相同**。
初始化与插值依据为 `miniaudio.h:53220–53249,53332–53346,53612–53639`。
本次 0 容差逐样本检查证明：同 rate 的差异符合移除一个 PCM frame 历史的精确模型，
即 44100 Hz 下约 22.7 微秒；seek、stop、restart 和不同 mix block 大小也纳入模型。

- 56 个真实声音 ID 均通过精确历史模型，包含左右和居中声像。
- `sfx-seek-stop-restart`：3650 frames，modelDifferences=0。
- `sfx-natural-end`：23712 frames，modelDifferences=0，明确记录旧路径未排空的最后一帧，
  即双声道共 2 个 tail samples。没有把它伪称逐位等价。
- `dense-56-looping`：56 声部、32768 frames，modelDifferences=0，原/候选能量均大于零。
  此项为了保持固定满载使用循环声部，不能代表生产每个音效本来就循环。
- `bgm-real-start` 的前 8192 frames 两路数值均为静音，能量均为 0；
  该起始窗口不作为“实际 BGM 已发声”证据。原始负零/正零位模式差异也保留在统计。
- `bgm-loop-seam`：8192 frames，modelDifferences=0，原/候选能量分别为
  283.159957261405 / 283.16190721352791，非零能量硬门通过，证明真实曲目接缝窗口有有效 PCM。
- `synthetic-48000-rate-guard`：7168 frames，数值差与位差均为 0，
  两路能量均为 457.92663437405821。不同 rate 的转换仍在执行。

dense、真实 BGM loop seam、48000 guard 三项都有“两路能量均大于零”硬门；
没有以“两路静音相同”冒充密集声音通过。

### 7.4 已接入生产的最小变化与性能范围

根执行仅在 `th11_web/cpp/sdl/AudioDevice.cpp:28–30` 的 Impl::attach 加入
`MA_SOUND_FLAG_NO_PITCH`，并注明一帧历史与不同 rate 仍保留转换的合同。
本次审计代理只读核实生产改动，没有修改生产文件。

正式 Web build09 的 WASM SHA-256（根执行提供）：

```text
8febab07604e03d362e13cfd05f28f550bb1917caf4877c82a08905dbc2afc02
```

单次组件诊断中，dense 32768 frames 的累计 engine mix 时间为：
原路径 **36.3762 ms**，候选 **23.026 ms**。
这是本地 WASI 组件的有限计时，不能换算成正式浏览器 4× CPU 的掉帧改善，
也不能据此承诺整场音频延迟或设备回调耗时。

**本次通过范围止于上述组件。** 正式 SDL/浏览器设备输出、确认后音效实际发布、
3P Challenge Lunatic 整场 BGM/SFX，以及普通 3P Bomb 压力仍需各自运行证据。
Challenge 明确屏蔽 Bomb，不能通过给该模式发送 KeyX 声称 Bomb 被覆盖；
Bomb 实际启动必须在普通三人场单独验证。
