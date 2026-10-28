# th13 ECL 虚拟机（ECL v13）行为规格

依据（本文所有结论的出处）：

- Ghidra 反编译导出 `artifacts/decomp/jp/functions/*.c`、`functions.json`（2291 函数）。
- 原版 EXE `games/th13/th13.exe`，经 `oracle/machine.py`（容器内 unicorn）直读内存：ECL 主分发跳转表（0x41f760 起 158 项）与密集化字节表（0x41f9d8 起 704 项）。
- 反汇编脚本 dump `artifacts/reference/dumps/ecl/*.txt`（thecl -d13 -j，无 eclmap 时未映射指令显示为 `ins_NNN`）。
- 原始 ECL 字节 `artifacts/reference/raw/*.ecl`（st01.ecl 已逐指令走查验证编码，见 §1.3）。
- th15 参考实现 `eagler-touhou/ports/th15/web/cpp/game/{EclContext,EclThreads,EclInterpreter,EclFile,EclResource}.{hpp,cpp}`。
- th13 指令/全局变量名映射已落库：`artifacts/reference/eclmap/th13.eclm`（取自 priw8/eclmap master，491 行）。带名重新 dump 可用
  `thecl -d13 -m artifacts/reference/eclmap/th13.eclm ...`。

本文地址均指 th13.exe（JP，image base 0x400000）。

---

## 1. 文件格式（已对原始字节验证）

### 1.1 SCPT 容器

```
+0x00  char[4]  "SCPT"
+0x04  u16      version = 1
+0x06  u16      include_length（ANIM+ECLI 区总长；st01 = 0x40）
+0x08  u32      include_offset = 0x24
+0x0C  u32      0
+0x10  u16/u32  sub_count（st01 = 59）
+0x14  u16[8]   0
+0x24  ANIM 列表: char[4]"ANIM" + u32 count + count 个 NUL 结尾文件名
      ECLI 列表: char[4]"ECLI" + u32 count + count 个 NUL 结尾文件名（各段 4 对齐）
      u32[sub_count] 子程序偏移表（相对文件头）
      子程序名表（NUL 结尾字符串，顺序与偏移表一致）
随后每个偏移处一个子程序块
```

子程序块：`char[4]"ECLH" + u32 data_offset(=0x10) + u32 zero[2]`，指令流从块内 +0x10 开始。名称查找是按名字二分（0x4797d0，表在上下文 +0x8C，条目 `{char* name; u32 offset}`，命中返回 `sub 块内偏移 + 0x10` 即指令流坐标）。

### 1.2 指令记录（16 字节头，文件内即运行时格式——原版直接在缓冲区上执行）

```
+0x00  u32  time            调度时刻（相对线程时钟；脚本里通常为 0，thecl 用 !time 渲染非 0）
+0x04  u16  opcode
+0x06  u16  length          本指令总字节数，4 的倍数，>=16
+0x08  u16  param_mask      bit i = 参数 i 需要"解析"（是变量/全局/表达式引用而非字面量）
+0x0A  u8   rank_mask       1111LHNE：bit0 Easy, bit1 Normal, bit2 Hard, bit3 Lunatic
+0x0B  u8   param_count
+0x0C  u32  stack_adjust    TH13 起记录参数表中弹出的表达式栈字节数
+0x10  ...  参数区（每参数 4 字节；调用类为 {u32 名字长度; char 名字[]}，名字后可跟
            8 字节参数描述符 {char 源类型; char 目标类型; u32 原始值}）
```

证据：反编译中三处独立读取与该布局一一对应——主分发读 `[Q+4]` 为 opcode、`[Q+8]` 为 mask（`0041b260.c:135-143`）；取参函数读 `[Q+8]` mask、`[Q+0x10+slot*4]` 参数（`00478e60.c` 反汇编 `movzx esi, word [eax+8]` / `lea ecx,[eax+ecx*4+0x10]`）；解释器读 `[Q+0]` 为调度时刻、`[Q+6]` 为步进长度（`004772a0.c:72-79,1030-1035`）。

### 1.3 st01.ecl 首子程序 GirlA01 逐指令走查（python 脚本按 §1.2 头走查 raw/st01.ecl 0x3e0 起）

| 文件偏移 | time | opcode | len | mask | rank | cnt | 参数 | dump 对应 |
|---|---|---|---|---|---|---|---|---|
| 0x3e0 | 0 | 40 stackAlloc | 20 | 0 | ff | 1 | (0) | `var`（隐藏） |
| 0x3f4 | 0 | 500 setHurtbox | 24 | 0 | ff | 2 | 24.0f, 24.0f | `ins_500(24.0f,24.0f)` |
| 0x40c | 0 | 501 setHitbox | 24 | 0 | ff | 2 | 16.0f, 16.0f | `ins_501(...)` |
| 0x424 | 0 | 404 moveVel | 24 | 0 | ff | 2 | 1.1780972f, 4.0f | `ins_404(...)` |
| 0x43c | 0 | 405 moveVelTime | 32 | 0 | ff | 4 | 60,1,1.5707964f,0.0f | `ins_405(...)` |
| 0x45c | 0 | 15 callAsync | 32 | 0 | **cc** | 1 | len=0xC,"GirlA01_at" | `!HL @GirlA01_at() async;` |
| 0x47c | 0 | 15 callAsync | 36 | 0 | **c8** | 1 | len=0x10,"GirlA01_at_l" | `!L @GirlA01_at_l() async;` |
| 0x4a0 | 0 | 23 wait | 20 | 0 | ff | 1 | 60 | `ins_23(60)` |
| 0x4b4 | 0 | 44 pushf | 20 | **1** | ff | 1 | 0xc61c1400(≈-9989.0f) | `[-9989.0f]`（ANGLE_PLAYER） |
| 0x4c8/0x4dc/0x4f0/0x500 | 0 | 44,44,57,51 | 20/16 | 1,1,0,0 | ff | | -999x.0f、20.0f、divf、addf | `[-9998.0f]/20.0f` 表达式续 |
| 0x510 | 0 | 429 moveVelNMTime | 32 | **4** | ff | 4 | 60,0,**-1.0f**,1.0f（stack_adjust=8） | `ins_429(60,0,[expr],1.0f)` |
| 0x530 | 0 | 12 jmp | 24 | 0 | ff | 2 | Δ=0x2C, time=0 | `goto GirlA01_396 @ 0`（子程序内 +0x160+0x2C=+0x18C=396 ✓） |
| 0x548 | 0 | 23 wait | 20 | 0 | ff | 1 | 1000 | GirlA01_376 |
| 0x55c | 0 | 42 pushi | 20 | 0 | ff | 1 | 1 | `if (1)` 条件 |
| 0x570 | 0 | 14 jmpNeq | 24 | 0 | ff | 2 | Δ=-40(0xffffffd8), 0 | `goto GirlA01_376 @ 0`（+0x1a0-40=+0x178=376 ✓） |
| 0x588 | 0 | 10 ret | 16 | 0 | ff | 0 | | `return;`（0x588+0x10=0x598=下一子程序 ECLH ✓） |

