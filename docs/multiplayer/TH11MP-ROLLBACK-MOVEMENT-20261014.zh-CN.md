# TH11MP 移动回滚 / 4× CPU 检查记录（2026-10-14）

后续实现、最终 r20 构建、测试延迟/FIFO 修正及当前未通过的 4× 结果，见同目录 `TH11MP-ROLLBACK-MOVEMENT-CONTINUATION-20261014.zh-CN.md`。本文保留上一阶段原始结论，不代表最新源码。

## 状态与入口

**性能目标尚未通过，不能据此宣称手机移动回滚已经流畅。** 本轮保留两项通过正确性检查的局部优化；没有提交、推送、合并或部署。没有修改 TH09 / TH10 工作树、common pin 或原作规则。

- 工作树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`
- 分支：`experiment/rollback-smoothness`；HEAD：`d32390730ae837fb1333bc1f1d03d746f4697936`。
- 原有大量未提交回滚实现继续保留；不要把整个 dirty diff 归为本轮修改。
- 测试 Launcher：`../launcher-th11-rollback`；common HEAD：`984a8a10834c35fe9ef86c4ffe03daf1fb61f87c`，未改。
- 最终 MP 二进制：`artifacts/rollback-smoothness/20261014-release-15-model-quad/runtime/`。
- Wasm SHA-256：`1cb551e6033166059df6a8c1bbe7f087a8d22cd4f9ad9fb91b6389b00d527220`。

本文提及的证据目录均以 `artifacts/rollback-smoothness/` 为根。字体只是本地沿用已有私有测试资源，不是可分享交付物。

## 保留的代码修改

### ANM 稳定地址分页

`AnmManager.hpp/.cpp`、`AnmRollback.cpp`：仅 MP 的物理 VM 存储改为每页 32 个对象，每页 34,432 字节。逻辑槽索引、ID、空闲槽 LIFO 顺序及 active map 的原有值语义不变。

每帧覆盖所有含有活跃 VM 的页，其余页在首次复用/创建之前完整保存。一页内后续写入不再反复进入 journal / JS bulk-copy。物理页跨 Undo 保留，逻辑存储前沿仍由 journal 恢复。页中闲置字节也会还原，不截断 VM 结构，不把 STL、引用计数或 allocator 元数据作字节快照。

嵌入 Player/Bullet/Stage 等对象的 VM 仍由各自 owner 保存。ANM 的 geometry、共享脚本写入、生命周期 leases 保持独立覆盖。

`anm-rollback.cpp` 新增跨 32/64 边界的空闲页首写、反复复用、新页分配、Undo 后原地址重建与完整字节比较。

### 独立模型四边形 VBO

`portable/sdl/Renderer.hpp/.cpp`、`MultiplayerRollback.inc`：回滚启用时，将符合 WorldUv/20-byte stride/4 顶点 strip 的 80 字节模型几何保留在独立 VBO。

只有完整 80 字节相同才能省略上传；不同内容仍替换 GPU backing。Screen 顶点和 instance 数据继续走原流式通路，不污染模型 VBO。VAO 区分流式/驻留及 singleton/instanced，但所有模型共用一个驻留 VBO，不按 draw 序号创建一批缓冲。不会省略 Update、authored Draw、GPU draw 或任何子弹。

缓存属于 Host 渲染派生状态，普通版默认关闭。回滚后仍以完整几何字节判定命中，不把 VM 身份、帧号或指针当作几何等价证明。

`texture-instance-gpu.cpp/.mjs` 保留原 17 个比较场景，新增 3 个跨 Draw、布局切换、几何变化及大量共享四边形场景，检查像素和精确上传计数。

### 独立流畅度评估

新增 `portable/multiplayer/movement-performance-gate.mjs`。它读取原始 receipt，生成新的派生评估文件，不覆盖或修改旧结果。自测包含“帧间隔整齐但只有 30 Hz”不能通过、长尾、缺少对端、诊断污染及正确性/源身份失败。

当前门槛为两端各自至少 58 个新逻辑帧/秒、呈现间隔 p95 ≤ 33.4 ms、p99 ≤ 50 ms、最长停显及 native callback ≤ 100 ms，并要求原浏览器门、确认字段、源稳定性及无错误通过。历史重演帧不计入新逻辑推进。此门不替代完整世界或逐像素 oracle。

## 舍弃的尝试

曾实现按 Draw 序号的 64 槽通用顶点/instance 驻留缓存，保留在 release 13 / 14 的不可变试验产物和 GPU 证据中。其像素正确，但跨运行性能不稳定，采样中额外 VAO 切换成为明显成本，不能凭一次约 16 Hz 的结果宣布成功。

**当前源码已经撤掉这套通用缓存**，改为上述一个 80 字节模型 VBO。不要恢复 64 槽版本或将其一次较高平均值作为最终成绩。

## 正确性证据

| 目录 | 结果与范围 |
| --- | --- |
| `20261014-anm-pages-01` | Ordinary：1 组 / 20 检查；MP：15 组 / 3,691 检查，全部通过。真实 WASI32 / O2 VM 布局。 |
| `20261014-world-pages-01` | World：8 场景 / 150,373 检查 / 1,124 次真实 Update+Draw，通过；峰值 journal 722,699 字节 / 422 块。Native：8,834 检查 / 16,979 logic/draw ticks，通过。 |
| `20261014-model-quad-gpu-01` | 20 个真实 GPU 场景，参考/候选逐字节像素一致，包含新上传计数断言。 |
| `20261014-release-15-model-quad` | MP 正式 O2、DEV=0、4 个构建 worker 通过；已冻结并本地打包。 |
| `20261014-release-15-ordinary` | 普通版正式 O2 / DEV=0 编译通过；没有替换普通版部署。 |

World 覆盖真实零售 Update/Draw、2P/3P、六种 Bomb、幽灵救援、Pause/关卡资源边界及容量失败。它不是浏览器完整世界 dump；GPU fixture 也不等于全关卡逐像素比较。浏览器收尾 56-word 字段一致只证明其实际导出范围。

## 最新串行性能对照

条件：两个独立桌面 Edge 进程、真实 WebRTC direct 通道、双方 4× CDP CPU throttle、持续 120 Hz managed touch 调度及射击、D=0、P=12、20 秒测量。没有开启 native/audio clocks 或 CPU sampling；没有真实手机设备测试。

| 产物 / 注入条件 | P1 / P2 新逻辑 Hz | P1 / P2 最长停显 ms | 结论 |
| --- | --- | --- | --- |
| 原 release 12，单向 35±5 ms | 4.01 / 4.14 | 1042.2 / 1090.2 | 失败 |
| 最终 release 15，单向 35±5 ms | 7.60 / 7.53 | 853.7 / 719.8 | 失败，仍远不流畅 |
| 最终 release 15，不注入额外延迟 | 16.99 / 17.02 | 159.6 / 173.7 | 失败 |
| 最终 release 15，单向 70±5 ms | 4.61 / 4.49 | 1075.9 / 1182.8 | 失败 |

对应 receipt 目录：

1. `20261014-control-repeat-12-4x-rtt70`
2. `20261014-final-15-4x-rtt70`
3. `20261014-final-15-4x-zero`
4. `20261014-final-15-4x-oneway70`

派生判定：`20261014-movement-assessment.json`，四组均未通过。不要选择更早某次高均值，替换这一轮完整对照。

上述四组均完成收尾：各自源稳定性检查通过、测量错误列表为空、收尾确认字段一致，两端测试浏览器均已关闭。它们失败的是性能门，不是本轮出现了新的已观测状态分叉；这也不能外推为所有对局都不会分叉。

**延迟限制：35 ms 是请求的单向附加延迟，不是已验证的 70 ms RTT。** 现有 direct 注入器在发送端主线程定时发送，会受限速和长回调拖延。最终 35 ms 场景的实际发送等待中位数为约 148.6 / 146.0 ms。不能称它为稳定 70 ms 真实网络测试。70 ms 单向压力场景也不是 70 ms RTT。

零注入且没有既有有序队列等待的分支会立即调用原始 send；有序发送仍保留原顺序约束，不能把零注入场景笼统解释为人为多排一次 setTimeout(0)。该控制仍仅约 17 Hz，说明只消除额外 35 ms 注入不足以恢复流畅，基础帧成本和预测重演成本也尚未解决。

## 定位与后续边界

采样证据：`20261014-movement-cpu-4x-rtt70`（release 12）、`20261014-pages-cpu-profile-4x-rtt70`（release 14），各自有严格匹配 Wasm/symbol 的 `cpu-profile-summary.json`。它们带诊断开销，不能当流畅度验收。

主要剩余成本仍在真实绘制/上传、journal 复制及音频混音/重采样。单端 4×、对端 1× 的 release 14 检查还观察到慢端自身没有重演、快端大量追补的情况，不能只按某一端 rollback 次数判断问题消失。

下一步应先建立不把主线程定时等待混成网络 RTT 的基准，同时保留现有严苛压力记录；再审计历史 GPU 提交与完整前向 Draw 的成本。TH09/TH10 的“保留语义 Draw，只省已证明不影响结果的 GPU 工作”值得迁移，但 **TH11 的 A/B/Screen、像素读取、几何与缓存副作用必须逐项证明**。不得直接跳过历史 Draw 或把部分纠正状态交给呈现。

不要用加输入延迟、关回滚、减少弹幕、降低逻辑速度或放宽状态比较来使测试变绿。没有完成新的性能门和真实设备验证前，本轮只能作为局部优化与可复现失败证据，不能上线标为“已解决”。
