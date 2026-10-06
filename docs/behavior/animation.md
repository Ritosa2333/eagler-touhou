# 动画、场景与界面行为

## ANM

454d10 reset后立即执行首个455630；454ee0保留部分状态重绑。primary tick27，secondary tick8；
嵌入VM由各自物理对象更新，绘制和只读查询不能推进逻辑或RNG。
461970给根与其直接child传播排队中断；4619e0同时立即tick。不能只改根，也不能无证据递归全部后代。

位置由script、engine、额外script offset三组相加。MSG opcode25写额外offsetY。
ANM96/97先构造并tick child，再读取父参数写额外XY，随机顺序必须保留。
ANM102固定game RNG；87选择domain不能改变此例外。负sprite回退ascii264。

## 场景与对话

Stage初始化入口402a40，基础构造402970。STD隐藏相关背景层不意味着暂停相机／脚本。
flags8完全返回；flags4且transitionAge>=30跳过object／STD，>=60完全返回。
camera最终位置还加offset。

MSG41fcc0处理四个独立动态文字矩形及肖像、窗口中断。原文为CP932；字体、spacing、
COLORREF、位置、alpha和进出动画都应来自原版字段。浏览器字体栅格化尚非原版GDI像素Oracle。

## Replay与存档

43c730跨面恢复RNG seed／calls、scoreStored、power、PIV、两个独立signed16残机字段、
两个独立signed16Bomb字段，以及按422e80重建的三槽UFO。
相邻16bit字段不能读成一个32bit值，跨面不能随意清空库存。
完整菜单、存档编码、音频消费线程及端到端回放仍需继续恢复。
