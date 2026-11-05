# 移动端 C 功能键

Launcher 为移动端保留普通 `KeyC` 输入通道。Runtime 决定游戏含义；Launcher 只发送按键状态。

## 归属与启用

| 内容 | Owner |
| --- | --- |
| 按键标识与支持游戏 | [`src/launcher/touch-function-key.mts`](../src/launcher/touch-function-key.mts)：`KeyC`、`c`、`67`；当前仅 `th11` |
| 移动按键 UI 和宿主键盘发送 | [`app/services/player-touch.ts`](../app/services/player-touch.ts) |
| Runtime 键位映射 | [`src/launcher/hosted-keyboard.mts`](../src/launcher/hosted-keyboard.mts) |
| 游戏手柄/键位配置中的 C 动作 | [`src/launcher/keyboard-preferences.mts`](../src/launcher/keyboard-preferences.mts) |
| 游戏效果与 Replay 输入 | TH11 Runtime；C 使用普通逻辑输入位 `1 << 4`（`0x10`），Replay 使用原有按键记录 |

新增支持游戏时，把游戏 ID 加入 `functionKeyGames`，并在该游戏 Runtime 中处理原生 C 输入。不要在 Launcher 实现游戏机制。

## 输入行为

- 一次触摸发出一个短按；长按不重复触发。
- 按下持续 `50 ms`，之后保留 `50 ms` 的释放间隔。快速连点会排队，每次都产生独立按下边沿，让 60 Hz 逻辑采样读到每一下。
- 只有取得该控件所有权的指针能释放它；其他手指、失焦、控件取消或页面生命周期结束不会留下按住状态。
- 按键通过现有宿主 `keyboard` 输入通道发送，不增加 Runtime 命令或网络消息。
- 控件的位置、尺寸、优先级沿用触控布局设置；旧版布局缺少此控件时使用默认布局。
- 不支持该按键的游戏和观战模式不显示控件。iOS 使用 Touch Events。

## TH11 游戏语义

| 游戏状态 | Runtime 处理 |
| --- | --- |
| 灵梦 A | 在原生水平边界触发既有二次按键 Warp 状态机；原有双击行为仍有效 |
| 魔理沙 B | 进入既有五种阵型切换分支，可与原有射击/低速组合使用 |
| 其他机体或状态 | 保留原生行为，不增加 C 技能 |

Warp 的敌机检查、Bomb 限制、音效、选项重建、持续帧数和越界位置仍由 Runtime 原状态机决定。Replay 按原有逻辑输入记录和重放 C；更新 Launcher 时需同时使用含 C 行为的 Runtime。

## 验证

```sh
cd launcher
npm run build:launcher
npm run test:touch
```

重点用例：[`tests/test-touch-function-key.mjs`](../tests/test-touch-function-key.mjs) 验证 50 ms 脉冲、快速连点、60 Hz 边沿、指针归属和取消；[`tests/react-main/player-touch.test.mjs`](../tests/react-main/player-touch.test.mjs) 验证控件可见性和触控面板消息。TH11 游戏语义由 Runtime/Replay 用例验证。

触控输入整体约定见 [`docs/adaptation/input.md`](../../docs/adaptation/input.md)；协议和代码 Owner 索引见 [`GAME_ADAPTER_CONTRACT.md`](GAME_ADAPTER_CONTRACT.md)。
