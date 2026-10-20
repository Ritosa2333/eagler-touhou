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

th06、th07 共目录双构建；th075、th08、th10、th11、th15 分普通版和 MP 目录；th09 共目录并保留两种产品身份。

每个成功构建后使用统一入口生成 Launcher Runtime 与包凭据：

```sh
node launcher/scripts/package-runtime.mjs --game=th10 --variant=normal
node launcher/scripts/package-runtime.mjs --game=th10 --variant=multiplayer
```

入口按正式配方调用作品打包器，或从本次 CMake 构建复制 HTML、JS、WASM，并核对来源凭据。闭集包输出沿用 Launcher 的作品目录；包凭据写入 `artifacts/validation/runtime-packages/<作品>-<模式>.json`，记录入口、整包身份、完整文件清单、当前源码输入和打包控制文件。th06、th07 的代码包位于 `build-eagler/{normal,multiplayer}/`；DATA 与预加载 JS 继续来自 `build/web/{normal,multiplayer}/`，在开发站点中作为内容输入提供。

产品目录、构建配方与打包清单逐变体对应。th075 的普通版执行文件位于 `web/artifacts/sdl3/th075.wasm`，MP 位于 `web/artifacts/sdl3-multiplayer/th075.wasm`，统一入口按各自源码目录生成代码包。CI 从产品目录核对全部变体，新增作品随登记进入构建与打包身份检查。

## 工具链

使用仓库外或被忽略的 `toolchains/` 目录安装官方 [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)。Windows、Linux 和 macOS 共用同一源码构建入口，通过 `--emsdk` 指定 SDK，通过 `--emscripten-version` 核对版本。Node 构建当前使用 6.0.9；th06、th07 的根 CI 配方登记其构建版本。

