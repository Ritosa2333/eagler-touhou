# TH11MP 预测回滚工作记录（2026-10-13）

## 当前阶段与恢复入口

用户在 TH10 阶段收尾后明确授权开始 TH11 回滚，并要求经常重读先前经验。
本轮目标是降低输入等待、保持真实运动连续；网络信息不足时允许渐进降速，正常游玩中的长停显不可用平均 FPS 掩盖。
**恢复基础、整世界纠正 oracle、纹理、帧调度与房间合同已通过；Application/JS 已接入正式 Web Release。浏览器已通过局部确认状态与 Pause/Resume，但 4× 性能明确失败，正在验证有限减负改动。3P 挑战 Lunatic 全流程尚未完成，不能交付。**

每次恢复上下文、开始一个新子阶段、修改捕获/恢复/调度/Draw/输入/确认输出之前：
1. 重读本文件的当前阶段、基线和未解决项。
2. 打开对应原始经验章节；不能只靠上一轮的口头总结。
3. 核对工作树状态和实际源码，旧文档的 HEAD/阈值/结论不自动适用于当前构建。
4. 完成一项后登记修改范围、精确命令/证据、失败和未运行项，再开始下一项。

## 基线与工作边界

- 稳定 MP 树：D:/workspace/eagler/worktrees/th11-multiplayer，分支 eagler-mp。
- 实施树：D:/workspace/eagler/worktrees/th11-rollback-smoothness，分支 experiment/rollback-smoothness；该隔离树已存在，本轮核对为 clean 后采用。
- 两树起始 HEAD：d32390730ae837fb1333bc1f1d03d746f4697936。
- 实施树嵌套 common 起始 HEAD：984a8a10834c35fe9ef86c4ffe03daf1fb61f87c，detached、clean。
- 构建入口 portable/build.mjs 目前固定该 common；正式选项包含 -O2、-ffp-contract=off、TH11_DEVELOPMENT_HARNESS=0。
- 当前 TH11 是纯锁步：旧 IMPLEMENTATION.md 中“不做预测与回滚”是上一阶段范围，本轮用户已授权扩展；原作与既有多人玩法规则仍须保留。
- 全部工作区操作通过 nig。构建和浏览器测试由主执行串行调度，代理不独立启动负载。
- 不使用手机；可在桌面浏览器按墙钟持续生成触控移动。
- TH10 封存，不继续开发或测试；不修改 TH08/TH09 成功树。
- 不自动合并稳定分支、不推送、不部署、不修改正式站或测试站。

## 必须回看的经验来源

路径相对于 D:/workspace/eagler/，除非另行标明。

| 修改领域 | 重读来源 |
| --- | --- |
| 全局分层与调度 | worktrees/multiplayer-rollback-architecture/docs/playbooks/multiplayer-rollback-architecture.zh-CN.md §§2–4 |
| 一次采样、早发、触控与 managed 生命周期 | 同文 §§5、6 |
| authored Draw、高刷与连续运动 | 同文 §7，尤其 7.1–7.5 |
| journal、资源释放、启动图与退休包 | 同文 §8 |
| 验证边界、失败实验 | 同文 §§9–10 |
| TH06/TH07 journal 经验 | 同目录 rollback.md，不能照搬跨帧 checkpoint span |
| TH10 最终边界 | worktrees/th10-rollback-smoothness/docs/multiplayer/TH10MP-CLOSEOUT-20261013.zh-CN.md |
| TH10 最新失败定位 | worktrees/th10-rollback-smoothness/docs/th10-rollback-status-20261013.zh-CN.md；主历史文档 §10.56–10.58 |
| TH11 原生/多人玩法 | 本树 docs/multiplayer/IMPLEMENTATION.md，尤其 §§3–6 与最后修订 |
| 当前工作区与产品契约 | AGENTS.md、WORKSPACE.md、docs/agent/ROUTES.md、eagler-touhou/docs/playbooks/multiplayer.md、runtime-build.md |

当前主 Launcher checkout 的部分旧 playbook 路径已迁移/不存在；跨作独立文档树仍可读。不能把真正的文件 NOT_FOUND 与 nig/FRP HTTP 404 混为一谈。

## 不得重复的错误

- 不把 confirmed、next、replayEnd、最后完整显示状态混为同一个前沿。
- 本地输入一帧只采样一次；重演不消费新触控、pulse、菜单动作。输入在昂贵恢复前尽早发送。
- TH11 的触控表示必须按实际源码判断，不能套用 TH08 fresh delta 或 TH09 held target 的名字。
- 快照不恢复设备/网络接收、ACK、wall clock 或已捕获输入。
- Update、语义 Draw、封帧构成完整事务。不能整段省略历史 Draw；GPU 省略必须单独证明。
- 分片恢复的中间世界不用于画面或触控 getter。CPU 让出与缺输入等待分开记账，不额外丢掉应有的 forward。
- 高刷 Present 数不是位移连续性。最终顶点取整、提前显示终点、旧端点倒退都需要实际检查。
- 动态容器、unique_ptr、多态/引用绑定对象不做对象头 memcpy；稳定地址、首写前像、物理退休是三份合同。
- 音频、Replay、观战、文件和资源动作只在其真实确认与寿命边界发布；预测不能泄漏不可逆输出。
- 稳定网络的必要预测量不因设备成本被主动增加 D；网络缺口与设备性能分别观测。
- TH10 受控 RAF 补偿默认关闭，300ms 对照收益不代表正常 4× 整场成功，不默认移植。
- 禁止降低 canonical/音频门禁、忽略真实晚输入或只保留最佳测试场次来取得 PASS。

## 已发现的 TH11 特有接入风险

1. NetplayRuntime 目前禁用预测并拒绝 predictedMask；SessionSetup/JS/协商/观战/能力元数据都有纯模式约束，不能只改一个开关。
2. Application 当前逐帧直接处理音频、文本、Draw、Replay 和菜单保存；该顺序只在纯锁步前提下成立。
3. AnmManager 使用 vector<unique_ptr<AnmVm>>、active/available 映射，geometry 会 free；EnemyManager 使用 unordered_map 与绑定引用的 Entry；LaserManager 有 new/delete。
4. Stage Draw、SceneCompositor、ANM draw callback 存在状态写入；现有 frozen draw 不是完整的只读高刷机制。
5. 当前 canonical 覆盖、对象复用/重开、开场轮询差异、所有机体 Bomb/护盾需按真实组件验证，不能只测静止射击。

## 实施顺序与门槛

1. 明确状态 owner、资源寿命和事务边界；先完成可独立验证的恢复基础。
2. 以不开放玩家预测为前提建立确定性回滚 oracle：参考、预测纠正、再次重演；核对完整状态、拓扑和副作用。
3. 接入输入历史、确认输出与 bounded recovery；保持纯锁步、Replay、观战路径。
4. 再接协商与能力声明、桌面真实 RTC/WS 和合成触控。
5. 正式构建身份冻结后，做有界 4× 稳态/尖峰/恢复/生命周期验证，报告尾部和运动连续性。

未通过上一层的核心正确性门槛，不用性能跑数代替。未测项目明确写未测。

## 已实施与实际验证（持续更新）

### 1. 纯锁步协议基线

命令 `node portable/multiplayer/network-test.mjs`，nig command `OLH14lpx4IHWih7nlkCIdsIz`，exit 0。
通过原有 2P/3P、D=0/2/9、空洞/乱序、一次采样、新代与旧 ACK、Replay/观战检查。
包含 common hybrid 协商单元用例不代表 TH11 世界已经支持预测。
证据：`artifacts/rollback-foundation/initial-checks-01.json`。

### 2. managed 触控接收修复

生产改动仅 shell.mjs；合法 touch/control/cancel 实际派发后不排队等待资源与 IDB。
保留原 Host 消息合同；拒绝生命周期内消费 source serial，但不生成 native action。
TH11 native reset 保留 mapped serial，不能复制 TH08 的重开归零。
初版复审发现 BFCache 恢复会永久关闭触控，已分开 persisted suspend/resume 与 terminal retirement。

- 初版 `UXWKw-RRGRQDCYxpH5wMXc7l`：13/13；只代表初版。
- 修正版 `pBtSr896FLlteVycT9nNWjI7`：14/14，exit 0。
- 修正版 shell SHA-256：`9b6c4826aa460661396740904fa8c16290d517effd512c01b7a3705e3737f6fb`。
- 相同测试对 git HEAD 旧 shell：`sY8kjL3tGep1bV1tJWWwchOK`，exit 1，3 pass / 11 fail。实际重现 IDB/resource 阻塞等问题。
- 旧 shell SHA-256：`e3d9ec86e152de9d6aafafb8e3723fdd560de983ed4d3ddcfcab2c9d0ab36614`。

