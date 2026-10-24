# 各作源码检查

普通版与多人版通过[同步基线和历史](../../docs/adaptation/port-sync.md)追踪来源与接收进度。`node tools/ports/sync.mjs status` 查看待同步提交，`log <game>` 阅读处理历史，`merge <game> --from <commit>` 预览 Git 三方合并。

需要原生 ahead／behind 与分支合并时，使用 `project-history.py` 按原仓库来源投影目录历史。先在临时克隆验证内容树、祖先和同步往返；操作方法与已验证快照见上述文档。

`audit-history.py` 对照目录历史分支，识别整提交、文件与修改块补丁对应，生成[历史对账表](../../docs/history/port-sync-audit.md)；MP 来源比较覆盖普通版完整历史；当前内容证据结合路径迁移检查，固定快照决定明确已接收、同次维护、共用目录接替和待同步范围。MP 逐项来源与合并节点也进入固定快照决定；待同步范围须通过来源新增与目标缺失证据检查。快照变化时保留历史决定并标明需要复核。

命令先编译当前前端模块，再执行各项检查。

从根目录运行 `npm run ports:check -- --all`，或用 `--game=th08 --variant=both` 选择一作。安装依赖使用 `npm --prefix launcher ci --ignore-scripts`，原生时钟组件检查使用 C++17 编译器（默认 `g++`，可设置 `CXX`）。

`--write-status` 将实际结果写入 `docs/adaptation/status.json` 并生成各作 `ADAPTATION.md`。`--check-status` 检查当前源码身份是否与已登记结果一致。完整日志、源码输入与报告保存在 `artifacts/validation/ports/`，可用 `--output=DIR` 指定证据目录。

| 结果范围 | 执行内容 |
| --- | --- |
| `source-inputs` | 读取真实构建配方，核对文件存在、普通版／MP 归属与目录规则。Node 使用实际构建入口，CMake 使用条件声明并集。 |
| `source-contract` | 分别检查各源码所有者的协议、选项转发和 native 输入接口。 |
| `module` | 执行共用输入、触控、Package、存储等实际模块检查。 |

这些结果在适配表中标为“已实现”；源码归属检查标为“已验证”。实际编译使用 `npm run build:runtime -- --game=th08 --variant=normal --emsdk=/path/to/emsdk`，浏览器、设备和网络验收按[适配分项](../../docs/adaptation/README.md)执行并附记录。

新增检查在 `shared-checks.json` 声明共用测试的编号和适用模式；作品接口由 `runtime-contracts.mjs` 分目录检查。完整覆盖才能更新整行状态，部分覆盖使用 `complete: false`。检查范围、失败、源码内容与构建参数进入报告，源文件或检查方法变更会使旧源码身份过期。

目录规则检查 Git 当前跟踪或新增的源码，生成物和本地依赖继续由忽略规则隔离。退役目录、组件内 CI 和未登记的汇编／目标文件会使检查失败。当前目录职责见[源码布局](../../docs/architecture/source-layout.md)。

根 CI 在 Windows 与 Linux 执行源码和模块检查。源码构建按目录映射与配方生成矩阵，成对选择普通版和 MP；共用生产程序也分别登记模式。th06、th07 使用已有 CMake 构建任务，其余作品使用 Emscripten 6.0.9 和共用构建入口。CI 保存检查日志与构建来源记录。

已执行的构建和打包可在 `status.json` 登记 `build`／`package` 范围的结果，附命令、环境与产物 SHA-256。源码身份相同时，重新检查会保留这些执行记录并显示“已验证”；源码身份改变后重新验收。源码接口检查失败始终优先显示。

桌面实跑使用 `ports:runtime` 与 `ports:rooms`，必须显式选择游戏和模式／人数／通道。两个入口都读取同一份私有 Runtime 清单：每个目标带规范化 Runtime 包 SHA-256、仓库相对 `sourceRoot` 和本次构建 receipt。验收器复核 builder 记录的源码 SHA-256 与当前文件，并要求 receipt 的执行 SHA 与包内正式 WASM 一致；房间验收再要求 Launcher 当前代的全部服务字节与该包身份一致。缺少 DATA、receipt、目标或包身份都会使场景失败。加 `--gamepad` 可验证生产映射到正式 WASM 的同步取样，报告保留驱动、生产映射 bundle 和逐次动作记录。桌面模拟输入不代表实体设备验收。操作范围见[运行验收](../../docs/adaptation/validation.md)。

`sfx-queue-invariants.mjs` 复用同一浏览器核对直接音效、队尾延迟反例、受控未来帧增益和真实输出探针的重触发／静音／断连反例。输入会话携带探针实际返回的历史就绪回执，判定器复核会话、时钟、采样率与关闭边界。

CMake 依赖的 Git 固定版本与本地变更进入源码身份。干净依赖在已初始化和按需检出环境中使用相同身份；已检出的依赖提交必须与登记版本一致。
