# 开发工具

所有命令在仓库根目录运行。

| 命令 | 用途 |
| --- | --- |
| `node tools/workspace.mjs list` | 查看普通版与 MP 的实际源码目录 |
| `node tools/workspace.mjs paths th10 --variant=both` | 输出所需目录 |
| `node tools/workspace.mjs select th10 --variant=both` | 为稀疏检出增加所需目录 |
| `npm run docs:check` | 检查文档链接、适配表与能力定义覆盖 |
| `npm run docs:index` | 更新全部文档表 |
| `node tools/docs/new-adaptation.mjs --game=th10` | 从配方创建缺少的游戏适配表 |
| `npm run ports:check -- --game=th10 --variant=both` | 检查本作各模式的构建输入、接口和共用模块 |
| `npm run ports:status -- --all` | 执行全作检查并更新适配表 |

`build/` 放共用编译参数和源文件依赖工具，`browser/` 管浏览器辅助器，`ports/` 管各作适配检查，`ci/` 放受影响组件选择和源码入口检查，`docs/` 放文档与表格工具。CI 使用根 `.github/workflows/`。

两条文档命令均可在稀疏检出中运行；未展开的文档从 Git 索引读取。

[工作区](../WORKSPACE.md) · [参与开发](../CONTRIBUTING.md)
