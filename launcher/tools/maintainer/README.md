# 维护工具

这里是发布产物工具和实际站点探测器。以下命令从 `launcher/` 运行，输入地址与输出目录由维护者指定。

| 工具 | 用途与说明 |
| --- | --- |
| `build-github-release-assets.ps1` | 从同一正式 Release 生成 Self-host、Runtime 下载包；见[公开附件](PUBLIC_RELEASE_ASSETS.md) |
| `build-operator-delivery.mjs` | 生成部署交接包；见[交接模板](operator-delivery/README.md) |
| `recover-external-site.mjs` | 从既有 Hosted generation 恢复 External 站点；见[恢复流程](EXTERNAL_RECOVERY_LANE.md) |
| `verify-public-first-install.mjs` | 用新浏览器环境检查安装提交、Runtime 就绪和首帧 |
| `verify-public-relay-fallback.mjs` | 检查指定 Relay 的回退连接 |
| `verify-public-targeted-relay.mjs` | 检查指定 Relay 的定向转发 |
| `verify-public-turn-quality.py` | 测量指定 TURN 的连接质量 |
| `verify-thcrap-real.mjs` | 检查本地 thcrap 工具链 |

先准备[正式 Release](../../docs/RELEASE.md)，再使用对应工具。生成目录、测试记录、私有资源与服务配置存放在源码目录之外。每次探测记录实际地址、时间、构建身份和操作结果。
