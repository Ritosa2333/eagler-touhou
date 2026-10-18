# BulletInf 源码恢复

本模块实现真实 `Bullet`（0x528）和 `Controller`（0x286da8）对象、2000 个活动槽与第 2001 个哨兵槽，使用普通 C++、shared_ptr、PMR 和已恢复的引擎模块。

已经写出的源码包括构造/生命周期、帧调度、九种运动、屏幕反弹/绕回、回收、矩形/圆形消弹、物品掉落、50 种弹种数据、扩展指令、13 种发射排列、自机碰撞和擦弹外围。`Command`/`ShotMetadata` 分别为 0x2c/0x4c；原版数据表由 `tools/recover_bullet_styles.py` 将 401280 的数值初始化翻译成 `style_data.cpp`，没有内嵌原机器码。

`pool_validation.json`、`commands_validation.json`（若已生成）及 `sprite_renderer/pool_cpu_validation.json` 是分层原 CPU 对照证据，每份报告只适用于其绑定的源码哈希。基础池/运动/反弹与通用扩展指令已通过原 CPU 对照；新增更换弹种、发射、自机碰撞须查看最新测试范围，不能据“已编译”推定通过。

当前扩展指令的敌机生成（case24）和激光生成（case27）仍依赖未完成模块。`unrecovered` 命名空间中有些旧接口现已有真正源码定义，未定义的接口不会链接出虚假的完整游戏。完整游戏与全关卡同输入逐帧验收尚未完成。

原版在部分非法输入上崩溃或无限循环；源码保留已确认的边界语义，不用额外截断改变正常关卡行为。NaN 反弹已对照修复；屏外延迟 NaN 位置触发原 CRT 初始化缺口的 oracle 样本在测试报告中单独列出。
