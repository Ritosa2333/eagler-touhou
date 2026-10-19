# 游戏 Package

`package/` 负责游戏资源安装与浏览器持久化。Runtime 的 HTML、JavaScript 和 WASM 独立构建；存档与 Replay 由各作存储接口管理。

| 文件或模块 | 职责 |
| --- | --- |
| `package-descriptor.mjs` | 描述资源组件、字节数与内容身份 |
| `package-generation.mjs` | 规划安装代次与当前/待提交状态 |
| `package-store.mjs` | IndexedDB 中的对象、安装指针与回收 |
| `package-installer.mjs` | 本地/远端安装和原子更新 |
| `package-zip.mjs`、`stored-zip.mjs` | ZIP 读取、写入与完整性检查 |

## 安装与资源版本

新资源准备完整后才切换 current generation。更新失败保持已安装版本，未被任何安装引用的对象才进入回收。

“原版”和“美化版”各有安装槽：原版使用原有 game key，美化版使用 `[game, "beauty"]`。两个槽共享内容相同的不可变对象，分别维护 current/pending 指针。资源版本共用游戏设置、存档与 Replay；运行中切换先完成保存同步，再加载所选版本。多人设置在下次启动生效。

美化资源清单位于站点的 `beauty/final/release-catalog.json`。现代导入包用 `gameVersion` 声明 `original` 或 `beauty`；旧美化 ZIP 按既有文件名规则识别，其他未声明的包使用当前选择。缺少所选资源时显示安装入口。

## 修改与验证

界面调用放在 `app/` 与 `src/`，安装与存储策略在本目录。修改更新逻辑时验证独立安装、对象共用、回收、失败回滚与并发提交；资源版本用 `tests/test-game-version-store.mjs` 检查。浏览器步骤见[测试入口](../tests/README.md)。

旧格式迁移集中在 [`compat/`](../compat/README.md)。完整的 Runtime 与 Host 分工见[架构](../docs/ARCHITECTURE.md)。
