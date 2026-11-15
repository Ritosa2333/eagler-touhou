# TH05 渲染、冷启动输入与简体显示

测试站发布恢复了冷启动标题／选人的单指确认和双指返回，降低 Music Room 与游玩画面的 WebGL 调用量，并让原生 FPS 显示实际呈现速度。此次继续使用原版 15 个 SE WAV 与 PR #143 的原版／幺乐团 BGM。

## 发布身份

- 站点：[test.touhou.vip](https://test.touhou.vip/)，单人入口 TH05；原跳转继续位于多人入口。
- Runtime generation：`28dd01bc23bc295a4f3fb4cff737ff1471f090aadd270ce847f201abd177e0c3`。
- Package revision：`c2bdca7b5b842835`。
- 生产 Wasm SHA-256：`9b0b984204d43960f2f989dad7e020875f36152abfb288f439866e2f1f7ccfad`。
- [published.json](published.json) 登记发布前后目录与主站／beta 的目录和部署文件身份；两者保持原值。
- [public-files.json](public-files.json) 核对公开地址 274 个文件，共 190606755 字节；[input.json](input.json)、[runtime-files.json](runtime-files.json) 和 [build.json](build.json) 保存发布输入及构建来源。

## 渲染与真实 FPS

相邻且兼容状态的几何合并提交，纹理上传、裁剪、混合、深度、颜色掩码和帧缓冲边界保留原有语义。Music Room 原发布版本每逻辑帧约 1242 次 draw，当前为 9 次。游玩场景由约 700–800 次降到约 100–160 次。追帧积累限制为 70 ms、每次最多 4 步，最后一步提交画面。

[baseline-runtime.json](baseline-runtime.json) 与 [fixed-runtime.json](fixed-runtime.json) 使用同一台 Windows 主机的 Chromium／D3D11。当前构建两分钟连续采样保持约 56 Hz 的游玩逻辑速度，纹理数量 371、CPU 纹理副本 38283264 字节、Wasm heap 115998720 字节稳定。测试主动阻塞主线程 1.5 秒，FPS 从 56 降至约 14，随后恢复约 50，证明显示值跟随实际新画面呈现。原来的移植构造函数将 FPS 固定初始化为 56，而更新函数为空。

[fixed-chinese-runtime.json](fixed-chinese-runtime.json) 同时记录简体图集、钢琴按键和小节计数；[简体 Music Room 截图](fixed-chinese-music-room.png) 显示标题、曲目说明及原生布局。动态文字采用 512 项 LRU，避免整批清空与重新上传纹理。

## 输入与先前实现

自动射击键限制在游玩场景，标题与选人使用独立确认边沿；原生 ImGui 默认窗口不参与菜单触控命中。独立 Runtime 验证两次全新加载后的单指进入及双指返回；公开启动器复测同一流程、自机拖动和 8 个 thprac 工具开关。

简体选项将原 CP932 图集中 1829 个字形替换为简体，保留原曲目说明的编码与单元布局。原始 PMD 的 107 首曲目按键／小节时间离线制备为 DATA 元数据，播放时跟随 Ogg 的实际时间；浏览器运行 C++ 游戏和音频播放器。APS 导出进入可持久保存的 `/savesth05/APS.csv`。

原修复对话 `01a17baf-714d-78e0-917a-dfa6a2e1fe5c` 的工作源码与移植源码对照见 [source-parity.json](source-parity.json)。保留命中框显示不标记辅助、ALL／高分持久保存、E/N/H/L 通关解锁 Extra、Extra 慢速入口、用户选定的 CONG1.PI 及结算汉化。此次补齐浏览器适配中缺失的图集简化、钢琴／小节接口和 APS 保存路径。

## 验证范围

- [browser-production-validation.json](browser-production-validation.json)：生产 Wasm 的原版／练习开关、语言、暂停重开、手柄、24 帧真实触控 Replay、定位与倍速、音频配置。
- [unit.log](unit.log)：14 项协议、触控、原版 SE 与音乐时间索引测试；[presentation.log](presentation.log)：28 组呈现节奏。
- [port-check.json](port-check.json)：11/11 接口检查；[typecheck.log](typecheck.log)：启动器类型检查。
- 公开启动器结果：`public-android.json`、`public-ios.json`、`public-browser.json`、`public-vanilla-browser.json`，以及 TH01–04 的 `public-android-music-room.json`。

手机路径在 Chromium 中分别使用 Android PointerEvent 与 iPhone UA／TouchEvent 模拟，使用桌面主机 GPU。上述结果覆盖发布文件、原生菜单状态与输入路径，手机设备上的实际持续帧率由同一实际 FPS 计数器显示。

源码子树身份在提交后以 Linux Git subtree 导出，核对原始 uth05win 祖先与当前目录树，结果见 `source-history.json`。TH05 当前是独立单人目录，普通版／MP 双目录投影不适用。