测试执行完整生产 ESM listener 与真实 host helper；native/会话接缝为 mock。
不是手机、真实浏览器 BFCache、物理输入延迟或游戏性能验证。

### 3. BulletManager / ItemManager 首写恢复

新增 MP-only BeginRollbackFrame / EndRollbackFrame，初始 active 连段捕获、inactive 初始化前完整槽捕获；
reset 补齐未覆盖段，避免与已捕获段部分重叠。共享 RNG/ANM/world effects 有另外的 owner。
mp_hash 改用 const at 读取，未删除或改变 hash 字段，避免观测本身触发全池捕获。

- `o2-01`：`GUzlLY1Iq95zhmD8s2kN-Y0H` exit 1，独立 runner 漏 Movement.cpp 与 EnemyState.cpp 链接依赖，未进入测试；原收据保留。
- `o2-02`：`NXnxYd0Cd_L-FTemMJu4BEcB` exit 0，**19 cases / 195 checks**，两 manager ordinary compile 也通过，源码前后身份一致。
- 证据：`artifacts/multiplayer-rollback-managers/o2-02/receipt.json`；专属说明 `TH11MP-ROLLBACK-POOLS.zh-CN.md`。

此接口只用于 fresh checkpoint，不允许未证明的 extendPrevious 跨帧合并。
容量错误锁存在 journal，拒绝 EndFrame；不承诺失败后生产函数无写入。

### 4. AnmManager 动态 VM 与 geometry 恢复基础

MP-only 原始 VM 地址保留，逻辑分配 cursor 可回滚；active VM 完整捕获，inactive/new slot 在初始化前捕获。
typed snapshot 保存 active/available/tags/geometry 容器值；不 memcpy STL/多态对象头。
geometry 使用物理 lifetime lease，最后一份 checkpoint 离开前不释放可恢复的旧指针。
恢复顺序必须是完整 common journal UndoTo → 所有 owner typed metadata 恢复 → 释放被撤销的 checkpoint。
外部嵌入 VM 由其所属 owner 捕获；ANM manager 只捕获其 geometry 数据，避免重叠登记。

- `0XJE8RqPzIwryPDG7fBiBLhP` exit 0。
- ordinary **1 group / 20 checks**，MP **8 groups / 331 checks**。
- 证据：`artifacts/rollback-smoothness/anm-component-01/receipt.json`。
- 覆盖多 checkpoint geometry 换址/释放、整个 VM 字节与 RNG、slot 同址复用、真实 nested spawn、错误 owner/frame、容量失败、外部 VM 和最后 lease 释放。
- 原 nested spawn 的 child-link 形状存在既存观察：未把推定树拓扑当合同，未改变旧语义；oracle 保留所有 active VM 字节，即使某节点不可从 child.next 到达。

typed 容器快照的分配/复制成本尚未实测，不能称为性能优化或最终热路径方案。
尚未接入 Application，不具备完整世界回滚能力。

### 5. LaserManager 稳定槽

- `Pfg2b5Vk40NI7H589Rh-ryUz` exit 0；ordinary **1 group / 21 checks**，MP **9 groups / 2,108 checks**。
- 证据：`artifacts/rollback-smoothness/laser-component-01/receipt.json`；专属 `TH11MP-ROLLBACK-LASERS.zh-CN.md`。
- MP 首次激光分配 256 个对齐槽，约 940 KiB，保留物理地址；捕获全部活跃 stride，inactive 在清零/跨类型 placement construction 前保存。
- 实测涵盖跨类型同槽、lazy 分配、失败初始化、切割/反射时真实嵌套生成、容量/ID、完整 payload/metadata/链表和失败封帧。
- 外部 world 仍为独立 owner；这不是整场或性能 PASS。

### 6. Enemy/ECL、deformation 与 ANM 脚本字面量

- Enemy/ECL：稳定 Entry 与按 size 的 allocation 池，覆盖 context、thread node 和调试输出 1,024 B 临时块；保留父先占槽、子先入链的原执行顺序。`xwAnRiHr45EvLVH_KKse_79H` exit 0，ordinary **1 group / 14 checks**，MP **12 groups / 416 checks**。证据 `artifacts/multiplayer-enemy-rollback/o2-01/receipt.json`；专属 `TH11MP-ROLLBACK-ENEMY-ECL.zh-CN.md`。
- ScreenDeformationPool：typed vectors + 稳定对象；逻辑 release 仍执行 erase，旧条带 VM 存在时禁止复用，并保留 partial initialize 的回调 owner。`2Q8MtCdpembuQqX3s33woPxG` exit 0，ordinary **1 group / 155 checks**，MP **7 groups / 809 checks**。证据 `artifacts/rollback-smoothness/deformation-component-01/receipt.json`；专属 `TH11MP-ROLLBACK-DEFORMATION.zh-CN.md`。此组件测试不代证 GameBattle/Bomb 新接线。
- 新发现 ANM int_dest/float_dest 可写脚本参数原字节，VM/RNG 恢复不足以恢复脚本。仅当真实目标为脚本参数时触发首写保护，避免与完整 VM 捕获重叠；保留原参数求值/RNG 顺序。`7r1WU7w8vrvPfhtvBI_y057R` 已完成且收据 passed/sourceStable 为 true（命令保留期过后以磁盘收据核对），ordinary **1 group / 20 checks**，MP **13 groups / 3,076 checks**。证据 `artifacts/rollback-smoothness/anm-component-literals-01/receipt.json`，完成于 2026-10-13T07:44:48Z。新增覆盖 31 个写入 opcode 的 93 组目标组合、合法 VM 目标、共享脚本、跨帧、nested spawn、外部 VM 和容量失败。
- 恢复仍须保留所有跨越 checkpoint 的 geometry leases 到整个 Undo 和 typed restore 完成，不能只留目标快照。

### 7. Stage 与全世界接线：仍在验证

- Stage 实施 StageState、固定 object VMs、mutable object/instance flags、deformation pool；历史存在时不得重分配对象动画 vector，提供 Undo 前形状/owner 验证。
- Stage `rqqKyB2tGuSpmClU25JjaP5-` / component-01 exit 1：runner 漏真实 FogInterpolation.cpp 链接依赖；未进入测试，证据保留。
- 修正依赖后的 `mVCYt1b7FBdmGaZ7DxfHaFTc` / component-02 exit 1：ordinary **1 group / 19 checks** 通过；MP group 5 check 160 `open fresh shared checkpoint` 失败。原因是负面测试 j.Clear() 后 journal 已 unconfigured，重开前漏 Reset；只修测试，未删除负面门或修改生产语义，原收据/stdout/stderr 保留。
- `XW9BlZCQ2xdzkZTlrWeZwmf_` / component-03 exit 0：ordinary **1 group / 19 checks**，MP **6 groups / 194 checks**。涵盖 authored Draw、跨帧、旧回调 owner、布局/身份预检和容量失败。
- GameBattle/Bomb 已接入 deformation pool；PlayerRollback 显式捕获 Player/Motion/Shot 值字段；WorldRollback 新增稳定战斗资源图的总 owner。容器 typed snapshot，引用绑定 GameEconomy 显式值捕获；三个 ANM manager、ECL、子弹/道具/激光、Stage、逐席 Player/Bomb 全部接线。
- World 捕获和 Undo 前验证均检查子 owner 观察器空闲，包含所有 Bomb pool；独立复审指出的两处 observer 前置门已补齐。
- Pause/结果/Ending/下一关在破坏资源前请求确认屏障；运行时调度尚未接入，因此不会改变当前纯锁步路径。完整纹理恢复正由单独 SDL owner 实施。
- 全 game O2/WASI 原有 native 回归：`D2z9RBHVH4jmU8Pn0pIapGe5` exit 0，**8,834 checks / 16,979 native logic/draw ticks**。输出 `artifacts/rollback-smoothness/world-native-01`，收据 passed/sourceStable=true，完成于 2026-10-13T07:54:32Z；包含六机体/Bomb/护盾/救援/暂停/重开等既有功能。证明当前完整游戏源码可编译并通过原有玩法回归，不证明新整世界 rewind 正确。新三世界/纠正 oracle 已落盘，尚未执行。

