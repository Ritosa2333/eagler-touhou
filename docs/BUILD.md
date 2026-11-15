# 源码快照与构建资料

本仓库首先用于保存最新开发源码。它不包含原作资源、SDK、已构建 WASM 或浏览器运行清单，尚未在独立检出目录完成整套启动验收。

## 原生核心

private100 使用 Emscripten 4.0.22，已完成首次完整链接与隔离浏览器检查。`build/private100.rsp` 保留该版本的编译参数，将本机绝对路径转换为仓库相对路径，输出为 `build/local/core.mjs`。

它保留 83 个翻译单元、205 个导出符号、1 MiB 栈、64 MiB 初始内存、C++20、异常与浮点选项。源文件摘要记录在 `docs/native-source-manifest.json`。

后续开发者在独立环境配置对应 Emscripten 后，构建配方为：

```powershell
New-Item -ItemType Directory -Force build/local
emcc '@build/private100.rsp'
```

本源码检出目录中的相对路径配方完成了文件与参数核对；private100的实际构建在隔离验收目录完成。构建输出不会自动发布到浏览器，也不承诺跨机器生成完全相同的二进制摘要。

`tools/build-runtime.mjs` 是历史脚本，保留作研究参考；其导出清单和发布逻辑早于本快照，不作为 private100 的构建入口。

## 原作资源

资源准备工具需要 Python 3.11+、自行持有的原作安装和官方 thtk Windows release 12。示例路径需要替换为自己的安装位置：

```powershell
python -X utf8 tools/prepare.py --original '你的原作安装目录' --thtk '你的thtk工具目录' --variant both
```

资源在本地 `local/` 下生成，该目录已经被 Git 忽略。原版存档、成绩、录像和个人配置不属于此仓库内容。

## 浏览器宿主与测试

`site/` 包含最新前端候选与EX控制器；原生菜单、持久化和EX前段已分别在隔离浏览器中验证。`tools/serve.py` 已补齐新增模块的显式路由并通过现有7项服务器测试。完整终局与独立检出目录的一键部署流程仍待验收。

`tests/` 为保留的测试源码。部分测试依赖本地原版资源、未分发的私有黄金记录、编译器或运行时。缺失依赖导致的跳过不等于测试通过。

原有六线和EX成功证据保持不变。新增测试使用针对性组件、实际核心短序列和独立浏览器，不重复完整路线。

新增回归源码：

- `tests/native/player-hit-long-session.cpp`：真实PlayerHitHost正常计时65754帧后受击，另含数值与坐标边界；运行参数为本地原始pl00.anm。
- `tests/native/ice-item-motivation-notice.cpp`：真实ItemHost与PerfectHud的拾取通知测试；先用 `node tests/extract-item-collection.mjs <隔离输出目录>` 提取当前完整生产回调，运行参数为本地front.anm和bullet.anm。
- `tests/native-extra-run-lifecycle.mjs`：生产EX控制器的顺序、取消、渲染失败及严格收据测试；通过 `TH128_ACCEPTANCE_ROOT` 指向已验收的本地case目录，以只读复用541档案和对应元数据。此测试的native与device接口明确为模拟实现。
