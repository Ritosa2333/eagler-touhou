![Banner](README/Banner.png)
# 东方夏夜祭 ～ Shining Shooting Star

[English](README.en.md)

本项目为 [eagler-touhou](https://github.com/YomotsuHisami/eagler-touhou) 项目的一部分。

原 Windows 版（.NET Framework 4.8、SlimDX、WinForms）的浏览器移植。游戏逻辑以入库的 `vendor/THSSS-src/`（由 `vendor/THSSS.exe` 反编译）为基线，再用 `patches/` 打上浏览器改动。Direct3D、XAudio2 和 WinForms 由本仓库的替身接到 WebGL2 和 Web Audio。

## 模块

| 目录 | 作用 | 现状 |
| --- | --- | --- |
| `vendor/THSSS-src/` | 未打补丁的游戏源码（入库）。 | 构建时复制并打补丁；可用 `decompile` 从 exe 重生。 |
| `vendor/THSSS.exe` | 原版游戏程序集。 | 仅在重生 `THSSS-src` 时用锁定的 `ilspycmd` 反编译。 |
| `patches/` | 有序的浏览器移植补丁（`series` + `ownership.json`）。 | 打到 `THSSS-src` 上，生成 gitignored 的 `THSSS/`。 |
| `THSSS` | 生成的工作树（不入库）。 | 在此改逻辑，再 refresh 补丁。仅浏览器目标，无桌面双构建。 |
| `vendor/THSSS-Config` | dnSpy 导出的配置器，独立解决方案，目标 `net20` WinForms，读写 `Setting.INI`。 | 不参与浏览器构建。网页版若没有 `Setting.INI`，会自行写入一份默认配置。 |
| `THSSS.Platform` | `net10.0` 上的 WinForms、GDI+、WMI 替身。 | 能解码 PNG / BMP，并用画布光栅化文字。其余 API 只保留游戏能编译、能跑主循环所需的部分。 |
| `THSSS.SlimDX` | SlimDX API 替身，程序集名仍是 `SlimDX`。 | 设备、Sprite、纹理、固定功能管线、渲染目标、雾和光照已接到 WebGL2。文本 `.x` 网格会读入，套用帧变换后按材质绘制。`FX` 里的 Blur、Warp、Filter 用 `wwwroot/shaders` 下手工改写的 `.vert` / `.frag` 画整屏贴图。XAudio2 接到 Web Audio；游戏仍可写 `.wav` 路径，运行时在存在 `.ogg` 时自动改映。键盘状态由页面按键写入；手柄是空实现。 |
| `THSSS.Browser` | `net10.0` / `browser-wasm` 宿主，NativeAOT-LLVM。 | 从 `THSSS-src` + 补丁准备游戏源码，再变换并编译，把运行时和游戏链成一个 `THSSS.Browser.wasm`。页面用旁边的 `THSSS.Browser.js` 启动它，资源进入虚拟文件系统，由宿主驱动帧循环。按键来自页面，存档写入 IndexedDB。 |

## 资源

游戏资源不入库。放在 `Assets/`：

- 必需：`Image`、`CS`、`FX`、`Model`、`Story`、`Sound`、`BGM`
- 可选：`Replay`、`Score.dat`
- `BPic`、`ScreenShot` 由运行时创建

`StageGameAssets` 默认为 `true`。构建时校验上述必需目录非空，并复制到 `THSSS.Browser/wwwroot/assets`（已有 `manifest.txt` 时只校验、不重拷）。没有资源时：

```powershell
dotnet build THSSS.Browser/THSSS.Browser.csproj -p:StageGameAssets=false
```

## 开始开发

需要 .NET SDK 10（`global.json` 把版本限制在 10.0 功能带）。发布还需要该 SDK 的 `wasm-tools`，用来提供 Emscripten。编译器是实验性的 NativeAOT-LLVM（`Microsoft.DotNet.ILCompiler.LLVM`），包从 `nuget.config` 里的实验源还原。只有要从 `vendor/THSSS.exe` 重生 `vendor/THSSS-src/` 时才需要 `dotnet tool restore`（锁定 `ilspycmd`）。

```powershell
dotnet workload install wasm-tools
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
```

用静态文件服务器打开发布目录 `THSSS.Browser/bin/Release/net10.0/browser-wasm/publish`。里面是一个 `THSSS.Browser.wasm`，加上加载器 `THSSS.Browser.js` 和页面文件。不能用 `file://`。先点击画布（解锁音频并取得焦点），然后方向键移动，Z 射击，X 炸弹，Shift 低速，Esc 暂停。

### 改游戏逻辑

1. 先构建一次（或跑 Prepare），生成 `THSSS/`。
2. 编辑 `THSSS/` 下的源文件。
3. 刷新补丁并提交补丁文件：

```powershell
dotnet build tools/PrepareGameSources/PrepareGameSources.csproj -c Release
dotnet exec THSSS.Browser/obj/prepare-game-sources/PrepareGameSources.dll refresh-patches --repo .
```

浏览器工程在编译前还会：跳过 `Program.cs` 和 `AssemblyInfo.cs`；把 `\\` 换成 `/`；把 `Encoding.Default` 换成 GBK；把 `List.ForEach` 换成 `ForEachNet48`；并规范化 ILSpy 的运算符调用与多余转型。图形、音频、输入的缺口改 `THSSS.SlimDX` 和 `THSSS.Browser/wwwroot` 下的 `gl.js`、`audio.js`。

`THSSS.sln` 是浏览器解决方案（Platform、SlimDX、Browser）。配置器用 `vendor/THSSS-Config/Config.sln`。

GitHub Actions（`.github/workflows/browser-build.yml`）在不含资源的检出上执行 `-p:StageGameAssets=false` 编译。

## 路线图

- 完成游戏配置接入（Config）
- 完整适配触摸屏
- 优化资源加载和大小
- 增加 THSSS-Prac
- 支持联机
