# 全作触点与音效核对 | 2026-10-30

对应 [TOUCH-09](../../docs/adaptation/input.md#touch-09) 与 [AUDIO-07](../../docs/adaptation/presentation.md#audio-07)。源码快照 `8d1f2d4a64409c0ae34ed625687dcaa1e1c5844c`，逐构建证据见 [checks.json](checks.json)。

## 修复

| 范围 | 内容 |
| --- | --- |
| th09 普通版与 MP、th15 普通版与 MP | 暂时禁移时保存移动触点，恢复后按当前位置重建拖动目标。th15 原有 Reisen 护盾例外合并为通用规则，决死 Bomb 脉冲继续逐帧消费。 |
| th12 普通版 | 接收 MP 已有的手势作用域连续性实现，旧 JS Shell 同步提供作用域。SDL 状态生产端两版均已有该字段。 |
| th12、th075 普通版与 MP | 预解码音效经共用 WebSfx 直接输出，音乐沿原有 SDL 混音队列。音量、暂停与资源销毁同步处理。 |
| th08、th10 两版、th11 MP、th20 | 将实际已参与编译缓存的 WebSfx 头文件补入编译输入报告。 |

## 逐构建范围

声音列覆盖正式编译单元中的上传与播放接线，以及 Node 构建入口的头文件身份。触控列按生产架构区分组件范围。所有构建均纳入动态枚举，未知输入架构直接失败。

| 作品 | 模式 | 触控检查 | 声音接线 |
| --- | --- | --- | --- |
| th06 | multiplayer | 生产增量消费函数，消费增量保留触点、释放后无输入 | 通过 |
| th06 | normal | 生产增量消费函数，消费增量保留触点、释放后无输入 | 通过 |
| th07 | multiplayer | 生产增量消费函数，消费增量保留触点、释放后无输入 | 通过 |
| th07 | normal | 生产增量消费函数，消费增量保留触点、释放后无输入 | 通过 |
| th075 | multiplayer | 生产 FighterControls，格斗动作切换保留方向、释放后中立 | 通过 |
| th075 | normal | 生产 FighterControls，格斗动作切换保留方向、释放后中立 | 通过 |
| th08 | multiplayer | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th08 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th09 | multiplayer | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th09 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th10 | multiplayer | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th10 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th11 | multiplayer | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th11 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th12 | multiplayer | 生产 TouchInput，状态作用域、禁移与换实例恢复、显式取消 | 通过 |
| th12 | normal | 生产 TouchInput，状态作用域、禁移与换实例恢复、显式取消 | 通过 |
| th15 | multiplayer | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th15 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |
| th20 | normal | 生产 TouchController，禁移与换实例恢复、摇杆、显式取消 | 通过 |


## 执行结果

- 共享工具 67 项、触点组件 16 份与音效接线 19 个变体通过。
- WebSfx 浏览器 PCM 3 项通过，包含音乐任务未完成时的音效输出及延迟输出反例。
- th12 普通版与 MP 的 SDL 测试为 27、43 项，触点专项合计 26 项通过。
- th15 两版各执行护盾、决死 Bomb 和动作夹具，共 6 组通过。
- th12、th075 两版和 th09 共用 Web 程序编译通过。th15 两版改动的生产 BrowserRuntime 分别编译通过；完整普通版编译在本机两次以 0xC000012D 中止。
- th12 普通版以真实素材执行 OGG stream、full 场景；MP 执行双人 lockstep、rollback，并完成各 3 轮旁观往返。菜单音效在真实 Runtime 中提交双声道 PCM，开始时间为 0；音乐错误恢复、暂停、回放及页面生命周期通过。

## 验收边界

th06、th07 的回归执行生产增量消费函数，配合触点所有者与 Player 调用链核对。th075 执行生产格斗输入消费代码，覆盖动作状态切换期间的方向持有；格斗位移与受击恢复由原生角色动作决定。其他作品执行生产输入类。全作实体触屏事件、死亡与决死窗口的游戏内重复操作，以及实际扬声器延迟另按逐作步骤验收。

适配表的两项共用检查均登记为部分覆盖，状态继续列为“待核对”，执行结果与完整验收分别记录。新增作品由产品构建目录自动纳入，新增输入架构需接入对应生产组件。

## 同步范围

th08、th10、th11、th12、th15 完成目录投影合并与写回，MP 内容树逐作核对为本轮审核快照，身份见 [sync.json](sync.json)。th075 的音效文件和构建入口登记在 audio-output 范围，当前历史投影工具支持的作品见[目录同步指南](../../docs/adaptation/port-sync.md)。

普通版此前新增的下列内容继续单独登记，监控基线保留在 [port-sync.json](../../docs/adaptation/port-sync.json)：

| 作品 | 待单独评估的普通版内容 | 监控 |
| --- | --- | --- |
| th10 | THPrac 被动面板触控交接 | practice、passive-touch |
| th11 | 新动画首帧、高刷位移阈值与标题菜单插值；THPrac 触控交接 | high-refresh、practice-pointer |
| th15 | THPrac 被动面板触控交接 | practice-ui、passive-touch |
