# TH11MP 最终验收计划：3P 挑战模式 Lunatic 主线与独立 Extra

日期：2026-10-13。状态：已按下述调查实现独立 `portable/multiplayer/rollback-full-game.mjs`。首次 smoke 在真实 room 的 challenge 合同断言处失败，尚未进入游戏；根代理已修正创建房间 URL 合同。本次输入边沿/正常退出证据修订仅完成源码核对，**未执行本修订版语法检查、smoke 或整局，没有整局通过结论**。执行由根代理串行安排。

## 当前入口与启动顺序

先 `node --check portable/multiplayer/rollback-full-game.mjs`。完整正式包和真实 P12 companion Launcher 就绪后，先作有限准备验证；以下路径占位符由根代理换成实际目录：

```text
node portable/multiplayer/rollback-full-game.mjs --launcher-root <真实companion树> --runtime <正式TH11MP包> --chrome "C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe" --cpu 1 --route direct --smoke-seconds 20 --output artifacts/rollback-smoothness/fullgame-smoke-01
```

`--smoke-seconds` 为 15–120 秒，只验证 3P 真合同/输入/音频/分段准备，receipt 始终 `passed=false`、`fullRunComplete=false`。准备无错误时可 exit 0，**绝不表示整局通过**；性能不达标仍在 failures 中保留。正式整局去掉 smoke，使用 `--minutes 90` 和新的目录；支持 10–90 分钟预算、CPU 1/4、RTC/direct 或 relay。根代理需采用自己的有界后台执行方式处理 nig 单次等待限制，本脚本不修改工作区服务、不修改测试站。

本机 Playwright 默认下载的 Chromium 1223 尚未安装，已有 Edge 可用路径如上；必须显式使用真实安装实例，不下载浏览器。OGG 默认实际目录 `D:/workspace/eagler/games/web-content/th11/music`，也接受 `--music-root`。已只读确认 18 个实际 OGG，包括原结果 cue 的 `th10_17.ogg`。真实 MusicCatalog 决定映射；仅预取 title/所选首关（主线 1、Extra 7），其后按实际 stage/请求 cue 用标准 Host resources 单曲渐进写入。

实现每 100 ms 记录 audio9 和 `core.memory.buffer.byteLength`（后者不是 live heap）；每 10 秒持久化 3 个 peer segment 与独立 relay heartbeat，segment 含前一段 SHA 链；每 2 秒写小型 `progress.json` 供轮询。原始 rAF/输入/包调度为有界缓冲；分段中的未到期 send 会在真正发送时另写 delivery 记录，避免段切换丢失实际发送证据。性能失败保留并继续观察后续关卡，完整正确性/音频/崩溃错误则终止，留下最近健康段和首次错误。完整结果保存真正原生菜单写出的 Replay，再由实际包内元数据检查器调用原生 ReplayArchive 校验，并核对 3P/Lunatic/challenge/completed、1–6 chapters 及三端 confirmed 输入共同前缀。

当前 runner 强制要求真实 `th11_mp_scene_status` 12 字段 version 1；根代理已增加生产源码，仍须使用实际包含它的新构建。主线每端必须在真实完整且 fully confirmed 的 native 回调后观察到 active、flags Staff 位及精确 `staff.msg`，并有至少两次 frames/time 推进，再进入 Results。周期截图保留为视觉证据，不能替代脚本状态门。Extra 原作没有 Ending/Staff，独立流程不要求、也不宣称该覆盖。

用户要求：在既定回滚正确性与 4× CPU 流畅度目标完成以后，挑战模式、Lunatic、3 人完整跑一遍整个游戏，注意性能、固定位置闪退与密集音效崩溃。此项追加在双端正确性/4× 验收之后，不替代前面的门。

## 1. 完整运行的定义

一套已冻结正式 Release 包、固定 common/Launcher 配套合同、真实 retail DATA、三个独立桌面 Chrome 进程。以**同一个新联机 generation 从 Stage 1 开始**，真实 3P 输入推进 Stage 1、2、3、4、5、6，进入原生 Ending、Staff、通关结果，完成正常结果/录像保存与退出生命周期。

必要证据：三端相同合同和 build identity；各关进入/离开时的 stage、phase、全局 multiplayer_frame/确认边界；每关完整连续输入区间；Ending/Staff/结果截图；无 unexplained gap 的分段记录及完整确认输入 Replay。判定不依赖一个最终 hash，也不能把六次从各关直接起跑拼成“完整跑通”。

本篇的“整个游戏”指 Lunatic 正常本篇 1–6 关与结局。Extra 是独立 Stage 7、difficulty 4 的另一条产品流程，不属于 Lunatic 1–6。完成主线后另用 `--extra` 开真实 3P 新房间/新收据，从 Extra 正常开场推进至原生紧凑结果与 Replay 保存、退出；两份连续运行分别列出，不能拼成同一 generation。

禁止通过 `th11_probe_stage`、`next_stage`、直接写 completion.exit、生命/无敌/敌血、跳过 Update/Draw、离线高速 tick 或重写原始输入来实现过关。现有 `native-gameplay.cpp` 有直接设 Ending/碰撞等用于组件边界的 fixture；那些证据绝不充当本整局过程。

## 2. 已读源码的真实规则

| 规则 | 已核对实现 | 对执行的影响 |
| --- | --- | --- |
| 挑战不扣残机 | `PlayerFrame::die` 的 `!input.challenge` 门 | 不需要人工无敌或补命 |
| 命尽不进 ghost | `PlayerFrame::death_frame` 的挑战门 | 三人反复自然被弹仍会复活 |
| 自然死亡过程 | hit 后 life_state 4；state_timer>7 后 die；死亡段 >=30；出场段 >=60 回到活动态 | 会经历真正死亡特效、P 掉落、清弹和复活；只能根据逻辑帧近似说明，不能按墙钟保证时长 |
| Miss | `MultiplayerPilot::record_death` 饱和递增；跨关保留；新局清零；参与 mp_hash | 不可因死亡继续就掩盖状态漂移 |
| 隐藏残机/资源仍在 | 原生赠命、碎片 Extend、Power 赠送逻辑保留 | 统计必须包含资源，不能只看是否活着 |
| 禁 Bomb | `mp_set_input` 清 bit 2；special_available=false；`start_bomb` 与 `BombController::start` 再挡 | 本整局**没有真实 Bomb/河童盾负载**；普通模式专项仍必要 |
| 正常关卡推进 | `StageCompletion` 非 6/7 关请求 next_stage；`GameSession::update_multiplayer` 调用真正资源切换 | 切关由原生脚本/完成逻辑决定 |
| 第 6 关结尾 | completion ending_frames 180 开始 fade、>=380 请求 Ending；原生 Ending/Staff 完成才进 Title Results | 不能看到第 6 关就提前算完 |
| 剧情输入 | `Dialogue::update` 处理 fire/Enter edge 或允许时的 Ctrl；`Ending::update/tick` 处理真实 pressed/held | 用玩家输入推进剧情，不修改脚本 PC |
| 结果操作席位 | `GameSessionMultiplayer.cpp` 结果菜单只读 P0；Ending 各席 story OR | P0 负责姓名/Replay Save/Return，其他席清输入 |
| 回放边界 | common `InputReplay.hpp` MaxFrames=250000；Application 会在档案满前退休 | 运行上限必须先于该边界，不能把容量触发结束误判清关 |

