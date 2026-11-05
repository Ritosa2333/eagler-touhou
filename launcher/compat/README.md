# 旧数据迁移

这里集中维护旧安装数据与当前 Package Store 之间的转换。当前导入、安装和更新由 [`package/`](../package/README.md) 负责。

| 兼容输入 | 处理方式 |
| --- | --- |
| `game-data-pack/1`、`offline-game-pack/1` ZIP | 读取旧格式并安装到当前 Package Store |
| `eagler-touhou-game-data-v1` Cache Storage 与元数据 | 启动时迁移；成功后清理对应旧记录 |
| 仅有 16 位 revision 的旧安装对象 | 更新时计算完整 SHA-256，哈希与字节数匹配后复用 |
| 旧 Host Manifest 文件名与 schema | 由当前 Site Manifest 校验后派生兼容响应 |
| 旧首次使用确认键 | 由 `src/contracts/compat/first-use-notice.mts` 读取 |
| 固定 URL 的 Runtime 缓存 | 由 Runtime generation reader 按内容身份读取完整快照 |
| 已发布的 `legacy/*.mjs` URL | 由 `path-aliases.mjs` 映射到本目录 |
| `EAGLER_FRONTEND=react` | 部署预检接受旧设置；当前前端由根 launcher 提供 |

## 修改步骤

1. 为待迁移的旧输入加入真实格式夹具。
2. 验证迁移结果可由当前读取器使用，并覆盖中断后的重试。
3. 检查存档与 Replay 文件内容保持一致。
4. 更新升级说明与对应迁移测试。

退役兼容入口时，先明确新的最低升级版本，并提供跳过该版本用户的导出恢复路径，再一并移除读取器、调用方与测试。短 revision 兼容在最低版本越过 2026-09-12 前的旧离线包、并经过一个正常发布周期后退役。
