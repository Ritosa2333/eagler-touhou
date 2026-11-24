# th13 / th13-mp 重构基线（2026-11-24）

重构动手之前，在统一容器里把普通版与多人版的全部测试套件各跑一遍并登记结果。此后每个里程碑用 `ports/th13/tools/gate.mjs --compare` 与本基线逐测试比对，出现基线之外的新失败即判定回归。

## 环境与范围

- 源码：monorepo `3acc88741`，分支 `refactor/th13-quality`。
- 容器：`th13-dev` 镜像（Node 22.23.3、Emscripten 6.0.9、wasi-sdk 34、g++ 12），经 `ports/th13/tools/container.sh` 进入。
- 输入：`games/th13/th13.exe`（v1.00c）、`artifacts/reference/raw`（202 个原版资源）、`games/replays/*.rpy`（6 份）、`artifacts/score_golden`。
- 范围：原生套件 anm、ecl、ui、replay、items、score、combat、bomb、audio、e2e，JS 套件；多人版另有 contracts 与 M1–M9 累计原生工作流。浏览器套件见下文；回归摘要另有记录。
- 复跑前先准备运行时资源目录：两侧各执行 `node web/sdl-runtime/prepare-assets.mjs`。基线记录时该目录尚未生成，anm 套件有两个用例因此失败。

## 结果

| 端口 | 套件 | 通过 | 失败 | 断言 |
| --- | --- | ---: | ---: | ---: |
| 普通版 | anm / ecl / ui / replay / items / score / audio / js | 952 | 11 | — |
| 普通版 | combat、bomb | 退出码 0 | 0 | — |
| 多人版 | anm / ecl / ui / replay / items / score / audio / js / contracts | 953 | 14 | 1,683,671 |
| 多人版 | bomb | 退出码 0 | 0 | — |

两侧并行各约 4 分钟。逐套件数字与逐条失败见 [baseline.json](baseline.json)。

## 25 项失败的成因

25 项全部来自环境输入缺席或测试自身写法。

| 成因 | 普通版 | 多人版 |
| --- | ---: | ---: |
| 缺原版字体 `msgothic.ttc`（e2e；多人版另有 M1–M9 工作流） | 1 | 2 |
| 多人版独有的对话气泡恢复测试 `independent_real_bubble_restore_…` 按旧的相对层级查找原版 ANM，在容器工作目录下落空 | 0 | 1 |
| ecl 测试把事件输出写死到本机不存在的历史证据目录 `artifacts/verification/*-20261027` | 5 | 5 |
| 运行时资源目录未准备：anm 测试读 `web/sdl-runtime/assets/bullet.anm` | 2 | 2 |
| 缺输入：`png_decode_stb` 读取 thtk 提取产物 `artifacts/reference/textures/effect/eff_switch.png` | 1 | 1 |
| `submission.test.mjs` 只编译 `GraphicsDevice.cpp`，缺 `LanguagePack.cpp` 而链接失败 | 1 | 1 |
| 容器内没有 launcher 的 `node_modules`（`managed-shell.test.mjs` 缺 acorn） | 1 | 1 |
| combat 套件要编译依赖 libdatachannel 的网络代码，镜像内没有该库 | 0 | 1 |

## 复跑与比对

```sh
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh bash -lc \
  'node tools/gate.mjs --group all --compare ../../archive/th13-refactor-baseline-20261124/baseline_th13.json'
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh --mp bash -lc \
  'node ../th13/tools/gate.mjs --group all --compare ../../archive/th13-refactor-baseline-20261124/baseline_th13_mp.json'
```

`baseline_th13.json` 与 `baseline_th13_mp.json` 是供 `--compare` 读取的逐套件失败清单。带 `--compare` 时，退出码只反映基线之外的新失败；基线中的失败被修好时列为“修复”。

基线记录之后，表中测试侧的成因已消除：事件出口改写到 `artifacts/verification/th13-ecl/`，anm 套件的两个用例改用内嵌 PNG 夹具，`submission.test.mjs` 补齐编译单元，两个 ui 测试补上与其余测试相同的资源路径。复跑时普通版 9 项、多人版 10 项列为“修复”。

基线的 ui 套件里另有每侧 5 个用例因同一资源路径问题打印“# 跳过”并以 0 断言通过：`dialogue_text_real_text_anm_label2_visibility`、`dialogue_text_real_front_bubble_and_side_position`、`world_quad_fore_pool_layer_keeps_current_camera`、`item_border_line_uses_original_stage_one_ordinary_gate`、`item_border_line_creates_real_front_script_67_tree`。它们计入上表的通过数；补上路径后真实执行并通过，通过数不变。

## 浏览器套件

浏览器套件没有在动产品代码之前登记；下表是呈现链加固合入后的首次结果（两侧各约 3 分钟）。

```sh
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh --shot bash -lc \
  'cd /work/eagler-touhou/ports/th13 && node --test web/tests/sdl/*.test.mjs'
```

| 端口 | 用例 | 通过 | 失败 |
| --- | ---: | ---: | ---: |
| 普通版 | 15 | 6 | 9 |
| 多人版 | 15 | 6 | 9 |

两侧失败用例与原因相同：

| 成因 | 用例数 |
| --- | ---: |
| 缺原版字体 `assets/msgothic.ttc`：对话与 ANM 文本栅格化、调试页启动报“资源缺失”，两个触屏用例因该 404 触发控制台错误断言（其行为证据均符合预期） | 7 |
| 缺外层 `oracle.replayfmt` 模块（独立 Python 录像读取器） | 1 |
| 缺 launcher 的 `node_modules`（`shell-replay-channel.test.mjs` 需要 acorn） | 1 |
