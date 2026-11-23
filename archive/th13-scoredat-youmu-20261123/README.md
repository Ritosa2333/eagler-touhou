# TH13 scoreth13.dat 四角色 + 合计列原版直调验证

TH13 原版 `score` 结构是 5 个 `0x56DC` 记录列：霊夢、魔理沙、早苗、妖夢、合计。每列内含 6 组难度榜表、游玩计数、练习行和 127 条符卡记录。本次修复把端口读写全部对齐这条坐标系统：妖梦完整进入导入导出，符卡个人列与合计列分别往返，续关数、游玩次数、游玩帧数和 Extra 解禁位不再留在应用档单侧。

验证直接驱动原版 `th13.exe`：在妖梦列写入分数、续关、计数、符卡与 Extra 位，调用原版保存入口 `0x449e50` 生成 `scoreth13.dat`，再调用原版装载入口 `0x449be0` 读回。全部字段从妖梦列原值返回；同一轮生成的五组黄金文件也由普通版与 MP 的 ScoreDat 容器逐字节重建通过。原始数值、文件哈希和镜像哈希见 [youmu-original-roundtrip.json](youmu-original-roundtrip.json)。

普通版与 MP 原生回归均通过：ScoreDat 黄金容器 9/9、FileStore 通道 6/6（含原版妖梦样本导入）、角色×难度映射 1/1，MP 侧共 2,456 条断言；原生全流程以 `th13_01.rpy` 走完 113,897 步并导出 92,149 字节 Replay，与原文件逐字节相同（[native-e2e.json](native-e2e.json)）；MP M1–M9 累计原生工作流 56/56、78,781 条断言通过，覆盖单机存档隔离、回滚、终止保存和旁观晚加入/慢旁观隔离，逐 case 结果见 [mp-native-workflow.json](mp-native-workflow.json)。UI 全量原生套件在 `th13-dev` 容器中全部通过。浏览器 Stage 2 冒烟用真实页面与真实键盘输入路径完成标题开局、Stage 1 全程和 Stage 2 关首分数硬门，期望分 `647062` 精确匹配，报告见 [browser-stage2-smoke.json](browser-stage2-smoke.json)。

浏览器完整 1CC 链路同样全绿：真实页面从标题打到六面 Ending、名录入、Replay 保存与 `scoreth13.dat` 导出，六关分数硬门全部精确命中；导出 dat 里霊夢 Normal 榜首即本局成绩 `16999054 / ARINATEI / stage 8`，ST 上次名同名，游玩次数、帧数与通关计数齐备。导出件经 `oracle.scorefmt` 与原版装载入口 `0x449be0` 直调双重读回通过，官方 C.4 断言（目录恢复、Replay 实播、整页刷新保真）PASS。镜像按帧分片压缩，保存点后由驱动泵帧至 `th13.scoredat` 稳定再采样。完整数据、哈希与时间线见 [browser-final-pass.json](browser-final-pass.json)。
