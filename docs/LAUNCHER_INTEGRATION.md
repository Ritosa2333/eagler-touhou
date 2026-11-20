# TH128 共用 Launcher 接入

接入范围是单机。保留 Runtime122 的原逻辑 60 Hz、1280×960 输出，以及高刷显示、触控、多语言、thprac 和 Replay。原 DAT＋OGG 由用户本机提供。共用启动器负责安装、版本及启动，TH128 继续消费原资源并实现游戏循环。

源码基线为 `restoration` 的 `350bf8f7cb9ed042c3d365abe25ac799189ec8ff`，保留对应 Runtime122 v17 源码提交 `e0057d1dee688bb7ac545b068ad2d7d91b0430fa`。桌面 `portable` 保持独立。对接依据是 `Ritosa2333/eagler-touhou` 的 `bc39ac6f7acddf8518feccd98a0343c5b637d2b1`。新增 `ports/th128` 时按其 `CONTRIBUTING.md` 登记来源连接与导出基点，以 `check-history.py` 和真实子树导出核验。作品自有代码的发布许可与继承组件声明见 [NOTICE](../NOTICE.md)。

| 接口 | 接入状态与检查 |
| --- | --- |
| 目录 Runtime | `play.html` 的模块和样式以及 `runtime/build.json` 随页面目录解析；仅同目录清单的 GET/HEAD 走静态站点，其余导入资源仍交原提供者。`tools/package-launcher-runtime.mjs` 组装上游所需的 `th128.html`、`manifest.json` 和 `runtime-files.json`，保留相对模块、CSS 与已验 Core122 代次。上游发布器接线待合并验证。 |
| 资源与启动 | `launcher-session.mjs` 复用会话代次、配置、首帧、键盘/触控与退出。Runtime 的 `managedPackage=1` 入口从同一个 Package Store 安装代次读取描述符、原 DAT、语言与 OGG，以 `createDatOggProviderFromFiles` 核对并挂载。旧的独立 ZIP 与只交接原 DAT 的入口保留。共用 Launcher 仍需供给下述回执。 |
| 产品与发布清单 | `launcher/config/workspace.json`、`launcher/config/runtime-builds.json` 和 `common/contracts/product-catalog.mjs` 的 TH128 登记随对应的实际源构建、入口和资源挂载一起加入。使用目录 Runtime，声明确实识别的文件、挂载、语言、音乐和存档能力；现有录像桥的写入时段与共用存档契约分别核对。 |

本次用 Node 24 运行 `node --test tests/launcher-session/runtime-mount.test.mjs tests/online-launch/resources.test.mjs`，17 项通过；`node --check site/play.mjs` 通过。检查内容是正式 HTML/CSS 路径、静态构建清单和资源提供者归属，未运行游戏或浏览器，也未改变成品包。

## Package Store 交接

父 Launcher 在相同 origin 的 Runtime URL 添加 `managedData=1&managedPackage=1`、其已装 `gameGeneration` 和当前 `runtimeEpoch`。数据回调仍是 `__eaglerPrepareManagedRuntimeDataV1`；TH128 传入 `{game, generation, epoch, packageVersion: 'th128-dat-ogg/1'}`。父端从它已持有租约的同一代安装返回原 `{buffer, bytes, fileId}`，附加 `managedPackage: {version, generation, epoch, descriptor, readFile}`。`descriptor` 是 UTF-8 JSON 的独立 `ArrayBuffer`，`readFile(fileId)` 返回 `{buffer, bytes, fileId, path}`，只读该代描述符列出的文件。每次异步读取前后检查 Runtime 实例、文档与代次的身份。其他游戏和没有请求该版本的调用沿用现有 DATA 回执。

上游当前先等 `ready`，再准备可选语言和音乐；父端须在为 TH128 创建 Runtime 之前确认它导入的完整 DAT＋OGG 安装已可读，不能让新入口在 `ready` 前等待 `ready` 后才发的资源。本作的首个接入验收范围是用户导入已有完整 ZIP。Runtime 验证描述符、文件标识、挂载目标、字节和 SHA 后发 `ready`；退出、来源变更或错误取消交接，不退回 iframe 的安装库，也不产生首帧事件。父端供给尚待在上游本地工作区接线。

`node --test tests/launcher-session/managed-entry.test.mjs` 20 项通过，覆盖完整回执、退出取消、失效代次、损坏字节及旧入口。`node --check site/import-entry.mjs` 通过。测试用真实 TH128 描述符解析和 DAT/OGG 提供者、自建 THA1 数据及模拟父端，不等同上游整页、实际原版资源或存档桥验收。

完整安装的启动配置复用已交付资源：`lang_zh-hans` 映射实际游戏 catalog `zh`，`runtimePack:null` 表示无附加包；音乐为整曲解码 OGG 或关闭，练习界面独立接收 `zh-CN`、`ja-JP`、`en-US`。高刷和触控字段仍沿原协议，当前不接受流式 OGG、常显判定点和原包没有的游戏语言。父候选选择完整安装的本地配置，不另派同一语言/音乐。2 项父配置及 22 项 Runtime 入口相关 Node 检查通过；实际 UI、程序首帧和声音待本机联调。

## 组装目录 Runtime

在本作源码目录用 Node 24 执行，`--assets` 指向已交付 Runtime122 v17 解压目录里的 `site`，`--output` 指向尚不存在的输出目录：

```text
node tools/package-launcher-runtime.mjs --assets="<v17>/site" --output="<本机预览工作区>/build-eagler"
```

省略 `--output` 时默认输出本作 `build-eagler`，也可通过 `EAGLER_RUNTIME_OUTPUT` 指定。清单要求已核验的 Core122 收据和 JS/WASM 字节，变更源码需要重新按对应构建流程取得可信核心；本命令只做目录组装。输出携带代码、许可和适配声明，原 DAT、OGG、翻译资源包、封面及存档由启动器和本机资源管理，不进入 Runtime 目录。当前版本声明练习支持；完整原包的游戏文本只有简中，练习界面语言独立。

`tests/launcher-runtime-directory-check.mjs` 使用实际成品核心和本作源码、固定上游协议断言验证目录、字节收据、本地模块闭包和会话代次。该检查不需要浏览器或重跑游戏。

## 交接前的本机预览

先按上游 `launcher/docs/SELF_HOSTING.md` 把共用 Launcher 和适配后的 TH128 Runtime 组装到单独的本机预览目录。作者仓库本身尚未带 TH128，需要带上本次接入改动。由用户打开本机浏览器地址后，导入手头现有的 DAT＋OGG ZIP，验证就绪、配置、启动、原画面首帧、退出清理与再次进入；核对安装与 Runtime 代次，按上游适配编号记录。预览使用本机资源，无需改线上站点或重新交付旧包。当前尚无组装完成的预览地址。
