# 自托管

使用 Launcher Host 命令，从已发布 Runtime Release 和依法持有的本地游戏安装资源构建静态站点。正式 Host 构建使用 Product Catalog 中全部注册游戏；单作开发使用 workspace 开发服务器。

## 准备输入

需要 Bun 1.4.4+、Python 3 和匹配当前 Launcher Product Catalog 的完整 `runtime-release/`。每个已登记游戏都准备 `games/<game-id>/` 目录，并放入对应原作文件。游戏列表见 [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs)。

在 `launcher/` 或自托管包根目录使用以下目录结构：

```text
host-root/
├─ package.json
├─ bun.lock
├─ .bun-version
├─ runtime-release/
├─ games/<game-id>/       # 每个已登记游戏一个目录
├─ shared/                # 可选：unifont.otf 和 japanese-font.otf
├─ eagler-touhou.config.json  # 可选站点配置
├─ .cache/                # 构建工具与可复用输出缓存
└─ dist/                  # 生成的站点与 Package
```

需要配置 WebSocket Relay 或手动 Package 下载链接时，将 [`config/eagler-touhou.config.example.json`](../config/eagler-touhou.config.example.json) 复制为 `eagler-touhou.config.json`。Host 从本地目录读取原作资源；这些资源由运营者提供，不包含在仓库或 Runtime Release 中。

## 构建与检查

从源码检出在 `launcher/` 运行：

```sh
bun install --frozen-lockfile
bun run build
bun run host:doctor
bun run host:build
bun run verify:server -- dist/site
```

自托管包已包含构建好的 Launcher Host 文件。从包根目录安装依赖并运行 Host 检查：

```sh
bun install --frozen-lockfile
bun run host:doctor
bun run host:build
bun run verify:server -- dist/site
```

`bun run host:doctor` 在组装前检查所有 Runtime 和游戏输入。Host 准备游戏数据、可选音乐与语言包、封面、Launcher App Shell 和部署清单，生成的站点位于 `dist/site/`。

从源码检出或自托管包根目录运行以下命令，可在 `http://127.0.0.1:8130/` 本地提供站点：

```sh
bun run host -- --no-open
```

## 发布

将 `dist/site/` 内容复制到静态网站、对象存储或 CDN 的文档根目录，并通过 HTTPS 提供服务以启用浏览器 Service Worker 存储。然后验证公开地址：

```sh
bun run verify:deployed -- https://example.com/
```

若站点让玩家自行导入 Package，运行 `bun run import`；详情见[自托管参考](SELF_HOSTING_REFERENCE.md)。若大体积 Package 放在独立源站，按[外置资源模式](EXTERNAL_RESOURCE_MODE.md)配置。

源码检出的 `bun run host` 和 `bun run import` 使用同一套 Launcher Host 实现。维护者通过[发布流程](RELEASE.md)生成便携自托管包。
