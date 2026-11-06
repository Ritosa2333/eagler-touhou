# 录像（.rpy）格式与回放注入

原版 th13 录像格式的完整逆向结论与移植侧解码/注入实现合同。依据：Ghidra 导出
`artifacts/decomp/jp/functions/*.c`（装载 0x448c40、保存 0x4484d0、XOR 0x472280/0x4723a0、
LZ77 0x458f30、录制 0x4480a0、回放 0x448200、分段分配 0x4494e0）、`games/th13/th13.exe`
反汇编（初始密钥经 AL 寄存器传参，调用点 0x448d50/0x448d67 与 0x448863/0x448879）、
真机录像 `games/replays/th13_udSr01.rpy` 全量解码验证。实现：`web/cpp/game/ReplayFile.*`。

---

## 1. 文件布局（magic "t13r"，版本 2）

| 偏移 | 类型 | 含义 |
|---|---|---|
| +0x00 | char[4] | `"t13r"` |
| +0x04 | u16 | 版本 = 2 |
| +0x0C | u32 | USER 段偏移 = 压缩尺寸 + 0x24（保存侧 0x4484d0:150 恒等式） |
| +0x10 | u32 | 0x100（固定） |
| +0x1C | u32 | 密文字节数（压缩后） |
| +0x20 | u32 | 解压后字节数 |
| +0x24 | … | 密文（压缩尺寸字节） |
| USER 段 | … | `"USER"` + u32 块总长 + u8 类型 + 3 填充 + 文本（可多块链式：首块摘要，次块备注） |

## 2. 加密（两趟，块内交织反转 + 递增 XOR）

单趟变换（0x472280 解密 / 0x4723a0 加密，互为逆、各自单独非自逆）：

- 数据按 `block` 分块；块内前 ⌈n/2⌉ 字节写到（加密：读自）块尾-1、块尾-3、…，
  其后 ⌊n/2⌋ 字节写到（读自）块尾-2、块尾-4、…。
- 每字节 XOR 递增密钥：`k_i = (init + i·step) mod 256`，序号 i 跨块连续、不随块复位。
- 变换区间门槛（iVar4）：`size - (size&1) - tail`，其中 `tail = size%block`（当
  `size%block < block/4`，否则 0）。即奇尺寸末字节不动；小余尾不动，大余尾并入末块变换。

| 趟 | init | step | block | 说明 |
|---|---|---|---|---|
| 保存先 / 装载后 | 0x7D | 0x3A | 0x100 | 调用点 0x448863（存）/0x448d67（载） |
| 保存后 / 装载先 | 0x5C | 0xE1（即 -0x1F） | 0x400 | 调用点 0x448879（存）/0x448d50（载） |

初始密钥 0x5C/0x7D 为硬编码常量（`mov al, imm8`），无按内容派生。

## 3. 压缩（ZUN LZ77，压缩 0x458c30 / 解压 0x458f30）

保存侧 0x4484d0:0x44883d 调 `void* compress(const u8* src, int size, int* out_size)`
（0x47a0ed malloc(size*2)，出口写 *out_size = 输出字节，EAX=缓冲）。实现：
`ReplayFile::deflate`（web/cpp/game/ReplayFile.cpp），规格全部出自 capstone 直读
反汇编（Ghidra 伪代码 00458c30.c 有漏读，不作为依据）；与外层
`oracle/replay_compress.py` 同构。

### 3.1 解码方向位流（0x458f30）

MSB 先行位流；环形缓冲 0x2000、写游标初值 1（与原版全局 0x4da2f8 一致）：

- 字面量：flag `1` + 8 位字节；字节同时写入输出与环。
- 拷贝：flag `0` + 13 位环地址 + 4 位长 n，复制 n+3 字节，读 `ring[(addr+i) & 0x1FFF]`、
  逐字节回填环（重叠复制合法）。
- 终止：拷贝令牌地址 0；输入越界后位读 0，天然终止。

### 3.2 编码方向（0x458c30，bit-exact 规格）

**全局布局（每次调用完全自初始化，.bss 零初值即可）**

- 0x4c22e8 起每环位 12 字节 = 三个 int：dad(0x4c22e8)/lft(0x4c22ec)/rgt(0x4c22f0)，
  共 0x2000 项；下标 8192 的越界"哨兵项"三字段与根变量重合
  （rgt[8192]=0x4da2f0=root）。
- 0x4da2f8：0x2000 字节文本环。入口 memset 环、dad/lft/rgt[0..8191] 全清，
  然后 root=rgt[8192]=1、dad[1]=0x2000（删根经哨兵别名改根）、lft[1]=rgt[1]=0
  （0x458cc5..0x458cdf）。

