# TH05 原生测试站发布

测试站 [test.touhou.vip](https://test.touhou.vip/) 的单人栏接入原生 C++ Wasm TH05，原 TH05 独立站入口移入多人栏，TH01–04 继续位于旧作栏。原版与 thprac 使用同一程序，通过启动器开关选择；音乐提供 PR #143 原版与“幺乐团专辑版”，音效使用 PR 中独立的 FM PCM 数据。

## 发布身份

| 项目 | 内容 |
| --- | --- |
| 本地分支 | `publish/th05-test-20261101` |
| 启动器来源 | `d2493feb4f2a64c528f2241b6d0fffd5a47dd32e`，沿用测试站现有 TH13、TH17 和启动器修复 |
| 源码父链 | `56b4e1edb470daaadc9dd84a4f2886f0ce0039dd`，继承 uth05win 的 thprac 分支 |
| 发布目录 | `/var/www/eagler-touhou-test/releases/th05-native-20261101-922cb60d4048` |
| Runtime generation | `811ca3c998bdbc288128c461f2a399ac9f9614aa233b6ff9df02b6dcadcec509` |
| TH05 内容版本 | `a39e0610d1bb1480` |
| App Shell | `4ccbe8c0147b731dcd10` |

[发布回执](published.json)固定发布身份及主站、beta 站的原版本；[组装记录](assembled.json)核对已有资源与运行时，7330 个非覆盖文件保持一致。新资源通过 `/games/th05/` 本地路径提供，旧 `/th05` 路径继续跳转到多人页面。

## 线上验收

- [公网文件检查](public-files.json)：188 个文件、77,358,393 字节的 SHA-256 与发布清单一致，包含 Wasm、46 首音乐、游戏数据、中文包及启动器资源；旧 TH05 多人跳转正常。
- [中文练习模式](public-browser.json)：真实启动器下载并启动 Wasm，中文代码页 936、Unifont、thprac 启用、幺乐团曲目实际播放，PR 音效独立输出，无页面或 Runtime 错误。
- [日文原版模式](public-vanilla-browser.json)：同一 Runtime 的 thprac 关闭、日文代码页 932、MSGothic、原版 Ogg 完整解码并播放，PR 音效正常。
- [作品契约检查](port-check.json)：TH05 normal 的 11 项源码、协议、输入、资源与显示检查通过。

原生逻辑、Replay、触控及诊断矩阵的验收范围见[原生移植记录](../th05-native-eagler-20261101/README.md)。本次发布额外选择中文 Unifont，并在真实测试站核对两种启动配置。

[发布前设置](public-settings.png) · [原生运行画面](public-game.png) · [原版设置](public-vanilla-settings.png) · [原版运行画面](public-vanilla-game.png)

## 构建与恢复

`prepare.mjs` 固定新前端、Runtime 和内容的发布输入，`assemble.mjs` 从原发布硬链接组装候选目录并重新生成 App Shell 与发布清单，`activate.py` 在测试站同步锁内校验并切换。激活前保存原 Nginx 配置，探测失败自动恢复原目录和配置。Nginx 重载探测包含短暂重试，等待新工作进程接管资源请求。

此前版本为 `/var/www/eagler-touhou-test/releases/latest-d2493feb4-20261031`，原 Nginx 配置保存在服务器 `/opt/th05-native-test-20261101/nginx.before`。构建、文件身份及执行脚本均保留于本目录；本次发布针对 test 站。
