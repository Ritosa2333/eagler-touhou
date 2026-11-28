# 启动器性能开销排查 | 2026-11-28

本记录延续[前一轮性能整改](../performance-20261128/report.md)，独立检查启动、绘制、排序、页面生命周期、资源、后台任务、文件处理、网络和网页容器。Windows 网页空白通过纹理适配器补丁修复，账号页自动退出通过历史通知合并修复，普通 Release 已重新构建。SDK、依赖与界面布局沿用；适配器 A/B 使用相同旧 AOT，账号修复产生新的 AOT。起始排查时工作副本相对当时记录的 origin/main 落后 8 个提交，后续样本各自登记来源提交。

## 测量结果

Windows Release 使用生产 `main()`、实际 11 部作品目录和 1280 × 720 窗口，通过临时集成入口完成 60 次游戏详情进出、12 次正倒序切换、3 次账号网页创建与退出，以及最小化和恢复。[原始记录](exhaustive-release.json)与[统计](audit-summary.json)配套保存。

| 检查 | 结果 | 含义 |
| --- | --- | --- |
| 返回后的页面存活 | 每个稳定采样点的详情页数量为 0，网页关闭后 WebView 控件为 0 | 本样本未发现详情页或网页控件不断堆积 |
| 返回后的树与图片 | 10,403 个元素；图片缓存固定 15,903,044 字节；待解码图片为 0 | 页面结构和图片缓存没有随 60 次往返增长 |
| 动画任务 | 所有稳定采样点的临时帧回调数为 0 | 稳态未发现持续运行的动画 ticker；这不代表整个进程低 CPU |
| 排序的构建时间 | 479 帧，中位 0.375 ms，95 分位 1.748 ms，最大 21.128 ms | 本轮主要长尾不在一般的 Dart 构建工作 |
| 排序的渲染时间 | 中位 3.608 ms，95 分位 14.569 ms，最大 511.471 ms | 存在慢帧，需要继续查画面提交链路 |
| 网页子进程 | 采样最多同时观察到 2 个；工作集相加最高 150.9 MiB，私有字节相加最高 49.5 MiB | 网页容器必须单独计费；工作集相加会重复计算共享页 |

第 10 次往返后的进程工作集为 567.8 MiB，第 60 次为 574.4 MiB，最后恢复窗口时约 674.8 MiB。它包含引擎、字体、GPU 映射、无障碍桥及测试入口。图片缓存和元素数稳定，工作集仍有变化，不能据此宣称长期无泄漏，也不能把它当 Dart 堆。同一普通 Release 独立启动的书架窗口另采样六点，工作集固定约 225.9 MiB，私有字节固定约 324.3 MiB，线程 97 个，句柄 952–953 个。该窗口没有完整轨迹或集成测试入口，也没有账号网页驻留；此短样本不代表长期峰值。[普通入口内存](exit-live-ordinary-memory.json)。该早期样本中的账号页均自动回到原生 `/`，记录的是容器创建与卸载；后续历史通知修复和独立实窗回归见下文。已登录内容的负载测量继续单列。

排序开始前与结束后的窗口焦点记录均为 false。后续普通预览检查中还观察到其他 th14 验证窗口。本机是共享测试环境，上述 511 ms 不能直接归为用户前台操作的固定延迟；后续需要在持续记录前台窗口的条件下复测。[进程树记录](process-tree-release.json)与界面状态采样分开保留。

## 渲染归因与观测开销

独立 Profile 全 Skia 轨迹在约 5.05 秒中包含 1,001,052 条事件。两次最长渲染分别为 506.7 / 480.3 ms，其中 `FlutterCompositorPresentLayers` 占 504.1 / 479.7 ms。事件定位到 Windows 宿主的画面提交回调，尚未区分驱动等待、窗口遮挡、其他 GPU 工作或平台线程竞争。不能从事件名推定最终根因。

同一轨迹内一般渲染中位约 2.16 ms，语义树更新中位约 0.76 ms，单次最高约 4.39 ms。[分组统计](sort-trace-summary.json)包含嵌套的包容时间，不能把各行相加作为总 CPU 时间。原始轨迹压缩保存在 [sort-full-trace.json.gz](sort-full-trace.json.gz)。

完整跟踪运行中，工作集随往返明显增长，取回轨迹后 Dart 堆也因保留大量事件而增大。[堆采样](heap-samples.json)属于这次 Profile 跟踪，不属于普通交付程序。关闭完整跟踪后的 Release 往返没有复现此前近似线性的增长。归因轨迹与验收耗时必须分别测量。

前一轮普通入口曾记录无新画面帧时约 0.29–0.40 个 CPU 核心，并在 10 秒内收到 10,518 次 WM_GETOBJECT。Microsoft 将该消息定义为无障碍对象查询；Flutter 的 Windows 桥在窗口消息处理中响应它。发起查询的具体客户端及其对提交慢帧的贡献仍待确认。没有关闭无障碍能力，也没有修改操作系统或依赖版本。

普通预览复开时曾出现白色捕获画面，同时语义树可读。再次复开后看到实际书架。这项呈现异常尚未与提交慢帧或退出卡住建立因果关系。

## 退出卡住的原生证据

后续直接读取 Windows 进程句柄，确认关闭的旧预览仍未完成终止：`GetExitCodeProcess` 已报告 0，但进程对象的 `WaitForSingleObject(0)` 返回 258，结束时间仍为 0，CPU 时间持续增长。该进程只剩一个线程。旧的 PowerShell 查询依据退出码提前认为进程已退出，不能据此判定终止完成。

