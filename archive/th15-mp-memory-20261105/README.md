# TH15 多人回滚快照内存

多人长局会经过多次动画与消弹峰值。回滚窗口原先为每个帧槽保留完整 VM 高水位存储；确认帧释放其他战斗对象后，动画副本继续占用内存。本轮让确认帧释放动画负载，并让相邻帧共享未变化的平面 VM。发生变化时保留各帧自己的副本，带动态几何的 VM 继续独立深拷贝。

基于 `YomotsuHisami/eagler-touhou` 的 `480ef2906ccc4bec00dc121a53cc4b8f6d465a6f`。改动范围为 `ports/th15-mp` 的逐帧回滚；普通版使用自己的检查点路径。WASM 最大内存保持 1073741824 字节。

## 内存与还原

4096 个未变化的 owning root、13 份快照，在相同 wasm32 分配器下的新增已分配内存由 82467736 降至 9037400 字节，减少 89.0%。每份快照都逐根还原并核对数值。[夹具](snapshot-memory.cpp)、[旧版](baseline-snapshot-memory.json)、[新版](fixed-snapshot-memory.json)。

Chromium 使用与主站相同的零售资源归档，SHA-256 为 `f3802e71db30f99b86d61b6e98b8325716c5cbae892547c563dded7837862ae7`。四至六面各测双人、三人，每组 240 帧预热后推进 6000 帧，远端输入延迟五帧，并注入一次密集弹幕。此组用于比较各关卡的快照驻留内存；输入一致，延迟确认不触发错误预测重演。

| 人数 | 关卡 | 旧版已分配峰值 MiB | 新版已分配峰值 MiB | 旧版线性内存峰值 MiB | 新版线性内存峰值 MiB |
| --- | --- | --- | --- | --- | --- |
| 2 | 4 | 750.3 | 537.8 | 839.0 | 646.8 |
| 2 | 5 | 580.1 | 518.9 | 646.8 | 550.7 |
| 2 | 6 | 625.7 | 542.3 | 646.8 | 646.8 |
| 3 | 4 | 748.8 | 540.6 | 838.9 | 646.8 |
| 3 | 5 | 588.0 | 522.0 | 646.8 | 550.7 |
| 3 | 6 | 624.4 | 541.7 | 647.0 | 646.8 |

六组端点的战斗指纹逐项一致。[旧版采样](baseline-focused.json)、[新版采样](fixed-focused.json)包含构建、源文件、浏览器与驱动身份。已分配内存通过 `mallinfo().uordblks` 采样，线性内存通过 `emscripten_get_heap_size()` 采样；WASM 增长后的线性内存不会收缩。

## 回滚正确性

wasm32 动画图、帧回调、敌机、激光与战斗检查通过，战斗夹具完成 432 次真实资源逐帧重演。新增动画回归覆盖 13 帧共享、数值变化、动态几何、历史不可变性与确认后释放，512 个未变化的根在其余 12 帧的新增分配为 734448 字节。[动画](animation-rollback.txt)、[回调](frame-rollback.txt)、[敌机](enemy-rollback.txt)、[激光](laser-rollback.txt)、[战斗](battle-rollback.txt)。

实际应用纠正循环分别运行双人一面和三人 Extra 密集场景，每组 600 次五帧错误预测回退；世界指纹、确认 Replay 输入与 canvas 均与完整输入执行相同。[纠正对照](corrections.json)。

正式构建使用 Emscripten 6.0.9，WASM SHA-256 为 `4c91e6f385e7f3368fa16f08fd75047b03f9028403bbbe5cff4957551ffaeb71`。打包核对源码及头文件哈希，并检查生产包的诊断导出隔离。[构建身份](release-build.json)。

## 复现

内存驱动为 [`tools/ports/th15-multiplayer-memory.mjs`](../../tools/ports/th15-multiplayer-memory.mjs)。以相同零售资源、共享字体和 Runtime shell 分别运行旧、新开发构建：

```sh
TH15_EMSDK=/path/to/emsdk node ports/th15-mp/web/scripts/build-sdl-application.mjs --multiplayer --profile
TH15_PROFILE_BUILD=/path/to/profile-build \
TH15_TEST_RUNTIME=/path/to/runtime-shell \
TH15_TEST_ARCHIVE=/path/to/th15.dat \
TH15_TEST_FONT=/path/to/msgothic.ttc \
TH15_TEST_UNICODE_FONT=/path/to/unifont.otf \
TH15_MEMORY_OUTPUT=/path/to/results \
node tools/ports/th15-multiplayer-memory.mjs
```

驱动从指定关卡运行实际应用与回滚窗口。`TH15_MEMORY_FRAMES` 选择推进次数；`TH15_MEMORY_CASES` 使用 JSON 数组配置 `players`、`stage`、`depth`。`TH15_MEMORY_CONTINUOUS=1` 从一面运行连续关卡，对全席位施加开发夹具的存活保护，并记录实际关卡、帧号和完成状态。
