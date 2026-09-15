# TH10 正式卡片

TH10 WASI Runtime 是正式产品。普通版 TH08（`th08.html`）始终是
独立产品；Launcher 仍只有一个 TH10 卡片。

Host Manifest 的 `shared.testBuild` 只保留开发清单兼容字段，不再控制 TH10
卡片显示。`false`、字段缺失、非法值或 Host Manifest 不可用都不应隐藏
正式 TH10 卡片。

开发服务器仍生成 `testBuild: true`，供旧客户端兼容；打包不应根据它筛除
正式 TH10。Runtime、DATA 和 OGG 仍须通过原有发布输入校验。

TH10 静态卡片带有正常导航地址并默认显示。正式版显示 6 张卡片（TH06、
TH07、TH08、TH10、TH06MP、TH07MP）。

产品目录中 TH06、TH07、TH08、TH10 均声明对应卡图。TH10 使用
`th10-card.webp`，由原始 `th10.dat` 中的标题背景提取、拼接生成。
Host artwork 清单据此生成打包依赖；卡片图不进入源码仓库。

验证命令：

```powershell
node tests/test-product-catalog.mjs
node tests/test-host-manifest.mjs
npm run test:test-build-cards:browser
```

卡片测试覆盖不同 Host Manifest 标志、清单失败、分类与刷新、深链接和无
JavaScript 的静态可见性；它不证明公网部署、真实游戏或手机性能。
