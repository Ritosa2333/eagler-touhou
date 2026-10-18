# OptionInf 源恢复

真实0xa4字节对象、唯一5c60b8存储，完整构造/注册/析构、6项设置状态机和绘制。窗口模式直接写Window状态；音量应用保留原4e0fc0使用music_level计算音效衰减的行为。关闭保存真实th20.cfg；析构写失败诊断使用已知路径替代原缺失的%s实参。

cpu_validation.json：393216项原CPU对照通过，包括32768个完整update帧与全部65536种音量字节组合。音频排队、屏幕效果、设备重置、KeyConfig工厂、磁盘保存及对象销毁采用明确服务边界；并不表示这些外围生命周期已再次验证。

draw_validation.json：18432项新增检查，含46b590样式及Options/KeyConfig三绘制入口。完整Renderer和两真实缓存Job比较，原CRT格式与新C++实际运行。该数同时包含在text_renderer总57113中，不能重复相加。绘制检查未覆盖新glyph上传。

KeyConfig依赖已由key_config模块实际实现；没有返回成功的空实现。构造/析构/factory仍只编译，需后续生命周期差分。
