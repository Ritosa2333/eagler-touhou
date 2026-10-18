# Git 管理约定

仓库位于工程根目录，主分支为 `main`。源码、CMake/PowerShell/Python 工具、可编辑脚本、验证报告及文本分析证据纳入版本管理。

构建目录、`dist/`、原始资产、第三方下载、反编译数据库、运行存档和大型诊断转储保留在本地，由 `.gitignore` 排除。少量 ASAN 错误报告和状态快照作为修复证据保留。恢复这些本地依赖的方法见 `SOURCE_BUILD.md` 和对应工具脚本。

`.gitattributes` 禁用换行转换，因为验证报告引用源文件的精确 SHA-256；编辑文件时也应保留原编码和换行。报告只证明其记录版本，修改源码后应按影响范围重跑验证。

日常提交前先检查内容：

```powershell
git status --short
git diff
git add <本次修改的文件>
git diff --cached --stat
git commit -m "说明这次改动"
```

新功能或较大的恢复工作可使用 `git switch -c <分支名>`。本次只建立本地仓库，远程地址可在选定托管位置后配置。

`source_reconstruction/audit/compile_probe/` 的未修改伪代码副本是本地生成物。运行需要它的审计前，先按 `source_reconstruction/audit/README.md` 的步骤生成；其他文本分析证据仍保留在 `analysis/` 的已跟踪子目录中。
