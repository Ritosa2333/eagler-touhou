# Touhou BossRush（BRO）网页版

[开发指南](docs/DEVELOPMENT.md) · [适配检查表](ADAPTATION.md) · [统一工作区](../../WORKSPACE.md) · [接入提案](proposals/README.md)

本目录登记第三方衍生作 **Touhou BossRush（BRO）** 接入 eagler-touhou 的产品身份与适配记录。BRO 是基于 LuaSTG Sub 的 Boss Rush 同人游戏，原作由吃药图书（Cyoltose）制作；网页化与联机适配由清酒（Tidew / TSake）维护。

> 本作是第三方衍生作，以 `testOnly + buildOptIn + importOnly` 登记，不进入默认发布列表。正式上线前的授权、署名、资源挂载等事项见[接入提案](proposals/README.md)，尚待维护者确认。

## 归属与来源

| 项 | 值 |
| --- | --- |
| 产品 ID | `bro`（联机 `bromp`） |
| 网页化产物库 | `https://github.com/TSake/TouHou-BossRushOnline`，分支 `web/multiplayer-port` |
| 上游原作 | `https://github.com/Cyoltose509/boss-rush`（只读，禁止 push） |
| 本目录登记 | `common/contracts/product-catalog.mjs`（`PRODUCT_GAMES.bro`） |
| Runtime 形态 | 单一自包含 WASM（`bro.mjs` + `bro.wasm`），纯 C Lua 5.1.5 + `lstg.*` C 绑定 + WebGL2 / WebAudio |
| 玩家数 | 首期 **2 / 3** 人共斗；引擎内部保留 4 槽，暂不开放第 4 席 |

## 玩法与联机

- 全队共用一套系统，团队资源（樱点 / 信仰 / UFO / 灵界 / 动物灵等）共享，个人数值与效果（永夜抄百分比、花映冢蓄力、绀珠传擦弹、5gaze 等）逐槽独立。
- 联机走 WebRTC DataChannel 直连，Full Mesh，输入缓冲 + 预测回滚 + 持续选路；数据面不逐帧走 postMessage。
- 多人 Replay 采用旁路多流（`replay/slotN.multi`），借 eagler `write(path, bytes, replayMatch?)` 的 `replayMatch` 字段存取，与官方文件桥契约兼容。
- Runtime 入口协议已按 `eagler-touhou/1` 实现（13 命令 / 8 事件，`runtimeEpoch` 校验），详见网页化产物库 `web/src/eagler_adapter.mjs`。

## 当前进度（诚实记录）

底层运行时 / 渲染 / replay / 存档 / 确定性，以及联机（WebRTC / 缓冲 / 回滚 / Mesh / 选路 / 多人 HUD / 11 项系统多人兼容）已在网页化产物库完成并验证；纯 Node 回归 30/30 绿。逐项证据与状态见[适配检查表](ADAPTATION.md)与 `proposals/MANIFEST_DIFF.md`。

尚未完成：19 关完整通关、真机多人性能、触控 / 手柄全套验收、正式 Host 流程「选游戏→启动→首帧→退出」记录、帧 ~182 引擎自身确定性分叉（另案）。

[贡献指南](../../CONTRIBUTING.md) · [适配流程](../../docs/adaptation/README.md) · [全系列作品进度](../../docs/TOUHOU_PROGRESS.md)
