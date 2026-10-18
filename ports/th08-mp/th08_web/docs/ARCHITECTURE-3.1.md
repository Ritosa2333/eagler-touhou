# 永夜抄 3.1：游戏管理器、平台接口与场景预加载

版本：3.1.0-sdl3。保留永夜抄自己的 GameApplication、场景、人物/妖怪切换、弹幕、计分、对话、音乐指令与录像规则；按参考 TH06/TH07 的职责划分连接共享平台。

## 所有者与接口

- `GameApplication` 与 TitleScene、GameplayScene、MusicRoom、ResultScene 等负责游戏生命周期，更新链保存 C++ 函数回调。
- 新的 `GameAudioManager` 负责音效队列、音乐选择、淡入淡出、音乐命令及 MIDI 时序。`SoundDevice` 只负责 PCM 播放资源和 MIDI 设备消息。
- 新的 `ResourceManager` 负责 SDL 流式 DAT、用户文件、路径处理及上限 32 MiB 的解压缓存。
- `PlatformDevices.hpp` 声明 `ZunGraphics`、`SoundDevice`、`FileDevice`；BrowserRuntime 通过这些明确方法连接平台。原编号式 BrowserCall ABI 移到 LegacyDeviceAbi/LegacyDevices，仅用于旧平台与比较构建。

SDL3 后端持有窗口/渲染、音效与 FLAC 音乐、字体和文件设备。浏览器启动器负责包安装、IDBFS 持久化、控制消息及 MIDI 合成设备。MIDI 事件时序仍在 C++，设备合成使用 SpessaSynth，与参考 TH07 的 MidiWeb 分工一致。

## 资源与数值

启动阶段分批准备 `title01.anm`、`music00.anm`、`result00.anm`、`resulttext.anm`，按当前设置的像素格式解析脚本、精灵和纹理。准备缓存上限 20 MiB，避免为未使用的像素格式重复保留大标题资源。Music Room 的常用字形及标题图片继续提前准备。

缓存保存初始模板，实际场景获得独立脚本数据和可写纹理。脚本指针重定位到新数据，精灵绑定新纹理；释放场景不会销毁模板。显示格式不匹配时使用正常加载器。关卡的同步资源创建仍有单次耗时，不把菜单预加载等同于全部加载工作均已异步化。

本轮将游戏层 490 处基础运算、63 处取整改为共享 ScalarMath：正常情况直接使用 C++ float/int32_t。复杂表达式与特殊精度继续使用 Extended/SoftFloat，保持原版中间结果与舍入含义。没有使用 fast-math。

主循环仍按原版 60 次每秒推进。网页触控与风神录共用 `TouchController.hpp`；各自提供人物速度、状态、边界和对话限制。没有增加联机、高刷插值或练习扩展。

## 检验与发行

- 600,000 组数值输入、15,600,000 次比较覆盖精度、舍入、正常范围和浮点边界。
- 四组 ANM、两种像素格式、24 次加载验证：缓存与非缓存的脚本/精灵/像素一致；主动修改后重新加载仍恢复初始状态；分步纹理创建一致，释放后无残留纹理。
- 三次进入/退出 Music Room，与 3.0 稳定版逐像素比较，确认四组预加载资源全部存在且被使用。
- 完整 character0-final 录像按 100,153 tick / 3,339 次状态抽样与已验证的历史轨迹比较。以发行包 validation 内的最终结果和 Wasm 哈希为准。
- SDL 触摸、低速/取消、摇杆、浏览器按钮、声音、存档迁移和离线重开。

桌面测试不等于实体手机测试，有限的轨迹不能证明全部角色、难度与输入组合等价。字体边缘仍存在 SDL_ttf 与 Windows GDI 的差异。

构建和发行命令沿用 `ARCHITECTURE-3.0.md`。正式包位于 `artifacts/sdl-release`，本地 8088，兼容旧 C++ 启动入口 8092。当前公网 8090 继续提供风神录。
