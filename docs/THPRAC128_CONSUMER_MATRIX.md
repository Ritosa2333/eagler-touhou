# TH128 thprac：源码与实际消费者矩阵

2026-11-17最终验收：本次TH128网页版thprac功能范围已完成，计100%；完整正式页/原资源/116/真实WebGL及文字纹理、原生标题路线、游戏内Backspace与F12、同练习重开实际11项通过，1008最终只读复核5项通过。内部测试栏未进入正式页面。旧“未完成”段落为迁移过程历史；本结论不包含多人、打包、公开发布或所有硬件逐像素一致。详见[THPRAC128_ACCEPTANCE.md](THPRAC128_ACCEPTANCE.md)。

2026-11-17设备补充：996实际WebAudio原WAV/OGG解码与输出节点、同练习重开保音源、原生MSG严格ACK、真实ended及退出清理10项通过；最终F12用整页截图按文档坐标离线裁图，已核不含内部测试栏。仍无实际游戏GPU/字体纹理合并验收，不冒完整thprac/第三阶段100%。见[THPRAC128_REAL_AUDIO.md](THPRAC128_REAL_AUDIO.md)。

2026-11-17补充：F12按固定原版补齐待应用草稿/Apply显隐/自由整数调整/原预设格式与中英日13项原词条，真实989验证参数→原生150FPS→RPY/真实IDB→Replay锁定，受影响990273通过；仍不计完整thprac或第三阶段完成。989裁图像素未覆盖目标区域，未作为画面证明，详见[THPRAC128_F12_PARITY.md](THPRAC128_F12_PARITY.md)。

2026-11-17 当前覆盖以[浏览器原生组合](THPRAC128_BROWSER_CLOSURE.md)为准：菜单16stage/126chapter/156section/EXphase/五资源、练习RPY与playLock、F1–F7和F12速度已经接入；978真实browser/native完整caller和取消/失败清理通过，980受影响265通过。以下早期矩阵保留为迁移过程记录，不再代表这些已接功能仍缺失。图形/文字纹理/物理音频在978仍为明确接缝，完整thprac/phase3仍未标完成，约90%仅粗略功能估计。内部测试控制栏未进入正式site入口。

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
| section按chapter/mid/end/nonspell/spell选择（260–351）；chapter126项（397–403） | 已实现Program覆盖、TH128 chapter组件及实际StageWorld启动；尚未接Web/导出taint | 867/870组件证据保；879原A1章节/双跳/EX实际World首出生与reset普通对照，仍须Web/参数全链 |
| Boss/多Boss/EX符卡与phase（657–735、1227–2051）；EX_END_S10四phase | 多文件SetFile(0/2)与分支/HP/位移原文存在；已同批移植156section/318参数组，事务式root+provider覆盖与实际StageWorld消费者；Web尚未接入 | 892/897全部原/中文表可prepare；A1对话/首卡HP/三Boss真实World，EX四phase为真实Vm资源接缝，Web音乐/intro规则仍需接 |
| THBGMTest/disableLogo（2067–2079、2253–2264） | 有StageMusic/native sourcefade/stop严格ACK；thprac section BGM/intro行为未接 | 按section/dialogue实际分流、重开保持/退出释放，不改volume95 |
| THGuiRep/ReplaySaveParam/锁（406–450、2080、2266–2310） | 原RPYcodec/25槽已验，但没有THPracParam辅助元数据或playLock/thHardLock策略 | 练习RPY+原参数绑定、正确起点/重播隔离/存储完整回读；不JSON改名RPY |
| F1..F7overlay（454–517）、F12/高级速度（520–645） | 当前无限复活setter不是原无敌/无限资源/时停/自动Bomb的完整等价实现 | TH128各toggle实际效果+禁用恢复；共有speed/key/nav模块适配；Windows vpatch不执行 |
| Eagler共用input/navigation/statistics/mobile/locale | common/docs/thprac.md声明共享工具与作品特定生命周期分离；当前JS宿主没有C++ thprac GUI | 实际菜单/键盘/鼠标owner/locale与核心效果；手机额外兼容单列但基础入口不得假ACK |

完整上游模块2375行已按段读完。TH128模式/label/156section枚举与多语言表已从同版games定义/loc_json取得；TH128专用文件已证明存在，不再猜测“不支持TH128”。上游eagler thprac helper当前未列th128，是要新增实际适配，不是照抄其他游戏默认schema。

