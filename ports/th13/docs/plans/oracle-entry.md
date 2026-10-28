# TH13 原版可直调入口施工图（Unicorn oracle）

来源：th13.exe Ghidra 导出（`artifacts/decomp/jp/functions.json` 调用图 + `functions/*.c`）、二进制 xref 扫描（.text 内 4 字节 VA 与 E8/E9 相对调用全量扫描）、RTTI vtable 枚举（.rdata COL→TypeDescriptor）、容器内 Unicorn 探针实测（`oracle/machine.py` 装载 `games/th13/th13.exe`）。已验证项均附命令与输出，见文末验证记录。

## 一、启动链（静态还原，oracle 不复刻）

```
entry 0x47bda0
  └→ ___tmainCRTStartup 0x47bc33（functions/0047bc33.c：GetStartupInfoW/__heap_init/__cinit…）
       └→ WinMain 0x45c110（functions/0045c110.c）
            ├→ 0x43a440  游戏主任务链构建（TaskInf 链：0x439ee0/0x43a040/0x43c4b0/0x43c600/…）
            ├→ DAT_004dc6a4 = Direct3DCreate9(0x20)；设备句柄 DAT_004dc6a8
            └→ 帧循环（按窗口模式三选一）：0x45cef0 / 0x45d2f0 / 0x45d570
                 └→ 0x45d2f0 每帧：0x470af0 任务链 run + D3D vtable 间址（BeginScene/Present，全可桩）
```

- 任务链运行器 **0x470af0**（调用方 = 三个帧循环 + 0x45bbf0/0x45ca80）：迭代 TaskInf 链，`ecx=[task+0x24]`（owner）、`edx=[task+8]`（run）后 `call edx`，即 **run(owner@ECX)**；返回值 0..8 走 `jmp [eax*4+0x470c08]` 状态表（0x470b30–0x470bc4 反汇编实证）。
- 注册接口：0x470990（运行链）、0x470a40（关闭链），24/26 个调用方（functions.json）。
- 结论：WinMain 依赖 D3D/窗口/线程（0x45fe60 渲染线程），oracle 不从启动链进入，直接调子系统入口；CRT 堆同样不初始化，malloc 走覆写（见 §四.2）。

## 二、RTTI 类与 vtable 地图（.rdata 实测枚举）

MSVC RTTI：`[COL_ptr][方法…]`，COL+0x0c 直接是 TypeDescriptor 绝对 VA（本二进制实测非 RVA）。游戏类：

| 类 | vtable | 方法（slot 偏移） |
|---|---|---|
| SptResourceInf | 0x4aabec | +0 0x4795b0（SCPT 基类解析）、+4 0x417850 |
| SptInf | 0x4aabf8 | +0..+0x10 0x4174d0/0x417660/0x417670/0x417680/0x417690（全 ret 0 桩）、+0x14 0x417800（基类 dtor） |
| EnemyInf | 0x4aac14 | +0 **0x41b250**（ECL run）、+4 0x420380、+8 0x420b80、+0xc 0x420d00、+0x10 0x421670、+0x14 0x417d20（deleting dtor） |
| EclResourceInf | 0x4aac30 | +0 0x4795b0、+4 0x418990（ANIM/ECLI chunk 解析）、+8 **0x417e20**（load） |
| LaserDataInf/Line/Curve/Infinite/Beam | 0x4ab064–0x4ab204 | 各 24 槽（激光族，第二阶段后） |
| Pbg::File / IFile / MemFile / ResFile | 0x4acb68/0x4acb90/0x4acc5c/0x4acca4 | 8–9 槽文件抽象 |

全局单例指针：`DAT_004c2174`（0x9752b8B 根对象，←0x40d920）、`DAT_004c2180`（0x2cB，←0x415570）、`DAT_004c2188`（200B 敌机 ctl，←0x418d30）、`DAT_004dc688`（0xf53c48B ANM mgr，←WinMain malloc+0x46bc60）、`DAT_004c2194`（游戏 ctx，0x418ef0 读取）。

## 三、已验证可直调入口（本机容器探针通过）

### 1. rand() = 0x47BBA2（已锁，本次复验）