挑战模式全死不会走普通 MP 的“全部 ghost 持续 180f 后 Game Over”。因为挑战死亡不会调用那个 game_over/ghost 路径。现有真实组件挑战用例覆盖超过 9 次零残机死亡、各机体 Bomb 禁止、掉落/复活/跨关，但它们不是浏览器耐久结果。

## 3. 新整局 runner 的最小实现范围

使用独立 `portable/multiplayer/rollback-full-game.mjs` 和专属 Host 配置，不把 15 秒双端 runner 临时放宽为无界进程。入口已经实现，以下是其来源和验收约束，实际执行结果仍待根代理收据。

继承已审阅的依赖迁移、真实 relay admission、两个 clock 的区分、WASM/manifest 哈希、独立 Chrome/GPU、受控发送延迟、managed 触控和错误留存方法。新增第三个进程与全局场景状态机。

现有 `browser-host.html::host.lobby` 写死 difficulty=1，`host.configure` 写死 music='none'。整局不能调用它们后在 native 偷换难度或音频。新 fixture 必须：

1. 创建真正的 3P room，query/room 为所选 difficulty（主线 3、Extra 4），create/join URL 明确 `challengeMode=1`。relay 的初始房间策略从 create URL 读取挑战模式。
2. 所有席位准备后，真实 start 请求 mode 0、D0/P12；start 消息没有 challengeMode 输入合同，不能向它附加字段后假定生效。服务返回的 challengeMode=true 及全部输入策略仍须逐项核对。
3. 三端 Runtime Host configure 同样 playerCount=3、所选 difficulty（主线 3、Extra 4）、challengeMode=true、同 seed/loadouts、真实 P12。使用 companion Launcher 实际 runtime options helper，由真正 room/catalog 生成。
4. 每端载入正常 DATA/fonts；真实 OGG 全部在磁盘预检大小/SHA-256，但不一次性传入三端。仅预取 title/Stage 1，随后按关卡与实际请求 cue，通过现成 Host `resources` 参数单曲传 `{path:'/music/th11_XX.ogg',url:同源URL,bytes:真实长度}`，configure 使用音乐开启的真实值。晚到曲目由现成 observeMusicWrites 唤醒 native pending cue。不得设置 music='ogg' 却不交付资源。
5. 读取 canvas WebGL renderer、三进程 PID、实际 ICE pair/native route、AudioContext running 状态。缺 OGG 时阻塞完整音频验收；可另做明确标为 SFX-only 的辅助运行，但不能替代完整音频结果。

机体第一局建议固定 `{0,3,5}`，覆盖不同角色与射击 owner；选择本身不声称是性能最差组合。完整 1–6 至少一次；需要覆盖另三机体时另开独立一局，不与第一局混为同一通关证据。

## 4. 玩家输入策略

不做敌弹内存寻路。三端持续正常射击，P1/P2 保持不同相位的墙钟 120 Hz managed 触控轨迹，P0 在中央附近移动以确保长期有有效射击。轨迹限制在正常可移动区域，低幅度扫动与固定位置驻留交替；不得随 native frame 降低输入生产频率。

自然被弹允许，挑战的正常复活会继续运行。每个席位记录 life_state 转换、位置、Power、命、ghost；只有真实输入导致的轨迹和资源变化。如果某关击杀进度不够，允许调整**后续真实输入策略**并记录修订时间；这轮不能再当作完全相同预设输入的横向性能比较。

剧情处理优先正常 KeyZ/Enter 按下、释放，保持每个边沿至少数个实际输入帧。可以在剧情段使用真实 Ctrl 加速文本，但不打开游戏逻辑加速/调试入口，不让 Ctrl 改动战斗推进速度。首次整局保留自然对话/结局足够长，以覆盖音效、字形和纹理释放；不把所有文本一步跳到结束。

Stage 6 进入 Ending 后取消触控和连续射击，按原生流程逐段确认，确认 Staff 真的结束。通关 Results 用 P0 键盘正常完成姓名和 Replay Save；取到持久化 Replay 后再正常 Return。不能进入错误结果菜单后盲按若干键，并把退出视为清关。

三个线程继续产生真实输入；不人为停一端取同步状态。需要精确共同确认世界时仍用现有真实 Pause barrier，并明确记录属于人工暂停。整局主要采用不停局日志分段；短测已覆盖的 Pause/Resume 不必每 15 秒重做。

## 5. 有界执行与分段采证

先完成双端 stable Direct 1× →4×、relay 和 spike/断连门，然后启动整局：

- 初次整局建议 1×、真实 Direct、单向 50±5 ms、3P Lunatic/challenge；保留全部关卡和音频。
- 如通过，4× 的密集阶段覆盖必须由真实同 generation 从 Stage 1 行进抵达；可以对完整第二局施加 4×，不能一边宣称完整 4× 一边只抽 15 秒。
- 每次进程整体预算建议 90 分钟墙钟，另设全局逻辑帧 <240000 的提前报警/停止门（低于实际 250000 上限）。
- 每关持续 15 分钟墙钟仍未推进只作进度警报并保存上下文，不能直接归因崩溃；同时检查 native work、networkWait、帧是否仍推进。脚本若不再推进且无明确正常剧情/网络等待，进入有界诊断。
- 原始统计以 10–15 秒为不可覆盖 segment；每个段有 generation、逻辑帧区间、页内 timeOrigin/时间范围、前段哈希、三端计数和输入计划 ID。
- 收集与落盘不能通过停止 native 循环实现；使用小批量数据、固定上界的页内缓冲，记录 observer 成本。大截图/录像导出放边界或最后；其成本仍记录，不暗中删掉坏帧。
- 日志溢出、磁盘落盘失败或输入 ACK 覆盖不足直接使本局“证据不完整”，不能继续积累后最终 PASS。
- 每 30–60 秒写小型进度 receipt；每关进入/离开保留截图和实际 route；坏帧前后至少 30 秒 raw 数据不可丢失。
- watchdog 只关闭本 runner 创建的 Chrome/relay。人工或环境意外断网明确记录，网络消失另一端自然停止并非性能算法错误；但本次整局若不能正常完成仍是未完成，不能拼接前后成一次连续成功。

**分段恢复的含义：**先保护已落盘坏场和输入；若进程仍正常，可在同 generation 继续。若浏览器或原生崩溃，则该次连续整局失败。修复后从 Stage 1 新局重跑，或使用保存的 confirmed Replay 从头重演定位确定性闪退；Replay seek/单关诊断只作为复现，不能替代新的完整 live 通关。

## 6. 密集音效与闪退的专门证据

TH10 曾有密集音效崩溃的历史提醒，不能只观察平均 FPS 或最后没有 error。TH11 实际音频实现为 56 个 sound voice、46 个 sample 资源；初始化后复用 voice，`sound_play/stop/position` 调用 miniaudio，`pump` 在主回调消费混音并向 SDL queue 提交。这个源码结构不是“不会崩”的证明。

