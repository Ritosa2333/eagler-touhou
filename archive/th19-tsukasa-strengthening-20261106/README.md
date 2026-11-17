# 管牧典 EX 弹幕强化

原版 EXE SHA256：8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9。

`pl08.ecl` 的 ExAttack 和 ExAttack2 通过 ECL624(2) 注册 0x4fbff0。旧实现拒绝 callback 2，导致强化未执行。现按原版检查距离平方严格小于 1024、Bullet+104c 大于 20、Bullet+1070 不等于当前攻击对象 ID。符合条件时先记录对象 ID，再转换弹种；同对象不会重复升级，不同对象可再次升级。

| 原弹种 | 新弹种 | 颜色 |
| --- | --- | --- |
| 0 / 1 | 4 | 保留 |
| 4 / 5 | 18 | 1 |
| 8 | 20 | 1 |
| 9 | 31 | 1 |
| 18 | 33 | 1 |

0x4f11a0 重建弹种动画、接触大小和消弹表现，保留运动与年龄。0x4f4df6–0x4f4e7a 更新年龄；0x4f5102 初始化上次对象 ID。网页固定逻辑帧累加年龄；新增字段由 WorldCheckpoint 的完整世界副本保存。

验证：`node ports/th19/tests/tsukasa-strengthening.mjs [pl08.ecl路径]`。覆盖全部映射、颜色、速度保留、严格范围、年龄边界、连续强化、世界更新和回滚；传入原版脚本时额外执行 ExAttack / ExAttack2，核对 callback 2 注册与实际转换。未启动原版游戏。