[线程采样](exit-thread-proof.json)的五个样本中，三个指令位置在 Intel `igd10um64xe.DLL`，两个在 ntdll。[完整原生栈](exit-stack-full.json)显示退出路径经过 `ExitProcess`、`LdrShutdownProcess`、网页插件 DLL 的 CRT 退出处理、Direct3D 和 Intel 驱动。该样本锁定了卡住的位置；未加载私有符号，不能把偏移很大的最近导出符号当作精确函数名。

当前锁定的 `flutter_inappwebview_windows 0.7.0-beta.3` 在管理器构造时建立共享图形上下文，管理器析构清空网页对象，但共享图形上下文保留为静态对象。调用栈与插件图形资源在 DLL 清理阶段释放的路径一致。该依赖原来通过默认适配器创建 D3D11 设备，与 Flutter 的高性能 GPU 选择分别配置；Microsoft 的 API 文档确认空适配器参数选择默认适配器。下节补丁已修正纹理设备选择，资源所有权与 DLL 卸载时序继续沿用原依赖。后续普通入口和公开主页窗口关闭后均确认进程消失，这些成功样本尚不足以证明间歇性退出问题已经消失。

另做三组原生对照：D3D 设备；D3D 加 WinRT 包装；D3D、WinRT 包装、DispatcherQueue 加 Composition。每组比较显式释放与 DLL 静态释放各三次，共 18 次均完成设备创建并正常终止。简化输入没有复现旧预览的问题，不能据此认定提前释放已经解决它。[D3D 对照](exit-device-d3d-results.json) | [WinRT 对照](exit-device-winrt-results.json) | [Composition 对照](exit-device-results.json)。最后一组使用的源码为 [device_lifetime.cpp](exit-device_lifetime.cpp) 和 [device_host.cpp](exit-device_host.cpp)，编译和运行脚本配套归档。

相同前台窗口的两次空闲采样各 11 点，全部确认前台 PID 相同。重置检查脚本会话前后，可见预览消耗约 16.1% / 17.0% 的一个 CPU 核心，关闭的旧预览约为 98.4% / 97.1%。重置脚本会话没有明显降低本轮空闲占用；它也不代表系统全部无障碍客户端已断开。[原始采样](exit-idle-connected.json) | [重置后](exit-idle-reset.json)

可见预览的后续 5 秒线程采样中，窗口线程 TID 34564 使用 1203.12 ms CPU，第二个线程为 218.75 ms。窗口线程的六次短栈采样中，一次经过 OLEACC 与 UIAutomationCore，一次经过 Flutter 与系统区域设置调用，其余四次落在消息等待或窗口子类处理链路。它把空闲开销进一步定位到窗口线程活动，并为无障碍查询假设提供证据；六个样本不足以量化各调用占比，也尚未识别查询客户端。[线程 CPU](exit-live-hot-threads.json) | [短栈](exit-live-stack-full.json)。Flutter 私有函数没有符号，未按最近导出名解释其职责。

匹配旧预览的完整路径和创建时间后，已终止该卡住的测试进程，随后确认 PID 不再存在。同一普通 Release 的标题栏关闭复测完成了窗口和进程退出，普通书架预览已重新打开。本次确认的是间歇性原生退出问题，不代表每次关闭都会卡住，也没有把清理测试进程当成产品修复。

## 双显卡资源核对

相同 SDK、驱动和 D3D 创建参数的独立原生程序，分别使用默认设置与导出 `NvOptimusEnablement=1`，交替运行各三次。默认程序三次均选中 Intel UHD Graphics；导出提示的程序三次均选中 NVIDIA RTX 4060 Laptop GPU。六次均以 0 退出。以设备返回的 LUID 对照系统进程 GPU 计数器，普通书架预览 PID 39464 确实在两张卡上分配了资源。[完整采样](gpu-adapter-results.json) | [原生程序](gpu-device-adapter.cpp) | [运行脚本](gpu-run-adapter.ps1)

| 适配器 | 专用内存 | 共享内存 | 提交量 |
| --- | --- | --- | --- |
| Intel | 0 | 15,028,224 字节 | 23,261,184 字节 |
| NVIDIA | 75,730,944 字节 | 364,544 字节 | 123,740,160 字节 |

这些是进程 GPU 计数器，不与工作集相加。本次 GPU 引擎利用率瞬时样本均为 0，不能解释动画期间的 GPU 负载。插件在注册时便创建默认适配器设备，Flutter 宿主另外选择高性能 GPU；源码与双适配器分配相符，但仍未逐设备确认资源属于哪个组件，也未证明存在导致慢帧的跨 GPU 复制。

小程序默认版本的三次整进程耗时为 756.32、76.68、102.74 ms，独显提示版本为 1048.40、315.79、230.05 ms。耗时包括进程启动、设备创建与关闭，样本量小且首样本包含不同的暖机状态，不能替代真实应用启动、功耗或动画 A/B。此对照说明 GPU 策略调整需要同时验收启动成本；尚未将全局独显提示写入产品。该导出机制参考 NVIDIA 2012 年的技术说明，是否生效以本机当前驱动实测为准，不作为普适的新指南。

## 网页纹理适配器修复

插件原生层通过默认适配器创建纹理，Flutter 通过高性能适配器呈现。使用相同普通 `main.dart` AOT、相同临时地址参数入口与相同公开个人主页，单独替换网页插件 DLL：原插件一直空白，适配器补丁显示头像、封面、WHR 图表与统计。手工两对象构建完成对照后，再用正常 `flutter build windows --release --no-pub --target lib/main.dart` 完成全链路构建，使用该 DLL 再次验证桌面显示、右边缘拖窄到 390 px、网页重排及返回书架。未填写凭据。诊断用地址入口不进入交付包。