### 8. 整世界、纹理、帧调度与协议新验证

- World oracle：`nDuzS61nUUzDAMm23IhjtvTz` exit 0，**8 cases / 150,373 checks / 1,124 real Update+Draw ticks**；`artifacts/rollback-smoothness/world-oracle-01/receipt.json` passed/sourceStable=true。同实例参考→撤销→错误预测→撤销→真实纠正→再次撤销，包含 2P/3P、绝对触控/摇杆、六机体 Bomb、Nitori 命中、life-hold 救援和确认边界。完整组件值、拓扑、脚本字节和 Draw 命令比较；不是 SDL/GPU/网络整场结论。详见 `TH11MP-WORLD-VALIDATION.zh-CN.md`。
- Texture：`_q4S-sGxYY1QNXCpkXhYL9Jj` exit 0，ordinary **1/115**，MP **8/600**；`artifacts/rollback-smoothness/texture-component-01/receipt.json` passed/sourceStable/assetsStable=true。覆盖字体 CPU 像素前像、跨越帧纹理 union、资源身份预检和 GPU dirty 通知，不代替真实 SDL/GPU 构建。详见 `TH11MP-ROLLBACK-TEXTURES.zh-CN.md`。
- FrameSchedule：`em6UV9VYNaCrxp0PtGOSE8G1` exit 0，**8 groups / 24,429 checks**；`artifacts/rollback-smoothness/frame-schedule-component-01/receipt.json`。区分 CPU debt 与 network retry，不重缩放旧相位、不把历史重演计作新 forward。详见 `TH11MP-ROLLBACK-FRAME-SCHEDULE.zh-CN.md`。
- Runtime network：`HLMBLWT8oi7utcONj3dTVR3s` exit 0，`node portable/multiplayer/network-test.mjs --native-only --output artifacts/rollback-smoothness/network-runtime-01`。新增预测 **8 groups / 1,202 checks**，包含 P12 缺口、按键边沿、触控、重复纠正/提前边界、确认输出门、v5/v6 Replay；既有 2P/3P D0/2/9、旧 ACK、新代测量、spectator tail 仍通过。JS 新合同检查另行执行。

### 9. Runtime 与 Launcher 接线进行中

- Application 新 `MultiplayerRollback.inc` 包含 fresh checkpoint → Update → text → authored Draw → seal，跨回调有界恢复；最终完整前沿触控缓存独立于历史世界。
- 在 `SetCommitLimit` 前退休 raw history 和 typed owners；音效、Replay、观战仅发布不可撤销的确认前缀。旧 mode1/只读仍走完整 exact 生命周期。
- Renderer 的 pending 保存的是可变 FBO handle；恢复/中间历史/失败不能 commit 它。新增 cancel_present，实际只在完整事务后更新 canvas，确认边界取消过的 Draw 不发布。
- JS setup 新 v6 仍22词，w14模式；mode0 固定 D0/P12，mode1 P0，native保留 mode2。旧 v5 Replay 保持识别。正式构建与打包元数据改为对应能力，尚未生成新正式产物。
- 配套 Launcher 隔离树 `D:/workspace/eagler/worktrees/launcher-th11-rollback`，分支 `experiment/th11-rollback-runtime`，基线 `b6aa0ae7798fae5cb0054891f3d9290307e5ccc9`；创建命令 `G0ewp-91Yd-7cTuFPcPecw2j` exit 0。原 dirty Launcher 不动。修 TH11 catalog 和实际 consumer 上限，不能用旧 P0 房间私设 P12 来伪造真实协商。

### 10. 用户追加整局验收要求

2026-10-13：既定回滚目标完成后，使用 **3P、挑战模式、Lunatic** 完整走过整局，注意性能及固定位置闪退，专项关注密集音效（用户提及 TH10 曾有此类崩溃）。不得关音效、跳 stage 或用短场 PASS 替代整局结论；记录各关/Boss/转场、实际音频状态、失败帧和日志。若失败须保留原证据并修复复测。

### 11. 正式构建与接线复审

- `release-integration-01` / `iTvLyip97SiDVivI7gDrCRNp` exit 1：Application rAF 外部 C 接口少了 `th11::` 的两处命名空间限定，编译器明确报错；修正后重编。原失败日志/收据保留。顺便修构建 worker 失败收尾，等待其余已启动 compiler 结束后才报告失败，避免下轮与残余进程冲突。
- `release-integration-02` / `WYu29TWTFd6hX1VXmlLzgTd-` exit 0，正式 `-O2 / TH11_DEVELOPMENT_HARNESS=0` Web MP，WASM `22c52faa120bd07c2440fbfbe06159eea0dba3f905534350f729f8c7cb058689`，sourceDigest `9c65ed44db2529088864cefa0f6f1e99475211b7c709c0d154cfeb2a2b2b43f1`。产物连 build.json/loader/wasm 冻结到独立 evidence 目录，不称已验收。
- World02 production journal：`-antv-yT5ZpsKNq92s5bMf14` exit 0，`world-oracle-02-bulk` 使用生产14帧容量 + fastBulkCopy/coalesceRestore，两项都开；仍 **8/150,373/1,124**，peak **657,523 B / 597 blocks**，passed/sourceStable=true。默认配置的 World01 证据保留。
- `network-runtime-02` / `qiHfWIodLg3wOamIR9byDefB` exit 0，含完整 JS 新合同与真实 native decoder 交叉验证，passed/sourceStable=true，新增 native **8/1,202**，WASM 与01相同。
- frontend01 / `tBdA4Tjg_4r12fcj19D6x3AJ` exit 1：命令漏 `--experimental-vm-modules`，managed test 未加载，package 7通过。frontend02 / `gBIBJkXY2-ClTKhosh9eNMkT` 加所需开关后 exit 0，**21/21（14 managed +7 package）**；原失败不删除。
- 独立复审并已修：Replay分数/完成标志使用封帧确认缓存；高刷无due的第一次Restore前执行capture-only早发；达到Replay容量停止新capture/forward但继续纠正确认后正常退休；重开在实际multiplayer_begin处清理cadence且循环优先判代次。这些改动晚于release02，正在构建release03，不能用02冒充最新代码身份。
- Launcher依赖准备 `TAzlrVXS4-8-juzIdQ_5k5Sl` exit 0：web与relay require.resolve/realpath均指新companion/contracts。`ensureClientServicesBuild(force:true)` / `iKDEl1azlqf71JyHHj2qtS7f` exit 0。合同定向 tests 尚待执行。

### 12. 正式03、真实合同与浏览器启动证据

- `release-integration-03` / `IU86FS1_DJBJiiQAK8UVkgG-` 的磁盘收据确认 exit0/passed（轮询遇 FRP404，后独立读盘核对）；WASM `a31d907c2692eef90dbca7e84984c1458fe1242dceec16e16c6d18bd6ace2582`，sourceDigest `eb92f159188707263c4d77cd65f9a6f7b91a7c3462b598afd807c6fd67fdc18f`。上述四项 Application 修复均已包含。
- 正式03独立打包 / `MTKbfSFEGKn66mxYtUutYu5b` exit0，22文件，同一 WASM，输出在 `artifacts/rollback-smoothness/release-integration-03/runtime`。
- companion contract01 / `9px3JpfjF10d53T_Ic1_U04_` 失败：测试复用 relay 留存房间触发每IP房间数规则。改为每个场景自己创建/关闭隔离 relay，没有修改生产上限。contract02 / `JgqGMj5XrFzhtwAakWBnf0G-` 三个命令全 exit0：真实房间五场景、Runtime options 回归、React UI 2/2；passed/sourceStable/generatedStable/dependenciesStable 全 true。失败原件保留。
- browser01 / `3mS3mZgmFaNjlC2qjHyDLkGs` 因默认 Playwright Chromium 未安装而阻断；未下载安装，随后显式使用现有系统 Edge。
- browser02 / `JtNOsv3ZmQMs5IfU8BHIB68I` 失败：Edge已启动、Intel ANGLE D3D11，但初始化资源报 `ERR_CONTENT_LENGTH_MISMATCH` / `Failed to fetch`，Host ready 等待120秒超时。未到真实房间准入/游戏/性能窗口，不能算回滚失效或性能结果；失败URL当时未记录，因此不能直接认定 archive 或 backpressure 是根因。
- 短 runner 已补显式长度/流式文件服务、finish/close/已读字节、requestfailed URL 与初始化error早退。browser03初次调用遇FRP404且磁盘无收据，确认未启动后重试，命令 `8JUIFIO7bRNtMheHY0CKZ6SZ`；结果待核对。
- 独立3P完整流程脚本已落盘，`node --check portable/multiplayer/rollback-full-game.mjs` / `WV0KZqR9jaaDxdXXF8BdUh-I` exit0，仅语法检查，不是整场结果。实际3P挑战Lunatic包含真实1–6关、Ending/Results、完整Replay、渐进OGG、10秒段/2秒进度证据；仍待短场准入和稳定性通过后实跑。

