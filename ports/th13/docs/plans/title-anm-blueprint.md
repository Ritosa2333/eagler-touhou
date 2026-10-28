# th13 主菜单 ANM 蓝图（title.anm / title_v.anm 脚本号总表与重锚方案）

> 目的：把主菜单重做所需的全部 ANM 锚定固化为可执行文档。一切数值出自
> `artifacts/reference/dumps/anm-list.txt`（下称 **dump**，行号直接引用）与移植侧源码
> （`web/cpp/` 相对 `eagler-touhou/ports/th13/`，行号引用）。字条/图标为烘焙位图，
> 内容语义按序号引用；无法从数据解出的一律标「待锚定/待人类辨认」。
> 事实源排序见 §六：unicorn 标题窗 oracle 实测 > 本蓝图。

## 〇、机制前提（从移植侧源码实证，蓝图全部序列依赖这两条）

1. **中断传播**：`AnmRegistry::interrupt`（AnmRegistry.cpp:95，注释锚 0x46fca0）把
   pending 标签同时下发给**被中断 VM 的全部直接子 VM**。因此 `interrupt(root, L)`
   的语义 = 根自身 + 每个子脚本各自扫描自己的 `ins_5(L)`：有该标签者响应、无者继续。
   th15 的 `interrupt(0, 3)` / `interrupt(0, cursor+7)` 调用形状在 th13 **原样成立**
   （标签号不同）。`ins_5(id)` 即中断标签定义；缺失且无 `-1` 回落标签时该 VM 无动作
   （AnmInterpreter.cpp:57 扫描逻辑）。
2. **暂停不冻结插值**：VM 停在 `ins_3()`（待中断，AnmInterpreter.cpp:109）期间
   `advance()` 仍逐帧推进插值器（AnmUpdate.cpp:8，由 tick 的 idle 路径调用）。
   因此白态字条 spawn 时的 `ins_409(15,0,255)` 会**自动完成 15 帧淡入**，无需任何中断。

指令速查（AnmInterpreter.cpp 行号）：`ins_300` 选精灵:117 / `ins_304` 层:121 /
`ins_400` 位置:135 / `ins_403` alpha:138 / `ins_404` 颜色:139 / `ins_407/408/409/412`
位置/颜色/alpha/缩放插值:142-147 / `ins_431` 时率锁定:164 / `ins_500` 建子 VM:172 /
`ins_5` 标签+`ins_7` 返回 / `ins_200` 计数回跳:115 / `ins_6(N)` 计时快进。

bank 表（Application.cpp:16-18，常驻）：ascii=**2**、effect=8、title=**16**、
title_v=**17**、help=**19**。title.anm 共 23 entry / 242 脚本（script0-241），
精灵号全文件连续（每精灵自带纹理归属，AnmInterpreter.cpp:37）。

## 一、脚本号总表

### 1.1 两类字条形态与公共 label 语义（主/难度/角色菜单同构）

| 形态 | 初始行为 | label 语义 |
|---|---|---|
| 灰态（层 26，色 192,192,192） | spawn 即 `ins_3()` 待机，alpha=0 **隐藏** | `7+k`/`17+k` 入场（5f 淡入×2→±1px 抖动 10f→192↔255 脉动 20f×10 循环，dump L17342-17453）；`2` 确认：64↔255 闪 20f→缩 0.5 飞至 (256,332) 停（L17421-17423）；`3` 淡出 5f；`6` 闪 20f 后淡出销毁（L17447-17453）；`27` 变黑 4f；`28` 恢复 192 4f；`29` 隐藏 1f；`30/31` **空操作**（`ins_7` 立即返回，L17393-17394） |
| 白态（层 25，色 192,192,192） | spawn 自动 15f 淡入（前提 2） | `3` 淡入显示 5f；`7`/`17` 淡出隐藏 5f（L18509-18514）；`27/28/29` 同灰态但 29=黑+alpha160；`6` 待 10f 销毁；`1` 淡出 20f 销毁；`30/31` 空操作 |

层 26 绘于层 25 之上（灰态盖白态）。选中机制（数据推定，oracle 锁定后回写）：
整体 `interrupt(root,3)` = 全部白态淡入+灰态淡出（未选中外观）；随后
`interrupt(root,7+k)` = 第 k 项灰态入场脉动+白态 k 淡出（选中外观）。

### 1.2 主菜单：根 script0（dump L17223）

- 锚点 **(70,280)**（L17227），仅 label 1（L17249-17252）；spawn 子脚本 3-22（L17228-17247）。
- **10 个菜单项**（每项灰白两条，内容为 title01.png 128×16 位图字条，左列/右列）：

| 项 k | 灰态(层26) | dump 行 | 白态(层25) | dump 行 | sprite 灰/白 | y |
|---|---|---|---|---|---|---|
| 0 | script3 | L17342 | script13 | L18482 | sprite0/sprite1 | 0 |
| 1 | script4 | L17456 | script14 | L18525 | sprite2/sprite3 | 18 |
| 2 | script5 | L17570 | script15 | L18568 | sprite4/5 | 36 |
| 3 | script6 | L17684 | script16 | L18611 | sprite6/7 | 54 |
| 4 | script7 | L17798 | script17 | L18654 | sprite8/9 | 72 |
| 5 | script8 | L17912 | script18 | L18697 | sprite10/11 | 90 |
| 6 | script9 | L18026 | script19 | L18740 | sprite12/13 | 108 |
| 7 | script10 | L18140 | script20 | L18783 | sprite14/15 | 126 |
| 8 | script11 | L18254 | script21 | L18826 | sprite16/17 | 144 |
| 9 | script12 | L18368 | script22 | L18869 | sprite18/19 | 162 |

灰态入场 label 为 `7+k`/`17+k`（逐项偏移，k=0→7/17 … k=9→16/26，见各脚本）。
**项数证据**：根 spawn 20 条=10 行×2 态；字条纹理共 50 条 128×16（dump L17148-17197）。
逐项文字内容为烘焙位图 → **待人类辨认**（可用 `artifacts/reference/golden/` 标题帧比对）。

### 1.3 难度菜单：根 script1（L17256）

- 锚点 **(70,312)**（L17260），仅 label 1；spawn 顺序
  23,24,25,26,27,33,34,35,36,41,42,43,44,28,29,30,31,32,45,46,47,48,37,38,39,40（L17261-17286）。
