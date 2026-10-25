# STG 适配流程

从一部作品、一个构建模式开始，用当前产物完成启动、操作、持久化与退出，再扩展到该作的全部适用编号。共用驱动执行操作与断言，作品适配器提供真实状态、导航和规则差异。

结果分别记录机器证据与整项完成状态。`machineComplete` 经过原始证据复算后表示自动部分完成，`complete` 表示该编号的全部适用要求完成；需要设备或视觉审查的编号继续关联集中审查材料。

[适配入口](README.md) | [验收方法](test-methods.md) | [测试命令](../../tests/README.md)

## 范围与责任

当前 STG 流程登记 th06、th07、th08、th09、th10、th11、th15、th20。普通版与 MP 分别构建、打包和验收；th09 的对战规则与其他作品的合作规则分别检查。支持模式、练习、MIDI 和语言功能以[产品目录](../../common/contracts/product-catalog.mjs)及[构建配方](../../launcher/config/runtime-builds.json)为准。

th075 声明 `inputProfile: fighting`，格斗玩法使用本作的对局、动作与结束条件。构建、包身份、协议、文件持久化和共享控件沿用对应共用契约，玩法驱动按格斗语义接入。STG 的低速、Deathbomb、落率与关卡导航属于本章的玩法范围。

| 位置 | 负责内容 | 接入要求 |
| --- | --- | --- |
| `common/` 与 Launcher | 共享输入、资源、存储、协议和控件 | 修改共享能力时核对全部消费者 |
| 本作 Shell 与原生实现 | 配置消费、原生输入、生命周期与文件操作 | 生产入口执行用户操作，观察接口只复制真实状态 |
| [原生状态映射](../../tools/ports/runtime-observations.mjs) | 阶段、已消费输入、逻辑帧、坐标和本机席位 | 字段注明单位、有效位与采样边界 |
| [主适配器登记器](../../tests/adaptation/adapters/registry.mjs) 与 `adapters/titles/<game>.mjs` | Runtime 挂载名、观察类型、流程、时钟、运动、受控场景、音频、语言、练习与 Replay 能力 | 普通版和 MP 分别声明；观察记录来源、单位和消费边界 |
| [作品流程适配器](../../tests/adaptation/adapters/title-flow.mjs) | 标题导航、玩法就绪和暂停重开 | 按本作实际菜单与确认步骤实现，普通版与 MP 各自登记 |
| [诊断登记器](../../tests/adaptation/adapters/diagnostics.mjs) | Presentation、quick 与 daily lane、捕获器、样本和终点语义 | `lanes` 显式列出可执行能力；未登记 lane 在计划中 blocked |
| [练习编号与观察](../../tools/ports/practice-observations.mjs) | 产品能力对应的 PRACTICE 编号、菜单与状态结果 | `practice` 声明本作快照 ABI 与独立参数预期，共享判据比较配置、已接受配置和实际应用值 |
| Replay 驱动与作品语义 | `tools/ports/replay-browser-semantics.mjs`、`tests/adaptation/adapters/replay.mjs`、标题 Replay 模块 | 格式、阶段输入和自然终点依据来自本作；通用驱动不替作品猜结束状态 |
| [场景目录](../../tests/adaptation/catalog.mjs) | 作品、变体、编号、输入和任务选择 | 缺驱动或输入时给出 blocked，已声明不支持的组合给出不适用 |
| 本作 `docs/DEVELOPMENT.md` | 资源准备、原版样本与特殊规则 | 维护者能据此准备相同输入并复跑 |

## 按阶段接入

每个阶段先完成当前模式，再核对对应模式。阶段结果绑定本次源码、构建计划、Runtime 和输入身份。

