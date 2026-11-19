# 自托管参考

本文说明 Host 输入、配置、生成的静态站点和部署要求。构建步骤见[自托管指南](SELF_HOSTING.md)。

## 输入与配置

Host 读取 `runtime-release/`、所选作品的 `games/<game-id>/`、可选 `shared/unifont.otf` 和 `shared/japanese-font.otf`，以及可选 `eagler-touhou.config.json`。Runtime Release 必须与当前 Launcher 版本匹配。默认作品范围由 [Product Catalog](../../common/contracts/product-catalog.mjs) 的 Host 配方决定。使用 `npm run host:doctor` 检查启用作品和缺失输入。运营者自行提供合法取得的原作资源。

th05 使用已有的普通版与多人 Runtime Release，以及 `games/th05/assets-eagler/` 中的完整预制内容。该目录须包含 `th05.data`、全部声明的 `music/` 文件和 `artwork/th05-card.webp`。当前前端接入不包含 th05 的原生编译器与原始音源制作链；更新界面时沿用已发布的 Runtime 和内容。

花映塚个人主页和排行榜共用 `private-assets/th09-portraits/` 中的 `0.webp` 至 `15.webp`。把获准公开使用的立绘放在这里，Host 会将其发布到 `assets/th09-portraits/`，由页面按需加载。

排行榜和个人主页使用的机体立绘取自《東方花映塚 ～ Phantasmagoria of Flower View.》。原作及立绘版权归 ZUN（上海爱丽丝幻乐团）所有，本站用于标识机体。

配置 schema 为 `eagler-touhou/host-config/1`：

```json
{
  "schema": "eagler-touhou/host-config/1",
  "netplay": { "relay": "wss://relay.example.org/" },
  "externalImportSource": {
    "url": "https://downloads.example.org/packages/",
    "hint": "Download a package and import it in the Launcher."
  }
}
```

两个配置项均可选。`netplay.relay` 设置站点 WebSocket 地址。`externalImportSource` 提供玩家手动下载 Package 的链接；它与外置资源模式分开配置。默认文件为 [`config/eagler-touhou.config.example.json`](../config/eagler-touhou.config.example.json)。

## 可选功能

- **账号**。源码生产构建默认启用账号。将 `/api/` 反代到兼容的账号服务，并把 `/u/<用户名>` 路由到应用入口。游客站构建时设置 `VITE_ACCOUNTS_ENABLED=false`。本地开发使用 `VITE_ACCOUNTS_ENABLED=true` 和 `EAGLER_UI_API_ORIGIN`。
- **账号双线路**。构建时设置 `VITE_ACCOUNT_EDGE_ORIGIN` 和 `VITE_ACCOUNT_DIRECT_ORIGIN`。两个 HTTPS 入口共用账号服务和站点域的会话 Cookie，允许本站携带凭据跨域访问，账号响应使用 `Cache-Control: private, no-store`。`/api/ready` 通过 `X-Eagler-Account-Route` 标识 `direct` 或 `edge`，跨域入口需暴露该头。
- **房间音乐**。构建时可用 `VITE_METING_API_URL` 指定浏览器可访问的 HTTPS Meting 接口，用 `VITE_LISTEN_TOGETHER_ENABLED=false` 关闭房间音乐。Relay 设置 `EAGLER_NETPLAY_LISTEN_TOGETHER=1` 可启用同步邀请。验收时检查网易云音乐和 QQ 音乐的搜索、歌单与播放，再用两个玩家席验证同步。
- **抢先体验作品**。玩家在「开发人员选项」里打开「测试版功能」并刷新，即可看到已经发布的抢先体验作品。`testOnly` 控制默认可见性。

## 生成模式

| 模式 | Launcher 与 Runtime | 游戏 Package | 玩家操作 |
| --- | --- | --- | --- |
| `hosted` | 发布 | 发布在同一站点 | 从站点安装 |
| `external` | 发布在玩家访问的站点 | 重定向到配对的 Hosted 源站 | 从站点安装 |
| `import` | 发布 | 不发布在站点 | 导入游戏 Package ZIP |

`npm run host:build` 生成 Hosted 站点 `dist/site/`。`npm run package:external-site` 从其构建配套 External 站点。`npm run import` 生成 `dist/import-site/`，并把各作 Package ZIP 放入 `dist/import/`。输入变化后重新构建对应输出目录。

每个目标站点可在打包时传 `npm run host:build -- --no-download`，从已有 Hosted 清单和 Runtime Release 生成 `dist/import-site/`。直接调用 `scripts/package-server.mjs` 时使用 `--no-download`（或 `--no-download=1`），并传入 Hosted `--site-manifest`；使用 `tools/maintainer/assemble-site.ps1` 时传 `-NoDownload -SiteManifest`。开关关闭时采用常规的 `resourceMode` 配置。

External 部署使用同一 Hosted 版本构建的站点配对。重定向、CORS、Range 请求及配对发布步骤见[外置资源模式](EXTERNAL_RESOURCE_MODE.md)。

## 静态托管

将文件发布到站点配置的挂载路径。Launcher 和 Runtime HTML 响应须带 `Cross-Origin-Opener-Policy: same-origin` 与 `Cross-Origin-Embedder-Policy: require-corp`。确认代理和 CDN 也返回这些头。使用正确的 JavaScript、WebAssembly 和 Web App Manifest MIME 类型，保留查询参数，缺失文件返回 404。Launcher HTML、可变 JSON 清单和 App Shell worker 需要重新验证，带内容哈希的 Runtime 与 Package 可以使用 immutable 缓存。

单独替换 Launcher 时，先用目标站点的清单检查新前端是否仍支持已发布的作品：

```sh
node scripts/verify-frontend-game-catalog.mjs .cache/build/ui-rewrite/client path/to/site-manifest.json
```

External 模式中，玩家站将 `/games/**` 和 `/shared/**` 重定向到资源源站；资源源站允许玩家站 HTTPS Origin 并保留字节范围响应。公开站点验证器会按 External 模式检查这些路径。

th075 的站点语言 ZIP 可由玩家站直接发布。验证器按站点语言清单核对 `games/th075/language/` 下每个 ZIP 的路径、长度与完整 SHA-256，其余游戏数据和共享资源继续按 External 模式分发。

离线使用要求 HTTPS 或浏览器信任的 loopback 源站。玩家需先安装 Launcher Shell、所选 Runtime 和游戏 Package，该游戏才可离线运行。Relay 是独立 WebSocket 服务；完整检出可在 `launcher/` 运行 `npm run relay`。自托管包提供 Host 工作流，Relay 需单独部署和启动。

## 本地验证

```sh
npm run host:doctor
npm run host:build
npm run verify:server -- dist/site
npm run verify:deployed -- https://example.com/
```

源码检出中的部署示例位于 [`examples/deployment/`](../examples/deployment/)。按运营者服务器配置调整后，使用以上命令验证公开站点。
