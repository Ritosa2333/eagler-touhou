# thsss（東方夏夜祭）本地开发入口

[适配表](ADAPTATION.md) · [练习](PRACTICE.md)

本目录是 eagler-touhou 对 [THSSS-WASM](https://github.com/sblzdddd/THSSS-WASM) 的本地接线，非正式 monorepo 子树迁入。仍为 `testOnly`。

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

## 本地验收

对照 [触控](../../docs/adaptation/input.md) 与 [练习](../../docs/adaptation/localization.md)。本目录保持 `testOnly`。

- TOUCH-01 / TOUCH-02：限速拖动留余量；无限速跟随请求位置。贴墙后未走出的分量回到下一帧。
- TOUCH-03 / TOUCH-04：8 向摇杆走方向键；自由摇杆连续转向。
- TOUCH-09：死亡或禁移时按住同一指，可移动后从当前位置继续。
- TOUCH-10：无限速实际移动后，结算落率为 `100.0%`。
- REPLAY-04：触控局为 `.trpy`，键位局为 `.rpy`，回放能还原位移。
- PRACTICE-01..08：启动器打开练习后，Backspace 开菜单，F1 无敌、F2 禁用炸弹、F3 失焦不暂停，关卡按钮可跳转，开局资源写入 Replay 头 `Prac=`。关闭练习开关后仍走原版选关。
