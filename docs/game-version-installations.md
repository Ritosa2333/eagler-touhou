# 游戏版本独立安装

高级设置中的“游戏版本”提供“原版”和“美化版”。选择按游戏保存；同一游戏的单人/联机入口使用同一个选择。

原版继续使用原有 Package Store installation key，已有导入无需迁移。美化版使用 `[game, "beauty"]` 安装槽。两者各有 current/pending generation，失败更新不会移动另一版本的指针；垃圾清理保留两个槽引用的资源。内容相同的音乐、字体等可共用不可变对象。

资源版本不参与运行时游戏身份、存档根目录、回放路径和配置设置的命名。因此版本切换共享这些数据。运行中的单人游戏切换会先完成原有保存同步，再重新加载游戏；不保存正在进行的关卡状态。联机设置在下一次启动时生效。

美化资源目录位于站点根目录 `beauty/final/release-catalog.json`，描述符和内容保持相对路径。已安装的美化版直接从本地读取；缺失时安装该目录公布的包。不会用原版替代缺失的美化版。现代导入包可以声明 `gameVersion: "original" | "beauty"`；现有美化 ZIP 名称带“美化”或 beauty/beautified 时识别为美化版，其他未声明的包使用当前选择。

## 验证

- TypeScript 检查、生产启动器构建通过。
- `tests/test-game-version-store.mjs`：独立安装、重复对象共用、垃圾清理、失败回滚、并发提交通过。
- Package Store、generation、launcher policy、TH06/TH07 runtime storage contract 通过。
- 设置失效、设置控件、游戏准备、元数据服务和重试共 61 项通过；真实 BrowserLauncher 挂载、设置和元数据的追加验证共 168 项通过。
- 本地 Chromium 载入真实 TH06 原版包，自动获取服务器美化版，再往返切换共四次成功；共享存档测试文件、回放文件字节双向一致，后续美化资源请求为零。
- 回放验证覆盖文件共享，不代表测试过每个美化包的回放播放兼容性。
- `settings-midi-title.test.mjs` 的四项旧提示文字快照在原仓库已有与固定基线不一致的 touch.thpracButtonsHint 文案；不属于这次游戏版本行为回归。

浏览器复验脚本和报告位于工作区 `prepared/beautified-import-20261012/version-switch.mjs`、`report.json`。发布 ZIP 位于 `游戏包/美化导入包-20261012/启动器-双版本切换-20261013.zip`，需要与此前服务器资源包中的 beauty/ 一起部署。

