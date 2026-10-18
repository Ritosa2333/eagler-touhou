# 独立源码版运行记录

状态：源码 EXE 已链接，完整玩法与 1:1 验收未完成。

## 构建与来源

`build_source_game.ps1` 使用 VS2019 Win32 C++ 源码工程，默认 RelWithDebInfo。输出 `dist/source_game/th20_source.exe`，仅复制 `th20.dat`、`thbgm.dat` 作为游戏资源；构建和生产运行不读取、装载或执行原 EXE。每次打包前后核对源码哈希，若编译期间源码变化会重编。`build_manifest.json` 记录实际项目编译输入、EXE 和资源摘要。

`Play.cmd` 把 APPDATA 指向包内 `userdata`。测试使用源码调试器启动同一个 EXE，不使用历史 `dist/playable` 包。

## 第一次实际运行

- EXE SHA-256：`d29166318a809787a969219c1decbf7057c089ceab90d82daf2ba5a14b643f67`。
- 运行进程 PID 43784，实际映像基址 `0x110000`。
- 启动设置窗口正常显示，选择 960×720 窗口后进入主菜单；标题背景、人物和菜单动画可见。
- 随后进入加载流程，在 Player 更新中读取空 Laser Controller，异常 `0xc0000005`，读取地址 `0x14`。
- PDB 解析：`laser::Controller::cancel_circle` → `GameFrame::cancel_lasers` → `update_player` → 调度器。不是渲染或资源文件缺失。

原始 dump、EXE、PDB、构建清单和调试日志保存在 `source_reconstruction/diagnostics/first_run/`。屏幕观察来自实际源码进程；此时未证明手动选关或完整演示能够运行。

## 按原汇编修复

Player 初始化调用的原 `412310`/`4123b0` 会执行 `4127f0`，即初始禁用回调。源码此前误传 enabled=true，导致工作线程完成其他对象创建前就更新 Player。现已修正，等待原有阶段激活逻辑启用。HUD 的同类三处注册也按相同原调用修正。

原 `4f7430` 在 `4f83e8` 执行 `mov eax,1`，外层 `4feda0` 透传这个返回值。源码此前返回 0，会让调度器移除更新节点；现已改为 1。证据位于 `player_entity/frame_evidence/`。返回值与注册标志测试已补齐；最新 Player 局部对照 1,032,161 项通过，0 失败。

## 演示关卡堆损坏与修复

第二、三次运行已实际进入演示关卡，随后出现堆损坏。`diagnostics/third_run/` 保存未修复程序、PDB、dump 和日志。ASAN 诊断版的 `diagnostics/asan_interception_probe2/asan.25132` 抓到首次非法写入：敌机释放后，`Iterator::advance` 再向 Enemy+0x84 写入，调用者为 `update_enemy_controller`。

嵌套敌机查询会覆盖链表节点的单个迭代器观察者，使外层迭代器无法在 unlink 时清空 current。隔离原函数组合对照也证实原程序存在相同危险组合，20 例中 13 例会写入已退役存储。生产源码在更新/清场的两个释放循环中显式清空 current，保留 cached next 和删除顺序，消除释放后访问。详细证据见 `source_reconstruction/gameplay/ENEMY_ITERATOR_HAZARD.md`。

新增测试在实际析构后立即把测试对象所在页设为 PAGE_NOACCESS，覆盖 512 种删除/嵌套查询组合和清场。帧检查 7,682 项通过；Enemy 局部 CPU 对照 1,254,134 项通过。原始未定义内存访问不作为应当复制的正常行为。

同时修复三处分配/释放配对：Player 和 Title 使用 operator new/delete；CurveNode 使用 malloc/free。Effect 回调按原调用先禁用注册，再显式启用。审计证据见 `source_reconstruction/audit/source_game_allocation_review.json`。

