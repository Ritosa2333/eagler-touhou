# TH17 MP 开发指南

[开发入口](../README.md) · [适配表](../ADAPTATION.md) · [Runtime 契约](../../../launcher/docs/GAME_ADAPTER_CONTRACT.md)

## 构建

使用 Node.js 22.18 以上、Python 3、Emscripten 6.0.9。编译默认读取仓库 `common/` 的键盘与手柄边界，在每次原生模拟采样时消费 Host 映射。TH17 的渲染、原版帧节奏与触控运动轨迹保留本作实现。

在仓库根目录运行：

```sh
node tools/workspace.mjs select th17 --variant=multiplayer
node launcher/scripts/build-runtime.mjs --game=th17 --variant=multiplayer --emsdk=/path/to/emsdk
node ports/th17-mp/web/scripts/build-sdl-application.mjs --release --print-inputs
```

直接构建可设置 `TH17_EMSDK` 或 `EMSDK`，Python 可由 `TH_PYTHON` 指定。产物位于 `web/artifacts/sdl-multiplayer/`。生产构建去除开发探针，保留游戏逻辑、文件格式验证与原生录像启动接口。

打包 Runtime：

```sh
node ports/th17-mp/portable/package-eagler.mjs
```

`build-eagler-multiplayer/` 输出 `th17.html`、程序、CP932 转码表、资源清单和逐文件 SHA-256，可通过共用 Runtime Release 打包入口使用。`EAGLER_RUNTIME_OUTPUT` 可指定输出目录。Host 从 Package Store 挂载共用 MS Gothic 和 Unifont、原版 DATA 与音乐。浏览器渲染时按需生成和缓存字形。SDL 浏览器测试可用 `TH17_SHARED_FONT_ROOT` 指向这两份共用字体。

## 本地输入

`games/th17/original/` 放个人原版 `th17.dat`、`thbgm.dat` 与开发对照用 `th17.exe`。`web/assets/`、`web/reference/assets/` 为被忽略的开发输入，可从个人交付包连接或准备，源码迁移清单记录了来源身份。原版素材、音乐、录像、测量字体、SDK 和编译输出分别位于仓库忽略目录。

```sh
node launcher/scripts/prepare-th17-content.mjs --original=/path/to/original --output=/path/to/assets-ogg
```

该入口生成原版 archive、18 首 OGG 及来源记录；编码与共用 Host 的固定 OGG 基线核对。已准备的内容可由 Host 的 `assets-ogg` 输入使用。单个 `th17.dat` 也可通过共用启动器的原版文件导入流程安装。

## thcrap 语言包

启动器为 TH17 提供日语原文、简体中文和英文。Host 使用共用的 thcrap 编译入口，从补丁依赖链读取贴图和文本。翻译使用 Unifont，MS Gothic 补充缺字；日语原文使用 MS Gothic，Unifont 补充缺字。离线语言包附带 Unifont 子集。手工准备时，对每个语言执行一次，并用 `--refresh 1` 获取当前补丁清单。

```sh
node launcher/scripts/prepare-th06-language-pack.mjs --game th17 --language lang_zh-hans --archive /path/to/th17.dat --thdat /path/to/thdat --thmsg /path/to/thmsg --font-file /path/to/unifont.otf --output /path/to/th17-language-packs --refresh 1
```

`assets-ogg/th17.data` 也是可用的原版档案输入。英文使用 `lang_en`。标题、菜单、关卡标识、人物名牌和结局等 ANM 贴图按原图集的精灵区域覆盖。Music Room 按列表序号匹配曲名和评论，播放仍使用原作的曲目文件。符卡名在战斗、符卡练习和玩家资料中按当前难度显示，存档保存原版名称。

## 协议与存储

Runtime 使用 `eagler-touhou/1`，校验同源父窗口、作品与正整数 `runtimeEpoch`，按 `ready → configure → launch → first-frame` 运行。DATA provider 为 `retail-memory`，Host 提供原始 `/th17.dat`，资源清单提供字体表，OGG 挂载到 `/music/`。原版文件与 OGG 的安装代次由 Host 统一管理。

配置支持 OGG 与无音乐、连续触控、摇杆、100–300 灵敏度、双指低速、双击符卡，以及指定文件录像启动。用户文件限定 `/savesth17/scoreth17.dat`、`th17.cfg` 和 `replay/th17_*.rpy[x]`；写入先由原生解析器验证。Runtime 定期保存，失焦、隐藏、退出时清除临时输入，复用 Host 音频上下文。帧与音频健康事件携带同一会话代次。

