# 键盘、手柄与触控

[适配流程](README.md) | [文档目录](../README.md)

先接入正常逻辑输入，再完成每种设备和手势。用同一组动作比较键盘、手柄与触控。

## 准备

准备键盘、手柄和触屏设备。先完成菜单、对话、Gameplay、暂停四种状态，再组合多种输入。布局编辑、按钮位置和放大镜直接使用启动器。

触控坐标按游戏画面换算；灵敏度改变滑动距离，角色的原作速度由逻辑层控制。

各作 `portable/input/touch-bomb-check.mjs` 共用 WASI 编译与执行入口。通过 `EAGLER_WASI_SDK` 或 `WASI_SDK_PATH` 指定 SDK 根目录，也兼容 `WASI_SDK_BIN` 与 `WASI_SDK_ROOT`；入口按当前平台选择编译器，并显式传入本仓 common 头文件目录。结果保存编译命令、工具链版本、源码与 WASM 哈希，以及原生 `verify()` 返回值。每次运行独立更新报告，缺少工具链和编译失败均留下失败记录。该检查验证生产触控模块，浏览器接线和完整手势另按下表操作。

th10 normal 的 Runtime 声明与移动规则共用一份契约，分别描述角色坐标精度和 float32 归一化 Pointer 输入。共享场景先按 640×480 输入空间计算目标，再按角色原生量化与速度规则比较实际位移；原始触控反馈不参与生成预期值。

`touch-native-invariants` 组件任务按产品目录生成 source-owner 集合，直接编译作品的 `TouchController.hpp` 和 `MotionTrack.hpp`。固定输入执行方向式与自由摇杆、焦点倍率、逻辑动作键、二指双击 Bomb、菜单／对话手势、Gameplay 已持有射击后触控确认的新样本转换、移动受阻时的 Bomb 脉冲、抬起释放与 `MotionTrack` 标记 API 的 limited、零位移 unlimited、非零位移 unlimited 样本；作品状态含 `choice` 字段时，同一生产 Controller 还验证选项滑动只发一次方向键。th08-mp `ResolveAnalogMovement` 和 th10-mp `InputLanes::ResolveMovement` 另接真实生产消费者，th06/th07 使用既有源码锚点作为参考。报告把组件行为、作品 Runtime 消费和按标题定义的手势能力分别列出，逐作品的 gameplay 接线由 Runtime 场景验收。

th08 Shell touch fixture 按 Runtime variant 加载对应 source owner：normal 使用 `ports/th08`，multiplayer 使用 `ports/th08-mp`。它执行生产 Shell 消息监听和命令队列，通过 JavaScript 导出接收端观察转发结果，检查 `direct-touch`、`touch-controls`、`touch-cancel` 在资源命令阻塞时即时处理，并核对启动资格、Touch 开关、可见性及取消生命周期。th10 的既有 fixture 固定加载 `ports/th10-mp`，其中单人配置与多人配置分别核对同一 MP Shell 的配置分支。

## 共用运动验收

`touch-motion` 场景组使用同一套操作和判定检查各作产物。作品接口负责读取真实逻辑帧、人物坐标与运动记录，协议入口接收触点和控件输入。[共享场景](../../tools/ports/touch-motion-scenes.mjs)先从原生键盘路径测量高速、低速，再用独立的输入轨迹计算触控目标。[数值判定](../../tools/ports/touch-motion-contract.mjs)逐逻辑帧比较实际位置，并检查样本连续性。

轨迹读取沿用本作 `runtime` 挂载声明，与原生状态观察指向同一实例；导出和内存均由该实例提供。动作、练习与 Replay 观察使用相同的归属约束。新作品可使用自己的全局对象、导出对象和模块字段名。

场景参数通过入口的第二个参数传入，与承载读取、触控和按键操作的 IO adapter 分开；错位配置会在输入开始前触发类型错误。

| 场景 | 输入与硬判据 |
| --- | --- |
| 微移 | 固定请求 `(0.3, 0.4)`，目标在本帧速度范围内时完整跟随，保留小数坐标。 |
| 任意方向与限速 | 固定请求 `(6, 8)`；原生速度为 4 时，首帧位移为 `(2.4, 3.2)`，后续继续消费剩余距离。 |
| 灵敏度 | 同一触点位移分别使用 100%、200%、300%，请求距离按 1、2、3 倍变化；低速场景重复微移，并以键盘校准确认速度比例。 |
| 低速 | 独立测量低速键盘位移，核对触点目标比例、逐帧移动上限和余量消费，再释放低速并验证恢复高速。 |
| 无限速 | 使用相同目标，首个移动逻辑帧到达请求位置。 |
| 自由方向摇杆 | 23°、37°、71°及其反向使用三档幅度；另测 43° 全幅及反向，逐帧核对向量、速度与回中；低速后再次验证高速。 |
| 释放与取消 | 分别发送抬手和取消，连续三个原生逻辑帧保持中立、位置稳定。 |
| 多指与输入所有权 | 第二指加入、移动和抬起时，移动指继续拥有目标；键盘与触控按本作输入所有权策略执行，取消触控后核对键盘动作。双指低速按该产物的启动配置单独执行。 |

受控输入场景在标题导航前通过本作 `armExport` 提交一次性新世界请求。请求在 `game.load()` 前锁定持久提交，加载期间的自动分数初始化通过 RAM 覆盖并返回成功，游戏外写入保持拒绝；只读写入回执记录尝试、RAM 覆盖、丢弃字节和持久提交字节。世界加载完成后，GameApplication 在首个 EnemySimulation 更新前核对资格并启动时间线门；弹幕危险计数覆盖 1536 个真实分配槽，排除池尾部的分配哨兵。只读生命周期回执确认请求接受、首条时间线抑制和会话锁。旧有 16-word 场景快照保持字段含义不变，时间线与写入状态使用独立版本化导出。无请求的正常启动、Replay 与练习沿原有加载和文件写入路径运行。

每种启动配置复用同一局，灵敏度和焦点场景连续执行。双指低速另以 `touchFocusMode=two-finger` 启动同一套输入所有权场景，核对移动指、焦点指和低速恢复。具有正式手动步进入口的 Runtime 在测试等待期间停止自动循环，由操作器显式推进正常游戏 Tick，结束后恢复自动循环。观察函数只读取已完成的记录；没有手动步进入口时，在发出场景输入前报告 incomplete。th08、th10、th20 按各自正式 `sdl_loop_stop`、`sdl_loop_tick` 和 `sdl_loop_start` 签名完成步进与恢复。

键盘速度校准通过 Runtime `KeyboardInputBridge` 确认每个按下／抬起转换。th08 normal 的原生录制链在 priority 17 发布下一次 movement 使用的输入，而玩家 movement 在 priority 9 执行。停钟后读取器以完成帧轨迹总数夹住一次 Runtime observation；该 observation 明确标记来源为 `diagnostics[11] | player_state.input.buttons`、阶段为 `after-application-update`。因此该值证明排队给下一次玩家 movement 的方向键／焦点状态，不声称是上一 movement 已消费的按键。释放转换后唯一允许的下一条 movement 必须符合此排队方向、速度和焦点状态；该回调发布的后续队列必须清除方向键与焦点键，接下来的连续三帧保持位置稳定且焦点关闭。方向键和焦点键在同一停钟边界释放时按同一个排队输入判定。

