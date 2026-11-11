# TH03 DOSBox-X 开发交接

2026-10-22 输入响应：联机物理 keydown／keyup 立即调用现有采样锁存并唤醒 pump，短按保留至 accepted frame；同步与异步 pump 在历史补算未完成时均可采样尚未发布的 live frontier。保留 RTT 缓冲、默认 page／可选 Worker、单机射击时钟、可靠通道、六步领先和十二步恢复。队列／pacing／Worker／RTT 回归、真实 Worker 双人 RTC 与主线程／Worker 原生恢复对照通过；72 模块静态审计通过。四项 behaviorFiles、inputSampling 元数据及发行清单已更新，核心／盘／美术音频不变；全员刷新重建房。完整证据及边界见根目录 `reports/pc98-input-latency-fixes-20261022.md`。未实测物理输入到显示延迟，不能从本机吞吐宣称公网提速。

2026-10-22 输入响应：联机物理 keydown／keyup 立即调用现有采样锁存并唤醒 pump，短按保留至 accepted frame；同步与异步 pump 在历史补算未完成时均可采样尚未发布的 live frontier。保留 RTT 缓冲、默认 page／可选 Worker、单机射击时钟、可靠通道、六步领先和十二步恢复。队列／pacing／Worker／RTT 回归、真实 Worker 双人 RTC 与主线程／Worker 原生恢复对照通过；72 模块静态审计通过。四项 behaviorFiles、inputSampling 元数据及发行清单已更新，核心／盘／美术音频不变；全员刷新重建房。完整证据及边界见根目录 `reports/pc98-input-latency-fixes-20261022.md`。未实测物理输入到显示延迟，不能从本机吞吐宣称公网提速。

2026-10-19 参照 TH02 ending-freeze：NativePause 低速／触控仅 phase=1 才写，拒绝签名完整但 phase>3 的桥，保留 phase=0 的退出认输 ack 回执；TH03 无 TH02 enable／stage 退出语义，不移植其地址。NativeMusic 原有签名＋INT60向量检查保持，新增 ramBase 随 capture／restore 保存，reset 清空；丢失 TSR 后继续完整640 KiB DOS校验和限定暂停扫描，未绑定时普通 syncHash 明确报错。没有修改原生桥／核心／磁盘／音频或 RTT。30项生命周期合成检查、两helper完整heap恢复、原BGM／22SE／64 journal／20960暂停搜索和280状态机检查通过；84资源中79项不变，50模块静态审计通过。本轮没有运行游戏／浏览器／服务或真实结局，不能宣称复现或修复全部TH03结局卡死。详见 reports/bridge-lifecycle-20261019.md，备份 build/bridge-lifecycle-20261019/，仍由用户试玩。

2026-10-18 后续：用户同意继续，正式接入全房 RTT 固定输入缓冲。回滚开启时，原生加载与资源／开局校验后，通过实际通道各取三次新 ping/pong，中位数向上取整后取两端较大值；≤20 ms 为零，否则 ceil(RTT/2/16.667ms)+1，上限6帧，房主提出、客机重算核对并确认，局内固定。关闭回滚不采样，保留原2帧确认锁步。队列启动预填 max(2,inputDelay)，避免从2帧启动切到4～6帧时缺输入；Netplay 缓存接收器安装前最多256条帧消息，避免快端先启动丢包。协议升 th03-rollback/6，更新 relay 校验、输入策略元数据和双清单；要重启服务、所有客户端刷新并重新建房。日志包含实际缓冲、开局样本和双方报告。42队列／8协商／24 pump合成状态场景、78有效／677无效relay向量、原同步／音频／journal检查通过；49模块与82资源审计，8冻结核心文件和上轮两份快照实现完全不变。没有启动游戏、浏览器、服务、真实连接或音频设备；不能把虚拟链路少补算当成实际FPS收益。报告 reports/th03-rtt-fixed-input-20261018.md，备份 build/rtt-input-20261018/。旧段落的零输入延迟描述是本轮之前的历史策略。

