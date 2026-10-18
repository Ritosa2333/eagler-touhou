# TH11MP 帧时钟与网络等待合同

日期：2026-10-13。工作树：D:/workspace/eagler/worktrees/th11-rollback-smoothness。

## 状态与改动范围

新增 th11_web/cpp/multiplayer/FrameSchedule.hpp、portable/multiplayer/frame-schedule.cpp、frame-schedule.mjs。root 已串行执行 frame-schedule-component-01：8 组 / 24429 checks 通过，收据 passed=true、sourceStable=true，已通过 nig 读取。Application 接线由 root 独立完成，未由本计数门禁代证。

没有改公共 PendingFrameSchedule、FrameBudget 或 TH11 原 FrameCadence。默认 mode 1 与普通版仍使用原时钟；新 wrapper 仅供新的预测回滚路径接入。它不读取输入、不调用 Update/Draw、不拥有回滚世界，不调整 fixed dt，也不更改 canonical hash。

实施前重读跨作架构 §4.4–4.6，以及 TH10 NetworkFrameSchedule 的实际代码。TH10 已证明：把 CPU 恢复一律映射成网络 pending，会同时丢失额外 offered ticks 与恢复期间新增 elapsed；计数测试通过又不能证明正式 4× RTC 流畅。本组件只解决明确的时间所有权问题。

## 复用与有界规则

底层是 Netplay::PendingFrameSchedule<touhou::sdl::FrameCadence>。保持 TH11 原数值：

- nominal interval 为 1/60 秒。
- 一次 advance 最多提供 4 个新 forward 义务。
- 保留的 nominal clock debt 最多 100 ms；这相当于最多 6 个逻辑 tick 的总时钟信用，仍需分 callback 按最多 4 个提供。
- common FrameBudget 提供 8 ms 的追加工作准入，wrapper 再限制到 TH11 的 4 个。没有采用 common 的 6 tick callback cap，也没有搬入 TH10 的 12 ms 实验尾部阈值。

8 ms 是开始追加原子工作前的准入，不是硬中断或 callback 最长耗时保证。第一笔已到期 forward 总应获得一次尝试；历史恢复不计为本 callback 已完成的新 forward。

归还 CPU debt 时不重新乘网络 pace。调用者只把新 elapsed 归一化一次再传 advance；旧 phase 和之前归还的 nominal credit 不随 pace 再缩放。

## API

类名 th11::multiplayer::FrameSchedule。

| API | 合同 |
| --- | --- |
| advance(normalized_seconds) | 提供当前 callback 的到期义务并记录 offered 数；返回最多 4。已有网络 retry 时只返回 1，忽略本次 elapsed |
| complete() | 仅在真正新增的完整 forward 事务完成后消耗 1 个 offered，返回是否成功；历史 resim 不能调用 |
| network_wait() | 仅当当前确实有 offered 且缺少网络信息时调用；保留 1 retry，丢弃其他整数债，保存小数 phase，并结清 offered |
| cpu_yield() | 恢复未完成或 CPU 追加预算耗尽时调用；清 retry，将本 callback 所有未完成 offered 归还一次，受 100 ms cap 约束，随后 offered 清零 |
| reset() | 清除 offered、retry 和全部 phase，供生命周期或模式身份切换 |
| retry_pending() | 是否由真实网络等待持有单个 retry |
| owed() | 当前 callback 尚未完成的 offered 数；不包含 phase 中尚未提供的整数信用，也不包含已结清的网络 retry |
| phase() | 尚未提供的 nominal 秒数，可以包含已归还的整数 CPU debt，不能直接当作 0–1 插值 alpha |
| can_start(completed_forward, elapsed_ns) | 本 callback 新 forward 的追加准入；first forward 允许一次尝试，其后必须小于 8 ms 且总数小于 4 |

重复 cpu_yield 不会再次退款。network_wait 之后尾部无条件 cpu_yield 也不会把网络 pending 加回一遍，因为 offered 已清零。complete 在没有 offered 时返回 false，不制造新帧。

advance 在发现上个 callback 未结清的 offered 时有防御处理：已处于 network retry 则保持网络等待；否则把剩余 offered 作为 CPU 义务归还。该防御不能替代 driver 明确分类原因，尤其不能通过反复调用 advance 取代正常 callback 结束结算。

