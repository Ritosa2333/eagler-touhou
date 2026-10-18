# 风神录 3.0：eagler Launcher 与原生 SDL 平台

这次迁移复用了 eagler-touhou 的启动器实现，并将风神录的浏览器平台职责迁入 C++。它不是红魔乡或妖妖梦的游戏逻辑复用；风神录的规则、ANM/ECL、原始数值精度和录像逻辑仍由独立的 TH10 C++ 实现负责。

## 参考与来源

- Launcher: `YomotsuHisami/eagler-touhou`, commit `5a73fd3323589b1ba9d08d74c9827191905577f2`。本地源代码位于 `th10_web/launcher`，保留 GPL-3.0-or-later 许可。
- TH07 native platform: `th10_web/reference/eagler-th07`, commit `5f80a0df8a8434afd2544d4aaa95691ea4d65f89`，CC0。参考了 SDL/miniaudio 音频、文件系统和 `Touch.cpp` 的手势规则。
- 没有使用参考仓库中的 TH10 源码。

## 平台职责

| 层 | 3.0 实现 |
| --- | --- |
| 网页入口 | 上游 TypeScript Launcher，复用资源安装、触控布局、导入导出、移动端方向与缩放界面 |
| 资源管理 | 上游 Package Store / IndexedDB，不可变资源版本、SHA-256 校验、安装完成后切换 generation |
| 离线 | 上游 App Shell Service Worker，缓存启动器和 Runtime；游戏内容由 Package Store 持久化 |
| 运行载体 | 一个同源 iframe，`eagler-touhou/1` configure / launch / input / storage / health 协议 |
| 游戏生命周期 | C++ `GameHost.cpp` 创建和释放 Application、状态、资源、音频、字体和图形对象 |
| 输入 | C++ SDL 键盘、手柄、触摸；Launcher 通过协议补充跨 iframe 的触控按钮 |
| 主循环 | C++ 60 Hz 调度和原版 Application 更新；网页只提供浏览器显示/可见性接口 |
| 图形 | C++ 渲染批处理 → SDL3 / GLES3 / WebGL2 |
| 音效 | 原始 PCM → C++ miniaudio 混音 → SDL_AudioStream → 浏览器设备 |
| 音乐 | C++ miniaudio 按原始 thbgm.dat 偏移和循环位置解码无损 FLAC |
| 文件 | C++ SDL_IOStream；浏览器只安装文件和持久化 IDBFS |
| 文字 | C++ SDL3_ttf / FreeType；保留原字体、原版阴影、颜色混合和贴图缩放流程 |
| 存档 | Runtime 内的 IDBFS `/savesth10/jp`、`/savesth10/chs`，协议只允许读写合法存档与录像路径 |

发布目录不包含旧 `native-worker.mjs`、`NativeFileStore`、`NativeFonts`、JavaScript 游戏对象组装器或 CPU 模拟器。旧实现保留在开发树内以便回归对照。

## 明确的差异

“架构一致”指上面的职责划分、实际复用的 Launcher 和运行时协议，不表示不同作品的源码、数据结构或游戏系统相同。TH10 的 C++ 图形适配仍保留其原版图形状态语义；上游 TH06/TH07 的游戏专用类不能直接替换它。

音乐使用 FLAC，未采用有损 OGG。18 首均做完整解码后 PCM 字节相等校验，原有循环偏移保持。资源总量约 388.6 MB（370.6 MiB），首次安装需要下载；此后可离线重开。资源驻留、浏览器缓存、GPU 与 Wasm 内存是不同的统计项，64 MiB 的 Wasm 堆不代表整个网页只占 64 MiB。

SDL_ttf 与 Windows GDI 的字体光栅化算法不同，因此文字边缘不是逐像素相同。使用同一原字体、编码和原始分辨率，测试覆盖日文、汉化对话与 Music Room，不再读取约 190 MiB 的汉化预烘焙字形表。

未提供 TH06/TH07 专用的 thprac、联机、高刷新率插值与判定点扩展。界面隐藏没有实现的功能；原版游戏按 60 Hz 更新。

## 存档兼容

同一网址首次打开 3.0 时，读取旧的 `th10-1.00a-jp` / `th10-1.00a-chs` 数据库，将不存在于 IDBFS 的合法文件迁入对应语言目录。旧数据库不删除，已有新存档不覆盖。迁移标记在成功 sync 后持久化。

触控录像内部保留原始 `.rpy` 及 THMOTION 扩展，导出文件名仍为 `.rpyx`。测试验证迁移、导入、导出、离线重开后字节一致。不同语言独立保存。

## 构建与验证

从工作区根目录执行：

```powershell
.\th10_web\tools\node.exe portable/build.mjs --th10
.\th10_web\tools\node.exe portable/package-architecture.mjs
```

固定 Emscripten 6.0.9、SDL3 3.4.2、SDL3_ttf 3.2.2。Launcher 以 TypeScript 5.9.3 严格编译。依赖见 `tools/architecture` 与 `tools/emsdk`。

发布包内的 `source` 目录包含对应 C++、启动器、构建脚本和运行时源文件。从发布包恢复开发环境时，在 `source` 内用发布包的 Node 执行 `portable/restore-release-assets.mjs`，从旁边经过哈希校验的 `site` 恢复资源；用 `python tools/download-emscripten.py` 安装固定 SDK。前端编译器可用 `npm install --prefix tools/architecture/typescript typescript@5.9.3` 安装。

输出 `th10_web/artifacts/architecture-candidate`。部署根目录仅为它的 `site` 子目录，源码与构建脚本位于网站根目录之外。`files.json` 是静态服务器公开文件白名单。

验证脚本位于 `portable/th10-architecture-*.mjs`，结果写入发布目录 `validation`。包括真实浏览器启动、实际音频输出、14,000 tick 日文/汉化回归、Music Room、触控、IDBFS 迁移、离线重开、PCM 随机读取，以及手机尺寸 + 4 倍 CPU 降速的一面完整流程。

手机尺寸和 CPU 降速是可重复的桌面诊断，不等于物理手机测试。场景资源的同步创建仍可能产生短帧尖峰，不能据平均 60 FPS 声称所有手机完全不掉帧。
