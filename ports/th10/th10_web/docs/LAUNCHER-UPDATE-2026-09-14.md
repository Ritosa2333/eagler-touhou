# 网页启动器更新修复（2026-09-14）

风神录选择简体中文、已安装旧版资源时，点击“立即更新”会提示：
`unknown requested Package component: language`。

原因是启动器把所有非日文选项都当成外部语言包。当前风神录的中日文资源均包含在基础 DATA 中，Host Manifest 用 `pack: null` 声明，因此没有独立的 `language` 组件。

更新时改为遵循 Host Manifest：基础包自带的语言不请求外部组件，真正的外部翻译仍按选中项更新。立即更新和后台更新共用这段选择逻辑。两款游戏共用启动器，修复同步至两款发行目录。

验证：

- 修复前，在浏览器中安装风神录 3.1 的资源描述，再通过简体中文界面升级，复现相同错误。
- 修复后，旧版升级成功并启动游戏；更新过程只请求 package.json，未重复下载 DATA 和音乐。
- 通过真实 Runtime 写入的录像在升级后逐字节一致。
- 保留 Service Worker 与 Cache Storage，依次切换旧风神录、永夜抄、修复后的风神录；首页、启动器、Host Manifest、Release Catalog 均与对应发行文件的 SHA-256 一致。

复测脚本位于 `portable/check-package-update.mjs`、`portable/check-launcher-cache-switch.mjs`。公网文件核对使用 `portable/check-public-launcher.mjs`。

本次仅调整启动器，C++/Wasm、游戏数据、音乐和存档数据库实现没有修改。原生运行时验证记录沿用同一 Wasm 的既有结果；本次浏览器更新记录单独保存于发行目录的 validation 中。
