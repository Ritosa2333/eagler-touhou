# 受击蓄力停止请求测试

这里保存实际执行的测试源码。生产PlayerHitHost和原pl00 ANM真实执行；初始快照、效果输出和音频请求接收器为显式组件接缝。无游戏World、原EXE执行、物理音频或通关历史。

- `fixture.cpp`：647首RED、648同组GREEN的源码，39项检查/6个Host/11次窗口调用。真实confirmedOverlap进入state4后验证31→70停止请求顺序、31/70拒绝、70抛错/true带error、失败不推进定时器/保护/ANM、后续不重复及零held对照。
- `player-hit-host-regression.cpp`：649执行的完整现有Host回归主程序，只将旧单stop契约改为31→70两次，其他hit/death/respawn/entry/motion/camera拒绝检查保留。主程序返回0；没有声称固定检查数量。
- `../core106-abi-noowner.mjs`：651使用首次构建的106实例，检查205个导出为函数、分配内存往返及5项无会话拒绝；不执行205个接口的完整功能。

两个C++程序须通过Emscripten将固定pl00资源嵌入虚拟路径`/pl00.anm`；649还使用显式camera-mutant测试文件。646因未嵌入资源而在0个Host处停止，原始失败保留，不能当作产品RED。精确本地资源/命令、输入pins和准入request未随仓库分发。

历史`tests/native/player-hit-host.cpp`是旧105契约，也是649不可变输入，不能用它的单停止请求预期评价106。使用本目录的更新版本。旧成功目录和旧核心不可为重复PASS重跑。完整来源及边界见[646–651记录](../../docs/native-held-sound646-651.md)。