- **5 个难度名项**（灰 L18912/19026/19140/19254/19368，白 L19482/19525/19568/19611/19654；
  sprite 26/28/30/32/20 与 27/29/31/33/21，y=0/18/36/54/72；入场 label `7+d`/`17+d`，d=0..4）。
  内容位图（Extra 疑为第 5 行 sprite20/21，第 10 行条）→ 待人类辨认。
- **2 行 4 位数字组**（16×16 图标精灵 sprite50-71，两行各 11 枚，L17198-17219）：
  行 0（y=0，x=70/82/94/106）：灰 script33-36（L19697/19811/19925/20039，sprite50×3+sprite60）
  / 白 script37-40（L20153/20196/20239/20282，sprite61×3+sprite71）；
  行 1（y=18）：灰 script41-44（L20325/20439/20553/20667）/ 白 script45-48（L20781/20824/20867/20910）。
  入场 label `7+r`/`17+r`（r=行号 0/1，与难度名行同号联动）。
  内容为位图图标/数字（sprite50-60=灰行、61-71=白行），**数值语义待锚定**
  （静态 sprite 选择，动态换字机制未见——推测由 supervisor 换 sprite 或仅静态装饰）。
- 现状代码沿用 th15 的「Easy..Lunatic 4 项+Extra 禁用」（TitleSelectionMenu.cpp:39）
  与 dump 的 5 行结构不符；Extra 解锁位**待锚定**（建议先按 5 项全开对 oracle）。

### 1.4 角色菜单：根 script2（L17295）

- 锚点 **(70,312)**（L17299），仅 label 1；spawn 49-82 顺序（L17300-17333）。
- **7 个角色名项**：灰 script49-55（L20953/21067/21181/21295/21409/21523/21637，
  sprite 34/36/38/40/42/32/18）、白 script56-62（L21751/21794/21837/21880/21923/21966/22009，
  sprite 35/37/39/41/43/33/19），y=18k，入场 label `7+k`/`17+k`（k=0..6）。
  7 项=4 角色+3 项其他（位图内容**待人类辨认**；不可凭记忆断言）。
- **5 行 2 位数字组**（x=124/136，y=0..72）：灰 script63-72（L22052-22964，sprite50）/
  白 script73-82（L23192-23579，sprite61），入场 label `7+r`/`17+r`（r=0..4，
  仅行 0-4 有数字，行 5-6 无）→ 数值语义**待锚定**。

### 1.5 背景：根 script89 与两层拼贴（L23903-24056）

- 根 script89（L23922）：alpha=0，spawn 顺序 **92,93,91,90**（L23924-23927），仅 label 1。
- 底图森林（title00a.png，512×480 + 右半 xOffset=512 的 128×480 切片拼成 640 宽）：
  script90 = sprite72 @(0,0) **层 22**（L23938-23947，`ins_6(170)` 后 60f 淡入）；
  script91 = sprite73 @(512,0) **层 21**（L23962-23968，同 170f+60f）。
  label：`3`=淡出 30f、`2`=淡入 60f、`1`=淡出销毁 60f（L23949-23957）。
- 红色星光 overlay（select00.png 512×480 + xOffset 512 切片）：script92=sprite74 @(0,0)
  层 21（L23986）、script93=sprite75 @(512,0) 层 21（L24022）。色 (64,64,64) 压暗
  （L23994）；label `2`=压暗淡入 170f 后隐藏（L23995-23999）、`3`=亮态：
  色→192 60f 后 **300f 交替 224↔192 呼吸，回跳 60 次共 61 轮**（L24006-24011）、
  `1`=淡出销毁（L24013-24014）。
- HANDOFF 称「60 帧呼吸」不准：60f 是入场调色，呼吸周期是 300f×2（上表以 dump 为准）。
- entry 结构：entry1=title00a（sprite72，L23903-23919）、entry2=title00a xOffset512
  （sprite73，L24059-24076）、entry3=select00（sprite74，L24080）、entry4=select00
  xOffset512（sprite75，L24100）。

### 1.6 LOGO：根 94/95 与五个子体（entry5 title_logo.png，L24121-24245）

- script94 首入场根（L24142）：`ins_6(110)` 延迟 110f → spawn 97（TEN DESIRES 翻转）→
  `ins_6(60)` 再延迟 → spawn 96+100（L24144-24148）。script95 复位根（L24155）：
  无延迟 spawn 99+96+100（自子菜单返回用，时机归属**待 oracle**）。
- script96 = sprite76（東方靈廟 256×192）@(132,150) 层 25，10f 淡入（L24166-24175）。
- script97 = sprite77（TEN DESIRES 256×256）**3D 翻转**：起 (448,368) scale2.5 旋转
  z=-π，60f 插值至 (132,150) scale1.0 旋转 0（ins_407/412/410，L24187-24192）；
  到位后 spawn 98 爆闪 + 恒慢速自转 `ins_415(0,0,0.009817477)`（L24194-24195）。
- script98 = 爆闪：alpha128→0 10f、scale→4.0、色→(64,64,255)（L24199-24213）。
- script99 = 静止慢转版（复位用，L24216-24227）。script100 = 副标题 sprite78（192×16）
  @(360,480) 层 25（L24230-24239），label 1 = 淡出 60f。

### 1.7 大行条与光标（entry6 select01.png，L24248-24273）

`[解剖轮2 修正]` 条带内容按位图辨认：条 1-5（sprite79-83）= 难度行
家内安全(EASY)/無病息災(NORMAL)/交通安全(HARD)/學業成就(LUNATIC)/六面◯◯
(EXTRA)；条 6-9（sprite84-87）= 霊夢/魔理沙/早苗/妖夢 角色行；条 10
（sprite88）=「やめる」行。

`[解剖轮2]` **难度屏光标 = script119（root123 的子体，rank00 条 sprite96）**，
非 111/112——层桶快照实证（蓝图原猜作废）。119 的层实测 L26（dump ins_304
25，+1 现象同 §8.3）；滑入轨迹由 supervisor 光标移动器（0x473210）驱动，
非脚本插值：f490(-202,8)→f508(32,8) 十九帧减速序列
`-202,-176,-152,-130,-110,-90,-72,-56,-40,-26,-14,-4,6,14,20,26,28,30,32`
（y=8 恒定）。101/114(L26)+116/121(L23) = rank00 徽记组（x=192 列：1 枚固定
(192,178) + 上下滚动展开各 1，见 §四）。111/112（dump L24437/24455）全窗
无创建——归属仍待 oracle（不属标题三窗已知画面）。

