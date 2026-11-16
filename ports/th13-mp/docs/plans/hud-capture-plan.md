# 对局窗 HUD 捕获面扩展方案（S2 前置研究报告）

事实源：`oracle/render_oracle.py`（现行对局窗，路 B 直调）、`oracle/title_oracle.py`
（路 A 真机驱动，已验证）、`oracle/stage1_oracle.py`/`stage1_compare.py`（事件流契约）、
`artifacts/decomp/jp/functions/`（反编译）、th13.exe 字节级核对（暂停脚本号）。
本报告纯研究，不改代码。缩写：RO=render_oracle，TO=title_oracle。

## 0. 现状缺口（为什么现有 draws 窗捕不到 HUD）

RO 的每帧驱动（RO:12-21 注释、RO:953-988 render_frame）只做四件事：
`0x418e00(EDI=ctl)` 敌机逻辑 → ANM mgr 增量装载 `0x46e510` → 主/副池步进
`0x46f170/0x46f240` → **手工逐层** `0x46f380(EAX=层, EDI=mgr)`×57 → UP 刷 `0x4679a0`。
装载也是手工拼的（RO:1004-1076：0x43bbc0 gamestate、假设备、0x46bc60 mgr、
0x4038a0 game_main、0x40d920 弹幕 root、0x46df40 effect.anm、MGR_CREATE 0x418d30、
SPAWN root）。**场景级任务（调度器 DAT_004dc658 双链上的 update/draw 任务）从未
被驱动**，因此以下绘制族全部缺席：

```
真机一帧 = 0x45d2f0:27-52
  0x470af0(EBX=sched)  update walk：场景机 0x439ee0→0x43c1c0、各子系统 update 任务、
                        ANM mgr 两池步进任务（mgr 构造时自注册 31 任务，TO:13-18）
  0x470c30()           draw walk（sched+0x40 链，优先级序）：
                        0x43c4b0 清屏 → 场景 draw → mgr 29 逐层任务 → 场景级直绘
```

RO 手工等价了其中 mgr 部分；**场景级 draw 任务**——对局 HUD/敌弹/ctx 叠加——
一帧都没跑过。TO 已验证路 A（真调度器+真场景机）在桩环境可行（两窗自证三件套
通过），本方案把该方法论移植到对局窗。

## 1. 对局窗真机驱动方案（battle_oracle，TO 子类）

### 1.1 初始化序（最大程度复用 TO 已验证件）

```
install_overrides()      ← TO 全套 6 修复原样复用（TO:183-255）：
  GetKeyboardState 键态注入 / joyGetPosEx=167 / GetModuleFileNameA 真路径
  / 0x459e00 影像探针 ret_pop=4 / _beginthreadex 桩返 0（线程体手动补调）
  / 0x47fde1 CRT ptd 直返 / OPEN_BY_NAME basename 兜底
  + RO 的 Encode/DecodePointer 弹参、HeapAlloc/Sleep/timeBeginPeriod 桩
init_ptd() / install_d3d() / install_hooks()      ← RO 原件（FakeD3D/D3DX/捕获）
patch_globals_title()    ← TO 调度器 19 字段构造（TO:257-290）原样复用；
                           KEYBOARD_ON=1、SOUND_MGR_FLAGS|=0x180（见 §5.1 风险）
装载序（真机代码，替代 RO:1004-1076 手工拼装）：
  Direct3DCreate9 → DAT_004dc6a4 → DEV_INIT 0x45dc80        （TO:460-466 同款）
  ANM_MGR_CTOR 0x46bc60 → DAT_004dc688                       （TO:468-472 同款）
  SCENE_TASKS_REG 0x43a440（场景 update 0x439ee0 + 10 个场景 draw 任务，
    注册时 0x470990 一次性回调 0x43a040 = 0x43bbc0 gamestate + 0x46c950 静态 VB）
    （TO:474-477 同款）
  补调 OPENING_THREAD 0x439730（开场装载线程体：sig.anm、thbgm.fmt、
    game_main 0x4038a0、**MsgInf/score 0x449b00**——score 数据是对局装载的
    前置依赖，见 §1.2）（TO:354-361 thread_bodies 同款，已验证）
  预置对局参数：DAT_004be7c4=难度、DAT_004be7b8=角色、DAT_004be81c=1、
    DAT_004be820=1、DAT_004c22dc=0x4bb56c（stage1 表，results-flow.md §2.4）
  投递场景请求：wr32(0x4dc6a0+0x584, 7)   ← 直接写请求字段（等价 0x42b990 里
    DAT_004be81c=1/表=0x4bb56c 的 0x51450 确认路径，results-flow.md §5）
  帧循环内首帧：场景机 0x43c1c0 case 7 → 0x44c530(空) → 0x42c490(0)
    （GameInf 0x7c @DAT_004c2194）→ 0x43b280 → _beginthreadex(0x42b990) 桩返 0
  oracle 补调 0x42b990（战斗装载线程体，见 §1.2）→ 0x42b990 尾部自行
    使能 GameInf 任务（0x42b990:165-168、258-264）
```

