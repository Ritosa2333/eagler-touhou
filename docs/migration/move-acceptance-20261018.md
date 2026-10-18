# 搬迁后验收

本次从已提交并与远端同步的来源开始搬迁。Launcher 只继承原 `main`。
`ports/` 保留各作普通版与 MP 源码归属，`common/` 为默认共享源码。
Relay、TURN 支持和 Telemetry 分别位于 `services/`，公共定义在 `common/contracts/`。

正在重新执行搬迁后验收。目录与服务检查、真实编译、打包和实际游戏行为分别记录，任何一层通过均不代替其他层。TH08 MP 先前失败必须在刷新后的实际 WASM 上重测。结论更新前不视为正式切换通过。

完整本地证据位于工作区 `artifacts/validation/mono-20261017/move-20261018/`。
搬迁前记录保留在同级 `refresh-20261018/`，不作为搬迁后通过证据。
