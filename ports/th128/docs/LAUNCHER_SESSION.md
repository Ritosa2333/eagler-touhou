# 启动器协议组件（尚未接完整页面）

2026-11-17新增runtime/launcher-session.mjs，只负责eagler-touhou/1会话传输：固定parent/referrer origin、safe runtimeEpoch、字符串request、数组resources、异步hook完成后的ACK、configure→launch时序、真实呈现通知入口、exit/pagehide取消及迟到结果隔离。独立打开页面返回null，不增加监听器。

795–804已由当前import-entry/play接有限已安装DAT＋OGG模式；源码接线见[当前页面](CURRENT_LAUNCHER_PAGE.md)。它不在已封存772网站或本地v7包中。资源安装、可变文件、配置应用、音频模式及实际游戏首帧要通过具体hook接入后才能验收完整上游adapter。markReady是组件调用边界，并不证明当前页面具备上游文件/DATA桥；完整页面就绪条件仍待。未实现的资源hook返回unavailable，file/touch/room/Replay命令拒绝，不假装成功。music类型检查后交给真实configure hook决定支持范围，不能据传输层接受ogg/none/midi三个枚举宣称后两种已能播放。

新WebInput setKeySource/clearKeySource/readKeySource以Symbol区分启动器输入来源，和触点/物理键盘共用持有计数；清理一个来源不释放其他来源按键，scene/blur/dispose会清理归属。此API已经在当前WebInput中，795–804已把启动器按键接入该独立来源；不能直接dispatch任意KeyboardEvent替代输入来源桥。

验证：786缺模块18失败→787真实传输层18通过；788新增来源桥6失败/20通过→789来源桥＋旧输入caller37通过；790首次两个模块联合清理与退出57通过；791完整受影响回归112通过/34pins。源码复核发现长期请求ID预算、已失败ID重试无回执；MAIN还确认健康消息可覆盖身份、缺exit hook也返回清理成功。792独立增加4断言复现18通过/4失败，793修复后22通过，794完整受影响116通过/34pins。回执缓存只保留最近256个完成请求，最多64个未完成请求，exit可越过未完成预算；较旧ID不保证永久去重，调用方应生成新ID。窗口、DOM、loader/keyboard和native出口都是明确接缝，0实际浏览器/World/Factory/编译/GPU。本轮没有改JS/WASM核心、资源v2、本地v7或旧772目录，也没有公开上线。

对照本地固定版本654717ac…的launcher/src/contracts/runtime-protocol.mts，真实request是字符串、resources是数组；旧worker候选的数字request和对象resources不直接沿用。下一步是当前entry的资源/文件桥、实际page lifecycle和presentation hook，随后实际隔离iframe验证。第三阶段尚未完成，第四阶段未实施。
源复核其余裁定：exit hook收到已中止的会话signal，用于识别已关闭owner，清理必须继续；键盘仅转发code/down/repeat/inputSource，当前动作键白名单不依赖keyCode；configure与resources必须等待各自ACK顺序调用。MIDI/配置未知选项最终由真实page hook拒绝或实施。最近回执窗口之外不承诺永久exactly-once。