共享边界统一核对 `inputSemantics`、`source` 与 `sampleBoundary`；只有适配器明确声明为 `queued-for-next-movement` 的原生输入样本才能证明下一逻辑步的排队状态。未适配的逻辑按钮快照、已消费输入、来源或采样阶段不一致均不能满足该边界。

触控位置释放与焦点采样按原生输入队列判定：普通指针 `up` 保留其它焦点所有者的状态，`cancel` 清除触控焦点 owner；取消后的首条记录按动作前队列检查焦点，随后队列必须发布焦点关闭。双指加入时，首条记录消费动作前状态，行后队列发布焦点开启，下一 movement 使用已校准低速；抬起焦点指时按同一顺序切回高速。队列中的方向键若与触控独立，取消后的首条 movement 必须精确符合键盘校准方向和速度；没有排队方向键时，触控取消首帧必须保持目标、起点与终点一致。缺少原生输入边界时场景报告 incomplete。

```sh
npm run test:adaptation -- --game=th08 --variant=normal --suite=touch-motion --inputs=.local/stg-inputs.json
npm run test:adaptation -- --variant=both --suite=touch-motion --inputs=.local/stg-inputs.json --plan
```

报告保存输入坐标、独立目标、键盘速度参考、逐帧轨迹、释放边界和首个差异。目标清单来自产品目录。当前 th08 普通版提供真实原生轨迹；其他普通版的轨迹接入按相同 ABI 检查，缺少记录返回 incomplete。MP 项目在计划中列为 blocked，接入房间操作与原生轨迹后复用共享断言。场景结果与整个适配编号的 `complete` 范围分别记录。

