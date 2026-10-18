# STD 场景源码

`Background` 为原 `0x336c` 对象，内含 `0x3310` 的脚本状态。源码读取原版 STD/ANM 数据，执行背景指令、摄像机与雾色插值、对象旋转、视锥裁剪、分层绘制和网格扭曲。生产代码不执行原 EXE。

验证分开记录，不能把局部检查合并理解成整个游戏已经一致：

- `state_cpu_validation.json`：62,048 项，两个构造器完整字节与雾色运算/插值。
- `vm_cpu_validation.json`：43,080 项，原 `4751b0` 完整脚本状态，时间边界、摄像机模式、五组插值及大部分指令。
- `../sprite_renderer/pool_cpu_validation.json` 和 `object_draw_validation.md`：真实 ANM/对象绘制链、正编号动画绑定、中断、网格、背景更新/绘制的原 CPU 对照。该报告也包含其他 sprite 模块，不能把总数全部归给背景。

已恢复加载、回调注册和释放源码；完整背景工厂/真实资源加载的生命周期仍需更大范围的集成验证。过渡场景创建 ScreenInf 的几何分支尚未纳入背景整段 oracle；ScreenInf 自身在 `../screen_effect` 单独验证。完整关卡的同输入逐帧/画面验收仍未完成。

从工程根运行 `build_sources.ps1 -Validate` 会编译本模块并运行状态与 VM 对照。带原机执行的测试只接收哈希匹配的只读原 EXE；最终生产库没有该测试映像。
