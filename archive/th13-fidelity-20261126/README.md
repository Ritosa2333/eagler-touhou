# th13 保真地图分歧收尾（2026-11-26）

保真地图「已知分歧」与「未决项」在普通版与多人版两侧成对推进，每项都以原版直调探针为依据，探针输出与移植逐位对照。

## ANM 变量表（原分歧 5）

| 项 | 原版依据 | 移植 | 验收 |
|---|---|---|---|
| 读值与随机流 | 浮点读 `0x461e80`：10000–10009 本地变量，10010–10012 取视觉流 `0x4dc31c`，10022 取视觉流后对 `+0x4dc` 取模，10030–10032 取游戏流 `0x4dc324`；`0x472de0` 得 [0,1)，`0x472e10` 得 [−1,1)；10016–10021 读摄像机块 `0x4dcaf4`–`0x4dcb44`，10026 把自身与父 VM 的 Z 旋转相加后过 `0x472f30` | `game/AnmVariables.cpp` 的 `floating`/`integer` 与 `AnmInterpreter.cpp` 的 10016–10021、10026 分支 | 直调 37 个 id 的读值与每次抽取后的种子、计数（[anm-variables.json](anm-variables.json)） |
| 写目的地 | 浮点 `0x4622c0`：10004–10007→`+0x4bc…+0x4c8`，10013–10015→`+0x3c…+0x44`，10023–10025→`+0x48…+0x50`，10027→`+0x4d4`，10028→`+0x4d8`；整型 `0x462380`：10000–10003、10008/10009、10029；其余 id 落指令参数槽 | `float_destination`/`integer_destination` 的映射与两张字节表逐项相同 | 同上，目的地按 VM 内偏移逐 id 记录 |

`tests/anm/anm_variables_oracle_test.cpp`（两侧各一份、内容相同）把上述读值、随机流消费与目的地逐位锁定。

## 练习行坐标（原分歧 4 的前置核对）

`0x42b990` 在关卡开始写游玩旗、`0x42cd40` 在过关时写通过旗，两条写入的地址都是
`score + 0x7fd / 0x7fc + (stage + difficulty*8)*8 + character*0x56dc`，`stage` 取 1 起始的关号
（[practice-rows.json](practice-rows.json)）。直调 `0x449e50` 生成镜像后，游戏侧偏移比列内偏移大 8：
通过旗 `0x7f4+8*stage`、游玩旗 `0x7f5+8*stage`、关卡分数 `0x7f0+8*stage`（难度每加一移 0x40）。

## etEx 0x400000 缩放补间（原分歧 1）

`0x40e4d0:24-29` 在 etEx 的参数 `0x400000` 分支里完成四件事：置弹的 behavior 位 `0x400000`、把起止缩放写进 `+0x12f4/+0x12f8`、把原始帧数与缓动写进 `+0x131c/+0x1320`、复位计时并把 `flags(+0x20)` 加上 `0x40`。弹更新每帧按缓动推进缩放，`duration==0` 时清 behavior 位，终值回到 1.0 再清 `0x40`。

移植侧 `StageSim::step_bullet_behaviors` 走同一条路径：`StageBullet::scale_tween` 是 `FloatInterpolation<1>`，`case 0x400000` 置位并记录起止与原始帧数，推进 `hit_scale`，`duration==0` 清位，终值 1.0 再清 `0x40`。`tests/ecl/ecl_bullet_scale_test.cpp`（两侧各一份、内容相同）锁定置位、帧数、逐帧缩放与清位。

## 插值终值的 x87 单次舍入（未决 2 的插值部分）

`0x4120f0` 的默认路径把 `(end-start)*weight+start` 整条算在 x87 扩展精度里，再一次性舍入到 float；游戏内 `0x472de0/0x472e10` 的 FINIT 把 FPCW 置 0x37F（PC=64），与探针容器的默认控制字相同。直调五组用例（线性 30 帧、平方 20 帧、线性 45 帧、正弦 10 帧、1 帧）共 111 个终值，见 [interpolation.json](interpolation.json)。移植侧 `Interpolation.hpp`/`.cpp` 的非角路径改为 `(float)((long double)(end-start)*weight+start)` 后与直调值逐位相同；WASI 构建 `long double` 为 113 位，x86 宿主为 80 位，两者在这五组用例上结果一致。`tests/anm/interpolation_oracle_test.cpp`（两侧各一份、内容相同）把这些终值同时锁定在 `FloatInterpolation<1>` 与 `EclInterpolation` 上。
