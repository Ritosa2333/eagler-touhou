# 启动与资源

[适配流程](README.md) · [文档目录](../README.md)

Runtime 是编译后的游戏程序和浏览器接口。启动器负责界面、安装与资源准备，Runtime 负责游戏循环和本作文件。

## 准备

按游戏开发指南生成 Runtime，在启动器中导入本地游戏数据。打开浏览器开发者工具，同时查看 Console 和 Network。协议字段见[Runtime 接口参考](../../launcher/docs/GAME_ADAPTER_CONTRACT.md)。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="runtime-01"></a>RUNTIME-01 · 协议身份与会话代次 | 从启动器启动、退出，再启动；检查协议、作品、origin、source 与 `runtimeEpoch`。 | 每次实例只处理属于自己的消息。 |
| <a id="runtime-02"></a>RUNTIME-02 · 就绪、配置、启动、首帧 | 依次观察 `ready → configure → launch → first-frame`。 | 配置完成后开始游戏；画面呈现后才报告首帧。 |
| <a id="runtime-03"></a>RUNTIME-03 · 命令响应与错误信息 | 调用配置、文件和输入命令，再用一个无效参数调用。 | 请求收到对应结果，错误可由启动器显示。 |
| <a id="runtime-04"></a>RUNTIME-04 · 后台、恢复与退出清理 | 切换前后台、退出、再次进入；中途保持按键和触点。 | 返回后声音和画面可恢复，退出后监听器与临时输入已释放。 |
| <a id="runtime-05"></a>RUNTIME-05 · Runtime 与资源管理分工 | 从启动器安装资源，再启动 Runtime。 | 启动器管理安装和下载；Runtime 读取已提供的资源。 |
| <a id="runtime-06"></a>RUNTIME-06 · 重开与常显判定点 | 暂停后按 R 重开；分别开关常显判定点并移动、低速。 | 重开进入本局初始状态；判定点沿用本作视觉，移动速度与碰撞规则正常。 |
| <a id="package-01"></a>PACKAGE-01 · 共用游戏包存储 | 从启动器安装并查看本地游戏包。 | 资源进入共用 Package Store。 |
| <a id="package-02"></a>PACKAGE-02 · DATA 身份与挂载目标 | 核对 DATA 文件、挂载路径与清单身份。 | 实际字节、哈希和 Runtime 声明一致。 |
| <a id="package-03"></a>PACKAGE-03 · 完整更新后切换版本 | 在更新中断开下载，再重新打开游戏。 | 使用上一完整版本；恢复更新后才切换。 |
| <a id="package-04"></a>PACKAGE-04 · 本地导入与在线安装一致 | 分别用在线安装、本地 ZIP 和支持的原版文件导入。 | 进入同一套游戏包与版本管理流程。 |
| <a id="package-05"></a>PACKAGE-05 · 已安装游戏离线启动 | 首次安装并运行后断网，刷新或重开页面。 | 启动器、Runtime 和所需本地资源均能加载。 |
| <a id="package-06"></a>PACKAGE-06 · 统一安装和更新入口 | 检查 Runtime 的安装、缓存和更新调用。 | 安装状态由启动器维护，游戏读取已安装资源。 |

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[消息协议](../../launcher/src/contracts/runtime-protocol.mts) · [数据布局](../../launcher/lib/content-definition.mjs) · [包管理](../../launcher/package/README.md)

## 安装代次与后台更新

Package Store 在领取暂存区和切换当前版本的 IndexedDB 事务中核对预期代次。后台 OGG 下载和共享字体升级携带所属安装代次，多个标签页同时导入或更新时，以完整的新安装为准。Web Locks 用于协调并发，持久化代次检查覆盖没有 Web Locks 的浏览器。

更新提示绑定当前 Runtime 实例。新版本完整下载并校验后才提示，关闭或替换实例后，其过期检查不会占用后继实例的通知标识。启动时先补齐必需资源，再固定 DATA 身份。

对应检查：[`test-package-generation-conditions.mjs`](../../launcher/tests/test-package-generation-conditions.mjs)、[`game-preparation.test.mjs`](../../launcher/tests/react-main/game-preparation.test.mjs) 与 [`runtime-service.test.mjs`](../../launcher/tests/react-main/runtime-service.test.mjs)。
