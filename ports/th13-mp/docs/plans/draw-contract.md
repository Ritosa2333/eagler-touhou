# 语义绘制清单契约（draw contract）v1

原版侧采集端 `oracle/render_oracle.py`（Unicorn + 假 D3D9 COM 设备）与移植侧
（同构导出器）之间的逐帧比对契约。范式对齐 `event-contract.md` /
`stage1_compare.py`：**JSONL、每帧一行、键序固定、浮点 `%.9g`（binary32 往返
无损）、末行 `RESULT first_divergence_frame=<n|none> window=<帧数> ops=<数>`**
是唯一裁决。

## 0. 驱动窗口（与事件流基线同窗）

| 窗口 | 帧号范围 | 行数 | 基线对照 |
|---|---|---|---|
| 无输入窗 | frame 0..3599 | 3600 | stage1-oracle.jsonl（10187 事件，末帧 3599） |
| 回放窗 | frame 0..11064 | 11065 | stage1-oracle-replay.jsonl（48406 事件，Boss 1非帧 10362 两侧同） |

【2026-10 勘误】v1 此前登记回放窗「frame 0..10662 / 10663 行」，与两侧实测均
不符（oracle 重导出 11065 行 = 帧 0..11064、port 11064 行 = 帧 0..11063）：事件
流基线窗口（10663 帧，th13_udSr01 stage0 全长）与渲染窗窗口（11065 帧）是两个
窗口——渲染窗以 replay stage 全长 11064 输入帧 + 帧 0 装载趟为口径（oracle
`run_replay` 的 `total = min(frames, stage.frame_count)` 后执行帧 1..total）。
两侧帧→输入映射同构（帧 f ↔ 输入 f-1），此前的 off-by-one 是 port 侧驱动循环
漏末帧（`frame < total` 应为 `frame <= total`，已修）；非输入序列错位。

frame 0 = 装载 + root spawn（含构造内首趟）。每帧序（对齐 0x45d2f0 帧序）：

```
0x418e00(EDI=ctl)     ECL 逻辑（事件流基线同拍）
0x46e510()            ANM 增量装载推进
0x46f170(EAX=mgr)     主池全部 AnmVm 步进 + 重建层链
0x46f240(EAX=mgr)     前景池步进 + 重建层链
BeginScene → 0x467960 清批 → 0x43b080(EDI=gamestate) fog 刷
→ 逐层 0x46f380(EAX=层, EDI=mgr)（主池 0..30 + 前景池 32..56）
→ 0x4679a0(ESI=mgr) 终批 → SetTexture(0,0) → EndScene → Present
```

前置装载（战斗初始化最小集，模拟 0x42b990/0x4145c0 序）：
- 0x4038a0(ESI=buf) 建 game_main（DAT_004c2160）
- 0x40d920() 建弹幕管理器（root 0x9752b8、bullet.anm mgr 槽 7 →
  root+0x9752b4；root 挂 DAT_004c2174）→ `game_main+0x19194` = bullet.anm file
- 0x46df40(EAX=8, ECX="effect.anm") → gamectx+0x10
- 0x418d30("st01.ecl")（ANIM 列表 → mgr 槽 10/11、ctl+0x88/0x8c）
- ECL 文件索引表 `ctl+0x80+idx*4`：0=bullet.anm、1=effect.anm、
  2..=st01 anim 列表（enemy.anm、st01enm.anm）

ECL no-op 跳表改写 = stage1 `NOOP_OPS` 剔除 ANM 指令族
（302/303/306/307/308/313/316/319 真实执行）；518→525 尾块保持。

## 1. 帧行格式

```json
{"frame":N,"viewport":[x,y,w,h,minz,maxz],"ops":[op,...]}
```

- `viewport`：本帧 SetViewport 的 6 float（%.9g）；无 SetViewport 的帧为
  `null`。层序即 `ops` 数组序（提交序）。

## 2. op 格式

### 2.1 quad（矩阵路径；IDirect3DDevice9::DrawPrimitive）