### 13. 真实浏览器：正确性局部成立，性能明确失败

- browser03 / `8JUIFIO7bRNtMheHY0CKZ6SZ` exit1：Edge创建profile报C盘空间不足0x70。`Get-Volume` / `3ZtU2aWefDcRodtHe5bgsSSS` exit0 当时C剩26,554,368B、D剩46,327,275,520B。仅把owned runner TEMP/TMP/TMPDIR改到该次artifacts/browser-temp，没有清理别人的文件或改系统环境。
- browser04 / `bYW0DO03f6mC--3Qhz3ZcOwc` exit1，正式03/1×/RTC/15s墙钟触控：真实room、约460/457次restore、1705/1658历史tick、共同确认frame987导出56字段相同、Pause/Resume均通过；sourceStable=true。性能失败：logic55.863/56.013Hz，native max50.7/71.4ms，present max106.3/152.2ms，>50ms间隔25/21次。不能称完整world browser oracle或流畅验收。
- 新增默认关闭的native16double分阶段wall profile，不在世界快照中；正式04profile / `stGv0DVK9ic7Z2NvZC0ggp52` exit0，WASM `5217ebdac1f1c975601967bbf556982708336e0d975c1a13d19cbc7784d17e75`，打包 `WcdLXNeo7G2BFB21lKItMP8V` exit0，22文件。
- browser05 / `KYYAeYhWEsVXyPRbSBLhSRka` exit1，1×/15s/nativeprofile：合同改为使用真正Launcher helper和真实room字段，fixture seed1234明确标注；共同确认和Pause/Resume仍通过，错误列表为空（owned shutdown事件另记cleanup）。Draw p95约9.6/9.7ms，capture p95约3.8/3.7ms，restore p95<0.9ms，确认输出p95约0.1ms。native max29/40.3ms；present max140.7/167.7ms，CPUyield373/372回调、networkWait0。capture包括前置两级flush，不把整bin归为journal。
- browser06 / `pwDk7g2SQ2-wJDAgdogQtTa4` exit1，4×初始化/data/th11再次ERR_CONTENT_LENGTH_MISMATCH；服务器expected/read均26,610,794B且finish=true，错误发生于finish后约6.026秒，增加owned localhost keepAlive65s进行单变量复测，暂不宣称确定根因。
- browser07 / `9FC3zUStr5xUHjSz3BnLz12n` 磁盘收据完成并failed（nig轮询输出丢失，独立读盘），4×/10s已进入真实游戏，确认/Pause/Resume通过，但logic2.971/2.610Hz，native p50 101.3/109ms、max277/354.5ms，停显1229.8/1730.6ms，性能/运动覆盖失败。sdl_stats窗口uploadBytes/readBytes/resamples全0，约4260/4149batches；Draw p50 44.5/53.4ms。所有坏记录保留，不归因成正常网络等待。
- 已复审全程脚本漏门并收紧实际PCM进展、跨segment停显、6关主BGM、实际保存.rpy、真实回滚/三席运动；新脚本语法 `gJhR9BPyrWH9a-BRG38izQv4` exit0。完整3P挑战Lunatic仍未运行。

### 14. 复制单变量对照、空浏览器校准与全程范围

- `release-integration-05-copy-control` 只把 journal fastBulkCopy 从 true 改 false（coalesceRestore 保持 true），正式构建 `oxo0bopajFWfZa3lj9e-TsEj` 收据 exit0/passed；WASM `e099a1644663ca2763014ed2d61be3104b875a012771d015301ea00f4ec3b784`，打包 `0BZpHW3xYREGyUyAKwcOz6d3` exit0。不是最终推荐配置。
- browser08 / `5P_yhL6Fhs6UCvrismYQct8r`，4×/10s 对照 exit1：确认与 Pause/Resume 通过，logic2.305/2.935Hz，native p50 102.4/118.7ms、max495.8/362.3ms，停显2443.2/1971.7ms。capture p95 42.3/27.2ms，Draw p95 124.9/143.3ms，没有稳定收益。**源文件已恢复 fastBulkCopy=true**，失败证据保留。
- 注意默认构建输出此时仍是05，而当前源代码恢复true。04的独立runtime才是true配置的冻结对照；新增改动必须重新正式构建，不能把默认目录05误贴成当前源码。
- 双独立空白 Edge 校准 `cpu-throttle-calibration.mjs` / `-_Bw1qQeRU0OsaH2OE7Dbo3O` exit0，1×→4×→1×固定JS计算约3→11–13→2.3ms；空rAF中位数约6ms，4×最大8.2/8ms。证明限速确实生效，且空页面未复现百毫秒rAF；不代表WASM、GPU或游戏通过，也不能把07/08坏记录归为正常网络等待。
- 全程合同只读复查：3P Extra合法，难度4/初始stage7，须独立房间与Replay，不能称Lunatic第7关。主线真实1–6关、Ending/Staff/Title结果/实际保存；Extra真实Stage7/紧凑Pause结果/实际保存。挑战禁Bomb，另需普通3P Bomb与密集SFX压力。
- 新增只读 `th11_mp_scene_status` ABI12，直接记录Ending解释器精确staff.msg、active/flags/frames/time及PauseMenu状态，不写模拟状态。用于证明Staff实际推进和Extra结果流程；新增后尚未构建验收。

### 15. 零注入延迟、带符号采样与有限优化候选

- browser09 / `-FVa48mBVD10nWh2hLimrODG` exit1，冻结04、4×、10s、注入delay/jitter均0。sourceStable，确认frame188导出一致，Pause/Resume通过；logic4.910/5.109Hz，native中位103.9/98.1ms、max268.4/282ms，画面窗口停留max594.2/591.8ms。浅回滚仍实际发生29/24次；不能声称这是无回滚基线，但证明确实不只由人为50ms延迟和深预测造成。
- 短runner加入显式 `--cpu-samples`，保留两端独立CDP cpuprofile，且这类运行固定标为diagnostic、不能算性能验收。语法 `ZgrYQQuxRJzmp8KGZUWpSVv0` exit0；browser10 / `bXohLniK7aHMNnHRjgm2agvk` 完成、failed，匿名WASM采样保留。
- 正式06 / `K8vATxuCEe-ZDjCxBP6uXaGC` 链接成功但收据失败：Emscripten对.mjs实际输出 `th11-sdl.mjs.symbols`，最初代码错误读取无.mjs的名字。失败日志及该次链接输出已单独保存（`l7p-ZshrH7WvaJh855dYF61v` exit0），没有沿用默认目录陈旧build.json冒充成功。
- 修正路径后正式07 / `x0EazxN6VpHkZNAukoLUOhwP` exit0，O2/g0、DEV0，WASM `124ec0e5381d95a2ef3f25f642829dcddb3dbced140dbab77d6360479da76be0`，外置符号表hash `fa93b6c1d47c645da996aa28042a62fc69156acbf30a5e13abc516c5cd5c27a9`；打包 `SAD4fvrOvwBTWJu63tCSWpQU` exit0/22文件，scene ABI已包含。符号表仅归档诊断，不作为Runtime payload。
- browser11 / `iSBFnWWELd1Kllb-5ZfLzq6o` 磁盘收据完成failed/sourceStable，冻结07、4×、零注入、CPU采样。确认frame281一致；logic10.45/11.94Hz、停显707.7/255.6ms，仍失败，不能把与09差异当优化收益。
- `summarize-cpu-profile.mjs` / `bzxX-gtxTkjKeF3tdy4XgpXv` exit0：严格核验Runtime WASM、外置map、cpuprofile hash后才对应函数。两端采样热点包含bufferData、Renderer::issue、miniaudio/SDL重采样、th11_mp_game_status、journal Touch/copy。采样窗口含启动/停止额外时间与限速效应，是定位证据，不是可直接加总成每帧CPU成本的验收。
- 确定下一包：Stage每primitive无条件调用renderer.pipeline()会强制flush，即使fog/depth值未变；候选改为只在真实状态变更前flush，保留全部CPU绘制语义及顺序。需普通/MP规范化triangle+pipeline完全对照和真实浏览器复测。
- 另两项有限候选：生产壳每rAF完整世界hash改低频、终止phase仍实时读；ANM仅geometry/tags快照改连续键值对。**active不能直接平铺**：clear按active.begin()销毁，destroy向available入栈，create按LIFO复用，哈希迭代顺序会影响物理槽复用。共享lease、raw Undo顺序、VM和geometry捕获全保留。以上候选此刻尚无性能承诺。
- 历史GPU全跳过不采用：A/B仅部分region清理，普通ANM和变形可采样RT且无静态UV范围保证。完整Draw、目标清理、截图/readback和实际GPUcache语义保持。
- 主线/Extra runner含scene ABI门版本语法 `u9ZSiAE2Md10WiwU6WhUx_EL` exit0；仍未实际整场运行。

