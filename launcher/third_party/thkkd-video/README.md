# 海恵堂专用视频转换内核

本组件将原版背景的 ASF／WMV3 转为 VP8 WebM。FFmpeg 5.1.4 负责读取、解码和封装，libvpx 1.13.1 提供 VP8 编码，Emscripten 3.1.40 生成单线程 SIMD WebAssembly。调用继续使用 `@ffmpeg/ffmpeg` 0.12.15 的 Worker 包装层。

`ffmpeg-core.js` 与 `ffmpeg-core.wasm` 随源码提供，普通网站构建直接使用它们。`SOURCE.json` 登记产物身份、实际编译组件及构建配方，`sources.lock.json` 固定上游源码和 Windows 工具链下载的版本与 SHA-256。

## 编译内容

启用 ASF 解封装、WMV3 解码及其 VC-1／H.263 依赖、VP8 编码、WebM 封装和本地文件协议。视频过滤保留格式、空操作、缩放及 FFmpeg 基础缓冲。libvpx 同时关闭 VP8 解码、VP9 和多线程。Worker 保留文件读写、执行、进度、日志及终止接口。

原版四段视频均为 384×448 的无音轨视频。导入器使用 VP8、CRF 10、2 Mbps、`realtime`、`cpu-used=8` 和可变帧率处理，保留原时间戳及 DATA 布局。实时模式的速度选择会读取编码耗时，验证时将固定时钟逐帧对照与真实时钟的画质、性能结果分别记录。

## Windows 重建

在仓库根目录执行以下命令。Python 3.14 用于下载、校验与编排；编译使用锁定的独立 Python、Node 和 Emscripten。工具放在指定目录，临时盘符由脚本分配并释放。

```powershell
python -X utf8 launcher/third_party/thkkd-video/build.py --tools-dir .cache/thkkd-video-tools
python -X utf8 launcher/third_party/thkkd-video/build.py --tools-dir .cache/thkkd-video-tools --verify
```

首次运行准备便携 MSYS2 和编译器，并校验源码归档及构建工具。`build.sh` 从关闭可选组件的配置开始，按白名单编译并链接；`--verify` 重新构建并比较既有产物的哈希和组件清单。WASM 的体积预算为原始 8 MiB、gzip 3 MiB。

## 许可与验证

FFmpeg 使用 [LGPL-2.1-or-later](FFmpeg-LGPLv2.1.txt)，libvpx 使用其 [BSD 许可](libvpx-LICENSE.txt)。FFmpeg.wasm 的绑定与 Worker 包装层遵循项目原有 MIT 署名；源码版本与补丁步骤保存在锁文件和构建脚本中。

运行 `bun run --cwd launcher test:repository tests/thkkd-video-core.test.mjs` 检查交付体积、组件身份及按需加载登记；`ports/thkkd/tests/video-transcode-browser.mjs` 对四段实际视频比较转换输出、进度、内存、失败清理和取消重试。完整资源导入与六份长录像按本作开发指南验证。
