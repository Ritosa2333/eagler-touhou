# TH17 换面长帧对照 · 2026-11-07

普通版和多人版共用两项资源加载优化。LZSS 从已解出的数据回拷，完成后按原布局写回环形字典。ANM 直接扫描贴图原始 alpha 位，只查每行透明边界，达到保留完整贴图的面积阈值后提前结束扫描。

| Wasm 基准 | 基线中位数 | 优化中位数 |
| --- | ---: | ---: |
| 读取原版档案的 256 个资源 | 645.87 ms | 464.70 ms |
| 打开原版档案的 54 个动画资源 | 70.12 ms | 32.85 ms |

256 个资源解压后共 280,214,842 字节，逐资源 SHA-256 一致。3,072 组连续解码对照覆盖字典回绕、重叠复制、输入截断和容量不足，逐次核对输出、返回状态和完整字典。54 个原版动画资源及覆盖 7 种像素格式的 42 个合成资源，透明边界逐字节一致。计时交替运行两种构建，舍弃头两轮后取七轮中位数。[基准结果](archive-bench.json)

## 普通关三人房

基线使用主站发布的 `1.0.14-eagler-315c4fdf2344`，优化包使用 `1.0.14-eagler-0bd1062cb7fa`。两个运行包依次在 Intel 核显的三个独立浏览器页面开房，分辨率 1280×960，普通难度，启用 OGG，向第二席注入 145 ms 输入延迟，对渲染进程施加 4 倍 CPU 限速。两轮分别核对 9,020 和 9,018 个三端一致的确认帧，覆盖一面道中、Boss 战和二面开场。

| 席位 | 换面回调基线 → 优化 | 换面呈现间隔基线 → 优化 | 全程 ≥50 ms 的间隔基线 → 优化 |
| --- | ---: | ---: | ---: |
| P1 | 208.68 → 176.73 ms | 220.56 → 189.94 ms | 25 → 27 |
| P2 | 214.36 → 171.22 ms | 225.83 → 181.46 ms | 31 → 34 |
| P3 | 208.41 → 176.50 ms | 220.68 → 190.42 ms | 22 → 23 |

换面取首次确认进入二面的帧。报告在导出确认帧前结束呈现计时。全程呈现间隔 P95 两轮都约为 20 ms。优化版道中第 8,183 帧仍出现了 227.23 ms 的呈现间隔，当帧游戏回调为 24.47 ms，回调内补算帧数为零。该间隔的剩余耗时待分段采样归因。[基线房间报告](baseline-room-report.json) · [优化房间报告](updated-room-report.json)

普通版 `1.0.14-eagler-0e4435ae1eb6` 通过浏览器适配器测试。多人原生测试通过，并核对 967 项原版射击配置及妖梦的各蓄力模式。

## 主站发布

[PR #199](https://github.com/Ritosa2333/eagler-touhou/pull/199) 已接收 `610a07110`。主站于 2026-11-07 13:07 发布本次普通版和多人版，公网核对两套运行包共 30 个文件的哈希和 Wasm 类型。[发布回执](activation.json) · [公网校验](public-verification.json)

## 复测

三人房环境变量参照[普通关函数采样](../th17-release-profile-20261107/README.md)，增加 `TH17_ROOM_RENDER_SCALE=2`，依次选择两个运行包。

```powershell
node ports/th17-mp/web/tests/multiplayer/room-browser.mjs --launcher-preview --check-presentation --independent-pages --through-stage-two
```

资源基准使用 [C++ 入口](archive-bench.cpp) 和 [Node 对照脚本](archive-bench.mjs)。在临时目录编译两份 Wasm，基线的 `Lzss.cpp` 和 `AnmResource.cpp` 取自 `9e1e9d70b`，优化版取自 `3aa413997`。两份均链接 `Archive.cpp` 和 `ResourceCrypt.cpp`，包含 `ports/th17/web/cpp/game` 头文件目录，并使用同一 Emscripten SDK。

```text
-O3 -flto -std=c++17 --no-entry -sDEFAULT_TO_CXX=1 -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=node -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_FUNCTIONS=_malloc,_free -sEXPORTED_RUNTIME_METHODS=HEAPU8
```

把生成的 `baseline-codec.mjs`、`updated-codec.mjs` 及各自的 Wasm 放在脚本旁，运行 `node archive-bench.mjs <th17.dat>`。
