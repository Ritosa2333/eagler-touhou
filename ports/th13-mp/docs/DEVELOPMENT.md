# th13 开发指南

## 总体架构

- **行为还原**：原版 th13.exe（v1.00c）经 Ghidra headless 反编译导出函数候选与调用图；还原后的 C++ 落在 `web/cpp/game/`，按子系统推进（RNG → 帧循环 → ANM VM → ECL VM → 敌/弹/道具 → 自机 → 神灵/灵界系统）。
- **双目标同源**：同一份 C++ 编译两次——
  1. 网页运行时：Emscripten + SDL3（`portable/build.mjs` → `web/artifacts/sdl3/`）；
  2. WASI 无头 harness：wasi-sdk（`portable/build-harness.mjs` → `web/artifacts/wasi/`），供 oracle 对照与 CI。
- **Oracle 对照**：Unicorn 无头模拟原版 EXE（桩掉 Win32/D3D），以录像驱动逐帧导出状态池，与 harness 输出逐字段比对，首个分歧即回归定位点。原版二进制与 oracle 轨迹只在开发期使用，不进网页构建。
- **地址锚点**：`web/cpp/rebuild/AddressMap.hpp` 登记每个子系统的原版入口，供 oracle 与回归脚本引用。

## 统一容器环境

全工作流在标准容器内进行（本地 podman 与 GitHub Actions 共用本目录 `container/Dockerfile`，多阶段：base → tools → dev → ci / shot）。Emscripten 端口缓存经 `EM_CACHE` 落工作区卷（`.cache/emscripten`），跨容器运行复用。

仓库内自带 Dockerfile，克隆 monorepo 后即可直接使用（无需外部包装）：

```sh
# 仓库根目录下：
podman build --target dev -t th13-dev --build-arg DEV_UID=$(id -u) --build-arg DEV_GID=$(id -g) ports/th13/container
podman run --rm --userns=keep-id -e HOME=/home/dev -it -v "$PWD:/work:Z" -w /work/eagler-touhou/ports/th13 th13-dev bash
# 单命令（构建/测试）：
podman run --rm --userns=keep-id -e HOME=/home/dev -v "$PWD:/work:Z" -w /work/eagler-touhou/ports/th13 th13-dev bash -lc 'node portable/build.mjs'
```

工具链版本（构建时固定）：Node 22 / Emscripten 6.0.9 / wasi-sdk 34 / Ghidra 12.1.3 / Temurin JDK 21。

开发工作区另有外层 `Makefile` 包装上述命令（`make image / shell / exec-port CMD='…'`），并管理原版资源与 thtk 临时构建；工作区顶层的 `games/`（原版资源）与 `artifacts/`（大体积产物、Ghidra 导出、参考视频帧）不入库。thtk 以临时克隆方式构建、用后清理。

## 原版资源准备

```sh
make extract   # 解包 th13ogg.7z → games/th13/，生成 SHA256 清单与版本判定
```

原版目录包含 `th13.exe`、`th13.dat`、`thbgmogg.dat`（OGG 音乐版）。资源提取（ECL/ANM/STD/msg dump）由 thtk 完成；dump 文本属开发期对照证据，落**工作区** `artifacts/reference/dumps/`（不入库），格式版本结论记录在 `docs/ASSETS.md`。

## 行为还原节奏

每个子系统三重门：Ghidra 参考 → oracle 比对绿灯 → `node --test`。恢复顺序：

1. RNG 与帧计时
2. ANM 虚拟机（动画脚本）
3. ECL 虚拟机（敌机脚本）
4. 敌人 / 弹幕 / 道具管理器
5. 自机
6. th13 特有：神灵收集、灵界 Trance、Overdrive
7. Boss / 符卡 / 全 Stage 通关

### 数值语义纪律（继承参考工作流 machine.hpp + softfloat 的教训）

原版以 MSVC x87 80 位中间量运算，移植侧 SSE2 语义默认不一致。已发生的修正（wrap_angle 每步落 f32、SquareSumRoot 单舍入链、圆周模式插值角维、floor 量化）全部属于此类。规则：

- 新子系统凡涉及**双精度链、超越函数、连加累乘后单次取整**的路径，先写 oracle 用例锁定原版结果，再落 C++；必要时引入 x87 语义助手（参照 ports/th10 的 `web/cpp/rebuild/machine.hpp` + softfloat extF80 子集），不逐例手修散落各处
- 遇到「分歧在数值第 N 位小数」类对齐失败，优先怀疑 x87/SSE 差异，其次才是逻辑错误

### 与静态重编译路线的关系（混合升级预案）

参考工作流（th10）采用机械静态重编译 + 渐进类型化替换的双层形态（`rebuild/` 与 `game/` 并存）。本项目当前为 clean-room + oracle 验收路线，Stage 1 事件流已逐字节对齐证明可行；若后续关卡/Boss 重符卡段频繁卡在浮点或边界语义，启动最小转译层（页级翻译仅覆盖卡住的函数，与 `game/` 并存），向参考工作流的混合形态收敛。

## 测试

```sh
node --test tests/*.test.mjs   # 仓库内单元测试（rng / ecl / ui / replay）
bash tests/ui/run.sh           # UI 基座六套件（输入/调度/ANM 句柄/HUD/标题/暂停/对局）单独运行
node portable/build-harness.mjs && node --experimental-wasi-unstable-preview1 tools/run-harness.mjs   # WASI harness（RNG 冒烟）
node --experimental-wasi-unstable-preview1 tools/run-harness.mjs stage1   # stage1：st01 跑 3600 帧，事件流落 web/artifacts/wasi/stage1-events.jsonl（契约见 docs/plans/event-contract.md）
node --experimental-wasi-unstable-preview1 tools/run-harness.mjs replay games/replays/th13_udSr01.rpy   # 录像解码 + 逐帧输入注入（格式见 docs/plans/replay.md）
```

## 同帧取像（golden 对拍）

```sh
# 工作区根：th13-shot 镜像内驱动（先 node portable/build.mjs 出产物）
make shot CMD='node eagler-touhou/ports/th13/tools/shot-frames.mjs'
```

链路：th13-debug.html 回放会话直入对局 → `th13_debug_frame` 固定 1/60 确定性帧泵 →
按 golden 游戏帧号跳帧导出 `artifacts/reference/golden/ours-frame-*.png` →
双会话确定性自检 → 合成 `side-by-side.png`。量测与阈值判定：
`python3 scripts/visual_diff.py --golden artifacts/reference/golden --ours <移植帧目录>`
（帧文件需按 `frame_%06d.png` 命名，驱动产物改名复制即可）。
