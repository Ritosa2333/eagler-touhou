# Adapter 行为不变量

本页记录可从玩家操作或游戏状态直接检查的行为。产品能力分类见 [`ADAPTER_CAPABILITIES.md`](ADAPTER_CAPABILITIES.md)；详细接入步骤见[输入与触控](../../docs/adaptation/input.md)、[呈现与音频](../../docs/adaptation/presentation.md)和[多人模式](../../docs/adaptation/multiplayer.md)。实现可以不同，观察结果必须一致。

## 输入与菜单

| 行为 | 可观察结果 | Owner |
| --- | --- | --- |
| 菜单滑动 | 一次触摸从按下到抬起最多产生一个方向步进；短 tap 确认，双指短 tap 返回 | Launcher 手势 → Runtime 原生菜单 |
| 多指操作 | 新增、抬起或取消一个手指只结束该指拥有的操作；移动、焦点、炸弹等其他输入继续 | Launcher 触点归属、Runtime 输入合并 |
| 菜单与对话 | 方向、确认、取消、Skip 进入原作输入状态机；等 Confirm 的流程可触控推进 | Runtime |
| Bomb 与移动 | 触控 Bomb 使用原作 Bomb/Deathbomb 窗口；Bomb 已消耗时，移动手势仍可继续 | Runtime |
| 抬手与失焦 | 抬手、pointer cancel、blur、隐藏页面或 pagehide 清除对应临时输入及尚未消费的拖动量 | Launcher 与 Runtime |

## 触控与判定

- `touchSensitivity` 的有效范围为 `100`–`300`。灵敏度缩放“手指位移到目标位移”的关系；角色仍按原作每个逻辑步允许的速度移动。
- `touchMovementMode: "touch-unlimited"` 只有在实际产生非零不限速移动后才标记本局 cheat movement。标记后 Result / 高分处理落率和已保存 Replay 的处理落率均为 `100%`；仅切换模式不影响本局。
- `alwaysHitbox` 显示作品原生低速/Focus 判定点，或视觉语义一致的判定点。它只改变可见条件，不改变焦点、速度、碰撞、RNG 或 Bomb 判定；绘制碰撞框不满足此行为。
- Runtime 根据设置执行 Unlimited Touch 移动与 Always Hitbox 判定点显示；Launcher 提供相应设置和输入。

主要 owner：[`runtime-protocol.mts`](../src/contracts/runtime-protocol.mts) 定义 `touchSensitivity`、`touchMovementMode` 和 `alwaysHitbox`；Launcher 的触点布局在 [`touch-layout-model.mts`](../src/launcher/touch-layout-model.mts)，输入校验在 [`adapter-capabilities.mts`](../src/contracts/adapter-capabilities.mts)。

## 暂停、音频与呈现

- 暂停期间游戏世界不因额外显示帧推进；暂停菜单可以继续动画。
- 页面从后台恢复时恢复音频输出通道，不重新开始 BGM，也不重置原作的暂停、淡入淡出或当前曲目状态。
- 切曲覆盖完整 source 生命周期：停止旧 source、打开/读取/解码新资源、启动新 source 后恢复输出，不能重复旧曲或新旧叠播。
- 高频显示的额外绘制不运行玩法、音频事件、RNG、Replay、存档或原作定时器。

音频与呈现 owner 在 Runtime；Launcher 仅负责选择模式和页面生命周期。统一行为定义见 [`adapter-capabilities.mts`](../src/contracts/adapter-capabilities.mts) 中的 `REQUIRED_MUSIC_BEHAVIORS` 与 `REQUIRED_PRESENTATION_BEHAVIORS`。

## 验证

```sh
cd launcher
npm run test:touch
npm run test:runtime-protocol
npm run test:always-hitbox:browser
npm run test:music-selection:browser
npm run check:workspace
```

触控源契约见 [`test-portable-touch-controller-contract.mjs`](../tests/test-portable-touch-controller-contract.mjs)，Always Hitbox 浏览器门禁见 [`test-always-hitbox-browser.mjs`](../tests/test-always-hitbox-browser.mjs)。这些共享测试之外，每个 Runtime 还要验证自己的原生菜单、Replay、暂停、绘制与实际音频行为。
