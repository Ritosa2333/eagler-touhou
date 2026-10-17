# Monorepo 迁移准备

日期：2026-10-16

采用现有 `eagler-touhou` 作为 monorepo，保留 Launcher、common 和各作品的原提交历史及提交 ID。旧 `launcher-refactor` 分支的目录分工和部分工具可以复用，迁移应从当前 main 重新应用这些设计，不将旧分支整体合回。首轮调查后，已在 main 原目录落实路径、构建计划和发布身份的兼容准备，没有迁移源码、变更依赖版本、创建工作树、提交、推送或删除分支。

## 原目录内已完成的准备

- `lib/source-layout.mjs` 统一识别当前独立仓库、现有 main 工作树、未来 `launcher/web` 布局和独立自托管包。Node、PowerShell 与 Python 共用同一解析入口，明确的 `EAGLER_WORKSPACE_ROOT` 仍可覆盖源码工作区。
- 修正 TH06/07/08/10 的现行源码目录，TH15 联机不再指向不存在的工作树，TH10 与 TH11 联机分别登记现有独立来源。main 工作树也不再将其父目录 `worktrees` 误当作工作区。
- 发布源码身份支持 monorepo 子目录，记录仓库 SHA、组件路径、组件内已暂存/未暂存/未跟踪输入与子模块。组件身份排除其他组件的脏改动。整体发布还会识别 monorepo 根源码，使根配置和共享协议不会遗漏。
- `config/runtime-builds.json` 补全八作品的源码编译声明及七个适用的联机变体。`scripts/build-runtime.mjs` 调用原有作品构建器，支持 `--dry-run`、显式源码/common 路径、Python、并行度和 SDK 版本检查，不承担 Runtime 打包或部署。
- 新编译入口要求显式 SDK 参数或 SDK 环境变量，避免本机 6.0.6 与 6.0.9 之间无提示切换。各作品已有的 common 固定版本继续优先，未统一版本或删除子模块。
- 回归门禁发现并修复了两处 TypeScript 编译兼容问题：Replay 策略的源码扩展名 import，以及 TH09 输入类的构造参数属性。两处均保持运行行为。
- TH09/TH15 原构建脚本现在接受 `TH_PYTHON` / `PYTHON`，统一入口传入的解释器路径会实际生效。没有改 common、本作编译参数或游戏逻辑。

例如在当前 main 目录查看构建计划：

```powershell
node scripts/build-runtime.mjs --game=th09 --emsdk=D:/workspace/eagler/th08-eagler/tools/emsdk --emscripten-version=6.0.9 --dry-run
```

去掉 `--dry-run` 会在原作品目录调用已有编译器入口。`--variant=multiplayer` 选择联机来源，输出仍由作品原构建器管理。此入口不打包游戏数据，也不上传产物。

用户随后明确 common 正在其他任务中修改，因此本轮对 common 本体、各作 common 固定版本、相对 include 和同步脚本只调查，不实施修改。相关未完成项保留在下文，不能将本轮准备视为 common 已完成统一。

## 当前源码与旧重构

当前 Launcher 源码位于 `D:/workspace/eagler/worktrees/launcher-th09-spectator-main`，main 为 `296875f3eaa6968ac7d6fa892dc4473e0d44646e`。主检出 `D:/workspace/eagler/eagler-touhou` 位于 `launcher-refactor`，提交为 `544cf7062dc87768eb65585379b80ba63cfcb55d`。两处检出属于同一 Git 仓库，但工作区内容不同。

两分支共同祖先为 `74b2f12f7cc3d83f784d6abdfd5a4a53dce4a9e5`。main 有 392 个独有提交，重构分支有 2 个独有提交。这一计数包括合并提交，不代表缺少 392 个功能。将旧树 `launcher/web/` 对照 main 原路径，346 个文件内容相同，349 个不同，130 个没有直接对应路径。后者包含新增或重新组织的文件，不能直接视为可删除文件。