关键编码结论：

- **跳转/调用偏移相对当前指令**（`target = 本指令子程序内偏移 + Δ`）。callAsync/call 的被调者只用名字字符串，运行时查名字表。
- **mask bit 置位的参数值语义**（取参函数 0x478e60 整型 / 0x478ee0 浮点，两套判据）：
  - 整型：`>=0` → 局部变量字节偏移；`-1..-100` → 全局变量码（走敌人虚表 +4 槽 0x420380）；`<=-101` → 表达式栈引用（值×8 为栈内偏移）。
  - 浮点：`>0` → 局部变量字节偏移（float 视图）；`-1.0..-100.0` → 表达式栈（-1.0 = 弹栈顶，-N = 第 N 项）；`<-100` → 全局变量码（浮点形式的全局码如 -9989.0f，走虚表 +0xC 槽 0x420d00）。
- **表达式参数**：脚本里方括号表达式被 thecl 编译成前置的栈机指令序列（pushi/pushf/运算），指令本体参数位置放 -1.0f 之类的栈引用，`stack_adjust` 记录事后弹掉的栈字节数。
- **stack_adjust 在执行成功后由解释器统一扣栈**（`004772a0.c` `caseD_0` 分支；call 指令会先把当前指令的 stack_adjust 清 0——`00477030.c:117` 与 th15 `Op::Call` 同款行为）。

---

## 2. VM 状态模型

### 2.1 线程状态块（每次分配 0x11C4 字节，0x4793e0）

| 偏移 | 类型 | 含义 | 证据 |
|---|---|---|---|
| +0x0 | float | 线程时钟 `time`（每帧 +elapsed） | 004772a0.c:75,1036 |
| +0x4 | ptr | 当前指令 Q（直接指向 ECL 缓冲区内 16 字节头） | 0041b260.c:133-135；00477030.c:117 |
| +0x8..+0x1007 | u8[4096] | 栈区：低端为调用帧/局部变量，高端为表达式栈；条目 8 字节 `{char 'i'/'f'; u32 值}` | 00479a20（SP/FP）；00478e60.c:24-29 |
| +0x1008 | u32 | SP（表达式栈顶字节偏移，向高增长） | 004772a0 反复读写 `[EBX+0x1008]` |
| +0x100C | u32 | FP/局部基（stackAlloc 压旧值后指向旧 SP） | 00479a20.c:13-19 |
| +0x1010 | u32 | 线程 id（callAsyncId 用；匿名 = -1） | 004793e0.c:23；00479480 按 +0x1010 查找 |
| +0x1014 | ptr | 宿主敌人对象（全局变量虚表调用入口） | 00478e60.c:19；00478ee0.c:39 |
| +0x101C | u8 | 难度位（spawn 时从主线程拷贝） | 004793e0.c:25；004772a0.c:78 |
| +0x1020+0x30*i (i=0..7) | 8×0x30 | 8 条插值通道 {flags(+0x1044+0x30i), time, …}（指令 91/92 挂接） | 00417450 初始化 8 组；004772a0 case 0x5b/0x5c |
| +0x11A0+4*i | ptr[8] | 各通道挂接的指令指针（每帧步进通道时借用作取参上下文） | 004772a0.c:1038-1085（`local_b8 = EBX+0x468`，0x468*4=0x11A0） |
| +0x11C0 | u32 | 线程标志 bit0=暂停（pauseAsync 置位） | 004772a0 case 0x12/0x13 |

> th15 的 `EclContext`（0x11E8 字节）与本表逐字段同构（`EclContext.hpp:36-49`），th13 差异仅：+0x1014 宿主指针、无 th15 新增字段。th13 的 0x11C4 = 0x11A0+0x20（8 指针 + 标志 + 对齐）。

### 2.2 敌人侧的 ECL 运行时（宿主）

- 敌人对象内嵌 ECL 上下文 T（自引用指针存于敌人 VM 区 +0x40C8 处，即敌人绝对 +0x52B4；`0041b260.c:133`、`00421b10.c:10`）。T 承担：当前线程槽（T+4，遍历时逐线程改写，0x478db0）、名字→偏移查找表（T+0x8C，0x4797d0）。SCPT 注册（0x4795b0）作用于另一个资源对象 `{+0 vtable, +4 文件计数, +8 label 总数, +0xC 文件指针表, +0x8C label 表}`，它与 T 的关系未完全定域（见风险 4）。
- 线程链表：哨兵 enemy+0x11D0，头 enemy+0x11D4，尾 enemy+0x11DC；节点 0x10 字节 `{线程指针, prev, next, 0}`（0x4793e0 链入、0x478db0 遍历拆除）。主线程状态是独立分配的同规格块（构造于 0x418f40，入口函数经 0x4797d0 名字解析）。
- th13 的"子程序参数→敌人槽"补充：全局码 -9985..-9982（I0..I3）/ -9981..-9978（F0..F3）读敌人 +0x1474/+0x1484 起 4+4 槽（`00420380.c` / `00420d00.c` 的 case 0xffffd8ff..0xffffd906）；BI0../BF0..（-9943..-9936）读 BOSS 槽敌人（mgr+0x5c 指向者）同偏移。调用实参本身写入被调线程栈帧（0x477030 参数拷贝循环），敌人槽由建敌指令族填充。

### 2.3 时间模型

- 每帧：解释器 `do { 执行 } while (下一指令.time <= time)`；循环退出后 `time += elapsed`（elapsed = 帧步长，通常 1.0，由 `*(*(enemy+0x2C4))` 提供，0x41a8a0 调 0x478db0 时传入）。
- 文件里指令 time 恒 0 ⇒ 顺序执行到 wait 为止；`wait(n)`/`waitf(f)`（opcode 23/24）做 `time -= n`，后续 time=0 的指令要等时钟追平，实现相对等待。
- 跳转（12/13/14）带第二参数：taken 时 `time = 参数1`（绝对重置，dump 的 `@ N`）、`Q += 参数0`（相对）。
- 帧计数类全局变量（TIME -9988 等）来自 mgr/敌人字段； EnemyCtrl mgr 的帧槽在 +0x98（上帧）/+0x9C（本帧）/+0xA0（浮点累计）（`0041ac70.c:30-41`）。

### 2.4 全局变量（gvar）分发

取参解析出的全局码经宿主敌人虚表调用（**EnemyInf vtable 基 0x4aac14**；RTTI：vtable 前 COL@0x4aac10 → TypeDescriptor `.?AVEnemyInf@@`，EXE 字节直读实证。构造 0x418f40:11 先写基类 SptInf 虚表 0x4aabf8，:21 覆写为 0x4aac14，故运行期 vptr 恒为 0x4aac14）：

