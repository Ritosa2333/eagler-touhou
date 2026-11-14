# th13-mp 端口局部入口

多人版目录（仓内派生自 `ports/th13`；SP 的原版 v1.00c 行为/渲染/最终画布还原
目标与证据沿 SP 目录维护，本目录不重复认证）。MP 规则真值是本目录
`docs/RULES.md`；实现与测试逐条引用其编号。局部单测不能代替全链路证据；
锁步/回滚自比证明"纠正正确"，不冒称独立原版 fidelity。

- SP 通用修复先落 `ports/th13` 再经 `tools/ports/sync.mjs` 接收；禁止只改
  MP 侧共享代码造成两侧静默漂移（`portable/sdl` 与 th15 的共享红线同理）。
- 引入首个 `#ifdef TH13_MULTIPLAYER` 代码块后必须复跑单席等价门（MP 产物 vs
  SP 产物，同输入逐位一致），见 `docs/MP-DEVELOPMENT.md`。

## 工作方式

- 外层仓库根是 `/home/mystia/项目/th13`（容器内 `/work`）；先读外层 `AGENTS.md`，再按当前产品任务施工。
- 构建、测试、原版分析经 `make exec CMD='...'`，浏览器验证经 `make shot CMD='...'`；不要在宿主安装依赖。
- `scripts/verify.py`、`resume` 和 `BLOCKED` 是统一工具契约中的固定入口/状态词。它们用于诊断和证据登记，不阻止用户已授权且范围明确的产品修复。
- 不从旧 HANDOFF、旧报告或代理自报恢复状态；每个修改绑定具名假设、原版依据和最窄回归。

## 不可破坏边界

- 原版资产、Replay、反编译 dump、帧和捕获产物只放外层 ignored 的 `games/`、`artifacts/`，不得加入 monorepo。
- 不接触凭证，不清理已有修改；用户未明确要求时不提交、推送、回退或删除。
- `portable/sdl` 共享后端与 th15 不得静默漂移；生产提交是唯一绘制真值，trace/harness 不能代替网页认证。
- 行为、资源、绘制语义、音频时序和最终 framebuffer 分别验证；缺独立原版 GPU 参考时最终输出保持 BLOCKED。
- 文档、状态文件、golden、比较器字段和容差不是制造通过的手段；无法证明的结果原样保留。

接口和契约见本目录 `docs/`；工具协议见外层 `verification/README.md`。