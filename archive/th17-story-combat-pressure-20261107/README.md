# TH17 普通关实战帧时间 · 2026-11-07

普通版和多人版共同减少 ANM 材质重复写入。普通版接收多人渲染器已有的短批次提交路径。WebAudio 将停止、归零、声像、音量和播放合并为一次重播命令，重播期间复用已连接的输出。自机弹命中查询按角度复用旋转量，保持伤害槽顺序和原版碰撞计算。

普通版四面 CPU 采样中，ANM 绘制、脚本更新和音效处理各自占游戏循环样本约 22%、12% 和 12%。独立 Wasm 对照每组进行 15,000 次命中查询，每次最多检查 176 个激活伤害源。统一射击角度耗时从 48.16 ms 降到 15.96 ms，五种交错角度从 39.71 ms 降到 24.06 ms，离散角度为 41.36 和 40.85 ms。三组的输出校验一致。原版可执行文件对照覆盖 3,072 次查询与 1,280 次分配，共 21,184 项检查。[函数采样](normal-stage-4-profile.json) · [命中耗时](damage-benchmark.json) · [原版判定校验](native-damage-oracle.json)

## 普通版录像

使用同一份原版六面录像分别进入各关。1280×960，独立 Chromium 页面，RTX 5080 Laptop 的 ANGLE D3D11，CPU 限速四倍，OGG。每关采样 900 个逻辑帧，FPS 按画布采集视频计数。

| 关卡 | 游戏回调 P95（ms） | 优化后画布采集 FPS |
| --- | ---: | ---: |
| 三面 | 6.42 → 5.57 | 60.01 |
| 四面 | 10.96 → 9.26 | 60.01 |
| 五面 | 6.05 → 4.07 | 59.97 |
| 六面 | 4.44 → 3.47 | 60.05 |

[普通版基线](normal-baseline.json) · [普通版优化包](normal-final.json)

## 双人键盘房

两席在独立 Chromium 页面通过本地 Relay 开房，均以 1280×960 运行，使用普通难度与 OGG。房测包可从指定关开局并保护残机，键盘脚本持续射击和横移。确认帧校验包含自机射击、敌机脚本、敌弹、激光与道具。四面密集采样覆盖至少 240 帧有 192 发或以上敌弹的时间，五面覆盖至少 180 帧有 128 发或以上敌弹的时间。启用回滚时每方向注入 60 ms 输入延迟和最多 15 ms 抖动。

| 场景 | 画布采集 FPS（P1 / P2） | 敌弹峰值 | ANM 节点峰值 | 一致确认帧 |
| --- | --- | ---: | ---: | ---: |
| [三面道中，4 倍](route-rollback-final-3.json) | 58.12 / 57.83 | 30 | 765 | 3,001 |
| [四面密集，6 倍，锁步基线](dense-lockstep-baseline-4.json) | 59.21 / 59.25 | 256 | | 1,274 |
| [四面密集，6 倍，锁步优化](dense-lockstep-final-4.json) | 60.00 / 59.96 | 256 | 946 | 1,274 |
| [四面密集，6 倍，回滚优化](dense-rollback-final-4.json) | 55.46 / 55.35 | 260 | 924 | 1,274 |
| [五面密集，4 倍，回滚优化](dense-rollback-final-5.json) | 56.99 / 57.12 | 196 | 808 | 1,266 |
| [六面道中至符卡，4 倍，回滚优化](boss-rollback-final-6.json) | 55.77 / 55.77 | 720 | 4,218 | 7,658 |

四面锁步两轮启用同样的分段计时。全程回调 P95 从 13.04／13.00 ms 降到 9.81／9.77 ms。以弹量筛选后的游戏提交为 59.98／59.96 FPS。回滚房四面密集段为 53.19／53.54 FPS，两席自机弹峰值为 98 和 31，确认帧中 53 帧产生射击伤害。

四面加长房测保留六倍 CPU 限速和双向延迟。2,353 个确认帧一致，其中 113 帧产生射击伤害，并记录 113 次非 Boss 击破。两席自机弹峰值为 100 和 31，场上最多 247 发敌弹、924 个 ANM 节点。画布采集为 54.33／54.36 FPS。[杂鱼击破房测](verified-route-4.json)

六面确认到 2,486 帧非符和 181 帧符卡。非符时敌弹峰值 720 发，两席自机弹峰值 93 和 16 发，并记录 121 帧射击伤害。符卡时敌弹峰值 587 发，两席自机弹峰值 92 和 16 发，提交帧率为 51.89／53.74。整段各席分别有 349、359 次提交未推进新的战斗帧。画布采集、提交与向前推进分别统计，推进帧率为 53.44／53.43 FPS。

## 复测

普通版入口为 `ports/th17/web/tests/story-render-browser.mjs`。环境变量 `EAGLER_TH17_BUILD`、`EAGLER_TH17_DATA`、`EAGLER_TH17_SHARED`、`EAGLER_TH17_MUSIC` 和 `EAGLER_TH17_REPLAY` 分别指向运行包、DATA、共享字体、OGG 和包含目标关卡的原版录像。`TH17_STORY_EVIDENCE` 指定报告目录，`TH17_STORY_STAGES=3,4,5,6`、`TH17_STORY_CPU_THROTTLE=4` 选择本轮场景。

双人房沿用[现有房测资源](../th17-render-release-pressure-20261107/README.md#复测)，使用 `--room-benchmark` 构建的包。设置 `TH17_ROOM_STORY_STAGE`、`TH17_ROOM_SAMPLES`、`TH17_ROOM_STORY_BULLET_FLOOR`、`TH17_ROOM_STORY_BULLET_FRAMES` 或 `TH17_ROOM_STORY_SPELL_FRAMES`。以 `--launcher-preview --check-presentation --independent-pages --trace-gaps --observe-display` 运行 `room-browser.mjs`，用 `--lockstep` 或 `--symmetric-delay` 分别复测基础成本和回滚。
