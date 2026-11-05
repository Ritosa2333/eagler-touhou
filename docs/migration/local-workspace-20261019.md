# 本地工作区归并 | 2026-10-19

房间、标题预加载与 th075 普通版、多人版采用同一个 monorepo 开发入口。目录和模式统一由 `launcher/config/workspace.json` 与 `runtime-builds.json` 声明，游戏直接读取仓库内的 `common/`。

## 源码归属

归并目录以 upstream/main `b7e322a3` 为基线。房间、旁观、Replay、设置草稿与预加载分别进入启动器的对应模块。

| 内容 | 来源 | 当前目录 |
| --- | --- | --- |
| th075 普通版 | `Goan114/th075` | `ports/th075`，来源提交 `66405aeabff2194a7e4b40dd981fdc944262c6f2` |
| th075 多人版 | `Goan114/th075` | `ports/th075-mp`，来源提交 `e02cf30ce021ff4ce0e1d302eac9516a4a98a343` |
| 游戏公共运行库 | 本仓库 `common/` | 编译和打包核对实际输入哈希，包含联机校准文件 |
| 启动器与交付工具 | 本仓库 `launcher/`、`tools/` | 共用产品、资源、构建与发布入口 |

th075 原始来源历史位于 `refs/archive/workspace-20261019/` 的 `th075-normal` 和 `th075-multiplayer`。源码保留 MIT 许可证和作者归属。历史验收文件保留原始哈希，读取时将 `th075_web/` 映射到当前 `web/` 目录。

## 工程验证

工作区检查覆盖 17 个 Runtime 源码构建计划、9 个作品的协议契约、目录归属与发布源码审计。启动器生产构建、类型检查，以及房间、Runtime、开发服务器的 121 项回归通过。

th075 普通版与多人版在当前目录完成编译和打包。多人原生检查覆盖回滚、D=0/1/2/9、丢包重排和冲突状态中止；真实双端浏览器检查分别验证回滚与纯延迟，在不同标题等待时长后获得一致的对局状态，并复用已加载实例。全作源码检查与标题时钟检查的结果由 `docs/adaptation/status.json` 和 `artifacts/validation/ports/` 提供。
