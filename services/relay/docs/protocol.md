# 客户端协议入口

服务支持多个壳。客户端通过 WebSocket 使用房间目录、等待室、信令和游戏传输接口。
静态的产品规则、快捷聊天 ID、昵称规范和时序结构来自 `common/contracts/`，没有前端 UI 依赖。

现行接口保留原有字段和兼容行为，不由目录搬迁创建第二套协议：

- `?directory=1&member=...` 获取目录消息。
- `?room=PRODUCT-ROOM&lobby=CLIENT&member=MEMBER` 进入等待室。
- 等待室使用 `take-seat`、`settings`、`set-ready`、`start` 等消息。
- 已发布联机程序的准备消息携带验证后的 `runtimeGeneration`。服务把一致的版本固定到 run，随后信令和游戏连接须携带同一标识。
- 信令、玩家和旁观连接沿用现有 run、player、spectator 和 member 字段及其授权检查。
- 昵称身份、客户端自报字段与 Origin 均不能替代认证；服务继续执行限流、角色和成员校验。

可执行的接入参考位于 `tests/`，包含独立 WebSocket 客户端、版本协商、成员权限、定向转发、旁观、快捷聊天及昵称 Worker/SQLite 生命周期。
这些测试不需要 Launcher。现有跨作品旁观语义说明继续由仓库 playbook 链接，不能把服务协议通过当作游戏状态恢复通过。
