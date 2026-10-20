# 前端与一个游戏的本地联调

使用 Git、Node.js 24（最低 22.18）和 npm。编译游戏还需要 Emscripten SDK；其他依赖和本地资源由该作[开发指南](../../ports/README.md)列出。

## 1. 取前端和 TH10

```sh
git clone --filter=blob:none --sparse https://github.com/YomotsuHisami/eagler-touhou.git
cd eagler-touhou
git sparse-checkout set launcher common tools docs ports/th10
npm --prefix launcher ci --ignore-scripts
```

之后的命令都从仓库根目录运行。只改页面时，可从检出列表中去掉 `ports/th10`。

## 2. 运行前端

已有完整打包 Runtime 的本地预览可指定 `EAGLER_DEVELOPMENT_SITE_MANIFEST`，由正常 Site Manifest 校验器读取运营者提供的 JSON。相对地址从预览站点解析；`EAGLER_TOUHOU_NETPLAY_RELAY` 可覆盖其联机地址。该选项只供源码开发服务器使用，不修改 Host 发布输入。

```sh
npm run dev
```

打开终端显示的地址，修改页面并查看刷新结果。接下来运行游戏时，停止这个命令，使用第 4 步的集成服务器。

## 3. 构建并打包 TH10

先按 [TH10 开发指南](../../ports/th10/docs/DEVELOPMENT.md)准备 SDK 和字体目录，再编译普通版：

```sh
npm run build:runtime -- --game=th10 --variant=normal --emsdk=/absolute/path/to/emsdk
EAGLER_FONT_ROOT=/absolute/path/to/th10-fonts node ports/th10/portable/package-eagler.mjs
```

字体目录需要 `blend.bin` 和 `codepages.bin`。编译输出在 `ports/th10/web/artifacts/sdl3/`，打包输出在 `ports/th10/build-eagler/`。游戏数据在页面中从本地 DATA 包导入。

查看配方时，在构建命令末尾加 `--dry-run`；构建记录位于 `artifacts/validation/source-builds/`。

## 4. 启动集成服务器

```sh
npm run build
EAGLER_DEVELOPMENT_GAMES=th10 npm start
```

打开 <http://127.0.0.1:8130/>，导入 TH10 游戏数据后启动。修改前端后重新运行 `npm run build`；修改游戏后重新编译、打包。

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

## 6. 检查改动

```sh
npm run docs:check
npm run typecheck
npm run build
npm test
```

游戏行为按本作 `ADAPTATION.md` 逐项验证。需要核心模块、服务或真实浏览器测试时，按[测试入口](../tests/README.md)准备其输入并运行对应命令。
