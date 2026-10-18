# TH15 高刷：实施前审计与验收方案

日期：2026-10-06。本文保存实施前审计；实现与当前验收结果见 [实施记录](EAGLER-HIGH-REFRESH-IMPLEMENTATION.md)。
这份文档不代表完整状态覆盖，也不代表 TH15 已通过 Presentation Lab。

## 基线与参考

- 原作追踪：`th15/main`，`fe113b6`。
- 已接受的非高刷修复：`th15-eagler/eagler`，`eaf0fb5`。
- 高刷实验：`_scratch/th15-high-refresh`，`experiment/th15-high-refresh`，从上述 Eagler 提交开始。
- TH10 参考：`th10/eagler`，`2649217`；实际入口是
  `th10_web/cpp/sdl/ApplicationHost.cpp` 与 `cpp/platform/Application.cpp`，
  不是仅供原作比较的 Presentation 捕获入口。
- TH10 固定的 common 子模块：`8316c4f861dedb67e1e0e7be75ddcf0b90f63448`。
- 工作区 common：`7086762de0160d2f5eb2ec1db6192f52b185d846`。
  两者不能混作同一版本；审计时 TH15 尚未固定 common 子模块；实现已固定下述 TH10 revision。
- 规范：`eagler-touhou/docs/ADAPTING_A_GAME.md`、
  `ADAPTER_CAPABILITIES.md`、`ADAPTER_BEHAVIOR_INVARIANTS.md`，
  `docs/playbooks/interpolation.md`、`adaptation-worktrees.md`，以及
  `eagler-common/testkit/presentation-lab/CONTRACT.md`。

用户要求先研究上述参考再实施。先前只重复屏幕纹理的原型不满足连续运动
插值要求，已保存为 Git stash
`codex-th15-cached-presentation-prototype-not-accepted-20261006`，
并从实验工作树移除。它的 Wasm、测试文件和构建结果不能用于高刷验收。
canonical Eagler 分支未收到该原型。

## TH10 可采用的行为边界

TH10 在固定 tick 采集输入并执行完整 Application step，额外显示帧只执行
presentation_draw。previous/current 端点属于逻辑 tick；alpha 来自剩余时间。
ANM 只对已分类的连续字段插值，资源、脚本、可见性和计时回退是生命周期边界。
Player、Bullet、Item、Laser、背景、弹出分数及 HUD 有各自的呈现端点，不能只
在通用 ANM 中插值后就宣称对象覆盖完整。

TH10 的 render-only 路径还保存/恢复共享相机、图形状态、临时文字和动画管理
记账，并通过对象副本处理 Draw scratch。其源码门禁证明这些具体机制存在，
不证明 TH15 的完整重复 Draw 是纯的。共享规范明确把 TH10 Lab 接入列为有限
覆盖；覆盖不全仍必须报告 unknown。

TH10 FrameCadence 每个 RAF 最多执行一个 tick，迟到时跳过过期时间槽；TH15
当前 FrameCadence 会积累最多 100ms、每回调补最多四个完整 update/draw。
不能为统一代码而直接替换策略。实施前需核对 TH15 自身原作调度语义，并在
短停顿、持续低帧率、焦点恢复与 Replay 中验证，避免引入补帧导致的移动端负载尖峰。

## TH15 Draw 入口与副作用

当前 ApplicationState::step 将逻辑推进、音频、Draw、符卡时间采样、checkpoint
压缩、资源清理与场景切换组成一个事务。SceneDisplay::draw 调用真实 scheduler
Draw，随后完成截图并 Present。不能把 step 或整个事务放进额外显示回调。

