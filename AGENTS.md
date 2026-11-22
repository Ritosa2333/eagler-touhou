# Agent rules

## Commits

Put one fix or one feature in each commit. Write the message in English using Conventional Commits, for example `fix(input): follow a touch drag at the original move speed`.

## Modules

Give each module one responsibility. Keep protocol handling, key ownership, touch movement, menu gestures, and save bridging apart.

Group modules in folders by that responsibility. Do not leave new modules loose in a shared root.

The browser entry bundles these modules into one `shell.mjs` with esbuild.

## Runtime and game content

Build on Windows with the .NET 10 SDK and its `wasm-tools` workload. From the repository root:

```powershell
dotnet tool restore
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
npm install
npm run stage:eagler
```

`dotnet publish` compiles the NativeAOT browser host. Before that compile, `THSSS.Browser/THSSS.Browser.csproj` runs `tools/PrepareGameSources` and `THSSS.Browser/TransformSources`, then `THSSS.Browser/StageContent`.

### Game SSOT

- `vendor/THSSS-src/` is the committed unmodified game source (ILSpy decompile of `vendor/THSSS.exe`).
- `patches/` holds an ordered series of browser port diffs (`series` + `ownership.json`).
- `PrepareGameSources prepare` copies `vendor/THSSS-src/` to gitignored `THSSS/` and applies patches. Builds do not run ilspycmd.
- Day-to-day: edit `THSSS/`, then `dotnet exec THSSS.Browser/obj/prepare-game-sources/PrepareGameSources.dll refresh-patches --repo .` and commit patch updates. Do not commit `THSSS/`.
- To regenerate pristine sources from the exe: `dotnet tool restore`, then `dotnet exec … PrepareGameSources.dll decompile --repo .`, and commit `vendor/THSSS-src/`.
- `TransformSources` then copies `THSSS` into the intermediate directory, skips `Program.cs` and `AssemblyInfo.cs`, rewrites `\\` to `/`, replaces `Encoding.Default` with `Shooting.Ansi.GBK`, replaces `.ForEach(` with `.ForEachNet48(`, and rewrites common ILSpy artifacts (`Color4.op_Implicit`, `EffectHandle.op_Implicit`, redundant casts, Font disambiguation).
- Audio: game code may keep `.wav` paths; `THSSS.SlimDX` `SourceVoice.Bind` remaps to `.ogg` when that file exists (or when `.wav` is missing).

`StageContent` reads the gitignored asset folders under `Assets/`. `Image`, `CS`, `FX`, `Model`, `Story`, and `Sound` are packed into `THSSS.Browser/wwwroot/assets/thsss.dat`. `BGM` is copied beside that archive as loose files. `Replay` is optional and is not packed. When `thsss.dat` already exists, the build validates the folders and keeps the archive; delete `thsss.dat` to pack again. A checkout without those folders builds with `-p:StageGameAssets=false`.

`npm run stage:eagler` bundles the browser host into `build-eagler/` (`thsss.html`, `shell.mjs`, `manifest.json`, and `THSSS.Browser.wasm`). The eagler-touhou launcher loads that directory from `ports/thsss/build-eagler/`.

To run that launcher against this content, set `EAGLER_THSSS_CONTENT_DIR` to `THSSS.Browser/wwwroot/assets` and `EAGLER_DEVELOPMENT_GAMES` to `thsss`, then run `npm start` from the eagler-touhou repository root. The content directory must contain `thsss.dat` and `BGM/`.