### 1.8 其余脚本组（如 dump 有内容，列出边界）

- script83-88（L23622-23902）：几何体（`ins_300(-1)` 回落精灵 + `ins_608/606` 屏幕四边形/
  模式19），扇形量表，label 6/7,17/8,18/9,19/10,20/1——为根 123/124 的子体（推定
  PlayerData/段位屏，**待锚定**）。
- script113-122（L24503-25247）：rank00.png 段位名条 sprite90-99（256×80，灰 113-117
  逐项 label 7..11/17..21/白 118-122）；根 123 @(320,178)（L25299，子 83,84,118-121,
  113-116）、根 124（L25322，子 83,84,117）——**屏幕归属待锚定**。
- 立绘 script125-128（L25418/25467/25516/25565）：sprite101-104（entry8-11 =
  face/pl00..pl03/title_pl0X.png，216×480/220×460/192×440/320×440），@(144,480) 层 23，
  anchor `ins_421(0,2)`。label（角色 c=0..3）：`7+c` 从右滑入（272→144，10f 淡入）、
  `12+c` 从左滑入（16→144）、`17+c` 向左滑出（层降 22、淡出）、`22+c` 向右滑出、
  `3` 中央 (160,480) 14f 淡出、`6` 恢复层 23、`1` 淡出销毁 20f。
- 角色菜单根 script129/130（L25614/25636）：129 spawn 125-128（立绘）+135-138（通常
  武器）+131-134（选择框）；130 同但 139-142（EX 武器）。129/130 分支条件
  （武器前后页？难度？）**待锚定**。
- 武器选择框 script131-134（L25658/25708/25758/25808）：sprite100（rank00.png 内
  128×96 块，L24499）@(64,128)/(80,96)/(96,128)/(96,96)，层 26 隐藏→25，3D 翻转
  （`ins_302(8)`+旋转 π→0）入场 20f；label `7+k/12+k/17+k/22+k/1`（k=0..3 逐项偏移）。
  用途（每角色一个框 or 每武器槽）**待锚定**。
- 武器图标 script135-138（通常，sprite105-108 400×256，L25938/26007/26076/26145）与
  139-142（EX，sprite109-112，L26294/26363/26432/26501）：@(416,240) 层 23，
  label `7+c` 从右（544→416，20f 3D 翻转）/`12+c` 从左（288→416）/`17+c` 出左/
  `22+c` 出右/`3` 淡出/`6` 层复位/`1` 销毁——与立绘同构的滑入体系。
- entry20 result00.png（L26571）+ script143-186（L26608-27796）= 结果屏；
  entry21 spell.png（L27851）+ script187-241（L27930-30481）= 符卡/练习屏。
  非标题三窗，仅登记边界，本轮不展开。

### 1.9 title_v.anm（bank17，dump L37812-37856）

仅 entry0 = title_ver.png 128×16（sprite0 126×14，L37828）+ **script0**（L37832）：
@(256,224)（L37836）层 26，alpha=0 起 30f 延迟淡入（L37840-37841）；label `3`=压暗 64、
`2`=回亮 255、`1`=10f 淡出销毁。**bank17 没有任何立绘**（HANDOFF 一致；现状代码把
bank17 当 portrait 用是病灶，见 §五-1）。

### 1.10 提示幕「按任意键」（ascii.anm=bank2，dump L15207-16314）

script19（L16282）：spawn 16×`ins_502(script18)` + 回跳 1 次（共 17 字符位循环，
L16290-16306），仅 label `1`（10f 淡出销毁，L16310-16311）。script18（L16242）=
变量驱动字符（ins_117 变量算术 + sprite304），**入场自带、无 label 2/3**——
th15 的 `interrupt(89,2/3)` 调用形状在 th13 不可用（标签缺失 → 该 VM 无动作）。
现状代码对提示幕 interrupt 2/3（TitleMainMenu.cpp:87/90/135/145）无效，须删。

## 二、TitleSceneResources 应改成的值（C++ 草案）

```cpp
// 证据：bank 表 Application.cpp:16-18；脚本号见 §一 各行号。
struct TitleSceneResources {
    i32 title = 16;    // title.anm
    i32 version = 17;  // title_v.anm（原 portrait=17 字段误当立绘，改名；§一.9）
    i32 ascii = 2;     // ascii.anm（TitleAnimations.hpp:10 默认 5 是坑，须显式传 2）
    i32 help = 19;     // help.anm（析构 retire_resource 用，§五-12）
    i32 main_root = 0;            // 主菜单根 (70,280)，子 3-22
    i32 difficulty_root = 1;      // 难度根 (70,312)，子 23-48
    i32 character_root = 2;       // 角色根 (70,312)，子 49-82
    i32 char_menu_root = 129;     // 角色菜单根（通常武器；130=EX，分支待锚定）
    i32 char_menu_root_ex = 130;
    i32 background_root = 89;     // 背景根（子 92,93,91,90）
    i32 logo_root = 94;           // LOGO 首入场根（内建 110f/60f 时序）
    i32 logo_restore_root = 95;   // LOGO 复位根（自子菜单返回）
    i32 version_bar = 0;          // title_v script0（bank=version）
    i32 prompt = 19;              // 提示幕 = ascii bank script19（≠title bank script3！）
    i32 portrait_base = 125;      // 立绘 125+角色号(0-3)，label 7/12/17/22+角色号
    i32 weapon_base = 135;        // 通常武器图标 135+角色号
    i32 weapon_base_ex = 139;     // EX 武器图标 139+角色号
    i32 weapon_selector_base = 131; // 武器选择框 131+k（用途待锚定）
};
```

派生常量：主菜单项数 `kTitleMainEntries = 10`（§1.2）；难度 5（§1.3）；角色 7（§1.4）。

## 三、TitleMainMenu::update 各 substate 创建/interrupt 序列

`[oracle 修正 2026-10-22]` 本节已按 §八 实测重写为移植侧现行实现（旧 th15
翻译稿废弃；原「提示幕/age120/portrait=17」病灶已随实现移除）。

