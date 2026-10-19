# Runtime 存档验证

本文给 Runtime 适配者一条最短的存档验证路径。存档路径由 Product Catalog 声明，Host 负责 IndexedDB 挂载与同步，Runtime 通过统一命令读写。

## 代码归属

| 内容 | 代码 |
| --- | --- |
| 游戏存档名、根目录、配置文件 | [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs) 中的 `storage` |
| Runtime 命令和消息字段 | [`src/contracts/runtime-protocol.mts`](../src/contracts/runtime-protocol.mts) |
| 不可变游戏 DATA 装载 | [`lib/runtime-data-provider.mjs`](../lib/runtime-data-provider.mjs) |
| Node 合约测试 | [`tests/test-runtime-storage-contract.mjs`](../tests/test-runtime-storage-contract.mjs) |
| 浏览器一致性测试 | [`tests/test-runtime-storage-conformance.py`](../tests/test-runtime-storage-conformance.py) |

Runtime 使用 `list`、`read { path }`、`write { path, bytes }`、`remove { path }`、`sync`。路径必须位于本游戏声明的存档根目录；`ready` 事件可带 `saveRoot`。

## 准备

在 `launcher/` 安装依赖，构建当前 Launcher，并为每个要测游戏准备存档文件：

```sh
npm run build:launcher
```

```text
<fixture-root>/<game>/<scoreFile>
```

`scoreFile` 从 Product Catalog 读取。另开终端运行 `npm start`，服务默认端口为 `8130`，可用 `EAGLER_TOUHOU_PORT` 覆盖；也可以用 `--url` 指定已有服务地址。

## 运行

以下命令都从 `launcher/` 目录执行。

先跑本地命令合约：

```sh
npm run test:storage
```

再跑 Chromium 与 WebKit 的浏览器矩阵：

```sh
npm run test:storage:browser -- --fixture-root /path/to/fixtures
```

Windows PowerShell 示例：

```powershell
npm run test:storage:browser -- --fixture-root D:\fixtures\saves --package th08=D:\fixtures\th08-content.zip
```

`--package GAME=PATH` 可重复使用，通过 Launcher 导入指定内容包。默认测试 Product Catalog 中的全部游戏、Chromium/WebKit，以及 `score-only`、`commands` 两种用例。

| 参数 | 用途 |
| --- | --- |
| `--games th06 th07` | 只测指定游戏 |
| `--browsers chromium webkit` | 选择浏览器 |
| `--cases score-only commands` | 选择基础用例 |
| `--cases nested-write` | 另测 `probe/nested.dat` 子目录写入 |
| `--cases restore-failure` | 注入首次 `syncfs(populate)` 失败，检查 Runtime 报错且不进入 `ready` / `first-frame` |
| `--url http://127.0.0.1:8130/` | 指定正在运行的 workspace |

## 检查结果

基础用例检查正常退出、整页重载、临时 Runtime 导出销毁、存档恢复及字节一致。`commands` 还检查列出、读取、写入、删除、同步及越界路径拒绝。浏览器用例在独立上下文挂载存档，并验证同步、重载与恢复。

报告默认写入 `artifacts/validation/runtime-storage/<timestamp>/`，失败时命令以非零状态退出，并保存逐例 JSON、导出文件和截图。

## 下一步

添加新 Runtime 时，在它的存档适配中实现上述命令，再跑两组检查。完整的持久化边界见 [`docs/adaptation/storage.md`](../../docs/adaptation/storage.md)；Runtime 消息入口见 [`GAME_ADAPTER_CONTRACT.md`](GAME_ADAPTER_CONTRACT.md)。
