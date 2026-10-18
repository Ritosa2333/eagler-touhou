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

原生浏览器检查按 `runtime-builds.json` 的源码目录绑定普通版和 MP，不要求 monorepo 的开发分支叫作旧的 `eagler` 或 `eagler-mp`。跨作品共用的编译器参数处理保留在根 `tools/build/`，相关 CI 检出该目录。

## 迁移候选限制

原工作区已提交的旁观与版本校验改动已按来源刷新。继续同步时逐处保留本仓库路径适配，不用整目录覆盖或在原工作区改依赖。

搬迁后的验收及具体阻塞见 [搬迁验收记录](migration/move-acceptance-20261018.md)。

旧上游分支不能直接合并到整个 monorepo。先核对上游提交范围，再按对应组件形成候选补丁或执行有明确前缀的导入，并复核普通版/MP 行为。

只有用户明确要求时才新建工作树。局部 clone 与含 SDL 子模块的 linked worktree 分开验收，不把前者通过当作后者已通过。
