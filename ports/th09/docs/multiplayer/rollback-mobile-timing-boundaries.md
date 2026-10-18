# TH09 手机触控实验：计时边界与实际游戏负载

状态：源码只读审计；未新增性能实验。日期：2026-10-11。

本文解释本轮 mobile rollback 实验的计时与输入含义，不为 relay B1 的高成本指定原因，也不把旧键盘实验当作相同工作负载。ABBA、后续链路实验的全部结果、失败与来源身份由本轮主证据保存；本文没有重新读取或筛选大 raw。

审计的 title 根目录为 `D:/workspace/eagler/worktrees/th09-mobile-rollback-4x`，来自 `b243bb1d24335b1fdab3606e27c6f67e4c636ad9`。common 只读参考为 `D:/workspace/eagler/worktrees/th09-rollback-input-batch-common`，提交 `512353cab9eaa4f6744f867b0eb07c61ee7f2b0e`。下文 title 路径相对前者；标为 common 的路径相对后者。行号按本轮读取版本，运行身份以冻结的 build/helper SHA 为准。

## 1. 首要结论

- `measure.samples[i][2]` 是整个 C++ rAF 回调范围内的墙钟工作时间，包含同步跨语言调用；不是一次 game tick 的纯 C++ CPU 时间。
- 本次 `onGameFrame` 中的 touch `onFrame`、音频观察、数组追加发生在该数值求值之后，不直接进入这个 ms。
- 由 native callback 内发送触发的 `touchInputProbe.onLocalInput` 是同步调用，进入这个 ms；由独立网络 pump 触发的同一 observer 在该区间之外。
- touch ABI、短状态读数和 gesture sample 没有隐含 world tick、draw、完整世界抓取或 hash。它们及探针仍有观察成本；源码不提供这个成本的实测数值。
- touch auto-fire 3/3、连续移动、转向与 focus 形成的真实输入，和旧键盘长按 tape 不同。它们可以进入不同的射击、蓄力、保护、碰撞及音频路径；本次影响多少、方向如何仍需实际证据。

## 2. samples[i][2] 从哪里开始、在哪里结束

源头是 `th09_web/cpp/sdl/Application.cpp:612–644` 的 `th09_loop_start`。Emscripten rAF 回调进入后记录 `begin = emscripten_get_now()`，处理 pacing/cadence，按需要调用零次或多次 `th09_game_tick`，末尾执行：

```cpp
probe->audio.pump();
TH09_PERF_STOP(callback_cost);
th09_browser_frame(ok, emscripten_get_now() - begin);
```

`Application.cpp:33` 的 EM_JS 把已经求值的 milliseconds 传给 `Module.onGameFrame`。`th09_web/tests/multiplayer/smoothness-browser.mjs:457–466` 原样保存这个 ms：

```js
m.samples.push([now - m.started, ok, ms, ...s, n, phase, rafTimestamp]);
```

| 工作或等待 | 是否进入 samples[i][2] |
|---|---|
| rAF 请求之后、进入 C++ 回调之前的等待 | 否 |
| C++ pacing/cadence 与该回调内的 game tick | 是 |
| tick 内输入捕获、恢复、快照、模拟和必要绘制 | 是 |
| 该范围内同步执行的网络、WebGL、音频 JS 桥 | 是 |
| rollback_tick 的音频 update/pump，以及外层 audio.pump | 是 |
| 本次 onGameFrame 的音频观察、play.frame、touch onFrame、数组追加 | 否 |
| 两个回调之间运行的独立 touch source timer / network pump timer | 否 |
| 最终 touchInputProbe.snapshot 的归档、filter 和分布排序 | 否 |

`rollback_tick` 自身还在 `Application.cpp:576–577` 调用音频 update/pump；外层 rAF 的 pump 是另一个现有调用点。本文只界定两者均在计时范围内，不据此认定重复或浪费。

一个样本可能包含零个、一个或多个 game tick，单个 rollback tick 又可能包含历史重演。回调进入后的宿主抢占、同步浏览器 API 或分配引发的停顿也可能体现在墙钟差值中，不能把 ms 当作精确线程 CPU。

`smoothness-browser.mjs:473–477` 中的 development `_th09_probe_tick_cost` 是另一组数据，注释明确它描述最后一个 tick，不是整个 callback 总成本；release 分析不能用它替换 samples[i][2]。

## 3. onGameFrame 内还有不同的观察时间点

当前 harness 的顺序是：

1. C++ 已求出 ms，调用 onGameFrame。
2. `audioOutputProbe.observe`。
3. `play.frame()`。
4. 取得 `now = performance.now()`。
5. 读取 native stats，调用 touchInputProbe.onFrame。
6. 把先前的 now 和 ms 写入 samples，再记录其他诊断字段。

