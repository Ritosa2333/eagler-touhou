# DOSBox-PC98 核心优化与五部作品迁移指南

记录日期：2026-10-15。当前已验证对象：TH04 合作版，以及 USB 连接的 Samsung SM-T500 平板。本文用于后续 TH01、TH02、TH03、TH05 的迁移，区分通用核心优化与游戏专用优化。

**已将本次验证过的核心接入正式 web/ 发行目录，随仓库上传即可运行；尚未达到这台平板稳定流畅的目标。其他四部尚未验证，不能据此保证相同收益。**

## 1. 当前版本与实测基线

工作目录：`D:\Documents\try\touhou_old\th04-coop-lab`。

- 仓库发行基线：`1a55f37e5d5e527fb159743b1e0055ef5a8aff46`。
- 最终冻结候选：`build/native-scheduler/runtime-tablet/`。后续实验不要覆盖这份目录。
- 正式 `web/` 已接入本次冻结核心及匹配包装器，并更新资源摘要；普通 `start-lan.bat` / 9868 入口直接使用优化版。
- 优化候选服务：`tools/native_scheduler_server.py`，通过 `NATIVE_RUNTIME` 选择核心。
- 启动入口：`tools/start_tablet_candidate.bat`，端口 19872。电脑和平板都须进入候选服务，不能混用两种资源身份。
- 当次局域网地址为 `192.168.1.102`；重启网络后应重新查看电脑 IP。

设备：Samsung SM-T500 / Snapdragon 662（SM6115），Android 12，约 2.64 GiB 可见物理内存；ARM64 Chromium 157.0.8102.0。不要与之前测试的 Redmi K70 混淆。

### 核心筛选成绩

双人、30000 cycles、关闭回滚、第一关、合成已确认输入，排除网络等待；各轮预热后测量约15秒，之后另做3秒热点采样。

| 路径 | 游戏帧/秒 | 同步步/秒 | step + confirm + present 平均耗时 |
|---|---:|---:|---:|
| 原发行核心 | 8.04 | 8.54 | 111.91 ms |
| 等价忙等合并 | 21.17 | 22.51 | 39.38 ms |
| 忙等合并 + 同步解释器辅助函数 | 31.05 | 33.04 | 25.13 ms |
| 上述版本 + 分块 memcmp | 33.04 | 35.10 | 23.42 ms |
| 上述版本 + PC-98 显存查表 | 35.78 | 38.05 | 21.40 ms |

这些是同配置的短时筛选，并非严格同检查点 AB：较快候选在测量期间推进了更多游戏帧，场景覆盖并不完全相同。微小差异不能直接视作确定收益。

### 实际局域网成绩

电脑和平板使用同一候选，两端持续射击，平板交替移动，第一关约22秒；通过局域网 WebRTC 直连，USB 仅用于调试。

| 设置 | 电脑游戏帧/秒 | 平板游戏帧/秒 | 平板 step 平均耗时 |
|---|---:|---:|---:|
| 关闭回滚 | 35.46 | 35.44 | 21.59 ms |
| 开启回滚 | 27.43 | 27.36 | 29.29 ms |

两端共同确认帧的状态校验一致。开启回滚时，平板没有实际回滚重算，但仍承担追踪内存写入的成本；电脑发生135次回滚、608步重算。音频欠载仍存在，所以不能将35帧称为已流畅达标。

同步步目标为60 Hz，TH04 原生刷新约56.4 Hz；浏览器绘制次数、同步步数、实际游戏帧推进是三个不同指标。其他作品应先测自己的原生时序。

## 2. 为什么从核心优化，而不是继续降游戏负载

当前单机与联机使用不同执行路径：单机使用 JSPI 和自动 cycles；联机使用确定性虚拟时间、固定30000 cycles以及完整 Asyncify，回滚开启时还记录内存写入。关闭回滚不意味着自动改用单机核心。

联机核心即使没有网络等待也只能达到约8帧，说明网络不是唯一瓶颈。最慢端每秒推进不足时，锁步房间整体受到约束；增加预测窗口不能增加慢端每秒完成的计算量。

优化保持以下条件：

- 不减少弹幕、碰撞检测、角色逻辑、声音内容。
- 不通过降低模拟周期或跳过游戏帧制造表面帧率提升。
- 不提前触发 VSync、中断或输入采样边界。
- 不用宿主真实时钟影响模拟结果。
- 保留完整挂起续栈、内存、文件系统和回滚语义。

30000 cycles 是本项目对照条件，不是五部作品通用的最佳配置；迁移时先固定每部原有正确配置，测清楚后再讨论配置调整。

