# common/

`common/` 是仓库内共享源码的默认维护位置。它提供 C++ 联机与回滚组件、浏览器模块、客户端和服务端共用的 JavaScript 契约，以及可复用的测试工具。

## 开始

需要 CMake 3.20、C++17 编译器；安装 Node.js 后，CTest 也会运行可用的 JavaScript 测试。从仓库根目录执行：

```sh
cmake -S common -B build/common -DEAGLER_COMMON_BUILD_TESTS=ON
cmake --build build/common --config Release
ctest --test-dir build/common -C Release --output-on-failure
```

## 组件位置

| 目录 | 用途 |
| --- | --- |
| [`include/`](include/) 与 [`src/netplay/`](src/netplay/) | C++ 共享接口和实现；由各游戏 Runtime 按需接入 |
| [`browser/`](browser/README.md) | 浏览器输入、回放、旁观时间轴和启动画面模块 |
| [`contracts/`](contracts/README.md) | Launcher、Relay 与客户端共用的产品和协议规则 |
| [`testkit/presentation-lab/`](testkit/presentation-lab/README.md) | 固定步模拟与绘制纯度检查 |
| [`testkit/replay-verifier/`](testkit/replay-verifier/README.md) | 标准化 Replay 轨迹采集与比较 |
| [`tests/`](tests/) 与 [`tools/`](tools/) | C++/浏览器行为测试和生成工具 |

## CMake 组件

| Target | 提供内容 |
| --- | --- |
| `eagler::netplay_headers` | 联机公共头文件 |
| `eagler::netplay_base` | Protocol、Core、Session 与 WebSocket 基础实现 |
| `eagler::session_channel` | 基于 `PeerTransport` 的会话通道 |
| `eagler::rollback` | 通用回滚存储 |
| `eagler::browser_peer_transport` | 浏览器 Peer transport |
| `eagler::netplay_input`、`eagler::input_replay` | 逻辑输入与 Replay 文件编解码 |
| `eagler::thprac` | 可选 THPrac 输入、HUD 和练习工具 |

共享实现放在这里；游戏专属的对象布局、状态覆盖、原生 Replay 语义和生命周期由对应游戏适配层维护。改动适配边界时，按 [开发指南](../docs/README.md) 找到对应组件的检查项。

## 下一步

- 改客户端或服务端共用规则：先读 [`contracts/README.md`](contracts/README.md)。
- 改浏览器模块：按 [`browser/README.md`](browser/README.md) 运行生成检查和行为测试。
- 改诊断工具：先读对应 testkit 的 README 与契约。

C++ 共享库使用 [MIT License](LICENSE)。`contracts/` 沿用其 [`GPL-3.0-or-later`](contracts/LICENSE) 许可。

各作 thprac 共用的 ImGui 源码与许可证位于 `third_party/imgui/`，构建直接引用这一份源码。
