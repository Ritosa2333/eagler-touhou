# 自托管

先根据手头已有的产物找入口。

| 你已有的文件 | 下一步 |
| --- | --- |
| 已组装的静态站点，包含 `index.html`、`site-manifest.json` 和 `runtime-manifest.json` | 直接[发布静态站点](#发布静态站点) |
| `EaglerTouhou-SelfHost.zip` | 解压后[组装站点](#组装站点) |
| 源码或单独的 `EaglerTouhou-Runtime.zip` | [准备源码构建输入](#准备源码构建输入)，再组装站点 |

## 组装站点

在自托管包根目录或源码检出的 `launcher/` 中准备 Node.js 22.18+ 和 Python 3。Linux／macOS 准备可从命令行运行的 `thdat` 和 `thmsg`。Windows 的 Host 工具可按需准备 thtk。

自托管包已附带配套的 `runtime-release/`。从源码构建时把配套 Runtime 放到此目录，确保其中直接包含 `runtime-release.json`。把各作原作文件放入 `games/<game-id>/`。运行检查后按输出补齐具体文件。原作资源和额外字体见[输入参考](SELF_HOSTING_REFERENCE.md#输入与配置)。

使用自托管包时，先在解压后的包根目录运行 `npm ci --ignore-scripts`。从源码来时，先完成下面的[前端构建](#准备源码构建输入)。然后在同一目录组装：

```sh
npm run host:doctor
npm run host:build -- --site-url=https://play.example.org/
npm run verify:server -- dist/site
```

如果系统只提供 `python3` 命令，在运行 `host:build`、`host` 或 `import` 时加上 `--python=python3`，放在 npm 的 `--` 后。

将示例 URL 换成最终访问地址。Hosted 模式生成的 `dist/site/` 就是要发布的目录，玩家可以直接安装游戏。仅在本机试用时，可运行 `npm run host -- --no-open`，打开 <http://127.0.0.1:8130/>。

如果游戏数据放在另一个域名，使用[配对的 External 站点](EXTERNAL_RESOURCE_MODE.md)。如果由玩家自行导入 ZIP，运行 `npm run import -- --site-url=https://play.example.org/`，站点发布目录是 `dist/import-site/`，本地 ZIP 输出在 `dist/import/`。

有些站点只接受本地导入时，用同一份 Hosted 构建和 `runtime-release/` 为这些站点单独打包：

```sh
npm run host:build -- --no-download --site-url=https://example.invalid/ --output=dist/site-a
npm run verify:server -- dist/site-a
```

发布 `dist/site-a/`；省略 `--output` 时输出到 `dist/import-site/`。产物包含 Launcher 和单人／多人 Runtime，站点不提供游戏数据、音乐、语言包或备用下载链接。玩家可导入完整游戏包；直接导入原版资源时，可将 `msgothic.ttc` 和 `unifont.otf` 一并选入，或重用浏览器里已导入的字体。该命令读取 `dist/site/site-manifest.json` 取得兼容性信息，不会重建 Hosted 站点或导出游戏 ZIP。其它站点打包时省略 `--no-download`。要同时生成本地导入 ZIP，运行 `npm run import`；大体积 Package 需要放在独立源站时，参见[外置资源模式](EXTERNAL_RESOURCE_MODE.md)。

## 准备源码构建输入

源码构建需要 Bun 1.4.4。下面从 monorepo 根目录进入 `launcher/` 后运行。自托管游客站可关闭账号功能：

```sh
cd launcher
bun run install:locked
VITE_ACCOUNTS_ENABLED=false bun run build
```

在 PowerShell 中用 `$env:VITE_ACCOUNTS_ENABLED = 'false'` 设置同一个变量，再执行 `bun run build`。接入账号或房间音乐见[可选功能](SELF_HOSTING_REFERENCE.md#可选功能)。

配套 Runtime 可向维护者取得，也可查看 [GitHub Releases](https://github.com/Ritosa2333/eagler-touhou/releases) 是否有该版本的 Runtime 附件。解压后使用其中的 `runtime-release/`。

自行编译时，在 `launcher/` 用 `node scripts/list-product-games.mjs` 列出需要构建的作品，按[各作指南](../../ports/README.md)准备 Runtime。运行 `npm run package:runtime-release -- --output=runtime-release` 时，加上每作的 `--<game>-build=PATH`。单独构建多人版的作品还要传 `--<game>-multiplayer-build=PATH`。这些路径指向各作已打包的 Runtime 目录。打包成功会替换输出目录，保有旧版时请先用新的输出路径。见[打包入口](../scripts/package-runtime-release.mjs)。

## 发布静态站点

1. 把站点目录的**内容**复制到 HTTPS 文档根目录，确保根 URL 能读到 `index.html`。Linux + Nginx 可参考[部署示例](../examples/deployment/README.md)。
2. 站点和 Runtime 页面都要提供跨源隔离响应头。确保 `.mjs`、`.wasm`、`.webmanifest` 使用正确的 MIME 类型，缺失文件返回 404。[静态托管要求](SELF_HOSTING_REFERENCE.md#静态托管)
3. 从 `launcher/` 或含验证脚本的自托管包根目录检查实际地址：

   ```sh
   npm run verify:deployed -- https://play.example.org/
   ```

4. 用新的浏览器配置打开站点，确认 Launcher 和 Runtime 页面的 `crossOriginIsolated` 为 `true`，实际安装并启动一作，再断网重开。更新既有站点时先验证新版本目录，再整体切换，上一版供回滚。

## 启用多人联机

先部署独立的 [Relay](../../services/relay/deploy/README.md)，让浏览器能经 HTTPS 域名建立 WSS 连接。需要在玩家无法直连时中继游戏流量的站点，再部署 [TURN](../../services/turn/README.md)。

在 Host 根目录复制[配置示例](../config/eagler-touhou.config.example.json)，保存为 `eagler-touhou.config.json`，把 `netplay.relay` 填成自己的 WSS 地址并重新组装。验收时从两个网络开房并开局，检查直连和 TURN，分别验证房间配额隔离与离房释放。