## 3. 有效方法及迁移边界

| 方法 | 通用程度 | 其他作品必须重新做什么 |
|---|---|---|
| 等价合并游戏忙等循环 | 思路通用，当前实现 TH04 专用 | 找到实际热点循环，重新证明指令预算、寄存器、flags、访存和事件边界等价 |
| 确定性原生帧内调度 | 固定核心和包装器内可复用 | 验证启动、暂停、时钟、输入边界和恢复重放 |
| 有保护条件的同步解释器及辅助函数 | 固定 DOSBox 构建内可复用 | 核查游戏的 CPU 模式、内存映射、设备处理器及回退覆盖 |
| 分块 memcmp | 算法通用，二进制补丁依赖构建身份 | 验证函数签名、精确返回值、未对齐与内存边界 |
| PC-98 平面显存查表转换 | 对相同显示路径可复用 | 验证各作品的显示模式、调色板、滚动、文字层与画面切换 |
| 回滚写入记录及快照 | 机制可复用，不能直接照搬游戏适配器 | 覆盖客体写入、JS 输入写入、内存增长和全部恢复状态 |

### 3.1 TH04 忙等循环的等价合并

历史客体指令采样发现，大量解释执行消耗在等待 VSync 的三指令循环。对应当前 TH04 映像：

```asm
; 客体线性地址 0x1BA75，即十进制 113269
MOV AX,[2AB2h]
CMP AX,[5390h]
JB  short -9
```

这类循环在当前事件预算内重复读取不变的 RAM、比较并跳转，可以合并若干完整迭代，按原本指令数扣除预算，最后仍正常执行完整迭代以重建寄存器、惰性标志及解释器临时状态。合并不越过原来的调度边界，也不直接跳到下一次中断。

保护条件包括：精确地址及代码字节、实模式/16位路径、非 VM86/单步、直接映射内存、仍处于等待状态。条件不满足立即走普通解释器。

实现：`tools/build_idle_loop_candidate.mjs`；源码版本在 `tools/patch_native_scheduler.py` 的 `TH_IdleTriplets()`。历史采样约77.35%是该循环的指令数量占比，不代表耗时占比。

**不能把地址113269、DS偏移2AB2/5390或 WASM 内部绝对地址直接复制到其他四部。** 即使换语言、补丁版本或链接布局，也要重新核对。现有源码候选仍包含这段 TH04 专用识别；建立通用五部核心时，应将其隔离为按映像版本选择的优化，未识别版本默认禁用。

### 3.2 原生帧内调度

原路径每同步步会多次在 JS 与 WASM 之间挂起/恢复。候选把虚拟时钟、截止时间和模式放入可恢复的原生内存；若下一次唤醒仍在该步截止时间内，内部推进到原唤醒时间继续运行，跨边界才挂起。

必须保留原有1ms最小等待规则、完整 BIOS 递归续栈、输入截止边界，并让所有相关时间源使用相同虚拟时钟。包装器写入时钟字段时必须覆盖回滚脏页。

实现：`tools/native-scheduler-clock.cpp`、`.h`、`tools/patch_native_scheduler.py`、`tools/prepare_native_scheduler_runtime.cjs`。

注意：此方法只是减少边界切换，历史测量单独收益很小。它不等于删除 Asyncify，也不等于已实现完全无挂起的模拟器主循环。

### 3.3 同步解释器和同步辅助函数

保留完整解释器作为回退，同时建立经过保护条件检查的同步路径，避免普通指令承担完整 Asyncify 插桩成本。

固定构建的 `core=simple` 配置实际落到 Normal 解释器；不要仅按 UI 或配置别名判断执行了哪套 CPU。

关键实现：

- `tools/native-cpu-preflight.h`：CPU模式与指令预检。
- `tools/native-cpu-helpers.h`：同步路径专用的取指、读写、栈等辅助函数。
- `tools/native-cpu-memory.h` 及 `patch_native_scheduler.py`：映射变化后的处理器身份认证。
- `patch_native_scheduler.py`：同步 decoder、完整回退、16位 EA 表、REP 辅助函数；CPU翻译单元 O3。
- `instrument_native_scheduler.ps1`：仅对已审查同步函数排除 Asyncify 插桩。

允许的 RAM/ROM/PC-98 显存与字体等处理器必须经审查；未知映射、特殊指令及可能挂起的 I/O 等交回完整路径。不能只因函数名称看似无害就加入 Asyncify 排除列表。