### 1.2 战斗装载线程体 0x42b990（路径 A 的核心新补调）

0x42c490 不直接装载——它经 0x43b280 `_beginthreadex(0x42b990)`（0x43b280:18）。
0x42b990 内容（全部实证于 functions/0042b990.c）：

- **头部等待环**（0x42b990:46-53）：`while (mgr+0x20 > -1) { if (DAT_004dcc5c &
  0x180) goto 失败出口; Sleep(1); }`——注意 0x180 在这里是**中止**语义（与
  0x46df40 内部等待环的「跳过」语义相反！），见 §5.1。
- score 应用分支（0x42b990:74-164，DAT_004dcc30≠0 即有 score 数据）：难度初值
  `DAT_004be7f4=2`（story）、点值底 `DAT_004be7e0/7e4`（难度表 0x4c0ab0/0x4c0ac8）、
  续关点数 `DAT_004be844`（表 0x4c0ae0）、游玩计数 +1（score+0x7c0）、
  `DAT_004be83c=0`。
- GameInf 任务注册：update=thunk 0x42cb90→**0x42c550**（对局每帧主函数）、
  draw=thunk 0x42cba0（仅清 mgr+0xc0..0xcc 四计数，非绘制）。
- 子系统创建（0x42b990:178-209）：0x447ed0(回放)、0x4065a0(自机)、0x4264f0、
  **0x40d920(弹幕 root→DAT_004c2174)**、0x42e270、0x42fc20、0x43db30、
  **0x44a8b0(FrontInf→DAT_004c22d4，敌弹)**、**0x418d30(敌机 mgr，读 stage 表[2]
  的 ECL 名)**、0x40a080、0x412c60、0x405760、0x4385d0、BGM=stage 表[3]/[4]
  （0x43adc0）、0x43af40(舞台 init)、0x43b5a0。
- 第二段装载在 **0x42c550 首帧**（DAT_004c2168==0 分支，0x42c550:71-151）：
  0x40d4c0/0x4424d0/0x413e50/0x414180/0x42b870/0x448ec0/0x4183f0/**0x425170(stage
  ctx+front.anm)**/**0x43da30(结算覆盖层)**/0x444df0/…——即真机装载分「线程体」
  与「首帧」两段，路径 A 下都会自然执行。
- stage ctx 任务注册在 0x424f00：update 0x428280→**0x4265a0**（MSG/对话/神灵槽/
  HUD 状态机）、draw 0x4282a0→**0x427910**（HUD 绘制）；**front.anm 开档在
  0x424f02（push 0x4aaca4 字节实证）→ stage ctx+0x2a8**；玩家 anm（stage 表+0x14
  按角色，plXX.anm）在 0x424fd0 → +0x14c。
- FrontInf 任务在 0x44a6f0 注册：update 0x44ad40→0x44a970、draw
  0x44ad50→**0x44aa20（敌弹绘制）**；绑定 game_main+0x19194（bullet.anm）。

补调方式：TO thread_bodies 模式（TO:353-386）新增一档——`DAT_004c2194` 非零且
`DAT_004dcec8==0` 时 drive(0x42b990)。等待环处置见 §5.1 三对策。

### 1.3 每帧驱动序（battle_frame，TO title_frame 同构，TO:347-389）