旧树没有本次新增的键盘和手柄设置、TH09 双人输入模块、当前 ReplayWorkspace，以及现有 adapter-capabilities 和 GAME_ADAPTER_CONTRACT 在预期对应位置的文件。迁移必须完整保留 main 的协议、功能和验证记录，不能直接使用旧树覆盖。

调查时 Launcher main 有 75 条工作区状态记录，旧重构检出有 993 条。其他作品和 common 也有并行工作。提交树的比较与工作区的未提交变化分别记录在审计文件中，正式迁移前要重新确定完整基线。

## 建议复用的目录分工

```text
eagler-touhou/
├─ launcher/web/       当前前端和它的构建、测试
├─ contracts/          完整的共享 TypeScript 协议与产品声明
├─ services/netplay/   中继、大厅、旁观和遥测服务
├─ eagler-common/      共享 C++ 和浏览器基础设施，普通目录
├─ th06/ … th20/       各作品源码，普通目录
├─ third_party/        固定版本的第三方依赖
├─ config/             工程路径与构建配置
├─ tools/              统一构建、检查和准备入口
└─ docs/               跨作品协议、适配和迁移记录
```

沿用旧设计的 `launcher/web`、`contracts`、`services/netplay` 和工程工具分工。各作品第一阶段保留内部目录和构建边界，不同时重构游戏逻辑、渲染器或测试体系。普通版和联机版尚不能共用一个源码树的作品，要在构建配置中明确不同来源，必要时在对应作品目录下暂存 normal 和 multiplayer 两套普通源码目录，待独立验证后再统一。

| 旧重构内容 | 处理方式 |
| --- | --- |
| npm workspaces 与根目录命令转发 | 复用结构，以当前依赖和 scripts 为准重建锁文件 |
| `launcher/web/lib/source-layout.mjs` | 复用源码工程根与独立交付包根的区分，重验现有打包流程 |
| `tools/project.mjs` 的构建计划和 dry-run | 复用设计，补全 TH20 及各作最新 normal/mp 入口 |
| Runtime 暂存、校验、切换工具 | 对照 main 现有发布校验复用，避免产生第二套发布权威 |
| `dependencyStatus()` 与 `dependencies init` | 改为检查内部目录和第三方依赖，不再要求游戏目录包含 `.git` |
| 旧协议拆分、服务拆分 | 对照最新 main 移植，不能恢复旧协议或漏掉新服务模块 |
| 旧三系统 CI、common 测试入口 | 复用组织方式，更新路径并加入实际八作品构建覆盖 |

旧 `sourceBuild` 配置已有 TH06/07 CMake、TH08/10/11 portable、TH09 SDL 和 TH15 application 入口，但没有 TH20 的源码构建声明，且 TH15 未声明当前联机来源。因此不能直接作为完整的新工程配置。

## 必须先调整的依赖与路径

### 统一 common 的来源

当前八个作品均通过 `third_party/eagler-common` 引用共享库。TH15 联机另有 `third_party/eagler-common-netplay` 和 `EAGLER_COMMON_NETPLAY_ROOT`。所选普通/当前提交中 common 至少固定了四个不同版本，TH11 独立联机树又固定另一版本，不能直接删掉所有子模块后统一指向 HEAD。

目标是 monorepo 内只有一份权威 common。先核对各版本的独有修改和当前未提交修改，合入选定版本，再验证全部 normal/mp 配置。TH15 的额外 netplay 版本在差异得到保留和验证前不能移除。版本依赖关系必须由工程清单和构建身份表达，不能依靠本机碰巧存在的目录。本轮按用户要求只调查，不整合 common 代码或改变固定版本。

common 的归属以可复用的职责划分：传输、会话、确认输入时钟、通用回滚/录像机制、浏览器桥接与共享练习输入保留在 common。具体作品的对象布局、ECL/敌人/玩家状态、关卡恢复和玩法规则由各作品源码库维护。TH09 当前独有的对战规则继续留在 TH09，通用对战基础设施应允许后续作品接入，不能把“暂时只有一个调用方”作为必须移出的唯一理由。

