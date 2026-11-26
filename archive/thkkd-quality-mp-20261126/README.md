# 海惠堂网页单机质量验收

本阶段从已交付的单机提交 `36a198b6ff16307ee4b701d84a917e7ae1e9c0ae` 开始。旧包使用其自带 Node 本机入口，修复版使用相同生产打包器、启动器、托管 Runtime 和 DATA 路径。浏览器为 Windows x64 的 Edge 158.0.4370.33，使用独立临时浏览器数据。

`delivered-default.json` 记录触控关闭后仍有 touch 来源持续持有 Z、六次按键未推进菜单的原交付问题，并记录菜单实际约 60 Hz、原生数字 120 FPS。`fixed-default.json` 使用正常启动按钮，未主动聚焦画布；五次 Z 确认进入游戏，射击生成 24 个子弹，暂停确认恢复；标题、难度、游戏和暂停均约 60 次实际 WebGL 提交与逻辑步/秒。

`fixed-high-refresh.json` 从公共设置打开高刷新率，刷新页面验证保存，实际启动后检查四种场景。该次无头 Edge 解除浏览器自身帧限制，测得实际额外提交约 1317–2595 次/秒，各场景逻辑约 60 步/秒。报告保存浏览器加载的 Runtime URL、manifest、configure 参数、响应缓存来源及 WASM 字节 SHA-256。原生 WASM 保持旧交付字节，修复位于网页输入与呈现后台。

`extensions.txt` 的 12 项局部检查和 `touch-continuity.txt` 的本作同触点恢复检查通过。运行入口为 `node ports/thkkd/tests/quality-browser.mjs --url=http://127.0.0.1:8177/`；高刷新率追加 `--label=candidate-high-refresh --high=1 --unlimited=1`。后续单机对话、结算及系统审核结果见下文。

第二轮的 `system-before.json` 记录重复开录像后 5 个已删除纹理留在集合中、原生手柄设置卡在等待输入；`system-fixed.json` 对应回收与手柄接线修复，残留为零且原生设置接收按键。后者使用浏览器默认音频策略，实测 BGM PCM 非零，并验证视频纹理及暂停恢复。WASM 从本轮 WebHost 手柄入口重新编译，SHA-256 为 `aba4fc31734881f77e191d52977f6a71021a75b4da8f6614139f62e929f24692`。

`controls-fixed.json` 检查正式启动器的布局保存、WASD、自定义改键、实际原生手柄输入与断开释放、触控移动取消、横竖屏、双指缩放、失焦释放、独立音量和全屏。`native-flow.json` 检查原生暂停重开、录像询问及署名保存、原格式导出和导入播放、定位输入隔离、刷新后原字节保留。当前 `extensions.txt` 更新为 14 项通过。核心与生成显示源码继续保持 Windows 基线原字节。

本轮生产复验使用 WASM `c842659b384e9492105ac7071e082c509d6f9c814b5d479e8c54e8445f4ca4dc`，最终普通 Runtime buildId 为 `3a222ab5062a9743e122a4d21f37e0fdc3ad640ca7e9881b280d6d1ec6414af8`。原生与生成显示文件分别 570、13 项保持 Windows `64d48dc` 原字节，见 `core-bytes.json`。旧的 60 个无绘图用例、835289 个逻辑步数值结果复用至该固定核心；本轮变更集中于网页输入、持久化、媒体生命周期和呈现接线。

`native-flow-immediate.json` 从原生菜单保存原版格式录像，关闭保存界面后立即在独立 IndexedDB 读到完整字节，并在刷新后恢复；还验证异常 Replay 元数据报错时保留当前回放。`dialogue-touch.json` 通过原生 Practice 一面 Easy 与独立测试进度文件进入 Boss 对话，实际 Z、触控短按和长按推进，录制缓冲包含原生对话推进位。`replay-corpus.json` 的 17 份已有授权录像通过格式范围与校验检查，原始录像继续留在私有归档。

`system-complete.json` 在正式 Runtime 中完成九机体射击与 Bomb，以及六面、EX、Omniverse 入口代表资源的呈现，三次回放重开后无已删除纹理残留。固定暂停状态额外呈现 100 次，完整 WASM 内存哈希一致，声音与存档写入次数为零；这些入口检查与既有完整无绘图关卡回归分别记录。`playback-cadence-60.json` 与 `playback-cadence-high.json` 分别测量录像暂停和快进的实际呈现与逻辑计数，限帧约 60 次/秒，额外呈现保持暂停逻辑不变。

`audio-lifecycle.json` 通过公共界面滑块设置独立增益、静音及恢复，取得菜单选择音效的非零 PCM，验证公共退出和原生 Quit 后再次启动。原作未加载的射击音效槽为句柄 0，实际同版 DxLib 检查 `CheckSoundMem(0)=-1`；网页保留该槽的无声调用，同时观察到原生射击对象。`files-ui.json` 实际使用存档文件选择器与覆盖提示，重启后原生音量读取为 77，下载 ZIP 保留原字节；录像工作台的导入、下载、改名和确认删除经过刷新复验。

`upgrade-offline.json` 从原交付程序与已有测试数据开始，使用网站程序更新提示重启，核对新 WASM 哈希，再关闭 HTTP 服务并切为离线；新程序、原存档和原格式录像持续可用。静态本地服务中，地区探测、账号就绪与可选资源路由接口返回 404，资源路由随后使用本地 DATA；报告保留这些 HTTP 记录，正式 Runtime 未出现页面异常或非预期音频错误。`contracts-final.json` 保存本作公共接口检查 10/10 的源码身份。

`performance-dense.json` 使用同一授权 EX 录像的游标 50000。起始场景有 360 个敌弹、405 条绘图命令，测量期间保持原生游戏场景；Edge 158、NVIDIA RTX 4060 Ti、128 MiB WASM 内存下，连续三秒实测逻辑与呈现均约 59.99 Hz，单步平均约 4.00 毫秒、95 分位约 4.70 毫秒。该结果采用正式启动器及真实输入流，长定位使用独立的协议等待时限；源码检查与局部检查共 18 项通过，见 `extensions-final.txt`。单机功能修复提交为 `5ce04ee11`，测试驱动长等待提交为 `95a927799`。
