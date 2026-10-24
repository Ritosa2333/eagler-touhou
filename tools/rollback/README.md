# Wasm 写入日志插桩

这个构建步骤把游戏状态区域的 Wasm 写入接入回滚日志。它覆盖整数、浮点和完整 `v128` 存储，以及 `memory.copy`、`memory.fill`、`memory.init`。写入指令的原始操作数先按原顺序求值，日志随后保存改写前的内容，最后执行原始写入。

## 构建接入

C++ 后端需要显式定义 `EAGLER_WRITE_JOURNAL=1`；CMake 消费者链接 `eagler::write_journal` 时由目标传入。未定义或值为 `0` 时，`WriteJournal.cpp` 与 `SimulationHeap.cpp` 不生成分配器、hook 或全局 `new` / `delete` 定义，兼容按目录收集公共源码的其他产品。TH11 MP、TH15 MP 和验收模块在各自构建入口启用该宏。

```sh
npm --prefix tools/rollback ci --ignore-scripts
node tools/rollback/instrument.mjs raw.wasm runtime.wasm
node tools/rollback/instrument.mjs --verify runtime.wasm
node --test tools/rollback/instrument.test.mjs
```

构建器将最终 `runtime.wasm` 交给打包步骤。如果还要运行 `wasm-opt`，在最终优化产物上再执行 `--verify`。CLI 在完整校验后原子写入输出文件，并打印包含各类写入数量的 JSON 构建记录。

插桩保留原有函数索引顺序，Emscripten 输出的函数索引 symbol map 可继续用于崩溃与性能采样。增加局部变量时，工具按原始顺序一次重建完整函数列表，并保留导出、间接调用表和 start 函数的绑定。

默认模式在 Wasm 写入位置判断模拟堆与外部 root 的范围交集。正式游戏构建使用 `--native-filter`，在写入位置只检查追踪开关与非零长度，把范围判断集中到不参与插桩的 C++ trampoline，减少重复的 Wasm 指令。两种模式均保留有效地址溢出检查。函数内优化清除常量判断与多余局部变量，并保持函数索引。

C++ trampoline 对状态区之外的写入直接返回；对同帧已记录的标量写入也直接返回，未对齐写入同时检查首尾两个块。这些快速判断只读取内存；需要保存首次写入内容时，先关闭追踪再调用日志辅助函数。`--native-filter` 要求使用具备完整范围裁剪与递归保护的后端，并保留配置导出。

原生后端另保留 `eagler_rb_capture` 冷路径导出，避免链接优化把完整的捕获循环重新内联到高频 trampoline；插桩器的必需导出约定保持下表三项。封帧通过 `LastFrameBlocks()` 读取已维护的块数，完整 `Stats()` 的分配器遍历用于诊断查询。

## C++ 导出约定

| Wasm 导出 | 签名 | 作用 |
| --- | --- | --- |
| `eagler_rb_touch` | `(i32 address, i32 bytes) -> void` | 保存写入范围内尚未记录的状态块 |
| `eagler_rb_config_address` | `() -> i32` | 返回静态、4 字节对齐的五个 `uint32_t` 配置字地址 |
| `eagler_rb_instrumented` | `() -> i32` | 运行时校验插桩标志 |

配置字依次是 `tracking`、`heapBase`、`heapBytes`、`sessionBase`、`sessionBytes`。两个状态范围都是有效且不绕回地址零的 wasm32 区间；空范围使用长度零。guard 检查整个写入范围与两个状态范围的交集，因此从状态区域之前开始的跨界写入也能进入日志。

`eagler_rb_config_address()` 必须直接返回静态数组的地址，使链接后的函数返回常量。marker 的 C++ 占位实现使用 `noinline` 和一次静态 volatile 零值读取；该标志字须 4 字节对齐，并与配置字分开。插桩器把该函数体改为 `0x52424a31`，同时追加数据段初始化其标志字，使最终优化器内联过的读取也获得相同版本。已有被动数据段的索引保持不变。初始化时检查实际读取结果，原始未插桩构建返回零并拒绝启用写日志后端。

touch、config、marker 三个函数本身不插桩。touch 在进入日志辅助函数前关闭 `tracking`，退出时恢复，避免日志自身的内存写入再次记日志；它必须将范围裁到已注册区域，处理首次写入、容量边界及失败状态。每个函数的临时局部变量按写入嵌套深度分配，嵌套存储、bulk 操作和递归调用各自保留外层目标地址。

导出名可以通过 `--touch=name --config=name --marker=name` 配置。`--no-config` 对每个非空写入调用 touch，范围与递归过滤由 touch 负责。

`SystemScope` 只改变分配归属，仍记录游戏内存写入。`HostScope` 还会暂时关闭追踪，仅适用于已审查的纯宿主调用链：它不能写入或释放游戏内存，也不能回调游戏。嵌套调用恢复进入前的追踪状态，保留帧事务边界；发生终止性错误后不会重新启用追踪。

