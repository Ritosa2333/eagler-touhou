# 自托管参考

本文说明 Host 输入、配置、生成的静态站点和部署要求。构建步骤见[自托管指南](SELF_HOSTING.md)。

## 输入与配置

Host 读取 `runtime-release/`、Product Catalog 中每个注册游戏的 `games/<game-id>/`、可选 `shared/` 字体和可选 `eagler-touhou.config.json`。Runtime Release 必须匹配当前 Launcher Catalog。Host 构建包含所有注册游戏；单作开发使用 workspace 开发服务器。

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

## 生成模式

| 模式 | Launcher 与 Runtime | 游戏 Package | 玩家操作 |
| --- | --- | --- | --- |
| `hosted` | 发布 | 发布在同一站点 | 从站点安装 |
| `external` | 发布在玩家访问的站点 | 重定向到配对的 Hosted 源站 | 从站点安装 |
| `import` | 发布 | 不发布在站点 | 导入游戏 Package ZIP |

`npm run host:build` 生成完整 Hosted 站点 `dist/site/`。`npm run package:external-site` 从其构建配套 External 站点。`npm run import` 生成 `dist/import-site/`，并在 `dist/import/` 下为每个注册游戏生成一个 ZIP。输入变化后，重新构建这些输出目录。

External 部署使用同一 Hosted 版本构建的站点配对。重定向、CORS、Range 请求及配对发布步骤见[外置资源模式](EXTERNAL_RESOURCE_MODE.md)。

## 静态托管

将文件发布到站点配置的挂载路径。服务器返回正确的 JavaScript、WebAssembly 和 Web App Manifest 内容类型，保留请求查询参数，并支持站点缓存策略。HTML、JSON 清单和 App Shell worker 需要重新验证；带版本的 Runtime 与 Package 资源可设为 immutable 缓存。`npm run verify:server -- dist/site` 检查本地目录；`npm run verify:deployed -- https://example.com/` 检查公开部署。

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