- **substate 0（进入）**：menu.count=10、wrapping、disable(1)；首次（flags 无 2）
  `create(89)` 背景 + `create(94)` LOGO（使能帧 = g181）；返回（flags 有 2）
  `create(95)` 复位 LOGO 并当场 open_menu。无提示幕、无 ascii。
- **substate 1（intro/就绪）**：首次 age==180（g361）open_menu = `create(0)`
  （灰条 3-12 停车、白条 13-22 **建后即 discard**——真机 boot 不建白条，引擎
  插值冻结语义差异的移植侧补偿，§8.2）+ `create(17,0)` 版本条 + 全项
  `child_interrupt(3+k, 7+k)` + 禁用项落色 0xA0000000；首次 age>=191（g372，
  =180+11）/返回 age>=11 → substate 2 + `interrupt(0,7+cursor)`。
- **substate 2（交互）**：↑↓=0x10/0x20 移动 → `sound(10)` +
  `interrupt(0,7+new)`（无 label 3 广播、无 paint()——30/31 空操作已删）；
  Esc/X(0x102) → 跳末项或 substate 4+`sound(9)`；Z(0x80001) 非 Quit →
  `sound(7)` + **确认序**（[窗A 修正] §8.2 帧级锁定）：`interrupt(0,6)` 全项
  20f 闪灭（g491 死点）+ 选中项 `child_interrupt(3+cursor,2)` 残留单条 + substate 4
  （字条保留、不 label 3 淡出）；Quit → `sound(9)` + substate 4；
  **其余入口（1/2/3/4/5/6/7/8）确认序同上、enter() 不切屏**（真机有画面，附 9）。
- **substate 4**：age==8 回收版本条（oracle g479 起 title_ver 无 op；Quit 分支
  保留版本条）；age>19 `enter(cursor)`：0 → flags|=2 + Difficulty（menu.push、
  count=5、select(configured_difficulty)；返回路径 open_menu 整树重建——旧树
  字条已死/选中项飞离）；9 → Quit；其余 no-op（菜单终局）。

## 四、TitleSelectionMenu 重锚（2026-10-22 随实现更新；推定条目待 oracle 难度/角色窗）

### 4.1 难度菜单（[解剖轮2 全量重写] difficulty-menu-anatomy.md；根 1 方案作废）

**真机构成**（f490=Z+21 起；脚本号 = 层桶快照 × 产物关联实证）：
- `create(123)` @(320,178) → spawn **83,84,118,119,120,121,113,114,115,116**：
  83/84 = 行条对（滑入期 f490-f516 title00a 微旋双片对 → 落位后 loading.png
  四条 1px 错位对）；119 = 光标（§1.7）；114/116/121 = rank00 徽记滚动组；
  118 = 右半承接（f517 起 select00 右半 L22）；113/115/120 = 辅助层。
- `create(101, translation{160,170})`：脚本自带 -256→32 20f 滑入 = 固定徽记
  (192,178) L26（x=192 = 320-128 徽记左缘）。
- **难度子脚本 23-48 真机不建**（池内全程不存在）——root1 名行/数字行联动
  方案作废；行为层仍按 5 项（家内安全..やめる）光标建模。
- 徽记动画：1 固定（101 自驱）+ 上滚（118 ← label 9：L23 y→-256 30f 插值，
  f509 样本 y=64 在中段 ✓）+ 下滚（L26 y 178→292，无对应标签 = supervisor
  置位，两点折线 6.67→3.5 px/帧）。
- **神霊粒子**：ascii bank2 script18 克隆（「打字机文字」纠偏——不存在逐字
  文本）；脚本经 RNG 槽 10010-10012（AnmVariables 映射游戏 RNG 流）自算全屏
  随机出生位/300f 漂移/旋转，+300 自灭（窗内不死灭 ✓）；节奏 f495 起
  17@f500→55@f599（≈1/3 帧，出生次序待对拍校准）。
- **Z 后主菜单残留**（§③ 时间线，Z=窗口 f469）：+8 ver 消、+10 灰条透明度
  -13/帧衰减（rgb 恒 192）、+14 LOGO 消、+20 转屏、+21 非选中灰条隐藏
  （VM 留池）+选中白化 @204 续 -13/帧至 +37 隐藏 + 白条 14-22 九枚闪 8 帧
  （64→0，-7/帧）、+47 行条落位/title00a 全消、+48 select00 右半 L21→L22
  （真机由 118 承接；移植侧直接搬既有 93 VM 层，几何等价、VM 构成差异待裁）。

**移植侧实现**（TitleDifficultyMenu）：独立屏龄计数（state.age 被 substate
切换重置）；build_difficulty（123+101+118 label9+119 层 26）；finish_menu_hides
（Z+21 隐藏时间线，白条建后 interpolators.alpha.duration=0 中和 spawn 淡入
——引擎插值冻结语义差异补偿 §8.2）；advance_choreography（光标缓动/徽记
下滚/粒子节奏/白条与选中衰减/f27 分层）；↑↓ 移动本轮仅音效（真机
0x473210 移动器 + 徽记列更新逐帧几何待窗对拍）。

### 4.2 角色菜单（根 2 + 根 129/130 立绘武器体系；2026-10-22 随实现更新）

- **substate 0**：retire(2)→create(2) + `interrupt(2,7+cursor,true)`；retire(129)→
  create(129)（130 EX 分支待锚定）+ `interrupt(129, 7+character)`（character=
  cursor-1，行 1-4 = 霊夢/魔理沙/早苗/妖夢 推定）——129 广播同号命中立绘 125+c/
  武器 135+c/选择框 131+c（§1.8），无需逐 child_interrupt。
- **substate 2 左右移动**（0x40/0x80）：→ = 旧出左 `interrupt(129,17+old)` +
  新自右入 `interrupt(129,7+new)`；← = 旧出右 `22+old` + 新自左入 `12+new`
  （方向配对推定待 oracle）；名条 `interrupt(2,7+new)`。非角色行（0/5/6）不带动
  立绘滑动（推定）。
- **确认 Z（角色行 1-4）**：`interrupt(129,3)`（树中央淡出）+ `interrupt(2,6)`
  （名行闪灭销毁）+ `sound(7)` → substate 3；Z（6=やめる行）→ 同 Esc 回难度；
  Z（0/5 行）→ 确认音不动作（语义待锚定）。
- **substate 3**：age==10 转场 effect bank8 script0 + interrupt 7；age>39 开局
  （character=cursor-1）。
