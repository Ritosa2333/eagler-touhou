# 東方風神録 TH10

本目录包含 TH10 的 C++ / SDL3 Web 实现。普通版 Runtime 配方启用 thprac 与 thcrap；多人源码位于 `ports/th10-mp/`。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

`portable/build.mjs --thprac`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 神德练习延迟

第六关最终符卡的延迟遵循紫版 thprac：Easy 为 0–140，Normal 为 0–120，Hard 为 0–100，Lunatic 为 0–50。首次进入或切换难度时使用该难度的上限作为默认值；同难度重新打开练习菜单保留设置。录像使用录制时保存的延迟参数。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
