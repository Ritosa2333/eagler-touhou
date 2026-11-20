# 魔理沙共享随机调度

种子1的两路随机连续一致范围推进到7125帧；前7126帧的主随机、CPU输入、自机位置和敌弹坐标全部一致。清屏辅助脚本在公告初始化之后、玩家SHT创建之前执行；出生帧保留玩家移动后的定位入口。新公告、擦弹登记、充能根及破碎波发射器按原版调用先后更新。

本轮对应构建为`b00f0d37aa32f2828a376ab36c3a21736c98b5e27708174f753a0ef15f9f3e11`，Web采集`th19-web-gHvISw`，使用原始运行参数，未指定第二轮出怪表。原始五种子36000帧完成数仍为1/5，种子42已复核通过；其余种子的跨轮验证继续进行。

第6507帧左右图保留同源原版轨迹和累计差异：

- [出生坐标修复前](screenshots/seed1-6507-discharge-before/frame-6507.png)：一颗普通弹提前进入取消状态。
- [出生坐标修复后](screenshots/seed1-6507-discharge-after/frame-6507.png)：主副随机、输入、自机及敌弹状态一致。

采集工具支持`--stop-on-rng-difference`，限定与`--opening-only`一起使用。诊断报告通过`requestedThrough`、`capturedThrough`和`stoppedOnRngDifference`区分目标与实际覆盖，最终36000帧验收继续使用完整运行。

- [擦弹和充能登记修复的完整复验](graze-charge-registration-seed1-full.json)
- [破碎波原版队列](breakwave-native-queue-4714.json)
- [破碎波修复的种子42完整复核](breakwave-seed42-full.json)
- [两场擦弹登记修复的完整复验](graze-heading-seed1-full.json)
- [清屏调度候选及出生坐标回归](discharge-candidate-7126.json)
