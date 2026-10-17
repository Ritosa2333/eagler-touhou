# 2026-10-16 旁观协议梳理与下放准备

## 范围

对照 Launcher main 工作树 `8b20f38`、TH09 eagler `80aa50e`、公共传输与 codec。
工作树已有输入、音频、安全等未提交改动，本轮在保留它们的基础上补协议。
初始阶段新增行为约束和迁移清单。后续按用户指示开始公共层下放，进展见本文
末节；仍没有启用六个合作作品的原生中途旁观。

## 交付

- [旁观协议](../SPECTATOR_PROTOCOL.md)：现行两类 wire 的布局、身份与准入、
  三类时间、完整初始化、历史/直播交接、本地回看、尾部排空、故障与容量边界。
- 同步适配指南、架构、产品说明与多人主手册，去掉 TH09 不支持旁观、
  时间线只能前进、所有作品只能开局前加入的过时概括。
- `REQUIRED_MULTIPLAYER_BEHAVIORS` 新增确认历史、故障隔离、尾部生命周期
  三项义务。保留 TH09-only Host 命令和现有准入范围，未仅靠改登记表宣称下放。
- 修复 TH09 `spectator-output.mjs` 的测试夹具：公共 STOP 已有 `graceful`
  参数，旧夹具没有声明/传入，导致故障路径的信令调用在 catch 中被忽略。
  现在明确传 0 测试故障停流，没有修改 Runtime 的停流行为。

## 主要结论与待完成条件

1. Relay 只保留当前局的完整前缀，旧观众在本地保留自己收到的旧局。
   新连接不补以前局，跨局回看不能扩大服务器 bootstrap 历史。
2. T9BG/2 含 RNG、菜单输入边沿和完整局参数，Retry 也能独立造 world。
   v1 仅保留旧的前进兼容。合作作品需要自己的完整重建证明，不能照搬 TH09 ABI。
3. 回直播使用固定目标。房间更新、run 变更、STOP 和 EOF 都不等同于逻辑结束。
   旁观侧应尊重暂停并排空已接收的完整尾部，玩家传输不能被旁观失败关闭。
4. TH09 已消费的本地档案没有总字节上限，未读限额不能证明总内存有界。
   全作品下放前需确定总档案预算与用户可见的容量处理策略。
5. main 工作区的八位新房码候选与 TH09 四位码校验冲突，需独立解决组合兼容。
   源头为 `fix/netplay-security-20261016` 的 `f83de94`，不是本轮修改。
6. Relay 与各 Runtime 的 timing codec/有效值集合、当前 TH09 native 导出
   能力探测、TH09-only room-state/目录/前端分支都需要随迁移一起核对。

## 本轮验证

| 检查 | 结果与证据边界 |
| --- | --- |
| Launcher client services 编译 | PASS，72 个 TypeScript 模块。没有构建或发布游戏 WASM |
| main：spectator frame、history relay、STOP relay、timeline relay | 4 个脚本 PASS。真实隔离 WebSocket Relay，覆盖中途准入、当前局 bootstrap、批量/直播交接、换局/换 run、停流后玩家继续 |
| TH09：tape / rewind / live / room-state | 15 项 PASS。包括跨局换角色初始化、倒退时继续接收、暂停尾部与旧 run 隔离 |
| TH09：spectator-output | 修复夹具后 PASS。覆盖 RTC/Relay 故障隔离、背压、32 包 / 2 ms、时序先于输入和观众超时 |
| TH09：spectator-playback / spectator-schedule | 本机 g++ 编译并执行 PASS。策略级验证，不代表完整游戏或设备性能 |
| common：browser-peer-spectator-tail | PASS。graceful STOP 同流排序、默认故障信令、EOF 后 257 个已收包排空 |
| runtime-protocol model、product catalog、multiplayer runtime options | 3 个脚本 PASS |
| adapter:inspect th09 | 新增三项 spectator 义务与更新后的 timeline 配置语义可见 |
| adapter-capabilities 总门禁 | FAIL：`pixelPerfectEnabled` 缺分类。用改动前快照核对，baseline 与 candidate 缺项相同，不归于本轮旁观修改 |
| runtime-protocol 源码总门禁 | BLOCKED：默认及显式 workspace root 都仍按旧映射查找 `th06/resources/shell.html`；现行文件在 `th06-eagler`。未修改路径配置或放宽门禁 |

