# TH13 ANM（v8）管线行为规格与移植方案

来源：th13 Ghidra 反编译（`artifacts/decomp/jp/functions/`，重点 0x462450 及其邻居）+ 资源实证（`artifacts/reference/dumps/anm-list.txt`、`artifacts/reference/raw/*.anm` 逐字节解码）+ th15 参考实现（`ports/th15/web/cpp/game/Anm*`）。贴图提取命令已在本机容器实测通过。目标：定义 th13 ANM 归档解析、脚本/精灵结构、AnmManager/AnmVm 生命周期的行为规格，给出 ports/th13 的落地模块与贴图管线。

## 一、规模与不变式（dump 实证）

对 `anm-list.txt` 全量统计（55 个 ANM，命令见文末验证记录）：

| 指标 | 值 |
|---|---|
| ANM 文件 | 55（effect/front/text/bullet/ascii/st0Xlogo|wl|enm|menm/敌我立绘等） |
| entry 总数 | 372（链式 chunk） |
| sprite 总数 | 2480（全部整数值坐标） |
| script 总数 | 1832（`grep -c '^script script' anm-list.txt`；标签以 `ins_5` 伪指令计入指令流，不单列） |
| version | 全部为 8 |
| 纹理 format | {1, 3, 5, 7}（THTX 像素格式同集合） |
| hasData=0（无内嵌 THTX） | 6 个 entry（外部名/空白） |
| lowResScale | 全部 0（th13 未用该字段） |
| 特殊纹理名 | `@`（空白）×4（help/title/text），`@R`（渲染目标）×2（text.anm） |

## 二、归档与 entry 结构（on-disk，逐字节验证）

ANM 归档没有全局魔数：文件即 entry 链，首 entry 在偏移 0，`nextentry`（相对本 entry 起点）为 0 表示链尾。字段表与 th15 `AnmResource::open`（`ports/th15/web/cpp/game/AnmResource.cpp:10-67`）完全一致，本节用 th13 二进制逐字段复核：

| 偏移 | 类型 | 字段 | front.anm entry0 实测 |
|---|---|---|---|
| +0x00 | u32 | version（==8） | 8 |
| +0x04 | u16 | numSprites | 0x52（82） |
| +0x06 | u16 | numScripts | 0x70（112） |
| +0x08 | u16 | （保留 0） | 0 |
| +0x0A | u16 | 纹理 width | 512 |
| +0x0C | u16 | 纹理 height | 512 |
| +0x0E | u16 | 纹理 format | 1 |
| +0x10 | u32 | name1 相对偏移 | 0x508 → `front/front00.png` |
| +0x14 | i16 | sourceX | 0 |
| +0x16 | i16 | sourceY | 0 |
| +0x18 | u32 | memoryPriority | 10 |
| +0x1C | u32 | THTX 相对偏移 | 0x8C08 |
| +0x20 | u8 | hasData | 1 |
| +0x22 | u16 | lowResScale | 0 |
| +0x24 | u32 | nextentry 相对偏移 | 0x108C18（=THTX 末尾，闭环校验通过） |
| +0x40 | u32×N | sprite 偏移表（N=numSprites，相对 entry；首项值 0x528） | 表起 0x40 |
| +0x40+4N | {i32 id; u32 offset}×M | script 表（M=numScripts，相对 entry） | 0x188 起，{0,0xB90},{1,0xCF0}… |

- **勘误（字段序更正）**：本表旧版把三个 u16 写成 format@+0x0A/width@+0x0C/height@+0x0E，与 th15 及字节事实相反——raw front.anm `+0x08..+0x0F = 00 00|00 02|00 02|01 00`，且 THTX 像素量 0x100000=512×512×4 仅在 width/height=512、format=1 时闭环；th15 `AnmResource.cpp:24` 同序（width=h+10/height=h+12/format=h+14），thanm dump 亦为 format:1/width:512/height:512。按旧序会读出 format=512（非法）。

