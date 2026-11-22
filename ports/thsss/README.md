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

然后在本目录建立 `build-eagler` 链接（同时挂上启动器卡面 `thsss-card.webp`）：

```sh
node ports/thsss/scripts/link-runtime.mjs /absolute/path/to/THSSS-WASM/build-eagler
```

卡面源文件是 THSSS-WASM 里预转换好的 `README/thsss-card.webp`（由 `README/card-artwork.png` 转出）；`stage:eagler` 复制到 `build-eagler/artwork/`，链接脚本再接到 `content/artwork/` 与 `games/host-artwork/`。

## 内容目录

把 `thsss.dat` 与 `BGM/` 链到本目录 `content/`（或用环境变量指过去）：

```sh
ln -sfn /absolute/path/to/THSSS-WASM/THSSS.Browser/wwwroot/assets/thsss.dat ports/thsss/content/thsss.dat
ln -sfn /absolute/path/to/THSSS-WASM/THSSS.Browser/wwwroot/assets/BGM ports/thsss/content/BGM
```

`artwork/thsss-card.webp` 由上面的 `link-runtime.mjs` 写入 `content/artwork/`；若内容目录指到 `wwwroot/assets`，`stage:eagler` 也会把同一张卡复制进该目录的 `artwork/`。
## 启动启动器

```sh
EAGLER_DEVELOPMENT_GAMES=thsss \
EAGLER_THSSS_CONTENT_DIR=/absolute/path/to/THSSS-WASM/THSSS.Browser/wwwroot/assets \
npm start
```

打开 `http://127.0.0.1:8130/?test=thsss`（不要单独打开 `thsss.html` 当正式入口；无 `hosted`/`managedData` 时 shell 会去拉旁边的 `assets/thsss.dat`，stage 后虽已链接，但仍缺启动器下发的存档与触控协议）。触控局 Replay 为 `.trpy`，键位局仍为 `.rpy`。

若 iframe 显示 `127.0.0.1 refused to connect`，确认 `npm start` 仍在监听 `8130`，并用同一主机名刷新（`127.0.0.1` 与 `localhost` 不要混用）。