2026-10-18 对照 TH02／TH04 密集弹幕经验：web/dosbox-memory-diff.js 迁入 WASM 常驻 undo 空闲复用与 heap 增长迁移，web/dosbox-snapshots.js 收集整条 newest-first 撤销链后合并恢复，每页只写最早所需旧值。仍使用现有 tracked/untracked helper；原生核心、磁盘／游戏／音频、16000 cycles、60 Hz、stride 2、12 帧和零输入延迟保持。日志 snapshotStorage 显示 resident page undo + merged restore。原实现备份 build/snapshot-resident-20261018。详细取舍、首分配成本及复核命令见 reports/th03-density-lessons-20261018.md；组件串行 AB/BA 和完整宿主状态对照见 reports/th03-resident-snapshot-20261018.json。稳定复用的合成保存＋恢复从 2.58→1.40／2.18→0.59 ms，首次分配反而约多 2 ms，不代表游戏或启动提速。真实发行核时钟快照、原 packed／64 次 journal 对照、确认音频、48 模块及 81 资源审计通过。未运行 DOS 游戏、浏览器、服务或设备联机；保留用户自行试玩的约定。TH02 逐弹银行交换补丁不适用于 TH03 的原生双人路径；RTT 固定缓冲需要全房协商且引入延迟，本轮未修改输入策略。

2026-10-17 联机独立 SE：新增 network-audio.json/native/network-audio.com，使用 patches/music.asm 的 LOCAL_SFX 条件编译，只额外复制 AH=0Ch 的原 PMD 音效命令，原命令仍原样转发，保持驱动查询、计时和游戏自身的 SE 优先级处理。原 music.com、solo-music.com、native-patch.json、MAIN/OP/MAINL、内核和网络时钟均未改动。app.prepareDisk 仅 online 时将私有盘 WEBMUSIC.COM 替换为新桥，NativeMusic 使用新 mailbox 偏移 566；BGM/SE 共用现有 ConfirmedMusic，保持已确认前沿不可回滚。LocalSfx 预载 22 个原音效，单一活动源沿用 FM effect part 的替换语义，Web Audio 按设备时间播完整波形；仅 lockstep+independentSfx 的 adapter 丢弃原 PCM 输出，防止重复／卡顿的模拟器音频混入。启动音效不在双方建房结束后补播。网络确认仍会延迟尚未触发的事件，不能宣称网络零音频延迟。

重建：python tools/build_local_sfx_assets.py 提取 JP/CN 相同的 YUME.EFC 与 PMD.COM，编译 patches/render-sfx.asm 的独立离线 fixture；node tools/render_local_sfx.mjs 只运行 PMD/该 fixture，不运行 ZUN、OP、MAIN 或浏览器，使用原 untracked Asyncify 核在虚拟时间下逐一生成音效。保留 8 秒隔离采样，检查末端已静音，去除混音器的微小 DC 偏置并裁掉无声部分，保留 10 ms 尾部；输出 44.1 kHz mono PCM16 WAV，总计 4,365,032 bytes。原始源摘要和各文件摘要保存在 web/sfx/manifest.json。辅助 Node 环境只有空显示／音频设备适配，不创建窗口或连接声音硬件。python tools/build_network_audio.py 编译网络桥并固定 JSON LF；node tools/write_dosbox_manifest.cjs 更新 81 项联机身份，包括 22 WAV，维持 th03-rollback/5，旧资源身份会在开局比较时拒绝混用。

验证：python tools/test_music_native.py [--network-audio] 在 Unicorn 执行真实 TSR，验证 22 SE、BGM、寄存器、查询／转发和 sequence wrap；node tools/test_local_sfx.mjs 检查 22 WAV 完整性、真实 NativeMusic+ConfirmedMusic 预测丢弃／更正／stride-2 已确认去重、网络等待不停止源、单通道替换、静音命令、清理、暂停上下文、JP/CN 私有盘只替换 WEBMUSIC、预载复用不重复下载、版本／损坏资源启动失败、仅联机 PCM 静音。原 test_music.mjs、test_write_journal.mjs 和 48 模块发行审计通过。未运行游戏、浏览器或真实联机，主观音色／音量及手机听感由用户试玩确认。日志每秒样本增加 soundEffects。

