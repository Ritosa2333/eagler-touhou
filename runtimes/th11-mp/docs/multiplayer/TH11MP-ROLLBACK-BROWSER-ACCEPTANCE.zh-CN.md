# TH11MP 真实浏览器回滚验收：有界双端入口

日期：2026-10-13。工作树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。

本说明对应新增 `portable/multiplayer/rollback-browser-acceptance.mjs`。编写时**尚未执行**；代码存在不等于通过。根代理串行执行、每次保留独立目录及 `receipt.json`。它补浏览器 Runtime/GPU/网络整合证据，不替代标准 Launcher UI 验收、完整世界组件 oracle 或整局耐久验收。

## 1. 本轮要证明什么

- 两个独立桌面 Chrome 进程，各自实际 CPU 限速 1× 或 4×，运行同一个来源可核对的正式 TH11MP 包。
- 真实共享 relay admission 接受 Direct 模式、D0/P12，真实 RTC 或 relay 二进制传输；不把 P0 房间偷偷改为预测合同。
- 正常原生循环接收墙钟 120 Hz 的 managed Host 触控移动和射击；不使用手机，不直接写玩家坐标或调用测试 tick。
- 在持续变化的输入下，测量窗内确实出现 Undo/历史重演，并观察逻辑推进、画面提交、原生工作时间和输入生产是否停顿。
- 性能窗结束后，通过真实 Escape 进入同步 Pause barrier，取得两端相同、完全确认的逻辑帧，对全部已导出的游戏状态字段作比较；再真实 Escape 恢复到可捕获的 Stage。
- 可选真正关闭 RTC input data channel 或 relay WebSocket，确认对端收到断连状态。故意断网后的正常停止单列，不误判为正常游玩的性能失败。

## 2. 入口及依赖

以工作树为当前目录，先做语法检查：

```text
node --check portable/multiplayer/rollback-browser-acceptance.mjs
```

根代理提供配套 Launcher 隔离树和正式构建包；以下占位符必须替换为实际绝对路径：

```text
node portable/multiplayer/rollback-browser-acceptance.mjs --launcher-root <真实配套Launcher树> --runtime <正式TH11MP包> --duration 15 --cpu 1 --route direct --profile stable --output artifacts/rollback-smoothness/browser-direct-1x-stable-01
```

有界矩阵分开运行，禁止多个浏览器测量同时争用 CPU：

| 项目 | 选项 | 目的 |
| --- | --- | --- |
| 基础 Direct | `--cpu 1 --route direct --profile stable` | 先验证合同、纠正和 Pause/Resume |
| 4× Direct | `--cpu 4 --route direct --profile stable` | 同一实际触控负载下的性能 |
| 基础 relay | `--cpu 1 --route relay --profile stable` | relay 独立链路 |
| 4× relay | `--cpu 4 --route relay --profile stable` | relay 与 CPU 负载交互 |
| 网络尖峰 | `--profile spike` | 延迟变化后的平滑降速及纠正 |
| 断连 | 加 `--disconnect` | 性能窗口后真实 channel 生命周期 |

每一项用新的 `--output` 目录。已有 receipt 拒绝覆盖，失败场景不能删除后只报告好的一轮。

已核对的迁移路径：

- relay：`<launcher>/services/netplay/netplay-relay.mjs`。
- 产品合同：`<launcher>/launcher/web/lib/contracts/product-catalog.mjs`。
- gameplay URL helper：`<launcher>/launcher/web/.cache/build/browser/assets/launcher/multiplayer-relay-url.mjs`。
- Playwright 默认：`D:/workspace/eagler/eagler-touhou/th11/tools/diagnostics/node_modules/playwright`，可用 `--playwright-root` 指向包含其 node_modules 的目录。
- Chrome：已有 Playwright 可执行文件或显式 `--chrome`；脚本不下载、不安装。
- 正式包：必须包含原始 manifest、runtime inventory、`th11-sdl.wasm`、loader/shell。
- retail DATA：默认真实 TH11 dat 路径，或环境变量 `TH11_TEST_ARCHIVE`。

缺少依赖、实际产品合同不允许 rollback/P12，结果为 `blocked`、exit 2；不是测试通过。能力检查通过后仍由未修改的 relay 按真实合同再次审核 start 请求。脚本直接测试 Runtime Host ABI，不证明 Launcher 产品开关和 UI 已接好。

