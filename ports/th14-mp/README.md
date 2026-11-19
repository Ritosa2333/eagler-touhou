# TH14 多人版

[构建与适配](../../docs/adaptation/th14.md#多人模式) · [延迟输入验收记录](../../archive/th14-rollback-20261114/README.md)

本目录提供东方辉针城的双人和三人合作 Wasm 运行时。启动器以 `th14mp` 识别多人版。两种模式使用同一套原版游戏数据，各自保存录像和配置。

从仓库根目录构建：

```sh
python ports/th14-mp/tools/th14-build-gles.py --common common --out /path/to/th14-mp-core
node launcher/scripts/assemble-th14-runtime.mjs --template=/path/to/th14-template --core-build=/path/to/th14-mp-core --output=/path/to/th14-mp-runtime
```

多人核心需要已激活的 Emscripten SDK。运行包模板、原版数据格式及浏览器验证命令见[适配指南](../../docs/adaptation/th14.md)。
