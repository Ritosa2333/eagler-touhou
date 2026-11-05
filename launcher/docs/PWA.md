# 安装与离线使用

支持安装的浏览器可将 Launcher 添加为应用。页眉的 **Install / offline** 入口会打开浏览器安装提示或对应平台说明。用户也可在该入口明确操作后申请持久化存储，是否保留由浏览器决定。

## 离线准备

Service Worker 存储要求 HTTPS，或浏览器信任的 loopback 地址。联网时完成以下步骤：

1. 打开 Launcher，安装 App Shell。
2. 打开要离线使用的游戏，等待其 Runtime 准备完成。
3. 安装或导入该游戏的 Package，以及需要的语言、音乐和字体资源。

Launcher 更新 App Shell 时不会下载所有游戏 Runtime。每个游戏首次联网启动时准备该游戏完整 Runtime 集；后续断网启动可使用最近一次完整的本地 Runtime 集。只更新 Launcher 时，未准备过的游戏仍未就绪。多人模式和新资源下载需要网络。

Service Worker 校验 Runtime 文件摘要；新 Runtime 无法准备时，使用本地完整版本回退。游戏包和存档由独立存储管理，不随 Runtime 更新清理。浏览器可能清除或回收本地存储，重要存档和 Replay 请从 Launcher 导出。

## 更新

App Shell 会等待 Launcher 空闲且满足切换条件后更新；运行中的游戏和操作可能延后切换。Runtime 在启动游戏时单独选择：先尝试当前在线版本，失败后使用完整且已校验的本地版本。

## 浏览器验证

完整 PWA 验证覆盖 Chromium、WebKit、Firefox 的冷启动、更新、离线启动和回滚。从 `launcher/` 安装浏览器依赖并运行：

```sh
npm ci --ignore-scripts
python -m pip install -r tests/requirements-browser.txt
python -m playwright install --with-deps chromium webkit firefox
npm run build:launcher
npm run build:content
node tests/runtime-cache-cases.mjs
node tests/shell-handoff-cases.mjs
python tests/browser/runtime-recovery-cases.py --browser=webkit
python tests/browser/test-pwa-boot.py --browser=webkit
```

将两个 Python 浏览器测试分别以 `--browser=chromium` 和 `--browser=firefox` 再运行一次。以上浏览器测试使用本地测试站点；验证发布站点运行 `npm run verify:deployed -- https://example.org/`，并在实际目标设备上检查安装流程。
