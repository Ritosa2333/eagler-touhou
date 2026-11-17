# TH128 — 妖精大战争行为还原与 Web 移植

《妖精大戦争 ～ 東方三月精》独立行为还原工程，使用 C++/WebAssembly 核心与 JavaScript 浏览器宿主。

本仓库保存截至 **2026-11-17** 的开发源码。第三步的协议、高刷新率、触控、多语言、thprac、Replay六项已按约定功能范围完成；最新本地交付为 **v8 / core116**，含真实TH128 thprac与原版界面。六条普通路线＋EX保持已接受的单人功能基线。多人原型保留、后续开发与公开发布仍暂停。见[最新本地交付](docs/RUNTIME116_V8.md)。

## 当前进度

已完成范围与兼容性边界见[WEB_FUNCTIONAL_SCOPE](docs/WEB_FUNCTIONAL_SCOPE.md)。真实TH128菜单、126章节/156选段/EX阶段、五资源、Backspace/F1–F7、F12、练习录像参数/存储/playLock、取消重开和实际正式页原资源游戏已验收，见[最终验收](docs/THPRAC128_ACCEPTANCE.md)。旧七项简易选关只保留历史证据。

用户已明确要求按 eagler-touhou 和上游 TH128 专用 thprac 实现对接，见[纠正与计划](docs/THPRAC_CORRECTION.md)。

多人部分已有[规则握手](docs/MULTIPLAYER_RULES.md)、[有界输入账本](docs/MULTIPLAYER_INPUT.md)和[双方确认重开](docs/MULTIPLAYER_RESTART.md)的真实协议组件，尚未交付可玩的网络对局、完整Replay/旁观或native World回滚。旧核心与已通过路线不因阶段推进而重复构建或验收。

下列v5/v4内容是历史性能交付记录，当前源码、核心与阶段以本节和链接中的最新记录为准。

历史v5交付为[v5 / core109](docs/performance-v5.md)：碎冰原生更新峰值约91→8.5ms，密集弹幕完整状态读取约16.8→2.2ms，并修复漏交反弹坐标；选定同输入状态、RNG和全部绘制记录保持一致。最终94项针对性回归与组装/HTTP通过；这些CPU计时不等于设备FPS。v4.1启动修复保持，原93.3MB资源包继续使用。

历史v4网页交付为[v4性能修复版](docs/performance-v4.md)，针对用户实际14FPS回退减少重复场景序列化、补帧快照和背景绘制提交，并修复背景地块插值相位错位。最终源码和Runtime组装/HTTP共79项检查通过；同1024原生弹幕快照读取约36→12ms为CPU对比，实际设备FPS另验。核心108及93.3MB资源包不变。

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
| 单人还原与移植功能基线 | 六条普通路线＋EX 完整流程已完成 |
| Web功能与资源入口 | thprac菜单/选段/初始资源及练习Replay已接新114，辅助工具/速度仍待；旧112/v7与93.3MB DAT＋OGG资源v2保持，部署和重新打包暂停 |

六线及 EX 的 CPU 测试覆盖实际 World/ECL/MSG 执行、输入记录、关卡交接及原生对象生命周期。本文的 **THPrac 式练习辅助**指由 WASM 原生接口提供的持续免伤等测试能力；未运行 Windows THPrac，也未实施进程注入。早期 CPU 测试的存储、音频和渲染为模拟实现；新增浏览器验收分别记录真实 IndexedDB、WebGL 页面与 WebAudio 运行状态，不将前段检查视为整局验收。

六线及 EX 的 CPU 验收见 [docs/acceptance-20261115.json](docs/acceptance-20261115.json)，新增修复和浏览器证据见 [docs/completion-checkpoint-20261115.json](docs/completion-checkpoint-20261115.json)，剩余任务见 [docs/STATUS.md](docs/STATUS.md)。

还原、移植、Web适配与多人联机分别记录，阶段职责和能力清单见 [ADAPTATION.md](ADAPTATION.md)。已完成阶段的证据直接复用，不因后续适配而重复验收。

最终交付还包括与 TH12 示例相同导入规范的 TH128 游戏资源 ZIP 及解压目录，详见 [导入包交付要求](docs/IMPORT_PACKAGE.md)。当前资源包按用户指定TH15布局使用原 th128.dat＋music/ogg/*.ogg，12个资源文件共93.3MB；已封存匹配core112/v7的Runtime与资源解压目录，本轮不重新打包。本地导入启动与验证边界见[网页交付](docs/WEB_DELIVERY.md)。源码仓库与游戏资源包分别交付。

## 源码内容

- `src/`：当前private112对应的C++核心源码；已成功核心保持不变。
- `site/`：最新前端候选，包含 EX 记录、呈现、严格存储与终局交接控制器。
- `tools/`：资源准备、研究和开发工具源码。
- `tests/`：历史测试源码；部分测试依赖未随仓库分发的本地资源、黄金记录或编译产物。
- `build/private112.rsp`：当前成功核心的相对路径构建配方；旧版配方作为历史资料保留。
- `docs/native-source-manifest.json`：原生文件摘要、编译清单和参考产物摘要。
- `docs/research/`：历史研究说明，其当时进度不替代本页当前状态。
- `licenses/`：保留的第三方许可文本。

当前 Git 源码快照不随提交附带原作 EXE/DAT、完整游戏贴图与音乐、录像、个人存档、浏览器用户数据、工具链及生成的 WASM。最终网页交付仍包括匹配的 JS/WASM Runtime，以及同 TH12 导入规范的游戏资源 ZIP 和解压目录；参考 TH12 的资源 ZIP 本身不带运行核心。详见 [资源包交付要求](docs/IMPORT_PACKAGE.md) 与 [构建说明](docs/BUILD.md)。

## 开发与验证

参考工具链为 Emscripten 4.0.22、Node.js 22+ 和 Python 3.11+。此前private102在录像字段修复、13项组件检查和3个完整生产翻译单元检查后首次构建成功；实际WASM初始化1个World且不运行游戏帧，恢复2529次擦弹并拒绝不一致的绑定包，205个导出接口检查通过。该检查不代表完整录像兼容通过。详见[621–627验证记录](docs/native-replay-graze621-627.md)。此前成功核心保持不变，没有重复执行六线或EX完整路线。

独立检出环境的完整构建与部署仍待验证，运行时产物及原作资源需另行准备。`tools/build-runtime.mjs` 为保留的历史构建脚本，当前构建参数以 `build/private112.rsp` 为准。

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
- [AgentMystia/th13](https://github.com/AgentMystia/th13)：新增参考其源码/资源分离和启动器适配记录方式；目录与具体宿主实现不同，不代表代码或兼容程度相同。

该仓库尚未向上游总进度表提交状态变更。