```
UP_FLUSH 0x4679a0(ESI=mgr) → wr32(0x4dcc18, GAMESTATE) →
SETVIEWPORT_HELPER 0x43b900(EDI=gamestate) → dev.SetViewport(gamestate+0xd8) →
wr32(0x4dcc1c, 1) →
0x470af0(EBX=sched)            # update walk：场景机+全部 update 任务（含敌机
                                # 0x418f04 thunk→0x418e00、ANM 池步进、0x42c550）
thread_bodies()                 # 补调 0x42b990（仅装载期命中）
BeginScene → UP_CLEAR 0x467960 → DAT_004dcf30=0xff → FOG_FLUSH 0x43b080 →
0x470c30()                      # draw walk：全部 draw 任务（优先级序）
UP_FLUSH → EndScene → Present → write_frame_line
```
**与 RO 的本质差异**：不再手工驱动 0x418e00 / 0x46e510 / 0x46f170 / 0x46f240 /
0x46f380×57——这些全部由调度器任务自跑（mgr 31 任务 + 敌机任务 + 场景任务）。
手工驱动任何一个都会双跑（§5.3）。

### 1.4 输入注入两条路

- **键态桩路（HUD/暂停窗用）**：TO 已验证——GetKeyboardState 桩每帧写受控 VK
  集（0x80 高位），位打包/边沿/连打由真机 0x471620/0x4711a0/0x474fb0 完成
  （TO:187-199）。注入 VK 表：Z=0x5A ↑=0x26 ↓=0x28 ←=0x25 →=0x27 **Esc=0x1B**
  （对局内 Esc 经 0x43dbc0:19 的 `DAT_004e49fc & 0x100` 触发暂停——bit8 的键位
  归属待一跑自证：注入 VK 0x1B 单帧读回 DAT_004e49f0 核对，TO docstring:47-53
  同法）。
- **回放 u16 流路（回放窗复刻用）**：现行回放窗（RO run_replay:1117-1160）在
  sink_frame_hook 里把回放 u16 流经 PlayerModel 换算成玩家坐标直写
  DAT_004be7xx/player+0x5B8——那是「模型旁路」。真机路径是 **0x448200 节奏取
  输入**（移植侧 replay_take_input 同源注释，ApplicationState.hpp:58-62）：对局
  中输入态由回放流注入 `DAT_004e49f0` 等价位（或键态桩按帧回放 VK 集——回放
  u16 的位布局与 GetKeyboardState 打包位不同，需按 replay.md 6 字节帧定义换算）。
  建议 S2 先用键态桩路（HUD/暂停窗不需要回放），回放窗多关化（0xc 链）时再做
  u16→键态换算（待解项：换算表需对照 0x471620 打包位序与 t13r 输入位定义）。

### 1.5 与现有两窗的兼容（不破坏 stage1_compare 契约）

- 事件流契约窗（无输入 3600 帧 + 回放 10662 帧）由 **stage1_oracle.py 独立产出**，
  与 RO/TO/battle_oracle 无共享运行时——零影响。
- 绘制契约窗（st01-draws.jsonl / st01-draws-replay.jsonl）由 RO 产出：**保留 RO
  原路径不动**，battle_oracle 产新文件（如 st01-draws-full.jsonl）——旧窗字节
  不变，draw_compare 旧对照不失效。
- 等价性论证：敌机逻辑 0x418e00 在两路径下每帧**恰一次**——路径 A 中它由
  0x418d30 注册的任务 thunk（0x418f04：检查 mgr+0x60 标志位后 `call 0x418e00`，
  字节实证 0x418f22）驱动；路径 B 由 RO 手工驱动。同一函数、同参数（EDI=ctl）、
  同频次，事件流逐字节等价。**自证门**：battle_oracle 加 0x418e00 入口 watch
  计数（每帧==1）+ 首前 N 帧事件与 stage1 基线对照。
- 双跑风险唯一来源：误保留手工驱动（§5.3 审查点）。另注意 RO 的
  patch_dispatch_table（RO:876-891）把 ECL 跳表改 no-op——**battle_oracle 禁用
  该改写**（要真实指令执行产生真实精灵/HUD 事件；no-op 集是事件流基线的产物，
  与绘制捕获面冲突）。

## 2. HUD/玩家/弹幕/ctx 层捕获面清单

### 2.1 op 族全表（0x470c30 draw walk 下新增的提交路径）