864只读新目的：真实32336111B DAT→16个原始stage ECL，按固定上游THStageWarp解析126个chapter选择/143条ECLJumpEx写入。所有源写入起点和目标均为原始指令边界。16个chapter1不写补丁，其余110个raw-copy补丁在本工程整段SCPT/ECLH解析边界检查中失败。例：A1-1源offset0x81d8原opcode22/size40；上游24B跳转覆盖后offset0x81f0留下原参数尾部，严格parser误读length29551。其他起点原指令为20/32/40字节；不能简单保原size，因为Vm::shape要求opcode12恰2个rawWords+2params。

这是 **移植结构差异核对**，不是原thprac错误，也不是新实现已通过。读取与模拟覆盖都在ownedcopies，DAT/source/公开页未改；没有实际native Program/Vm执行、World/Factory/compile/browser/GPU。864 inventory1PASS只证明上述读取/边界记录完成，不能替代新参数/跳转实际RED→GREEN。

865–873已把上述指令覆盖方案落实为原生组件，详见THPRAC128_BRANCH_OVERLAY.md。Program保持原资产、原offset与其他指令；TH128专用表按固定上游源映射16stage/126chapter/143跳转。870用原ECL与真实Vm验证110个非平凡首跳转，但测试显式使用入口跳板绕过原初始化，并在目标执行前以预算停止；这不是World/游戏起点验收，也不证明所有第二跳已在游戏执行。873完整reader/VM/held-host受影响检查通过。

下一必须接实际StageWorld加载→练习拥有者→start/reset与Web消费者，之后完成fresh参数组、Boss/section/phase、多ECLI、菜单、Replay与共用工具；仍未交付完整thprac。core112与已接受路线保持原证据范围，不为组件重编核心。

874–880实际World消费者已接通，详见THPRAC128_WORLD_CHAPTER.md。879以真实C++ StageWorld/Vm、原ECL前导代码与原profile，验证A1 chapter2的GirlB1在frame91、reset后普通GirlA1b在101、A1第二面双跳GirlD2在91、EX chapter2两名GirlD1在161；没有入口跳板或预算伪停止。1108断言、12次start调用（含4次拒绝），并通过失效/重复/异源/同大小错误SHA/reset/退休/重入控制。880完整StageWorld/WorldExports生产TU syntax通过。

这次只有C++章节拥有者和固定原root SHA校验。Web/导出入口尚未绑定该练习身份与正常成绩资格，完整ECLI提供者来源绑定、参数组与section/phase/菜单/Replay/工具仍未完成；不能据此再宣称完整thprac或第三阶段完成。没有新全核心、browser/设备或公开部署。

881–883补上实际可达ECLI的资源身份绑定：prepare前校验root/default/boss闭包，仅支持已验证原33文件与现有中文包的22个变体。881真实caller出现10个失败（异变依赖被接受、缺依赖仍可prepare、六个中文第三面root被拒）；882同夹具48断言全部通过，883受影响1108断言通过。中文六个第三面固定偏移4/8由原/中文子程序名称和指令序号逐项比对143操作的286端点，绑定完整资源SHA后使用；不是通用翻译兼容或任意字节补丁。

现有中文版16个stage的chapter2都能完成preparation；实际中文A1第三面chapter2通过World原prelude到frame91首出生。完整126chapter/phase游戏执行尚未全部验收。下一转入原五字段的fresh参数组与实际消费者；本次没有新增参数效果、菜单、Web入口、WASM API或全核心。第三阶段完成结论仍撤回。详见THPRAC128_INCLUDE_OWNER.md。

884–888原五字段已接实际原生消费者：StageWorld拥有64位score与其余四字段，验证／向下10舍入／复制；完整WorldExports启动初始化真实scoreRaw、干劲、charge、power、area并设置非正常taint，四个配置函数拒绝字段或练习rank不一致。888六个独立组件进程跑完整WorldExports与真实PlayerHitHost/PlayerShotGroups，默认／0／上限EX／中间值、10项越界拒绝、重置隔离和自然EX守卫通过；原SHT/ANM组0/6/15实际读回。不是纯codec或提取函数测试，也不是浏览器交付。

