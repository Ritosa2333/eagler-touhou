# th13 网页版发布打包排除清单（Pre-Release，2026-11-05）

范围：`web/sdl-runtime/` 静态目录发布时的排除项。以下文件是认证/诊断工具面，
代码保留在仓库，**不进入发布产物**：

| 排除项 | 理由 |
|---|---|
| `sdl-runtime/th13-debug.html` | 无头回放取帧/捕获调试页（oracle 驱动面，非用户页面） |
| `sdl-runtime/replays/*.rpy` | 内置 demo/认证录像（版权资产，按原版资产红线不入发布；交互页 `[R3-M]` 导入入口对缺失文件已 404-跳过） |
| `sdl-runtime/serve.py` | 开发服务器（no-store 调试用） |
| `web/artifacts/sdl3-capture/` | 捕获构建变体（仅 oracle 取证用） |

保留（不可排除）：
- 生产构建 `web/artifacts/sdl3/th13-sdl.mjs/.wasm`——`th13_debug_*` 导出族是
  oracle 认证工具驱动面，与生产同一二进制（"生产绘制与 trace 同源"红线）；
  闲置成本为空指针/布尔检查，近零。
- `shell.mjs`/`th13.html` 的遥测（1/s frame-health、audio-health）——性能
  回归观测面，非调试探针。