| # | 族 | 提交锚点（链） | 纹理来源 | op 形态 |
|---|---|---|---|---|
| 1 | mgr 层 VM（敌机/弹幕外观/关卡logo/对话框线） | 0x46f380→0x46a700→0x467d80/0x468130/…/0x46ad00/0x46afa0（0x46a700:29-129 按 vm+0x18>>0x19 类型分发 15 路） | mgr 26 槽 anm | DrawPrimitive quad / UP（**已捕获**，但 battle 期新开的 plXX/st0Nenm 等槽是新增量） |
| 2 | **stage ctx 5 桶叠加**（对话立绘/横幅/星/信仰槽 VM） | **0x4032d0:76-83**：`0x46a700(EAX=gamectx+0x58c+i*0x5a4, push mgr)` ×4，+0x1c34==2 时第 5 桶（+0x1690） | effect.anm 模板（桶结构由 0x402ff0 从 gamectx+0x10=file+0x10c+0x5a4 拷 0x14c dword 初始化，0x402ff0:36-100） | 桶绘制（同族 1 的 15 路分发） |
| 3 | ctx 直绘调试/练习边框 | 0x4032d0:32-74：renderer vtable +0xe4(SetRenderState)×8、+0x10c×8、+0x164、**+0x14c DrawPrimitiveUP(5 顶点, FVF 0x44)** | 无纹理（纯色/边框） | UP ×1 |
| 4 | **HUD 文本**（分数/HiScore/帧数/Graze 列） | 0x427910(stage ctx draw)→**0x403e70** vsprintf("%3d."/"%.2ds")→0x403da0、0x403ec0(值+0x2ac)、0x403fd0（0x427910:47-149） | ascii.anm/文本图元 | 文本 quad（经 game_main/AsciiInf 桶或 UP，具体路径待捕获后归类） |
| 5 | HUD 图形 quad（信仰槽/框/黑带） | 0x427910→**0x45efc0**（vtable +0x10c×4、+0xe4(0x14,6) 混合、+0x164、+0x14c UP 5 顶点，0x45efc0:10-27） | 无纹理 | UP ×N |
| 6 | 残机/灵击/符卡数/难度标 | stage ctx update 0x4265a0 创建的 front.anm VM（script30-37 残机、38-45 灵击——GameHud.cpp:2 已锚，dump anm-list.txt:6643 附近） | front.anm（stage ctx+0x2a8） | mgr quad（族 1 路径，**bank 为 front** 是新增纹理来源） |
| 7 | 神灵/灵界槽动画 | 0x4265a0:225-260（每槽 0x54 步长、+6 偏移起 8 槽 VM） | front.anm | mgr quad |
| 8 | **敌弹批** | **0x44aa20(FrontInf draw):95-110**：0x40 步长池遍历→0x467b50 顶点填充+**0x467350(mgr,1) UP 批提交** | bullet.anm（FrontInf+0x10←game_main+0x19194，0x44a6f0:8） | UP 批（每池一批） |
| 9 | 自机弹/自机特效 | 0x40e4d0/0x40e630（各 0x46a700×2，0x40e4d0:30/45） | plXX.anm/bullet.anm | 桶绘制 |
| 10 | 玩家精灵 | 自机 ctx 0x4065a0 系 + mgr 层 VM（plXX.anm 对局期开槽，路径 B 从未装载） | plXX.anm | mgr quad |

捕获侧无需新增 op 类型：FakeD3D 已覆盖 DrawPrimitive/DrawPrimitiveUP/SetTexture/
SetRenderState/SetTransform（RO:380-476），族 3/5 的无纹理 UP 也能落 op（tex=0）。
**层号语义扩展**：0x46a700 调用不经过 0x46f380 的层 watch——族 2/8/9 的 op 落盘时
`cur_layer=None`。方案：给 0x46a700 增设入口 watch 记录「桶调用源」（返回地址或
EAX 范围 → 标签 ctx0..ctx4/front/pbullet），draw-contract 的 layer 字段扩展为
`layer`+`src` 双字段（移植侧同构导出）。

### 2.2 HUD 窗设计（窗 C）

boot：§1.1 序（含开场线程补调）+ 请求场景 7；prefix 剥离 = **首个「对局 ops>0」帧**
（战斗装载完成后，判定依据：场景 current(+0x580)==7 且 frame_ops 非空）。