```json
{"seq":k,"op":"quad","prim":5,"anchor":a,"count":c,
 "tex":[槽,entry,"纹理名"],"layer":L,
 "blend":[src,dst,bop],"tfactor":u32,"fvf":F,
 "world":[16×%.9g],"texmat":[16×%.9g],
 "view":[16×%.9g],"proj":[16×%.9g]}
```

- `prim`：D3D 图元类型（TRIANGLESTRIP=5）。`anchor`：startVertex（静态 VB
  九宫格锚点组号 ×4，render-chain.md §2.1 第 10 步）。`count`：图元数（2）。
- `tex`：`[mgr 槽号, entry 序号, name1（cp932 解码，如 "enemy/enemy.png"）]`；
  `SetTexture(0, NULL)` 后的绘制为 `null`；未知纹理 `[-1,-1,"ptr:%08x"]`
  （**比对视为分歧**，不允许静默）。
- `layer`：提交时经 0x46f380 进入的层号（主池 0..30、前景池 32..56）。
- `blend`：`[SRCBLEND(19), DESTBLEND(20), BLENDOP(171)]`（§5.2 八模式表）。
- `tfactor`：SetRenderState(TEXTUREFACTOR=60) 的 ARGB u32。
- `fvf`：本绘制时刻设备 FVF（矩阵路径 0x102=XYZ|TEX1）。
- `world`/`texmat`：本次 DrawPrimitive 前最近的 SetTransform(D3DTS_WORLD
  =0x100 / D3DTS_TEXTURE0=0x10) 全 16 元（行主序，%.9g）。
- `view`/`proj`：同理 TS_VIEW=2 / TS_PROJ=3（帧内通常恒定）。

### 2.2 up（预变换顶点路径；DrawPrimitiveUP）

```json
{"seq":k,"op":"up","prim":P,"count":C,"stride":S,
 "tex":[槽,entry,"名"],"layer":L,"blend":[...],"tfactor":u32,"fvf":F,
 "verts":[[x,y,z,rhw,argb,u,v],...]}
```

- `verts`：C×顶点，每顶点按 stride 展开：FVF 0x144（stride 0x1c）为
  `[x,y,z,rhw,diffuse_u32,u,v]`；FVF 0x44（stride 0x14）为
  `[x,y,z,rhw,diffuse_u32,0,0]`。浮点 %.9g；`argb` 为 32 位色（u32 十进制）。
  【2026-10 更正】0x44 分支的 u/v 槽为 JSON **整数 0**（两侧一致；port 侧
  曾输出字符串 "0"，str-vs-int 属潜在假分歧，已修——当前窗口 port 无
  0x44 up op，oracle 的 0x44 up 集中在帧 10362+）。
- 一次 DrawPrimitiveUP = 一个 op。`verts` 序列化为扁平顶点流（3C 个 float32
  槽中每顶点前 `per = stride/4` 项）的**前 C 项前缀**——TRIANGLELIST 批的
  每图元 3 顶点、钩子 count = PrimitiveCount（每精灵 2 三角形）时，单精灵
  保留上缘 2 顶点（丢 4/6）、多精灵批后续精灵全部丢弃（v1 此处「全部
  6×count 三角形顶点平铺」的括注与实现不符，两侧实现同构截断、比对裁决
  不受影响——帧 91 count=4 样本 verts=[c0,c1,c2,c1] 实证）。

## 3. 数值与比对规则

- 全部 float 以 `%.9g` 序列化（与 event-contract §5 相同，binary32 往返无损）；
  NaN 序列化为 `"nan"`，两侧按「任一 NaN == 任一 NaN」处理（cmp_float 语义）。
  【2026-10 对齐轮勘误】v1 基线里 world/texmat 的 NaN 并非「角度未初始化
  pattern 0xfff0bdc1」，而是 oracle 假设备三处缺陷的产物（GetLevelDesc 误以
  ECX 取 this、D3DSURFACE_DESC 多写一个幻影 Size 字段致 Width 错位到 0、
  D3DXCreateTexture 的 cdecl 栈参偏移整体错 8 字节使纹理登记宽高失真）→
  精灵记录 X 维 0/0=NaN。三缺陷修复后重导出的基线不再含此类 NaN。