- **substate 4**：retire(129)+retire(2) → 回难度屏（preferred 随预览行同步）。

## 五、TitleFrame / TitleState / TitleScene 差距清单（HANDOFF 13 项 → 代码落点）

| # | 差距 | 落点与修法 |
|---|---|---|
| 1 | portrait 用错 | TitleMainMenu.hpp:22 `portrait=17`→删；TitleMainMenu.cpp:101-104 `create(portrait,0)`→`create(16,125+char)`+interrupt 7+char（§三） |
| 2 | 背景缺失 | 全代码无 create(89)；在主菜单 substate 0 新增（§三），92/93 的 label 2/3 时机待 oracle |
| 3 | 提示幕脚本号错 | TitleMainMenu.hpp:24 `main_prompt=3`→ascii bank2 script19；TitleMainMenu.cpp:87/90/135/145 的 interrupt 2/3 删（§1.10） |
| 4 | LOGO 缺失 | 新增 create(94)/95（§1.6）；th15 无对应物，序列推定 |
| 5 | 版本号缺失 | 新增 `create(17,0)`；退场 interrupt 1（§1.9） |
| 6 | 菜单项数/枚举错 | TitleMainMenu.hpp:17-18 `kTitleMainEntries=5`→10；`TitleMainEntry` 枚举语义待辨认（位图文字）；TitleScreen 枚举值待 supervisor 锚定（见下） |
| 7 | 难度/角色脚本号未锚 | TitleSelectionMenu.cpp:41-42/55-56/63（难度）与 114-115/126-135（角色）的 th15 标签算式→§四 值；根自身无 3/6/7 标签（L17290/17337 仅 1），靠子传播（§〇-1）生效 |
| 8 | 角色选中态子脚本错 | TitleSelectionMenu.cpp:91-95 `126/147/80/81`→§4.2 确认序列 |
| 9 | TitleFrame 缺 demo/音乐重播/return_reason/Quit | TitleFrame.cpp:34-49 `initialize()` 只分 reason 0/其他；`music_age`（TitleFrame.hpp:40）未消费；demo/重播分支无；**待办**：需 supervisor 反汇编锚 return_reason 值域与 demo 计时（锚点待登记 AddressMap.hpp，候选 0x44e600 对应物） |
| 10 | TitleScene 缺屏 | TitleScene.cpp:49-61 仅 Main/Difficulty/Character；Quit 流（TitleFrame.cpp:62-67）已有雏形，Demo/音乐室等分期不做（ui-base.md §三） |
| 11 | Draw pass 占位 | TitleFrame.cpp:15-16 callbacks[1] 仅返回 enabled（TitleFrame.hpp:46 `drawing()`）；真实绘制回调体待主菜单对齐后接 ANM 渲染管线 |
| 12 | 析构漏 bank19 | TitleScene.cpp:20 只 retire {title,portrait}→补 `retire_resource(help=19)`（bank 常驻表 Application.cpp:18） |
| 13 | 测试用合成 ANM | tests/ui/ui_test_fixtures.hpp:32 `synthetic_anm`；主菜单落地后按本蓝图 §一 数值加真实 title.anm 脚本号回归（标签/锚点/层逐项断言） |

另：TitleState.hpp:14-15 `Quit=2` 与 `StartGame=2` **同值冲突**，TitleScreen 枚举整体
待锚（th15 世代值不可信）。

## 六、事实源排序声明

1. **oracle 真机行为 > 本蓝图**。unicorn 标题真机窗实测的创建/中断序列若与本文任何
   「推定」条目冲突，以 oracle 为准，并**回写本文档**（§八 为 2026-10-22 标题窗
   实测回写章；冲突处已就地标 `[oracle 修正 2026-10-22]`）。
2. 本蓝图非推定条目（脚本号/锚点/层/标签号/精灵尺寸/插值参数）均直接出自 dump，
   与 oracle 冲突时优先怀疑 dump 工具链（thtk 版本/解码），上报而不是擅自改数。
3. 位图内容（菜单项文字、数字组语义）只能由人类辨认 golden 帧或真机截图确认，
   禁止凭模型记忆断言（前科：Boss 名幻觉事故）。

## 七、验收门禁

1. 标题窗 / 难度窗 / 角色窗 draw_compare 均 `RESULT first_divergence_frame=none`
   （三窗各自独立 draws 窗口，覆盖矩阵原则）。
2. 对局窗回归 ≥ 分歧帧 511（不得回退；HANDOFF 已达成值）。
3. 每轮修复附带覆盖矩阵更新：本轮 oracle 覆盖（三窗）/ 未覆盖（Quit/Demo/音乐室等
   分期屏）清单交接下一阶段。

## 八、oracle 标题窗实测回写（2026-10-22，`oracle/title_oracle.py` + `artifacts/oracle-draws/title-draws.jsonl` 600 帧窗）

真机 boot 全链帧序（全局帧 g；窗口帧 = g-1）。以下全部为 oracle 直出事实，
与 §〇-§五 冲突处以本章为准：

### 8.1 boot 时序（无提示幕）

| 帧 | 事件 | 绘制证据 |
|---|---|---|
| g1-g179 | 开场 sig.anm logo 幕（script0 左半 + 自 spawn 的 script1 右半，自黑场 10f 淡入） | sig 两半 ops（L23/L26），无任何 ascii/title 绘制 |
| g180 | sig 卸载，空白帧 | ops=0 |
| g181 | 标题任务使能：背景根 89 创建（92/93 亮、91/90 内建 170f 延迟） | select00 两半 ops 出现（L21/L23） |
| g291 | LOGO script94 内建 110f 延迟到点 → 97（TEN DESIRES）翻转入场 | title_logo@26 起自 (732,663) 60f 滑至 (260,278) |
| g350 | 森林 title00a 淡入（script90/91 内建 170f+60f） | title00a 两半 ops（L23/L22） |
| g361 | 菜单展开：root0 + 十灰条（L26 批次）+ title_v 版本条（30f 延迟淡入） | title01@26（10 quad，alpha 全员渐升）+ title_ver@26 起 g390 |
| g372 | 菜单可交互（= 使能帧 +191） | INPUT_SCRIPT 锚（f380/f420/f470 均在其后） |

