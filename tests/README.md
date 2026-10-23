# 测试

从仓库根目录运行。测试源码由各组件维护，`tests/suites.mjs` 统一登记执行入口、范围、资源和时间预算。功能怎样判定、观察什么以及新作品如何接入，见[验收方法](../docs/adaptation/test-methods.md)。

通用适配要求使用共享场景与断言，作品模块提供本作接口、状态含义和格式解析，普通版与 MP 分别登记。原版数值、脚本和特有玩法使用本作样本与夹具，复用执行、输入校验和报告处理。新增作品沿用已有共享断言；生产行为缺失时，适配实现按同一要求补齐。

| 入口 | 内容 |
| --- | --- |
| `npm test` | 工具、Launcher 核心与三个服务的快速回归 |
| `npm run test:integration` | 快速回归、React、逐作公共夹具、共享触控与实际音效输出、浏览器接线与源码检查、common 原生测试 |
| `npm run test:acceptance` | 当前打包 Runtime、键盘与触控、语言与练习、Replay、音乐 PCM、离线存储、多人生命周期与旁观 |
| `npm run test:oracles` | th09、th11、th15 及对应独立 MP 源码的原版／原生对照套件 |
| `npm run test:list` | 各组的实际执行范围 |

选择一组并指定记录目录：

```sh
node tests/run.mjs --suite=tooling --output=artifacts/validation/my-check
node tests/run.mjs --suite=launcher-core,launcher-support
```

执行器为每项启动独立进程。共享源码、构建目录或浏览器的任务通过资源锁安排；配置、编译和执行按依赖顺序运行。超时会终止该任务的进程树。失败、取消、缺输入或空选择均返回非零状态。

各组按选择顺序运行，`--jobs` 控制组内任务并发。React 测试按文件隔离，最多两个进程；冷构建测试共用排他资源。前端模块在测试前准备一次，临时工作区各自创建和释放。

`tooling` 最多并行两个测试文件，单项测试限时 30 秒，整组限时 180 秒。单项超时由 Node 保存具体用例或文件的失败，外层执行器负责终止超时任务的进程树并封存日志。

`tooling` 的 wasm32 布局夹具使用 WASI SDK 与 Node WASI，实际编译并执行游戏源码。SDK 路径按 `EAGLER_WASI_SDK`、`WASI_SDK_PATH`、旧 `WASI_SDK_BIN`、旧 `WASI_SDK_ROOT` 的顺序选择；前两项和最后一项是 SDK 根目录，`WASI_SDK_BIN` 是 `bin` 目录。首个已设置路径无效时检查失败，不会切换到其他编译器。CI 按固定 SHA-256 准备 SDK 34。普通 C++ 夹具通过 `CXX` 选择主机编译器。

`ports-unit` 的 th06／th07 合作规则任务从源码提取选定函数体，在最小桩环境中编译执行，检查资源捐赠阈值、资源变化、复活和道具生成数量。使用 Python 3 与 C++20 编译器，`CXX` 可指定编译器可执行文件。

th15 MP 断线回归调用生产 bridge 与共享 calibration，分别核对活跃连接中断后的错误冻结和原生已退休会话的正常收尾。

th06／th07 的 Replay 源码契约随 `ports-unit` 执行，核对输入注入顺序、状态恢复与呈现步进接线。逐帧播放由独立的 Replay 验收记录。

th07 portable Replay／补丁入口通过 `--mode=unit|retail|all` 选择范围。`unit` 执行 Replay 与补丁检查，`retail` 执行原版 DAT 的 ECL 场景；默认 `all` 在编译前要求 `TH07_RETAIL_DAT`，各模式分别报告结果。

每组保存逐项日志、命令、工作目录、起止时间、耗时和结果，根目录的 `summary.json` 汇总所选组。`report.json` 在执行过程中原子更新，同一记录目录由一个执行进程独占。报告保存或结果处理异常时，执行器先停止并等待全部活动任务，再释放记录目录。Launcher 的内部检查记录也归入本次选择目录。耗时记录用于调整并发和分组。测试执行结果随当前源码和构建产物复核，历史记录可直接定位对应输入。

