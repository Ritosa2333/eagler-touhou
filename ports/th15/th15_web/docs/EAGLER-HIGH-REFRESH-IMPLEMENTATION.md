# TH15 高刷实施记录

日期：2026-10-06。用户明确要求推送并合并高刷与普通分支；`b8bc2cb` 和实验 head `40186c3` 已先推送。当前将实验实现合入 canonical `th15-eagler/eagler`，保留以下历史验收及未完成门禁，不升级 unknown 为 PASS。

## 身份与行为

原作 `th15/main`：`fe113b6`；Eagler base：`eaf0fb5`；实施前审计：`0614ff0`。完整变更以实验分支 HEAD 与 `eaf0fb5` 的 diff 为准，包含本记录；具体提交可从 `git log` 查询，不用生成产物替代源码身份。
TH10 参考 `2649217`，新增 common 子模块固定 `8316c4f861dedb67e1e0e7be75ddcf0b90f63448`，测试直接导入其 analyzer，未复制 controller。

逻辑、输入、Replay、音频请求、截图和 checkpoint 继续由原有 60Hz `ApplicationState::step` 事务执行。保留 TH15 已有 bounded catchup；不把 TH10 的逻辑调度直接套入 TH15。

TH15 的原始 Draw 有章节过渡、ASCII 队列清理、Replay 名称写入和截图消费等副作用。因此额外显示帧不再次执行游戏 Draw，而在 GraphicsDevice 记录原始 Draw 的不可变 GPU 状态、清屏与几何提交，显示帧在临时缓冲上插值并提交。不是重复上一张截图，也不推进 ANM、ECL、RNG、输入或音频请求。两帧缓冲复用、端点索引每 tick 建立，保留原有批处理。

插值按 owner/instance/generation/occurrence 识别。角色、子机、焦点、屏障、子弹、敌人、道具、射击、激光和浮动数字标记实际运动；通用 ANM 仅处理有明确曲线或速度的连续位置、旋转、缩放、颜色、透明度与 UV。STD 共用 VM 的实例有独立身份，父级重建、脚本/资源/可见性改变、年龄回退、翻转与瞬移丢弃旧端点。暂停的世界取当前状态；屏幕抖动保持当前 tick 的离散偏移，角色运动相对偏移插值。固定文字保留原字库与像素对齐，运动几何保留小数坐标。

加载、资源释放、重试、checkpoint 恢复及截图复制/缩放重置缓存。RAF 检测高刷后启用额外呈现；`limitPresentationTo60` 由 shell/managed 连接原生入口，重复应用同值不重置检测。Frame Health 回报实际显示与执行 tick 数；生产构建不含诊断 ABI。

## 验证边界

高刷核心原始提交 `dc4309b` 的 Development Wasm：`7461967949f78e974de949d2572e12767ac45c4755a3a49dcdb92ec8280e53f0`，2242868 bytes。
高刷核心原始提交 `dc4309b` 的 Release Wasm：`910e5bea4d04e5d1f707b7b08cafe27204327162bc70f8f6254448899dbd3901`，1980698 bytes。
生产导出检查通过：没有 `application_*`、`th15_probe_*`、`audit_*`、`presentation_lab_*`；存在 `th15_limit_presentation`。

固定版本 common 的 controller/release-contract 共 8 项通过；shell/managed 与新测试语法检查、diff whitespace 检查通过。
启用真实额外呈现后，三项实际原作 DAT / measured fonts 浏览器回归通过：2x 栅格、进入与换关加载、Replay/Player Data 页面、Stage Clear 移动；普通六关 ending/staff/results 和 Extra 完成及 Replay 写出；六关 Pointdevice IDBFS 冷启动恢复并继续 300 帧、待编码保存删除取消。完成测试使用控制完成与测试无敌，不能当作自然通关或完整 Replay 轨迹对照。

