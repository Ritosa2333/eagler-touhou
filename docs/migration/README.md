# Monorepo 迁移记录

| 文件 | 内容 |
| --- | --- |
| [import-sources.json](import-sources.json) | 原仓库、分支与导入提交 |
| [move-sources-20261018.json](move-sources-20261018.json) | 搬迁时采用的来源 |
| [refresh-20261018.json](refresh-20261018.json) | 搬迁前刷新记录 |
| [th08-mp-refresh-20261018.json](th08-mp-refresh-20261018.json) | th08 MP 刷新来源 |
| [publication-review-20261018.json](publication-review-20261018.json) | 发布审阅记录 |
| [reviewed-source-assets.json](reviewed-source-assets.json) | 已记录的源码素材身份 |
| [stb-vorbis-provenance.json](stb-vorbis-provenance.json) | stb_vorbis 来源 |
| [th12-eagler-20261020.json](th12-eagler-20261020.json) | th12 导入身份与子树基点 |
| [th12-subtree-20261024.md](th12-subtree-20261024.md) | th12 子树继承与独立导出 |

搬迁当时的验收原文见[旧文档去向表](../history/INDEX.md)。继续开发使用[工作区说明](../../WORKSPACE.md)与各组件开发指南。

`python tools/ports/check-history.py --base <目标分支>` 读取来源记录，验证原仓库提交、子树连接点与目录导出内容。主分支的 `Source history` 检查执行同一命令，新增作品目录随来源登记一同进入验收。
