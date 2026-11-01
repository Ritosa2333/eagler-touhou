# th13 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md)（对齐 th15 体例）· [全部作品](../README.md)

源码目录：`ports/th13`。构建模式：`sdl3`（网页运行时）+ `wasi-harness`（oracle 对照专用，非可玩目标）。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为"已实现"；完成对应操作并附执行记录后记为"已验证"。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

本轮（2026-10-23 整改）依据：行为/渲染一致性以 oracle 裁决（外层仓库 `oracle/`，AGENTS.md 方法论）；场景覆盖以 [coverage-matrix.md](docs/plans/coverage-matrix.md) 为 home。

## 源码与构建

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | 源码目录与作品归属 | 已验证 | `ports/th13`（分支 `port/th13`）；sparse 检出 31% |
| BUILD-02 | 工具链与外部依赖（容器化） | 已验证 | `container/Dockerfile` + 外层 Makefile（podman/CI 共用）；th13-dev 镜像（node 22 / emcc 6.0.9 / wasi-sdk / ghidra 12.1.3） |
| BUILD-03 | 网页运行时构建 | 已验证 | `node portable/build.mjs` → `web/artifacts/sdl3/th13-sdl.mjs`（本轮实测 sha256 58234abb…，62 例单测后复跑通过） |
| BUILD-04 | WASI 对照 harness 构建 | 已验证 | `node portable/build-harness.mjs` → `th13-harness.wasm`（全 game 层 wasm32-wasip1 同源；本轮实测通过） |
| BUILD-05 | 前端搭配单个游戏运行 | 已验证 | `web/sdl-runtime/th13.html`（18 资产 MEMFS）；`th13-debug.html` 回放取帧宿主（确定性帧泵） |
| BUILD-06 | 源码与素材来源 | 已验证 | 原版资产永不入库（外层 `games/`+`artifacts/` gitignored；红线 1） |

## 启动与资源

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | destination 状态机与装载挡板 | 已验证 | `Application.cpp`（begin_title/begin_run/begin_replay_run/apply_destination + 180 帧 boot 幕） |
| RUNTIME-02 | 键盘输入 | 已实现 | `portable/input/KeyboardMap.inc`（原版 32 位掩码；Z/X/Shift/方向 + 双 Shift 归一）；手机触控 = 用户令「按用户要求不做」，`不适用` |
| RUNTIME-03 | 音频 | 进行中 | `AudioDevice` 桩（接口先立；音频接线分期） |
| RUNTIME-04 | 存档/音乐室/练习 | 不适用（分期） | 菜单项已登记禁用（coverage-matrix ⛔ 有意偏差；功能补齐后销账） |
| PACKAGE-01 | 资产清单完整性 | 已验证 | 24→41 项与 `prepare-assets.mjs` 对齐（st01.std/st01wl.anm/sht 四机体等曾漏已补，提交 833118aa） |

## 行为一致性（oracle 裁决）

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| VERIFY-01 | 事件流·无输入窗 | 已验证 | st01 3600 帧 / 10187 事件 `RESULT first_divergence_frame=none`（本轮整改后复跑） |
| VERIFY-02 | 事件流·回放窗 | 进行中 | udSr01 st01 段当前 f122（基线勘误：旧登记 2652 已漂移，oracle 侧基线需重锚——见 HANDOFF）；th13_01 2625（引用前须复跑） |
| VERIFY-03 | RNG 语义 | 已验证 | `0x47BBA2` MSVC CRT 序列 = C++ 镜像 = srand(1) 标准；ECL/ANM 双独立种子 0 流实证 |
| VERIFY-04 | replay 格式 | 已验证 | t13r 双趟 XOR + LZ77 全解；udSr01 6 关 105476 帧差分零错（`docs/plans/replay.md`） |
| VERIFY-05 | Boss 时序 | 已验证 | udSr01 Boss 1非帧两侧同 2380/5583（harness 实测登记） |

