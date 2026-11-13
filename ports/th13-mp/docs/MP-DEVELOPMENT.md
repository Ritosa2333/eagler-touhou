# TH13 MP 开发指南（M0 骨架 → M10 交付）

规则真值见 [RULES.md](RULES.md)；网络/回滚架构参考 th12-mp 与共用 SDK：
`../../docs/adaptation/multiplayer.md`（验收清单 MP-RULE/MP-01..15）、
`common/include/eagler/netplay/`（RollbackCore/SessionChannel/BrowserPeerTransport/
ConfirmedInputClock/旁观协议族）、`ports/th12-mp/src/multiplayer/`（Session/WorldState/
InputLanes/NetplayRuntime/SessionSetup）与 `ports/th12-mp/docs/ROLLBACK_CADENCE.md`
（12 帧纠正 + 1 新帧预算）。SP 构建与工具入口见同目录 [DEVELOPMENT.md](DEVELOPMENT.md)。

## 目录与身份

- 派生自 `ports/th13` @ `e1111b4daeb21296a5d0da526e2d512efc449aa2`
  （登记：`docs/migration/th13mp-derivation-20261108.md`；同步对见
  `docs/adaptation/port-sync.json` 的 `th13` 条目，`sync.mjs` 管理）。
- SP 目录 `ports/th13` 是冻结基线：MP 需要的通用修复**先落 SP、再 sync 进 MP**；
  禁止只改 MP 侧共享代码造成静默漂移（`portable/sdl` 与 th15 共享同理）。
- 构建入口 `portable/build.mjs`（本目录副本）：`-DTH13_MULTIPLAYER=1` 常开；
  产物落 `web/artifacts/sdl3/`（与 SP 同名不同目录）。M0 门 = MP 构建产物与
  SP 构建产物 **wasm 字节一致**（define 未被消费前必须一致；此后每引入首个
  `#ifdef TH13_MULTIPLAYER` 代码块，须复跑单席等价）。
- 测试缓存隔离：`tests/*/run.sh` 的对象缓存不得与 SP 共用目录；缓存键须含
  头文件与编译选项，不能只看 cpp mtime。

## 阶段工单（顺序执行，禁止跳依赖）

| 阶段 | 主题 | 主战场文件 | 退出条件 |
|---|---|---|---|
| M1 | 模拟/呈现/副作用拆分 | `web/cpp/sdl/Application.cpp:1364 step`、`BrowserRuntime.cpp:229`、`RunScene.*` | 同输入：一次 Update 不呈现 vs Update 后呈现 → 同一权威状态；重复 Draw 不改 hash/RNG；Update 22/24/25 序不变；失焦不单端暂停 |
| M2 | 一套世界多席自机 | `StageBattle.{hpp,cpp}`、`RunScene.*`、`ApplicationState.hpp` | 3 席独立移动/开火/Bomb/死亡；单席等价模式与 SP 逐位一致；敌方帧数不乘人数 |
| M3 | 目标/碰撞/共享资源 | `StageSim.*`、`StageCommands.cpp`、`StageGvars.cpp`、`ItemManager.*` + 新 `PlayerTargeting.*`/`ResourceGrant.*` | RULES R-I/R-G 目标与资源数值表全绿 |
| M4 | 共享灵界状态机 | `MultiplayerRules.*` + `StageBattle.*`、`SessionState.hpp`、`ItemManager.*` | RULES R-T1..T10 逐条测试全绿（T-T 系） |
| M5 | 合作生命周期与表现 | `StagePresentation.*`、`GameHud.*`、暂停/结果界面 | MP-RULE-01..05；单共享槽 + 每席资源 HUD |
| M6 | 完整快照 + 确定性基准 | 新 `web/cpp/multiplayer/WorldState.*` + ANM/敌机/调度 snapshot API | S→前进→restore(S) 全字段等价；再喂同输入逐帧一致（含池复用/父子 VM/暂停/换关边界） |
| M7 | 确认输入联网（先纯延迟） | `web/cpp/multiplayer/{SessionSetup,InputLanes,NetplayRuntime,Session}` + exports | 2/3 独立客户端同 seed 同帧一致；坏身份 frame0 前拒绝 |
| M8 | 12 帧回滚 | `Session.*`、`WorldState.*`、音频事件汇 | 注入延迟/乱序/丢包真实触发 rollback>0；纠正后全状态=按时基准；音效恰播一次 |
| M9 | Replay/旁观/生命周期 | `ReplayArchive.*`、`StageEntry.*`、旁观 schema | MP-06..11/15；断线保存返回；退出重入×3 |
| M10 | 主代理发布审查与正式接入 | `common/contracts/product-catalog.mjs`、launcher 配置、`MultiplayerExports.cpp`、ADAPTATION.md | 主代理独立审查、重跑真实浏览器验收并裁定发布标准；此前禁止推送、部署或发布 |