**环语义（与解压侧严格镜像）**：预入环（0x458c96..0x458cbf）把 n=min(18,size)
个源字节写入 ring[1..n]，wp=1；token 后每步 r=(wp+18)&0x1fff，dad[r]!=0 先删树、
有剩余源则 ring[r]=下一字节（超前 18 预填）否则 lookahead--，wp 推进，lookahead!=0
则 insert(wp)。源内 0xFF 无特殊含义（`movzx; cmp -1` 是死分支，0x458caa/0x458eae）。

**BST（0x459160 insert / 0x4593a0+0x4593f0+0x459440+0x4594d0 delete）**：
Okumura LZSS 型二叉搜索树，按"以 p 起的 18 字节环串"字典序，0=NIL，无哈希链。

- insert(p)：自根（rgt[8192]）下降，每节点顺序比较 ring[(p+k)&m]-ring[(node+k)&m]
  （k=0..17），记录首失配下标 i 与符号 c；**i>=best 含并列更新（后访问者获胜）**；
  i==18 整串相等 → node 被 p 原位顶替（0x4592df：父槽先查 lft 改指 p、
  dad[p]=dad[node]、p 接管两子、dad[node]=0，node 成幽灵）返回 18；否则按 c 走
  lft/rgt，空槽挂 p 返回 best；p==0 直接返回 0（位置 0 永不入树，0x459164）。
- delete(x)（仅当 dad[x]!=0）：rgt[x]==0 用 lft 顶替、lft[x]==0 用 rgt 顶替；
  两子俱全取 v=左子树最右（沿 rgt 下行，0x4594d0），先递归删 v（v 必无右子），
  再把 v 拼到 x 位（0x459440：父槽**先查 lft**）。简单顶替 0x4593f0：父槽
  **先查 rgt** 改 repl。

**编码主循环（0x458cfb..0x458f15）**：当 lookahead>0：ml=min(match_len, lookahead)
（match_len/pos 为最近一次 insert 返回/写出值，首 token 前为 0 → 必为字面量；
贪心无 lazy）。

- ml<=2：字面量 = 1 位 `1` + ring[wp] 8 位（MSB 先行）。
- ml>=3：拷贝 = 1 位 `0` + match_pos 13 位 + (ml-3) 4 位（均 MSB 先行）；
  4 位域 0..15 ↔ 长 3..18。
- 位流 MSB 先行按位打包（掩码 0x80 起右移，字节满即出）；结尾（lookahead==0）
  补 1+13=14 个 0 位（0x458ef2..0x458f15）——解压侧读地址 0 即终止；尾字节剩余
  位为 0 不被读取（空输入输出恰 1 个 0x00：14 位只有首字节满）。

**验证记录**：`oracle/replay_compress.py` 8/8 真 .rpy 逐字节往返 + 35/35 Unicorn
直调向量；移植侧 `tests/replay/deflate_test.cpp`（Unicorn 黄金向量夹具
`deflate_golden.hpp` + 8 份真 .rpy packed 段逐字节 + inflate 互逆 + 自复位）。

### 3.3 保存路径聚合（0x4484d0）

- 顶层 +0x0C u64 time_t：保存确认时刻写入——对话框路径 0x4549d0:84、
  game over 流 0x43fa90:196 / 0x440960:191、中断保存 0x43e780:360 均为
  `__time64(top+0x0C)`；USER Date 行 = 对该值的 localtime。
- 顶层 +0x14 总分：0x4486ea 写 DAT_004be7c0（实时分）。
- 顶层 +0x54 慢速率 f32：0x4486e5..0x448715 按保存时刻 fps 管理器（0x4c218c）
  实时计数器计算 `slow = (float)(100.0 - (double)(float)(num/den) * 100.0)`，
  其中 num=+0x24、den=+0x2c（double），0x424990:37-50 逐秒窗聚合：
  游戏进行中每窗 `den += 60.0`，窗内 fps≤57 时 `num += fps` 否则 `num += 60.0`；
  事件字节（0x4480a0:25-32，每 30 帧）= 饱和 `(u8)(当前 fps + 0.5)`（≥256 取 0xff），
  是瞬时 fps 采样而非聚合值本身。
- 顶层 +0x58 = 存在的 stage 槽数（0x4486d9）；+0x68 = 保存确认流程写入——
  对话框路径恒 8（0x4549d0:86），对局中断路径写当前关 DAT_004be81c
  （0x43fa90:197 / 0x43e780:361）；0x448a35 以 +0x68>7 与否选择 All Clear 行。
