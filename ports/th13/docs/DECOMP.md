# th13 反编译函数地图

来源：Ghidra 12.1.3 headless（容器内运行），目标 th13.exe v1.00c（SHA256 `f12760ca…80d`）。
产物在工作区 `artifacts/decomp/jp/`（开发证据，不入库）：`functions/`（2291 个函数反编译体）、`functions.json`（调用图/字节数/artifact 标记）、`blocks.json`、`imports.json`、`strings.json`。

## 总量与分区

| 分区 | 函数数 | 说明 |
|---|---|---|
| `< 0x470000` | 1166 | 游戏代码主体（ZUN 对象文件先链） |
| `≥ 0x470000` | 1125 | 静态链接 MSVC CRT 与编译器支持代码 |

全部 2291 个函数反编译成功；941 个含寄存器传参 artifact（`unaff_*`/`in_E*`），由 Refine 轮与人工签名修正消化。

## 已锚定目标

| 原版地址 | 结论 | 证据 |
|---|---|---|
| `0x47BBA2` | MSVC CRT `_rand()`：`ptd = _getptd(); s=[ptd+0x14]; s = s*0x343FD + 0x269EC3; [ptd+0x14]=s; return (s>>16)&0x7FFF` | 常量对唯一出现于 0x47BBAE/0x47BBB3；Unicorn oracle 直调序列 = MSVC srand(1) 标准序列 |
| `0x47FE5A` | MSVC CRT `_getptd()`（线程数据块） | rand 体内 call 目标；oracle 以伪造 ptd 覆写 |
| `0x47BDA0` | PE 入口（CRT 启动） | PE 头 |

C++ 镜像：`web/cpp/game/Rng.hpp`（常量已锁），锚点登记于 `web/cpp/rebuild/AddressMap.hpp`。

## 恢复优先级线索（按体积 × 调用密度）

| 地址 | 体积 | 调用数 | 被调 | 推测 |
|---|---|---|---|---|
| `0x41B260` | 17.6KB | 491 | 1 | ECL 解释器主分发（switch 密集） |
| `0x462450` | 13.6KB | 320 | 84 | ANM VM / 管理器主循环 |
| `0x4265A0` | 4.9KB | 94 | 0 | 独立大函数（无调用者，或为导出/回调） |
| `0x428AE0` | 4.5KB | 149 | 1 | 子系统 update |
| `0x44CD10`/`0x44ED70` | ~3.9KB | ~100 | 1 | 敌机/道具管理器 |

恢复顺序按 `docs/DEVELOPMENT.md` 行为还原节奏推进；每锚定一个子系统即在 `AddressMap.hpp` 登记。
