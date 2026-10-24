# 源码与构建

[适配流程](README.md) | [文档目录](../README.md)

先选作品和模式，再准备工具链。各作的额外依赖和输出目录见其 `docs/DEVELOPMENT.md`。

直接从源码产物组装站点时，Release Manifest 同时记录 `launcher`、`common` 和选中 `ports/<作品>` 的源码身份及 `sourcePath`，共享运行时与 thprac 实现随真实消费者一起进入发布来源。使用已验证 Runtime Release 的发布沿用该 Release 与 Launcher 的来源凭据。

单作站点沿用完整 Launcher 作品目录。组装器从显式 `--artwork-dir` 输入复制各作封面，随站点进入离线缓存；Runtime 和 DATA 按所选作品准备。正式发布核对完整可见目录的封面与站点图标。离线行为验收在启动浏览器前检查同一输入闭集，记录每个文件的字节数和哈希。

Launcher 内容生成在全文一致时保留现有文件和行尾，内容新鲜度检查统一比较 LF 文本。实际模板变化重新生成页面。适配队列继续按原始字节核对完整输入身份，重复准备可复用相同内容。

## 准备

在仓库根目录运行：

```sh
node tools/workspace.mjs list
node tools/workspace.mjs select th10 --variant=both
node launcher/scripts/build-runtime.mjs --game=th10 --variant=normal --emsdk=/path/to/emsdk --dry-run
```

将 SDK 路径换成本机路径；确认计划后去掉 `--dry-run` 执行编译。MP 使用 `--variant=multiplayer`。

th08 的 Node 构建按任务数并行编译。任一编译失败后停止分配新文件，等待本轮已启动的编译器退出，再封存失败结果；每轮编译与进程队列保持一致的结束边界。

th06、th07 共目录双构建；th075、th08、th10、th11、th15 分普通版和 MP 目录；th09 共目录并保留两种产品身份。

每个成功构建后使用统一入口生成 Launcher Runtime 与包凭据：

```sh
node launcher/scripts/package-runtime.mjs --game=th10 --variant=normal
node launcher/scripts/package-runtime.mjs --game=th10 --variant=multiplayer
```

入口按正式配方调用作品打包器，或从本次 CMake 构建复制 HTML、JS、WASM，并核对来源凭据。闭集包输出沿用 Launcher 的作品目录；包凭据写入 `artifacts/validation/runtime-packages/<作品>-<模式>.json`，记录入口、整包身份、完整文件清单、当前源码输入和打包控制文件。th06、th07 的代码包位于 `build-eagler/{normal,multiplayer}/`；DATA 与预加载 JS 继续来自 `build/web/{normal,multiplayer}/`，在开发站点中作为内容输入提供。

产品目录、构建配方与打包清单逐变体对应。th075 的普通版执行文件位于 `web/artifacts/sdl3/th075.wasm`，MP 位于 `web/artifacts/sdl3-multiplayer/th075.wasm`，统一入口按各自源码目录生成代码包。CI 从产品目录核对全部变体，新增作品随登记进入构建与打包身份检查。

正式构建与打包仍由 `build-runtime.mjs` 和 `package-runtime.mjs` 执行。适配队列的 `build` 组只复核已封存的两份 producer 报告：当前源码计划、工具链、构建收据、Runtime 文件与包收据都必须再次匹配，且打包报告引用同一份源码收据。复核沿用 producer 的运行编号、时间、命令和报告字节身份；它不会触发编译或打包。缺少报告时该作品／模式记录 blocked，先用正式 producer 生成报告再复核。

## 工具链

使用仓库外或被忽略的 `toolchains/` 目录安装官方 [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)。Windows、Linux 和 macOS 共用同一源码构建入口，通过 `--emsdk` 指定 SDK，通过 `--emscripten-version` 核对版本。Node 构建当前使用 6.0.9；th06、th07 的根 CI 配方登记其构建版本。

