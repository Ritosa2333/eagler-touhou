# 東方妖々夢 TH07

本目录包含 TH07 的 SDL3 / Web 源码，以及普通版与回滚多人版实现。普通、多人是独立构建变体；运行时配方中的 thprac 可配置，默认关闭。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

CMake Web CI 预设：`web-ci-normal` 或 `web-ci-netplay`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