- 约定：cdecl、无参；返回 EAX & 0x7FFF。
- 前置：`override_call(0x47FE5A)`（_getptd）返回伪造 ptd，`ptd+0x14` 即种子（等价 srand(1)）。
- 本次输出 `[41, 18467, 6334, 26500]` ✓（与 MSVC 常量表一致）。

### 2. _malloc = 0x47A0ED（所有深入口的公共前置）

- 约定：cdecl，`[esp+4]`=size，返回 EAX=ptr。CRT 实现（HeapAlloc 基），未初始化堆会崩（实测 FETCH 0x0）。
- 建议桩：`override_call(0x47A0ED)` 读 `[esp+4]`，从 HEAP 区后半 bump 分配（16 对齐）。实测使 0x4795b0 全程跑通。

### 3. SCPT（ECL 容器）解析 = 0x4795b0（SptResourceInf vtable slot0）

- 约定：**thiscall 变体：ECX=this，[esp+4]=buffer 指针，`ret 4`**（0x4795b0 反汇编：`mov esi,ecx; mov ecx,[ebp+8]`）。
- `this` 最小布局（合成对象即可）：
  - `+0x00` vtable 指针（slot1 [+4] 须为可递归的解析函数；基类场景填 0x4795b0 自身）
  - `+0x04` 当前槽位号（入口时 0；出口 +1）
  - `+0x08` 累计 timeline 数（入口 0）
  - `+0x0c + 槽位*4` 各槽 SCPT buffer（slot0 填目标 buffer；slot1 填全零页，使 `word[slot1+6]==0` 跳过 ANIM 递归）
  - `+0x8c` 出口：malloc 出的 `{char* name; u32 绝对偏移}` 表
- 语义：校验 magic `SCPT`（0x54504353）与 `word[+4]==1`；`word[+0x10]`=timeline 数；偏移表位于 `0x24 + word[+6]`；随后槽位号 +1，若 `word[下一槽 buffer+6]!=0` 经 vtable slot1 递归解析 ANIM/ECLI chunk（0x4797b8 处 `call [vtable+4]`）。
- 实测：st01.ecl（36236B）→ 返回 0，`this+8`=59，表前 3 项 `GirlA01@0x…3d0 / GirlA01_at@0x…598 / GirlA01_at_l@0x…77c`，末项 `main@0x…89f0`——与 raw 头部偏移表逐一吻合。

### 4. ECL 指令分发器 = 0x41B260（17.6KB / 491 call）

- 约定：**ECX=vm**（即 EnemyInf+0x11ec），无栈参；SEH 帧 + cookie（0x41b27a 读 `0x4b9c34`，栈可写即可）。
- vm 关键字段：`+0x40c8` → `pA`；`[pA+4]` → `pB`；`[pB+4]` = 指令流地址。指令布局（raw 逐字节对齐实证）：`{u32 time; u16 opcode; u16 size; 参数…}`，分发器读 `word[流+4]` 为 opcode，合法域 `0x12c..0x3eb`（`edx=opcode-0x12c; cmp edx,0x2bf; ja 0x41f738`）。
- `0x41b250`（EnemyInf vtable slot0）= `add ecx,0x11ec; jmp 0x41b260` 的包装。全 .text 扫描（E8/E9/4 字节 VA）**未发现运行时 slot0 调用点**——该槽仅被 vtable 引用；oracle 直接调 0x41b260，绕开此问题。
- 实测：vm=0x40f8 零填充 + `pB+4 → st01.ecl+0x3f4`（time=0, opcode=0x1f4=ins_500）→ 执行 0x41d1d5 案分支（含 CRT 数学 0x478ee0）后干净返回 EAX=0，无异常。
- 同族只读入口：子时间轴解析 0x421b20/0x421b70（读 vm+0x40c8 链做标签→地址换算）。

## 四、敌机子系统（第二阶段目标，依赖链完整）

### 1. 敌机管理器

