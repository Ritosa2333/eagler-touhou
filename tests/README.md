# 测试

从仓库根目录运行。测试源码由各组件维护，`tests/suites.mjs` 统一登记执行入口、范围、资源和时间预算。功能怎样判定、观察什么以及新作品如何接入，见[验收方法](../docs/adaptation/test-methods.md)。

| 入口 | 内容 |
| --- | --- |
| `npm test` | 工具、Launcher 核心与三个服务的快速回归 |
| `npm run test:integration` | 快速回归、React、逐作公共夹具、15 变体浏览器接线与源码检查、common 原生测试 |
| `npm run test:acceptance` | 当前打包 Runtime、存储与重开、多人房间和断线行为 |
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

## 环境

先运行 `npm --prefix launcher ci --ignore-scripts`、`npm --prefix services/relay ci --ignore-scripts` 和 `npm --prefix ports/th09/web ci --ignore-scripts`，再执行 `python -m pip install -r launcher/requirements-ci.txt` 安装固定版本的图像与 CMake 工具。浏览器依赖按 [Launcher 测试环境](../launcher/tests/README.md#浏览器环境)安装。统一执行器在 Windows 和 Linux 管理完整的子进程生命周期：Windows 使用 Job Object，Linux 使用 child subreaper。驱动正常退出后遗留进程也会导致失败。源码检查使用 Node.js 22.18 以上、Python 3 和 C++17 编译器。Launcher 的 staging 夹具与 common 原生测试使用 CMake，common 还使用 CTest。路径可通过 `EAGLER_TEST_CMAKE` 和 `EAGLER_TEST_CTEST` 指定。

浏览器 Shell 回归执行真实 Shell 的初始化、监听器与 Launcher 改键路径。Runtime 验收加载带文件哈希清单的正式包，并从原生运行状态观察输入、帧推进和生命周期。

## Runtime 验收输入

`EAGLER_TEST_RUNTIME_MANIFEST` 指向本地清单，逐项登记八作普通版的 Runtime 目录、入口和 DATA，结构见[运行验收](../docs/adaptation/validation.md)。多人验收通过 `EAGLER_TEST_LAUNCHER_URL` 指定本地 Launcher；`EAGLER_TEST_PACKAGES` 可提供作品到本地导入包的映射。

普通版逐作执行，多人按产品目录生成支持的 2P／3P 和 RTC／Relay 组合。输入矩阵必须完整，清单中的产品身份、文件哈希和实际启动目录逐项核对。资源、字体、SDK 和结果保存在本地目录。

## 原版对照输入

各作 `docs/DEVELOPMENT.md` 列出原版程序、DATA、字体、回放与固定轨迹的准备方法。先构建本作 `cpp:build` 对照核心，再执行 `cpp:test` 或根 `test:oracles`。五个源码目录共用 `tools/testing/frozen-oracle.mjs`，执行前后核对核心、编译来源与完整用例清单。选中用例必须全部执行成功，跳过、取消或零用例均使结果失败。每组默认使用两个 Node 测试进程，在本作 `artifacts/cpp/verification/` 保存逐项结果与日志。

## 新增检查

工具回归以 `tools/**/名称.test.mjs` 登记，`tooling` 自动发现并运行。Launcher 核心与工作区检查进入其 `test-plan.mjs`，React 与辅助器检查进入对应测试目录。新增组件执行入口登记到 `suites.mjs`，说明输入、可观测结果和时间预算，并用故意破坏行为的反例确认失败路径。源码状态检查读取 th06/th07 配方声明的实际 CMake 依赖；运行 `node launcher/scripts/prepare-runtime-dependencies.mjs --game=th06,th07 --variant=normal` 后再执行 `npm run ports:check -- --all --check-status`。准备命令使用合成配置的单元测试不访问真实子模块。

源码接口、组件执行、浏览器接线和正式 Runtime 行为分别保留结果范围。CI 的汇总检查依据变更计划核对每个必跑任务成功，完整编译和真实游戏资源按对应测试组提供输入。