Runtime 提供[只读轨迹 ABI](../../common/include/eagler/input/TouchMotionTrace.hpp)：每个完成的 movement 回调记录起点、触控请求目标、实际终点、原生速度、轴倍率、灵敏度与原生帧号；无法还原的字段由 ABI 标志显式注明。th08 [`BrowserRuntime`](../../ports/th08/web/cpp/platform/BrowserRuntime.cpp#L73) 在触控运动有效时写入触控请求目标，方向键清除触控运动后，mode 0 的键盘运动行保持 `target=before`，实际位移只体现在 `after-before`（回调完成位置见该文件 [line 244](../../ports/th08/web/cpp/platform/BrowserRuntime.cpp#L244)）。ABI v2 按本机席位和执行类型分流；共享场景在发送输入前要求 ABI v2 的本机席位 0 正向流，再逐帧检查序列。读取结果通过 `available` 明确能力状态，缺少正式导出时附带原因；ABI 结构错误在解码阶段保留为失败证据。历史 v1 记录仍可由读取器解码，严格场景会在发出输入前报告其缺少 v2 身份信息。th08 在真实 `app.update()` 完成后读取人物位置，以回调计数标记无法逐条观察的重复回调；原版 Replay 和触控轨迹 Replay 分别写入 Replay 流，不参与正向验收。

数值预期由输入命令、独立键盘速度和作品原生坐标规则组成。th06 的 [`Touch.cpp`](../../ports/th06/src/Touch.cpp#L824) 按触点差值积累位移、按灵敏度缩放，并分别维护移动指和焦点指；[`Player::HandlePlayerInputs`](../../ports/th06/src/Player.cpp#L1934) 按焦点比例映射目标、限制移动区域，再按径向速度预算移动；[`Touch::GetPlayerDelta`、`SetPlayerDelta`、`ConsumePlayerDelta`](../../ports/th06/src/Touch.cpp#L1005) 与 `HandlePlayerInputs` 的消费分支共同保留未消费余量。这些源码证实 th06 的微移、任意方向限速和余量语义。th08 的 [`TouchController.hpp`](../../ports/th08/portable/input/TouchController.hpp#L45) 以主指维护拖动目标，按启动配置识别双指焦点，再按独立测得的高速和低速比例换算目标。它们分别作为对应作品的数值依据；其他作品按本作源码建立薄适配，不把作品差异写入统一常数。

每个产品的 `movementContract` 从源码锚点给出原生世界坐标单位、舍入规则和轴倍率相对限速的位置；共享判定据此逐帧预测，不用统一 epsilon 吞掉精度差异。例如 th10 的 [`WorldPlayer.cpp`](../../ports/th10/web/cpp/platform/WorldPlayer.cpp#L41) 以 1/100 固定坐标求目标差，限速触控通过[共用量化器](../../common/include/eagler/input/LimitedTouchVector.hpp)选择速度圆盘内距离请求向量最近的整数格点。独立固定样本要求速度 400 时，受限向量 `(293,273)` 量化为 `(292,273)`，小幅 `(30,40)` 保持原值。th15 的 [`PlayerMotion.cpp`](../../ports/th15/web/cpp/game/PlayerMotion.cpp#L4) 把移动速度量化到 1/128，触控向量按原生速度限幅后向零截整，再经过移动倍率和 fixed-position 边界处理。每种精度都用独立固定样本核对预测位置，实测位移继续接受独立键盘校准得到的径向速度硬上限约束。

th10 的 [SDL 触控入口](../../ports/th10/web/cpp/sdl/GameHost.cpp#L164) 接收 `float` 归一化指令；[`TouchController::pointer`](../../ports/th10/portable/input/TouchController.hpp#L40) 先以 float32 计算指针差值、640×480 像素位移、灵敏度与焦点比例，再累加世界坐标目标。独立预期按同一已记录的指针起点与命令终点逐步执行 float32 运算，之后才应用 1/100 世界坐标量化。例如归一化指针命令对应请求 `0.145000001` 世界单位时，float32 路径得到 `0.144996643`，原生百份位移动量为 `0.14`；直接用 JS double 目标会得到 `0.15`。归一化指针坐标、世界位置单位和按逻辑帧校准的原作速度分别保留各自单位。

手柄桥检查按产品目录和构建配方生成变体集合，逐项核对 Launcher 映射进入本作输入路径；新增产品自动加入，无固定作品数量限制。格斗作品的原生 SDL 手柄与 Launcher 改键分别核对各自接线。

浏览器手柄验收也从产品目录选择 STG 目标及本机席位上限。新作登记 Runtime、原生观察和玩法流程后复用相同的操作与断言；合作作品使用本机映射，双边对战按席位选择 Launcher 映射。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="input-01"></a>INPUT-01 \| 键盘逻辑输入 | 依次短按、长按、松开各逻辑键。 | 移动、射击和菜单行为与原作相同。 |
| <a id="input-02"></a>INPUT-02 \| 启动器键盘改键 | 在启动器切换默认、WASD、自定义键位，再进游戏。 | 新的映射立即生效，同一动作只执行一次。 |
| <a id="input-03"></a>INPUT-03 \| 启动器手柄映射 | 保存手柄按钮与轴映射，连接两个控制器并指定席位。 | 每个席位只接收所选设备的动作。 |
| <a id="input-04"></a>INPUT-04 \| 手柄方向与按钮 | 使用方向键、摇杆、按钮完成移动、低速、射击和菜单操作。 | 手柄进入与键盘相同的逻辑输入路径。 |
| <a id="input-05"></a>INPUT-05 \| 多种输入独立持有 | 同时用键盘与触控保持射击，先释放其中一种。 | 另一输入仍然有效。 |
| <a id="input-06"></a>INPUT-06 \| 失焦与断开释放 | 保持移动时切换窗口、隐藏页面、断开手柄。 | 输入及时释放；重新连接可继续操作。 |
| <a id="input-07"></a>INPUT-07 \| 原作菜单与对话输入 | 用键盘与手柄分别完成菜单选择、确认、取消、对话快进。 | 沿用原作的菜单状态和确认时机。 |
| <a id="touch-01"></a>TOUCH-01 \| 限速滑动移动 | 在整个游戏显示区域滑动，比较高速与低速，快速拖到远处。 | 人物沿滑动轨迹移动，速度遵守原作上限，拖动距离保留。 |
| <a id="touch-02"></a>TOUCH-02 \| 无限速滑动移动 | 启用无限速触控后向远处滑动，再关闭。 | 开启时跟随请求位置；关闭后恢复限速行为。 |
| <a id="touch-03"></a>TOUCH-03 \| 方向式虚拟摇杆 | 选择方向式摇杆，依次推向四向与斜向。 | 方向与键盘逻辑一致。 |
| <a id="touch-04"></a>TOUCH-04 \| 自由方向虚拟摇杆 | 选择自由方向摇杆，绕一圈并改变推杆幅度。 | 角色连续转向；逻辑步进保持原速。 |
| <a id="touch-05"></a>TOUCH-05 \| 按住低速 | 按住低速按钮移动，再抬手。 | 按住低速，抬手恢复高速。 |
| <a id="touch-06"></a>TOUCH-06 \| 切换低速 | 点击两次切换低速按钮。 | 每次点击切换一次状态。 |
| <a id="touch-07"></a>TOUCH-07 \| 双指低速 | 移动时加入第二个触点，再分别抬起。 | 第二指改变低速状态，移动指继续独立工作。 |
| <a id="touch-08"></a>TOUCH-08 \| 射击、Bomb 与暂停 | 分别点击射击、Bomb、暂停，再组合移动与按钮。 | 使用本作正常动作与暂停流程。 |
| <a id="touch-09"></a>TOUCH-09 \| Deathbomb 与移动保持 | 在本作可用的 Deathbomb 窗口内触发 Bomb，同时保持移动。 | 判定窗口沿用原作；移动持续有效。 |
| <a id="touch-10"></a>TOUCH-10 \| 无限速移动落率记录 | 仅打开无限速选项，再实际移动；完成结算并保存 Replay。 | 实际非零无限速移动后，结算与 Replay 的处理落率按约定记录为 100%。 |
| <a id="touch-11"></a>TOUCH-11 \| 双击 Bomb | 启用双击 Bomb，在移动过程中双击。 | 触发一次正常 Bomb，移动触点保留。 |
| <a id="touch-12"></a>TOUCH-12 \| 菜单与对话手势 | 在菜单滑动、点击、返回；在对话短按、长按。 | 一次滑动前进一步；短按确认，长按沿用跳过行为。 |
| <a id="touch-13"></a>TOUCH-13 \| 确认键的新按下边沿 | 保持射击进入对话或结算，再触控确认；MP 中其他席位保持射击。 | 确认产生新的按下边沿，流程继续。 |
| <a id="touch-14"></a>TOUCH-14 \| 100–300% 灵敏度 | 在 100%、200%、300% 灵敏度下重复同样滑动。 | 只改变触控距离映射；角色原始速度和低速比例保持一致。 |
| <a id="touch-15"></a>TOUCH-15 \| 触控与其他输入隔离 | 键盘保持动作时取消触控或切换触控模式。 | 键盘和手柄仍保持自己的动作。 |
| <a id="touch-16"></a>TOUCH-16 \| 触控取消与生命周期 | 触控过程中触发 pointer cancel、隐藏页面、切换窗口。 | 该触点释放，恢复后无残留移动或误触发。 |
| <a id="touch-17"></a>TOUCH-17 \| 共用布局编辑器 | 从启动器调整布局、缩放并切换横竖屏。 | 复用共用编辑器，两个方向的布局分别恢复。 |
| <a id="touch-18"></a>TOUCH-18 \| 共用重开按钮 | 暂停后用启动器的重开按钮重开。 | 进入与 R 键相同的本局重开流程。 |
| <a id="touch-19"></a>TOUCH-19 \| 共用双指放大镜 | 在游戏中双指缩放，恢复默认大小。 | 放大由启动器完成，触控坐标仍对应游戏画面。 |

## 可审查的验收方法

以下方法供逐项审查，当前入口说明实际覆盖范围。

`desktop-input` 以 headless Chromium 执行生产 DOM 键盘映射、Runtime 输入桥、触控协议和原生玩法效果，逐阶段采样 Host 与 Runtime 文档的可见、焦点状态。headed `desktop-background` 持有真实 Chrome 页面与窗口身份，核对启动前台、最小化和同窗标签页生命周期；每次配置后恢复正常窗口并重新核验原生页面与窗口身份，再启动 Runtime。

### INPUT-01 | 键盘逻辑输入

- 操作：从菜单进入关卡，对每个适用逻辑键分别执行短按、持续按住、松开，并在菜单、对话和玩法中重复其对应动作。
- 观察与独立预期：读取原生已消费输入位、菜单或对话状态、角色坐标和逻辑帧；期望行为以原作键位语义和固定场景为准。
- 判定：动作到达指定状态，持续输入只在按住期间有效，松开恢复中立；缺少原生输入或状态观察时记 incomplete。
- 反例：只发送 keydown、不发送 keyup，测试应因输入位持续有效而失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 在正式普通版关卡逐一执行四向按下、位移与松开；每个 Runtime epoch 都按原生阶段、输入、位置及本机席位复核玩法资格。缺少原生观察或当前作品尚未进入可操作状态时记录 incomplete。`runtime-browser.mjs` 另验右移；`browser-shell` 负控检查 Shell 接线。

### INPUT-02 | 启动器键盘改键

- 操作：分别选默认、WASD 和自定义布局，保存后重新载入设置，再用物理键完成对应菜单和玩法动作。
- 观察与独立预期：把保存的映射表与发往 Runtime 的规范键码、原生消费位和目标菜单/角色状态逐项对应。
- 判定：新映射按选择生效，已取消的自定义绑定不再生效，原生兼容键按键盘契约处理；每次动作只有一个语义结果，持久化布局在重载后保持。
- 反例：保留旧物理键同时增加新别名，按一次造成双重移动或确认，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 使用生产 KeyboardMapping，分别从 Host 和 Runtime 画布发送 WASD 四向，核对原生输入、位移与释放；屏蔽映射的反例要求动作消失。每作均等待原生输入、位置和玩法资格，th09 同时核对原生触控上下文及本机席位。启动器配置 UI、自定义绑定与刷新恢复由对应组件和独立场景验证。

### INPUT-03 | 启动器手柄映射

- 操作：绑定按钮和轴到逻辑动作，分配设备到席位，保存并重载后分别操作两个已连接设备。
- 观察与独立预期：记录设备身份、每席映射快照、生产采样器输出和原生席位输入；测试预期来自用户配置的绑定。
- 判定：只有指定设备的指定席位收到对应动作，保存映射重载后相同，断开设备不把其映射转交给另一控制器。
- 反例：两只同型号设备交换索引后，原席位沿用另一设备的旧绑定，应失败。
- 当前入口/边界：`runLauncherInputRuntime({kind:'gamepad', ...})` 安装生产 `createGamepadMappingModel`、`createPlayerGamepad` fixture，并把两台独立虚拟设备的映射采样与调用方推进的原生消费者 Tick 关联；场景覆盖按钮、摇杆轴、未选设备隔离、切换设备、索引重排及隔离存储重载。首台设备的 Shoot 固定用非默认 button 3，避免原始标准手柄 button 0 的回退输入替代映射路径；每条生产采样同时保留设备身份、按钮／轴原值、映射位和对应原生 Fire 状态。失败证据保留最近八个 tick，并区分映射位未产生与原生消费者未收到。调用方注入已运行 Runtime 的原生 step、动作 reader 与位置 observer；实体设备与可视化配置流程由设备和组件场景补充。

### INPUT-04 | 手柄方向与按钮

- 操作：分别使用方向键和摇杆四向、斜向，再操作低速、射击、菜单确认与取消。
- 观察与独立预期：同时记录生产映射采样位、原生已消费输入、本机席位和角色坐标；各动作的预期取自键盘等价逻辑和本作原作规则。
- 判定：每种输入到达相同逻辑动作，低速和菜单状态符合原作，松开后回到中立；缺少相应 getter 时记 incomplete。
- 反例：摇杆右移被映射成射击，或 MP 的另一席位输入改变本席位，应失败。
- 当前入口/边界：runtime-browser 普通版及 room-browser MP 的 native probe 覆盖射击、低速、右移、释放及持有低速时断开；launcher/tests/test-gamepad-contract.mjs 按产品目录与构建配方核对普通版和 MP 的生产接线。完整方向与菜单矩阵使用各自的原生效果记录。

### INPUT-05 | 多种输入独立持有

- 操作：让键盘和触控同时保持同一逻辑动作，先后释放其中一个；再以键盘和手柄重复该所有者隔离场景。
- 观察与独立预期：记录每个输入所有者的按下/释放消息、汇总逻辑位和原生消费位；释放一个来源不得清除仍持有的来源。
- 判定：剩余来源持续有效，全部释放后才回到中立；每次状态变化与来源拥有权一致。
- 反例：键盘与触控都按住射击时，先松开触控却发出射击释放，应失败。
- 当前入口/边界：keyboard-mapping 测试核对物理/虚拟所有者隔离；`touch-actions` 共用 DOM 场景在同一正式 Runtime 中交叉保持键盘和生产触控控制器的射击，逐 Tick 读取原生消费位，分别释放每个来源并检查触控取消保留键盘输入。浏览器输入是软件生成，实体设备另行验收。

### INPUT-06 | 失焦与断开释放

- 操作：持有移动或低速时依次触发窗口失焦、页面隐藏和手柄断开；恢复窗口或重连后先保持输入，再重新按下。
- 观察与独立预期：读取输入所有者清理事件、设备连接状态、原生输入位及角色状态；恢复后由本次有效设备采样或按键事件继续操作。
- 判定：失焦、隐藏和断开均清除当时持有，恢复事件本身不重放清理前的旧输入。
- 反例：持有右移时隐藏页面，恢复后仍继续移动，应失败。

th08 普通局清理输入时，由 ReplayRecording 同步取消尚未消费的实时输入与录像尾样本。玩家实际消费后才退役该样本，清理保留已消费历史、释放边沿、游标、时间和种子；Replay／Demo 播放沿用文件输入。`ports-touch-wasi` 队列中的 `th08-pending-input-clear` 执行真实录制与回放代码，核对清理、重复清理、新输入和不追加记录的边界。真实窗口最小化场景继续检验 SDL 接线与角色消费。

恢复检查等待同一关卡的移动时钟实际前进，比较隐藏期间与恢复后的角色坐标及输入位。输入都为零但角色仍发生残余位移，也使场景失败；时钟或坐标缺席保留未完成结果。

- 当前入口/边界：`runtime-desktop-probes.mjs` 经 Playwright `launchServer` 和 loopback CDP 网关运行全尺寸 headed Chrome，按真实窗口最小化或同窗标签切换采集原生 blur/hidden、输入位和释放结果。最小化期间要求 Host keyboard-clear 被接受且原生输入已中立；没有有效原生输入样本时记为 incomplete，有效非零样本仍判 failed；恢复后的读数不替代隐藏期间证据。标签检查关闭临时页后再次核对宿主页与 Runtime iframe 的 `visibilityState`、`hidden`、`hasFocus()`，并确认仍为原来的 Chrome page target 与 window ID。报告保存实际可执行文件与版本、启动参数、CDP target/window ID、窗口边界，以及按命令 id/session/page target 关联并取得 Chrome 成功响应的焦点改写记录。`--background-only` 专用于窗口、标签和声音三项生命周期检查，报告将其他桌面检查单列为 omitted；手柄断开由 gamepad mapping 与 native gamepad probe 覆盖。

### INPUT-07 | 原作菜单与对话输入

受控场景在标题导航前检查请求已武装，进入关卡后检查原生世界已接受请求、首段时间线已拦截且存档写入被隔离。菜单、对话与 Launcher 控件沿用这条共同生命周期，再核对各自的真实输入消费者。

每段输入场景分别记录 iframe URL 的 Host epoch 与原生 generation、指针和 loop epoch。场景前后保持同一绑定，逐条消费者样本归属于该原生实例；两种 epoch 按各自所属生命周期核对。菜单等待逐样本编号，对话按实际排队、消费和释放顺序重算证据。

Headless 场景记录页面关闭、崩溃、上下文关闭和浏览器断开事件，附目标、epoch、输入清理或导航阶段与清理归属。预期的上下文和浏览器退休带有明确标记；场景中意外关闭或任意崩溃阻断通过，并保留最早事件。

- 操作：分别以键盘和生产手柄映射运行 `runOriginalMenuInputScene`。从已就绪的标题起始项确认进入难度菜单，依次下移、上移各一项，再取消返回标题；触控路径点击起始项、单次滑动并以双指静止手势返回。另以 `runLoadedDialogueSkipScene` 在实际加载的对话 Wait 中发送 Skip。
- 前置：共享菜单入口在生产时钟上有界等待本作原生标题场景，再取得手动逻辑时钟。逐次状态、等待范围和 Runtime 身份写入 `titleReadiness`，菜单确认门槛由后续逻辑步检查。首帧消息与原生标题就绪分别观察。取得时钟前后核对同一 Runtime 实例，场景身份绑定接管后的循环 epoch，并在整段操作结束时复核。
- 观察与独立预期：原生标题状态来自 `BrowserRuntime::status` 的 screen、cursor、phase、confirmation-ready 和 current/previous input；共享场景将这些消费者的源码锚点写入原始证据，汇总器与作品适配器逐项核对。对话候选来自当前已加载 ECL/MessageProgram，Skip 由 `GuiFlow::update` 的消费回执及 MessageProgram 游标共同确认。
- 判定：菜单输入产生一个新边沿并只移动一项，上下操作相互恢复原游标，确认进入登记的下一菜单，取消返回原始标题项；对话 Skip 位被真实 Dialogue consumer 消费且游标前进，confirm 位保持独立。
- 反例：输入只得到传输确认但原生状态未变化、单次导航跨越多项、取消离开合法标题流、输入源与声明不符、对话缺少真实 Skip 消费或把 Skip 误识为确认，均不能通过。
- 当前入口/边界：共享判据位于 [`original-menu-input-scenes.mjs`](../../tools/ports/original-menu-input-scenes.mjs) 与 [`loaded-dialogue-skip-scenes.mjs`](../../tools/ports/loaded-dialogue-skip-scenes.mjs)。标题入口只检查本次声明的键盘、生产手柄或触控 ingress；游戏手柄必须先由产品映射器产出来源回执。真实对话 Skip 复用 DTO1 消费 receipt 和已加载脚本清单，不写 menu cursor、dialogue timer 或 instruction。

### TOUCH-01 | 限速滑动移动

- 操作：在玩法中从固定起点按住并沿固定画面轨迹滑动，比较短距离、长距离和快速拖动。
- 观察与独立预期：记录归一化触点、触控目标、原生坐标及逻辑帧；速度上限按本作原生移动规则或独立原作样本确定。
- 判定：角色沿输入轨迹移动，过长拖动仍遵守原作速度上限且目标距离没有被截断。
- 反例：只测试慢速短滑，快速拖动时角色超过原作速度上限，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 的 `shared-touch-limited` 执行共用运动场景，以真实完成的原生 Tick 核对微移、斜向长目标、限速余量、焦点下的 100–300% 微移映射、低速释放恢复和各档灵敏度。抬手与取消分别观察，目标来自已发送输入和独立键盘校准。player-touch 检查 Launcher 几何与消息端口。

### TOUCH-02 | 无限速滑动移动

- 操作：使用同一起点和目标，先按限速模式滑动，再启用无限速模式移动至远处，最后关闭选项重测。
- 观察与独立预期：记录触点位置、请求目标和每帧角色坐标；“无限速”按适配定义只解除距离映射限速，不改原作逻辑步进规则。
- 判定：开启期间角色跟随请求目标，关闭后回到限速路径，选项切换不残留旧目标。
- 反例：关闭无限速后仍沿上一远距离目标移动，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 的 `shared-touch-unlimited` 在对应启动配置执行相同微移、斜向、灵敏度与释放场景，要求首个移动逻辑帧到达独立目标。编号完整结果同时核对选项关闭后的限速恢复。

### TOUCH-03 | 方向式虚拟摇杆

- 操作：选择方向式摇杆，分别推向上、下、左、右和四个斜向，再回中。
- 观察与独立预期：记录摇杆输出方向、原生逻辑方向位和角色位移方向；目标方向与键盘逻辑方向相同。
- 判定：所有方向都进入正确的逻辑输入，回中后输入释放；斜向规则以原作和已登记摇杆实现为准。
- 反例：松开摇杆仍保留上一次方向位，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 的 `runtime-epoch-touch-mode-joystick` 进入 `desktop-input` 和 `touch-motion` 必跑清单，在实际关卡逐一检查四向及四个斜向的原生位移和释放；th09 同时核对原生连续移动通道回到未启用状态。player-touch 单测补充 dead zone、signed axes 与取消回中。

### TOUCH-04 | 自由方向虚拟摇杆

- 操作：保持虚拟摇杆并沿圆周改变角度和推杆幅度，经过原始表所要求的连续转向样本。
- 观察与独立预期：读取归一化摇杆向量及原生角色移动方向；连续角度的参考值应由本作控制规则或独立原作测量给出。
- 判定：移动方向随输入连续变化，逻辑步进速度保持本作原有规则。
- 反例：推杆旋转时方向被固定到上一次方向，或坐标不再变化，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 的 `shared-touch-joystick-free` 在非八向角度使用三档幅度，以固定起点和已发送轴值计算每个 Tick 的目标，核对真实轨迹、原生速度、低速恢复与回中。完整编号按其圆周转向范围汇总。传入 `--suppress-touch-movement` 可运行真实包反例：驱动记录并屏蔽拖动／摇杆移动，位移断言应失败；反例报告单独保存。

### TOUCH-05 | 按住低速

- 操作：沿同一玩法路线先以高速移动，再保持低速按钮重复同一动作，最后抬起按钮。
- 观察与独立预期：用原生位置和逻辑帧比较高速、低速位移；低速比例取本作原作规则或当前已接受值，不在测试里临时设阈值。
- 判定：按住期间进入低速，抬起后恢复原速度，角色移动方向保持。
- 反例：按钮抬起后仍持续低速，或只改变触点映射而未进入原生低速，应失败。
- 当前入口/边界：`shared-touch-limited` 通过 Runtime 控制输入启用焦点，并从原生键盘路径独立校准低速，再逐帧核对微移映射、移动比例及抬起后的高速恢复。player-touch 场景核对 Launcher 焦点按钮的真实 DOM 路径；两组场景分别覆盖控件接线和原生运动行为。

### TOUCH-06 | 切换低速

- 操作：将低速设置为切换模式，点击一次、再次点击，并重载同一运行配置。
- 观察与独立预期：读取切换状态、原生低速输入和相同场景的角色速度；每次点击应按该模式翻转状态。
- 判定：第一击启用、第二击关闭，状态及角色速度与选择同步。
- 反例：按住时每个采样帧反复切换，点击一次出现多个状态变化，应失败。
- 当前入口/边界：`touch-actions` 的 `launcher-touch-native-consumers` 将生产 `createPlayerTouch` 接到正式 Runtime。两组 PointerEvent 点击分别打开和关闭低速，抬手后继续逐 Tick 读取真实玩家消费者，并将实际位移与独立键盘 Shift 校准的低速、高速比较。共用驱动使用有明确几何的 DOM 控件，布局与设备表现进入集中人工审查。

### TOUCH-07 | 双指低速

- 操作：第一指保持移动，第二指在游戏操作区的非按钮位置加入，再分别抬起两指；同时记录移动指在全程的拥有权。
- 观察与独立预期：记录每个 pointerId 的拥有者、低速位与本机角色坐标；第二指只控制低速，第一指继续提供移动。
- 判定：加入和抬起第二指不取消第一指移动，低速状态按本作双指规则变化。
- 反例：第二指抬起时清除第一指移动，或把两根手指都当成移动输入，应失败。
- 当前入口/边界：`shared-touch-limited` 在同一局让第二指加入、移动和抬起，同时逐 Tick 核对第一指仍拥有移动目标。该场景按实际 `touchFocusMode` 选择焦点比例；只有以 `two-finger` 配置启动的执行才证明双指低速状态，其他焦点配置的执行只证明指针 owner 隔离。player-touch 另有合成生命周期场景。

### TOUCH-08 | 射击、Bomb 与暂停

- 操作：在关卡中依次点射击、Bomb、暂停；再组合移动与按钮输入并恢复游戏。
- 观察与独立预期：检查原生射击/Bomb 消耗、角色位置和暂停阶段；暂停状态以本作原生状态与帧冻结为准。
- 判定：射击按钮按本作的保持或切换模式改变状态，每次 Bomb 点击只消费一次；暂停冻结世界，恢复后输入和移动继续。
- 反例：暂停后世界帧仍推进，或 Bomb 点击被重复消费，应失败。
- 当前入口/边界：th08 normal 的 `runTouchActionSceneGroup` 通过 Runtime 控件与真实触点执行焦点开关、射击、普通 Bomb、移动保持和 Escape 暂停恢复；`touch_action_state_v1` 从完成的玩家消费者边界读取动作、计时、活动弹数与库存状态。Deathbomb 与结算 Replay 按各自玩法生命周期独立核对。

th08 normal 的动作观察使用 `touch_action_state_v1`，在 `GameplayScene::update_player` 成功完成 `PlayerSimulation::update_player_frame` 后复制当前消费者状态。20 个 `u32` 依次提供 ABI/有效位/阶段、消费者序号/逻辑帧/关卡、实际消费的当前与前一输入、焦点、射击计时与活动弹数、Bomb 状态与库存计数、生命状态。阶段 `1` 表示玩家消费者本逻辑步已完成，阶段 `2` 表示暂停步；暂停只公开帧、关卡和生命字段。有效位 `0x01–0x80` 依次代表帧关卡、输入、焦点、射击计时、活动弹数、Bomb、Bomb 库存和生命状态。普通 Bomb 操作先核对原生 `active=false`、`triggered=false`，随后以消费的 Bomb 位、实际活动状态与库存计数变化共同确认一次触发。观察读取不推进游戏，也不写入输入。

### TOUCH-09 | Deathbomb 与移动保持

- 操作：在原作可触发 Deathbomb 的关卡样本中保持移动并于死亡弹窗口触发 Bomb，按固定帧观察结果。
- 观察与独立预期：用原版规则或已锁定样本确定有效窗口，读取 Bomb 消耗、死亡/复活状态和移动输入是否持续。
- 判定：Bomb 在窗口内按原作生效，移动保持按适配规则继续；窗口外输入不得延迟到复活后才生效。
- 反例：窗口外点击被缓存，复活时意外消耗 Bomb，应失败。
- 当前入口/边界：共享场景 `runTouchDeathbombScene` 分别执行键盘末帧、触控末帧和触控迟到三条路线。th08 normal 在 `validation_scene` 与 disposable-write 锁有效时，通过 `validation_deathbomb_hit_v1(app)` 发射一颗 `etama.anm` 已加载模板的真实零速子弹；位置取当前玩家位置，随后由正常 BulletUpdate、PlayerCollision、PlayerLife 和 PlayerBomb 流程处理。场景等待自然无敌结束，保持移动触点并关闭射击，使碰撞来源可单独核对。
- `validation_deathbomb_state_v1(app)` 提供固定 36-word 只读 ABI：包含刺激请求/弹体状态、带对象身份的 BulletSystem kind-2 碰撞收据、碰撞时消费者序号、当前消费者序号、Deathbomb 窗口、Bomb 状态、库存、生命和死亡计数。碰撞收据要求正常玩家消费者已完成，并确认 life state 从 0 进入 2 且 `predead_count>0`。
- [`runTouchDeathbombScene`](../../tools/ports/touch-deathbomb-scenes.mjs) 使用同一套共享判据覆盖键盘与触控 Bomb 输入，以及窗口最后一帧与窗口结束后的输入。场景以已完成的玩家消费者序号配对真实碰撞、输入位、Bomb 状态和库存；同一根移动指针先校准位移，再核对死亡窗口内的位置保持及有效复活后的移动恢复。late 组继续运行至自然失去生命和可移动复活，并确认延迟输入没有触发 Bomb。
- 玩家消费与后续子弹碰撞分别保留采样边界。命中当帧通过同一消费者序号和带对象身份的碰撞收据关联前后生命状态；输入排队时长由本作生产顺序声明，Bomb 必须实际消费在窗口最后一帧。消退中的命中弹体按本作回收状态核对，并在恢复场景结束前确认槽位回收。
- 死亡窗口以命中位置检查运动锁定，复活阶段以本作声明的原生出生位置检查；原作复活移位与触控位移分别核对。恢复后先让输入所有者采样可移动状态，再用同一根指针提交新的相对位移。生产 `TouchController.hpp` 的组件回归同时覆盖恢复事件先到与输入采样先到两种顺序，验证重基线与指针所有权。
- Node 组件回归验证真实碰撞、窗口内生效、窗口外不缓存、库存/生命变化、移动保持及序号缺口处理。完整 Runtime 结论由产品 driver 提供控制输入和原生观察；标题声明死亡状态、Bomb 类型/成本、触控脉冲和画面坐标。

### TOUCH-10 | 无限速移动落率记录

- 操作：在普通录制局启用无限速移动，先执行零位移对照，再实际移动，跨阶段进入结算，保存并重新读取同一局 Replay。
- 观察与独立预期：确认移动坐标实际发生变化，再分别读取结算值和 Replay 内落率字段；期望值是原表声明的 100%。
- 判定：完成同一局之后，结算与保存记录均显示约定值；未移动、未结算或 Replay 不可读都不算通过。
- 反例：只打开选项、全程不移动却报告通过，应失败。
- 当前入口/边界：`runTouchUnlimitedResultSceneGroup` 接收已准备好的 normal Runtime 与产品 IO，先以零位移作为不置标记对照，再要求真实非零 unlimited 消费、同一 run 跨阶段保留标记、生产 Result 落率和正式 Replay finalizer 均可读取。Replay 结果由标题解析器从保存的原始字节独立提取；作品适配器提供只读标记/Result 观察和有界场景转换请求。

### TOUCH-11 | 双击 Bomb

- 操作：分别以启动选项 `doubleTapBombEnabled=true` 和 `false` 建立 Runtime。按住第一根移动指，在同一位置用第二根手指完成两次点击；第一次点击后先取一次原生消费者样本，第二次点击沿共享批次紧接执行。关闭选项的实例发送相同手势作负控。
- 观察与独立预期：使用 `touch_action_state_v1` 的玩家消费者样本读取当前 Bomb 输入、实际 `triggered/active` 状态和剩余/已用库存；原生位置证明第一根手指在 Bomb 期间继续移动。`bombPulseTicks` 与首击/间隔时限来自本作声明；样本覆盖输入脉冲清空的一帧，消费次数由原生库存变化计数，不按持续数帧的 Bomb 位重复计数。
- 判定：首击不消耗 Bomb；启用实例的双击令剩余数减一、已用数加一且仅一次，关闭实例的同一手势保持库存不变；主移动指在 Bomb 激活时仍产生位移。原生 Bomb 持续状态按实际 `active` 观察，不以其多帧持续误判重复消费。
- 反例：单击即消费、关闭选项仍响应、一次双击增加多次已用数、脉冲未在声明的有界 tick 数内清除，或次指手势取消主移动指，均应失败；缺少启动选项回执、原生消费者样本、transport 时序或有界样本则为 incomplete。
- 当前入口/边界：[`runTouchDoubleTapBombScene`](../../tools/ports/touch-double-tap-scenes.mjs) 共用相同判据，要求 Runtime driver 在一个有界 direct-touch 批次中回传首击检查、第二击后的逐帧消费者样本和每个事件的真实 transport 时间。`touch-native-invariants` 的 WASI fixture 继续承担生产 Controller 组件检查；Node 场景测试验证断言与负控，完整 Runtime 证据由实际接入的 driver 产生。

### TOUCH-12 | 菜单与对话手势

菜单滑动场景保持同一触点越过本作原生方向键的自动重复起点，逐逻辑 tick 核对游标只移动一次；手指返回起点后松开也保持这次滑动的所有权。th08 的 Controller 为每个菜单手势保留已导航状态，下一次 DOWN 建立新手势。生产组件回归同时运行真实 `InputFrame::update`，覆盖方向键自动重复与新手势恢复。该规则适用于全部作品，其他普通版与 MP 按各自 Controller 接入同一场景。

- 操作：以触控 ingress 运行 `runOriginalMenuInputScene`：在起始菜单点击一次进入难度选择，保持触点滑动一次，双指静止返回；在真实已加载 MessageProgram Wait 中先执行短按确认场景，再以 `runLoadedDialogueSkipScene` 保持单指超过本作声明的 Skip 阈值。
- 观察与独立预期：菜单方向、确认和 Escape 必须从原生 title input 与 cursor/screen 状态读取；对话短按复用 `runTouchDialogueFreshConfirmScene` 的 fresh-confirm consumer receipt，长按则检查真实 pointer transport 时长、Dialogue Skip 消费位和 MessageProgram 指令游标。
- 判定：一次滑动对应一个方向边沿和一项游标变化，点击进入声明菜单，双指返回恢复起始菜单；短按产生一次确认边沿，长按消费 Skip 并前进脚本，二者不互相冒充。
- 反例：没有菜单原生输入却仅凭触点回执通过、一次滑动跨越多项、双指取消没有产生菜单返回、短按被计成长按，或长按没有被 Dialogue consumer 消费，均不能通过。
- 当前入口/边界：[`original-menu-input-scenes.mjs`](../../tools/ports/original-menu-input-scenes.mjs) 将触控事件逐段送入生产 `TouchController`，在 swipe 指针仍持有时采样方向；[`loaded-dialogue-skip-scenes.mjs`](../../tools/ports/loaded-dialogue-skip-scenes.mjs) 只接受当前 ECL 引用且已加载的 MSG entry，并以真实触摸持续时间验证长按。短按确认仍由 [`touch-dialogue-confirm-scenes.mjs`](../../tools/ports/touch-dialogue-confirm-scenes.mjs) 验证；Launcher DOM 组件测试不能代替原生 Runtime consumer 结果。

### TOUCH-13 | 确认键的新按下边沿

- 操作：先保持射击进入对话或结算，再通过触屏重新按下确认；MP 时由另一席位保持射击。
- 观察与独立预期：记录按下边沿、当前席位、对话/结算状态和下一阶段原生状态；确认必须产生新边沿并推进流程。
- 判定：原有射击保持不吞掉确认；MP 两席输入和流程状态分别核对。
- 反例：确认只改变一个共享 held 位、没有新按下边沿，流程停在原状态，应失败。
- 当前入口/边界：共享场景 `runTouchDialogueFreshConfirmScene` 先要求 `touch-controls` Fire 在普通玩家消费者中真实出现，再经 `sdl_dialogue_touch_request_v1` 打开当前普通关卡 ECL 实际引用、且已加载 MSG 首个分支前含 Wait 的 entry。受控请求经 `globals.timeline_actions->message(entry)` 进入 `EnemySimulation::message`、`GameplayScene::message` 和正式 `Dialogue::read`，不直接改 message、instruction、timer 或 input。driver 再以普通 Touch Pointer down/up 产生确认，按 `GuiFlow::update` 调用 `Dialogue::update` 时记录的 current/previous input、消费序号与有效位验证新边沿、message 指令游标推进一次和后续 pulse 不重复；priority-17 Replay publisher 后写入的下一 tick 输入不会替代消费回执，停钟或没有活动 Dialogue 时回执无效。TH08 的 `consumerLatencyTicks:1` 对应 GuiFlow priority 15 到 Replay recording priority 17 的输入发布顺序。

`sdl_dialogue_touch_manifest_v1` 的固定 344-word ABI 使用 `DTM1`、版本 1；header word 0..23 依次为 magic、version、总 word 数、available、reason、Runtime generation/pointer/frame-loop epoch、stage/character/team/difficulty mask、实际加载 ECL 字节长度与 FNV64、timeline 数、实际加载 MessageProgram entry 数/字节长度与 FNV64、ECL/MSG asset id、候选数和 row stride。asset id 1 是普通 `ecldata1.ecl`，MSG asset id 1..4 分别是 `msg1a.dat` 到 `msg1d.dat`；team 来自生产 `GuiFlow::setup` 的映射。候选行从 word 24 开始，每行 10 words，依次记录 MessageProgram entry、普通 ECL timeline、ECL 触发 frame、难度位、ECL 字节 offset、活跃 opcode-6 引用数、首个可直达 Wait 的 MSG byte offset/指令 time/自动超时 frame 数，以及该 entry 的指令数。只有当前正常 Stage 1 的已加载 ECL opcode 6 在 active difficulty 引用该 entry，且 Wait 出现在 Selection、ReadSelected、Delete、Halt 或 StageEnd 之前，entry 才进入候选表。

`sdl_dialogue_touch_observation_v1` 返回固定 45-word `DTO1` ABI，分别提供 Runtime generation/pointer/epoch、host/frame/stage identity、native validation gate 与 disposable-write receipt、Dialogue present/message/instruction offset/opcode/time/timer/paused/minimumWait、真实 Dialogue consumer 的 current/previous input、fresh confirm edge、消费序号与有效位、control/ECL frame、Touch Fire/context、host Z 和刺激请求回执。word 34 是 `consumerSequence`，word 44 是 `consumerValid`。`sdl_dialogue_touch_request_v1(entry)` 每次重新验证当前普通场景、gate、write lock、无现存 Dialogue、Fire 控件保持开启及 ECL/MSG 候选；成功只调用生产 timeline action，不写存档。缺少候选、加载来源、锁或原生观察都作为 incomplete。

共享判据与 Node 合成 IO 反例位于 `tools/ports/touch-dialogue-confirm-scenes.mjs` 和 `.test.mjs`。组件测试验证 source receipt、Fire 前置消费者、无触摸时 Wait 保持、真实消费者 fresh edge、指令游标推进和三 tick pulse 不重复。`tests/adaptation/adapters/dialogue.mjs` 按登记 contract 中的 Runtime mapping 与 export/ABI 数据读取原始字并按固定布局归一化，保留实际加载源哈希和全部候选行；reader 和 Node 场景错误都以 `error.evidence` 交给 runner。适配器声明本作 `waitOpcode`、`inputMasks.confirm` 和带源码锚点的 `gesture.maxTapDurationMs`，共享判据按声明识别确认位置、边沿和有效手势。它不构成 Runtime 实跑证据；统一结果只有在桌面 driver 读取上述 ABI 并完成真实 pointer 场景后才能记录 TOUCH-13。

### TOUCH-14 | 100–300% 灵敏度

- 操作：在 100%、200%、300% 下，从相同起点执行同一组滑动，再重复低速控制样本。
- 观察与独立预期：记录触控坐标映射、原生坐标变化和逻辑速度；灵敏度范围与处理规则来自产品契约，低速比例按原作值。
- 判定：灵敏度只改变触控距离映射，原始速度与低速比例保持不变；超出声明范围的值按契约处理。
- 反例：提高灵敏度使同帧原生速度或低速比例改变，应失败。
- 当前入口/边界：共用运动场景在同一局发送三档 `touch-controls` 设置，以已发送指针位移计算目标，并逐帧核对真实位置、原生速度和消费的灵敏度；同一组微移再以独立键盘校准的低速重复，并在关闭焦点后验证高速恢复。runtime-contracts 核对范围声明，player-touch 核对 Launcher 触控桥接。

### TOUCH-15 | 触控与其他输入隔离

`touch-actions` 将键盘、生产手柄映射和触控 Controller 接到同一 Runtime，分别检查释放后另一来源继续射击，再保持触控焦点并取消，要求焦点释放、另一来源的原生射击消费连续保持。切换式射击在取消前显式关闭，避免它掩盖其他来源丢失；手柄释放后核对最终中立状态。

物理键盘场景按自己发送的按键维护所有权，并读取生产 mapper 的物理事件回执。桥接汇总状态可以由虚拟触控继续保持，不能用这个汇总状态决定是否补发物理键。按住式触控射击另验取消释放和新触点恢复，切换式射击配置保持自己的生命周期。

- 操作：物理键保持一个动作时开始再取消触控，之后换成手柄保持并重复。
- 观察与独立预期：按来源记录输入 owner、汇总按键位及原生消费位；取消一个来源只释放它自己的按下。
- 判定：其他来源保持有效，最终来源释放后才回到中立。
- 反例：取消触控时发出全局释放，把仍按住的键盘动作清掉，应失败。
- 当前入口/边界：th08 normal 的 [`GameHost::poll`](../../ports/th08/web/cpp/sdl/GameHost.cpp#L102) 把方向键交给 `TouchController::sample`；[`TouchController::sample`](../../ports/th08/portable/input/TouchController.hpp#L72) 在方向键生效时清除触控移动。场景因此先核对 keydown 前的中立队列，再要求首个 movement 行保持原位并发布精确方向位；取消触控前该方向必须仍在队列中。取消回执已读取的首行和随后两行都按独立键盘校准方向、速度逐条判定，keyup 回执队列若仍有方向，只允许下一行消费该方向并清除队列，之后连续三行回中。此项证明触控取消不会释放键盘方向，同时尊重 th08 normal 的方向键接管行为；其他产品按各自来源策略使用对应场景。

### TOUCH-16 | 触控取消与生命周期

- 操作：触点按住时触发 pointer cancel、页面隐藏和窗口切换，恢复后再次输入。
- 观察与独立预期：记录 pointerId 的 down/cancel/up、输入桥消息和原生输入；离开前台时触点应释放且恢复后无旧触点。
- 判定：取消只结束对应触点，页面恢复后角色和按钮处于中立，新的手势可正常开始。
- 反例：hidden 时未发送释放，回到前台仍移动，应失败。
- 当前入口/边界：player-touch 测试用合成 PointerEvent、visibility 和生命周期验证桥接取消；这不覆盖真实移动浏览器、设备指针路由或实际 WASM 效果。

### TOUCH-17 | 共用布局编辑器

- 操作：在 Launcher 编辑按钮位置和大小，保存后切换横竖屏并分别重开编辑器。
- 观察与独立预期：读取编辑器保存的规范化布局、方向标记与恢复后的按钮矩形；预期来自共用布局数据模型和当前屏幕方向。
- 判定：编辑值持久化，横竖屏布局独立恢复，操作控件不会替代游戏输入。
- 反例：竖屏保存覆盖横屏布局，重返横屏时仍出现竖屏位置，应失败。
- 当前入口/边界：`runLauncherInputRuntime({kind:'layout', ...})` 使用生产 `createTouchLayoutModel` 与 `applySavedPlayerTouchLayout`，在隔离存储中分别编辑横屏、竖屏配置，并在已启动的 `#player` 控件树中重载和应用；捕获回执逐个绑定控件 ID 与 Player owner。桌面场景由生产 `PlayerSurface`、`TouchControlPreview` 和 Launcher 样式表渲染，CSS 引用的公开字体与图片经 Launcher public-source 路由提供。Playwright 改变真实 viewport 并读取安全区、控件矩形和缩放值，结束时恢复调用方 viewport，原生消费者序号及输入状态保持不变。设备方向传感器与实体屏幕由设备验收补充。

### TOUCH-18 | 共用重开按钮

- 操作：先用物理 R 建立独立重开基线，再在新的暂停状态点生产 Launcher 重开按钮；作品确认步骤沿用登记的 title flow。每次动作开始前先启动原生观察。
- 观察与独立预期：以稳定暂停快照的本作逻辑帧为边界，在 R／按钮与作品确认操作期间连续读取原生状态；记录第一条 gameplay-ready 且逻辑帧低于暂停边界的重置样本，再核对恢复后的 gameplay readiness 与后续帧推进。同一 Host、Runtime iframe 文档、Runtime/Module/core、epoch 与首帧事件必须保持同一实例。
- 判定：键盘基线和按钮路径各自具有真实输入证据、暂停边界以下的原生重置样本及恢复后的帧推进；观测缺失时结果为 incomplete，两条路径不得共用输入事件。
- 反例：按钮只重载页面、关闭暂停或伪用物理 R，应失败。
- 当前入口/边界：`runtime-desktop-probes.mjs` 的 `shared-touch-restart` 使用生产 `createPlayerTouch` 控制器的 Restart DOM，和原生 Runtime 观察共同核验；触控版式仍由小范围人工审查负责。

### TOUCH-19 | 共用双指放大镜

- 操作：在游戏画面内双指缩放、回到默认比例，再点击一个可观测的游戏坐标目标。
- 观察与独立预期：读取缩放比例、画布位置和原生触点坐标；缩放只改变画面显示，游戏坐标仍映射到正确目标。
- 判定：缩放和复位正确，缩放前后同一画面目标产生同一游戏坐标。
- 反例：放大后点按位置偏移到另一原生目标，应失败。
- 当前入口/边界：`runLauncherInputRuntime({kind:'magnifier', ...})` 通过 CDP 双指触摸驱动生产 zoom controller，触点命中 `PlayerSurface` 中 `TouchControlPreview` 实际提供的 `#touchDirectSurface`，并读取可信 DOM Pointer 身份、surface owner 与 iframe 矩形；随后在缩放前后各执行相同画框相对坐标的可信触摸，并将原生 `native-touch-trace-v2` 目标位移逐项比较，最后验证恢复默认比例。调用方提供原生 step 和 target trace reader，Playwright facade 负责触摸派发与 fixture 清理。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[能力定义](../../launcher/src/contracts/adapter-capabilities.mts) | [键盘桥](../../common/browser/keyboard-input-bridge.mjs) | [直触桥](../../common/browser/direct-input-bridge.mjs)
