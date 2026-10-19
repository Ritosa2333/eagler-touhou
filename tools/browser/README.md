# 浏览器测试工具

`launch.mjs` 为各作浏览器检查提供统一 Chromium 启动入口。`playwright.mjs` 使用已安装的 JavaScript Playwright 或 Python Playwright 驱动；Python 依赖按 `launcher/tests/requirements-browser.txt` 安装。

`PLAYWRIGHT_CHROMIUM_EXECUTABLE` 可指定浏览器。默认使用软件渲染；`NATIVE_GPU=1` 或 `PIXEL_GPU=1` 使用本机 GPU，Windows 采用 D3D11，其余平台沿用 Chromium 的平台后端。
