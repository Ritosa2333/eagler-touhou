# 工作区

在 monorepo 根目录选组件。开发启动器、编译游戏和发布站点各有独立入口。

| 要做什么 | 从哪里开始 |
| --- | --- |
| 自托管、上线或更新网站 | [部署指南](launcher/docs/SELF_HOSTING.md) |
| 从输入组装正式发布候选 | [发布候选](launcher/docs/RELEASE.md) |
| 改启动器或联调单作 | [启动器开发](launcher/docs/DEVELOPMENT.md)，源码在 `launcher/` |
| 编译或适配游戏 | [各作开发指南](ports/README.md)，源码在 `ports/` |
| 改共享运行库、浏览器桥或协议 | [公共库](common/README.md)，源码在 `common/` |
| 部署大厅信令和穿透中继 | [Relay](services/relay/deploy/README.md) 和 [TURN](services/turn/README.md) |
| 部署下载分流或遥测 | [资源路由](services/resource-router/README.md) 和 [遥测](services/telemetry/README.md) |

## 找到正确的游戏源码

以下命令从仓库根目录运行。普通版和多人版可能共用源码，也可能分目录。先看实际路径和各自的构建配方。

```sh
node tools/workspace.mjs list
node tools/workspace.mjs paths th10 --variant=both
node tools/workspace.mjs select th10 --variant=both
```

`list` 和 `paths` 读取[目录映射](launcher/config/workspace.json)和[构建配方](launcher/config/runtime-builds.json)。`select` 将对应目录加入已有的稀疏检出，完整检出可直接查看路径。首次检出见[贡献指南](CONTRIBUTING.md)。向独立作品仓库同步时，查[来源历史](docs/history/README.md)和[普通版／多人版同步流程](docs/adaptation/port-sync.md)。

## 区分源码、输入和构建结果

| 路径 | 内容 |
| --- | --- |
| `common/contracts/`、`common/browser/`、`common/include/eagler/` | 共享契约、浏览器桥和 C++ 公共运行库 |
| `launcher/runtime-release/`、`launcher/games/` | 从源码运行 Host 时的程序包和本地原作输入 |
| `launcher/dist/site/` | 从源码运行 Host 生成的 Hosted 站点 |
| `launcher/.cache/`、`artifacts/validation/source-builds/` | 构建缓存和源码构建记录 |
| `archive/` | 历史实验、验证和发布记录 |

SDK、子模块、字体等输入按各作开发指南准备。

## 验证改动

文档运行 `npm run docs:check`。游戏变更运行各自的构建与联调检查。可用 `npm run ports:check -- --all` 检查源码契约，用 `npm run ports:status -- --all` 更新[适配记录](docs/adaptation/README.md)。服务在各自目录执行测试，完整入口见[文档索引](docs/INDEX.md)。