真机 boot **无「按任意键」提示幕**：DAT_004dc670=0 直进菜单，全程无 ascii 绘制 op；
th15 式提示幕/`interrupt(89,2/3)` 体系不适用（§1.10 的 script18/19 属文本路径，
boot 虽建有 ascii#18（主池插入、L22）但**无独立绘制 op**，机制未明——移植侧不建，
登记差异）。

### 8.2 主菜单真机行为

- **10 项全员可见**：建帧起各灰条 alpha 渐升（≈15-20f）、色恒 (192,192,192)；
  白态 13-22 **boot 不建**（全窗 f470 前无 title01@L25 op；Z 展开子菜单后才建，
  可见期 f490-f497 约 8 帧闪烁）。⇒ §三 substate 1 的「白态 15f 自淡入即未选中
  外观」不成立；真机 ins_500 子 VM 建后停在 ins_3 的插值**冻结**语义与移植侧
  AnmUpdate 的「暂停仍推进插值」相悖——**AnmUpdate 语义待裁定**（本轮移植侧
  以「建白条后即 discard」保绘制面一致，裁定后改回保留）。
- **选中机制**：移动光标只 `interrupt(0,7+new)`（见 §三 就地修正）。旧项脉冲
  自然结束静置于 255/192；新项 alpha 自 ~0 快速重入。
- **1 号项禁用（Extra Start）**：字条黑 (0,0,0) × alpha 160（顶点色 0xA0000000，
  自建帧恒定、无淡入）。机制疑为 supervisor 逐 VM 置色（脚本内无对应标签序列：
  灰 label 27=变黑 4f 但无 alpha160 步）——移植侧直接落子 VM 色，机制待锚。
  光标跳过（↓ 一次 0→2 实证）。
- **Z 展开子菜单**（[解剖轮2 精修] Z=窗口 f469；全部窗口帧锚，见 §4.1 时间线）：
  +8 title_ver 消（末日 f477）；+10 灰条透明度 -13/帧（f479-f489 顶点色，rgb
  恒 192）；+14 LOGO 消（末日 f483）；+20 转屏；+21 非选中灰条隐藏（**VM 留池
  不销毁**——轮1 的 label6/label2 序作废）+ 选中白化 0xccffffff 续 -13/帧至
  +37 隐藏 + 白条 14-22 九枚闪 8 帧（64→0 -7/帧）；+47 行条落位；+48 select00
  右半 L21→L22。子菜单构成按解剖 §4.1（spell 屏树 root187/240 + sig 槽位
  f491 重载 20 VM@L22 待查）。

### 8.3 层值偏移现象（[窗A 首轮比对 2026-10-22 已按帧落码]）

oracle 实测层与 dump `ins_304` 不一致处及精确帧（title-draws.jsonl）：
- **sig 右半**（script1，dump 25）：boot 全程 f0-f178 实测 **L26**（左半 23 与 dump 一致）；
- **select00 右半**（script93，dump 21）：自 **g181（f180）起 L23**；**g350（f349）起 L21**
  （最后 23 帧 f348、首个 21 帧 f349——与森林淡入同帧搬移；f517 起再搬 22，属
  子菜单转场期，本轮不复刻）；
- **title00a 左/右半**（script90/91，dump 22/21）：自 g350 淡入起实测 **23/22**（各 +1）；
- select00 左半（script92）与 dump 一致恒 21。
移植侧落码：`TitleMainMenu::adopt_background_layers()`（建根时 90→23/91→22/93→23，
92 不动）+ substate 1 age==169（g350）93→21；sig 右半在 `ApplicationState::draw_loading`
建后置 26（**web 侧已落，tests/harness/harness.cpp 的 prepare_loading 镜像需同步一行**
——该文件非标题链路所有权，已向协调方报告）。机制（supervisor 逐 VM 置层）仍未反汇编。

**[窗A 首轮比对附带发现] 移植侧捕获管线层错记**：DrawCapture 导出的 op 层与 VM
真实层存在系统性串扰（疑似 sprite() 钩子与 op 落笔次序滞后一个绘制调用）。wasm
内 printf 实证 VM 层全对（90=23/91=22/92=21/93=23→21@t169 恒稳），而导出呈
「右半 +1 / 下一 VM 层」样偏移（f349 导出 [2]=23/[4]=22 vs VM 91=22/93=21）。
**draw_compare 前需修 DrawCapture/AnmRenderer 钩子次序（harness 所有权）**，
否则层值比对永差一。

### 8.4 位图辨认补充（title01.png 行序 → 内容，视觉模型多轮交叉读，待人类终审）

y=0..144：開始/Extra開始/練習開始/スペル練習/リプレイ/プレイヤーデータ/
音楽室/オプション/マニュアル/やめる（主菜单 10 项，y=144 四对 sprite 复用同位图
——主 9 号项与难度 5 号项共条）；y=160 もどる；y=176..240 = 家内安全/無病息災/
交通安全/學業成就/六面◯◯(Extra)；y=256..304 = 霊夢/魔理沙/早苗/妖夢；
y=320..352 = スロー表示/スキップ/スピード（回放面板）；y=480/496 = 灰/白数字
字模（11 枚/行）。角色根 script2 的 7 项布局推定：0=六面◯◯字条、1-4=角色、
5=學業成就字条、6=やめる（0/5 行语义待锚定，移植侧按确认音不动作处理）。

### 8.5 本轮未实现项登记（「未实现，真机有画面」——覆盖矩阵有意偏差）

- 主菜单 1/2/3/4/5/6/7/8 号入口（Extra開始=无存档禁用、練習開始、スペル練習、
  リプレイ、プレイヤーデータ、音楽室、オプション、マニュアル）：Z 确认音后
  不切屏；真机均有各自画面（3 号项子菜单树已被 oracle 捕获：root187/240 +
  sig 槽位重载，§8.2）。
- 难度数字行动态换字（存档统计值）、Extra 解锁换行（第 5 行 → 105 行条/字条
  34）、角色 0/5 行语义与数字行、Demo 1800 帧、音乐室/结算等分期屏。
- TitleFrame：demo/音乐重播/return_reason 细分（supervisor 反汇编后回填，
  TitleScreen 枚举值维持现状待锚）。


### 8.6 [解剖轮2] 难度屏解剖回写与 @R 边条（2026-10-22，difficulty-menu-anatomy.md / at-r-strips.md）

