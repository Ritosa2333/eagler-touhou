# TH08 Presentation Lab 审计记录

这是隔离测试树对四份真实 Replay 的机器审计结果，不是正式 Runtime 的发布声明，也不等价于实际 120/165 Hz 显示器的主观流畅度验收。

## 自动 Replay 审计构建

- 分支：`test/th08-presentation-lab`
- 基线：`8effb75534323c1a17d7493932dfa9d69ac32f00`
- 仪器化 WASM：`f101c72f7501a1ac4f12d4ed0e5ace04d9ba70cb057a52ffeac3f8122e21d402`
- 本地实验服：`http://127.0.0.1:8245/`
- 构建清单记录 407 个源文件；最终验证时当前磁盘哈希无过期项。

这里的「最终」只表示该轮四份 Replay 自动巡检结束，不表示后续人工游玩不会发现自动覆盖之外的显示路径。

## 完整 Replay 结果

最终证据位于 `artifacts/presentation-lab/campaign/final-current5-f101/`。

| Replay | 逻辑步 | 审计窗口 | 覆盖关卡 | 最终分数 | 结果 |
| --- | ---: | ---: | --- | ---: | --- |
| lunatic-magic | 146,950 | 2,442 | 0/1/2/3/5/7 | 587069133 | 完整、分数一致、无错误 |
| lunatic-ghost | 121,221 | 2,013 | 0/1/2/4/5/7 | 262393114 | 完整、分数一致、无错误 |
| lunatic-border | 99,212 | 1,648 | 0/1/2/4/5/6 | 140328188 | 完整、分数一致、无错误 |
| extra-border | 43,816 | 730 | 8 | 158690137 | 完整、分数一致、无错误 |

四份共 411,199 个逻辑步、7,833 个多 alpha 审计窗口。每份汇总的最高严重度均为 3；severity 4–6 问题组为 0。所有窗口保持 presentation purity，完整运行没有浏览器错误或回放分数漂移。

## 本轮确认并修复的缺口

- Stage 6A 第 8074 帧：同一敌人/使魔的 ANM VM 在 ECL phase 切换时仍有连续 offset 动画。视觉连续性改由 VM 身份和敌人生命周期判定，不再被 ECL 子程序号误切断。
- Stage 5 第 1815 帧：敌人命中/妖怪 tint 会切换实际提交颜色通道。现在保持 RGB tint 离散，但插值真正被提交的 active alpha。
- Extra 第 13589 帧：一次 authored draw 内可能包含三个嵌套 Replay 更新。分数弹字以明确 generation/lifecycle 判定复用，不再用任意的 timer 差上限拒绝有效区间。
- Stage 4B 第 7965 帧：deathbomb tint 从无到有时，提交 alpha 会乘 `255/128` 并饱和。Bullet owner 保存前后提交变换，对最终可见 alpha 插值，再反解当前源通道。
- Stage 4B 第 17412 帧：Boss marker 的距离淡出没有视觉 sidecar。现在保存上一逻辑帧的 marker 视觉端点，并只对 owner 派生 opacity 做 presentation-only 插值。

## 后续人工验收补充

用户在实际高刷新显示器逐帧暂停、拖动 presentation alpha，并用 F8 导出相邻端点后，又确认了自动 Replay 覆盖之外的三类根因：

- 符卡大环在一个逻辑帧内跨越几何分支时，旧代码会把前后两种拓扑的参数混合成不存在的椭圆。现在拓扑选择保持离散，只在同一拓扑内插值连续几何参数。
- 3D 背景脚本会把相机坐标按 512 单位回卷以复用场景。旧距离阈值把 `511.5` 的回卷误认成真实运动，产生一帧反向视角；现在脚本显式标记 authored rebase，presentation 不跨该边界插值。
- Boss 符卡背景的平移由 `AddU/AddV` 直接累计循环 UV，`uvScrollVel` 为零，且展开遮罩由 callback 直接读取固定 Effect 槽位。现在同一 ANM／脚本／精灵生命周期内的小幅循环 UV 使用统一 owner continuity，callback-owned Effect 也复用生命周期门控的 presentation copy；换图、大幅跳变和新生命周期仍保持离散。

上述人工验收后的仪器化构建为 `f10b61221d280c292363b01c9f8add7b28d97afb6e44e3c7e6ff408b76cba49e`。它已通过编译和装配，实际视觉结论以用户验收为准；不能拿它冒充前一节四份 Replay 已重新全量巡检。

## 明确保留的离散边界与审计边界

- Stage 4B 第 28032 帧的敌人位置变化发生在 ECL 子程序 `46 → 53`。位置、方向和拖尾原本就以此作为生命周期边界；审计记录 owner 的明确离散 motion 标记，不跨 phase 强行平滑。ANM VM 自身的连续属性仍可独立插值。
- 幽冥组 Stage 5 第 1281/1701 帧的 3D 背景输入有限，但 CPU 诊断投影在齐次 `w≈0` 时穿过相机裁剪平面。审计现在记录 `projection-clip`，不把诊断侧透视除法的 NaN 当作 GPU 提交 NaN，也不 clamp 或改变真实相机路径。
- `held-review` 仍表示「保持当前值，需要核对离散语义」，不是自动失败。屏幕外、缺失样本、换贴图、生命周期、裁剪平面和 CPU 投影限制都继续显式保留。

## 检测器自证

- `node --test portable/presentation-lab/test-analyzer.mjs`：46/46 通过。
- `node portable/presentation-lab/test-visual.mjs`：29 个真实 WASM 断言通过。
- `node portable/check-high-refresh-contract.mjs`：通过。
- `node portable/check-presentation-purity.mjs`：通过。
- 定点负控会重新报告被故意保持的 offset、position、scale、opacity 等问题，证明正常结果不是通过关闭检测器得到。

诊断工具不打包或分发游戏 DATA、字体和原作 Replay。忽略目录中的报告和截图只作为用户本地证据。
