# 東方風神録 TH10 多人版

本目录提供 TH10 多人变体的独立 WASM 构建。它启用多人玩法和网络代码；该变体 Runtime 关闭 thprac，并启用 thcrap。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

`portable/build.mjs --multiplayer`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
