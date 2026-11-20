# C2 飞行派发顺序

C2 飞行的随机时长在共享飞行管理器处理已有 EX 到达后分配。原版第 22309 帧先执行两次 ExtraAttack00，各读取两次主随机数，再构造六条 C2 飞行。网页端按相同顺序初始化待派发 C2，并从下一帧开始检查飞行计时。

种子 123456789/987654321 的[窗口验证](c2-flight-dispatch-window.json)覆盖至累计第 22374 帧，主 RNG、CPU 输入、自机、敌弹数量与精确坐标全部一致。构建 `4c6ad54aaf314e75929bed5312ec74bad23cbfcc70229696ecf9b42aed8e1075`。

[第 22373 帧左右对照](screenshots/seed123-22373-after/frame-22373.png)左侧网页、右侧原版，显示同帧主 RNG、输入、自机坐标和累计玩法差异帧数。原版采集 `present-hqc1u9mx`，Web 采集 `th19-web-6JsyJq`，两者 trace 哈希一致。

[原基准累计 36000 帧](c2-flight-dispatch-baseline.json)主 RNG、CPU 输入、自机与敌弹精确坐标全部一致。[种子 123456789 完整采样](c2-flight-dispatch-seed123.json)的玩法一致前缀达到 26500 帧；第 26501 帧进入原版结果阶段的连续模拟核对。
