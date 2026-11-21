# 启动回归：成绩存在、录像姓名尚未保存

用户在结束一次游戏后重新进入Runtime或共享工作台，看到`Independent9B observation is absent`。成绩档案与上次录像姓名是独立存储；合法的失败成绩可以先被保存，姓名只有保存录像后才写入。Main误把EX资格严格姓名证明作为整个标题加载前提，造成第二次启动被拒。

修复只捕获已完整验证成绩/clear counters之后的`name-absent`，且要求姓名读取确实为strictRead、absent、revision0/digestnull，并经原姓名packet验证原生缺省域。保留原成绩供标题显示，EX资格暂不可得；不写数据库、不从成绩伪造独立姓名、不解锁。损坏记录、非法计数、姓名不一致、存储错误及owner取消仍显式拒绝。

695用生产observeOriginalExtraQualification复现2失败/8正常；696原检查10通过；697最终源码与v4调度绘制、OGG、结局、ECL及Runtime组装/HTTP共89通过，无跳过。存储为显式测试接缝，本轮未读取或修改用户数据库。v4.1核心108和93.3MB资源ZIP保持。

v4.1 ZIP3149669B，SHA2566c3a14df179dd7f2668a09a9b7df499dafa4304cf63c68097b6ef0e3695f4b14。它恢复启动，大范围冻结/密集弹幕性能和小彗星后火球问题仍独立处理中。先停旧服务，再运行新目录Start.ps1并在同8138刷新，无需清存档或重下资源包。
