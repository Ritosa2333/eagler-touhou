# Game Adapter 契约入口

新 Runtime 的流程见 [适配开发指南](../../docs/adaptation/build.md)。本页定位声明、协议字段与代码 owner；机器契约以源码为准。

## 契约来源

| 内容 | Owner |
| --- | --- |
| 产品能力与静态元数据 | [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs)，Launcher 类型入口 [`product-catalog.mts`](../src/contracts/product-catalog.mts) |
| 必需、可选及 profile 能力 | [`adapter-capabilities.mts`](../src/contracts/adapter-capabilities.mts) |
| Runtime 命令、事件、字段与响应要求 | [`runtime-protocol.mts`](../src/contracts/runtime-protocol.mts) |
| 原始内容与 Host 准备方式 | [`content-definition.mjs`](../lib/content-definition.mjs) |
| DATA provider | [`runtime-data-provider.mjs`](../lib/runtime-data-provider.mjs) |
| Runtime Release 清单 | [`package-runtime-release.mjs`](../scripts/package-runtime-release.mjs) |
| thprac / 语言接入 | [`integrations/thprac.mjs`](../integrations/thprac.mjs)、[Runtime 集成契约](../integrations/RUNTIME_CONTRACT.md) |
| 旁观协议 | [`spectator.md`](../../docs/architecture/spectator.md) |

## 产品声明

在 `PRODUCT_GAMES` 中维护标题、Runtime 地址、`storage.saveRoot`、`scoreFile`、`configFiles`、`replay.prefix`、`dataProvider`、`package`、`musicCapabilities` 和已支持的可选 capability。目录 Runtime 另声明 `runtimeFileLayout`、`runtimeAssets`、`requiredShared`；多人产品同时声明 `multiplayerRuntime` 与 `multiplayer`。部署产生的 URL、摘要和实际安装代数由发布物维护，不写入产品能力表。

共享代码读取产品声明中的 provider、布局与功能字段；游戏编号用于选择对应的产品记录。产品 capability 由 `PRODUCT_GAMES` 声明；`content-definition.mjs` 中的 `hostPreparation` 描述内容构建配方。

## Runtime 消息

每条消息都带 `protocol: "eagler-touhou/1"`、`game`、正整数 `epoch`。Launcher 把同一 epoch 放入 Runtime URL 的 `runtimeEpoch` 查询参数。命令增加 `command`，可用 `request` 关联响应；响应带 `request` 与布尔值 `ok`，失败时可带 `error`、`errno`；事件增加 `event`。

| 命令 | 阶段 | 必需字段 |
| --- | --- | --- |
| `configure` | 启动前 | `music`（`ogg`、`midi` 或 `none`）；可带 `language`、`resources`、`runtimeResources`、`sharedResources`、`runtimePack`、`options` |
| `resources` | ready 后 | `resources` |
| `keyboard` | 运行中 | `down`、`code`；可带 `inputSource`、`repeat`、`launcherOwnsFocus` |
| `keyboard-clear` | 运行中 | 无 |
| `touch-controls` | 运行中 | `fireEnabled`、`focusEnabled`、`bombSerial`、`escapeSerial`、`joystickX`、`joystickY`、`touchSensitivity`；thprac 面板字段可选 |
| `touch-cancel` | 运行中 | 无 |
| `direct-touch` | 运行中 | `type`（`down`、`move` 或 `up`）、`id`、`x`、`y` |
| `launch` | 启动前 | 无 |
| `list`、`sync` | 运行中 | 无 |
| `read`、`remove` | 运行中 | `path` |
| `write` | 运行中 | `path`、`bytes`；`replayMatch` 可选 |

必须上报的事件：`ready`、`first-frame`、`runtime-info`、`frame-health`、`audio-health`、`exit`、`error`。`ready` 可带 `saveRoot`；`first-frame` 表示游戏确实呈现首帧。按 capability 使用的命令包括 `playback`、`retry-music`、`thprac-mouse`、`network-cancel`、`spectator-room-state`、`spectator-playback`；条件事件包括 `midi-fallback`、`music-complete`、`music-error`、`music-incomplete`、`network-request`。`notice` 与 `player-debug` 为可选事件。事件和命令的完整类型见 [`runtime-protocol.mts`](../src/contracts/runtime-protocol.mts)。

| 阶段 | 命令 / 事件 | 约定 |
| --- | --- | --- |
| 初始化 | `ready`、`configure`、`resources`、`launch` | `ready` 表示文件与 DATA 命令桥可用；`first-frame` 表示游戏已实际呈现首帧 |
| 输入 | `keyboard`、`keyboard-clear`、`touch-controls`、`touch-cancel`、`direct-touch` | 进入原作逻辑输入路径；条件不适用时拒绝命令 |
| 用户文件 | `list`、`read`、`write`、`remove`、`sync` | 仅访问声明的存档、配置与 Replay 文件 |
| 健康与结束 | `runtime-info`、`frame-health`、`audio-health`、`exit`、`error` | 按协议要求上报身份、健康状态与终止状态 |

命令必需性、可用阶段和 ACK 要求见 `RUNTIME_PROTOCOL_COMMAND_BEHAVIOR`。命令标记为必需时，其行为始终必需；可选 ACK 表示回执可以省略。

## 同步输入桥

键盘、手柄及即时输入桥由同源 Host 提供，并校验 `schema`、`protocol`、`game`、`epoch`、`origin`。当前 schema 为 `eagler-touhou/keyboard-input/1`、`eagler-touhou/gamepad-input/1` 与 `eagler-touhou/direct-input/1`。Launcher 负责别名、设备配置和物理输入归属；Runtime 在原生输入采样点同步读取，并在生命周期边界清除残留。

## 资源边界

- Runtime Release 只包含 Runtime 程序和资源；原版 DATA、音乐与封面属于 Package / Site 内容。
- Site Manifest 声明已发布资源；Package Descriptor 声明安装文件、Runtime 目标与兼容身份；Package Store 管理安装版本。
- 保存与 Replay 属于独立的可变用户文件，目录由产品 `storage` 声明；不得建 Runtime 私有安装库。
- 新语言包在启动前校验并安装；缺失的翻译资源回退至原作资源。OGG 与无音乐模式是必需行为，MIDI 为可选 capability。

## 下一步

- [Build 与发布](../../docs/adaptation/build.md)
- [Runtime 协议](../../docs/adaptation/runtime.md)
- [输入与触控](../../docs/adaptation/input.md)
- [数据保存](../../docs/adaptation/storage.md)
- [能力义务](ADAPTER_CAPABILITIES.md)