### 16. 有限减负组件与全程准入修正

- 生产 JS 完整世界诊断哈希改为首次/新 generation/每1000ms采样，并带实际frame、generation、采样年龄和correcting标记。终止与校准pause使用实时phase，公开gameStatus仍实时完整。`runtime-diagnostics-01` / `QG32aO5if2cYPXWvrB_ZiT0Z` exit0，Node总计10/10，sourceStable=true；覆盖同步退出清缓存等生命周期。不是浏览器收益证据。
- 有限ANM平铺只覆盖geometry/tags，active/available仍保留原容器/顺序。`anm-flat-metadata-01` / `CIpMTI6IrRgxLR_L34PaCPlz` exit0：ordinary1组20检查，MP14组3138检查；新增嵌入body、无geometry的席位标签、两checkpoint直接Undo和lease最终释放用例通过。
- Stage无变化fog/depth请求不再强制flush，保留局部depth初值与所有Draw语义。`anm-stage-batching-01` / `4rcU_p7WG4s4yslIcqmgwPB9` exit0，sourceStable=true；ordinary和MP各6组1228检查。真实旧loop与新loop按顺序展开triangle，逐字节对照vertex、全部PipelineState、texture/target/layout/viewport/四矩阵，以及Stage/VM结果；重复fixture提交96→1、24→1，混合切换18→13。这里只证明提交边界等价与fixture批数，不外推整场性能/实际GPU像素。
- 全程smoke01 / `eS8VWPlpXZJa6quGG4XL_uAf` exit1，正式07/3P/1×，在真实房间start后challenge断言false!=true处失败，尚未进入游戏。原因是runner把challenge放在start消息，而真实relay只从建房URL或设置消息接收。已把challengeMode=1加入create URL，移除start中的无效字段；严格断言不降低，失败收据保留。smoke02尚待运行。
- 下一步：有限改动的完整World复核、正式08/ordinary构建、同条件浏览器性能复测，然后3P挑战Lunatic主线、独立Extra和普通3P Bomb/SFX压力。完整长场仍未运行。

### 17. 正式08复测：有限减负通过正确性，4×仍不合格

- `world-flat-batch-03` / `20JuehfVKqoLmNbZxUbJPyDg` exit0/sourceStable，8cases/150373checks/1124真实Update+Draw。ANM平铺与Stage batching组合没有破坏该组件覆盖范围。
- 正式08 `release-integration-08-batch` / `cEig-KiQU9Xg99li776auGjt` exit0；O2/DEV0，WASM `03d8dcff96746962167b5febe403bff76570e811ab05c9420e394b23889efd8f`，sourceDigest `a849fd9b7e2d9b912b557524426ab49c4005f6d5780c0171636fccbc60c790e9`。打包 `6rWYlJqevIA10Xn2GkrgVlrj` exit0/22文件，包含低频JS诊断。
- browser12 / `u2SxrpI73x4Cn5O9RiVCrAUT` exit1，08/1×/50±5ms/15s/RTC。sourceStable，确认frame987一致、Pause/Resume通过、errors=[]；logic56.255/56.218Hz，native中位5.5/5.8ms、max26.6/24.4ms，present max103/87.6ms。唯一阈值失败为P0 103ms；P95仍45.8/42.5ms，不宣称完全流畅或独立优化百分比。
- browser13 / `eqEmDufC5EZhj2wGjE8UX-5M` exit1：4×请求RTC，真实warmup两端都relay，严格路由断言失败，**没有进入性能测量**。协议错误列表为空，原因未证实；没有以回退结果冒充直连通过。
- browser14 / `RxhyiobHKJbCAz65cAGg9Ich` exit1，08/双端4×/显式relay/50±5ms/10s。sourceStable、共同确认frame253一致和Pause/Resume通过；logic11.207/11.318Hz，native中位34.5/34.3ms、max103/96.2ms，present max622.1/830.7ms，性能失败。不同route不能直接与旧RTC作单变量收益结论。
- browser15 / `HrOHgmIpoihjNq3oG3L8ioXl` exit1，08/仅P0 4×，P1 1×/RTC/10s。sourceStable、共同确认frame201一致；P0 logic5.79Hz、native中位69.5ms/max178.1ms，停显371.2ms；P1约4.70Hz、native max18.8ms、停显431ms。**P0整个测量窗capture/restore/history均为0，昂贵部分在exactTick**。因此本场不能归因为journal快照或深回滚；P0实际回滚与运动覆盖也未通过。下一步定位完整exact事务的游戏/绘制/音频成本，不让快端等待掩盖慢端长回调。
- Fullgame runner仍在修真实输入边沿等待、Extra菜单ready及正常退出清状态后的证据锁存。关于自动Replay覆盖，已读Application保护逻辑：API slot0是自动找空文件，菜单index0实际保存slot1，命名保存会清同路径自动目标；不能把API slot0误认作固定菜单第一槽并声称已发现生产覆盖bug。改为实际菜单产物先验、sync后核对等证据强化。

### 18. exact路径定位、普通构建和3P带BGM短程

- browser16 / `B777OWzHI3DKAh7A53OPN5cj` exit1，为08/单端4×的显式CPU采样诊断，不作性能验收。符号汇总 `4ALNzLzxbOxPVedFd-jpNQwZ` exit0，严格核对08 WASM/map/profile身份。慢端14.287s采样窗中Renderer::issue inclusive约2389ms、GameSession::draw约3710ms，bufferData self750ms、miniaudio线性resampling backend667ms，仍有5531ms未归属(program)。不能把未归属样本直接定成环境问题。
- 普通正式构建 `release-ordinary-08-batch` / `Y49u1fKpkPgSSLqcBi7Miw9H` exit0，WASM `9c0603462ee03bd27812c4c3facb7b4b4b48998b4b30cb69c49c20bc422b4cf1`。验证Stage setter在普通正式源码集可构建；不是普通浏览器/整场通过。
- 全程runner输入边沿/Extra菜单ready/退出证据修订：held与release分别等待CaptureFrame和NextFrame各+2，6s期限；Extra slots等实际12帧；原shell hook前锁存fullyconfirmed phase4，并要求真实Host success exit。`node --check` / `WylQl8i3huBbwueyNrT0TuoN` exit0；终场实际操作尚未覆盖。
- fullgame-smoke02 / `pWG7rBB4dTNr0aj6udIHBb82` exit1，08/3P挑战Lunatic/1×/RTC/真实OGG。已通过真实challenge建房，三端独立RTC进入Stage1，AudioContext running/48k、cue1正音量、PCM推进，实际纠正与移动发生；仍只是短程准备，不是整场。严格性能失败也保留。
- 本次收据12:12:03→12:14:32Z，出现4个request-failed与4个console-error，均为ERR_NETWORK_CHANGED；请求URL属于me.kis.v2.scr.kaspersky-labs.com注入服务。脚本因全局错误门失败，未把它记为游戏崩溃。该证据只说明本轮存在第三方网络错误，不证明其导致已测native/GL长回调。没有禁用、拦截或改动安全软件。正在只读核对超出smoke墙钟的时间归属。
- 新有限候选正在独立验证：WorldUv实例附带完整textureMatrix，以保留表达式和所有绘制边界的方式减少同atlas不同UV矩阵造成的拆批；需真实GPU旧新逐像素oracle。音频仅准备NO_PITCH独立PCM oracle，生产AudioDevice尚未修改；实际异采样率转换必须保留，不以删音效换性能。

### 19. 新候选验证前的再次审查