- `draw_compare.py`：逐帧逐 op 逐字段；浮点按 **ULP 容差**（默认 4 ULP，
  `--ulp N` 可调；位级相同直接通过）；整数字段（prim/anchor/count/tex 槽/
  entry/layer/blend/tfactor/fvf/argb）精确相等。
- 顶点数组逐元素比对；矩阵逐元素比对。
- `tex` 名字段参与比对（纹理内容不比——动态纹理/后台缓冲拷贝按事件语义比，
  见 §4）。
- 分歧报告：首个不匹配的 (frame, seq, 字段, 两侧值)。末行
  `RESULT first_divergence_frame=<n|none> window=<帧数> ops=<两侧 op 总数较大者>`。

## 3.5 字段过滤纪律（--ignore；整改 2026-10-23）

`draw_compare.py --ignore <field>` 为**诊断模式**开关，仅限隔离工作面
（回答「除 X 外是否还有别的分歧」）。规则：

1. **绿灯链路禁用**：任何交付门禁、HANDOFF 战果、golden 登记（replay-verifier
   run-gate）不得基于带 --ignore 的 RESULT 声明对齐。
2. **输出显式登记**：带过滤的 RESULT 行尾强制追加 `ignored=<字段,...>`，
   且退出码 = 3（无过滤通过 = 0、分歧 = 1、用法错 = 2）——脚本化门禁按
   code 判定，杜绝「过滤后 none」被当全绿。
3. **限期销账**：每个进入 --ignore 的字段必须在 HANDOFF 工作面登记销账
   条件（根因修复或证据化等价）与期限；销账后该字段必须回到全字段比对。
   登记台账：~~`tfactor`~~（**已销账 2026-10-23**：根因 = ECL 30x/镜像
   创建点的色/α 插值缺一步「ctor 趟步进」——原版 0x41fe80/case3 与
   0x419450 镜像块在敌机更新趟内创建 VM 时色通道多推进一步（oracle
   VM+0x15c α=6/timer=3 vs port 4/2 实测）；修复 = StagePresentation
   创建点 `create_color_step`（AnmInterpolators::advance_color，仅色/α
   两通道，脚本与矩阵不动）。销账实测：对局无输入窗 RAW f511、回放窗
   RAW f124——tfactor 全窗对齐，无过滤比对恢复）。当前无在册登记项。

NaN 相等性（`cmp_float` 的「任一 NaN == 任一 NaN」）是本契约的既定语义
（§3 序列化规则的一部分，两侧同式），非遮蔽——勿与 --ignore 混淆。

## 4. 已知边界（v1 不覆盖、如实标注）

1. **纹理内容不比**：像素（THTX/文本 GDI 上传/后台缓冲→@R 拷贝）不进清单；
   动态纹理在清单中就是普通 tex 引用（名字 + 槽稳定）。
2. **stage 侧固定层**：0x4032d0 的关卡 ctx 内嵌 5 AnmVm、HUD、玩家、弹幕
   遍历绘制（0x40e4d0 族）不在窗口内（stage1 环境无这些系统对象）；
   窗口覆盖 = ANM mgr 两池全部精灵（敌机/ECL 挂接精灵/ANM 装载预置精灵）。
3. **视/投矩阵**：设备创建时一份（0x45dc80），不随游戏 zoom 变化（真实游戏
   每帧 0x43b900 刷新——stage1 环境 gamestate 静止，视矩阵恒定等价）。
4. **render 或acle 与 stage1 事件流的差异**：ANM 指令族真实执行后 RNG 消耗
   与基线不同——渲染窗是独立基线（窗口帧数与事件流基线一致即可，事件流
   逐字节一致性属事件窗自己的验收）。

## 5. 自证（确定性）与实测基线

同参数连跑两次输出**逐字节一致**；每帧绘制数 > 0；窗口帧数与事件流基线一致。
`RESULT` 行是比对唯一裁决。

原版侧实测（容器内实际执行，2026-10 v1；**2026-10 对齐轮重导出**＝假设备
三缺陷修复后，见 §3 勘误）：

