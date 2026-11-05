# TH19 开发与构建

TH19 的游戏逻辑位于 `web/cpp/game/`，SDL3 应用位于 `web/cpp/sdl/`，渲染器位于 `portable/sdl/`，浏览器适配位于 `web/sdl-runtime/`。普通版与双人对战版共用 C++ 程序，分别打包并使用独立的浏览器存档库。

## 工具链

使用 Node.js 22.18+、Python 3、Emscripten 6.0.9 和 SDL3 port。设置 `TH19_EMSDK` 或激活 SDK，使 `EMSDK` 指向 SDK 根目录。构建支持 `upstream/emscripten` 和 `install/emscripten` 布局；`TH19_PYTHON` 可选择 Python。

从作品目录运行：

```powershell
$env:TH19_EMSDK='D:\tools\emsdk'
npm run build
npm run package:runtime
npm run package:multiplayer
npm test
```

编译对象和程序保存在 `web/artifacts/sdl3/`。普通版 Runtime 输出至 `build-eagler/`，多人版输出至 `build-eagler-multiplayer/`。`scripts/build-eagler.mjs --print-inputs` 无需 SDK，输出编译单元、头文件及 SHA-256。

在总仓库根目录构建：

```sh
npm run build:runtime -- --game=th19 --variant=normal --emsdk=/path/to/emsdk
node ports/th19/portable/package-eagler.mjs
node ports/th19/portable/package-eagler.mjs --multiplayer
```

## 游戏资源与本地联调

本地 `content/` 保留 `th19.dat`、`music/th19_*.ogg` 和 `msgothic.ttf`，不进入源码提交或 Runtime 包。完整轨道顺序与已知资源身份保存在 `game-resources.json`。字体在运行时通过共享 `/msgothic.ttc` 资源加载，源码编译不依赖原版资源。

独立运行使用 `npm start`，默认端口 8130。`npm run check` 校验本机原版数据、24 首 OGG 和 Runtime 的完整性。

总仓库联调设置 `EAGLER_DEVELOPMENT_GAMES=th19`、`EAGLER_TH19_CONTENT_DIR=/path/to/content` 和 `EAGLER_DEVELOPMENT_VANILLA_FONT=/path/to/msgothic.ttf`，构建 Launcher 后启动开发服务器。PowerShell 使用 `$env:变量名='值'` 设置环境变量。

## 输入、文件与双人对战

原生按键为方向键移动、Z 射击／确认、X 蓄力／取消、C Bomb、Shift 低速、Esc 暂停。手柄优先消费 Launcher 映射，独立入口使用 SDL3 手柄。

Runtime 使用 `eagler-touhou/1`，按 `protocol`、`game`、`epoch` 和同源父窗口识别会话；普通版数据库为 `/savesth19`，多人版为 `/savesth19mp`，原生应用沿 `/save` 访问当前模式。用户文件命令限制在成绩、配置和录像目录，原版 DATA 和音乐保持只读。

多人使用公共 Relay 的房间和 WebRTC 信令，交换双人对战的确定性输入，按模式启用原生检查点回滚。TURN 候选与本机线路偏好来自 Launcher。旁观能力按产品声明独立管理。

## 验证入口

`npm test` 验证回滚纠正、确认输出和连接屏障。真实浏览器验收通过 `tests/browser-smoke.mjs` 执行，参数与输入见该脚本的帮助。源码接入检查从总仓库运行 `npm run ports:check -- --game=th19 --variant=both`；逐项行为记录在 `ADAPTATION.md`。

## 浏览器验收

设置 `TH19_BROWSER` 指向 Chrome / Edge 可执行文件，`TH19_PUPPETEER_MODULE` 指向 `puppeteer-core/lib/puppeteer/puppeteer-core.js`。总仓库安装 Launcher 依赖后可省略 Puppeteer 路径。

```sh
npm run test:browser -- --url=http://127.0.0.1:8130/ --relay=ws://127.0.0.1:18142/ --output=artifacts/browser
npm run test:launcher-gameplay -- --url=http://127.0.0.1:8140/ --output=artifacts/launcher
npm run test:launcher-room -- --url=http://127.0.0.1:8140/ --output=artifacts/launcher
```

独立浏览器检查配置持久化、两种 RTC 输入模式及原生房间返回。Launcher 验收从原有 UI 安装资源，分别检查原生玩法和创建／加入／入座／准备／同步开局。多人标题预加载在后台标签页也完成 ACK，返回房间保留已加载的 Wasm 和 DATA。

## 音效重复播放验收

运行 `npm run test:sfx:browser -- --url=<独立游戏服务> --output=artifacts/audio-browser`。测试关闭背景音乐，操作原生菜单，同一音效连续触发四次，每次检查从头播放且产生非零 PCM 波形。

原生音效命令对即时输出执行 stop → seek(0) → play，保持与 miniaudio 分支一致的从头播放行为。WebSfx 服务本身仍按当前游标播放。
