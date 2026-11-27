# th16 代码质量与可读性标准

本文件规定 `ports/th16` 的代码与文档写法。**代码首先是写给人读的**——读代码的人应当不用打开 Ghidra、不用数结构体字节偏移，就能看懂每个类在游戏里扮演什么角色、每个字段代表什么物理量、每段分支对应什么游戏规则。反编译地址与原始偏移只作为可追溯的锚点保留在注释里，绝不能反客为主变成代码的表达方式。

---

## 1. 命名：用领域语言，不用反编译符号

1. **禁止把 Ghidra 符号或偏移名带进标识符**。
   - 禁：`flags_1ac`、`timer_154`、`vm_110`、`unknown_c7c`、`f04`、`f80`、`iVar1`、`param_1`、`func_4314a0`。
   - 倡：`stage_clear_active`、`dialogue_timer`、`boss_title_vm`、`tick_interval`、`radius`、`LaserManager::create()`。
   - 若某个原版字段的语义在当前已还原的代码中确实未被读取（仅做清零或透传），取名需体现其所在结构或已知用途（如 `reserved_slot`、`unused_z_velocity`），并在行尾注释标明原偏移与「原版仅写不读」。
2. **类型、枚举、常量必须具名**。
   - 状态机的值（如自机状态、弹幕状态、激光状态、场景号）一律定义具名 `enum`；不在业务逻辑里裸写 `state == 2`、`flags & 0x4000`。
   - 位标志一律定义为具名常量（如 `kStageClear = 0x4000`、`kFlagNoCancel = 1u`），组合判断写辅助内联函数（如 `is_collidable()`、`is_frozen()`）。
3. **函数名表达「做什么游戏行为」**，而不是「调了哪个地址」。
   - 例：`LaserManager::cancel_all(bool drop_items)`、`LaserLine::collide_player()`、`Gui::roll_score()`。

---

## 2. 文件与模块结构

1. **一子系统一对 `.hpp` / `.cpp`**，职责边界与原版管理器一一对应：
   `AnmManager`、`Stage`、`Player`、`Enemy`、`Bullet`、`Laser`、`Item`、`Gui`、`Spellcard`、`Bomb`、`Effect`、`ScreenEffect`、`TitleScene`、`Supervisor`、`GameManager`、`GameSession`。
2. **文件头注释三要素**（控制在 15–30 行内，排版对齐）：
   - 一句话讲清本模块在整局游戏中的职责与生命周期（何时创建、挂在哪条更新/绘制链、何时销毁）。
   - 核心原版函数锚点表（`0x4314a0  构造与链登记` 等），方便与 `AddressMap.hpp` 和 Ghidra 对照。
   - 与相邻管理器的交互关系（例如 `LaserManager` 如何通过 `BulletManager::convert_cancel_item` 转化消弹道具、通过 `Player::graze` 结算擦弹）。
3. **类内排布顺序**：
   公开常量与枚举 → 公开数据成员（按语义分组，每组配一行组标题注释）→ 生命周期方法（`create` / `reset` / `destroy`）→ 每帧主入口（`update` / `draw`）→ 公开操作接口 → 私有辅助函数。

---

## 3. 函数与控制流可读性

1. **先写清主干，再拆子步骤**。
   - 每个 `update()` 读起来应当像一张帧内流程图：先做等待/冻结判断，再跑脚本或状态机，再推进位移，再做碰撞与出界，最后推进计时器。
   - 超过约 60 行的复杂分支（如激光矩形切分、曲线样条递推、Ex 指令解释）拆成命名清晰的私有方法，避免三层以上的嵌套 `if/else`。
2. **早返回（early return / continue）优先于深层缩进**。
   - 把守卫条件（如未激活、无敌、对话中、长度不足 16 像素）放在函数开头直接返回。
3. **数学与几何计算写出物理含义**。
   - 局部变量用 `cos_angle`、`sin_angle`、`local_x`、`half_width`、`step_vec`、`dist_sq`，不用 `fVar1`..`fVar9` 或 `x0`/`x1` 混用。
   - 当原版因 SSE 单精度顺序或 `wrap_angle` 次数有特定数值要求时，保留原版运算顺序，并用一句简短注释说明原因（例如「保持 `dx * cos - dy * sin` 的单精度乘减顺序以对齐原版」）。

---

## 4. 注释规范：写「为什么」和「原版规则」，不写废话

1. **禁止复述代码字面意思的注释**（如 `// 计数器加一`、`// 返回 0`）。
2. **必须注释的三类信息**：
   - **函数头锚点**：`// 0x432f40：直线激光每帧更新（Ex 指令 → 伸长/前进 → 出界 → 自机碰撞 → VM 推进）`。
   - **结构体字段注释**：对齐写明原版偏移与单位/含义，例如 `float width = 0;  // +0x74：激光全宽（像素）；判定半宽按 <32 / ≥32 两档折算`。
   - **非显然的原版特殊规则**：例如「直线激光前 1/10 长度不参与自机被弹判定」「激光擦弹每 3 帧才生成一枚得点道具」「`DAT_004a5bec` bit1 换关时 `game_rng` 种子取自 `visual_rng`」。

---

## 5. 精度、对拍与登记硬门禁

可读性与 100% 原版对齐并重，任何美化不得改变数值语义：

1. **浮点与随机**：全程 `float` + `-ffp-contract=off`；`game_rng`（`DAT_004a6d80`）与 `visual_rng`（`DAT_004a6d88`）严格分流，调用次序与次数逐帧一致。
2. **先 oracle 后实现**：新增或改动子系统前，先在 `tools/oracle/game-oracle.py` 导出原版状态快照与 FNV-1a 64 摘要，再写 C++ 实现并通过 `node --test` 黄金测试验证逐帧位级一致。
3. **双处登记**：每还原一个原版函数，同步更新 `web/cpp/rebuild/AddressMap.hpp` 与 `docs/DECOMP.md`，保持文档与代码实时一致。
