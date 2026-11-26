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
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 已验证 | normal：25 项 Runtime 文件与 609 项 C++ 输入核对，清单、字节数与哈希一致；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
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
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 待验证 | normal：暂停后 R 重开已实测；常显判定点接在绘图层，开关、移动与碰撞的完整专项验收待补；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[平台绘制](web/platform.mjs)。 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已验证 | normal：双 DXA 与原版 ZIP 转换后进入 Package Store，安装及刷新复用通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已验证 | normal：327 项原始资源逐项校验，生成 301 项运行资源；DATA 身份与 /thkkd.data 挂载声明一致；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)、[公共接口检查](../../docs/adaptation/status.json)。 |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已实现 | normal：共用安装器完整提交后切换版本的模块检查通过；本作更新中断、恢复的浏览器场景待验收；[公共接口检查](../../docs/adaptation/status.json)。 |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 已实现 | normal：本作为 importOnly；双 DXA 与原版 ZIP 共用 Package Store 安装流程，在线安装部分按本作无资源下载入口记录；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 已验证 | normal：首次安装后断网刷新、重开并进入游戏通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已验证 | normal：正式导入窗口统一管理转换、安装及错误反馈，游戏使用已安装资源；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)。 |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 已验证 | normal：默认方向键、射击、Bomb、暂停与菜单确认通过正式 Runtime 检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已验证 | normal：WASD、自定义绑定、保存布局及恢复默认映射通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已实现 | normal：虚拟标准手柄的设备选择、按钮与轴映射通过生产接口检查；实体设备选择待验收；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已实现 | normal：虚拟手柄方向、射击、断开及原生 Key Config 接线通过；实体控制器完整操作待验收；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已实现 | normal：多来源持有与释放通过模块检查，键盘保持时断开手柄通过浏览器检查；键盘与触控组合操作待验收；[本作模块检查](tests/extensions.test.mjs)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 已验证 | normal：浏览器失焦事件和虚拟手柄断开后释放输入，键盘独立持有保留；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 已验证 | normal：原生菜单选择、确认、取消，Z／Enter 确认和对话推进已验证；手柄覆盖采用虚拟设备及模块检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 已验证 | normal：实际触控表面的限速移动、位置变化及取消通过；事件由 DevTools 注入；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待验证 | normal：无限速跟随及实际非零位移标记通过模块检查，完整开关往返操作待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 待验证 | normal：方向式摇杆进入原生方向位图的模块检查通过，四向与斜向逐一浏览器验收待补；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待验证 | normal：自由摇杆向量与幅度进入移动计算的模块检查通过，连续转向画面待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 待验证 | normal：低速按键与独立输入持有已接入；按住、抬手及速度变化待逐项浏览器验收；[输入模块](web/input.mjs)。 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 待验证 | normal：共用控件的低速状态接入本作逻辑按键；连续切换与移动效果待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待验证 | normal：双指低速与单指菜单确认隔离的模块检查通过；移动过程中逐指抬起待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 待验证 | normal：射击、Bomb、暂停按钮序列进入原生输入的模块检查通过；移动与按钮组合操作待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 待验证 | normal：暂时禁移、恢复后沿当前坐标继续控制的同触点检查通过；实际死亡、复活和决死场景待验收；[本作模块检查](tests/extensions.test.mjs)、[公共接口检查](../../docs/adaptation/status.json)。 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待验证 | normal：实际非零无限速移动触发标记，扩展录像处理落率为 100%；真实结算画面的专项验收待补；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待验证 | normal：双击 Bomb 进入原生输入的模块检查通过；移动触点保留的实际操作待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 已验证 | normal：单指确认、双指返回已有模块检查；Boss 对话中的触控短按及长按已实测；[本作模块检查](tests/extensions.test.mjs)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待验证 | normal：持续射击进入对话后，新确认边沿与原生 Replay 推进位的检查通过；结算触控确认待验收；[本作模块检查](tests/extensions.test.mjs)、[单机运行验收](../../archive/thkkd-quality-mp-20261126/README.md)。 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已实现 | normal：100%、200%、300% 距离映射模块检查通过；浏览器已覆盖 200% 操作，其余比例待实测；[本作模块检查](tests/extensions.test.mjs)、[控制器记录](../../archive/thkkd-web-only-20261126/r5-final.json)。 |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待验证 | normal：键盘、手柄与触控使用独立输入归属，多来源释放模块检查通过；组合操作待验收；[本作模块检查](tests/extensions.test.mjs)。 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 已验证 | normal：实际触控移动后取消释放触点，浏览器失焦后清除移动输入；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已验证 | normal：共用编辑器保存布局，横竖屏控件与布局检查通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已实现 | normal：共用重开事件接入原生 R 路径；原生 R 已通过，启动器重开按钮专项操作待验收；[公共接口检查](../../docs/adaptation/status.json)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已验证 | normal：共用双指放大镜缩放与恢复默认大小通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |

