# th11 高刷新呈现 | 2026-10-24

固定 60 Hz 逻辑与显示呈现分别调度。连续 ANM 字段和相机端点在绘制副本上插值，出生、脚本切换、计时回退和场景切换沿各自边界处理。生产、诊断与 Lab 构建使用独立输出和准入条件。

## 当前普通版验证

| 项目 | 结果与证据 |
| --- | --- |
| 来源 | PR #133 原头 `775adbb78`，接入主线后追加修复；接回本地 `124e89115` 修复历史，保留原提交父链 |
| 工具链 | Windows、Node 24、Emscripten 6.0.9、Chromium |
| 普通版 | 完整构建与生产打包通过；启用 thprac；WASM `ef41874fd7eac7557a0309faee01afc87093958ba54f57bb21f9760ce92c8428` |
| Lab | 完整构建通过；WASM `3fb3904f5858691603a47754e5af7bb9f9ff3e49bb8c56fbbce445087cb6211b`；生产打包拒绝此诊断构建 |
| 功能配置 | 实际 Application.cpp 在 thprac 关闭、开启和 Lab 三种配置中编译通过 |
| CPU 契约 | ANM、固定逻辑／显示节奏、相机不连续性、原生重基标记四组检查通过 |
| 位移与打包 | 24 项测试通过，覆盖 300／512／1024 像素旧端点、突变和恒定采样，以及真实打包器的写入前拒绝 |
| 浏览器 | 自备原版素材，Demo 0 推进 300 tick，10 个采样窗口，220 次对象检查，位移告警 0，页面错误 0 |
| CI | th11 普通版构建后执行源码契约、CPU 契约、三种编译配置和准入回归 |

## 复现入口

```text
node ports/th11/portable/check-presentation-lab.mjs
node ports/th11/portable/check-high-refresh-contract.mjs
node ports/th11/portable/check-presentation-purity.mjs
node ports/th11/portable/check-build-modes.mjs
node ports/th11/portable/check-presentation.mjs
node --test ports/th11/portable/production-build.test.mjs ports/th11/portable/presentation-lab/flicker.test.mjs
```

构建与素材参数见[开发指南](../DEVELOPMENT.md)，实际场景采样见[巡检入口](../../portable/presentation-lab/CAMPAIGN.md)。本地构建回执位于 `artifacts/validation/source-builds/th11-normal.json`。

## 呈现与证据

关卡中的 fps 读数对应呈现次数，冻结场景和 60 Hz 限制沿逻辑帧率显示。插值观察覆盖实际接入的 ANM 字段与提交几何，状态拥有者以观察器声明的覆盖范围核对。额外绘制的状态摘要与首帧、音频、网络专项分别记录。

普通版与 MP 沿独立源码分支接收通用呈现修改，MP 的联机、回滚、旁观和多人构建所有者在对应目录维护。接收关系与处理范围登记到仓库 `docs/adaptation/port-sync.json`。

## MP 接收与逻辑时钟

MP 接收共享字段处理、诊断与准入回归，联机前进使用独立 `SimulationCadence`，保留每回调最多四 tick、100ms 债务上限和完成后扣账规则。联机驱动继续提交权威前沿，插值缓存采集在该模式下关闭；普通配置与 Lab 驱动分别验证高刷呈现。

- MP 完整构建通过，WASM `834ccb907798c777c3f71053c8933076314221265d4dda0b60c711a7f76be0b8`。
- MP 的三种宿主配置编译、六组呈现／时钟 CPU 契约和 31 项位移／生产包隔离回归通过。
- 独立网络时钟通过 10 组、26,288 项账本检查，包含普通版显示时钟与网络追帧时钟的分离。
- ANM 回滚夹具覆盖 ordinary 与 multiplayer 两条通道，校验注册表、VM、脚本文字和恢复事务。

复现入口位于 `ports/th11-mp/portable/multiplayer/frame-schedule.mjs` 与 `anm-rollback-test.mjs`，通过 `--output` 选择新的本地回执目录。
