# TH128 thprac：源码与实际消费者矩阵

状态：**未完成适配，第三阶段完成结论已撤回**。依据用户2026-11-17直接纠正，使用TH128专用thprac模块，七项简易选关仅保留为临时工具。源码只读固定commit与完整SHA见THPRAC_CORRECTION；本表不把字段归一化或UI渲染冒实际游戏效果。

| 上游功能与真实位置（thprac_th128.cpp） | 当前TH128实际消费者/缺口 | 后续验收 |
|---|---|---|
| THPracParam/JSON（13–77）；mode/stage/section/phase/dlg及5个资源字段 | 无对应thprac会话codec，现debugJump只含有限选关；eagler固定版本launcher thprac.mjs尚无th128 schema | 原字段/上下界/舍入、开关/语言、不可用字段拒绝、会话与Replay元数据往返 |
| start/path/stage映射（354–362） | 上游0-based16stage；现prepareStage使用1-based1..16；不能套TH12的stage语义 | A/B/C双分支/各三面、EX及不同难度，真正菜单→正确World/ECL |
| score0..9999999990并向下10舍入（246、383），写原全局score/10（2243） | 当前scoreRaw内部为显示值/10，无独立合法thprac初始分数接口 | fresh owner配置、边界与全字段读取、不伪nativecapture/default |
| motivation默认100000、范围0..100000（85、384、2244） | world_enable_shot_hits(motivation,...)有fresh配置消费者，可复用但必须纳入统一初始参数收据 | actualplayercontact/shotquery读取，不仅setter返回 |
| perfect_freeze默认30000、范围0..30000（87、385、2245） | world_enable_player_context(...perfectCharge)消费原charge，有fresh guard | 0/上限/中间值及X实际生命周期、taint/重开不串旧owner |
| ice_power默认10000、范围0..10000（91、386、2246） | world_enable_graze(count,power,...)的grazeGlobals.powerRaw为已有消费者 | 资源/贴图和真实HUD/玩法字段一致，非同名JSON |
| ice_area默认3000、范围0..3000（89、387、2247） | world_enable_player_shots(...cumulativeArea,difficulty)及playerShotGroup选择可复用，EX另有初始area0原生guard | 需明确thprac与自然EX守卫区分；不得绕正常资格/修改既有成功路径 |
| section按chapter/mid/end/nonspell/spell选择（260–351）；chapter126项（397–403） | 当前原ECL消费者是Program::load严格SCPT/ECLH解析，Vm以指令vector执行；无thprac补丁适配层 | 见864原DAT布局核对；先实现指令级语义覆盖组件，非原始内存覆盖 |
| Boss/多Boss/EX符卡与phase（657–735、1227–2051）；EX_END_S10四phase | 多文件SetFile(0/2)与分支/HP/位移原文存在；现缺thprac section adapter，其他一般ECL逻辑已实现 | 逐实section与phase覆盖，包含对话/音乐/intro规则；不能仅跳到Boss名称 |
| THBGMTest/disableLogo（2067–2079、2253–2264） | 有StageMusic/native sourcefade/stop严格ACK；thprac section BGM/intro行为未接 | 按section/dialogue实际分流、重开保持/退出释放，不改volume95 |
| THGuiRep/ReplaySaveParam/锁（406–450、2080、2266–2310） | 原RPYcodec/25槽已验，但没有THPracParam辅助元数据或playLock/thHardLock策略 | 练习RPY+原参数绑定、正确起点/重播隔离/存储完整回读；不JSON改名RPY |
| F1..F7overlay（454–517）、F12/高级速度（520–645） | 当前无限复活setter不是原无敌/无限资源/时停/自动Bomb的完整等价实现 | TH128各toggle实际效果+禁用恢复；共有speed/key/nav模块适配；Windows vpatch不执行 |
| Eagler共用input/navigation/statistics/mobile/locale | common/docs/thprac.md声明共享工具与作品特定生命周期分离；当前JS宿主没有C++ thprac GUI | 实际菜单/键盘/鼠标owner/locale与核心效果；手机额外兼容单列但基础入口不得假ACK |

完整上游模块2375行已按段读完。TH128模式/label/section枚举与多语言表仍需从上游games定义固定版本取得；TH128专用文件已证明存在，不再猜测“不支持TH128”。上游eagler thprac helper当前未列th128，是要新增实际适配，不是照抄其他游戏默认schema。

864只读新目的：真实32336111B DAT→16个原始stage ECL，按固定上游THStageWarp解析126个chapter选择/143条ECLJumpEx写入。所有源写入起点和目标均为原始指令边界。16个chapter1不写补丁，其余110个raw-copy补丁在本工程整段SCPT/ECLH解析边界检查中失败。例：A1-1源offset0x81d8原opcode22/size40；上游24B跳转覆盖后offset0x81f0留下原参数尾部，严格parser误读length29551。其他起点原指令为20/32/40字节；不能简单保原size，因为Vm::shape要求opcode12恰2个rawWords+2params。

这是 **移植结构差异核对**，不是原thprac错误，也不是新实现已通过。读取与模拟覆盖都在ownedcopies，DAT/source/公开页未改；没有实际native Program/Vm执行、World/Factory/compile/browser/GPU。864 inventory1PASS只证明上述读取/边界记录完成，不能替代新参数/跳转实际RED→GREEN。

下一源支持选择：为Program的不可变解析结果增加受控TH128练习指令覆盖，替换指定fileOffset对应Instruction的time/opcode/count/rawWords，保留原subroutine边界与其他指令和原资产字节；Vm的原relative target/atFrame语义仍使用原offset。此为拟议方案，尚未编写或验收；必须先组件+实际Vm消费者RED，再最小实现和原ECL差分检查。之后接fresh参数组/菜单/Replay与Web caller，不因一块通过再宣称全thprac完成。