- **sprite 记录（20 字节）**：`{u32 id; f32 x; f32 y; f32 w; f32 h}`，x/y 为左上角、w/h 为尺寸（th15 `AnmResource.cpp:41-48` 同构；2480 个 sprite 坐标全部整数值，dump 实证）。
- **THTX 块**：`'THTX'` + u16 zero + u16 format + u16 width + u16 height + u32 size + 像素数据（th15 `AnmResource.cpp` 读 `h+tex+6/+8/+10/+12`；front.anm entry0 实测字节 `00 00|01 00|00 02|00 02|00 00 10 00`，闭环 0x8C08+16+0x100000=nextentry 0x108C18）；size==w*h*bpp（format→bpp：1→4，3/5/7 见下）。
- **像素格式**（th15 `AnmTexture::rgba`，`AnmResource.cpp:69-88`）：1=ARGB8888、3=RGB565、5=ARGB4444（u16 = A<<12|R<<8|G<<4|B）、7=A8。th13 dump 只出现 {1,3,5,7}。
- **名称语义**（Ghidra `FUN_0046e230`，`functions/0046e230.c:18-38`）：name1 以 `@` 开头且第二字符 `R` → 渲染目标纹理（调用 0x46DC60）；`@` 其他 → 空白纹理（0x46DC10）；否则 hasData=1 走 THTX 上传（0x46DAE0/0x46D8F0），hasData=0 为外部名。
- **跨 entry 全局编号**：装载器把各 entry 的 sprite/script 按表序追加到全局表（Ghidra `FUN_0046e0b0` 以 u16@+4/u16@+6 累加 sprite/script 计数传入 `FUN_0046e230`；脚本偏移写入 ctx+0x120 的全局槽）。脚内 `ins_300(sprite)` 的编号是**归档内全局 sprite 序号**，不是 entry 内序号——移植时必须保持同样的追加顺序。

## 三、脚本与标签结构（逐字节验证）

指令布局（th15 `AnmInstruction`，`AnmResource.hpp:6-12`；Ghidra 分发器同构）：

```
struct Insn { u16 opcode; u16 size; i16 time; i16 mask; u32 args[(size-8)/4]; };
```

- **执行门槛**：`insn.time <= vm.current` 才执行（Ghidra `00462450.c:314`：`if ((int)insn.time <= vm.time)`；th15 `AnmInterpreter.cpp:42` 同）。
- **mask 位**：bit n 置位 = 第 n 个参数是变量引用（值≥10000），读侧经 0x4621B0（整数）/0x461E80（浮点）解析，写侧经 0x462380/0x4622C0 取字段地址。证据：`00462450.c:407-425`（case 100/101 的四象限组合）。
- **终止符**：`opcode=-1`（8 字节，size=0）。下一 script 紧随其后（front.anm：script0 的 -1 在 entry+0xCE8，script1 恰在 0xCF0，与 script 表 {1,0xCF0} 闭环）。
- **标签 = opcode 5 伪指令**：`{5, 12, time, 0, i32 id}` 内嵌在指令流中。运行期按 NOP 跳过；中断时从脚本头线性扫描：`id==请求值` 命中，`id==-1` 为默认标签（fallback），扫描遇 `-1` 终止符或 0xFFFF 停止。证据：th13 `00462450.c:363-384`（LAB_004625B5 扫描）与 th15 `AnmInterpreter.cpp:37` 逐句对应；dump 中 `ins_5(-1)` 22 处、`ins_5(1)` 760 处即标签表。
- **跳转类**：op200 `jump{offset, time}`（IP=脚本基址+offset，time 重置）；op202–213 条件跳转（int/float × {==,!=,<,<=,>,>=}）；op201 循环计数器自减非零则跳。证据：th15 `AnmInterpreter.cpp:84-98`；th13 case 0xC9（201）`00462450.c:1082-1101`（计数器减一后 `param_1[10] = param_1[0xe] + arg1` 即 base+offset）。

## 四、AnmVm::tick 执行模型（0x462450，13.6KB/84 调用方含 ECL 0x41B260）

