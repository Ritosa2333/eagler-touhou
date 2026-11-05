# 房间预加载与首帧确认 | 2026-10-19

## 原生呈现与输入解锁

多人房间预先加载原生标题，开局复用同一实例。正式对局的首帧确认独立于标题预览，画面呈现后启动器解除遮罩并接通输入。

th06、th07 在复用标题实例开局时请求一次呈现确认，由下一次实际 `Present()` 报告。标题首帧、命令应答和对局首帧分别对应各自阶段，呈现确认沿用原有逻辑帧、输入时序和 RNG。

th08 通过 `prepare-room` 选择标题预览角色，正式开局继续校验双方确认的对局身份。浏览器检查直接使用启动器的房间配置，覆盖标题预览到对局的完整协议链。

## 验收

检查对应 RUNTIME-02、MP-03、MP-04、MP-05。

- th06、th07 Web MP 构建成功；th08 MP 构建和打包成功。
- `launcher/tests/browser/test-room-title-preload.mjs`：th06 mode 0、th07 mode 0、th08 mode 0/1 通过真实 Runtime 双实例检查，覆盖标题持续等待、同实例开局、DATA/Wasm 保留和同逻辑帧世界哈希一致。
- th06、th07 每次开局产生一次真实首帧事件，浏览器输入桥的按下与释放正常。
- th08 初始化回归使用相同配置对照修复前后 Runtime，确认标题预览角色正确进入原生初始化。
- 启动器 Runtime/多人服务单测共 118 项通过，新增回归检查保证预览首帧及 launch 应答均不能提前解除正式开局等待。

浏览器原始结果位于 `launcher/.cache/room-title-preload/<game>-<mode>/result.json`。真实 Runtime 双实例检查验证呈现、输入和同步，服务单测验证启动器阶段切换与等待条件。

## test 发布与回滚

测试版本：`room-start-fix-20261019-c8daa36f4c64`。App Shell：`9aec0e61bc812e4a48f0`。交付内容为三作 MP Runtime 及关联清单。

回退入口为 `rollback-before-room-start-fix-20261019-c8daa36f4c64`。游戏包通过本地导入安装，th08 两个程序编码表按本次 Runtime 的精确 URL 提供；公开请求、文件大小及 SHA-256 均与构建记录一致。

发布操作记录位于服务器 `/var/tmp/room-start-fix-test-20261019/`。回滚时同时恢复版本链接及 Nginx 配置；`program-tables.py --restore` 校验配置身份后恢复 `nginx-test-before.conf`。固定和旧别名 Service Worker 公开内容一致。
