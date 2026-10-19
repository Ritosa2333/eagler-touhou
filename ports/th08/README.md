# 東方永夜抄 TH08

本目录包含 TH08 的 C++ / SDL3 Web 实现。普通版构建同时启用 thprac 与 thcrap 语言支持；多人版位于 `ports/th08-mp/`。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

`portable/build.mjs --th08`，输出普通 SDL3 WebAssembly 构建。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
