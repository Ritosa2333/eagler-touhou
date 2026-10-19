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

先运行 `npm --prefix launcher ci --ignore-scripts`、`npm --prefix services/relay ci --ignore-scripts` 和 `npm --prefix ports/th09/web ci --ignore-scripts`，再执行 `python -m pip install -r launcher/requirements-ci.txt` 安装固定版本的图像与 CMake 工具。浏览器依赖按 [Launcher 测试环境](../launcher/tests/README.md#浏览器环境)安装。统一执行器在 Windows 和 Linux 管理完整的子进程生命周期：Windows 使用 Job Object，Linux 使用 child subreaper。驱动正常退出后遗留进程也会导致失败。源码检查使用 Node.js 22.18 以上、Python 3 和 C++17 编译器。Launcher 的 staging 夹具与 common 原生测试使用 CMake，common 还使用 CTest。路径可通过 `EAGLER_TEST_CMAKE` 和 `EAGLER_TEST_CTEST` 指定。

浏览器 Shell 回归执行真实 Shell 的初始化、监听器与 Launcher 改键路径。Runtime 验收加载带文件哈希清单的正式包，并从原生运行状态观察输入、帧推进和生命周期。

共用 protocol-host 使用独立 localhost HTTPS 站点，并校验浏览器实际收到的响应字节。需要 OpenSSL，`EAGLER_OPENSSL` 可指定路径。测试浏览器按本次临时证书的 SPKI 指纹连接；站点退出后清理证书和连接。

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
