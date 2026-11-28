# thsss（東方夏夜祭）

[适配表](ADAPTATION.md) · [练习](PRACTICE.md) · [上游说明](README-UPSTREAM.md)

源码在本目录，来自 [THSSS-WASM](https://github.com/sblzdddd/THSSS-WASM) 的 Git 子树。产品仍为 `testOnly`。来源登记见 [迁移记录](../../docs/migration/thsss-subtree-20261122.md)。

## 准备 Runtime

在 Windows 上用 .NET 10 SDK 与 `wasm-tools`，于本目录构建并 stage：

```powershell
dotnet tool restore
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
npm install
npm run stage:eagler
```

`stage:eagler` 写入本目录 `build-eagler/`。卡面源文件是 `README/thsss-card.webp`；stage 复制到 `build-eagler/artwork/`。把卡面接到启动器：

```sh
node ports/thsss/scripts/link-runtime.mjs
```

传入其他 stage 目录时，脚本把 `ports/thsss/build-eagler` 指到该目录。

## 内容目录

游戏资源不入库。把已打包的 `thsss.dat` 与 `BGM/` 放到本目录 `content/`，或用环境变量指向外部资源：

```sh
EAGLER_DEVELOPMENT_GAMES=thsss \
EAGLER_THSSS_CONTENT_DIR=/absolute/path/to/assets \
npm start
```

打开 `http://127.0.0.1:8130/?test=thsss`。触控局 Replay 为 `.trpy`，键位局为 `.rpy`。

## 本地验收

对照 [触控](../../docs/adaptation/input.md) 与 [练习](../../docs/adaptation/localization.md)。

- TOUCH-01 / TOUCH-02：限速拖动留余量；无限速跟随请求位置。贴墙后未走出的分量回到下一帧。
- TOUCH-03 / TOUCH-04：8 向摇杆走方向键；自由摇杆连续转向。
- TOUCH-09：死亡或禁移时按住同一指，可移动后从当前位置继续。
- TOUCH-10：无限速实际移动后，结算落率为 `100.0%`。
- REPLAY-04：触控局为 `.trpy`，键位局为 `.rpy`，回放能还原位移。
- PRACTICE-01..08：启动器打开练习后，Backspace 开菜单，F1 无敌、F2 禁用炸弹、F3 失焦不暂停，关卡按钮可跳转，开局资源写入 Replay 头 `Prac=`。关闭练习开关后仍走原版选关。
