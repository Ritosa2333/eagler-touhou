# 源码快照与构建资料

本仓库首先用于保存最新开发源码。它不包含原作资源、SDK、已构建 WASM 或浏览器运行清单，尚未在独立检出目录完成整套启动验收。

## 原生核心

已验收 private99 使用 Emscripten 4.0.22。`build/private99.rsp` 来自当次成功构建的参数，仅把本机绝对路径转换为仓库相对路径，并把输出放到 `build/local/core.mjs`。

它保留 83 个翻译单元、205 个导出符号、1 MiB 栈、64 MiB 初始内存、C++20、异常与浮点选项。源文件摘要记录在 `docs/native-source-manifest.json`。

后续开发者在独立环境配置对应 Emscripten 后，构建配方为：

```powershell
New-Item -ItemType Directory -Force build/local
emcc '@build/private99.rsp'
```

此配方在本次备份中仅做参数/文件检查，没有重新编译成功的 private99。构建输出不会自动发布到浏览器，也没有承诺跨机器生成完全相同的二进制摘要。

`tools/build-runtime.mjs` 是历史脚本，保留作研究参考；其导出清单和发布逻辑早于本快照，不作为 private99 的构建入口。

## 原作资源

资源准备工具需要 Python 3.11+、自行持有的原作安装和官方 thtk Windows release 12。示例路径需要替换为自己的安装位置：

```powershell
python -X utf8 tools/prepare.py --original '你的原作安装目录' --thtk '你的thtk工具目录' --variant both
```

资源在本地 `local/` 下生成，该目录已经被 Git 忽略。原版存档、成绩、录像和个人配置不属于此仓库内容。

## 浏览器宿主与测试

`site/` 保留最新完整前端候选源码，但新 EX API 的生产浏览器接线尚待验收。当前未提供一键部署流程；历史 `tools/serve.py` 与部分测试仍有旧版本假设，不能将启动旧脚本等同于验收新版本。

`tests/` 为保留的测试源码。部分测试依赖本地原版资源、未分发的私有黄金记录、编译器或运行时。缺失依赖导致的跳过不等于测试通过。

原有六线和 EX 成功证据没有因上传源码而重跑。当前快照的完整性检查只核对文件摘要、相对导入、编译参数和所选源码的语法。
