# TH128 共用 Launcher 接入

接入范围是单机。保留 Runtime122 的原逻辑 60 Hz、1280×960 输出，以及高刷显示、触控、多语言、thprac 和 Replay。原 DAT＋OGG 由用户本机提供。共用启动器负责安装、版本及启动，TH128 继续消费原资源并实现游戏循环。

源码基线为 `restoration` 的 `350bf8f7cb9ed042c3d365abe25ac799189ec8ff`，保留对应 Runtime122 v17 源码提交 `e0057d1dee688bb7ac545b068ad2d7d91b0430fa`。桌面 `portable` 保持独立。对接依据是 `Ritosa2333/eagler-touhou` 的 `bc39ac6f7acddf8518feccd98a0343c5b637d2b1`。新增 `ports/th128` 时按其 `CONTRIBUTING.md` 登记来源连接与导出基点，以 `check-history.py` 和真实子树导出核验。作品自有代码的发布许可与继承组件声明见 [NOTICE](../NOTICE.md)。

| 接口 | 接入状态与检查 |
| --- | --- |
| 目录 Runtime | `play.html` 的模块和样式以及 `runtime/build.json` 随页面目录解析；仅同目录清单的 GET/HEAD 走静态站点，其余导入资源仍交原提供者。打包为上游所需的 `th128.html`，保留全部相对模块、CSS 与 Core122 代次目录。仓库构建脚本与上游目录清单的接线待补。 |
| 资源与启动 | `launcher-session.mjs` 复用会话代次、配置、首帧、键盘/触控与退出。Runtime 的 `managedPackage=1` 入口从同一个 Package Store 安装代次读取描述符、原 DAT、语言与 OGG，以 `createDatOggProviderFromFiles` 核对并挂载。旧的独立 ZIP 与只交接原 DAT 的入口保留。共用 Launcher 仍需供给下述回执。 |
| 产品与发布清单 | `launcher/config/workspace.json`、`launcher/config/runtime-builds.json` 和 `common/contracts/product-catalog.mjs` 的 TH128 登记随对应的实际源构建、入口和资源挂载一起加入。使用目录 Runtime，声明确实识别的文件、挂载、语言、音乐和存档能力；现有录像桥的写入时段与共用存档契约分别核对。 |

本次用 Node 24 运行 `node --test tests/launcher-session/runtime-mount.test.mjs tests/online-launch/resources.test.mjs`，17 项通过；`node --check site/play.mjs` 通过。检查内容是正式 HTML/CSS 路径、静态构建清单和资源提供者归属，未运行游戏或浏览器，也未改变成品包。

## Package Store 交接

父 Launcher 在相同 origin 的 Runtime URL 添加 `managedData=1&managedPackage=1`、其已装 `gameGeneration` 和当前 `runtimeEpoch`。数据回调仍是 `__eaglerPrepareManagedRuntimeDataV1`；TH128 传入 `{game, generation, epoch, packageVersion: 'th128-dat-ogg/1'}`。父端从它已持有租约的同一代安装返回原 `{buffer, bytes, fileId}`，附加 `managedPackage: {version, generation, epoch, descriptor, readFile}`。`descriptor` 是 UTF-8 JSON 的独立 `ArrayBuffer`，`readFile(fileId)` 返回 `{buffer, bytes, fileId, path}`，只读该代描述符列出的文件。每次异步读取前后检查 Runtime 实例、文档与代次的身份。其他游戏和没有请求该版本的调用沿用现有 DATA 回执。

上游当前先等 `ready`，再准备可选语言和音乐；父端须在为 TH128 创建 Runtime 之前确认它导入的完整 DAT＋OGG 安装已可读，不能让新入口在 `ready` 前等待 `ready` 后才发的资源。本作的首个接入验收范围是用户导入已有完整 ZIP。Runtime 验证描述符、文件标识、挂载目标、字节和 SHA 后发 `ready`；退出、来源变更或错误取消交接，不退回 iframe 的安装库，也不产生首帧事件。父端供给尚待在上游本地工作区接线。

`node --test tests/launcher-session/managed-entry.test.mjs` 20 项通过，覆盖完整回执、退出取消、失效代次、损坏字节及旧入口。`node --check site/import-entry.mjs` 通过。测试用真实 TH128 描述符解析和 DAT/OGG 提供者、自建 THA1 数据及模拟父端，不等同上游整页、实际原版资源或存档桥验收。

接入验收从共用 Launcher 导入本地原 DAT＋OGG 开始。用真实页面验证就绪、配置、启动、原画面首帧、退出清理与再次进入；同时核对安装代次和 Runtime 代次，按上游适配编号记录结果。其余功能按实际改动及上游要求验证。