2026-10-16 启动准备优化：sha256.js 的 HTTP fallback 按 8 ms 工作预算让出（每 64 KiB 检查一次），优先 scheduler.yield，不支持时保留 setTimeout；去除压缩循环中逐块迭代器和临时数组，摘要算法、原生 crypto.subtle 路径保持。startup-assets.js 的 loadLocalCore 共用已验证内核 Promise，失败清除以便重试。solo.js 并行加载磁盘／辅助／内核／音乐，音乐拆分只读 prepare 和 install；app.js 本地入口同步启动内核读取，adapter 的包装器 import 与二进制读取并行。未修改游戏／模拟时钟、CPU 预算、bootToSelection、磁盘／原生二进制或网络协议。node tools/test_startup_assets.mjs 检查 51 个含边界／偏移的 SHA 向量、存储禁用／下载失败／损坏资源拒绝、内核并发复用与失败重试、中日文完整音乐补丁磁盘与 HEAD 旧实现逐字节对照；可用 build/startup-optimization/sha256-before.js 做旧校验对照，报告 reports/startup-assets-20261016.json。该基准显式模拟嵌套定时器 4 ms 下限，不是浏览器或游戏启动测量。已更新联机资源清单，46 模块静态审计及原 write-journal 合成回归通过；未运行游戏、浏览器、服务器或音频设备。

2026-10-15 第三轮：针对 TH03 选人/切换的 NativePause 双签名搜索，新增只读 WASM SIMD helper web/pause-search.js（tools/build_pause_search.cjs 生成，524 bytes），保持扫描时机、范围、游标、候选验证与快照字段不变。按 16 字节筛首字节并在 WASM 内核对完整签名，避免逐个候选跨 JS/TypedArray 验证；未知签名或无 SIMD/原生 memory 时回退旧 JS 路径。20,960 项搜索对照、280 次原 NativePause 状态机/恢复对照、堆末端/增长/只读/JS 回退通过。桌面 640 KiB 全零/随机合成扫描约 0.326/0.721 ms 降至 0.068/0.104 ms；前缀密集人工压力例约 56.36 降至 1.41 ms，不能当成真实 TH03 内存或手机收益。报告 reports/th03-pause-search-20261015.json。

同时发现 app.js 只创建日志历史，没有调用 mountNetworkPerformance，导致 README 所称下载入口不存在；已在普通/播放器工具栏挂回“日志”，只在联机模式显示。日志增加资源身份、只读 CPU 开关/同步指令/单指令回退原始计数和搜索实现。原生计数 uint32 可溢出且会随回滚倒退，整体模式/映射回退不计入该 fallback 字段，不能用它计算精确快路径百分比。下载内容无房间令牌或 URL，保留 600 条采样及 100 条错误。清单更新至 54 联机资源，46 模块静态审计、原有 write-journal 检查通过。尚未启动游戏/浏览器/服务，未证明 TH03 忙等热点，也未启用显存查表。本轮工作区改动未自动提交或推送。

2026-10-15 正式接入：用户明确同意将两轮优化合入 main，准备自行上传 GitHub。普通 web 已使用 runtime-snapshot-adaptive-raw 的五文件集（原生调度/同步 CPU 开，显存查表关，adaptive-word8）；新增 web/vendor/dosbox-x/core-build.json 记录构建身份与保留资源摘要。发行清单更新为 53 个联机资源、14 个核心资源，16000 cycles/60 Hz/stride 2/零时间等待保持。切换前文件和工作区补丁保存在 build/core-optimization/before-main-promotion-20261015。固定浏览器文本 LF，web/native 保留原始字节，避免 Git 自动 CRLF 导致 GitHub 下载包摘要不符；额外检查暂存区实际 blob。历史“正式 web 未替换”描述对应当时的候选阶段。未自动运行游戏、浏览器或服务器，也未推送。

2026-10-15 第二轮：新增回滚脏页 adaptive-word8 扫描候选，冻结于 build/native-scheduler/runtime-snapshot-adaptive-raw。只替换 tracked snapshot helper，上一轮四个核心/包装器文件与正式 web 均保留；手动 tools/start_snapshot_candidate.bat 使用 19874 和 cpu profile。26,292 项精确差分、真实 JS 快照栈的 64 次全扫描对照/文件/全局/mailbox/增长、真实追踪核心的时钟回滚、四 profile 双清单和 127 个原网页摘要检查通过。合成稀疏 scan 约快 2.6–8.3 倍、真实静态页面分布的合成场景约快 1.2–1.8 倍；全页相等的 256 MiB 场景约慢 3%，不宣称所有负载提速。测量只涵盖 scan，不等于游戏/手机帧率。没有运行 DOS、游戏、浏览器或服务器。详见 CORE-OPTIMIZATION.md 第二轮及 reports/th03-snapshot-scan-20261015.json。整 helper O2 与其他过渡实验未作为最终入口。

