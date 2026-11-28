# th13 端口局部入口

目标是还原原版 v1.00c 的行为、真实渲染和网页最终画布；局部单测不能代替全链路证据。

## 工作方式

- 构建、测试与原版分析都在容器里跑，入口是 `tools/container.sh`（浏览器验证加 `--shot`）；不要在宿主安装依赖，持久写入只落本仓库和外层工作区的 `artifacts/`、`.cache/`。命令见 [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)。
- 提交前跑 `tools/gate.sh`：排版检查、harness 构建、回归摘要、全部套件与基线比对。
- 源码不写注释：依据、契约和原版地址写进提交说明、测试与 [docs/fidelity-map.md](docs/fidelity-map.md)，`node tools/no-comments.mjs` 在门槛里强制。
- 不从旧 HANDOFF、旧报告或代理自报恢复状态；每个修改绑定具名假设、原版依据和最窄回归。

## 不可破坏边界

- 原版资产、Replay、反编译 dump、帧和捕获产物只放外层 ignored 的 `games/`、`artifacts/`，不得加入 monorepo。
- 不接触凭证，不清理已有修改；用户未明确要求时不提交、推送、回退或删除。
- `portable/sdl` 共享后端与 th15 不得静默漂移；生产提交是唯一绘制真值，trace/harness 不能代替网页认证。
- 行为、资源、绘制语义、音频时序和最终 framebuffer 分别验证；缺独立原版 GPU 参考时最终输出保持 BLOCKED。
- 文档、状态文件、golden、比较器字段和容差不是制造通过的手段；无法证明的结果原样保留。

接口和契约见本目录 `docs/`。