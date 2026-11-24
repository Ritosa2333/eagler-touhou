# th13 / th13-mp 重构基线（2026-11-24）

重构动手之前，在统一容器里把普通版与多人版的全部测试套件各跑一遍并登记结果。此后每个里程碑用 `ports/th13/tools/gate.mjs --compare` 与本基线逐测试比对，出现基线之外的新失败即判定回归。

## 环境与范围

- 源码：monorepo `3acc88741`，分支 `refactor/th13-quality`。
- 容器：`th13-dev` 镜像（Node 22.23.3、Emscripten 6.0.9、wasi-sdk 34、g++ 12），经 `ports/th13/tools/container.sh` 进入。
- 输入：`games/th13/th13.exe`（v1.00c）、`artifacts/reference/raw`（202 个原版资源）、`games/replays/*.rpy`（6 份）、`artifacts/score_golden`。
- 范围：原生套件 anm、ecl、ui、replay、items、score、combat、bomb、audio、e2e，JS 套件；多人版另有 contracts 与 M1–M9 累计原生工作流。浏览器套件（`th13-shot` 镜像）与回归摘要另有记录。

## 结果

| 端口 | 套件 | 通过 | 失败 | 断言 |
| --- | --- | ---: | ---: | ---: |
| 普通版 | anm / ecl / ui / replay / items / score / audio / js | 952 | 11 | — |
| 普通版 | combat、bomb | 退出码 0 | 0 | — |
| 多人版 | anm / ecl / ui / replay / items / score / audio / js / contracts | 953 | 14 | 1,683,671 |
| 多人版 | bomb | 退出码 0 | 0 | — |

两侧并行各约 4 分钟。逐套件数字与逐条失败见 [baseline.json](baseline.json)。

## 25 项失败的成因

全部来自环境输入缺席或测试自身写法，没有一项是产品代码失败。

| 成因 | 普通版 | 多人版 |
| --- | ---: | ---: |
| 缺原版字体 `msgothic.ttc`（e2e；多人版另有 M1–M9 工作流与 1 项对话气泡测试） | 1 | 3 |
| ecl 测试把事件输出写死到本机不存在的历史证据目录 `artifacts/verification/*-20261027` | 5 | 5 |
| 缺测试夹具：reimu2406 真实弹幕 ANM、`eff_switch.png` | 3 | 3 |
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
