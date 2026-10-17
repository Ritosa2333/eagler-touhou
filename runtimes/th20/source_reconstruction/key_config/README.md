# KeyConfigInf 源恢复

真实0x114字节对象、唯一5c4d28存储，恢复完整设备页和按键页的update/draw、真实工厂/注册/析构。直接使用input::Controller的devices、selected及mappings；按键重复判定采用原pressed|repeat8。键重复分配会交换旧绑定，保留原键盘与DirectInput/XInput的不同等待、提交与默认分支。

cpu_validation.json：32768完整主帧、229376断言通过。比较整对象、真实输入Controller、Configuration、预分配deque历史块、返回值与事件序列。音效、系统设备重枚举和对象退休为明确外部边界；原Cursor历史函数在无需扩容的真实块上完整执行。

两页绘制与公共样式已在../options_system/draw_validation.json联合通过；与text_renderer报告重复收录，不重复计数。原CRT字符串格式、整Renderer和缓存Job均实际比较，不代表新的GDI图集上传或全游戏完成。生命周期当前编译通过，尚未整入口CPU比较。
