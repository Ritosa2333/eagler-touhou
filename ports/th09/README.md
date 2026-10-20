# 東方花映塚 TH09

本目录包含 TH09 游戏实现和网络 Runtime。普通与多人使用同一个 release WASM 构建；产品包通过独立变体元数据区分。

**适配明细：** [本游戏适配表](./ADAPTATION.md) · [全项目适配总览与分类](../../docs/adaptation/README.md)

## 构建

在 `web/` 执行 `npm run web:build`；普通和多人 Runtime 分别执行 `npm run web:eagler`、`npm run web:eagler:multiplayer`。

快速步骤和源码布局见[开发指南](docs/DEVELOPMENT.md)。源码编译后，由仓库根目录发布流程组装启动器包。

## 联机蓄力显示

回滚对局的蓄力条、可用槽和蓄力等级按已确认帧显示。画面使用确认后的蓄力外观，并跟随当前机体位置；移动、蓄力积累、技能释放和录像继续使用原有游戏逻辑。显示延迟取决于对方输入到达的时间。

蓄力显示与游戏快照分别保存，确认后的进度按帧顺序提交。单机、录像和旁观使用各自正在播放的游戏状态绘制。

显示策略有独立运行期开关：在游戏 iframe 内执行 `window.Module._th09_confirmed_gauge_mode(0)`，下一次绘制立即恢复原有显示；传入 `1` 重新启用。Runtime 地址参数 `confirmedGauge=0` 可在启动时关闭。开关保留当前对局、输入和录像，发布时保留上一代 Runtime，支持整代切回。

## 资源

仓库不附带原版可执行文件、游戏数据、音乐、存档或 Replay。运行包需要本地合法取得的原版资源；按开发指南准备对应文件。
