# 存档与 Replay

[适配流程](README.md) · [文档目录](../README.md)

先打通文件管理，再检查刷新、退出、更新和离线后的结果。Replay 的文件操作与回放一致性分别验证。

## 准备

记录本作普通版和 MP 的存储根。保存一个设置、一份成绩与一个 Replay，按下面的步骤操作。存储根的当前定义见[产品目录](../../common/contracts/product-catalog.mjs)和[MP 路径规则](../../launcher/src/launcher/replay-storage-policy.mts)。

跨源迁移按普通版与多人版各自的数据库归属传输存档和 Replay，覆盖全部注册产品。迁移检查将公开页面的数据库白名单与同一套 MP 存储规则逐项核对，新增作品时同步验证其普通版和多人版迁移入口。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="replay-01"></a>REPLAY-01 · 导入原版 Replay | 导入本作原版 Replay 并播放到声明结束点。 | 可读取并按对应原版规则回放。 |
| <a id="replay-02"></a>REPLAY-02 · 按原版格式导出可表达输入 | 用可由原版格式表达的键盘或手柄输入录制并导出。 | 导出为原版兼容格式。 |
| <a id="replay-03"></a>REPLAY-03 · 记录实际生效输入 | 分别用键盘、手柄和触控录制同类动作。 | 记录的是输入合并后实际生效的动作。 |
| <a id="replay-04"></a>REPLAY-04 · 扩展格式与版本 | 用原版格式无法表达的输入录制，检查扩展标识和版本。 | 扩展信息明确、可验证，回放可还原输入。 |
| <a id="replay-05"></a>REPLAY-05 · Replay 文件管理 | 列出、读取、写入、删除 Replay，并重新打开游戏。 | 操作作用于本作允许目录，结果持久保存。 |
| <a id="replay-06"></a>REPLAY-06 · 回放与实时输入隔离 | 回放中移动摇杆、滑动屏幕、按射击。 | 实时操作只影响明确的回放或菜单控制。 |
| <a id="replay-07"></a>REPLAY-07 · 回放独立于显示刷新率 | 同一回放在 60 Hz 和高刷新率下播放。 | 逻辑状态与结束结果一致。 |
| <a id="save-01"></a>SAVE-01 · 已声明的存储根 | 核对产品目录中的存储根、成绩文件和配置文件。 | 正常版和 MP 使用各自声明的路径。 |
| <a id="save-02"></a>SAVE-02 · 用户文件允许范围 | 调用文件操作，包含允许文件与一个目录外路径。 | 用户文件正常操作；目录外请求返回错误。 |
| <a id="save-03"></a>SAVE-03 · 明确持久化时机 | 修改设置、保存成绩或 Replay，然后执行同步。 | 同步完成后可关闭实例。 |
| <a id="save-04"></a>SAVE-04 · 退出重开后数据保留 | 退出 Runtime 后创建新的实例。 | 已同步的成绩、设置与 Replay 仍在。 |
| <a id="save-05"></a>SAVE-05 · 离线读取已有数据 | 已安装并运行一次后断网，关闭页面再打开。 | 同一份用户数据可继续读取和保存。 |
| <a id="save-06"></a>SAVE-06 · 游戏包与用户数据分开 | 安装新版游戏包，再读取原有成绩与 Replay。 | 游戏资源版本切换后用户数据保留。 |

## Replay 音频接管

播放器按回放控制能力组织，作品类型与普通版、MP 的资源和存储身份由各作维护。

| 播放器类别 | 当前作品 | 共享入口 | 本作职责 |
| --- | --- | --- | --- |
| 关卡录像 | th06、th07、th08、th10、th11、th15 的普通版与 MP | `common/browser/local-replay.mjs`、`local-replay-native.mjs` | 解析格式、选关初始化、逻辑步进、结束判定与音频输出 |
| 对战录像 | th09 普通版与 MP | `common/browser/versus-replay-th09.mjs`、`playback-clock.mjs` | 原生双人回放、对局信息、回合边界与本作音频接口 |
| 游戏内菜单回放 | th075 普通版与 MP、th20 普通版 | 本作菜单与游戏循环 | 原生选片、回放操作与音频生命周期 |

th09 对战播放器保留完整的本作实现，普通版和 MP 打包均直接读取共享目录。开发 shell 使用逐字节一致的本地副本，通过 `node common/tools/sync-versus-replay.mjs --check` 核对。该模块随本作的 `auto-replay.mjs` 一同发布，继续读取对局与回合信息。新增对战作品时按本作格式和原生 ABI 接入，再提取已验证的共同能力。

th06、th07 通过原生 `Render()` 推进录像，停止自动迭代时保留音频设备，声音由 `SoundPlayer` 与本作前后台入口管理。目录 Runtime 通过独立的输出适配接口交接设备和 PCM 填充；两类入口分别核对。

录像工作台在同一个游戏 iframe 中切换文件和关卡。共享 `local-replay` 桥先停止普通循环，在原生录像打开完成后恢复音频设备，再由录像时钟推进固定逻辑步。浏览器音频恢复异步进行，命令立即回复；等待输出就绪期间保留当前逻辑帧。

目录 Runtime 通过 `pauseAudio` 连接本作原生输出，已有前台恢复函数通过 `resumeAudio` 继续管理 SDL 节点连接。暂停、定位、结束与后台共同决定输出状态；定位结束保留原来的暂停选择，并清理跳过区间的瞬时音效。切换文件后重新应用输出状态，关闭工作台时暂停输出。

th08 普通版与 MP 的 `eagler_replay_audio_pump`、th10 普通版与 MP 的 `sdl_audio_pump` 在录像逻辑步后填充 PCM，持续提供 OGG 背景音乐。th11、th15 的原生录像步包含 PCM 填充。th09 使用对战播放器中的恢复、音频填充和静音流程。

验收首次选片、连续换片、重播、暂停定位与前后台切换。分别检查 BGM 和 SFX，确认定位结束后按原来的暂停选择恢复输出，跳过区间的瞬时音效不补播。浏览器恢复测试包含 pending 的 `AudioContext.resume()`，核对命令响应和逻辑帧保持。

共享桥的行为测试位于 [`local-replay-bridge.test.mjs`](../../common/tests/local-replay-bridge.test.mjs)。[`test-replay-audio.py`](../../launcher/tests/browser/test-replay-audio.py) 通过 `--game` 和 `--multiplayer` 选择 th08、th10、th11、th15 的正式 Runtime。明确提供录像、DATA、OGG 与字体后，在 iframe 中分别采集 BGM 和 SFX 音频图，检查持续填充、换片、暂停与定位，并记录输入哈希。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[持久化桥](../../common/browser/save-sync.mjs) · [存储检查](../../launcher/docs/RUNTIME_STORAGE_TESTING.md)