### 长程 Workflow 的施工边界（2026-11-08 用户修订）

- 一次运行连续完成 M1–M9，不以“一个里程碑结束”“需要用户选方案”或阶段性交接为停工点。
- 当前规则见 [RULES.md](RULES.md)。全队同时进入各自角色的灵界射击，期间全员禁雷。保护灵界耗尽后由发动者承担死亡。
- M7–M9 为完成真实多客户端验收，可提前接入必要的原生导出、独立 MP Runtime 壳及测试专用启动器路由。
  不得因这些文件原列在 M10 而只测一个脱离产品的网络模拟器；正式产品可见性与发布另由主代理裁定。
- 各阶段自主做小工单、独立回归测试、只读审查与修复，全部子代理使用 Start Plan GLM-5.3-Flash/max。
  常规技术选择按本作规则与现有共用 SDK 自主解决，不向用户发中途选择题。
- Workflow 的结束状态最多为“候选实现，等待主代理验收”，不是 RELEASE_READY。
  主代理亲自读改动、复核证据、运行验收并确认达到发布标准后，才可进入发布操作。
- 不降低既有比较标准、不以 stub/skip/TODO 代替实现、不丢弃 SP 基线；无法完成的范围如实列出，不能用成功返回值伪装完成。

每个工单绑定：具名假设、允许修改文件清单、最窄测试、完整回归入口、
实际命令与退出码。三轮无机制进展 → 保存最小复现换证据路径。

## 关键接缝（SP 已有，直接复用）

- `StageBattle(GameRandom* shared_random, StagePlayerState* shared_player)`
  （StageBattle.hpp:238）——玩家状态外置注入缝。
- `RunScene(progress, player, score, …, shared_random, shared_hud, shared_player)`
  （RunScene.hpp:225）——跨关共享件注入缝。
- `SessionState` 灵界 helper 族（SessionState.hpp:128-172）与
  `ItemManager::set_spirit_trance`（全场灵转换）。
- `RunScene` Update 20/22/24/25/28 任务序（RunScene.cpp:205-239）——多席改造
  不得重排。
- Replay 关头 22 dword + 种子恢复域（ReplayRecorder.cpp:23-41）——锁步对齐复用。

## 快照红线（M6）

- 禁 memcpy `ApplicationState`/std::function/容器/unique_ptr/引用/整堆。
- `FrameScheduler`（外部 callback 侵入链）按稳定 callback 身份恢复；
  `AnmRegistry`（裸指针/迭代器/borrowed/父子链）专用 capture/restore，
  恢复分配顺序；句柄 generation 一致。
- 资源（ANM bank/ECL/STD/SHT）在未确认快照存活期不卸载；换关只在确认栅栏后。
- hash：显式序列化全权威状态（稳定顺序，float 按 bit）；不 hash 指针/padding/
  unordered_map 顺序/localSeat/墙钟。

## 与共用协议的接口约定（M7 前冻结）

- TH13 专属：SessionSetup Words/GameplayVersion/GameplayAbi 身份串（含当前规则版本
  与构建身份）；输入位掩码以本作 GameInput 为准（注意 C=0x800 且键盘同时置 0x200，
  进网线路前归一化）；`/savesth13mp` 存储根与 `th13mp_XX.rpy` 前缀。
- 共用不 fork：RollbackCore/传输/旁观/relay 直接链接 `common/`；发现通用缺口先提
  精确设计 + 跨作品回归，不在作品目录内复制网络栈。
