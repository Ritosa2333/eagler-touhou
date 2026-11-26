# thkkd 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/thkkd`。构建模式：`normal`。

实现情况与实际验收分别记录。自动检查登记通过的源码接口和模块记为“已实现”；源码或模块已有结果、专项操作尚待验收的项目记为“待验证”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=thkkd --variant=normal`

## 源码与构建

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已验证 | normal：普通版目录、构建配方与源码输入归属一致；[公共接口检查](../../docs/adaptation/status.json)。 |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 已验证 | normal：Emscripten 6.0.9、Node.js 24.19.0、Bun 1.4.4 及字体、转换依赖按开发指南完成构建；[构建与接入记录](../../archive/thkkd-import-20261126/README.md)。 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 已验证 | normal：normal WASM 与启动器独立构建通过；本作只有单机模式；[构建与接入记录](../../archive/thkkd-import-20261126/README.md)。 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 已验证 | normal：当前 26 项 Runtime 文件、611 项 C++ 输入与构建记录一致；正式 WASM 与 r7 相同，诊断写入接口单独构建。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 已验证 | normal：单作品站点实际启动，以及四种前端隔离构建、10 项检查通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待验证 | normal：源码基线、素材位置与哈希已登记；见[来源登记](../../docs/migration/thkkd-file-import-20261126.md)及[第三方来源](THIRD_PARTY.md)。 |

## 启动与资源

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已验证 | normal：启动、退出、再次启动使用新的 runtimeEpoch；协议与消息归属接口检查通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[公共接口检查](../../docs/adaptation/status.json)。 |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已验证 | normal：正式卡片完成就绪、配置、启动并呈现首帧，重新进入正常；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已验证 | normal：文件命令正常响应；无效 Replay 元数据返回错误并保留当前回放；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 已验证 | normal：平台挂起恢复、退出释放音频／图形／媒体及再次启动通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 已验证 | normal：启动器转换并安装原版资源，Runtime 从共用存储取得 DATA；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 已验证 | normal：共用暂停及重开按钮、原生低速判定点和常显判定点跟随通过；关闭常显开关及额外绘制只读检查通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已验证 | normal：双 DXA 与原版 ZIP 转换后进入 Package Store，安装及刷新复用通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已验证 | normal：327 项原始资源逐项校验，生成 301 项运行资源；DATA 身份与 /thkkd.data 挂载声明一致；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)、[公共接口检查](../../docs/adaptation/status.json)。 |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已验证 | normal：资源版本更新中途取消保持旧版本，恢复安装后一次切换完整新版本；浏览器安装事务验证通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 不适用 | normal：本作只提供本地资源导入 |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 已验证 | normal：首次安装后断网刷新、重开并进入游戏通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已验证 | normal：正式导入窗口统一管理转换、安装及错误反馈，游戏使用已安装资源；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 已验证 | normal：默认方向键、射击、Bomb、暂停与菜单确认通过正式 Runtime 检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已验证 | normal：WASD、自定义绑定、保存布局及恢复默认映射通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 待验证 | normal：虚拟标准手柄经过生产映射完成方向、射击、低速与原生录像录制；实体设备操作待验收。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 待验证 | normal：虚拟标准手柄按钮和方向录入原生 .rpy，保存与重新播放通过；实体控制器操作待验收。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已验证 | normal：键盘持续持有时取消触点，键盘继续生效；共用输入归属及手柄释放检查通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 已验证 | normal：浏览器失焦事件和虚拟手柄断开后释放输入，键盘独立持有保留；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 已验证 | normal：原生菜单选择、确认、取消，Z／Enter 确认和对话推进已验证；手柄覆盖采用虚拟设备及模块检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 已验证 | normal：实际触控表面的限速移动、位置变化及取消通过；事件由 DevTools 注入；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 已验证 | normal：无限速跟随与切回限速进入真实 WASM 移动；实际触控表面录制扩展录像并回放通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 已验证 | normal：方向摇杆四向及四个斜向逐一进入原生位移，方向一致。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 已验证 | normal：自由摇杆八向及 0.3／0.4 向量的幅度进入原生位移，幅度为最大速度的一半。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 已验证 | normal：按住低速、释放及原生速度变化通过，低速时使用原生判定点。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 已验证 | normal：实际共用切换按钮持续持有低速，再次点击释放；原生按键计数同步变化。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 已验证 | normal：双指触摸激活低速，抬起一指后恢复高速并保留移动触点。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 已验证 | normal：射击、Bomb 与暂停进入原生输入；移动触点与放雷同时保持，共用暂停按钮生效。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 已验证 | normal：真实中弹和复活期间保留同一触点，保持原入场坐标与速度，复活后继续拖动；修复复活时向场内跳位。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 已验证 | normal：实际无限速移动显示处理落率 100%，原生保存菜单生成带坐标和 100% 标记的 .thkkdr，导入回放通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 已验证 | normal：短双击触发原生 Bomb 并保留另一触点；拖动、长按、取消和菜单确认不会串成双击。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 已验证 | normal：单指确认、双指返回已有模块检查；Boss 对话中的触控短按及长按已实测；[本作模块检查](tests/extensions.test.mjs)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 已验证 | normal：实际触控完成保存询问、槽位、署名与结束确认；持续射击进入对话的新确认边沿检查通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已验证 | normal：100%、200%、300% 灵敏度逐一进入真实 WASM 位移，距离与倍率一致。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 已验证 | normal：触控取消后键盘继续持有；手柄、键盘与触控归属及录像隔离的模块检查通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 已验证 | normal：实际触控移动后取消释放触点，浏览器失焦后清除移动输入；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已验证 | normal：共用编辑器保存布局，横竖屏控件与布局检查通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已验证 | normal：实际点击共用暂停和重开按钮，原生场景恢复、游戏帧重置并继续运行。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已验证 | normal：共用双指放大镜缩放与恢复默认大小通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |

## 画面与声音

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已验证 | normal：正式普通帧循环在模拟 60～360 Hz、显示限频开关共 14 组合中推进相同步数，原生状态相同；录像四种刷新率到末帧一致。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 待验证 | normal：解除浏览器限频后额外呈现已验证；120／144 Hz 实体显示器的呈现间隔与画面体验待验收；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 已验证 | normal：GPU 复用相邻逻辑快照，额外绘制保持完整 WASM 内存；当前逻辑帧与 r7 逐像素一致。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 已验证 | normal：GPU 平滑位置、最短角差与缩放，图集换帧保留连续位移；九机体及辅助池两绘制层、第 0／7／31 槽的定点显示通过。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 已验证 | normal：出生、复用、瞬移、材质、裁剪及圆形分段的边界检查通过；辅助对象池定点和九机体画面通过。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 已验证 | normal：保留原生暂停表现；录像暂停恢复、切速、定位和结束时重建显示时刻，EOF 立即冻结。 [高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)。 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 已验证 | normal：正式启动与首帧检查通过，首帧事件在提交实际绘制后发送；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[呈现入口](web/shell.mjs)。 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待验证 | normal：60／120／144／240 Hz 模拟时钟限制到 60 Hz，以及浏览器约 60 Hz 呈现通过；实体高刷设备切换待验收；[本作模块检查](tests/extensions.test.mjs)、[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-01 | [原版 MP3 音乐](../../docs/adaptation/presentation.md#audio-01) | 已验证 | normal：18 首原版 MP3 保留字节；标题、开局和原生 Music 菜单检查及实际 BGM PCM 输出通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 已验证 | normal：无 BGM 模式与音效独立输出已有媒体验收，r5 复核静音后 BGM PCM 为零；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 已验证 | normal：标题与游戏切换、音乐及视频暂停恢复、原生退出停止和重新进入通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 已验证 | normal：平台挂起与恢复后媒体继续播放，音频上下文恢复及独立增益通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 已验证 | normal：损坏 MP3 的错误持续上报，换回有效 MP3 后恢复；缺失音乐报告错误并保留游戏运行，无音乐模式独立通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 已验证 | normal：额外绘制 100 次音频事件为零，高频呈现与原生逻辑分开；本作为单机模式；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 已验证 | normal：实际延迟音乐请求 1500 ms 时音效独立输出，首个非零 PCM 在 12.1 ms 检出，随后音乐正常开始。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | normal：本作使用原版 MP3 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 已验证 | normal：兼容 rep 系统版的原生 .rpy，17 份录像实际连续播放到最后输入；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 已验证 | normal：原生菜单完成录像询问、槽位和署名保存，导出 .rpy 并重新读取／播放通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 已验证 | normal：生产手柄映射记录实际射击、方向和低速位，原生保存和回放通过；触控坐标及落率元数据往返通过。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 已验证 | normal：.thkkdr 版本、原生流哈希和坐标记录检查，实际录制回放及扩展录像播至结束通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 已验证 | normal：多选导入、读取、原字节导出、重命名、删除与刷新保留通过；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 已验证 | normal：播放／暂停／定位期间实时键盘输入隔离通过；触控与手柄隔离另有模块检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待验证 | normal：同一原生录像在模拟 60／120／144／240 Hz 调度下以相同步数结束，最终状态一致；实体高刷全程体验待验收。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 已验证 | normal：存储根 /savesthkkd、五份 DAT、config.txt 与 replay 目录已登记并实际读写；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 已验证 | normal：允许文件导入导出通过，目录外写入请求返回错误；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)、[网页接入验收](../../archive/thkkd-web-20261126/README.md)。 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 已验证 | normal：原生关闭文件后同步，保存录像立即进入 IndexedDB；批量导入后重启校验通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 已验证 | normal：原生设置、存档和 Replay 在退出重开、刷新后保留；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 已验证 | normal：关闭 HTTP 服务并离线刷新，原存档及录像继续可读；[单机运行验收](../../archive/thkkd-quality-mp-20261126/README.md)。 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 已验证 | normal：资源安装事务中断、恢复及版本替换后，存档、配置与 Replay 保持原字节并可重新进入游戏。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |

## 语言与练习

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 不适用 | normal：该构建未提供 thprac；原生 Practice 与 Omniverse 保留在游戏菜单 |
| LANG-01 | [内置中文资源](../../docs/adaptation/localization.md#lang-01) | 已验证 | normal：使用现有内置中文资源，菜单、游戏与对话正常显示，离线启动通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 不适用 | normal：本作使用内置原文（当前中文资源），未提供可选语言包 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 不适用 | normal：本作使用内置原文（当前中文资源），未提供可选语言包 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 不适用 | normal：本作使用内置原文（当前中文资源），未提供可选语言包 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 已验证 | normal：扫描核心、显示常量及网页标记使用的 1091 个字符，随包 Unifont 字形全部覆盖。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 不适用 | normal：本作使用内置原文（当前中文资源），未提供可选语言包 |

## 多人接入与规则

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| MP-RULE-01 | [死亡、幽灵与救援](../../docs/adaptation/multiplayer.md#mp-rule-01) | 不适用 | normal：单人模式 |
| MP-RULE-02 | [个人资源与团队资源](../../docs/adaptation/multiplayer.md#mp-rule-02) | 不适用 | normal：单人模式 |
| MP-RULE-03 | [给予资源与反馈](../../docs/adaptation/multiplayer.md#mp-rule-03) | 不适用 | normal：单人模式 |
| MP-RULE-04 | [对话、结算与全灭](../../docs/adaptation/multiplayer.md#mp-rule-04) | 不适用 | normal：单人模式 |
| MP-RULE-05 | [自机可见性与多人HUD](../../docs/adaptation/multiplayer.md#mp-rule-05) | 不适用 | normal：单人模式 |
| MP-01 | [独立 MP 产品变体](../../docs/adaptation/multiplayer.md#mp-01) | 不适用 | normal：单人模式 |
| MP-02 | [房间人数、难度与机体](../../docs/adaptation/multiplayer.md#mp-02) | 不适用 | normal：单人模式 |
| MP-03 | [所有席位准备并同步开局](../../docs/adaptation/multiplayer.md#mp-03) | 不适用 | normal：单人模式 |
| MP-04 | [确定性输入与状态检查](../../docs/adaptation/multiplayer.md#mp-04) | 不适用 | normal：单人模式 |
| MP-05 | [各席位输入归属](../../docs/adaptation/multiplayer.md#mp-05) | 不适用 | normal：单人模式 |
| MP-06 | [旁观加入与输入隔离](../../docs/adaptation/multiplayer.md#mp-06) | 不适用 | normal：单人模式 |
| MP-07 | [旁观只读取连续已确认历史](../../docs/adaptation/multiplayer.md#mp-07) | 不适用 | normal：单人模式 |
| MP-08 | [慢旁观者与玩家隔离](../../docs/adaptation/multiplayer.md#mp-08) | 不适用 | normal：单人模式 |
| MP-09 | [旁观尾帧与结束时机](../../docs/adaptation/multiplayer.md#mp-09) | 不适用 | normal：单人模式 |
| MP-10 | [联机暂停与重开](../../docs/adaptation/multiplayer.md#mp-10) | 不适用 | normal：单人模式 |
| MP-11 | [多人 Replay](../../docs/adaptation/multiplayer.md#mp-11) | 不适用 | normal：单人模式 |
| MP-12 | [WebRTC 与 Relay 路径](../../docs/adaptation/multiplayer.md#mp-12) | 不适用 | normal：单人模式 |
| MP-13 | [本地玩家可见性](../../docs/adaptation/multiplayer.md#mp-13) | 不适用 | normal：单人模式 |
| MP-14 | [联机与回滚诊断](../../docs/adaptation/multiplayer.md#mp-14) | 不适用 | normal：单人模式 |
| MP-15 | [游戏结束返回房间](../../docs/adaptation/multiplayer.md#mp-15) | 不适用 | normal：单人模式 |

## 验证与交付

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 不适用 | normal：本作没有内置 Demo |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 已验证 | normal：17 份原生录像和两个补充用例共 649645 个逻辑步，逐份记录帧数、结束与停止状态；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 已验证 | normal：60 用例逐项核对原 EXE、两份 DXA、录像与采样内容哈希，逐区域验证固定原始状态内容。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 已验证 | normal：60 用例读取固定 expected，输出字节数与 SHA-256 一致；校验器把结果写入单独运行目录；[固定基准回归](../../archive/thkkd-web-20261126/wasm-regression.json)、[校验入口](scripts/verify-wasm.py)。 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待验证 | normal：固定原版基准已离线核对；重新采集与审核安排在原程序可独立运行的桌面时段。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 进行中 | normal：60 用例累计 835289 逻辑步，23163 个原始采样帧、1637293 个状态区域一致；首个差异采样帧／字段定位与注入反例通过。每个逻辑步均有原版采样的覆盖待补。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 已验证 | normal：正常／诊断构建沿用相同源码清单与数值编译选项；正常 Runtime 的观察接口和 100 次额外绘制保持游戏状态及完整 WASM 内存。 [专项验收](../../archive/thkkd-adaptation-20261127/README.md)。 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 已验证 | normal：状态映射、Replay 元数据与 cursor + 1 == count 结束规则已核对，连续推进及结束后停止通过；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |

<!-- manual-verification -->

## 运行验收记录

r8 在 r7-final 基础上完善 GPU 高刷显示与插帧，结果见[高刷验收](../../archive/thkkd-high-refresh-20261127/README.md)；此前在 `f0013c1c` 基础上补齐网页单机功能，专项结果见[适配验收](../../archive/thkkd-adaptation-20261127/README.md)。测试使用独立站点、专用浏览器数据和后台 Edge，原版资源由实际双 DXA 导入。既有完整录像与文件管理记录继续对应各行已验收范围。

触控事件通过 DevTools 和正式共用控件注入，手柄使用虚拟标准设备及生产映射；高刷新使用 60／120／144／240 Hz 的可重复调度。实体触屏、手柄及高刷显示器的体验单独验收。

### 后续验收范围

| 范围 | 后续工作 |
| --- | --- |
| 实际设备 | 实体手柄、触屏、120／144 Hz 显示器的连续操作，以及其他浏览器引擎 |
| 连续视觉位置 | 九机体及辅助池定点显示已验证；后续结合实体高刷体验观察完整关卡 |
| 原版对照 | 在独立桌面时段重新采集原版，补足逐逻辑步的原始状态覆盖；现有固定采样逐区域对照已通过 |
| 来源登记 | 源码与素材入口按 BUILD-06 继续核对 |

### 本作功能范围

本作没有标题待机自动播放的内置 Demo；VERIFY-01 按产品范围标为不适用。资源通过本地导入安装，PACKAGE-04 的在线安装部分不适用。原生 Practice 与 Omniverse 保留在游戏菜单；PRACTICE-01～08 对应启动器 thprac 扩展接口。原作 Practice 返回后游戏内 Replay 沿用满残满雷的行为按既定要求保留。

详情页从浏览器内 `th_kaikeidou01.dat` 读取九种机体、五种难度的通关进度，区分续关通关和不续关通关。读取和刷新保留原存档字节。Omniverse 成功结算先同步关卡场景，再按原生流程进入保存录像询问；三角色、八挑战的普通游玩及录像返回共 48 个用例通过。
