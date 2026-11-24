# bro 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/bossrush`。构建模式：`normal`、`multiplayer`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=bro --variant=both`

> 注：BRO 为第三方衍生作，以 `importOnly + testOnly + buildOptIn` 登记，未在 `runtime-builds.json` 中声明原生源码配方，故 `ports:check` 的源码/运行时契约门不适用；本表按适配清单如实记录实现与验证状态。
>
> **Runtime 产物已入库**：`ports/bossrush/web/`（31 文件 / 28 MB，`bro.wasm` 26.9 MB + glue + 清单，HTTP gzip 传输约 4.7 MB），当前 build_id `96902659`，详见 [RUNTIME_ARTIFACTS.md](RUNTIME_ARTIFACTS.md)。资源包（140.3 MB / 1004 文件）按 importOnly 由玩家导入，不入库。署名：游戏作者吃药图书（Cyoltose）、网页端维护清酒（Tidew）、eagler 开发者。原作者已出具网页版分发授权声明，见 [DISTRIBUTION-PERMISSION.md](DISTRIBUTION-PERMISSION.md)（源码非开放、需许可查看；ZUN/上海爱丽丝素材权利保留）。

## 源码与构建

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已实现 | 已实现 | BRO fork `TSake/TouHou-BossRushOnline` 分支 `web/multiplayer-port`；原作 `Cyoltose509/boss-rush`。MP 同目录、共用 wasm。 |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 已实现 | 已实现 | 纯 C Lua 5.1.5 + emsdk；`web/build.sh`，工具链复用 RGL 不复制。 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 已实现 | 已实现 | `bash web/build.sh` 指纹去重；normal/MP 共用同一 wasm。 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 已实现 | 已实现 | `bro.mjs` + `bro.wasm` + `runtimeAssets`，见 `web/manifest.json`。 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 已验证 | 已验证 | 真 Edge `play.html` 出画 PASS（PROBE_1C）；MP 真 Edge `?coop=1` 2~3 人出画 PASS（PROBE_3AE）。 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待核对 | 待核对 | 素材归属吃药图书；音频压缩 158→55MB、PNG 量化见 PROBE_3A。第三方衍生作授权口径待维护者确认。 |

## 启动与资源

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已实现 | 已实现 | `eagler_adapter.mjs` 信封 `eagler-touhou/1`，epoch 从 `runtimeEpoch` 注入校验。 |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已实现 | 已实现 | 13 命令 / 8 事件；`bro_boot`→首帧（PROBE_1B/1C）。 |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已实现 | 已实现 | adapter 统一 `{ok,...}` 响应，错误事件已定义。 |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 待核对 | 待核对 | 有 pagehide/visibilitychange flush；后台恢复需按官方流程实测。 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 已实现 | 已实现 | 资源走 JS HTTP 不进 MEMFS，磁盘枚举恒空、用清单枚举（PROBE_1F）。 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 待验证 | 待验证 | R 重开已接；判定点显示见 `Creative.lua`（调试用，发布排除），正式常显点待核对。 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已实现 | 已实现 | `web/dist/{package-manifest,app-shell,runtime}.json` 三层清单。 |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 待核对 | 待核对 | `dataTarget:"scripts.json"`（非官方 `.data` 惯例），需与启动器 package-store 对齐；见 proposals/MANIFEST_DIFF.md。 |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 待核对 | 待核对 | 组件 revision=组内(p,h)摘要，有差量设计，未做官方切换实测。 |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 待核对 | 待核对 | — |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待核对 | 待核对 | storage.mjs 有 IndexedDB + localStorage 降级，离线能力理论上具备。 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 待核对 | 待核对 | 需启动器接线。 |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 已实现 | 已实现 | KEY_BIT 8 键位与官方同构（PROBE_1D）；MP 用 per-instance `p.key`（3R）。 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 待核对 | 待核对 | — |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 待核对 | 待核对 | — |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 待核对 | 待核对 | — |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 待核对 | 待核对 | — |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 待核对 | 待核对 | — |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 待核对 | 待核对 | — |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 待核对 | 待核对 | — |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待核对 | 待核对 | — |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 待核对 | 待核对 | — |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待核对 | 待核对 | — |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 待核对 | 待核对 | — |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 待核对 | 待核对 | — |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待核对 | 待核对 | — |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 待核对 | 待核对 | — |
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 待核对 | 待核对 | — |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待核对 | 待核对 | — |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待核对 | 待核对 | — |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待核对 | 待核对 | — |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待核对 | 待核对 | — |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 待核对 | 待核对 | — |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待核对 | 待核对 | — |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 待核对 | 待核对 | — |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 待核对 | 待核对 | — |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 待核对 | 待核对 | — |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 待核对 | 待核对 | — |

