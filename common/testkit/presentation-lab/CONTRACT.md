# Presentation Lab 接入契约

会话导出保留当前窗口的完整原始证据，最近窗口构成有界索引，标记 `history-summary` 与 `rawEvidenceRetained:false`。索引保存场景、对象与判定摘要；逐窗原始素材通过窗口导出保存。新增原始字段不会自动复制进历史索引。

控制器 `preview` 在返回的观察帧中保留原生成功绘制回执 `status:drawn`，配对采样据此确认额外绘制确实执行。手动诊断模式通过其 Runtime epoch 和逐步原生读取绑定 Replay；正式包的首帧呈现使用独立的冷启动／刷新图像证据。

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
- Replay 消费 trace 环清空时 `count` 为 `0` 且 `latest` 为空；环有样本时 `latest` 保存完整的 36-word 最新行，`firstAvailable`、`count` 与 `nextSequence` 必须一致。
- `StateEvidence.authority` 必须按 `state-authority.mjs` 的共享 schema 映射十个权威域。每个域列出对应摘要组与真实源字段；任何缺域、空映射、未关联摘要组或样本间映射变化都会令纯度为 `unknown`。
- 对象身份由内存地址与生命周期或 generation 信息共同确定。
- 适配层说明状态写入者、发布点、绘制消费者、最终提交点、连续性和恢复规则。

## 权威域

标题适配层使用共享域表达相同的观察责任，每作只负责把本作生产状态字段接到对应摘要组：

| 域 | 必须接入的权威状态 |
| --- | --- |
| `simulation-world` | RNG、时钟、世界、对象池、场景控制与绘制使用的游戏状态 |
| `gameplay-session` | 运行会话、分数与关卡历史、结果和持久配置 |
| `practice-state` | 练习设置、辅助状态、练习结果及其输入缓冲 |
| `replay-state` | 正在消费的 Replay、正在录制的 Replay、游标与编码缓冲 |
| `input-transients` | 键盘、控制器、触摸手势及游戏侧已消费输入状态 |
| `user-payload-buffers` | 用户导入、录制或待保存的原始字节与逻辑长度 |
| `persistent-storage` | 成功写入的用户载荷身份、字节数及写入状态 |
| `synchronous-audio` | 绘制边界内可同步观测的音频操作计数 |
| `host-cadence` | 固定步、呈现调度及其逻辑时钟状态 |
| `presentation-owners` | 绘制对象的权威字段及生产路径出生／退役身份 |

每条 `sourceFields` 应使用可定位的类／字段或函数／缓冲范围，例如 `ReplayStream.input[0..input_end)`。摘要应逐字段或逐原始载荷字节计算，排除地址、填充、纹理缓存、GPU缓存及设备墙钟。固定池和动态池都应在真实分配与退役处发布代次事件。负控应在实际绘制调用内更改一个已映射域，由共同比较器检测到该域差异。

新作从自身真实摘要表构造映射。`groupFields` 列出每个摘要实际覆盖的字段，`domains` 将摘要组归入共同责任域；共同构造器会保留缺项，比较器据此保持 `unknown`：

```js
import {defineStateAuthority} from 'common/testkit/presentation-lab/state-authority.mjs';

const groupFields = {
  world: ['World::tick', 'World::rng.state', 'EnemyPool[0..capacity)'],
  input: ['InputController::held', 'TouchGesture::activePoints'],
};
const domains = {
  'simulation-world': ['world'],
  'input-transients': ['input'],
  // 其余共享域也在本作映射中显式提供。
};
const authority = defineStateAuthority({groupFields, domains});
const evidence = {
  coverageVersion: 'game/state/1',
  groups: Object.entries(groupFields).map(([id, includedFields]) => ({
    id, digest: digestFields(id), includedFields,
  })),
  missingGroups: [],
  authority,
};
```

`includedFields` 与 `sourceFields` 必须对应真实摘要输入。构造器不会补齐遗漏的域；校验还会拒绝未映射的摘要组、字段和跨样本映射变化。标题测试负责确认列出的源字段确实被采样并参与摘要。

## 接入验收

在该游戏的真实绘制路径上审计所有状态拥有者，采集重复绘制证据，并覆盖可能被路径修改的权威状态与回滚状态。诊断导出只进入该游戏的审计构建配置；生产包不包含诊断导出或 Lab。

修改必需方法、回执、标准化记录或纯度规则时提升 API major；新增报告字段可留在当前 major。