只读内容对比还发现，TH11 的 `84b5eae` 和 TH15 的 `f4d318d` 虽不是 common/main 的祖先，但所选 `ConfirmedInputClock.hpp` 与 main 内容相同。不能仅因提交 ID 不同就重复合并。TH15 的 `AdonisSpectatorTiming.hpp` 则确有游戏 15 的编码、识别和校验差异，应在当前协议兼容约束下整合或参数化。common 工作区还有未提交的旁观/Replay 修改，整合基线必须由其当前修改完成后的状态确定。

路径调整不仅涉及环境变量：七个作品的已提交输入源码直接通过相对路径 include `third_party/eagler-common/include/eagler/input/DialogueGesture.hpp`。TH08、TH11、TH15 等构建脚本的头文件身份清单也直接引用这个旧路径。需要同时改为显式 include 目录和统一 common 路径，否则编译或增量构建身份仍会失效。

TH15 的 `portable/common-root.mjs` 和联机构建身份逻辑还要求 common 自身是 Git 顶层目录，否则会返回 `unversioned`。成为 monorepo 普通目录后，这部分也必须适配。

### 保留并登记第三方固定版本

TH06 和 TH07 的 SDL、SDL_image、SDL_ttf 分别是三个子模块，目前两作的对应提交一致。自有代码进入 monorepo 不要求将第三方完整历史也导入，可以继续固定为第三方子模块。迁移第一阶段可以保留两作现有 vendored 路径，去重另做。

