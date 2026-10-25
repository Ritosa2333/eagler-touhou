# th08 普通版 | 运行验收记录

日期：2026-10-25。按正式检查表逐项关联运行范围、完整证据与后续操作，下一作沿相同方法和作品接口接入。

[适配表](../../ADAPTATION.md) | [结构化记录](adaptation.json) | [共享验收方法](../../../../docs/adaptation/test-methods.md)

## 运行身份

- 审查时源码提交：`fd534069c87a01236d0a83ca1ee952f1a02cfaf8`。提交标识与构建收据共同定位实际输入，带未提交修改的运行以报告中的源文件摘要为准。
- 各次报告保留自己的源码、Runtime、工具与输入身份。下表“已验证”对应所引用报告的完整范围；后续版本通过身份预检后复验。
- 浏览器报告保留各次原采集时间，独立复核记录保留自己的复核时间。原始报告、私有数据、截图和备份留在本地，仓库保存结果摘要与 SHA-256。
- 普通版原生观察构建与正式打包通过来源复核；MP 本轮独立覆盖 BUILD-01 至 BUILD-04。
- 显示与高刷、日英中练习面板两组人工结论来自用户“均ok”。设备参数、实体刷新时序和 Lab 窗口导出材料按实际提供范围记录。

## 结果分布

已验证 82 | 待材料 2 | 部分验证 11 | 不适用 20 | 检查失败 2

“部分验证”逐行区分真实行为范围、完整判据接入和证据材料。通过场景按其实际职责记录，完整编号由对应判据验算。

## 逐项结果