| 虚表槽（自 vptr 0x4aac14） | 地址 | 作用 |
|---|---|---|
| +0x0 | 0x41b250→0x41b260 | 游戏指令分发（本文 §4.2；解释器 `(*(void**)enemy)[0]`，004772a0.c:82） |
| +0x4 | 0x420380 | 全局码取 int（00478e60.c:19 即 `vptr+4` 调用） |
| +0x8 | 0x420b80 | 全局码取 int 槽地址 |
| +0xC | 0x420d00 | 全局码取 float |
| +0x10 | 0x421670 | 全局码取 float 槽地址 |
| +0x14 | 0x417d20 | deleting dtor（0x418e00 销毁支路用，oracle-entry §四.1） |

> **勘误（身份更正）**：0x4aabf8 不是 EnemyInf 虚表，而是基类 **SptInf** 虚表（RTTI `.?AVSptInf@@`，六槽 0x4174d0/0x417660/0x417670/0x417680/0x417690/0x417800 全为 ret 桩，仅构造首写时短暂生效）。若按旧表的"+0x1C/+0x20…"槽位读**运行期 vptr**，`0x4aac14+0x1C=0x4aac30` 会落到 EclResourceInf 虚表（首槽 0x4795b0 SCPT 解析）取错函数；探针与 AddressMap 登记一律以本表 0x4aac14 基址 +0/+4/+8/+0xC/+0x10 为准。与 oracle-entry.md §二（EnemyInf 0x4aac14）一致。

0x420380/0x420d00 内 switch 的 case 值 = eclmap `!gvar_names` 码值（-10000 RAND→case 0xffffd8f0 调 0x472c90 = rand()&0x7fffffff，已对 -10000 首项验证）。eclmap 的 gvar 表（-10000..-9904）可直接用作实现清单。

---

## 3. 线程与时间片模型

### 3.1 每帧驱动顺序（0x41a8a0，敌人 update；0x80000 防重入）

1. 0x419450：运动积分（各插值 mover、ANM 朝向同步、出界自杀判定）——**先于** ECL。
2. 0x478db0(elapsed)：遍历线程链表逐线程跑 0x4772a0；线程跑完（Q 空）则释放并摘链。
3. `enemy+0x40E8()` / `enemy+0x40F0()`：funcSet（opcode 632）挂接的每帧回调。
4. 0x419a90：死亡/掉落/结算路径（内部也会经 0x478db0 跑"死亡标签"线程）。
5. 0x41a3d0：雾化圈特效；随后 16 路 ANM 槽坐标同步（enemy+0x120 数组）。

⇒ ECL 里本帧写入的位置/速度类字段**下帧**才被积分消费。移植时保持该顺序。

### 3.1a 敌机链节拍（0x418e00，mgr 逐帧遍历；oracle 逐帧探针实证）

- 遍历游标在**每台敌机处理前**取其实时 next（尾插不改既有 next）——创建者是链尾时，
  其新建敌机本帧不进遍历；非尾创建者的新敌机同帧到达（空转一趟）。
- 0x41a8a0 以 `vm+0x4030` 的 0x80000 位自门：干活置位；0x418e00 成功趟尾清位。
  构造趟（0x4183f0:65）置位后，首个到达的 mgr 更新趟对该敌机空转——root 的构造在
  mgr 更新之外，故 frame 1 全空转（time 序 1,1,2,3）。
- 0x41a8a0 返回 -1（死亡/主线程结束）当场摘链销毁（不耗 RNG）；0x4000000 位敌机
  直接走摘链支路。构造趟内 enmCreate 的后代先入链（0x4183f0:106 挂链在构造趟之后），
  链序与创建序不同——stage1 首帧即 [MapleEnemy, root]。

### 3.1b 圆周插值角维（408/409，oracle 逐帧实证）

- moveCircleTime（409）插值结构的角维 from=to=**0.0**（0x41c627 fldz）；mover mode
  非 2/3 时 0x419450 每帧把该 0 写回 vel.angle（408 设置的角会被归零）。
- mover mode 为 2/3 时插值角维不写回（0x419450:38-43），angle 仅由 0x4736a0 圆周推进。
- 极坐标插值的 elapsed 为 **float**、每帧 +1.0（0x4234b0 头部 0x401400 递增）。

### 3.2 解释器主循环（0x4772a0，6.5KB/125 调用）

```
step(thread, elapsed):
  if Q == null: return 结束
  while Q.time <= time:
    if (Q.rank_mask & thread.difficulty) == 0: 跳过本指令（仅前移 Q += Q.length）
    op = Q.opcode
    if op > 93:                       # 游戏指令
        r = enemy->vt[0](vm)          # EnemyInf slot+0 = 0x41b250 thunk → 0x41b260
        r == -1 → 线程终止
        r == 1  → 本帧让出（不前移 Q，下帧重取）——th15 语义"re-read"
        else    → 前移
    else: 按 §4.1 基础表执行；栈机指令执行后按 Q.stack_adjust 扣 SP
  time += elapsed
  步进 8 条插值通道（duration != 0 时以通道挂接指令为取参上下文向目标变量写值）
```

无指令条数预算（th15 加了 100000 步保险，建议移植保留）。循环体由脚本中的 wait 保证终止；纯 `goto` 环会挂死，属脚本责任。

### 3.3 线程派生与回收

- `callAsync`（15）：0x4793e0 分配 0x11C4 线程 + 0x10 节点，**头插**线程链表；难度字节拷自主线程；新线程 time=0、入口按名字解析（0x477030 → 0x4797d0）。当帧已开始的遍历不会执行它（遍历游标先取好，th15 `EclThreads::step` 同款"新线程下帧参战"）。
- `callAsyncId`（16）：同上，线程 id 取自名字后的内联参数（查找按 id，0x479480）。
- 同步 `call`（11）：0x477030 在**同一线程**压帧：保存 {resume SP, time, Q, routine} 到栈区，切入被调者；`ret`（10）弹帧恢复；栈底帧返回 = 线程结束（0x4772a0 case 10 与 th15 `return_from_call` 一致）。
- killAsync(17)/pauseAsync(18)/resumeAsync(19)/setThreadState(20)/killAllAsync(21)：按 id 或全体置 Q=null / 置 +0x11C0 bit0 / 写 +0x1018（004772a0 case 0x11-0x15）。
- 线程结束即释放节点（0x478db0 内 free）。

### 3.4 难度过滤

指令级 rank_mask 与线程难度字节按位与为零则跳过（004772a0.c:78）。`!HL`=0xcc、`!L`=0xc8 已对 st01 字节验证。callAsync 派生线程继承调用者主线程难度。

---

## 4. 指令语义表

### 4.1 基础指令（opcode 0–93，解释器 0x4772a0 内联实现；已逐 case 读过反编译）