TH17 多人版的产品能力表登记实际构建功能。逐项运行验收记录见适配表。

当前生产构建的接入结果见[2026-10-30 验证记录](../../../archive/th17-development-20261030/migration-validation.md)，覆盖桌面 Chromium 的生产 Wasm、两种音乐模式、存档恢复与指定原版演示文件启动。

## 多人房间

测试版大厅提供双人和三人合作房。每个席位选择原版角色与动物组合，使用自己的自机、火力、残机、Bomb 和原版 HUD。房间身份绑定难度、各席位配置、种子、输入时序与 Wasm 构建。回滚模式保留 12 帧战斗快照，方向最多预测两帧。纯延迟模式可设置 0–9 帧输入延迟。

房主可在开局前开启挑战模式。各席位掉残后正常复活，HUD 的残机栏显示本局累计 Miss。Bomb 和 Bomb 碎片不入库存，动物灵与暴走保留原有规则。多人录像和观战会恢复开局时的挑战设置。

动物灵池、五格库存和暴走计时全队共享。有效席位中距离最近的玩家拾取灵体，距离相同按席位顺序处理。拾取残机碎片或 Bomb 碎片时，全队各得一份。满残玩家把自己的残机碎片转为 Bomb 碎片，其余席位按自己的库存结算。小 P 只给拾取者补充火力，双人房增加 `0.02`，三人房增加 `0.03`。

道中、Boss 非符和符卡使用同一套战斗快照，保存各席位的自机和 Bomb、敌机与 ECL 线程、弹幕和激光池、道具与共享暴走、符卡、背景、动画关系及随机数状态。迟到输入与预测有差异时，从最早受影响的帧恢复并重演。音效按连续确认帧提交。对话、暂停、曲目切换和关卡加载在确认帧进入，保持文字、音频和场景资源的创建顺序。回滚房在这些确认阶段以 60 Hz 采集并发送最多八帧的输入队列，回到战斗后按帧消耗已采集的输入，再恢复实时采样。

弹池快照按原顺序保存活动列表和空闲列表，复用休眠弹槽的不可变副本。重演每轮最多执行四帧，执行完一帧后检查 8 ms 的预算，再决定是否继续。中间帧运行逻辑绘制和计时回调，最终帧和需要输出资源的场景边界提交 GPU 绘制。逐帧状态哈希由房间验收脚本开启。

```sh
cmake -S ports/th17-mp/web/tests/multiplayer -B build/th17-multiplayer -G Ninja -DCMAKE_CXX_COMPILER=clang++
cmake --build build/th17-multiplayer
ctest --test-dir build/th17-multiplayer --output-on-failure
```

Windows 从 Visual Studio 开发者终端执行，可加 `-DCMAKE_LINKER_TYPE=SYSTEM` 选用系统链接器。

多人 Wasm 使用 `node ports/th17-mp/web/scripts/build-sdl-application.mjs --release --multiplayer` 构建，再用 `node ports/th17-mp/portable/package-eagler.mjs --multiplayer` 打包。`node ports/th17-mp/web/tests/multiplayer/room-browser.mjs` 启动本地 Relay 和 Chromium，创建双人和三人房，在 WebRTC 输入通道注入延迟并逐帧比较各端确认后的战斗状态。加 `--lockstep` 验证纯延迟模式，加 `--extended` 运行 Normal 双人长测，加 `--through-stage-two` 验证 Easy 第一面道中、非符、符卡和切关。`TH17_ROOM_DIFFICULTY=4` 从 Extra 开局。用 `TH17_ROOM_DELAY_MS`、`TH17_ROOM_JITTER_MS` 和 `TH17_ROOM_CPU_THROTTLE` 调整网络与 CPU 条件。脚本读取 `EAGLER_TH17_BUILD`、`EAGLER_TH17_DATA`、`TH17_ROOM_GOTHIC` 和 `TH17_ROOM_UNIFONT`，把截图和 JSON 报告写入 `TH17_ROOM_EVIDENCE`。

加 `--dense-extra` 记录 Extra 的密集区段、活跃堆采样峰值和 Wasm 线性内存高水位。耗时统计以最近确认帧的敌弹数量分组，密集组从 256 颗开始，采样限于游戏画面。用 `TH17_ROOM_SAMPLES` 调整每个房间的确认帧样本数。