回退保持剩余 cycles、惰性 flags、指令执行次序，不插入额外 PIC 调度。跨页内存、栈回绕、REP 与显存副作用必须保留。同步栈运行时若发生挂起，应触发断言，而不是继续执行损坏的续栈。

不是把所有 CPU/内存函数都改成不挂起。其他作品可能使用 TH04 没触及的路径，必须验证回退可用，不能为追求速度直接删掉它们。

### 3.4 用8字节相等比较加速 memcmp

当前工具链的体积优先构建使 `memcmp` 走逐字节比较，它在平板热点中占明显比例，尤其涉及画面缓存比较。

新实现：仅在剩余长度至少8时读取两侧8字节；相等则前进8字节，否则回到逐字节比较，返回首个不同字节的精确差值。不足8字节也逐字节处理。

不能用64位数值大小判断原始字节串顺序；只用它判断相等。不得向范围外多读，也不得假设指针对齐。保留原始函数导出用于测试。

实现：`tools/build_memcmp_candidate.mjs`。验证：`tools/test_native_memcmp.cjs`，72,774组精确返回值对照，包含未对齐、相等、首/中/尾差异和内存末端。

### 3.5 PC-98 四平面显存查表展开

原 `VGA_PC98_Xlat32_Draw_Line()` 对每8像素逐位取4个平面的最高位、移位、组合色号，然后查询调色板。

新实现用256项常量表将一个字节的8位展开成8个 nibble 的最低位；四个平面的结果分别左移0、1、2、3位并按位或，得到8个4-bit色号，再查询**当前** `vga.dac.xlat32`。

保留当前扫描行/扫描地址、显示页、8/16色选择、文字层覆盖、调色板实时变化和完整帧提交时机。没有跨帧缓存最终颜色，也没有跳过文字和栅格处理。

实现：`tools/patch_pc98_planar.py`。保留 `thPlanarState()` 开关，能从同一快照分别走原路径和新路径，逐帧比较 RGB、GPU读回、游戏状态与音频。

其他作品必须覆盖自己的渐变、闪屏、切页、滚动、菜单/文字及特殊显示模式。优化只作用于匹配的平面路径；256色等其他分支仍保留原实现。

## 4. 没有效果或被否决的尝试

| 尝试 | 结果与处理 |
|---|---|
| 只减少 JS/WASM 唤醒次数 | 单独收益很小，不作为弱设备问题已解决的证据 |
| 每条指令详细检查所有操作数 | 检查成本抵消收益；改用映射变化时认证及必要指令预检 |
| 同步解释器只保留16位操作数 | 函数约222KB缩至114KB，正确性通过，但平板31.05→30.68帧；已撤回 |
| 四个热点函数额外 O3 后优化 | 35.78→34.78帧，未见收益；不采用 |
| 直接引用原生 RGBA surface | surface会被后续栅格改写，直接保留指针不正确 |
| 给 RGBA surface 增加整帧复制 | 历史对照正确，但复制/上传/回滚成本抵消收益；最终关闭此路径 |
| 降低 cycles | 历史实验降低原游戏推进能力；不纳入本方案 |
| 省略 Binaryen O1 后直接完整 Asyncify | 本次内存超过6GB，宿主可用内存不足1GB，主动停止；恢复原分阶段流程 |

不能仅看函数体积、调用次数或编译级别决定是否采用。修改必须同时通过正确性与目标弱设备实测。

## 5. 本地构建流程与注意事项

以下是现有工作区的复现流程，不是干净机器的一键安装脚本。源码、依赖、工具脚本及测试数据必须另外保存。

固定版本：

- js-dos emulators：`6bbf0925d221a7304ea8e0dda1e89b8dc01569b2`。
- DOSBox-X：`788fde99d3d451eab3621009186e5abe516f2b9c`。
- 原 npm 包：`emulators@8.5.2`。
- Emscripten 4.0.15、Binaryen 132、CMake 3.31.6。
- 工具链目录：`build/native-scheduler/toolchain/`；Binaryen JS包：`build/binaryen-132/package/`。

在项目根目录执行，各步成功后才执行下一步：

```powershell
# 1. 组装固定源码，并应用核心/显存改动
python tools/prepare_native_scheduler.py
python tools/patch_native_scheduler.py
python tools/patch_pc98_planar.py

# 2. 单编译任务生成原始核心；再做完整 Asyncify 与包装器适配
python tools/build_native_scheduler.py --native --raw
./tools/instrument_native_scheduler.ps1

# 3. 原生挂起核心中替换 memcmp，并规范化 WASM
node tools/build_memcmp_candidate.mjs build/native-scheduler/runtime/wdosbox-x-rollback.wasm build/native-scheduler/planar-memcmp-native.wasm
node tools/prepare_core_candidate.cjs build/native-scheduler/planar-memcmp-native.wasm build/native-scheduler/runtime/wdosbox-x-rollback.wasm

# 4. 从最终未追踪核心生成回滚写入追踪版本
node tools/build_native_scheduler_tracked.cjs
```

