# TH11MP 普通 3P Bomb02 证据摘要

日期：2026-10-13。工作树：`D:\workspace\eagler\worktrees\th11-rollback-smoothness`。

## 结论与范围

**普通 3P、Lunatic、Stage 1 的 selection 0/2/4 Bomb 定向覆盖已完成；严格性能验收仍未通过。** 不能将这项结果写成全角色 Bomb、完整主线、Challenge Bomb 或 4× CPU 已通过。

本次复核读取已完成的收据、两个确认暂停边界的原始 ABI 数组及 runner/游戏源码，没有重新运行命令、浏览器或测试。六份 Replay 的 native 校验、字节级公共输入前缀比较来自本场 runner 的实际执行；本次文档复核没有重新解析二进制文件。

主要证据目录：`artifacts/rollback-smoothness/bomb-3p-lunatic-02/`。

- `receipt.json`：开始 14:24:00.972Z，结束 14:24:28.585Z；`bombStress.completed=true`、`coverageError=null`、`sourceStable=true`。
- 总结果仍为 `status=failed / passed=false / performancePassed=false`；两项 failure 都指向 seat 0 的 107.3 ms 停显，同一现象分别被分段与跨段统计保留。
- CPU 1×；真实 RTC direct；每次发送注入单向 50±5 ms；D=0、P=12；独立三个 Chrome 进程；BGM/SFX 开启；普通模式 `challenge=false`。
- 原生 power 未注入，使用真实拾取；本场触控管理输入和暂停均走 Host 合同，没有修改 native 状态。
- runtime：`release-integration-10-upload-cache/runtime`；WASM SHA-256：`a8f2fdb9a0160f68f06ae31e7b1e9be64559d877357e26f8adc1dab2c6bf4a60`。
- runner SHA-256：`4ee9519bb801fa8bd58bcf384bfd5b74a9fea603703fda67f25d6ac79aa47a7a`。具体源码身份和依赖身份保留在收据 `identity`。

## 两次确认暂停证据

收据路径：`bombStress.proofs[0]`（overlap）与 `bombStress.proofs[1]`（natural-end）。

| 证据 | 本次暂停前最大 NextFrame | 同代同帧 | confirmed / committedThrough | 各席 active | 各席 elapsed | 各席 last_error |
|---|---:|---|---|---|---|---|
| 三 Bomb 重叠 | 384 | generation 0，F=388 | 387 / 387 | 1 / 1 / 1 | 13 / 10 / 7 | 0 / 0 / 0 |
| 自然结束后 | 657 | generation 0，F=658 | 657 / 657 | 0 / 0 / 0 | 210 / 260 / 181 | 0 / 0 / 0 |

本次对每个 proof 的三份原始数组另行逐元素比较：**game56 完全相等，Bomb24 也完全相等**。这是对保存内容的核对，不将有限 ABI 比较扩大为全部世界字节相等。

两次均满足 Bomb ABI `valid=1`、phase=2、world=next=frontier、`correcting=0`、rollback request 为 `0xffffffff`；确认前缀覆盖当前 post-tick 世界，待恢复记录及 journal 帧数为 0。第二次 proof 的 F 大于本次暂停前下界，不能误选第一次暂停的旧 tail。

Resume 只发出一次真实 Escape，保留 Pause 32→34→Stage 的过渡。三端在 F=404 实际回到 Stage 后开始计数，至结束证明均推进 254 个逻辑帧。收据 `resume.waitAfterTapMs=790.4667`；不能在按键 ACK 后立即要求已恢复 Stage。

## 确认 Replay 的实际输入

收据 `bombStress.replays` 保存六份真实导出文件、SHA-256、native 验证后的 metadata 与 pulse 帧号：

| 玩家席位 | selection / Replay loadout | buttons & 0x2 的三个连续帧 |
|---|---|---|
| 0 | 0 / character 0, shot 0 | 371、372、373 |
| 1 | 2 / character 0, shot 2 | 374、375、376 |
| 2 | 4 / character 1, shot 1 | 377、378、379 |

所有导出均验证 pulse 窗口恰有三个 Bomb 帧、连续、前后存在 release；不是只根据发送了 touch command 或收到 ACK 推断按键生效。D=0 的新场 capture 与 Replay 从 frame 0 起，不减 warmup 或 Stage 局部帧号。

- overlap 三份 Replay 均为 388 帧、14,144 字节。
- natural-end 三份分别为 661 / 658 / 658 帧、23,972 / 23,864 / 23,864 字节。
- 最终公共输入前缀 658 帧逐字节相同。metadata 中 recordedPlayer 不同，因此不要求三个完整文件 SHA-256 相同。
- metadata 均为 3P、difficulty=3、Stage 1、mode=0、D=0、P=12、challenge=false、cheat=false、completed=false。
- `bombStress.completed=true` 指本项定向覆盖完成；Replay 的 `completed=false` 正确表示没有通关。