入口序（`00462450.c:291-309`）：

1. 前置回调 vm+0x590 非空则先调用，失败即返回；
2. IP（vm+0x28）为 0 或 flags（vm+0x18）含 0x100000 → 直接返回（休眠/停机）；
3. render_flags（vm+0x1c）bit8 置位时全局倍率临时置 1.0（`DAT_004c0a28`，退出时恢复）；
4. `(render_flags & 0xc000)==0x4000`（脚本已停、等中断）且暂停旗标 → 返回（th15 `AnmInterpreter.cpp:34` 同句）。

派发分四段：0–214（基础/变量运算）、301–508（状态与过渡）、601–608（几何/粒子形态）、特例 300 前置处理。每条执行后 `IP += insn.size`。

**时间推进**（函数尾，`00462450.c` 末段）：`prev = current`；timescale（vm+0x544 指向 float，默认 1.0）偏离 1 时走浮点累加器（vm+0x540）后取整，否则 `current+1`。op6 为时间回退（fractional -= frames），op7 为中断返回（恢复 vm+0x00 保存的 time 与 vm+0x14 保存的 IP）。证据：case 7 `00462450.c:401-405`、th15 `AnmInterpreter.cpp:90`。

**结束语义**：

| opcode | 行为 | 返回 |
|---|---|---|
| -1 / 1 | 结束：清 visible 位，IP=-1 | 非零 → 管理器回收 VM |
| 2 | 停止：IP=0，VM 存活休眠 | 0 |
| 3 / 4 | 脚本停、等中断：flags|=0x4000（4 额外清 visible），推进一帧后返回 | 0 |
| 0 / 5 | NOP（5 兼作标签载体） | 继续 |

**中断流程**：pending（vm+0x4a0）非零时，先扫描标签（见上节）；命中后：保存 `{time, IP}` 到 vm+0x00/vm+0x14，置 active 时间 = 标签 time（0x4016C0/0x467080 初始化 time 结构 {cur=-999999, req, frac, &1.0}），IP = 标签条目+size，flags|=1（可见）。无标签命中 → 推进一帧返回（th15 `AnmInterpreter.cpp:37-38` 同序）。

**opcode 语义总表**：以 th15 `AnmInterpreter.cpp` 为准（100–131 变量运算/随机/三角、200–213 跳转、300/301 选精灵（301 支持随机 `rand()%count+first`，`00462450.c:1427-1465` 与 th15 :100 同构）、302–317 渲染旗标/层/混合（302 的 mode 10 触发扭曲几何 0x46B790，`00462450.c:1465-1471`）、400–438 即时设置与插值过渡（400/407/415/420/433 位置，401/410/411 旋转，402/412 缩放，403/409/417 透明度，404/408/413/414 颜色，425–430 UV 滚动/缩放）、500–508 派生子 VM、601–608 几何形态）。mask 变量语义：10000–10002 位置（vm+0x4AC/4B0/4B4）、10013–10015 旋转基准（vm+0x3C/40/44）、10022 随机 u32（0x472C90=rand）、10039–10041 基准位置（vm+0x48/4C/50）、10043–10045 随机幅度/上界（vm+0x4D4/4D8/4DC）——证据 `004621b0.c`、`00461e80.c` 与 th15 `AnmVariables` 对齐。

**th13 opcode 清单差异**（dump 频次实证）：th15 有而 th13 dump 未出现的：108–111、123、126–130、422、507、509、610–611，条件跳转 202–213 仅 204 有 8 处（其余零使用，但分发器 case 存在）；th13 出现而分发器未见 case 的：`ins_600`（10 处，落入默认分支，行为待 oracle 验证，见风险表）。

## 五、AnmManager 与 VM 生命周期

全局管理器 `DAT_004dc688`。VM 池：mgr+0xDC .. mgr+0xB77B20，槽位步长 0x5BC，VM 体 0x5A4 字节（尾部 0x5A8/0x5AC 空闲链指针、0x5B4 占用标记、0x5B8 槽号缓存；`functions/004706c0.c`、`functions/0046f690.c:49-66`）。

