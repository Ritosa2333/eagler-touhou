# 星莲船开发入口

[源码说明](../README.md) · [适配项目](../ADAPTATION.md) · [SDL3 平台指南](../portable/sdl/README.md)

## 应用与平台边界

`src/game` 是现有 C++20 游戏核心。`portable/sdl` 管理标题与菜单、加载、关卡衔接、结算、音乐、绘制和录像；`Application.cpp` 通过 SDL3 回调驱动同一套 Web／桌面应用。

Web 的 `site/game-sdl.mjs` 接主站协议、资源安装、映射后的输入和浏览器存档。浏览器边界不推进游戏、不渲染游戏场景。`src/sdl-content.mjs` 把原素材包挂载到虚拟文件系统，语言包、字体与 DATA 继续由主站安装。

## 构建与输出

需要 Node.js 22+、Python 3、CMake 3.24+；Web 使用根 CI 登记的 Emscripten 版本，见[构建指南](../../../docs/adaptation/build.md#工具链)。SDL3 和 FreeType 的源码版本与散列固定在 `portable/sdl/Host.cmake`。

在 monorepo 根目录构建主站登记的普通版：

```sh
npm run build:runtime -- --game=th12 --emsdk=/path/to/emsdk
```

直接在作品目录构建：

```sh
npm run build:eagler
node scripts/package-sdl.mjs
```

`build-eagler` 是主站代码包，入口为 `th12.html`；`build-sdl-eagler` 是相同程序的人工预览包。两者包含模块、Wasm、样式与清单。资源准备、语言包和主站组装见 [Launcher 接入](LAUNCHER_INTEGRATION.md)。旧前端单独使用 `scripts/build-legacy-eagler.mjs`。

桌面构建：

```sh
cmake -S ports/th12 -B artifacts/native/th12 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DTH12_BUILD_SDL_HOST=ON -DTH12_ENABLE_THPRAC=ON -DTH12_FETCH_SDL_DEPS=ON
cmake --build artifacts/native/th12 --target touhou12 --config Release --parallel
```

桌面通过 `--data-dir` 指定自有原版数据，通过 `--save-dir` 指定存档。路径、系统字体、手柄和图形上下文的差异集中在平台层。Web 使用 GLES 3／WebGL 2，桌面支持 GLES 3 和 OpenGL 核心上下文。

## 录像、触控与声音

普通录像沿用 TH12 1.00b 格式。`ReplayExtension.cpp` 读写与既有网页一致的 `TH12WEB1` 尾部：版本化 JSON、长度、CRC32，以及实际生效的触控位移、练习参数和跨关伤害源。解析有大小、深度、字段与身份限制。回放输入独立于实时触控，定位从所选关卡起点重算。

游戏固定逻辑步进，额外显示帧对捕获的连续位置插值；显示结果不写回核心。触控的确认、Bomb、暂停分别保留按下信号，坐标在浏览器边界转换为 640×480 逻辑空间。

音乐的流式和整曲解码都使用 C++ miniaudio，音效沿用原版 ID 和音量规则。暂停、定位与后台分别管理输出；跳过区间的瞬时音效不补播。整曲解码持有当前曲目的 PCM，换曲释放，单曲最多 256 MiB。

## 存储与交付

Web 存档根为 `/savesth12`。C++ 文件通过既有浏览器存储白名单同步，保留原版配置、成绩与录像格式；退出、手动同步和定时同步共用同一条写入队列。升级代码包不覆盖用户数据。

主站直接使用登记的构建脚本与 Runtime 入口。`manifest.json` 声明 SDL 适配、语言、THPrac、音乐模式及扩展录像；`runtime-files.json` 提供包内文件大小和 SHA-256。人工验收状态在 [ADAPTATION.md](../ADAPTATION.md) 单列，代码实现状态与实际操作记录分别维护。

普通版和 MP 的 `portable/sdl` 同源并分别链接各自核心。MP 构建目标同为 `touhou12`；C++ Session 负责确认与回滚，SDL 应用负责多人画面、菜单、换面和结局结算；对应指南见 [MP 开发入口](../../th12-mp/docs/DEVELOPMENT.md)。

## 呈现热路径

动画采样器在缓存项内登记显示帧代次，持续显示的轨道复用节点；未显示的轨道在帧末回收，年龄倒退时重新绑定。停止或结束的 VM 在速度、插值和子节点全部结束后直接推进采样年龄；每次采样先执行一帧，以接纳中断和状态修改。背景裁剪在一次捕获内复用多边形、裁剪输出和投影顶点缓冲，保留原先的裁剪、顶点提交与稳定排序顺序。

`tests/sdl-animation.test.mjs` 检查稀疏年龄、倒退、中断、子节点、轨道回收和稳定轨道分配。`tests/native/sdl-animation-performance.cpp` 可用原版 ANM 检查逐帧输出与显示随机数，`tests/native/sdl-background-performance.cpp` 检查七关的背景顶点流。基准数据与运行时回归记录见[迁移版性能验证](../../../archive/th12-sdl-performance-20261029/README.md)。

## SDL 浏览器输入与输出

Web 画布沿用 640×480 离屏目标，窗口呈现负责缩放；背景复制在首次校验后复用纹理。桌面保持 2 倍离屏目标。启动器的 direct-touch 坐标按 iframe 归一化，thprac-mouse 坐标使用 CSS 像素，均通过画布实际边界换算。

SDL 输入保留触控 Esc 的 0x100 位。ImGui 鼠标与键盘、菜单手势、原生初始化和对局首帧分别遵守各自生命周期。正式代码包在入口内嵌布局样式，房间及观战使用同一页面布局。验证范围见[输入与呈现记录](../../../archive/th12-sdl-input-20261029/README.md)。

## SDL 呈现工作复用

绘制调度沿用 Web 适配器的优先级分桶：顺序已正确的桶直接拼接，只排序发生逆序的桶。同帧身份查询使用复用槽位和帧代次，重复身份保留首次登记，稀疏优先级和并列顺序维持原输出。

背景裁剪、圆形特效、标题、加载和结局缓冲随场景复用；背景深度排序以原插入次序处理并列深度。三个常用 HUD 中断指令内联存储，长序列完整保留。多人额外呈现与网络等待复用场景捕获，世界帧、关卡或回滚恢复序号变化时重建；实时 FPS 字段独立更新。

组件耗时、临时分配、原版动画输出及浏览器回归见[呈现性能记录](../../../archive/th12-sdl-render-perf-20261029/README.md)。
