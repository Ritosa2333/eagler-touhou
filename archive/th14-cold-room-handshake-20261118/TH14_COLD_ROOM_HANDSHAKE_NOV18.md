# TH14 冷启动房间握手

TH14 多人版在原生标题资源加载结束后才进入会话泵。握手计时改为从首次会话泵开始，使冷启动加载与后续 45 秒连接窗口各自完成。

## 实现与构建

`RoomTh14::begin` 继续创建浏览器传输、设置房间身份、输入核心与录像；`poll` 首次执行时启动会话握手。房间选项、原生加载顺序和观战入口保持现有实现。普通版没有此多人会话路径。

候选 Wasm `34791e87b7cf8e7cce96e48a12deba941830cc273cd2c2b29ae6b6932b7f16b5` 重编 1 个多人单元、复用 162 个对象；395 份作品及 73 份公共绑定全部与候选源码匹配。构建不含诊断导出。候选以独立运行包验收，生产保持 `12219d17`。

公共浏览器测试服务提供 COOP／COEP／CORP，支持当前 Worker 的共享内存；无头浏览器使用 `--mute-audio`。准备等待读取最新房间 `state`，避免把其他消息当成房间状态。

## 验证

实际 `RoomTh14` 原生测试使用受控传输模拟入房后 59,990 毫秒加载：旧代码在首次 `poll(60000)` 失败，候选发出会话包，并在其后第 45 秒按原策略超时。测试不读取游戏资产。

候选包通过独立浏览器双人 WebRTC 开局，两端达到 184 帧，随后分别推进至 613／631 帧。观战端已加入并收到同一会话身份；历史播放等待超时，位置为 0。继续定位观战发布／接收与原生初始化，当前不标记多人集成完成。

浏览器测试音乐为 None，Chromium 强制静音。私有诊断导出、截图、运行二进制和游戏数据均不在发布清单。数值、构建身份与报告哈希见[验证记录](th14-cold-room-handshake-nov18.json)。TH14 目标保持可观察行为级对齐，整作进行中。

## 原生回归入口

`ports/th14-mp/tools/tests/test-th14-room-handshake.cpp` 链接实际房间与公共会话实现；传输替身只提供连接和收发边界。在仓库根目录使用具备 32 位 C++ 库的工具链：

```sh
clang++ -m32 -std=c++17 -O1 -DPORT_TITLE=14 -DTH14_MULTIPLAYER=1 -DTH_NATIVE_PLATFORM=1 \
  -ffunction-sections -fdata-sections -Icommon/include -Icommon/third_party/imgui \
  -Iports/th14-mp/runtimes/th12/th12_web/cpp/multiplayer \
  ports/th14-mp/tools/tests/test-th14-room-handshake.cpp \
  ports/th14-mp/runtimes/th12/th12_web/cpp/multiplayer/RoomTh14.cpp \
  common/src/netplay/{NetplayCore,NetplayProtocol,NetplaySession,SessionChannel,InputReplay}.cpp \
  -Wl,--gc-sections -o /tmp/th14-room-handshake
/tmp/th14-room-handshake
```
