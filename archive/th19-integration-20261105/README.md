# TH19 接入验收

TH19 以共用 C++ / SDL3 / WebGL2 核心接入普通版与双人对战产品，源码编译、资源安装、输入、存档与对战均经过执行验证。

| 字段 | 本次范围 |
| --- | --- |
| 基线 | `c2e94c5506c436f09a9f5854f634fe02b82b1f98`，源码来源登记见 [本地交付映射](../../docs/migration/th19-eagler-20261104.json) |
| 源码 | `ports/th19`，normal / multiplayer 共用，模式身份与 IDBFS 数据库独立 |
| 编译 | Emscripten 6.0.9-git，14 个 C++ 单元，SDL3 port，`-O2 -ffp-contract=off` |
| WASM | `a10571aca07be8a581c52e0826143b42fff0e879f0a7c00f275656328d2e6380` |
| 公共库 | 本仓库 `common/` 的浏览器手柄、触屏手势和 WebSfx 接口 |
| 浏览器 | Windows Edge 156，Puppeteer，无头软件图形，本机 2 席 WebRTC / Relay |

## 已执行验收

- Launcher 正常入口安装 DATA 和 24 首 OGG，完成首帧；从原生标题依次选择难度、角色，通过对话进入战斗，键盘移动改变原生自机坐标，Esc 打开暂停，原生 SFX 进入共享音效输出。
- 配置文件经过 IndexedDB 重载恢复，`../th19.dat` 越界写入被拒绝。
- 精确输入与回滚模式各运行至少 360 帧，两端在第 120、240、360 帧的确认状态哈希相等。
- 多人预加载呈现原生标题；返回房间后仍保留同一个 Module 与原版资源，并恢复标题场景。
- 回滚纠正、确认输出、预测输入约束、RTC 路线屏障与显式 Relay 回退共 7 项测试通过。
- 逻辑时钟在 15、20、30、50、59、60、75、90、120、144、165、240 Hz 调度下保持 60 Hz 逻辑，暂停清理时间债务。
- Launcher 资源准备、Runtime 生命周期、多人服务与发布模板共 192 项测试通过；repository 项目覆盖见 [逐文件结果](repository-tests.json)。

## 执行入口与证据

- [浏览器协议、存储、RTC 与原生房间返回](runtime-browser.json)：`ports/th19/tests/browser-smoke.mjs`，浏览器与 Puppeteer 模块通过环境变量选择。
- [Launcher 服务层检查](launcher-services.log)：`runtime-service.test.mjs`、`game-preparation.test.mjs`、`multiplayer-services.test.mjs` 与 `test-operator-delivery.mjs`。
- [源码编译日志](source-build.log) 与 [编译来源记录](th19-normal-source-build.json)：`node launcher/scripts/build-runtime.mjs --game=th19 --variant=normal --emsdk=<SDK> --emscripten-version=6.0.9`。
- 作品执行 `npm test`、`node portable/check-frame-cadence.mjs`；总仓库执行 `npm run ports:check -- --game=th19 --variant=both`。

本记录覆盖桌面协议、原生玩法入口与本机联机一致性，不作硬件性能数据。触屏实机、跨公网 TURN、全角色长流程与多人 Replay 使用各自专项验收；产品旁观能力声明为关闭。源码与 Runtime 交付不包含原版 DATA、音乐或字体。
