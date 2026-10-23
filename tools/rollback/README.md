# Wasm 写入日志插桩

这个构建步骤把游戏状态区域的 Wasm 写入接入回滚日志。它覆盖整数、浮点和完整 `v128` 存储，以及 `memory.copy`、`memory.fill`、`memory.init`。写入指令的原始操作数先按原顺序求值，日志随后保存改写前的内容，最后执行原始写入。

## 构建接入

```sh
npm --prefix tools/rollback ci --ignore-scripts
node tools/rollback/instrument.mjs raw.wasm runtime.wasm
node tools/rollback/instrument.mjs --verify runtime.wasm
node --test tools/rollback/instrument.test.mjs
```

构建器将最终 `runtime.wasm` 交给打包步骤。如果还要运行 `wasm-opt`，在最终优化产物上再执行 `--verify`。CLI 在完整校验后原子写入输出文件，并打印包含各类写入数量的 JSON 构建记录。

## C++ 导出约定

| Wasm 导出 | 签名 | 作用 |
| --- | --- | --- |
| `eagler_rb_touch` | `(i32 address, i32 bytes) -> void` | 保存写入范围内尚未记录的状态块 |
| `eagler_rb_config_address` | `() -> i32` | 返回静态、4 字节对齐的五个 `uint32_t` 配置字地址 |
| `eagler_rb_instrumented` | `() -> i32` | 运行时校验插桩标志 |

配置字依次是 `tracking`、`heapBase`、`heapBytes`、`sessionBase`、`sessionBytes`。两个状态范围都是有效且不绕回地址零的 wasm32 区间；空范围使用长度零。guard 检查整个写入范围与两个状态范围的交集，因此从状态区域之前开始的跨界写入也能进入日志。

`eagler_rb_config_address()` 必须直接返回静态数组的地址，使链接后的函数返回常量。marker 的 C++ 占位实现使用 `noinline` 和一次静态 volatile 零值读取；该标志字须 4 字节对齐，并与配置字分开。插桩器把该函数体改为 `0x52424a31`，同时追加数据段初始化其标志字，使最终优化器内联过的读取也获得相同版本。已有被动数据段的索引保持不变。初始化时检查实际读取结果，原始未插桩构建返回零并拒绝启用写日志后端。

touch、config、marker 三个函数本身不插桩。touch 进入后先关闭 `tracking`，退出时恢复，避免其辅助函数的内存写入再次记日志；它必须将范围裁到已注册区域，处理首次写入、容量边界及失败状态。每个函数的临时局部变量按写入嵌套深度分配，嵌套存储、bulk 操作和递归调用各自保留外层目标地址。

导出名可以通过 `--touch=name --config=name --marker=name` 配置。`--no-config` 对每个非空写入调用 touch，范围与递归过滤由 touch 负责。

## 验证职责

测试实例化真实 Wasm，比较插桩前后的内存结果，并用独立日志恢复已跟踪区域。覆盖标志检查、标量宽度、向量存储、两个状态区、未对齐跨块、两端跨界、嵌套操作数求值、递归、被动数据段初始化、数据段释放及原始陷阱行为。

`node tools/rollback/memory-test.mjs` 使用 `EMSDK` 指定的 SDK（默认 `toolchains/emsdk`）以 `-O2` 编译真实 C++ 后端和 `common/tests/write-journal-test.cpp`，然后执行同一个插桩步骤。测试先验证原始模块拒绝初始化，再为每个场景创建独立实例，检查容器与分配器元数据恢复、外部 root 邻接字节保留、1,100 次回放与标签轮转、嵌套分配作用域和失败状态。构建产物与结果保存在 `artifacts/rollback/`；可用 `--emxx=/path/to/em++` 和 `--output=/path/to/artifacts` 指定编译器与产物目录。

构建限定为单个、非共享 wasm32 线性内存。原子操作、SIMD lane 内存操作、memory64、多内存和未知指令形态会明确中止插桩。宿主 JavaScript 直接写入线性内存需要在对应桥接处接入 touch，或使用系统区域承载这些写入；Wasm 指令插桩覆盖的是模块内部执行的写入。

## 来源

整堆与写入日志方案参考 [apeirs/th11 的公开实现](https://github.com/apeirs/th11/blob/324fb143e78560ae34f5930a19048e34035eb2f8/portable/rollback/instrument-rollback.mjs)，关联 [th11 issue #7](https://github.com/Ritosa2333/th11/issues/7)。本工具保留该方案的导出 hook 结构，并加入操作数顺序保持、范围相交、`memory.init` 覆盖及构建标志验证。
