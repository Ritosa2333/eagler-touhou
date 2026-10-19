# 发布候选

在 `launcher/` 运行 `npm run release`，使用当前 Runtime Release 和本地原作资源组装可验证的正式候选。输出目录供后续检查和发布。

## 准备输入

复制 [`tools/maintainer/release-input.example.json`](../tools/maintainer/release-input.example.json)，填写 Runtime Release、游戏目录映射、功能配置、音乐模式，以及需要的语言、字体和封面输入。输入 schema 为 `eagler-touhou/release-input/1`；相对路径以输入文件所在目录为基准。正式候选包含所有已登记产品。

构建器通过 PowerShell (`pwsh`) 执行 Host 组装，因此需要 Node.js 22+、PowerShell 7、Python 3 和 Host 准备工具。原作 `.dat`、`.data`、音乐和封面由 Host 从维护者本地资源目录读取；仓库和 Runtime Release 均不提供这些游戏资源。

## 构建与验证

选择一个尚不存在的输出目录，然后运行：

```sh
npm run release -- --input=release-input.json --output=releases/candidate
npm run verify:release -- releases/candidate
```

输出包含 `hosted-site/`、配套的 `external-site/`、`import-site/`、Runtime Release、每作 Package 目录和离线 ZIP、输入与 Release 清单、校验和及完成报告。目标目录必须不存在。构建成功后，报告将实现、构建和结构检查标为完成；Runtime 验收、语义审查和人工验收在报告中单独记录。

## 发布

从已验证候选发布。Hosted 或 Import 模式发布后运行 `npm run verify:deployed -- https://example.org/`。External 模式需发布配对的 Hosted 与 External 站点，并验证两个公开地址；步骤见[外置资源模式](EXTERNAL_RESOURCE_MODE.md)。

上述命令创建并验证 Release 文件；站点由维护者使用常规部署流程发布。准备 GitHub Release 附件见 [`tools/maintainer/PUBLIC_RELEASE_ASSETS.md`](../tools/maintainer/PUBLIC_RELEASE_ASSETS.md)。