| 入口 | 地址 | 约定/参数 | 前置 |
|---|---|---|---|
| 创建 | 0x418d30 | cdecl `[esp+4]`=**ECL 文件名**（caller 0x4145c0 从 `iVar1+0x34` 起的名字表按 `iVar1+0x114` 下标取出；0x42b990 传 `DAT_004c22dc[2]`），经 0x4187d0 内虚调用 EclResourceInf vtable slot2（0x417e20 load）消费 | `DAT_004c2174`（根对象+0x9752b4 处读 bullet-mgr 指针）、`DAT_004c2180`（+0x10 被读）、任务链全局可注册（0x470990/0x470a40 直接写全局链头，需先有链对象）、0x4724c0 资源缝或其 override |
| 逐帧更新 | 0x418e00 | **EDI=ctl**（200B，DAT_004c2188）；无栈参 | ctl+0xb0 敌机链表已建 |
| 更新回调 | 0x418ef0 | ECX=owner（任务链注入）；内部再查 `DAT_004c2194` 门控 | 注册于 0x4187d0 内 |
| 关闭回调 | 0x418f30 | 无参，`mov eax,1; ret`（桩） | — |

0x418d30 行为：malloc 200B → memset → `DAT_004c2188=ctl` → 调 0x4187d0（分配 0x1098B EclResourceInf，vtable 0x4aac30，挂 ctl+0xac；注册两个 TaskInf：run=0x418ef0/0x418f30；ctl+0x98 置 -99999、+0x9c/+0xa0 置 0、+0xc4 置 99999）。

0x418e00 帧行为（0x418e00–0x418edd 反汇编）：

```
遍历 ctl+0xb0 链表节点 esi：
  eax=[esi]（敌机）；敌机 flags(eax+0x521c) & 0x4000000 → 销毁支路
  否则 call 0x41a8a0(eax+0x11ec)   ← EAX=vm 的寄存器约定
    返回非 0 → 销毁支路：vtable slot5（[vtable+0x14]=0x417d20）push 1 后调用
帧率计数：ctl+0x9c/+0xa0 与 ctl+0xa4 指向的 rate 表（同 0x401400 模式）
```

### 2. 单敌机（EnemyInf，0x52e4B）

| 入口 | 地址 | 约定/参数 | 说明 |
|---|---|---|---|
| 构造 | 0x418f40 | **EDI=this**、EAX 路径（内部 `lea eax,[edi+8]` 传 0x417450）；`[esp+4]`=生成参数首字节 | 设基类 vtable 0x4aabf8 → 0x4aac14；memset VM 0x40f8B@+0x11ec；登记 DAT_004c2188（+0xbc/+0xc0 序号） |
| 生成 | 0x4183f0 | `[esp+4]`=param1（透传 ctor）、`[esp+8]`=param2（0x15 dword：pos xyz/life/score/掉落/flags 等，函数体逐字段拷贝实证） | malloc 0x52e4 → ctor → 填参数 → call 0x41a8a0 首帧 |
| 析构 | 0x417d20 | vtable slot5，`[esp+4]`=1 时 delete | 0x418e00 销毁支路使用 |
| VM tick | 0x41a8a0 | **EAX=vm** | 见下 |

0x41a8a0（敌机 VM 逐帧，探针未覆盖、静态链完整）：重入标志 `vm+0x4030.0x80000`；依次 0x419450(vm 栈参，插值) → `vm+0x40e8` 可选回调 → 0x419a90(vm 栈参，async 子时间轴调度，内调 0x4218f0 查 vm+0x40c8 名表) → 0x41a3d0(激光) → ANM 位姿同步：`0x46fb90(栈参=sprite_id, EDX=DAT_004dc688)` ×14 槽，返回 NULL 则跳过同步（**ANM mgr 缺席不致死**，0x41a966/0x41aa4c `test eax,eax; je` 实证）。

### 3. 资源装载（绕开文件系统）

| 入口 | 地址 | 约定/参数 | oracle 桩 |
|---|---|---|---|
| Pbg 按名打开 | 0x4724c0 | cdecl：`[esp+4]`=名, `[esp+8]`=&size, `[esp+0xc]`=flags；返回 buffer | **核心缝**：`override_call` 按名查表返回 emulated 内存里的 raw dump（`artifacts/reference/raw/`），写 `*size`。真实现先查 `DAT_004e50c8` 名录再落 CreateFileA（全被导入桩拦） |
| 归档根打开 | 0x457cb0 | EBX="th13.dat"、EAX=0x4e50c8（寄存器传 this）；Pbg::File vtable 0x4acb68 slot0 虚调用 | 不需要（0x4724c0 已盖住出口） |
| EclResourceInf::load | 0x417e20 | ECX=this、`[esp+4]`=名字符串 | 名字复制到 0x4e536f 后即走 0x4724c0+0x4795b0，全链可跑 |
| .ver 补丁链 | 0x439f90 | 无参 | 忽略 |
| ANM 注册 | 0x46df40 | EAX=ANM 编号 | 内部 Sleep 等线程完成；oracle 直接预填 `DAT_004dc688+0xb77b34+id*4` 或整体覆写 |