高刷测试直接比较原始权威提交端点、alpha=0/.25/.5/.75/1 与重复 alpha、每次绘制后的九模块 checkpoint 原始序列化摘要和世界状态，核对 alpha=1 像素一致与重复 alpha 像素幂等；故意强制 alpha=1 的负对照必须被 analyzer 拒绝。屏幕偏移测试独立验证离散抖动与平滑移动分离。

此前共享一个浏览器反复打开完整 Runtime 页面，在并行构建时 240Hz 页面初始化超时，串行重跑也曾在后续页面初始化等待；并非已证实的游戏异常。最终测试改为每个刷新率使用独立浏览器进程，结果见下方最新验收。失败日志保留，不升级为 PASS。

## 尚未验收

Presentation Lab 只观测 Player 顶点，其他视觉 owner 的完整观察覆盖、Replay 游标、所有 UI 与 rollback 状态仍为 unknown；不是完整 RuntimeDriverV1/ObservationAdapterV1 Lab consumer。九模块摘要与不可变提交实现提供部分状态与机制证据，不能伪造完整纯度结论。
实体手机/GPU 稳帧、全字段与全生命周期观察、黄金 Replay 全程对照、打包 Launcher 导入/离线/资源协议复验尚未运行。桌面 SwiftShader 的 CPU 提交时间不是移动端性能结论。OGG 验证仅在明确列出的运行范围内成立；TH15 没有 MIDI 音乐支持。

根据 `docs/playbooks/adaptation-worktrees.md` 和 `interpolation.md`，canonical 推广还需完整 diff、明确所有 unknown/not-run 门禁，以及 canonical Runtime、音频、直接入口 Replay、Launcher/package 检查；本次先保存可复现的实验提交，未修改 Launcher 指向。

## 最终验收（2026-10-06）

| RAF Hz | 2 秒执行 tick | 2 秒实际呈现 | 开启 60Hz 限制后 1 秒 tick / 呈现 |
| --- | --- | --- | --- |
| 60 | 120 | 120 | 60 / 60 |
| 120 | 120 | 237 | 60 / 60 |
| 144 | 120 | 283 | 60 / 60 |
| 165 | 120 | 326 | 60 / 60 |
| 240 | 120 | 474 | 60 / 60 |

无音乐和 OGG 两轮均通过五档完整测试，无 page error。高刷呈现数含检测预热，故小于 RAF 总数。额外绘制逐次检查音频统计，除实时 SDL 队列消耗外均不变，当前曲目、播放位置和 fade 不被额外绘制推进。OGG 范围为真实标题与 Stage 1；使用原作 PCM 私有转换的 `th15_01`、`th15_02`、`th15_04`。最初 OGG 夹具仅准备前两首，Stage 1 预加载 boss 曲目时失败；补齐 `th15_04` 后全部通过。这不是完整音频/所有换曲生命周期验收，也不把测试转换文件混入正式资源包。

本地复现：先设置 `TH15_EMSDK` 与私有原作 `TH15_ORIGINAL_DAT`，并提供 measured fonts；运行 `node th15_web/scripts/build-sdl-application.mjs --profile`，然后 `node --test th15_web/tests/sdl/high-refresh-browser.test.mjs`。OGG 模式另设置 `TH15_HIGH_REFRESH_MUSIC` 为上述三首 OGG 所在目录。三项场景回归设置 `TH15_PRESENTATION_TEST=1`，运行 `render-loading-lifecycle-browser.test.mjs`、`archive-completion-browser.test.mjs` 和 `checkpoint-persistence-browser.test.mjs`。Release 使用 `--release`。

忽略目录内报告：`artifacts/cpp/verification/high-refresh-cadence.json` 与 `high-refresh-cadence-music.json`；最终日志 `final-high-refresh-fresh-browser.log`、`final-high-refresh-music-complete-input.log`、`final-high-refresh-lifecycle.log`。源码与验收记录提交；Wasm、DAT、fonts、音乐和报告不提交。

## 加载缓冲同步