- EOF：仅第三参非 0（中断保存 0x43e780:508）时末 stage 追加 6×0xFF 输入字节
  （0x44854c..0x448598），无事件字节；对话框/game over 路径传 0，完整录像无 EOF。
- USER 段：两块（"USER"+u32 块总长+u8 flag+3 填充+文本），文本后 sprintf NUL +
  `+1` 预留 NUL（0x448ace/0x448b61），块长 = align4(13+文本长)
  （0x448ad9/0x448b6c）。块 1（flag=0）sprintf 序（0x448990..0x448ac9，
  参数经反汇编核对——伪代码漏 sprintf 参数）：
  `"東方神霊廟 リプレイファイル情報\r\n"`（标题串 .rdata 0x4abdc0：神=90 5F、
  廟=95 5F）、`"Version %s\r\n"`（1.00c）、`"Name %s\r\n"`（顶层名字缓冲，
  0x20 填充至 8）、`"Date %.2d/%.2d/%.2d %.2d:%.2d\r\n"`（localtime(top+0x0C)），
  `"Chara %s\r\n"`（表 0x4bb904，索引=+0x5C++0x60）、`"Rank %s\r\n"`
  （表 0x4bb914，索引=+0x64）、Stage 行（+0x68>7：first==7 → "Extra Stage Clear"
  否则 "Stage All Clear"；否则 first==last → "Extra Stage"/"Stage %d"，区间
  `"Stage %d 〜 %d\r\n"`（0x4abe88，〜=81 60），first/last=存在槽最小/最大关号）、
  `"Score %d\r\n"`（+0x14）、`"Slow Rate %2.2f\r\n"`（+0x54 f32 提升 double）。
  块 2（flag=1）= "コメントを書けます"（0x4abebc，无 \r\n）。
  移植侧注入缝 = `ReplaySaveMeta`（date / slow_numerator+slow_denominator /
  stage_field / append_eof / version），默认值即 0x4549d0 对话框路径语义。

## 4. 解压后结构

顶层 0x74 头：+0x00 玩家名（0x20 填充至 ≥8）、+0x14 总分、+0x54 慢速率 f32、
+0x58 关卡数、+0x5C 角色、+0x60 机体、+0x64 难度（0=Easy…3=Lunatic）、+0x70 练习关卡
（-1 = 本篇）。其后 stage 块链，块步进 = 0xC4 + 数据长（0x448c40:79）：

| stage 头偏移 | 类型 | 含义 |
|---|---|---|
| +0x00 | u16 | 关卡号（1..6，7=Extra） |
| +0x04 | u32 | 输入帧数（6 字节/帧） |
| +0x08 | u32 | 数据字节长 = 输入 + 事件 |
| +0x1C | u32 | 关卡起始分 |

数据区：先 `6×帧数` 输入字节，后事件字节（每 30 帧 1 字节原版 FPS，0xff 截断；录制
0x4480a0:25-37 在帧计数 %30==0 时写入，回放 0x448200:36-42 同节奏消费）。

## 5. 输入帧（6 字节 = 3×u16）

当前态 / 按下沿 / 释放沿（0x4480a0:41-43 落盘；键位结构 0x4e49f0+0x218/0x224/0x228）。
沿字段与相邻帧差分全等（真机录像 105476 帧零错验证），回放注入按当前态驱动、沿由
`GameInput::calculate()`（0x414f30 同构）幂等重算即可。位定义（方向向量表 0x4abc30 实证）：

| 位 | 键 |
|---|---|
| 0x01 | Z（射击） |
| 0x02 | X（符卡） |
| 0x08 | Shift（低速/聚焦） |
| 0x10 / 0x20 / 0x40 / 0x80 | 上 / 下 / 左 / 右 |

结束标记：三字全 0xFFFF（保存于中断保存时追加，0x4484d0:55；完整通关录像无此标记）。
录制侧 quirk：快进标志 0x200 置位且 Z 持续 ≥10 帧时强制置 0x08（0x4480a0:21-23），
`GameInput::recording_update` 的 auto_focus 同构。

## 6. harness 回放注入

`tools/run-harness.mjs replay <rpy> [资源目录] [帧数] [stage 索引]`：解码 → StageBattle
装载对应 `stNN.ecl`（难度位 = 1 << 难度）→ `ReplayInputCursor` 逐帧 `GameInput::update`
→ `battle.step`。注入观察不模拟终局（近似判定死亡按无敌复活继续）。真机录像 th13_udSr01
（Sanae/Normal/全通）一阶段 Boss 出现于第 2380 帧——`tests/replay/replay_test.cpp` 的
「Boss 前关键帧」锚点即 frame 2379。
