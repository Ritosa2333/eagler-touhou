# 风神录 3.4：具名 GLES 图形接口与手机顶点上传

与永夜抄 3.3 共用新的 `GraphicsState.hpp` 和 GLES Renderer。实际路径为 `AnmRenderer / AnmProjection → AnmRenderEnvironment → ZunGraphics → GLES Renderer → WebGL2`，由 SDL3 提供平台服务和 GL 上下文。

正式构建通过具名的深度、混合、雾、颜色运算、纹理参数、矩阵槽位、顶点布局和拓扑类型连接游戏与渲染器，不再按 D3D8/9 状态编号、FVF 位掩码维护和解释 GPU 状态。`PipelineState` 替代原来的 render/stage 数组；状态比较只检查实际参与绘制的字段。素材的存储格式在资源入口解码。原版对照所需的数字接口仅在比较夹具分支保留，正式构建禁止包含 `LegacyGraphics.hpp`。

网页每批采用 `glBufferData` 提交数据并替换存储，取消分段 `glBufferSubData` 更新。这针对参考 TH06 / TH07 记录的部分安卓 WebGL 驱动瓶颈。两款参考游戏采用此策略的具体来源见永夜抄 `PERFORMANCE-AUDIT-3.2.1.md` 的参考核查节。

保留风神录自己的 C++ 游戏逻辑、精灵坐标计算、绘制顺序、模型批次和已实现的实例化路径；深度、像素对齐、透明和雾效果仍与原版对照。CPU 精度兼容没有被不加验证地替换成普通 float。该改造对齐的是图形接口及缓冲提交方法，不声称两款游戏与 TH06 / TH07 全部内部实现相同。

新增 `check-gles-streaming.mjs` 对照旧版与候选版的实际关卡状态、像素、GPU 批次和上传调用，追加 `sdl_stats` 上传统计。完整结果见当前交付 `validation/graphics-summary.json`；桌面测试不能保证所有手机均为 60 FPS。