- 再读跨作架构经验 §7.3、§7.5、§8.1–8.3：保留 authored Draw 的状态写入与 GPU 输出，不以重画推定可删历史提交，不更改浮点表达式或放宽确定性比较。WorldUv 候选仅扩大实例数据，仍需实际 GPU 逐像素核对。
- browser16 注入脚本归因只读命令 `BCVzE8ba1w4BjJDWrVJQC6PJ` exit0：P0 窗口14286.998ms，Kaspersky URL节点 self20.325ms/inclusive69.745ms；P1 窗口11445.498ms，self8.491ms/inclusive11.56ms。该显式调用树不支持把此前数秒 native/GL 耗时归因注入脚本；未归属(program)和系统层影响仍未知。
- smoke02 首次逐端 segment 采集窗口实际10.138/17.621/35.822秒，失败时三份快照各约4.46–4.96MB。大对象经浏览器自动化逐字段序列化/传输是需要单独核验的观察开销，不能把整个148.963秒收据称为20秒有效测试，也不能自动扣除所有坏帧。拟改为有界批次、浏览器内 JSON 字符串传输并记录采证时间；原始记录及严格性能阈值保留。
- 用户要求最终完整3P挑战Lunatic通关，BGM/SFX开启，逐关观察性能与固定崩溃位置。仍需独立 Extra，以及普通3P Bomb压力（挑战本身屏蔽Bomb）；此刻尚无全关通关证据。

### 20. WorldUv真实GPU与NO_PITCH组件验证通过

- `texture-instance-gpu-01` / `HFfM558B0_zags0MBQPgPWcI` exit0，15场景，sourceStable/buildStable=true。旧Renderer/Shader来自固定HEAD并核对正式08前像；同一真实Renderer fixture分别编译O2，硬件WebGL2逐像素BGRA零容差通过。图集/完整textureMatrix 8→1 batch，透明与fog/depth叠加12→1，1023/1024/1025实例分别1/1/2；原始quad差异、材质/target/clear/read/CPU atlas更新、generic shader与VAO切换均覆盖。只证明这些组件图像与批数，不是全游戏像素或4×收益。
- `audio-pitch-bypass-01` / `Qms_09BsKJSw1mqYDGC33vQf` exit0，62cases/1881506checks，sourceStable=true。实际46WAV/56ID、seek/stop/restart、自然结束、56声部、真实BGM起始/loop seam、48000合成输入真实rate转换均通过零容差模型；三项压力/loop/异rate例均有非零能量硬门，原始PCM保留。同rate存在已明确的一帧PCM历史差异及旧末尾未排出样本，不能称逐位一致。
- 音频56声部组件mix耗时本次36.3762→23.026ms，只是WASI组件诊断，不能外推浏览器或全游戏比例。通过后root仅给 `AudioDevice::Impl::attach` 增加 `MA_SOUND_FLAG_NO_PITCH`；异rate miniaudio自动保留真正重采样，音量/pan/音效数量与确认后发布规则不改。
- 正式09开始构建：`release-integration-09-instance-audio` / `LLZqSm9DpbyV7J4BsttZTc5N`，生产源码冻结；构建结果及真实浏览器收益尚待核验。全程runner正在单独修采证传输，不属于该构建观察源。

### 21. 正式09浏览器复测：减少了工作量，仍未跨过流畅门槛

- 正式09 `LLZqSm9DpbyV7J4BsttZTc5N` exit0，137TU/O2/DEV0，WASM `8febab07604e03d362e13cfd05f28f550bb1917caf4877c82a08905dbc2afc02`，sourceDigest `d1d95d4a2b9bf5832f73ec6aebc5558d27246802b9373d6175da6d6a816dc64b`；打包 `U7CiAOmbEMi5V4ob8uWuoxeR` exit0/22文件。
- browser17 / `VqoW0wIv7ZK4QqocaG7eXpO7` exit1：09/双端1×/RTC/50±5ms/15s，sourceStable、errors=[]、共同确认frame936一致、Pause/Resume通过。logic52.927/53.039Hz，native中位6.4/5.5ms、max56.5/42.9ms，停显228/239.6ms；严格性能失败。Draw中位2.6/2.5ms、audioPump P95 1.1/1.2ms，但长尾并未消失，不能仅用常见成本下降宣称整局改善。
- browser18 / `SEpcaPii2Fou2mxs8sMdTGLn` exit1：09/双端4×/显式relay/50±5ms/10s，sourceStable、errors=[]、共同确认frame270一致、Pause/Resume通过。logic12.640/12.923Hz，native中位26/25.4ms、max80/101.7ms，停显618.1/695.9ms；仍不合格。
- 与相同设置browser14相比，本次Draw P95由38.7/43.8降至25.1/26.6ms，audioPump P95由11.4/11.2降至5.6/5.7ms。实测batches/calls分别13162/29105、13308/29583，旧版18873/26185、20461/28517。路径确实减少了拆批，逻辑进度与历史次数不同，单场比较不作稳定收益百分比承诺。frame停留长尾仍须解决。
- 三人全程runner的采证修订尚在进行，正式09未开始完整主线或Extra通关。仍不得标记可交付。

### 22. 三人采证修订实际生效，短程仍非性能通过

- 普通正式09 `SFZ5cJVh27RslHGI1RP6tSif` exit0，125TU，WASM `c8ee4be408ac46c90b17f3d7494c3e61ebd6f03015a920e416f13ed9c31ec32a`。不是普通浏览器/整场验收。
- 全程runner语法 `T66o9yZzuODgi0HbPz3WNwGP`、专用有界后台job语法 `hi8Iwu0n0pjYSaLG6YVPCl_e` 均exit0。job固定运行本runner、fresh目录、显式PID与状态，最多duration+360秒，不自动重试。
- `fullgame-job-smoke-01` 由 `pCmts7n4mITUSwblXFIXfNAX` 启动，worker53744/runner39600，最终退出0/status=smoke-complete；仅表示20秒准备场完成，passed=false。冻结09/3P/挑战Lunatic/RTC/BGM/SFX，run receipt sourceStable=true，实际20.35秒测量；总55.452秒，加job收尾共58.58秒。三端第一段均10.034–10.040秒，旧10/18/36秒串行窗口膨胀已在本次消除。
- 新计时发现仍有采证开销：每段约1.76–2.33MB，browser stringify28–32ms、UTF8计数18–31ms，checker请求至返回261–422ms。第一段取证前停显149.1/152.4/197.1ms；整段聚合最大208.4/267.4/215.2ms，严格性能仍失败。下一步考虑有界增量块，而不是先整段stringify再切字符串；原始记录和失败门不能删。
- run cleanup-start=52055ms，随后52177/52179ms出现两条由本次关闭浏览器引起的RTC断连console，原错误仍全部保存。当前报告的blocking标签与已结束验证窗口需明确区分；不能据此把游玩前段错误或未知来源错误一概忽略。
- 下一构建准备两个窄GPU减负（省未使用实例uniform、缓存上一份完全相同80B quad）与两个独立只读观察点（各席Bomb状态、实际SFX start计数），用于普通3P密集Bomb验证。正式09包不变，生产新源尚未构建，下一次测试前必须重新冻结。

### 23. 正式10：减少冗余GPU上传与补齐音效观察

- `texture-instance-gpu-02` / `0FhuXo6W5rJuluztHctWWlJy` exit0：保留原15场景并增加uniform缓存返回、新VAO与Screen上传失效场景，共17个实际GPU零容差场景。实例world/texture/factor uniform不再无效上传，缓存保持实际GL值；Web只复用上一实际vertices上传的完全相同80B WorldUv quad，其他vertices上传失效，命中仍正确绑定。未改为BufferSubData，没有删历史绘制。
- 新增独立只读 `th11_mp_bomb_status` 24word与 `th11_sfx_statistics` 120word。Bomb只在完整前沿有效，纠正中无效字段不能解释为Bomb结束；SFX统计原有单次ma_sound_start结果、固定56声部，既不改变发布规则也不证明可听输出。
- 正式10 `release-integration-10-upload-cache` / `U5dKs9XsJVi53kO72-QspQwx` exit0，137TU/O2/DEV0，WASM `a8f2fdb9a0160f68f06ae31e7b1e9be64559d877357e26f8adc1dab2c6bf4a60`，2878813字节。打包 `OudQVtTNFwiHOgxCOo7s9pRi` exit0/22文件。开始browser19实际复测，结果未定。
- 全程采证分块与teardown精确归因仍在独立runner修订，完整3P挑战Lunatic/Extra/Bomb尚未开始，不把组件通过或构建成功当交付。

