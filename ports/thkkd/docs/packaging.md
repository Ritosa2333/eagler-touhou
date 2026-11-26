# 资源与打包

## 原版资源保留在本机

原版 `dat.dxa`、`bgm.dxa` 和解包素材存放在本地资源目录，`assets/`、`content/` 均排除于 Git 和源码包。素材保留原始字节、正常署名和现有修改时间，浏览器生成独立的运行缓存。

网页接受同时选择的原版 `dat.dxa`、`bgm.dxa`，也接受根目录或单层游戏目录中包含这两份文件的 ZIP。两份 DXA 必须完整且属于受支持的资源版本。资源导入后通过现有 Package Store 管理；用户存档和 Replay 继续独立导入导出。

## 网页交付

在仓库根目录运行 `ports/thkkd/scripts/build-site.ps1`，输出不含原游戏资源的网站与运行组件。打包入口为：

```powershell
python -X utf8 -B ports/thkkd/scripts/release-web.py --site ports/thkkd/build/site --output ../exports/thkkd-original-import
```

输出源码 ZIP 和本机网站 ZIP，各自附有文件清单、SHA-256。玩家启动本机网站后，同时导入原版 `dat.dxa`、`bgm.dxa`，或包含两者的 ZIP。运行程序、字体和原版资源导入组件由站点提供。本机网站使用 EMSDK 内的 `node/24.19.0_64bit`；也可通过 `--node-dir` 指定同时含有 `node.exe` 与 `LICENSE` 的 Node 分发目录。

## 包内容

源码包包含网页源码、公共模块和构建工具；网站包包含启动器、运行组件与字体。保留第三方署名和许可文本。

ZIP 使用相对路径，保留文件修改时间。公开源码的核心调度目录 `src/core/runtime` 必须完整保留，编译产物仅排除本作根部的 `runtime/`。
