# Web阶段有限功能范围（2026-11-17）

还原与移植以用户已接受的六条普通路线526/532/531/539/535/540及EX560为功能基线。下表只定义本阶段实际应交付的Web能力，避免把未定义的“所有兼容性”不断追加为阶段门槛。

| 能力 | 当前实际范围与证据 | 仍需交付或单列边界 |
|---|---|---|
| 自动入口/原资源 | 原DAT＋OGG/中文包真实消费者；772完整93MB CPU HTTP分块、687资源浏览器各保原范围；840当前入口＋真实IDB/小自编包20项 | 公网域名/账号部署获准后置，未公开；不能合称完整原资源浏览器游戏或任意CDN兼容 |
| 配置/父协议 | 805真实iframe；814原DATA采用；816真实同源回调/Blob（自编素材）；有限origin/epoch/request及取消/退出；none实际输出833/837 | partialAdapter/fileBridge:false，完整兼容包仍需；MIDI/任意runtimePack明确拒绝 |
| 输入 | 键盘/别名、多来源隔离与清理、触控UI已接；774–782/794/804CPU及785真实浏览器有限证据 | 物理手机多指/可信Shift混用是设备兼容待办，不加为本阶段完成门槛 |
| 60Hz与显示 | FrameStepper固定1000/60、最多4步、失焦/长停顿复位；显示插值与逻辑分开，当前选项接实际入口 | 性能CPU证据不冒60FPS；设备刷新率和像素比较单列 |
| 语言 | 真实catalog提供zh/ja、已选语言初始化、缺失选择回退；父配置只接受已安装catalog语言 | 中文资源真实使用；原生日文字形/额外语言/缺font设备兼容保待办，不冒已全验 |
| 存储/录像 | 25槽Replay exact-CAS/strict事务/完整回读822、823原RPY与游戏读取者；原生捕获分数及T821只读导出；JSON设置与独立9B姓名保持 | 不伪造scoreimport/原binarycfg或默认/解锁；原版完整录像617为单列兼容 |
| 生命周期 | 840启动失败/版本不符保旧/退出取消；843持久缓存返回请求fresh document，845受影响110；844真实历史返回＋IDB保持 | 844实际pageshow.persisted=false；cached true分支仅CPU事件边界验证，设备BFCache单列 |
| thprac 练习适配 | 真TH128主菜单/126章节/156选段/EX四phase/五原字段/中英日/取消重开已接真实原生桥；903完整消费者、908真实113首tick、914真实浏览器菜单各保独立范围 | **仍需交付**：练习Replay元数据/录制与playLock、Backspace/F1..F7工具、F12速度和启动器持久开关。完整thprac/第三阶段仍未完成；详见THPRAC128_WEB_MENU.md |

2026-11-17 用户明确纠正：要求的是 TH128 对应的 thprac。此前把七项简易选关计为练习适配完成并宣布第三阶段完成，属于范围判断错误；该完成结论撤回。已验证的其他Web能力和原单人基线保持，当前先补实际thprac适配，多人原型保存、后续推进置后。公网与设备兼容仍分别记录。

历史简易选关仅支持已有六路线第三面Normal与EX起点，固定seed34772/progress0/fresh，不带已验证的前关carry。练习局保持非正常成绩/解锁资格，不清库、不改设置/音量。854使用实际HTML/CSS/组件及逐字节源提取的当前startLaunch/cancel/taint/exit；native模块、资源加载、暂停和渲染明确为边界替身，0World/Factory/GPU/物理音频。完整游戏引擎沿用已接受基线，不重跑六线/EX。

thprac实际来源与后续工作见[THPRAC_CORRECTION.md](THPRAC_CORRECTION.md)。保留的多人规则见[MULTIPLAYER_RULES.md](MULTIPLAYER_RULES.md)，可见练习说明及验收见[VISIBLE_PRACTICE.md](VISIBLE_PRACTICE.md)。
