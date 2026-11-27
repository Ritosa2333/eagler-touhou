# 第三方依赖与资源

网页字体使用 GNU Unifont 15.1.05 的完整字形集，原文件为 `unifont-15.1.05.otf`，来源为 `https://unifoundry.com/pub/unifont/unifont-15.1.05/font-builds/unifont-15.1.05.otf`，SHA-256 为 `7b62b50acbb186689dc30c446ce4367b87d79489e9907b83255f9fbe0dcfb9e1`。fontTools 4.68.0 与 Brotli 1.2.1 转为随源码携带的 `third_party/unifont.woff2`，保留完整命名和字形；许可证见同目录 `OFL-Unifont.txt`。网页程序直接使用随源码携带的 WOFF2，构建时复制到 Runtime 的 `fonts/`。

浏览器使用 UPNG.js 2.1.0（MIT，Photopea）读取和生成 PNG；BMP 按原位深解码，JPEG 交给浏览器。原生 alpha 与独立 `_a` 遮罩的红色通道相乘。MP3 与 WAV 使用原字节，四段 WMV 通过 FFmpeg WebAssembly 在本机转为 VP8 WebM（CRF 10、2 Mbps，保留原时间戳）。UPNG 的许可证见 `third_party/UPNG-LICENSE.txt`。

媒体转换使用仓库维护的专用 FFmpeg WebAssembly 内核，编译 FFmpeg 5.1.4、libvpx 1.13.1 及 ASF／WMV3→VP8 WebM 必需组件；Worker 包装层保持 `@ffmpeg/ffmpeg` 0.12.15（MIT，Jerome Wu）。源码、固定工具链、校验值与重建入口见[专用内核说明](../../launcher/third_party/thkkd-video/README.md)。FFmpeg 的 LGPL-2.1-or-later 与 libvpx 的 BSD 许可全文随组件及 Runtime 提供，FFmpeg.wasm 的 MIT 署名继续保留在 `third_party/FFmpeg-WASM-LICENSE.txt`。JS、Worker 和 WASM 随网站交付，首次转换视频时按需获取。

DXA v3/v4 的默认密钥、索引与 LZ 压缩布局参考 [GARbro 的 DxLib 实现](https://github.com/morkt/GARbro/tree/master/ArcFormats/DxLib)。morkt 的 MIT 署名与许可全文保存在 `third_party/GARbro-LICENSE.txt`。

本地历史资源对照脚本 `scripts/prepare-assets.py` 使用 Pillow 13.0.0 与命令行 FFmpeg，供已有测试数据复用。网页导入使用上述浏览器转换器；原版档案和本机原始素材保持原字节与修改时间。