未新增真实前端完整对局、实体移动设备或公网证据。协议中列出的 browser runner
是后续逐作激活门禁，不因本轮模型/Relay PASS 自动获得全作品验收结论。

## 后续澄清：换面与 Boss 节点

用户要求调查所有非对战作品按换面和 Boss 分段的可行性，排除 TH09。
新增 [逐作调查](../SPECTATOR_NODES.md)，覆盖 TH06/07/08/10/11/15/20。
实际读取各作的分面 Replay 捕获/恢复、Boss slot 注册与当前 MP 启动/接收路径。

- 每作都有可采集的原生事件入口，面首恢复具备现有 Replay 基础。
- 推荐先实现面首完整初始化 + 本面输入 + Boss 定位标记。入场播放位置与后台
  重建起点分离；没有 Boss 快照时仍保留完整道中输入供重演。
- TH07 多 Boss/Phantasm 槽、TH08 路线/多人起点重演、TH10 全席位 Option、
  TH11 跨面背景/多人经济、TH15 带惩罚的章节 retry 都需要专门验证。
- TH20 当前没有 MP；核对实际 ReplayStage 与 ECL Boss owner，仅记录未来接缝。
- 本次为源码可行性调查和协议补充，没有新增阶段恢复、Boss checkpoint 或
  全作品中途旁观的运行证据。

## 开始下放与大厅补充

用户随后要求先做换面恢复/Boss 定位，并指出满员但未开局的房间无法从大厅
进入旁观。本次在既有 main 工作树继续修改，未创建工作树，09 原生代码不变。

- common 新增 E?SV/1 codec、确认发布 journal、有界分段历史、固定目标播放
  策略和 stage-history 传输入口。journal 在 resimulation 替换未确认节点，
  背压重试不重复 checkpoint/输入/Boss，显式 end 必须等于已发布前沿。
- Relay 识别六个合作 marker，仅完整有效 checkpoint 激活中途准入。Boss
  保留道中输入；首次下载中换面保持连续；下载完成的 bootstrap 标记固定入场
  目标。失效流只终止观众，玩家传输继续。
- 前端七个 MP 的满员待开局房间允许目录旁观，链接携带显式观众身份，
  不自动占玩家席。进行中合作房间仍要求实际 spectatable。
- 公共合作播放面板已接 runtime-info 状态与 spectator-playback 命令，
  包括换面/Boss、暂停继续、拖动、回直播、run/epoch 隔离。没有 native ready
  状态时面板不显示。TH09 继续使用原有面板。

实施边界：**六作的标量初始化器、原生 Boss journal、实际恢复/控制桥尚未
接入，未构建新的六作 Runtime，也未更新游戏的 common pin。不能把公共层
和前端 PASS 写成全作品恢复完成。** 真实 Stage2+ canonical state 比对与完整
入场路径仍是后续门禁。TH10 本地有 eagler-mp 分支而无检出 MP 工作树，当前
普通版存在未提交改动，用户尚未授权新开工作树。

已执行候选检查：前端模型/挂载/房间服务 110 项、六产品真实隔离 Relay（含
3072 帧 bootstrap 下载中换面）、codec 2P/3P、common native 策略与传输检查。
源码和日志记录于工作区 `dist/audits/spectator-rollout-20261016`。
这些是策略/协议/合成 DOM 和隔离 WebSocket 证据，不是各作原生世界或公网验收。
