# Presentation Lab

Presentation Lab 是供开发和验收使用的固定步模拟、绘制检查工具。common 提供控制器、浏览器工作台、报告和通用分类；每个游戏适配层负责调用实际 Runtime，并解释自己的状态与原生记录。

## 运行契约测试

从仓库根目录执行：

```sh
node --test common/testkit/presentation-lab/controller-core.test.mjs common/testkit/presentation-lab/release-contract.test.mjs
```

## 接入游戏

实现 [`CONTRACT.md`](CONTRACT.md) 规定的 `RuntimeDriverV1` 与 `ObservationAdapterV1`，然后从标题专属审计配置启动工作台。适配层提供 `/lab-config.mjs`，说明固定步更新、绘制、输入、状态覆盖和 Replay 导航。

工作台支持原始当前帧、回顾插值和外推三种对照。外推使用当前帧到未来一帧的权重 `1..2`，由支持该能力的作品适配器显式开放。

现有原生观察适配器包括 TH08、TH10、TH11 与 TH15。是否支持由该游戏自己的绘制纯度证据决定；common 测试通过不代替游戏适配检查。

## 目录

- `controller-core.mjs`：冻结、固定步推进和绘制样本调度。
- `analyzer.mjs`、`report-core.mjs`：标准化观察数据分类与报告。
- `workbench.html`、`workbench.mjs`、`workbench.css`：浏览器工作台。
- `contracts.mjs`、`contracts.d.ts`：运行时与 TypeScript 契约。

下一步：[公共开发入口](../../../docs/README.md)。
