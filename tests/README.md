# 测试

从仓库根目录运行。测试源码由各组件维护，`tests/suites.mjs` 统一登记执行入口、范围、资源和时间预算。功能怎样判定、观察什么以及新作品如何接入，见[验收方法](../docs/adaptation/test-methods.md)。

| 入口 | 内容 |
| --- | --- |
| `npm test` | 工具、Launcher 核心与三个服务的快速回归 |
| `npm run test:integration` | 快速回归、React、逐作公共夹具、15 变体浏览器接线与源码检查、common 原生测试 |
| `npm run test:acceptance` | 当前打包 Runtime、键盘与触控、练习、Replay、音乐 PCM、离线存储、多人生命周期与旁观 |
| `npm run test:oracles` | th09、th11、th15 及对应独立 MP 源码的原版／原生对照套件 |
| `npm run test:list` | 各组的实际执行范围 |

选择一组并指定记录目录：

```sh
node tests/run.mjs --suite=tooling --output=artifacts/validation/my-check
node tests/run.mjs --suite=launcher-core,launcher-support
```

执行器为每项启动独立进程。共享源码、构建目录或浏览器的任务通过资源锁安排；配置、编译和执行按依赖顺序运行。超时会终止该任务的进程树。失败、取消、缺输入或空选择均返回非零状态。

各组按选择顺序运行，`--jobs` 控制组内任务并发。React 测试按文件隔离，最多两个进程；冷构建测试共用排他资源。前端模块在测试前准备一次，临时工作区各自创建和释放。

`ports-unit` 的 th06／th07 合作规则任务从源码提取选定函数体，在最小桩环境中编译执行，检查资源捐赠阈值、资源变化、复活和道具生成数量。使用 Python 3 与 C++20 编译器，`CXX` 可指定编译器可执行文件。

th15 MP 断线回归调用生产 bridge 与共享 calibration，分别核对活跃连接中断后的错误冻结和原生已退休会话的正常收尾。

th06／th07 的 Replay 源码契约随 `ports-unit` 执行，核对输入注入顺序、状态恢复与呈现步进接线。逐帧播放由独立的 Replay 验收记录。

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

该输入绑定 common configure/build/test、native-gamepad、ports-source 和 th06／th07 cooperative rules 任务，缺省为空。清单用于不写测试产物的编译器辅助进程；路径或哈希不符、其他后代残留都会使测试失败。此清理方式由 Windows Job Object 提供，Linux 收到清单时明确报错。common 原生任务设置 `MSBUILDDISABLENODEREUSE=1`，让构建节点随任务结束。

浏览器 Shell 回归执行真实 Shell 的初始化、监听器与 Launcher 改键路径。Runtime 验收加载带文件哈希清单的正式包，并从原生运行状态观察输入、帧推进和生命周期。

共用 protocol-host 使用独立 localhost HTTPS 站点，并校验浏览器实际收到的响应字节。需要 OpenSSL，`EAGLER_OPENSSL` 可指定路径。测试浏览器按本次临时证书的 SPKI 指纹连接；站点退出后清理证书和连接。

## 逐作适配队列

STG 的接入顺序、作品接口和单作输入准备见[STG 适配流程](../docs/adaptation/stg.md)。

`npm run test:adaptation` 按作品、普通版／MP 和场景组生成队列，执行过程与既有测试共用进程管理器。先预览同一计划，再按需执行：

```sh
npm run test:adaptation -- --game=th08 --variant=both --suite=runtime,replay --plan
npm run test:adaptation -- --game=th08 --variant=normal --suite=runtime,desktop-input --inputs=.local/adaptation-inputs.json
npm run test:adaptation -- --game=th08 --variant=normal --check=INPUT-01 --inputs=.local/adaptation-inputs.json
npm run test:adaptation -- --resume=artifacts/validation/上次目录
npm run test:adaptation -- --rerun-failed=artifacts/validation/上次目录
```

`--variant=normal|multiplayer|both` 选择产品身份，`mp` 是 `multiplayer` 的别名。`--suite` 可选 `runtime`、`desktop-input`、`desktop-background`、`practice`、`replay`、`music`、`offline`、`offline-update`、`rooms`、`spectator`。省略时生成全部场景。`--check` 选择适配编号；编号存在但没有对应场景时记录 unmapped，并返回未完成。显式选择编号要求对应完整证据，场景通过但编号覆盖 partial 时返回未完成。

任务边界为作品 | 变体 | 场景组；房间再按人数与 RTC／Relay 拆分。输入和后台分别运行，支持更新的作品把基础离线与更新旅程合在一次任务里。依赖本地 Launcher 构建的任务声明 `requiresLauncherBuild`，整轮共享一次准备，作品打包复用内容缓存。准备失败时保留失败报告，并阻断依赖它的任务；独立 Runtime 任务继续执行。`--jobs=1` 控制排队并发，浏览器与共享资源按互斥声明运行。单项缺输入会记录 blocked，其他独立项继续。