| 入口/owner | 已观察到的行为 | 额外显示帧的处理要求 |
| --- | --- | --- |
| StageDrawFrame background/foreground | 创建背景 fade；写 flags、transition、tint；foreground 递减 transition | 状态变更留在一次原作 tick；临时背景副本或只读呈现路径 |
| StageScene::draw | 写 object/instance flags、剔除计数、draw_objects；同一 primitive VM 在多个实例中重写 translation/scale | 实例身份必须包含实例编号；不能只用 VM 指针；临时变换不得写回 |
| GameDraw spell/HUD/popup/pause | 生成并排入 ASCII 队列；prepare_frame 重建 draw context | 每 tick 生成一次，额外帧读取同一份视觉输出 |
| SpellDraw | 无效 banner 时清零 handles[3]；调整 style；输出 Bonus/history | handle 清理属于 tick；保留离散数值与稳定文字队列 |
| GameHud::draw | 清理 chapter/result handle；改变 tutorial_state，可能 retire background_notice | 额外 Draw 不得执行清理、retire 或状态转移 |
| Player::draw | 重写 root translation/render_flags | 从 Player 端点派生临时 root；碰撞与运动读取当前逻辑位置 |
| BulletVisual::draw | 重写位置、朝向、secondary_scale，并写 state.visual_flags | Bullet owner 控制生命周期；render copy；消弹/重用/类型变化不能跨边界插值 |
| ItemManager::draw | 重写 body/arrow translation、arrow Y/color，并写 state.appearance | 显示/箭头分支使用当前逻辑判断；呈现位置单独插值 |
| LaserVisual | 重写 body/head/tip；曲线由点序列生成 strip | owner 端点、宽度/长度/朝向与曲线拓扑需分别分类 |
| PauseDraw | 改 snapshot_animation、captured color、文字 scratch；replay_name 直接填写 live Replay 描述 | 额外显示帧不得写 Replay bytes；菜单文字每 tick 生产一次 |
| TitleFrame | 分派 Practice、Replay、Player Data、Records、Replay Save 文字 Draw | 必须审计各被调用 owner；读日历与文字生产也不能随显示帧重复 |
| AsciiText | 更新 glyph VM 的 position、sprite、scale、flags、color 与几何 scratch | 字形 VM 是共享临时对象，不能以其指针作为每个字形身份；文字不可重复入队 |
| ScreenCompositor | 修改 environment、target、captures color、view offsets、renderer tint 与 pipeline | 呈现资源操作可重复，但作者状态与共享 scratch 必须恢复 |
| SceneCapture::finish_frame | 执行 copy 并消费 requests | 截图任务只执行一次；额外帧不能重复消费或改变捕获时机 |
| ScreenFade | alpha/age 在 Update 推进，Draw 写 pipeline/viewport | 生命周期稳定时插值 alpha；额外帧恢复渲染状态 |
| ScreenMotionFrame | Update 消耗 RNG 生成 offset | 不重复 Update；随机震动端点不能自动当作连续运动 |
| EndingFrame | 原作脚本/age 在 Update，Draw 回调本身为空 | 仍需覆盖动画、背景换页、资源加载与文字 owner |
| FPS Draw callback | 生成 FPS 文字并加入 captions | 分开 simulationFps 与实际 presentation fps；不得因额外帧重复入队 |
| FrameScheduler | Draw callback 支持移除、重复、停止，callback 执行不天然幂等 | 不通过多调用 scheduler.draw 来假定纯度 |

表中是入口和已确定副作用。Title 下属 Draw、敌人/附着特效、弹出分数、overlay/
trail/distortion/custom mesh 的完整字段覆盖仍为 unknown；实施需逐项补充 owner
登记及运行证据，不能把这份静态清单称作完整状态纯度证明。

## 实施选择与顺序

1. 先建立完整 tick/presentation 边界，保留一次原作 Draw 的副作用顺序及符卡
   wall-clock 采样时机。另设只读额外呈现入口，不调用 sample_and_tick，不推进
   Replay、RNG、ANM 脚本、音频请求、checkpoint 或场景切换。
2. 优先评估 TH10 的对象副本/owner sidecar 方案。另一个可研究的方向是保存
   原作 Draw 产生的渲染命令，额外帧只提交命令与临时视觉端点，从而避免重入
   副作用；它尚未选定，也不能退化成上一张屏幕纹理重复 Present。
   命令方案仍须处理多 render target、clear、capture、资源版本及失效，不能
   以 draw 序号或全局顶点混合代替对象身份；复制成本也必须实测。
3. 明确对象登记：权威 writer、端点发布边界、presentation consumer、最终
   提交点、生成代号、连续字段、跳变条件、恢复规则与指纹覆盖。
   Player/Option/判定点及附着视觉必须使用同一最终 owner 位置；Bullet、Item、
   Enemy、Laser、背景、HUD、标题动画与效果分别处理。字段语义未知时使用当前端点。