其他参数：`--duration` 为 5–60 秒（默认 15）；`--latency-ms` 默认 50；`--jitter-ms` 默认 5；`--max-gap-ms` 默认 100；`--gpu hardware|swiftshader`。4× 是 Chrome CDP CPU throttle 4 被接受的证据，不等价于测得机器主频恰好四分之一。GPU SystemInfo 与实际 canvas WebGL renderer 都保存；软件渲染不能写成硬件性能。

## 3. 网络延迟和实际路线

二进制 WebSocket/RTC send 在原来的浏览器传输端延时，再调用真实 send：

- 稳定单向延迟为 50 ms ±5 ms；双方各自发送端施加，约相当于额外 100 ms 往返传播，但实际调度晚到另计。
- 延迟队列复制二进制载荷，避免待发送数据仍引用可重用的 WASM 内存；不修改包内容。
- JSON lobby/signaling 不延迟。可靠有序通道维持发送顺序；本来无序的 input channel 可以受到抖动的正常重排。
- `spike` 在测量时间 32% 处的 250 ms 窗内加 150 ms，在 66% 处的 350 ms 窗内加 300 ms。
- 保存 schedule、目标 due、actual send、lateness、长度和内容签名；receive 也保存签名和本地时刻。没有跨进程减时间推算 one-way latency。
- 队列最多 1024 个包/4 MiB，超过直接失败；日志溢出不能当作没有坏事。
- Direct 保存实际选中的 ICE candidate pair，要求非 TURN relay；relay 测试在初始浏览器能力中关闭 RTC，让共享 transport 的真实 fallback 选 relay，同时核对 native route。
- relay 是独立 Node 子进程，50 ms heartbeat 只测该进程 timer lateness；它不是逐包转发 trace，不能凭 heartbeat 排除 relay 的全部问题。

性能结束的确认尾段把新增发送延迟改为零，同时继续排空已经排定的包。该段单独计时，不能混入之前的 15 秒性能统计。

## 4. 触控和 rAF 观测边界

输入 producer 运行在每个真实 Host 页，以 `performance.now()` 和 `1000/120` 计算下一个槽。逾期槽计数并跳过，不按逻辑帧推进，也不补发洪峰。脚本发正常 `direct-touch`/ `touch-controls`/ `touch-cancel` 消息。Host 的高优先级事件分发、native touch controller 和逻辑帧输入采样仍为实际实现。

每个请求记录墙钟时刻和 acknowledgement；ACK 证明 Host 接受命令，**不能**证明每个 120 Hz 点都进入一个 60 Hz 逻辑帧。原生最终位置变化和 rollback touch cache 字段另外保留，但当前没有逐请求到 native frame 的一一映射。

每个真正调用 `Module.onGameFrame` 的 rAF 行保存：

| 字段组 | 意义 |
| --- | --- |
| requested → entry | 注册下一次 rAF 到执行入口的等待，包含正常等待刷新 |
| stamp → entry | 浏览器提供的 rAF timestamp 到 JS callback 入口 |
| entry → exit | 整个 callback 墙钟成本，含原生和 shell |
| nativeMs/nativeTicks | 实际 C++ 回调报告的工作时间和推进 tick |
| existingShellHookWall | 原来的 shell onGameFrame 消费时间 |
| observerWall | 新只读统计本身的时间 |
| rb presentations | 实际 backend present/commit 计数变化在 callback exit 的观测 |
| net NextFrame 高水位 | 前沿向前推进；回滚后的倒退不当作新进度 |

该观测不产生屏幕像素轨迹，不证明 GPU scanout 每次不同、子弹每帧视觉位移正确。present 计数与原生玩家位置至少比单独 FPS 强，但完整视觉验收仍需独立证据。

指标保留 count/p50/p95/p99/max 和 >33.4/50/100 ms 数量。present/frontier 另包含窗口起止处的停显下界，防止首尾长停顿被排除；它们是被测窗口截断的下界。测量中的原生单次工作或 present/frontier 窗口停顿超过默认 100 ms 会失败。该默认是“不得大卡”的最低门槛，不把 99 ms 自动称为理想高刷流畅。

降速本身允许；logicHz 与 networkWait/CPU yield 单独报告。网络尖峰导致长前沿停顿仍保留为坏场和用户体验证据，分析时结合输入信息不足与实际 native work，不预先归因设备。