## 画面与声音

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已实现 | 已实现 | `bro_frame` 固定 60Hz 逻辑。 |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 已实现 | 已实现 | JS renderer rAF 渲染，逻辑/渲染分离（PROBE_1C/2B2）。 |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 待核对 | 待核对 | — |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 待核对 | 待核对 | — |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待核对 | 待核对 | — |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 待核对 | 待核对 | — |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 待核对 | 待核对 | — |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待核对 | 待核对 | — |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 待核对 | 待核对 | — |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 待核对 | 待核对 | — |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 待核对 | 待核对 | — |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 待核对 | 待核对 | — |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待核对 | 待核对 | — |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 待核对 | 待核对 | — |
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 待核对 | 待核对 | — |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | 不适用 | normal：本作使用 OGG 音乐；multiplayer：本作使用 OGG 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | 不适用 | normal：本作使用 OGG 音乐；multiplayer：本作使用 OGG 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | 不适用 | normal：本作使用 OGG 音乐；multiplayer：本作使用 OGG 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | 不适用 | normal：本作使用 OGG 音乐；multiplayer：本作使用 OGG 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 已实现 | 已实现 | 原版 `Replay.lua` 零改动跑通 STGR 保存/读取/逐帧回放（PROBE_1D）。 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 已实现 | 已实现 | STGR 字节序列一致（PROBE_1D）。 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 已实现 | 已实现 | 见 PROBE_1D。 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 已实现 | 已实现 | MP 旁路多流 `replay/slotN.multi`（JSON），借 `replayMatch`（PROBE_3AC）。 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 已实现 | 已实现 | `BRO_fs_read/write/remove` MEMFS 真身 + eagler 文件桥（PROBE_3AC）。 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 已实现 | 已实现 | GetInput shim 在 replay 分支主动 return（PROBE_3AB）。 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待核对 | 待核对 | — |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 已实现 | 已实现 | `/User`、`/replay`、`/bro/data` → IndexedDB `BossRush-save`（PROBE_1E）。 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 已实现 | 已实现 | setting localStorage 镜像 + 降级链。 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 已实现 | 已实现 | 签名差分 30 帧轮询 + pagehide flush。 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 待核对 | 待核对 | — |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 待核对 | 待核对 | — |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待核对 | 待核对 | — |

## 语言与练习

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 不适用 | 不适用 | normal：该构建未提供 thprac；multiplayer：该构建未提供 thprac |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 已实现 | 已实现 | 原作中文/日文文本随资源。 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 不适用 | 不适用 | normal：本作使用内置原文；multiplayer：本作使用内置原文 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 不适用 | 不适用 | normal：本作使用内置原文；multiplayer：本作使用内置原文 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 不适用 | 不适用 | normal：本作使用内置原文；multiplayer：本作使用内置原文 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 不适用 | 不适用 | normal：本作使用内置原文；multiplayer：本作使用内置原文 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 不适用 | 不适用 | normal：本作使用内置原文；multiplayer：本作使用内置原文 |

