# 验证与交付

[适配流程](README.md) | [文档目录](../README.md)

每条检查保存可重复的操作和结果，再更新本作适配表。

## 共用测试范围

以下命令从仓库根目录运行。测试结果按实际观察边界填写，游戏内行为按对应分项验收。

| 分项 | 共用入口 | 观察范围与逐作验收 |
| --- | --- | --- |
| 启动器与共用模块 | `npm --prefix launcher run check`、`npm --prefix launcher test`；原生共用组件使用 [common CTest](../../common/tests/CMakeLists.txt) | 核心模块、生产前端组件、共用输入与网络组件及其夹具。真实设备与游戏效果使用专项入口。 |
| 构建与打包 | `npm --prefix launcher run check` | 配方与发布包校验，包含文件集合和哈希错误的反例。正式编译、工具链及产物按 [BUILD](build.md) 分模式执行。 |
| 启动、暂停、重开 | `npm run ports:runtime -- --manifest=/absolute/inputs.json --output=/absolute/results`；合作 MP 使用 `ports:mp` 的 `--lifecycle` | 使用私有 DATA 和真实 Runtime，执行驱动中的协议与生命周期断言。记录实际选择的作品、模式和动作。 |
| 键盘与手柄映射 | `npm --prefix launcher run test:keyboard-mapping:browser` | 从产品、构建和源码目录配置枚举 STG 普通版与 MP，在 Chromium 中检查生产 JS 接收代码和键值记录端，包含虚拟手柄。角色移动、触屏和实体控制器按 [INPUT／TOUCH](input.md) 验收。th075 使用格斗输入，执行本作专项测试。 |
| 射击、Bomb、决死 | [逐作自动检查与 STG 动作验收](input.md#stg-动作验收) | th11、th12、th15 等已有原生行为对照，按本作脚本准备输入并运行。Launcher 动作送达、原生效果和窗口边界分别记录。 |
| 保持触点与禁移恢复 | `node --test tools/ports/touch-continuity.test.mjs` | 按构建目录枚举生产输入架构，检查触点、增量消费或格斗方向输入在暂时禁移后的连续性。th12 同时核对状态生成与输入接线。游戏内死亡、决死窗口与设备事件按 [INPUT／TOUCH](input.md) 验收。 |
| 音乐与音效 | `node --test tools/ports/sfx-routing.test.mjs`、`npm --prefix launcher run test:audio-output:browser` | 前者逐构建核对生产音效接线与头文件输入，后者离线渲染生产 WebSfx 的双声道 PCM、音量、暂停恢复、停止、清空及音乐任务未完成时的输出，并校验延迟输出反例。各作曲目与设备输出按 [AUDIO／MIDI](presentation.md) 验收。 |

根 CI 的 `shared-input-audio` 运行上述改键与 WebSfx 浏览器测试，以及已有的[浏览器程序交付身份检查](../../tools/browser/verified-runtime.test.mjs)。`port-adaptation` 遍历 `tools/ports/*.test.mjs` 与 `tools/testing/*.test.mjs`，包含 MP 打包归属、帧率统计与驱动覆盖校验。这些检查使用源码与合成输入；真实 DATA、房间及设备验收按各自输入另行执行。

`Checks` 汇总启动器核心、界面、输入音频、适配工具及本次受影响的 common、Relay、TURN 任务；其中任一失败或取消都会阻止汇总通过。

共用网络测试覆盖自适应选路、旁观会话恢复、Replay 传输与播放桥；[合作菜单输入测试](../../common/tests/cooperative-menu-input-test.cpp)验证多席位按键边沿和状态恢复。common CTest 自动发现 `tests/`、`browser/`、`testkit/` 中的 Node 测试，包含回放时钟、旁观时间轴和采集文件校验。Launcher 的旁观往返与角色切换场景按产品目录的联机能力生成，夹具沿用目录中的网络策略。这些测试随对应的 common 或前端 CI 执行。`shared-checks.json` 维护适配表编号与执行检查的映射，完整测试入口见[启动器测试](../../launcher/tests/README.md)及根 CI。

改键命令可用 `-- --game=th08 --variant=all` 选择单作，模式可选 `normal`、`multiplayer`、`all`。未指定作品时检查全部适用作品；新增作品对应现有接收代码布局时随配置纳入，其他布局在浏览器夹具中接入其生产模块。

`ports:runtime` 默认要求全部普通版，`--variant=all` 要求全部普通版与 MP。`ports:mp` 默认要求产品目录中的全部合作 MP，普通与挑战场景由 `--challenge` 选择。局部验收显式使用 `--game=th08`，清单缺少所选目标时直接失败；未知作品、重复目标或拼错参数也会失败。

两个 Runtime 驱动启动时先将 `report.json` 标为运行中，结束后记录通过或失败。报告列出必测作品，成功执行后汇总必测、已执行和通过数量。浏览器异常、Console 错误、HTTP 错误与未执行的必测场景会阻止整轮通过。

## 准备

先运行本作源码检查与编译，再跑浏览器操作、真实设备和网络对局。Replay 对照使用[共用工具](../../common/testkit/replay-verifier/README.md)和本作适配器。

普通版与原版做逐帧对照；MP 检查各端和多人 Replay 的自一致性。记录模板见[验证记录](record-template.md)。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="verify-01"></a>VERIFY-01 · 全部内置 Demo 对照 | 运行本作全部内置 Demo 的 quick 检查。 | 每个 Demo 覆盖到其声明结束点。 |
| <a id="verify-02"></a>VERIFY-02 · 长 Replay 样本 | 运行本作登记的 Lunatic、Extra 等长回放集合。 | 每份样本都有帧数、结束原因和结果。 |
| <a id="verify-03"></a>VERIFY-03 · 原版基准与内容哈希 | 核对基准所用 Replay、EXE、资源及 trace 的哈希。 | 来源与校验对象可完整定位。 |
| <a id="verify-04"></a>VERIFY-04 · 读取固定基准 | 以普通 candidate 检查运行一次，再核对基准文件。 | 预期数据保持原值。 |
| <a id="verify-05"></a>VERIFY-05 · 单独维护原版基准 | 在单独的基准维护流程从原版重新采集。 | 新基准附来源与审核记录。 |
| <a id="verify-06"></a>VERIFY-06 · 逐逻辑帧比较 | 比较同输入的逐帧状态，并注入一个已知差异验证报告。 | 报告首个差异帧及字段；帧序列完整对齐。 |
| <a id="verify-07"></a>VERIFY-07 · 诊断构建与正常构建 | 比较正常构建与诊断构建的编译选项和观察接口。 | 观察器只读，正常游戏沿用自己的 Demo 和输入流程。 |
| <a id="verify-08"></a>VERIFY-08 · 本作状态映射与结束条件 | 核对本作状态字段、样本选择和完成条件。 | 本作维护这些映射，共用工具负责采集格式与比较。 |

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[工具契约](../../common/testkit/presentation-lab/CONTRACT.md) · [启动器测试](../../launcher/tests/README.md)

## 测试辅助器

桌面 Runtime 验收使用 `node tools/ports/runtime-browser.mjs --manifest=/absolute/private-inputs.json --output=/absolute/results`。输入清单逐项声明 `id`、`game`、`variant`、`runtime`、`entry` 和私有 DATA 路径 `data`，共用字体放在 `shared` 字段。命令核对打包清单、启动正式 Runtime，记录协议握手、首帧、文件命令、实际按键、窗口尺寸与截图。每个场景保留独立结果，作品和模式分别执行。多人房间与实际设备使用对应专项验收入口。

该入口默认执行普通版，可用 `--game=th08` 选择单作。记录包含暂停与重开前后状态、已同步用户文件的刷新恢复、旧代请求拒绝及当前驱动身份。条目中的 `complete` 表示是否覆盖该编号的完整操作；分项截图与状态用于复核具体行为。

真实多人房间可运行 `npm run ports:rooms -- --url=http://127.0.0.1:8130/ --game=th11 --players=2 --route=rtc --output=/absolute/results`。驱动通过产品卡片、建房、入座、准备和开局控件进入本作 Runtime，记录实际通道、席位、原生帧推进与截图。`--route=relay` 验证 Relay 路径，`--players=3` 选择本作支持的三人场景。失败记录保留当前界面、房间状态和资源请求，便于区分前端准备与原生运行阶段。

导入模式通过 `--packages=/absolute/packages.json` 提供作品到本地 ZIP 的映射，驱动使用房间设置中的导入按钮和文件选择器安装。ZIP 由 `package-server.mjs` 和 `package-offline-game.mjs` 生成，测试配置与私有游戏内容放在本地证据目录。

多人命令加 `--disconnect` 后关闭一个实际客户端，检查另一端收到离开通知并停止推进。截图同时保存启动器界面与原生画布，驱动源码和哈希随结果保存。

合作 MP 的游戏行为通过统一入口 `npm run ports:mp -- --manifest=/absolute/private-inputs.json --output=/absolute/results` 验证。私有输入清单沿用 `ports:runtime` 的 `targets` 与 `shared` 格式，选择 `variant: multiplayer` 的 Runtime。默认逐作执行普通与挑战两组真实多人房间：中途加入旁观、暂停、后退后重建 Replay、恢复播放、主机旁观上传受阻 150 毫秒后的恢复、持续播放，以及第二名观众中途加入。每项检查玩家继续推进和旁观持续播放，报告保存每个实际 Runtime 文件身份、播放进度、受阻发送次数与独立观众截图。普通局自然结算时转入尾部播放检查，确认连续历史消费完成后正常结束。

`--game=th11 --players=3 --route=rtc --challenge=both --frames=3600` 可指定作品、人数、实际玩家通道、普通／挑战场景与持续观察帧数。`--route=relay` 检查玩家和旁观共用 Relay 的路径，`--challenge=challenge` 单独运行挑战模式。驱动完成各端配置后同时启动，并核对实际通道；RTC 回落到 Relay 会判为通道不符。各作使用生产选项转换器和真实的房间准备、开局协议，私有 DATA 通过正式 Runtime 协议载入。本地 Relay 为 RTC 连接提供 30 秒等待窗口，具体配置随报告保存。

加 `--wait-end` 后，普通局在持续播放检查后暂停观众，等待玩家自然结算。结束事件到达后，观众保持暂停位置，再恢复播放并核对播完最后一个输入；正常退出须发生在完整尾部消费之后。

`--restart=both` 通过实际按键操作暂停菜单，分别执行 R 快捷重开与菜单重开，核对玩家局次、旁观原生 Replay 身份、新局播放及新观众入场。th06 的原生菜单提供继续与退出，重开使用 R；报告逐项记录原生可用操作。检查以新局的播放身份与当前帧为准。

`--recover` 由独立测试进程分别在首次接收与播放中关闭真实 Relay 接收连接，检查同一旁观授权自动重连、重新接收当前关头与后续输入，以及玩家持续演算。测试控制通过父子进程管道连接本地 Relay，正式服务沿用生产入口。

`--stop-stream --challenge=challenge` 在持续播放后主动结束主机旁观流，检查两名观众均报告异常原因，同时玩家继续推进，覆盖旁观失败隔离和退出桥接。

Relay 的作品规则测试为每作分配独立成员身份，逐项验证普通／挑战设置及其开局参数。独立房间与独立成员共同隔离场景，断开回调按真实 WebSocket 生命周期完成。

断线通知包括当前通道的恢复状态或已结束状态。驱动检查共用返回按钮、原生帧停止与返回同一房间，保留当时的通道状态和提示内容。

逐作房间验收为每作创建独立浏览器进程，完成后释放进程与三个席位的资源。报告先写入临时文件再替换，后续写入中断时保留上一份完整记录。

两个浏览器入口均可加 `--gamepad`，使用标准虚拟设备和生产手柄映射，观察正式 WASM 的同步取样。检查射击、低速、方向的按下与松开，以及断开后的中立输入；原生循环未调用已启用的映射端口即判失败。该分项记录桌面 API 输入路径，实体控制器与 USB／蓝牙兼容性另行验收。

虚拟设备与其生产映射 bundle 在同一个浏览器初始化脚本中按顺序安装。运行报告分别保留页面异常、Console 错误与 HTTP 失败，初始化错误可直接定位到驱动入口。

在 `launcher/` 运行 `npm run test:support`，检查浏览器辅助器、独立 Package 模块准备、当前键盘入口和本地 HTTP 服务。历史场景输入从固定夹具读取并校验哈希，当前代码沿用生产模块。根 CI 在界面行为任务中执行这组检查。

开发服务的 HTTP 检查读取清单中的字体别名并核对响应字节，同时保持游戏 DATA 身份与每次运行的 Relay 配置。字体路径发布与游戏内容身份分别验证。

真实启动器驱动先等待首次使用提示进入可操作状态，再通过关闭按钮继续。前端启动信号与提示弹窗的显示分别等待，保证首次进入和已有偏好的流程都按实际界面执行。

独立 Package 服务通过父进程管道管理生命周期，结束检查时关闭管道并等待服务退出，同时核对端口释放和临时站点清理。

Windows 本地发布检查验证 `EAGLER_ATOMIC_DEPLOYMENT_UNSUPPORTED_PLATFORM` 提前返回且无写入；POSIX 检查验证目录符号链接原子替换。预备目录刷新另行验证共享锁重试、完整候选检查和失败回滚。按实际平台记录对应结果。