| 窗口 | 行数 | ops | quad | up | 零绘制帧 | 两次连跑 |
|---|---|---|---|---|---|---|
| 无输入 st01-draws.jsonl（v1） | 3600 | 301983 | 289456 | 12527 | 0 | cmp 逐字节一致 |
| 回放 st01-draws-replay.jsonl（v1） | 10663 | 774756 | 711279 | 63477 | 0 | cmp 逐字节一致 |
| 无输入 st01-draws.jsonl（重导出） | 3600 | 289456 | 289456 | 0 | 0 | cmp 逐字节一致 |
| 回放 st01-draws-replay.jsonl（重导出） | 11065 | 735235 | 735157 | 78 | 0 | cmp 逐字节一致 |

重导出后 up 归零/骤减的原因：极小环境下敌机 ANM 的出生位置来自随机/环境
变量（观察值 y≈-5745 恒在屏外），v1 基线里它们因记录宽高 NaN 使 0x467350
的视口裁剪比较恒假而**误画**；记录修好后被正确裁剪。敌机 up 的语义对齐
（位置种子链）与 stage1 事件窗 §4.4 的 RNG 消费差异同属后续工作面。

移植侧实测（容器内实际执行，2026-10；`tools/run-harness.mjs draws` /
`draws-replay`，WASI 无头链路——确定性最强：无浏览器/rAF/GL 时序）：

| 窗口 | 行数（帧号） | ops | quad | up | 零绘制帧 | 两次连跑 |
|---|---|---|---|---|---|---|
| 无输入 port-draws.jsonl | 3600（0..3599） | 293792 | 289456 | 4336 | 0 | cmp 逐字节一致 |
| 回放 port-draws-replay.jsonl | 11064（0..11063） | 750436 | 735098 | 15338 | 0 | cmp 逐字节一致 |

【2026-10 勘误】上表回放窗为 off-by-one 修复前的实测（11064 行 = 帧
0..11063，漏末帧 f=11064）；修复后重导出应为 11065 行（0..11064），与本
节 oracle 表对齐。§5 下文轮次注记中的行数引用以各自导出时点为准。

`draw_compare.py` 当前裁决（2026-10 轮 9 后）：无输入窗 `first_divergence_
frame=511`、回放窗 `first_divergence_frame=124`（帧 0-510 / 0-123 逐帧逐
op 逐字段全过——f92 层内序 + f159 缓动权重 80 位链两个根因已修，新工
作面见后文登记）。轮 4 已修：ECL/ANM 双独立种子 0 流（[-9987] 新流第 1 抽实证，
单流合并勘误）。轮 4 的关键更正：**ECL 与 ANM 是两条独立种子 0 的 0x472c90 流**——
ECL 的 [-9987]（RANDF2）在 f61 仍返回新流第 1 抽（MainSub00 的生成坐标
x=%A+[-9987]*32 → GirlRedA01 x=-163.783264），与 ANM 流（装载 2 调+每神灵
10 调，f61 前已耗 82）互不推进；轮 2 的单流合并是过度修正（f61 前 ECL 恰
零抽签故神灵匹配，f61 首抽即偏）。此前轮 3 的更正保留：原版侧曾缺
0x43bbc0 战斗 gamestate 初始化（视口块=UP 裁剪数据源恒零致全裁）。敌机
链已通：ECL 303/306 槽位 VM 创建（0x41fe80/case3）+ 每帧位置跟随
（0x41a8a0 把敌机 ECL 位置写槽 VM+0x574）+ UP 层口径（flush 时 watch 残
值 56）。

神灵链已通：ECL `MapleEnemy` 的 `ins_308(1,91)`（0x41b260 case5 →
0x4243e0）→ 移植侧 `StageCommands` 307/308 真实现 + `StagePresentation`
排空创建（主池头入链），移植 quad 总数 289456 与原版**完全相等**；对齐轮
2 的关键修复：ANM/ECL 共享同一条 0x472c90 全局 RNG（状态 0x4dc31c 种子
0，装载期 2 调 = effect script23 模板 ins_120×2、每神灵 spawn 10 调）+
装载序 bullet→effect→stage + vec3 实参求值序 x→y→z + matrix_product
double 中间量 + 帧尾 SetTexture(0,0)（设备态清空、mgr+0xb78188 缓存不
失效——oracle f1-8 tex=None 的来源）。