现有 `th11_audio_statistics` 每 100 ms 读取 9 字段：ready、混音块累计、PCM frames 累计、错误锁存、queue frames、track、fade、volume、musicPaused。错误槽 1=mix 失败，2=SDL stream 写入失败。每段要求混音持续推进、错误为 0；队列为 0 的次数与最长连续时间单独统计，不能从混音成功推导没有 underrun。

同时保留：

- AudioContext state/currentTime 的本地推进；浏览器 pageerror/console error/crash、GPU context lost、native error、运行状态。
- 各音乐 track 的真实切换和正常静音段。第 6 关 intro 自带 pause，必须结合已核对的 GameBattle/音频事件解释，不能把所有静音都判错或都忽略。
- 各关 deaths、graze、击破、拾取和同时三人射击的负载代理；当前 56-field native game status **没有 Miss 数和每音效播放计数**，p15 未用，不得填假数。
- 短测只含 SFX，整局需真正 OGG 解码和播放。配置关闭音乐、未载资源或 AudioContext suspended 的运行不算完整音频验收。
- 原生错误前最后一个 confirmed frame、rollback depth/history ticks、声音发布 exclusive frontier 与 native work，以判断是否和历史重演重复外部输出有关。

必要时新增**只读版本化统计 ABI**记录 confirmed 音效事件数、每帧最大数、各音效 ID 分布、实际播放/重启声道数及失败；只有根代理授权实现后才能使用，不能把现有混音帧数冒作声音事件数。采样仅观察，不调测试 audio_probe 额外播放、不重置 decoder/queue 掩盖故障。

挑战禁 Bomb，所以密集爆炸/六 Bomb/河童盾另用普通 MP 真实射击、拾取 Power、正常 Bomb 输入的有界专项；至少在三人同时触发附近保留更细音频与 rollback 输出记录。普通专项和挑战整局分别报告。

## 7. 合格/失败的报告方式

合格必须同时满足：合同真实、同包身份稳定、Stage 1–6 与 Ending/Staff/结果全经过、三端实际 route 正确、连续 generation 完整、输入/观测覆盖足够、没有崩溃/native/audio 错误、同 confirmed 边界导出状态一致、Replay 可保存并校验来源，且约定性能阈值通过。

逐关表必须给 logicalHz、present/frontier 最大间隔与 >50/100 ms 次数、native work p95/p99/max、rAF timestamp→entry、CPU yield/networkWait、rollback depth/history、journal 峰值、混音与排队统计。平均 FPS 掩盖单次百毫秒卡顿时不能判合格。网络信息不足允许平滑减速，突然完全停显仍按用户体验坏场保留。

若无崩溃但性能失败，结论写“完整运行稳定性通过、流畅度未通过”；若只完成前几关，写明确完成区间；若音频资源缺失，写完整音频验收阻塞。不得把本计划或已有组件 150373 checks 写成整局通过。

## 8. 开始前尚需完成

1. 双端 runner 实际收据和 4× 目标，不能跳过。
2. 正式 Release 与 companion Launcher 真实 P12 合同验证。
3. 新 3P fullgame runner 的语法检查和短程自检，检查房间 difficulty=3/challenge、第三端 route/独立 PID、资源和音频准备。
4. 对 Stage/Ending/结果状态以及 Replay 提取，用实际已导出的 ABI 定义验证；无定义的字段不得猜。
5. 决定是否增加只读 Miss/声音事件统计，以避免只能用负载代理。此为诊断补足，不要求改游戏规则。
6. 根代理串行启动；本子任务未自行运行任何命令或浏览器。

## 9. 启动夹具与验收门复审补记

2026-10-13，独立 runner 已更新并逐字 readback 为 728 行、64262 bytes；当时尚未执行本版本的语法检查、smoke 或整局。根代理负责后续实际收据，不能把下列源码修改列为已经跑通。

- 沿用双端 browser04 已成功启动的本地 HTTP 流式传输和每次运行专属 TEMP/TMP/TMPDIR。大 DATA、WASM 与 OGG 统一 `createReadStream/pipeline`、真实 Content-Length，记录读出字节、finish/close 和错误；若 headers 已发送后发生异常，destroy 该 response，避免再写 500。browser02 的 content length 失败原因没有被该修改单独证明，browser03 的 C 盘 temp 不足也不归因回滚性能。
- 请求失败记录 URL/method/failure；Host 初始化出现真实 error 时立即保存失败，不再等满 ready timeout。临时目录只影响本 runner 及其子进程，不改全局环境、不删除其他人的浏览器。
- Runtime options 改由 companion Launcher 真正生成的 `buildMultiplayerRuntimeOptions` 生成；先检查服务返回的 3P、Lunatic 3、challenge true、mode 0/D0/P12、每席 client/loadout，再用实际 catalog 映射。catalog、helper、relay URL helper 均加入身份固定检查。seed 1234 明示为夹具固定 seed，原始 relay room 没有 seed，不能宣称来源于服务。
- Chrome 启动排除 Playwright 默认 `--mute-audio`，收据保留这个选项。AudioContext running、PCM 有推进仍不等价于人工耳听或每个 SFX 都有效。
- 每席独立检查连续 Stage 中 3 秒 PCM counter 不变；菜单/暂停重置这个连续窗口。网络等待没有豁免，因为 Application 的 rAF 尾部仍须 pump 音频。逐段仍保留完整 audio9；实际到达各关的主曲须至少观察到对应 cue、正 volume、非 paused。初始 Stage 1 曲已预载，不能以它替代后续渐进 OGG 的证据。
- present/frontier 的开放间隔跨 10 秒 segment 保留 exact max，最终逐关 aggregate 同样进入性能门；150 ms 卡顿不能被分段切成两段 75 ms 而通过。离开 Stage 的正常菜单/暂停结束该连续窗口，避免把正常暂停时间报成战斗停显。
- 最终每席须实际经过 restore 与历史重演，并观察三个玩家的真实坐标变化。模式标签、预测允许位与 Host ACK 本身不能代证这些路径被执行。
- Replay Save 必须确实进入原生 name 子状态 3，再回到 slots 子状态 2；随后读取新槽的真实文件字节，经实际包 inspector 与原生 validator 校验 completed/3P/Lunatic/challenge/六章。三端 live archive 的共同 confirmed 输入前缀对照保留，独立于“保存文件正确”。
- 任意异常入口将 passed 强制设为 false，避免最后日志溢出等错误留下 status failed / passed true 的矛盾收据。

仍没有新增 production 音效热路径观测、人工无敌、跳关、手机测试或测试站修改。普通 3P 的真实 Bomb/护盾音频专项也仍是独立待验范围。

## 10. 独立 Extra 与直接 Staff 证据修订

本修订 runner 已逐字 readback：785 行、68846 bytes；保留根代理的 HTTP keepAliveTimeout=65000 / headersTimeout=70000。本子任务未执行 node、浏览器或任何命令；需由根代理对本版本语法检查、用含 scene ABI 的正式包各作短程准备验证后再安排完整运行。

### 分开的执行入口

主线原命令不加新 flag。Extra 使用另一个输出目录及独立新房间：