### 4. ANM（第三阶段）

- ANM mgr：`DAT_004dc688` ← WinMain `malloc(0xf53c48)` + **0x46bc60**（构造，2989B）。sprite 槽访问 **0x46fb90**：`ret 4` 单栈参 id、EDX=mgr，返回 sprite 对象。
- ANM VM 执行 **0x462450**：ECX=sprite（读 `[sprite+0x590]`），84 个调用方；ECL VM 插值步 0x419450 内部亦调用它。
- 本阶段未做探针实测（mgr 未立），地址与约定来自反汇编。

## 五、oracle 施工顺序

1. **桩基线**（已有）：PE 装载 + 导入桩 + `override_call`；新增 `_getptd`、`_malloc` 两个覆写（§三.1/2）。
   - 注意：`oracle/machine.py` 的 `call()` 现把参数写在 `[esp+0..]`、哨兵在 `[esp+n]`——对 0 参目标成立（rand 探针即证），**对带栈参目标布局应为 `[esp]=RET_MAGIC、[esp+4..]=args`**（本次探针即按此绕开；改造点在 machine.py，本任务只读未改）。
2. **ECL 管线对拍**：合成 `this` → 0x4795b0(st01.ecl) 得 59 时间轴 → 合成 vm（0x40f8 零填充 + `vm+0x40c8` 三级链）→ 循环 0x41b260 → 与移植 VM 对拍每帧敌机态。深入时把 `pB+4` 指到 `main`（0x89f0）或 GirlA01（0x3d0）起始。
3. **敌机管理器全链**：0x418d30 → 0x4183f0 生成 → 每帧 0x418e00（或直接 0x41a8a0 单敌机）。前置补：DAT_004c2174/DAT_004c2180/DAT_004c2194 占位对象、任务链头（0x470990 注册写入的全局）。ANM mgr 可缺省（同步自动跳过）。
4. **ANM 接入**：0x46bc60 立 mgr（或合成）→ 0x462450 对拍；ECL 的 ins 里创建 sprite 的指令自此可跑全。
5. **资源缝**：0x4724c0 override 全量接管按名装载（ecl/anm/std/msg 一并通过 raw dump 喂入）。

## 六、未决与边界

- EnemyInf vtable slot0（0x41b250）的运行时调用方未在静态调用图定位（.text 无 slot0 间址调用、无 E8/E9/VA 引用）；不影响 oracle（直调 0x41b260 已实测）。若后续动态追踪发现真实分发点，更新本表。
- 0x462450/0x46bc60/0x4183f0/0x418d30 未做探针实测（前置对象未立），地址与约定均出自反汇编；接入时按 §五.3/4 顺序验证。
- CRT 完整启动（__heap_init/__mtinit 等）不在 oracle 路径，全部以覆写/桩替代。
- `artifacts/decomp/jp/refine-report.json` 存在 JSON 尾逗号（941 项 artifact 待修，见 git log af9a3f2），未影响本次 functions.json 使用。

## 七、验证记录（本机容器实测）

命令均形如 `podman run --rm --userns=keep-id -v <workspace>:/work:Z -w /work localhost/th13-dev bash -lc 'python3 <探针>'`，探针脚本临时挂载 /tmp，未入库。关键输出：

1. RNG（复验）：`[41, 18467, 6334, 26500]` ✓。
2. xref 扫描：`th13.dat@0x4ab38c` → 唯一代码引用 0x439fa6（0x439f90）；`bullet.anm` 5 处；`../../data/*.ecl` → 0x4146d0/0x44b410（eclview 调试遗物）。
3. RTTI vtable 枚举：23 个游戏 vtable 全量（§二表为其子集）。
4. 0x4795b0 直调：`ret=0, subcount=59`，名字表 `GirlA01/…/main` 与 raw 偏移一致（含 0x10003d0=buf+0x3d0）。
5. 0x41b260 直调：block 追踪 `0x41b260→0x41b2cf(分发)→0x41d1d5(ins_500 案)→…→0x41f75a` 干净返回 EAX=0。
6. malloc 缺失时 0x4795b0 崩溃路径（FETCH 0x0）与 bump 覆写后跑通——桩必要性实证。

