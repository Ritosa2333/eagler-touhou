# 前端与一个游戏的本地联调

前端使用 Git 和 Bun 1.4.4，工作区工具使用 Node.js 24。编译游戏还需要 Emscripten SDK；其他依赖和本地资源由该作[开发指南](../../ports/README.md)列出。

## 1. 取前端和 TH10

```sh
git clone --filter=blob:none --sparse https://github.com/Ritosa2333/eagler-touhou.git
cd eagler-touhou
git sparse-checkout set launcher common tools docs ports/th10
bun install --cwd launcher --frozen-lockfile --ignore-scripts
```

之后的命令都从仓库根目录运行。只改页面时，可从检出列表中去掉 `ports/th10`。

## 2. 运行前端

已有完整打包 Runtime 的本地预览可指定 `EAGLER_DEVELOPMENT_SITE_MANIFEST`，由正常 Site Manifest 校验器读取运营者提供的 JSON。相对地址从预览站点解析；`EAGLER_TOUHOU_NETPLAY_RELAY` 可覆盖其联机地址。该选项只供源码开发服务器使用，不修改 Host 发布输入。

```sh
bun run dev
```

打开终端显示的地址，修改页面并查看刷新结果。接下来运行游戏时，停止这个命令，使用第 4 步的集成服务器。

界面开发先查 [UI 设计与扩展](ui-controls.md) 中对应的完整模块、官方源码和组件组合，再沿通用 UI 与业务模块的边界修改。视觉与交互按同状态的亮暗、横竖屏对照验收。

界面消息与语言地址按[三语指南](paraglide-i18n.md)维护。开发、类型检查和构建共用翻译生成入口；修改词库后先运行 `bun --bun run --cwd launcher i18n:check`。

本地开发服务器的 `/__ui-audit/` 提供实时布局检测，`?page=lobby` 从大厅开始。操作页面时记录位移与尺寸变化，点击记录可查看前后位置，也可暂停、标记预期变化和导出记录。“检查当前页”同时检查输入设置对齐、弹层遮挡与浮层重复的 CSS 动效。检测页不进入发布构建。

`tests/visual/layout.html` 的“记录下一次动效”从真实操作开始逐帧检查透明度、状态和动画清理。浏览器回归使用 `npm run test:ui-motion:browser -- <开发地址>`，分别验证正常动效、站内减弱动效和系统减弱动效，覆盖亮暗与横竖屏。结果保存实际资源地址和逐帧记录；静态截图的减弱动效设置不代替正常动效验收。

### th09mp 评分原型

准备一份本地 JSON 快照。`as_of` 是快照时间，`games` 的每项是 `[玩家编号, 对手编号, 对局时间, 胜方座位]`，末尾可再加双方机体编号，缺失的机体用 `null`。时间用毫秒时间戳，胜方用 `0` 或 `1`，每条已确认的双账号对局录入一次。

`profiles` 按玩家编号提供 `username`、`nickname` 和可选的公开头像缩略图 `avatar`。只导出允许公开战绩的资料。提供 `profiles` 时，榜单只列其中的玩家。

```sh
python -m pip install numpy
python launcher/tools/th09-ratings.py snapshot.json
```

启动前端开发服务器，打开 `/ui-review.html?preview=th09-ratings`。最少局数默认 10，和交手组、定级状态一起决定榜单范围。搜索可在当前榜单中定位玩家，保留其名次。常用机体按有记录的对局次数列出前三个。窄屏默认显示名次、玩家和评分，展开一行查看完整战绩。原型入口只在开发服务器启用。

## 3. 构建并打包 TH10

先按 [TH10 开发指南](../../ports/th10/docs/DEVELOPMENT.md)准备 SDK 和字体目录，再编译普通版：

```sh
bun run build:runtime -- --game=th10 --variant=normal --emsdk=/absolute/path/to/emsdk
EAGLER_FONT_ROOT=/absolute/path/to/th10-fonts node ports/th10/portable/package-eagler.mjs
```

字体目录需要 `blend.bin` 和 `codepages.bin`。编译输出在 `ports/th10/web/artifacts/sdl3/`，打包输出在 `ports/th10/build-eagler/`。游戏数据在页面中从本地 DATA 包导入。

查看配方时，在构建命令末尾加 `--dry-run`；构建记录位于 `artifacts/validation/source-builds/`。

## 4. 启动集成服务器

```sh
bun run build
EAGLER_DEVELOPMENT_GAMES=th10 npm start
```

打开 <http://127.0.0.1:8130/>，导入 TH10 游戏数据后启动。修改前端后重新运行 `bun run build`；修改游戏后重新编译、打包。

开发服务器按内容目录中实际完整存在的音乐文件集提供音乐选项。DATA 已准备、OGG 尚未准备时，可先使用无背景音乐模式运行游戏。

DATA、音乐和共用字体可位于工作树之外。服务器为清单明确声明的文件分配同源下载地址，独立工作树复用本机资源时无需复制素材。

PowerShell 中先用 `$env:EAGLER_FONT_ROOT='C:\path\to\fonts'` 或 `$env:EAGLER_DEVELOPMENT_GAMES='th10'` 设置相应变量，再运行该行的 `node` 或 `npm` 命令。

## 5. 增加作品或模式

```sh
node tools/workspace.mjs list
node tools/workspace.mjs select th10 --variant=both
```

`select` 会保留已有目录，并补齐所选作品的源码。多人版按相应源码目录的开发指南构建和打包。

| 作品 | 普通版源码 | MP 源码 |
| --- | --- | --- |
| TH06 | `ports/th06` | `ports/th06` |
| TH07 | `ports/th07` | `ports/th07` |
| TH08 | `ports/th08` | `ports/th08-mp` |
| TH09 | `ports/th09` | `ports/th09` |
| TH10 | `ports/th10` | `ports/th10-mp` |
| TH11 | `ports/th11` | `ports/th11-mp` |
| TH15 | `ports/th15` | `ports/th15-mp` |
| TH20 | `ports/th20` | — |

目录来自 [`workspace.json`](../config/workspace.json)，构建模式来自 [`runtime-builds.json`](../config/runtime-builds.json)。TH06/07 的 SDL 子模块在各作指南中初始化。

th11 与 th11mp 均提供高刷新呈现设置，首次开放时默认关闭，用户主动开启后保存选择。两种入口共用 Product Catalog 的显示能力声明，高刷开关直接控制 Runtime 的显示限频，游戏逻辑保持 60 Hz。

## 6. 检查改动

```sh
npm run docs:check
bun run typecheck
bun run build
bun run test
```

游戏行为按本作 `ADAPTATION.md` 逐项验证。需要核心模块、服务或真实浏览器测试时，按[测试入口](../tests/README.md)准备其输入并运行对应命令。

播放器进入全屏即请求浏览器键盘锁，包含资源准备及 Runtime 初始化阶段。
支持 Keyboard Lock 的浏览器将短按 Esc 交给游戏，长按 Esc 保留浏览器退出全屏行为；
退出全屏释放锁。