```text
node portable/multiplayer/rollback-full-game.mjs --extra --launcher-root <真实companion树> --runtime <含scene-v1的正式TH11MP包> --chrome "C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe" --cpu 1 --route direct --smoke-seconds 20 --output artifacts/rollback-smoothness/fullgame-extra-smoke-01
```

正式 Extra 去掉 smoke，加 `--minutes 90` 并换新目录。脚本按运行类型选择默认输出名称，仍拒绝覆盖已有 receipt。三独立进程、真实 room admission、源身份、音频/网络/回滚/移动/分段性能门共用，没有停用任何已有门。

### Extra 的实际产品流程

companion catalog 的 TH11 difficulty 索引 4 为 Extra，人数支持 [2,3]。relay 的 cooperative challenge 门没有排除 Extra。`SessionSetup::initial_stage` 令 difficulty 4 对应 Stage 7；`MultiplayerOptions::valid` 强制 stage 7 与 difficulty 4 同时成立。这不是 native probe 跳关。

Extra 完成走 `StageCompletion::complete → StageExit::Results → multiplayer_open_result(true)`，没有 Ending/Staff。原生 PauseMenu 先 state 21；上榜时正常姓名 state 25，不上榜可直接紧凑 choices 22。真实 P0 Down/Confirm 选择 Save 后进入 slots 23；确认槽进入 name 24，保存回 23，再 Back 回 22、选 Return 并正常 finished。TitleMenu Replay UI 的12字段不能代表这些状态。

Extra 的实际保存 Replay 及 live archive 都要求 difficulty 4、3P、challenge true、completed true、cheat false、mode 0、chapters [7]；主线仍要求 difficulty 3 和 [1,2,3,4,5,6]。Extra 主曲 cue 13 是 th11_15.ogg，Boss cue 14 是 th11_17.ogg，结果 cue 17 是 th10_17.ogg，继续渐进资源合同。

### scene ABI 与 Staff 门槛

| 索引 | 含义 |
| --- | --- |
| 0–3 | version 1、phase、stage、multiplayer_frame |
| 4–6 | Ending 是否存在、active、flags |
| 7 | message：0 无，1 e两位数字.msg，2 精确 staff.msg，3 其他 |
| 8–9 | Ending frames、time.current |
| 10–11 | PauseMenu state（无为 0xffffffff）、completed |

getter 只读取现有 owner，不推进解释器。所有运行强制导出存在且版本匹配。主线各端每次真实 `onGameFrame` 后检查 phase=Ending、owner active、flags&2、message=staff.msg，且确认前沿覆盖该完整帧、不在纠正中。每端至少记录初次状态及随后两次 frames 和 time 均推进的状态；首次、末次、推进次数保留于分段/收据。进入 Results 前必须通过。单一 Ending phase、hash、BGM cue 或截图不能替代。

Extra 若意外进入 Ending 直接失败；到 Results 必须读到 PauseMenu.completed，不能把 Game Over 当通关。普通 Save/Return 输入仍通过真实 Host 键盘消息完成，不调用直接保存/退休/跳场函数。

### 清理范围复核

正常、异常和 watchdog 都只操作本 runner 保存的 Browser 对象、本次 relay 子进程及本次 HTTP server；没有按进程名杀进程、全局 TEMP 变更或清理他人文件。异常保留首错、最近健康段和已落盘数据。完整运行仍受总体墙钟预算和 <240000 输入帧边界限制。


## 11. 输入边沿与正常收尾复审

本修订只编辑 fullgame runner 和本篇；没有生产、relay、short runner 修改，没有启动进程。以下均为源码落实的验收修正，等待根代理实际运行。

### 按键必须经过真实输入采样

原 180ms 墙钟按下/释放只能证明 Host 收到消息。在 CPU 让出、网络等待或大回调下，这段墙钟内可能没有新的本地输入被捕获。现在每次真实 Host keyboard 按下及释放后，分别等待本地 `rollbackStatus[6]` CaptureFrame 与 `mpStatus[3]` NextFrame 都前进至少 2 帧；generation 必须保持本局，错误标记必须为零。单边沿及 Host ACK 各有 6 秒墙钟期限，超时失败并保留起止 capture、next、phase、时间。没有调用 native 直接写键位、强制确认或推进函数。

每次 tap 的起止采样和实际墙钟耗时进入下一份 seat0 segment 的 `keyInputs`；收据有累计次数、held/released 最大等待、最后一次操作。结束键导致已经确认的真实终止时，仍发送 Host release，但不会要求已关闭输入通道再生出虚假的 release 帧，该例外由终态证据明确标识。其它操作不能使用这一例外。

Extra 的 `PauseEnd.cpp` slots23/name24 在 timer<10 时不接受按键。脚本进入 slots23 或在保存后返回 slots23 后，先等待真实 capture/NextFrame 推进 12 帧，才发送选槽/返回键，避免一次性 Down/X 落在原作入场保护时间内。姓名界面继续按真实 state 判定，不靠固定次数提前宣布保存。

### 退出证据不能依赖已经被清空的联机会话

`shell.mjs::stop` 正常退出会保存、调用 `th11_mp_stop` 并发出 Host exit；`th11_mp_stop` 又调用 `NetplayRuntime::Clear`，清除 generation、席位数量、确认前沿。这是正常生命周期。原脚本退出轮询仍断言旧 generation/3P，可能把正常退出误判失败。

现在 observer 在调用原 shell `onGameFrame` **之前**，只在真正 phase4 时锁存原生终态：同 generation、3P、scene-v1/finished、非零游戏帧、confirmed 覆盖该帧、NextFrame 已执行至该帧、非纠正且 native/audio error 为零。同时记录 retired 位，不修改它。最后正常收尾必须三端各自具有这个证据，且收到真正的 Host `exit {code:0,status:'success'}`；任意退出事件、断线或清空状态本身不能替代通关。

只有已经发出正常 Return 后的退出等待允许读取到清空的 netplay；此前的关卡、Ending、Staff、Results 与 Replay 检查仍使用本局实时 ABI。额外重启 generation、提前退休或缺少 confirmed 终态均失败。

### 原生 Replay 保存的来源复核与纠正

复审时曾把 `multiplayer_save(0)` 错当作固定写菜单第一个槽，进而推测 Host sync 会覆盖该槽；这个推测已撤回。实际 `Application.cpp` 合同是：

- API 参数 0 表示 `AutomaticReplayPath` 自动选择空文件，或复用本局已取得的自动路径。
- 菜单 index i 调用 `multiplayer_save(i+1, name)`，写 `th11_XX.rpy` 的对应编号。
- 若用户命名保存恰好接管原自动路径，生产代码会清除自动路径，下次备份重新找空路径，避免改名覆盖。

因此没有据此修改生产保存逻辑，也没有把“避开菜单 index0”当保护措施。runner 仍按实际 list 选择真正空槽。证据顺序增强为：先读菜单新文件并通过原生 decoder/元数据校验、将字节存入收据产物，再发标准 Host sync，之后重新读同文件并要求字节完全不变。这个检查用于明确原生菜单保存来源及持久化操作后的完整性，不能称为修复了一个已证实的生产覆盖 bug。

### 仍保留的范围边界