新作品随产品目录进入计划，原生观察与作品流程尚未登记时显示 blocked，并给出接入位置。th075 的正式构建和组件测试沿用本作入口，通用行为驱动在接入观察与流程后启用。

本地输入配置采用 `eagler-touhou/adaptation-inputs/1`，可登记 `runtimeManifest`、`packages`、`updatePackages`、`musicManifest`、`artwork` 和 `launcherUrl`。前五项的相对路径以配置文件目录为基准；也可继续使用下文的环境变量。配置、私有资源与原始报告留在本地。

每轮默认创建新目录，`--output` 可指定新的证据目录。`summary.json` 包含计划、实际命令、逐项状态、日志、原始封存报告的哈希、产物与输入身份，以及逐编号覆盖。`--plan` 保存同样的任务选择与缺失输入，不启动测试，报告为 planned。执行状态与编号完整度分别呈现：全部所选场景通过可返回 passed，完整适配证明由 `coverage.complete` 和逐编号记录给出。

离线报告的 `scenarios` 列出本次实际选择的基础和更新旅程。未选择更新时，其适配要求保留在排除范围与逐编号记录中；显式选择更新但缺少第二代包时，队列记录 blocked/incomplete。基础场景及其完整编号证明独立判定。

普通版离线场景与指纹选择同一份 normal Runtime，输入清单可以只含该目标。报告组装和封存统一经过 `runOfflineRuntimeReport`，浏览器旅程完成并收齐最终错误审计后生成场景结果。组件回归使用真实报告写入、封存文件读取和队列判定，检查未选更新、已选更新失败及晚到错误；实际离线玩法沿用正式浏览器场景验收。

离线驱动通过 Launcher 界面安装 ZIP，展开高级设置后从可见音乐菜单选择无 BGM；包未提供音乐选择时使用界面默认值，并记录实际模式。只读 IndexedDB 观察器核对实际代际与 DATA 字节，数据库版本、表名和实际注入源码进入证据身份；数据库缺失或版本不符时读取失败，观察过程保持已有数据原值。此路径直接检查正式打包站点的持久化结果。

`--resume` 沿用身份一致且封存证据完整的已完成任务，执行未完成或身份变化的任务。`--rerun-failed` 额外重跑 failed、timeout 和 cancelled；已记录的 incomplete 覆盖范围保留原证据，补充输入或修改驱动后由身份变化触发重跑。沿用结果标为 retained，保留原运行编号、时间和报告路径，本次执行标为 executed。父报告、子报告、测试依赖、Runtime、DATA、录像、字体、音乐、离线包及浏览器身份共同约束复用；文档正文变化不触发游戏重跑。执行后独立复核身份。

## Runtime 验收输入

`EAGLER_TEST_RUNTIME_MANIFEST` 指向本地清单，逐项登记八作普通版的 Runtime 目录、入口和 DATA，结构见[运行验收](../docs/adaptation/validation.md)。多人验收通过 `EAGLER_TEST_LAUNCHER_URL` 指定本地 Launcher；`EAGLER_TEST_PACKAGES` 可提供作品到本地导入包的映射。

`runtime-music` 由 acceptance profile 调度，要求 `EAGLER_TEST_MUSIC_MANIFEST`，并固定选择 th08、th10、th11、th15、th20。清单采用 `eagler-touhou/music-inputs/1`，逐作登记私有来源目录、产品目录 mount 和每个 OGG 文件的名称、绝对路径、字节数及 SHA-256；任何已选作品或文件缺失都会使验收失败。

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

各作 `docs/DEVELOPMENT.md` 列出原版程序、DATA、字体、回放与固定轨迹的准备方法。先构建本作 `cpp:build` 对照核心，再执行 `cpp:test` 或根 `test:oracles`。五个源码目录共用 `tools/testing/frozen-oracle.mjs`，执行前后核对核心、编译来源与完整用例清单。选中用例必须全部执行成功，跳过、取消或零用例均使结果失败。每组默认使用两个 Node 测试进程，在本作 `artifacts/cpp/verification/` 保存逐项结果与日志。

## 新增检查

工具回归以 `tools/**/名称.test.mjs` 登记，`tooling` 自动发现并运行。Launcher 核心与工作区检查进入其 `test-plan.mjs`，React 与辅助器检查进入对应测试目录。新增组件执行入口登记到 `suites.mjs`，说明输入、可观测结果和时间预算，并用故意破坏行为的反例确认失败路径。源码状态检查读取 th06/th07 配方声明的实际 CMake 依赖；运行 `node launcher/scripts/prepare-runtime-dependencies.mjs --game=th06,th07 --variant=normal` 后再执行 `npm run ports:check -- --all --check-status`。准备命令使用合成配置的单元测试不访问真实子模块。

源码接口、组件执行、浏览器接线和正式 Runtime 行为分别保留结果范围。CI 的汇总检查依据变更计划核对每个必跑任务成功，完整编译和真实游戏资源按对应测试组提供输入。
