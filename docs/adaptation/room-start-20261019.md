# 房间预加载转开局修复 · 2026-10-19

## 范围与原因

源码基于 `2b796124`，包含本轮未提交修复；模式为 multiplayer。普通版未重新发布。

TH06、TH07 在房间标题预览时已经报告过首帧，转为正式游戏后没有再次报告。游戏实际运行，但启动器仍等待开局首帧，保留加载遮罩和输入锁。现在复用预加载实例时请求一次呈现确认，仅在下一次实际 `Present()` 后报告，不使用命令应答代替画面呈现，不改变逻辑帧、输入时序或 RNG。

TH08 的房间预加载没有开局合同，初始化却只接受正式开局、预检或 Replay。现在由 `prepare-room` 显式选择内部标题预览角色，保留正式开局身份校验。浏览器测试也移除了先前人为添加的预检配置，覆盖启动器真实配置。

## 验收

检查对应 RUNTIME-02、MP-03、MP-04、MP-05。

- TH06、TH07 Web MP 构建成功；TH08 MP 重建并打包成功。
- `launcher/tests/browser/test-room-title-preload.mjs`：TH06 mode 0、TH07 mode 0、TH08 mode 0/1 均通过真实 Runtime 双实例检查。标题等待不进入 Demo，复用实例开局，不重新下载 DATA/WASM，并比较相同逻辑帧的世界哈希。
- TH06、TH07 每次开局必须新增且只新增一次真实首帧事件；浏览器输入桥按下、释放均通过。
- 同一 TH08 测试运行旧 Runtime 可复现用户报告的初始化错误，新 Runtime 通过。
- 启动器 Runtime/多人服务单测共 118 项通过，新增回归检查保证预览首帧及 launch 应答均不能提前解除正式开局等待。

浏览器原始结果位于本地 `launcher/.cache/room-title-preload/<game>-<mode>/result.json`。以上为真实 Runtime 协议测试与服务单测，尚未据此宣称公网完整双客户端 Launcher 人工验收通过。

## test 发布与回滚

SSH 发布版本：`room-start-fix-20261019-c8daa36f4c64`。App Shell：`9aec0e61bc812e4a48f0`。仅更新三作 MP Runtime 及关联清单；main、beta 和联机服务未变。

保留上一版本链接 `rollback-before-room-start-fix-20261019-c8daa36f4c64`。资源节点未同步，游戏包仍手动导入。TH08 两个程序编码表以本次 Runtime 的精确 URL 由 test 提供，公开请求均为 HTTP 200，大小及 SHA-256 与构建文件一致；其他资源路由保持原样。

发布操作记录位于服务器 `/var/tmp/room-start-fix-test-20261019/`。回滚时同时恢复版本链接及 Nginx 配置；`program-tables.py --restore` 校验配置身份后恢复 `nginx-test-before.conf`。固定和旧别名 Service Worker 公开内容一致。
