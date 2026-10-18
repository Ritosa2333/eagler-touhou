# 永夜抄 3.4.1：触屏决死修复

手机触屏 B 按钮与双击符卡手势曾被 `TouchState.ready` 限制。
该标志只表示自机能否移动；中弹后的决死等待期间它为 false，
导致触屏 X 输入被挡住，且三帧符卡脉冲不消耗，可能在复活后才发动普通符卡。

`portable/input/TouchController.hpp` 现在分别处理动作与移动：

- 游戏期间允许提交射击、低速和符卡，原版游戏逻辑决定是否接受。
- 自机死亡与复活期间仍停止拖动和摇杆移动；不会改变原版速度、碰撞或决死窗口。
- 符卡脉冲每帧消耗，过期输入不会等待复活。菜单、对话、录像不发送符卡。
- 双击符卡手势在中弹前后及决死窗口内均能识别。

验证命令（工作区根目录，Node.js 24）：

```
th10_web/tools/node.exe portable/input/touch-bomb-check.mjs
th10_web/tools/node.exe portable/check-th08-deathbomb.mjs --base http://127.0.0.1:8095
th10_web/tools/node.exe portable/check-th08-deathbomb.mjs --input keyboard
th10_web/tools/node.exe portable/check-th08-deathbomb.mjs --input double-tap
th10_web/tools/node.exe portable/check-th08-deathbomb.mjs --input late
```

浏览器回归使用实际 SDL/Wasm，原版一面弹幕自然中弹后输入，不修改玩家状态。
及时符卡应消耗两枚库存、进入无敌状态且不掉残机；晚按不能在复活后再放符卡。
`artifacts/touch-bomb` 保存前后状态与截图。原版对照测试为
`tests/cpp/player-bomb.test.mjs` 和 `tests/cpp/player-life.test.mjs`。
这些对照测试使用已有 WASI fixture；本次修复没有更改游戏层源文件。

电脑键盘沿用原版 **X** 符卡键；手机显示 **B** 的按钮代表 Bomb。
验证在桌面 Chromium 手机视口中进行，不代表已经完成实体手机测试。