【帧 92 已修（本轮轮 9）】轮 8 的「Blue 丢弃」假设证伪——0x467350
反编译（`00467350.c:130-156`）只有视口裁剪四向比较，四角在内必 enqueue；
f92 实测（trace_spawn 新增 0x46f1d7 主链遍历 watch + 0x46f4a0/0x46f3f0
插入归表 watch）：f91 Blue 出生走 0x4243e0→0x46f4a0 **头插**（链头），
f92 Blue 翻转重挂走 0x419764→0x46f3f0 **尾插**（链尾）——oracle 层桶序
（主链头→尾遍历序）f91 = Blue 先、f92 = Red 先。op[12] 的 up 批
`verts` 截断序列化只保留批内**第一个**精灵的顶点，f92 oracle 批序
Red 在前（verts 全 Red），port 批序 Blue 在前 → 首分歧 483.5 vs 178.5。
port 根因：翻转重挂复用 `AnmManager::create` 默认 ordering=2（0x46f4a0
头插语义；303/306 建池 0x4243e0 才是头插），原版重挂直调 0x46f3f0 尾插。
修复：`StagePresentation.cpp` 镜像重挂 create 调用 ordering 2→0（尾插）。
（此前「x 误加 16」疑点亦证伪：port v0 是镜像精灵的右上角，中心
467.5 = oracle Blue p574=147.63+320 ✓ 位置本身一致。）

【帧 159 已修（轮 9 续）】f92 修复后首分歧 159（op[18].world[12]
-6.02060699 vs -6.02061462，16 ULP）。取证：world[12] = 灵行走 VM
+0x3c 直接值；两侧插值通道输入**位级一致**（oracle trace 通道字段
dump vs port `TH13_DUMPCHECK=159` harness dump：start=4264ad62、
end=c283e607、elapsed=152、dur=300、mode=9 全同）。根因 = `0x473a40`
缓动权重 mode9 族 else 分支 `(2-(2-t2)²)×0.5` 全程 x87 80 位链、仅
最终一次落 f32（0x473b98-0x473baa 无中间 fstps）；port `Interpolation.cpp`
逐步 f32 落位。数值实证：double 链复现精确命中 oracle 位 c0c0a8e0，
逐步 f32 复现 port 位 c0c0a8d0。修复：`interpolation_weight` 幂族
（mode 1-6、9-14、22-31）中间量改 double、最终单次落 float（t 入口
除法与 mode9 族 t2=t+t 加倍点仍单次落 f32——0x473a50/0x473b65
fstps 实证）。mode 18-21（sin 族）未逐位验证、窗口无消费，保持现值。
无输入窗裁决推进 159→511。

【新工作面登记（轮 9 实测）】
- 无输入窗 f511：层 56 敌机 up 批 verts[12][1] 446.5 vs 447.5——
  port 侧中心 x≈509.5 的敌机 f511 起 ECL 位置冻结（x/y 均停、uv 帧
  动画仍走 → ANM VM 活着、位置跟随断），oracle 继续每帧 +1。ECL 敌机
  运动链分歧，未定位。
- 回放窗 f124：op[16] 层 56 敌机批 count 6≠4——oracle f124 起批内
  少一个行走敌机精灵，port 保留。与事件窗 replay frame 124 分歧
  （§事件窗回归注，50433ebc 前已存在）同帧同源，属既有登记面。
两窗裁决（draw_compare ULP-4）：无输入 92→**511**，回放 92→**124**；
确定性自证：port draws 重跑 `cmp` 逐字节一致。

【轮 8 取证存档（f92 工作面的当时假设，轮 9 已证伪并翻案，见上）】
trace_spawn p574 dump + DPUP/flush 序当时记录：oracle f92 层 8 dispatch
双行走 VM（Red t=31 p574=-125.29、Blue t=2 p574=147.63），四角均在视口
内，「enqueue 只发生 Red」的观察其实来自 _enq_log 打印配额（3 条上限）
吞掉了 Blue 的第二条 enqueue——f91 实际两条 enqueue（452.5 与 177.5）
同帧并存，f92 同样双精灵批（count=4=2 prim×2 精灵），截断序列化只见
第一个精灵。「0x467350 后段丢弃条件」与「port x 误加 16」两个修复面
均已被轮 9 证伪（层内提交序 + 尾插才是根因）。