挑战模式原生禁止 Bomb，死亡不扣命、经过原复活过程；本轮输入没有伪造无敌或复活，因此它适合连续整局，但不能证明普通 3P Bomb/护盾密集音效路径。P0 Enter 确实通过正式键盘映射及 GameBattle/Ending 的 bit256→0x80000 转换推进剧情；不同阶段输入 owner 没有被绕过。主线 Staff 的两次真实推进门、独立 Extra、全音频/性能/source/Replay 门和仅清理本 runner 拥有进程的边界均保留。


## 12. smoke02 采证开销与最小修订

本节根据 `artifacts/rollback-smoothness/fullgame-smoke02-3p-lunatic` 的原始收据和已冻结证据记录。旧 smoke02 仍为失败，不能因后来发现第三方错误或修改夹具而追溯改为通过。本次只修改本 runner 与本文档，没有运行命令、浏览器、完整游戏或新性能测试，根代理须串行检查与复跑。

### 已证实的时间和性能边界

smoke02 的 receipt 起止是 2026-10-13T12:12:03.401Z 至 12:14:32.364Z，总计 148.963 秒。20 秒是主循环的目标，不是包括资源准入、任意一次取证和失败清理在内的整个进程上限。按浏览器 timeOrigin 和文件时间对齐，准入至起测约 30.766 秒，P0 初段 10.138 秒，首轮三端串行取证/分析/落盘约 38.875 秒，随后失败快照约 51.200 秒，最后清理约 17.721 秒。四次 Enter 都成功，held/release 最大分别 190.4/131.6 ms，没有触发 6 秒按键等待。

三端首段同时开始，窗口却变成 10.138/17.621/35.822 秒，段文件达到 1.98/2.88/4.71 MB；失败时循环尚未先停，三份失败文件又达到 4.46/4.68/4.96 MB。现有证据能定位整个导出/处理/落盘链很重，不能把每一秒都归因于 Playwright 序列化。新计时就是为进一步区分环节。

P0 第一段取证前已经出现 216.8 ms 停显，native 最大 33.7 ms、没有 networkWait、cpuYield 回调 253 次；这是需要保留的真实纠正停显，不能用采证开销解释掉。P1/P2 的 1410.5/6711.6 ms 坏场也保留，只有复跑能评估夹具修正减少了多少干扰。

### 新的分段导出合同

- 三端 `Promise.all` 同时请求截取 segment；每端交换原有缓冲后，在浏览器内把完整原始对象一次 `JSON.stringify`，返回“短 metadata JSON + 换行 + 原始 JSON”的**单一字符串**。Node 收齐后逐端解析、分析和写文件。没有删减 transport 字段、抽掉坏帧或停止正常测量中的游戏循环。
- 页内保留当前未写入文件的 segment 引用，成功写完对应 JSON 或故障原始字符串文件后才释放。如果一端失败，仍处理和保全其他已成功截取的端，再抛出首个错误；解析或分析失败时保存完整 `.snapshot-wire.txt`。失败快照包含未写完的旧 segment，已从正常缓存交换出去的数据不能静默丢弃。
- 每端保存 `browserCaptureMs`、`browserSerializeMs`、`browserByteCountMs`、`browserStringBytes`、`browserStringCodeUnits`；Node 保存请求/返回时间、`checkerTransferMs`、parse、字节核对、analysis、encode、write、hash 耗时。两侧核对 UTF-8 字节数。
- `checkerTransferMs` 包含浏览器捕获/序列化、调度和跨进程返回，**不是纯传输时间**。文件本身无法包含自己的已完成落盘和 hash 时间，因此完整末段计时放在 receipt 的 `segments[].snapshotTiming`，失败快照计时放在 `failureSnapshots`。
- 每端采证期限 15 秒，冻结/释放操作期限 5 秒，并保留原有缓冲上限、溢出失败门和总 watchdog。超时是证据失败，不代表已经取消浏览器里的调用；未返回原始数据不能声称已保存。最后仍只关闭本 runner 拥有的浏览器/relay。
- 该改动仍有字符串序列化和 UTF-8 字节核对的主线程开销，不能把它称为零干扰观测。若新计时仍超出可接受范围，再另行评估分块增量导出，当前没有以压缩字段或丢弃原始记录换通过。

### 失败取证与首错

失败入口先锁存首个异常及原有 failureContext，尝试先保存 receipt，再并行将本次创建的触控生产器 active 置为 false、调用各端原有 `th11_loop_stop`。不生成额外 release 输入、不重写游戏状态、不清除已有 transport 记录或定时器。然后按相同字符串协议并行取失败快照，串行落盘；冻结、采证和清理的后续异常只增加诊断，不覆盖首错。原始 touch submitted/ACK、尚未写入的段和 native/probe 状态继续保留。

这只适用于已经失败后的证据冻结；正常 10 秒分段没有停止 native 循环。发生冻结或取证超时仍维持失败，并记录具体是哪一端、哪一步没有取到证据。

### 错误来源分类

旧 smoke02 的四条 requestfailed URL 确认来自 `me.kis.v2.scr.kaspersky-labs.com`，四条相邻 console-error 没有保存 location，不能追溯按相同文本全部豁免。browser16 旧 CPU 采样中该注入脚本自身约 20.325/8.491 ms、含子调用约 69.745/11.560 ms，没有 WASM/GL 节点位于其祖先链下；这些采样不能解释 native/GL 主耗时，也不能证明系统外部影响绝对为零。

新记录完整保留所有错误，并增加 request 的实际 URL、frame URL、page URL，以及 console 的原始 location 和当时各 frame URL。仅当**该条 request URL 或 console location URL**的解析主机严格等于上述域名，且协议为 HTTP(S)，才分类 `known-external-kaspersky`、标为非阻塞；不以 console 文本、当前页面地址或页面里存在注入脚本作豁免依据。未知 console、其它来源、pageerror、crash、server/native/Host 错误仍严格失败。

`errorSummary` 分开列原始总数、阻塞数与明确外部数；外部错误不是被删除，也不改变音频、Replay、场景、实际回滚/移动或性能门。脚本没有禁用安全软件、拦截请求或修改 relay 服务。既有主线 Staff、独立 Extra、Challenge 禁 Bomb 和普通 3P Bomb 另测的边界全部保留。


## 13. 正常 segment 分块与验证/清理边界

本次在 §12 实际 smoke 后继续修订，只编辑 runner 与本文档；本子任务仍没有执行命令。根代理已运行的旧版本 job 为 `fullgame-job-smoke-01/run`，语法检查 `T66o9yZzuODgi0HbPz3WNwGP` exit 0，启动 `pCmts7n4mITUSwblXFIXfNAX`、终态只读 `Ldojnp8pgB8TXgbC-_IDmV6K`。这些是**本次分块之前**的证据，不能冒充新版本已通过。

旧 job 总墙钟 55.452 秒、有效测量约 20.35 秒，三端首段已对齐到 10.034–10.040 秒，证明前一版消除了串行窗口膨胀。但大对象 stringify 仍约 28–32 ms、TextEncoder 约 18–31 ms，checker 返回约 261–422 ms、parse 约 44–103 ms；分段仍扰动游戏。采证前第一段已有 149/152/197 ms 停显，后段聚合约 208–267 ms，流畅度仍未通过。

