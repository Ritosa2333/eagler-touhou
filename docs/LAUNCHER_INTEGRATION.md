# 真实 Launcher 接入

## monorepo 开发

本作已接入当前工作区，源码位于 `ports/th12`。在仓库根目录运行：

```sh
node tools/workspace.mjs select th12
npm run build:runtime -- --game=th12 --emsdk=工具链目录
npm run ports:check -- --game=th12
```

构建产物位于 `ports/th12/build-eagler/`。源码来源见 [导入记录](../../../docs/migration/th12-eagler-20261020.json)，逐文件来源见 [eagler-source.json](../eagler-source.json)。Launcher 按测试作品提供星莲船入口，使用 `resources` 协议安装共享字体、音乐与语言包。

普通版标准构建入口现使用 SDL3 / GLES 共用应用，产物仍为 `build-eagler/`。Runtime 保留主站约定的 `th12.html`、`game.mjs`、`core.mjs`、`core.wasm`、`manifest.json` 和闭合集合清单 `runtime-files.json`；DATA、安装代次、输入与存档协议沿用当前契约。音频解码已进入共用 Wasm，产品目录以两种前端共有的核心文件作为必需集合，具体模块由各自清单记录。

`package-launcher.mjs` 默认同时装入 `../th12-mp/build-eagler-multiplayer`，可用 `--multiplayer-runtime=PATH` 指定该模式的独立构建。普通版与多人版分别构建，不将普通版程序当作多人版。旧网页前端可通过 `scripts/build-legacy-eagler.mjs` 重建。SDL 功能验收范围见 [共用运行层](../portable/sdl/README.md)。

THPrac 使用 [紫版原生 UI 接入](../src/practice/INTEGRATION.md)，与 TH10/11 共用 ImGui、FreeType、输入工具和移动端面板。`thpracLocale` 决定工具语言；共享 `unifont.otf` 在 UI 初始化前通过 `resources` 安装。

以下独立仓库接入步骤用于同步原仓库或重建独立环境；monorepo 已包含接入改动，日常开发从上面的命令开始。

本地接入使用 `YomotsuHisami/eagler-touhou` monorepo 中的真实 React Launcher、
Package Store、存档管理和 Runtime Generation。`adapter-preview.html` 继续作为单独
的协议观察页。平台改动保存在 `integration/eagler-touhou-th12.patch`，来源基线见
`integration/common-dependency.json`；本地可用不代表上游已经合并或发布。

## 源码与构建

本仓库使用 `eagler` 作为 Web 适配集成分支。以下操作在该分支执行：

```powershell
git switch eagler
```

在声明的上游基线上应用集成补丁，再物化本仓库源码：

```powershell
git -C D:\Sources\eagler-touhou apply D:\Sources\th12\integration\eagler-touhou-th12.patch
node scripts/install-eagler-source.mjs --workspace=D:/Sources/eagler-touhou
```

在上游 `launcher` 目录安装锁定依赖。然后在 TH12 仓库生成浏览器代码包：

```powershell
npm --prefix D:/Sources/eagler-touhou/launcher ci --ignore-scripts --no-audit --no-fund
$env:EAGLER_COMMON_ROOT = 'D:/Sources/eagler-touhou/common'
npm run build:eagler
```

`build:eagler` 编译完整 THPrac 核心和 Vorbis 解码器，生成只含浏览器代码的
`build-eagler/`。普通 `build` 仍可生成供本地行为工具使用的 Web/Node 核心；
带 Node 内置模块依赖的核心不能作为封闭的网页 Runtime 发布。

## 本地内容

按照 [内容准备](CONTENT_PREPARATION.md) 从自有原版生成以下目录：

```text
CONTENT/
  th12.data
  music/                  # 18 首 OGG
  artwork/th12-card.webp
  shared/msgothic.ttc      # 由平台资源所有者提供
  shared/unifont.otf
  language/catalog.json   # 可选，统一 thcrap 准备器的输出
  language/lang_zh-hans.zip
  language/lang_en.zip
```

