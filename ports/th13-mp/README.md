# 东方神灵庙（th13）多人版移植（th13-mp）

仓内派生自 `ports/th13`（基线与登记见
[docs/migration/th13mp-derivation-20261108.md](../../docs/migration/th13mp-derivation-20261108.md)）。
SP 行为还原的能力与证据沿 SP 目录维护；本目录在其上叠加 2–3 人合作联机
（回滚架构参考 `ports/th12-mp` 与共用 netplay SDK）。

- **MP 规则真值**：[docs/RULES.md](docs/RULES.md)
- MP 阶段工单（M0–M10）：[docs/MP-DEVELOPMENT.md](docs/MP-DEVELOPMENT.md)
- SP 开发流程（构建/取像入口同源）：[docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)
- 适配验收记录：[ADAPTATION.md](ADAPTATION.md)
- 原版资源由使用者本地提供，仓库不携带任何原版文件；
  `web/sdl-runtime/assets` 符号链接共享 SP 的资产目录。

## 构建

```sh
node portable/build.mjs            # 网页运行时（-DTH13_MULTIPLAYER=1 常开）
node portable/build-harness.mjs    # WASI 无头对照 harness（oracle 比对用）
```

工具链要求见仓库 [docs/adaptation/build.md](../../docs/adaptation/build.md)；本项目提供统一容器环境（工作区 `container/Dockerfile`，本地 podman 与 CI 共用）。

M0 门：宏未被代码消费前，MP 构建产物须与 SP 构建产物 wasm 字节一致。

## 运行时

`web/sdl-runtime/th13.html` 装载 `web/artifacts/sdl3/` 产物：装载挡板（sig+ascii）→
七 bank 常驻装载 → 标题 →（主菜单 → 难度 → 角色 → 开局）→ 对局 → Esc 暂停。
资源从 `/assets/<文件名>` 读取（`text/sig/ascii/effect/title/title_v/help.anm` 与
对局期 `front.anm`，文件名与原版 dat 内一致）；键盘经 SDL3 键位态映射为
原版 32 位输入掩码（Z/X/Shift/方向/小键盘/Esc/Ctrl/Enter）。

## 同帧取像（golden 对拍）

`web/sdl-runtime/th13-debug.html` 为回放取帧宿主页：`.rpy` 录像与移速源
`plXXa/b.sht` 备入 `/assets` 后，`th13_debug_replay` 按录像头（难度/角色/关卡）
直入对局，`th13_debug_frame` 以固定 1/60 步长逐帧推进（不经 rAF 帧钟），
按**游戏帧号**跳帧（golden 换算 `video = game + 400`，见工作区
`artifacts/reference/golden/README.md`）后经 `canvas.toDataURL` 导出 PNG。
驱动入口 `tools/shot-frames.mjs`（th13-shot 镜像，chromium+playwright 无头）：
对照 golden 帧号导出 `ours-frame-*.png`、双会话确定性自检、合成
`side-by-side.png`，量测用 `scripts/visual_diff.py`。