th06、th07 使用 CMake 3.20 以上与 Ninja。构建配方固定 vendored PNG，并由根仓库 Git pins 管理 SDL、SDL_image、SDL_ttf、libpng、zlib、FreeType、PlutoVG 与 PlutoSVG。运行 `node launcher/scripts/print-runtime-submodule-init.mjs --game=th06` 或 `--game=th07` 可生成按父模块顺序排列的最小初始化命令；命令逐层指定所需模块，不递归检出未参与构建的编解码器。原版素材、字体和可选练习扩展通过显式构建参数提供。构建入口在 `artifacts/validation/source-builds/` 输出两份对应记录：`作品-模式.json` 保存工具链、参数和执行计划，`作品-模式.runtime.json` 保存正式 WASM、编译输入 SHA-256 和必需子模块提交，供[浏览器验收](validation.md#测试辅助器)复核。源码状态检查和编译输入身份使用 th06/th07 配方声明的依赖闭集。执行 `npm run ports:check -- --all --check-status` 前，运行 `node launcher/scripts/prepare-runtime-dependencies.mjs --game=th06,th07 --variant=normal`，按配方顺序初始化所需 Git 子模块。

th06、th07 的 CMake 配置会把 SDL_image、libpng 与 zlib 的 tracked 构建闭集复制到 `build/web/<模式>/eagler-staging/sdl-image/<SHA-256>/`。zlib 的原始 `zconf.h` 保留在源码凭据中；stage 按 pinned CMake 的配置约定使用 binary-tree 生成头文件，源码 checkout 可直接复用。staging 清单由 CMake 输入和 Git index 生成，排除未跟踪文件以及 VisualC、Xcode 等平台包资源。`node launcher/tests/test-runtime-cmake-staging.mjs` 用独立 CMake fixture 验证隔离效果和上游源目录 rename 的反例。

Runtime 凭据在编译开始前采集仓库生产输入，编译成功后复核源码并登记本次正式输出。版本 3 对应固定的 `planSha256`、`requiredSubmodules` 与 `outputs` 字段：计划身份覆盖工具链、实际命令、参数、环境和特性配置；每种配方都记录执行 WASM 和加载器，CMake 另记录 HTML，Node 另记录 `build.json`，每项使用配方确定的仓库相对路径与 SHA-256。配方声明的环境变量同时传给编译器与正式打包器。th09 普通版和 MP 共用源码目录，分别写入 `web/artifacts/sdl-release/` 与 `web/artifacts/sdl-multiplayer/`，各自凭据和包配方定位对应产物。CMake 凭据记录必需子模块路径及其父级 gitlink 提交，并只哈希配方声明的子模块源码。CMake 增量复用按实际编译计划、CMake 输入闭集、必需子模块提交和全部正式输出哈希核对；Launcher JS 控制变化仍写入完整凭据，CMake 输入与计划不变时可沿用相同产物。声明非空 `sourceExclusions` 时按完整凭据严格匹配。编译失败、输入中途改变或输出缺失均不能生成成功凭据。验收清单的 `buildReceipt` 使用凭据的仓库相对路径；包内 WASM、加载器以及 CMake HTML 均须与对应输出哈希一致。完整工具链和构建步骤保存在构建计划，游戏 DATA 内容哈希由运行验收记录。

直接检查 Node 构建输入可运行本作构建入口的 `--print-inputs`。CMake 的源码声明检查记录条件分支的声明并集；实际编译使用构建目录的 `compile_commands.json`。

th09、th11、th15 正式 Node 构建配方将 `web/tests` 声明为测试目录排除项；诊断构建按自身输入集合绑定导出代码。

Node 增量构建可在已验证的同一计划、输入、WASM 与加载器上刷新 `build.json` 元数据，凭据记录刷新后的实际哈希。构建前的完整输出须与旧凭据一致，构建后其他字段和输出保持原值；单独改变加载器或 CMake HTML 会使复用失败。
SDK 版本核对读取带引号或纯文本的版本文件，统一比较版本号并保留 `-git` 来源标记。各作 Node 构建入口在 `--print-inputs` 模式等待完整 JSON 写入管道后退出，调用方可直接解析大型源码清单。

并行编译前先准备 SDL 与字体依赖缓存。th11 普通版和 th15 两种模式每次启动编译均执行串行准备，已缓存的游戏对象可直接复用。th20 模块搜索目录按名称固定排序，跨平台保持相同的头文件搜索优先级。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="build-01"></a>BUILD-01 \| 源码目录与普通版／MP归属 | 运行 `node tools/workspace.mjs list`，选定作品和 `normal`／`multiplayer`。 | 源码目录与构建配方一致；成对目录分别检查。 |
| <a id="build-02"></a>BUILD-02 \| 工具链与外部依赖 | 按本作开发指南准备 Emscripten、编译器、SDL 和字体，运行构建计划。 | 计划显示实际 SDK、common 和源码路径。 |
| <a id="build-03"></a>BUILD-03 \| 对应模式独立构建 | 执行本模式的构建命令；共用源码的模式也分别构建。 | 两种构建各有对应输出和来源记录。 |
| <a id="build-04"></a>BUILD-04 \| Runtime 文件与来源记录 | 执行本作 Runtime 打包命令，检查文件清单、字节数和哈希。 | 运行文件完整，清单指向本次构建。 |
| <a id="build-05"></a>BUILD-05 \| 前端搭配单个游戏运行 | 按开发指南只检出启动器、common、工具和目标游戏，启动本地联调。 | 选中的游戏能加载当前 Runtime，其他游戏源码可以缺席。 |
| <a id="build-06"></a>BUILD-06 \| 源码与素材来源 | 登记上游、许可证和外部素材位置。 | 源码许可证、作者归属和素材出处可从入口找到。 |

## 验收方法

### BUILD-01 | 源码目录与普通版／MP归属
- 操作：选定一个作品和模式，运行 `node tools/workspace.mjs list`，再执行对应 `build-runtime.mjs --dry-run`。
- 观察与判定：把计划中的 owner、源码根目录、模式和输出路径逐项对照 `workspace.json` 与构建配方；四者一致才通过。
- 反例：把 th09 MP 指向普通版产物目录，或用另一作品的源码根目录生成计划，应被配方校验拒绝。
- 入口与覆盖：`test-runtime-source-plan.mjs`、`test-runtime-build-profiles.mjs` 检查计划规则；它们不证明每个实际检出目录都可编译，逐变体执行仍需记录。

### BUILD-02 | 工具链与外部依赖
- 操作：按该作品 `DEVELOPMENT.md` 准备 SDK、编译器与必需模块，运行正式构建计划并检查打印的执行步骤。
- 观察与判定：确认计划记录实际 SDK 版本、工具路径、受 pin 管理的必需子模块及环境参数；实际构建成功并生成来源记录才通过。
- 反例：缺少必需 FreeType 子模块或 SDK 版本不匹配时，预检必须失败，不能拿旧产物当成本次结果。
- 入口与覆盖：`test-runtime-build-dependencies.mjs` 与 `test-runtime-cmake-staging.mjs` 覆盖依赖闭集和 CMake 暂存规则；各主机工具链可用性按实际构建记录。

### BUILD-03 | 对应模式独立构建
- 操作：normal 与 multiplayer 分别运行正式 `build-runtime.mjs`，共用源码的作品也按两种模式各执行一次。
- 观察与判定：每种模式都生成自己的当前源码收据，且收据中的 WASM、加载器和适用的 HTML 均对应该模式的正式输出。
- 反例：只编译 normal，却将它的 WASM 或来源收据登记给 multiplayer，应因模式或输出身份不符而失败。
- 入口与覆盖：`test-runtime-source-plan.mjs`、`test-runtime-build-profiles.mjs` 检查计划差异；它们不替代 15 个产品变体各自的真实编译记录。

### BUILD-04 | Runtime 文件与来源记录
- 操作：对刚完成的构建运行 `node launcher/scripts/package-runtime.mjs --game=<作品> --variant=<模式>`，逐项检查包凭据。
- 观察与判定：入口、完整文件清单、文件哈希、源码收据及编译输出身份一致，包中 HTML/加载器/WASM 与收据记录相符才通过。
- 反例：打包后改动一个加载器字节，重新验包应失败，不能靠手填新的整包身份绕过来源绑定。
- 入口与覆盖：`test-runtime-package-evidence.mjs` 与 `tools/build/runtime-evidence.test.mjs` 覆盖凭据和篡改拒绝；正式 producer 命令提供本次包的实际证据。

### BUILD-05 | 前端搭配单个游戏运行
- 操作：准备只含 Launcher、common、构建工具和目标作品的检出，启动 `npm run dev:workspace`，再用当前作品 Runtime 打开该站点。
- 观察与判定：所选卡片能从当前包加载 DATA 并进入首帧；不属于目标的作品源码目录缺席时，目标流程仍可完成。
- 反例：目标 Runtime 请求另一个作品的源码或本地构建目录才能启动，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 可实际启动清单指定的普通版包，`npm run test:support` 检查本地服务辅助器；最小检出中其他作品缺席的完整场景待接入。

### BUILD-06 | 源码与素材来源
- 操作：为源码和外部素材各记来源地址或本地来源、版本/提交、许可证及文件哈希，再按记录重新定位输入。
- 观察与判定：独立审阅者能从登记位置找到对应来源，并用哈希确认构建实际素材与登记输入相同。
- 反例：素材只有文件名、没有来源或身份，不能据此判定来源记录完整。
- 入口与覆盖：源码构建收据登记被编译输入及子模块提交；许可证和原版素材来源的逐项检查没有自动验收驱动，待接入。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[工作区目录](../../launcher/config/workspace.json) | [构建配方](../../launcher/config/runtime-builds.json) | [构建计划](../../launcher/lib/runtime-source-plan.mjs)