注意：

1. `patch_pc98_planar.py` 从固定上游文件生成补丁；若未来另加 VGA 修改，必须合并到补丁生成流程，避免被覆盖。
2. `instrument_native_scheduler.ps1` 保留 `-O1`，Binaryen并行数为2；C++构建已改为 `-j1`。测试与编译串行运行，关注宿主可用内存。
3. Binaryen输出可能带紧凑导入格式。继续交给另一个二进制处理工具前，必要时先经 `prepare_core_candidate.cjs` 规范化。不能将解析失败当作游戏或浏览器问题。
4. 不跨不同构建按函数索引硬拷贝函数；必须匹配源码版本、函数身份、内存布局、包装器及导入导出。
5. 源码中仍有此前 RGBA 实验的支持代码；最终运行必须保持 `nativeFramePresentation:false`，使用原 RGB 呈现器。
6. 正常包装器配置为 `nativeScheduling:true,nativeCpu:true`。候选服务会注入这些设置并更新资源身份；裸替换一个 WASM 文件不构成完整部署。
7. 构建完成后验证，才把5个匹配文件冻结到新目录：两个核心 WASM、两个 `.mjs` 包装器、`snapshot-diff-tracked.wasm`。不要覆盖已经验收的冻结目录。
8. 回滚写入追踪必须由最终核心生成；不要拿旧追踪核心搭配新未追踪核心作为同一版本发布。

当前候选启动：

```powershell
$env:NATIVE_RUNTIME='build/native-scheduler/runtime-tablet'
python tools/native_scheduler_server.py --bind 0.0.0.0 --port 19872
```

候选服务会为覆盖的资源生成对应摘要，防止不同版本混房。本次已用 `tools/install_tablet_runtime.py` 校验冻结文件后同步更新两个 runtime.json 与适配器摘要。后续版本仍须同步维护这些身份，不能保留旧摘要。

## 6. 验证方法：迁移时不能省略

### A. 指令级与同状态对照

- `tools/test_native_cpu_budgets.cjs`：算术、内存、跨页读写、16/32位栈回绕、REP、显存、特殊指令回退、flags和中断影子；120组小预算。
- `tools/test_native_cpu_parity.cjs`：从相同检查点分别走完整与同步解释器，比较客体内存、寄存器、CPU结构、惰性flags、scratch、段、剩余cycles、指令计数、时钟和音频。
- `tools/test_native_scheduler.cjs`：旧逐毫秒恢复与原生批调度的同状态对照。
- `tools/test_idle_instruction_parity.cjs`：忙等循环各入口相位及临界预算；历史中/日文共186组。
- `tools/test_native_memcmp.cjs`：精确返回值和内存边界。
- `tools/test_pc98_planar.cjs`：原/新显存路径同状态逐帧比较，当前中/日文各96步。

### B. 回滚与实际连接

- `tools/test_grouped_checkpoints.cjs`：六组中/日文双/三人、完整复制回退及暂停；包含完整堆、文件系统、重放和音频行为。
- 当前候选的检查点报告：`reports/tablet-core-checkpoints-20261015.json`。
- `tools/measure_usb_phone_room.cjs`：真实电脑与 USB 调试设备通过局域网联机；支持 BASE、DEVICE、DEVICE_TAG、REVISION、SECONDS、OUTPUT 参数。名字包含 phone，不代表只用于手机。
- 房间验收不只看FPS：必须检查共同确认帧哈希、实际回滚次数/重算步数、断线/暂停、画面和音频。

### C. 弱设备性能

- `tools/tablet_core_probe.cjs`：当前平板 CDP 端口9224，独立前台测试页；模式 `baseline`、`idle` 或 `native*`，后者可通过 `NATIVE_RUNTIME` 指向候选。
- 固定设备、浏览器、游戏设置、cycles、测试输入、前台状态；只跑一个核心筛选任务，不用桌面CPU限速代替真机结论。
- 分别统计 step、capture、restore、confirm、present；区分客体游戏掉速、宿主计算不足和网络等待。
- 先短测筛选，再做同检查点AB、密集场景、长时间运行与实际房间。较快版本可能进入更重场景，不能只比较不同游戏阶段的平均数。
- 检查音频欠载与帧间隔P95/P99。平均35帧不等于稳定35帧，更不等于流畅。

