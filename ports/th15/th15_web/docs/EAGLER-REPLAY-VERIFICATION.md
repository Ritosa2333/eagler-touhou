# common Replay verifier 接入与范围

`scripts/diagnostics/verify-replay-integrity.mjs` 使用固定 common 子模块的
`adapter.mjs`、`trace-jsonl.mjs`、`compare.mjs`，没有另写比较器。
这是候选版本内部的呈现完整性 profile，不能代替与原作的 golden 对照。

运行前提供私有 `TH15_ORIGINAL_DAT`、measured fonts 与 `TH15_VERIFY_REPLAY`
（待测 `.rpy` 的绝对路径），构建 development application：

```powershell
$env:TH15_EMSDK='C:\path\to\emsdk'
$env:TH15_ORIGINAL_DAT='C:\path\to\th15.dat'
$env:TH15_VERIFY_REPLAY='C:\path\to\th15_01.rpy'
node th15_web/scripts/build-sdl-application.mjs --profile
node th15_web/scripts/diagnostics/verify-replay-integrity.mjs
```

实际 BrowserRuntime 停止 RAF 后逐 tick 执行原有输入、update 与 Draw。
60 档不启用额外呈现，144/240 档按比例插入多个插值 Draw transaction。
每次额外呈现前后核对观察状态；每个 tick 都写入 common trace，包含过场。
窗口交换按 120 tick 批次提交，所有游戏 Draw 和额外 GPU 指令仍然执行。
测试使用 Windows D3D11 ANGLE；没有无敌、强制结束关卡或替换游戏工厂。
实际 RAF 墙钟调度、加载缓冲和音乐播放继续由独立测试覆盖。

必需类别：world probe（玩家位置/生命、得分、库存、威力、游戏 RNG、
敌人与可见弹数）、Replay 游标及输入边沿、符卡时钟、暂停菜单生命周期。
这些是有限字段，不覆盖全部实体载荷、视觉 RNG 和所有回滚可见状态。
`th15_probe_replay_state` 是只读 development 导出；生产构建不包含它。

按实际游戏完成事件结束采集，再验证结束菜单及返回 Replay 选择。
Replay 可以经终止输入标记完成，也可以经最终关完成服务打开结束菜单。
报告分别记录 `terminalMarker`、最终游标与各关声明输入长度。
后者不能被表述为“所有文件输入均已消费”；其与原作的等价性需要原作轨迹。

忽略目录 `artifacts/replay-verifier` 保存三档 JSONL、`captures.json` 与
`result.json`。报告中的 PASS 仅指候选内部逐 tick 一致；
`originalComparison` 在没有原作 golden 时明确保持 INCOMPLETE。
输入突变负控须得到 DIVERGED，截断轨迹负控须得到 INCOMPLETE。
原始 Replay、DAT、Wasm 和生成的轨迹不提交。

## 2026-10-06 实测

在合并后的 `eagler` 上测试本机自然游玩的 `th15_01.rpy`，SHA-256
`680404336344b65a7163afed7c193d35ed554fe1e54422f8e32dd51dca662559`。
common 固定 `8316c4f861dedb67e1e0e7be75ddcf0b90f63448`；诊断 Wasm
`6a8db692b4c737ebca1b68e7ef94b2bbddd12fad8d7ebd902f2ad4fb42b11170`。

| 候选呈现档 | 六关逻辑帧 | 与 60 档逐帧比较 |
| --- | ---: | --- |
| 60 | 106565 | 基准，common trace schema/lifecycle 通过 |
| 144 | 106565 | PASS，所有必需类别一致 |
| 240 | 106565 | PASS，所有必需类别一致 |

三档均正常进入结束菜单、返回 Replay 选择，没有页面错误。
输入突变负控在第一个 tick 报 DIVERGED，截断负控报 INCOMPLETE。
每次额外绘制前后观察到的状态一致。最终游标均为 23642，六面声明输入
24022，剩余 380 帧未消费，`terminalMarker=false`。该次运行经六面通关
服务打开菜单，不把它升级为所有文件输入 EOF 已验证；与原作是否相同未测。

原作逐帧轨迹以及原先测试的 `ud0186`、Sanae Extra、Reisen Easy 样本
均不在本机工作区。当前总认证状态是 INCOMPLETE，只有候选高刷一致性
通过；其他 Replay、全部实体字段、原作等价性仍未认证。
Release 重建后 SHA-256 仍为
`4b5711a407cfc259d5576c4a7d0f4e22decf2cdd6bfc1903f0b129c5e1e21348`，
无 development 诊断导出。以上测试没有修改游戏完成或 Replay 播放逻辑。