**句柄**（`functions/0046f3f0.c:34-42`）：`handle = slot(低 13 位) | generation(高 19 位)`；代计数器 mgr+0xF53C3C 自增回绕跳 0。slot=0x1FFF 表示池外堆分配，查找走两链线性扫描（`functions/0046fb90.c:10-27`）。

**VM 关键字段**（本节全部有 Ghidra 行号证据）：+0x18 flags（1=可见、0x4000=停等中断、0x100000=停机）；+0x1C render_flags（0x200=活动、0x20=待回收、0x8000 新绑→0x4000 停等、0xc000 状态域）；+0x24 layer（0–30 主表 / 31–32 副表）；+0x28 IP；+0x30 归档 ctx；+0x38 脚本基址（标签扫描起点）；+0x4A0 pending 中断；+0x4A4 sprite_frame；+0x530 句柄；+0x534 槽号；+0x538/0x53C/0x540/0x544 time{prev,current,fractional,*scale}；+0x54C 活动链节点；+0x560/0x564 子链首/尾；+0x56C 绘制桶 next；+0x584 精灵源回调；+0x590 前置回调；+0x598 析构回调；+0x59C 子中断回调。

**create（游戏侧，0x46F820，45 个调用方含 ECL 0x41B260）**：`create(&handle, script, pos*, anmArg, layer, flags)` → 池分配（0x4706C0）→ 从 ctx+0x10C 模板数组复制 0x530 字节并重置 time（0x403800）→ render_flags|=0x200 → layer∈[0,21) 时 render_flags|=0x40000 → 位置旗标 bit0 时偏移 `(+320,+16,0)`（游戏坐标→ANM 坐标系）→ **立即执行一次 tick** → 按 flags 插入链表：bit1=主链头（0x46F4A0），bit2=副链尾（0x46F540），bit1|bit2=副链头（0x46F5F0），否则主链尾（0x46F3F0）。

**spawn（脚本侧 op500–508，0x46F990/0x46FAB0）**：同 create 流程；子 VM 继承父 layer（`0046f990.c:20-22`：`vm[9] = *(parent+0x24)`），首 tick 后插链。505/506 附带子 VM 锚点缩放（vm 子+0x4E0/0x4E4）。

**bind（0x461CB0）**：按脚本号取 ctx+0x90 表 → vm+0x30=ctx、IP=脚本基址=vm+0x38 → render_flags 0x8000 → 首 tick → 若停等（0xc000==0x8000）转 0x4000。脚本不存在则整 VM 清零。

**每帧 tick（0x46F170 主链 / 0x46F240 副链）**：遍历链表逐 VM 调 0x462450；返回 0 → 存活，按 vm+0x24 归入绘制桶（主链 31 桶；副链把层钳到 31/32），桶链指针 vm+0x56C；返回非零 → 0x46F690 回收。副链独立于主链（后台/前景两组 tick 源）。

**interrupt（0x46FCA0）**：句柄查 VM（0x46FB90）→ 子中断回调 vm+0x59C → pending=+0x4A0=label → 立即 tick → 遍历子链（vm+0x560）对每个子 VM 同样置 pending 并 tick（中断向子树传播）。

**回收（0x46F690）**：从四条链摘除 → vm+0x598 析构回调 → 子链全体标死（render_flags|=0x20）→ 释放 vm+0x58C 动态缓冲 → 池内回收或 free。

