# th11 帧间位移巡检

巡检以真实 Draw 端点和冻结后的 alpha 采样检查连续位移，记录对象身份、脚本、图层与首个异常窗口。普通版和 MP 使用各自的构建与驱动，报告同时保存 WASM 身份。

## 判据

- 每隔 `stride` 个固定 tick 读取相邻两个权威端点。
- 在同一逻辑状态采样 alpha 0、0.25、0.5、0.75、1。
- `referenceStep` 表示两端点的合成位移，`maxSampleStep` 表示相邻采样的最大位移，`maxError` 表示采样与线性路径的距离。
- `large-interpolated-step` 和 `endpoint-mismatch` 保留在候选列表。300、512、1024 像素跳变与其他幅度使用同一套异常判据。
- 位移幅度和端点形状用于分类，不构成作者重基的证明。出生、脚本切换与生命周期由对象身份和实际状态单独核对。
- `complete` 表示请求的采样窗口全部执行且页面、驱动与采样没有错误。中断或异常保存报告并以非零状态退出。

| 参数 | 默认 | 用途 |
| --- | --- | --- |
| `--ticks` | 3600 | 固定 tick 数量 |
| `--stride` | 30 | 采样间隔，范围 1–600 |
| `--min-step` | 8 | 连续位移检查的最小幅度 |
| `--max-step` | 48 | 相邻采样位移告警线 |
| `--max-error` | 2.5 | 线性路径误差告警线 |
| `--rebase-step` | 256 | 大幅跳变的分类线 |
| `--alphas` | 0,0.25,0.5,0.75,1 | 包含两端点的有序采样序列 |

## 运行

设置 `EMSDK`、`EAGLER_FONT_ROOT`、`TH11_LAB_FONT`、`TH11_LAB_DATA`，分别指向 SDK、cp932.bin 目录、Gothic 字体和自备 th11.dat。

```powershell
node ports/th11/portable/build.mjs --presentation-lab
node ports/th11/portable/presentation-lab/serve.mjs
node ports/th11/portable/presentation-lab/campaign.mjs --demo 0 --ticks 300 --stride 30
node ports/th11/portable/presentation-lab/campaign.mjs --replay D:/replays/th11_12.rpy --ticks 12000
```

浏览器入口共用 `tools/browser/launch.mjs`，自动使用工作区 Playwright 与已有 Chromium。`TH11_OUTPUT` 选择构建目录，`TH11_LAB_OUTPUT` 选择隔离运行目录，`PORT` 与 `--url` 配合选择本地端口。

原版素材与报告保存在本地。默认报告为 `ports/th11/artifacts/presentation-lab/campaign/<name>.json`，可用 `--out` 指定路径。浏览器启动、宿主资源、命令回执、Demo／Replay 装载和采样构成一条实际运行链。

## 验证

[2026-10-24 验收记录](../../docs/records/2026-10-24-display-high-refresh.md)登记当前源码上的构建、17 项准入与位移回归、三种 SDL 宿主编译配置及浏览器 Demo 巡检。

ANM 字段、对象身份与 CPU 提交几何负责定位帧间位移问题。GPU 像素、声音、存档与网络状态分别使用对应专项验收。