| 窗内帧 | 输入 | 预期覆盖 |
|---|---|---|
| 0-299 | 无 | HUD 稳态：分数列/HiScore/残机灵击 script30-45/信仰槽/st01 首波敌机+敌弹批 |
| 300-389 | Z 按住 | 自机弹族 9、得点/灵吸收（HUD 数值变动）、自机移动可选 ↑← 组合 |
| 390-509 | 无 | 回稳态（含射击后的 HUD 回落） |
| 510 | Esc 1 帧 | 暂停触发（0x43dbc0 case 0→0x43e3f0）：front script75 底板+132 菜单起绘、对局冻结 |
| 511-699 | 无 | 暂停菜单入场动画 |
| 700 | ↓ | 光标移动（interrupt 标签族） |
| 760 | ↓ | 第二项 |
| 900 | Esc | 关闭暂停（等价「继续」），对局恢复 |
| 901-1199 | 无 | 恢复后稳态（暂停收场动画） |

窗长 **1200 帧**。中弹（残机-1/无敌闪烁）不在本窗：真机判定的死亡会走
复活/掉落逻辑，输入可控性差——由**回放窗**（已含玩家死亡路径）覆盖死亡 HUD 态；
若 S2 后仍缺，再开窗 C2（f300-420 按住 ↑ 冲撞已知弹幕——弹位确定性由无输入窗
3600 帧基线保证）。
门禁自证三件套同 TO：同参数两跑 sha256 一致 / 窗内每帧 ops>0 / 帧数=窗定义值；
另加第四件：0x418e00 每帧计数==1（双跑防护，§1.5）。

移植侧 harness 对应模式：`run-harness.mjs` 新增 `hud` 模式（输入时刻表内置于
Application 侧 replay/键态注入接口——现有 step(held,pressed,repeated) 即键态路，
帧时刻表由 harness 脚本侧按上表注入），输出 `port-draws/hud-draws.jsonl`。

### 2.3 暂停窗设计（窗 D，可并入窗 C 的 f510-901 段）

独立窗的价值：暂停菜单全部 interrupt 标签序列的独立基准。设计（同 boot，
prefix 同窗 C）：

| 窗内帧 | 输入 | 覆盖 |
|---|---|---|
| 0-239 | 无 | 对局稳态锚定 |
| 240 | Esc | 暂停开（script75+132） |
| 300/360 | ↓×2 | 菜单导航（三项目前两项） |
| 420 | ↑ | 回第一项 |
| 480 | Z | **选中「继续」**（cursor 0 → 0x43e700 恢复路径——非破坏性） |
| 481-839 | 无 | 收场动画+恢复 |

窗长 840。**不按破坏性项**（重开=0xa 重装载、回标题=场景 4 转场——那属于
results-flow 的场景链窗，另行覆盖）。移植侧 harness `pause` 模式同构。
暂停菜单脚本号（字节级实证，§3）回填后 PauseMenu 的 interrupt 标签序列
（1=收起/3=展开/7+光标=选中，PauseMenu.cpp:40-63）以 oracle 侧捕获的
ins_ 标签序列校正。

## 3. 暂停菜单 front.anm 脚本号（从代码+dump 解出）

- 触发链：0x43dbc0 case 0（`DAT_004e49fc & 0x100`（Esc 边沿）或 `DAT_004dcc5c &
  0x10`（失焦自动暂停））且 0x441a10()（对局任务使能）且帧>0x1d → **0x43e3f0**
  （暂停/结算A 覆盖层打开，置 GameInf 0x10 冻结对局）→ 状态机 0x43e780
  （=results-flow.md §2.3 结算A，暂停菜单与其同体）。
- 脚本号（push 立即数字节实证）：
  - **0x43e474: push 0x4b** → front.anm **script75**（底板/暗幕，dump 行 8041）
  - **0x43e504: push 0x84** → **script132**（常规暂停菜单根，dump 行 10242）
  - **0x43f4f0: push 0x85** → **script133**（练习模式菜单根，dump 行 10261）
  - GameOver 入口（0x43f570→0x43f633 同样 push 0x4b=75）后由 0x43fa90 态 0xf
    建菜单，变体族 **142/143/147/148**（dump 行 10774/10793/11060/11076）待
    GameOver 窗定档。
