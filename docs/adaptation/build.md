# 源码与构建

[适配流程](README.md) · [文档目录](../README.md)

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

## 源码入口

[工作区目录](../../launcher/config/workspace.json) · [构建配方](../../launcher/config/runtime-builds.json) · [构建计划](../../launcher/lib/runtime-source-plan.mjs)
