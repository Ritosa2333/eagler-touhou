# TH128 — 妖精大战争行为还原与 Web 移植

《妖精大戦争 ～ 東方三月精》独立行为还原工程，使用 C++/WebAssembly 核心与 JavaScript 浏览器宿主。

本仓库保存截至 **2026-11-16** 的开发源码快照。原生核心对应private108，恢复普通Perfect激活时遗漏的46、51号播放请求；13项生产调用组件检查、首次完整链接和实际核心181帧声音请求轨迹通过。此前holding门槛及受击声音31/70修复保持。续关后完整终局、浏览器端完整流程与兼容性验收仍在进行中。

## 当前进度

| 范围 | 状态 |
|---|---|
| 六条普通路线 A1/A2/B1/B2/C1/C2 | CPU 端流程验收通过（THPrac 式练习辅助） |
| 六线通关资格累计与 EX 解锁 | 已通过完整档案累计及资格校验 |
| EX 终局、成绩、姓名、录像、返回主菜单 | CPU 端流程验收通过（THPrac 式练习辅助） |
| 浏览器录像持久存储 | 25 槽完整读回、并发冲突、覆盖及重新打开验证通过 |
| 浏览器菜单与 EX 入口 | 五个菜单页已实际显示；真实六线档案解锁 EX，前段运行通过 |
| 浏览器完整终局、音频与 GPU 一致性、焦点切换 | 待完成集成验收 |
| 失败及续关分支 | 自然失败、一次续关及失败成绩已验证；续关后完整终局待验 |
| 原版指定录像完整 23707 帧 | 实际回放4906帧后提前GameOver；兼容性差异待定位 |
| 原生暂停/录像保存对象重置后重开 | 629实际核心检查通过；完整保存及浏览器交接待验 |
| 整体还原、整体移植、Web 发布 | 开发中 |

六线及 EX 的 CPU 测试覆盖实际 World/ECL/MSG 执行、输入记录、关卡交接及原生对象生命周期。本文的 **THPrac 式练习辅助**指由 WASM 原生接口提供的持续免伤等测试能力；未运行 Windows THPrac，也未实施进程注入。早期 CPU 测试的存储、音频和渲染为模拟实现；新增浏览器验收分别记录真实 IndexedDB、WebGL 页面与 WebAudio 运行状态，不将前段检查视为整局验收。

六线及 EX 的 CPU 验收见 [docs/acceptance-20261115.json](docs/acceptance-20261115.json)，新增修复和浏览器证据见 [docs/completion-checkpoint-20261115.json](docs/completion-checkpoint-20261115.json)，剩余任务见 [docs/STATUS.md](docs/STATUS.md)。

还原、移植、Web适配与多人联机分别记录，阶段职责和能力清单见 [ADAPTATION.md](ADAPTATION.md)。已完成阶段的证据直接复用，不因后续适配而重复验收。

最终交付还包括与 TH12 示例相同导入规范的 TH128 游戏资源 ZIP 及解压目录，详见 [导入包交付要求](docs/IMPORT_PACKAGE.md)。当前尚未生成或验收正式导入包；源码仓库与游戏资源包分别交付。

## 源码内容

- `src/`：private108 的 226 个原生源文件，编译清单含 83 个翻译单元、205 个导出符号。
- `site/`：最新前端候选，包含 EX 记录、呈现、严格存储与终局交接控制器。
- `tools/`：资源准备、研究和开发工具源码。
- `tests/`：历史测试源码；部分测试依赖未随仓库分发的本地资源、黄金记录或编译产物。
- `build/private108.rsp`：从首次成功构建记录转换出的相对路径编译参数；private100/101/102/103/104/105/106/107配方作为历史资料保留。
- `docs/native-source-manifest.json`：原生文件摘要、编译清单和参考产物摘要。
- `docs/research/`：历史研究说明，其当时进度不替代本页当前状态。
- `licenses/`：保留的第三方许可文本。

当前 Git 源码快照不随提交附带原作 EXE/DAT、完整游戏贴图与音乐、录像、个人存档、浏览器用户数据、工具链及生成的 WASM。最终网页交付仍包括匹配的 JS/WASM Runtime，以及同 TH12 导入规范的游戏资源 ZIP 和解压目录；参考 TH12 的资源 ZIP 本身不带运行核心。详见 [资源包交付要求](docs/IMPORT_PACKAGE.md) 与 [构建说明](docs/BUILD.md)。

## 开发与验证

参考工具链为 Emscripten 4.0.22、Node.js 22+ 和 Python 3.11+。此前private102在录像字段修复、13项组件检查和3个完整生产翻译单元检查后首次构建成功；实际WASM初始化1个World且不运行游戏帧，恢复2529次擦弹并拒绝不一致的绑定包，205个导出接口检查通过。该检查不代表完整录像兼容通过。详见[621–627验证记录](docs/native-replay-graze621-627.md)。此前成功核心保持不变，没有重复执行六线或EX完整路线。

独立检出环境的完整构建与部署仍待验证，运行时产物及原作资源需另行准备。`tools/build-runtime.mjs` 为保留的历史构建脚本，当前构建参数以 `build/private108.rsp` 为准。

private103新增的ECL 428缺失分派修复见[630–633记录](docs/native-ecl428630-633.md)：36项完整StageWorld组件检查使用明确的合成ECL与效果边界；实际产物633检查不启动游戏World，也不代表617原版录像问题已解决。

private104修复第二、第三关符卡奖励基数，范围和合成边界见[635–639记录](docs/native-spell-stage635-639.md)。已通过路线不重跑，原版完整录像问题仍待定位。

private105 的实际验证使用2个World、183次tick：X激活时符卡标志包含0x40，phase180随mode清零而清除。ECL脚本、音频输出和收据存储边界为明确的合成测试，未运行原作整局；范围见[640–645记录](docs/native-spell-mode640-645.md)。

private106 恢复受击后停止蓄力声音时遗漏的70号请求，保留31→70顺序与失败保护。验证范围为完整生产PlayerHitHost组件和产物ABI，音效输出接缝模拟；原版完整state4、死炸和实际听感仍待验，详见[646–651记录](docs/native-held-sound646-651.md)。

private107移除普通Perfect激活额外要求“重新按下X”的条件：实际1个Factory、4个World、11次tick验证无按键、只有pressed、只有holding、holding与pressed同时存在四种组合，继续按住3帧没有再次消费。原资源和原生World真实执行，180字节ECL、音频输出及收据存储明确模拟；完整phase、死炸和617录像问题仍待验，见[652–655记录](docs/native-perfect-held652-655.md)。

private108只补普通Perfect成功激活时的46→51有序播放请求，保留原有phase120的47/69播放、51停止及phase180的71播放。实际1Factory/1World/181tick验证6个请求的编号、顺序、操作与非零坐标，7项检查通过；音频输出和收据存储模拟，不代表物理听感或完整Perfect时序等价，详见[656–659记录](docs/native-perfect-sound656-659.md)。

## 来源与许可

原作的程序、角色、美术、文字和音乐归各原权利人。工程中保留了所适配组件的 MIT 许可；项目自有代码的整体许可证尚未指定。详细来源和适用范围见 [NOTICE.md](NOTICE.md)。

参考工程：

- [Tweet-META/th12](https://github.com/Tweet-META/th12)
- [Ritosa2333/eagler-touhou](https://github.com/Ritosa2333/eagler-touhou)

该仓库尚未向上游总进度表提交状态变更。
