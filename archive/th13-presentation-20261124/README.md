# th13 呈现链加固与打包链路的验证记录（2026-11-24）

| 字段 | 内容 |
| --- | --- |
| 提交 | 普通版 `cfc5c447b`，多人版 `159672dc8`；下表的复测在合并 `main` 之后进行 |
| 源码 | `ports/th13`、`ports/th13-mp` |
| 构建 | `normal` 与 `multiplayer`；多人版网页运行时 wasm `619fc3ae…` |
| 公共库 | `common/`、`launcher/`、`services/` 相对分支起点 `cbf99745e` 的差异只有产品目录里 th13 条目的必需资源一行 |
| 环境 | Linux；浏览器验证在 `th13-shot` 镜像内的无头 Chromium（ANGLE SwiftShader，软件渲染）中运行，原生套件在 `th13-dev` 镜像内 |
| 检查编号 | DISPLAY-01、DISPLAY-02、DISPLAY-03，normal 与 multiplayer |

## 改动后的行为

- 画面只在动画帧回调内提交。`desynchronized` 画布、按键即时微拍与页面看门狗提帧三条路径删除。
- `commit()` 在窗口尺寸为零时保留待提交的帧。帧缓冲首次作为绘制目标绑定时检查完整性，不完整时写入渲染器错误，该帧以失败结束；深度渲染缓冲释放后，引用它的绘制目标重新检查。
- 页面带 `?glDebug=1` 时每次呈现后读取 GL 错误码，变化时输出到控制台。
- 画布上下文丢失时停止循环并提示刷新，恢复时记一条诊断。
- `powerPreference: 'high-performance'` 保留在 `web/sdl-runtime/context-attributes.mjs`。

`th13.html`、`context-attributes.mjs`、`portable/sdl/Renderer.cpp` 在两个端口逐字节相同。

## 打包链路

启动器为 th13 调用 `portable/build-eagler.mjs`（`build.mjs` 加 `package-eagler.mjs`），再用 `launcher/lib/runtime-release.mjs` 的 `runtimeFileNames` 按产品目录的 `runtimeAssets` 校验目录完整性。这条链路不在一键门槛和浏览器验收的路径上，单独验证：

| 环节 | 结果 |
| --- | --- |
| `package-eagler.mjs`，普通版 | 退出码 0，14 个文件 |
| `package-eagler.mjs --multiplayer`，多人版 | 退出码 0，15 个文件；`shell.mjs` 的构建信息为 `"multiplayer":true`，语法检查通过 |
| `runtimeFileNames` 对上面两份产物 | 通过 |

多人版打包脚本原先靠 `shell.mjs` 里的一行注释定位构建信息对象，源码去注释后改按对象字面量定位，并要求它在文件里恰好出现一次；替换后的 `shell.mjs` 与去注释之前打包得到的文本相同。产品目录的 th13 条目把必需资源里的 `low-latency-canvas.mjs` 换成 `context-attributes.mjs`。

## 验证

| 检查 | 结果 |
| --- | --- |
| 静态检查：两个端口的 `web/` 与 `portable/` 源码 | `desynchronized`、`th13_input_kick`、`th13_frame_tick` 均无匹配 |
| 回归摘要（10 份录像） | 状态与事件、绘制清单、逐关绘制三项 `digest-ok` |
| 一键门槛（两侧） | 相对基线无新增失败 |
| 浏览器套件（每侧 22 项，4 并发） | 13 通过、8 跳过、1 项基线内失败（缺 `oracle.replayfmt`），与基线相同 |
| 多人版浏览器验收 M1–M9 | `m1.draw_purity_rng`（额外绘制不改 hash/RNG）与 `m1.side_effect_boundary` 通过 |
| 仓库级检查 | `docs:check`、`audit-publication`、源码契约测试 15 项通过 |

设备上撕裂类花屏的现场确认不在本记录范围内，无头软件渲染复现不了撕裂。本记录覆盖的是回调外的提交路径已全部移除，以及移除后的行为摘要、测试与打包结果。
