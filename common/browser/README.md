# 浏览器共享模块

`browser/` 保存游戏 Runtime、Launcher 可复用的浏览器代码。模块源文件在这里维护；需要嵌入旧版独立 Shell 或目录 Runtime 时，用 `tools/` 中的同步脚本生成副本。

## 开始

从仓库根目录进入 `common/`。模块使用原生 ECMAScript，无需安装包：

```sh
cd common
node --test browser/spectator-timeline-view.test.mjs
node --test tests/browser-keyboard-owners.test.mjs tests/keyboard-input-bridge.test.mjs tests/direct-input-bridge-test.mjs
```

## 组件与同步

| 组件 | 源码 | 生成到消费者 | 检查 |
| --- | --- | --- | --- |
| 键盘物理按键归属 | `browser/keyboard-owners.mjs`、`browser/keyboard-input-bridge.mjs` | `node tools/sync-browser-keyboard.mjs TARGET.html ...` | 同命令加 `--check`；运行 `node --test tests/browser-keyboard-owners.test.mjs tests/keyboard-input-bridge.test.mjs` |
| 目录 Runtime 键盘 | `browser/directory-keyboard.mjs` | `node tools/sync-directory-keyboard.mjs TARGET.mjs ...` | 同命令加 `--check` |
| 旁观时间轴 | `browser/spectator-timeline-view.mjs` | `node tools/sync-spectator-timeline-view.mjs --target TARGET.mjs` | 同命令加 `--check`；运行 `node --test browser/spectator-timeline-view.test.mjs` |
| TH06–TH09 启动画面 | `browser/startup-branding.mjs`、`browser/startup-wordmark.svg` | `node tools/sync-startup-branding.mjs TARGET ...` | 同命令加 `--check` |
| TH10、TH11、TH15、TH20 开发者署名 | `browser/startup-developers.mjs`、`browser/startup-developers.svg`、`browser/StartupBranding.hpp` | `node tools/sync-startup-developers.mjs TITLE_REPOSITORY ...` | 同命令加 `--check` |

将 `TARGET` 替换为相应 Runtime 文件或 Shell 的路径。同步后把消费者改动与 common 源码一起检查；`--check` 用于确认生成副本没有漂移。

## 其他模块

- `playback-clock.mjs`、`spectator-playback.mjs`：回放时钟与旁观播放状态。
- `local-replay.mjs`、`local-replay-native.mjs`：浏览器与原生端的本地 Replay 桥接。
- `save-sync.mjs`：浏览器存档同步。
- `direct-input-bridge.mjs`、`browser-gamepad-input.hpp`：触摸与手柄输入桥接。

对应行为测试在 [`tests/`](../tests/)；需要确认所有接入副本时，运行相关同步脚本的 `--check`。

下一步：[公共库开发入口](../../docs/README.md)。
