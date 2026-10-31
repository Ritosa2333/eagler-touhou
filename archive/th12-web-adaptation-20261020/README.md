# th12 旧网页前端接入记录

归档范围为 2026-10-20 的旧网页前端接入。正式 SDL3 / GLES 应用见[开发指南](../../ports/th12/docs/DEVELOPMENT.md)。普通版与 MP 原有文档内容相同，此处合并保存。

还原分支 `experiments` 保留在 `492eddf16a3d9389cb529bedf06555b2cafcabe5`。
用户此前对该还原基线的确认，不覆盖本轮新增的 Web 适配和设备表现。
适配工作从 canonical `b40dfb5c55bec2ebbfa4bdcabdb877a5f9500f27` 出发；
高刷新率、THPrac 和语言分别在独立实验工作区开发，当前实现集成于 `eagler` 分支。
`eagler` 从原 `main` 的 `32af3d9c9d6dc5ffe83f9f3a7c3b2b89cbd1a0c0` 建立，
承接真实 Launcher 接入及后续 Web 适配。`main` 与 `experiments` 保留原有提交，
本次分支整理不重写其历史，也不将已经进入 `main` 的早期适配提交倒退删除。
这些工作区和基线用于追溯来源，不代表实验已经完成正式推广或人工验收。

本轮已编译包含 THPrac 的完整 C++ 核心和独立 Vorbis 解码器。按用户要求，
没有运行游戏、Oracle、浏览器试玩、语法检查或测试。编译成功不等于运行和听感验收。
代码包继续声明 `profile: integration-in-progress`、`completeAdapter: false`。

## 本轮契约修正与剩余门槛

启动器 Replay 工作区、IDBFS 存档兼容、blur 行为、单指取消、诊断构建隔离和
自托管准备器依赖已补入源码。quick/daily 从固定未完成占位入口改为逐 tick 的流式
采样、不可变 golden 绑定和首个差异报告；候选状态、输入路线和终局规则必须完整。
它们不会自动运行原版或将候选输出当成期望值。

完整原版 gameplay provider、经审阅的整局 golden／终局证据仍未提供；现有原版
RNG 和解密探针不能替代它们。运行、设备、存储往返、正式 Runtime Release、
Hosted／External／Import 和公开部署验收也未执行。因此本轮修正不能表述为
“完全符合适配规范”或“quick/daily 已通过”。详见
[验证器说明](../../ports/th12/tools/replay-verifier/README.md)。

## 已接入的六项能力

| 项目 | 当前源码中的实现 | 尚需人工确认的边界 |
| --- | --- | --- |
| 输入与触控 | 启动器键盘/手柄映射；四种触控移动模式、低速方式、双击 Bomb、菜单手势；在玩家逻辑步内消费已解析的位移；原判定点可常显。 | 手机手势归属、失焦释放、暂停/死亡/对话切换及不同灵敏度。`focusHitboxEnabled` 判定点增强仍不支持，开启时明确拒绝。 |
| 高刷新率展示 | 固定 60 Hz 逻辑与展示分离；关闭 60 帧展示上限后，在 120 Hz 等显示环境插值展示；连续性由实体/动画代际约束。 | 高刷新率下弹幕、动画、镜头、文字与菜单观感；展示不得额外推进输入、碰撞、ECL、RNG 或动画逻辑。THPrac 主动调整逻辑速度是独立功能。 |
| THPrac | 原 Practice 位置进入真实参数菜单；71 分段、43 章节、符卡阶段、ECL 事务补丁、初始资源、F12/Backspace/Tab 工具与触控面板；真实资源事件和跨关钩子已连接。 | 原版/自定义练习、暂停、Retry、退出重入、阶段选择和辅助录像。局部 ECL 保护表只保存摘要，不含原指令字节。 |
| 存档与 Replay | 原版 `scoreth12.dat`、`th12.cfg` 编解码及白名单导入导出；IDBFS v21 存储；启动器录像工作区的详情、选关、播放／暂停、调速、定位；原版 Replay 输入格式保留，Web 扩展统一存于 `TH12WEB1`。 | 存档迁移、录像工作区及跨关需要人工验收；原版 THPrac 的 raw damage-source blob 无法导入。 |
| 音频、字体与图卡 | OGG full/真实分块 stream/静音；原音效事件按 WAV 原采样率由 WebAudio 播放；共享字体安装；从本地原资源生成独立 Host 图卡。 | 音乐循环、切曲、渐隐与暂停恢复；音效听感及长时间播放；共享字体和图卡的最终呈现。 |
| 独立语言包 | 日文原始路径与 thcrap 简体中文、英文静态语言包；与 TH10、TH11 共用依赖解析、校验和发布流程。 | 42 个 MSG、12 个结局、113 个符卡和17首音乐说明；自动文字保留时间节点，Unicode 字体按实际字形排版。 |

