# th12 性能测量 | 2026-10-24

PR #111 的 th12 展示管线将顶点提交、姿态整理和音效节点作为优化重点。在真实资源与音效参与的 Stage 6 固定场景中，浏览器 CPU 降速 4× 后呈现 59.75 FPS，游戏逻辑保持 60.00 Hz。本文保存这轮测量的输入、结果和画面一致性证据。

## 测量输入

| 项目 | 配置 |
| --- | --- |
| 参考代码 | PR #111 的 `6ee07670e3ea35e6a3d055a53cf4ecb06f8d205d` |
| 测试资源 | `test.touhou.vip` 包修订 `34ab253b6786668c`；`games/th12/th12.data` SHA256 `bedf5456e7aa7a1db673dfca410aa97f35fbefefcd08997b3d64fa62e736fcc4` |
| 浏览器 | Windows / Chrome 156.0.8078.47 / Intel UHD / ANGLE D3D11 / 约 165 Hz |
| 游戏核心 | Emscripten 6.0.6-git / `-O2` / Web + THPrac；`core.wasm` SHA256 `c43970ba4b78ca94a6b109d519260a04bc7d6c91145569fc47d42a4370e0ea89` |
| 压力条件 | Chrome DevTools CPU 降速 4×、60 Hz 呈现限制、真实 WebAudio 音效、固定随机种子 `0x1234` |

## JS 绘制提交

Windows / Node.js 24.13.0 记录式 WebGL 边界基准，参考代码与本轮代码交替运行五轮。每轮预热 60 帧、测量 120 帧，下表取各轮帧耗时中位数的中位数。

| 固定负载 | 参考提交 | 本轮代码 | 耗时下降 |
| --- | ---: | ---: | ---: |
| 4096 个精灵，每 64 个换图 | 1.897 ms | 1.168 ms | 38.5% |
| 2048 个精灵，每个换图 | 2.862 ms | 0.743 ms | 74.0% |
| 8192 个精灵，单批提交 | 6.613 ms | 2.131 ms | 67.8% |

三组负载的绘制次数和上传字节数一致。预热后本轮代码的新增上传缓冲数及重复贴图参数调用均为 0。这组实验衡量 JS 提交层；关卡测量覆盖游戏逻辑、资源、音效与浏览器呈现。

## 关卡压力

两组场景均使用早苗 B、固定输入、无敌与无限 Bomb 测试开关，以保持弹幕负载。测量从第 120 逻辑帧后开始。

| 场景职责 | 逻辑帧 | 弹幕峰值 | 呈现帧率 | 逻辑频率 | 帧间隔 P95 | 处理耗时 P95 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Stage 6 Section 45 / Easy / 60 Hz 验收 | 1800 | 799 | 59.75 FPS | 60.00 Hz | 18.3 ms | 15.2 ms |
| Extra Section 69 / 300、900 帧 Bomb / 峰值特效画像 | 1200 | 783 | 58.88 FPS | 60.00 Hz | 18.3 ms | 17.6 ms |

Stage 6 按预设规则通过 60 Hz 验收：呈现与逻辑频率均达到 59.5 Hz，处理耗时 P95 低于 16.67 ms，帧间隔 P95 低于该显示器的 19.1 ms 刷新量化预算。Extra 样本用于观察连续 Bomb 与密集特效下的负载分布。

## 一致性

- Chrome / Intel UHD / D3D11 的四组浏览器负载覆盖三种精灵密度，以及背景截图、暂停合成、混合、寻址与线带；绘制数据和 640×480 像素逐字节一致。
- 单声道与双声道音效在居中、左偏、右偏共六种配置下，离线 PCM 输出逐样本一致。居中路径的节点数从 6 降至 2。
- Stage 6 的 60 次抽样与 Extra 的 40 次抽样，每 30 逻辑帧对比参考实现的完整核心状态、绘制次数、像素及上一展示端点独立性，均一致。
- `npm run test:renderer` 的 61 项测试、`node --test tools/ports/frame-pacing.test.mjs` 的 4 项测试与文档检查通过。

在 `ports/th12` 目录运行渲染基准和浏览器对照：

```sh
npm run test:renderer
npm run benchmark:renderer
npm run check:renderer-browser -- 6ee07670
```

## 实现与复测

绘制端使用跨帧复用的 Float32 顶点缓冲，按有效长度向 WebGL 2 上传；贴图过滤与寻址参数按贴图缓存。展示端点独立保存姿态排序信息，按绘制层收集，仅对乱序层排序。精灵与网格在顶点提交时应用震动，居中音效使用双声道增益直通。60 帧呈现门限保留显示刷新余量，追赶逻辑帧时按插值需要选择展示端点。

浏览器检查使用仓库共用的 Chromium 启动入口。`PLAYWRIGHT_CHROMIUM_EXECUTABLE` 可指定浏览器，`NATIVE_GPU=1` 可启用硬件渲染；基准命令可接收另一版 `renderer.mjs` 路径，浏览器检查以上方命令中的 Git 提交为参考版本。浏览器检查还对比单声道与双声道音效在居中、左偏、右偏时的 PCM 输出。

关卡压力工具为 `tools/ports/th12-byakuren-profile.mjs`。本轮使用 `TH12_PROFILE_PACED=1`、`TH12_PROFILE_CPU_RATE=4`、`TH12_PROFILE_AUDIO=1`、`TH12_PROFILE_LOCKED=1`；Stage 6 使用默认场景，Extra 另设 `TH12_PROFILE_STAGE=7`、`TH12_PROFILE_SECTION=69`、`TH12_PROFILE_BOMB=1` 和 `TH12_PROFILE_FRAMES=1200`。`TH12_PROFILE_ASSETS` 可指定资源目录，默认读取 `site/assets`；`TH12_PROFILE_RUNTIME` 可指定包含 `core.mjs`、`core.wasm` 和 `build.json` 的核心目录。报告和截图输出到 `artifacts/th12-byakuren/`。

`TH12_PROFILE_PACED=1` 按浏览器刷新回调推进固定 60 Hz 逻辑，同时采集呈现帧率、逻辑频率、帧间隔和处理耗时；弹幕数随绘制记录采集。`TH12_PROFILE_AUDIO=1` 启用真实 WebAudio，`TH12_PROFILE_LOCKED=1` 启用与游戏入口一致的 60 帧门限。`TH12_PROFILE_REQUIRE_60=1` 按呈现与逻辑频率均达到 59.5 Hz、处理耗时 P95 不超过 16.67 ms、帧间隔 P95 不超过实测显示器的刷新量化预算验收，未达标时返回非零状态。刷新量化预算为覆盖 16.17 ms 所需的整数个刷新周期加 0.8 ms 调度余量；60 Hz 约为 17.47 ms，165 Hz 约为 18.98 ms。报告记录浏览器、GPU、编译器及核心哈希。

`tools/ports/th12-render-equivalence.mjs` 在相同关卡输入下每 30 帧对比核心状态、绘制次数、像素和上一展示端点独立性。`TH12_PROFILE_REFERENCE_RUNTIME` 指定参考核心，命令参数指定参考代码提交。帧率统计规则由 `node --test tools/ports/frame-pacing.test.mjs` 验证。
