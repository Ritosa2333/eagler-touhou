# 启动器结构与职责

启动器负责作品选择、游戏包、浏览器存储和页面导航。Runtime 运行游戏逻辑。所有作品共用 `launcher/` 中的前端和 Host。

## 源码归属

| 目录 | 职责 |
| --- | --- |
| `launcher/app/`、`launcher/src/` | React 页面、路由和浏览器逻辑 |
| `common/contracts/` | 前端与服务共用的产品和协议定义 |
| `launcher/src/contracts/` | 上述定义的 TypeScript 接口 |
| `launcher/package/` | 游戏包导入、存储、更新与 ZIP 处理 |
| `launcher/host/` | 按输入装配可部署的站点 |
| `launcher/integrations/` | 字体、汉化及外部资源格式 |
| `launcher/server/`、`launcher/scripts/serve.mjs` | 本地 HTTP 与 THCRAP 服务 |
| `launcher/lib/` | Node 构建、策略和产物模块 |
| `launcher/scripts/` | 命令行入口 |
| `launcher/public/` | 浏览器静态资源 |
| `ports/<owner>/` | 各作游戏源码及 Runtime 构建、打包 |
| `common/` | 各作共用运行库 |
| `tools/`、`.github/workflows/` | 全仓构建工具与 CI |

表中路径均从仓库根目录定位。

[`workspace.json`](../config/workspace.json) 映射源码目录；[`runtime-builds.json`](../config/runtime-builds.json) 声明构建模式和命令。TH06、TH07、TH09 的普通版与 MP 共用各自源码目录；TH08、TH10、TH11、TH15 的两种模式分目录维护。TH20 提供普通版。

## 从源码到运行

1. `build:runtime` 按配方编译游戏，记录源码与构建信息。
2. 该作打包脚本收集 HTML、JavaScript、WASM 和 Runtime 资源。
3. 前端选择游戏包，准备 DATA、语言和音乐组件。
4. Host 提供页面和 Runtime；浏览器会话加载所选作品。
5. 存档与 Replay 写入浏览器的可变存储，随游戏包更新继续保留。

批量发布 Runtime 使用 `scripts/package-runtime-release.mjs`；装配部署站点使用 `host/build.mjs`。本地开发需要前端构建、所选 Runtime 和本地游戏包，作品由 `EAGLER_DEVELOPMENT_GAMES` 选择。

## 命令入口

| 命令（从 `launcher/` 运行） | 结果 |
| --- | --- |
| `npm run dev` | 启动带热更新的 React 开发服务器 |
| `npm run build` | 准备锁定版本的浏览器库，构建内容页面、客户端模块和 React 产物 |
| `npm run typecheck`、`npm test` | 检查前端类型与行为 |
| `npm run check` | 检查启动器核心模块 |
| `npm run check:workspace` | 检查跨作品 Runtime 集成 |

具体输入和步骤见[本地联调](DEVELOPMENT.md)与[测试入口](../tests/README.md)。脚本职责见[命令目录](../scripts/README.md)。
