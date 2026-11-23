# th16 待办

对照 `docs/DECOMP.md`、`web/cpp/rebuild/AddressMap.hpp` 与黄金测试后的未达交付项。`DECOMP.md` 已登记对齐的系统（Laser、Bomb、Gui、标题各画面逻辑、过关、回放文件、EffectManager、魔理沙激光、RT 合成、视锥剔除、关卡畸变）不在此列。`docs/plans/roadmap.md`「顺序」早于上述登记，不以它为准。

`flags & 0x1000` 矩形体碰撞（0x443af0 / 0x4049c0）无 ECL 设置路径，不排期。钩子 6（0x46a330，ANM 渲染模式 10）全部 `.anm` 无 302(10)，数据不可达，不排期。

## 本轮交付项

1. 已关闭：难度画面绘制窗口第 344–353 帧已对拍。见「已关闭」。
2. 已关闭：Extra 四季 600 帧矩阵已锁。见「已关闭」。
3. 标题画面绘制对拍已关闭。见「已关闭」。
4. 已关闭（数据不可达，不排期）：ANM mode 6（0x466820）在 `artifacts/th16/dat` 全部 54 个 `.anm` 的 ins_302 渲染模式中没有实例，实际只有 0/1/2/3/4/8/21。与钩子 6 相同，`AddressMap.hpp` 的空登记保留为说明。
5. 已关闭：长程像素对拍扩到整段，二十九段 1594703 帧逐帧位级一致。见「已关闭」。
6. 已关闭（容器性能门禁）：WASI 3600 帧逻辑三次 0.334–0.363 秒，逻辑加完整绘制三次 8.230–8.451 秒，均通过 <10 秒门禁；normal Runtime 与发布打包已验收。Snapdragon 865 级 60fps 保持真机待验证。见[验收记录](../../../../archive/th16-delivery-20261123/README.md)。
7. 已关闭：启动器工作区、构建配方、产品目录、`docs/TOUHOU_PROGRESS.md`、`ports/README`、`WORKSPACE.md` 与 `ADAPTATION.md` 登记和验收完成；`docs/migration` 与源码哈希清单由收尾快照固定本轮身份。全程不开 PR。
8. 已关闭（可后接）：版本文件 0x43b480（`th16_%.4x%c.ver`）开机 0x43b520 读取并写入 DAT_004c1af0 / DAT_004c1aec，退出路径 0x43b681 释放缓冲，没有其它读者。宿主落盘是后续可选增强，不影响原版等价与本轮交付。

## 已关闭