| op | 名称 | 语义 |
|---|---|---|
| 0 | nop | 前移 |
| 1 | delete | 线程立即终止（P+4=null，返回 -1） |
| 10 | ret | 弹调用帧恢复 {SP,time,Q}；栈底则线程结束 |
| 11 | call | 同步调用（按名字）；Q.stack_adjust 清 0 |
| 12 | jmp | Q += Δ（相对）；time = 第二参数 |
| 13 | jmpEq | 弹条件，==0 跳转（同上双参数） |
| 14 | jmpNeq | 弹条件，!=0 跳转 |
| 15 | callAsync | 派生匿名线程（按名字 + 可选实参描述符） |
| 16 | callAsyncId | 派生带 id 线程 |
| 17 | killAsync | 按 id 置线程 Q=null |
| 18 | pauseAsync | 置 +0x11C0 bit0 |
| 19 | resumeAsync | 清 bit0 |
| 20 | setThreadState | 目标线程 +0x1018 = 值 |
| 21 | killAllAsync | 全部异步线程 Q=null（0x4794b0） |
| 22/30/31 | nop 变体 | 前移 |
| 23 | wait | time -= int 参数 |
| 24 | waitf | time -= float 参数 |
| 40 | stackAlloc | SP += S（上限 0xFFC），压旧 FP，FP=旧 SP（0x479a20） |
| 41 | stackFree | 弹 FP（0x479a60；eclmap 缺名） |
| 42 | pushi | 解析参数压栈（类型 'i'） |
| 43 | seti | 弹值写参数 0 目标槽 |
| 44 | pushf | 压栈（'f'） |
| 45 | setf | 弹值写 float 目标槽 |
| 50/51 | addi/addf | 弹两次运算压回（下同） |
| 52/53 | subi/subf | |
| 54/55 | muli/mulf | |
| 56/57 | divi/divf | 整除/浮点除 |
| 58 | modi | 取模 |
| 59–62 | eqi/eqf/neqi/neqf | 比较压 0/1 |
| 63–70 | less/leq/greater/geq ×i/f | |
| 71/72 | noti/notf | 逻辑非 |
| 73/74 | or/and | 逻辑或/与 |
| 75/76/77 | xor/or/and | 位运算 |
| 78 | postDec | 取参数 0 值压栈，目标写回 值-1（`$A--`） |
| 79/80 | sin/cos | 0x4978c0/0x4979f0 |
| 81 | circlePos | 参数 2/3 = 角度/半径 → 目标 0/1 写 cosθ·r, sinθ·r（0x479540） |
| 82 | validRad | 角度归一到 ±π 写目标 0 |
| 83/84 | negi/negf | 取负 |
| 85/86 | squareSum/squareSumRoot | 参数 1/2 平方和 / 开方 |
| 87 | getAngle | 参数 1..4 两点 atan2 写目标 0（0x4977aa） |
| 88 | stackSqrt | 弹栈开方压回 |
| 89 | linearFunc | 角差 wrap ±π（0x4794d0）写目标 0 |
| 90 | pointRotate | 参数 2/3 向量按参数 4 角度旋转写目标 0/1 |
| 91 | interpolate | 插值通道：参数 0=槽号，2=帧数，3=模式，4/5=起止；目标 1 写起始值；挂接本指令（eclmap 误名 floatTime） |
| 92 | interpolateHermite | 同上 + 参数 6/7 控制点（eclmap 误名 math92） |
| 93 | randomPolar | 参数 2/3=半径范围；两个 rand（0x472e10）合成环内随机点写目标 0/1 |

### 4.2 游戏指令（opcode 300–1003，经 EnemyInf 虚表槽 +0（thunk 0x41b250）进入 0x41b260）

分发结构：`0x41b260` 以 opcode-300 查字节表（0x41f9d8，704 项）得密集 case 号，再查跳转表（0x41f760，158 项）进处理块；范围外（<300 或 >1003 或 336-399 等空档）落 0x41f738 直接返回。**完整 锚点↔opcode 对照表如下（由 oracle 读表生成，处理块地址均在 0x41b260 体内）：**

| 处理块 | opcode | 处理块 | opcode |
|---|---|---|---|
| 0x41b2dd | 309-312 | 0x41d573 | 600 |
| 0x41b2ec | 300-301,304-305,321 | 0x41d61a | 614 |
| 0x41b2f7 | 302 | 0x41d6b1 | 601 |
| 0x41b30c | 424 | 0x41d7ad | 602 |
| 0x41b32f | 631 | 0x41d7ed | 603 |
| 0x41b34f | 552 | 0x41d837 | 626 |
| 0x41b364 | 553 | 0x41d8ac | 627 |
| 0x41b379 | 638 | 0x41d8e2 | 628 |
| 0x41b3a2 | 303 | 0x41d954 | 604 |
| 0x41b3ae | 320 | 0x41d9a2 | 605 |
| 0x41b400 | 319,325-333,335 | 0x41d9f0 | 606 |
| 0x41b40c | 323 | 0x41da32 | 624 |
| 0x41b437 | 561 | 0x41dad7 | 625 |
| 0x41b4de | 322 | 0x41db66 | 617 |
| 0x41b506 | 554 | 0x41dbdf | 618 |
| 0x41b510 | 307 | 0x41dc79 | 619 |
| 0x41b5f5 | 314 | 0x41dd49 | 620 |
| 0x41b63e | 308 | 0x41ddf7 | 621 |
| 0x41b67a | 315 | 0x41de70 | 622 |
| 0x41b7ac | 334 | 0x41df20 | 607 |
| 0x41b7cc | 306 | 0x41df50 | 608 |
| 0x41b897 | 318 | 0x41df90 | 609-612 |
| 0x41b903 | 313 | 0x41e135 | 640 |
| 0x41b98f | 316 | 0x41e16c | 700 |
| 0x41b9e7 | 317 | 0x41e1ec | 701 |
| 0x41ba18 | 427 | 0x41e262 | 514 |
| 0x41bb0d | 400,402 | 0x41e33f | 521 |
| 0x41bbce | 416-417 | 0x41e360 | 556 |
| 0x41bc48 | 401,403,436-437 | 0x41e36e | 613 |
| 0x41bdf4 | 434-435,438-439 | 0x41e382 | 516 |
| 0x41bfb1 | 425-426 | 0x41e3a3 | 517 |
| 0x41c137 | 404,406,428,430 | 0x41e3da | 518 |
| 0x41c21f | 405,407,429,431 | 0x41e3fd | 525 |
| 0x41c471 | 408,410 | 0x41e407 | 519 |
| 0x41c57c | 409,411 | 0x41e42e | 520 |
| 0x41c791 | 418-419 | 0x41e451 | 558 |
| 0x41c80c | 420,422 | 0x41e473 | 522,528,537-539 |
| 0x41c990 | 421,423 | 0x41e570 | 513 |
| 0x41cca8 | 414 | 0x41e582 | 523 |
| 0x41ccda | 415 | 0x41e593 | 542 |
| 0x41cd0c | 432 | 0x41e5a1 | 543 |
| 0x41cd1f | 433 | 0x41e5ab | 544 |
| 0x41cd32 | 504 | 0x41e5cd | 702 |
| 0x41cd9c | 505 | 0x41e77a | 703 |
| 0x41cdab | 526 | 0x41e96d | 713 |
| 0x41cdc2 | 412-413 | 0x41eaf3 | 711 |
| 0x41d0c2 | 527 | 0x41ec89 | 710 |
| 0x41d137 | 632 | 0x41ecd1 | 704 |
| 0x41d15d | 639 | 0x41ed2b | 705 |
| 0x41d183 | 633 | 0x41ed7b | 706 |
| 0x41d19f | 634 | 0x41eda9 | 707 |
| 0x41d1bb | 637 | 0x41edd7 | 708 |
| 0x41d1d5 | 500 | 0x41ee05 | 709 |
| 0x41d203 | 501 | 0x41ee36 | 714 |
| 0x41d231 | 502 | 0x41ee6b | 615 |
| 0x41d26a | 503 | 0x41eea1 | 712 |
| 0x41d2a5 | 506 | 0x41eee3 | 616 |
| 0x41d2bd | 507 | 0x41ef19 | 635 |
| 0x41d2e0 | 508 | 0x41ef3e | 636 |
| 0x41d316 | 509 | 0x41ef74 | 524 |
| 0x41d325 | 562 | 0x41ef8e | 529 |
| 0x41d338 | 510 | 0x41efd8 | 530 |
| 0x41d34c | 511 | 0x41f057 | 531 |
| 0x41d395 | 512 | 0x41f0b3 | 532 |
| 0x41d3f1 | 515 | 0x41f0f9 | 533 |
| 0x41d40b | 541 | 0x41f172 | 534 |
| 0x41d425 | 551 | 0x41f1ba | 540 |
| 0x41d439 | 535 | 0x41f1d4 | 545 |
| 0x41d4bc | 536 | 0x41f1de | 546 |
| 0x41f228 | 559 | 0x41f6a1 | 1000 |
| 0x41f242 | 547 | 0x41f6c8 | 1001 |
| 0x41f256 | 623 | 0x41f6d9 | 1002 |
| 0x41f2c4 | 548 | 0x41f6ea | 560 |
| 0x41f344 | 629 | 0x41f71d | 1003 |
| 0x41f3f6 | 630 | 0x41f516 | 801 |
| 0x41f40b | 557 | 0x41f565 | 802 |
| 0x41f4c0 | 549 | 0x41f5e9 | 800 |
| 0x41f4df | 550 | 0x41f4f3 | 555 |
| 0x41f61e | 324 | 0x41f738 | 其余（空档 no-op） |

