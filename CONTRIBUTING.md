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

新增作品目录时，在 `docs/migration/` 登记原仓库提交、子树连接点与导出基点，并运行 `python tools/ports/check-history.py --base upstream/main`。该检查同时核对已登记来源提交的祖先关系和新目录的来源登记。[th12 子树](docs/migration/th12-subtree-20261024.md)展示单目录作品的导出与同步命令。

普通版修改默认进入 MP 同步范围。日常代码在 monorepo 开发分支中正常提交，来源提交与 MP 的处理决定保存在[同步历史](docs/adaptation/port-sync.md)。th08、th10、th11、th15 的同步维护者按下方流程保留 Git 祖先。

### 普通版与 MP 怎么提交

| 本次工作 | 提交方式 |
| --- | --- |
| 修改普通版 | 提交普通版代码与文档，刷新该作目录历史，让 MP 维护者查看新增提交 |
| 修改 MP 自身逻辑 | 提交 MP 代码与文档，刷新该作目录历史 |
| 将普通版同步到 MP | 从该作 MP 历史分支合并普通版历史，处理冲突，用写回工具接回 monorepo，再提交同步记录 |
| 同时修改普通版与 MP，或手工迁入普通版修改 | 完成代码提交后，由同步维护者完成上述历史合并和写回，并登记各项处理决定 |

例如 th10 的历史查询在仓库根目录运行：

```sh
python tools/ports/project-history.py --repo . --game th10 --write-refs --track-normal
git branch -vv --list 'port-history/*'
git log --oneline port-history/th10mp..port-history/th10
```

日常开发使用原目录和开发分支。`port-history/*` 是各作的目录历史，合并同步时以 MP 分支为起点。`sync.mjs record` 登记处理进度，目录历史合并建立 Git 祖先，两者随同步一起维护。完整操作见[下一次提交与同步](docs/adaptation/port-sync.md#下一次提交与同步)。

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

作品目录与普通版到 MP 历史同步的 PR 使用 **Create a merge commit**，保留来源提交和同步节点的父链。合入后刷新目录历史或子树导出端，确认原仓库提交仍是祖先。主分支要求 `Checks` 通过，其中包含 `Source history`。

分支经过 rebase 或 squash 后，先从本地 reflog 或 PR 提交记录找回改写前通过 `Source history` 的提交。核对当前目录与该提交的内容；内容已完整保留时，在修复分支合并旧提交以接回父链，内容有差异时正常合并并逐项处理。最后运行 `python tools/ports/check-history.py --base <目标分支>`，确认祖先和导出树。不要用纯文件复制代替历史连接。

本项目的自动化代理在发布提交说明、PR 和评论前，将完整文字交由维护者审阅。

旧仓库改动转入 monorepo 时，按[历史与迁移](docs/history/README.md)找到原源码归属，再把差异应用到当前组件目录。
