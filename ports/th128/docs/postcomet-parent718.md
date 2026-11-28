# 小彗星后非符：原始父对象与移动上下文复核

718在不重编core109的情况下，实际1Factory/1World运行Normal600tick。比713直接把攻击子程序当main，更进一步加载原`boss02.anm`、`enemy_bf.anm`、`st_a1_2.msg`与完整`st_a1_2b.ecl`/`default.ecl`依赖；创建真实localenemy，从原Boss的17条/384字节设置前缀进入未改的`Boss2 → Boss2At → Boss2At1`。主入口、出生时刻及跳过前一非符/小彗星的调用是显式测试引导，不冒完整关卡历史。

600次原生tick全部成功。type25火球在167个帧出现；53个靠边移动样本没有出现“速度非零但坐标停住”，共记录317次靠边速度变号，其中35次属于type25。该计数是轨迹事件或出现帧，不能当作唯一弹量、原版逐帧对照或设备FPS。

715首次测试在ECL读取前因新引导的子程序名表错误失败（0World/0tick）；716新引导过早创建Boss，在所有宿主配置好之前要求phase flags（1World/0tick）；717缺少引导ECLI声明，在63tick时找不到已读取但未入依赖图的Boss2。首错误与原源都保留，718分别修正测试引导，无产品源码修改。718有28输入pins、sourcecheck0、fresh MAIN工程准入与ROOT独占once，runtime0/stderr空，ROOT释放。

本次没有创建浏览器标签/服务或运行GPU、读回canvas。既有canvas265/设备执行限制仍保留，不作绕过；实际v5帧率、背景像素与完整非符/参考视频同帧验收仍待完成。v5 ZIP、93.3MB资源ZIP、原生构建和已交付17文件实现均未改变；六线+EX功能基线保持。