st01.ecl 使用集（86 op）、default.ecl（17）、st01bs.ecl（65）已列于 dumps；三文件并集即下表覆盖范围。语义列标 ★ 的已逐一读过对应反编译块；标 ◇ 的有处理块锚点 + eclmap 名 + th15 同族对照，实现期按锚点核对。

#### 300 族（敌人/ANM）

| op | 名称 | 语义 |
|---|---|---|
| 300/301/304/305/321 | enmCreate 族 ★ | 0x41fcc0：解析坐标（300/309/321 相对当前敌人 +0x44/+0x48；304/311/312 带镜像标志，受敌人 mirror 位 0x100000 翻转 x）；拷贝 12 dword ANM 状态（vm+0x288）作遗产；0x4183f0(名字,{pos,…}) 按名建敌。带 A 后缀 = 继承 ANM 变换 |
| 302 | anmSelect ★ | vm+0x98 = ANM 文件槽号（mgr+0x80+id*4 文件表） |
| 303 | anmSetSprite ★ | 0x41fe80(槽,精灵)：16 路 ANM 槽 vm+0x120+slot*4；slot0=主显示，更新 hurtbox 尺寸（vm+0x3f30/0x3f34） |
| 306 | anmSetMain ★ | 0x41b7cc：按 vm+0x98 文件 + 精灵重建 slot0 AnmVM；置 vm+0x100C bit0x200000；记录 life 相关基址 |
| 307 | anmPlay ★ | 0x41b510：对 slot0 以当前坐标新建 AnmVM（0x4243e0 系），含打断旧 VM（0x470110 停旧、0x4706c0 取新） |
| 308 | anmPlayAbs ★ | 0x41b63e：同 307 但用绝对坐标（0x421b10(0)=槽） |
| 309-312 | enmCreateF 族 ★ | 0x41b2dd：mgr+0x5c BOSS 槽空时才建（转 0x41fcc0） |
| 313 | anmSelectedPlay ★ | 0x41b903：在选定文件上按精灵新建 VM 写入参数 0 指定槽 |
| 316 | anm316 ★ | 0x41b98f（313 的换文件变体，slot 参数化） |
| 319,325-335 | anm 属性插值族 ★(处理块共享) | 0x41b400：ANM VM 字段 {时长,模式,起,止} 插值器（color/alpha/scale/角度等，eclmap 名为准）；time 后缀变体即时置终值 |

#### 400 族（运动）

通用记号：-999999 参数 = 保持当前值；坐标 mover 挂 vm+0x33（3D），速度 mover 挂 vm+0x22；`*Time` 变体配 FUN_00423450/0x41bc48 插值；所有 move 结束调 0x419950 刷新碰撞/派生位置。

| op | 名称 | 语义 |
|---|---|---|
| 400/402 | movePos(±) ★ | 0x41bb0d：直接设定位置 mover 目标（402 相对） |
| 401/403 | movePosTime ★ | 0x41bc48：{时长,模式,x,y,(z)} 插值移动 |
| 404/406/428/430 | moveVel(±NM) ★ | 0x41c137：设定速度矢量（角度+速率） |
| 405/407/429/431 | moveVelTime ★ | 0x41c21f：{时长,模式,角度,速率} 插值；429/431 = NM（No-Mirror，不受镜像位改写角度——处理块内 0x194/0x196 比较分支） |
| 408/410 | moveCircle ★ | 0x41c471：角速度圆周（记录旧坐标、置 mover 模式 2） |
| 409/411 | moveCircleTime ★ | 0x41c57c：定时圆弧插值 |
| 412/413 | moveRand(Rel) ★ | 0x41cdc2：按移动界限三带选随机方向（左/右缘 120° 扇、中带 90° 扇按玩家侧加 π 门），尾块 from=(angle,speed)→to=(angle,0) 直冲减速到停（0x41d0b9 清模式低 4 位）；逐块实证见 event-contract.md §7.5 |
| 414/415 | moveBoss(Rel) ★ | 0x41cca8/0x41ccda：坐标 = BOSS 槽敌人 +0x1230 位置 |
| 416/417 | movePos3d(Rel) ★ | 0x41bbce：位置 +=（含 z） |
| 418/419 | move419 ★ | 0x41c791：速度 {x,y,(z)} 直设 |
| 420-423 | moveEllipse 族 ★ | 0x41c80c/0x41c990：椭圆轨道（中心、半径、角速度/时长） |
| 424 | moveSetMirror ★ | 0x41b30c：翻转 vm+0x100C bit0x100000（镜像位；影响后续建敌与角度改写） |
| 425/426 | moveBezier(Rel) ★ | 0x41bfb1：贝塞尔 mover（vm+0xBC/0xD6 控制点组，0x41bc48 插值） |
| 427 | moveReset ★ | 0x41ba18：清全部运动 mover 与累积量，速度归 (0,0,0) |
| 432/433 | moveEnm(Rel) ★ | 0x41cd0c/0x41cd1f：按敌人 id（0x417f40 查 mgr 表）复制其 +0x1230 坐标为本 mover 目标 |
| 434-439 | moveCurve 族 ★ | 0x41bdf4：曲线（角速度+速率的合成 mover） |

