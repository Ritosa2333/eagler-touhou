# th13 开发指南

## 总体架构

- **行为还原**：原版 th13.exe（v1.00c）经 Ghidra headless 反编译导出函数候选与调用图；还原后的 C++ 落在 `web/cpp/game/`，按子系统推进（RNG → 帧循环 → ANM VM → ECL VM → 敌/弹/道具 → 自机 → 神灵/灵界系统）。
- **双目标同源**：同一份 C++ 编译两次——
  1. 网页运行时：Emscripten + SDL3（`portable/build.mjs` → `web/artifacts/sdl3/`）；
  2. WASI 无头 harness：wasi-sdk（`portable/build-harness.mjs` → `web/artifacts/wasi/`），供 oracle 对照与 CI。
- **Oracle 对照**：Unicorn 无头模拟原版 EXE（桩掉 Win32/D3D），以录像驱动逐帧导出状态池，与 harness 输出逐字段比对，首个分歧即回归定位点。原版二进制与 oracle 轨迹只在开发期使用，不进网页构建。
- **地址锚点**：`web/cpp/rebuild/AddressMap.hpp` 登记每个子系统的原版入口，供 oracle 与回归脚本引用。
- **呈现**：画面只在动画帧回调内提交。窗口尺寸为零时保留待提交帧，帧缓冲首次作为绘制目标绑定时检查完整性；页面带 `?glDebug=1` 时每次呈现后读取 GL 错误码。画布上下文属性集中在 `web/sdl-runtime/context-attributes.mjs`。打包入口是 `portable/build-eagler.mjs`（`build.mjs` 加 `package-eagler.mjs`）；目录型运行时的文件清单由 `package-eagler.mjs` 与产品目录的 `runtimeAssets` 共同维护，改清单时两处一起改。

## 统一容器环境

全工作流在标准容器内进行（本地 podman 与 GitHub Actions 共用本目录 `container/Dockerfile`，多阶段：base → tools → dev → ci / shot）。Emscripten 端口缓存经 `EM_CACHE` 落工作区卷（`.cache/emscripten`），跨容器运行复用。

仓库内自带 Dockerfile，在 monorepo 根目录构建一次：

```sh
podman build --target dev -t th13-dev --build-arg DEV_UID=$(id -u) --build-arg DEV_GID=$(id -g) ports/th13/container
podman build --target shot -t th13-shot ports/th13/container
```

日常全部经统一入口 `tools/container.sh`，它固定挂载、用户映射与缓存目录，容器内即 `/work/eagler-touhou/ports/th13`：

```sh
# 构建与测试（th13-dev 镜像）
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh node portable/build.mjs
# 多人版目录
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh --mp node portable/build.mjs
# 浏览器验证（th13-shot 镜像）
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh --shot bash
```

容器内进程不可转储：宿主用 systemd-coredump 收集转储时，容器里的崩溃会写进宿主的 `/var/lib/systemd/coredump`。需要在容器里取转储时设 `TH13_ALLOW_COREDUMP=1`。

外层工作区提供 `games/th13`（原版资源）与 `artifacts/`，默认取 monorepo 的上级目录；以 `TH13_` 或 `TH_` 开头的环境变量原样传入容器。

工具链版本（构建时固定）：Node 22 / Emscripten 6.0.9 / wasi-sdk 34 / Ghidra 12.1.3 / Temurin JDK 21。

工作区顶层的 `games/`（原版资源）与 `artifacts/`（大体积产物、Ghidra 导出、参考视频帧）不入库。外层参考工作区另有 `Makefile` 包装这些命令（`make image / shell / exec-port CMD='…'`）；本目录的入口不依赖它。

## 原版资源准备

```sh
make extract   # 解包 th13ogg.7z → games/th13/，生成 SHA256 清单与版本判定
```

原版目录包含 `th13.exe`、`th13.dat`、`thbgmogg.dat`（OGG 音乐版）。资源提取（ECL/ANM/STD/msg dump）由 thtk 完成；dump 文本属开发期对照证据，落**工作区** `artifacts/reference/dumps/`（不入库），格式版本结论记录在 `docs/ASSETS.md`。

## 行为还原节奏

每个子系统三重门：Ghidra 参考 → 回归摘要门禁（oracle 缺席时的主门槛）→ `node --test`。各子系统对应的原版入口、已知分歧与未决项见 [fidelity-map.md](fidelity-map.md)。恢复顺序：

1. RNG 与帧计时
2. ANM 虚拟机（动画脚本）
3. ECL 虚拟机（敌机脚本）
4. 敌人 / 弹幕 / 道具管理器
5. 自机
6. th13 特有：神灵收集、灵界 Trance、Overdrive
7. Boss / 符卡 / 全 Stage 通关

### 数值语义纪律（继承参考工作流 machine.hpp + softfloat 的教训）

原版以 MSVC x87 80 位中间量运算，移植侧 SSE2 语义默认不一致。已发生的修正（wrap_angle 每步落 f32、SquareSumRoot 单舍入链、圆周模式插值角维、floor 量化）全部属于此类。规则：

