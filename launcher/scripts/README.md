# 命令行脚本

`scripts/` 存放命令行入口。可复用策略与组装逻辑放在 `lib/`，测试放在 `tests/`。

## 贡献者命令

以下命令从 `launcher/` 运行：

```sh
npm run dev
npm run build
npm run typecheck
npm test
```

`dev` 启动 React Router 前端。`build` 更新 vendored 浏览器资源和生成内容、编译客户端服务并构建前端产物。`typecheck` 检查应用和构建工具 TypeScript。`test` 运行 React 应用测试集。

`npm start` 和 `npm run dev:workspace` 运行 `serve.mjs`，提供已构建前端和 workspace 开发元数据。稀疏检出时，将 `EAGLER_DEVELOPMENT_GAMES` 设为一个作品编号。

## 构建与发布入口

- `build-runtime.mjs` 按 `config/runtime-builds.json` 构建作品源码；参数为 `--game`、`--variant`、`--emsdk`、`--dry-run`。
- `package-runtime-release.mjs` 将已打包的 Runtime 目录组装为 Runtime Release。
- `build-launcher.mjs`、`build-content-pages.mjs` 和 `build-app-shell.mjs` 分别构建前端、撰写页面和 App Shell 输入。
- `host/build.mjs` 使用已准备输入组装 Host 站点；`host/build-import.mjs` 构建本地导入模式。
- `serve.mjs` 启动本地集成开发服务器。
- `check.mjs` 执行 Launcher 仓库检查；`audit-publication.mjs` 检查公开源码树。

常用工作流优先使用 `package.json` 中的命令入口。脚本需要专属参数或没有稳定命令别名时，直接运行脚本。源码归属和 Runtime 流程见[开发指南](../docs/DEVELOPMENT.md)与[架构说明](../docs/ARCHITECTURE.md)。