### 正常测量改为有界块

本节替代 §12 的**正常 segment**单次大字符串导出；失败已冻结后的快照仍沿用单字符串。

1. 三端同时交换原有 raf、transport、motion 和 touch.events 缓冲，立即对其余小 metadata 做 JSON 序列化/解析冻结，包括 Staff、terminal、overflow、errors 等共享引用。数组行在发布后保持不可变；不先 stringify 整个大 segment 再切字符串。
2. 四个 section 固定顺序是 raf、transport、motion、touchEvents。每块最多 64 条记录，完整 UTF-8 返回字符串不超过 64 KiB；为小 header 预留空间。单条记录不能放入该预算时显式失败，绝不截断、抽样或静默跳过。
3. 每端每轮最多取一块，三端并行请求，然后轮转至下一块。header 带 snapshot token、连续 sequence、section、offset、count、该 section 总数；Node 全部逐项核对，末尾必须有独立 end 标记，并核对四个总数和全段记录总数。
4. Node 重建原 probe/touch 对象结构，继续使用原有 analysis、连续关卡 aggregate、完整最终 segment JSON 与 hash 链。原有音频、实际回滚/移动、性能和源身份门保持，不排除分块传输期间产生的坏帧。
5. 未写入文件的原始 snapshot 由浏览器持有至显式 ACK；ACK 必须匹配 token、完整结束状态和最后 sequence。解析/校验/分析失败时保存已经收到的所有原始块，失败大快照仍带未写完的完整页内段。一端失败不会阻止另两端保全数据。
6. 15 秒期限覆盖整个段的 begin 至最后 end，不是每块重新取得 15 秒。元数据本身超出 64 KiB、缺块、乱序、重复、计数不符、溢出或超时均失败。未返回的数据不能宣称已保存。

transport 的 schedule 行现在创建后不再修改。实际送达时创建同 id 的独立 delivery 行，包含 actual/lateness，取消和异常信息在发布该 delivery 行之前写入；避免分块期间定时器改写旧段的 schedule 行。分析仍按 delivery 的 actual 计算延迟，两个记录共同保留调度与完成事实。

新增块级序列化耗时 sum/max、payload 与完整 wire 字节、Node 单次传输与 parse 的 sum/max、section 接收计数/块数和 complete 标记。小 metadata 冻结耗时另记。每个最终 segment 仍保留原有分段分析、写盘和 hash 计时。分块有真实主线程/Node/协议开销，实际是否减少停顿须由新 smoke 的坏帧与计时验证，不能凭源码改法先宣称无扰动。

### 明确验证窗口和清理动作

旧 job 在 checker 52055.0536 ms 开始 cleanup，随后 52177.7033/52179.866 ms 收到本地 shell 的 RTC control channel error/disconnected；此前 status 已设为 smoke-complete，末尾原始 errors 又显示 blocking=true，形成语义矛盾。旧文件不修改。

新 runner 显式记录 setup、validation、validation-complete、cleanup、closed 阶段及验证结束时间。完成测试时只写候选结果 `validationOutcome`，`passed` 仍为 false；最终状态在关闭本次拥有的浏览器/relay 后，结合全部新增错误统一决定。smoke 仅能标记 `smokeSetupPassed`，不能代替完整通关或性能通过，`performancePassed` 继续独立。

每个本次创建的 browser.close 都先记录目标席位、action id、requestedMs，返回后记录 completedMs/结果。只有同时满足以下条件的 console-error 才分类 `expected-owned-teardown`：

- 验证窗口已关闭且当前确实进入 cleanup；
- location 是本夹具同源的精确 `/runtime/shell.mjs`；
- 首行是精确的 RTC peer P1/P2/P3 disconnected，或 input/control channel closed/error；
- 消息所指的目标席位已经有明确发出的本次 owned browser.close 动作，时间早于该错误被收到。

P号映射已核对真实 `third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp`，文本使用 `P${peerId+1}`，不能把任意一个浏览器关闭当作任何错误的豁免。该分类表示与明确清理动作关联，原错误、来源、blocking 原值和关联 action id 均保存；不声称仅靠晚到时间证明所有物理因果。

其余晚到未知 console、pageerror、crash、server 错误仍进入最终失败判定，即使测试循环已经结束也不白名单。收据分开列 sourceBlocking、validationBlocking、expectedTeardown、lateBlocking 和明确外部错误数量。Kaspersky 的精确来源规则保持不变，没有关闭安全软件、拦截请求或改变游戏协议。

根 job helper 核验的原句 `const totalBudgetMs=(300+(smokeSeconds||duration))*1000;` 与预算语义保留，helper 文件未修改。没有加入 Bomb 模式；普通 3P Bomb 仍待单独授权和真实 ABI 接线，不混入 Challenge 整局。

## 14. 首轮完整主线运行期间准备的独立补丁（未应用、未执行）

正式 10 的首轮 3P Challenge Lunatic 完整流程正在进行。为保持其 source identity，当前 runner、helper、Host、Launcher 和生产源码完全冻结。本节只记录两个离线 patch artifact；写出 patch 文件不等于已应用，更不等于通过语法、浏览器或性能验收。旧 smoke / full-run 收据保留原结论，不追溯更改通过状态。

1. `artifacts/rollback-smoothness/prepared-fullgame-diagnostics-cleanup.patch`：先单独审阅／应用的窄改动，解决明确退出级联的清理顺序和默认主线／Extra 的低频 SFX 证据。可以独立于 Bomb 场使用。
2. `artifacts/rollback-smoothness/prepared-bomb-mode.patch`：以第一份应用后的 runner 为基准，另外增加显式 `--bomb-stress`；它不改变主线／Extra 默认关卡、难度、Challenge、Ending／Staff／Replay 门槛。执行前需再确认第一份基线未漂移。

两份内部的 Update File 路径均相对于 TH11 隔离树根目录。不能把 artifact 自身作为当前 runner，不能在首轮观察期应用。所有执行、语法检查、正式场启动仍由 root 串行决定。

### 14.1 已核实的正常退出级联及最小修复

最新分块 smoke 没有缺块／长度／ACK 失败，单块序列化峰值为 2.4–9.8 ms，但 3P 停显仍有 133–190 ms，性能门未通过。其结束时，先请求关闭 seat 0 的浏览器，seat 1 收到 P1 control 错误后主动广播 peer-disconnected；seat 2 因而在 seat 1 浏览器尚未正式关闭前收到 P2 disconnected。原规则没有把“任意别席已关闭”当作豁免，因此保留失败是正确的。

该级联由真实 `BrowserPeerTransport.cpp` 的 `fail(message, notify=true)`、peer-disconnected 信令接收以及 `close()` 顺序支持。待审修复在验证窗口与最终取证结束后，并行发出各席真实 `__eaglerPeerTransport.close()`，每席 5 秒上限，保存请求、完成、失败、close 前后状态，再关闭各自浏览器。该 close 先设置真实 transport 的 closed 状态，再关信令、relay、DC 和 PC，避免依靠浏览器突然消失启动故障广播。

