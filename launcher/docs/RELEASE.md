# 发布候选

在 `launcher/` 运行 `npm run release`，使用当前 Runtime Release 和本地原作资源组装可验证的正式候选。已有站点目录或自托管包时，直接从[部署指南](SELF_HOSTING.md)开始。

## 准备输入

复制 [`tools/maintainer/release-input.example.json`](../tools/maintainer/release-input.example.json)，填写 Runtime Release、游戏目录映射、功能配置、音乐模式，以及需要的语言、字体和封面输入。输入 schema 为 `eagler-touhou/release-input/1`；相对路径以输入文件所在目录为基准。正式候选包含所有已登记产品。

构建器通过 PowerShell (`pwsh`) 执行 Host 组装，因此需要 Node.js 22.18+、PowerShell 7、Python 3 和 Host 准备工具。原作 `.dat`、`.data`、音乐和封面由 Host 从维护者本地资源目录读取。

## 构建与验证

选择一个尚不存在的输出目录，然后运行：

```sh
npm run release -- --input=release-input.json --output=releases/candidate
npm run verify:release -- releases/candidate
```

输出包含 `hosted-site/`、配套的 `external-site/`、`import-site/`、Runtime Release、每作 Package 目录和离线 ZIP、输入与 Release 清单、校验和及完成报告。目标目录必须不存在。构建成功后，报告将实现、构建和结构检查标为完成；Runtime 验收、语义审查和人工验收在报告中单独记录。

## 发布

从候选中取本次要发布的目录。Hosted 用 `hosted-site/`，Import 用 `import-site/`。External 需要同一候选的 `hosted-site/` 和 `external-site/`，并分别验证两个地址。见[静态站点部署](SELF_HOSTING.md#发布静态站点)和[外置资源模式](EXTERNAL_RESOURCE_MODE.md)。

公开附件按[自托管与 Runtime 下载包](../tools/maintainer/PUBLIC_RELEASE_ASSETS.md)准备。向指定运营者交付已构建站点与服务时，可生成[部署交接包](../tools/maintainer/operator-delivery/README.md)。