`Clear()` 在已关闭的帧边界释放历史；如果事务仍然开启，则将后端标为终止失败。Wasm 越界陷阱会跳过 C++ 析构，可能留下仍开启但追踪已暂停的事务，因此清理根据帧状态判断。清理保留首个错误原因，后续 `Ready()` 与 `BeginFrame()` 均拒绝重新启用该后端。

堆用量与 `mspace_mallinfo().uordblks` 保持精确一致。分配器 wrapper 在宿主静态区保留一个按 mspace 地址识别的缓存槽。首次查询建立总量；缓存有效时，分配和对齐分配按返回对象的实际 chunk 大小增加总量，释放按合并前的实际 chunk 大小减少总量，不用请求大小或 usable bytes 估计。该不变量依赖固定的外部内存段；编译期断言禁止开启堆增长、mmap、footers、错误恢复或锁。创建、失败、跨 mspace 修改，以及成功恢复分配器元数据后和清理边界会使缓存失效。allocator 写入期间若日志失败，也不会重新启用已失效的缓存。多个 mspace 交替查询会替换缓存槽，不会复用其他空间的值。普通系统堆操作不影响该缓存；它与现有分配器一样只用于单线程调用。

## 验证职责

测试实例化真实 Wasm，比较插桩前后的内存结果，并用独立日志恢复已跟踪区域。覆盖标志检查、标量宽度、向量存储、两个状态区、未对齐跨块、两端跨界、嵌套操作数求值、递归、被动数据段初始化、数据段释放及原始陷阱行为。

`node tools/rollback/memory-test.mjs` 使用 `EMSDK` 指定的 SDK（默认 `toolchains/emsdk`）以 `-O2` 编译真实 C++ 后端和 `common/tests/write-journal-test.cpp`，然后分别使用默认模式、`--native-filter` 及 native inline dirty guard 插桩。测试先验证原始模块拒绝初始化，再为每种模式的十五个场景创建独立实例，检查容器与分配器元数据恢复、跨越外部 root 两端的写入及邻接字节保留、1,100 次回放与标签轮转、嵌套分配作用域、失败状态，同时覆盖已记录块和未记录块的非对齐标量写入、无外部 root 的整堆恢复，以及宿主调用的嵌套追踪暂停与失败保护。场景 9–11 用独立的 uncached mallinfo 查询和扫描计数核对堆用量缓存，覆盖一次和嵌套恢复、系统分配作用域、清理、无效目标及开启帧上的恢复拒绝。构建产物与结果保存在 `artifacts/rollback/`；可用 `--emxx=/path/to/em++` 和 `--output=/path/to/artifacts` 指定编译器与产物目录。

每种插桩模式另有三个真实日志耗尽案例：先填满日志并建立有效的堆用量缓存，再分别在 malloc、memalign、free 的元数据写入期间触发失败，要求返回后的首次查询重新扫描且与 uncached oracle 完全相等。还有两个真实越界写入案例：分别在 `GameScope` 和嵌套 `HostScope` 中触发 Wasm 陷阱，JavaScript 捕获后再调用清理入口，验证未封帧状态始终保持终止失败。总计运行 52 个独立模块实例：1 个未插桩拒绝实例，以及 3 种配置各 15 个场景与 2 个陷阱场景；另检查四个未启用后端的真实 Wasm 对象不定义任何符号。

补充的 native 分配器检查可运行 `node tools/rollback/heap-cache-test.mjs`，需要支持 C++20 的 `c++` 或 `CXX` 指定的编译器，也可传入 `--cxx=/path/to/c++` 与 `--output=/new/directory`。该入口编译 `common/tests/simulation-heap-cache-test.cpp`，逐项比较精确用量与独立 uncached oracle，检查成功分配/释放后的查询无额外扫描、多 mspace 切换、同地址重建、失败及零字节分配、对齐、碎片化、释放和普通系统分配，并运行 20,000 步确定性分配轨迹。每次比较后再做 128 次缓存命中检查。`EAGLER_WRITE_JOURNAL_TEST_HOOKS=1` 仅用于这些测试的 oracle 和扫描计数；生产构建不包含这些入口。真实 undo 的验证仍由上述 Wasm 场景承担。

构建限定为单个、非共享 wasm32 线性内存。原子操作、SIMD lane 内存操作、memory64、多内存和未知指令形态会明确中止插桩。宿主 JavaScript 直接写入线性内存需要在对应桥接处接入 touch，或使用系统区域承载这些写入；Wasm 指令插桩覆盖的是模块内部执行的写入。

## 来源

整堆与写入日志方案参考 [apeirs/th11 的公开实现](https://github.com/apeirs/th11/blob/324fb143e78560ae34f5930a19048e34035eb2f8/portable/rollback/instrument-rollback.mjs)，关联 [th11 issue #7](https://github.com/Ritosa2333/th11/issues/7)。本工具保留该方案的导出 hook 结构，并加入操作数顺序保持、范围相交、`memory.init` 覆盖及构建标志验证。
