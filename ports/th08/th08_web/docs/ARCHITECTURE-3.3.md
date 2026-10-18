# 永夜抄 3.3：具名 GLES 图形接口与手机顶点上传

与风神录 3.4 共用 `portable/sdl/GraphicsState.hpp` 和 GLES Renderer。参考 TH06 / TH07 的接口分层和网页顶点缓冲存储更新方式，保留两款游戏各自的绘制顺序、矩阵、精灵几何和原版效果。

## 正式运行路径

`AnmRenderer → SpriteBackend → ZunGraphics → SDLGraphics → GLES Renderer → WebGL2`。

图形状态由 `PipelineState` 的具名字段保存，接口使用 `set_depth_compare`、`set_destination_blend`、`set_texture_argument`、`set_fog_range`、纹理句柄、`MatrixKind`、`Topology` 与明确顶点布局。原来的 256 项 render/stage 数字状态数组、FVF 解析和设备状态编号分发已从正式渲染路径移除。SDL3 提供窗口、输入和 GL 上下文，实际绘图通过 GLES / WebGL2。

原始文件里的像素存储格式在资源入口解码为 `PixelFormat`；它们是素材编码，不能删除或解释成需要 Direct3D 运行库。`LegacyGraphics.hpp` 仅供原版画面对照夹具，正式构建如果误引用会直接编译失败。原版测试适配代码留存，以继续验证绘图行为。

CPU 仍按永夜抄自己的 ANM 和坐标规则生成顶点，六顶点精灵按相邻兼容状态合批。保留原有数值精度规则，不改变弹幕运动和碰撞。没有将参考游戏的逻辑直接替换进永夜抄。

## 网页上传

每个 GPU 批次通过带数据的 `glBufferData` 替换顶点缓冲存储，避免同一存储被之前的绘制引用时继续分段 `glBufferSubData`。这与参考 TH06 / TH07 网页分支的处理方式一致。索引和实例缓冲采用相同上传规则；原生构建保留分段上传分支。

`sdl_stats` 的原六项统计保持位置不变，追加缓冲替换次数、分段更新次数和顶点上传字节数。`portable/check-gles-streaming.mjs` 拦截实际 WebGL 调用，以旧版和候选版同输入对照像素、状态、批次数及上传方式；结果保存在交付目录 `validation/gles-streaming.json`。桌面移动视口测试不代表实体手机帧率保证。

阴影遮挡、三面精灵颜色、深度量化、纹理颜色和透明混合仍按游戏原规则处理。正式交付的测试范围和结果以当前 Wasm 对应的 `validation/graphics-summary.json` 为准。