### 24. 正式10真实短程：1×通过本场门槛，4×仍失败

- browser19 / `2ZMND5I_JUqHiIhvbekQ8E45` exit0：2P/1×/RTC/50±5ms/15秒、sourceStable、errors=[]；共同确认frame1030的56字段相等，Pause/Resume通过。logic59.361/59.434Hz；native中位2.5/2.2、max18.6/17.9ms，最大present59/55.4ms、frontier59/77.9ms。实际restore708/603、history2655/2243，非关闭回滚取得的数字。仅第一关短场，非全世界浏览器oracle或全游戏保证。
- browser20 / `JqM0XT2iUHb-nzLXXtVFSFin` exit1：2P/双4×/relay/50±5ms/10秒；sourceStable、errors=[]、共同确认frame236相同。logic10.272/9.715Hz；native中位30.4/32.5、max130.1/98.5ms，present911.2/461.6ms。Draw P95 33.6/35.6，audioPump max72.6/79ms，仍为严格性能失败；不把它归因未经证实的环境变化，也不因1×通过而覆盖。
- 全程分块runner `RFfeOy4jbKm9jsMgXwiC2n3B`语法exit0；`fullgame-job-smoke-02` / `2ecm7bM6v8hm1SPiGUsyxLlk`正式10/3P/Lunatic挑战/RTC/20秒准备场已启动。新段严格每块≤64records且≤64KiB，完整记录、序号/数量校验，disk ACK前保留浏览器原快照；清理阶段仅精确来源与对应已开始关闭的席位RTC消息可标为teardown，最终状态在清理后确定。

### 25. 三人分块实测与首个完整主线运行

- `fullgame-job-smoke-02`（worker65172/runner28248）已结束：13:05:55→13:06:55Z，正式10，sourceStable，6个段均complete、snapshotErrors=[]；单块serialize最大2.4–9.8ms，每段完整传输2.46–4.47秒，所有原始记录保留。三端native max24.6/25.4/25.9ms、present170.5/189.8/133ms，性能仍失败，不能把块传输优化当成游戏流畅通过。
- 该smoke验证窗口结束后，关闭seat0触发seat1报P1 RTC错误；seat1尚未开始browser.close就发生seat2报P2 disconnected。严格目标匹配只归类第一条为owned teardown，第二条保持blocking，最终job failed而不是假pass。raw消息、shell来源、关闭动作时间均保留。需要检查正常关闭的退房级联，不能放宽为“cleanup中的所有网络错都忽略”。
- 为避免继续只跑准备场，已于13:07:49Z启动 `fullgame-lunatic-01` / `9D3phoY99zwnBko0rAc4b_s7`，worker72964/runner49216。正式10/3P/Lunatic挑战/1×/RTC/50±5ms/真实BGM与SFX、90分钟上限、自然stage1–6与结束流程；已知性能失败继续记录，不会用通关覆盖。当前runner/helper/生产/launcher保持冻结，root不同时运行任何其他构建测试。
- 普通3P Bomb场将先准备独立待审patch，完整主线当前运行期间不改被观察源码。结束后才按真实结果决定修正与执行，不提前声明全程通过。

### 26. 全程运行期间的有限只读审查

- GPU02收据再次核实：17/17 exact0，实际比较41943040字节BGRA，sourceStable/buildStable=true。新VAO与失效路径的buffer replacement/byte精确门均成立。实例stride增加，总上传字节不保证比最初基线更少；只承诺已验证的拆批/重复调用减少。
- browser20最坏seat1 callback98.5ms，其中audioPump79ms，historyTicks=0/forwardTicks=1；seat0 callback130.1ms，其中audio72.6ms。该短场musicIncluded=false，不可归因为OGG解码。目标50±5ms的页内发送定时器在4×下actual中位120.5/129.3ms，lateness P95 136.3/130.4ms，不能当成不受CPU影响的纯50ms网络，也不能直接归罪独立relay。
- 已准备但未应用 `artifacts/rollback-smoothness/prepared-audio-timing.patch`：独立默认off的18double，分mix/SDL_Put/queue读取，rAF前reset并累计exactTick内pump。原audio9/rollback16不变；它只是未来定位工具，不是优化收益。当前完整主线的源码未改。
- 历史screen GPU省略方向在此止步：普通ANM未发现screen1采样、正常TargetScreen会全屏color+depth清理，但pause/result创建capture_requested；Application在下一次Draw前从screen1 resample。回滚barrier可EndCorrection(true)提前结束，此前中间history屏幕可能成为下一confirmed截图来源，保留原frontier-1不足。screen/A/B还共用depth2，不能把正常清理前提扩到失败/早退。尚无事务与完整像素证明，不能直接historical&&target==1 skip，更不能从Draw后状态随意重画补救。
- 主线至13:27Z已自然运行到第三关Boss，无game/audio latch错误；已发现第三关最大present约823.7–992.5ms，完整原始段保留，严格性能明确失败，未因自然通关进度而忽略。

### 27. 第五关进度、容量审查与待应用观察补丁

- `fullgame-lunatic-01` 于13:46:56Z自然运行至第五关约96800帧；三端按序访问1–5，native/audio latch/errorCount均0，WASM reserved buffer分别400949248/400949248/334102528字节。未通关、未覆盖Extra/Bomb；第三关近1秒停显仍是失败，不能因未崩溃改判流畅。
- 只读内存审查确认 `Reset({14,32*1024*1024,65536,true,true})` 的32MiB是**每帧槽maxBytesPerFrame**，14槽arena理论上限448MiB，非预分配、非全部回滚总内存。DiscardBefore/UndoTo保留backing capacity，Clear才销毁；关卡barrier不Clear。Enemy/ECL/ANM/deformation也保留可复用高水位。未发现确认后typed快照永久强持有旧StageResources的明确路径：先DiscardBefore，再world/texture.reset，旧关retire_previous→Graphics.unload→textures/resources.erase→previous_stage.reset。现有buffer增长不足以判泄漏，后续如测内存需同时量化各槽capacity、pool活跃/物理数和allocator，而不能用buffer byteLength代替活跃内存。
- 两个全程runner离线patch已准备、独立审阅，尚未应用：`prepared-fullgame-diagnostics-cleanup.patch`用于低频SFX120/关闭前终态计数与实际transport close证据；`prepared-bomb-mode.patch`以其为前置，普通Lunatic3P真实吸取Power/触控Bomb三Capture帧/确认Pause/Replay按钮/自然结束。静态审查不是可达性通过，首个完整主线不能事后补称SFX120全程零失败。
- 当前生产/runner/helper/launcher继续冻结，root没有同时启动其他命令负载。准备中的audio18和短场采样都仅为诊断，不是已实现性能收益。

### 28. 首轮完整主线在第六关中断，失败已保留

- `fullgame-lunatic-01` 于13:55:34Z结束，job13:55:37Z terminal/exit1/非timeout。三端自然访问1–6，第六关开始frame116235，末次健康状态117216/117216/117217；随后seat1浏览器目标关闭，主错误 `page.evaluate: Target page, context or browser has been closed`。未走完第六关，Ending/Staff/Results/保存均未覆盖。不能称完整通关或已确认音效崩溃；原生/audio9最后均无错，page-crash未捕获，但没有进程退出原因，仍属未解释的严重失败。
- 保留555完整segment（185×3）、两端failure snapshot、job/receipt/stdout/stderr。原场没到sourceStable最终门；额外只读核对 `postfailure-identity.json` 的12个有hash文件全部一致（`1umS-a1XaHM0YrkMKBkCNO0-` exit0），不回写原failed收据，也不把HEAD/status记录当完整源校验。全场实际是3独立**headless** Edge+硬件GPU；不是人工可见窗口/肉眼逐关检查。
- 离线汇总 `summarize-fullgame-stalls.mjs` 语法 `I_ii3M7Uxr3k1P0wZBVnTd3D` exit0，执行 `e8WcZYLBVr72lKF67oO9V_OU` exit0：555段hash与逐席链均通过，18种present/frontier超过1秒的观测区间（9个present），产物在 `fullgame-lunatic-01-stalls-01`。不扣观察器开销、不删除失败。
- 13:43:24–29Z第五关约5.1–5.4秒停显，三端仍有486/520/525个rAF，networkWait474/508/513，预测顶到12帧，最大回调空隙120/69/50ms。该组是信息缺口等待，不能写成5秒rAF阻断；实际为何输入迟补仍待packet级定位。另13:45:25约1秒和13:45:57–13:46:00约2.3秒，确有约1/2.23秒回调间隙，须分别调查。当前full原始记录无audio/draw/capture细分profile，不能跨场补数。
- Windows Application Error/WER对应时段未查到msedge/node匹配记录（`iBg1GPbuVAyU_Ts_XOnAE4X_` exit0）；System资源耗尽2004查询明确NoMatchingEventsFound（`BloE1uKiCjUEOmDw-IjU-9Gs` exit0）。缺日志不等于没崩溃，也不能直接定性OOM/环境。
- 只读审查未发现本场启用host.observe逐帧Map或永久保留全部touch ACK/transport payload。host.events正常约3条/秒且small两次filter，确有随时间增长的测试器开销；不能据此判定它导致browser关闭。实际raf/transport/motion、touch、pending payload及段ACK有界。
- 已应用窄SFX120/精确transport cleanup补丁，语法 `6mhsWPBNXkkTCDTWBbjnpIfm` exit0。正式10生产包不变，`fullgame-sfx-smoke-03` / `4o6pWz-8amzvJxXZDvDCQncM` 20秒准备场正在串行验证。Bomb、audio18与audio短场、browser生命周期patch均尚未应用，不能报告其已测通过。

