# STG 适配流程

从一部作品、一个构建模式开始，用当前产物完成启动、操作、持久化与退出，再扩展到该作的全部适用编号。共用驱动执行操作与断言，作品适配器提供真实状态、导航和规则差异。

[适配入口](README.md) | [验收方法](test-methods.md) | [测试命令](../../tests/README.md)

## 范围与责任

当前 STG 流程登记 th06、th07、th08、th09、th10、th11、th15、th20。普通版与 MP 分别构建、打包和验收；th09 的对战规则与其他作品的合作规则分别检查。支持模式、练习、MIDI 和语言功能以[产品目录](../../common/contracts/product-catalog.mjs)及[构建配方](../../launcher/config/runtime-builds.json)为准。

th075 声明 `inputProfile: fighting`，格斗玩法使用本作的对局、动作与结束条件。构建、包身份、协议、文件持久化和共享控件沿用对应共用契约，玩法驱动按格斗语义接入。STG 的低速、Deathbomb、落率与关卡导航属于本章的玩法范围。

| 位置 | 负责内容 | 接入要求 |
| --- | --- | --- |
| `common/` 与 Launcher | 共享输入、资源、存储、协议和控件 | 修改共享能力时核对全部消费者 |
| 本作 Shell 与原生实现 | 配置消费、原生输入、生命周期与文件操作 | 生产入口执行用户操作，观察接口只复制真实状态 |
| [原生状态映射](../../tools/ports/runtime-observations.mjs) | 阶段、已消费输入、逻辑帧、坐标和本机席位 | 字段注明单位、有效位与采样边界 |
| [作品流程适配器](../../tests/adaptation/adapters/title-flow.mjs) | 标题导航、玩法就绪和暂停重开 | 按本作实际菜单与确认步骤实现，普通版与 MP 各自登记 |
| [场景目录](../../tests/adaptation/catalog.mjs) | 作品、变体、编号、输入和任务选择 | 缺驱动或输入时给出 blocked，已声明不支持的组合给出不适用 |
| 本作 `docs/DEVELOPMENT.md` | 资源准备、原版样本与特殊规则 | 维护者能据此准备相同输入并复跑 |

## 按阶段接入

每个阶段先完成当前模式，再核对对应模式。阶段结果绑定本次源码、构建计划、Runtime 和输入身份。

| 阶段 | 实现与操作 | 通过依据 | 入口 |
| --- | --- | --- | --- |
| 1. 登记与构建 | 登记源码归属、产品、构建与打包配方，执行正式构建和打包 | 当前模式有完整来源收据，WASM、加载器、入口与包字节一致 | [BUILD 方法](build.md)、`build-runtime.mjs`、`package-runtime.mjs` |
| 2. 启动与生命周期 | 经真实 Shell 配置、启动、暂停、重开、退出 | 协议身份与代次正确，首帧已呈现，暂停停逻辑帧，重开后继续推进 | `runtime` |
| 3. 输入与触控 | 从 Launcher 改键、手柄映射及触控入口执行按下、组合、释放和取消 | 原生已消费输入与本机角色效果一致，释放与切换后无残留 | `desktop-input`、`desktop-background` 与 [INPUT／TOUCH 方法](input.md) |
| 4. 呈现与声音 | 固定逻辑步进并改变呈现频率，播放已知曲目，暂停和恢复 | 额外绘制保持逻辑状态，连续呈现符合本作规则，实际音频身份及输出符合选择 | `music` 与 [DISPLAY／AUDIO／MIDI 方法](presentation.md) |
| 5. 存档与 Replay | 实际同步、关闭、新实例恢复，导入并播放有效样本 | 文件字节持久化和回放语义各有结果，播放到声明的样本结束边界 | `runtime`、`replay` 与 [SAVE／REPLAY 方法](storage.md) |
| 6. 语言与练习 | 按产品声明提供翻译、字体与练习构建，执行启用和关闭对照 | 实际显示与原生消费配置匹配，练习参数作用于本作状态 | `practice` 与 [LANG／PRACTICE 方法](localization.md) |
| 7. 安装与离线 | 从 Launcher 安装当前游戏包，关闭后断网重开，再按选定范围验证更新 | DATA、代际和已同步文件保持预期身份，所选更新失败会使任务失败 | `offline`、`offline-update` |
| 8. MP | 按产品支持的人数与通道开真实房间，逐席操作并测试退出、旁观和 Replay | 输入归属正确，各端逻辑收敛，生命周期及本作合作／对战规则成立 | `rooms`、`spectator`、`replay` 与 [MP 方法](multiplayer.md) |
| 9. 适配表验收 | 对照全部适用编号审查方法和执行证据 | 每个“已验证”均对应当前模式、产物和完整操作范围 | 本作 `ADAPTATION.md` 与[记录模板](record-template.md) |

输入测试先等待原生可操作阶段。菜单导航完成、按钮收到输入和角色产生效果各自观察。Replay 文件可读、播放开始、完整输入消费和原版一致性分别记录。普通版的原版对照与 MP 的多人自一致性沿用本作样本和结束条件。

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
| `music` | `musicManifest` | 目录登记的普通版 OGG 场景 |
| `offline` | `packages`、`artwork` | 所选普通版，MP 包独立验收 |
| `offline-update` | `packages`、`artwork`、`updatePackages` | 已登记更新驱动的普通版 |
| `rooms`、`spectator` | `launcherUrl`，按安装方式提供 `packages` | 所选 MP 的支持人数与通道 |

多人单独选择模式并使用该模式的本地输入：

```sh
npm run test:adaptation -- --game=th08 --variant=multiplayer --suite=rooms,spectator,replay --inputs=.local/stg-mp-inputs.json --plan
npm run test:adaptation -- --game=th08 --variant=multiplayer --suite=rooms,spectator,replay --inputs=.local/stg-mp-inputs.json
```

## 判定与重跑

`summary.json` 保存实际选择、逐任务结果、证据路径和逐编号覆盖。`--plan` 返回 planned，表示任务预览。实际运行缺输入、缺驱动、失败或未完成时返回非零状态。过期源码或包收据在浏览器启动前阻断，重新构建、打包后再运行。

场景 passed 表示该场景的断言成立；适配编号的完整证明同时要求 `complete: true`。显式 `--check=编号` 按这一门槛判定，未映射编号保留 unmapped。整作验收逐行汇总 `ADAPTATION.md` 的全部适用编号，补充对应专项驱动和执行证据。

```sh
npm run test:adaptation -- --resume=artifacts/validation/上次目录
npm run test:adaptation -- --rerun-failed=artifacts/validation/上次目录
```

续跑在任务边界核对源码、产物、输入、浏览器与封存报告。沿用证据标为 retained，本次运行标为 executed。设备验收记录实际硬件；桌面虚拟输入记录其软件路径。共用行为改变后，复验受影响的作品和模式。
