# 游戏源码目录

`ports/<作品>/` 是当前游戏适配的维护入口，普通版和 MP 沿用各自源码归属。

| 目录 | 职责 |
| --- | --- |
| `src/` | 游戏源码。TH06、TH07 直接在这里维护逻辑，TH20 按逻辑模块组织。 |
| `web/` | 浏览器 C++ 平台实现、Runtime Shell、接口与本作构建检查。 |
| `portable/` | 构建、打包、SDL 适配与现有专项检查。 |
| `include/` | 本作编译需要的兼容头。 |
| `resources/` | TH06、TH07 的 Runtime Shell 与应用资源。 |
| `vendored/` | TH06、TH07 通过 Git 子模块固定版本的 SDL 构建依赖。 |
| `android/`、`ios/` | TH06、TH07 的平台工程与应用元数据。 |
| `tests/`、`audit/` | 可执行回归检查及其固定输入。 |
| `tools/` | 本作使用的Replay 验证适配器。 |
| `docs/` | 开发步骤、玩法差异和验收记录。 |

构建配方读取 [runtime-builds.json](../../launcher/config/runtime-builds.json)。Node 构建入口支持 `--print-inputs`，直接输出该入口选择的编译单元、头文件与 SHA-256，不启动编译器。

TH20 的当前源码树保留浏览器程序的构建输入、Runtime、验证适配器。原始反编译转储、汇编证据、历史执行报告和独立重建工具通过[固定来源与 Git 历史](../history/README.md)查询。

源码目录按当前构建与检查的职责维护。新增输入同时登记到实际构建或检查入口，生成产物写入被忽略的输出目录。

跨作依赖统一维护：ImGui 位于 `common/third_party/imgui/`，浏览器加载与启动辅助器位于 `tools/browser/`。版本相同的第三方源码共用一份，各作编译参数保留自己的功能选择。根 `.github/workflows/` 调度有效 CI。

浏览器辅助器先读取 Launcher 的 Node Playwright，未安装该模块时使用本机 Python Playwright 自带的驱动。两种 Node 模块解析错误均进入同一回退入口。

[自动适配检查](../../tools/ports/README.md)按源码所有者核对实际编译输入与接口。完整目录可读为：

```text
common/
  contracts/       产品与协议
  include/eagler/  C++ 共用组件
  third_party/    共享第三方源码
launcher/         共用启动器与 Host
ports/<作品>/
  src/            游戏源码（TH06、TH07、TH20）
  web/            Web 平台实现与 Runtime
  portable/       SDL 适配、构建与专项检查
  tools/          本作验证适配器
  docs/           本作开发指南
services/         Relay、TURN、统计
tools/            共用构建、浏览器和检查工具
```

各作原独立 Windows 交付检查、交付摘要与内嵌服务器由当前构建、源码记录及共用 Host 接替。源码检查同时阻止这些退役入口重新进入各作根目录。

共用源码按各作自己的编译参数分别生成目标文件，输出限定在本作构建目录。共用依赖目录保持源码内容，绝对路径与跨目录输入由构建辅助器统一转换为缓存名称。

Replay 与性能回归直接引用当前 common；网络检查使用独立 Relay 入口。依赖旧对象布局或内嵌启动器的单次实验驱动从固定历史查询。历史基准比较要求显式指定原始提交。

Python 性能检查通过 `tools/build/emscripten_sdk.py` 复用显式 SDK，兼容官方 `install` 与 `upstream` 布局。原生检查直接使用 `CXX`，WASM 检查通过 Python 调用 SDK 的编译入口。
