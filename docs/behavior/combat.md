# 战斗行为与原版接口

固定TH12 1.00b。逻辑顺序：Player16、Bomb17、Enemy18、UFO19、Laser20、Bullet21、Item22、Spell23、HUD/MSG25、primary ANM27；secondary ANM在8。

## 受击与死亡 Bomb

初次碰撞调用438370，进入state4并建立8tick死亡窗口，保护时钟为6。
4381e0在窗口结束确认Miss并扣残机。Spell age>=60时，初次碰撞就失去捕获和清bonus；
死亡Bomb救回不会恢复已失去的捕获。age<60与pendingBomb是独立分支。

普通友弹离场基于ANM四个世界顶点、年龄和类型，不能以中心y<-100替代。
魔理沙A激光松键后保留主／辅动画中断1的退场阶段，不能立即销毁。

## 伤害源与特殊行为

友弹池256，持久Source池128；439ed0合计友弹／Source／Bomb伤害并限制单敌单帧80。
几何、cadence和Source累计值属于Source；特定Spell减伤与屏蔽在Enemy层处理。
六套Bomb、动态尾迹、自定义回调和动画绑定顺序都可能消费game RNG。

## 三种敌方激光

| kind | vtable | initialize / tick | 形态 |
| --- | --- | --- | --- |
| 0 | 4a0654 | 4287c0 / 429610 | 移动／延长直线 |
| 1 | 4a0704 | 42ab80 / 42ada0 | 预告、展开、保持、缩宽 |
| 2 | 4a06ac | 42c3f0 / 42c770 | 历史节点曲线 |

Factory428450、manager428270。kind1局部消除可能保留原段并派生kind0短段，
不能只按初始类型解析所有节点。全清、矩形清、圆清、转点和退休标记分别处理。
ECL600/601配置参数；602/603/611分别建立三类；604..609查ID后改字段；610可循环清同ID多条。

529安装持续callback，530替换hurt，531替换自定义碰撞，534立即调用529同一表。
534实际用于吸力、Extra特殊UFO分数、激光派生发弹／消弹等，不能忽略。
700仅在debug routines中出现，经原版dispatcher默认返回并正常前进PC，不需要虚构行为。

已恢复独立C++ MovingLaser、TimedLaser、CurveLaser与Manager；900tick生命周期、60几何查询、
522碰撞场景及72个ECL构造参数有原版局部对照。未完成的重点：全作敌弹扩展组合、
未验证的激光扩展组合与网格／UV呈现、534特殊回调、满池复用、
所有保护标记和整场回放对照。反编译结构及局部规则不是整局Oracle通过的证明。
