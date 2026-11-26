# 第三方依赖与资源

桌面后台使用 DxLib 3.25a 的 Windows Visual Studio x64 静态库。发行包来源为 https://dxlib.xsrv.jp/dxdload.html，固定版本与下载包校验值保存在 third_party/dxlib/VERSION.json，原项目说明保存在 third_party/dxlib/DxLib.txt。

随库提供的图像、声音及其他依赖库保留原文件名和声明。SDK 头文件从 CP932 转换为 UTF-8，方便与本工程统一编译。游戏图片、声音和视频沿用原游戏资源及其原有权属。

网页字体使用 GNU Unifont 15.1.05 的完整字形集，原文件为 `unifont-15.1.05.otf`，来源为 `https://unifoundry.com/pub/unifont/unifont-15.1.05/font-builds/unifont-15.1.05.otf`，SHA-256 为 `7b62b50acbb186689dc30c446ce4367b87d79489e9907b83255f9fbe0dcfb9e1`。fontTools 4.68.0 与 Brotli 1.2.1 转为随源码携带的 `third_party/unifont.woff2`，保留完整命名和字形；许可证见同目录 `OFL-Unifont.txt`。站点共用字体由这个 WOFF2 还原为 OTF，并生成同字体的 TTC，兼容路径 `shared/msgothic.ttc` 的内容也是 GNU Unifont。

浏览器使用 UPNG.js 2.1.0（MIT，Photopea）读取和生成 PNG；BMP 按原位深解码，JPEG 交给浏览器。原生 alpha 与独立 `_a` 遮罩的红色通道相乘。MP3 与 WAV 使用原字节，四段 WMV 通过 FFmpeg WebAssembly 在本机转为 VP8 WebM（CRF 10、2 Mbps，保留原时间戳）。UPNG 的许可证见 `third_party/UPNG-LICENSE.txt`。

媒体转换固定使用 `@ffmpeg/ffmpeg` 0.12.15（MIT，Jerome Wu）及 `@ffmpeg/core` 0.12.10（GPL-2.0-or-later）。依赖由启动器的包管理器安装，Worker、JS 和 WASM 随网站构建，不从 CDN 加载。包装层源码与构建配方见 [ffmpeg.wasm](https://github.com/ffmpegwasm/ffmpeg.wasm)，FFmpeg 源码见 [FFmpeg](https://github.com/FFmpeg/FFmpeg)。许可证分别保存在 `third_party/FFmpeg-WASM-LICENSE.txt` 与 `third_party/FFmpeg-GPLv2.txt`。

DXA v3/v4 的默认密钥、索引与 LZ 压缩布局参考 [GARbro 的 DxLib 实现](https://github.com/morkt/GARbro/tree/master/ArcFormats/DxLib)。morkt 的 MIT 署名与许可全文保存在 `third_party/GARbro-LICENSE.txt`。

本地历史资源对照脚本 `scripts/prepare-assets.py` 使用 Pillow 13.0.0 与命令行 FFmpeg，供已有测试数据复用。网页导入使用上述浏览器转换器；原版档案和本机原始素材保持原字节与修改时间。