切换型低速在 Miss 复活后保留选中状态，复活前的拖动坐标和待发手势按玩家实例清理。

第一关开场的收点线提示使用原版 `front.anm` 第51号动画及其49／50子动画，
按原资源执行展开、闪烁和退出；普通录像、THPrac录像、续关及其他关卡不显示该提示。

Practice 参数面板外单指轻触开始练习，双指轻触返回机体选择；面板内触控用于调整参数和操作弹出菜单。暂停菜单、快捷键和结果菜单中的重新开始保留本局关卡、分段、难度及初始资源。

THPrac 工具自身的 `ja-JP`/`en-US`/`zh-CN` 标签选项不等于游戏语言支持。
语言编译保持日文非文字 MSG 控制命令和 ECL 游戏逻辑；不能据此声称与汉化 EXE 的
全部显示时序完全相同。编译、字体与验证入口见 [LOCALIZATION.md](../../ports/th12/docs/LOCALIZATION.md)。

## Host / Runtime 契约

协议为 `eagler-touhou/1`，游戏为 `th12`。契约对照为 monorepo 的
`common/contracts/product-catalog.mjs`、对应类型声明和 `launcher/`；当前来源固定信息见
`integration/common-dependency.json`，平台登记改动保存在
`integration/eagler-touhou-th12.patch`。本地登记与源码物化不等于对方仓库已发布。

Runtime 校验父窗口、同源身份和正整数导航 epoch，串行处理命令，回复保留 request 身份。
存储及受管理 DATA 就绪后发送 `ready`；`configure` 安装所需音乐、字体和选定语言资源，
`launch` 才进入加载画面与游戏。实际呈现后发送 `first-frame`，并报告版本、帧率和音频状态。

| 契约面 | 当前约束 |
| --- | --- |
| 导航与 DATA | 使用 `managedData=1`、`gameGeneration`、`runtimeEpoch`；正式入口读取 Package Store 提供的 DATA，不回退到开发服务器。 |
| 配置 | 支持音乐、展示上限、触控、THPrac 等已连接选项；未知选项或不支持的判定点增强报错。语言接受 `ja` 与已安装的 thcrap 静态包，所选语言与安装包必须匹配。 |
| 资源 | 附加资源的路径、来源、大小和声明摘要受校验；语言包只能安装受支持的文字/贴图数据，不执行包内代码。 |
| 输入 | `keyboard`、`keyboard-clear`、`touch-controls`、`direct-touch`、`touch-cancel`；`thprac-mouse` 走练习面板指针路径。游戏拖动与工具面板在按下时决定归属。 |
| 文件 | `list/read/write/remove/sync` 使用存档白名单和相对路径；正常游玩中不导入/删除文件；录像工作区可管理录像文件，已打开的输入保持独立副本；未知成绩、配置或损坏录像拒绝导入。 |
| 录像工作区 | `replayViewer` 配置和 `playback` 的 inspect/open/pause/play/seek/rate；支持 0.5/1/2/4/8 倍和满速，选择起始关卡并继续录像路线。 |
| 生命周期 | 普通 blur 只释放输入，不打开原版暂停菜单；隐藏页停止推进并保存；退出等待存储同步；WebGL 丢失停止游戏并报告错误。 |

真实 React Launcher 的本地 Hosted 接入及构建方式见 [Launcher 接入](../../ports/th12/docs/LAUNCHER_INTEGRATION.md)。
它沿用实际 Package Store、Runtime Generation、文件管理与离线资源流程；
`/adapter-preview.html` 保留为单独的协议观察入口。
Hosted 实际运行、External、Import、缓存/离线交付和真实设备链仍需人工验收。
多人 profile 未声明启用。

## 输入、存档与录像边界

权威输入每个逻辑步只消费一次。无法用原版三字键流表示的移动，记录核心实际接受的
1/128 像素定点增量；Replay 使用已解析结果，不重新识别触摸手势。无速度限制移动
和实时作弊按辅助记录处理，不作为普通排名成绩。