错误分类仍只接纳同源精确 shell 路径、精确 RTC peer 文本、同一目标席位、时间上已经发出的 owned close。transport 分支还要求实际返回 performed=true；仅计划关闭、对象不存在或 close 失败都不能豁免。错误到达时动作还未完成，可以在最终结论前凭动作完成证据重新分类，原错误和来源仍全部保留。未知晚错、pageerror、crash、服务错误继续失败。并发 close 不是三个进程原子操作，不能声称消灭任何调度竞争。

### 14.2 默认主线／Extra 的 SFX 低频证据

正式 10 已导出 `th11_sfx_statistics(): u32[120]`，但首轮正在运行的旧 runner 没有读取它，因此首轮只能报告 PCM 推进及 mix／SDL latch；不能事后补称全程 ma_sound_start 零失败。

窄补丁在现有低频 small() 与最终状态读 SFX120，不在每 rAF 拷贝 120 words。逐席检查 ABI、native start failure 总数／每 ID 数、lastStartError、invalidVoiceCalls，并保留首次、最后有效值和成功数差。现有游戏／Host 原始错误先判定，避免生命周期变化掩盖首因。收尾采证之后的 final small 也硬性检查，避免尾窗漏报。

`AudioDevice::close()` 会重建 Impl 并合法清空计数；普通暂停不会。正常原生 finished 事务在 shell stop 之前已有只读终态钩子，补丁在该处一次锁存 SFX120，要求同 generation、audio ready 和已确认终态；关闭后的空对象不能被当作连续运行证据。未解释的 live 计数下降判为证据连续性失败。当前 ABI 没有额外 Impl 身份 token，报告应称“同一观察到的音频生命周期内的连续累计计数”，不得虚构更强资源身份。

成功数表示原生 `ma_sound_start` 返回成功；同帧聚合不等于每条事件各启动一次，也不证明扬声器可听、有限 PCM 或没有泄漏。

### 14.3 普通 3P Bomb 最小独立场

新模式仅显式 `--bomb-stress` 启用，新房间、新收据，真实 room difficulty=3、challengeMode=false、mode0/D0/P12、三席 loadout=[0,2,4]，从原生 Stage 1 开始。`--bomb-seconds` 默认 300，范围 60–600；禁止与 Extra／smoke 混用。默认完整主线仍为 1–6 Lunatic Challenge，Extra 仍独立 difficulty=4／Stage 7。保留 job helper 检查的原总预算句，Bomb 的 duration 改为其明确秒预算。

流程如下：

1. 连续真实射击，通过 managed 相对触控轮流把欠 Power 的活着席位移到 native y<128 的原生吸取线，另外两席保持较低且分开；按实际 game56 的位置、生命与 Power 判断，不按经过时间假设获得资源。只改触控目标与 Host 键，不修改 native Power、生命、tick 或 stage。准备阶段真实 Enter 处理剧情。
2. 三席均 alive、非 Ghost、Power>=power_step 且落回下方后，各发一次 managed bombSerial 增量。所有后续 controls 包保持每席当前 serial，防止恢复 0 产生额外边沿。每席单独保存 CaptureFrame／NextFrame、ACK 和实际进展；按前席 ACK 后至少两个捕获／模拟帧错开，不把墙钟近似宣称精确 2f。
3. native TouchController 的一次 serial 变化持续三个新输入捕获帧。最后一席也经过至少四个捕获帧后才允许暂停，避免 touch-cancel 截断。保存的真实原生 Replay 必须经 native decoder 与 build metadata 检查；按 frame→seat、12 bytes/sample、buttons u16 的 0x2 位逐席证明恰好三个连续帧，并有前后 release，不用 touchBomb flag 替代。
4. 三 Bomb 实际 active 重叠后，真实 Host Escape 进入 Pause。使用已有有限 onGameFrame tail 按相同 generation + world frame 交集取三端样本；要求完整 frontier、已确认、无 correction/history/journal、三席 present/active/error 正确，并比较 canonical game56/hash 与 Bomb body。暂停逻辑帧仍推进，不能把三次 small 的异帧结果强比较。
5. 真实 Resume 后至少执行 200 个新的逻辑帧，观察三 Bomb 自然 inactive，再真实 Pause 同帧确认并保存第二份 Replay。观察到死亡、Ghost、场景改变或 native error 的场不算自然结束。selection 0 必须在第二次同帧已确认 natural-end 样本中满足 inactive、elapsed>=181、error=0，才证明 tick(current=180) 已完成；可撤销预测样本的 maxElapsed 仅作诊断；selection 2 覆盖 distortion；selection 4 的真实 tick 会经过 10/20/30/40 重复 sound 38 路径。这里的“执行 200f”不是声称覆盖 selection 1 专有的 200f 分支。
6. 比较本场同生命周期 SFX ID 38、49 成功数确有增长，检查全 SFX failure/invalid 保持 0，同时保留 PCM、实际 rollback、运动、route、原始错误、性能和 source identity 门。相同 ID 的同帧事件会聚合，不能要求三次 native start 代表三枚 Bomb。

为了发现 critical window 的可见生命／Bomb 变化，该短窗口额外在实际 onGameFrame 回调读取 game56+Bomb24，开销进入现有 observerMs；不是每个历史重演 sub-tick 的完整生命周期证明。原始 raf/motion 行仍保存，不能排除观察导致的坏帧。critical Stage 窗口不穿插大段取证，暂停后可分块落盘；原有数组容量门保留。每个已公开回调的观察、两次 exact Pause 和真实 Replay 三类证据各自有明确范围。

普通 Lunatic 自动移动可能死亡、长期没有资源、碰到剧情或在覆盖前走到下一关。此时明确记录 Bomb coverage incomplete／失败，保留现场，不把它当回滚已坏，也不通过补 Power、无敌、跳关或延长成无限测试强行得到通过。若该最小脚本实际无法达到准备条件，应据真实轨迹调整输入策略后重新开房，不虚构成功。

### 14.4 应用后的串行验证顺序（尚未执行）

先应用窄补丁并由 root 运行 `node --check portable/multiplayer/rollback-full-game.mjs`，用独立短 smoke 验证清理级联和终态／final SFX 门后再启动 Extra。普通 Bomb 补丁另行应用及语法检查，使用显式 `--bomb-stress --bomb-seconds 300` 的独立实际场；每项失败收据保留，不能用新场覆盖旧场。当前首轮完整主线不受这些待审文件影响。


静态独立复审已修三项易误判处：每次 Pause 先记录本次请求前的 max NextFrame 下界，仅使用晚于该下界的新暂停样本，不能误取上一轮 overlap 的旧 tail；Replay 取证后重新读 SFX，避免使用早于已确认事件发布的旧 small；Bomb 的 Staff required 明确为 false。窄补丁同时确保原始 game 错误优先，且 final small 之后补上 SFX 硬门。两份 artifact 已读回核对；独立审阅没有新增阻断发现，语法及实际可达性仍未执行。

## 15. Stage 5 五秒停显：接收停滞与恢复门的证据

这是首轮完整主线终态后的只读归因记录，不修改旧场结论。时间按原始收据时钟。root 对原始 raf 前缀作离线截取（本次消息标记 `nig read137`），得到 13:43:23–29、Stage 5、逻辑帧约 90376 附近的下列计数。这里记录 root 提供的实际截取结果，不把它误写为新增浏览器复跑。

