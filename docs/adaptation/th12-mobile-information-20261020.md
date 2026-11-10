# 星莲船手机信息面板与通关排行

手机端高级状态信息在640×480游戏坐标内距底部5像素对齐。窗口根据折叠标题和全部计数行自动计算高度，展开后完整显示Miss、Bomb及UFO计数；桌面端保留原定位。

触控沿用TH15的`PracticePointer::route_touch`：信息面板内短按控制折叠，超过8逻辑像素的滑动交还游戏，并先补发原始按下坐标。游戏区域开始的滑动跨过面板时保持原归属；弹出菜单、设置和练习菜单继续独占其交互。取消触点清除待处理手势。

Story及Extra通关后均进入Score Ranking。使用无限速触控等不参与排名的对局展示已有排行榜，不插入本局成绩、不保存排行姓名；确认后继续Replay保存或Extra结果菜单。普通入榜成绩继续保留姓名输入，录像播放与非通关流程保持原行为。

验证入口：

- `node tools/ports/th12-practice-browser.mjs`：生产Wasm、共享ImGui和字体，检查面板边界、折叠点击、滑动原点交接及跨面板归属，输出`artifacts/th12-purple-ui/`截图和报告。
- `node --test ports/th12/tests/reward-menu-audio.test.mjs`：Story、Extra入榜与不入榜通关、音乐顺序及Replay姓名保存音效。

浏览器面板检查使用合成游戏背景；手机实机整局通关由实际游玩验收。
