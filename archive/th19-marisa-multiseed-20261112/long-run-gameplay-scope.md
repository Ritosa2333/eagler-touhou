# 魔理沙五种子长程玩法验证

每个种子累计验证36000个战斗帧，跨轮累计。画面动画、副随机和不影响实际运动的输入差异按用户要求搁置。CPU追击目标在结算阶段统一使用冻结位置，消除种子20261112第27611帧提前结束斜向移动的问题。

换轮出怪表沿用原版共享副随机状态。当前原始运行的共享副随机仍存在偏差，原始全轨迹36000帧完成数为1/5，完成种子为42。为继续检查长程玩法机制，诊断模式可临时提供原版第二轮出怪表种子。每份报告通过`diagnosticRoundWaveSeed`标明条件，每张受控左右图同时标明该种子。受控结果用于判断相同出怪条件下的CPU、位置和弹幕机制。

种子1在第二轮种子468994369、种子20261112在985282850、种子123456789在268227792条件下，36000帧主随机、CPU输入、自机坐标和敌弹坐标均一致。原始种子问题继续保留。

- [种子1受控完整结果](seed1-controlled-round2-full.json)
- [种子20261112受控完整结果](seed2026-controlled-round2-full.json)
- [种子123456789受控完整结果](seed123-controlled-round2-full.json)
- [CPU结算目标位置诊断](result-target-position-diagnostic.json)
- [CPU结算目标完整结果](result-target-position-seed2026-full.json)
- [27611帧左右图](screenshots/seed2026-27611-target-after/frame-27611.png)
- [33861帧原始左右图](screenshots/seed1-33861-gameplay-current/frame-33861.png)
- [33900帧受控左右图](screenshots/seed1-controlled-round2-36000/frame-33900.png)

种子987654321在原版第二轮出怪表种子2125168245条件下，36000帧自机位置及敌弹精确坐标全部一致。输入与主随机存在不改变上述实质状态的差异，按用户要求搁置。见 [完整受控报告](seed987-controlled-round2-full.json)。

最终汇总见 [五种子条件与覆盖](five-seed-gameplay-final-summary.json)。最终构建25944ca01b64764bce928f4cc9ec6b7586c031d9cbfd9d3b7e15f76c4255c597下，[种子42原始复核](final-seed42-original-full.json)及[种子1受控复核](final-seed1-controlled-round2-full.json)均通过对应36000帧玩法验证。56项测试通过，566份文档检查通过。
