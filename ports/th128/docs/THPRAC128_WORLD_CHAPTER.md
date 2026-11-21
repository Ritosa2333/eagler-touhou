# TH128 thprac：实际World章节消费者

StageWorld::prepareThpracChapter在启动前绑定stage/chapter/rank，验证原root ECL的完整SHA，生成已有指令覆盖组件的候选Program。实际start使用该候选链接include并创建真实Vm；load锁定、错误原子、重复/迟到拒绝、reset/退休隔离均经过行为断言。原sources_始终保存原ECL，reset后的普通流程使用原Program。

| 验证 | 实际观察 |
|---|---|
| A1第一面 chapter2 / Normal | 原MainSub01的GirlB1在World frame91出生 |
| reset后同资源普通开局 | 原MainSub00的GirlA1b在frame101出生，没有沿用chapter |
| A1第二面 chapter8 / Normal | 两处原跳转均由真实World消费，MainSub07的GirlD2在frame91出生 |
| EX chapter2 / rank4 | 原MainSub01保留70 tick等待，GirlD1两名在frame161出生 |

879编译/运行0，1108断言、12次真实start调用，包含4次拒绝。该用例没有入口跳板、改写原脚本、改Vm时钟/RNG或预算停止。type8原profile与固定player位置是明确组件输入；bullet输出回调拒绝超出范围的创建。测试止于首批敌人出生，未跑完整关卡或声画设备。880完整StageWorld和WorldExports两生产TU仅syntax编译通过。

875是缺实际消费者的RED。874漏link依赖、876夹具混合auto类型、877/878未完整配置原-9989引用上下文均保首失败；877“只需aim”判断被878后续源码复核纠正：该引用要求bulletsConfigured，879通过原configureBullets配置真实profile。生产代码从876到879保持相同，未放松原引用守卫。

当前只有C++章节消费者。完整thprac尚需Web/导出taint与正常成绩隔离、全部include来源绑定、fresh初始参数组、完整section/phase、菜单/热键/速度与练习Replay。root SHA只是已支持资源字节一致性，不是发布者身份认证；它也不单独证明include来源。此次0全核心编译/Factory/browser/GPU/公开部署，core112与既有资源不变。详细结果和完整指纹见thprac128-world-chapter874-880.json。