真人指出推进过慢：下一批复用现成TH128菜单和完整section/phase规则，按完整功能批次接实际Web调用，减少碎片审计。当前仍缺完整Boss/section/phase、菜单工具、练习Replay和Web入口；第三阶段完成结论不恢复。详见THPRAC128_INITIAL_RESOURCES.md。

889–898完整选段已作为一批接原生消费者，详见THPRAC128_SECTIONS.md。318固定上游写入记录生成156section/对话/EX四阶段计划，按55原/现中文身份与子程序/指令坐标映射；Program不可变虚拟指令区，StageWorld同时采用root/provider。892与897分别完成整表preparation和实际对话/HP1200/Normal spell1/三Boss代表验证，EX为原prelude真实Vm在HP处hold的资源host接缝；不是全156段游戏或EX完整World。898受影响reader/mutations/VM/heldhost编译运行全0。

当前仍无真实thprac Web参数/菜单/Replay工具链或新WASM API；第三阶段未完成。下一直接完整菜单/native bridge/actual play参数消费者，停止旧选关/include长审计。历史新增一项外部Python二进制变更：case772旧工具无owned精确档案，保1未解而不伪0；当前Emsdk固定工具与8不可变核心/资源指纹不受影响。

899–914真实Web主菜单与原生桥已接：TH128专用schema保原10字段，126章节/156选段/EX四phase/对话及中英日label；实际页面load后/start前prepare，五字段进真实配置并核nativeframe0/五值/taint，取消/重开与正常资格隔离。新private113首次构建通过，908实际3Factory/3World/427checks/每个首tick；914真实HTML/CSS/组件和9wholecaller16检查通过。两者范围独立，不能冒完整原资源browser游戏。

实际native section初始化顺序901RED→903修复，真实空select默认910/911RED→912/914修复；所有首FAIL保留。完整thprac/第三阶段仍未完成：练习Replay metadata/recording/playLock、F1..F7/Backspace overlay、F12speed、持久launcher开关还需实际消费者。菜单内enable/locale已经生效，未改个人设置。旧112/v7/public产物不动，新113只owned编译产物，打包/公开发布暂停。详见THPRAC128_WEB_MENU.md及thprac128-Web-menu899-914.json。

2026-11-17更新：915–934练习Replay批次已接实际native输入录制、原RPY＋上游USER/PRAC元数据、严格25槽CAS保存/回读、同参数播放与playLock。921实际录制/回放3帧，930新114真实2Factory/2World/6tick/294检查投影一致；933真实浏览器IDB与当前save/watch页面10项通过。114仅owned编译产物，旧112/v7/public不动；不重新打包。剩余Backspace/F1..F7/F12速度/持久launcher gate仍须完成，完整thprac/第三阶段仍未完成。具体范围见THPRAC128_REPLAY.md。

2026-11-17最新：935–954已接实际F1..F6原生效果、F7 source owner、固定60Hz高级调度、工具/速度Replay元数据与owned enable/locale。按真人原版截图改三种游戏内面板：Backspace左上七行，F12全画面高级选项，正常模式仅起点/模式、练习模式按原顺序展开与百分比显示，字体优先级取固定上游。937真实World组件677检查，115首次构建/949实际核心301检查，951受影响259项、954真实浏览器14项/37.598s通过。浏览器游戏/原生/背景是明确接缝，完整thprac与第三阶段仍false；下一955一次当前完整caller、F7重开/真实音乐ACK、速度/Replay组合集成验收。详见THPRAC128_TOOLS_UI.md。成功93..115不重编，打包与公开发布继续暂停。

2026-11-17接续955–967：首次运行完整当前checkWorld/startLaunch/prepareFresh/advance与真实DATOGG provider，发现并修复练习录制漏actual routeChoice MSG辅助owner的首tick问题（957RED→958119检查/2World→新116一次构建）。959资源identity/路线ready两RED→960261项通过。963完整启动/重开F7同source/Replay114检查；966实际路线函数→完整菜单Enter→真实116/原资源→120fps固定60Hz调度、自然击破末卡与MSG/方向选择→frame1028原MSG22 fade480帧严格ACK并保持F7source→退出与3帧回放，1145检查/6Factory3World1034ticks通过。967普通默认隔离131检查/8Factory4World7ticks通过。图形/字体栅格、物理WebAudio和ending准备接缝明确，未冒实际原资源browsergame；完整thprac/阶段3仍false，最终浏览器native组合/取消错误收口下一968。详见THPRAC128_CURRENT_CALLER.md。成功93..116禁重编，Runtime/public暂停。

