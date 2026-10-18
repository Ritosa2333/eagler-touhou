# 七作旁观生命周期修复（2026-10-18）

本轮针对 TH06/07/08/09/10/11/15 检查重复进入、长暂停、定位进度、停止定位、
历史播放与直播后的速度，以及观众快捷聊天。TH06/07 仍在各自 `eagler` 分支，
没有新建 `eagler-mp`。本轮只提交本地修复，不推送或部署。

## 行为与原因

- Relay 退出旁观席时同时释放同身份在各 run 的 admission、claim 和 receiver，
  关闭旧 receiver。迟到的旧 socket close 不能清掉新连接。同一局可再次申请旁观。
- 首次获准旁观后若缺 DATA，显式导入/资源重试继续原启动。续接意图绑定 owner、
  run 和旁观身份，离开或已成功启动后失效。单纯收到房间状态不会自动启动。
- 合作播放器初次在加载罩内有界恢复至固定有效节点。只有认证的当前 Runtime
  报告恢复完成才撤罩。之后普通历史播放、被动跟播均保持正常输入预算。
- 多输入预算只用于隐藏初始化和显式定位/直播。Live 固定请求时目标，到达后关闭
  加速，后续输入积压不能再次触发。各作原有 60Hz 调度仍由各自时钟负责。
- 定位显示实际 native position，target 单独保存。合作播放器在定位中 Pause
  立即停在实际游标并取消本次重演；未安装的首个 checkpoint 仍需完整安装。
  TH09 保留其暂停并保留固定目标的原生语义，修正可见进度。
- 15 秒无输入检查只用于首次有效 bootstrap 前。初始化后，健康 transport 没有新
  游戏帧允许持续等待；真实 socket 错误/关闭、非法包和玩家同步保护继续有效。
- 七作观众快捷聊天使用现有房间身份与 09 展示路径。当前旁观名单决定权限，
  观众文字不触发玩家语音，玩家发言保留原语音。
- TH08 原子采样证明 checkpoint0 完全一致，但消费输入0后玩家/效果组偏离。
  源端提交玩家输入前剥离 route bootstrap，观众端原先直接提交带路线信息的输入。
  修复让观众仅对 frame0 的 P1 使用相同剥离规则，保留原始 wire/journal。
- 开发 HTTP 服务从实际 workspace root 解析源码 Runtime URL，支持嵌套 main 树。

## 验证范围

原始收据统一保存在工作区 `dist/audits/spectator-lifecycle-20261017/`，
实际原生报告路径也记录在该目录的最终报告。目录名保留任务开始日。

| 层 | 本轮结果 | 边界 |
| --- | --- | --- |
| common | native C++ codec/playback/controls 检查通过 | 固定目标、取消定位、首次 checkpoint 安装、持续新输入下被动 1x |
| 公共前端/Relay | 当前共享工作区专项合并 122/122、七作挂载矩阵 7/7、类型检查与正式构建通过 | 挂载矩阵每作三次入口，使用实际 React/session owner 与受控 native transport |
| HTTP | 实际源码服务 root 回归和 HTTP contract 通过 | development-server 全文件未完成，既有 before-hook 新构建在 120 秒上限超时；独立正式构建已通过 |
| TH06 | 原生专项通过 | 同身份重复入场、17 秒本地暂停、18 秒测试停源、定位取消、正常历史和直播后速度、Boss 节点入场 |
| TH07 | 原生专项通过 | 同上；本轮仅两人模式，不替代旧轮三人/换面证据 |
| TH08 | 原生专项通过 | 生产包、同一 Host/iframe 三次入场（epoch 1/2/3），每次 CP0..4 全部 13 组状态等于两源；正常跟播与停止定位、16.2 秒健康停源和 16.2 秒本地暂停 |
| TH09 | 实际前端及原生专项通过 | 产品导入/资源续接、观众文字与玩家语音、16 秒本地暂停、实际定位进度与暂停、固定 Live 目标后约 66.25fps、同房间/run 两次 fresh Runtime 重入、最后 16 秒健康源静默 |
| TH10 | 原生专项通过 | 同身份两次重新构造 native、17 秒本地暂停并继续接收、17 秒停源、定位取消、普通及直播后每次正常 tick ≤1；真实 Relay |
| TH11 | 原生专项通过 | 同 client/member/run 两次 fresh epoch 重入、17.05 秒本地暂停和测试停源、定位取消、历史及直播后 1x；诊断 MP 包 |
| TH15 | 原生专项通过 | 同 identity/run 两次 fresh epoch 重入、17.05 秒本地暂停与测试停源、定位取消、历史及直播后 1x；诊断壳保持 completeGame=false |

