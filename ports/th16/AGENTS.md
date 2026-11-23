# th16 端口局部入口

长程闭环：一口气长程交付到 100% 原版对齐（行为/画面/系统对原版等价、全关卡·难度·机体×子季节、代码质量与可读性对齐 `ports/th06` 与 `ports/th07`、Snapdragon 865 级 60fps、WASI 3600 帧 <10s），达成黄金交付标准前不得提前停止；持续完成代码 + 测试 + 提交推送的可验证推进，不得只做几个部分就停下来等待下一轮。

目标是还原原版 v1.00a 的行为、真实渲染和网页最终画布：对任意稳定输入，移植版输出与原版输出一致。局部单测不能代替全链路证据。

## 工作方式

- 先读仓库根 `AGENTS.md`，再读本目录 `docs/CODE_QUALITY.md`（代码质量与可读性标准）、`docs/DEVELOPMENT.md`（工具链、构建、oracle）与 `docs/plans/roadmap.md`（当前里程碑）。
- 每个还原件都以 Ghidra 导出（`artifacts/th16/decomp/jp/functions/<addr>.c/.asm`）为依据，函数头注明原版地址，并在 `web/cpp/rebuild/AddressMap.hpp` 登记。
- 浮点语义：原版为 SSE2 标量单精度（`MULSS/ADDSS/CVTPD2PS`），C++ 用 `float` 且全部构建加 `-ffp-contract=off`；只有 x87 超越函数（43 个函数）需要逐个核对。
- 新子系统先补 oracle（Unicorn 直跑原版函数或原版录像回放对拍），再写实现，再加 `node --test`。

## 不可破坏边界

- 原版资产、Replay、反编译 dump、截图与捕获产物只放外层 ignored 的 `games/`、`artifacts/`、`toolchains/`，不得加入 monorepo。
- `portable/sdl` 共享后端与 th13/th15 同源，不得静默漂移；需要改共享件时同步三处并说明。
- 文档、golden、比较器容差不是制造通过的手段；无法证明的结果原样保留。
- 提交按逻辑改动分开，短中文标题；黄金交付标准达成前不开 PR。