各作品成为普通目录后，保留的第三方 gitlink 必须登记到 monorepo 根 `.gitmodules`，路径加上作品目录前缀。原作品目录里的 `.gitmodules` 不能直接承担新 Git 顶层配置的职责。[Git 官方说明](https://git-scm.com/docs/gitmodules)

miniaudio、ImGui、softfloat 等现有源码依赖先保留各自版本，不因搬目录顺带升级或全部去重。TH06/07 的可选 THPrac 仍引用工作区外 `thprac-reallyportable`，应改为显式配置固定的 ImGui/扩展输入，避免依赖开发者机器上的邻居目录。

### 统一工具链发现

目前存在 `EMSDK`、`TH09_EMSDK`、`TH15_EMSDK`、作品内 `tools/emsdk`、邻居 `th08/tools/emsdk` 和 `toolchains/emsdk` 等多种发现方式。TH06/07 CI 固定 6.0.5，本机 TH08 SDK 元数据为 6.0.9，迁移时不能把路径统一顺带视为版本兼容已验证。

统一工程入口应显式传入 SDK、Python、CMake/Ninja、字体与原版资源路径，并记录版本。先保持每个已验证构建的工具链版本，再另做升级验证。SDK、浏览器、node_modules、原版 DATA/音乐和构建缓存不进入源码 Git。

### 修复发布源码身份

调查时 `lib/release-manifest.mjs:sourceIdentity()` 检查传入目录必须等于 `git rev-parse --show-toplevel`。只读探针将现有仓库内的 `src/` 传入，稳定得到 `source repository is not initialized`，说明此检查会拒绝未来的普通作品目录。现已修复并增加真实临时 Git 仓库回归，旧重构中的这一限制不能再照搬。

改造后应记录 monorepo 提交 ID、组件相对路径及实际输入文件身份，保留脏工作区、生成桥接文件和第三方版本的识别能力。不能只放宽报错，再对每个组件记录同一个 HEAD 而漏掉真实输入。正式发布还要保留只接受明确候选和完整 Runtime 文件集合的现有边界。

### 更新 npm 与独立交付包

可以采用旧重构的根 npm workspace，但 workspace 列表要精确列出 contracts、web、netplay 等实际组件，不能递归纳入各作品中历史复制的 Launcher/package.json。保留 main 的依赖版本，将 scripts、TypeScript 配置、相对 import、包解析和锁文件作为一组调整，不引入新包管理器或任务框架。

旧重构将 Node 要求从 main 的 `>=22` 提高到 `^24.15.0 || >=26`。该变化不是 monorepo 的必需条件，应根据共享 `.mts` 的实际执行和打包方式决定，不直接照搬。当前自托管交付包仍要能脱离整个源码仓库安装运行，不能依赖只在根 node_modules 下成立的解析。

### 保持各作品的构建边界

TH06/07 继续各自使用 CMake，其他作品继续使用其正式 Node 构建入口，由根工具调度。不把所有工程直接塞进一个顶层 CMake `add_subdirectory()`：现有配置广泛使用 `CMAKE_SOURCE_DIR`，直接嵌套会改变路径语义。

根路径配置要区分工程根、Web 根、作品源根、普通/联机变体、输出目录和外部资源根。更新 `config/workspace.json`、`config/runtime-builds.json`、workspace-layout、开发服务器、内容准备和各作品脚本的同层路径假设。公开 Runtime URL、存档根、协议名称与源码检出路径分开，不因目录移动改变用户数据位置。

common 的同步脚本中，`tools/sync-gamepad-input.mjs` 直接寻找 `thXX-eagler` 邻居目录，需要改为消费统一路径表。其他 keyboard、startup、Replay 和 spectator 同步器也要清点目标。第一阶段保留现有生成桥接机制并核对生成内容，之后再考虑将共享源码直接纳入构建。

## 普通版与联机版的基线

| 作品 | 当前检出分支 | eagler 与 eagler-mp 独有提交数量 | 迁移前处理 |
| --- | --- | --- | --- |
| TH06 | eagler | 无独立本地 eagler-mp 分支 | 保留同树构建开关 |
| TH07 | eagler | 2 / 9 | 检查旧 mp 分支是否仍有应保留的独有改动 |
| TH08 | eagler-mp | 2 / 76 | 当前构建已支持 normal/mp 参数，但仍需保留普通分支的独有修改 |
| TH09 | eagler | 无独立本地 eagler-mp 分支 | 保留当前 normal/mp 打包方式 |
| TH10 | eagler | 5 / 60 | 不用普通树覆盖联机树，分别冻结来源 |
| TH11 | eagler | 4 / 18 | 独立 mp 检出仍存在，作为单独来源登记 |
| TH15 | eagler-mp | 0 / 21 | mp 已包含普通分支历史，仍需验证两种构建及 common 差异 |
| TH20 | eagler | 无独立本地 eagler-mp 分支 | 补齐统一构建声明 |

这些数字来自调查时的本地分支，包含合并提交，不是功能差异数量，也不代表远端最新状态。调查时 Launcher 的 workspace 配置还引用已经不存在的 `worktrees/th15-multiplayer`，现已修正为 `th15-eagler` 的 eagler-mp，没有恢复旧工作树。

## 体积与历史保留

各当前源码提交的已跟踪文件合计 17,364 个、331.31 MiB，包含文档、测试证据、图片和分析材料，不含子模块内容、原版游戏数据、工具链或构建产物。

| 输入 | 当前跟踪文件 MiB | 所选历史独立打包 MiB |
| --- | ---: | ---: |
| Launcher | 48.44 | 46.08 |
| common | 0.86 | 0.36 |
| TH06 | 4.74 | 4.14 |
| TH07 | 7.95 | 10.87 |
| TH08 | 21.27 | 10.96 |
| TH09 | 68.81 | 10.07 |
| TH10 | 17.41 | 197.04 |
| TH11 | 21.40 | 13.89 |
| TH15 | 23.18 | 12.66 |
| TH20 | 117.27 | 17.04 |
| 合计 | 331.31 | 323.11 |

历史测量将 HEAD 和本地 eagler/eagler-mp 分支可达对象通过 `git pack-objects --stdout --revs` 流式计数，没有写入 pack 文件、修改 refs 或执行 gc。323.11 MiB 是各仓库分别打包的总和，不是真实合并仓库的克隆大小，未计第三方子模块、未提交变化、其他未选择分支及未来提交，也未计跨仓库去重效果。

TH10 所选历史中仍可达旧 `tools/node.exe`，单个未压缩 blob 约 87.45 MiB，另有大量旧站点字体产物。删除当前文件不会消除这些历史对象。按已选定的“保留原提交 ID”方案保留它们，不执行过滤历史或大文件重写。TH09 当前 docs 约 58.96 MiB，TH20 当前跟踪树包含约 56.03 MiB 的 analysis 和约 30.17 MiB 的 source_reconstruction。迁移阶段只盘点这些目录，不展开旧工作流或自动删除证据。

源码历史通过保留原提交、增加带目录前缀的导入提交，并将原来源接入迁移后的可达提交图来保存。目录重排发生在新提交中，旧提交继续显示旧路径。不能使用 squash 或过滤导入来代替这一要求。路径前缀导入的 Git 能力与无共同历史的合并机制见 [read-tree](https://git-scm.com/docs/git-read-tree) 和 [merge](https://git-scm.com/docs/git-merge)。

原作品仓库保留，在迁移点建立明确的归档引用和来源清单。各仓库同名分支、tag 导入时需要命名空间。只有确认来源提交在 monorepo 中可达、历史对象完整、所有使用方已切换后，才另行执行原 eagler 分支退役。本轮没有创建远端归档引用，也没有删除任何分支。

各目录的 LICENSE、作者声明和第三方清单随组件保留。当前 TH06 是 GPL 文本，TH07 是 CC0，common 和 TH15 含 MIT 文本，不能拿 Launcher 的一个 LICENSE 替代所有目录的来源说明。TH20 的所选提交没有顶层 LICENSE，需要单独核对来源许可记录。

## 后续执行顺序与验收

1. 冻结主线及所有活跃 normal/mp/common 的完整基线，登记并行未提交工作，准备来源 SHA、归档命名和文件清单。
2. 先在现有结构内支持显式工程路径、单份 common、可注入工具链和 monorepo 源码身份，保留旧布局测试通过。
3. 从最新 main 重建旧重构中可用的 workspace 与目录映射。目录迁移、路径适配、历史导入分别保持可审查，避免同时修改游戏行为。
4. 导入各作品历史与源码，处理顶层第三方依赖登记，再更新 npm 锁文件、CI、测试、开发服务器、发布和独立自托管包。
5. 从全新检出验证：npm 安装、类型检查、前端构建、common/native 测试、八作品适用的 normal/mp 源码构建、桥接同步检查、Runtime 闭合清单、正式发布身份和独立包运行。浏览器覆盖启动、输入、存档、Replay 及联机生命周期。
6. 所有构建和使用方切换完成后，再处理旧分支退役。推送、部署和删除分支各自按明确授权执行。

迁移门禁还应验证旧 SHA 对象存在、对应来源是新历史的祖先、输入源码和许可文件完整、fresh clone 不依赖本机邻居目录，以及公开存档身份和协议兼容性保持。CI 必须迁入各作品现有的有效检查，不能仅使用旧重构的 common 测试来宣称八作品可构建。

## 调查产物

本机审计目录为 `D:/workspace/eagler/artifacts/monorepo-preparation-20261016/`：

- `inventory.json`：源码 SHA、分支、脏工作区、gitlink、文件大小和所选历史的大对象。
- `analysis.json`：旧重构对比、功能文件位置、源码身份探针和 common 固定版本。
- `history-packs.json`：十个来源的历史打包体积和 SHA256。
- `prepared-source-plans.json`：当前实际检出的十五个构建变体的路径/依赖预检查。
- `implementation-check.log`、`implementation-typecheck.log`、`implementation-input-tests.log`：完整检查、类型检查和输入回归的输出。
- `audit.mjs`、`analyze.mjs`、`measure-history.mjs`：可重复运行的只读调查脚本，仅写本目录下的报告。

## 原目录准备的验证结果

新布局解析、Git 组件身份、发布清单和发布入口、自托管打包、构建配置和测试入口归属检查通过。合成源码/SDK 夹具覆盖十五个 normal/mp 变体、固定 common 优先、显式路径覆盖、错误工具链版本和无效选项。前端类型检查与生产构建通过，输入回归 46 项通过。

本机现有文件的构建计划预检查为 14/15。TH15 联机的 `third_party/eagler-common-netplay` 只有 `.git` 文件，缺少 `include/eagler/netplay/SessionChannel.hpp` 等源码，预检查明确失败。按用户对 common 的最新要求，本轮没有初始化、替换或更新这一依赖，也没有自动切到另一版本。

完整 `check` 跑到 `tests/test-runtime-generations.mjs` 时，Windows 创建目录符号链接返回 `EPERM`，因此全量门禁未通过。未更改系统权限或跳过该测试。前面的前端构建、专项检查和打包验证不能替代此项 PASS。

本轮没有执行十五个变体的实际 Runtime 全量编译、迁移后的 fresh clone 或完整浏览器/发布验收。目录迁移、历史导入、common 整合和对应验收仍属于后续工作。

## 全量复查

2026 年 10 月 16 日在当前 main 工作区补跑全部仓库测试、显式跨作品检查、前端测试和输入浏览器专项，单项失败后继续执行。完整检查未通过，不能只归因于 Windows 权限。

仓库基础测试任务为 133 通过、1 失败，失败为目录符号链接的 `EPERM`。跨作品检查为 7 通过、17 失败，涉及旧工作树路径、已退役入口、遗漏作品、过时源码断言和未更新的打包夹具。TH06 SDL 当前也没有既有契约测试要求的 4096 帧下限，需要单独核对音频策略。

前端 84 个测试文件均执行完成。最终统计为 1,149 个用例通过、194 失败、无取消或跳过，失败集中在 11 个文件。多数失败来自旧翻译表、产品声明及 CSS 的逐字基线比较或夹具初始化。仍需核对后台 OGG 额外失败提示，以及仅导入模式下有效 ZIP 导入后弹窗未按预期隐藏的断言。此前构建超时的 development-server 测试保持源码和超时设置不变复跑，3 项全部通过。

生产构建、两个 TypeScript 类型检查、语法检查与公开内容审计通过。输入专项覆盖三个界面宽度、八作品键盘桥接、三引擎七种虚拟手柄布局的 168 个组合，以及实际 TH09 Release 的四种输入组合，均通过。这些结果不代表实体手柄或输入延迟验证。十五个实际构建计划仍为 14/15，TH15 MP 的固定 common-netplay 缺少源码。

目录迁移前还需去掉 TH11 构建头文件清单中直接读取旧 common 子目录的依赖，并让 TH10/TH20 底层脚本完整采用统一入口的并行度设置。本轮没有修改 common、移动源码目录或改动功能以绕过测试。

完整失败分类、逐项日志与复查结果保存在 [全量检查记录](D:/workspace/eagler/artifacts/full-validation-20261016/report.md)。

## 10 月 17 日修复与原版视觉复核

按用户追加要求，在本轮补齐 TH09/15/20 的常显判定点，并检查 TH06/07/08/09/10/11/15/20。TH10 已有原生视觉实现，但前端先配置、World 后创建会丢掉开关，普通/联机树均修正为由 Application 保存并在创建 World 时继承。TH09/15/20 使用各自原版素材与动画，新增状态属于显示侧，不写入原生焦点输入或改变移动/碰撞。绘制复用原版特效层并避开原生判定圈的退场阶段，防止重复叠画。

原版资源的组件对照共 1,440 个逻辑帧：TH09 240 帧，TH15 720 帧，TH20 480 帧。后两作覆盖常速和半速，TH15 另覆盖两种缩放倍率。对照执行真正的原生 ANM 解释器/插值组件，检查缩放、旋转、透明度、素材选择、TH15 顶点坐标、重复绘制稳定性以及共享 RNG/注册表边界。它不是完整游戏逐帧确定性或浏览器逐像素证明。TH06 原版没有可见判定圈，保留已有 TH07 素材扩展作为明确例外。全作规则、来源与重跑入口已记入 input-touch 主 playbook。

修复了跨作品测试的旧路径、退役入口和覆盖缺口，保留独立行为断言，用实际 portable C++ 组件替换部分旧源码文本判断。前端旧整份翻译/CSS/产品表的逐字检测改为真实行为或结构/样式分区验证。有效导入包测试等待实际安装完成，修正 OGG 夹具的已准备轨道状态。Windows 刷新目录共享锁采用有界重试并保留失败回滚。原子符号链接发布在 Windows 上提前明确拒绝，不退化为非原子复制；POSIX 仍使用原实现和测试。

TH08/09/10/11/15 及独立 TH10MP/TH11MP 的触摸头文件通过选定的 include 路径解析 common，构建输入清单跟随同一依赖。每个使用方在固定 common 和显式 common 下都做了实际头文件编译，共 14 组。TH09 接受统一并行参数并保留最多 4 路的限制，TH10/20 采用统一并行参数，TH20 打包支持显式输出路径。

新增字体实现的依赖也纳入源码构建预检。最新默认固定依赖为 12/15，通过显式指定现有 common 为 14/15：TH11 普通版和 TH15 的固定 common 缺 RuntimeFontRaster.hpp；TH15MP 另外缺固定 common-netplay 源码。此前的 14/15 只检查旧依赖标记，不能覆盖后来并行加入的字体头文件。没有初始化、替换、更新 common 或其固定版本。TH15 本地普通版构建明确使用现有 common 覆盖，因此该成功不等于默认固定依赖已修复。

全量修复初跑 222 个任务，其中 215 通过、7 失败。7 项随后分别复跑通过：测试归属、开发服务器、目录页面、外部前端组装、游戏准备、部署刷新、CSS 分区。跨作品 24 项初跑仅帧调度测试自身的变量名错误，修正后该项通过，其余 23 项通过。独立冷源码 App Shell 构建的 8 个用例再次通过，两个 TypeScript 检查及公开内容审计再次通过。本工作区同时有其他任务修改，以上是逐项运行证据，不是冻结提交上的单次全绿 CI。

实际编译 TH09、TH10 普通/联机、TH15 普通及 TH20，并生成私有验证包。浏览器覆盖修改涉及的普通 Runtime，启动前启用判定圈、进入实际关卡、按住及松开低速，截图与清单保存在本机修复验收目录。没有执行十五个变体的全量真实构建、迁移后的 fresh clone、实体手柄或公开部署验收。

本轮没有移动目录，没有导入/重写历史，也没有提交、推送或部署。完整修复证据位于 `D:/workspace/eagler/artifacts/repair-validation-20261016/`，保留之前的全量检查失败报告，不用新结果覆盖旧日志。

随后按用户要求补齐 TH11、TH15 普通版的 common 字体依赖，并提交共享字体实现及两作的 gitlink，统一固定到 `edcd08eb441e3175583c504e1a0dd63636a8a5d7`。两作本地默认依赖构建通过，提交的干净快照也已完成默认依赖构建。原固定版本和本地已有依赖分支的祖先均保留，联机进行中的源码与双人版专用依赖由另一对话处理。尚未推送，不能据此宣称远端全新克隆已通过。详见 [common 依赖更新记录](2026-10-17-common-dependency-update.md)。