公共回归在当前共享工作区执行，保留其他任务的未提交差异；提交仅选择旁观
修复，不把这些数字表述为独立干净 HEAD 的全套检查。

本轮原生专项以两人、本机私有 Relay 为主。测试停源指停止 native 调度并保留
接收连接，不等于按游戏暂停键，也不证明公网或实体手机表现。06/07 测试停源后
没有宣称玩家恢复；09 停止源循环及轮询后先采样稳定边界，观众仍可正常播放已有历史，接收前沿保持不变。原始失败（构建内存/上限、旧脚本 selector/动态 edge/生命周期
假设、直接 RTC 同步超时）保留，不与最终通过收据混淆。

## 依赖与构建身份

六个合作 adapter 均逐字使用同一公共 Playback header，SHA-256：
`016f51cda3546229e3b21ac943ca83bfa8580e2c191925a02d0d959a309a1e3b`。
公共视觉模块继续使用既有唯一实现，未新增玩家端每帧面板循环。

各 vendor 保留各自已测祖先，仅提交本轮 header backport；不将 canonical common
最新历史整棵替换进去。最终 common 与各 vendor 提交、游戏提交、前端提交分别
写入审计 commit 映射。编译包保留编译时 HEAD/dirty 身份；提交不改变已验证源码
blob，不能把旧编译元信息冒称为新 HEAD 重编。

| 作品 | 冻结 WASM SHA-256 |
| --- | --- |
| TH06 | 2aa9cacaab8857fe30c013d9fb015129107b6d459c9293fb90c2917936a71216 |
| TH07 | 01e355d12b0318c23005d372877c916b8829e7cc2a7bf66749c5f8990b5d9ba0 |
| TH08 | 4ba490d6fccee122afe7f76c4934737478805dc046c31e35b51f49e922bd01fc |
| TH09 | 96b2d9b67cbe3490e941c6d8970bc4eae849d59363f0d9c3894cae0640fb1e30（本轮 WASM 未变，更新 JS） |
| TH10 | 2fed9e05e407b5869752917a73fa5b9d7d5542e049feb7cdee752c8b80145bb0 |
| TH11 | 12d9a0deced04b68ec30fe2ca735feb9bc46a5a07e2b52fa9a5adedcf1a2cb5f |
| TH15 | 7b7e5e4e6e29f2628aed9ff7d3335b13269b94d6da935c824fb8d74b2b7d48f6 |

TH11/15 使用诊断入口；TH15 最终通过后仅加固测试清理的幂等性，未改原生源码，
该清理改动只做语法检查，未声称再次执行浏览器。

TH15 的旧 SPECTATOR-DEPENDENCY 描述保留历史导出来源；目前在已恢复的真实
MP 子模块上记录本轮 overlay 和 gitlink，不据此冒称过去已存在可发布依赖。

## 本轮本地提交

Canonical common：`0b236bfccf2a1eb9d4a9e227ad049abfb4e68345`。六份 backport 的完整祖先和提交
写入 `dist/audits/spectator-lifecycle-20261017/commits-20261018.json`。

| 作品 | 游戏提交 | common gitlink |
| --- | --- | --- |
| TH06 | `b76c49719e09` | `09e98493d816` |
| TH07 | `73c85f9f2e39` | `89e59e6a207c` |
| TH08 | `ed28bff70dd7` | `44d0732e0f35` |
| TH09 | `9d3cc8b03ad7` | `既有 JS transport，未改 pin` |
| TH10 | `806b77fa2cb1` | `996dbd06d736` |
| TH11 | `79936b181054` | `61009a19c721` |
| TH15 | `9288b48eef5a` | `49079568c549` |

Launcher 提交包含本文；其最终 SHA 同步记录在上述审计映射。TH06/07 仍为 `eagler`，
TH08/10/11/15 仍为各自 `eagler-mp`，没有新增工作树或合并普通版祖先。