## 5. 正确性尾段和错误门

稳定性能窗结束后先发 P0 真 Escape down，等两端 `th11_phase()==paused` 后释放。Pause 的实际 barrier 会等待相应帧确认，不改变核心 confirmed，也不停止一端输入制造“同步”。

仅采集暂停中、无 correction、无 rollback error、确认输入覆盖当前 multiplayer_frame 的 native 56-word 样本。按 generation+frame 找共同帧，比较 hash、stage、phase、frame 及所有席位导出值；排除 replay publisher 的不同到达进度。之后确保释放边沿经过多个真实逻辑帧，再发送第二次 Escape，等待两端正常 Stage 且 world.CanCapture。

当前浏览器 56 字段仍不是全内存世界 oracle。独立全世界证据见 `TH11MP-WORLD-VALIDATION.zh-CN.md`：world-oracle-01 实际通过 8 cases /150373 checks /1124 Update+Draw ticks。随后 `world-oracle-02-bulk` 以生产 `{14,max_bytes,max_blocks,true,true}` 配置复跑同样通过，实际 nig 命令 `-antv-yT5ZpsKNq92s5bMf14`；已独立读取收据，`passed=true`、`sourceStable=true`。两轮均不是浏览器性能结果。

整轮失败条件包括：native/shell/uncaught/console/crash 错误；实际路径不符；状态 ABI 缺失；Pause 或 Resume 不成功；相同已确认状态不等；容量或观测日志溢出；managed 请求未全部 ACK；动作/呈现/前沿覆盖不足；测量窗内 restore/history 增量为零；音频错误或混音不推进；性能阈值失败。不是只做 hash 相等或“没报错”。

## 6. 音频观测

每 100 ms 原生位置样本同时读取真实 `th11_audio_statistics()` 9 words：

| Index | 含义 |
| --- | --- |
| 0 | audio ready |
| 1 | 成功写入 SDL 的 1024-frame 混音块累计 |
| 2 | 累计混音 PCM frames（u32） |
| 3 | 错误锁存：1 mix 失败，2 SDL_PutAudioStreamData 失败 |
| 4 | 当前排队 stereo PCM frames |
| 5 | 当前音乐 track（无轨为 u32(-1)） |
| 6 | fade 余量 |
| 7 | 音乐音量 ×1,000,000 |
| 8 | music paused |

本短测沿用现有 Host `music:'none'`，仍实际打开并混合 DATA 音效。必须有混音增量、错误槽为零。它没有 BGM 覆盖、没有每音效播放计数，也没有 PCM 有效值/听感/音频设备实际输出证明。完整 3P 整局要求 BGM 和密集音效单独采证，见整局计划。

## 7. 收据和清理

开始前即创建 receipt。保存真实源码 HEAD/dirty、common、Launcher、archive、manifest/inventory、WASM、loader、shell、relay、host、runner、Chrome executable 哈希。正式 WASM 要与未修改 manifest 一致。结束再检查关键文件哈希未变；实际错误时仍尽量保存原始数据和初始错误。

每端完整 raw JSON 包含 rAF、transport、motion、Pause tail、触控生产/ACK；另外保存最终截图、relay 日志、relay heartbeat、实际路线和 GPU 信息。大型 raw 导出安排在确认字段完成且停止 native 循环以后，防止报告传输自身造成 watchdog 或被误纳入性能窗。

总墙钟预算为 300 秒 + duration；超时先写失败 receipt，再只关闭本次持有的 Chrome 和 relay，使卡住的 evaluate 有机会拒绝退出。不按进程名称杀其他实例。HTTP favicon 返回 204，避免无关 404 污染 console 错误门。只停止该次 runner 创建的 runtime、Chrome、HTTP server、relay。无部署、无测试站修改、无产品目录替换。

## 8. 当前尚不能承诺的覆盖

- 尚未运行本入口，语法/依赖/浏览器实际结果待根代理串行收据。
- 不包含标准 Launcher 的房间 UI、产品安装/构建更新和重新加入 UI。
- 不包含 3P、全部机体/六 Bomb、整局、Extra；六 Bomb 与复杂对象回滚已有组件证据，但不等于浏览器全部场景已通过。
- 不包含高刷真实显示器的像素扫描、移动设备或不同 GPU 驱动。
- 不因短测通过而宣称“所有网络、所有设备绝无掉帧”。