## driver 接线要求

每个 callback 只计算一次归一化的新 elapsed。advance 后尝试到期的新 forward。一次尝试可能先完成多个历史事务；这些事务不调用 complete，也不减少 owed。

如果恢复仍在 Correcting，调用 cpu_yield 后让出浏览器。不能因为没有新 forward 就调用 network_wait。若恢复在本 callback 完成，应继续尝试原本欠着的 forward；历史工作消耗 8 ms 并不消灭第一笔新 forward 的义务。是否仍需等待实际输入由 driver 的明确结果决定。

真正 NetworkWait 才调用 network_wait。CPU gate 拒绝开始下一笔或普通 CPU defer 才调用 cpu_yield。全部 forward 完成后不 reset，不清 phase；否则会产生“每次稍慢就人工减速”。

网络等待转 CPU 恢复的例子：等待前 phase 为 0.25 tick，网络 pending 为 1；即使中间等待了 100 秒，下一次 advance 仍只提供 1。该 callback 因纠正而 CPU yield 后仅保留 1.25 tick。再经过新 0.75 tick 时间时提供 2，而不是补偿此前 100 秒。

调用者仍须保证完整恢复、typed owner 恢复和 semantic Draw 封帧后才呈现；这个时钟不授权显示半恢复世界，也不负责 input capture/packet batching。mode 1、回放 seek、未 Ready、reset/session 切换必须在 Application 中明确分支，不能凭同一个 !advanced 结果推断等待原因。

## 独立确定性验证

runner 编译真实新 header、TH11 原 FrameCadence、common PendingFrameSchedule 和 FrameBudget，使用 WASI32、-O2、TH11_DEVELOPMENT_HARNESS=0。没有浏览器、SDL、网络、真实 world、计时性能或零售数据依赖。

8 组 oracle：

1. 60/90/120/144/165/240 Hz 下与真实原 FrameCadence 逐 callback 比较，完整消费时五秒恰好 300 ticks；另覆盖不规则短停顿。
2. 多 offered 中一部分完成后 CPU 归还，重复归还无效，保留小数 phase；历史工作不消费新 forward。
3. 网络等待中 0、短间隔、长间隔都只重试 1，不累计新债；恢复输入的 callback 也不补回旧等待时间。
4. network → CPU 和 CPU → network 转换，分别维持不同债务合同。
5. 极长 CPU 停顿保持 100 ms 总信用，按 4+2 批次排空；遗漏 callback 结算的防御、不合法 elapsed。
6. reset 清除未完成 offer、network retry 与 CPU debt。
7. 8 ms 准入边界、4 tick 上限，以及历史完成后原第一笔 forward 的准入。
8. 高刷周期性 CPU yield 下的纯逻辑计数，排空剩余有界债后仍为五秒 300 ticks。

这不是恢复吞吐、真实 rAF 延迟或 4× CPU 流畅度测试。driver 是否按正确结果调用这些 API，需要 host 接线后的独立验证。

## 执行入口与证据

root 在工作树根目录串行执行：

    node portable/multiplayer/frame-schedule.mjs --output artifacts/rollback-smoothness/frame-schedule-component-01

源码 manifest 精确包含新 header、test、runner、原 FrameCadence、公共 PendingFrameSchedule/FrameBudget 六个文件，不包含 Application、game owners、公共 journal 或字体。runner 保存源码 hash 与稳定性、编译器 hash/version、git identity、命令日志、WASM 身份和完成 marker。已有 output receipt 一律不覆盖。

实际收据：artifacts/rollback-smoothness/frame-schedule-component-01/receipt.json。命令 ID em6UV9VYNaCrxp0PtGOSE8G1，exit 0；UTC 2026-10-13T08:17:14.427Z 至 2026-10-13T08:17:15.824Z。

| 身份 | 结果 |
| --- | --- |
| sourceDigest | b9eb51ee869b02e9b4e936eb14664bff1dfa9bbdb3aa3d367e0f2dbff82e1341 |
| WASM SHA-256 | dd933b3d80bdf9ed58608c26c8f35f11eb1772c569edd36f2c517ec9e0382964 |
| WASM 大小 | 130980 bytes |
| oracle | 8 组 / 24429 checks |
| 完整性 | passed=true、sourceStable=true |
