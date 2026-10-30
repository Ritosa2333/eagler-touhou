# 源码与构建

[适配流程](README.md) | [文档目录](../README.md)

先选作品和模式，再准备工具链。各作的额外依赖和输出目录见其 `docs/DEVELOPMENT.md`。

## 准备

在仓库根目录运行：

```sh
node tools/workspace.mjs list
node tools/workspace.mjs select th10 --variant=both
node launcher/scripts/build-runtime.mjs --game=th10 --variant=normal --emsdk=/path/to/emsdk --dry-run
```

将 SDK 路径换成本机路径；确认计划后去掉 `--dry-run` 执行编译。MP 使用 `--variant=multiplayer`。

TH06、TH07 共目录双构建；TH08、TH10、TH11、TH15 分普通版和 MP 目录；TH09 共目录并保留两种产品身份。

## 工具链

使用仓库外或被忽略的 `toolchains/` 目录安装官方 [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)。Windows、Linux 和 macOS 共用同一源码构建入口，通过 `--emsdk` 指定 SDK，通过 `--emscripten-version` 核对版本。Node 构建当前使用 6.0.9；TH06、TH07 的根 CI 配方登记其构建版本。

TH06、TH07 使用 CMake 3.20 以上与 Ninja，并按根 `.gitmodules` 初始化本作 SDL 子模块。原版素材、字体和可选练习扩展通过显式构建参数提供。构建入口输出成功记录到 `artifacts/validation/source-builds/`，其中包含模式、工具链、参数和实际源码身份。

直接检查 Node 构建输入可运行本作构建入口的 `--print-inputs`。CMake 的源码声明检查记录条件分支的声明并集；实际编译使用构建目录的 `compile_commands.json`。

SDK 版本核对读取带引号或纯文本的版本文件，统一比较版本号并保留 `-git` 来源标记。各作 Node 构建入口在 `--print-inputs` 模式等待完整 JSON 写入管道后退出，调用方可直接解析大型源码清单。

并行编译前先准备 SDL 与字体依赖缓存。th11 普通版和 th15 两种模式每次启动编译均执行串行准备，已缓存的游戏对象可直接复用。th20 模块搜索目录按名称固定排序，跨平台保持相同的头文件搜索优先级。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="build-01"></a>BUILD-01 · 源码目录与普通版／MP归属 | 运行 `node tools/workspace.mjs list`，选定作品和 `normal`／`multiplayer`。 | 源码目录与构建配方一致；成对目录分别检查。 |
| <a id="build-02"></a>BUILD-02 · 工具链与外部依赖 | 按本作开发指南准备 Emscripten、编译器、SDL 和字体，运行构建计划。 | 计划显示实际 SDK、common 和源码路径。 |
| <a id="build-03"></a>BUILD-03 · 对应模式独立构建 | 执行本模式的构建命令；共用源码的模式也分别构建。 | 两种构建各有对应输出和来源记录。 |
| <a id="build-04"></a>BUILD-04 · Runtime 文件与来源记录 | 执行本作 Runtime 打包命令，检查文件清单、字节数和哈希。 | 运行文件完整，清单指向本次构建。 |
| <a id="build-05"></a>BUILD-05 · 前端搭配单个游戏运行 | 按开发指南只检出启动器、common、工具和目标游戏，启动本地联调。 | 选中的游戏能加载当前 Runtime，其他游戏源码可以缺席。 |
| <a id="build-06"></a>BUILD-06 · 源码与素材来源 | 登记上游、许可证和外部素材位置。 | 源码许可证、作者归属和素材出处可从入口找到。 |

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

验收 BUILD-03／04 时，分别定位当前模式的构建命令、实际输出和打包清单。确认入口、加载器与 WASM 的字节数和哈希，再启动该产物。发布包校验的反例包括文件被修改、缺失或额外混入原版资源；`npm --prefix launcher run check` 执行这些合成包检查，正式构建结果附本次工具链和命令。

## 源码入口

[工作区目录](../../launcher/config/workspace.json) · [构建配方](../../launcher/config/runtime-builds.json) · [构建计划](../../launcher/lib/runtime-source-plan.mjs)

## 新增构建模式

构建计划按注册的作品和模式生成，th12 普通版与多人版分别指向自己的源码目录，开发站清单核对各自运行包的实际路径。公共库的跨作品平台回归同时检出各自 SDL 适配入口，在同一公共头文件版本上核对输入行为。
