# 背景物件、网格与帧内验证

`object_draw.cpp` 和 `object_projection.cpp` 是独立 C++ 实现，生产路径只使用恢复后的动画/绘制代码及系统 D3DX9。原游戏函数只在 `sprite_renderer/pool_cpu_compare.cpp` 的隔离测试映射中执行；不运行原程序入口，不修改其函数指令。

| 原地址 | 源码入口 | 原 CPU 对照范围 |
|---|---|---|
| `473b20` | `cull_object` | 4,096 组距离及 16 点投影裁剪 |
| `4d9f60` | `update_perspective_camera` | 512 组完整相机与两次矩阵 COM 调用；含零/接近零方向 |
| `477520`, `4750a0`, `474780`, `474880` | 相机选择、缓存清理、内嵌动画及 `draw_object_layer` | 400 组完整 Background、Graphics、物件、实例、动画、几何缓冲、顶点及 COM 调用；49 个动画绘制模式进入实际 C++ 分发 |
| `4751b0` 的操作 14、19 | `execute_script` + 生产 `vm_adapter.cpp` | 各 384 组动画绑定/中断；标签命中、默认标签及无匹配路径；完整状态、primitive 动画和文件计数 |
| `4751b0` 的操作 17 | `execute_script` + `reset_meshes` | 48 组两个实际网格、动画句柄、池状态、字段和文件计数，含图形资源不可用路径 |
| `4722e0` | `update_mesh_distortion` | 512 组完整状态、顶点、位置与 strip 缓冲；保留先复制 strip、后修改主顶点的顺序 |
| `472100`, `4739e0` | `Background::update`, `update_objects` | 1,024 组完整 Background、物件、primitive 动画、全局相机和返回值；包括停用/淡出边界与 16 种相机运动 |
| `472d10`, `472a30` | `draw_geometry`, `draw_foreground` | 各 400 组完整对象、Graphics、动画、顶点、状态缓存及矩阵/Viewport/Clear/绘制调用与返回值 |

本次集成还将 `4d9db0`、`4ddf80`、`4dda60` 三个现有状态控制函数迁入 `platform_window/draw_controls.cpp`。公开 API 不变，原来的 `reset_sprite_queue` 薄适配直接调用相同的 `flush_textured_quads` 实现。

可复跑报告为 `../sprite_renderer/pool_cpu_validation.json`，当前总计 **239,658** 项通过，含此前池、动画绘制和网格验证。该 JSON 内含原 EXE SHA256 及本次实际编译的每个源文件 SHA256；总数是多字段检查数，不是恢复函数数，也不是完整游戏进度。

在项目根目录执行：

```powershell
& D:\cmake\bin\cmake.exe --build source_reconstruction/sprite_renderer/pool_test/build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw 'CPU test compilation failed' }
$manifest = Get-Content reports/source_manifest.json -Raw | ConvertFrom-Json
& source_reconstruction/sprite_renderer/pool_test/build/Release/th20_pool_cpu_compare.exe `
  (Join-Path $manifest.source_directory 'th20.exe') `
  source_reconstruction/sprite_renderer/pool_cpu_validation.json
```

这些比较使用 COM 记录器，证明给定有效输入域中的状态/顶点/调用一致，未证明整场游戏逐帧画面一致。网格比较仅归一化两次独立分配产生的指针地址，不归一化字段数值或浮点误差。`draw_geometry` 的淡出计时小于 30 且创建 ScreenInf 的分支由此测试显式排除，若误入会抛异常；ScreenInf 的生产实现存在于 `screen_effect/`，由其单独验证覆盖。当前背景前景测试的全局动画层 39/40 列表为空；物件和内嵌动画仍执行完整实际绘制。异常文件、并发生命周期和整局玩法不在此报告覆盖范围内。