补丁使用 Flutter 官方 `PluginRegistrarWindows::GetGraphicsAdapter`，COM 引用由 `winrt::com_ptr` 释放。四处修改为设备参数传递和 D3D11 适配器选择，原软件回退路径沿用。CMake 核对锁定版本、四个源文件与补丁输出哈希，所有改动只生成到构建目录。[构建入口](../../windows/CMakeLists.txt) | [补丁应用](../../windows/webview_renderer_adapter.cmake) | [四文件差异](../../windows/webview-renderer-adapter.patch)

正式构建生成的 223 份插件输入逐项核对通过，重复配置通过。应用 AOT SHA-256 与原候选同为 `6bdb5ec6b5498fd8d4368700747dc360f02c2cd578a7b1f1bb22b4cb45caa269`。[构建日志](webview-production-build.log) | [重复配置](webview-production-repeat-configure.log) | [输入校验](webview-production-source-check.json) | [对照记录](webview-adapter-receipt.json)

完整构建的公开主页样本同时存在 6 个 WebView 子进程，宿主私有字节约 240.6 MiB。资源分配转向 NVIDIA，但进程仍有 Intel 分配，不能推定所有跨卡活动消失。普通书架前台空闲 11 点采样，目标始终在前台，CPU 约占一个核心的 19.8%，私有字节约 319.8 MiB，工作集约 544.6 MiB。它没有运行完整跟踪；与早先独立样本的差异没有满足同环境对照条件，不作内存改善结论。网页显示修复也不代表空闲 CPU 或提交慢帧已经修复。[公开主页采样](webview-profile-production-observed.json) | [书架采样](webview-production-library.json)

## 各职责的开销核对

| 范围 | 当前链路与处理 | 证据等级 |
| --- | --- | --- |
| 启动 | 先做 Windows 着色器预热，再异步读取目录与偏好；账号恢复异步发起 | 三次原生首帧分阶段计时；完整进程到可交互时间待测 |
| hover | 公共作品行已有重绘边界，收藏符号复用原动效 | 前轮真实 Release 测量及当前源码核对 |
| 详情、设置、切栏 | 详情背景使用现有模糊边界；保留分支维持滚动状态；隐藏分支停 ticker、输入、焦点与语义 | 前轮 32 类动作、此次 60 次往返 |
| 排序、滚动 | 懒构建 sliver 与现有位移动画；原排序流程复用 | 前轮滚动测量、此次排序帧与完整轨迹 |
| 封面、壁纸 | 共用封面缓存；壁纸解码限于 1920 × 1440 | 资源清单、运行时缓存及源码核对 |
| 字体 | 采用系统字体与各语言 fallback，不新增远程字体加载 | 源码核对 |
| 大厅、排行榜、消息 | 隐藏大厅停止时钟；排行榜按路由、ticker 与生命周期启停；消息摘要每 30 秒轮询，按账号与前台状态控制 | 前轮回归及当前源码核对；繁忙真实房间待测 |
| 文件导入、导出、云存档 | ZIP 解压、压缩、统计与集中比较在 worker；文件复制异步，进度按整数百分比更新；原作资源在工作 isolate 中流式校验 | 真实 th07 导入回归、23.8／90.8 MB 资源组件测量及前轮 8 MiB 云存档哈希 |
| API、WebSocket | HTTP 响应限量、分页与超时；过期请求不应用到新状态；房间聊天上限 50 条；JSON 解码仍同步 | 源码核对；大响应解码开销待真实输入测量 |
| WebView | 共享 Windows WebView2 环境与缓存，纹理与 Flutter 共用适配器，控件按页面卸载 | 真实公开主页驻留、竖屏重排与返回通过；登录历史通知修复通过控件与隔离 Windows Debug 实窗检查 |
| 手柄、MIDI、音频、网络诊断 | 手柄监听随设置页释放；MIDI 仅启用后监听；快捷语音按需建播放器；WebRTC 工厂按方法调用初始化 | 源码核对；硬件与持续媒体负载待测 |
| 原生游戏 | 本启动器仍为统一占位入口 | 游戏帧率、音频与下载运行时不计入此记录 |

## 启动首帧分阶段计时

临时原生入口复用普通 `main.cpp`、正式构建 DLL 与相同 Dart AOT，在现有 `SetNextFrameCallback` 中写一次本地计时，然后执行原有显示窗口动作。计时起点是 Windows 进程创建时间，结束点在写文件之前。三个独立进程均进入实际书架并完成关闭。

| 墙钟区段 | 第一次 | 第二次 | 第三次 |
| --- | --- | --- | --- |
| 进程创建到首帧回调 | 3633.22 ms | 3485.84 ms | 3427.86 ms |
| 进程创建到窗口 OnCreate | 173.135 ms | 122.380 ms | 135.636 ms |
| OnCreate 到 Flutter 控制器创建完成 | 2609.60 ms | 2656.42 ms | 2529.21 ms |
| 注册全部插件 | 129.491 ms | 71.758 ms | 80.889 ms |
| 插件注册完成到首帧回调 | 720.993 ms | 635.281 ms | 682.133 ms |

本样本的启动长段在 Flutter 控制器创建期间，插件注册总计为 72–129 ms。引擎、驱动与 Dart 初始化可能并行，区段不能直接等同于这些组件各自的 CPU 消耗。系统与文件缓存没有清空，采样期间另观察到 th17 验证窗口，因此不称为隔离冷启动基准。首帧回调也不等于桌面已呈现或全部输入已经可用。[原始记录](startup-phases.jsonl) | [计时源码](startup-flutter-window.cpp) | [构建与来源](startup-proof.json)

