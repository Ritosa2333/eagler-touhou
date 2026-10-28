# 场景覆盖矩阵（coverage matrix）

> 覆盖矩阵唯一 home（AGENTS.md「覆盖矩阵与交付门禁」§1）。每阶段交付前
> 更新；**「未覆盖/销账条件」必须被下一阶段消费**，不得默认通过。
> 形态：行 = 场景窗（用户会看到的每一类画面），列 = 两侧链路状态。
> 状态取值：✅ 两侧对齐（附裁决数字）/ 🟡 窗已建待推进（附首分歧）/
> 📦 未建 / ⛔ 有意偏差（附理由）。
> 更新纪律：引用任何数字前重跑实测（比对工具纪律 §4——绿基线会腐烂）。

## 2026-10-24 审计更正

普通开局曾漏设 `SPELL_ID=-1`，battle_oracle 实际跑了符卡练习。旧“M1全绿/死亡同拍/参考基准就绪”声明撤回。`0x404140/0x4042a0/0x404b50` 是AsciiInf文字渲染，不是STD；`0x47b16d`是localtime64，不是分配器。当前参考窗必须重新获得资格，不能对移植侧拟合错误场景。

本轮实跑：容器Node 11/11顶层用例、独立ANM 25例、Python捕获完整性/开局/时钟/分配边界单测10例通过；网页构建、浏览器统计、像素canary和全golden未复跑，因此不声明交付全绿。

## 一、场景窗矩阵（裁决 = draw_compare RESULT / 事件窗 RESULT）

| 场景窗 | oracle 捕获面 | 移植侧导出 | 状态（2026-10-24 实测） | 备注 / 销账条件 |
|---|---|---|---|---|
| 标题窗 A（主菜单 600 帧） | fresh `audit-title-fresh.jsonl`，两跑SHA相同 | fresh `audit-title-restored.jsonl`，两跑SHA相同 | 🟡 f487/600（81%，辅助窗） | 首差select00 RGB 66≠64；只确定色曲线不同，未证明是ins_408本体缺陷。禁止绝对帧颜色表；主线W-S1参考资格仍未完成 |
| 标题窗 B（开局链 750 帧） | `title_oracle.py` 窗 B | harness `title-b` | 🟡 f379 | 与窗 A 同根因族（f379=count 22≠20 属 A/B 输入序列差异面） |
| 对局窗·无输入（st01 3600 帧） | `render_oracle.py` → `st01-draws.jsonl` | harness `draws` | 🟡 **f511 RAW**（2026-10-23 机制层双修后） | f61 位差（0x40000 屏幕原点 320）与 f0 tfactor（创建点色步进）已销账；新前沿 f511 = op[64] 顶点 y 1px（x87/SSE 数值语义面）；事件流同窗 10187 **none** 硬基线 |
| 对局窗·回放（udSr01 11065 帧） | `st01-draws-replay.jsonl` | harness `draws-replay` | 🟡 **f124 RAW**（同上） | **本窗是对局主线**：新前沿 f124 = op[16] count 6≠4（批内多一精灵——回放输入驱动的下一机制面） |
| 对局窗·真机战斗（th13_01 11675 帧） | battle_oracle 已修复SPELL_ID普通开局；600帧65009 ops、普通敌机f61、无提前Boss，带/无探针双跑哈希相同 | ❌ 尚未同构对拍 | 🟡 参考资格未完成 | 修时钟后11675帧/2026812 ops完整捕获且无崩溃，但global f6972进入GameOver、后续敌机更新停摆。Replay seed/起始资源/六字节输入采样相位仍须自证；不能由捕获完整推出Boss击破/关末/Stage2链完成 |
| 难度窗 | ❌ 未建（真机行为已锚：蓝图 §8.6） | ❌ | 📦 | 入场条件：窗 A 的 ins_408 工作面销账后（难度屏构建期 vm120 自 spawn 崩溃已修） |
| 角色窗 | ❌ 未建（蓝图 §8.4/窗B 解剖部分完成） | ❌ | 📦 | 窗 B 前沿推进后 |
| HUD 窗 | ❌ 未建（捕获面研究：`hud-capture-plan.md`） | ❌ | 📦 | oracle 捕获面扩展：0x40e4d0 族不在现捕获面——需假设备扩展 |
| 暂停窗 | ❌ 未建（脚本 132-133 已锚） | ❌ | 📦 | 同 HUD 窗 |
| 结算/GameOver 窗 | 已定位localtime64(-1)来自未实现的Win32 FILETIME输出；补确定性合法UTC时钟，保留真实转换与绘制链 | ❌ | 📦 独立窗未建 | 先自证Replay初始条件；结算时两侧须使用同一环境时间。禁止把错误场景的GameOver时间当原局锚点 |
| Boss 战窗 | ❌ 未建 | ❌ | 📦 | 对局窗主线收敛后 |
| Stage2+ | ❌ 未建 | ❌ | 📦 | 资产已备齐（st02 全套）；全链路验收口径含 Stage2 入场 |

