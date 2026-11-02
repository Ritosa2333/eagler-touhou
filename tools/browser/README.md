# 浏览器测试工具

`launch.mjs` 为各作浏览器检查提供统一 Chromium 启动入口。`playwright.mjs` 使用已安装的 JavaScript Playwright 或 Python Playwright 驱动；Python 依赖按 `launcher/tests/requirements-browser.txt` 安装。

`PLAYWRIGHT_CHROMIUM_EXECUTABLE` 可指定浏览器。默认使用软件渲染；`NATIVE_GPU=1` 或 `PIXEL_GPU=1` 使用本机 GPU，Windows 采用 D3D11，其余平台沿用 Chromium 的平台后端。

`verified-runtime.mjs` 为各作验收记录浏览器实际收到的脚本和 WASM 响应，沿重定向核对候选摘要。验收结束调用 `await delivery.assertComplete()`，同时检查两类响应均已收到且字节匹配。`node --test tools/browser/verified-runtime.test.mjs` 使用真实浏览器和独立 HTTP 入口，覆盖匹配构建、脚本变化、WASM 变化与缺少 WASM 响应。