**装载/卸载**：0x46DCA0 驱动，逐帧逐 entry（0x46E0B0 → 0x46E230）：建纹理对象入 ctx+0x124 槽（0x18/entry）、sprite 追加进 ctx+0x11C 全局表（0x48/sprite，含 texW/H@+0x1C/20、x1y1x2y2@+0x24-30、像素 w/h@+0x34/38、UV 缩放@+0x3C/40）、脚本偏移入 ctx+0x120、模板 VM 入 ctx+0x10C（0x5A4/脚本）。卸载 0x46E620：先 0x46FEA0 把绑定该 ctx 的全部 VM 标死，再逐项释放。选精灵（op300 路径 0x461A60）按全局 sprite 序号 ×0x48 取记录，写入 VM 的 UV/尺寸/sprite_matrix（缩放因子 1/256=0.00390625，`functions/00461a60.c:69-81`，th15 `select_sprite` 同常数）。

## 六、th15 参照与 th13 差异

- 解析层：th15 `AnmResource::open` 直接适用（版本==8、chunk 链、表结构、THTX 校验全一致，第二节逐字节验证）。差异仅统计面：th13 无 lowResScale 用例、格式集合 {1,3,5,7}、`@R` 渲染目标 2 处。
- 解释层：th15 `AnmInterpreter.cpp` 的 opcode 覆盖是 th13 超集（th15 另有 509/600/611 等 th15 专属形态指令）。th13 移植按第四节清单裁剪，未覆盖 opcode 显式报错（th15 :169 同策略）。
- 管理层：th15 `AnmManager`（bank=resource id、create/interrupt/pause/retire、`AnmRegistry` 句柄表）与 th13 原版行为同构；th13 特有：双 tick 链 + 31/2 层桶、创建即首 tick、坐标 +320/+16 偏移、中断向子树传播。

## 七、落地模块（ports/th13/web/cpp/game/）

| 模块 | 来源 | 改造点 |
|---|---|---|
| `AnmResource.{hpp,cpp}` | 拷贝 th15 同名 | 版本==8 不变；格式表保留 {1,3,5,7}（6/2/4 分支保留无害）；加"全局 sprite/script 序号跨 entry 追加"断言测试 |
| `AnmVm.hpp`、`AnmVariables`、`AnmVisualState`、`AnmInterpolators` | 拷贝 th15 | 变量表按第四节 th13 实测偏移对齐命名 |
| `AnmInterpreter.cpp` | 拷贝 th15 | opcode 清单按第四节裁剪；`ins_600` 暂按未实现报错，待 oracle 定行为 |
| `AnmManager.{hpp,cpp}`、`AnmRegistry` | 拷贝 th15 | `create` 增加坐标偏移旗标与 layer∈[0,21) 语义；中断传播到子树；双链 tick 合并为单 update 内两遍 |
| 坐标系 | — | th13 游戏区 384×448（偏移 +320/+16 证据，0x46F820） |

测试（`ports/th13/tests/`，沿用现有 harness）：① 解析 golden——用第二节 front.anm entry0 数值表逐字段断言；② 指令流解码——script0 前 6 条 + 终止符 + script1 起点闭环；③ 标签扫描——`ins_5(-1)` fallback 命中；④ tick 顺序——time 门槛/结束语义/中断保存恢复；⑤ 桶序——31 桶与副链 31/32 钳位。

## 八、贴图提取方案（已实测）

容器内命令序列（产物不入库，`ports/th13/.gitignore` 已有 `**/artifacts/`）：

```bash
bash scripts/thtk-build.sh                       # 临时编译 thtk（自带 libpng/zlib-ng 源码构建，容器无系统 libpng 已确认无碍）
mkdir -p artifacts/reference/png && cd artifacts/reference/png
for f in ../../raw/*.anm; do /work/tools/thtk-tmp/build/thanm/thanm -x13 "$f"; done
bash scripts/thtk-clean.sh
```

实测记录（本轮，容器 `localhost/th13-dev`）：

- `thanm -x13 effect.anm` → `effect/eff_switch.png` 等 5 张，`file` 报 `PNG image data, 8-bit/color RGBA, non-interlaced`，尺寸与 entry 声明一致（64×64/256×256 等）；
- `thanm -x13 front.anm` → 按 name1 路径分目录 `ascii/ effect/ face/ front/`；
- `thanm -x13 text.anm`（`@R`/`@`、hasData=0）→ exit 0、零输出（外部名/渲染目标条目静默跳过）。

