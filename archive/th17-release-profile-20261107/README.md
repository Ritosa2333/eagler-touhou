# TH17 主站发布与普通关函数采样 · 2026-11-07

主站已发布 `dc0e6bc7aeb93262ca65bcc17108f86d8c3241f5` 对应的普通版与多人版运行包。公网逐项核对 28 个文件的长度、SHA-256 和 Wasm MIME，通过启动器配置、版本选择器与不可变运行目录确认入口一致。发布保留其他作品与现有多人功能入口。

| 身份 | 值 |
| --- | --- |
| PR | [#199](https://github.com/Ritosa2333/eagler-touhou/pull/199) |
| 普通版 | `1.0.14-eagler-3bdc287705b9` |
| 多人版 | `1.0.14-eagler-716274413148` |
| Release | `sha256-e4c87946fe131bd80b0c99e31dda877bb1e293b2c11931c0552c6d2eb51a05ce` |

[发布回执](activation.json)记录前后 release 和完整运行文件身份；[公网验证](public-verification.json)记录入口与校验结果。

## 普通关三人回滚采样

本轮函数采样覆盖一面道中、Boss 非符与符卡、同步换面和二面开场。三个独立页面连接本地 Relay，使用 Intel 核显、1280×960、145 ms 注入延迟、4 倍 CPU 限速。运行包启用函数名采样，诊断 Wasm 为 `18f7762b8b123723d22f7c7f0379d0f71eacd7589243430cbe9505026534084f`；游戏源码对应上述发布提交。主站继续使用发布表中的运行包。

OGG 场景加载标题、一二面道中与 Boss 音乐，启动时核对每席音频健康事件中的 `musicEnabled`。9,019 个确认帧在三端一致，覆盖 1,464 帧 Boss 非符、1,000 帧符卡与二面。三席实际呈现为 58.53 / 58.66 / 58.55 FPS，平均快照耗时为 1.834 / 2.006 / 2.001 ms。最大呈现间隔为 485.56 / 605.20 / 621.91 ms，包含同步换面。

主线程 CDP 以 1,000 µs 间隔采样。OGG 场景中，快照抓取占已归因游戏主循环采样权重的 33.00%，绘制占 30.66%，音频 pump 占 3.92%。快照路径中的动画 checkpoint 约占 34.87%，其字节比较约占动画 checkpoint 的 29.36%。这些数据确定后续普通关降耗的优先级：

1. **动画快照。** `AnmCheckpoint` 为每个动画复制 VM 与几何状态，并比较静止插值器。优先评估按修改版本复用静止插值块；版本记录需要覆盖脚本直接写入、对象复用和回滚恢复，继续保留完整战斗状态。
2. **绘制提交。** `Renderer::issue` 更新实例偏移时重复启用九个属性并设置 divisor。可将 VAO 的一次性属性启用与每批次偏移更新分开，再核对 ImGui 与重采样切换后的状态。该路径占绘制采样权重约 41.13%，下一轮沿批次与布局调用继续分解。
3. **换面加载。** `RunGameplay::next` 同步构造下一关资源并预载图像。独立记录资源读取、解压、ANM 打开和 GPU 上传，优先压缩换面长帧。透明范围扫描的改进按加载指标衡量。

另一次关闭音乐的独立采样核对 8,898 个确认帧，快照与绘制同样居于主要路径。两次采样分别保存；性能收益比较沿用[上一轮固定构建对照](../th17-overhead-20261107/README.md)的口径。

## 复测入口

`room-browser.mjs` 新增 `--cpu-profile`，每种人数将房主渲染进程的原始采样写为 `<人数>p.cpuprofile`。报告保存音乐模式和采样开关。设置 `TH17_ROOM_MUSIC_DIR` 后加载本地 OGG；该音乐资源集覆盖标题和普通一、二面。

```powershell
$env:EAGLER_TH17_BUILD = '<诊断运行包目录>'
$env:EAGLER_TH17_DATA = '<th17.dat>'
$env:TH17_ROOM_GOTHIC = '<msgothic.ttc>'
$env:TH17_ROOM_UNIFONT = '<unifont.otf>'
$env:TH17_ROOM_MUSIC_DIR = '<music/ogg>'
$env:TH17_ROOM_EVIDENCE = '<输出目录>'
$env:TH17_ROOM_SEATS = '3'
$env:TH17_ROOM_DIFFICULTY = '1'
$env:TH17_ROOM_DELAY_MS = '145'
$env:TH17_ROOM_CPU_THROTTLE = '4'
$env:NATIVE_GPU = '1'
node ports/th17-mp/web/tests/multiplayer/room-browser.mjs --launcher-preview --check-presentation --independent-pages --through-stage-two --cpu-profile
python archive/th17-release-profile-20261107/summarize-profile.py '<输出目录>/3p.cpuprofile' '<输出目录>/cpu-summary.json'
```

函数名构建保留 `--profiling-funcs`。汇总脚本按调用栈去除递归同名函数的重复计数；表中耗时为限速渲染进程的采样权重，包含启动阶段。

[OGG 房间报告](ogg-room-report.json) · [OGG 函数汇总](ogg-cpu-summary.json) · [关闭音乐的房间报告](silent-room-report.json) · [关闭音乐的函数汇总](silent-cpu-summary.json) · [汇总脚本](summarize-profile.py)
