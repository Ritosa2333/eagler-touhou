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
dotnet workload install wasm-tools
dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release
npm install
npm run stage:eagler
```

`dotnet publish` compiles the NativeAOT browser host. Before that compile, `THSSS.Browser/THSSS.Browser.csproj` runs `THSSS.Browser/stage-content.ps1`. The script reads the gitignored asset folders at the repository root. `Image`, `CS`, `FX`, `Model`, `Story`, and `Sound` are packed into `THSSS.Browser/wwwroot/assets/thsss.dat`. `BGM` is copied beside that archive as loose files. `Replay` is optional and is not packed. When `thsss.dat` already exists, the build validates the folders and keeps the archive; delete `thsss.dat` to pack again. A checkout without those folders builds with `-p:StageGameAssets=false`.

`npm run stage:eagler` bundles the browser host into `build-eagler/` (`thsss.html`, `shell.mjs`, `manifest.json`, and `THSSS.Browser.wasm`). The eagler-touhou launcher loads that directory from `ports/thsss/build-eagler/`.

To run that launcher against this content, set `EAGLER_THSSS_CONTENT_DIR` to `THSSS.Browser/wwwroot/assets` and `EAGLER_DEVELOPMENT_GAMES` to `thsss`, then run `npm start` from the eagler-touhou repository root. The content directory must contain `thsss.dat` and `BGM/`.
