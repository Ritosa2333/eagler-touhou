# 风神录 3.2：具名游戏回调与明确平台接口

版本：3.2.0-sdl3。参照本地固定版本 eagler TH06/TH07 的职责划分，保留 TH10 自己的关卡、ECL/ANM、人物、计分、录像与更新顺序。没有参考上游仓库里的 TH10 游戏实现。

## 游戏组织

`Application` 管理场景生命周期；`World` 管理游戏会话、人物与关卡；`AnimationEngine`、`Title`、`Hud`、`Backgrounds`、`Audio` 等是各自资源与回调的所有者。

`CallbackNames.hpp` 为更新、绘制、菜单和特殊动画声明语义名称。这些更新/绘制项在原生构建使用独立枚举值，地址映射留在比较原版程序的编译分支。管理器在注册时绑定 C++ 函数；`UpdateChainEntry` 保存已绑定的函数、上下文与参数，每帧直接调用。注销管理器时解绑，删除通知也直接绑定。原生路径缺失回调会明确报错，不会扫描所有接收者寻找编号对应实现。WorldSession 的合作式加载任务仍保留一个原版来源的任务标记，只比较任务类型，不解析或执行该地址。

原版布局仍保留在需要匹配脚本、资源、存档及原版验证的游戏结构中。这些布局并不等同于一个正在运行的 x86 执行器。

## 图形与资源

`Graphics.hpp` 的 `ZunGraphics` 提供创建纹理、获取/映射表面、绘制、变换、复制、呈现、释放资源等明确方法。`GraphicsDevice` 连接该接口；SDL 后端实现它。发布构建不包含 `DeviceOperation` / `ResourceOperation` 的编号分发入口。原版混合、深度、纹理格式和状态含义继续由后端实现。

`TransitionResources.cpp` 在启动游戏时准备 `title.anm`、`title_v.anm`。TH10 的标题、Music Room 和结果菜单共用这些动画，不能套用 TH07 的文件名。缓存保存已解析的精灵/脚本表和已转换的初始像素，约 12.72 MiB，容量上限 16 MiB。进入场景时建立独立的可写数据、纹理和脚本指针；同一标题生命周期内的 Music Room 保留原有资源，不反复创建。改变显示设置导致缓存不适用时走正常加载路径。

关卡资源继续使用既有的分步加载任务，每轮约 2 ms 后让出浏览器。单次资源操作仍可能超过预算，因此这不是“任何手机绝不掉帧”的保证。

## 数值与平台

两款游戏共用 `portable/numeric/ScalarMath.hpp`。本轮 TH10 的 256 处基础运算、23 处取整直接接收和返回普通 `float` / `int32_t`；正常单精度、最近舍入时使用 C++ 运算，无需创建 Extended 中间对象。复杂表达式、特殊舍入、非有限值及指数边界保留兼容运算，不启用 fast-math 或浮点融合。

主循环、键盘/手柄、音频、字体、文件接口继续由 C++/SDL3 管理。两款游戏共用 `TouchController.hpp`，各自提供移动速度、判定和场景状态。Launcher、资源安装、浏览器存档与触控协议沿用 eagler-touhou/1。没有增加联机、练习扩展或高刷插值。

## 检验与构建

- 数值：600,000 组输入覆盖三种精度、四种舍入模式；15,600,000 次比较，对照独立 SoftFloat 结果。
- 原版资源加载：268 组创建、失败、重试和切换场景。
- 游戏：14,000 次更新正常进入二面；三次 Music Room 进出截图与 3.1 稳定版完全一致。
- 交互：SDL 触摸与 Launcher 按钮、存档迁移、PCM 输出、离线重开；另有桌面手机尺寸及四倍 CPU 降速测试。

最终结果与对应 Wasm 哈希见发行包 `validation/summary.json`、`transition-preload.json`、`scalar-math.json`。测试覆盖有限，桌面模拟不等于实体手机测试；SDL_ttf 字形边缘也不保证等同于 Windows GDI。

构建与资源恢复参照 `ARCHITECTURE-3.0.md`。执行 `node portable/build.mjs --th10`，再执行 `node portable/package-architecture.mjs` 生成候选包；公网运行正式包 `artifacts/sdl-release/site`，端口 8090。
