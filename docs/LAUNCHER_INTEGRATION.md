# 真实 Launcher 接入

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
  language/catalog.json   # 可选，本仓库语言编译器的输出
  language/lang_zh-hant.zip
```

语言包也可以放在单独目录，通过 `--language-packs=PATH` 指定。工具接受本仓库
`th12-localization-catalog/1` 或上游准备脚本生成的标准 Host catalog；语言归档和
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

发布 profile 保持 `web-validation-th12`，Runtime 保持 `completeAdapter: false`。
本轮只完成源码接线、编译和本地打包；首次运行、离线重开、设备输入、存档往返
及完整流程仍由用户实际确认。新端口是独立浏览器存储来源，不自动迁移旧预览页
的存档；可通过文件管理导出、导入允许的成绩、配置及 Replay。
