# THPrac 信息窗触控归属

TH15 独立仓库提交 `4ec052163901946e30b366ce2b5c866bfdb1968a` 将只读信息窗的点击保留给 ImGui，拖动交回游戏，并在第一个移动事件前重放原始按下坐标。共享 `PracticePointer::route_touch` 保留这套判定；本轮恢复各作平台入口对它的调用。

共享移动面板登记实际 Information 窗口的 ImGui ID，命中使用当前可见窗口边界。弹出菜单、高级设置和交互控件保留各自触控归属。普通版与多人版的 TH08、TH10、TH11、TH15 接入相同的拖动转交。TH06、TH07 同时保留 SDL 窗口坐标与游戏逻辑坐标，并从原始位置建立游戏触点。TH12 现有平台桥已实现这套转交。当前 TH09 运行包未提供 THPrac 信息窗。

## 验证范围

- 共享触点所有权与实际 ImGui 信息窗命中测试。
- TH06、TH07 使用实际 ThpracImGui 实现与队列，覆盖轻点、框内起拖、从框外跨入、64 位触点、原始按下坐标、缩放、第二根手指及取消。
- TH08、TH10、TH11、TH15 普通版和多人版共八条平台路由，从各自源码提取实际入口进行原生编译。测试只替换 SDL 时钟、UI 命中与末端输入接收器，使用共享触点归属实现。

对应自动测试为 `common/tests/thprac-pointer-test.cpp`、`common/tests/thprac-mobile-panels.cpp`、`common/tests/thprac-passive-adapters.test.mjs`，以及 TH06、TH07 的 practice-pointer 回归测试。运行包的设备验收与部署另行登记。

TH08 现有结局修复已在上游，双人三种结局、三人 Extra 与真实 Replay 保存的验证范围见[结局证据](th08-ending-proof.json)。正式运行包与公网验收见[两站发布记录](../main-thprac-th08-test-th12-20261028/README.md)。