`/savesth12/` 对外只开放原版成绩/配置和允许的 `replay/th12_*.rpy`，
录像工作区还可使用一层安全子目录 `replay/<folder>/th12_*.rpy`。
浏览器内部 `web-profile.json` 保留 Web 历史及迁移信息，不作为原版文件或 Replay 侧车对外开放。

主库为 `/savesth12`、版本 21、`FILE_DATA`，使用 IDBFS 的文件、目录和时间索引结构，
由 Runtime 写入，启动器可直接列出已保存录像。旧 `th12-eagler-preview-files` 只读迁移：
已有新文件优先，文件和迁移标记一次提交，旧库保留；不同端口仍属于不同存储来源。
未知库结构明确拒绝，不能以清空存档作为恢复手段。

游戏内录像暂停菜单的“再次播放录像”从最初选择的起始关卡重播同一文件。
Z（或映射的开火键）四倍快进；Shift（或映射的低速键）慢放；
THPrac 开启时 Ctrl 和 Shift 使用紫版设置的回放快进与慢放帧率。

工作区不消费实时游戏输入。向后定位会重新初始化所选起始关卡，再按录像输入重演，
跨关保留既有伤害源桥接；定位以分片任务推进，呈现仍由浏览器显示循环负责。
定位和非正常速度期间不重播追赶过程的音效，恢复正常播放时按逻辑位置恢复 BGM。

`TH12WEB1` 是唯一新增尾部扩展，负载沿用 `th12-web-motion/1` 与坐标比例 128，
包含关卡移动行和可选 `practice`、`damage`；长度及 CRC32 必须匹配。
不需要 Web 特有数据时不追加扩展；不会另写第二个练习尾块。
`practice` 保存练习参数及辅助标记；`damage` 是完整流程灵梦 A 的版本化语义快照。
游戏内播放按 THPrac 开关接入伤害源兼容路径；启动器录像工作区自动启用回放核心，
保持交互式工具面板关闭，支持带练习参数及伤害源快照的 Web 录像。
播放不重新施加实时作弊，辅助录像可能不同步，不承诺原版程序能够理解 Web 扩展。
旧原版录像缺少伤害源数据时，兼容状态标记 unavailable，不能制造或推断原始堆内存。
详见 [THPrac 集成说明](../../ports/th12/src/practice/INTEGRATION.md) 和
[伤害源快照说明](../../ports/th12/src/practice/DAMAGE-SOURCES.md)。

## 本地准备、构建与来源物化

下面是供重建使用的入口。本轮实际执行了核心/解码器编译、Runtime 打包、
汉化资源构建、菜单元数据准备及 DATA 重打包、源码物化；没有运行游戏或验收脚本。
需要 Node.js 22+、Python 3.10+、Emscripten 4.0.22，以及准备脚本所用的公开依赖和 thtk。
原版与汉化安装文件只在本地读取，不上传 EXE、DAT、原贴图、音频或提取资源。

```powershell
node scripts/prepare-retail.mjs --original="D:\Games\TH12" --thdat="D:\Tools\thtk\thdat.exe" --python=".venv\Scripts\python.exe"
```

该入口静态准备游戏 DATA、18 首独立 OGG、来源清单和 Host 图卡，不运行原版程序。
已有转换资源时也可单独运行：

```powershell
node scripts/prepare-managed-content.mjs --assets="site/assets" --output="artifacts/eagler-content"
```

语言包以日文 `th12.dat` 为输入，在 monorepo 根目录使用统一准备器：

```powershell
node launcher/scripts/prepare-th06-language-pack.mjs --game th12 --language lang_zh-hans --archive D:/Games/TH12/th12.dat --thdat D:/Tools/thtk/thdat.exe --thmsg D:/Tools/thtk/thmsg.exe --output artifacts/th12-language
```

英文将语言改为 `lang_en`。输出为 `catalog.json` 与 `language/*.zip`，供 Host 组装。
准备器从 thpatch 获取文字和贴图；本地安装目录中的旧汉化不参与输入。
语言包及转换资源保存在 Git 忽略目录，默认语言为日文。
完整准备细节见 [CONTENT_PREPARATION.md](../../ports/th12/docs/CONTENT_PREPARATION.md) 与 [LOCALIZATION.md](../../ports/th12/docs/LOCALIZATION.md)。

固定 common 来源及文件摘要见 `integration/common-dependency.json`。
工作区可显式指定它和本机 Emscripten，再生成代码包：

