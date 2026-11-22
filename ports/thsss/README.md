# thsss（東方夏夜祭）本地开发入口

本目录是 eagler-touhou 对 [THSSS-WASM](https://github.com/sblzdddd/THSSS-WASM) 的本地接线，非正式 monorepo 子树迁入。

## 准备 Runtime

在 Windows 上用 .NET 10 SDK + `wasm-tools` 构建 sibling 仓库，再挂到本目录：

```powershell
cd ..\..\THSSS-WASM   # 或你的 THSSS-WASM 检出路径
dotnet tool restore
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
npm install
npm run stage:eagler
```

然后在本目录建立 `build-eagler` 链接：

```sh
node ports/thsss/scripts/link-runtime.mjs /absolute/path/to/THSSS-WASM/build-eagler
```

## 启动启动器

内容目录需含 `thsss.dat` 与 `BGM/`（通常是 `THSSS.Browser/wwwroot/assets`）：

```sh
EAGLER_DEVELOPMENT_GAMES=thsss \
EAGLER_THSSS_CONTENT_DIR=/absolute/path/to/THSSS-WASM/THSSS.Browser/wwwroot/assets \
npm start
```

打开 `http://127.0.0.1:8130/?test=thsss`。触控局 Replay 扩展名为 `.trpy`，键位局仍为 `.rpy`。
