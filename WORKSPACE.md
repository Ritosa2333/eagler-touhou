# 工作区

本仓库同时维护启动器、游戏、公共运行库和独立服务。各组件按自己的入口运行；贡献分支从本仓库主线创建。

## 目录与构建模式

| 游戏 | 普通版源码 | MP 源码 | 构建方式 |
| --- | --- | --- | --- |
| th06 | `ports/th06` | `ports/th06` | 共用源码，分别构建 normal 与 multiplayer |
| th07 | `ports/th07` | `ports/th07` | 共用源码，分别构建 normal 与 multiplayer |
| th08 | `ports/th08` | `ports/th08-mp` | 分目录、分模式构建 |
| th09 | `ports/th09` | `ports/th09` | 共用生产程序，保留普通版与 MP 产品身份 |
| th10 | `ports/th10` | `ports/th10-mp` | 分目录、分模式构建 |
| th11 | `ports/th11` | `ports/th11-mp` | 分目录、分模式构建 |
| th15 | `ports/th15` | `ports/th15-mp` | 分目录、分模式构建 |
| th20 | `ports/th20` | — | 普通版实验适配 |

目录来自 [`launcher/config/workspace.json`](launcher/config/workspace.json)，构建命令来自 [`launcher/config/runtime-builds.json`](launcher/config/runtime-builds.json)。`node tools/workspace.mjs list` 直接读取这两份配置。

## 公共依赖

`common/` 是默认共享源码。客户端与服务共用的纯协议和产品定义位于 `common/contracts/`；浏览器桥位于 `common/browser/`；C++ 输入、联机等组件位于 `common/include/eagler/`。

TH06、TH07 的 SDL 依赖按根 `.gitmodules` 取对应游戏的子模块。SDK、原版资源、字体和构建输出放在开发指南指定的本地目录。

| 本地目录 | 内容 |
| --- | --- |
| `toolchains/` | SDK 与编译工具 |
| `dependencies/` | 额外字体与构建依赖 |
| `games/` | 自己提供的原版资源和准备结果 |
| `artifacts/` | 构建记录与验证输出 |
| `launcher/.cache/` | 前端编译和内容准备缓存 |

## 按需检出

[贡献指南](CONTRIBUTING.md)给出完整 clone 命令。已建立稀疏检出后，可增加单个作品和相关模式：

```sh
node tools/workspace.mjs select th08 --variant=both
```

完整检出使用 `git sparse-checkout disable`。旧的 `eagler`、`eagler-mp` 分支身份保存在[来源记录](docs/history/README.md)中，当前源码位置按上表选择。

## 检查入口

根 `.github/workflows/` 是 CI 入口。前端、游戏和服务分别执行自己的检查，文档和适配表共用 `npm run docs:check`。`npm run ports:check -- --all` 检查各作各模式的源码输入和实现，`npm run ports:status -- --all` 更新[适配表与检查记录](docs/adaptation/README.md)。构建成功的来源记录位于 `artifacts/validation/source-builds/`。
