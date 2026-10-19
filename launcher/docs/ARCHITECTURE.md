# 启动器结构与职责

启动器负责作品选择、游戏包、浏览器存储和页面导航。Runtime 运行游戏逻辑。所有作品共用 `launcher/` 中的前端和 Host。

## 源码归属

| 目录 | 职责 |
| --- | --- |
| `launcher/app/`、`launcher/src/` | React 页面、路由和浏览器逻辑 |
| `common/contracts/` | 前端与服务共用的产品和协议定义 |
| `launcher/src/contracts/` | 上述定义的 TypeScript 接口 |
| `launcher/package/` | 游戏包导入、存储、更新与 ZIP 处理 |
| `launcher/host/` | 按输入装配可部署的站点 |
| `launcher/integrations/` | 字体、汉化及外部资源格式 |
| `launcher/server/`、`launcher/scripts/serve.mjs` | 本地 HTTP 与 THCRAP 服务 |
| `launcher/lib/` | Node 构建、策略和产物模块 |
| `launcher/scripts/` | 命令行入口 |
| `launcher/public/` | 浏览器静态资源 |
| `ports/<owner>/` | 各作游戏源码及 Runtime 构建、打包 |
| `common/` | 各作共用运行库 |
| `tools/`、`.github/workflows/` | 全仓构建工具与 CI |

表中路径均从仓库根目录定位。

[`workspace.json`](../config/workspace.json) 映射源码目录；[`runtime-builds.json`](../config/runtime-builds.json) 声明构建模式和命令。TH06、TH07、TH09 的普通版与 MP 共用各自源码目录；TH08、TH10、TH11、TH15 的两种模式分目录维护。TH20 提供普通版。

## 从源码到运行

1. `build:runtime` 按配方编译游戏，记录源码与构建信息。
2. 该作打包脚本收集 HTML、JavaScript、WASM 和 Runtime 资源。
3. 前端选择游戏包，准备 DATA、语言和音乐组件。
4. Host 提供页面和 Runtime；浏览器会话加载所选作品。
5. 存档与 Replay 写入浏览器的可变存储，随游戏包更新继续保留。

批量发布 Runtime 使用 `scripts/package-runtime-release.mjs`；装配部署站点使用 `host/build.mjs`。本地开发需要前端构建、所选 Runtime 和本地游戏包，作品由 `EAGLER_DEVELOPMENT_GAMES` 选择。

作品目录以 `testOnly` 统一控制默认入口。th075 普通版和多人版、th20 普通版在常规游戏列表与大厅中隐藏，通过页面 `?test` 或站点 `shared.testBuild: true` 显示。测试模式同时覆盖大厅筛选与创建房间表单，构建和资源能力继续由同一作品目录声明。

隔离部署的 App Shell 按 origin 与挂载目录核对 Service Worker 身份，将已发布的字体修复别名纳入同一应用的迁移入口。后续安装统一使用 `app-shell-sw.js`，更新等待正在运行的游戏和交互完成。

## 房间与共用播放器

房间与 Replay Manager 使用常驻的 `PlayerSurface`。`useWorkspaceViewport` 统一计算画面尺寸与可用高度：手机按内容宽度呈现 4:3 画面，桌面使用面板宽度和可用高度。窗口与设备方向变化时重新适配，触控区域跟随实际画面边界。Replay 的文件列表和播放器共用独立顶栏，顶部对齐。

`RoomWorkspace` 组合成员、设置、聊天和播放器区域。成员卡片用鲜红、青绿和暖橙区分未准备、已准备与加载状态，头像列靠上，信息组独立垂直居中。状态字逐字进入，按钮和页签提供即时过渡；减少动态效果设置下直接显示最终状态。手机双人、三人房共用左侧纵排卡片与右侧旁观栏，桌面按席位并排显示。

原生菜单负责角色与难度选择的作品沿用该入口，大厅按实际选择归属显示信息。

`RoomSpectatorPreview` 默认展示头像，加入与离开入口位于栏底。点击标题、图标或头像打开完整名单，复用成员管理操作。空席整张卡片可点击入座，玩家转入旁观席后恢复空席入口，并按角色显示准备或选席操作。

快捷发言与成员更多菜单共用 `RoomActionMenu`，统一表面、展开收起动画、视口定位和键盘焦点。菜单进入共用浮层宿主，按可用空间展开。竖屏快捷发言位于消息上方；Escape、方向键、Tab 和外部点击按菜单交互处理。

进入房间后先完成资源准备，再通过 `prepare-room` 呈现原生标题。标题保持动画，正式开局按确认的种子和配置重置对局状态。退出旁观通过 `return-room` 恢复同一实例的标题菜单，继续保留 iframe、程序和 DATA。每次启动独立确认首帧和接收时间线，协议字段与阶段见 [Runtime 契约](GAME_ADAPTER_CONTRACT.md)。

设置变更先保留为草稿，游戏区域提供“重载资源”和“返回原有设置”；确认重载后切换 Runtime，设置抽屉独立开关。关闭回滚时显示输入延迟和自动实测，手动模式保留包括零在内的所选帧数。th15 进入战斗前排空菜单捕获队列。游戏与触控编辑器共用安全区几何，大尺寸控件临时适配当前空间，保存布局沿用用户设定。

房间准备校验完整程序版本。玩家就绪与 Relay 开局共同维护版本一致，运行中的对局固定自身版本，晚加入旁观者按该版本接入。更新完整下载并校验后提供使用入口；缺少必需资源时在房间内完成导入，并继续原有准备流程。多人 Replay 从大厅直接进入共用播放器。

`FullscreenTransient` 在房间、Replay 和设置抽屉上方承载导入、下载链接与错误详情，全屏时跟随当前播放器宿主。连接、重连与标定进度位于播放器右上角，确认和报告使用顶层弹窗；各窗口按当前视口限制尺寸并支持滚动。窗口关闭后，空白宿主直接响应底层操作。

## 命令入口

| 命令（从 `launcher/` 运行） | 结果 |
| --- | --- |
| `npm run dev` | 启动带热更新的 React 开发服务器 |
| `npm run build` | 准备锁定版本的浏览器库，构建内容页面、客户端模块和 React 产物 |
| `npm run typecheck`、`npm test` | 检查前端类型与行为 |
| `npm run check` | 检查启动器核心模块 |
| `npm run check:workspace` | 检查跨作品 Runtime 集成 |

具体输入和步骤见[本地联调](DEVELOPMENT.md)与[测试入口](../tests/README.md)。脚本职责见[命令目录](../scripts/README.md)。

## 共用资源下载

所有作品的 DATA、字体、音乐和语言包使用同一下载器。资源分流器按当前发布清单核对节点的字节数与 SHA-256，浏览器按返回顺序下载，并在连接失败、HTTP 错误、传输中断或长时间无进度时自动切换。多节点时先用十秒观察首段速度，低于 64 KiB/s 切到下一节点；最后一个节点持续接收时继续下载。成功节点在当前会话中优先复用，普通静态托管直接使用原资源地址。

每个文件完成后沿用 Package Store 的完整性校验与原子提交，已完成对象可跨重试复用。下载取消立即终止当前请求。路由查询保留版本参数，各节点提供同一文件路径与内容身份。分流服务的配置、接口和检查见[资源下载服务](../../services/resource-router/README.md)。

旁观在桌面与手机上都使用页面内播放器，保留退出与回放控制，隐藏游戏触控、操作帮助和旋转按键。旁观准备阶段即启用该显示规则。
