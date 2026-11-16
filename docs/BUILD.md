# 源码快照与构建资料

本仓库首先用于保存最新开发源码。当前 Git 提交不附带原作资源、SDK、已构建 WASM 或完整网页运行产物，尚未在独立检出目录完成整套启动验收。最终交付另含匹配的 JS/WASM 网页 Runtime 和 TH12 同规范资源 ZIP/解压目录；资源包与 Runtime 各自校验和验收，见 [导入包交付要求](IMPORT_PACKAGE.md)。

## 原生核心

private108使用Emscripten 4.0.22，case658首次完整链接通过；case659实际加载产物，1个Factory、1个World、181次tick验证普通Perfect的6个有序声音请求，7项检查通过，另检查205个导出均为函数。657另通过13项完整生产调用块与真实Host组件检查及完整WorldExports语法检查。`build/private108.rsp`来自首次成功参数的相对路径转换，输出为`build/local/core.mjs`。private100/101/102/103/104/105/106/107配方保留为历史资料；相同参数不代表相同源码或产物。

它保留 83 个翻译单元、205 个导出符号、1 MiB 栈、64 MiB 初始内存、C++20、异常与浮点选项。源文件摘要记录在 `docs/native-source-manifest.json`。

后续开发者在独立环境配置对应 Emscripten 后，构建配方为：

```powershell
New-Item -ItemType Directory -Force build/local
emcc '@build/private108.rsp'
```

本源码检出目录中的相对路径配方完成了文件与参数核对；private108的实际构建在隔离验收目录完成。此前private100的浏览器检查作为历史证据保留，不能视为108已完成浏览器验收。构建输出不会自动发布到浏览器，也不承诺跨机器生成完全相同的二进制摘要。

`tools/build-runtime.mjs` 是历史脚本，保留作研究参考；其导出清单和发布逻辑早于本快照，不作为private108的构建入口。

## 原作资源

资源准备工具需要 Python 3.11+、自行持有的原作安装和官方 thtk Windows release 12。示例路径需要替换为自己的安装位置：

```powershell
python -X utf8 tools/prepare.py --original '你的原作安装目录' --thtk '你的thtk工具目录' --variant both
```

资源在本地 `local/` 下生成，该目录已经被 Git 忽略。原版存档、成绩、录像和个人配置不属于此仓库内容。

## 浏览器宿主与测试

`site/` 包含最新前端候选与EX控制器；原生菜单、持久化和EX前段已分别在隔离浏览器中验证。`tools/serve.py` 已补齐新增模块的显式路由并通过现有7项服务器测试。完整终局与独立检出目录的一键部署流程仍待验收。

`tests/` 为保留的测试源码。部分测试依赖本地原版资源、未分发的私有黄金记录、编译器或运行时。缺失依赖导致的跳过不等于测试通过。

原有六线和EX成功证据保持不变。新增测试针对新缺陷或尚未覆盖的范围；不为重复获得通过结论重跑成功路线。`tests/core101-abi-noowner.mjs`是615实际执行的源文件，显式参数为包含已验收`result.json`、`request.json`和`run/runtime/`的本地核心目录。运行产物及这些本地准入资料不随仓库分发；新测试应使用新目的和受控输出，禁止覆写历史成功目录或重新构建已成功的93至108。新增录像字段组件和零帧实际检查位于`tests/replay-graze/`，完整范围见`docs/native-replay-graze621-627.md`。

新增回归源码：

- `tests/native/player-hit-long-session.cpp`：真实PlayerHitHost正常计时65754帧后受击，另含数值与坐标边界；运行参数为本地原始pl00.anm。
- `tests/native/ice-item-motivation-notice.cpp`：真实ItemHost与PerfectHud的拾取通知测试；先用 `node tests/extract-item-collection.mjs <隔离输出目录>` 提取当前完整生产回调，运行参数为本地front.anm和bullet.anm。
- `tests/native-extra-run-lifecycle.mjs`：生产EX控制器的顺序、取消、渲染失败及严格收据测试；通过 `TH128_ACCEPTANCE_ROOT` 指向已验收的本地case目录，以只读复用541档案和对应元数据。此测试的native与device接口明确为模拟实现。

新增ECL428分派组件源码位于`tests/ecl428/`，明确使用合成ECL及效果回调，完整生产StageWorld/EclVm与原有校验真实执行。`tests/core103-abi-noowner.mjs`为633实际执行源码，只检查已产生的核心接口与无会话拒绝；完整范围见`docs/native-ecl428630-633.md`。

阶段奖励及兼容性源码在`tests/spell-stage-bonus/`，使用合成ECL和效果回调，完整生产StageWorld/EclVm真实执行。实际639源码在`tests/core104-abi-noowner.mjs`，范围见`docs/native-spell-stage635-639.md`；不得用接口烟测替代原作整局或设备验收。

符卡mode组件及真实WASM测试源码位于`tests/spell-live-mode/`。642为实际core104首RED，643为组件GREEN，645为实际core105正向及phase180边界；完整本地准入request/资源不随仓库分发。测试使用合成ECL并明确模拟输出/收据，不代表原作整局、完整录像或设备验收；详见`docs/native-spell-mode640-645.md`。

受击蓄力声音停止请求的当前测试在`tests/held-window-sound/`：`fixture.cpp`为647/648实际执行源码；`player-hit-host-regression.cpp`为649在新生产源码上执行的完整Host回归；`tests/core106-abi-noowner.mjs`为651实际产物检查。旧`tests/native/player-hit-host.cpp`是105单停止请求契约的历史源码和649的固定输入，保留原字节；验证106应使用上述31/70新契约版本，不能据此声称旧测试源码已原样通过106。详见`docs/native-held-sound646-651.md`。

普通Perfect held门槛测试位于`tests/perfect-held-gate/`：`fixture.cpp`为653执行的44项组件源码；`actual-held-matrix.cjs`为655执行的真实核心输入矩阵源码。它们依赖未随仓库分发的本地准入记录、资源和产物，不应覆写历史成功目录。旧`tests/native/perfect-freeze-host.cpp`保留早期fresh-press/服务契约，未声称原样通过107；本轮当前门槛回归使用新目录。详情见`docs/native-perfect-held652-655.md`。

普通Perfect激活声音修复的当前测试位于`tests/perfect-activation-sound/`：fixture与实际生产块提取文件为657执行源码，actual-phase-requests.cjs为656/659相同测试源码。组件使用真实Host与明确模拟的World/Hit/Ice/HUD/请求容器，实际WASM使用原资源及合成ECL/输出/收据边界。完整本地准入记录不随仓库分发；历史成功目录不得覆写或重跑，详见`docs/native-perfect-sound656-659.md`。