4. 修正最终采样边界：AnmRenderer::pack_quad 当前像素路径在 2x 下吸附到
   半个逻辑像素。连续运动保留 fractional coordinates，只有明确像素对齐的
   字形/静态元素保留吸附。保留原作字形和现有高清字体采样，不能统一恢复线性模糊。
5. 接通规范中的 `configure.options.limitPresentationTo60`，不是 Launcher
   偏好名 `frameLimit60Enabled`。60 限制只限制显示，不能改变逻辑、输入与音频。
   shell/managed 的 first-frame 必须来自实际成功呈现；frame-health 显示帧率
   也不能继续只统计含逻辑 tick 的 RAF。
6. 暂停、加载、重试、checkpoint 恢复、Stage Clear/换关、结束、标题返回与
   焦点恢复清理/同步端点。世界暂停时冻结世界 alpha，菜单/UI 依自身逻辑处理。
   不新增固定最短加载时长，不让旧端点遮住已修复的“少女祈祷中”。
7. 使用固定 Git 版本的 common testkit，并直接导入其 controller/contracts。
   比较 TH10 的固定版本与当前 common 兼容性后选择准确 revision；不要依赖
   sibling main 浮动。TH15 提供自己的 RuntimeDriverV1、ObservationAdapterV1，
   不复制 shared controller；冻结 token、完整 tick receipt、原始 Draw reference
   和状态覆盖必须符合共同契约。诊断 profile 与生产 Runtime 分离。

## 验收门禁

| 门禁 | 预期证据 | 实施前 TH15 高刷状态 |
| --- | --- | --- |
| 60/120/144/165/240Hz | 实际 RAF，固定 tick/input/Replay 数量；更高显示频率 | 未运行 |
| 开关 60Hz 限制 | 限制后无额外 Draw，逻辑及 Replay 轨迹一致 | 未实现 |
| repeated-alpha Draw | 同一 tick 的 alpha=0/.25/.5/.75/1，多次重复；状态指纹不变 | unknown |
| 故障负对照 | 人为破坏插值/纯度会被检测，不能只验证正常路径 | 未实现 |
| 连续字段/生命周期 | 原始权威 Draw 独立端点，分类已观测/unknown/通过 | unknown |
| 暂停/焦点/加载 | 无额外输入、旧运动重放或墙钟补帧；祈祷画面仍呈现 | 未运行 |
| 普通/符卡/六关/Extra/结束 | 各 owner 与资源边界、Stage Clear 移动修复无回归 | 未运行高刷版本 |
| Replay/存档/Pointdevice | 原作文件兼容、checkpoint 九模块、重试与音乐定位一致 | 未运行高刷版本 |
| OGG/无音乐 | 音频队列与控制请求只按原作事务执行 | 未运行高刷版本 |
| 移动端性能 | 同基线测 CPU/GPU、内存、绘制提交；实体设备稳帧 | 未运行 |
| 生产构建 | 固定 common/source/Wasm 身份；无诊断 ABI 或私有内容入包 | 未构建高刷版本 |

2026-10-06 本次只运行参考检查：TH10 check-high-refresh-contract 与
check-presentation-purity 通过（源码结构检查）；工作区 common 的
controller-core/release-contract 共 8 项通过（共享 JavaScript 契约测试）。
这些结果不是 TH15 高刷、实体移动端或完整 Lab 的 PASS。

非高刷 eaf0fb5 的加载、高清字形、Stage Clear 移动、checkpoint 保存与整局
完成检查仍是各自原有证据，不自动覆盖调度/Draw 拆分后的高刷实现。

## 合入边界

高刷只在本实验分支修改，canonical `th15-eagler/eagler` 继续作为已接受 Runtime
来源。推广前记录原作 commit、Eagler base、实验 head、完整 diff、全部通过与
unknown/not-run 门禁；不能只凭 cadence 测试或截图合入。合入后重新构建 canonical
并跑 Launcher/runtime/package 检查。未证实的物理手机稳 60 或完整视觉覆盖不做承诺。