Extra Boss 压测为构建和打包命令同时加 `--room-benchmark`，并为 `TH17_OUTPUT` 和 `EAGLER_RUNTIME_OUTPUT` 指定独立目录。用该包运行 `room-browser.mjs --extra-boss`，三席按物理采样编号发送固定输入，并使用碰撞保护持续经过原版非符和符卡。报告分开记录快照、逻辑绘制和实际绘制耗时。`TH17_ROOM_TICKS` 指定结束确认帧，`TH17_ROOM_BASELINE` 指向同一输入长度的旧版报告，以核对 Boss 段落和终点状态。复测在基线关底 Boss 开始前 1500 帧应用 CPU 限速。

使用 `--extra-finale` 跑完关底最后一张符卡，并核对关底十张符卡的覆盖范围。此模式从第 9000 个确认帧应用 CPU 限速。房主样本记录在 `heap-samples.json`。基线与复测按同一模拟帧、弹量、激光数、符卡号和快照数配对，分别报告活跃 Wasm 堆与各类快照数组的容量。

## 多人标题、录像与旁观

房间准备后，原生标题持续绘制并开放 Options。标题通过 `room_preview` 关闭待机 Demo 和游戏开始入口。对局退出沿用同一运行实例返回房间标题，启动器通过 `launchSerial` 对应每次启动与返回事件。

多人录像保存在独立的 `/savesth17mp/replay/`，文件前缀为 `th17mp_`。录像器在确认帧提交时追加全席位输入，保存房间 seed、难度、角色、动物灵、规则 ABI 和运行包身份。文件通过临时写入与重命名提交，定期同步 IDBFS。原生结果菜单可以命名保存，启动器录像管理器提供分关选择、暂停、变速和时间定位。一次连续记录包含游戏内重试，重复经过的关卡使用独立章节编号。

多人默认按 640×480 渲染，`renderScale=2` 可指定 1280×960。敌机、弹幕和激光按来源位置选择最近的有效自机。Bomb 延长队友无敌时间，水獭暴走给各有效席位生成消弹护盾。P 道具遇到满 P 的拾取者时转向最近未满 P 的有效队友，拾取价值仍按房间席位数计算。

旁观复用共用的 `SpectatorStagePublisher`、`SpectatorStagePlayback` 和时间轴。关头在原生关卡构造后、首次输入和 ECL 更新前采集，以命名标量编码各席位资源和位置、共享动物灵、RNG 与输入边沿。房主发布确认后的输入和 Boss 节点，服务器维护当前关的历史。旁观按 90 帧缓冲播放，暂停、回跳与回到直播使用同一原生恢复入口。接收与发送每次最多占用 2 ms，主动定位每次演算最多占用 6 ms，历史上限沿用共用协议。终局录像经旁观通道传输，播放结束后进入本地结果菜单。

Boss 采用 TH15 的有效人数规则。双人伤害乘 `9/12`，三人乘 `8/12`，剩余一名有效玩家时恢复原伤害。临时死亡保留人数，残机耗尽的玩家退出倍率计算。每个 Boss 保存整数除法余量，该值随敌机状态一起回滚。各席位临时死亡的伤害折减在合并伤害前计算。

## 检查

```sh
npm run ports:check -- --game=th17 --variant=multiplayer
node --test ports/th17-mp/web/tests/adapter-browser.test.mjs
npm run docs:check
```

`adapter-browser.test.mjs` 读取 `EAGLER_TH17_BUILD`（默认 `build-eagler-multiplayer/`）、`EAGLER_TH17_DATA`（默认 `games/th17/original/th17.dat`）、`EAGLER_TH17_MUSIC`（默认 `games/th17/assets-ogg/music`）和 `EAGLER_TH17_REPLAY`（默认个人解包的 `demo1.rpy`）。检查覆盖两种音乐模式、会话隔离、用户文件、输入清除、原生退出、IDBFS 跨文档恢复和原版录像启动。

原生模块对照使用 WASI SDK，可通过 `TH17_WASI_CXX` 指定 clang++，先运行 `web/scripts/cpp/build.mjs`，再执行 `web/tests/cpp/*.test.mjs`。`TH17_UNICORN_MODULE` 指向个人安装的 `@alexaltea/unicorn-js/dist/unicorn_x86.js`；`web/reference/native/` 放开发包对照用 `unicorn-bounded.wasm`。字体重测工具的 `TH17_NATIVE_PACKAGE` 指向提供 Koffi 的 package.json。对照工具使用个人原版 EXE 和解包素材。原交付构建的版本、路线与设备范围随历史记录保留。

## 目录历史

普通版位于 `ports/th17`，多人版位于 `ports/th17-mp`。两侧独立编译和打包，沿 [TH17 子树来源](../../../docs/migration/th17-subtree-20261106.md)保留交付包身份及目录提交。通用修复使用[三方同步与历史写回](../../../docs/adaptation/port-sync.md)。