canonical 加载缓冲提交 `b8bc2cb` 已移植为实验提交 `9ccd891`。启动、游戏入场与换关加载画面至少保留 1000ms；资源准备计入该时间，慢加载不额外再等待整秒。等待分支不采样输入、不执行游戏 step，清除逻辑积压；高刷检测和插值缓存同时重置。恢复后 FPS 采样窗口重新开始，加载时间不混入游戏 FPS。

实际浏览器使用系统时间等待；诊断单步接口继续用于原作事务验证，不将逐 tick 模拟伪装成一秒墙钟。高刷 cadence 测试在开始模拟 RAF 前明确等待诊断准备画面的期限结束；独立 loading-buffer 测试验证真实 RAF 的缓冲。原有 bounded catchup 仍允许正常慢 RAF 中执行最多四次逻辑更新，测试不要求恢复时每次 RAF 必须恰好一次更新。

canonical Development / Release 都重新构建，实际加载缓冲与 2x / 祈祷画面 / Stage Clear 移动回归通过；生产无诊断导出。实验分支重新构建与复验结果见下方。测试第一次因夹具跨 Playwright 调用时期限已过失败，改为在浏览器内立刻启动等待并记录实际时间；另一次误把正常 RAF catchup 判断为必须单 tick，改为核对原有 bounded 行为。失败日志保留。

加载缓冲同步后的实验 Development Wasm：`0d4d89802200b4f2a111673030864604966d82f0ea8cdb09fdede079edb4a4ad`，2243441 bytes；Release：`547b5bc450fd024bbdcd1ea03b6e05b7c38b943e85dd8206e65b752985da7d6b`，1981099 bytes。生产导出检查通过。`loading-buffer-browser.test.mjs` 与 `high-refresh-browser.test.mjs` 组合 2 项通过，五档 RAF 仍各执行 120 tick / 2 秒，帧率限制仍为 60 tick / 60 呈现每秒。日志：`artifacts/loading-buffer-high-verification.log`。同步后这一轮音乐关闭；此前 OGG 证据属于高刷核心原始版本，不升级为新组合的全音频验收。

## 合并记录

用户在本次会话明确要求“推送，然后高刷和普通分支合并，推送”。合并输入：canonical `b8bc2cb` 与实验 `40186c3`；upstream 仍为 `fe113b6`。完整合并范围为 `eaf0fb5..40186c3`，保留原实验审计、代码与加载缓冲历史。BrowserRuntime 调度入口冲突采用实验分支内已验证的同时包含缓冲/高刷版本；文档更新前，canonical 合并工作树与实验 head 的完整 diff 为空。共同子模块固定 `8316c4f861dedb67e1e0e7be75ddcf0b90f63448`。

完整 Lab、实体手机、黄金 Replay、Launcher/package 复验的 unknown/not-run 状态仍保留。按用户明确要求执行合并，不表示这些门禁已通过。canonical 构建与关键回归结果见后续记录。生成产物与私有内容不提交。

canonical 合并后验收：Development `e9b138c155afe0175915f8e2bb197aaf3127774c21aa1f241f3d6a9690c9381e`（2243398 bytes）、Release `4b5711a407cfc259d5576c4a7d0f4e22decf2cdd6bfc1903f0b129c5e1e21348`（1981054 bytes）构建通过，生产无诊断导出且保留帧率限制入口。固定 common controller/release-contract 共 8 项通过；实际高刷/加载组合 2 项、启用额外呈现的加载/移动/整局完成/checkpoint 3 项全部通过，无页面错误。日志 `artifacts/merge-high-refresh-browser.log`、`merge-high-refresh-lifecycle.log`、`merge-high-refresh-development.log`、`merge-high-refresh-release.log`。完整 Lab、实体手机、OGG 新组合、黄金 Replay 与 Launcher/package 仍未升级为 PASS。源码合并提交的双亲记录两个输入 head；产物和私有资源仍在忽略目录。
