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

一键复跑：`ports/th13/tools/gate.sh` 与 `ports/th13/tools/gate.sh --mp`，在容器里依次执行排版检查、WASI harness 构建、三项回归摘要（普通版）、网页运行时构建、上述套件与基线比对。

`baseline_th13.json` 与 `baseline_th13_mp.json` 是供 `--compare` 读取的逐套件失败清单。带 `--compare` 时，退出码只反映基线之外的新失败；基线中的失败被修好时列为“修复”。

基线记录之后，表中测试侧的成因已消除：事件出口改写到 `artifacts/verification/th13-ecl/`，anm 套件的两个用例改用内嵌 PNG 夹具，`submission.test.mjs` 补齐编译单元，两个 ui 测试补上与其余测试相同的资源路径。复跑时普通版 9 项、多人版 10 项列为“修复”。

基线的 ui 套件里另有每侧 5 个用例因同一资源路径问题打印“# 跳过”并以 0 断言通过：`dialogue_text_real_text_anm_label2_visibility`、`dialogue_text_real_front_bubble_and_side_position`、`world_quad_fore_pool_layer_keeps_current_camera`、`item_border_line_uses_original_stage_one_ordinary_gate`、`item_border_line_creates_real_front_script_67_tree`。它们计入上表的通过数；补上路径后真实执行并通过，通过数不变。

## 浏览器套件

浏览器套件没有在动产品代码之前登记；下表是呈现链加固合入后的首次结果（两侧各约 3 分钟）。

```sh
TH13_WORKSPACE=<外层工作区> ports/th13/tools/container.sh --shot bash -lc \
  'cd /work/eagler-touhou/ports/th13 && node --test --test-concurrency=4 web/tests/sdl/*.test.mjs'
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

### 替代字体下的复跑

本机没有原版字体 `msgothic.ttc`。验证时把系统字体 `NotoSansCJK-Regular.ttc` 复制为 `ports/th13/web/sdl-runtime/assets/msgothic.ttc`（该目录被 Git 忽略，旁边放 `msgothic.ttc.SUBSTITUTE.txt` 标记），两侧共用。替代字体只让套件跑起来，不进产物；文字外观类断言不在覆盖范围内。两侧各约 6.5 分钟。

| 端口 | 用例 | 通过 | 失败 | 跳过 |
| --- | ---: | ---: | ---: | ---: |
| 普通版 | 22 | 13 | 1 | 8 |
| 多人版 | 22 | 13 | 1 | 8 |

| 去向 | 用例数 | 说明 |
| --- | ---: | --- |
| 失败 | 1 | `browser EOF fixture…` 依赖外层 `oracle.replayfmt` 模块，本机没有 |
| 跳过 | 8 | `shell-replay-channel.test.mjs` 需要 acorn，`th13-shot` 镜像里没有；该文件由 `th13-dev` 镜像的 js 套件运行 |

并发固定为 4：按 `node --test` 的默认并发（本机约 22）时，无头 Chromium 的音频线程被饿死，报 `The AudioContext encountered an error from the audio device or the WebAudio renderer.`，两个断言“零控制台错误”的用例因此失败。`tools/gate.sh --browser` 已固定 4 并发，两侧按此复跑结果一致。

## 一键门槛与回归摘要

| 端口 | 耗时 | 套件 | 排版 |
| --- | ---: | --- | --- |
| 普通版 | 383 秒 | 980 项通过；e2e 1 项基线内失败 | 484 个文件通过 |
| 多人版 | 255 秒 | 1017 项通过（含 M1–M9 累计原生工作流 56 项、78,781 条断言）；combat、e2e、js 各 1 项基线内失败 | 479 个文件通过 |

两侧均为“相对基线无新增失败”。三项基线内失败都来自环境输入：e2e 需要原版字体，combat 需要镜像外的 libdatachannel。多人版的 js 失败（`managed-shell.test.mjs` 缺 acorn）在这次运行之后改用镜像内 emsdk 自带的 acorn，复跑 30 项全过。

普通版三项回归摘要全部 digest-ok（10 份录像、35 关、580,001 帧），harness 哈希 `6bf3bdd4…`；多人版网页运行时 wasm 哈希 `2de61752…`，与本轮保真地图和注释修正之前相同。

## 多人版浏览器验收

`tests/multiplayer/acceptance.mjs`（th13-shot 镜像，逐例打开生产调试页）：

relay 依赖在容器内临时安装（`ws@8.21.3`，运行结束即清理）。完整结果：

| 里程碑 | 用例 | 结果 |
| --- | ---: | --- |
| M1–M8 连跑 | 25 | 25 项全部通过，传输路径覆盖 rtc 与 relay |
| M9（含观战） | 10 | 10 项通过，共 642 条断言，含观战者加入与观看（`spectator_confirmed_latejoin`、`slow_spectator_isolation`） |

对照组是分支前的树（`main` `cbf99745e` 加调试页多席接口），同一镜像、同一 relay 条件下运行：M1–M8 连跑 25 项全部通过，`runtime_page_soak` 通过（161 条断言）。

`m8.rollback_one_three_twelve`（延迟注入后触发回滚）与 `m9.runtime_page_soak`（帧号差上限）按实时帧率计时。当前树在机器被并行构建占满时的一次连跑里，这两项各失败过一次（回滚未触发；帧号差 14 帧）；机器空闲时的两次复跑和对照组全部通过。复跑这两个用例时保持 CPU 空闲。

## 源码去注释后的等价性

提交 `9fe317d7b`（普通版，555 个文件）与 `a2ada0d97`（多人版，674 个文件）用 `tools/no-comments.mjs` 删除源码、脚本、测试、页面与配置文件里的全部注释并重新排版，对照 `089c4066c` 逐文件验证：

| 文件类型 | 验证方法 | 普通版 | 多人版 | 结果 |
| --- | --- | ---: | ---: | --- |
| C/C++ | g++ `-fpreprocessed -dD -E -P` 预处理后分词比较 | 465 | 540 | 词法单元序列一致 |
| JS | acorn 语法树，去掉位置信息 | 51 | 94 | 一致 |
| HTML | 内联脚本语法树与非脚本文本 | 2 | 5 | 一致 |
| Python | 语法树，两侧都去掉 docstring | 3 | 5 | 一致 |
| shell | `bash -n` | 18 | 18 | 通过 |
| CSS、`.gitignore`、Dockerfile | 去掉注释后的文本比较 | 2 | 2 | 一致 |

另有 3 个编译单元的 `#include` 顺序随排版改变（夹在 include 之间的注释删除后，clang-format 把它们并成一块重新排序），编译产物证明它没有语义影响：

| 产物 | sha256（去注释前后相同） |
| --- | --- |
| 普通版网页 wasm / JS 胶水 / WASI harness | `55a873c2…` / `9da5ebcb…` / `6bf3bdd4…` |
| 多人版网页 wasm / JS 胶水 / WASI harness | `2de61752…` / `77f4de06…` / `b64f8459…` |

去注释后两侧 `tools/gate.sh --browser` 均为“相对基线无新增失败”，浏览器套件与基线相同（13 项通过、8 项跳过、1 项基线内失败）。
