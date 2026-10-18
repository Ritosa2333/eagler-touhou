# 共用 thprac 工具

`eagler::thprac` 提供输入、按键统计、速度控制和移动端面板组件。游戏保留自己的练习项目、ECL 修改、资源、Replay 和状态生命周期。

## 1. 接入

CMake 使用 `eagler::thprac`。脚本构建把 `common/include` 加入头文件搜索路径，并把所用头文件和 `.inc` 纳入构建缓存指纹。

| 文件 | 接入位置 |
| --- | --- |
| [`PracticeInput.hpp`](../include/eagler/thprac/PracticeInput.hpp) | 逻辑输入、自动射击与禁用键顺序 |
| [`PracticeKeyMonitor.hpp`](../include/eagler/thprac/PracticeKeyMonitor.hpp) | 会话拥有的按键统计、60 帧 APS 与 CSV |
| [`PracticeSpeed.hpp`](../include/eagler/thprac/PracticeSpeed.hpp) | 逻辑速度；显示插值仍由绘制层负责 |
| [`PracticeKeyHud.inc`](../include/eagler/thprac/PracticeKeyHud.inc) | 绑定会话的 `key_monitor` |
| [`PracticeReaction.inc`](../include/eagler/thprac/PracticeReaction.inc) | 提供平台时钟、频率、计数器与随机数 |
| [`PracticeSpeed.inc`](../include/eagler/thprac/PracticeSpeed.inc) | 保留各作的重置标志和 UI 生命周期 |

`.inc` 在 ImGui 和标准库头文件之后，包含进作品自己的 UI 命名空间。界面文字通过作品的 `label(practice_...)` 与启动器语言同步。

## 2. 移动端面板

启动器通过 `touch-controls` 传入 `thpracMobilePanelsEnabled`、`thpracMobilePanelsVisible`。用 [`PracticeMobilePanels.hpp`](../include/eagler/thprac/PracticeMobilePanels.hpp) 的 `show`、`begin` 包装原窗口；折叠时也配对 `ImGui::End`。

一个移动端入口控制面板组，窗口初次打开与重新打开时折叠。统计、录像和玩法更新在可见性判断之外执行。`captures_advanced` 只在高级面板展开时捕获游戏输入。

[`PracticePointer.hpp`](../include/eagler/thprac/PracticePointer.hpp) 在按下时决定手势归属；游戏拖动跨过面板标题仍归游戏。失焦、触摸取消和会话重置时清理手势。折叠标题由直接触摸操作，键盘与手柄继续遵守原窗口导航。

## 3. 各作保留的差异

TH08/10/11/15 使用抽取的紫版工具。TH06/07 保留自己的实现和构建开关。F12 项目、作弊、warp、catch-up 上限与回放标记在作品层维护；练习录像保留辅助元数据，播放时显示其使用条件。

## 4. 验证与更新

先运行 [common 测试](../README.md)，再运行受影响作品的输入、速度、面板及练习构建检查，最后填写[语言与练习适配项](../../docs/adaptation/localization.md)。移动端记录应包含面板展开、折叠和多指操作。

在 `common/` 中检查抽取源：

```sh
node tools/generate-purple-thprac.mjs /path/to/thprac-purple --check
```

`--write` 更新公共抽取；作品生成器更新作品自己的 hook 与资源。保留 [Ack 的 MIT 许可](../include/eagler/thprac/THPRAC-LICENSE.txt)。
