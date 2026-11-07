# 自托管

使用 Launcher Host 命令，从已发布 Runtime Release 和依法持有的本地游戏安装资源构建静态站点。正式 Host 构建使用 Product Catalog 中全部注册游戏；单作开发使用 workspace 开发服务器。

## 准备输入

公开发布版默认使用游客模式。自行接入兼容的账号服务时，在构建前设置 `VITE_ACCOUNTS_ENABLED=true`，将本站 `/api/` 反向代理到自己的账号服务，并将 `/u/<用户名>` 路由到应用入口。本地开发可以用 `EAGLER_UI_API_ORIGIN` 指定 API 监听地址。匿名联机可以独立使用。

开放房间音乐时，在前端构建中设置 `VITE_LISTEN_TOGETHER_ENABLED=true`，并以 `VITE_METING_API_URL` 指定可从浏览器访问的 HTTPS Meting 接口。Relay 设置 `EAGLER_NETPLAY_LISTEN_TOGETHER=1` 后开放同步邀请。验收时用网易云音乐和 QQ 音乐分别检查搜索、歌单与播放，再用两个玩家席验证邀请和同步。

需要 Node.js 22+、Python 3 和匹配当前 Launcher Product Catalog 的完整 `runtime-release/`。源码前端构建另需 Bun 1.4.4。每个已登记游戏都准备 `games/<game-id>/` 目录，并放入对应原作文件。游戏列表见 [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs)。

在 `launcher/` 或自托管包根目录使用以下目录结构：

```text
host-root/
├─ package.json
├─ package-lock.json
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
bun install --frozen-lockfile --ignore-scripts
bun run build
npm run host:doctor
npm run host:build
npm run verify:server -- dist/site
```

自托管包已包含构建好的 Launcher Host 文件。从包根目录安装依赖并运行 Host 检查：

```sh
npm ci --ignore-scripts
npm run host:doctor
npm run host:build
npm run verify:server -- dist/site
```

`npm run host:doctor` 在组装前检查所有 Runtime 和游戏输入。Host 准备游戏数据、可选音乐与语言包、封面、Launcher App Shell 和部署清单，生成的站点位于 `dist/site/`。

从源码检出或自托管包根目录运行以下命令，可在 `http://127.0.0.1:8130/` 本地提供站点：

```sh
npm run host -- --no-open
```

## 发布

将 `dist/site/` 内容复制到静态网站、对象存储或 CDN 的文档根目录，并通过 HTTPS 提供服务以启用浏览器 Service Worker 存储。然后验证公开地址：

```sh
npm run verify:deployed -- https://example.com/
```

若站点让玩家自行导入 Package，运行 `npm run import`；详情见[自托管参考](SELF_HOSTING_REFERENCE.md)。若大体积 Package 放在独立源站，按[外置资源模式](EXTERNAL_RESOURCE_MODE.md)配置。

源码检出的 `npm run host` 和 `npm run import` 使用同一套 Launcher Host 实现。维护者通过[发布流程](RELEASE.md)生成便携自托管包。

## 测试版功能

右上角菜单的“开发人员选项”包含“显示调试信息”和“测试版功能”。测试版开关保存于本机，刷新当前页面后显示已发布的抢先体验作品。它只控制可见性，所有作品共用站点清单、Runtime、存档、录像与联机大厅。

按[发布流程](RELEASE.md)把隐藏作品和常规作品一起发布。`testOnly` 控制作品默认是否显示，`--test-build=1` 可让站点默认显示测试作品。账号与内部预览入口由各自的部署配置控制。

th13 的程序产物目前需显式提供：给 `scripts/package-runtime-release.mjs` 传入 `--th13-build=PATH`，组装站点时将 `th13` 加入 `--games`。它随同一站点发布，玩家开启测试版功能后即可看到。
