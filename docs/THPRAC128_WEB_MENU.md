# TH128 thprac：真实Web菜单与原生调用

现在有真正的TH128专用菜单：起点/路径/关卡/难度、道中章节、道中Boss/关底Boss/非符/符卡、EX最后符卡四阶段、对话，以及分数/干劲/完美冻结/冰冻力量/冰冻面积。参数、126章节、156选段及中英日标签复用固定上游；七项简易选关不再作为生产练习入口。主动打开/关闭、替换确认、取消迟到不采用、按当前实际参数重开及页面内启用开关均接实际调用。

Web会话保留eagler-touhou/thprac-session/1与TH128字段，严格整数和上下界，分数向下10取整后以raw值过11word/44B原生接口。真实checkWorld在资源load后/start前prepare，使用owned值配置四个原生消费者，检查frame0/generation/实际五值/nonNormal；练习局不取得正常成绩/解锁/原普通录像资格，不自动跨关。Boss BGM顺序与logo跳过按固定上游规则接当前宿主。

新核心private113是实际Web接口需要后首次构建：231源/83TU/208声明导出（实际206个th128接口加malloc/free），generation0b85f8c65ea8bed7326c14207ddd3a8777180a3a002f4b89bd91d7e82af7e8f6。core.mjs135899B/SHA6f89ce838f1d91447980829236ca20a251bb6b792a49218dbb6b0c31384092fc；wasm2419295B/SHA020deaea9c45f1cc0388cfa0e1be0e1a8a9f6e492a1384483caac6fea9efc9c0。已成功的112及更早核心未重编；113成功后也不重复构建。源码仓库不收录生成WASM/原资产；未替换旧交付目录、打包或公开发布。

验证分开记录：912的17项实际schema/menu/currentcaller行为和904的156项受影响检查；903完整WorldExports+真实PlayerHitHost/shotgroups，332断言/10次start；908真实113的3Factory/3World、427检查、原DAT/ECL/ANM/SHT/消息/geometry/子弹/玩家资源和每World一个实际tick，实际五值与组6/0/15读回。914实际HTML/CSS/完整菜单和9个whole当前页面函数16检查通过，61.564秒/console0，同workflow关闭1ownedtab/server并释放ROOT。914的引擎/资源/checkWorld/暂停是显式边界，不和908合称完整原资源浏览器游戏或设备60FPS。

本批修复两个真实调用缺口：section root原time0出生早于玩家资源配置，现只练习section推迟到首个真实tick；原生select在空options时丢失默认值，现显式保Practice/Normal初值。失败夹具/观察竞态及每个首FAIL原raw保留，不覆盖成功证据。完整历史有1项旧case772外部Python二进制更新未解，当前Emsdk及8不可变产物指纹不受影响。

**完整thprac与第三阶段仍未完成。** 练习Replay元数据/录制与playLock、Backspace/F1..F7 overlay、F12高级速度及启动器持久开关仍须接真实消费者；手机/JA/617兼容不追加为无关门槛。当前enable与locale是页面内控制，未写个人设置。普通模式EX沿用既有资格入口。完整菜单代码与私有113已具备，旧本地v7和公开页仍是112；本地Runtime打包与公开发布继续暂停。

2026-11-17更新：915–934练习Replay批次已接实际native输入录制、原RPY＋上游USER/PRAC元数据、严格25槽CAS保存/回读、同参数播放与playLock。921实际录制/回放3帧，930新114真实2Factory/2World/6tick/294检查投影一致；933真实浏览器IDB与当前save/watch页面10项通过。114仅owned编译产物，旧112/v7/public不动；不重新打包。剩余Backspace/F1..F7/F12速度/持久launcher gate仍须完成，完整thprac/第三阶段仍未完成。具体范围见THPRAC128_REPLAY.md。
