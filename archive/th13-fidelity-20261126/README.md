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

## 结局 BGM 第二参数与文本 VM（原分歧 2）

结局脚本 op 表 `0x4171e8` 的 op 10（`0x416a10:210-237`）先调 `0x43adc0(0, name)` 播放音乐文件，再把脚本里的音乐名与 `0x4aaaf4` 的 `bgm/th13_18` 比较：相同则 `0x43ae40(0, 0xf)`，否则 `0x43ae40(0, 0x10)`。`0x43ae40(bgm, idx)` 做两件事：`0x461830(2, bgm)` 向 BGM 通道请求播放，`save[0x1b27a + idx] = 1` 置音乐室已听标记。第二个参数就是音乐室曲目序号。移植侧 `EndingScene::op_music` 以同一比较取 0xf/0x10，经 `record_music_unlocked` 写入 `FileStore::heard_mask`。

文本页是 5 个 ANM VM：op 3（`0x416a10:51-92`）在行号 `+0x78` 为 0 时先给 5 个 VM（`+0x40…+0x4c`）各写一次白色的 `" "` 并逐个 `0x46fc30` 复位，再把本行文本经 `0x46e8f0` 写进第 `+0x78` 个 VM 的贴图并 `0x46fc30`，行号加到 5 回绕；op 4 对 5 个 VM 逐个 `0x46fc30`。`0x46e8f0` 经 `0x46e810` 进入 `0x45aef0`：`SelectObject` 选字体后 `TextOutA` 以描边色（结局传入 0）在 (±2, 0)、(±2, 4) 各画一遍，再以正文字色 `+0x7c` 在 (0, 2) 画一遍，随后 `0x45a3c0` 反相、`0x45a080` 把背景像素取邻域均值，结果作为该 VM 的贴图上传。移植侧 `ApplicationState::initialize_ending_text` 建 5 个文本 VM（脚本 68–72），`ending_page_line` 用同一字体家族（MS Gothic）以 2 倍分辨率栅格化后按 0.5 倍重采样进 VM 的 512×20 精灵区并 `interrupt(2)`，`ending_page_reset` 写白色空白行并 `interrupt(3)`，`EndingScene::op_page_line` 实现同一套行号复位与回绕。VM 结构、脚本路径与贴图更新方式与原版同构；字形由平台字体引擎栅格化（原版为 GDI `TextOutA`，移植为运行时字体栅格）。

## 结算B 的 BClear 落档与 0x414fc0 降级（原分歧 3）

`0x43d820` 只有一个调用点：结算B 态 `0x1f`（`0x440960:549-560`）。该态由 `0x43f720` 在 `DAT_004be830 >> 4 & 3 == 1` 且 `GameInf+0x74 == 0` 时进入。v1.00c 里 `DAT_004be830` 的 `0x10` 位没有任何写入方：整个 `.text` 对 `0x4be830` 的 90 处引用中，写 `0x30` 位的只有回放装载（`0x447cd2` 置 `0x20`、`0x447ce5` 清 `0x30`）与标题回放入口（`0x44cd80` 清 `0x30`），`0x10` 位从不置位。`GameInf+0x74` 由场景机 `0x43c1c0` 写入：对局（场景 7、10、0xe）为 0，练习、回放与关内重试（场景 0xd、0xb）为 1，下一关（场景 0xc）继承。因此模式位非零的运行恒有 `+0x74 == 1`，`0x42cd40` 把它们全部送往结算A（`0x43e5a0`）；`0x1f` 与 `0x1e` 两态不可达，`0x43d820` 的分数落档没有执行路径。

`0x414fc0(请求值)`：`[this+0x5bc] & 0x2000` 置位时把请求改写为 2。`+0x5bc` 是 supervisor 的场景旗标；对 `0x4dcc5c`（模式旗标）与 `+0x5bc` 的全部写入中都没有置 `0x2000` 的指令（`0x439e10` 只清位）。降级分支在 v1.00c 同样不可达，移植侧按请求值直通即对齐。

## Boss 血环只处理槽 0（原未决 1）

血环段的槽循环从 `0x426910` 把槽号置 0 开始，到 `0x42733f` 处 `INC EAX; CMP EAX, 1; JL 0x426918` 结束——上界是 1，循环体只执行一次。HUD 里虽然按 `0x54` 字节给两个槽各留了一块（比例、当前生命、7 个 VM 句柄与创建标志，`+0x1a8` 起），原版每帧只对 `mgr+0x5c`（槽 0）走显示与隐藏两条路径，槽 1 的块从不读写。移植侧 `BossPresentation` 的单环与原版同构：五个门（计时 ≥ 0、槽内有 Boss、`mgr+0x7c & 1 == 0`、`HUD+0x198 == 0`、管理器非空）加槽内的存活、`flags & 0x31 == 0`、无敌计时 < 1，比例 `life / life_max`、每帧最多加 0.025、超出即回落，7 个 VM（环弧、两个覆盖、四颗星）跟随 Boss 位置，自机距离 80 内隐藏、96 外恢复。