```powershell
$env:EMSDK = 'D:\Tools\emsdk'
$env:EAGLER_COMMON_ROOT = 'D:\Sources\eagler-touhou\common'
npm run build:eagler
```

该入口依次编译核心、编译 Vorbis 解码器、生成 Runtime；单独的入口为
`npm run build`、`npm run build:audio`、`npm run package:eagler`。
核心构建记录源码及共享头文件摘要；打包会拒绝与当前核心源码不一致的生成物。

`build-eagler/` 仅为代码 Runtime：入口 HTML、模块、Wasm、manifest 与 `runtime-files.json`。
`artifacts/eagler-content/` 是本地 DATA、音乐、语言及来源产物；其中
`artwork/th12-card.webp` 单独交给 Host，不进入 Runtime 或 DATA。
共享字体由平台资源所有者提供，不从系统字体目录复制进代码包。

monorepo 声明的源码归属为 `ports/th12`。需要物化本仓库源码时运行：

```powershell
node scripts/install-eagler-source.mjs --workspace="D:\Sources\eagler-touhou"
```

该工具复制受版本管理及未忽略的源码，排除原版资源、构建产物和 `.git`，写入
`eagler-source.json` 来源清单；目标存在独立改动时拒绝覆盖。物化源码不负责发布，
也不能拿旧预览目录替代正式来源。

共享开发入口可用 `EAGLER_TH12_CONTENT_DIR` 指向准备好的 DATA/音乐目录。
本机人工入口为 `http://127.0.0.1:8114/adapter-preview.html`，可选择日文与已安装语言、
stream/full 音乐、THPrac 与触控模式；该检查页不进入 Runtime Release。

真实 Launcher 的本机入口为 `http://127.0.0.1:8130/`，由上游 React 页面与
`package-server.mjs --games=th12` 组装，当前输出位于忽略的 `artifacts/launcher-site/`。
2026-10-20 已完成页面、浏览器核心与音频编译，以及日文 DATA 和音乐打包。
Chromium 受管理资源验证覆盖标题、第一关、紫版高级选项及移动面板折叠；
原生 ImGui 输入检查覆盖快捷键门控、数值编辑、弹出项取消和指针归属。test 使用日文资源。
可重建入口为
`scripts/package-launcher.mjs`，详见 [Launcher 接入](../../ports/th12/docs/LAUNCHER_INTEGRATION.md)。

## 留给用户的人工确认项

以下项目本轮没有执行；确认结果应另行记录，不从编译、打包或源码存在推断。

- 60 Hz 与 120 Hz 展示下，同一普通路线/Replay 的逻辑、弹幕和动画连续性。
- 四种触控模式、低速、双击 Bomb、菜单/对话、失焦释放及原判定点常显。
- THPrac 原位置入口、分段/阶段、Pause、Retry、退出重入、跨关与辅助 Replay。
- 原版成绩和配置导入、Web 修改后导出，以及 Replay 全流程/独立关卡兼容。
- stream/full 音乐循环和渐隐、音效、共享字体；日文、简体中文与英文对话/结局/菜单/音乐室显示。
- 实际 Launcher 的 Hosted/External/Import、离线资源、存储落盘，以及发布范围和资源版权分离。

## 换面时序

MSG21 按原版场景回调进入 Stage Clear，120 帧计分等待期间继续执行
自机、弹幕、道具和 MSG 回调，随后按30帧游戏区域淡出切入下一关。
MSG6 退出文本动画后不再生成旧字幕；下一关沿用外围 HUD 动画时钟，
游戏区域独立淡入。资源等待期间保留上一逻辑帧的外围 UI。

`TH12_TEST_ASSETS` 指向日文资源提取目录后运行
`node --test ports/th12/tests/stage-flow.test.mjs`，可检查真实过关 MSG
的结束指令、自机与弹幕持续运行，以及跨关资源继承。

### 对话退出和暂停保存

MSG 的动画所有权覆盖公开句柄与已触发退出的动画。对话结束及替换时回收该对话的整组动画，Ctrl 跳过与切关使用相同清理边界。暂停菜单的“保存录像并退出”直接进入保存页，以游戏暂停画面为背景；完整失败结算继续使用既有疮痍和成绩流程。

回归入口：`tests/message-animation-retirement.test.mjs`（`TH12_TEST_RETAIL` 指向零售资源，`TH12_TEST_CORE` 指向支持 Node 的构建）与 `tests/pause-save-direct.test.mjs`。