## 二、捕获面边界（oracle 侧，§4 已知边界对位）

- **battle_oracle（M1，对局窗真机面）**：ANM mgr 两池全部精灵 + 敌弹
  （bullet1/2/3.png 逐批）+ 神霊/符卡/立绘/立绘框 + HUD（front00/agauge/
  lifebar/cardbg/ename 全族）+ 玩家 pl00 + 效果族（etbreak/eff_base/
  eff_line/eff_aura）——HUD 窗原「未捕获」已在 battle_oracle 面销账。
- 旧 render_oracle（无输入窗）现覆盖：ANM mgr 两池全部精灵（敌机/ECL
  挂接/装载预置）。
- **尚未证明覆盖**：①真实STD背景（0x404140族是AsciiInf，旧“STD已销账”无效）；②结算排行全链绘制与日期环境；③对话文字与字形内容；④每draw的viewport/render target；⑤quad完整顶点/UV与UP图元数对应的全部顶点。后两项补齐前，当前draws一致不足以推出画面必然一致。
- 纹理内容（THTX/GDI/后台拷贝）不比——契约既定（draw-contract §4.1）。

## 三、事件流窗（行为侧裁决）

| 窗 | 状态 | 数字（2026-10-24 实测） |
|---|---|---|
| st01 无输入 | ✅ | 3600 帧 / 10187 事件 `none`（2026-10-23 整改后复跑） |
| udSr01 回放（st01 段） | 🟡 | f122（2026-10-23 fresh oracle 基线复跑同值=真分歧；根因=卫星追尾弹模型两侧均为占位——**旧 bypass 基线的击杀/弹幕面从未对照原版（M1.3a 解剖实证）**；W-S1 以 battle_oracle 为准重建） |
| th13_01 回放（st01 段） | 🟡 | 旧f2625为历史登记，本轮未复跑；新battle_oracle修正普通开局后参考资格待验。f278/f814/f1160死亡同拍声明撤回，来源是错误符卡练习场景 |
| th13_01 战斗窗·raw状态流 | 🟡 | 实验schema `th13-state-raw-v1`：玩家位置/power/原始地址值，含frame0；撤销把ctl+28叫score、be810/814叫lives/bombs的错误命名。不是事件契约v3验收 |

## 四、有意偏差登记（⛔）

- 菜单项禁用语义（Replay/PlayerData/MusicRoom 分期禁用后灰态）——与真机
  10 项全亮不同，`title-anm-blueprint.md` §8.5 登记；分期功能补齐后销账。
- th13 分叉的设备态语义（TFACTOR 初始 0 等）非偏差：已参数化到本作配置点
  （渲染基座纪律 §1），蓝图 §8.8 登记。

## 五、门禁集（战报前全量复跑；ADAPTATION.md 为验收清单 home）

1. 容器单测四套：`bash tests/{anm,ecl,ui,replay}/run.sh`（62 例）。
2. `node --test tests/*.test.mjs`。
3. 浏览器渲染统计契约：`make shot CMD='node --test …/graphics-stats.test.mjs'`。
4. golden 对跑：`node tools/replay-verifier/run-oracle.mjs && run-gate.mjs`
   （退化即 FAIL——golden 只升不降，改进须 `--accept-golden` 回写）。
5. 双目标构建：`node portable/build.mjs` + `build-harness.mjs`。
