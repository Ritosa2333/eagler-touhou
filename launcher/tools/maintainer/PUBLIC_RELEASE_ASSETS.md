# 公开下载附件

从同一个已验证的[正式 Release](../../docs/RELEASE.md)生成两份不含原版游戏资源的下载包：

| 文件 | 读者与内容 |
| --- | --- |
| `EaglerTouhou-SelfHost.zip` | 自托管用户；Host 工具、前端、可再分发共用资源、Runtime Release 与游戏资源目录 |
| `EaglerTouhou-Runtime.zip` | 集成与升级；同一套 `runtime-release/` |
| `SHA256SUMS.txt` | 下载校验 |
| `release-assets.json` | 产物与源 Release 的身份关系 |

## 1. 生成

在 `launcher/` 中运行，替换输入和新的输出目录：

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File tools/maintainer/build-github-release-assets.ps1 -Candidate D:/Releases/candidate -OutputDirectory D:/ReleaseAssets/version
```

工具先验证正式候选，再派生两个附件。游戏原始文件、含 DATA 的 Hosted 站点、离线游戏包和用户数据属于独立的私有交付内容。

## 2. 检查首次安装

将待验证站点部署到已确认的测试地址。在 `launcher/` 中用 `node scripts/list-product-games.mjs` 查看作品列表，然后对每个本次发布作品运行：

```sh
node tools/maintainer/verify-public-first-install.mjs https://staging.example.com/ --game=th06
```

检查结果应包含 IndexedDB 安装 generation 提交、Runtime 就绪和实际首帧。把测试环境与结果放进发布记录，审阅附件和公开说明后再发布。