## 八、stage1 oracle 全窗实跑记录（2026-10-21，容器实测）

工具：`oracle/stage1_oracle.py`（装载→0x418d30 建管→0x4183f0 建 root→0x418e00×3600），
`oracle/stage1_compare.py`（逐帧逐字段比对）。命令与输出：

```
python3 -u -m oracle.stage1_oracle
  → stage1-oracle frames=3600 enemies=85 events=10187
    (spawn=84 move=7611 bullet=2492) malloc=347/30668KB seam=5   # 2.5s
python3 -m oracle.stage1_compare \
  eagler-touhou/ports/th13/web/artifacts/wasi/stage1-events.jsonl \
  eagler-touhou/ports/th13/web/artifacts/wasi/stage1-oracle.jsonl
  → RESULT first_divergence_frame=60 window=3600 events=11
```

第 0 帧事件两侧逐字节一致（MapleEnemy 全字段）；首个分歧在帧 60/61。本轮沉淀的事实：

1. **敌机"落格帧"**：0x4183f0 构造内首趟把 vm+0x4030 置 0x80000，清除只发生在
   0x418e20（0x418e00 成功趟尾 `&= 0xfff7ffff`）。因此建敌后的**第一趟 mgr 更新对该敌
   是空趟**（0x41a8a0 见位即跳）：root 首个真实更新趟在第 2 帧。移植侧 StageSim 无此
   语义——60/61 帧差与后续全部时间轴偏移的根源。
2. **被调方清栈约定**（覆写桩必须带 ret_pop，错一字节即栈错位、症状散布远处）：
   0x4724c0 资源缝 ret 0xC；0x460c50 音效 ret 4；0x419a90 死亡结算 ret 4；
   0x46f820 死亡环弹 ret 0x18；0x47a0ed _malloc/0x47a181 _free 为 cdecl。
3. **建敌参数块**（0x41fcc0 → 0x4183f0 param2，0x15 dword）：[0..2]=x/y/z、[3]=分值
   （1000）、**[5]=life（int 位模式，0x417500(ctx,3) 解析 postname 第 2 参）**、
   [6]=镜像位、[8..19]=ANM 遗产、[0x14]=创建者 id。
4. **弹体 z**：0x40e840 在写入出生坐标后无条件把节点 z 覆为 0.1f（0x40edd6 前后），
   oracle 按原版导出 z=0.1；契约 §4.3 的 z 语义与之不符，属契约修订项。
5. **CRT 无头基线**：crtheap 占位（0x4c1d40=1）+ HeapAlloc 桩 + __mtinitlocknum 报成功
   + EncodePointer 返回可调用 ret 桩 + 预跑 __initptd(0x47fd2d) 后覆写 _getptd——
   sprintf 的 locale 路径（_LocaleUpdate）即依赖真 ptd。
6. **调度器/全局占位**：DAT_004dc658 任务链锚（+0x14 运行链/+0x3C 关闭链，0x60 零块）、
   DAT_004c2178 分数、DAT_004c2190 血条显示（+0x198 被读）、DAT_004c2194 更新门（0=关）。
   0x419a90 整体空转（stage1 无击杀）；0x461cb0/0x462450（弹体视觉 ANM）空转。
7. **GameRandom**：状态对象 0x4dc324（u16 种子 + u32 计数），0x420380 的 RAND case 以
   `mov esi, 0x4dc324` 装载；EXE 镜像初值 0，与契约 §2 的种子 0 一致。
8. **探参法**：0x41b260 入口停机 → 以同状态直调 0x478e60（EDX=thread、[esp+4]=index、
   EAX=值）/ 0x478ee0（EAX=thread、ECX=index、ST0=值，经 fstp 转换壳取回）→ RNG 快照
   回滚（ptd 种子 + 0x4dc324）→ 恢复寄存器重入分发器。分发器内 140 处取参全部经这
   两个函数，无第三变体。