## 2026-11-18 真人反馈接续至1071

练习终局Replay1062真RED→最小sameowner修复→1064真实browser7PASS/7336tick/realIDB与音频ACK，未播放本次Ending或采用Title。原生触控1065缺消费者真RED→1069真实PlayerHitHost/ANM固定步录播2World22tick逐帧15投影同，原RPY触控USER扩展保原载荷；1071整29Websuite142PASS0skip。页面仍旧方向盘，完整触控UI/布局/移动窗口/协议、普通无限流程、魔理沙EX与117生产页整合待，完整thprac/第三步false。详见USER_FEEDBACK_TERMINAL_TOUCH.md。

## 2026-11-18 触控当前caller/布局接入至1091

完整上游TouchInput及launcher布局/model/joystick/gameZoom源已接正式页面，C512/固定步/物理来源所有权与direct/control/cancel真实桥接；新高级窗口释放透明touch命中层，移动ESC真实关闭。1087实际browser11PASS31.955s/1Factory2World20tick/真实隔离IDB1393B RPYTCH1PRAC回放10步每15投影同，F12close实际恢复。1091整46suite284PASS0skip。设备入口仍是显式World组件driver，非完整productionplay/Title/GPU/physicaltouch；普通无限模式与完整原移动浮窗/设备、EX画面/117及包待，完整thprac/第三步false。详见USER_FEEDBACK_FULL_TOUCH.md。

## 2026-11-18 普通触控与选路辅助录像（1101）

普通mode2已接真实PlayerHitHost；按固定上游实际消耗的非零无限位移标记辅助，保持正常方向键优先/播放锁/世代和新开局隔离。辅助运行保真实自然运行身份及录像，原profile字节恢复，普通成绩/符卡/解锁写入和postrun排行插入独立阻止，录像slowdown100；实际自然跨关/终局仍待。1099原11809World/11806主帧/3选路辅助帧缺失真RED，现成功tick记录v2TCH1并由匹配native World帧PAUX消费，原6B载荷不变。1100组件2Factory5World23711tick、录播15投影逐帧同；1101受影响16suite75PASS0skip。World组件/strict合成IDB，0本轮browser/Title/GPU/音频ACK/Ending；完整第三步false，117/包未准备。见[本批范围](USER_FEEDBACK_ORDINARY_TOUCH.md)。

## 2026-11-18 普通触控自然结局与回放终局（1115）

1107原三面Perfect120对后出生激光错误要求Empty真RED；1108仅改为实际manager可用校验，80TU组件通过。1110实际普通[1,2,3]两自然carry/89799游戏步/89715主帧+84选路辅助，真实IDB原RPY和成绩ACK、原Ending15进入，整轮时限失败保留。1114同实际记录输入经LIVE NativeRecord重建（非playLock），14项99.600s/1Factory3World89799游戏+206Ending、真实音频/strictIDB/source.stop/cleaned16→sealed普通结算current4mode3、profile不变/allcontexts关闭通过。1115真实nativeplayLock前两段40360World步通过，第三Worldframe0被普通NormalRecord terminal资源门拒绝，专属Replay终局待修。0新117/包/部署，完整第三步false。见[本批范围](USER_FEEDBACK_NATURAL_ENDING.md)。

## 2026-11-18 原生Replay终局与移动浮窗（1129）

1118真正native playLock原三关89799步/89715主帧+84选路辅助、两carry、原MSG与真实音乐ACK→独立Replay终局380/score795972通过，17项85.981s，未创建普通记录；World组件范围，不冒Title/Ending回放/GPU。1126首关录制重入真RED→1127最小原生守卫与真实1World0步消费者GREEN。1124真实顶层横竖视口21交互/存储/重挂载/受控iOS事件通过；离线图发现字段裁切后1125最小CSS修复、8实际内容边界和局部像素复核通过。1129最终48受影响Websuite293PASS0skip。两个80TU组件、无117/包/部署；魔理沙EX材质与背景、完整生产页/Title/原字形音频、最终本地包仍待，第三步false。详见[本批范围](USER_FEEDBACK_REPLAY_MOBILE.md)。
