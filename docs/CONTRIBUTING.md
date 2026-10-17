# 贡献方式

## 只改启动器

在 GitHub fork 仓库后，可以只下载启动器文件并直接向主仓库提交 PR：

```sh
git clone --filter=blob:none --sparse <fork-url> eagler-touhou
cd eagler-touhou
git sparse-checkout set launcher
git switch -c <branch>
npm --prefix launcher ci --ignore-scripts
npm --prefix launcher run vendor
npm --prefix launcher run check
npm --prefix launcher run dev:ui-rewrite
```

这一流程不要求获取游戏源码、SDK 或 DATA。普通 `npm start` 是具备游戏内容的工作区联调入口。

## 修改作品或 common

使用 `git sparse-checkout add common runtimes/<作品> tools` 扩大范围。普通版和 MP 已独立的作品同时选中两份源码，并按 `launcher/config/workspace.json` 核对实际目录。

统一源码构建入口为：

```sh
node launcher/scripts/build-runtime.mjs --game=th08 --variant=normal --emsdk=<SDK> --dry-run
node launcher/scripts/build-runtime.mjs --game=th08 --variant=multiplayer --emsdk=<SDK> --dry-run
```

确认计划后去掉 `--dry-run`。默认直接使用仓库 `common/`，不需要更新每作的 common 版本号。成功构建会在忽略的 `artifacts/validation/source-builds/` 记录实际源码、依赖和工具链来源。

TH06/TH07 所需 SDL 等外部依赖仍按根 `.gitmodules` 初始化当前作品所需路径。原始游戏、字体和其他私有输入由参数或现有环境变量显式提供，不放入源码。

普通版与 MP 保持独立语义。提交或 PR 简短说明 MP 已适配、无需修改的原因，或明确未完成项。公共行为说明和必要差异只维护一处，不抄写逐提交日志。

common 改动需要验证全部实际消费者。根 CI 汇总保留的适配检查，但来源计划、编译、打包和游戏行为是不同层次的证据，不能互相代替。

## 迁移候选限制

当前导入点不包含原工作区尚未提交的旁观修改。需要在其完成后按来源提交刷新，并逐处保留本仓库的路径适配。不要用整目录覆盖或在原工作区改依赖来同步。

旧上游分支不能直接合并到整个 monorepo。先核对上游提交范围，再按对应组件形成候选补丁或执行有明确前缀的导入，并复核普通版/MP 行为。

只有用户明确要求时才新建工作树。局部 clone 与含 SDL 子模块的 linked worktree 分开验收，不把前者通过当作后者已通过。