2026-10-15：参照 TH04 的《DOSBox-PC98核心优化与五部作品迁移指南》建立独立 TH03 源码候选。详情见 CORE-OPTIMIZATION.md。保留 16000 cycles、零时间协作等待、stride 2 和现有 RGB 呈现器，移除 TH04 的地址专用忙等识别；迁入原生调度、受保护同步解释器、8 字节 memcmp 与可开关显存查表。最终五文件集在 build/native-scheduler/runtime-candidate，默认启动 tools/start_core_candidate.bat（19873，cpu profile，显存查表关闭）。两套核心各 72,774 memcmp、20,000 调度、20,480 显存对照通过；真实追踪核心的时钟分组回滚、宿主时钟脏页、双清单身份和 127 个原 web 文件未变检查通过。没有启动 DOS/游戏、浏览器、服务或真实联机；CPU 游戏中对照与弱设备性能仍未验收，不发布、不声称已提速。原版本和工作区补丁保存在 build/core-optimization/baseline-20261015。新增工具、报告、候选和 CORE-OPTIMIZATION.md 默认受发行白名单忽略，尚未提交。构建只读借用相邻 TH04 固定上游与工具链，不修改 TH04 源码或冻结产物。

2026-10-14：已从 TH04 迁入原生页面写入记录、批量撤销块与缓冲复用。TH03 原有 JSPI／未记录 Asyncify 的二进制和包装器指纹保持不变；16000 cycles、零等待合并、12 帧窗口、stride 2、确认音频前沿保持。新包装器从 TH03 自己的 dosbox-rollback.mjs 生成，NativePause 的 request/setFocus/setTouch/result 在写 mailbox 前标记完整 44 字节范围。startup-assets 和 adapter 共用 usesWriteJournal，校验后传入对应 WASM；资源身份保留两种变体指纹，各端只下载自己执行的变体。

重建：node tools/build_dosbox_runtime.cjs 已接入 build_write_journal.cjs，需要 build/binaryen-132、build/acorn 和原有 Binaryen native 132。存储 helper 已改用 TH04 的 packed undo 布局，shadow 数据偏移 262144，不再使用后文历史 zero flags 布局；seal/restore 可能为同一检查点积累多个块，按逆序恢复。旧 helper 测试中的 copyRange/refreshFlag 不适用于新布局。当前静态审计入口 node tools/audit_assets.mjs 使用 audit_runtime.cjs，旧 audit_dosbox_migration.mjs 是迁移早期记录，不应作为现版验收。

本轮只做构建、45 个模块语法、所有发布指纹、独立内存／文件／全局撤销和核心选择校验，不运行游戏、浏览器、服务器或设备联机。运行 node tools/test_packed_snapshot_diff.cjs 和 node tools/test_write_journal.mjs 可检查 helper 稀疏页、重复修改、shadow 增长、跨页 mailbox、回退完整复制和预加载核心匹配。新记录核心与 TH04 最终 tracked WASM 的 SHA-256 完全相同，但 TH03 实际游戏状态下的页面覆盖和提速仍待试玩确认。原始改动文件备份在 build/before-dirty-pages-20261014；日志增加 writeJournal 字段。

当前工作目录为 `th03-netplay-lab`，用户已把 `th03-pc98-fast` 覆盖回此目录。先读 README.md、MIGRATION.md；原 NP21 说明存于 build/NP21-README.md、build/NP21-DEVELOPMENT.md，属于历史，不能作为当前运行验收。

用户要求由他亲自逐步试玩，本次另已明确授权本机无窗口内核诊断，可自动选人并记录执行状态。已完成 Node 内核中日文选人到战斗、切换与战斗 12 帧完整内存回滚检查；旧调度对照复现黑屏停顿。未启动浏览器、真实音频设备、服务器或两台设备联机；这些仍由用户试玩，扩大测试范围需尊重原授权。按键和玩法沿用 TH03 原设计。

入口 app.js/solo.js 和 solo-worker.js 调用 dosbox-adapter.js。dosbox-disk.js 只提取原 HDI 的 YUMEZIKU，不启动系统 DOS/VEM486。dosbox-keys.js 仅转换原生键码。原磁盘和 patches/native 文件保持。

