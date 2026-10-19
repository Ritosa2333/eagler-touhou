# 部署交接模板

本目录保存交接包的静态说明和能力配置。构建器从同一个正式 Release 收集站点、服务与验证工具。

从 `launcher/` 运行：

```sh
node tools/maintainer/build-operator-delivery.mjs   --release=/releases/candidate   --output=/deliveries/eagler-touhou   --archive=/deliveries/eagler-touhou.zip
```

输出目录与 ZIP 使用新路径；`--archive` 可省略。工具验证能力配置与 Hosted 站点，ZIP 关闭后写入 SHA-256 文件。

将生成包交给指定部署者，按包内 README 操作。交接包可能包含原版游戏资源，应存放在源码目录之外，并通过约定的私有渠道交付。