## 渲染一致性（draws oracle 裁决）

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| RENDER-01 | 语义绘制清单导出（两侧同构） | 已验证 | oracle `render_oracle.py`/`title_oracle.py` ↔ port `DrawCapture`（契约 draw-contract.md v1） |
| RENDER-02 | 标题窗 A | 进行中 | f487/600（81%）；f487 起 ins_408 升级机制层工作面（微调冻结，AGENTS.md 机制层优先） |
| RENDER-03 | 对局窗（无输入） | 进行中 | golden 锚 f0（tfactor）/f61（敌机行走位差）；replay-verifier BASELINE-HELD |
| RENDER-04 | 对局窗（回放） | 进行中 | 同上两工作面（对局主线战役）；`--ignore tfactor` 唯一登记项（销账=装载趟残留态建模，draw-contract §3.5） |
| RENDER-05 | 共享渲染基座零漂移 | 已验证 | Renderer/Shaders/RenderCommands/LegacyGraphics 与 th15 逐字节一致；**GraphicsState.hpp 已归一**（2026-10-23，语义下沉 GraphicsDevice/DrawCapture 配置点）；敌机句柄化默认路径（`enemies_from_ecl` + `retire_sink` 回收，0x41fe80/case3 同构） |
| RENDER-06 | 渲染统计契约（浏览器实测） | 已验证 | `web/tests/sdl/graphics-stats.test.mjs`：无 glBufferSubData 流式 / glBufferData 整批替换 / quad 直通占比主导 / 布局缓存有界（th13-shot 真实 chromium 实测通过） |
| RENDER-07 | draws golden 对跑 | 已验证 | `tools/replay-verifier/`（corpus + run-oracle + run-gate）：两案例 BASELINE-HELD，退化即 FAIL（golden 只升不降） |
| RENDER-08 | HUD/暂停/结算/Boss/难度/角色窗 | 待验证 | 未建（coverage-matrix 📦——oracle 捕获面扩展前置；下一阶段消费） |

## 画面与声音

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | 固定逻辑步进 | 已验证 | 60fps 逻辑帧；FrameCadence + `th13_debug_frame` 确定性帧泵（取帧自检两跑一致） |
| DISPLAY-02 | 高刷插值 | 不适用（分期） | PresentationFrame stub；补件登记（ui-base.md「不做（后续分期）」→ S3 入场条件） |
| DISPLAY-03 | 重复绘制无逻辑副作用 | 已实现 | 高刷插值未接入，当前单绘制帧天然成立；接入时按 AGENTS.md 性能检查单复验 |

## 验证与交付

| 编号 | 检查项 | 状态 | 记录 |
| --- | --- | --- | --- |
| VERIFY-10 | 单测门禁集 | 已验证 | 容器四套 62 例全绿（anm/ecl/ui/replay——ui 套件本轮修复既有编译破损）+ `node --test tests/*.test.mjs` |
| VERIFY-11 | 全链路验收口径 | 进行中 | 菜单→S1 过关→结算→S2 双一致（用户令）；当前 S2/S3 阶段推进中 |
| VERIFY-12 | 视觉终审 | 待验证 | MCP 图像并列对比（v1 交付协议，HANDOFF）——逻辑+渲染语义全绿后执行 |

## 运行验收记录

历史逐轮战报（ECL 虚拟机 / UI 基座 / stage1 对拍 / 录像注入 / 弹型域 / STD 背景 / draws 导出器等 9 轮）已随本轮整改归档至外层仓库 `HANDOFF.md`「战役进展」与 `docs/rectification-2026-10-23.md` §2；本表只保留可复跑的编号验收项，战报叙事不再入本文件（AGENTS.md 汇报纪律：数字必须实测可复现，历史叙述去 HANDOFF）。

- 2026-10-23 · 整改轮 · 方法论治理 + 基座归一 + 测试契约（`docs/rectification-2026-10-23.md`）：BUILD-03/04 复跑通过；VERIFY-01 复跑 none；RENDER-05/06/07 新落地并实测通过；VERIFY-10 62 例全绿（ui 既有破损顺手修复）。