本地 JSPI 自动 cycles，网络完整 Asyncify 固定 16000 cycles。CPU 预算从 30000 调整后，常规战斗 600 步仍推进 564 原生游戏帧；中文密集蓄力／攻击 1200 步分别推进 1129／1102 帧，30000 对照也有原作攻击／结果停顿，未见预算降低造成额外刷新不足。不要替换为上游不完整 Asyncify、禁用回滚掩盖问题，或自动退回 NP21。网络恢复同时涉及 native RAM、4 个可变全局、continuation、虚拟时间、FS 写入与 app 端 TH03 暂停/输入/音乐状态。回滚确认前不输出预测音频，回滚批次结束后才呈现画面。

build_full_rollback.cjs 的 syncSleep：零等待协作让出不增加 virtualNow，nonSkippable 真实等待至少 1 ms。此前 1/64 ms 仍让嵌套 execl 的大量调用累积额外等待，连续选人切换为约 409 步；现在为 373／374 步，旧 NP21 相同按键对照为 370 步。禁止恢复 1/64 ms 或更早的 1 ms 处理。Normal 状态下连续协作调用上限为 1048576，每个真实等待和 step 开始重置；它只保护执行次数，不修改游戏时间。step 的 Asyncify 边界保护仍为 4096 次。app 暂停桥统一使用 module.virtualNow，并通过 nativeMusic.dosRange 将扫描限制在 640 KiB 常规内存。

同一同步帧内部的 syncSleep 在 Asyncify.State.Normal 且截止时间 <= targetNow 时直接返回，真实等待到达帧边界才挂起。必须保留 Normal 状态判断：重绕时直接返回会跳过 stop_rewind，触发 unreachable。test_dosbox_yields.cjs 检查生成代码的零等待、真实等待、重绕和循环上限。--no-coalesce 仅适合同时指定旧 --yield-quantum=0.015625 的历史诊断；零等待逐次挂起会超过 step 边界限制。不能通过跳过同步输入、加速载入阶段或改变游戏时钟来掩盖切换问题；用户明确不要等待条／载入提示。更改 CPU cycles 必须同时更新 FASTBOOT、config、manifests／wire version，并用原生游戏心跳与旧预算做对照。

app 的 Lockstep 使用 snapshotStride:2，输入仍是逐帧且 MAX_ROLLBACK 仍为 12。发生奇数帧修正时恢复不晚于 dirty 的最新检查点；原生与 app 快照须一同保留 confirmed 前的检查点，confirm(prefix,retainFrom) 分别控制音频确认与快照释放。回退可能跨过一个已确认帧，所以 NativeMusic.confirmed 和 emulator.confirmedAudio 是不可回滚的输出前沿；补算时不能再次播放这些帧。不要只改快照频率而漏掉保留边界或音频去重。

snapshot-diff.wat 仍检查所有 live bytes；全零 shadow 页只读 live 页，其余页完整比较。目录占 262144 bytes，zero flags 占 65536 bytes，shadow 数据偏移 327680。每次 copyRange、init、extend 更新 flags，restore 后 refreshFlag。相邻变化页共用 before-image buffer，各页保持最早 undo；恢复必须同时还原 live 与 shadow。test_snapshot_diff.cjs 覆盖零／非零页每个字节、随机变化、连续复制和恢复标志。

Lockstep 普通时间债务最多保留两 tick，四 tick 以上长停顿恢复一 tick；对端输入等待的恢复也限制为一 tick。每次任务仍限制 4 步 / 6 ms，历史修复单独计数。test_lockstep_pacing.mjs 使用交替 24/9 ms 更新验证有足够平均 CPU 能力时不会人为跌破 50 步/秒，并保留长卡顿／断流不会爆发追赶的检查。

startup-assets.js 缓存的是按路径和 hash 区分的原始字节，不能写入修改后的存档／补丁镜像。每次读取仍校验，存储异常可退回下载。最多 4 个并行加载，已验证的 rollback.wasm 作为 wasmBinary 传给 factory。network manifest 不包含单机专用 JSPI/fast WASM，但 vendor/runtime.json 保留完整发布资源指纹。

