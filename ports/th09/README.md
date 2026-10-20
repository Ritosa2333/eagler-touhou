# 東方花映塚 TH09

本目录包含 TH09 游戏实现和网络 Runtime。普通与多人使用同一个 release WASM 构建；产品包通过独立变体元数据区分。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

在 `web/` 执行 `npm run web:build`；普通和多人 Runtime 分别执行 `npm run web:eagler`、`npm run web:eagler:multiplayer`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

技能停顿的共同调度与浏览器验证见[联机技能停顿](docs/NETPLAY-STOPS.md)。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
