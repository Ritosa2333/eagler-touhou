# th13-mp 目录来源登记（仓内派生）

## 来源

- 派生方式：**仓内目录派生**（同 th075 先例，目录级提交历史；无外部源仓库）。
  2026-11-11 起补建标准子树连接：split `8a45a9f33972c89217368c9ca1c7e71209ea07fb`
  → join `24c6c26c53b7d2ad090e872de00ab0ebc01df669`，机器可读登记见
  [th13mp-eagler-20261108.json](th13mp-eagler-20261108.json)，目录内容零变化。
  来源校验以已验证的最新 split `156002536dc6a851e941a7fa189e77cd16045020` 为锚，
  用 `git subtree split --prefix=ports/th13-mp --ignore-joins --onto=156002536dc6a851e941a7fa189e77cd16045020` 导出。
  这样能保留原有普通版与多人版父链，同时跳过旧提交正文里失效的 split 引用。
- 来源目录：`ports/th13` @ monorepo 提交 `e1111b4daeb21296a5d0da526e2d512efc449aa2`
  （分支 `pr159-update` 尖端，2026-11-08）。
- 派生内容：SP tracked 树全量（web/portable/tests/docs/tools/container 与 meta
  文件），剔除 `archive/`；`web/sdl-runtime/assets` 以符号链接共享 SP 的资产
  目录（同一份游戏数据，不复制）；ignored 构建产物不携带。
- 派生提交：见 `git log ports/th13-mp` 首个提交。

## 同步与维护

- 同步对登记：`docs/adaptation/port-sync.json` 的 `th13` 条目
  （base = 上述派生基线）。SP→MP 接收流程与命令见
  [port-sync.md](../adaptation/port-sync.md)；MP 专属实现沿自身历史保留。
- 通用修复先落 SP 再 sync 进 MP；`portable/sdl` 共享后端与 th15 的漂移红线
  同样适用于本目录。
- MP 规则真值：[ports/th13-mp/docs/RULES.md](../../ports/th13-mp/docs/RULES.md)；
  阶段工单：[ports/th13-mp/docs/MP-DEVELOPMENT.md](../../ports/th13-mp/docs/MP-DEVELOPMENT.md)。
- 2026-10-28 登记中「本次不建立 MP 目录」的边界自此由本登记更新：MP 目录为
  仓内派生，SP 目录继续独立维护。

## 产品身份（M10 落地时补全）

- 产品目录条目、多人 Runtime 入口、存储根 `/savesth13mp`、Replay 前缀
  `th13mp_XX.rpy` 见 RULES/MP-DEVELOPMENT 的接口约定节；未注册前 Launcher
  不可见（th13 本体 `testOnly`，MP 同样不进默认产品集）。
