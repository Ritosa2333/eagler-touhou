# 成就页、标题帧和生命周期

`stones.cpp` 恢复 `0x52a8a0`，`stones_draw.cpp` 恢复 `0x52b480`。
虽然保留了历史接口名称 `stones`，该页面显示的是 41 个成就记录，
不是 `WeaponStoneInf` 武器控制器。前 18 项可播放结局，18..33 项可按
保存的角色/配置进入 Extra 回放路径。标题和说明取实际 `trophy::messages`，
保存状态取实际 `SaveManager`；未新建替代存档。

`last_stone_record` 是 `0x5c6130` 的唯一 BSS 存储。隐藏输入使用独立的
`0x5c6550..0x5c6857` 三组 256 字节键盘状态与两个计数器，不能与玩家数据页的
解锁输入合并。序列在 `0x5755cc` 是八个 DWORD；`extract_stones_data.py` 校验
原 EXE 的 SHA-256 后导出该只读数据和坐标/CP932 文本，证据写入
`evidence/stones_data.json`。解锁操作调用共享 `unlock_all_data` 与 RNG 流 1。

`oracle/stones_compare.inl` 比较完整 `0x52a8a0` 原函数的 16,384 个场景，
每个场景检查 Title 全对象、Session 全对象、776 字节键盘状态、两组游标历史、
8 个全局、返回值、外部调用顺序，共 114,688 项通过。资源、声音、存档和页面
切换终点采用对等的测试服务；测试包含原代码保留的键盘返回值 2 分支，但
实际 Win32 `read_shortcut_keyboard` 只返回 0 或 1。绘制的 Renderer、文字、
成就数据完整对照由共享绘制验证另行记录。

`frame_core.cpp` 恢复完整 `0x51e3c0` 更新与 `0x51f220` 绘制分派，
`frame.cpp` 将其接到实际生产服务。`FrameEnvironment` 只引用现有全局，
并不拥有另一套游戏状态。自动演示检查 `ReplayInf::playback[i].stage`
（原 `0x488770`，`+0xe8+i*0x2c+0x10`），而不是 `ReplayInf::stages[i]`。
原反编译将 `0x51e3c0` 错标为 void，但汇编在 `0x51e419/0x51ed19` 两条
退出路径都明确将 EAX 设为 1，`0x52c1e0` 原样转发。源码同样返回 1。

`oracle/frame_compare.inl` 保持两个原函数本体不变，覆盖 state0 的 9 种
菜单来源、各更新页面、结局提前返回、音乐延迟、自动演示四个文件和八个关卡
槽、两组 RNG 种子、三个计时器，以及页面调用之后的计时顺序。夹具故意使
`stages[]` 与 `playback[].stage` 不同来检查字段来源。首轮 16,384 帧 × 8 项加
4,096 次绘制分派 × 3 项，共 143,360 项通过；另加每帧返回值检查，后续总量以报告为准。页面本体、资源/音频/随机设备
终点仍分别验证；这不是整体 GPU 帧或完整游戏回放的验收。

`lifecycle_core.cpp` 保留 `0x51db00` 构造、`0x51dea0` 析构和
`0x51f3c0` 初始化控制流；实际平台服务在 `lifecycle.cpp`。
Title 为 `0x5978` 字节，五个 Cursor 位于 `0x24/0x70/0xbc/0x108/0x56e8`，
三个 Timer 位于 `0x154/0x5904/0x5914`，真实 Worker 位于 `0x5968`。
`ui_flags` 构造仅清低四位，原工厂先清零整个对象。
带有 “initialize/shutdown TitleInf” 字符串的 `0x40c6b0` 实际是空的 CRT
检查 helper，没有日志副作用。工作线程、文件就绪等待和完整场景线程竞争
需要独立运行验证，不能由构造测试自动推定。

当前有效检查数与编译输入哈希以 `cpu_validation.json` 为准。源码变更后须
重新编译并运行对应 oracle；不能仅重写报告的哈希或把局部测试称为全游戏 1:1。