语言包也可以放在单独目录，通过 `--language-packs=PATH` 指定。工具接受本仓库
统一 thcrap 准备器生成的标准 Host catalog；语言归档和
目录声明的大小、摘要必须一致。没有语言目录时只发布日文。不能用解包后的预览
目录替代语言 ZIP。

## 组装与启动

在 TH12 仓库运行：

```powershell
node scripts/package-launcher.mjs --workspace=D:/Sources/eagler-touhou --content=D:/Private/th12-content
node D:/Sources/eagler-touhou/launcher/scripts/serve-static.mjs D:/Sources/th12/artifacts/launcher-site 8130
```

默认入口为 `http://127.0.0.1:8130/`。脚本会构建真实 Launcher 页面，并调用上游的
`package-server.mjs --games=th12` 生成站点。已有最新页面产物时可加
`--build-launcher=0`。`--site-url` 可指定实际站点根地址，`--runtime` 可指定另外的
TH12 浏览器代码包。`--font` 和 `--vanilla-font` 可指定平台字体位置。

输出固定在忽略的 `artifacts` 目录下，包含独立的 DATA、OGG、共享字体、语言 ZIP、
Package Descriptor、Release Catalog、不可变 Runtime Generation 和离线页面资源。
默认仅绑定本机；不会执行游戏、测试或自动上传。该输出包含私有游戏素材，不能
当成源码提交到 GitHub。

页面使用原启动器的游戏卡片、下载、设置、触控、THPrac、存档、Replay 及离线包
流程。TH12 通过现有 `resources` 协议接收资源，不提供虚构的全局 Emscripten FS。
共享字体在语言配置前安装，音乐资源可在启动后继续补齐，代码与 DATA 代际分别
固定。其他游戏继续使用原来的资源安装路径。

录像管理使用启动器已有的持久工作区。文件列表读取 `/savesth12` IDBFS v21，
Runtime 负责详情、选关、暂停／播放、速度和前后定位；定位通过重新执行录像输入完成。
允许管理一层录像子目录。普通 iframe 失焦只清理输入，页面隐藏才暂停，
避免点击启动器控制按钮时误打开原版暂停菜单。

自托管工具包通过 `preparedContent.buildSources` 显式携带 TH12 内容准备器及其
Python／JS 依赖，优先使用包内工具，不再依赖打包者本机的 `ports/th12` 存在。
这些文件是私有准备工具，不属于公开 App Shell 或游戏 Runtime。

普通核心已移除诊断专用导出；诊断构建单独输出到 `artifacts/diagnostic-runtime`，
生产打包拒绝诊断核心。quick/daily 的逐 tick 数据比较入口与不可变 golden 接受流程见
[录像验证器](../tools/replay-verifier/README.md)。仍缺完整原版提供器和经审阅的整局
golden，不能把旧的 RNG／解密局部 Oracle 当作整局验收。

发布 profile 保持 `web-validation-th12`，Runtime 保持 `completeAdapter: false`。
2026-10-20 的 Chromium 验证使用日文 1.00b 生成的 514 项 DATA，完成受管理资源、共享
字体、标题与第一关启动，并展示紫版高级选项和移动端折叠面板。test 发布语言为日文；
中文本地化按 thcrap 路径接入。离线重开、实体触控、存档往返及完整流程使用各自的验收记录。
新端口是独立浏览器存储来源，不自动迁移旧预览页
的存档；可通过文件管理导出、导入允许的成绩、配置及 Replay。

## 主站语言与封面

整站组装默认沿主站选择 `ja,lang_zh-hans,lang_en`，分别对应日文、简体中文和英文。`--language-packs=PATH` 提供主站准备的 `catalog.json` 及其中声明的语言 ZIP；封面从内容准备目录的 `artwork/th12-card.webp` 读取，也可用 `--artwork=PATH` 指定统一的主站美术目录。缺少所选语言包或封面时，组装入口直接报告缺少的资源，不自动缩减为日文或更换语言标识。

人工构建可通过 `--languages=ja` 明确选择日文。繁体包保持 `lang_zh-hant` 身份，只有显式选择该语言时才加入站点；简体包由主站内容准备流程提供。
