# Replay Verifier

Replay Verifier 按每个完整模拟 tick 比较标准化的权威状态。common 负责轨迹校验、顺序对齐、比较和报告；游戏适配层负责 Replay 生命周期、字段选择和运行环境。

## 比较两份轨迹

轨迹为 JSON Lines。运行比较命令：

```sh
node common/testkit/replay-verifier/cli.mjs compare \
  --expected expected.jsonl --actual actual.jsonl --report result.json
```

每份文件含 `run-start`、分段生命周期记录、tick 记录和完整的 `run-end`。序号缺口、提前结束、身份不兼容或缺少必需类别都会使比较失败。

`run-start.coverage` 的必需与可选类别必须在两份轨迹中一致且不重叠；省略 `optionalCategories` 等同于空列表。`--require` 可将已声明的可选类别提升为必需类别，不能移除轨迹头中声明的要求。`run-end.summary` 可省略；提供时使用 `{ "segments": 数量, "ticks": 数量 }`，并须独立符合本轨迹已闭合的实际数量。两份轨迹无需同时提供该摘要。零 tick 轨迹可以通过，但必须至少包含一个完整结束的 segment。

## 工具位置

| 文件 | 用途 |
| --- | --- |
| `adapter.mjs` | 分段生命周期封装及兼容的 JSON 摘要 |
| `capture-file.mjs`、`golden-files.mjs`、`file-suite.mjs` | th11/th15 文件提供器采集和金标套件 |
| `join-traces.mjs` | 合并分段轨迹并保留原始 tick |
| `fetch-fixtures.mjs` | 下载带 SHA-256 校验值的公开样本 |
| `compare.mjs`、`report.mjs`、`trace-jsonl.mjs` | 校验、比较、输出报告和读写 JSONL |

运行本工具测试：

```sh
node --test common/testkit/replay-verifier/replay-verifier.test.mjs common/testkit/replay-verifier/capture-file.test.mjs
```

下一步：[Windows Present 采集器](windows-present-observer/README.md) 或[公共开发入口](../../../docs/README.md)。