因此 `samples[i][0] + measure.started` 不是整个 JS 回调的退出时刻。它在本次 touch onFrame 之前，但在本次 audio observe 和 play.frame 之后。timestamp 之后的探针工作仍可能推迟下一次回调；不能因为它不计入当前 ms 就认定对连续画面间隙没有影响。（`smoothness-browser.mjs:457–481`）

`play.frame()` 只是读取进度并更新全局状态，见 `th09_web/sdl-runtime/shared-netplay.mjs:370–379`；不要把它写成网络 pump。独立的 `pump()` 才调用 `_th09_rollback_pump`，并在通常路径安排约 16 ms 后再次执行，见同文件 180、233–242 行。

touch source 的时间使用 `touchInput.clock.startedAt` 作为相对原点，主样本使用 `measure.started`。同页比较须先还原成各自的 Page performance.now 值；不同页面或 relay 时钟仍需沿用已有原点和时钟资格，不能直接相减相对数值。

## 4. 同步 wire observer 是 native 时间内外都可能出现的成本

计时区间内发送的源链如下：

| 位置 | 操作 |
|---|---|
| title `Application.cpp:491–500` | 新逻辑输入只捕获一次 |
| title `RollbackSession.cpp`，Capture | ScheduleLocalInput 后通知 LocalCaptured |
| common `src/netplay/SessionChannel.cpp:228–244` | LocalCaptured 调用 SendInputs |
| common 同文件 180–190 行 | 构造/编码冗余输入包，调用 transport.SendTo |
| common `src/netplay/BrowserPeerTransport.cpp:861–865` | 同步进入 eagler_peer_send_to |
| common 同文件 610–633 行 | 复制 payload、封装 relay envelope，调用 WebSocket/RTC send |
| title `smoothness-browser.mjs:348–377` | send wrapper 在原始 send 之前调用 observeLocalInput |
| title `touch-input-probe.mjs:123–167` | 同步验证和记录本地输入 |

如果这条链由 C++ rAF 内部触发，observer 工作在 samples[i][2] 内。如果来自独立的 SharedNetplay pump timer，同样的 observer 在 C++ rAF 计时之外运行。所有发送都可能影响主线程，不能把全部 observer 成本一律算入或一律排除出 ms。

本轮读取的冻结 touch helper 已做重复历史短路：每条先读取三个 u32，重复帧比较 `word0/xBits/yBits` 后返回；`rawHex` 只为首次观察的逻辑帧构造。每包按 v4 输入格式验证，并检查最多 32 条历史。不要用早期“重复历史也每次生成 hex 字符串”的实现解释本轮 B1。（`touch-input-probe.mjs:131–145`）

旧键盘 tape 也有同步 observer：它验证包内所有历史输入，并调用五次 `_th09_key` 设置下一帧键值，见 `smoothness-browser.mjs:435–454`。源码能说明两种观察内容不同，不能直接给出哪一种更贵。

`firstWireAtMs` 是 send-intent hook 首次看到已序列化输入的时刻，不是成功发送完成、网络送达或精确 native capture 时刻。背压可能延迟首次可见发送；“某事件在 firstWire 之前提交”不自动证明它早于这帧真正的 capture。冻结 helper 已明确保留这条限制。

## 5. touch ABI 和 probe 没有额外推进世界

`Application.cpp:651–661` 的相关 ABI：

- `th09_touch` 读取简短 touch state，再调用 `gestures.pointer`。
- cancel/controls/options/stick 更新设备手势、选项或限幅后的向量。
- `th09_touch_state` 返回四个状态字段。
- `th09_touch_probe` 返回本地玩家位置、速度及当前 motion sample，共八个 float。

它们没有调用 game tick、draw、WorldState Save 或完整 hash。`sample_keys` 的 touch 分支读取设备、调用 `gestures.sample`，再生成按钮和 motion sample；目标变换读取当前玩家的少量状态，不遍历全体游戏实体。（`Application.cpp:325–349`；`portable/input/TouchController.hpp:23–64`）

实时 touch probe 仍做以下工作，不能称作无开销：

- 每次实际提交 movement 事件，前后各读取四字段 touch state，并保存事件行。
- 每次新前沿，读取八字段运动值和四字段状态，追加一条 position 记录。
- 每次发送，观察完整输入与重复历史。
- source timer 用二分查找定位到期事件，按预计算的同 gesture/leg 范围合并迟到 MOVE。

完整 events 的分类、filter、排序和输出组装在最终 snapshot；不是每帧对整个事件集合做全量扫描。实时 onFrame 的 pending 队列按已到达的新前沿向前处理。源码不支持“每帧抓取整个世界”或“每帧排序全部事件”的说法。（`touch-input-probe.mjs:48–61、81–101、168–224`）

source coalesced 数量表示源定时器迟到时按既定规则未逐条提交的 MOVE。它不是成功的逐 MOVE 响应计数，也不能单独用来认定迟到由探针、native 模拟、浏览器还是宿主造成。

## 6. 自动射击与旧键盘长按不是相同游戏负载

TH09 普通射击与持续蓄力有不同输入语义。当前 native 触控 auto-fire 有意产生真实按下/释放：