## 多人接入与规则

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| MP-RULE-01 | [死亡、幽灵与救援](../../docs/adaptation/multiplayer.md#mp-rule-01) | 不适用 | 待验证 | normal：单人模式；BRO 有 miss/死亡记账（`BRO_net_stat`），无「幽灵救援」机制；需按共斗规则确认映射。 |
| MP-RULE-02 | [个人资源与团队资源](../../docs/adaptation/multiplayer.md#mp-rule-02) | 不适用 | 已实现 | normal：单人模式；3Z/3AE：樱点/信仰/UFO/灵界等团队共享；man/charge 逐槽独立（PROBE_3Z/3AE）。 |
| MP-RULE-03 | [给予资源与反馈](../../docs/adaptation/multiplayer.md#mp-rule-03) | 不适用 | 待验证 | normal：单人模式；收卡/绿道具蓄力广播全员（3AE）；无主动送残机/Power。 |
| MP-RULE-04 | [对话、结算与全灭](../../docs/adaptation/multiplayer.md#mp-rule-04) | 不适用 | 待核对 | normal：单人模式；结算入 summary 未做完整版。 |
| MP-RULE-05 | [自机可见性与多人HUD](../../docs/adaptation/multiplayer.md#mp-rule-05) | 不适用 | 已实现 | normal：单人模式；右侧安全区多玩家并排比例条 + 11 项系统显示逐槽（PROBE_3Y/3AE）。 |
| MP-01 | [独立 MP 产品变体](../../docs/adaptation/multiplayer.md#mp-01) | 不适用 | 已实现 | normal：单人模式；product-catalog 声明 `bro` + 自动派生 `bromp`。 |
| MP-02 | [房间人数、难度与机体](../../docs/adaptation/multiplayer.md#mp-02) | 不适用 | 已实现 | normal：单人模式；首期 `playerCounts:[2,3]`（官方标准常量），4 角色 loadouts；引擎内部已具备 4 槽但首期不开第 4 席。难度 0=Vanilla / 1=Legacy(legacy) / 2=天空璋AEX / 3=Bad Apple!，后两项是独立章节入口（见 MP-CUSTOM-04）。 |
| MP-CUSTOM-01 | BRO 专属房间选项 | 不适用 | 已实现 | 房主选全队携带系统（1..11）+ 章节游玩队列（1..19 有序），照 `th19Rules` 全链路按 `bromp-` 前缀隔离；UI 新 `BroRoomSettings`，共享合约 `bro-room-options.mjs`，切关继承中弹/收卡、不进 replay。队列末章走 `ReturnToTitle(false,0)` 跳过全作评分界面（公式分母是全作常量，部分关卡必失真），每关小结算保留（PROBE_3AQ）。 |
| MP-CUSTOM-02 | legacy 符卡/弹幕模式 | 不适用 | 已实现（联机驱动） | 房间难度 0=Vanilla（5.x 弹幕+符卡名）/ 1=Legacy（4.0 legacy 弹幕+4.0 符卡名）。复用现有 `netplayDifficulty`，adapter 白名单映射为 `legacy=`，web 层 hook `lstg.DoFile` 加载 `mod/GAME_L/` 并对新增符卡沿用当前版、去重。单人默认 Vanilla，游戏内无切换 UI。WASM `0fd414b6`（PROBE_3AR）。 |
| MP-CUSTOM-03 | A/B 类绑定玩家符卡机制多人适配 | 不适用 | 已实现 | 逐卡特殊化（不整体重跑含弹幕/音效/成就的 card 方法）：th06 全员换到 boss 原位且 boss 只换 P1（pin task resume 边界广播 + hscale/vscale 同步）；th07 樱花/th10/th11_2 全员传送固定点；th095 圆周推动各自独立；th08 境界の彼方虚影与判定钉 P1；th125 天花板按各自 x 压 y；th143 布内无敌逐人 + 旋转世界按 P1 推进、隙间**传送门逐玩家独立**（两门共享 per-slot 闩锁，弹幕穿门原逻辑）、blackhole 推动全员、异界送行提灯幽灵点位钉 P1+全员可碰共同收集+全员无敌+收缩圆看本机、替身地藏进圈需全员进过才破解；th185 扫码框钉 P1。`web/src/spell_player_mp.lua`，game/ 零改动，单人零行为变化，按 card_id 覆盖 GAME(5.0)+GAME_L(4.0 legacy)。WASM `00266e98`（PROBE_3AS/3AT；收尾加固 th06 del 全员缩放复位不依赖原脚本 + 清 swap_prev 闩锁防重打误判）。 |
| MP-CUSTOM-04 | 独立章节入口 + 随角色内容对齐 P1 | 不适用 | 已实现 | 房间难度扩为 0=Vanilla / 1=Legacy / 2=天空璋AEX / 3=Bad Apple!（复用 netplayDifficulty 零新协议，adapter 把 2/3 映射为 chapter）。AEX(HSiFS AfterExtra) 与 BadApple(Bad Apple!!) 是独立 stage group、不走 19 关队列，作为整局独立章节入口、全队 4 实体共斗（spawn 由 JS 驱动不依赖 stage:Option）。TH16 与 AEX 的随角色内容（季节顺序/boss group/无敌色/Unlock 门控）在多人下对齐 P1（房主）选角——协程 resume 期钉 P1 视图，各端内容一致但自机实体/输入仍归本机。BadApple 光点判定补判其余 3 实体（团队扣分），lock/nextshoot 广播全员。BadApple file1..7+lyrics 恢复（实证为逐帧光点坐标文本非视频，旧视频剔除是误判），embed 进 MEMFS /BadApple，gzip 传输约 3.5MB。web/src/chapter_content_p1.lua，game/ 零改动。WASM c91ac7a2（PROBE_3AV）。 |
| MP-CUSTOM-05 | 联机适配审计修复 | 不适用 | 已实现 | 审计发现并修复四类真问题：① configure/launch 两次下发都早于 `bro_boot`，网页设置与房间选项从未落 Lua → 新增 `applyAfterBoot()`，由 runtime_entry 在 boot 后、进关前调用（真机 E2E `boot_options_e2e` 验证 BGM=80/chapter=badapple/系统[1,2] 均落游戏）；② BadApple 其余实体扣分忽略 `colli()` 返回值，无敌/死亡仍误扣 → 与原版同语义按返回值扣分；③ AEX 的 `lock/hide` 只作用全局 player → hook `player_lib.system.lock` 与 `_editor_class.TH16_AEX.summary.init` 广播全员；④ 独立章节沿用上一局 legacy、card_credit 模块级状态残留、新房间 UI 默认零系统 → 均修正。WASM `544898ba`。 |
| MP-03 | [所有席位准备并同步开局](../../docs/adaptation/multiplayer.md#mp-03) | 不适用 | 已实现 | normal：单人模式；GATE 开局栅栏（PROBE_3J）。 |
| MP-04 | [确定性输入与状态检查](../../docs/adaptation/multiplayer.md#mp-04) | 不适用 | 已实现 | normal：单人模式；同种子同输入 360 帧逐帧指纹一致（PROBE_1F）；DIGEST 摘要（3I）。 |
| MP-05 | [各席位输入归属](../../docs/adaptation/multiplayer.md#mp-05) | 不适用 | 已实现 | normal：单人模式；per-instance `p.key` + Byte2KeyState（3R/3AC）。 |
| MP-06 | [旁观加入与输入隔离](../../docs/adaptation/multiplayer.md#mp-06) | 不适用 | 待核对 | normal：单人模式；旁观未启用（`spectator:false`）。 |
| MP-07 | [旁观只读取连续已确认历史](../../docs/adaptation/multiplayer.md#mp-07) | 不适用 | 待核对 | normal：单人模式 |
| MP-08 | [慢旁观者与玩家隔离](../../docs/adaptation/multiplayer.md#mp-08) | 不适用 | 待核对 | normal：单人模式 |
| MP-09 | [旁观尾帧与结束时机](../../docs/adaptation/multiplayer.md#mp-09) | 不适用 | 待核对 | normal：单人模式 |
| MP-10 | [联机暂停与重开](../../docs/adaptation/multiplayer.md#mp-10) | 不适用 | 已实现 | normal：单人模式；任意玩家暂停 + R 重开。 |
| MP-11 | [多人 Replay](../../docs/adaptation/multiplayer.md#mp-11) | 不适用 | 已实现 | normal：单人模式；旁路多流 + eagler replayMatch（PROBE_3AC/3AD）。 |
| MP-12 | [WebRTC 与 Relay 路径](../../docs/adaptation/multiplayer.md#mp-12) | 不适用 | 已实现 | normal：单人模式；直连/TURN 回退 + 滚动质量选路（PROBE_3O/3P/3Q）。 |
| MP-13 | [本地玩家可见性](../../docs/adaptation/multiplayer.md#mp-13) | 不适用 | 已实现 | normal：单人模式；全局 player swap 后稳定恢复（多处 hook）。 |
| MP-14 | [联机与回滚诊断](../../docs/adaptation/multiplayer.md#mp-14) | 不适用 | 已实现 | normal：单人模式；回滚 + checkpoint + 摘要对账（3L/3M/3N）。 |
| MP-15 | [游戏结束返回房间](../../docs/adaptation/multiplayer.md#mp-15) | 不适用 | 待核对 | normal：单人模式；需 Host 接线。 |

## 验证与交付

| 编号 | 检查项 | normal | multiplayer | 记录 |
| --- | --- | --- | --- | --- |
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 待核对 | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 待核对 | 不适用 | 120 帧多人录像/回放 e2e（PROBE_3AD）已具备，官方样本待对照。；multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 已实现 | 不适用 | 分包 manifest 逐文件 sha256（PROBE_3A）。；multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 待核对 | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待核对 | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 已实现 | 不适用 | 双 WASM 同种子 360 帧指纹一致（PROBE_1F）。；multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 待核对 | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 待核对 | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |

<!-- manual-verification -->

## 运行验收记录

- 真 WASM Node 回归 30/30 绿；真 Edge `play.html?stage=TH08@BossRush&coop=1` 2~3 人出画 PASS（BRO 仓库 PROBE_3AE）。
- 多人 replay 真浏览器手点 PASS（BRO 仓库 PROBE_3AD mre_e2e）。
- 待补：官方 Host 流程「选游戏→启动→首帧→退出」记录；触控/手柄实测；官方基准对照。
- 注意：引擎基线帧 ~182 自身非确定性分叉（另案），回滚摘要校验在 <180 帧可靠，`inputTiming` 已如实反映。

## 分发与封面（2026-11-22）

- BRO 为 `importOnly`：Runtime（WASM/glue/JS）随本站部署，1004 个游戏资源由玩家在 GAME DATA 导入窗口选择 ZIP 提供。`launcher/scripts/package-bro-import.mjs <broRepoRoot> <out.zip>` 读 BRO 源库 `web/dist/package-manifest.json` + `game/` 生成可导入包（`eagler-touhou/package/1`：descriptor + 1 字节 bro.data + `game/...` 资源，target `/game/<p>` 经 `resource_cache` 归一化匹配）。已用 `parsePackageZip` 验证 1005 文件、哈希全匹配。
- 启动器卡片封面 `assets/bro-card.webp`：源图 `ports/bossrush/artwork/bro-card.webp`（被 git 跟踪），`prepare-host-artwork.py` 已登记 `bro` 适配器，构建时生成并随站点发布；`audit-publication.mjs` 白名单已放行 `assets/bro-card.webp`。
