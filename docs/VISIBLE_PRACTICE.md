# 可见练习入口

游戏页顶部“练习”按钮在资源及现有原生接口就绪后可用；主动打开后选择六路线第三面Normal或EX起点。正在准备时禁止重复启动；替换当前局需勾选确认。重新开始以当前实际练习局的路线为准。关闭/Escape取消界面，启动中关闭只取消匹配的launchId/selection，迟到完成不能冒新局成功；启动器退出会释放界面和待处理工作。

每次seed34772/progress0/fresh，练习局不计正常通关与解锁，不使用前关carry。不提供清库/模拟通关/解锁按钮，不自动弹出开发面板；原开发工具仍hidden/inert。练习重用现有startDebugThirdStage/prepareStage及真实native taint校验，没有改核心/RNG/HP/碰撞/历史。

846缺组件/调用者24RED→84724GREEN。848首受影响283PASS/18FAIL，均是两个源提取fixture遗漏新practiceMenu词法变量；849补齐fixture及2条输入隔离行为断言后303PASS。850通过实际PageLaunchContext.abort发现runtime.exit未关闭练习对话框1RED；851增加一行dispose，全部304PASS、0skip/0stderr。首FAIL证据原样保留。

852只在sourcecheck拒绝了命令路径分隔符，未进入运行。853首次浏览器4项通过后，A1已采用但等待定位器同时匹配隐藏旧option而失败；新854缩窄测试定位器，产品字节不变，15项通过/14pins/35.792s/console0。实际HTML/CSS/组件与当前installPracticeMenu、startLaunch、cancelLaunch、taint与exit函数逐字节源提取；7个固定路由、替换确认、EX重开、延迟取消不采用、退出均验证。一个测试tab/server在同workflow finally关闭，ROOT释放。

浏览器native/module/resource/pause/presentation是自编边界，0World/Factory/compile/GPU/真实游戏帧/物理音频/个人数据。不能把此结果与旧772CPU/840资源缓存验收合成完整retail浏览器游戏。截图仅存工作证据[practice-visible.jpg](D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113/case854-browser-visible-practice-current-caller/practice-visible.jpg)，未Git发布图片。

独立源码review5393B/SHA593100414836bbfd75acd9898c3a817238fa4af982adc17e8dfb62226a83e1a6，读的是847快照。MAIN将“hosted exit残留”提升为实际缺陷并850→851修复；mounted页面缺口由854补证。焦点陷阱/对话框外Escape、取消完成临界文案、重复文案为小改进待办；晚到guard拒绝误cancel外国owner仅理论防御线索，实际UI同步ready/busy guard使该入口不可达，保当前identity校验。未循环重审或扩大原生兼容门槛。

有限Web功能基线据[WEB_FUNCTIONAL_SCOPE.md](WEB_FUNCTIONAL_SCOPE.md)完成；公网发布和本地Runtime打包仍暂停。第四阶段已实际进入规则握手855–856，尚无可玩多人对局。
