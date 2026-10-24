# th11 Presentation Lab

Lab 将固定逻辑推进、真实绘制和帧间采样拆成可重复的操作。原生 ANM 提交记录包含对象身份、generation、重复提交编号、脚本、图层、连续字段、顶点和包围盒；状态摘要按拥有者分组读取。

## 构建与启动

设置 `EMSDK`、`EAGLER_FONT_ROOT`、`TH11_LAB_FONT`、`TH11_LAB_DATA` 后运行：

```text
node ports/th11/portable/build.mjs --presentation-lab
node ports/th11/portable/presentation-lab/serve.mjs
```

默认地址为 `http://127.0.0.1:8136/`。构建位于 `web/artifacts/presentation-lab`，隔离运行文件位于 `artifacts/presentation-lab`。`TH11_OUTPUT` 与 `TH11_LAB_OUTPUT` 可分别选择两个目录，`PORT` 可指定空闲端口。

服务从当前源码准备宿主模块、运动录像模块和资源清单，每次启动刷新隔离运行目录。字体与原版素材由显式路径提供。浏览器命令等待当前实例的实际回执，Demo、Replay 和关卡种子在冻结驱动后加载。

## 操作与证据

- `freeze`／`resume` 控制显示回调，并丢弃冻结期间的墙钟积累。
- `advanceOneTick` 执行完整逻辑事务与权威 Draw。
- `drawOnly(alpha)` 执行真实呈现路径，观察前后状态摘要。
- `?stage=1` 可直接进入生产重启路径，`character`／`difficulty` 选择场景参数。
- Demo／Replay 巡检按连续逻辑端点与 alpha 序列定位异常，保存构建身份和采样完整性。
- 负控开关改变观察字段，用于验证分析器能识别重复绘制差异。

构建、CPU 契约、实际浏览器场景和完整状态拥有者验收承担各自的论证职责。当前运行证据见[验收记录](../../docs/records/2026-10-24-display-high-refresh.md)，巡检参数见[CAMPAIGN](CAMPAIGN.md)。

## 生产交付

生产包核对构建 profile、诊断标志和真实 WASM 导出。Lab 与诊断构建在写文件前被拒绝。检查入口为 `portable/check-presentation-lab.mjs`、`portable/check-build-modes.mjs` 和 `portable/production-build.test.mjs`。