共享基础先由一作贯通以下各阶段，核对全部适用编号的方法与证据，再接入其他作品。自动检查和人工审查在同一张[逐项审查表](record-template.md#逐项审查表)汇总。高刷新设备上的视觉观察使用 Presentation Lab，其他功能继续执行各自的行为断言。

| 阶段 | 实现与操作 | 通过依据 | 入口 |
| --- | --- | --- | --- |
| 1. 登记与构建 | 登记源码归属、产品、构建与打包配方，执行正式构建和打包 | 当前模式有完整来源收据，WASM、加载器、入口与包字节一致 | [BUILD 方法](build.md)、`build-runtime.mjs`、`package-runtime.mjs` |
| 2. 启动与生命周期 | 经真实 Shell 配置、启动、暂停、重开、退出 | 协议身份与代次正确，首帧已呈现，暂停停逻辑帧，重开后继续推进 | `runtime` |
| 3. 输入与触控 | 从 Launcher 改键、手柄映射及触控入口执行按下、组合、释放和取消 | 原生已消费输入与本机角色效果一致，释放与切换后无残留 | `desktop-input`、`touch-actions`、`desktop-background` 与 [INPUT／TOUCH 方法](input.md) |
| 4. 呈现与声音 | 固定逻辑步进并改变呈现频率，播放已知曲目，暂停和恢复 | 额外绘制保持逻辑状态，连续呈现符合本作规则，实际音频身份及输出符合选择 | `music` 与 [DISPLAY／AUDIO／MIDI 方法](presentation.md) |
| 5. 存档与 Replay | 实际同步、关闭、新实例恢复，导入并播放有效样本 | 文件字节持久化和回放语义各有结果，播放到声明的样本结束边界 | `runtime`、`replay` 与 [SAVE／REPLAY 方法](storage.md) |
| 6. 语言与练习 | 按产品声明提供翻译、字体与练习构建，执行启用和关闭对照 | 实际显示与原生消费配置匹配，练习参数作用于本作状态 | `language`、`practice` 与 [LANG／PRACTICE 方法](localization.md) |
| 7. 安装与离线 | 从 Launcher 安装当前游戏包，关闭后断网重开，再按选定范围验证更新 | DATA、代际和已同步文件保持预期身份，所选更新失败会使任务失败 | `offline`、`offline-update` |
| 8. MP | 按产品支持的人数与通道开真实房间，逐席操作并测试退出、旁观和 Replay | 输入归属正确，各端逻辑收敛，生命周期及本作合作／对战规则成立 | `rooms`、`spectator`、`replay` 与 [MP 方法](multiplayer.md) |
| 9. 适配表验收 | 对照全部适用编号审查方法和执行证据 | 每个“已验证”均对应当前模式、产物和完整操作范围 | 本作 `ADAPTATION.md` 与[记录模板](record-template.md) |

输入测试先等待原生可操作阶段。菜单导航完成、按钮收到输入和角色产生效果各自观察。Replay 文件可读、播放开始、完整输入消费和原版一致性分别记录。普通版的原版对照与 MP 的多人自一致性沿用本作样本和结束条件。

`runtime` 和桌面行为组在排队前核对作品注册的 Runtime 实例、原生观察和导航接口；输入与运动组再核对运动规则、逻辑时钟、受控场景及所需的玩家动作消费者。计划逐项列出缺少的接口，补齐声明后即可沿用同一组共享断言。旧导航名单与旧状态读取入口继续保留各自用途，正式验收按当前场景需要的完整接口调度。

## 一作独立执行

以下命令从仓库根目录运行。以 th08 为例，换作时同时替换作品名和本地输入。工具链、私有字体、DATA 与有效 Replay 按本作开发指南准备。

```sh
node tools/workspace.mjs select th08 --variant=normal
node launcher/scripts/build-runtime.mjs --game=th08 --variant=normal --emsdk=/path/to/emsdk
node launcher/scripts/package-runtime.mjs --game=th08 --variant=normal
```

把该模式的目标写入[Runtime 输入清单](validation.md#测试辅助器)，清单可以只含 `th08/normal`。目标记录正式包路径、源码与打包收据、DATA、Replay 和各自身份。字体等共享输入填写本作实际使用的文件。需要私有字体的打包器使用本作指南登记的 `EAGLER_FONT_ROOT`。

本地配置 `.local/stg-inputs.json` 指向这份清单，相对路径以配置所在目录为基准：

```json
{
  "schema": "eagler-touhou/adaptation-inputs/1",
  "runtimeManifest": "th08-normal-inputs.json"
}
```

先预览，再执行同一选择：

```sh
npm run test:adaptation -- --game=th08 --variant=normal --suite=runtime --inputs=.local/stg-inputs.json --plan
npm run test:adaptation -- --game=th08 --variant=normal --suite=runtime --inputs=.local/stg-inputs.json
```

按接入阶段增加场景组。每轮生成新证据目录，共用准备在该轮复用一次，任务通过既有队列执行：

```sh
npm run test:adaptation -- --game=th08 --variant=normal --suite=desktop-input,desktop-background,practice,replay --inputs=.local/stg-inputs.json
npm run test:adaptation -- --game=th08 --variant=normal --suite=desktop-input --check=INPUT-01 --inputs=.local/stg-inputs.json
```

| 场景组 | 追加输入 | 模式范围 |
| --- | --- | --- |
| `runtime`、`desktop-input`、`desktop-background`、`practice`、`replay` | `runtimeManifest`，按场景提供有效 Replay、字体与练习构建 | 玩法与桌面驱动使用普通版；Replay 可显式选择普通版或 MP |
| `touch-actions` | `runtimeManifest` 与本作玩家／模态消费者、生产时钟和场景声明 | 焦点、射击、普通 Bomb、Deathbomb、双击、确认边沿及无限速结果链路；按编号独立选择。MP 通过真实房间与本机席位接入 |
| `replay-recording` | `runtimeManifest` 与本作 `replay` 语义、原生 trace、独立格式解析和生产时钟 | 一次普通游戏录制键盘、虚拟手柄、触控及指针轨迹。保存字节独立解析，再用相同文件配对检查回放消费者与实时输入隔离 |
| `music` | `musicManifest` | 目录登记的普通版 OGG 场景 |
| `language` | `runtimeManifest`、`languageManifest`、`artwork` | 已登记实际文本与字形观察的普通版，语言与场景使用独立 WASM 实例 |
| `presentation`、`verify-quick`、`verify-daily` | `diagnosticManifest` | 本作诊断适配器登记的模式 |
| `display-replay` | `runtimeManifest`、`diagnosticManifest` 与登记的配对 Replay | 同一 Replay 对照呈现节奏、权威状态、输入、音频事件和观察纯度，生成两个集中显示审查素材组 |
| `offline` | `packages`、`artwork` | 所选普通版，MP 包独立验收 |
| `offline-update` | `packages`、`artwork`、`updatePackages` | 已登记更新驱动的普通版 |
| `installation-authority` | `packages`、`artwork`、`updatePackages` | 实际卡片、更新提示、资源准备和 Runtime 消费同一安装代次 |
| `single-game` | `minimalRuntime` | 最小检出、正式构建来源和真实 Launcher 首帧关联 |
| `source-review` | `sourceReview` | 来源文件、素材与已接纳原版参考的身份核对 |
| `rooms`、`spectator` | `launcherUrl`，按安装方式提供 `packages` | 所选 MP 的支持人数与通道 |

录制接入同时声明 `replay.recording.consumer` 的原生消费帧偏移和源码依据。计划在启动前列出缺失声明；共享录制判据把设备输入、保存字节与实际玩家消费关联，复用有限窗口核对按下和释放。

多人单独选择模式并使用该模式的本地输入：

```sh
npm run test:adaptation -- --game=th08 --variant=multiplayer --suite=rooms,spectator,replay --inputs=.local/stg-mp-inputs.json --plan
npm run test:adaptation -- --game=th08 --variant=multiplayer --suite=rooms,spectator,replay --inputs=.local/stg-mp-inputs.json
```

## 诊断与原版对照队列

绘制纯度与原版对照沿用同一任务选择和证据管理。先按本作指南准备诊断 Runtime，将本地输入配置中的 `diagnosticManifest` 指向一份 `eagler-touhou/diagnostic-inputs/1` 清单：

```json
{
  "schema": "eagler-touhou/diagnostic-inputs/1",
  "products": {
    "th08": {
      "normal": {
        "font": "/absolute/msgothic.ttc",
        "data": "/absolute/th08.dat",
        "inputRoot": "/absolute/prepared-demos",
        "replayFiles": {
          "lunatic": "/absolute/lunatic.rpy",
          "extra": "/absolute/extra.rpy"
        }
      }
    }
  }
}
```

路径均为本机绝对路径。`presentation` 使用字体与 DATA；`verify-quick` 增加本作登记的 Demo；`verify-daily` 读取本作适配器声明的 `replayFiles` 键。按所选场景检查输入，单独运行绘制场景时可省略 `replayFiles`。

```sh
npm run test:adaptation -- --game=th08 --variant=normal --suite=presentation,verify-quick --inputs=.local/stg-inputs.json --plan
npm run test:adaptation -- --game=th08 --variant=normal --suite=presentation,verify-quick --inputs=.local/stg-inputs.json
npm run test:adaptation -- --game=th08 --variant=normal --suite=verify-daily --inputs=.local/stg-inputs.json
```

同一作品与模式的诊断场景共用一个服务，浏览器逐个执行，场景失败后继续独立场景并保存各自结果。前后核对正式 Runtime、诊断 WASM、构建源码、工具、固定参考和所选私有输入。复用结果前同样核对这些身份，沿用报告保留原采集时间。

[诊断适配器](../../tests/adaptation/adapters/diagnostics.mjs)声明服务、采集与比较入口、Runtime 位置、样本集合和本作报告含义。新增作品填写本作声明，复用队列、生命周期管理和结果封存。编号的完整度由实际观察决定；高刷新和文字的人审继续使用同一份精简场景表。

作品级能力的完整字段与共用／本作边界见[能力登记点](test-methods.md#能力登记点)。练习编号由产品能力和 `practiceAdaptationIds` 显式生成；Replay 的自然终点依赖本作格式与来源依据。BUILD-05 按[最小检出方法](build.md#build-05)独立构建、打包并经 Launcher 安装启动，来源收据与本次检出绑定。

## 判定与重跑

输入基础可单独选择 `--suite=touch-motion`。[共用运动验收](input.md#共用运动验收)集中核对微移、连续方向、原生限速、灵敏度和释放。相同模式的场景共享一局，切换启动配置时建立新代；与 `desktop-input` 同时选择时复用完整输入任务。普通版与 MP 按产品目录逐项列入计划。当前 th08 普通版接入原生轨迹；MP 计划项保持 blocked，由真实房间操作和逐 Tick 观察接口完成接入。

动作场景复用同一组判据，作品在 `adapters/titles/<game>.mjs` 声明生产语义：

| 编号 | 作品接口 | 共享场景核对的结果 |
| --- | --- | --- |
| TOUCH-09 | `deathbomb` | 真实弹体命中、原作最后有效消费帧、原生 Bomb 成本与存活、迟到输入跨复活清除、移动触点恢复 |
| TOUCH-10 | `runBoundaries`、`parseReplayMetadata` | 零位移不置标记、非零无限速消费、同局跨阶段、真实 Result 与保存 Replay 落率 |
| TOUCH-11 | `doubleTapBomb` 与已有 `actions` | 启动配置开关对照、单击不触发、次指双击只消耗一次 Bomb、主移动指保留 |
| TOUCH-13 | `dialogueTouchScene` | 当前关卡实际脚本入口、保持 Fire 的前置消费、无触摸对照、实际模态消费者的新边沿与脚本推进 |

观察回执在本作消费者处捕获输入和序号。后续录像模块写入的下一帧输入与当前消费分别记录；输入延迟和弹体消退状态按本作生产顺序声明。场景请求通过正式碰撞、脚本和转场入口执行，结果字段由游戏产生。编号预览会列出所选场景缺少的接口；共享断言变更后，按产品目录核对全部普通版与 MP 消费者。

`summary.json` 保存实际选择、逐任务结果、证据路径和逐编号覆盖。`--plan` 返回 planned，表示任务预览。实际运行缺输入、缺驱动、失败或未完成时返回非零状态。过期源码或包收据在浏览器启动前阻断，重新构建、打包后再运行。

场景 passed 表示该场景的断言成立；适配编号的完整证明同时要求 `complete: true`。显式 `--check=编号` 按这一门槛判定，未映射编号保留 unmapped。整作验收逐行汇总 `ADAPTATION.md` 的全部适用编号，补充对应专项驱动和执行证据。

```sh
npm run test:adaptation -- --resume=artifacts/validation/上次目录
npm run test:adaptation -- --rerun-failed=artifacts/validation/上次目录
```

续跑在任务边界核对源码、产物、输入、浏览器与封存报告。沿用证据标为 retained，本次运行标为 executed。设备验收记录实际硬件；桌面虚拟输入记录其软件路径。共用行为改变后，复验受影响的作品和模式。