- 新子系统凡涉及**双精度链、超越函数、连加累乘后单次取整**的路径，先写 oracle 用例锁定原版结果，再落 C++；必要时引入 x87 语义助手（参照 ports/th10 的 `web/cpp/rebuild/machine.hpp` + softfloat extF80 子集），不逐例手修散落各处
- 遇到「分歧在数值第 N 位小数」类对齐失败，优先怀疑 x87/SSE 差异，其次才是逻辑错误

### 与静态重编译路线的关系（混合升级预案）

参考工作流（th10）采用机械静态重编译 + 渐进类型化替换的双层形态（`rebuild/` 与 `game/` 并存）。本项目当前为 clean-room + oracle 验收路线，Stage 1 事件流已逐字节对齐证明可行；若后续关卡/Boss 重符卡段频繁卡在浮点或边界语义，启动最小转译层（页级翻译仅覆盖卡住的函数，与 `game/` 并存），向参考工作流的混合形态收敛。

## 代码排版

C++ 源码由 `tools/format.mjs` 按端口目录下的 `.clang-format` 统一排版，检查与修复同一条命令：

```sh
ports/th13/tools/container.sh bash -lc 'node tools/format.mjs --check'        # 普通版：只检查，未排版即非零退出
ports/th13/tools/container.sh bash -lc 'node tools/format.mjs'                # 普通版：排版
ports/th13/tools/container.sh --mp bash -lc 'node tools/format.mjs --check'   # 多人版
```

- **范围**（`tools/source-layout.mjs`）：`web/cpp`、`portable`、`tests` 下的 C/C++，不含 `third_party` 与生成文件。多人版只排版与普通版同路径的文件，联机目录保持自己的原样。
- **工具链**：镜像内 wasi-sdk 自带的 clang-format，全仓库同一个版本；`.clang-format` 两侧逐字节相同，由 `tests/shared-sources.test.mjs` 守住。
- **内联 JS**：`EM_JS` / `EM_ASM` 的函数体在预处理里是裸 token，按 C++ 规则重排会把它改坏（`??` 变成 `? ?`，`=>` 变成 `= >`）。每一处都放在 `// clang-format off` 与 `// clang-format on` 之间；`tools/format.mjs` 发现未受保护的内联 JS 会在动手前停下并指出位置，同一条规则由测试固化为门禁。

排版后的产物与排版前逐字节相同：两个端口的网页 wasm、JS 胶水层与 WASI harness 的 sha256 全部不变。全量排版提交登记在仓库根的 `.git-blame-ignore-revs`，执行 `git config blame.ignoreRevsFile .git-blame-ignore-revs` 后 `git blame` 自动跳过它们。

## 源码不写注释

两个端口的源码、脚本、测试、页面与配置文件都不带注释：`//`、`/* */`、`#`、`<!-- -->` 和 Python docstring 一律不出现。原本写在注释里的信息各有去处：

| 内容 | 去处 |
| --- | --- |
| 原版地址与行为依据 | 提交说明；[fidelity-map.md](fidelity-map.md) |
| 契约与不变量 | 断言它的测试 |
| 用法与流程 | 本目录 `docs/` |
| 排版保护 | `// clang-format off` / `on` 是给排版器的指令，保留 |

`tools/no-comments.mjs` 按语言取注释区间：C/C++/Java 与 CSS 用词法状态机，字符串、字符字面量与原始字符串字面量都识别；JS 与 HTML 内联脚本用 acorn；shell 识别引号、`$()`、`${}` 与 heredoc；Python 用 tokenize 与 ast，docstring 一并删除。范围是 git 已跟踪的文件，生成文件（`*.gen.hpp`、`MsgPlayerCp932.hpp` 等）与 `third_party/` 不在内。多人版 `shell.mjs` 的 `// BEGIN GENERATED SPECTATOR PLAYBACK` 到 `// END GENERATED SPECTATOR PLAYBACK` 是与 `common/browser/spectator-playback.mjs` 逐字节一致的副本，原样保留。

```sh
ports/th13/tools/container.sh bash -lc 'node tools/no-comments.mjs'          # 普通版：检查，发现注释即非零退出
ports/th13/tools/container.sh bash -lc 'node tools/no-comments.mjs --strip'  # 普通版：删除
ports/th13/tools/container.sh --mp bash -lc 'node tools/no-comments.mjs'     # 多人版
```

`tests/no-comments.test.mjs` 把规则纳入门槛。全量删除的两个提交登记在仓库根的 `.git-blame-ignore-revs`，`git blame` 自动跳过它们。

## 测试

