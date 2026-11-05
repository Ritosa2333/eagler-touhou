# TH19 蓄力触屏按钮

TH19 的触控 HUD 与布局编辑器显示独立 X 蓄力按钮，按住发送 X DOWN，松开发送 X UP。捕获丢失、触点取消、后台、布局编辑和 Runtime 切换均释放瞬时输入。Bomb 按钮显示 C，与蓄力分开。键盘、手柄的 TH19 专属操作名称统一为“蓄力”，帮助显示 X 蓄力／菜单取消、C Bomb。

TH11、TH13 的特殊功能继续使用 C 点按；其他作品的按钮可见性和输入方案保持不变。

29 项针对性测试覆盖通用功能键点按、TH19 指针与 iOS 长按／释放、取消和编辑态隔离、作品键盘／手柄方案隔离。UI 与工具类型检查通过。

浏览器验收使用 Edge 软件图形与 Android 触屏模拟，通过实际 CDP touchStart / touchEnd 触发页面事件，不直接调用按键入口。

[双人触屏验收](room-report.json)：两端可见 X 蓄力／C Bomb，实际 touchStart 使双方原生状态进入蓄力，touchEnd 使双方释放。RTC 运行超过 400 帧，120/240/360 帧哈希一致，errors 为空。 [按住时截图](room-charge-held.png)。

[单机触屏验收](gameplay-report.json)：可见按钮文字为“蓄力”，标记 X；Bomb 标记 C。真实触控按住使原生蓄力值升至 500，松开后 lastRelease=1 且 held=0；点击 Bomb 后库存 3→2。剧情选难度、选角、对话、移动和暂停同时通过，errors 为空。[按钮截图](charge-held.png)。
