# 永夜抄 3.2：明确图形命令与驻留菜单纹理

对应本轮风神录 3.3，按 C++ 参考 TH06/TH07 的共同分层方式整理。参考 TH06 的图形接口名为 `GfxInterface`，TH07 为 `ZunGraphics`；两者都是具名管理器、原生回调、图形接口和 SDL3 平台，接口与内部游戏结构本来就不完全相同。详细横向对照见同包 `REFERENCE-ARCHITECTURE.md`。

## 游戏与平台边界

继续由 `GameApplication`、各场景、`GameAudioManager` 和 `ResourceManager` 管理永夜抄的生命周期、音乐指令与资源。更新链使用 C++ 函数回调，`PlatformDevices.hpp` 的 `ZunGraphics`、`SoundDevice`、`FileDevice` 连接 SDL3 后端。原版兼容 ABI 留在独立的 Legacy 分支。

本轮动画、关卡背景和符卡背景改用共享 `portable/sdl/RenderCommands.hpp` 的具名图形命令，包含深度、雾、颜色、纹理参数和混合设置。命令在平台边界映射到原有固定功能含义，保持调用顺序与画面结果。渲染后端仍需要支持原版的图形规则；不是只把接口改名就认为四款游戏的实现完全一样。

## 菜单资源的生命周期

`AnmLibrary` 预先解析 `title01.anm`、`music00.anm`、`result00.anm`、`resulttext.anm`，并为非空白纹理保留实际 TextureStore 记录，通过 `ZunGraphics::prepare_texture` 提前上传 GPU。释放场景后保留静态纹理，下次加载可重新借用。

每个活动场景仍获得独立的可写脚本和精灵数据。空白文字/截图纹理独立分配；两个同时存活的 ANM 实例不会共享可写纹理；被场景改写过的缓存纹理在复用前恢复初始像素。`clear_preloads` 先释放引用模板的场景，再释放驻留纹理，避免悬空指针。

模板上限为 20 MiB，静态纹理的驻留 CPU 像素及 GPU 存储另计，因此这不是全部缓存或游戏总内存上限。只保留当前格式、固定的四组菜单动画。Music Room 字形和标题图片仍有预热，关卡继续使用已有分步任务；仍存在单次同步资源创建，不宣称所有加载均无等待。

## 普通 C++ 数值与触控

已接入的基础 float/int32_t 快速路径继续保留。本次 `ScalarMath` 扩展整数乘法、乘后取整与加减缩放，TH08 慢速计时采用它；常规单精度、最近舍入时使用普通 C++ 运算，特殊情况对整个表达式使用原来的 Extended/SoftFloat。复杂表达式保留其精度语义，不用 fast-math。

SDL3 循环、音频、文件、字体和共享触控继续有效。永夜抄自己提供人物/妖怪切换、速度、边界、对话和按键规则，不复制别的游戏逻辑。按用户要求不添加联机、练习扩展或高刷插值，保持原版 60 Hz 更新。

## 验证与发行

- 数值：600,000 组输入，20,400,000 次独立 SoftFloat 对照，覆盖 3 种精度和 4 种舍入方式。
- 缓存所有权：四组 ANM、两种格式，20,678 项比较；修改后恢复、同时加载实例隔离、分步生成和完整释放。
- Music Room：三次进入/返回，画面与 3.1 稳定版逐像素一致；每次上传从 3,457,024 降到 3,325,952 字节，驻留命中随重复进入增长。
- 完整 character0-final 录像、浏览器音频、触摸、存档迁移及离线重开：结果与实际 Wasm 哈希记录在发行包 `validation`，以最终报告为准。

这些检查不能替代实体手机实测，也不证明全部角色、难度和输入都与原版相同。复杂关卡加载和 SDL_ttf 字体边缘仍有明确的验证边界。

构建：`node portable/build.mjs --th08`，随后 `node portable/package-architecture.mjs --th08`。正式包在 `th08_web/artifacts/sdl-release`，本地 8088，兼容旧 C++ 入口 8092；公网 8090 可用 `public-host.ps1 -Action start -Game th08` 切换。

## 3.2.1：夜雀遮罩与三面黑屏修复

ECL 原生回调 0（原版 `00423390`）现在直接写入 `AsciiState::blindness_color/radius`，符卡结束直接清除此遮罩。移除了 `EclNativeServices` 中无人消费的 `draw_mode/draw_parameter` 副本，避免脚本执行了而真实画面没有更新。

`BrowserRuntime::Graphics::triangles` 补回原版 `AnmManager::FlushVertexBuffer`（`00462e40`）每次提交精灵前的颜色和透明度第二参数恢复。背景的 3D 绘制使用纹理常量色；精灵批次改回各顶点颜色，否则三面背景最后的黑色/透明常量会令自机、弹幕和 HUD 消失。此修复位于 TH08 平台适配器，共享 SDL 渲染器未改动。

回归包括：原版 ECL 回调、符卡结束和 ASCII 遮罩几何对照；连续一至三面的 42,000 次更新、2869 次实际 GPU 精灵提交检查及截图；完整 character0-final 录像 100,153 次更新、3339 个状态样本与已有原版对照基线一致。画面测试工具是 `portable/th08-stage-visual-probe.mjs --verify`，连接候选包 8095 运行（设置 `TEST_URL`），图像与检查记录在 `artifacts/stage-visual`。实际发行以 `validation/summary.json` 的 Wasm 哈希为准。