```sh
node --test tests/*.test.mjs   # 仓库内单元测试（rng / ecl / ui / replay）
bash tests/ui/run.sh           # UI 基座六套件（输入/调度/ANM 句柄/HUD/标题/暂停/对局）单独运行
node portable/build-harness.mjs && node --experimental-wasi-unstable-preview1 tools/run-harness.mjs   # WASI harness（RNG 冒烟）
node --experimental-wasi-unstable-preview1 tools/run-harness.mjs stage1   # stage1：st01 跑 3600 帧，事件流落 web/artifacts/wasi/stage1-events.jsonl（契约见 docs/plans/event-contract.md）
node --experimental-wasi-unstable-preview1 tools/run-harness.mjs replay games/replays/th13_udSr01.rpy   # 录像解码 + 逐帧输入注入（格式见 docs/plans/replay.md）
```

### 一键门槛

```sh
ports/th13/tools/gate.sh            # 普通版
ports/th13/tools/gate.sh --mp       # 多人版（套件另含 M1–M9 累计原生工作流）
ports/th13/tools/gate.sh --browser  # 追加浏览器套件（th13-shot 镜像）
```

依次执行排版检查、WASI harness 构建、三项回归摘要（普通版）、网页运行时构建、全部套件与基线比对，任何一步回归即非零退出。基线是动产品代码之前登记的逐套件失败清单（`archive/th13-refactor-baseline-20261124/`），带基线比对时退出码只反映基线之外的新失败。

`--browser` 需要运行时资源目录，固定 4 并发：更多并发会让无头 Chromium 的音频线程饿死，报 AudioContext 错误。

### 回归摘要门禁

重构类改动以逐位不变的回归摘要证明行为不变：harness 在整段录像上每 60 帧采样一次状态池、事件流与绘制清单的滚动 FNV-1a 摘要，与 `tools/replay-verifier/golden/` 的金标逐行比对，报出首个分歧的录像、关卡与帧。

```sh
node portable/build-harness.mjs
node tools/regression-digest.mjs --check                              # 状态与事件：10 份录像、35 关、58 万帧
node tools/regression-digest.mjs --draws --check                       # 绘制清单：每份录像首关前 600 帧
node tools/regression-digest.mjs --draws --draws-stages all --check    # 绘制清单：每关前 600 帧
```

三项合计约两分钟。工具默认就是比对；重生成金标要显式加 `--write`，只有改变模拟或绘制行为的修正才这样做，并在提交说明里写明依据。

输入是外层工作区的 `games/replays/*.rpy` 与 `artifacts/reference/raw/demo0..3.rpy`。harness 在 `TH13_DIGEST_LOG` 下逐采样帧写 `stage <s> frame <f> state <16hex> events <16hex> [draws <16hex>] stepped <0|1>`，末行是 `total`；没跑完的录像在金标里记为 `stable:false`，只保留停止点，不参与摘要比较。构建身份（wasm sha256）只作来源记录，比对不看它——门禁比较的正是不同构建。工具拒绝使用旧于源码的 harness 产物。

harness 进程固定带 `--no-incremental-marking`（调用方没带时运行器重启自身）。Node 22.23 的 V8（12.4.254.21）在增量标记的栈扫描里会遍历到返回地址无效的帧，在 `CommonFrame::IterateTurbofanOptimizedFrame` 内解引用崩溃，Node 的信号处理器把它重发为 SIGSEGV。这与移植代码无关：崩溃与 `--stack-size`（200KB–15MB）和 JIT 模式无关；关掉增量标记后，叠加 `--gc-interval=500 --stress-compaction --verify-heap` 的整卷回放全部跑完、堆校验通过，原生 ASan/UBSan 构建同样全绿。

### 性能测量

热点以带函数名的 harness 剖析为准：`portable/build-harness.mjs` 的链接参数加 `-g`，再用 `node --cpu-prof` 采样。计时一律用交替 A/B（基线与改动版交替构建运行三轮取中位数），因为同一台机器在不同时刻的绝对耗时相差可达一倍。保留一项改动的条件：三项回归摘要不变、门槛无新增失败、交替 A/B 有实测收益。首轮结果与剖面分布见 `archive/th13-performance-20261124/README.md`（WASI harness 回放计时，不含浏览器端帧时间）。

### 网页 thprac

在启动器开启单机 thprac 后，按 P 打开面板并应用参数。设置用于下一次从标题开始的普通对局，选定节点会从所属关卡的开头开始。关内重试沿用同一组参数，返回标题后可直接开始普通对局。

thprac 对局使用关卡练习的结算流程，普通通关、最高分、符卡记录和游玩统计均不计入存档，也不生成普通录像。

## 同帧取像（golden 对拍）

```sh
# 工作区根：th13-shot 镜像内驱动（先 node portable/build.mjs 出产物）
make shot CMD='node eagler-touhou/ports/th13/tools/shot-frames.mjs'
```

链路：th13-debug.html 回放会话直入对局 → `th13_debug_frame` 固定 1/60 确定性帧泵 →
按 golden 游戏帧号跳帧导出 `artifacts/reference/golden/ours-frame-*.png` →
双会话确定性自检 → 合成 `side-by-side.png`。量测与阈值判定：
`python3 scripts/visual_diff.py --golden artifacts/reference/golden --ours <移植帧目录>`
（帧文件需按 `frame_%06d.png` 命名，驱动产物改名复制即可）。
