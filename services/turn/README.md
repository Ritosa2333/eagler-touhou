# TURN 配置工具

本目录保存 coturn 配置生成器和环境变量示例。coturn 本体由部署方安装和管理。
渲染工具沿用原启动器的 GPL-3.0-or-later 许可，文本见本目录 `LICENSE`。

按 `turn.env.example` 提供环境后执行 `node render-coturn-config.cjs`，按工具要求指定输出路径。
Relay 的短期凭据使用相同的私有共享密钥。真实密钥和生成的配置留在源码、静态站点及公开产物之外。
公网 TURN 分配、实际双端应用路由和设备验收沿用 deployment-network playbook，目录迁移不宣称这些验证已完成。
