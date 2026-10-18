# 地灵殿开发与验证

游戏入口是 `cpp/game/GameSession.cpp`；战斗推进在 `GameBattle.cpp`，具体系统由自机、敌人、弹幕、激光、道具、对话、菜单和结局管理器负责。`cpp/sdl/Application.cpp` 连接实际浏览器宿主。

渲染调用链是：`AnmRenderer` 计算顶点 → `ZunGraphics` 语义接口 → `cpp/sdl/GraphicsDevice` → 工作区共用的 `portable/sdl/Renderer.cpp` → SDL3 / WebGL2。共享后端的精度配置编号不是 Direct3D 版本；发布代码没有原版 EXE 调用或 x86 执行器。

Windows 版 Chromium 的 WebGL 驱动信息可能显示 ANGLE / Direct3D11，这是浏览器自身的 GPU 后端。游戏向浏览器提交的仍然是同一套 WebGL2 调用。

`launcher` 沿用最终花映塚共用启动器；`sdl-runtime/managed.mjs` 实现启动、键盘／触控转发、资源与 IDBFS 协议。存档保存在当前玩家浏览器的 `/savesth11` 挂载中，不上传给服务器。

## 本地构建

以下命令从工作区根目录运行。使用现有 `th10_web/tools/node.exe`、`th10_web/tools/wasi-sdk-34.0-x86_64-windows`、`tools/emsdk`、Python，以及 `tools/architecture/typescript`。游戏数据来自工作区的日文 1.00a `th11.dat`；最终目标版本与哈希在 `target.json`。

```powershell
# 原版比较用 C++ / WASI 核心
.\th10_web\tools\node.exe th11_web/scripts/cpp/build.mjs

# 实际 SDL3 / WebGL2 运行时
.\th10_web\tools\node.exe th11_web/scripts/build-sdl.mjs --release

# 隔离的本地开发站点；不更改公网服务
.\th10_web\tools\node.exe th11_web/scripts/package-release.mjs --development
.\th10_web\tools\node.exe th11_web/artifacts/architecture-preview/scripts/serve.mjs --port 8113
```

开发站点地址为 `http://127.0.0.1:8113/`。未完成验收的构建会拒绝不带 `--development` 的最终打包。

当前版本已完成本地发布验收，`target.json` 的 `completeGame` 为 true。最终构建方式：

```powershell
.\th10_web\tools\node.exe th11_web/scripts/build-sdl.mjs --release
.\th10_web\tools\node.exe th11_web/scripts/package-release.mjs
.\th10_web\tools\node.exe th11_web/scripts/verify-release.mjs --final
powershell -NoProfile -File th11_web/scripts/start-local.ps1
```

最终站点在 `artifacts/sdl-release/site`。`启动地灵殿网页版.cmd` 使用 8113 端口；现有 3007 被另一个项目占用。`启动地灵殿临时公网.cmd` 为用户手动启动独立 Cloudflare Quick Tunnel 的入口，会先确认该端口正在提供已验收的地灵殿；本轮自动启动因审批服务 404 故障未执行，不能将本地服务已启动当作公网已开通。

`scripts/verify-release.mjs` 核对当前 C++／SDL 源码哈希、完整原版战役／Extra／四份演示、实际浏览器回放及启动器报告。检查点尾段报告不能满足全流程条件，其他构建的旧报告也不能直接充当当前构建的通过证据。

## 验证入口

```powershell
# 原版模块对照，较慢
.\th10_web\tools\node.exe --test --test-concurrency=1 'th11_web/tests/cpp/*.test.mjs'

# 实际最终启动器：退出、重启、触控、暂停、录像管理
.\th10_web\tools\node.exe th11_web/tests/browser/launcher-check.mjs

# 在同一浏览器来源上由最终花映塚更新为地灵殿，并检查离线启动
.\th10_web\tools\node.exe th11_web/tests/browser/update-check.mjs

# 带开发探针的 SDL 构建，供音频／结局／IDBFS 验证
.\th10_web\tools\node.exe th11_web/scripts/build-sdl.mjs
$env:TH11_CHECK_ENDING='1'
.\th10_web\tools\node.exe th11_web/tests/browser/persistence-check.mjs
```

长录像对照由 `world-replays.test.mjs` 执行，使用固定的 `TH11_CORE_TEST_FILE`，防止其他构建改变正在验证的核心。全战役设置 `TH11_WORLD_CAMPAIGN=1`。当前默认保留原版当前关卡与上一关背景在绘制回调里的淡入／淡出计时；`TH11_WORLD_DRAW_CLOCKS=0` 仅用于复现旧诊断行为，不作为最终验收。省略当前关卡绘制计时会让背景脚本停住，在六面的变形效果开始使用游戏随机数时造成假失同步。

`TH11_ORACLE_COUNT_INSTRUCTIONS=0` 可省略 Unicorn 的逐指令预算钩子以加速固定录像验证；没有替换游戏逻辑或取消逐帧断言。检查点保存原版／C++ 内存、寄存器、分配器状态和 EXE／录像／核心身份，恢复时核对身份；使用不同构建不能复用检查点。

开发期原版执行器在 `scripts/native`，不会打进游戏站点。原版 EXE、Windows 字体／D3DX 对照工具同样只用于开发期验证。

六面最终证据还包含 `world-prefix-equivalence.test.mjs` 与 `scripts/compose-world-proof.mjs`：前者把当前 C++ 与已完成原版比较的固定前缀逐字段比较，并核对保存边界；后者仅在同一检查点的原版尾段连续覆盖至终局时组成完整报告。所有组件哈希和帧区间均保留，不将单独尾段视为完整证明。

`campaign-soak.test.mjs` 为覆盖脚本主动补残机，不代表自然通关。`external-campaign.test.mjs` 使用公开原版录像，不补残机。`world-replays.test.mjs` 才是原版与 C++ 的逐帧状态比较。浏览器的加速回放和手机尺寸模拟都不等同于真机帧率测试。

公开录像的来源与校验值记录在 `reference/replays/sources.json`，这些测试样本不进入站点资源包。验证结果在 `artifacts/cpp/verification` 和 `artifacts/sdl3/browser`；报告中的核心哈希与比较范围必须一起检查。
