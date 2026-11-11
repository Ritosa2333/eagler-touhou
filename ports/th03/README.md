# TH03 原生网页版本

单机和双人房间均使用 th98port 原生核心编译的 WebAssembly。房间开局直接进入游戏内选人，联机暂停使用项目的双人菜单。这个分支不再包含 TH03 的 DOSBox、NP2、磁盘镜像或旧模拟器录像运行时。旧版录像和观战暂不支持。

构建需要 th98port 固定版本生成的 `classic.js`、`classic.wasm`，以及 PoDD 原始资源和 PODDUI 资源。可用 `tools/ports/th03-native-probe/build-visual.ps1` 构建原生核心；资源中的原始游戏文件供原生核心读取数据，并不通过模拟器执行。

设置 `TH03_NATIVE_BUILD`、`TH03_NATIVE_CONTENT`、`TH03_NATIVE_UI` 后，在仓库根目录运行：

```powershell
npm --prefix launcher run build:ui-rewrite
node ports/th03/scripts/dev.mjs
```

打开 `http://127.0.0.1:18330/?test=th03`，多人大厅为 `http://127.0.0.1:18330/lobby.html?test=th03`。主项目使用独立的单机和联机存档空间。

运行 `node ports/th03/tests/native-solo-browser.mjs` 和 `node ports/th03/tests/native-room-browser.mjs` 可检查主项目单机与双人房间。测试需要已经运行的本地服务。