【敌机 aura up 缺口登记（评审确认轮新增，独立于上面 Blue 丢弃+offset
工作面）】f92 起 port 侧敌机 aura 的 up op 与 oracle 出现系统性差异
（port-draws.jsonl f1371+ vs st01-draws.jsonl f1371+）：层号错（port
0/2 vs oracle 4/8——aura 创建层与敌机本体层未按构造分置）、缺 op、
提交序错且该段 up 总量约为 oracle 的 1.72 倍。根因主链已字节码+反汇编
双重实证：① port 槽位 VM 创建曾传 layer=-1（脚本自定层）——原版
0x41fe80:0x41ff24 以敌机内嵌 VM `[+0x278]+7` 建层（内嵌 VM 基址 =
敌机+0x11EC——0x418e31 `add $0x11ec,%eax` 后进 0x419450 族，故
「+0x278」即敌机结构+0x1464），已改为 `[+0x278]+7`；② 镜像重挂行走
VM 层同样以 `[+0x278]+7`（0x419450:0x419764），此前 mirror_script_base
全仓无赋值点恒 0 → 落层 7（已修：构造默认 1——0x418f40 体内 0x419089
`mov $1,%edx`→0x419195 写敌机+0x1464，Unicorn 写 watch 实证每敌机均
写 1；ECL 552（0x41b359，zIndex）为覆盖指令，窗口 st01 未用、st05bs
父子敌机有用例——port 侧已实装 552）；③ slot1 父脚本 enemy.anm 85/91
无 op304/相关出生 op（oracle 侧 aura 为独立 op，port 侧缺失待逐 op
补齐）。此缺口此前未登记，修复后需重导出并以 draw_compare 复核 f92
后的裁决移动（评审注：严格 ULP-4 下 f159 浮点分歧先于层号分歧暴露，
非「f92 修复后立即暴露」）。
【本轮修复后实测（无输入窗）】层号主链修复生效：敌机本体 up 落层 8
（此前 0/2），f92 帧层 56 双侧同、f1371+ 敌机层 8 与 oracle 侧 8 对齐；
aura 独立缺口仍在（port 层 7 vs oracle 层 4——父脚本无出生 op 的登记项
③），up 总量 4336→30812（当时裁决保持 f92；轮 9 层序+权重修复后见上方
新裁决）。回放窗行数 11064→11065 与 oracle 对齐。
轮 7 已修：翻转重挂机制全解（脚本号查表/门=sign(x速度)/layer=+0x278+7/
位置继承），行走 VM 身份两侧已逐一对齐（Red→7、Blue→1=oracle s4aa）。
轮 5 已修：镜像翻转重挂（0x419450:101-152，flags&0x200000 + x 朝向
符号变化 → 回收槽 0 VM、以脚本号 +0x278+7 重建行走动画、尾入池）——
帧 63-91 敌机运动+行走动画逐字段全过。次级：敌机 up 的顶点色链、ECL
313/316（MBoss2 case7/10）未实现、回放窗自机弹 VM 的 RNG 消费、stage
合并 bank 的文件 3+ 脚本号偏移（窗口内敌机均用文件 2，暂无影响）。

【事件窗回归注】replay 事件窗（vs stage1-oracle-replay.jsonl 48406 事件）
在 50433ebc（本轮改动之前）即已分歧于 frame 124（54816 事件；stash 本轮
改动后原样复现，非本轮引入）；stage1 无输入事件窗 10187 事件仍逐字节一致
（本轮实证）。

主要纹理（槽:entry:名）：8:2 `effect/eff_base.png`（神灵粒子）、
10:2 `enemy/enemy_aura.png`、10:1 `enemy/enemy2.png`、10:0 `enemy/enemy.png`
（杂兵族）、回放窗另见 11:0 `stgenm/stage01/enm1.png`、8:3 `effect/eff_aura.png`。