th06、th07 使用 CMake 3.20 以上与 Ninja。构建配方固定 vendored PNG，并由根仓库 Git pins 管理 SDL、SDL_image、SDL_ttf、libpng、zlib、FreeType、PlutoVG 与 PlutoSVG。运行 `node launcher/scripts/print-runtime-submodule-init.mjs --game=th06` 或 `--game=th07` 可生成按父模块顺序排列的最小初始化命令；命令逐层指定所需模块，不递归检出未参与构建的编解码器。原版素材、字体和可选练习扩展通过显式构建参数提供。构建入口在 `artifacts/validation/source-builds/` 输出两份对应记录：`作品-模式.json` 保存工具链、参数和执行计划，`作品-模式.runtime.json` 保存正式 WASM、编译输入 SHA-256 和必需子模块提交，供[浏览器验收](validation.md#测试辅助器)复核。源码状态检查和编译输入身份使用 th06/th07 配方声明的依赖闭集。执行 `npm run ports:check -- --all --check-status` 前，运行 `node launcher/scripts/prepare-runtime-dependencies.mjs --game=th06,th07 --variant=normal`，按配方顺序初始化所需 Git 子模块。

th06、th07 的 CMake 配置会把 SDL_image、libpng 与 zlib 的 tracked 构建闭集复制到 `build/web/<模式>/eagler-staging/sdl-image/<SHA-256>/`。zlib 的原始 `zconf.h` 保留在源码凭据中；stage 按 pinned CMake 的配置约定使用 binary-tree 生成头文件，源码 checkout 可直接复用。staging 清单由 CMake 输入和 Git index 生成，排除未跟踪文件以及 VisualC、Xcode 等平台包资源。`node launcher/tests/test-runtime-cmake-staging.mjs` 用独立 CMake fixture 验证隔离效果和上游源目录 rename 的反例。

Runtime 凭据在编译开始前采集仓库生产输入，编译成功后复核源码并登记本次正式输出。版本 3 对应固定的 `planSha256`、`requiredSubmodules` 与 `outputs` 字段：计划身份覆盖工具链、实际命令、参数、环境和特性配置；每种配方都记录执行 WASM 和加载器，CMake 另记录 HTML，Node 另记录 `build.json`，每项使用配方确定的仓库相对路径与 SHA-256。配方声明的环境变量同时传给编译器与正式打包器。th09 普通版和 MP 共用源码目录，分别写入 `web/artifacts/sdl-release/` 与 `web/artifacts/sdl-multiplayer/`，各自凭据和包配方定位对应产物。CMake 凭据记录必需子模块路径及其父级 gitlink 提交，并只哈希配方声明的子模块源码。CMake 增量复用按实际编译计划、CMake 输入闭集、必需子模块提交和全部正式输出哈希核对；Launcher JS 控制变化仍写入完整凭据，CMake 输入与计划不变时可沿用相同产物。声明非空 `sourceExclusions` 时按完整凭据严格匹配。编译失败、输入中途改变或输出缺失均不能生成成功凭据。验收清单的 `buildReceipt` 使用凭据的仓库相对路径；包内 WASM、加载器以及 CMake HTML 均须与对应输出哈希一致。完整工具链和构建步骤保存在构建计划，游戏 DATA 内容哈希由运行验收记录。

直接检查 Node 构建输入可运行本作构建入口的 `--print-inputs`。CMake 的源码声明检查记录条件分支的声明并集；实际编译使用构建目录的 `compile_commands.json`。

Node 构建将 common 输入头文件目录纳入编译清单与对象缓存身份。共享触控算法、观察 ABI 或新增输入头文件变化时，各消费者自动重新编译；`--print-inputs` 同时列出这些依赖，便于检查当前产物与共享实现的对应关系。

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
| <a id="build-05"></a>BUILD-05 \| 前端搭配单个游戏运行 | 只检出 Launcher、common、工具、测试入口和目标游戏，运行 offline 适配场景。 | Launcher 从目标 Runtime 与本地 DATA 包启动并进入首帧，其他作品源码目录缺席。 |
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

### 适配队列复核 BUILD-01..04

把正式 producer 的 `report.json` 保存在仓库内的记录目录，并在本地 Runtime manifest 的对应目标上登记相对路径：

```json
{
  "targets": [{
    "game": "th08",
    "variant": "normal",
    "runtime": "build-eagler",
    "entry": "th08.html",
    "sourceRoot": "ports/th08",
    "buildReceipt": "artifacts/validation/source-builds/th08-normal.runtime.json",
    "packageReceipt": "artifacts/validation/runtime-packages/th08-normal.json",
    "packageSha256": "<包收据中的 SHA-256>",
    "buildAcceptance": {
      "sourceBuildReport": "../artifacts/validation/build/th08-normal-source/report.json",
      "packageReport": "../artifacts/validation/build/th08-normal-package/report.json"
    }
  }]
}
```

`buildAcceptance` 中两个文件名相对于 Runtime manifest 所在目录；producer 报告由下方 `build-acceptance.mjs --stage=source-build` 与 `--stage=package` 生成，且须与所选作品和模式一致。底层 `build-runtime.mjs`、`package-runtime.mjs` 的报告之外还需保留这些封存报告；两者不能互相替代。为另一种模式登记另一组报告。随后可单独复核：

```sh
node tools/ports/build-acceptance.mjs --game=th08 --variant=normal --stage=verify-existing --runtime-manifest=.local/runtime-manifest.json --output=artifacts/validation/build-th08-normal-recheck
```

适配队列中使用 `--suite=build` 选择同一复核。它读取 `EAGLER_TEST_RUNTIME_MANIFEST`，并为所选产品与模式产生 BUILD-01..04 结构化证据；identity 预检核验清单、producer 报告和当前构建／打包身份，不读取 DATA、不发现浏览器，也不启动编译器。报告哈希、原始 producer 阶段及本次复核结果共同进入运行身份。BUILD-05 的最小检出运行和 BUILD-06 的来源审查按各自方法记录。

### BUILD-05 | 前端搭配单个游戏运行
- 操作：运行 `node tools/workspace.mjs select th08 --variant=normal`。该选择包含 `tests/`，供现有 `tests/run.mjs` 适配入口使用。交互联调服务命令为 `npm --prefix launcher run dev:workspace`。正式验收使用同一构建输入清单提供外部 Emscripten SDK 和作品字体目录，让构建与打包入口共享字体身份。
- 操作：按[Runtime 输入清单](validation.md#测试辅助器)登记本次检出的 Runtime、构建与打包收据、外部 DATA；另准备本地 Runtime ZIP 与完整可见作品目录封面。收据路径相对于本次检出，SDK、DATA 等私有输入填写实际外部路径。将清单、ZIP 映射和封面目录登记到 `.local/stg-inputs.json`，再运行 `node tests/run.mjs --profile=adaptation --game=th08 --variant=normal --suite=offline --check=RUNTIME-05 --inputs=.local/stg-inputs.json --output=<外部证据目录>`。更新安装另由 PACKAGE-03／SAVE-06 选择并提供 `updatePackages`。
- 观察与判定：offline 场景使用正式包凭据在 Launcher 站点安装本地 ZIP，通过当前普通版 Runtime 读取对应 DATA 并进入首帧；源码收据来自本次检出，其他作品源码目录缺席。
- 反例：清洁检出中的来源收据与所选 Runtime 源码、打包文件或 DATA 身份不一致，或目标流程依赖未检出的其他作品源码，应判失败。
- 入口与覆盖：`tests/adaptation/catalog.mjs` 将单作选择路由到 `tools/ports/offline-runtime-browser.mjs`；该驱动调用 `package-server.mjs` 准备仅含所选 Runtime 的验证站点，再经 Launcher 导入流程启动游戏。Launcher 目录依赖安装并准备浏览器运行环境：`npm --prefix launcher ci --ignore-scripts`。
- 输入范围：检出包含 Launcher、common、tools、tests、docs 和目标作品源码。Emscripten SDK、需要的私有字体、原版 DATA、本地 Runtime ZIP、Launcher 可见作品目录封面、Node 依赖和浏览器来自本机输入目录。运行产物与来源收据在该检出内重新生成，验收报告保存在检出外的证据目录。

### BUILD-06 | 源码与素材来源

统一队列的 `source-review` 组读取本地配置 `sourceReview` 指向的 `eagler-touhou/adaptation-source-review/1` 清单。每个 `products.<game>.<variant>.source` 登记上游与固定 revision、作者、许可证及通知文件身份；`externalInputs` 分别记录 `game-data`、`font` 的来源、分发方式、路径、字节数与 SHA-256。`review` 记录审查者、日期、接纳结论及原始审查材料的路径和哈希。队列逐文件核对本次内容，变更后的输入按新身份重新审查。

`BUILD-06` 与 `VERIFY-05` 将清单及所有已审材料的文件身份封存在产品证据中。完整证据检查器重新读取这些路径，核对已接受记录、原版 EXE/DATA 与 golden 身份，再确认报告与输入清单逐字节一致。

人工审阅或原版输入尚未就绪时，产品记录填写 `status: "blocked"` 和按检查编号列出的 `blockers`。`adaptation-source-review.mjs` 封存带清单身份的 blocked 报告，两个检查保持未完成；材料补齐并完成独立审阅后再运行身份核对。

`single-game` 组执行 BUILD-05。本地配置 `minimalRuntime` 指向 `eagler-touhou/minimal-runtime-inputs/1`，在 `products.<game>.normal` 下填写 `checkoutRoot`、`runtimeManifest`、`packages`、`artwork`。这些路径以清单所在目录为基准。入口在所选最小检出中检查源码归属、复核正式构建与打包 producer，再执行真实 Launcher 导入、受管 DATA 读取和首帧。源码检查、包身份与浏览器结果按同一作品和模式关联；其他作品源码目录必须缺席。

BUILD-05 保留 checkout、正式构建复核及 Launcher Runtime 的封存子报告。完整证据检查器逐项重算嵌套 BUILD-01..04 与 RUNTIME-05，再核对同一打包身份、首帧和实际受管 DATA 读取。
- 操作：为源码和外部素材各记来源地址或本地来源、版本/提交、许可证及文件哈希，再按记录重新定位输入。
- 观察与判定：独立审阅者能从登记位置找到对应来源，并用哈希确认构建实际素材与登记输入相同。
- 反例：素材只有文件名、没有来源或身份，不能据此判定来源记录完整。
- 入口与覆盖：`build-acceptance.mjs --stage=provenance` 汇集源码收据和子模块身份，生成逐项来源审阅任务；许可证、作者归属与原版素材出处由独立审阅者确认。

### 分阶段构建证据

以下入口按所选作品和模式分别执行正式构建、打包或检出检查，并在输出目录封存带运行编号、执行时间、阶段和可用收据身份的报告：

```sh
node tools/ports/build-acceptance.mjs --stage=source-build --game=th08 --variant=normal --manifest=.local/build-inputs.json --output=artifacts/validation/build/th08-normal-source
node tools/ports/build-acceptance.mjs --stage=package --game=th08 --variant=normal --manifest=.local/build-inputs.json --output=artifacts/validation/build/th08-normal-package
node tools/ports/build-acceptance.mjs --stage=checkout --game=th08 --variant=normal --output=artifacts/validation/build/th08-normal-checkout
node tools/ports/build-acceptance.mjs --stage=provenance --game=th08 --variant=normal --output=artifacts/validation/build/th08-normal-provenance
```

为每个本地作品/模式保存构建输入清单，例如 `.local/build-inputs.json`：

```json
{
  "schema": "eagler-touhou/build-acceptance-inputs/1",
  "targets": [
    {
      "game": "th08",
      "variant": "normal",
      "fontRoot": "../fonts/th08",
      "options": {
        "emsdk": "../toolchains/emsdk",
        "assets": "../games/th08",
        "jobs": "8"
      }
    }
  ]
}
```

`game` 和 `variant` 由命令显式选择，清单必须恰好包含一个匹配目标；同一清单可登记不同作品和模式的独立输入。`options` 接受正式 `build-runtime.mjs` 的构建参数，路径相对该 JSON 文件所在目录解析。`fontRoot` 同样相对清单目录解析，并作为 `EAGLER_FONT_ROOT` 传给正式构建或打包子进程。清单不接受目标、阶段或输出目录覆盖，也不能与命令行构建参数混用。源码构建使用 `options`；打包只使用命令选择的作品和模式及 `fontRoot`，不会把 Emscripten、jobs 等编译选项传给 `package-runtime.mjs`。报告保存已解析的字体目录身份、清单 SHA-256 和调用身份。

`source-build` 运行 `build-runtime.mjs`，再复核当前源码计划、完整编译输入、子模块 pin 和正式输出；`package` 运行 `package-runtime.mjs`，再复核封闭文件清单和来源收据。报告标明本次命令调用及它验证的产物身份；增量构建复用由正式构建入口按其计划规则处理，报告不把复用写成全量重新编译。`checkout` 对照 workspace/build 配置确认必需目录齐全且其他作品源码目录缺席，该阶段报告保持未完成，直到 BUILD-05 与同一选择通过 `RUNTIME-05` 浏览器首帧旅程。`provenance` 提供构建输入和子模块身份，并保留来源、许可证、作者归属及外部素材出处的人审状态；它不会生成自动通过结论。

打包报告同时保存正式输出目录及其实际目录。工作区使用目录链接把构建产物放到外部磁盘时，验包器核对该正式输出的实际目录与全部文件身份。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[工作区目录](../../launcher/config/workspace.json) | [构建配方](../../launcher/config/runtime-builds.json) | [构建计划](../../launcher/lib/runtime-source-plan.mjs)

端口源清单由各作正式构建入口的 `--print-inputs` 输出生成。清单通过异步子进程读取，stdout 与 stderr 各自受 8 MiB 上限约束，30 秒后以 SIGKILL 结束；失败诊断记录所选作品与模式、完整命令、实际耗时、退出状态和子进程输出。查询不继承宿主 `NODE_OPTIONS` 或 `NODE_TEST_CONTEXT`，并能完整接收超过 64 KiB 的 JSON 清单。逐目标回归为每个查询设置独立子进程时限，使超时诊断保留准确的作品与模式。