```cpp
if (state.context == 1 && gestures.enabled && gestures.fire && !keys[90])
    sample.keys[90] = ((auto_fire_frame++ % 6) < 3);
```

它按逻辑 capture 的采样次数推进，不是按 60/120 Hz source timer 推进，也不在历史 resimulation 中重新取设备输入。它进入完整输入与 Replay。（`Application.cpp:329–333、491–510`）

旧键盘 frame tape 的 shot 位是 `frame % 126 < 90`；旧 wall-clock keyboard 是 `age % 2100 < 1500`。它们与 3/3 的射击边沿不同。`TouchController::cancel` 不关闭 fire 字段；当前 profile 在 UP/CANCEL 后仍保持 fire 控制开启，所以 lifted 段也可以继续产生射击边沿。（`smoothness-browser.mjs:427–441`；`TouchController.hpp:17–22`；`touch-input-probe.mjs:23–33`）

真实游戏消费链：

1. `cpp/game/GameInput.cpp:3–16` 从前后输入计算 pressed/released。
2. `cpp/game/ShotControl.cpp:17–47` 区分 tap、held/focus 蓄力和 release。普通 tap 启动/调整 normal_fire；蓄力推进 charging_time/charge；release 在满足条件时触发攻击与保护。
3. `cpp/game/Player.hpp:51` 将 fire(set,time) 交给 PlayerShots。
4. `cpp/game/PlayerShots.cpp:33–38` 按对应射击定义创建 shot。
5. `cpp/game/Player.cpp:54–62` 将运动、shot 更新、射击与碰撞放进完整世界更新顺序。

`automatic_focus` 还改变 ShotControl 分支；没有核对实际配置前，不能无条件写“长按必然蓄力、3/3 完全不蓄力”。可以确认的是：这些输入足以进入不同的普通弹、蓄力攻击、保护、碰撞及音效路径。每条实际 tape 的世界密度、声音数量和成本差异，需要实测数据；不能从源码推导 B1 一定是弹幕更多。

同样，连续拖动和旧方向键的玩家路径不同，会改变射击原点与交互结果。墙钟计划相同也不保证跨运行捕获 tape 相同，因为 capture 取决于实际逻辑前沿。必须保留真实 tape；不能称旧 keyboard 与本轮 touch 是同一个工作负载。

## 7. correction 少不等于回调工作少

即使某次没有 correction，回调仍可能执行正常世界更新、绘制、音频、发送与快照。`Application.cpp:539–545` 依据 predictedMask 或尚未完全 reconciled 的历史决定保存状态，并不要求本次已经出现 correction。

持续恢复也可能出现 corr=0、resim>0。相反，一个 callback 内可能没有新 game tick。比较时应同时保留完整正文区间的总工作时间、fresh/resim/wait/零推进组成、真实输入变化、连续新状态完成间隙和单独标注的 RAF 时间戳间隙。

B1 成本高于 C1、C1 的 correction/resim 更多，都不能仅凭这两个方向给 B1 归因。不能据此认定是探针、宿主、游戏密度，也不能据此宣布回滚不再是瓶颈。ABBA 全部运行、异常和不同 tape 应一起保留；不得选择较顺的一次，也不能因组件或端点正确性 PASS 就称流畅目标完成。

## 8. Begin/Delta 不是现成的免费优化

common `NetplayCore.cpp:235–275` 默认对 DirectTouchDelta 预测零新增位移，同时不重复 Begin。若持续 MOVE 每帧有真实非零 delta，完整 FrameInput 仍可能不同，仍需纠错。可回滚余量解决“新增位移只消费一次、未完成运动可恢复”的状态契约，本身不保证降低持续移动的 correction。

接入 TH09 还要定义 Begin 的位置基准、focus/scale/rate 与边界消费顺序，把余量纳入 WorldState 和状态验证，并处理现有 spectator 0..3 模式与 common Delta/Begin 编码的区别。TH09 MotionTrack 记录实际消费的 float 速度；不能把格式仍相同说成旧新 Replay 字节必然相同。

TH07 可选的 Delta 预测、量化、等价吸收在现代源码中默认是 0/0/false；common 现成等价证明还限制单轴修正、无跨轴余量及安全边界，不能直接覆盖当前双轴持续拖动。它们是后续独立语义/正确性研究入口，不是本轮已实现或已测得收益的修改。（`th07-eagler/src/netplay/Th07LanStageProbe.cpp:1137–1147`；common `include/eagler/netplay/DirectTouchEquivalence.hpp:44–149`）

## 9. 本文的使用范围

本文仅固定源码解释。没有新增实验、改变生产/harness、重算 raw 或建立 B1 的因果归因。后续若需要低观察成本对照，应独立预声明、保留原始失败和所有运行身份，并保持实际输入与完整正确性检查可核对；不能为了得到好看的计时而删除输入变化、Replay 状态或不利间隙。