每次运行先建立新的运行身份和进行中记录，再解析所选组与私有输入。配置错误也保存本次失败结果。根汇总与分组记录共享运行身份，验收依据本次完成状态、进程退出码和源码身份。

执行器清除子进程的 `NODE_OPTIONS`，Node 选项由登记入口的参数提供。分片与预加载选项不能从宿主环境改变测试选择。

桌面验收入口核对实际场景参数，未知、重复或空选项立即生成失败报告。场景开关使用独立 flag；例如 `--spectator=false` 会被拒绝，避免拼错开关后改跑基础场景却得到成功结果。

## 环境

先运行 `npm --prefix launcher ci --ignore-scripts`、`npm --prefix services/relay ci --ignore-scripts` 和 `npm --prefix ports/th09/web ci --ignore-scripts`，再执行 `python -m pip install -r launcher/requirements-ci.txt` 安装固定版本的图像与 CMake 工具。浏览器依赖按 [Launcher 测试环境](../launcher/tests/README.md#浏览器环境)安装。源码检查使用 Node.js 22.18 以上、Python 3 和 C++ 编译器；`CXX` 指定的编译器须能编译并运行 C++17 与 C++20 程序。Launcher 的 staging 夹具与 common 原生测试使用 CMake，common 还使用 CTest，路径分别由 `EAGLER_TEST_CMAKE` 和 `EAGLER_TEST_CTEST` 指定。

执行器在 Windows 使用 Job Object，在 Linux 使用 child subreaper 管理完整子进程生命周期。驱动退出后的未声明后代使检查失败。Windows 任务可用 `cleanupImages: [{path, sha256}]` 声明已核实的编译器后台辅助进程；监督器按完整映像路径和 SHA-256 匹配 Job 成员，结束匹配进程，并记录 PID、映像、哈希与退出码。主任务退出码和功能断言继续决定测试结果。

本机需要这类清理时，将 `EAGLER_TEST_WINDOWS_COMPILER_HELPERS` 设置为 JSON 数组，填写实际工具路径与哈希：

```json
[{"path":"C:/Tools/Compiler/vctip.exe","sha256":"<实际的64位SHA-256>"}]
```

该输入绑定 tooling 的编译夹具、common configure/build/test、Launcher 模块构建、native-gamepad、共享 STG gate 子任务和 th06／th07 cooperative rules 任务，缺省为空。清单用于不写测试产物的编译器辅助进程；路径或哈希不符、其他后代残留都会使测试失败。此清理方式由 Windows Job Object 提供，Linux 收到清单时明确报错。common 原生任务设置 `MSBUILDDISABLENODEREUSE=1`，让构建节点随任务结束。

浏览器 Shell 回归执行真实 Shell 的初始化、监听器与 Launcher 改键路径。Runtime 验收加载带文件哈希清单的正式包，并从原生运行状态观察输入、帧推进和生命周期。

共用 protocol-host 使用独立 localhost HTTPS 站点，并校验浏览器实际收到的响应字节。需要 OpenSSL，`EAGLER_OPENSSL` 可指定路径。测试浏览器按本次临时证书的 SPKI 指纹连接；站点退出后清理证书和连接。

## 逐作适配队列

STG 的接入顺序、作品接口和单作输入准备见[STG 适配流程](../docs/adaptation/stg.md)。

`npm run test:adaptation` 按作品、普通版／MP 和场景组生成队列，执行过程与既有测试共用进程管理器。先预览同一计划，再按需执行：

```sh
npm run test:adaptation -- --game=th08 --variant=normal --suite=runtime,replay --plan
npm run test:adaptation -- --game=th08 --variant=normal --suite=runtime,desktop-input --inputs=.local/adaptation-inputs.json
npm run test:adaptation -- --game=th08 --variant=normal --check=INPUT-01 --inputs=.local/adaptation-inputs.json
npm run test:adaptation -- --resume=artifacts/validation/上次目录
npm run test:adaptation -- --rerun-failed=artifacts/validation/上次目录
```

`--variant=normal|multiplayer|both` 选择产品身份，`mp` 是 `multiplayer` 的别名。`--suite` 可选 `runtime`、`desktop-input`、`desktop-background`、`touch-motion`、`touch-actions`、`audio-latency`、`practice`、`replay`、`replay-recording`、`music`、`language`、`presentation`、`verify-quick`、`verify-daily`、`offline`、`offline-update`、`build`、`rooms`、`spectator`。省略时生成全部场景。`--check` 选择适配编号；编号存在但没有对应场景时记录 unmapped，并返回未完成。显式选择编号要求对应完整证据，场景通过但编号覆盖 partial 时返回未完成。

`--profile=adaptation --suite=build` 复核已封存的 BUILD-01..04 源码构建与 Runtime 打包报告，不触发编译或打包。使用 `EAGLER_TEST_RUNTIME_MANIFEST`，也可在 `.local/adaptation-inputs.json` 的 `runtimeManifest` 登记路径。目标项通过 `buildAcceptance.sourceBuildReport` 与 `buildAcceptance.packageReport` 引用正式 producer 输出的 `report.json`，两条路径相对于该 manifest；normal 和 multiplayer 分别登记各自报告。identity 预检及执行都会重新核对报告字节、当前源码计划与收据、Runtime 文件、包清单和 package→source receipt 绑定。缺少输入或报告时对应产品模式为 blocked；此组不读取 DATA，BUILD-05 与人工来源审查 BUILD-06 仍按各自流程记录。

任务边界为作品 | 变体 | 场景组；房间再按人数与 RTC／Relay 拆分。输入和后台分别运行，支持更新的作品把基础离线与更新旅程合在一次任务里。依赖本地 Launcher 构建的任务声明 `requiresLauncherBuild`，整轮共享一次准备，作品打包复用内容缓存。准备失败时保留失败报告，并阻断依赖它的任务；独立 Runtime 任务继续执行。`--jobs=1` 控制排队并发，浏览器与共享资源按互斥声明运行。单项缺输入会记录 blocked，其他独立项继续。

新作品随产品目录进入计划，原生观察与作品流程尚未登记时显示 blocked，并给出接入位置。th075 的正式构建和组件测试沿用本作入口，通用行为驱动在接入观察与流程后启用。

本地输入配置采用 `eagler-touhou/adaptation-inputs/1`，可登记 `runtimeManifest`、`packages`、`updatePackages`、`musicManifest`、`diagnosticManifest`、`languageManifest`、`artwork` 和 `launcherUrl`。文件与目录的相对路径以配置文件目录为基准；也可继续使用下文的环境变量。配置、私有资源与原始报告留在本地。

每轮默认创建新目录，`--output` 可指定新的证据目录，也支持其他本地磁盘的绝对路径。源码、产物和私有输入按各自清单读取，原始证据统一写入所选目录；续跑时提供该目录的完整路径。`summary.json` 包含计划、实际命令、逐项状态、日志、原始封存报告的哈希、产物与输入身份，以及逐编号覆盖。`--plan` 校验已提供的本地清单、所选 Runtime 目标与可读取的包、DATA、录像等输入，缺失或不匹配的输入记在对应场景的 blocked 状态；它不生成执行身份、不准备 Launcher 或其他构建，也不启动浏览器。远端 Launcher 地址只核对 HTTP(S) 格式，运行身份在实际执行前复核。计划报告整体状态为 planned。全部所选自动场景通过可返回 passed，`coverage.complete` 和逐编号记录说明自动证据完整度。整体验收同时汇总明确登记的人审场景集。

同目录的 `review.md` 默认展示精简的人审场景集、自动结果计数和执行问题，逐编号的机器结果、操作和通过判据折叠供追查。自动项无需逐项人工签字；“未关联自动任务”和失败项继续由测试处理。人审按[显示场景](../docs/adaptation/presentation.md#人工显示审查表)及文字显示场景集中查看，一组结论关联多个编号，发生异常时记录对象与时间。按[验证记录模板](../docs/adaptation/record-template.md)保存素材、环境和结论。

Presentation Lab 的共用控制器、报告判定、人工记录绑定和 th08 分类器回归进入 `ports-unit`，由 CI 统一执行。真实 WASM 工作台操作按本作开发指南准备输入，实体显示器的现场结果由人填写。

配对呈现任务先运行带虚拟手柄的正式包浏览器，再运行诊断工作台。正式包保存冷启动与刷新时的合成帧和独立 Replay 执行；首帧事件抵达后停止对应图像采集。诊断包保存各节奏的逐步读取与额外绘制，中央验收重读全部产物。首帧依赖真实 PNG 与发送时钟，观察一致性依赖两套二进制的共同游戏状态。视觉素材的路径隔离回归在 Windows 使用目录 junction，在其他平台使用目录 symlink，无需创建 Windows 文件符号链接的额外权限。

`presentation`、`verify-quick` 与 `verify-daily` 选用同一作品和模式时合并为一个诊断任务，共用一次本地服务准备，按顺序执行各场景。`presentation` 核对真实 WASM 的绘制纯度、权威状态、负控与恢复；`verify-quick` 比较全部登记 Demo，`verify-daily` 比较登记的长 Replay。输入通过 `diagnosticManifest` 或 `EAGLER_TEST_DIAGNOSTIC_MANIFEST` 提供，字段、场景范围和接入方法见[诊断与原版对照队列](../docs/adaptation/stg.md#诊断与原版对照队列)。每个场景分别保存采集、比较和进程报告，并沿用任务边界续跑。

离线报告的 `scenarios` 列出本次实际选择的基础和更新旅程。未选择更新时，其适配要求保留在排除范围与逐编号记录中；显式选择更新但缺少第二代包时，队列记录 blocked/incomplete。基础场景及其完整编号证明独立判定。

普通版离线场景与指纹选择同一份 normal Runtime，输入清单可以只含该目标。报告组装和封存统一经过 `runOfflineRuntimeReport`，浏览器旅程完成并收齐最终错误审计后生成场景结果。组件回归使用真实报告写入、封存文件读取和队列判定，检查未选更新、已选更新失败及晚到错误；实际离线玩法沿用正式浏览器场景验收。

离线驱动通过 Launcher 界面安装 ZIP，展开高级设置后从可见音乐菜单选择无 BGM；包未提供音乐选择时使用界面默认值，并记录实际模式。只读 IndexedDB 观察器核对实际代际与 DATA 字节，数据库版本、表名和实际注入源码进入证据身份；数据库缺失或版本不符时读取失败，观察过程保持已有数据原值。此路径直接检查正式打包站点的持久化结果。

`--resume` 沿用身份一致且封存证据完整的已完成任务，执行未完成或身份变化的任务。`--rerun-failed` 额外重跑 failed、timeout 和 cancelled；已记录的 incomplete 覆盖范围保留原证据，补充输入或修改驱动后由身份变化触发重跑。沿用结果标为 retained，保留原运行编号、时间和报告路径，本次执行标为 executed。父报告、子报告、测试依赖、Runtime、DATA、录像、字体、音乐、离线包及浏览器身份共同约束复用；文档正文变化不触发游戏重跑。执行后独立复核身份。

## Runtime 验收输入

`runtime-browser` 的范围是普通版 STG。目标从产品目录按 `inputProfile` 自动选择，新增 STG 会进入选择；未登记原生手柄接口时，运行器明确失败。格斗 profile 保留其输入适配要求，进入格斗专用验收计划。

`--profile=adaptation --suite=touch-actions` 选择共用动作组，在一次游戏准备中执行生产触控控制器的 DOM 点击和 Runtime 动作场景。切换低速使用原生键盘 Shift 的实际移速作独立参照；射击交叉保持、分别释放和取消检查输入来源隔离。Bomb 与暂停读取本作在玩家消费者完成后发布的版本化快照。动作组结束并恢复生产时钟后，独立比较物理 R 与生产 Restart 按钮：输入期间捕获原生重置帧，再核对同一实例的玩法就绪和帧推进。缺观察接口的普通版或 MP 在计划中列为 blocked；布局与设备表现按相应场景汇总。

`--profile=adaptation --suite=replay-recording` 选择有限录制与回放组。键盘、虚拟手柄、触控按钮和指针轨迹共用一次真实游戏准备与保存，独立解析实际 Replay 字节。基线和实时输入扰动回放使用相同文件，逐逻辑步核对 Replay 游标、玩家已消费输入及实际应用的指针。作品模块提供本作语义与格式解析；原版 Replay 自然终点由 `replay` 组另行验证。

触控基础使用 `--profile=adaptation --suite=touch-motion`，并沿用同一份 Runtime 输入清单。`--variant=both --plan` 展示产品目录中的普通版与 MP 接入情况；`--game=th08 --variant=normal` 执行单作。共用场景以键盘原生速度为参考，连续检查三档灵敏度、微移、斜向限速、自由摇杆、焦点与释放，并在无限速之后重新核对限速。每种配置在同一局完成整组操作，支持的 Runtime 复用正式手动步进入口。

音效队列时序使用 `--profile=adaptation --suite=audio-latency --game=th08 --variant=normal`，沿用相同 Runtime 输入清单。共用桌面驱动在一次标题导航中测量真实逻辑事件、目标 voice 的实际 PCM，以及事件发生时的 SDL 混合队列末端；第二个菜单操作把同一目标音效延迟到实测队尾，要求相同判据拒绝该反例。[音频适配器](adaptation/adapters/audio.mjs)登记本作音效映射与标题就绪状态，共用判据按实际采样率转换时间。其他作品与 MP 的接口缺项直接显示 blocked。该组记录所选音效的时序，AUDIO-02 全部玩法音效与音乐分轨按[验收方法](../docs/adaptation/presentation.md#audio-02)合并审查。

`browser-shell` 同时执行共用原生触控组件和真实 WebAudio 队列检查，CI 使用同一集合。独立任务按顺序全部执行，再汇总失败。原生组件夹具按产品目录枚举 STG 普通版和 MP source owner；th06／th07 保留已锚定原生路径的 reference-only，其余 owner 必须成功执行生产组件，缺少组件会使任务失败。固定预期取自 th06 已验证规则；五个定向破坏检查验证灵敏度、微移、限速、解析器和释放。编译器由 `CXX` 与既有原生编译入口选择，Windows helper 身份由执行器环境统一提供。

WebAudio 检查实际渲染已有 PCM 与新音效、验证队尾排程反例，并在 localhost 子 frame 的真实 AudioContext 中核验继承的 stop 观察、短声重触发区间、Worklet PCM、零音量输出和目的地断路判定。重触发要求前一次结束帧不晚于后一次起始帧，间隔取实际音频时钟。调度检查负责组件入口，音频图行为由真实渲染判定。报告保存探针依赖与生产音频源码的身份，并在渲染前后复核。组件、源码接线、真实 Runtime 场景在报告中分别标注。

`EAGLER_TEST_RUNTIME_MANIFEST` 指向本地清单，逐项登记八作普通版的 Runtime 目录、入口和 DATA，结构见[运行验收](../docs/adaptation/validation.md)。多人验收通过 `EAGLER_TEST_LAUNCHER_URL` 指定本地 Launcher；`EAGLER_TEST_PACKAGES` 可提供作品到本地导入包的映射。

适配队列的 `music` 组按产品目录与各模式源码归属生成普通版和 MP 条目，覆盖 AUDIO-01／03／04／05；声明 MIDI 的产品同时列入 MIDI-01 至 MIDI-04。当前 normal 驱动按实际登记的观察执行，未接入的适用编号列为 blocked；MP 使用独立条目。`runtime-music` 沿用同一队列选择全部普通版，统一保存缺接口、缺输入和实际运行结果。可运行场景使用 `eagler-touhou/music-inputs/1`，逐作登记私有来源目录、产品 mount 和每个 OGG 文件的名称、绝对路径、字节数及 SHA-256。

`runtime-desktop` 检查真实 Shell 的键盘、改键、模拟触控与声音输出；`runtime-practice` 使用练习启用包检查原生菜单和关闭对照；`runtime-replay` 通过正式 Shell 导入 Replay，验证 120 个默认速率原生帧、Runtime `ended` 终止信号与 HTML 原文响应，并分别记录终止信号和完整输入消费状态。现有 Runtime 信号不能区分场景退出原因，因此完整输入消费保持 unproven，原版格式兼容完成度另行报告。`runtime-offline` 逐作打包实际 Launcher，要求 `EAGLER_TEST_PACKAGES` 和 `EAGLER_TEST_ARTWORK`，检查 ZIP 安装、离线重启和文件恢复。`runtime-offline-update` 是单独的五作更新旅程，另需 `EAGLER_TEST_UPDATE_PACKAGES` 显式登记基础包与含 OGG 的第二包；它检查归档完整性、本地导入、Replay 恢复和新代离线启动。`runtime-rooms` 在支持的人数和通道组合中执行暂停、恢复、重开与断线；`runtime-spectator` 单独运行 2P Relay 旁观加入及输入隔离，保证旁观失败不会跳过玩家生命周期。每组报告按实际断言注明覆盖范围。

OGG 曲目验收使用正式普通版 Runtime 与本地私有资源清单。Runtime、字体和 OGG 通过短时 localhost HTTPS fixture 提供，浏览器响应按 fixture 的资源身份核对：

```sh
node tools/ports/music-browser.mjs --manifest=artifacts/validation/adaptation-20261019/inputs.json --music-manifest=artifacts/validation/adaptation-20261019/music-inputs.json --game=th11 --variant=normal --output=artifacts/validation/music-th11-run
```

`--output` 指向新的记录目录。音乐清单由产品目录中的来源目录和 mount 生成，并逐文件核对长度与 SHA-256；音乐源文件保留在本机。th11／th15 的报告包含原生曲目映射与 Runtime PCM，th08／th10／th20 的报告包含 decoder 统计和 PCM。

浏览器验收前可用同一组输入执行路由预检：

```sh
node tools/ports/music-browser.mjs --manifest=artifacts/validation/adaptation-20261019/inputs.json --music-manifest=artifacts/validation/adaptation-20261019/music-inputs.json --game=th11 --variant=normal --plan-only --output=artifacts/validation/music-th11-preflight
```

`--plan-only` 核对 Runtime、DATA 和每条 OGG 路由的字节身份，报告状态为 `incomplete`，不会启动 Runtime 或记作音乐播放通过。OGG 路由按作品隔离，同名曲目不会覆盖其他作品的输入。

正式运行会在 `music=none` 首帧后等待 1.8 秒，再采集 600 毫秒 Runtime 输出。只有运行中的音频上下文在该窗口持续静音，报告才确认无 BGM 对照；检测到声音时按未分类输出记录，不用于证明 OGG 效果。

实际标题 OGG PCM 场景独立记录运行结果，th11／th15 同时核对可用的原生曲目身份；无 BGM 对照作为独立观察记录。AUDIO-01 的完整切曲、场景变化、循环与生命周期范围保留在逐项覆盖记录中，不由标题场景结果替代。

普通版逐作执行，多人按产品目录生成支持的 2P／3P 和 RTC／Relay 组合。输入矩阵必须完整，清单中的产品身份、文件哈希和实际启动目录逐项核对。资源、字体、SDK 和结果保存在本地目录。

## 原版对照输入

各作 `docs/DEVELOPMENT.md` 列出原版程序、DATA、字体、回放与固定轨迹的准备方法。先构建本作 `cpp:build` 对照核心，再执行 `cpp:test` 或根 `test:oracles`。五个源码目录共用 `tools/testing/frozen-oracle.mjs`，执行前后核对核心、编译来源与完整用例清单。Node 结构化事件逐文件核对测试注册、实际执行与结果，文件级汇总和整组汇总必须一致；每个清单文件至少运行一个非 skip、非 todo 用例。spec 输出继续保存在可读日志中，跳过、取消、失败或空文件均使结果失败。每组默认允许两个 Node 测试并行，整轮执行预算为 120 分钟；测试超时、取消与总预算到期都会封存失败终态。本作 `artifacts/cpp/verification/` 保存逐项结果、结构化事件与日志。

## 新增检查

工具回归使用 `test-*.mjs` 与 `*.test.mjs` 命名，`tooling` 自动发现并运行。Launcher 核心与工作区检查从 `launcher/tests/test-plan.mjs` 展开，React 与辅助器检查由各自 suite 的测试目录发现；Relay 与 TURN 从组件 test-plan 自动发现。`node tests/run.mjs --ownership` 汇总 suite、Launcher、服务、shared-check 与本地原生 oracle manifest 的路径归属、profile 重用及同 profile 重复。每作 `oracle-suite.json` 的发现与分类沿用 oracle runner 的校验器；候选组与 maintenance 任务分别记录，漏登记、重复分类或目录外路径会使归属检查失败。`ports-oracle` 与 `oracle` profile 表示统一 `tests/run.mjs` 的调度关系；本地 manifest 登记或 profile 被选中，都不单独证明 CI 已调度或执行。ports 中没有 suite、组件清单、包脚本或原生 oracle manifest 归属的测试命名候选会列作待归属项，文件名本身不计为测试。

新增组件执行入口登记到 `suites.mjs`，声明覆盖层、输入、可观测结果和时间预算，并用故意破坏行为的反例确认失败路径。源码状态检查读取 th06/th07 配方声明的实际 CMake 依赖；运行 `node launcher/scripts/prepare-runtime-dependencies.mjs --game=th06,th07 --variant=normal` 后再执行 `npm run ports:check -- --all --check-status`。准备命令使用合成配置的单元测试不访问真实子模块。

`ports-unit` 逐项登记确定性的作品模块与 VM 夹具，使用合成轨迹测试不代表打包 Runtime 或完整游戏玩法；高刷新源结构合同随 `ports-source` 单独执行。依赖预构建产物、Emscripten/WASI、浏览器或原版程序的脚本保留作品开发指南或 package script 中的专用入口，不计入这两组的通过结果。

th08／th10 normal 与 MP 的 `test-eagler-host.mjs` 由同一 helper 注册既有 Host 断言；wrapper 显式声明产品 owner 与预期字体别名，helper 只解析该 owner 的 Runtime／Shell。字体操作使用分离的文件与目录模型，逐项检查源文件存在、`/fonts` 创建和链接目标；真实 Host 函数的缺源反例及移除目录创建调用的定点变异都必须失败。此夹具属于合成模块验证，不声明浏览器或完整游戏行为。

`ports-touch-wasi` 从产品目录生成生产触控模块 owner，直接运行每个 owner 既有 WASI wrapper；th06／th07 继续使用原生参考锚点。Linux CI 的 `source-contracts` job 使用固定 SDK 34 执行该组。本地可显式指定 SDK 并运行 `$env:WASI_SDK_PATH='D:\workspace\eagler\toolchains\wasi-sdk-34.0-x86_64-windows'; node tests/run.mjs --suite=ports-touch-wasi --output=artifacts/validation/touch-legacy`。

### 触控检查职责

| 检查 | 实际边界 |
| --- | --- |
| Launcher `player-touch` | 生产 Controller 与合成 DOM／消息端口 |
| `outside-touch-contract` | 各自本地 host 函数、几何映射与释放行为 |
| th08 `shell-touch` | normal／MP 各自生产 Shell、JS 导出接收端、资源并发与生命周期 |
| th10 MP `shell-touch` | MP 生产 Shell 的单人／多人配置、JS 导出接收端、资源并发与生命周期 |
| `touch-bomb-*` WASI wrappers | 各 production source owner 的旧 C++ Controller、输入过期与跨 hit 双击夹具 |
| `touch-native-invariants` | 产品目录矩阵、共享算法、已接入的原生消费者与破坏性反例 |
| `runtime-desktop` | 当前正式包的原生位置／消费者观察与新增 DOM Controller |

这些入口分别说明自身执行边界；其组件或 Runtime 证据不单独代表所有触控适配编号均已验收。

th09 的 relay125 冻结平滑度比较需要独立原始报告，使用作品入口 `npm --prefix ports/th09/web run test:smoothness-window-evidence` 并设置 `TH09_SMOOTHNESS_WINDOW_RAW`；执行时核对报告的固定 SHA-256，普通模块夹具不会合成这份证据。

源码接口、组件执行、浏览器接线和正式 Runtime 行为分别保留结果范围。CI 的汇总检查依据变更计划核对每个必跑任务成功，完整编译和真实游戏资源按对应测试组提供输入。
