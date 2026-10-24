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

MP 的网络前进与显示呈现沿两个时钟运行。`SimulationCadence` 保留每回调最多四 tick、100ms 债务上限和完成后扣账规则；`PresentationSchedule` 从完整原生事务的前后端点生成额外显示帧。缺输入时显示到达当前端点后保持，纠正期间保留画面，恢复和场景边界重新建立插值对。

- MP 完整构建与生产打包通过，WASM `18ae09b5a7d4c478c8d000281291fd5d9033a97e11f4dac77fb5cfe89af33ad5`。
- 三种宿主配置编译、六组呈现／时钟 CPU 契约和 31 项位移／生产包隔离回归通过。显示检查覆盖 60／120／144 Hz 节奏、等待、追帧债务、恢复、代数、换关和定位边界。
- 独立网络时钟通过 10 组、26,288 项账本检查，包含普通版显示时钟与网络追帧时钟的分离。
- 完整原生世界对照通过 43 组场景、1,335,299 项检查和 2,182 次真实 Update＋Draw。2P／3P 在五个插值系数下重复绘制，所有已观察世界拥有者保持一致；当前端点的完整几何命令与原生绘制相同。场景包括错误预测后恢复、六种 Bomb、幽灵救援、暂停屏障和连续换关。
- Chromium 双进程通过真实 WebRTC 对局。一端使用 120 Hz 模拟回调并启用高刷，一端限制到 60 Hz。八秒采样中逻辑频率分别为 59.91 和 60.02 Hz，高刷端记录 182 次纯显示绘制，限制端为 0。该计数剔除所有逻辑前进、历史重演和纠正提交。
- 双端分别执行 341／430 次恢复和 1,158／1,800 次历史重演，在第 613 帧核对确认状态一致，实际输入导出的 Replay 与纠正后的实时状态一致。暂停与恢复操作通过。本项使用模拟回调和 SwiftShader，设备显示验收沿 DISPLAY-02 单独记录。

## MP 呈现所有权

插值样本以所属时钟和对象身份分开保存。绘制副本保留席位归属，角色透明度与队友遮挡跟随同一原生拥有者。低速圈从当前 ANM 资源、活动对象与席位标签重建，包含淡出对象；回滚撤销的未来对象不会进入新一轮显示。额外场景绘制保护 VM、实例标记、可见性统计和相机窗口，原生事务继续负责可写状态。

## MP 复现入口

```text
node ports/th11-mp/portable/check-presentation.mjs
node ports/th11-mp/portable/multiplayer/world-rollback-test.mjs --output <fresh-output>
node ports/th11-mp/portable/multiplayer/rollback-browser-acceptance.mjs --launcher-root launcher --display-hz 120 --compare-display --duration 8 --replay-oracle --output <fresh-output>
```

浏览器命令使用已打包的 MP Runtime，通过 `TH11_TEST_ARCHIVE`、`TH11_TEST_FONT` 和 `TH11_TEST_UNICODE_FONT` 提供本地输入。完整世界夹具使用 WASI SDK 34；网络时钟专项入口为 `portable/multiplayer/frame-schedule.mjs`。本轮私有回执保存在 `.codex-tmp/th11-mp-high-refresh/world8/receipt.json` 和 `browser120-2/receipt.json`。