## 存档镜像的练习行（原分歧 4）

`0x42b990` 在关卡开始把游玩旗写进 `score + 0x7fd + 8*stage + 64*difficulty`，`0x42cd40` 在过关时把通过旗写进 `score + 0x7fc + 8*stage + 64*difficulty`，`stage` 取 1 起的关号（[practice-rows.json](practice-rows.json)）。直调 `0x449e50` 生成镜像后，这两个字节落在列内 `0x7fd/0x7fc`，关卡分数在 `0x7f8`（[save-image.json](save-image.json)）；旧镜像把行基放在 `0x7f4`，整体早 4 字节。通过旗在原版里没有读取方（`0x451cb0`、`0x452170`、`0x452299` 一带读的都是游玩旗），移植侧照样写入：`ScoreDat::kProgressRowsOff` 取 `0x7F8`，`FileStore::practice_cleared[16]` 按角色×难度各存 6 位，`RunScene::msg_stage_clear` 经 `record_stage_passed` 在普通版直接落盘、在多人版走 `PendingFileEvent::StagePass`。应用档 `TH13SAVE` 升到 v8：新增 `CLR1` 段（16 字节），v7 及更早的档读入时按零补齐、下次保存升级。`tests/score/filestore_score_mapping_test.cpp`、`tests/score/filestore_score_test.cpp` 与 `tests/ui/run_scene_test.cpp` 锁定坐标、v8 往返、v7 升级与写入失败回传。

## ECL 全局变量供给（原分歧 5）

ECL 全局读是两个 94 项跳转表：整型读 `0x420380`，浮点读 `0x420d00`；写槽是 `0x420b80` 与 `0x421670`。读槽与写槽一一对应：管理器可写整数落在 `mgr+0x10/0x14/0x18`，Boss 槽 0 存在时 Boss 自变量写 Boss、否则写当前敌机；共享整数、共享浮点和敌机本地槽也按原地址表映射。敌机 id（−9914）只有读路径，没有写目的地。

−9988 是敌机相位时钟 `Timer.current`（`+0x14a8` 整型视图 / `+0x14ac` 浮点视图），513/522 与相位切换复位；−9954 是敌机当前 life（`+0x5138` 整型视图）；−9986 是 `+0x521c` 的 bit25，相位由超时结束时置 1、由 life 触发切换时保持 0。探针输入把 ENEMY、Boss 槽 0、第三台敌机、玩家、ANM VM、管理器与系统全局分别设成可区分值，每次读前重置游戏流种子 `0x5678`，因此当前敌机、Boss 槽 0 与自机三个供给源不会互相混叠。

`tools/oracle/probes.py --out-dir` 生成的 [stage-gvars.json](stage-gvars.json) 记录 94 个整型读、94 个浮点读和完整 setup；两次运行逐字节相同，随机流 id 附带抽取后的种子与计数。`tests/ecl/ecl_stage_gvars_oracle_test.cpp`（普通版与多人版同文件同内容）用同一 setup 重建输入，用 constexpr 数组逐位断言全部读值、浮点 bits 与随机流状态，并先通过写槽验证 −9949..−9947 的读写同槽。

## 插值角路径与 Bezier 的 x87 中间量（未决 2 的剩余部分）

角度插值是 `0x4662e0`，与 `0x4120f0` 共用同一插值块布局。mode 7 把 `wrap(start + ctrl2)` 写回 start；mode 17 先 `wrap(start + ctrl2)` 再 `wrap(end + ctrl2)`；mode 8 的四个基函数各在 x87 里算好后转 float，再乘控制点、再转 float、再 `wrap`，按 end、start、ctrl1、ctrl2 的顺序依次累加，每次累加转 float 再 `wrap`；默认缓动先把 `0x438220` 的角差 wrap 一次，乘权重后 wrap，加 start 再 wrap。`0x472f30` 的归一是带 33 次上限的 2π 循环，每圈结果转 float。非角 mode 8 的四个基函数各转 float，四项在 x87 累加后一次舍入。

[interpolation-angle.json](interpolation-angle.json) 记录 mode 7、17、8、0、1、18、22 七组用例的逐帧值（起止角跨 ±π 多圈）。`tests/anm/anm_interpolation_oracle_test.cpp`（普通版与多人版同文件同内容）把它们锁定到 `AngleInterpolation`，并以 `scalar_bezier_matches_retail_oracle` 锁定 `0x4120f0` 的 mode 8。