#### 500 族（敌人属性/关卡事件）

| op | 名称 | 语义 |
|---|---|---|
| 500 | setHurtbox ★ | 0x41d1d5：受击框半宽/半高（vm+0x110/0x114 一带） |
| 501 | setHitbox ★ | 0x41d203：判定框（对玩家碰撞） |
| 502/503 | flagSet/Clear ★ | 0x41d231/0x41d26a：vm+0x100C 按位或/与，随后 16 路 ANM 槽重绑/解绑 |
| 504 | moveLimit ★ | 0x41cd32：vm+0xFCE/0xFCF 中心 ±vm+0xFD0/0xFD1 半宽的活动界限（0x419450 出界自杀用），置 0x40000 位 |
| 505 | moveLimitReset ★ | 0x41cd9c：清 0x40000 位 |
| 506-510 | drop 族 ★ | 掉落相关：506 dropClear（清 0x3C 字节掉落态）；507 dropExtra(槽,值)；508 dropArea(中心/范围)；509 dropItems(0x417ad0 重算掉落)；510 dropMain(主物品 vm+0xFDA) |
| 511 | lifeSet ★ | 0x41d34c：生命 ×4 槽（vm+0xFD3..0xFD6=值*7 星数显示） |
| 512 | setBoss ★ | 0x41d395：注册 BOSS 槽（mgr+0x5c+id*4 = T），清 mgr+0x7C bit0，负参撤销并清槽 |
| 513 | timerReset ★ | 0x41e570（case 0x33 → 尾块）：FUN_004016c0(0) 把 boss 计时器组清零 |
| 514 | setInterrupt ★ | 0x41e262：登记中断跳转 {id, 优先级} 到 T+0x5234 表；BOSS 槽敌人额外登记 "BossDead"/"BossEscape" 标签（0x421880） |
| 515 | setInvuln ★ | 0x41d3f1：FUN_004016c0(id) 初始化无敌倒计时器组 |
| 516 | playSound ★ | 0x41e382：FUN_00460c50(声音 id)（死亡路径 0x41aad0 同款调用） |
| 517 | setScreenShake ★ | 0x41e3a3：FUN_0045fa60(1, id, 强度…) 震屏登记 |
| 518 | dialogRead ★ | 0x41e3da：FUN_00428820(对话管理器, id) + 0x411df0/0x430300；落入 525 处理（清场+全体中断 0x41ac70） |
| 519 | dialogWait ★ | 0x41e407：对话未结束则本帧让出（返回重取，即"等待"实现） |
| 520 | bossWait ★ | 0x41e42e：mgr+0x5c/0x60/0x64 任一 BOSS 存活则让出 |
| 521 | setTimeout ★ | 0x41e33f：T+(id+0x524)*0x10 表项存本指令参数区指针（超时回调登记） |
| 522/528/537-539 | spell 族 ★ | 0x41e473：符卡开始：xor 加密符卡名（0x77,7,16 解密）、rank 修正（0x219-0x21b 分支）、对话/弹幕环境登记，汇入 boss 计时复位（caseD_33 尾块 0x4016c0(0)） |
| 523 | spellEnd ★ | 0x41e582：0x413a60 结束符卡 |
| 524 | setChapter ★ | 0x41ef74 |
| 525 | enmKillAll ★ | 0x41e3fd：0x41ac70 全体敌人中断跳转 |
| 527 | lifeMarker ★ | 0x41d0c2：写 boss 血条显示阵（DAT_004c2190+0x1BC+id*8 起的生命/显示比例） |
| 535/536 | diffI/diffF ★ | 0x41d439/0x41d4bc：按 DAT_004be7cc（难度）选第 1..4 参写入目标 |
| 540 | stars ★ | 0x41f1ba |
| 541 | noHitboxDur ★ | 0x41d40b |
| 542 | spellTimeout ★ | 0x41e593 |
| 545 | laserCancel ★ | 0x41f1d4：0x430220 |
| 547 | bombShield ★ | 0x41f242 |
| 548 | diffWait ★ | 0x41f2c4：`thread.time -= 按难度选的第 N 参数`（难度加权等待） |
| 554 | stageLogo ★ | 0x41b506（case 0x58）：FUN_0042adc0 全屏关卡 logo |
| 555/556 | enmAlive/setDeath ★ | 0x41f4f3（按 id 查敌存活写目标）/0x41e360 |
| 557 | fogTime ★ | 0x41f40b（case 0x5b）：{id, 时长, 打包 RGBA 色（参数 2 四字节）} → 0x4099a0 + 0x405f00 雾化圈 |
| 559 | enmLimit ★ | 0x41f228：mgr+0xC4 = 帧上限 |
| 560/561 | unknown560/die ★ | 0x41f6ea / 0x41b437（case 0x5f：FUN_00460c50 音效 + 0x4244e0 朝向 + 0x46f820 死亡炸裂环弹） |
| 562 | dropItemsSp ★ | 0x41d325 |

#### 600 族（弹幕发射器 et）

模型：每敌人 8 路"发射器槽"，槽状态 0xE0 字节位于 vm+0x15C+slot*0xE0（600 etNew 清零 + 初始化常量）；出生点 vm+0xF6C+slot*3；发射标志 vm+0xF9C+slot*3（<0 走绝对模式）。601 etOn（0x41d6b1）按槽状态合成出生参数调 0x40ffe0 真正发弹。