- 菜单项子脚本：script132/133 均 `ins_304(25)` 后 `ins_501(script117); 
  ins_501(script118)`（+119 三项变体，dump 行 9198/9237/9267）——117=「继续」、
  118/119=后两项（文案 cp932 在 anm 内，标签语义待捕获后比对）。
- 移植侧现状：`kPauseMenuScript = 0`（PauseMenu.hpp:13 占位）→ 回填
  **132（Pause）/133（练习）**，底板 75、子项 117/118/119；PauseMenu.cpp:59
  猜测的 {123,132,144} 子脚本集应以实测 ins_501 序列替换。

## 4. 移植侧清单（对照 results-flow.md 场景表，只列不动手）

1. **ApplicationState.hpp**：Destination 回填（results-flow 清单 1）后，
   `Run=7` 与本方案 boot 投递一致；`begin_run` 增「对局装载序对齐」注释锚
   （0x42b990 子系统表 §1.2 为核对清单）。
2. **RunScene.cpp**：st01 硬编码多关化（results-flow 清单 2）之外，新增三绘制族
   接入 AnmDrawSchedule：stage ctx 5 桶（§2.1 族 2）、HUD 文本（族 4，GameHud
   update_score 已有雏形）、HUD 直绘 quad（族 5）。**优先级表回填**：现
   kRunUpdatePriority=20/kRunTextPriority=76/kStageDrawPriorities{12,17,19,43}
   均为「th15 同族布局推测」（RunScene.cpp:8-9 注释自认）——battle_oracle 跑通
   后从 0x470c30 实际执行序（0x470ff0/0x470a40 注册时的优先级参数）回填真值。
3. **GameHud.cpp**：难度标签/通知句柄（GameHud.cpp:87 待锚）→ front.anm
   script 待 HUD 窗捕获后定位；神灵/灵界槽（族 7）分期接入。
4. **敌弹渲染**：FrontInf 同构（族 8：池遍历→顶点批→UP 提交）；当前
   StageBattle 弹幕遍历位对齐 0x44aa20 的池序与批切分。
5. **RunPause/PauseMenu**：kPauseMenuScript 0→132/133；底板 script75；子项
   117/118/119；GameOver 变体 142/143 系待其窗口后再接。
6. **tools/run-harness.mjs**：新增 `hud`/`pause` 两模式（§2.2/§2.3 输入时刻表），
   输出与 oracle 新窗同名同窗。
7. **oracle 侧**（研究伴生件，实现在 S2）：battle_oracle.py（TO 子类）+ 
   draw-contract.md 的 layer 字段扩展（+`src` 标签，§2.1）。
8. **覆盖矩阵更新**：对局窗未覆盖清单（HUD/敌弹/ctx 桶/暂停）逐项销账。

## 5. 风险与坑

### 5.1 最大风险：0x42b990 头部等待环 × 0x180 预置语义冲突
TO/RO 均预置 `DAT_004dcc5c |= 0x180` 破 0x46df40 族等待环；但 0x42b990:46-53
的头部等待环把同一位置当**中止**（goto 失败出口 LAB_0042be39：置错误标志、
禁任务、返 -1）。若沿用 0x180 预置直调 0x42b990，装载必失败。三对策（按优先序）：
(A) mem-read hook：drive(0x42b990) 期间挂钩 mgr+0x20 读取强制返回 -1 → 环零迭代
（干净、无补丁字节）；(B) 预置清 0x180，装载推进改由「分段驱动 0x42b990」
（跑到首个 0x46df40 后停下 load_finish 再继续——需地址级分割，装载调用点
0x42bd?? 区段可从反汇编取）；(C) watch 循环头计数，每 N 次命中手动置 mgr+0x20=-1。
推荐 A；B 为回退。
同类坑：0x42b990:222 还有第二个等待环 `while (DAT_004e14c0 != 0) Sleep(0x10)`
（某线程忙标志，桩环境下预期恒 0，需一跑确认）。

### 5.2 线程/装载桩外推（TO 6 修复之外的新增项）
- _beginthreadex 返 0 的语义副作用：0x43b280 把句柄存 DAT_004dcec0、线程体指针
  存 DAT_004dced4——补调判据用 DAT_004dcec8/0x4dcecc（0x42b990 首尾各置位）。