### 源码与构建

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="build-01"></a>BUILD-01 | 源码目录与普通版／MP归属 | 已验证 | 当前工具来源重新执行增量源码构建及正式打包，并以新的构建凭据重新验证工具链、输出和包绑定。 | — | [scope-audit](#evidence-scope-audit)、[build](#evidence-build)、[build-freshness](#evidence-build-freshness)、[current-build](#evidence-current-build)、[rebuilt-current-source](#evidence-rebuilt-current-source)、[current-observer-build](#evidence-current-observer-build) |
| <a id="build-02"></a>BUILD-02 | 工具链与外部依赖 | 已验证 | 当前工具来源重新执行增量源码构建及正式打包，并以新的构建凭据重新验证工具链、输出和包绑定。 | — | [scope-audit](#evidence-scope-audit)、[build](#evidence-build)、[build-freshness](#evidence-build-freshness)、[current-build](#evidence-current-build)、[rebuilt-current-source](#evidence-rebuilt-current-source)、[current-observer-build](#evidence-current-observer-build) |
| <a id="build-03"></a>BUILD-03 | 对应模式独立构建 | 已验证 | 当前工具来源重新执行增量源码构建及正式打包，并以新的构建凭据重新验证工具链、输出和包绑定。 | — | [scope-audit](#evidence-scope-audit)、[build](#evidence-build)、[build-freshness](#evidence-build-freshness)、[current-build](#evidence-current-build)、[rebuilt-current-source](#evidence-rebuilt-current-source)、[current-observer-build](#evidence-current-observer-build) |
| <a id="build-04"></a>BUILD-04 | Runtime 文件与来源记录 | 已验证 | 当前工具来源重新执行增量源码构建及正式打包，并以新的构建凭据重新验证工具链、输出和包绑定。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[audio02](#evidence-audio02)、[recording](#evidence-recording)、[background](#evidence-background)、[package](#evidence-package)、[seven-desktop-input](#evidence-seven-desktop-input)、[seven-touch-actions](#evidence-seven-touch-actions)、[build-freshness](#evidence-build-freshness)、[current-build](#evidence-current-build)、[rebuilt-current-source](#evidence-rebuilt-current-source)、[current-observer-build](#evidence-current-observer-build) |
| <a id="build-05"></a>BUILD-05 | 前端搭配单个游戏运行 | 已验证 | 最小检出、单作构建与 Launcher 安装首帧的独立判据复算通过。 | — | [seven-single-game](#evidence-seven-single-game)、[current-observer-build](#evidence-current-observer-build) |
| <a id="build-06"></a>BUILD-06 | 源码与素材来源 | 待材料 | 已定位原版 EXE、DATA，并核对其 SHA-256 与现有参考一致。 | 待源码作者与许可、DATA／字体来源和独立审阅材料。 | [source-review](#evidence-source-review)、[input-facts](#evidence-input-facts) |

### 启动与资源

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="runtime-01"></a>RUNTIME-01 | 协议身份与会话代次 | 已验证 | 正式包实跑通过；启动顺序、两个代次的协议正负控、官方 Replay 文件管理与新实例恢复／删除的中央完整判据通过，实际读取字节分别保存并重新复核。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[protocol-storage-complete](#evidence-protocol-storage-complete) |
| <a id="runtime-02"></a>RUNTIME-02 | 就绪、配置、启动、首帧 | 已验证 | 正式包实跑通过；启动顺序、两个代次的协议正负控、官方 Replay 文件管理与新实例恢复／删除的中央完整判据通过，实际读取字节分别保存并重新复核。 | — | [scope-audit](#evidence-scope-audit)、[protocol-storage-complete](#evidence-protocol-storage-complete) |
| <a id="runtime-03"></a>RUNTIME-03 | 命令响应与错误信息 | 已验证 | 正式包实跑通过；启动顺序、两个代次的协议正负控、官方 Replay 文件管理与新实例恢复／删除的中央完整判据通过，实际读取字节分别保存并重新复核。 | — | [scope-audit](#evidence-scope-audit)、[protocol-storage-complete](#evidence-protocol-storage-complete) |
| <a id="runtime-04"></a>RUNTIME-04 | 后台、恢复与退出清理 | 已验证 | 真实 Launcher 中关闭默认自动射击后，按住实体方向键与可见触控低速控件，实际标签切后台清空输入并恢复画面和八窗混音 PCM。通过浏览器返回完成旧输入桥释放和文档退役，新 epoch 单次按键生效且宿主／Runtime 监听器数量与位置一致；中央完整判据通过。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[background](#evidence-background)、[launcher-lifecycle-complete](#evidence-launcher-lifecycle-complete)、[launcher-lifecycle-current](#evidence-launcher-lifecycle-current) |
| <a id="runtime-05"></a>RUNTIME-05 | Runtime 与资源管理分工 | 已验证 | 线上安装、实际资源边界、离线重开的对应完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="runtime-06"></a>RUNTIME-06 | 重开与常显判定点 | 部分验证 | 正式 Launcher 关闭／开启／关闭三次启动，原生常显消费、判定点与角色位置对齐、高速保持、低速区分、碰撞盒尺寸、松键静止及同实例暂停重开通过。关闭／低速／常显 PNG 保存实际字节身份，中央复算通过。 | 对应画面组按用户安排稍后集中审查。 | [scope-audit](#evidence-scope-audit)、[native-view-options-machine](#evidence-native-view-options-machine)、[native-view-options-current](#evidence-native-view-options-current) |
| <a id="package-01"></a>PACKAGE-01 | 共用游戏包存储 | 已验证 | 线上安装、实际资源边界、离线重开的对应完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="package-02"></a>PACKAGE-02 | DATA 身份与挂载目标 | 已验证 | 线上安装、实际资源边界、离线重开的对应完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[audio02](#evidence-audio02)、[storage](#evidence-storage)、[recording](#evidence-recording)、[background](#evidence-background)、[seven-desktop-input](#evidence-seven-desktop-input)、[seven-touch-actions](#evidence-seven-touch-actions) |
| <a id="package-03"></a>PACKAGE-03 | 完整更新后切换版本 | 已验证 | 生产安装器下载并暂存真实新增 OGG，再取消第二个 HTTP 200 响应体；浏览器实际请求取消、旧代不变、暂存代及无引用对象清理均通过。旧 Runtime 重新首帧、完整 ZIP 提交新代、配置／成绩／Replay 保留和离线启动实跑通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage)、[staged-http-update-complete](#evidence-staged-http-update-complete) |
| <a id="package-04"></a>PACKAGE-04 | 本地导入与在线安装一致 | 已验证 | 本地 ZIP、在线发布包与声明的原版 DATA 文件分别在独立浏览器存储中安装，每条路径两次实际启动与重开通过。Runtime 固定代次和当前安装分别读取，同一描述符、基础对象、版本、来源及实际 managed DATA 字节通过中央复算；在线可选资源扩充保留基础身份。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage)、[package-import-sources-complete](#evidence-package-import-sources-complete) |
| <a id="package-05"></a>PACKAGE-05 | 已安装游戏离线启动 | 已验证 | 线上安装、实际资源边界、离线重开的对应完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="package-06"></a>PACKAGE-06 | 统一安装和更新入口 | 已验证 | 安装、更新提示与 Runtime 消费代次的完整判据复算通过。 | — | [seven-installation-authority](#evidence-seven-installation-authority) |

### 键盘、手柄与触控

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="input-01"></a>INPUT-01 | 键盘逻辑输入 | 已验证 | 原生键盘、WASD、自定义持久布局、恢复默认和手柄输入同批实跑通过；Dpad 与摇杆各八向、原生动作、原作菜单和对话由共享判据复算。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[native-keyboard-complete](#evidence-native-keyboard-complete)、[native-inputs-complete](#evidence-native-inputs-complete) |
| <a id="input-02"></a>INPUT-02 | 启动器键盘改键 | 已验证 | 原生键盘、WASD、自定义持久布局、恢复默认和手柄输入同批实跑通过；Dpad 与摇杆各八向、原生动作、原作菜单和对话由共享判据复算。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[keyboard-layouts-original-export](#evidence-keyboard-layouts-original-export)、[native-inputs-complete](#evidence-native-inputs-complete) |
| <a id="input-03"></a>INPUT-03 | 启动器手柄映射 | 已验证 | Launcher 生产手柄映射和席位归属观察通过，完整判据复算通过。 | — | [desktop](#evidence-desktop)、[seven-desktop-input](#evidence-seven-desktop-input) |
| <a id="input-04"></a>INPUT-04 | 手柄方向与按钮 | 已验证 | 原生键盘、WASD、自定义持久布局、恢复默认和手柄输入同批实跑通过；Dpad 与摇杆各八向、原生动作、原作菜单和对话由共享判据复算。 | — | [scope-audit](#evidence-scope-audit)、[native-inputs-complete](#evidence-native-inputs-complete) |
| <a id="input-05"></a>INPUT-05 | 多种输入独立持有 | 已验证 | 正式 Runtime 射击／Bomb／暂停、键盘／手柄／触控交叉持有和原生重开实跑通过；输入清理保持当前代次的动作序号，新 DOM 控制器按新实例接入。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[actions-complete](#evidence-actions-complete) |
| <a id="input-06"></a>INPUT-06 | 失焦与断开释放 | 已验证 | 真实窗口最小化及同窗口换页、触控取消和手柄断连／重连实跑通过；持有输入清空、重连中立门槛和恢复后的原生移动逐项复算。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[background](#evidence-background)、[background-inputs-complete](#evidence-background-inputs-complete) |
| <a id="input-07"></a>INPUT-07 | 原作菜单与对话输入 | 已验证 | 本作实际菜单、确认、取消和对话输入观察通过，完整判据复算通过。 | — | [desktop](#evidence-desktop)、[seven-desktop-input](#evidence-seven-desktop-input) |
| <a id="touch-01"></a>TOUCH-01 | 限速滑动移动 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-complete](#evidence-motion-complete)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-02"></a>TOUCH-02 | 无限速滑动移动 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-complete](#evidence-motion-complete)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-03"></a>TOUCH-03 | 方向式虚拟摇杆 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-04"></a>TOUCH-04 | 自由方向虚拟摇杆 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-complete](#evidence-motion-complete)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-05"></a>TOUCH-05 | 按住低速 | 已验证 | 生产 DOM 焦点按钮的按住、切换及切换配置重载实跑通过；三个独立实例各自用键盘高速与 Shift 低速校准，再比较原生焦点、位移、持续持有、释放和取消。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[focus-buttons-complete](#evidence-focus-buttons-complete) |
| <a id="touch-06"></a>TOUCH-06 | 切换低速 | 已验证 | 生产 DOM 焦点按钮的按住、切换及切换配置重载实跑通过；三个独立实例各自用键盘高速与 Shift 低速校准，再比较原生焦点、位移、持续持有、释放和取消。 | — | [scope-audit](#evidence-scope-audit)、[focus-buttons-complete](#evidence-focus-buttons-complete) |
| <a id="touch-07"></a>TOUCH-07 | 双指低速 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-08"></a>TOUCH-08 | 射击、Bomb 与暂停 | 已验证 | 正式 Runtime 射击／Bomb／暂停、键盘／手柄／触控交叉持有和原生重开实跑通过；输入清理保持当前代次的动作序号，新 DOM 控制器按新实例接入。 | — | [scope-audit](#evidence-scope-audit)、[bomb-resource-rechecked](#evidence-bomb-resource-rechecked)、[actions-complete](#evidence-actions-complete) |
| <a id="touch-09"></a>TOUCH-09 | Deathbomb 与移动保持 | 已验证 | 当前产物真实碰撞后三条路线实跑通过；键盘与触控末帧有效、迟到脉冲经过自然死亡和复活保持无 Bomb，同一移动触点恢复并遵守命中前实测速度，输入与清理回执绑定本次代次。 | — | [scope-audit](#evidence-scope-audit)、[touch-last-window](#evidence-touch-last-window)、[deathbomb-complete](#evidence-deathbomb-complete) |
| <a id="touch-10"></a>TOUCH-10 | 无限速移动落率记录 | 已验证 | 普通录制局零位移对照、实际无限速移动、跨关标记、生产结算与保存 Replay 实跑通过；五个状态边界、实际移动和独立解码的 100% 落率由中央完整判据重算。 | — | [scope-audit](#evidence-scope-audit)、[touch-other-actions](#evidence-touch-other-actions)、[unlimited-result-complete](#evidence-unlimited-result-complete) |
| <a id="touch-11"></a>TOUCH-11 | 双击 Bomb | 已验证 | 双击 Bomb 启用／关闭与保持射击后的对话确认实跑通过，指针批次、原生消费、Bomb 消耗与新的确认边沿由共享判据重算。 | — | [scope-audit](#evidence-scope-audit)、[touch-other-actions](#evidence-touch-other-actions)、[bomb-resource-rechecked](#evidence-bomb-resource-rechecked)、[gesture-complete](#evidence-gesture-complete) |
| <a id="touch-12"></a>TOUCH-12 | 菜单与对话手势 | 已验证 | 生产触控菜单／对话手势完整判据复算通过。 | — | [seven-touch-actions](#evidence-seven-touch-actions) |
| <a id="touch-13"></a>TOUCH-13 | 确认键的新按下边沿 | 已验证 | 双击 Bomb 启用／关闭与保持射击后的对话确认实跑通过，指针批次、原生消费、Bomb 消耗与新的确认边沿由共享判据重算。 | — | [scope-audit](#evidence-scope-audit)、[touch-other-actions](#evidence-touch-other-actions)、[gesture-complete](#evidence-gesture-complete) |
| <a id="touch-14"></a>TOUCH-14 | 100–300% 灵敏度 | 已验证 | 当前包六项共用运动验收同批实跑通过；中央判据复算连续原生轨迹、固定输入向量、八向方向位、限速恢复、双指低速与灵敏度。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[motion-complete](#evidence-motion-complete)、[motion-rechecked-complete](#evidence-motion-rechecked-complete) |
| <a id="touch-15"></a>TOUCH-15 | 触控与其他输入隔离 | 已验证 | 正式 Runtime 射击／Bomb／暂停、键盘／手柄／触控交叉持有和原生重开实跑通过；输入清理保持当前代次的动作序号，新 DOM 控制器按新实例接入。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[actions-complete](#evidence-actions-complete) |
| <a id="touch-16"></a>TOUCH-16 | 触控取消与生命周期 | 已验证 | 真实窗口最小化及同窗口换页、触控取消和手柄断连／重连实跑通过；持有输入清空、重连中立门槛和恢复后的原生移动逐项复算。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[background-inputs-complete](#evidence-background-inputs-complete) |
| <a id="touch-17"></a>TOUCH-17 | 共用布局编辑器 | 已验证 | 共用布局编辑器的生产交互及恢复完整判据复算通过。 | — | [seven-touch-actions](#evidence-seven-touch-actions) |
| <a id="touch-18"></a>TOUCH-18 | 共用重开按钮 | 已验证 | 正式 Runtime 射击／Bomb／暂停、键盘／手柄／触控交叉持有和原生重开实跑通过；输入清理保持当前代次的动作序号，新 DOM 控制器按新实例接入。 | — | [scope-audit](#evidence-scope-audit)、[actions-complete](#evidence-actions-complete) |
| <a id="touch-19"></a>TOUCH-19 | 共用双指放大镜 | 已验证 | 共用双指放大镜的生产交互及退出完整判据复算通过。 | — | [seven-touch-actions](#evidence-seven-touch-actions) |

### 画面与声音

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="display-01"></a>DISPLAY-01 | 固定逻辑步进 | 已验证 | 同 Replay 在不同呈现节奏下的逻辑、RNG、时钟和输入轨迹完整判据复算通过。 | — | [display](#evidence-display) |
| <a id="display-02"></a>DISPLAY-02 | 跟随显示器刷新率 | 部分验证 | 限帧开关的原生提交／逻辑步数已验证，用户高刷与显示观感结论通过。 | 实体刷新时序、设备参数与 Lab 窗口导出记录尚未提供。 | [display](#evidence-display)、[limiter](#evidence-limiter)、[human-confirmation](#evidence-human-confirmation) |
| <a id="display-03"></a>DISPLAY-03 | 额外绘制无逻辑副作用 | 已验证 | 重复绘制权威状态、生命周期及音频操作纯度的完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[display](#evidence-display) |
| <a id="display-04"></a>DISPLAY-04 | 连续视觉字段插值 | 部分验证 | 相应对象、边界、暂停的机器证据复算通过，用户显示审查结论通过。 | 用户结论已登记；Lab 窗口素材集导出记录尚未随回复提供。 | [display](#evidence-display)、[human-confirmation](#evidence-human-confirmation) |
| <a id="display-05"></a>DISPLAY-05 | 出生、销毁和瞬移 | 部分验证 | 相应对象、边界、暂停的机器证据复算通过，用户显示审查结论通过。 | 用户结论已登记；Lab 窗口素材集导出记录尚未随回复提供。 | [display](#evidence-display)、[human-confirmation](#evidence-human-confirmation) |
| <a id="display-06"></a>DISPLAY-06 | 暂停画面的表现 | 部分验证 | 相应对象、边界、暂停的机器证据复算通过，用户显示审查结论通过。 | 用户结论已登记；Lab 窗口素材集导出记录尚未随回复提供。 | [display](#evidence-display)、[human-confirmation](#evidence-human-confirmation) |
| <a id="display-07"></a>DISPLAY-07 | 实际呈现后报告首帧 | 已验证 | 正式包冷启动／刷新后的实际合成画面与 first-frame 时序完整判据复算通过。 | — | [display](#evidence-display) |
| <a id="display-08"></a>DISPLAY-08 | 限制到 60 Hz | 部分验证 | 限帧开关的原生提交／逻辑步数已验证，用户高刷与显示观感结论通过。 | 实体刷新时序、设备参数与 Lab 窗口导出记录尚未提供。 | [display](#evidence-display)、[limiter](#evidence-limiter)、[human-confirmation](#evidence-human-confirmation) |
| <a id="audio-01"></a>AUDIO-01 | 正常 OGG 音乐 | 已验证 | 当前正式包的 OGG 标题／首关、流式／完整解码、MIDI 新曲实际音符和输出、缺失／损坏诊断、生产 Launcher 设置与恢复实跑通过；中央重新读取原始源文件、实际交付字节、消息和采样窗口。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[retained-state](#evidence-retained-state)、[music-source-and-output-complete](#evidence-music-source-and-output-complete) |
| <a id="audio-02"></a>AUDIO-02 | 无 BGM 模式 | 已验证 | 同一正式 Runtime 的菜单音效正向、实测 PCM 队尾延迟负控和无 BGM 真实射击实跑通过；中央判据复算源文件映射、原生子弹消费、原始起播记录和可归属 PCM，并重新读取静音基线与射击采样文件。 | — | [scope-audit](#evidence-scope-audit)、[desktop](#evidence-desktop)、[audio02](#evidence-audio02)、[audio-no-bgm-complete](#evidence-audio-no-bgm-complete) |
| <a id="audio-03"></a>AUDIO-03 | 切曲、渐变、暂停、停止 | 已验证 | stream／full 正式包的原生暂停恢复、同局暂停后 R 重开、退出再启动与实际源绑定 PCM 通过；21 曲 Music Room、自然循环、淡出和 DATA 独立解析的 Boss 切曲共同验算。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[retained-state](#evidence-retained-state)、[native-ogg-lifecycle-complete](#evidence-native-ogg-lifecycle-complete) |
| <a id="audio-04"></a>AUDIO-04 | 返回前台后恢复声音 | 已验证 | 完整 Chrome 同窗口真实标签切换中，Host／Runtime 的可信隐藏与恢复事件、原生曲目游标和实际源绑定 PCM 实跑通过；stream／full 两条解码策略均由中央判据复算。Windows 窗口遮挡推断的标签隔离配置随浏览器参数保存。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[background](#evidence-background)、[native-music-background-complete](#evidence-native-music-background-complete) |
| <a id="audio-05"></a>AUDIO-05 | 音乐资源错误信息 | 已验证 | 当前正式包的 OGG 标题／首关、流式／完整解码、MIDI 新曲实际音符和输出、缺失／损坏诊断、生产 Launcher 设置与恢复实跑通过；中央重新读取原始源文件、实际交付字节、消息和采样窗口。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[music-source-and-output-complete](#evidence-music-source-and-output-complete) |
| <a id="audio-06"></a>AUDIO-06 | 高刷新与音频事件分开 | 已验证 | 高刷及回放窗口的音频事件计数、来源与守恒完整判据复算通过。 | — | [display](#evidence-display) |
| <a id="midi-01"></a>MIDI-01 | 声明 MIDI 选项 | 已验证 | 当前正式包的 OGG 标题／首关、流式／完整解码、MIDI 新曲实际音符和输出、缺失／损坏诊断、生产 Launcher 设置与恢复实跑通过；中央重新读取原始源文件、实际交付字节、消息和采样窗口。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[music-source-and-output-complete](#evidence-music-source-and-output-complete) |
| <a id="midi-02"></a>MIDI-02 | 可听到的 MIDI 输出 | 已验证 | 当前正式包的 OGG 标题／首关、流式／完整解码、MIDI 新曲实际音符和输出、缺失／损坏诊断、生产 Launcher 设置与恢复实跑通过；中央重新读取原始源文件、实际交付字节、消息和采样窗口。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[music-source-and-output-complete](#evidence-music-source-and-output-complete) |
| <a id="midi-03"></a>MIDI-03 | MIDI 生命周期 | 已验证 | 原生标题／开局曲目、暂停时序、同局 R 重开、真实后台恢复、退出静音与标题重启实跑通过。恢复按同一合成器与 tap 的完整分析窗分阶段采样，保留过渡状态与实际 PCM。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[native-midi-lifecycle-complete](#evidence-native-midi-lifecycle-complete) |
| <a id="midi-04"></a>MIDI-04 | MIDI 与 OGG 分开 | 已验证 | 同一个 Chrome 存储环境中正常退出 MIDI，关闭原页面后读取原 MIDI 偏好并切换 OGG；两个真实页面 target、同一 browserContextId、退出消息、同一安装代次和各自输出实跑通过。 | — | [scope-audit](#evidence-scope-audit)、[music](#evidence-music)、[music-mode-change-complete](#evidence-music-mode-change-complete) |

### 存档与 Replay

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="replay-01"></a>REPLAY-01 | 导入原版 Replay | 部分验证 | 正式 Shell 导入与原生 Replay 推进已有观察。 | 同一 Replay 哈希的独立自然结束游标待原版复验。 | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state) |
| <a id="replay-02"></a>REPLAY-02 | 按原版格式导出可表达输入 | 已验证 | WASD、自定义布局持久保存和默认恢复的三个实例逐边沿及原生动作实跑通过；原版 Replay 导出单独保留文件，校验 v6 原始块与 USER 信息，并重新对照键盘、手柄的实际录制帧。 | — | [scope-audit](#evidence-scope-audit)、[recording](#evidence-recording)、[keyboard-layouts-original-export](#evidence-keyboard-layouts-original-export) |
| <a id="replay-03"></a>REPLAY-03 | 记录实际生效输入 | 已验证 | 真实录制和四条回放扰动路线实跑通过；中央判据重开保存文件，独立解析每帧，逐条核对键盘方向／射击、手柄和触控实际进入、释放及玩家消费隔离。 | — | [scope-audit](#evidence-scope-audit)、[recording](#evidence-recording)、[recording-complete](#evidence-recording-complete)、[replay-isolation-complete](#evidence-replay-isolation-complete) |
| <a id="replay-04"></a>REPLAY-04 | 扩展格式与版本 | 已验证 | 同一真实录制旅程独立解析有效 v2 指针扩展并核对原生逐点消费，再只改版本字段，通过实际 Shell 请求验证明确拒绝；后续有效命令、文件列表和原有效录像字节保持正常。 | — | [scope-audit](#evidence-scope-audit)、[recording](#evidence-recording)、[replay-extension-version-complete](#evidence-replay-extension-version-complete) |
| <a id="replay-05"></a>REPLAY-05 | Replay 文件管理 | 已验证 | 正式包实跑通过；启动顺序、两个代次的协议正负控、官方 Replay 文件管理与新实例恢复／删除的中央完整判据通过，实际读取字节分别保存并重新复核。 | — | [scope-audit](#evidence-scope-audit)、[protocol-storage-complete](#evidence-protocol-storage-complete) |
| <a id="replay-06"></a>REPLAY-06 | 回放与实时输入隔离 | 已验证 | 真实录制和四条回放扰动路线实跑通过；中央判据重开保存文件，独立解析每帧，逐条核对键盘方向／射击、手柄和触控实际进入、释放及玩家消费隔离。 | — | [scope-audit](#evidence-scope-audit)、[recording](#evidence-recording)、[replay-isolation-complete](#evidence-replay-isolation-complete) |
| <a id="replay-07"></a>REPLAY-07 | 回放独立于显示刷新率 | 已验证 | 正式 Replay 在三种呈现节奏下的权威状态与输入消费完整判据复算通过。 | — | [display](#evidence-display) |
| <a id="save-01"></a>SAVE-01 | 已声明的存储根 | 已验证 | 同一正式包的原生成绩、配置和 Replay 在真实 IDBFS 根读取并逐字节比较；目录外 read、write、remove 的请求及明确拒绝、操作前后文件列表与挂载外探针实跑通过。 | — | [scope-audit](#evidence-scope-audit)、[native-user-paths-complete](#evidence-native-user-paths-complete) |
| <a id="save-02"></a>SAVE-02 | 用户文件允许范围 | 已验证 | 同一正式包的原生成绩、配置和 Replay 在真实 IDBFS 根读取并逐字节比较；目录外 read、write、remove 的请求及明确拒绝、操作前后文件列表与挂载外探针实跑通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage)、[native-user-paths-complete](#evidence-native-user-paths-complete) |
| <a id="save-03"></a>SAVE-03 | 明确持久化时机 | 已验证 | 配置、成绩和 Replay 经真实原生操作写入、同步、新实例恢复及离线读取，完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="save-04"></a>SAVE-04 | 退出重开后数据保留 | 已验证 | 配置、成绩和 Replay 经真实原生操作写入、同步、新实例恢复及离线读取，完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="save-05"></a>SAVE-05 | 离线读取已有数据 | 已验证 | 配置、成绩和 Replay 经真实原生操作写入、同步、新实例恢复及离线读取，完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |
| <a id="save-06"></a>SAVE-06 | 游戏包与用户数据分开 | 已验证 | 配置、成绩和 Replay 经真实原生操作写入、同步、新实例恢复及离线读取，完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[storage](#evidence-storage) |

### 语言与练习

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="practice-01"></a>PRACTICE-01 | 发布包中的练习能力 | 已验证 | 正式启用包下，真实 Launcher 用户开启、用户关闭与 Host 关闭三场景通过，设置、configure 和原生菜单相符。 | 当前完整判据尚需把产品上限、发布包关闭的合法对照与已测 Host／UI 矩阵共同核对。 | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[practice-capability-complete](#evidence-practice-capability-complete) |
| <a id="practice-02"></a>PRACTICE-02 | 开局前启用与语言 | 已验证 | 真实 Launcher 预启动配置与四个普通版实例的原生首帧实跑通过；日英中启用及英语关闭分别核对 configure、launch、first-frame 顺序与原生开关。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[practice-startup-complete](#evidence-practice-startup-complete) |
| <a id="practice-03"></a>PRACTICE-03 | 游戏内练习状态 | 已验证 | 当前产物四种 locale／开关实例及同一原生世界的重开、标题、Replay 主动退出、普通开局和再次练习实跑通过，中央完整判据重算每个状态边界。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[practice-lifecycle-complete](#evidence-practice-lifecycle-complete)、[practice-lifecycle-current](#evidence-practice-lifecycle-current) |
| <a id="practice-04"></a>PRACTICE-04 | 关卡与资源参数 | 已验证 | 11 个冷启动参数案例通过，资源上下界、关卡、时间线、章节、对话、阶段与特殊定点分别核对冻结初始化和实际 ECL 字节。菜单确认前核对原生输入已武装、松键和完成绘制。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[native-practice-parameters-complete](#evidence-native-practice-parameters-complete)、[practice-input-regression](#evidence-practice-input-regression)、[practice-parameter-current](#evidence-practice-parameter-current) |
| <a id="practice-05"></a>PRACTICE-05 | 练习 Replay 元数据 | 已验证 | 保存文件经独立解析、持久化、新实例恢复和原生 Replay 消费，完整判据复算通过。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[practice-lifecycle-current](#evidence-practice-lifecycle-current) |
| <a id="practice-06"></a>PRACTICE-06 | 触控功能键与指针 | 已验证 | 正式 Launcher 安装当前产物后进入同一原生 Practice 无出怪场景，F1–F7／U 实体键改变原生状态，真实浏览器触摸可见控件恢复原值，其他开关保持不变。Tab／F12 与移动 Tracker／Advanced 展开、可信 Pointer 坐标、同局零写入和最终释放通过中央原始证据复算。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[native-practice-controls-complete](#evidence-native-practice-controls-complete)、[practice-controls-current](#evidence-practice-controls-current) |
| <a id="practice-07"></a>PRACTICE-07 | 练习语言跟随设置 | 已验证 | 原生日英中菜单开闭、locale、真实 Unifont 交付与三张原始截图身份复算通过，并关联用户已确认的整组文字结论。保留原采集时间及包／DATA 身份，复算未重新运行浏览器。 | — | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[human-confirmation](#evidence-human-confirmation)、[reviewed-practice-locales](#evidence-reviewed-practice-locales) |
| <a id="practice-08"></a>PRACTICE-08 | 练习界面的可用功能 | 部分验证 | 训练资源 9 场景、高级输入 4 组、按键／APS／CSV、残机边界、自动重开与一键死亡，以及真实 CPU 数学／表文件往返、弹体矩形／许可和 Boss 锁时文字均有实跑证据。 | Practice 后续专项按用户安排暂缓。 | [scope-audit](#evidence-scope-audit)、[practice](#evidence-practice)、[native-practice-controls-complete](#evidence-native-practice-controls-complete)、[native-practice-parameters-complete](#evidence-native-practice-parameters-complete)、[practice-trainer-effects](#evidence-practice-trainer-effects)、[practice-input-options](#evidence-practice-input-options)、[practice-boss-effects](#evidence-practice-boss-effects)、[practice-key-monitor](#evidence-practice-key-monitor)、[practice-consumer-build](#evidence-practice-consumer-build)、[practice-mapped-life](#evidence-practice-mapped-life)、[practice-lifecycle-options](#evidence-practice-lifecycle-options)、[practice-math](#evidence-practice-math)、[practice-display-tools](#evidence-practice-display-tools)、[practice-lock-text](#evidence-practice-lock-text) |
| <a id="lang-01"></a>LANG-01 | 内置日语原文 | 已验证 | 正式包经已安装 App Shell 在断网新页面启动，日语标题、Result 统计和真实加载的 MSG 对白通过原文字节、字形与呈现核验。同一原生实例完成菜单进入、返回与对白操作。 | — | [scope-audit](#evidence-scope-audit)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes) |
| <a id="lang-02"></a>LANG-02 | 语言包身份与发布清单 | 已验证 | 日英中正式选择器、持久语言偏好、本次发布归档与实际挂载成员逐一核对，归档成员、字节数和 SHA-256 由中央重新读取。 | — | [scope-audit](#evidence-scope-audit)、[language-complete](#evidence-language-complete)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes) |
| <a id="lang-03"></a>LANG-03 | 开局前挂载翻译与字体 | 已验证 | 所选翻译与字体在原生世界构造前配置，标题、Result 统计及实际翻译 MSG 的源字节、真实字形和后续呈现通过独立核验。 | — | [scope-audit](#evidence-scope-audit)、[language-complete](#evidence-language-complete)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes) |
| <a id="lang-04"></a>LANG-04 | 语言包失败时回到原文 | 已验证 | 独立冷存储分别注入实际 404 与同长度错哈希归档，本次返回原文并保留原语言偏好，响应来源与控制字节通过中央复算。 | — | [scope-audit](#evidence-scope-audit)、[language-complete](#evidence-language-complete)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes) |
| <a id="lang-05"></a>LANG-05 | 字体字符覆盖 | 部分验证 | 日英中声明的标题、统计与对白字符样本均通过源成员、码点、真实字形来源、文字透明度与呈现核验。六张统计／对白图片分别绑定同一次原生实例与哈希。 | 对应画面组按用户安排稍后集中审查。 | [scope-audit](#evidence-scope-audit)、[human-confirmation](#evidence-human-confirmation)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes)、[language-evidence-negatives](#evidence-language-evidence-negatives) |
| <a id="lang-06"></a>LANG-06 | 缺译资源使用原文 | 已验证 | 有效部分翻译包保留的键显示本次译文，缺失键与独立离线日语基线一致，两条路径均经过实际栅格、字形与呈现。 | — | [scope-audit](#evidence-scope-audit)、[language-complete](#evidence-language-complete)、[language-consumers-complete](#evidence-language-consumers-complete)、[language-native-scenes](#evidence-language-native-scenes) |

### 多人接入与规则

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="mp-rule-01"></a>MP-RULE-01 | 死亡、幽灵与救援 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-rule-02"></a>MP-RULE-02 | 个人资源与团队资源 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-rule-03"></a>MP-RULE-03 | 给予资源与反馈 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-rule-04"></a>MP-RULE-04 | 对话、结算与全灭 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-rule-05"></a>MP-RULE-05 | 自机可见性与多人HUD | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-01"></a>MP-01 | 独立 MP 产品变体 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-02"></a>MP-02 | 房间人数、难度与机体 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-03"></a>MP-03 | 所有席位准备并同步开局 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-04"></a>MP-04 | 确定性输入与状态检查 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-05"></a>MP-05 | 各席位输入归属 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-06"></a>MP-06 | 旁观加入与输入隔离 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-07"></a>MP-07 | 旁观只读取连续已确认历史 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-08"></a>MP-08 | 慢旁观者与玩家隔离 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-09"></a>MP-09 | 旁观尾帧与结束时机 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-10"></a>MP-10 | 联机暂停与重开 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-11"></a>MP-11 | 多人 Replay | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-12"></a>MP-12 | WebRTC 与 Relay 路径 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-13"></a>MP-13 | 本地玩家可见性 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-14"></a>MP-14 | 联机与回滚诊断 | 不适用 | 普通版为单人模式。 | — | 单人模式 |
| <a id="mp-15"></a>MP-15 | 游戏结束返回房间 | 不适用 | 普通版为单人模式。 | — | 单人模式 |

### 验证与交付

| 编号 | 检查项 | 验收记录 | 已验证范围 | 后续操作 | 证据 |
| --- | --- | --- | --- | --- | --- |
| <a id="verify-01"></a>VERIFY-01 | 全部内置 Demo 对照 | 检查失败 | 保留比较中 demo3 一致，demo0／1／2 存在差异。 | 按用户安排稍后做原版复验，基准保持原值。 | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state) |
| <a id="verify-02"></a>VERIFY-02 | 长 Replay 样本 | 检查失败 | 保留比较中 Lunatic／Extra 存在差异。 | 按用户安排稍后做原版复验，基准保持原值。 | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state) |
| <a id="verify-03"></a>VERIFY-03 | 原版基准与内容哈希 | 部分验证 | 原版 EXE 和 DATA 哈希与 golden 一致，样本及 trace 各有字节身份。 | 独立来源、采集与接纳记录待补。 | [scope-audit](#evidence-scope-audit)、[input-facts](#evidence-input-facts)、[source-review](#evidence-source-review) |
| <a id="verify-04"></a>VERIFY-04 | 读取固定基准 | 已验证 | Quick 4 份与 Daily 2 份输入的原采集前后快照独立复核通过，完整 7 份 golden 文件、声明外部输入、原候选队列和 gate 均绑定原字节。新增／删除／改写参考反例及中央判据通过，统一入口完整接纳；续跑保留原结果。游戏对照差异在 VERIFY-01／02 单独记录。 | — | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state)、[reference-guard-negatives](#evidence-reference-guard-negatives)、[reference-integrity-complete](#evidence-reference-integrity-complete)、[reference-integrity-resume](#evidence-reference-integrity-resume) |
| <a id="verify-05"></a>VERIFY-05 | 单独维护原版基准 | 待材料 | 已定位与 golden 哈希一致的原版输入。 | 待独立采集、staging 维护与单独接纳审查记录。 | [source-review](#evidence-source-review) |
| <a id="verify-06"></a>VERIFY-06 | 逐逻辑帧比较 | 已验证 | 原始 Quick 4 份与 Daily 2 份捕获由当前共用比较器复算，两条 lane 各 14 个实际参考窗口反例通过。首差帧、字段、漏帧、重复、错位及各状态类别由统一入口和中央流式判据重新核对；7 个伪造证据反例拒收，续跑保持原采集时间与结果。游戏比较差异在 VERIFY-01／02 保留。 | — | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state)、[replay-method-complete](#evidence-replay-method-complete)、[replay-method-resume](#evidence-replay-method-resume)、[replay-method-guards](#evidence-replay-method-guards)、[replay-current-regressions](#evidence-replay-current-regressions) |
| <a id="verify-07"></a>VERIFY-07 | 诊断构建与正常构建 | 已验证 | 正式包与诊断构建同输入观察、读取纯度与生产 Replay 行为的完整判据复算通过。 | — | [display](#evidence-display) |
| <a id="verify-08"></a>VERIFY-08 | 本作状态映射与结束条件 | 部分验证 | 逐帧账本、输入与 timing 游标、状态映射及首差异定位已有记录。编译实际 ReplayPlayback.cpp 验证重入上限 15 帧；账本拒绝 16 帧、游标脱离和应用步缺口。普通版与 MP 共用原始字段判据，拒绝非法整数、非有限坐标、截短状态块及冻结帧中的无效字段。 | 本作状态映射的完整证据判据待接入；Quick／Daily 原版差异与独立 Replay 自然终点按用户安排稍后验证。 | [scope-audit](#evidence-scope-audit)、[retained-state](#evidence-retained-state)、[replay-current-regressions](#evidence-replay-current-regressions) |

## 证据身份

本地归档按下列相对标识定位，JSON 保存字节数、时间和 SHA-256；原始输入继续沿用本地清单。

| 标识 | 原报告 | 时间 | SHA-256 |
| --- | --- | --- | --- |
| <a id="evidence-scope-audit"></a>scope-audit | `remaining-scope-audit.json` | 见原报告 | `a16dd7a31bb91a27b0b0bc75d476276a8f12e6d2209eaff2f51998b8419c165e` |
| <a id="evidence-desktop"></a>desktop | `main-validation-v75/adaptation/adaptation/desktop-input-normal-th08/report.json` | 2026-10-24T05:12:42.044Z | `31903075da468516d133457631fee9b10d9075e6850817f3d7963fd3f74db5ab` |
| <a id="evidence-practice"></a>practice | `main-validation-v75/adaptation/adaptation/practice-normal-th08/report.json` | 2026-10-24T05:14:30.357Z | `4e89e18e56e881795c6a35fa55a6838beaeef9649755e59ca13f02ea5a380268` |
| <a id="evidence-display"></a>display | `main-validation-v75/adaptation/adaptation/display-replay-normal-th08/report.json` | 2026-10-24T05:25:40.754Z | `a64656aaed64815cff494bd0357140bb448b69f306d824d3f04d86cccaf50fd7` |
| <a id="evidence-audio02"></a>audio02 | `focused-followups-v76/audio02/adaptation/desktop-input-normal-th08/report.json` | 2026-10-24T05:35:18.553Z | `d870dc11236097729f874c9fa756e611b59508090d901b8c4fad8ee0e10a9769` |
| <a id="evidence-music"></a>music | `focused-followups-v76/music/adaptation/music-normal-th08/report.json` | 2026-10-24T05:40:45.619Z | `ef9e3b9249f7eded582beff0c7f48148c5a18148e11774caa47522fb02ca4831` |
| <a id="evidence-storage"></a>storage | `remaining-normal-v16/storage/report.json` | 2026-10-23T12:59:05.218Z | `a10b5cc0b84028ee3d65ad98934dac01f1f2535c52e046dc80923527a844d612` |
| <a id="evidence-recording"></a>recording | `remaining-normal-v16/recording/report.json` | 2026-10-23T12:58:16.846Z | `92c5a3e30432776bc2966b5914fe9400ce1215464de9f611bfcd0e1b283c570e` |
| <a id="evidence-background"></a>background | `remaining-input-clear-v17/report.json` | 2026-10-23T13:06:01.019Z | `81cb25563835f94f5fa2614df35997e5890e6e7e2db07de210895025537859eb` |
| <a id="evidence-build"></a>build | `main-validation-v75/normal/source-build/report.json` | 2026-10-24T05:08:44.710Z | `7dfa85e6764586211d1d531332bef7aacfd60ebf665dc6c2a46754e0b4f3a229` |
| <a id="evidence-package"></a>package | `main-validation-v75/normal/package/report.json` | 2026-10-24T05:08:47.921Z | `8074edc98282b396a4446fd11ef5ca045c09a19cace65f3c59f7cbf89e6cebf7` |
| <a id="evidence-seven-desktop-input"></a>seven-desktop-input | `seven-current-final/adaptation/desktop-input-normal-th08/report.json` | 2026-10-23T08:33:58.388Z | `1944958768ec1fc583c5aacdc8784335bc3089a475be654154167bafe55d323d` |
| <a id="evidence-seven-touch-actions"></a>seven-touch-actions | `seven-current-final/adaptation/touch-actions-normal-th08/report.json` | 2026-10-23T08:35:48.744Z | `e4c3a93ad575bd817fbfc7dfd28ce6a197777e4cbc5c629f991c287b0e9e7f34` |
| <a id="evidence-seven-single-game"></a>seven-single-game | `seven-current/queue/adaptation/single-game-normal-th08/report.json` | 2026-10-23T07:56:19.000Z | `c19ee7d5084a1e1d4a0e50bba5d4a49557dbafd2e30a2f5c945fdcc6330b7aae` |
| <a id="evidence-seven-installation-authority"></a>seven-installation-authority | `seven-current-fixed/adaptation/installation-authority-normal-th08/report.json` | 2026-10-23T08:30:59.783Z | `901829aef4fd7cd6035adec0529f5b08a7e78833c914c8949f113628e2522f81` |
| <a id="evidence-human-confirmation"></a>human-confirmation | `human-review-v88/user-confirmation-v92.json` | 见原报告 | `4d0df09187f7d9df317307d48ba7ad71121cc0de95e8b1aed34c330ffe81e29d` |
| <a id="evidence-limiter"></a>limiter | `lab-limit-control-v87/report.json` | 见原报告 | `96d55f88b1824dac8e4b89de77cae8b65c53a149c1b503f577b8ca46abdab559` |
| <a id="evidence-build-freshness"></a>build-freshness | `reconciliation-v93/build-revalidation/summary.json` | 见原报告 | `d04b56b5016c66370aae589a0b27373d773c4503a8e903565625a0de9e24abc0` |
| <a id="evidence-input-facts"></a>input-facts | `source-review-facts-v90.json` | 见原报告 | `90746c5af147f75c7890d8f99d1c6ca5200daecf5eed132864a977d79657faea` |
| <a id="evidence-source-review"></a>source-review | `source-review-facts-v90-run/summary.json` | 见原报告 | `1831377319beb68f6c123c514296e4325e6c0f088ca76a3ea0a13dda0f4816d5` |
| <a id="evidence-retained-state"></a>retained-state | `current-results-v92.json` | 见原报告 | `f2fceb412b7e63ab0d0d79920790774dbe5dc9340b6227dc2a7eb7781cf77675` |
| <a id="evidence-touch-last-window"></a>touch-last-window | `prior/input-gaps-v52/behavior/report.json` | 2026-10-21T08:58:54.485Z | `cdb3950e39bdb359b01b8e89e06b0dd5f9d8f28d82524db486c55ed4672695ea` |
| <a id="evidence-touch-other-actions"></a>touch-other-actions | `prior/input-gaps-v50/behavior/report.json` | 2026-10-21T08:34:05.083Z | `b37d1b9f99e3a0f0f71ec2a048e114dc98492ec0e40b5c61bf88adce49a4335a` |
| <a id="evidence-current-build"></a>current-build | `motion-v95/build-revalidation/summary.json` | 2026-10-24T08:53:56.326Z | `11463a77b98e3255d6f6e4eff2b7c6bafa3c6552c01abba6287513d0daf38eff` |
| <a id="evidence-motion-complete"></a>motion-complete | `motion-v95/motion/summary.json` | 2026-10-24T08:55:45.476Z | `02de95ee3647bda078663f1739263b6d406169b9bd8b936b19d9f96ff4a91398` |
| <a id="evidence-language-complete"></a>language-complete | `language-v98/summary.json` | 2026-10-24T09:33:07.659Z | `4d2cb5a1961090f73e4fc47a786470b076cc8e08a8034d1d2a0a79b2c30489a4` |
| <a id="evidence-recording-complete"></a>recording-complete | `core-behavior-v97/adaptation/replay-recording-normal-th08/report.json` | 2026-10-24T09:24:19.400Z | `d27fe09a52a72700d61129ff117d5a71810fe9dde4003b88e0191240d855f93b` |
| <a id="evidence-practice-lifecycle-complete"></a>practice-lifecycle-complete | `practice-v100/summary.json` | 2026-10-24T09:41:19.486Z | `966ffe5eb7d4088fd235746c7a544e3e03cef06eb37f98527887259c7e3f63b1` |
| <a id="evidence-deathbomb-complete"></a>deathbomb-complete | `deathbomb-v104/summary.json` | 2026-10-24T09:56:53.710Z | `fee6989688438deb0dbd785fc02857c29229e4e6e2ad0dd9499df931f9625577` |
| <a id="evidence-native-music-background-complete"></a>native-music-background-complete | `music-background-v176/acceptance/summary.json` | 2026-10-24T16:32:51.936Z | `7c03c19a3cae200d01d9e6bb37afce8d55a943f85adc08fa9fa73436485b70b8` |
| <a id="evidence-reviewed-practice-locales"></a>reviewed-practice-locales | `music-close-v180/practice-review/report.json` | 2026-10-24T17:03:08.072Z | `a03f5fe6006ebb6e900030740f6b05e32c5a284968bbe4e70bd91f8cba6ee764` |
| <a id="evidence-staged-http-update-complete"></a>staged-http-update-complete | `package-interruption-v177/acceptance/summary.json` | 2026-10-24T16:34:28.614Z | `36735e9c2a5adb060308699276925a70d1fd86ea9d0a62cefe51050ae515398d` |
| <a id="evidence-practice-capability-complete"></a>practice-capability-complete | `practice-capability-v161/acceptance/summary.json` | 2026-10-24T15:13:18.843Z | `85cd7101b8f0522f7380a896eaf9069833a3c14de321c9f5188b8eda2094e908` |
| <a id="evidence-replay-extension-version-complete"></a>replay-extension-version-complete | `replay-version-v163/acceptance/summary.json` | 2026-10-24T15:24:09.236Z | `e7c15e2a535e87277b3a94d7b1ec13b84c473b8620f9a5b8d62ed6b2fc5db175` |
| <a id="evidence-music-source-and-output-complete"></a>music-source-and-output-complete | `music-storage-v158/acceptance/summary.json` | 2026-10-24T14:50:23.082Z | `b3a979e005f4d3cf64d5b99c757c973aad3497e5ccb2da00ff10f7238fe5dba5` |
| <a id="evidence-native-user-paths-complete"></a>native-user-paths-complete | `music-storage-v158/acceptance/summary.json` | 2026-10-24T14:50:23.082Z | `b3a979e005f4d3cf64d5b99c757c973aad3497e5ccb2da00ff10f7238fe5dba5` |
| <a id="evidence-music-mode-change-complete"></a>music-mode-change-complete | `music-mode-change-v159/acceptance/summary.json` | 2026-10-24T15:02:51.196Z | `1653eac53a0c2f7c44880758842124b2fed8dc924a7ce46a194be2a3db9896c7` |
| <a id="evidence-native-keyboard-complete"></a>native-keyboard-complete | `keyboard-native-v132/summary.json` | 2026-10-24T12:16:02.536Z | `8b61c41f7f77b3643c061e5cfa4ea4cd82e27ec7ae41091458c690a7b1431843` |
| <a id="evidence-keyboard-layouts-original-export"></a>keyboard-layouts-original-export | `keyboard-original-export-v135/summary.json` | 2026-10-24T12:48:26.210Z | `d4360a1634d7eff145a416015e83b1db7a2097b0166cf7da955d7fcff8b67328` |
| <a id="evidence-practice-startup-complete"></a>practice-startup-complete | `practice-startup-v139/summary.json` | 2026-10-24T12:56:04.797Z | `d9b8d1ff70ba7bce338d45445b51b953657bfbe7f10f52820ce034118a9ad5e7` |
| <a id="evidence-native-inputs-complete"></a>native-inputs-complete | `native-inputs-v143/summary.json` | 2026-10-24T13:09:12.166Z | `954e13b20bd08c5b032281ec8eb7a9f9d1d7c880378c159dc553a0587eb0a141` |
| <a id="evidence-background-inputs-complete"></a>background-inputs-complete | `rebuild-v152/motion/summary.json` | 2026-10-24T13:55:28.928Z | `36903d2faf282b8581523309e21d325d5cddb458f2be77e1ddc1411e0bc44969` |
| <a id="evidence-rebuilt-current-source"></a>rebuilt-current-source | `rebuild-v152/build-revalidation/summary.json` | 2026-10-24T13:55:02.046Z | `30231f528b6a06814db8ea553fa46baa8999cede8e98a92f2eb6bc4719de4c65` |
| <a id="evidence-motion-rechecked-complete"></a>motion-rechecked-complete | `motion-complete-v121/summary.json` | 2026-10-24T11:27:08.895Z | `293f4210306d7cd20c981102352ebf14df5ef584080ab32399ba40ece76bd8fc` |
| <a id="evidence-audio-no-bgm-complete"></a>audio-no-bgm-complete | `audio-complete-v125/summary.json` | 2026-10-24T11:39:07.481Z | `b492082f7eb18f612924fd543a6a775d87967c7df7ef343a28e1d4752c6f8e2e` |
| <a id="evidence-bomb-resource-rechecked"></a>bomb-resource-rechecked | `bomb-resource-v128/summary.json` | 2026-10-24T11:53:19.428Z | `b1c2a53a64e8bc5b717db83e598cf6cc831fa44eb9c89529144b74036ca6252c` |
| <a id="evidence-protocol-storage-complete"></a>protocol-storage-complete | `protocol-gesture-v111/summary.json` | 2026-10-24T10:48:07.027Z | `b8a779274886b3eb3d49be8fe67138bacd566d6dbbfbe22e55302f63731509dd` |
| <a id="evidence-gesture-complete"></a>gesture-complete | `protocol-gesture-v111/summary.json` | 2026-10-24T10:48:07.027Z | `b8a779274886b3eb3d49be8fe67138bacd566d6dbbfbe22e55302f63731509dd` |
| <a id="evidence-actions-complete"></a>actions-complete | `actions-v110/summary.json` | 2026-10-24T10:39:04.908Z | `1e8d5a29680af90cbaa8a3675e94e080085e47d9fb72c789f188b33c835ab58b` |
| <a id="evidence-focus-buttons-complete"></a>focus-buttons-complete | `focus-v112/summary.json` | 2026-10-24T10:54:44.272Z | `55075ec41490d9545c8d5386811a6660e21a430b471d3ab3fac27f2e69a78966` |
| <a id="evidence-replay-isolation-complete"></a>replay-isolation-complete | `replay-isolation-v113/summary.json` | 2026-10-24T10:57:54.545Z | `f06c597e80ac1b47ea8d3944a759134d07843f5c9913102e6140b1071c91b461` |
| <a id="evidence-unlimited-result-complete"></a>unlimited-result-complete | `unlimited-result-v114/summary.json` | 2026-10-24T11:01:56.879Z | `e48e971cc225a176f779ec5809221aab19d3caa62891a2d8a0fce180d92229ef` |
| <a id="evidence-native-ogg-lifecycle-complete"></a>native-ogg-lifecycle-complete | `music-close-v180/ogg-native-lifecycle/summary.json` | 2026-10-24T17:02:07.895Z | `b8d22b0d8da16a9fc1a29088ae06226206134e5a6acdafddcd14c4557eccee9e` |
| <a id="evidence-native-midi-lifecycle-complete"></a>native-midi-lifecycle-complete | `midi-resume-v184/acceptance/summary.json` | 2026-10-24T17:21:07.474Z | `7c0c7261e0c65f734ff004c4b3ccbad6931eb9b56723612afb5725054cabe363` |
| <a id="evidence-launcher-lifecycle-complete"></a>launcher-lifecycle-complete | `launcher-lifecycle-v193/acceptance/summary.json` | 2026-10-24T17:54:43.529Z | `caf8fa623d70824b03304f2d49907e9221ee623d2672f06dc88a342eca5b9e68` |
| <a id="evidence-native-practice-controls-complete"></a>native-practice-controls-complete | `practice-controls-v206/acceptance/summary.json` | 2026-10-24T18:51:56.978Z | `06b05406e22337dc4cab2c2e3d3b1846662e9914febe2a6087879684820f93a5` |
| <a id="evidence-native-practice-parameters-complete"></a>native-practice-parameters-complete | `practice-parameters-v213/acceptance/summary.json` | 2026-10-24T19:26:09.302Z | `3ce1322e0d07248deec1f253d4725353743827fdb0b9539f91b5df0f515b9c74` |
| <a id="evidence-native-view-options-machine"></a>native-view-options-machine | `view-options-v216/acceptance/adaptation/view-options-normal-th08/report.json` | 2026-10-24T19:41:48.244Z | `5ece21f7a02c34012c8761d1b757eec5c41685fdb4baca65c24dda830e1c559d` |
| <a id="evidence-package-import-sources-complete"></a>package-import-sources-complete | `package-imports-v223/acceptance/summary.json` | 2026-10-24T20:34:06.640Z | `66a691f4ec2ed520b62f0dd4bb1fb411a1e98f0c765c440301a5ed2819088a2a` |
| <a id="evidence-practice-input-regression"></a>practice-input-regression | `regressions-v239/acceptance/adaptation/practice-parameters-normal-th08/report.json` | 2026-10-24T22:07:11.870Z | `4137c459d713da6fec93ed95b4d05cb537d48668ad57e5e14638c48e7d5b97a3` |
| <a id="evidence-language-consumers-complete"></a>language-consumers-complete | `language-v234/language/summary.json` | 2026-10-24T21:48:16.891Z | `730c6342d37a5f705dfa29ae0531ae01a2597933f795da9739c54149d6cdf55e` |
| <a id="evidence-language-native-scenes"></a>language-native-scenes | `language-v234/language/adaptation/language-normal-th08/report.json` | 2026-10-24T21:47:59.725Z | `e20998387cb4de6b686e8459683381803fdcdde964cd58a110d0bb45c3954ec8` |
| <a id="evidence-language-evidence-negatives"></a>language-evidence-negatives | `language-evidence-audit-v235a/report.json` | 见原报告 | `93295ea76fd6d6d9439b501c51c586ea083a9033d90095949c343e5854216ac5` |
| <a id="evidence-current-observer-build"></a>current-observer-build | `native-observers-v232/build-revalidation/summary.json` | 2026-10-24T21:33:44.659Z | `ec494e4d68dc146603abc171847626bbed1c0c92362a1d92a55a3e9fe6d9d28a` |
| <a id="evidence-practice-lifecycle-current"></a>practice-lifecycle-current | `practice-lifecycle-v244/acceptance/summary.json` | 2026-10-24T22:42:45.392Z | `4242fd04adbc6fa3fdf112af5830b12b0058622450dada9faf641708a67668ea` |
| <a id="evidence-practice-parameter-current"></a>practice-parameter-current | `practice-menus-v243/acceptance/adaptation/practice-parameters-normal-th08/report.json` | 2026-10-24T22:35:16.246Z | `7db11d357d7b53e8386a263731b8f3186ffafb4d72f573bd81fd7fc4e6a3648a` |
| <a id="evidence-practice-controls-current"></a>practice-controls-current | `practice-menus-v243/acceptance/adaptation/practice-controls-normal-th08/report.json` | 2026-10-24T22:32:17.622Z | `6db001dc8e27a998369ad390a54d46e9196a418a5bc1b92759ef52c87b5de20d` |
| <a id="evidence-practice-trainer-effects"></a>practice-trainer-effects | `practice-consumers-v250/acceptance/adaptation/practice-effects-trainers-normal-th08/report.json` | 2026-10-24T23:16:35.040Z | `bfae9187ffe376eb2a1a8cc91c938b800a40e58a1e34ff6ef404d323072125e9` |
| <a id="evidence-practice-input-options"></a>practice-input-options | `practice-options-v252/adaptation/practice-effects-options-normal-th08/report.json` | 2026-10-24T23:25:47.677Z | `1347ff4c79bb4e18a95b2d773b14a8d7a49fd5f2c3d67d4471cfeae0d46e1d0a` |
| <a id="evidence-practice-boss-effects"></a>practice-boss-effects | `practice-boss-v253/adaptation/practice-effects-bosses-normal-th08/report.json` | 2026-10-24T23:28:58.438Z | `9902dc0f66a147afd9987130f39424dbe07718741ae631daaa108c594c161bda` |
| <a id="evidence-practice-key-monitor"></a>practice-key-monitor | `practice-key-monitor-v254/adaptation/practice-effects-key-monitor-normal-th08/report.json` | 2026-10-24T23:40:43.316Z | `6b71f10e6e450a4ca66ab590c8020c580620b59cc1d41e37c36eae9a2607dcdf` |
| <a id="evidence-practice-consumer-build"></a>practice-consumer-build | `native-observers-v251/build-revalidation/summary.json` | 2026-10-24T23:23:58.963Z | `6c9549e683cce46158db316a0084bbf9d6f94f4f182ff863b5c7818705827fbd` |
| <a id="evidence-practice-mapped-life"></a>practice-mapped-life | `practice-gameplay-v256/adaptation/practice-effects-gameplay-normal-th08/report.json` | 2026-10-24T23:51:48.133Z | `fb177fbbe3107b56fbb13e05326b7af5349fb3f737611dae55cffdaa5ace90ee` |
| <a id="evidence-practice-lifecycle-options"></a>practice-lifecycle-options | `practice-lifecycle-options-v257/adaptation/practice-effects-lifecycle-normal-th08/report.json` | 2026-10-24T23:59:51.603Z | `ebdcb5ad41c654afbc6a5385805a2bbd2b43d7247625e39911bd9d6262d921ed` |
| <a id="evidence-practice-math"></a>practice-math | `practice-math-v268/adaptation/practice-effects-math-normal-th08/report.json` | 2026-10-25T00:32:31.411Z | `24d74072c15e41e54baf0df381de595dc041a9450e94b704faa53a382c6ab506` |
| <a id="evidence-practice-display-tools"></a>practice-display-tools | `practice-display-v272/adaptation/practice-effects-display-normal-th08/report.json` | 2026-10-25T00:39:34.195Z | `11f11a141f649d1eab01e5f4f56e9d2fdaf92be1b588e31455373136b1e0b1f0` |
| <a id="evidence-practice-lock-text"></a>practice-lock-text | `practice-lock-text-v271/adaptation/practice-effects-bosses-normal-th08/report.json` | 2026-10-25T00:39:48.886Z | `20314cff3a2ce539df320b07b1c05672fde0a86513079325f95626d117cd24a5` |
| <a id="evidence-launcher-lifecycle-current"></a>launcher-lifecycle-current | `regressions-v274/acceptance/adaptation/launcher-lifecycle-normal-th08/report.json` | 2026-10-25T00:49:54.941Z | `42a433b17c2d003eba83bf6ab575ed42443e14f52e13203d7b13e8533b964ec4` |
| <a id="evidence-native-view-options-current"></a>native-view-options-current | `regressions-v274/acceptance/adaptation/view-options-normal-th08/report.json` | 2026-10-25T00:51:41.809Z | `4402ff4c004eaeed79ffb2427f9728dbdfd478f11a42045a2b1cd27df8b15d14` |
| <a id="evidence-mp-build-revalidation"></a>mp-build-revalidation | `mp-revalidation-v280/acceptance/summary.json` | 2026-10-25T01:23:56.387Z | `65acec399c0da57205e29e8eda20bb83d32e7e871310eb710b404225b56d04e0` |
| <a id="evidence-reference-guard-negatives"></a>reference-guard-negatives | `reference-guard-v284/report.json` | 2026-10-25T01:32:25.068Z | `052145d003a76f8e26be00e8c0ebcc806787973dd1d0ed40d36fd1713ce820f2` |
| <a id="evidence-reference-integrity-complete"></a>reference-integrity-complete | `reference-final-v289/acceptance/summary.json` | 2026-10-25T01:51:10.068Z | `2ae8a34e023e9244a05ab13c7d06513a1387c90212dab77f7c5f3cf1ea84a025` |
| <a id="evidence-reference-integrity-resume"></a>reference-integrity-resume | `reference-resume-v290/summary.json` | 2026-10-25T01:52:42.582Z | `b8db5ef0bb4893550b7f7c89bf87cc879c87b36892e29ed6ba8cfb51cdced23f` |
| <a id="evidence-replay-method-complete"></a>replay-method-complete | `replay-method-final-v306/acceptance/summary.json` | 2026-10-25T02:53:05.816Z | `bc0528fdaa0097ee2e18ac72710e9f7d0d87380cf9a14d917b2c9ebf95c9ea78` |
| <a id="evidence-replay-method-resume"></a>replay-method-resume | `replay-method-final-v306/resume/summary.json` | 2026-10-25T02:53:20.185Z | `ed899f88a4968b044ccc3357158959deb81f6ab380fe3f2ecc8df96f3603530b` |
| <a id="evidence-replay-method-guards"></a>replay-method-guards | `replay-method-final-v306/negative-controls/report.json` | 2026-10-25T02:53:17.064Z | `953a5e02953e150f6f10f2b5b6abd1b3a9a382006733d9e8bdfda4a6a58af6e3` |
| <a id="evidence-replay-current-regressions"></a>replay-current-regressions | `replay-regressions-v308/queue/report.json` | 2026-10-25T02:54:55.515Z | `6f8c720b038eeda6714caf9c7e2b6f656d017be207eb1383fad199e8536c6fa3` |
