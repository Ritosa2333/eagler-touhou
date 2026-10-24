# 插值位移巡检（campaign）

用途：跑一场 TH11 Replay，自动记录“插值帧里位移过大/不沿端点路径”的对象，
用于发现 presentation 状态没有在作者重置处失效导致的闪烁。

## 原理

1. 隔离诊断 Runtime 冻结 rAF，但保留游戏逻辑与真实 Draw。
2. 每 N 个固定 tick 取一对 reference frame（上一 tick / 当前 tick）。
3. 对同一 tick 依次发出 alpha = 0, .25, .5, .75, 1 的 draw-only 采样。
4. 对每个对象比较：
   - **referenceStep**：上一/当前端点的位移；
   - **maxSampleStep**：相邻 alpha 采样之间的最大位移；
   - **maxError**：采样位置与“端点线性插值”的最大偏差。
5. 超过阈值即写入报告，保留 owner / objectId / layer / tick / alpha。

## 阈值

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `--min-step` | 8 | 端点位移小于该值的对象不检查（静止/慢速） |
| `--max-step` | 48 | 相邻 alpha 位移超过该值 → `large-interpolated-step` |
| `--max-error` | 2.5 | 采样偏离端点线性路径 → `endpoint-mismatch` |
| `--stride` | 30 | 每多少个 tick 采样一次 |
| `--alphas` | 0,0.25,0.5,0.75,1 | 采样 alpha 序列 |

- `large-interpolated-step`：插值确实在做，但把一次“应当重置”的跳变平滑地拉过去了（位移过大）。
- `endpoint-mismatch`：采样没有落在端点之间（通常是 state 未重置 / 该对象根本没插值，120Hz 下会表现为“两个坐标来回跳”）。

## 用法

```powershell
# 内置 archive demo（0..3），无需外部文件
node portable/presentation-lab/campaign.mjs --demo 0 --ticks 3600 --stride 30

# 自己的 Replay（建议选能到 Boss 的）
node portable/presentation-lab/campaign.mjs --replay "D:/path/th11_01.rpy" --ticks 7200

# 更严格阈值
node portable/presentation-lab/campaign.mjs --demo 0 --max-step 24 --max-error 1.5
```

输出默认写入 `artifacts/presentation-lab/campaign/<name>.json`，字段：

```json
{
  "tick": 120,
  "owner": "ANM layer 10",
  "objectId": 12345,
  "layer": 10,
  "referenceStep": 24,
  "maxSampleStep": 0,
  "maxError": 24,
  "worstAlpha": 0,
  "reasons": ["endpoint-mismatch"]
}
```

## 当前已知结果（demo 0，2026-10-23）

- 首轮 240 tick：`ANM layer 10` 的 3D 场景对象全部被标记，
  `maxSampleStep = 0`、`maxError = referenceStep`：
  采样帧完全没有插值，直接在两个 tick 坐标之间切换。
  在 120Hz 上就是“每个坐标显示两帧”的来回跳，和群友描述的 Boss 战背景现象一致。
- 已加入的防线：相机硬切（STD op 2）与 >64 单位/视角跳变的 snap、
  舞台 pass 的位置门限 32、position/script_position 反向重基时的合成位置插值、
  以及舞台对象在 Draw 前的端点补发；该层对象的 owner 侧端点仍在继续修。

## 实测：th11_12.rpy（Normal 通关档，2026-10-24）

命令：`--replay .../th11_12.rpy --ticks 12000 --stride 60`

结果：200 个采样窗口、3950 个对象、3469 条命中，按行为分三类：

| 类别 | 数量 | 说明 |
| --- | --- | --- |
| `ANM layer 10`，步长 <32，采样静止 | 2099 | **没有插值**：每 tick 直接切坐标，120Hz 下每个坐标显示两帧（就是“两个坐标来回跳”） |
| `舞台背景/前景`，步长 ≥400，采样只落在端点 | 1184 + 144 | 512 单位的场景重基，已 snap（144 条是 alpha 0 取到上一端点、alpha≥0.5 取到当前端点，`maxError≈step/2`）；这是**正确**的离散边界，归入 `authored-rebase` |

结论：
- 真正的闪烁告警只剩第 1 类：layer-10 对象完全不插值（2099 条）。
- 512 重基的 1328 条（1184 静态 + 144 端点跳变）都是正确 snap，归入 `authored-rebase`，不再当作告警。
- 复跑后：`flagged=2243`（其中 2099 为 `no-interpolation`、144 在分类前计入）、`rebases=1226`；按新规则 144 也应归入 rebase，最终告警应只剩 2099。

## 定位与修复：layer 10 = 自机弹

layer 10 不是创建 owner，而是 ANM 脚本用 opcode 68（SetLayer）设置的绘制层。命中的对象指纹：

`fileIndex=7`（player.anm）、`scriptIndex=5/20/23`（ShotManager 的 `spec.animation+5`）、`spriteIndex=24/35`、`layer=10`。

原因：`ShotManager::update` 先写 `a->position=screen(...)`，之后 `animations.update` 才执行
`AnmVm::update`，端点采集发生在“写入之后”，所以 `before == current`，插值层拿不到 delta。

修复：

- 在 `ShotManager::update` 写 position 前调用 `presentation::capture(*a)`（extra animation 同样）；
- spawn 时写入 position 后调用 `presentation::snap(*vm)`，避免第一帧从原点插值过来。

复跑结果：`no-interpolation` 从 **2099 → 14**，`rebases` 保持 1370。剩余 14 条是
`script 20/23 + sprite 35`（约 10 px/tick，疑似 type-4/激光或 extra animation 的另一条写入路径），后续再收。

## 边界

- 只覆盖 ANM 提交记录；非 ANM 屏幕矩形、HUD 文本、音频、存档不在范围内。
- 采样是 CPU 顶点记录，不是 GPU 像素回读。
- 报告是候选清单，最终视觉验收仍需真机高刷观察。