- thbgm/BGM：0x42b990 尾部 0x43adc0 播 stage 表[3]/[4] 曲名——SOUND_PLAY 已桩，
  但曲名表读取需 seam（thbgm.fmt 已在开场线程装载，TO 已验证）。
- 装载量与超时：st01 全套（plXX/st01enm/enemy/front/bullet/effect/st01wl）+
  game_main 0x191a4 + score 0x1b680——单次 drive 预算放宽（TO 用 600s），
  每 300 帧进度日志。
- score 前置：0x42b990 的 score 应用分支要求 DAT_004dcc30≠0（=0x449b00 成功）；
  开场线程补调已含——**跳过开场线程直接请求 7 会走 DAT_004dcc30==0 分支**
  （无 score：点值/续关点数用默认），事件数值与真机有存档时不同——契约窗必须
  固定其一（建议：补调开场线程=「有 score」路径，与真机首跑一致档）。

### 5.3 双跑与重复提交
- 禁手工驱动清单（battle_oracle 不得出现）：0x418e00、0x46e510、0x46f170、
  0x46f240、0x46f380、UP_FLUSH（帧序内的 0x4679a0 除外——那是 0x45d2f0:27 的
  帧首刷，非 mgr 层刷）、MGR_CREATE/SPAWN/0x40d920（0x42b990 内自跑）。
- **0x4032d0 ctx 桶 vs ANM 池双驱动疑点**：0x46a700 的调用者含 0x46f380（mgr
  层绘）——若某 VM 同时挂 mgr 层链与 ctx 桶会双份 op。静态证据：ctx 桶结构
  （0x402ff0 分配、独立于 mgr+0xf48784 层头区）与 mgr 池互斥，且 0x425170 建
  stage ctx VM 时先查 `mgr+0xb77b24`（stage ctx 专用槽，非 26 槽表）——两域
  分离。但**待捕获后以 op 无重复（同帧同脚本同位置双份）自证**；若出现重复，
  检查 0x46f3f0 尾插是否被 ctx VM 复用。
- op 序变化：路径 A 的 op 顺序=调度器优先级序，与 RO 手工层序（0..0x38）不同
  ——旧窗文件与新窗文件不可混比；移植侧 draw schedule 优先级必须按新窗回填
  （清单 2），否则全窗分歧。

### 5.4 待解清单
1. Esc 的输入位归属（VK 0x1B → DAT_004e49f0 bit8?）：一跑自证（TO 输入位表
   未含 Esc）。
2. 回放 u16 流 ↔ 键态位的换算表（0x471620 打包位序 × t13r 6 字节帧定义）——
   回放窗多关化时解。
3. 族 4 HUD 文本的具体提交路径（AsciiInf 桶 or 直绘 UP）：捕获后按 op 归类。
4. GameOver 菜单变体（142/143/147/148）与 GameOver 窗设计（属 results-flow
   场景链窗，S3 后续）。
5. game ctx（DAT_004c2180）的任务注册点（0x4032d0 挂在哪条链、优先级多少）：
  路径 A 下自跑无需知道，但移植侧 AnmDrawSchedule 排位需要——从 0x470c30
  执行序读。
6. 0x42b990 等待环字段 mgr+0x20 的真实语义（装载线程计数?）：对策 A 不依赖
   语义，但若对策 A 失败需回退 B 时要解。

—— 完 ——

## 附录：真机全链路窗 HUD 实证（oracle 侧，2026-10-23）

全链路分片跑法（boot→菜单→难度→角色→开局装载→战斗，attack5.jsonl
1100 帧稳定产物）实测：**HUD front00.png 在战斗段 f761（全局帧）首现
并稳定为 15→17 项 up op**，同帧伴随 pl01.png（玩家）、eff_base.png
（背景特效递增）、st01logo.png、eff_switch.png；战斗窗（render_oracle
直调装载序）同段无任何 HUD op——HUD 仅由真机全链路（场景 7 战斗
update/draw 任务族）产生，捕获面必须走全链路路径。证据文件：
`artifacts/tmp-scratch/attack5.jsonl`（oracle 侧）/ `f752-attack-report.md` §12。