### 29. SFX 观察、普通 3P Bomb 与真实菜单等待

- `fullgame-sfx-smoke-03` 已终止：smokeSetupPassed/sourceStable=true、errors=[]；每席87次SFX观察、成功start增量886、失败/非法handle均0。三席transport实际close后再close browser，清理无阻断错误。native max9/9.9/9ms，但present235.3/207.9/204ms，9项性能失败，status不能标完整验收通过。
- 已应用普通Bomb模式与有界浏览器生命周期证据，保留formal10生产包。生命周期包括每席page/context/browser close与page crash，只有已签发同席owned close才标正常；首次异常锁存，不能由cleanup覆盖。应用后语法检查均通过。
- `bomb-3p-lunatic-01` / `hVyhvNDyoGuKOGWcM438YTlT` exit1：真实Power收集、三席Bomb overlap、frame377同帧已确认game56一致、原生Replay每席三连续Bomb帧与SFX增量已覆盖；失败发生在Resume过早断言。实际frame385/Pause state34，原生错误空，Escape已被接受；源码要求关闭动画timer.current>11。保留此失败，不能称游戏吞键或崩溃。
- runner仅修为一次Escape后在原总预算内有界等待真实34→Stage，不重复Escape、不改变输入、确认Pause/Replay/自然结束门。语法 `TMip0qOm6kQ7rU-sNwEQgvdC` exit0。
- `bomb-3p-lunatic-02` / `OthIapvnI1T1c3d8XErzRkk4` 于14:24:00–28Z完成。普通3P/Lunatic/1×/RTC/50±5ms/选择0,2,4，**bombStress.completed=true、sourceStable=true、errors=[]**；真实34→Stage等待约790ms，Stage恢复frame404，之后254逻辑帧；两次确认Pause、原生Replay三个按钮脉冲、同帧完整状态和confirmed inactive elapsed>=181均通过。不是Challenge通关，也未覆盖selection1的200帧分支。native max5.4/5.8/8.3ms；present max107.3/70.1/93.3ms，严格性能仍失败，exit1和passed=false原样保留。
- 第五关约5秒信息停滞的补充raw证据：13:43:23–29Z三席received分别固定424547/424647/424614，sent增长约526/541/534，repair也持续增长，ignored=0、IsOpen=1；CanStart在约26.38秒降0，约29.23–.30恢复。证明不是P12停止重发；具体链路/ICE原因尚缺每peer包头和恢复阶段证据。另一方面两个短些的大卡，rAF请求至进入确为1006.7/2249.8ms，stamp→entry仅1.4/4ms；不能混为同一原因。

### 30. 音频细分诊断候选开始构建

- 已重读跨作品架构、performance-rendering和audio playbook；root及input agent复审后应用`prepared-audio-timing.patch`与`prepared-browser-audio-profile.patch`。独立18double默认OFF；OFF保留原mix/refill/error循环，无新时钟/队列查询。ON分mix、SDL_Put、queue，并覆盖exactTick内pump；getter(-1)纯读，audio9/rollback16保持不变。它是定位工具，不是性能收益。
- JS诊断开关独立，严格ABI与callback序列，读取计入observer；开启后明确diagnosticOnly，不允许当无观察器性能验收。语法 `NNB2Y3Wpu8OJACtDr8eEy1fG` exit0。
- formal11 `release-integration-11-audio-profile` / `L21wMgB9udHm0DivVF0RRb5a` 正在正式O2/DEV0/最多4workers串行构建。尚未取得构建结果或新的性能数据。
- RAF有界服务候选仍只在prepared文件中，默认OFF设计，尚未接入/编译/实测；不把TH10受控收益移称TH11已修复。

### 31. 正式11诊断结果与 Extra 首轮启动

- formal11 / `L21wMgB9udHm0DivVF0RRb5a` exit0，137TU/O2/DEV0/4workers；WASM `16c8f014a96784b066c7be039d9b739d8b05271bbd41d9f08414e2995d59ab7b`，2881149字节，sourceDigest `a75005f10178d9bb0b65fe53f21353cce544cf60a9d8d9c38ef1d4a43f3c5e6b`。打包 `NnIn6fLH0tVMs21Hpvtn9rdX` exit0/22文件；fonts只读取正式10已冻结资源。
- browser21 / `Z6UzH7DhRNhT9CCG_HAlQlRI` exit1：10秒2P/4×/relay/stable/native+audio profile，sourceStable、errors=[]、audioProfileCollectionComplete=true。audio采样894/1011、mix/put失败0；pump/mix P95=2/2ms,max13/13ms，Put和queue记录max1ms。逻辑51.18/51.36Hz，native max20.1/22.5ms，present128.3/132.7ms。诊断资格和两端性能门均保留失败。
- browser22 / `Mlg2-YYErnA2iMa4X6gkvHyJ` exit1：同条件45秒扩大观察，4300/4445个完整audio样本、sourceStable、errors=[]、mix/put失败0；pump/mix P95=1/1ms,max11/10ms，Put/queue记录max1ms。逻辑52.98/52.88Hz，native max24.5/25.9ms，present128.8/128.3ms，callback最大29.6/37.9ms。此次最长native分别含draw16.1/13.3ms，与audio长峰不是同一个样本。旧browser20的72.6/79ms音频长回调没有复现；**仅增加观察器而未改播放策略，不能把跨场改善归功于优化，更不能抹掉旧失败。** 目前不进行缺少证据支持的SFX预解码改写。
- rAF prepared策略已由root全文审查，`08V9WJOIVGABYIPdvSV0Fclz` exit0，19组4364项；范围为纯策略+真实FrameSchedule，非Application/浏览器，输入部分仍只是合同模型。接入仅在离线patch准备阶段，生产未启用。
- fullgame helper增加本次runner限定的`DEBUG=pw:browser`有界stderr采证，2MiB头+6MiB环形尾，metadata保存total/discarded/complete/退出身份；正常收尾等stdio close，不改全局环境。语法`DTZM6chpqRY3eidWW_Rm0xe-` exit0。
- `fullgame-extra-01` / `pPEfGsSQ8wfpBQn03LidM8eF`于14:34:39Z启动，worker34048。正式11、3P Challenge **Extra难度4 / Stage7**、1×/RTC/50±5ms/真实BGM/SFX，45分钟预算。Extra不是Lunatic；它补充主线Lunatic覆盖。root只让这一场运行，没有并行构建/测试，生产/runner/helper/launcher冻结。

## 当前未完成项

Application/JS 已有正式 O2 SDL/Web 构建，短场RTC/relay及普通3P Bomb有明确状态一致性证据；不外推全游戏。
生产 fastBulkCopy/coalesceRestore 已有真实World组件复核。Launcher P12合同已通过，严格4×性能仍失败。
完整3P Challenge Lunatic首场进入第六关后因一端browser关闭失败，原因未明；第六关结束、Ending/Staff/Results/菜单Replay保存及Extra均未完成。
普通3P Bomb自然结束已覆盖，音效大量触发和长场生命周期仍须继续验证。当前不能宣称完整性能或交付通过。