runner 证据合同位于 `portable/multiplayer/rollback-full-game.mjs:1073–1135`：真实确认暂停门、native Replay validator、`base + (frame*players+seat)*12` 的按钮位读取及公共 payload 比较。

## 精确覆盖哪些 Bomb 分支

源码依据：`th11_web/cpp/game/BombController.cpp:47–146,178–230`。

| selection | 本场支持的具体结论 | 不能越界推断的内容 |
|---|---|---|
| 0 | 启动脚本 35/36、body 26 后保持 active；最终 confirmed inactive 且 elapsed=210，已越过 tick 180。按连续正常更新路径，涵盖 180 帧尾爆分支及随后 ANM 结束时 body/flags 恢复。 | 180 帧不是所有 Bomb 的统一结束阈值；没有逐条伤害区域、被消弹数量或可闻波形证据。 |
| 2 | 启动脚本 37、真实 text resource 所需的 13×13 distortion、ring_noise 回调所属对象的正常生命周期；最终 confirmed inactive、elapsed=260，走 ANM 消失时的 distortion 清理。 | 没有给每次 deformation 写入单独计数，也没有证明每个内部 geometry 分支都经历了错误预测撤销。 |
| 4 | 启动脚本 47，运行至 confirmed inactive、elapsed=181；正常更新进入五个子脚本 48/50/52/54/56 的处理循环，包含 t=10/20/30/40 的发声条件。 | 子脚本查找有条件门，本次 ABI 不能单独证明每个子 VM 在每个 tick 均存在、命中或消弹。 |

`inactive && elapsed>=181` 的专门硬门施加在 **selection 0 的席位**，目的是避免把 tick 179 后的 elapsed=180 误认成已执行尾爆。本场另两席的 260/181 是实际观察值，不能把同一个 181 门推广到其他角色。

未覆盖 selection 1 的 200 帧分支、selection 3 的 beam/>90 帧路径、selection 5 的 shield 命中/退款/secondary 路径。Challenge 会在 `start()` 前置拒绝 Bomb，因此本项明确使用普通模式。

两次 proof 间三端 rollback restores 分别增加 2/2/2，history ticks 增加 7/4/4；说明观察区间内有实际纠正活动。有限 ABI 与这些累计数不能证明每个 Bomb 子分支都曾被错误预测后撤销。

## SFX 与音频健康

收据 `sfxBefore / sfxAtOverlap / sfxHealth`：

| 每个浏览器实例中的计数 | Bomb 前 | 确认重叠后 | 最后同生命周期采样 |
|---|---:|---:|---:|
| SFX ID 38 成功 start | 0 | 3 | 8 |
| SFX ID 49 成功 start | 0 | 2 | 2 |
| 全部 SFX 成功 start 总数 | 211 / 210 / 212 | 三端均 235 | 三端均 513 |

ID 是 SoundEffects 的 0-based ID，计数位置为 `sfx[8+id]`。这些是每个实例汇总三席后的 API 成功数，不是“每席各 8 次”，也不是同时存活的 voice 数。

三端采样均为固定容量 56，失败 start=0、invalid voice=0；audio9 错误 latch=0，PCM 持续推进，`errors=[]`，没有本场 native/audio 错误证据。ID 38 的 3→8 与三次起始、selection 4 的四次早期声音及 selection 0 尾爆的源码路径相容；计数没有额外验证 stop49 的实际调用或可闻输出。

这证明本场确认发布能够进入真实固定声部的 start 调用，没有报告调用失败；不证明 56 声部同时满载、不证明每个事件均可闻、不证明有限 PCM、长期内存稳定或整场音频无崩溃。

## 性能仍未验收通过

来源：`performance.json`，单位 ms。

| 席位 | 最大 native callback | 最大画面提交间隔 |
|---|---:|---:|
| 0 | 5.4 | 107.3 |
| 1 | 5.8 | 70.1 |
| 2 | 8.3 | 93.3 |

本场 1× CPU 的 native 最大值较小，但 seat 0 的停显超过 100 ms 门。不能用 Bomb 行为成功抵消此失败，也不能据此宣称高刷流畅或 4× CPU 已达标。

六个清理动作成功；九条 page/context/browser 关闭均标明对应 own-close 的正常 cleanup，没有 page-crash 或验证期意外退出。该结论仅属于这次约 28 秒的定向运行，不覆盖主线或 Extra 长程。