## 账号网页历史通知修复

普通 Release 的实际导航记录显示：游客打开 `/login.html` 后，History API 短暂通知根地址 `/`，约 6.842 ms 后恢复 `/login.html`。原容器在根地址事件中立即执行 `_close`，身份仍为游客；退出来自历史回调，并非登录成功。[安全事件记录](account-history-events.jsonl) | [诊断来源](account-history-source.json)

现有账号模块把可能返回原生的历史与加载通知合并 100 ms，再读取当前引擎地址。更新事件、重试、新文档与关闭取消旧结果，直接主框架导航仍即时处理。没有新增轮询或登录协议。单次地址读取不足以避开临时根地址，回归样例同时覆盖加载、响应式更新和晚到读取。

83 项账号控件检查通过，静态分析无问题。布局、语言与导航等 133 项通过，210 张新采图与原图逐字节一致。Windows Debug 的隔离真实引擎测试两次进入账号页，每次按 1280、390、320、1280 px 改变窗口，再分别使用返回和关闭，均恢复 th08 和相同标题位置。测试检查真实纹理，不以控件被创建代替引擎可用。[实窗结果](account-history-live-result.json) | [测试入口](account-history-live-test.dart) | [截图比较](account-history-capture-comparison.json)

截图依次观察到[桌面加载](account-history-loading-1280.png)、[390 px 骨架](account-history-skeleton-390.png)和[320 px 登录表单](account-history-login-320.png)。它们记录网页加载过程，不证明三个宽度均已完成出图。普通最终 Release 已构建，其手工跨宽度复验在检测到现有窗口用户输入后停止。未输入凭据，登录退出、上传与裁切继续需要对应输入。

## 最终 Release 账号页与关闭异常

相同普通 `main.dart` AOT 通过既有正式站账号链接入口，分别在三个独立游客进程加载登录页。只增加原生入口的初始尺寸、不激活显示、隔离应用元数据与关闭计时，没有修改 Dart、插件或引擎。三张实际窗口图均显示完整登录表单，加载后仍停留账号容器。系统为 144 DPI；原生日志核对的内容宽度分别为 1265.33、375.33 和 320 逻辑像素，不把外窗宽度当作内容宽度。[桌面实拍](account-release-login-1280.jpg) | [竖屏实拍](account-release-login-390.jpg) | [320 px 实拍](account-release-login-320.jpg) | [来源、尺寸与结果](account-release-summary.json)

三轮各观察 24 次，五秒一次的前台 PID 均为其他进程。120 秒后向诊断窗口发送普通关闭请求，三轮都进入引擎销毁，但没有到达销毁完成标记。两轮在 140 秒期限后仍未终止，由测试结束自己的进程；另一轮在约 139.46 秒进入内核终止状态。系统 Application Error 同时确认三者均在 `flutter_windows.dll+0x15cdf` 出现 `c0000005` 与 `c000041d`，因此第三轮也属于崩溃，不能以句柄已终止判为正常退出。[筛选后的系统错误](account-release-errors.json) | [三轮原始结果](account-release-result-320.json)

320 px 窗口在关闭后保留了 UI 线程栈。严格匹配的引擎 PDB 将故障地址定位到 `FlutterDesktopMessengerSetCallback`。插件使用原 94 份对象文件独立重链接，仅增加 `/MAP`，其 `.text` 的 987,648 字节与实际 DLL 完全相同；映射中的邻近函数依次包括 `FindInteractionController::dispose`、`ChannelDelegate::UnregisterMethodCallHandler`、`InAppWebView` 和插件析构。它们对应引擎 `Stop()` 的插件清理回调，区别于此前 `ExitProcess` 后的 Intel 驱动栈。[现场栈](account-release-close-stack-320.json) | [引擎精确符号](account-release-resolved-engine.tsv) | [插件映射与字节核对](account-release-resolved-plugin.json) | [完整链接映射](account-release-plugin-map.gz)