运行时加载：复制 `ports/th10/portable/sdl/third_party/stb_image.h`（v2.08）到 `ports/th13/portable/sdl/third_party/stb_image.h`，include 方式照抄 th10 `web/cpp/sdl/FileHost.cpp:13`，解码 `stbi_load_from_memory(bytes,size,&w,&h,&ch,4)` 强制 4 通道（th10 `FileHost.cpp:75` 同款）。PNG 由构建期（`portable/build.mjs`）打进 web 产物，仓库不存图像。

## 九、风险清单

| # | 风险 | 应对 |
|---|---|---|
| 1 | `ins_600`（dump 10 处）在 0x462450 无对应 case，落入默认分支，原版行为未定 | oracle 直调验证后再定实现；先按未实现报错 |
| 2 | timescale≠1 时的取整路径（0x4971F0）未逐指令验证 | 先按 th15 `Timer` 语义实现，oracle 对拍分数帧场景 |
| 3 | 中断保存/恢复的 time 结构（vm+0x00）字段语义部分推断 | 用 oracle 脚本做「中断→op7 返回」时序对照 |
| 4 | `@R` 渲染目标（text.anm×2）需要离屏纹理，th15 有 AnmOverlay 先例但 th13 用法未对照 | 排入 oracle 清单；首版允许空白纹理占位并显式日志 |
| 5 | 全局 sprite 序号依赖 entry 追加顺序，解析器实现顺序偏差会造成 ins_300 错位 | golden 测试断言 front.anm 全 153 sprite 序号连续 |
| 6 | thanm -x 对 hasData=0 静默跳过，外部名纹理需与 name1 建映射 | 提取脚本输出 name→png 清单 JSON 供构建期消费 |
| 7 | ECL 侧（0x41B260）大量直调 ANM API（44 处 0x46F820），接口签名偏移若有误读会在 ECL 集成期放大 | ECL 管线方案（另文）先行对拍 create 调用序 |
| 8 | stb_image v2.08 不支持 16bpc PNG | thanm 输出 8-bit RGBA（实测），无碍 |

## 十、实施顺序

1. 贴图管线：执行第八节命令批量提取 + stb_image.h 就位（半天）；
2. `AnmResource` + golden 测试（1 天）；
3. `AnmVm`/`AnmInterpreter` + 指令/标签/时间测试（2–3 天）；
4. `AnmManager`/`AnmRegistry` + 生命周期/桶序测试（2 天）；
5. oracle 对照点：`ins_600` 行为、中断时序、`@R` 用法（并入 oracle 清单）。

## 验证记录（本轮执行）

- 统计与结构：`python3` 解析 `anm-list.txt`（55 文件/372 entry/2480 sprite/1832 script/格式集合；script 计数以 `grep -c '^script script' anm-list.txt` 为准）与 `front.anm` 头部逐字段解码；脚本 0 的 -1 终止符与 script1 偏移闭环。
- Ghidra：`functions/00462450.c`（分发/时间/中断）、`004621b0.c`、`00461e80.c`、`00461a60.c`、`00461cb0.c`、`0046f170.c`、`0046f240.c`、`0046f3f0.c`、`0046f690.c`、`0046f820.c`、`0046f990.c`、`0046fab0.c`、`0046fb90.c`、`0046fca0.c`、`0046fea0.c`、`0046dca0.c`、`0046e0b0.c`、`0046e230.c`、`0046e620.c`、`004706c0.c`、`00403800.c`、`004016c0.c`、`00467080.c`。
- 提取：容器内 `bash scripts/thtk-build.sh` → `thanm -x13 {effect,front,text}.anm`（输出与 `file` 类型已核）→ `bash scripts/thtk-clean.sh`。
- 依赖：容器 `pkg-config --exists libpng` 为 MISSING，thtk `WITH_LIBPNG_SOURCE=ON`（CMakeLists 官方仓库确认）源内构建覆盖，实测无碍。
