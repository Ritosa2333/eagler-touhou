# TH12 普通版与多人版完整 SDL 入口

本轮在 PR #161 的 `50f8c0316` 上补齐多人应用：两侧正式构建均生成 SDL3／GLES Runtime，各自链接自己的核心。旧 JS 宿主和渲染器不进入正式模块闭包，代码包继续使用现有 DATA、共享字体、语言包、存档和启动器 RPC。

## 实现与证据

- 多人 SDL 场景接入输入确认、12 帧回滚、换面屏障、原生暂停／确认菜单、结局、成绩排名、姓名输入和 Replay 槽位；C++ Session 的规则与线协议保持原所有者。
- 高刷从当前帧外推，呈现时钟不增加逻辑 tick；静止的同一世界不计为新画面。全屏事件清除显示补帧积压。THPrac 保留只读面板的待定触控和滑动交还。
- 多人键盘在映射接入前后使用同一持有者，松开时清理遗留副本；SDL 的 Alt 组合键也会处理松开。两个针对性回归在修复前失败、修复后通过；真实运行用例在按住 Shift 后接入映射桥并松开，核对各席输入解除。
- 新入口继续载入错误报告器，并绑定原生状态、堆内存、当前键盘持有者和多人世界；TH12 的八字段回滚诊断按自己的 ABI 解读。

[构建与正式模块清单](builds.json)列出普通／多人 Web 包及 Windows 应用身份。[普通版浏览器记录](normal-browser.json)覆盖菜单、游戏、THPrac、全屏、原版录像保存、定位、暂停与恢复；暂停录像的 FPS 实测为 0。[多人浏览器记录](multiplayer-browser.json)使用真实 RTC 通道并延迟发送 25 ms，覆盖双人延迟同步、双人／三人回滚、同步暂停、晚接入观战、定位及录像重建，比较已确认世界哈希。

[定向测试](tests.log)和[键盘／报告器测试](input-reporter-tests.log)使用实际时钟头及前端处理函数。[原生素材用例](native-assets.log)另验证六种机体、Normal／Extra、世界与动画恢复、共享 UFO、六面继承，以及跨关后释放 Shift 和回滚恢复。

浏览器运行使用静音音乐设置；Windows 项目记录编译结果，未执行桌面游戏。完整通关结局与各机体音画仍属于人工操作验收范围。本文记录源码与本地验证；本轮未部署站点。

## 复现

设置 `TH12_ASSETS` 为自行提取的素材目录，`TH12_FONTS` 为共享字体目录后，运行两侧的 `scripts/check-sdl-browser.mjs`／`scripts/check-sdl-multiplayer-browser.mjs`。多人用例输出于本地 `artifacts/sdl-multiplayer-browser`。构建身份登记于 JSON，子树检查记录标准导出、祖先与源码树核对结果。

## 历史核对

`source-history.json` 记录功能提交 `de7385d78` 的标准 `git subtree split` 结果、13 个来源锚点及逐项源码树和祖先核对。检查在 SSH 129 的临时源码验证目录运行，未修改站点。`directory-projection.json` 记录两侧目录投影；同步范围登记于 `docs/adaptation/port-sync.json`。后续凭据提交只修改这些记录。

此前尚未合入的全屏性能目录投影保存在本地 `port-history/th12-fullscreen-stage-clear-20261029` 和 `port-history/th12mp-fullscreen-stage-clear-20261029`。默认 `port-history/th12`／`port-history/th12mp` 已按本轮源码重新生成，多人投影跟踪普通投影；未把这些投影作为额外父提交接回本轮分支。新增功能和凭据提交均无 `git-subtree-*` 控制标记。
