# TH128 thprac：练习录像与播放锁

练习局现在有独立的真实原生录制owner。它绑定当前stage/chapter/section/难度/seed与五个初始资源，在实际WorldExports成功tick记录原held/pressed/released三组16位输入，使用原NativeReplayInput格式。普通录像仍要求原路线起点，不放宽正常成绩与解锁资格；练习录制不取得normalRecord。暂停/非法时钟/重复clock、reset与旧owner隔离有实际消费者检查。

文件仍是实际TH128 RPY：原生录制数据经原压缩codec编码，追加固定上游ReplaySaveParam的USER/PRAC区块。其magic0x52455355、kind0x43415250，UTF8原THPracParam JSON后补零，整块4字节对齐；保原USER对与压缩前输入字节。额外_web域绑定core/resource SHA，仅证明一致性，不证明发布者身份。元数据没有伪造成普通原版参数或JSON改名录像。

页面练习菜单提供姓名、25槽选择、明确覆盖确认、保存与观看。保存使用现有strict-CAS事务及完整文件回读；观看恢复录像内参数和seed，走练习专用native输入绑定。playLock拒绝现场玩法输入与辅助修改，重播/取消有独立owner。当前姓名明确为1..8字节ASCII，不写个人原姓名档案。

验证：921完整WorldExports152断言/3start调用，真实3帧录制→nativeRPY→fresh同参数3帧回放，移动结果一致；925实际完整页面save/watch与codec/store11项、927受影响191项0skip；928真实nativeRPY存储回读后packet精确一致；934证明原USER对加PRAC第三块保持原字节。新114首次构建231源/83TU/214声明导出，成功后不重复构建；113及旧交付包不动。

930真实114的2Factory/2World/6tick/294检查，原DAT/ECL/ANM/SHT/MSG/geometry/子弹/玩家资源均真实。录制保存成RPY+PRAC后，经strict模拟IDB关库重开，实际native回放恢复同13项投影（含playerY391、RNG0、score123456789、power6804）。这是3帧有界确定性证明，不是完整原录像617或整关。933真实浏览器10项、32.867秒、console0，实际页面/组件/真实IDB完整No.13文件/CAS/覆盖/重开/观看/退出通过；native capture/start/pause仍是显式边界，不与930合称完整浏览器游戏/FPS。

本批只有私有114编译产物，未部署旧772/个人页、本地v7或公开页；不重新打包或公开发布。**完整thprac与第三阶段仍未完成**：Backspace/F1..F7辅助工具、F12高级速度和启动器持久开关接下来继续。历史首FAIL与边界、原上游参考字节归一化和1项旧工具指纹未解见thprac128-Replay915-934.json。