这些脚本目前依赖 TH04 的 `th04Sync`、邮箱、磁盘和启动配置，**不能直接拿去给其他作品验收**。应复用验证框架，替换每部作品的启动、帧计数、状态提取和输入适配。

## 7. 其他四部的实施顺序

1. 为每部记录原始游戏/补丁/核心哈希，建立独立基线与目录，不覆盖 TH04 已验证候选。
2. 先用正确的原版配置完成启动、单机与联机测量，确认实际 CPU、显示和音频路径。
3. 从公共核心层迁移同步解释器、调度、memcmp和平面转换；先禁用 TH04 专属忙等识别。
4. 每项单独开启，从同状态对比正确性及弱设备耗时，不一次性叠加无法归因的改动。
5. 用该作品的客体指令采样找真正热点；只有发现适合合并的循环后，才加入按游戏/版本识别的专用优化，并写预算边界测试。
6. 接入该作品的确定性输入、帧计数和状态校验，验证完整恢复，不只比较屏幕。
7. 跑菜单、关卡切换、暂停、密集场景、调色板/显示变化、双/三人或实际支持的模式，再进行真实弱设备房间测试。
8. 冻结通过的核心和包装器、更新身份摘要、保留报告，再提供普通玩家入口。

建议维护一套固定版本的公共 DOSBox 核心和测试框架，游戏差异留在各作品适配层；专属循环优化以明确的游戏版本选择。五部不必各自重复改同一套内存比较和显存转换，但必须各自通过验收。

## 8. 证据与保存范围

主要报告位于 `reports/`：

- `tablet-core-work-20261015.md`：本轮工作记录。
- `tablet-core-baseline-20261015.json`、`tablet-core-idle-20261015.json`、`tablet-core-native-20261015.json`：原版与已有候选实机结果。
- `tablet-core-native-memcmp-20261015.json`、`tablet-core-native-planar-20261015.json`：本轮有效修改。
- `tablet-core-native-cpu16-20261015.json`、`tablet-core-native-hot-20261015.json`：未采用候选。
- `tablet-memcmp-parity-20261015.json`、`tablet-planar-parity-20261015.json`、`tablet-core-checkpoints-20261015.json`：正确性证据。
- `tablet-real-room-20261015.json`：实际局域网、回滚开关两轮数据。
- `weak-core-idle-optimization-2026-10-14.md`、`native-scheduler-refactor-2026-10-14.md`：前期原理、实验与历史验证。里面的“已发布”等描述属于当时状态；当前状态以本文为准。

**仓库采用发行文件白名单：本文与本轮选定的核心构建、补丁和验证脚本已纳入待上传范围。reports/、build/、vendor-source/ 仍忽略，原始报告和工具链需单独保存。** 为后续四部保留工程时，需保存补丁脚本、固定源码/版本信息、工具链说明、测试脚本和报告，以及冻结候选的5个运行文件；只保存 WASM 或只保存这份文档都不足以完整复现。

## 9. 尚未解决的问题

SM-T500实际联机关闭回滚约35.4帧、开启约27.4帧，仍有音频欠载。剩余热点包括解释执行与图像处理；完整写入追踪进一步增加弱端成本。

目前没有证明全部关卡长期流畅，也没有证明其他四部达到同样速度。下一轮工作应继续从真实设备热点及每步预算出发，不重做已否决的“盲降cycles、仅加大内存、仅换线程、只改编译级别”等实验。

## 附录：本次冻结候选 SHA-256

目录：`build/native-scheduler/runtime-tablet/`。用于核对是否仍为本次已测试产物。

| 文件 | SHA-256 |
|---|---|
| dosbox-rollback.mjs | b86ffabafe90b43c0e12745ebd2d03e0da3353a53f765ad50a2008d12e291e8e |
| dosbox-rollback-tracked.mjs | f7224bcb7e5ea881525760f3b71b194d01e127acf31103a5400a06d9d004d048 |
| wdosbox-x-rollback.wasm | 318af02f807f494613083c444a35c1abcde2823a9ad2c81a160e785d8d7d3948 |
| wdosbox-x-rollback-tracked.wasm | 075a3dead5f49cf3a38b7d382fd0460ac7ad23935cec0f55db0d4bb4ea5c848d |
| snapshot-diff-tracked.wasm | fb69ccf0818caa9199ac3f357435528622413b690af90a353b99c88d81adb5c2 |
