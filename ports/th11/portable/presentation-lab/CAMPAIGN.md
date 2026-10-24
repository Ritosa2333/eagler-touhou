# 插值位移巡检（campaign）

## 目标

跑一场 TH11 Replay，自动记录“插值帧里位移过大 / 不沿端点路径”的对象，
用于发现 presentation 状态没有在作者重置处失效导致的闪烁。只做诊断，
不影响生产 Runtime。

## 判据

1. 冻结 rAF，但保留游戏逻辑与真实 Draw。
2. 每 N 个固定 tick 取一对 reference frame（上一 tick / 当前 tick）。
3. 对同一 tick 依次发出 alpha = 0, .25, .5, .75, 1 的 draw-only 采样。
4. 对每个对象比较：
   - **referenceStep**：上一/当前端点的位移；
   - **maxSampleStep**：相邻 alpha 采样之间的最大位移；
   - **maxError**：采样位置与“端点线性插值”的最大偏差。
5. 超过阈值写入报告，保留 owner / objectId / layer / file / script / sprite / tick / alpha。

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `--min-step` | 8 | 端点位移小于该值不检查（静止/慢速） |
| `--max-step` | 48 | 相邻 alpha 位移超过该值 → `large-interpolated-step` |
| `--max-error` | 2.5 | 采样偏离端点线性路径 → `endpoint-mismatch` |
| `--rebase-step` | 256 | 端点跳变达到该值且采样只落在端点时，归入 `authored-rebase` |
| `--stride` | 30 | 每多少个 tick 采样一次 |
| `--alphas` | 0,0.25,0.5,0.75,1 | 采样 alpha 序列 |

分类：

- `large-interpolated-step`：插值确实在做，但把一次“应当重置”的跳变平滑拉过去了。
- `endpoint-mismatch`：采样没有落在端点之间（通常是 state 未重置 / 对象根本没插值）。
- `authored-rebase`：512/1024 等世界重基导致的 snap（采样只落在端点）。
  这是启发式豁免，不是“生命周期证明”；阈值以下（如 200）的旧端点仍会告警。
  后续应以作者重基标记替代该启发式。

## 复现命令

需要先构建诊断 Runtime 并启动 Lab（`start-lab.ps1 -Build`），
`campaign.mjs` 依赖本机 `playwright`（可从仓库 launcher 的 node_modules 解析）。

```powershell
# 内置 archive demo（0..3）
node ports/th11/portable/presentation-lab/campaign.mjs --demo 0 --ticks 3600 --stride 30

# 自己的 Replay（建议能到 Boss）
node ports/th11/portable/presentation-lab/campaign.mjs --replay "D:/path/th11_11.rpy" --ticks 12000
```

输出默认写入 `ports/th11/artifacts/presentation-lab/campaign/<name>.json`。

## 最终结果（原 `th11@eagler` 分支，2026-10-24）

`th11_12.rpy`（Extra，12,000 tick / 3,950 对象）：

| 分类 | 数量 | 说明 |
| --- | --- | --- |
| `no-interpolation` | 2099 → **14** | 自机弹端点修复前 / 后 |
| `authored-rebase` | 1370 | 512 重基 snap（采样只落在端点） |
| `blended-discontinuity` | 0 | 没有跨重基平滑插值 |

剩余 14 条证据：`ANM layer 10`，`file 7 / script 20,23 / sprite 35`，
`referenceStep≈10 px/tick`，`maxSampleStep=0`，`maxError≈referenceStep`。
这是自机弹的另一条写入路径，尚未修完；不要把它当作通过。

## 输出字段

```json
{
  "tick": 900,
  "owner": "ANM layer 10",
  "objectId": 268522040,
  "layer": 10,
  "fileIndex": 7,
  "scriptIndex": 20,
  "spriteIndex": 35,
  "referenceStep": 10.587,
  "maxSampleStep": 0,
  "maxError": 10.587,
  "worstAlpha": 0,
  "reasons": ["endpoint-mismatch"],
  "kind": "no-interpolation"
}
```

## 边界

- 只覆盖 ANM 提交记录；非 ANM 屏幕矩形、HUD 文本、音频、存档不在范围内。
- 采样是 CPU 顶点记录，不是 GPU 像素回读。
- 本改动为 **normal** 版本；`ports/th11-mp` 的同步按 CONTRIBUTING 由同步维护者处理。
- 报告是候选清单，最终视觉验收仍需真机高刷观察。