固定 SDK 的引擎析构先将 messenger 的引擎指针清空，再执行 `Stop()`；插件析构仍调用注销消息通道。本轮把下一步缩小到引擎停止期间的通道清理与网页释放顺序，候选修复需要用相同入口对照。上游 [#2884](https://github.com/pichillilorenzo/flutter_inappwebview/issues/2884)记录控制器晚于宿主窗口关闭，[#2896](https://github.com/pichillilorenzo/flutter_inappwebview/issues/2896)记录异步纹理注销的资源寿命问题，二者作为独立参考，不直接代替本机归因。

本轮图片由 Computer Use 直接返回 JPEG，按原字节归档；未输入凭据或操作表单。当前程序包与 113 个交付源码哈希不变。同一窗口跨宽度重排、返回发起游戏和真实账号写入继续分别验收；该旧包的网页驻留关闭未通过，后续修复与复验见下一节。

## 网页驻留关闭修复

插件注销消息通道前，现在先检查 messenger 是否仍有可用引擎。补丁进入现有 Windows 构建适配模块，锁定 Flutter 3.50.1 包装层的输入与输出哈希，并生成独立构建输入。SDK、Pub 缓存、Dart 和界面均未改动。该检查与 SDK 处理晚到回复时已有的可用性判断一致。[官方接口](https://api.flutter.dev/windows-embedder/flutter__messenger_8h.html) | [构建模块](../../windows/webview_renderer_adapter.cmake)

同一普通 AOT 与正式站游客登录地址，完成六轮独立插件候选和三轮完整 Release DLL 集合的真实窗口关闭。九轮内核退出码均为 0，全部到达引擎销毁完成标记，没有强制终止；筛选系统 Application Error 1000 未发现这些诊断宿主的错误。诊断宿主增加不激活显示、隔离应用元数据、320 逻辑像素内容区及定时普通关闭，和交付 EXE 的差异单列。[逐轮结果与范围](shutdown-guard-summary.json) | [320 px 登录实拍](shutdown-guard-login-candidate.jpg)

普通 `lib/main.dart` 已用完整 Flutter Windows Release 命令构建成功，沿用默认图标裁剪。Dart AOT 与修复前逐字节一致，字体子集也完全相同；新 ZIP 已逐文件校验哈希与 CRC。正在使用的原预览目录 371 个文件全部未变。[构建日志](shutdown-guard-production-build-final.log) | [程序包校验](shutdown-guard-canonical-package.json)

这组结果解决上述引擎已失效后的消息通道注销崩溃。此前 `ExitProcess` 后的驱动清理等待、连续前台无障碍查询开销、实际呈现帧率、真实登录写入、同窗口跨宽度返回及其他硬件仍按各自验收项继续核对。

## 账号页同窗口尺寸切换

普通 Release AOT 与完整交付 DLL 集合在同一个诊断进程中完成 1280 → 390 → 320 → 1280 逻辑像素切换。原生客户端实际宽度与 144 DPI 逐步核对；390 px 实拍显示完整游客登录表单。窗口从首帧起运行约 115 秒，普通关闭后 234 ms 到达引擎销毁完成标记，内核退出码为 0，没有强制终止。[尺寸、身份与范围](account-resize-summary.json) | [逐秒原始记录](account-resize-events-1.jsonl) | [同窗口 390 px 实拍](account-resize-mobile-390.jpg)

诊断宿主自行定时调整尺寸，保留普通 AOT 与网页，未操作正在使用的预览。桌面及 320 px 捕获受到其他前台窗口遮挡，图像已丢弃；因此本轮提供尺寸切换和关闭结果，以及 390 px 的显示证据。完整跨宽度视觉、返回发起游戏和真实账号写入继续待验。115 次前台 PID 快照均为其他进程，采样间隔为一秒。[原生入口](account-resize-resize-main.cpp) | [窗口计时](account-resize-resize-window.cpp) | [进程结果](account-resize-result-1.json)

## 前台与后台的同进程对照

另对同一个普通 Release 进程、相同创建时间采集三组各 11 点。该版本处于历史修复的单次地址读取候选阶段，不能当作最终包性能证书。CPU 百分比均以一个逻辑核心为 100%。

| 采样条件 | 前台样本数 | CPU |
| --- | --- | --- |
| 界面检查前，后台 | 0／11 | 0.00% |
| 界面检查后，后台 | 0／11 | 0.77% |
| 界面检查后，前台 | 11／11 | 34.78% |

两次后台样本都较低，前台样本明显较高。这组记录支持继续沿窗口激活、消息及无障碍链路排查，不能把差异归因于检查连接本身。采样期间代理没有输入，外部用户输入未连续监测。[检查前](foreground-control-before.json) | [后台](foreground-control-background.json) | [前台](foreground-control-foreground.json)

## 原作资源校验

资源 SHA-256 改在工作 isolate 流式执行，只传入文件路径与预期摘要，返回校验结果。复制进度、错误提示和安装由原资源模块管理。该规则用于所有作品的资源导入，没有新增下载或安装协议。[改动](resource-hash-change.patch) | [组件统计](resource-hash-summary.json)

Dart AOT 读取两份已有真实资源，分别匹配固定版 th07 清单和本机 th14 构建来源记录。每份文件交替执行三轮原链路与 worker，12 次摘要全部匹配。独立 isolate 每约 16 ms 发送时间戳，接收端使用同一单调时钟计算消息延迟。空载最坏为 0.057 ms。

| 输入 | 字节 | 原链路三轮最坏延迟范围 | worker 三轮最坏延迟范围 |
| --- | --- | --- | --- |
| th07 | 23,829,135 | 1.097–1.321 ms | 0.040–0.054 ms |
| th14 | 90,776,747 | 5.208–6.492 ms | 0.080–0.325 ms |

同样的代码此前也用主 isolate 的 1 ms 定时器采过样，空载已出现约 15.7 ms 间隔，因此该间隔不直接解释为文件计算阻塞。两个方法的原始记录均保留：[定时器](io-resource-hash.json) | [独立时间戳](io-resource-mailbox.json)。

现有导入检查用 th07 真实 DATA 和以该文件构造的 ZIP，通过现代与旧版包、错误作品声明、非法路径、损坏替换、原安装复开，以及桌面和两种窄屏的详情导入、返回和启动占位，共 6 项。ZIP 是本地参考样例，不称为独立发布包。静态分析通过，另重跑 133 项界面、语言、导航和采集检查，210 张同条件控件图全部逐字节一致。[输入登记](io-inputs.json) | [实际入口检查](io-resource-tests-final.log) | [画面比较](resource-hash-capture-comparison.json)

这里测量 Dart 组件的消息延迟，不等于 Flutter 整页帧率。样本使用共享 Windows 主机和文件缓存；th14 是额外的大文件输入，没有给固定目录新增作品。校验总时长未形成稳定加速结论。新普通 Release 使用 Flutter 官方 assemble 增量生成，原生构建输入沿用；cnativeapi.dll 仅 PE 时间戳不同，除时间戳外全部字节一致。产物写入独立目录，运行中的旧预览文件逐项核对未改动。[构建输入](io-build-inputs.json) | [原生库比较](io-native-asset-equivalence.json)

## 网络响应解析

账号接口达到 64 KiB 的成功与错误响应使用 SDK `Isolate.run` 解析 UTF-8 和 JSON，小响应直接解析。独立函数只接收字节，避免带入 HTTP 客户端或账号状态。现有响应大小限制、模型转换、状态码及错误码处理沿用原接口。[官方后台解析指导](https://docs.flutter.dev/cookbook/networking/background-parsing) | [SDK 传值与异常约定](https://api.dart.dev/dart-isolate/Isolate/run.html)

五份正式站公开响应从 596 到 182,420 字节，另保留 37,844 字节战绩、20,137 字节大厅评分及 110,138 字节站点清单。组件使用锁定 SDK 的原生 AOT，榜单与评分复用产品模型构造器，数量由 Python 独立解析核对。首组采样量化解析与模型耗时，后两组用独立发送端时间戳测消息接收延迟，逐次解析间隔 17–23 ms，三轮交替测试顺序。交付采样开始前确认本轮 Flutter 编译、构建及测试进程均已结束。[输入身份](payload-inputs.json) | [完整样本与范围](payload-measurements.json) | [组件源码](payload-mailbox.dart)

| 实际输入 | 原链路三轮消息接收延迟 p95 | 当前三轮消息接收延迟 p95 |
| --- | --- | --- |
| 265 人榜单，182,420 字节 | 14.634–15.871 ms | 0.182–0.501 ms |

后台任务增加调度与传值，总墙钟耗时不由消息延迟表推定。20,137 字节大厅评分继续走直接解析。测量覆盖相同实际输入与返回数量，来自共享 Windows 主机；消息到达时间包含系统调度，不等于 Flutter 帧耗时、实际呈现帧率或手机性能。其他输入的应用模型构造、已认证大响应及 32 MiB 上限没有由这份样本验证。[分项与待验项](payload-summary.json)

93 项账号传输、会话、网页及头像检查通过；真实榜单回归覆盖全部 265 行、损坏 JSON 和大错误响应。当前客户端另直接读取正式站榜单与大厅评分成功。133 项布局、语言、导航与采集检查通过，210 张控件图逐字节一致。普通 Windows Release 已完整构建，新 AOT 的账号容器通过只读状态检查，普通关闭后 94 ms 到达引擎销毁，退出码为 0；诊断宿主的非激活显示、隔离元数据和关闭计时与交付 EXE 的差异另列。[回归](payload-regression.log) | [正式服务入口](payload-public-entry.json) | [画面校验](payload-capture-comparison.json) | [构建](payload-release-build.log) | [新 AOT 实窗范围](payload-smoke-inputs.json) | [进程退出](payload-smoke-result.json)

## 资源体积

初始性能候选发布目录为 68,889,904 字节，约 65.7 MiB，其中 DLL 为 44.4 MiB，AOT 数据约 12.1 MiB，全部 Flutter 资源约 8.2 MiB。主要固定体积来自 Flutter 引擎和 WebRTC。11 张封面的理论 RGBA 展开量约 14.9 MiB，最大宽 640、高 632 像素。[逐文件清单](footprint.json)区分磁盘字节和理论解码字节，二者都不是实际驻留内存。

## 同一 AOT 的渲染器对照

当前 Windows 产品显式禁用 Impeller，使用 Skia；该兼容设置源于本机早先白屏。FlClash 最新 main 在 Flutter 3.50 恢复了 Impeller，旧搜索结果中的禁用说明已过期。其维护记录同时说明：曾复现 Windows 冻结的机器未完成移除设置后的复验。[当前入口](https://github.com/chen08209/FlClash/blob/main/windows/runner/main.cpp) | [兼容记录](https://github.com/chen08209/FlClash/blob/main/.agents/flutter-upgrade.md#impeller-on-windows)

本机正在运行的 FlClash 为 `0.9.4+2026111907`。只读核对其已加载路径对应的引擎与当前 SDK：两份 DLL 均为 21,332,480 字节，`.text` 的 16,625,152 字节全部一致，摘要为 `bc78ae5150e044536548a97aa3329fbf0a8b7ea04c46fab969a51171230db02c`。全部 65 个不同字节归属于嵌入的引擎版本字符串、COFF／调试时间戳和 PDB GUID；在内存副本中归零这些字段后，剩余字节全部相同。版本字符串分别为 `4d35714800b9266b6693a58e1225ecd7e9e45ed7` 和 `371f77202a73e8dff57a8b3bff75bca55c090aee`，不据机器代码相同推断源码提交相同。这个结果支持继续检查应用状态、查询负载和渲染路径，DLL 本身不能证明运行时选用了 Skia 或 Impeller。[已安装版本](engine-installed-flclash-version.json) | [逐区段及字段比较](engine-installed-engine-comparison.json) | [只读比较程序](engine-compare-installed-engine.py)

本机分别编译启用和禁用 Impeller 的原生入口，采用同一份诊断 Release AOT、引擎、插件、资源及高性能 GPU 偏好。两者运行生产 LauncherApp，使用隔离游客偏好与路径，均执行 32 类实际页面操作。窗口焦点每 100 ms 记录；测量期间没有构建、全量轨迹或截图读回。Skia 全程前台，Impeller 两轮均主要在后台；一次系统前台 PID 查询确认是其他应用。焦点条件不匹配，耗时不能用于判断哪种渲染器更快。此入口不包含生产 main 的全部初始化，bootstrap 不代表进程启动或可交互时间。[来源与哈希](renderer-source.json) | [全部分阶段原始统计](renderer-summary.json)

| 渲染路径 | 完成的操作类别 | 前台样本 |
| --- | --- | --- |
| skia | 32 | 831／831 |
| impeller | 32 | 5／851 |

另一轮 Impeller 同样完成 32 类，前台样本 8／846。两轮低焦点样本保留其范围，不换算成前台用户体验。Skia 的关闭期限检查在 15 秒内仍查询到进程，后续查询已不再显示；该期限检查使用进程枚举，不能代替内核终止时序。后续内核同步检查另存，继续与先前 Intel 驱动栈证据分别解释。[退出复核](renderer-exit-followup.json)

这是一组同机配对样本，尚不足以证明全部卡顿来自渲染器，或保证其他机器的兼容性。CPU 消息查询、驱动退出和真实呈现 FPS 继续分别核对。未据此更改产品渲染器；持续拖动四边、最小化恢复、公开网页纹理及多轮交替样本通过后，再判断是否移除 Skia 兼容设置。

## 无障碍连接与退出复测

两份诊断原生入口复用当前普通 `main.dart` AOT `93968c238086dfe40a6725bf2fe048bda672d15bfcd9293134e79986290c2e05`、相同引擎、插件及资源，产品的 113 个交付源码哈希全部匹配。第一份记录 Flutter 子窗口的 `WM_GETOBJECT`；第二份补充本线程全部窗口的消息计数，实际记录同时包含父窗口与 Flutter 子窗口。每 5 秒读取进程 CPU、前台 PID 和微软的客户端连接快照，六次后请求正常关闭。[来源与统计](client-summary.json) | [子窗口入口](client-client-flutter-window.cpp) | [线程入口](client-client-thread-window.cpp)

第一份入口虽使用不激活的显示命令，前两次采样仍位于前台，其间进程 CPU 约占一个核心的 47.19%；随后焦点发生变化。子窗口全程记录到一次 MSAA 查询，但这一范围没有覆盖父窗口，不能据此排除无障碍链路。第二份入口在创建阶段加上不激活窗口约束，六个采样点均为后台，相邻区间占用约 0.31%–1.88% 核心，线程消息没有复现此前的高频查询。两轮的焦点与观察范围不同，分别保存，不作性能 A/B。[第一轮](client-host-events.jsonl) | [第二轮](client-thread-events.jsonl)

当前 Windows 可以创建 `IUIAutomationClientInfoSource`，两轮共十二次读取均返回成功且数组为空。该接口提供调用时的连接快照，本结果没有识别出此前查询的发送者，也没有证明 MSAA 消费者不存在。声明取自微软元数据，未更换产品 SDK、依赖或无障碍实现。[接口约定](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-iuiautomationclientinfosource-getconnectedclients) | [声明来源](client-metadata.json)

两轮从关闭请求到引擎销毁标记分别为 141 和 266 ms，进程句柄均进入终止状态，没有强制结束测试进程。它们补充了当前 AOT 的正常退出样本，间歇性的驱动释放问题继续保留；未改动共享图形资源的生命周期，也没有操作原有预览窗口。[内核终止记录一](client-host-result.json) | [记录二](client-thread-result.json)

## 无障碍查询的已知客户端对照

新增两轮后台诊断使用相同普通 AOT，以 Windows 应用元数据隔离偏好和安全存储。被动轮六次快照均未记录到 `WM_GETOBJECT` 或已连接客户端。主动轮只对隔离窗口读取一次可访问性状态，十二次快照均在后台；第三次起，连接快照准确列出 `codex-computer-use.exe`，子窗口记录 387 次取对象消息，全线程为 451 次。随后九次快照的取对象计数不再增长，连接快照仍显示该客户端。由此验证了连接接口能够识别这个已知客户端，也区分了「保持连接」和「持续发出取对象消息」。[统计与范围](caller-summary.json) | [被动轮](caller-passive-events.jsonl) | [已知客户端轮](caller-control-events.jsonl)

这两轮在 UI 线程注册的 `IMessageFilter` 均返回成功，关闭时恢复前一个过滤器；主动轮实际发生跨进程无障碍查询，但该过滤器收到零次回调。因此不将它用于这条查询路径的发送者识别，也不以零回调证明没有查询。[诊断源码](caller-observer.cpp) | [主动轮差异](caller-control.patch) | [微软接口约定](https://learn.microsoft.com/en-us/windows/win32/api/objidl/nf-objidl-imessagefilter-handleincomingcall)

关闭请求到引擎销毁分别为 78 和 63 ms，两轮内核进程句柄均正常终止。前台 PID 每五秒采样，取对象计时不覆盖其后属性读取；这里的后台对照没有复现持续前台开销，也没有确认历史查询发送者。产品 113 个交付源码与 Windows 程序包哈希全部不变。

## 引擎调用栈的离线符号定位

SDK 自带的 Release PDB 可用于解析此前保留的地址。当前 SDK、预览包和原运行目录的引擎 DLL 均为 SHA-256 `81d227453d4e34da29e399c62d615091ffc712e54fb2ec20010c1c95487e90b6`。DbgHelp 开启严格符号匹配，实际加载 PDB，CodeView GUID 为 `d7878747-e1df-406d-9b77-ff1459176f1a`、Age 为 1；没有回退到导出名称。15 个地址均落在解析函数的范围内。[身份与结果](symbols-summary.json) | [解析入口](symbols-resolve-engine.cpp)

六个既有样本中的第 5 个包含 `AXPlatformNodeWin::get_accValue`、`GetValueAttributeAsBstr`、`GetString16Attribute`、`UTF8ToUTF16` 和 `fml::Utf8ToUtf16`，随后进入转换器与 `std::locale` 构造。这把此前「Flutter 与系统区域设置」样本进一步定位到 MSAA 属性读取链路，不能按最近的 GPU 导出名称解释。对应 SDK 源码每次调用都会创建字符串转换器。[解析明细](symbols-resolved.tsv) | [固定源码](https://github.com/flutter/flutter/blob/ae9b586f34a8443573f82ef57c3bf914a908cbf7/engine/src/flutter/fml/string_conversion.cc#L42)

独立程序直接编译该份 FML 源码，用目录中的中日文标题、英文副标题及空字符串，共 23 项，分别对照 Windows UTF-8 转换接口。每轮各执行 23,000 次，交替运行三轮。两条链路均与 Python 严格编码生成的 UTF-16 预期一致。

| 组件 | 三轮墙钟耗时范围 |
| --- | --- |
| 固定 FML 源码 | 460.19–551.45 ms |
| Windows 转换接口 | 2.07–2.58 ms |

输入、编译参数和逐轮结果均保留。[输入](symbols-conversion-inputs.json) | [组件程序](symbols-conversion-probe.cpp) | [运行结果](symbols-conversion-results.jsonl) | [Windows 接口](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar)

本次只离线读取原地址和本地符号，没有挂起正在使用的窗口。既有栈记录没有独立保存当时的 DLL 摘要，身份核对以当前相同 SDK 及发布目录为依据。组件程序使用本机 MSVC 14.50 和静态运行库，官方引擎符号记录 MSVC 14.34；组件结果不代表替换引擎后的整页帧率，也不能由六个短栈样本计算 CPU 时间占比。查询发送方及持续前台相关性继续核对。产品 SDK、引擎、113 个交付源码和 Windows 程序包均未改变。

## D3D 对象生命周期

当前普通 `main.dart` AOT 的 Intel 样本在关闭后报告退出码 0，但进程句柄持续未终止。原生栈位于 `LdrShutdownProcess` 的 DLL 清理链，插件公共符号表将调用点定位到静态 `graphics_context_` 的退出析构，随后进入 D3D11 与 Intel 驱动。用于映射的 DLL 由同一批对象重新链接，`.text` 与现场版本一致；驱动内部只保留模块与偏移，不将最近的导出名当作精确函数。[调用栈](gpu-preference-default-close-stack.json) | [符号对应](gpu-preference-resolved-stack.json)

修复将 D3D 上下文和有效标记交给网页管理器实例持有，浏览器与无头视图管理器先于它释放。WinRT 启动对象和上游 compositor 兼容处理沿用原实现。六个补丁文件按输入、输出哈希锁定，三个内联访问器调用方也从同一生成目录编译，避免新旧类布局混用。参考项目采用实例持有图形上下文；微软的 DLL 指南说明复杂清理与线程同步不能留到持有 loader lock 的卸载阶段。[参考实现](https://github.com/jnschulze/flutter-webview-windows/blob/main/windows/webview_platform.h) | [DLL 指南](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices)

隔离候选以当前普通 AOT 完成 Intel、NVIDIA 交替六轮库页关闭及一次引擎销毁后重建。真实登录入口另有 NVIDIA 容器关闭和 Intel 原生回调采样；后者记录导航成功、585 × 1131 的 GPU 纹理帧与正常退出。回调采样增加诊断日志，与无日志候选分开登记。早期仅提前重置静态对象的候选曾出现一次图形上下文丢失，未采用。全部输入、分轮状态与当前构建复验见[生命周期回执](graphics-lifetime-summary.json)。

窗口均由诊断入口自行定时关闭，通过内核句柄终止与退出码共同判断结束。未提交凭据。账号窗口曾被最小化，未将其截图作为网页视觉验收，也没有激活用户正在使用的窗口。这组结果验证资源释放与重新初始化，前台帧率、长时间功耗和其他设备继续单列。

## 后续优先级

1. 继续检查图形上下文丢失后的退出行为，以及多引擎并存、其他 GPU 驱动与持续网页负载。D3D 实例释放与引擎顺序重建见上述生命周期回执。
2. 持续记录前台窗口、遮挡和并行 GPU 工作，定位 Windows 提交回调的长等待；核对实际双显卡资源归属、跨卡传输和偶发白色捕获。
3. 定位高频无障碍查询客户端，测量 UIA 查询和画面提交的相互影响。
4. 继续拆分 Flutter 控制器创建长段，补齐可交互时间、已认证网页负载和长时间私有内存；账号历史通知修复的最终 Release 手工跨宽度验收单列。
5. 使用真实大响应、繁忙房间和硬件输入补测同步解析与媒体负载；资源校验已补两份真实文件和 th07 导入入口检查。

原有[性能整改](../performance-20261128/report.md)保留其对应构建范围。账号历史通知修复后重新采集 210 张控件图，全部逐字节一致；当前包包含新的账号 Dart AOT 与 Windows 适配器补丁。回执区分旧性能样本、当前控件回归、Debug 实窗与普通 Release 构建。未进行 Android 构建或部署。截图、帧耗时、进程资源和真实设备验收分别记录。

参考：[Flutter 性能分析](https://docs.flutter.dev/perf/ui-performance) | [Windows 无障碍桥](https://github.com/flutter/flutter/blob/master/docs/platforms/desktop/windows/Accessibility-on-Windows.md) | [Microsoft WM_GETOBJECT](https://learn.microsoft.com/en-us/windows/win32/winauto/wm-getobject) | [进程终止与 DLL 清理](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess) | [D3D11 默认适配器](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice) | [NVIDIA Optimus 导出提示说明 | 2012](https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf)
