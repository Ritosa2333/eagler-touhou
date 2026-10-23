# 参与开发

先选择一项工作，运行对应组件，再开始修改。需要 Git 和 Node.js 24（最低 22.18）；游戏编译所需的工具链在各作开发指南中列出。

## 1. 取需要的目录

在 GitHub 上 fork 本仓库。把下面 URL 的 `YOUR-NAME` 换成你的账号：

```sh
git clone --filter=blob:none --sparse https://github.com/YOUR-NAME/eagler-touhou.git
cd eagler-touhou
git sparse-checkout set launcher common tools docs
git remote add upstream https://github.com/YomotsuHisami/eagler-touhou.git
git switch -c your-change
```

| 工作 | 接下来的命令或文档 |
| --- | --- |
| 前端 | `npm --prefix launcher ci --ignore-scripts`，然后 `npm run dev` |
| 前端与 th10 | `node tools/workspace.mjs select th10`，然后读[本地联调](launcher/docs/DEVELOPMENT.md) |
| th10 普通版与 MP | `node tools/workspace.mjs select th10 --variant=both` |
| Relay | `git sparse-checkout add services/relay`，然后读[服务开发](services/relay/README.md) |
| 公共运行库 | [common 开发与检查](common/README.md) |

`select` 只增加所需目录。其他作品的实际路径可用 `node tools/workspace.mjs list` 查看。

## 2. 跑起当前版本

只改页面时运行前端开发服务器。需要游戏时，按[联调指南](launcher/docs/DEVELOPMENT.md)依次完成 Runtime 构建、打包、资源准备和启动。

记录所用作品、模式和构建。普通版与 MP 的目录和输出关系见[工作区](WORKSPACE.md)。

## 3. 修改并填写适配表

每次修改入口、命令或行为，在同一改动中更新对应文档和适配记录。

修改游戏时，打开该目录的 `ADAPTATION.md`，按[适配流程](docs/adaptation/README.md)找到对应项目。通用改动同时检查普通版与 MP；模式专属改动在记录中写明适用模式。

普通版修改同时判断多人版的适用性，并将来源和多人版处理写入[同步历史](docs/adaptation/port-sync.md)。独立目录通过 `tools/ports/sync.mjs` 按各自基线进行三方同步，源码和历史一并提交。

修改共用代码时，用[目录映射](launcher/config/workspace.json)和[构建配方](launcher/config/runtime-builds.json)找出受影响的游戏。接口定义、调用方和对应测试在同一改动中更新。

## 4. 检查

在仓库根目录运行：

```sh
npm run docs:check
npm run typecheck
npm run build
npm test
```

以上包含文档和前端检查。游戏改动再运行该作开发指南中的编译与行为检查；Relay 等服务运行自身测试。

新建或移动文档后，运行 `npm run docs:index` 更新[全部文档表](docs/INDEX.md)，再执行 `npm run docs:check`。验收记录使用[模板](docs/adaptation/record-template.md)。

## 5. 提交 PR

确认 `git diff` 中的内容与本次工作一致，提交到你的分支并创建 PR。正文说明改了什么、影响哪些作品和模式、运行了哪些检查，并链接相关适配表和记录。

本项目的自动化代理在发布提交说明、PR 和评论前，将完整文字交由维护者审阅。

旧仓库改动转入 monorepo 时，按[历史与迁移](docs/history/README.md)找到原源码归属，再把差异应用到当前组件目录。