修复后的普通包 SHA-256 为 `420d7379dc5f17999e07db17b976d2c67cb4bed1e1e8d7432cebb44f2bb2e91b`。诊断源码版 PID 27092 已实际运行第 1、3 关演示，第 3 关已观察到 frame 2613，尚未出现此前的非法访问。诊断 EXE/PDB 和状态快照保存在 `diagnostics/asan_after_enemy_fix/`。

随后检测到实际用户输入，同一进程进入 Easy 非演示游戏：session_flags 从演示的 32 变为 0，后续为 8，演示位始终未置位。只读观察确认第 1 关到 frame 12331、第 2 关到 frame 15557，随后进入第 3 关到 frame 7096。第 1、3 关状态快照保存在该运行目录。未再向窗口注入按键，未修改游戏内存。这是实际跨关运行证据，不是整局状态与原版相等的证明。

输入模块源码测试及 44,271 项隔离 CPU 对照也已使用当前输入文件重跑通过；不能据此断言之前 UI 工具短按 Enter 未触发的具体原因。

当前 VS2019 的 ASAN 在此 Windows 上有一个拦截初始化失败；本次仅显式继续该已公告的诊断断点，所有其他异常仍正常处理。它能够给出上述有效错误报告，但无报错不能等同于完整内存安全通过。未完成：全部角色/关卡/难度、结束与保存流程，以及同输入/同种子的整局逐帧对照。主菜单、实际跨关运行和局部 CPU 对照不能作为整款游戏 1:1 的证明。

## 第 3 关激光缓存配对错误

上述 PID 27092 随后在第 3 关被 ASAN 停止。完整报告为 `diagnostics/asan_after_enemy_fix/asan.27092`：`Type0Laser::initialize` 以 scalar operator new 分配 60 字节 `Segment`，共同 `Laser::~Laser` 却用 `runtime::release_bytes`（free）释放。它是碰撞段缓存，不是动画 ID。Type1 存在同样的分配点；Type2 的同一字段原本使用 `allocate_bytes` 分配数组。

Type0、Type1 现已统一为 `allocate_bytes`，保留原 placement Segment 构造和分配失败返回行为，与共同析构配对。普通版、ASAN 版均已重编。新诊断运行 `diagnostics/asan_after_laser_fix/`，PID 27572，使用前次隔离存档的副本；原游戏存档不受影响。此时只是开始复测，不能将前次长时间运行记录解释为最终版本完整通过。

最终普通包 SHA-256：`35d2253e50d08bbb7e0f0dc9e3cbdbf182640bdec060bde5a18c0a20dbd2ed09`。独立共享池回归已在这两处分配修改后重编并再次运行：2,915,831 项通过，0 失败，389 个源文件哈希匹配；其中包含 Type0/Type1 实际初始化和 Segment 对象内容对照。它不替代 ASAN 下的跨关运行复测。报告见 `audit/pool_segment_allocator_regression_summary.json`。

新诊断进程 PID 27572 已再次实际跨入第 3 关，观察到 frame 8286，迄今未出现新的 ASAN 错误报告；快照为 `diagnostics/asan_after_laser_fix/stage3_state.json`。该采样证明运行达到这一点，不是完整关卡或内存安全通过。

静态 ECL 路由另核对 21 个资源、23,760 条指令；原分发器 224 个实体和 75 个通用有效 opcode 均有具体源码处理器。报告见 `audit/ecl_opcode_coverage.md`；静态覆盖不等于语义或整局一致。

## 可交付源码快照

`dist/th20_cpp_source_20260919/` 从空构建目录完整编译成功，并在激光缓存修复后重编。实际生产依赖为 54 个 MSBuild 项目、464 个编译输入，均来自快照内部；文件摘要见 `SOURCE_SNAPSHOT.json`，依赖核验见 `SOURCE_BUILD_VERIFICATION.json`。包还包含 299 个可编辑脚本文本（部分格式同时保留可读和原始转储形式），对应既有 191 个档案重编译报告。生产目标编译不需要原 EXE；脚本工具链单独构建。