- **难度屏重做**（§4.1 已重写）：root123 树 + 101 固定徽记 + 光标 **119**
  （0x473210 移动器缓动序列）+ 徽记 1 固定/2 滚动 + 神霊粒子（ascii script18
  克隆，RNG 槽自算几何）+ 23-48 不建（root1 方案作废）。
- **@R 边条**（每帧 4 up op，f0-f599 恒定）：提交者 = 四个场景 draw 任务的
  精灵栈（**非 ANM 池 VM**，层桶 L20/29/30 无 VM 实证）；纹理 = text.anm
  （槽 0）entry2/3 名 "@R"（RenderTarget 用途真名）；几何 = 条 1-3 屏矩
  (127.5,15.5)-(511.5,463.5) UV (0.2,1/30)-(0.8,29/30)、条 4 左移 96px，
  TRIANGLELIST×2 rhw=1 白不透明 blend(2,1,1)；op 的 layer（29/30/30/20）=
  各任务 0x46f380 残值。移植侧落 TitleFrame draw 回调（优先级 2，先于 ANM
  层走）经 draw_capture 直发（web 构建无捕获指针即无视觉副作用）；
  **boot 期（f1-179）四条同在——tests/harness 装载幕镜像需同步**（登记）。


### 8.7 [锚点补档 2026-10-23] 捕获栈纪律 + 注册池崩溃根因（原「§8.7」悬空引用的正式落锚）

> 本节为补档：轮4/轮5 回写跳号（8.6→8.9）后，代码注释中既有的「§8.7」
> 引用（DrawCapture 栈纪律、build-harness 栈扩、AnmRegistry 崩溃根因
> 证据链）悬空——整改轮登记为正式小节，内容按既有代码证据归档，非新取证。

- **wasm 捕获栈纪律**（`DrawCapture.hpp` emit 系 noinline + harness
  `-Wl,-z,stack-size=1048576`，取代 `-fno-inline-functions` 旗语）：
  V8 wasm 原生栈远小于宿主——emit_quad 内联 4×append_matrix×16
  append_float 在窗B 转场期粒子全量绘制的深调用链
  （绘制层→draw→quad→material→flush→emit 系）下溢出 V8 栈并越过栈
  检查，且 Liftoff/TurboFan 分层编译帧尺寸不同使症状间歇出现。
  修复=栈扩 1MB + emit_* 系定点 noinline（帧尺寸有界），旗语退场。
- **注册池崩溃根因**（阻断 UI 全链用例，移交引擎所有权）：AnmRegistry
  整树 retire 子节点双入 pending_deletion → 双 erase → 槽位二次入空闲链
  → allocate 复用存活槽 → tick 中 VM 被覆写（难度屏 vm(120) "自 spawn"
  连锁假象与宿主 SIGSEGV 的真根因；DOUBLE ERASE 插桩实证；-fno-inline
  仅扰动其表现）。修复 = collect/erase 幂等（0x46f690 同构：每节点恰
  回收一次，链扫 0x40 已置位者跳过）后窗B 10 连跑零崩。
  证据链正文见 §8.9 引擎 bug 实证条（复现载体：
  tests/ui/title_state_test.cpp 的 title_full_chain_start_game）。

### 8.8 [锚点补档 2026-10-23] TFACTOR = 跨帧持续设备态（原「§8.8」悬空引用的正式落锚）

> 本节为补档：TFACTOR 语义模型此前只在代码注释与 GraphicsState 缺省值
> 修改里引用「§8.8」，蓝图未落节——整改轮正式登记。oracle 实证全在
> 标题窗 A（600 帧捕获）。

- **语义**：textureFactor（TFACTOR）= **持续设备态**——D3D 设备初始 0、
  跨帧保持、仅 world_quad 族批按 VM 色显式设置写入（真机 0x4692d0
  同位）+ 变化缓存；up 路径不写 TFACTOR（色调全走顶点色）。
- **实证**：oracle 标题窗 idle 期全 op tfactor=0、无任何写入；粒子期
  @R 边条携带上一次 world_quad 族写入的残留值（f500 @R=0x2aff0140）。
- **移植侧落点**：① `portable/sdl/GraphicsState.hpp` textureFactor
  缺省 0（真机初始态）；② `configure_game` 组合器第二参数 Diffuse
  （此前 Factor 曾靠缺省 0xffffffff 掩蔽，缺省改 0 后 Factor 会把全像素
  着色为黑——web 视觉全灭根因）；③ 帧首管线复位保持 textureFactor
  （`Application::render_loading`/`begin_run`，tests/harness 装载幕镜像
  同步）。**对局窗 f0 tfactor 残留分歧（a=4 vs 6）= 本族未竟工作面**
  （装载趟插值进度残留态，见 HANDOFF 已知缺口），禁用 --ignore 长期
  遮蔽——按契约须限期销账。
- **配套**：LOGO 翻转族层搬移（97 f290-348 L26 / f349 起 L25、98 全命
  L26、96/100 f357-359 L26 窗）此前曾被注为「§8.8」——层时间线定档与
  机制归 §8.9，引用已就地更正。

### 8.9 [轮4 2026-10-23] LOGO 层时间线定档 + 翻转窗浮点取证 + 引擎 bug 实证

**LOGO 层（已落码，fda7be91）**：97 翻转 f290-348 L26 / f349 起 L25（60f 插值
完成即落）；98 爆闪全命 L26；机制=「3D 变换期层+1」族（sig 右半同款），
VM 层写 watch 待 oracle 侧（trace_spawn 走对局窗不覆盖标题流）。

**翻转窗（f290-348）浮点取证（移交 oracle/引擎组）**：
- 角落公式 = **float 链**（sin/cos 先落 f32、逐积 f32、两次加法 f32）——
  wf299 双侧逐位一致实证；double 化角落反而错（曾试，已回退）。
- wf300 起间歇 1-16 ULP：输入量（rot/scale/pos）某处 1ULP 漂移经角落
  560 量级项（320cs+240sn）放大。微扰三候选均能复现 oracle 值：
  rot+1ulp / scale−2ulp / center+1ulp（翻转龄 10 处 python 双精度模型
  显示 f32 权重链与 80 位权重链恰差 1ULP——但 port 侧双精度权重实验
  未能移动实测值，帧号映射复杂化定位）。**需 oracle 侧逐帧 VM 字段
  dump（rot/scale/pos 三通道）锁定差异源**。
