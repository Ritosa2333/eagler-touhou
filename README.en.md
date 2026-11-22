![Banner](README/Banner.png)
# 东方夏夜祭 ～ Shining Shooting Star

[中文](README.md)

This project is part of [eagler-touhou](https://github.com/YomotsuHisami/eagler-touhou).

A browser port of the Windows game (.NET Framework 4.8, SlimDX, WinForms). Game logic starts from committed `vendor/THSSS-src/` (decompiled from `vendor/THSSS.exe`) and is patched for the browser. Direct3D, XAudio2, and WinForms are replaced here and drawn with WebGL2 and Web Audio.

## Modules

| Path | Role | Status |
| --- | --- | --- |
| `vendor/THSSS-src/` | Unpatched game sources (in git). | Copied and patched at build; regenerate with `decompile` from the exe. |
| `vendor/THSSS.exe` | Original game assembly. | Decompiled with pinned `ilspycmd` only when regenerating `THSSS-src`. |
| `patches/` | Ordered browser port diffs (`series` + `ownership.json`). | Applied onto `THSSS-src` into gitignored `THSSS/`. |
| `THSSS` | Generated working tree (not in git). | Edit here, then refresh patches. Browser-only; no desktop dual-build. |
| `vendor/THSSS-Config` | dnSpy export of the config tool. Separate solution, `net20` WinForms, reads and writes `Setting.INI`. | Not part of the browser build. The web build writes a default `Setting.INI` when none exists. |
| `THSSS.Platform` | WinForms, GDI+, and WMI stand-ins on `net10.0`. | Decodes PNG and BMP, and rasters text through the canvas. Other APIs exist only so the game can compile and run its main loop. |
| `THSSS.SlimDX` | SlimDX stand-in. The assembly name is still `SlimDX`. | Device, sprites, textures, the fixed-function pipeline, render targets, fog, and lighting go through WebGL2. Text `.x` meshes are loaded, transformed by their frames, and drawn per material. Blur, Warp, and Filter in `FX` run as hand-written `.vert` / `.frag` files in `wwwroot/shaders` on the full-screen blit. XAudio2 plays browser-decodable audio; `.wav` requests remap to `.ogg` when packed. Key state is filled from the page; the joystick API is empty. |
| `THSSS.Browser` | `net10.0` / `browser-wasm` host, NativeAOT-LLVM. | Prepares game sources from `THSSS-src` + patches, transforms them, and links the runtime and game into one `THSSS.Browser.wasm`. The page starts it through `THSSS.Browser.js`, loads assets into the virtual file system, and drives the frame loop. Keys come from the page. Saves go to IndexedDB. |

## Assets

Game content is not in git. Place it under `Assets/`:

- Required: `Image`, `CS`, `FX`, `Model`, `Story`, `Sound`, `BGM`
- Optional: `Replay`, `Score.dat`
- `BPic` and `ScreenShot` are created at runtime

`StageGameAssets` defaults to `true`. The build checks that the required folders are non-empty and copies them to `THSSS.Browser/wwwroot/assets` (if `manifest.txt` already exists, it only checks). Without the assets:

```powershell
dotnet build THSSS.Browser/THSSS.Browser.csproj -p:StageGameAssets=false
```

## Development

Requires the .NET SDK 10 (`global.json` stays on the 10.0 feature band). Publishing also needs that SDK's `wasm-tools` workload, which supplies Emscripten. The compiler is the experimental NativeAOT-LLVM package `Microsoft.DotNet.ILCompiler.LLVM`, restored from the experimental feed in `nuget.config`. Restore the local tool manifest (`dotnet tool restore`, pins `ilspycmd`) only when regenerating `vendor/THSSS-src/` from `vendor/THSSS.exe`.

```powershell
dotnet workload install wasm-tools
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
```

Serve `THSSS.Browser/bin/Release/net10.0/browser-wasm/publish` with a static file server. That directory contains one `THSSS.Browser.wasm`, the `THSSS.Browser.js` loader, and the page. `file://` does not work. Click the canvas first (unlocks audio and focuses the canvas). Arrows move, Z shoots, X bombs, Shift slows, Esc pauses.

### Editing game logic

1. Build once (or run Prepare) so `THSSS/` exists.
2. Edit files under `THSSS/`.
3. Refresh patches and commit them:

```powershell
dotnet build tools/PrepareGameSources/PrepareGameSources.csproj -c Release
dotnet exec THSSS.Browser/obj/prepare-game-sources/PrepareGameSources.dll refresh-patches --repo .
```

Before compile, TransformSources skips `Program.cs` and `AssemblyInfo.cs`, rewrites `\\` to `/`, replaces `Encoding.Default` with GBK, replaces `List.ForEach` with `ForEachNet48`, and normalizes ILSpy operator/cast artifacts. Graphics, audio, and input gaps belong in `THSSS.SlimDX` and in `gl.js` / `audio.js` under `THSSS.Browser/wwwroot`.

`THSSS.sln` is the browser solution (Platform, SlimDX, Browser). The config tool remains `vendor/THSSS-Config/Config.sln`.

GitHub Actions (`.github/workflows/browser-build.yml`) compiles a checkout that has no assets, with `-p:StageGameAssets=false`.

## Roadmap

This round:

- Wire up game configuration (Config)
- Full touchscreen support (baseline: `docs/touch-th06.plan.md`)
- Smaller and faster asset loading
- Add THSSS-Prac

Later:

- Online play