| 席位 | 停滞期间 received 固定值 | 同期 sent 约增量 | repair 起止 |
| --- | ---: | ---: | ---: |
| 0 | 424547 | +526 | 21 → 65 |
| 1 | 424647 | +541 | 26 → 71 |
| 2 | 424614 | +534 | 25 → 69 |

三端 ignored 始终为 0、IsOpen 始终为 1。约 13:43:26.38，三端 CanStart 从 1 变为 0；约 13:43:29.23–29.30，CanStart 恢复为 1，received 与 confirmed 开始快速增长。这证明了“发送和可靠补包计数仍推进、接收计数停滞、随后进入并退出可开跑门”的实际序列，不支持“达到 P12 后停止重发”的解释。

此前同一坏窗的离线汇总还记录三端仍有 486/520/525 次 rAF，networkWait 为真的回调有 474/508/513 次，最大回调间隙约 120/69/50 ms；历史重演很少，人工 send timer 实际延迟峰值约 174/157/112 ms。预测已顶到 12 帧，confirmed 约 90363/90365。由此可以排除“这五秒完全没有执行 rAF”这一具体解释；不能用这一排除宣称没有短长任务，也不能把整个坏窗归给 native 渲染。

### 源码支持的时序

以下路径均相对本 TH11 隔离树；行号是本次只读核对时的位置。

- `th11_web/cpp/sdl/Application.cpp:975–1023`：有效 rAF 先调用 `multiplayer_pump()`，然后才决定节奏、采样和模拟。P12 停止新增捕获不停止网络 pump。
- `th11_web/cpp/multiplayer/NetplayRuntime.cpp:19–20,81–115`：本作将 `inputResendMs` 设为 16 ms。`NeedsCapture` 限制新输入采样，`Pump` 独立驱动 channel；新 capture 只是强制立即发送的一个触发。
- `third_party/eagler-common/src/netplay/SessionChannel.cpp:123–175,369–417`：即使没有新 capture，仍按时间发送现有输入；ACK 前沿未推进时，每 250 ms 尝试通过可靠控制通道补包。旧发送序号不自动阻止填洞：接收路径先应用合法输入，再单独更新较新的序号和节奏信息。gate 使用 `!Disconnected() && !Recovering()`，故 IsOpen=1 仍可同时 CanStart=0。
- `third_party/eagler-common/src/netplay/NetplayCore.cpp:215–233,277–313,429–468`：确认前沿取所有远端连续输入的最小值，预测深度按连续缺口计算；发送包从对方尚未确认的最旧本地输入开始，最多携带 32 个连续输入。一个方向的缺口也能通过共同等待使三端都抵达 P12，不必假设六条有向链路同时物理断开。
- `third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp:79–82,187–247`：RTC health 每 1000 ms 检查一次。存在最近发送、超过 3000 ms 没有收到消息且满足活跃/积压条件时，进入 recoveryPending；非立即失败分支再等 2500 ms，复核仍无进展后请求 ICE restart。因此可产生约 5.5–6.5 秒加调度误差的静默恢复时序，并非“固定等五秒才补包”。约三秒后 CanStart 下降、随后恢复与这个门控路径相符；本记录没有直接证明这一次确实发出了 ICE restart。
- 同文件 `setupChannel`、`SendTo`、`SendRepairTo`：普通 input DC 为 unordered/maxRetransmits=0，control DC 为 ordered reliable；二者收到消息均更新 lastReceivedAt。可靠补包受 control 缓冲限制，但不要求有新 capture。另有 signaling 的 5000 ms 重试上限，不能仅凭数字接近就混同为本次原因。confirmed watchdog 默认 15000 ms，也不是这次五秒停显的固定定时器。

### 已知与仍未知的边界

sent/repair 的增加表示发送 API 接受或夹具成功排队，不等于已到达对端；received 是 SessionChannel 收取的包数，不直接等价于物理网卡收包计数。三端 ignored=0 也不能证明浏览器以下没有丢包。当前原始记录缺少逐 peer、逐 input/control 通道的完整收发包头，因此不能唯一定位 NIC、ICE、SCTP、操作系统、安全软件或其他底层原因，也不能把三端同时等待直接判为同一底层设备故障。

若继续定位，最小增量证据应沿六条有向 peer 链路对应 sessionId/sender/sequence/ackSequence、firstInputFrame/inputCount/latestFrame/ackFrame/senderFrame，区分普通 input 与可靠 control，并保留 scheduled/due/actual/onmessage、各通道 bufferedAmount、lastReceivedAt、recoveryPending/timer/restartInFlight 和 ICE/signaling 状态。现有包大小/签名只可辅助匹配；两条通道可能发送完全相同载荷，不能据相同签名倒推出实际交付通道。该建议仅为后续精确观测范围，本次未实施新采集器或改动网络协议。

## 16. 普通 Bomb 第一场：Resume 夹具时序修正

真实命令 `hVyhvNDyoGuKOGWcM438YTlT` 的 `artifacts/rollback-smoothness/bomb-3p-lunatic-01/receipt.json` 为 exit 1／failed，起止 2026-10-13T14:12:08.496Z–14:12:30.190Z。本场已取得真实 Power、三 Bomb 重叠、同一已确认帧 377 的三端相等 Pause body/hash、三份通过 native decoder 的 Replay 与各席恰好三个 Bomb 位输入、SFX 38/49 增量；尚未完成自然结束覆盖，旧失败不能转为通过。

失败时三端均在 world frame 385、native phase=2、PauseMenu state=34，原生错误为空。最后一次 Host Escape 的 held 实际推进 381→383、release 推进 383→385；输入已经被接受。根因是夹具在这次两帧按下／两帧释放后立即断言回到 Stage。

实际 `th11_web/cpp/game/MultiplayerMenus.cpp::update_multiplayer_pause` 在 state32 和 state33 都接受 Escape，随后进入 state34，只有 `timer.current>11` 才发出 Resume；state32 的十帧入场门仅延后普通选项开放，并非吞掉 Escape。因此这次不是“头十帧屏蔽 Escape”的生产缺陷。`GameSessionMultiplayer.cpp::update_multiplayer` 只有收到真实 PauseAction::Resume 后才销毁 pause_menu 并恢复 Stage。

当前 runner 的最小修正仍只发送一次 Host Escape，保留原 capture/NextFrame held/release 证据，然后在原普通场总预算内最多再等 10 秒，轮询真实三端 phase/PauseMenu 状态，直到三端均回到 Stage 且 Bomb 只读 ABI 有效。记录请求前、输入证据、状态转换、最后状态及等候耗时；意外菜单状态、生命周期变化、原生错误或到期均失败。不会通过再次 Escape 盲重试，以免已恢复的席位又进入暂停。200 个后续模拟帧从真正观测到 Stage 后计数，两个 confirmed Pause、Replay、SFX、生命与最终 elapsed>=181 门保持。

此修改只涉及专属 runner 和本文档，不改生产行为、不放宽确认门。本子任务未运行命令；实际语法与第二场结果由 root 串行执行并另留收据。