- 插值计时实证：94 根首延迟 107 tick（97 于 update#108 出生、#110 首
  绘 alpha=0x04）；96/100 于 #167 出生（oracle f349 首绘 alpha=0x33=2
  tick）；port 与 oracle 出生帧一致，差异仅在层/批构成。

**引擎 bug 实证（阻断 UI 全链用例，移交引擎所有权）**：难度构建期
vm(source_script=120) 执行 ins_500(120) 自 spawn 死循环并宿主 SIGSEGV
（case500 插桩实证 child_script=120；合成 fixture 亦触发）。**疑窗B
node 崩溃同根因（注册池槽复用/模板绑定状态污染类），-fno-inline-functions
仅扰动其表现**。复现载体：tests/ui/title_state_test.cpp 的
title_full_chain_start_game（难度屏构建后第一帧）。

**f349+ 诊断清单（翻转顶点遮蔽后逐帧比对，供下轮）**：
- f349：port 10 ops vs oracle 11——97(L25) 与 98(L26) 跨层合批（renderer
  批键不含层；oracle 逐层任务天然分离）→ renderer 需层边界 flush（导出员
  所有权）；
- f357-359：96/100 短暂 L26 窗（f357 起 0xffffffff 全亮时上移一层？机制
  未定）；
- f360+：菜单条顶点色族（首项 0x22c0c0c0 vs port 0x5bc0c0c0 等——入场
  脉动相位差）与菜单批次 count 差；
- 其后：粒子 quad 矩阵路径化（world_quad+TFACTOR）。


### 8.10 [轮5 2026-10-23] ULP 根因定案 + 菜单色序全解 + LOGO 族逐帧身份

**ULP 根因（已修，e82a2e8c）**：ANM 插值权重 = **逐步 f32 链**（t→f32、
u=1−t→f32、w→f32、积→f32、和→f32；flip_probe 14/14 帧位级命中；差异点
在 u 先舍入）。**与 ECL 0x473a40 的 double 链不同源**——f159 修的是 ECL，
不得外推到 ANM。翻转窗 60 帧全部位级一致，标题窗 300→349。
取证工具：`oracle/flip_probe.py`（0x46f3f0 尾插 watch 捕获 VM，逐帧 dump
pos/rot/scale/alpha/layer，f32 位型双写；LOGO 族 94-100 全体捕获）。

**LOGO 族逐帧身份（probe 实测）**：vm+0x24 层字段 96-100 全程 25（+1 全在
绘制侧）；97 完成于 f349（px 精确 (132,150)、rz=π、scale=1、alpha=0x80）、
此后慢速自转（ins_415 0.0098/f）至 f483 回收；98 于 f349 由 97 spawn
（scale 10f→4.0、rot 10f→π、alpha 128→0 十帧亡，色 0x51bbbbff→0x004040ff）；
96/100 于 f350 spawn（96 alpha 10f 0→255 = f349 首绘 0x33 谱系）。
f349 剩余分歧 = 层桶结构（97@L25 与 98@L26 分属桶+批序），导出员域。

**菜单色序全解（已修，94111a4e；稳态 82/86 帧全对）**：boot 渐升帧 0-10
线性 17/帧→尾段 {249,255}；就绪闪场 {142,79,35,8}/隐4/{214,244,255}；
移动闪场 {163,91,40,10}→ff；禁用项恒 0xa0000000；无脉动。遗留 4 帧
（f419-422 移动帧）= 白条冻结插值闪现（n24/n22 多 1-2 quad）——
AnmUpdate 语义裁定域（§8.2），非菜单层可修。

## 附：待锚定清单汇总（消费顺序即优先级；[oracle 2026-10-22] 标注本轮回写后状态）

1. 主菜单 10 项 / 难度 5 项 / 角色 7 项的位图文字语义——§8.4 视觉模型交叉读已出
   候选（人类辨认终审待办）。
2. [oracle 2026-10-22 已锁] 主菜单选中切换 = 仅 interrupt(0,7+new)；boot 展开
   全项 7+k；禁用项直落 0xA0000000（机制待 supervisor 反汇编）。
3. [解剖轮2 修正] 难度屏无独立数字行——golden 帧右侧四位数字属徽记/粒子层
   误读；角色屏数字行 63-82 语义仍待解剖（窗B）。
4. 角色 7 项构成（0/5 行语义）——窗B 解剖待做（本轮维持 script2/129 现实现）。
5. 根 129/130 分支条件（通常/EX 武器页）。
6. 光标 111/112 的屏幕归属（全窗无创建；难度光标已证 = 119）。
7. LOGO 94/95 的启用时机（返回用 95 为推定）；背景 92/93 label 2/3 的调用点。
8. TitleScreen 枚举真值、return_reason 值域、demo 计时（需 supervisor 反汇编，
   锚点待登记 `web/cpp/rebuild/AddressMap.hpp`）。
9. 菜单项禁用语义（Replay/PlayerData/MusicRoom 分期禁用后灰态处理）——与真机
   10 项全亮不同，需在覆盖矩阵登记为「有意偏差」或补齐。
10. [oracle 2026-10-22] AnmUpdate「暂停冻结插值」语义裁定（§8.2 白条证据；
    解剖轮2 的白条改为 duration=0 中和 + 保留 VM，待裁定后回归脚本驱动）。
11. [oracle 2026-10-22] 背景半张层值偏移/搬移（§8.3）+ 捕获管线层错记
    （draw_compare 前必须修，VM 真值已证恒对）。
12. [解剖轮2] 粒子出生节奏/次序对拍（17@f500→55@f599；移植侧先按突发 17+
    1/3 帧）；行条 83/84 落位动画驱动方式（label 序列未解剖，本轮建而不驱动）；
    徽记下滚 114 的 supervisor 序列两点折线待精化；光标移动（↑↓）真机
    0x473210 响应待窗对拍。
13. [解剖轮2] spell 屏 sig.anm 槽位重载（f491，20 VM@L22）与角色屏解剖（窗B
    f590 Z 之后）——下一轮。
14. [解剖轮2] harness 镜像同步两项：boot 期 @R 边条 ×4、sig 右半层 26
    （tests/harness/harness.cpp 装载幕镜像，非标题链路所有权）。
15. [解剖轮2] Extra 解锁后难度第 5 行表现（行为层 5 项含やめる；解锁换行待锚）。
