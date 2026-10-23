# 東方紺珠伝 TH15 多人版

本目录包含 TH15 多人编译变体与配套 Runtime。多人发布配方开启 thcrap，关闭 thprac；普通版源码在 `ports/th15/`。

双人、三人对局使用独立模拟堆与 32 字节首次写入日志，提供固定 12 帧的回滚窗口。迟到输入修正角色、弹幕与脚本状态，音效、旁观和多人 Replay 在连续已确认的模拟水位上提交。实现说明与真实浏览器验收入口见[回滚开发说明](docs/DEVELOPMENT.md#回滚状态与输出)。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

`web/scripts/build-sdl-application.mjs --release --multiplayer`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
