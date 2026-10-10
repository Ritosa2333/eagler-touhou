# TH15 高刷可视化工作台

隔离诊断 profile，复用固定 common 8316c4f 的 workbench/controller/analyzer。不用于生产部署。

设置 TH15_EMSDK 后运行 `node th15_web/scripts/build-sdl-application.mjs --presentation-lab`。
设置 TH15_LAB_FONTS 为原作 measured fonts 目录；可选 TH15_LAB_DATA 为私有日文 th15.dat。
运行 `node portable/presentation-lab/serve.mjs`，默认 http://127.0.0.1:8145/。

点击启动实验，正常键盘游玩；切换高刷/60Hz限制对比。冻结并检查、单步、alpha滑块与 F8 保存使用 shared Lab。
实体显示器应启用 120/144/165/240Hz；60Hz屏幕无法肉眼显示更高刷新率。
音乐关闭、独立内存 save，不读取或写入正式存档；Replay自动导入未接入。

Freeze 保留 Draw 引用而不调用游戏暂停/loop_stop；Resume 丢弃墙钟积压；单步调用原有完整 sample_and_tick，加载返回 blocked-loading。
Player 根 ANM 首顶点 X：权威 Player motion → 原始 Draw packet → immutable presentation → renderer submission；身份由 ANM generation 提供。
无同 generation 的旧 packet 时无记录；跳变不连续。其他字段、owner、完整 Replay/UI/audio/rollback 状态未知。
九模块 checkpoint 与 world probe 只是部分指纹，纯度必须保持 unknown。

基线 4350363；canonical 未提交的 Replay诊断改动未混入此隔离工作树。
