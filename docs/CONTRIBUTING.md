# 贡献方式

## 只改启动器

在 GitHub fork 仓库后，可以只下载启动器文件并直接向主仓库提交 PR：

```sh
git clone --filter=blob:none --sparse <fork-url> eagler-touhou
cd eagler-touhou
git sparse-checkout set launcher common/contracts services
git switch -c <branch>
npm --prefix launcher ci --ignore-scripts
npm --prefix services/relay ci --ignore-scripts
npm --prefix launcher run vendor
npm --prefix launcher run check
npm --prefix launcher run dev:ui-rewrite
```

这一流程不要求获取游戏源码、SDK 或 DATA。公共协议和服务源码用于保留启动器的真实集成测试。普通 `npm start` 是具备游戏内容的工作区联调入口。

## 修改独立服务

Relay 只需要 `services/relay/` 和 `common/contracts/`，可在服务目录运行 `npm ci`、`npm test`、`npm run build` 和 `npm run package -- <新目录>`。生产发布包只安装其自身锁定的 `ws` 依赖，启动不需要 Launcher、前端缓存或 TypeScript 编译器。其他壳按服务的 [协议说明](../services/relay/docs/protocol.md) 接入。

`services/telemetry/` 是无第三方依赖的 HTTP 服务。`services/turn/` 保存 coturn 配置渲染和部署支持。它们均有独立 CI 检查。启动器保留的 `server/netplay-relay.mjs` 仅为旧本地探针的转发入口，实际服务只维护在 `services/relay/src/`。

## 修改作品或 common

使用 `git sparse-checkout add common ports/<作品> tools` 扩大范围。普通版和 MP 已独立的作品同时选中两份源码，并按 `launcher/config/workspace.json` 核对实际目录。

统一源码构建入口为：

```sh
node launcher/scripts/build-runtime.mjs --game=th08 --variant=normal --emsdk=<SDK> --dry-run
node launcher/scripts/build-runtime.mjs --game=th08 --variant=multiplayer --emsdk=<SDK> --dry-run
```

确认计划后去掉 `--dry-run`。默认直接使用仓库 `common/`，不需要更新每作的 common 版本号。成功构建会在忽略的 `artifacts/validation/source-builds/` 记录实际源码、依赖和工具链来源。

TH06/TH07 所需 SDL 等外部依赖仍按根 `.gitmodules` 初始化当前作品所需路径。原始游戏、字体和其他私有输入由参数或现有环境变量显式提供，不放入源码。

普通版与 MP 保持独立语义。提交或 PR 简短说明 MP 已适配、无需修改的原因，或明确未完成项。公共行为说明和必要差异只维护一处，不抄写逐提交日志。

common 改动需要验证全部实际消费者。根 CI 汇总保留的适配检查，但来源计划、编译、打包和游戏行为是不同层次的证据，不能互相代替。

完整源码检出后可运行 `node launcher/scripts/audit-publication.mjs --workspace`。已审阅资源的精确内容和许可证据由 `docs/migration/reviewed-source-assets.json` 约束，新增或变化的资源需要单独核对。游戏画面和原版数据通过显式外部 Host 输入提供。

使用 `tests/support/runtime-source-owner.mjs` 的原生浏览器检查按 `runtime-builds.json` 的源码目录绑定普通版和 MP，不要求 monorepo 的开发分支叫作旧的 `eagler` 或 `eagler-mp`。本轮实际验收入口与未覆盖的旧审计入口列在搬迁验收记录中。跨作品共用的编译器参数处理保留在根 `tools/build/`，相关 CI 检出该目录。

编译测试的依赖清单同时记录作品和根 common 的真实文件，使用 `tools/build/source-dependencies.mjs` 核对归属，仍拒绝其他作品、SDK 或无关目录。源码资源审阅对文本按 LF 规范化校验，对二进制保留逐字节校验；Windows 换行不能成为 Linux 检出时放宽资源白名单的理由。

## 迁移前的 PR 与工作树

现有 PR、分支和工作树继续保留。此次迁移只继承来源记录中选定的提交，不代表已接受其他分支的独立工作。无需为了搬迁先合入所有旧 PR；明确要求本次包含的改动，应先完成审核，再刷新候选和相关验收。

适配旧分支前，检查并保留未提交内容，记录旧基线和分支头。按对应组件的旧祖先审阅增量，例如 TH08 MP 的改动以原 MP 分支为基线，不能拿普通版或启动器作为祖先。

| 旧改动归属 | 新位置 |
| --- | --- |
| 启动器界面、配置、测试和构建配方 | `launcher/` 下的对应路径 |
| Relay 实现、测试和独立构建 | `services/relay/` |
| TURN 支持、Telemetry 服务 | `services/turn/`、`services/telemetry/` |
| 客户端和服务共用的中立定义 | `common/contracts/`，其他启动器定义仍在 `launcher/` |
| 作品适配 | `ports/<作品>`，普通版和 MP 分别归属 |
| 跨作品运行时基础设施 | `common/` |

Git 可能识别目录移动，但新增文件位置、修改/删除和内容冲突仍需逐处审阅。旧分支自带的根 CI、工作区映射和内嵌 common 不能覆盖新仓库规则。完成适配后运行受影响组件的检查；能够生成合并树不等于功能验收通过。

远端更新不会自动改动本地工作树。获取远端引用后，各工作树仍保留自己的分支和文件；更新分支或合入改动时才处理新目录。不要为迁移批量切换、清理或删除旧工作树。

## 迁移候选限制

原工作区已提交的旁观与版本校验改动已按来源刷新。继续同步时逐处保留本仓库路径适配，不用整目录覆盖或在原工作区改依赖。

搬迁后的验收及具体阻塞见 [搬迁验收记录](migration/move-acceptance-20261018.md)。

旧上游分支不能直接合并到整个 monorepo。先核对上游提交范围，再按对应组件形成候选补丁或执行有明确前缀的导入，并复核普通版/MP 行为。

只有用户明确要求时才新建工作树。局部 clone 与含 SDL 子模块的 linked worktree 分开验收，不把前者通过当作后者已通过。