- 难度画面绘制窗口第 344–353 帧已对拍（第 313 帧确认进入，第 334 帧 Difficulty，第 342 帧光标下移）。测试 `difficulty screen: 10 draw frames around a cursor-down match original structure and vertices`。
- Option 画面绘制窗口第 425–434 帧已对拍（第 376 帧确认进入，第 397 帧 Option）。未读 `th16.cfg` 时配置块保持 BSS 初值 0，`0x44dc70` 按该音量藏前导零。测试 `option screen: 10 settled draw frames match original structure and vertices`。
- Replay 画面绘制窗口第 360–369 帧已对拍（第 322 帧确认，第 343 帧 Replay，第 351 帧子状态 2）。测试 `replay screen: 10 settled draw frames match original structure and vertices`。
- Player Data 画面绘制窗口第 362–371 帧已对拍（第 324 帧确认，第 345 帧进入，第 353 帧子状态 2）。测试 `player data screen: 10 settled draw frames match original structure and vertices`。
- Music Room 画面绘制窗口第 370–379 帧已对拍（第 326 帧确认，第 347 帧进入，第 362 帧子状态 2）。测试 `music room: 10 settled draw frames match original structure and vertices`。
- Spell Practice 画面绘制窗口第 360–369 帧已对拍（第 320 帧确认，第 341 帧进入，第 353 帧子状态 2）。测试 `spell practice: 10 settled draw frames match original structure and vertices`。
- Manual 画面绘制窗口第 360–369 帧已对拍（第 330 帧确认，第 351 帧进入）。help.anm 由装载线程写入，标题路径不跑该线程，目录项不出现。测试 `manual screen: 10 settled draw frames match original structure and vertices`。
- Extra 四季 600 帧矩阵已锁：第 7 面 4 机体 × 春夏秋冬（子季节 0..3）× 难度 Extra，16 组十个摘要字段与 `game-oracle.py` 一致，写入 `tests/golden/stage-matrix-st7.json`，原有 4 条土用 case 保留，由 `tests/stage-matrix.test.mjs` 驱动 harness `game-digest`。
- 第 1–6 面 Easy 灵梦夏/秋/冬第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 Easy Reimu summer/autumn/winter: frames 590–599 match original vertices`。
- Extra 四机体春夏秋冬第 590–599 帧绘制顶点已对拍。测试 `Extra four shots spring/summer/autumn/winter: frames 590–599 match original vertices`。
- 第 1–6 面 Normal 四机体夏/秋/冬第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 Normal four shots summer/autumn/winter: frames 590–599 match original vertices`。
- 第 1–6 面 Hard 四机体夏/秋/冬第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 Hard four shots summer/autumn/winter: frames 590–599 match original vertices`。
- 第 1–6 面 Lunatic 四机体夏/秋/冬第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 Lunatic four shots summer/autumn/winter: frames 590–599 match original vertices`。
- 第 1–6 面四难度四机体春第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 four shots spring four difficulties: frames 590–599 match original vertices`。
- 第 1–6 面四难度四机体四季与 Extra 四机体春夏秋冬土用、空闲输入第 2000–2009 帧绘制顶点已对拍。测试 `stage 1–6 and Extra idle: frames 2000–2009 match original vertices`。
- 第 1–6 面四难度、四机体、四季在空闲与矩阵射击输入下的第 4000–4009 帧抽样已对拍。测试 `stage 1–6 idle and firing: frames 4000–4009 match original vertices`。
- 第 1 面 Easy 灵梦春、第 3 面 Normal 灵梦春、第 6 面 Lunatic 灵梦冬空闲第 6000–6009 帧，以及 Extra 灵梦春空闲第 4000–4009 帧已对拍。测试 `stage 1, 3, 6 and Extra: frames 6000–6009 and Extra 4000–4009 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 6000–6009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 6000–6009 match original vertices`。
- Extra 灵梦春夏秋冬土用空闲第 6000–6009 帧已对拍。测试 `Extra Reimu spring/summer/autumn/winter/doyou idle: frames 6000–6009 match original vertices`。
- 第 1 面 Easy、第 3 面 Normal、第 6 面 Easy 灵梦春与 Extra 土用空闲第 8000–8009 帧已对拍。测试 `idle frames 8000–8009: stage 1 Easy, stage 3 Normal, stage 6 Easy, Extra doyou match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 8000–8009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 8000–8009 match original vertices`。
- 第 1、3、6 面 Easy 灵梦春与 Extra 土用空闲第 10000–10009 帧已对拍。测试 `idle frames 10000–10009: stage 1 Easy, stage 3 Easy, stage 6 Easy, Extra doyou match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 10000–10009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 10000–10009 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 12000–12009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 12000–12009 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 14000–14009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 14000–14009 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 16000–16009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 16000–16009 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 18000–18009 帧已对拍。测试 `stage 1–6 Easy Reimu spring idle: frames 18000–18009 match original vertices`。
- 第 1、2、3、5、6 面 Easy 灵梦春空闲第 22000–22009 帧已对拍。测试 `stage 1, 2, 3, 5, 6 Easy Reimu spring idle: frames 22000–22009 match original vertices`。
- 第 6 面 Easy 灵梦春空闲第 24000–24009 帧已对拍。测试 `stage 6 Easy Reimu spring idle: frames 24000–24009 match original vertices`。
- 第 6 面 Easy 灵梦春空闲第 25000–25009 帧已对拍。测试 `stage 6 Easy Reimu spring idle: frames 25000–25009 match original vertices`。
- 第 1–6 面 Easy 琪露诺/文/魔理沙夏/秋/冬第 590–599 帧绘制顶点已对拍。测试 `stage 1–6 Cirno/Aya/Marisa summer/autumn/winter: frames 590–599 match original vertices`。
- 第 1–6 面 Easy 灵梦春空闲第 20000–20009 帧已对拍。第 4 面此前的错位来自敌机析构 0x41ba10 漏清 Boss 槽：槽内残留已消失的 id，0x417580 落到链表最后一只，Gui 给同一只 Boss 画第二套血环，ECL 520 也一直等待，第 4 面空闲跑因此不过关。补上清槽后第 42223 帧过关进第 5 面，与原版逐帧一致。测试 `stage 1–6 Easy Reimu spring idle: frames 20000–20009 match original vertices`、`stage digests match oracle: st4-easy-reimu-spring-idle-stage-clear-43000`。
- 第 4 面并入 Easy 灵梦春空闲第 22000–22009 帧档。测试 `stage 1–6 Easy Reimu spring idle: frames 22000–22009 match original vertices`。
- 十段整段逐帧位级一致（`long draw stream matches original` 的 st1-easy-reimu-spring-idle-23000 / st1-easy-reimu-spring-fire-23000 / st1-lunatic-aya-autumn-idle-21500 / st2-easy-reimu-spring-idle-29000 / st2-easy-reimu-spring-fire-29000 / st2-hard-cirno-winter-idle-29500 / st3-easy-reimu-spring-fire-40200 / st4-easy-reimu-spring-idle-42800 / st4-easy-reimu-spring-fire-42800 / st4-lunatic-marisa-doyou-idle-46400）。整段对拍修正两处浮点次序：屏幕四边形 UV 先舍入（滚动 + 基 UV）再加（差 × uv_scale）（0x465480）；D3DXMatrixRotationX/Y/Z 的 sin / cos 取双精度结果再截 float，不用单精度库函数。
- 二十九段整段逐帧位级一致，共 1594703 帧（`long draw stream matches original`）：全程空闲 190600 帧直到结局；第 1–6 面与 Extra 的空闲、射击 + 炸弹、其他机体 / 难度 / 子季节共二十一段；通关整段七段（第 1、2 面点射通关进下一面，第 3–5 面持续射击 + 炸弹依次通关，第 6 面两种机体点射通关到结局）。整段对拍沿链路再对齐六处：曲线激光首帧只有一个采样点时，0x438750 末尾仍设置状态并提交 PrimitiveCount = 0 的条带（0x4681f0 不检查顶点数）；对话指令 14 按参数直接取 Msg+0x44 起的 VM 槽、不做掩码（0x42aff8）；缩放中的敌弹在 0x412a60 把缩放写进 VM 的 scale2（子弹 +0x80 = VM + 0x58），与出生闪光脚本的 scale 插值相乘；Boss 帧钩子 0x4253b0 清空火力与季节量表后调 0x42c600 刷季节量表 HUD；0x42e150 在第 6 面、Extra 通关时置的 0x4000 同步到自机的射击条件（0x442560）；游戏速度的读取时机（0x45f980 / 0x416840 / 0x406190 / 0x402ff0 直接读 DAT_004a5788，0x41b3d0 逐敌机入口取值，ECL 步进的 elapsed 取调用当时的值）。场景切换帧两侧都不记录（oracle 在那一帧仍画游戏场景）；输入脚本超过命令行单参数上限时用 `@<路径>` 从文件读入。
