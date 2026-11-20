# 魔理沙消弹与延迟擦弹登记顺序

魔理沙种子 1 在同帧内按对象登记顺序执行消弹、充能与擦弹对象，短测 2380 帧两路随机、CPU 输入、自机坐标和敌弹坐标全部一致。构建为 `d63a41b624f51533d67fd5c081cf17f13a62140a67f6de0542e30fa561696fc2`，完整 36000 帧验证另行记录。

原版在 2323 帧先执行消弹根 197，再初始化 aura14；2351 帧则先初始化 aura14，再执行新登记的消弹根 185。根对象登记顺序决定本次循环抽取的随机等待值，固定类别顺序无法表达这一关系。

擦弹奖励请求携带延迟。原版在延迟结束后登记轨迹对象，并于 `0x4dc08c` 抽取初始方向；轨迹的逐帧更新在 `0x4dc250` 执行。Web 在实际登记时分配视觉顺序并准备方向，等待中的请求不进入活动视觉队列。玩法奖励仍在原有帧结算。

证据见 [消弹根调用顺序](cancel-aura-order-diagnostic.json)、[登记顺序短测](cancel-aura-registration-window.json)、[擦弹登记诊断](graze-registration-order-diagnostic.json) 和 [2380 帧通过报告](graze-registration-order-window.json)。

左右截图依次为 Web、原版，并标记同一模拟帧及累计玩法差异帧数：[2327 帧](screenshots/seed1-2327-rng-after/frame-2327.png)、[2353 帧](screenshots/seed1-2353-graze-after/frame-2353.png)。

独立敌机子动画由 enemy.anm267 的 ANM500 创建脚本103，脚本103每帧通过ANM504创建99。原版独立103根与消弹根位于同一登记队列，在第2519帧右场队列中，103、200、103依次位于378、435、442号。按267父对象对应的独立子动画顺序穿插更新后，构建 `a4a958330291ee788f5f3b18be5ce969997c6077fbf43904968afef5b33bcbbc` 完整验证两路随机一致到3113帧，玩法一致到33860帧。证据见 [独立动画队列诊断](independent-anm-order-diagnostic.json)、[2530帧短测](independent-anm-order-window.json) 和 [种子1完整报告](independent-anm-order-seed1-full.json)。