## 画面与声音

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已验证 | normal：60／120／144 Hz 模拟序列的原生状态、RNG 和游标一致；普通与解除限频的浏览器逻辑约 60 Hz；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 待验证 | normal：解除浏览器限频后额外呈现已验证；120／144 Hz 实体显示器的呈现间隔与画面体验待验收；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 已验证 | normal：固定状态额外绘制 100 次，完整 WASM 内存、RNG、绘图命令、文件及音频事件保持一致；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 进行中 | normal：普通游玩已接相邻帧位置外推；回放呈现仍使用当前快照，全部连续视觉字段与高刷画面待验收；[呈现入口](web/shell.mjs)、[渲染器](web/renderer.mjs)。 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待验证 | normal：对象身份、出生、切面、瞬移与池复用的绘制模块检查通过；对应实际画面逐项验收待补；[本作模块检查](tests/extensions.test.mjs)。 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 已验证 | normal：原生暂停、恢复，录像暂停时逻辑不动而画面继续呈现通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[单机运行验收](../../archive/thkkd-quality-mp-20261126/README.md)。 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 已验证 | normal：正式启动与首帧检查通过，首帧事件在提交实际绘制后发送；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[呈现入口](web/shell.mjs)。 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待验证 | normal：60／120／144／240 Hz 模拟时钟限制到 60 Hz，以及浏览器约 60 Hz 呈现通过；实体高刷设备切换待验收；[本作模块检查](tests/extensions.test.mjs)、[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-01 | [原版 MP3 音乐](../../docs/adaptation/presentation.md#audio-01) | 已验证 | normal：18 首原版 MP3 保留字节；标题、开局和原生 Music 菜单检查及实际 BGM PCM 输出通过；[r6 导入](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 已验证 | normal：无 BGM 模式与音效独立输出已有媒体验收，r5 复核静音后 BGM PCM 为零；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 已验证 | normal：标题与游戏切换、音乐及视频暂停恢复、原生退出停止和重新进入通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 已验证 | normal：平台挂起与恢复后媒体继续播放，音频上下文恢复及独立增益通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待验证 | normal：生产音频错误事件已有损坏 OGG 注入记录；当前原版 MP3 的损坏、恢复选择场景待验收；[网页接入验收](../../archive/thkkd-web-20261126/README.md)。 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 已验证 | normal：额外绘制 100 次音频事件为零，高频呈现与原生逻辑分开；本作为单机模式；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 待验证 | normal：音乐等待期间音效立即调度的模块检查通过，浏览器菜单音效 PCM 非零；音乐准备期实际输出时延待验收；[本作模块检查](tests/extensions.test.mjs)、[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | normal：本作使用原版 MP3 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | normal：本作使用原版 MP3 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 已验证 | normal：兼容 rep 系统版的原生 .rpy，17 份录像实际连续播放到最后输入；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 已验证 | normal：原生菜单完成录像询问、槽位和署名保存，导出 .rpy 并重新读取／播放通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 待验证 | normal：键盘录制与触控坐标往返已验证，合并输入位图已有模块检查；完整手柄录制专项待验收；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 已验证 | normal：.thkkdr 版本、原生流哈希和坐标记录检查，实际录制回放及扩展录像播至结束通过；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 已验证 | normal：多选导入、读取、原字节导出、重命名、删除与刷新保留通过；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 已验证 | normal：播放／暂停／定位期间实时键盘输入隔离通过；触控与手柄隔离另有模块检查；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[本作模块检查](tests/extensions.test.mjs)。 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待验证 | normal：模拟 60／120／144 Hz 的状态一致性与浏览器暂停、定位已验证；同一完整录像在实体高刷下的对照待验收；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 已验证 | normal：存储根 /savesthkkd、五份 DAT、config.txt 与 replay 目录已登记并实际读写；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 已验证 | normal：允许文件导入导出通过，目录外写入请求返回错误；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)、[网页接入验收](../../archive/thkkd-web-20261126/README.md)。 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 已验证 | normal：原生关闭文件后同步，保存录像立即进入 IndexedDB；批量导入后重启校验通过；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)。 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 已验证 | normal：原生设置、存档和 Replay 在退出重开、刷新后保留；[r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)、[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 已验证 | normal：关闭 HTTP 服务并离线刷新，原存档及录像继续可读；[单机运行验收](../../archive/thkkd-quality-mp-20261126/README.md)。 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待验证 | normal：游戏包与用户数据分开存储；Runtime 升级保留数据已验证，资源包版本替换专项待验收；[单机运行验收](../../archive/thkkd-quality-mp-20261126/README.md)、[存储架构](docs/architecture.md)。 |

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
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 不适用 | normal：本作使用内置原文，本项随可选语言包配方列为不适用。运行字体使用完整 Unifont，字体屏障与参数、实际画面对照已有记录；全流程逐字覆盖待验收；[显示说明](docs/RENDERING.md)。 |
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
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 待核对 | normal：已核对本作无内置演示录像，主菜单等待玩家输入。公共清单将 Demo 定义为必需项，本行保留待核对，待确认无 Demo 作品的标记方式；[主菜单逻辑](src/core/frontend/MenuStart.hpp)。 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 已验证 | normal：17 份原生录像和两个补充用例共 649645 个逻辑步，逐份记录帧数、结束与停止状态；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 待验证 | normal：原始素材、Replay 和状态输出已有哈希；EXE、采样 trace 与这些记录的对应关系待集中核对；[来源登记](../../docs/migration/thkkd-file-import-20261126.md)、[固定基准回归](../../archive/thkkd-web-20261126/wasm-regression.json)。 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 已验证 | normal：60 用例读取固定 expected，输出字节数与 SHA-256 一致；校验器把结果写入单独运行目录；[固定基准回归](../../archive/thkkd-web-20261126/wasm-regression.json)、[校验入口](scripts/verify-wasm.py)。 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待验证 | normal：原版采样与候选校验分开维护，本轮复用 60 用例固定基准；重新采集和审核流程待验收；[网页接入验收](../../archive/thkkd-web-20261126/README.md)、[开发指南](docs/DEVELOPMENT.md)。 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 进行中 | normal：835289 个逻辑步的状态输出字节数与 SHA-256 对照通过；首个差异帧／字段定位及差异注入反例待补；[固定基准回归](../../archive/thkkd-web-20261126/wasm-regression.json)、[校验入口](scripts/verify-wasm.py)。 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 待验证 | normal：normal 与 --probe 分别构建且固定数值选项；正常／诊断构建同输入和观察器只读专项待验收；[构建入口](scripts/build-eagler.mjs)。 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 已验证 | normal：状态映射、Replay 元数据与 cursor + 1 == count 结束规则已核对，连续推进及结束后停止通过；[完整录像与文件管理](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)。 |

<!-- manual-verification -->

## 运行验收记录

当前交付为 r6（`aa5f6e78`），逐项状态依据源码与对应验收记录填写。自动检查记录源码接口与构建身份，浏览器记录实际操作；具体结果与后续验收范围见上表。

r6 的[原版资源导入回归](../../archive/thkkd-web-only-20261126/R6-IMPORT.md)覆盖首次打开导入 ZIP，以及 Service Worker 接管、跨源隔离开启后的双 DXA 导入；两者均显示视频转换百分比，完成 301 项资源安装、进入游戏、刷新和断网重开。游戏运行沿用 [r5 验收](../../archive/thkkd-web-only-20261126/R5-FINAL.md)与[批量导入和完整录像](../../archive/thkkd-web-only-20261126/BATCH-VALIDATION.md)中一致的核心和 Runtime；原始核心的 60 用例、835289 步输出对照见[固定基准回归](../../archive/thkkd-web-20261126/wasm-regression.json)。

浏览器记录来自 Windows x64、Edge 158。触控使用 DevTools 事件，手柄使用虚拟标准设备和生产映射；高刷新覆盖模拟 60／120／144 Hz 时间序列与解除浏览器限频。实体触屏、实体手柄、120／144 Hz 显示器及其他浏览器引擎的体验另列验收。

### 尚需完成的验收

| 范围 | 后续工作 |
| --- | --- |
| 显示与输入 | 实体高刷、手柄与触屏；常显判定点专项；触控各模式、低速、禁移恢复、双击 Bomb、结算及共用重开按钮的完整操作 |
| 连续视觉位置 | 普通游玩的相邻帧位置处理已有实现；回放仍使用当前快照，完整字段覆盖与真实画面验收对应 DISPLAY-04 |
| 安装与存储 | 更新中断、恢复后切换完整资源版本；替换资源包后保留原用户数据 |
| 声音 | 当前 MP3 的资源错误与恢复路径，音乐准备期间音效实际输出时延 |
| 原版对照 | 原程序与采样 trace 身份对应；重新采集流程；首个差异帧／字段定位和已知差异注入；正常与诊断构建对照 |

### 本作功能范围

本作没有标题待机自动播放的内置 Demo；公共清单将 VERIFY-01 定义为必需项，当前保留“待核对”，待统一无 Demo 作品的标记方式。录像验收使用 rep 系统版的原生 .rpy 与网页扩展 .thkkdr，完整样本和结束条件按本作记录维护。

原生 Practice 与 Omniverse 保留在游戏菜单；PRACTICE-01～08 的“不适用”对应启动器 thprac 扩展接口。原作 Practice 返回后游戏内 Replay 沿用满残满雷的行为按既定要求保留。音乐使用原版 MP3，游戏沿用内置中文资源；MIDI、可选翻译包和多人模式按产品范围列为“不适用”。

游戏内部已按[原生存档布局](src/core/frontend/SaveData.hpp)读写通关进度并处理解锁。网页游戏详情页的通关进度表当前由[云端统计视图](../../launcher/app/components/launcher/BookGameStatistics.tsx)提供，本分支尚未接入海恵堂本地存档的通关统计解析；该功能另列为待接入项。
