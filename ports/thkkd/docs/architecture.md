# 网页架构

游戏由 C++ 核心、显示计算与浏览器平台组成。`src/core` 中的 GameState 持有菜单、关卡、录像和资源状态，CombinedGameHost 按固定 60 Hz 推进。`src/render` 根据逻辑状态生成绘图调用；`src/platform/web` 将平台调用接到 JavaScript，`web` 负责 WebGL2、音频、输入、存储和启动器协议。

## 数值与状态

游戏计算沿用 24 位有效数规则、字段别名、随机顺序与写入顺序。`WebMemory.hpp` 和 `IndexedGameMemory.hpp` 为原地址访问提供索引，跨区间访问继续遵守原有边界和别名行为。数学、随机数、状态布局或调度调整时运行相关原版录像回归。

`src/platform/web/LegacyDisplayData.hpp` 保存显示计算使用的三张只读索引表，共 79 字节，用于选择弹型、机体武器与 Bomb 显示分支。显示函数通过 `DisplayState` 读取这些数据。

## 图形与媒体

逻辑画布为 640×480，浏览器按 4:3 比例适配窗口。WebGL2 保留原绘制顺序、双线性过滤、透明及加减混合；原生 alpha 与独立遮罩按原规则组合。整帧顶点一次上传，再按原批次绘制。高刷新呈现读取相邻逻辑帧的命令快照，额外画面只更新绘制。

字体由 `text.mjs` 维护测量与栅格缓存，随程序携带 GNU Unifont 字形。背景视频在需要绘制时更新纹理，音乐与音效使用独立音量。暂停、失焦、录像定位和退出通过统一生命周期挂起或释放媒体。

## 导入与用户数据

启动器接受同时选择的 `dat.dxa`、`bgm.dxa`，或包含它们的 ZIP。DXA 解码后核对 327 项资源的哈希，图片与视频在本机转换，原版 MP3 和 WAV 保留字节。安装结果进入共用 Package Store，后续启动直接读取本地缓存。

五份原版 DAT 存档、`config.txt` 和 Replay 位于独立的 `/savesthkkd`。原生文件关闭后排队同步 IndexedDB，导入、退出和备份使用显式同步屏障。测试使用独立浏览器配置与合成用户数据。

原版 `.rpy` 按原输入流播放；自由坐标等扩展操作写入 `.thkkdr`。录像播放器共用原生状态机，支持播放、暂停、快进、定位和结束状态。回放期间实时操作输入与录像输入分离。

构建与测试入口见 [DEVELOPMENT.md](DEVELOPMENT.md)，显示细节见 [RENDERING.md](RENDERING.md)，资源和交付规则见 [packaging.md](packaging.md)。
