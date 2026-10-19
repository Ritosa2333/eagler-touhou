# Presentation Lab 接入契约

游戏适配层提供两个边界：

- `RuntimeDriverV1`：控制真实 Runtime 的冻结、固定步更新、绘制和输入。
- `ObservationAdapterV1`：解码原生记录，并给出状态拥有者、生命周期和连续性信息。

TypeScript 接口见 [`contracts.d.ts`](contracts.d.ts)，运行时校验见 [`contracts.mjs`](contracts.mjs)。报告记录 common 源码修订、适配层版本和原生 ABI 标识；API major 用于标记兼容性。

## Runtime 要求

- `freeze()` 暂停 Lab 驱动并返回当前会话令牌；`resume(token)` 只接受当前令牌，恢复逻辑时间并丢弃冻结期间累积的墙钟时间。
- `advanceOneTick()` 在完整游戏固定步事务结束后返回成功回执；加载中或场景结束时返回对应状态。
- `drawOnly()` 走实际绘制路径，不读取输入或推进 Replay、随机数、音频队列及模拟计数器。

## 观察要求

- 基准状态来自原始权威绘制，并记录实际模拟 tick 和 draw serial。
- 每个样本声明状态覆盖范围。未覆盖、缺失或格式错误的数据分类为 `unknown`。
- 对象身份由内存地址与生命周期或 generation 信息共同确定。
- 适配层说明状态写入者、发布点、绘制消费者、最终提交点、连续性和恢复规则。

## 接入验收

在该游戏的真实绘制路径上审计所有状态拥有者，采集重复绘制证据，并覆盖可能被路径修改的权威状态与回滚状态。诊断导出只进入该游戏的审计构建配置；生产包不包含诊断导出或 Lab。

修改必需方法、回执、标准化记录或纯度规则时提升 API major；新增报告字段可留在当前 major。
