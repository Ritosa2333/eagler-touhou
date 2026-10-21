# 東方花映塚 TH09

本目录包含 TH09 游戏实现和网络 Runtime。普通与多人使用同一个 release WASM 构建；产品包通过独立变体元数据区分。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

在 `web/` 执行 `npm run web:build`；普通和多人 Runtime 分别执行 `npm run web:eagler`、`npm run web:eagler:multiplayer`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

技能停顿的共同调度与浏览器验证见[联机技能停顿](docs/NETPLAY-STOPS.md)。

## 联机蓄力

回滚对局以玩家本机接受的蓄力进度为准。双方同步同一份逐帧蓄力结果，回滚重算和录像播放复用这些结果；技能在可执行的游戏帧释放，停顿期间保留待释放的蓄力攻击。

房间规则提供「关闭本地优先的蓄力条进度」，默认关闭。需要原版兼容 Replay 时启用此选项，双方恢复原有蓄力逻辑、实时蓄力显示和对应的 Replay 记录方式。技能停顿边界确认独立生效。选项由 P1 在大厅设置，修改后双方重新准备，开局后固定。

前端通过 `configure.options.netplayDisableLocalChargePriority` 传入布尔值。`true` 走原有蓄力路径，`false` 启用回滚对局的本地蓄力进度；运行时在 HELLO 前核对双方的规则身份。本地优先录像携带独立蓄力轨道，以 `.rpyx` 导出。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
