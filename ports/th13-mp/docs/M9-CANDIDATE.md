# TH13 MP M9 候选交接（M9-CANDIDATE）

状态：**候选整理文档，非发布判定**。整理时间 2026-11-10（P5-C 收尾轮）；依据为本轮
实际执行的命令、落盘报告与源码锚点。历史版本（2026-11-08，记录当时 M1–M9 无实现）
已被本轮实现全面取代，不复引用。

**一页结论**：M1–M9 全部落地并经固定 case 表验证（native 56、browser 35）；M10
launcher 正式接入经真实房间验收（rtc×2 / relay×2 / relay×3 全 PASS）。P5-C 轮补齐：
满窗回滚浏览器实证（`m8.rollback_full_window_raf`，maxDistance=12）、页面级浸泡
（`m9.runtime_page_soak`）、relay 路由能力链（launcher 产品缺口修复）、伤害注册池
饱和缺陷修复（2–3 人长程对局可崩）。遗留缺口见 §4（如实登记）。

## 1 本轮（2026-11-10）新增

| 项 | 内容 | 验证 |
| --- | --- | --- |
| SP 符卡练习返回修复 + MP 同步 | 按 `00456650` case4 / `004560a0` case0 原版序列重建 187；对局返回级联（DAT_004bb4c4/c8/cc 语义，reason=5 直入难度层） | SP/MP `tests/ui/run.sh` 各 532 ok（新 view 套件 4 case，红灯先行）；生产页截帧像素差分 0.9%（after-esc vs stage-list） |
| relay 路由能力链 | relay lobby 快照 `gameplayRelayFallback` 通告 → launcher 归一化 → runtime options `allowWebsocketFallback`；修复真实房间 relay 路由 `transport disconnected` 必挂 | relay×3/relay×2/rtc×2 真实 launcher 房间全 PASS（`room-p5cr-*`） |
| 伤害注册池饱和修复 | `reserve_damage_registration` 满池改环形逐出（原版满池为越界写；移植曾为硬错误，2–3 人恒射 soak frame≈337 必崩）；SP/MP 同一份（M0 字节等价保持） | soak 2p 全维度通过；native 56 / combat / e2e 无回归 |
| `m8.rollback_full_window_raf` | 正式页 rAF 自然泵 + 350ms 真时 wireDelay（帧率换算满窗）；通道自检 `th13-input`；两侧 max 断言 | PASS（37 断言；seat1 maxDistance=12 restores 34、seat0 恒输入席零回滚为合法结果） |
| `m9.runtime_page_soak` | 2/3 人各 5 分钟、50ms±20ms+2% 丢包；零 digest 失配、帧号差 ≤4、无停死硬断言 | 机制全通；fps≥58 阈值不降、容器 swiftshader 实测 5.9–6.0 记 BLOCKED（`TH13_SOAK_REQUIRE_FPS_58=1` 环境启用断言） |

## 2 实现面（当前真实状态）

- 模拟/呈现/副作用拆分（M1）：`Application` 帧体三段、副作用账本；
  `web/cpp/multiplayer/`（Session/NetplayRuntime/WorldState/SessionSetup/
  DigestWireFilter 等）。
- 一套世界多席（M2）＋目标/碰撞/共享资源（M3）＋全队灵界（M4，RULES v2）
  ＋表现/幽灵/生命周期（M5）：`StageBattle` 席位域、所有权语义；
  native case `m2.*–m5.*`。
- 结构化快照（M6）：WorldState capture/restore/digest（覆盖缺口见
  [../ADAPTATION.md](../ADAPTATION.md) 已知缺口 2/3）。
- 确认输入联机（M7）：`common/` BrowserPeerTransport（RTC mesh vs relay 竞速、
  frame0 屏障）、SessionChannel；relay 服务 `services/relay/src/netplay-relay.mjs`。
- 12 帧回滚（M8）：RollbackCore 预测/纠正、P4 环槽 checkpoint、音频恰播一次
  （`sfx_suppressed`/`music_deferred` 观测面）。
- Replay/旁观/生命周期（M9）：ReplayArchive、SpectatorJournal、断线确认保存。
- launcher 正式接入（M10）：`th13mp` catalog/builds、`multiplayer.mjs` 生产桥、
  `MultiplayerExports.cpp`、relay 能力链；真实房间开局进 battle。

## 3 验证入口与实测（本轮命令）

```sh
# native 56（全部通过）
bash tests/multiplayer/run-native.sh --through M9 --scope cumulative --source-digest <d> --report <p>
# browser 35（含两个新 case；soak 5 分钟档）
node tests/multiplayer/acceptance.mjs --through M9 --source-digest <d> --report <p>
# 真实 launcher 房间（rtc/relay × 2/3 人）
node tools/ports/room-browser.mjs --url=<launcher> --route=relay --players=3 --output=<dir> --game=th13
# 单席回归
sh tests/combat/run.sh && bash tests/e2e/run.sh
```

外层 gate：`scripts/mp_workflow_gate.py --stage M9 --tier full|final`
（`protect_sp` 基线随 SP 修复提交前移后复跑；`final` 需同源 full 收据）。

本轮实测（日志与报告：`/artifacts/mp-round-20261110/logs/p5c-regress-*`）：
native 56/56 PASS、combat ALL PASS、e2e 2/2、room 三轮 PASS、
`m8.rollback_full_window_raf` PASS；browser 35 全量结果见 P5C-report §5。

## 4 遗留缺口（如实登记，不虚报）

1. soak fps≥58：无硬件渲染容器物理不可达（实测 5.9–6.0）；阈值不降，
   具备硬件渲染的环境以 `TH13_SOAK_REQUIRE_FPS_58=1` 启用该断言。
2. M6 快照覆盖空白与 digest 覆盖不全（ADAPTATION 缺口 2/3，原样有效）。
3. 断线重连未实现（断线语义=保存确认历史后结束，ADAPTATION 缺口 5）。
4. 场景覆盖矩阵难度维度未覆盖（录像资产仅关 1..3）。
5. SP `curve_laser_presentation_test` 2 case 因 fixture
   （`artifacts/verification/curve-production-20261027`）在 2026-11-08 清理轮后
   缺失而红（预存、与 MP 无关、测试文件未改动）。
6. 音频恰播一次的感知残余（ADAPTATION 缺口 7 原文有效）。

## 5 提交分组（待授权执行）

①`fix(th13)` PR#159 转场/UNORM 残留＋符卡练习修复＋池逐出（SP）
②`feat(netplay)` relay 能力链（services/relay + launcher 归一化/组装）
③`feat(th13-mp)` M1–M9 主体（multiplayer 目录、tests、桥、导出、构建）
④P1–P4 回滚优化 ⑤P5 正式页/catalog/房间 ⑥外层 gate/文档。
