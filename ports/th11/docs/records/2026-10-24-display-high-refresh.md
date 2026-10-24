# TH11 高刷新率呈现验证记录（2026-10-24）

| 字段 | 填写内容 |
| --- | --- |
| 提交 | `e683d5e0c`（基于 `upstream/main` `445eb6ce2`；移植自 `th11@eagler` d24e240） |
| 源码 | `ports/th11` |
| 构建 | `normal`；源码契约与 CPU 契约通过；完整 launcher Runtime 构建未在本机执行（缺少本地原版资源/字体/内容准备） |
| 公共库 | 仓库 `common`（`upstream/main` tree `c45af77`） |
| 环境 | Windows 11；Node 24；Edge + SwiftShader（源码/CPU 契约）；原分支运行证据来自 120Hz 合成 rAF 与 60Hz 真实数据 smoke |
| 网络 | 不适用 |
| 检查编号 | DISPLAY-04、DISPLAY-05、DISPLAY-06、DISPLAY-08 |
| 操作 | 在 `ports/th11` 运行：`node portable/check-presentation-lab.mjs`、`node portable/check-high-refresh-contract.mjs`、`node portable/check-presentation-purity.mjs`、`node portable/check-presentation.mjs`（需 EMSDK）。原分支另跑：60Hz smoke（真实 th11.dat）、120Hz 合成 rAF、`th11_12.rpy` 巡检。 |
| 结果 | 四条源码/CPU 契约全部 PASS。原分支：60Hz logic 60.0–60.33 fps、0 错误；120Hz 合成 `presentationFps≈111.6`、72 呈现/36 逻辑、60Hz 限制开关生效；`th11_12.rpy`（12,000 tick / 3,950 对象）`no-interpolation` 2099 → 14，`authored-rebase` 1370，`blended-discontinuity` 0。 |
| 附件 | 原分支 `portable/presentation-lab/CAMPAIGN.md`；巡检 JSON `artifacts/presentation-lab/campaign/replay.json`（本地，不提交） |

## 普通版 / MP 与读数

- 本改动属于 **normal**（`ports/th11`）。`ports/th11-mp` 未修改；普通版到 MP 的历史同步按 CONTRIBUTING 由同步维护者处理。
- 右下角 `fps` 在高刷关卡中显示**实际呈现帧率**（按 0.5s 内 present 次数计算）；标题、对话、暂停、结局显示逻辑帧率（约 60）。`limitPresentationTo60` 强制逻辑读数。

## 边界

- 标题/对话/暂停/结局不插值（冻结门控）。
- 3D 场景 512/1024 重基按 snap 处理，不跨边界插值。
- 剩余 14 条自机弹路径（`script 20/23 + sprite 35`，约 10px/tick）待继续收。
- Presentation Lab 覆盖缺口报告为 `unknown`，不宣称准入通过。
- THPRAC 变速构建未验证。