| op | 名称 | 语义 |
|---|---|---|
| 600 | etNew ★ | 0x41d573：清槽、置默认（速率 2.0 等） |
| 601 | etOn ★ | 0x41d6b1：发射一批 |
| 602 | etSprite ★ | 0x41d7ad：弹 sprite（槽+0x15C/0x15D） |
| 603 | etOffset ★ | 0x41d7ed：出生点偏移（vm+0xF6C+slot*3） |
| 604/605 | etAngle/etSpeed ★ | 0x41d954/0x41d9a2：基础角度/速率（+0x163/+0x164） |
| 606 | etCount ★ | 0x41d9f0：发弹数与间隔（+0x238/+0x239） |
| 607 | etAim ★ | 0x41df20：对玩家瞄准开关 |
| 608 | etSound ★ | 0x41df50：发射音（0x263/0x264 变体读内联发射次数） |
| 609-612 | etEx(Set/2) 族 ★ | 0x41df90：多值扩展参数块（角速度/加速度/延迟等打包入 0x5A8.. 区） |
| 613 | etClearAll ★ | 0x41e36e |
| 614 | etCopy ★ | 0x41d61a：槽间复制 0xE0 状态 |
| 615/616 | etCancel/etClear ★ | 0x41ee6b/0x41eee3：0x4114f0(pos, 1/0) |
| 617-622 | etSpeedR/etCountR 族 ★ | 0x41db66..0x41de70：按 rank（DAT_004be7cc）索引选取 3/5/2 路参数写速率/数目（case 0x6f-0x74 均为难度选参结构） |
| 624/625 | etSpeedD/etCountD ★ | 0x41da32/0x41dad7：四难度参数选写 +0x163/速率、+0x235/数目 |
| 626/627/628 | etOffsetRad/etDist/etOffsetAbs ★ | 0x41d837/0x41d8ac/0x41d8e2：出生点极坐标/距离/绝对 |
| 629 | fog ★ | 0x41f344：屏幕雾对象（0x18 字节分配 + {中心,半径,速率,颜色}） |
| 630 | interruptSTD ★ | 0x41f3f6 → 0x409100：触发 STD 相机脚本 ins_16 标号中断（扫描 [stage+0x1c] 找 opcode==16 且 arg0==id 的标号，游标跳标号后首指令、计时三槽设为目标 time；旧记 "callSTD 调用 default.ecl 函数" 有误，capstone 更正） |
| 631 | lifeHide ★ | 0x41b32f |
| 632 | funcSet ★ | 0x41d137：置 mgr+0x7C bit0 并登记每帧回调（enemy+0x40E8 槽被 0x41a8a0 调用） |
| 633/634 | flagExtDmg/setHitboxFunc ◇ | 0x41d183/0x41d19f |
| 635/636 | etCancel2/etClear2 ◇ | 0x41ef19/0x41ef3e |
| 637 | funcCall ★ | 0x41d1bb：原生函数表调用（DAT_004bb9fc 索引） |
| 638 | scoreAdd ★ | 0x41b379 |
| 640 | etExSub ◇ | 0x41e135 |

#### 700 族（激光）

| op | 名称 | 语义 |
|---|---|---|
| 700 | laserNew ★ | 0x41e16c：按 id 建/取激光对象（0x418300），写宽度/角度参数 |
| 701 | laserTiming ★ | 0x41e1ec：{x,y,z}+定时参数 |
| 702 | laserOn ★ | 0x41e5cd：按槽状态合成激光弹（0x42fed0） |
| 703 | laserStOn ★ | 0x41e77a：以槽状态合成激光弹（0x418290 + 0x42fed0(0)） |
| 704-709 | laserSt 族 ◇ | 0x41ecd1..0x41ee05：St 变体（Offset/Trajectory/Length/Width/Angle/Rotation/End） |
| 710/711 | laserStEnd/laserCuOn ◇ | 0x41ec89/0x41eaf3 |
| 712 | etCancelRect ★ | 0x41eea1（case 0x93）：按 id 解除并 0x4119c0 改写位置 |
| 713/714 | unknown ◇ | 0x41e96d/0x41ee36 |

#### 800/900/1000 族

| op | 名称 | 语义 |
|---|---|---|
| 800 | enmRestart ★ | 0x41f5e9（case 0x96）：按 id 取敌（0x417f40；id 0/空表走默认尾）后 0x4176c0 释放其异步子线程链 + 0x417740 复位 ECL 上下文 + 0x479a90 用指令 +0x18 内联例程名经 0x4797d0 查号重挂（例程→[ctx+4]、time=0）；st04bs Boss 相位推进 11 处（旧记 "enmCall 调原生方法" 有误，capstone 更正） |
| 801 | enmPos ★ | 0x41f516：按 id 设置敌人位置（+0x50/+0x54/+0x58） |
| 802 | broadcastInterrupt ◇ | 0x41f565 |
| 900 | debug900 ★（空操作） | **落默认分支 0x41f738**：字节表 bt[600]=0x9d（oracle 直读验证）；st01 的 `ins_900(4)` 在原版即 no-op，eclmap 的 "900 debug900" 在本构建未接线 |
| 1000 | spec0 ★ | 0x41f6a1（case 0x99）：FUN_00438ec0 特效 |
| 1001/1002/1003 | spec1-3 ★ | 0x41f6c8/0x41f6d9/0x41f71d：写 vm+0xFFD / vm+0xFFE / vm+0x100D bit1 |

---

## 5. 原版函数锚点总表

| 地址 | 角色 |
|---|---|
| 0x41b260 | 游戏指令分发（17.6KB；EnemyInf 虚表槽 +0 经 0x41b250 进入；跳转表 0x41f760/字节表 0x41f9d8） |
| 0x41b250 | `enemy->vt[0]`（EnemyInf slot0）thunk：调 0x41b260(enemy VM 视图 = enemy+0x11EC) |
| 0x4772a0 | ECL 基础解释器（线程 step；opcode≤93 内联，>93 转虚表） |
| 0x477030 | call/callAsync 帧压入与实参拷贝 |
| 0x4793e0 | 异步线程分配（0x11C4）+ 节点链入 |
| 0x478db0 | 线程链表遍历器（每帧跑线程、回收死者） |
| 0x4797d0 | 子程序名字二分查找（表在 T+0x8C） |
| 0x4795b0 | SCPT 注册/label 表登记（EclResource 侧） |
| 0x478e60 / 0x478ee0 | 整型/浮点取参（mask 位 + 变量/全局/表达式三路解析） |
| 0x479130/0x479020/0x479260/0x479330/0x478fb0/0x4791f0/0x4790c0/0x479300 | 同族取参/取地址变体 |
| 0x479a20 / 0x479a60 | stackAlloc / stackFree |
| 0x4794b0 | killAllAsync |
| 0x479480 | 按 id 找线程节点 |
| 0x420380 / 0x420d00 | 全局变量取值（int / float；case 值 = eclmap gvar 码） |
| 0x420b80 / 0x421670 | 全局变量取槽地址（int / float） |
| 0x41a8a0 | 敌人每帧 update 总控（运动→ECL→funcSet→死亡→雾→ANM 同步） |
| 0x419450 | 运动积分/出界判定 |
| 0x419950 | move 族收尾的位置刷新 |
| 0x418f40 | Enemy 构造（:11 先写基类 SptInf 虚表 0x4aabf8、:21 覆写为 EnemyInf 0x4aac14；主线程建立、0x1474+ 参数槽清零） |
| 0x419320 | Enemy 析构（摘链、清 mgr 槽） |
| 0x41fcc0 / 0x41fe80 / 0x41ffb0 | enmCreate / anmSetSprite / case 0xd 处理块 |
| 0x472c90 / 0x472de0 / 0x472e10 | rand() / RANDF / 随机浮点（RNG=MSVC CRT，见 DECOMP.md 0x47BBA2） |
| DAT_004c2188 | EnemyCtrl 模块全局（+0x5C BOSS/敌人槽表，+0x80 ANM 文件表，+0xB0 敌人链头，+0xB8 计数） |
| DAT_004be7cc / DAT_004be7c4 | 当前难度（rank）/ 机体 shot type |
| 0x4aac14 | EnemyInf vtable 基址（RTTI `.?AVEnemyInf@@`；slot+0/+4/+8/+0xC/+0x10/+0x14 = 0x41b250/0x420380/0x420b80/0x420d00/0x421670/0x417d20，即 §2.4 表） |
| 0x4aabf8 | SptInf 基类虚表（RTTI `.?AVSptInf@@`；构造首写、运行期被覆写，六槽 0x4174d0..0x417800 全桩。**勿用作 EnemyInf 基址**） |

