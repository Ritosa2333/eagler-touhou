# th11 高刷新率呈现验证记录（2026-10-24）

固定 60 Hz 逻辑与显示呈现分别验证。Presentation Lab 直接运行真实关卡，提供冻结、逐帧、帧间采样与对象字段证据。

| 字段 | 填写内容 |
| --- | --- |
| 提交 | PR #133 的 `775adbb7` 加本记录随附修复 |
| 源码 | `ports/th11` |
| 构建 | 完整 `presentation-lab` 构建；普通、THPRAC、Lab、Lab + THPRAC 四种配置通过 `Application.cpp` 语法编译 |
| WASM | SHA256 `f648fedf6130e2de57f4d587c12885198fba73c7475aded54c8b57592fde53f5` |
| 公共库 | 同一 monorepo 快照的 `common` |
| 环境 | Windows；本机 Emscripten；Node；Codex 内置浏览器；本地真实 th11 DATA 与 MS Gothic |
| 网络 | 不适用 |
| 检查编号 | DISPLAY-03、DISPLAY-04、DISPLAY-05、DISPLAY-06、DISPLAY-08 |
| 源码契约 | Presentation Lab、高刷门控、绘制纯度三项检查通过 |
| CPU 契约 | ANM 呈现、固定步进、相机插值、原生 authored-rebase 标记四项通过 |
| JS 契约 | flicker 11 项、公共 Lab 控制器与发布契约 8 项通过 |
| 浏览器操作 | `?stage=1` 启动真实一面；逐帧与 180 帧扫描；启动等待 Runtime 应答，关卡种子在冻结后写入 |
| 浏览器结果 | 扫描推进至逻辑帧 180 → 181，末窗口 183 个对象、7 个端点问题组；画面正常显示一面场景与自机 |
| 显示器验收 | 用户在本机高刷屏继续游玩，检查帧率、暂停及 60 Hz 限制切换 |

## 大位移与显式重基

原生舞台绘制将 STD 相机设置事件写入观察记录的 bit 8，保持 456 字节 ABI。检测器同时要求显式事件、全部采样存在、每个采样贴合当前端点，才记录为 `authored-rebase`。256／512／1024 单位的旧端点与混合过渡均进入问题列表。

## 普通版 / MP 与读数

- 本改动属于 **normal**（`ports/th11`）。`ports/th11-mp` 未修改；普通版到 MP 的历史同步按 CONTRIBUTING 由同步维护者处理。
- 右下角 `fps` 在高刷关卡中显示**实际呈现帧率**（按 0.5s 内 present 次数计算）；标题、对话、暂停、结局显示逻辑帧率（约 60）。`limitPresentationTo60` 强制逻辑读数。

## 对象证据

- 180 帧扫描中的 7 组问题来自动画循环端点，保留逐字段证据供复核。
- 对象插值结果按实际记录展示。状态摘要覆盖未接齐的组使用 `unknown`。
- Replay 选择器通过诊断驱动载入本地文件；本次浏览器操作使用固定一面种子。

## 原分支证据索引

原分支曾记录 60 Hz smoke、120 Hz 合成 rAF 与 `th11_12.rpy` 巡检，入口见 [CAMPAIGN.md](../../portable/presentation-lab/CAMPAIGN.md)。其中按位移阈值统计的 1370 个重基候选，须按本记录的显式事件与全采样端点条件重新统计。
