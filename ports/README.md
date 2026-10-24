# 选择游戏

每个源码目录都有开发指南和适配表。先按模式选择目录，再执行本作命令。

修改普通版时，适用改动进入 MP 同步范围。日常代码在开发分支正常提交；同步维护者从对应 MP 目录历史合并普通版，再保留祖先写回总仓库。包含历史同步的 PR 使用合并提交。提交方式见[贡献指南](../CONTRIBUTING.md#普通版与-mp-怎么提交)，同步步骤见[下一次提交与同步](../docs/adaptation/port-sync.md#下一次提交与同步)。

| 游戏 | 普通版 | MP | 适配检查 |
| --- | --- | --- | --- |
| th06 | [源码与开发](th06/README.md) | 同一源码目录，单独构建 | [普通版／MP](th06/ADAPTATION.md) |
| th07 | [源码与开发](th07/README.md) | 同一源码目录，单独构建 | [普通版／MP](th07/ADAPTATION.md) |
| th075 | [开发](th075/README.md) | [开发](th075-mp/README.md) | [普通版](th075/ADAPTATION.md) · [MP](th075-mp/ADAPTATION.md) |
| th08 | [开发](th08/README.md) | [开发](th08-mp/README.md) | [普通版](th08/ADAPTATION.md) · [MP](th08-mp/ADAPTATION.md) |
| th09 | [开发](th09/README.md) | 共用生产程序，分产品身份 | [普通版／MP](th09/ADAPTATION.md) |
| th10 | [开发](th10/README.md) | [开发](th10-mp/README.md) | [普通版](th10/ADAPTATION.md) · [MP](th10-mp/ADAPTATION.md) |
| th11 | [开发](th11/README.md) | [开发](th11-mp/README.md) | [普通版](th11/ADAPTATION.md) · [MP](th11-mp/ADAPTATION.md) |
| th12 | [开发](th12/README.md) | — | [普通版](th12/ADAPTATION.md) |
| th15 | [开发](th15/README.md) | [开发](th15-mp/README.md) | [普通版](th15/ADAPTATION.md) · [MP](th15-mp/ADAPTATION.md) |
| th20 | [开发](th20/README.md) | — | [普通版实验适配](th20/ADAPTATION.md) |

从仓库根目录增加需要的源码：

```sh
node tools/workspace.mjs select th10 --variant=both
```

[贡献指南](../CONTRIBUTING.md) · [适配流程](../docs/adaptation/README.md) · [全系列作品进度](../docs/TOUHOU_PROGRESS.md)