---

## 6. th15 风格 C++ 模块拆分（th13 版）

沿用 th15 五模块 + 指令实现回调，文件放 `web/cpp/game/`：

```
EclResource.{hpp,cpp}   SCPT 解析：EclInstruction(16B 头)/EclSubroutine/子程序名表/ANIM+ECLI 列表。
                        th13 与 th15 同构，直接采用 th15 EclResource::open 的解析骨架
                        （§1.1 布局逐字段一致），仅 stack_adjust 按 u32 读。
EclProgram（并入 EclResource.hpp）
                        多文件聚合 + 名字二分（th15 find_index 可复用；插入序=原版 0x4795b0
                        的"首文件顺序 + 后续按名插入"）。
EclContext.{hpp,cpp}    线程状态（§2.1 字段表）；取参/压弹栈/帧切换/难度/8 插值通道。
                        th15 EclContext 去掉 th15 专有字段即得 th13 版；宿主指针 +0x1014
                        以 `EclCommands*`/`EclVariables*` 接口呈现。
EclThreads.{hpp,cpp}    main + children（头插、当帧不执行、step(elapsed) 顺序 main→children、
                        死者摘链）。th15 实现可直接沿用。
EclInterpreter.cpp      step() 主循环（§3.2 伪码）：time 比较、难度门、基础表 0-93、
                        >93 转 EclCommands::execute，返回契约 {0 前移, 1 重取, -1 终止}。
                        保留 th15 的 100000 步预算与错误报告。
EnemyEclCommands.cpp    th13 游戏指令（§4.2 表）按 300/400/500/600/700/800/1000 分文件亦可；
                        每条实现挂 §4.2 锚点地址注释，供 oracle 对照与 AddressMap.hpp 登记。
                        登记虚表锚点时以 §2.4/§5 修正后身份为准：EnemyInf=0x4aac14
                        （slots +0..+0x14）；0x4aabf8 是 SptInf 基类虚表，不得当作
                        EnemyInf 基址回填，否则"读运行期 vptr 加槽位"类探针会取错函数。
EclFile.{hpp,cpp}       （oracle 对照用）线程快照落盘/恢复；块大小改 th13 的 0x11C4，
                        字段序按 §2.1。
```

与 th15 的行为差异清单（实现时逐条对齐）：

1. 指令记录 +0x0C：th13 为 u32 stack_adjust，th15 为 u8+reserved。
2. th13 无 th15 的 91/92 之外的通道语义差异；通道槽区 0x1020..0x11C0 与 th15 0x1024.. 一致，仅基址差 4。
3. gvar 集合不同：以 `artifacts/reference/eclmap/th13.eclm` 的 !gvar_names 为准（-10000..-9904），th15 表不能直接复用。
4. th13 的 I0-I3/F0-F3 读宿主敌人槽（§2.2），th15 走局部帧——移植按 th13 行为实现。
5. 时间步长来源：th13 elapsed = `*(*(enemy+0x2C4))`（每敌人，允许慢速个体），th15 为全局插值率——保持 th13 语义。

---

## 7. 风险清单

1. **处理块地址→语义的映射靠密集 case 表还原**（0x41f9d8/0x41f760 由 oracle 读出），表本身是数据不是代码；若后续换 EXE 版本需重跑该脚本。映射与 eclmap 名的对应在 302/500/405/15/23/12/14/44/51/57 等指令上已双验证（raw 字节 + dump 文本 + 反编译三向一致）。
2. **§4.2 中标 ◇ 的指令**只锚定了处理块与名字，语义按 eclmap 名 + th15 同族推断，未逐行读反编译；实现期需按锚点回读核对（涉及 630、633-636、640、704-711、713/714、800、802）。case↔opcode↔处理内容的三方核对表由脚本生成（`artifacts/reference/eclmap/case2ops.json` + 反编译 case 块 join），已消除手工誊写错位。
3. **0x41b260 的返回值**（0/1/-1 契约）在反编译中不可见（Ghidra 判为 void）；1=让出重取的语义由解释器侧（004772a0 caseD_2）与 th15 `EclCommands` 注释共同锁定，519/520 的"等待"实现依赖它，需在 oracle 对照中重点验证。
4. **主线程/异步线程的宿主容器细节**（enemy+0x11D0 哨兵、[enemy+0x52B4] 自引用）有 Ghidra 寄存器追踪歧义：本文化简为"宿主敌人内嵌线程链表"，逐字段偏移仅 §2.1/§2.2 标 ★ 者经过直读验证；移植按模块接口实现，不逐偏移复刻。
5. **同步 call 的实参描述符编码**（名字后 8 字节 {源型,目标型,值}）由 0x477030 反推 + th15 `call_into` 佐证；st01/default/st01bs 的同步调用均无实参，带实参路径（如 default.ecl 的 `I0/F0` 用法）需在扩展面（Stage2+）dump 出现时再核对。
6. **难度加权等待 548** 直接改线程时钟，帧率非 1.0 步长（变速/暂停恢复）下的累计误差需 oracle 对帧验证。
7. eclmap 个别名称与代码事实不符：91/92 误名 floatTime/math92（已按代码语义定名 interpolate/interpolateHermite）；321 enmMapleEnemy 在 th13 处理块与 300 族共享，语义即建敌；**900 debug900 在本构建落默认分支为 no-op**（st01 实际调用之，oracle 读表验证 bt[600]=0x9d）。以本表 ★ 语义与原版代码为准。
