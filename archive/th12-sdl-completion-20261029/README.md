# TH12 SDL3 应用构建身份

本次构建将触控与练习扩展录像、整曲 OGG 解码、输入按下信号和录像帧率显示接入共用 C++ 应用。Web、Windows 应用及 MP 平台库均编译成功；产物大小、SHA-256、源码指纹和依赖版本见 [构建清单](build.json)。

## 构建范围

| 输出 | 构建目标 | 内容 |
| --- | --- | --- |
| Web | `touhou12.mjs` / `touhou12.wasm` | SDL3 应用、GLES 3、THPrac、音频及录像 |
| Windows | `touhou12.exe` | 共用 C++ 应用与桌面 SDL3 入口 |
| MP | `th12_sdl_host` | 链接多人核心的共用平台静态库 |

主站代码包由 `ports/th12/scripts/build-eagler.mjs` 生成，入口为 `th12.html`；人工预览包由 `scripts/package-sdl.mjs` 生成。主站预览组装使用 `scripts/package-launcher.mjs --build-launcher=0`，保留现有启动器，更新本作 Runtime 与资源清单。

MP 正式 Web 构建继续使用既有房间、回滚和多人录像宿主；本次 MP 输出的身份是平台库。

## 人工验收入口

本次执行编译、打包和文件核对，未启动游戏、运行自动用例或进行游玩验收。代码包沿用 `sdl-manual-acceptance-pending`。当前适配状态由 [普通版适配表](../../ports/th12/ADAPTATION.md) 和 [MP 适配说明](../../ports/th12-mp/ADAPTATION.md) 维护。

在主站选择星莲船后，人工检查以下路径：

- 触控游玩保存录像，再从主站打开、定位和重播；无限速记录保持辅助成绩标记。
- THPrac 分段与多阶段符卡开局，保存后重播，核对练习开局参数。
- 流式和整曲解码分别切曲、暂停、返回前台；录像定位后无跳过区间的瞬时音效。
- 常规键盘录像、跨关、结算、菜单及退出重开后的存档。
- 正常倍速录像的高刷新率显示、原录像帧率数字和当前帧率数字。

原始游戏数据、语言素材、字体、音乐、录像及编译产物继续由本地资源安装和构建目录管理，本记录保存构建身份。
