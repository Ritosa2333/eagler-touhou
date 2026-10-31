# TH17 共用启动器接入验证 · 2026-10-30

TH17 1.0.14 的日文 1.00b 源码在 monorepo 中重新编译为生产 Wasm，运行文件由 Runtime Directory 清单逐项固定 SHA-256，原版 DATA 与音乐通过共用 Host / Package Store 边界提供。

## 构建与身份

生产构建使用 Emscripten 6.0.9、SDL3、WebGL2、C++17 和严格浮点选项，保留原游戏逻辑与固定 60 Hz 模拟。Wasm 为 `24b9d5201cb41286a3e226fa0ffe950ce1e91658731aa1bde8754bcf8c50bd30`，Runtime 版本为 `1.0.14-eagler-24b9d5201cb4`。20 项 Runtime 文件包含程序、Shell、输入桥、GPU 计时器与字体表。

实际源码、头文件、SDK 与产物身份记录在本地 `artifacts/validation/th17/source-build.json`；逐文件 Runtime 身份位于 `ports/th17/build-eagler/runtime-files.json`。

## 执行结果

| 范围 | 入口与结果 |
| --- | --- |
| 适配源码与模块 | `npm run ports:status -- --game=th17 --variant=normal`：9/9，包括协议、触控灵敏度、原生手柄、常显判定点 ABI、包管理和帧节奏 |
| 桌面浏览器生产程序 | `web/tests/adapter-browser.test.mjs`：OGG／无音乐、过期 epoch 过滤、先配置后启动、命令错误回执、首帧、音频／帧健康事件、输入清除、存档验证、原生退出、Host 音频复用、IDBFS 跨文档恢复和原版演示文件启动 |
| 原生模块 | 随机数 65,536 种种子与原版对照通过；三份原版演示解码后的每一输入帧与原版一致；连续触控运动的录像导出、暂停、切关及重复选择往返通过 |
| 帧时钟 | 原生 `FrameCadence.hpp`：15–165 Hz、60 秒、3,600 次模拟步进，覆盖量化、抖动、追帧上限、暂停清债与借时偿还 |
| 音乐准备 | 18 首固定 OGG 的字节身份通过 Host 基线校验；`ffprobe` 核对 44,100 Hz、双声道和每首原版 PCM 帧数 |
| 共用工程 | 产品目录、20 项构建计划、Runtime Release、资源选择、工作区映射、静态内容和 TypeScript 类型检查通过 |
| 迁移来源 | 原交付校验清单固定 780 项迁入文件的原始身份；非 Git 交付的来源检查覆盖校验值、归属、登记保留及当前目录存在性 |
| 发布源码边界 | 工作区发布审计通过，原版 EXE／DATA、OGG、录像、测量字体与工具链保存在忽略目录 |

浏览器环境为 Windows 桌面 Chromium、软件 WebGL2。记录对应本次生产 Wasm、当前 Shell 和所列模块。实体手机操作、整局玩法及长录像状态对照按[适配表](../../ports/th17/ADAPTATION.md)继续记录。

TH17 的交付来源校验覆盖全部 780 项迁入记录；新增来源检查的 8 项回归通过。全仓附加历史导出在 Windows Git 的旧 `ports/th12` 子树处触发 Bash 段错误，其余历史子树的完整导出交由仓库现有 Linux CI 入口复核。

浏览器事件与截图位于本地 `artifacts/validation/th17/adapter-browser.json`、`title.png` 和 `replay.png`。原生对照详细结果位于 `ports/th17/web/artifacts/cpp/verification/`；原交付历史报告按其原版本身份保留。