tools/build_dosbox_runtime.cjs 从本目录 vendor-source 和 build 重建包装器及 manifests。tools/build_lockstep_runtime.py 转调此构建，--fingerprints-only 更新指纹。修改运行文件后运行 node tools/write_dosbox_manifest.cjs，再 node tools/audit_assets.mjs。需要重建 native Asyncify 时运行 tools/build_asyncify.ps1，Binaryen 132 已随开发目录复制。

旧 NP21 构建工具和浏览器测试脚本随 tools 保留作参考，不应运行旧包装器构建来覆盖 DOSBox-X。静态审计报告写入 reports/migration-static-audit.json。

diagnose_network_transition.mjs 的 --transition-rollback 在选人／切换全程保留 stride 2 和完整窗口，i367 恢复 i356 再执行 12 步并检查完整堆；--paced-transition 使用真实 Lockstep 60 Hz 调度，包含成本与一次回滚。时限改为 battleAt < 480（相同输入确认 i90），有节奏的 Node 耗时还需 < 8 秒；旧 15 秒模拟限制过宽。JP/CN 连续诊断及稀疏补算通过；关闭／开启回滚 JP 的最后完整 DOS RAM 哈希相同。诊断程序标记仅在工具内观察 FS.open，不能依赖 _emGetRunningProgram 在 overlay 时返回 MAINL／MAIN，它仍为 OP。

diagnose_np21_transition.mjs 只读 disk-maintenance/th03-netplay-lab/web 的 NP21 真内核，使用相同补丁盘、32 倍频及相同原生按键，在 Node 中提供空画布与无音频设备适配；不能运行该目录中的浏览器／联机脚本扩大测试范围。中日文测得确认到战斗 370 步，不代表实际浏览器两端已验证。

本地端口 9876，联机后端 9877；.mjs MIME 必须为 text/javascript。新 wire/schema th03-rollback/5；房间 API 仍为 th03-lan/1。旧 MHz 字段仅作兼容固定 32，不代表 DOSBox cycles 或游戏帧率。

用户再次报告选人、标题、战斗、返回选人都等待几十秒且回滚开关无差别。读取当前进程与既有 9877 的静态资源发现：服务来自 th03-pc98-fast/lan_server.py，HTTP 返回 th03-rollback/3、30000 cycles，实际 syncSleep 仍逐次增加 1 ms 且没有帧内合并。当前工作目录为 th03-netplay-lab，不能以该旧服务表现判定新版修复失败。lan_server.py 启动显示目录和版本；端口占用时读取现有静态版本并明确新版没有启动，避免继续使用旧页。未因此改动原游戏时钟或引入载入加速／等待 UI。

后续用户明确实际是手机 + PC 局域网，两端均黑屏，连接毫秒数低；NP21 在同一低性能设备正常。不能以设备性能不足解释，也不能以 PC Node 测试替代移动端验收。已将修复资源备份后同步至 th03-pc98-fast，备份清单 reports/runtime-copy-v5.json。同步期间旧 9877 服务被关闭，未完成该端口 HTTP 复核。relay_server.py 遗漏的旧 /3 校验现改为 /5。

曾获准本机浏览器诊断：19877 仅绑定 loopback，使用 web/reports/transition-browser.html 两个实际 local.html iframe 和 WebRTC。PC 首次关闭回滚切换黑屏约 0.6 秒、回选人黑屏约 1.7 秒；开启回滚连续运行后观察到回选人黑屏约 9 秒、模拟降至十余步/秒。这个双实例共享浏览器的结果不能直接归因到手机；原任务尚未解决。Node 新 --round-trip 检查认输、胜者台词确认、返回选人、再次进入战斗；NP21 返回 774 步，DOS 约 770-774 步，进入约 370 步。总时长包括结算和台词，不等于纯黑屏时长。post-Asyncify -O2 试编译没有显著提速，仅留 build 诊断文件，未替换生产 WASM。

用户最新要求“别测了，弄一个日志，我自己测”。已停止自动诊断和本次 19877 服务；未经用户再次要求，不继续运行游戏或浏览器测试。现有“日志”按钮直接下载最近 600 个每秒样本（约 10 分钟）和 100 条错误、64 条程序读取、120 条慢步骤，日志在本地，不上传。按钮同时出现在普通工具栏和播放器工具栏。原生计时／程序读取日志属于宿主观察数据，不纳入回滚状态，不改游戏时钟。日志资源加入联机 manifest，两份目录均同步，用户后续提供手机与 PC 两份日志后再定位。
