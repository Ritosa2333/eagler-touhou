# Replay Verifier

Replay Verifier 按每个完整模拟 tick 比较标准化的权威状态。common 负责轨迹校验、顺序对齐、比较和报告；游戏适配层负责 Replay 生命周期、字段选择和运行环境。

## 比较两份轨迹

轨迹为 JSON Lines。运行比较命令：

```sh
node common/testkit/replay-verifier/cli.mjs compare \
  --expected expected.jsonl --actual actual.jsonl --report result.json
```

每份文件含 `run-start`、分段生命周期记录、tick 记录和完整的 `run-end`。序号缺口、提前结束、身份不兼容或缺少必需类别都会使比较失败。

## 工具位置

| 文件 | 用途 |
| --- | --- |
| `adapter.mjs` | 分段生命周期封装及兼容的 JSON 摘要 |
| `capture-file.mjs`、`golden-files.mjs`、`file-suite.mjs` | TH11/TH15 文件提供器采集和金标套件 |
| `join-traces.mjs` | 合并分段轨迹并保留原始 tick |
| `fetch-fixtures.mjs` | 下载带 SHA-256 校验值的公开样本 |
| `compare.mjs`、`report.mjs`、`trace-jsonl.mjs` | 校验、比较、输出报告和读写 JSONL |

运行本工具测试：

```sh
node --test common/testkit/replay-verifier/replay-verifier.test.mjs common/testkit/replay-verifier/capture-file.test.mjs
```

下一步：[Windows Present 采集器](windows-present-observer/README.md) 或[公共开发入口](../../../docs/README.md)。
